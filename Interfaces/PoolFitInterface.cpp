#include "Interfaces/PoolFitInterface.hpp"

PoolFitInterface::PoolFitInterface(int nfits, int npoints, int n_parameters, int model_id, int estimator_id) : n_fits_(nfits),
                                                             n_points_(npoints),
                                                             n_parameters_(n_parameters),
                                                             model_id_(model_id),
                                                             estimator_id_(estimator_id),
                                                             data_(n_fits_ * n_points_),
                                                             initial_parameters_(n_fits_ * n_parameters_),
                                                             parameters_to_fit_(n_parameters_, 1),
                                                             output_parameters_(n_fits_ * n_parameters_),
                                                             output_states_(n_fits_),
                                                             output_chi_squares_(n_fits_),
                                                             output_n_iterations_(n_fits_)
{
    if (n_points_ < 10)
        std::cout << "[PoolFitInterface]: n_points_ has to be grater than 10!" << std::endl;
    tolerance_ = 0.0001;
    max_n_iterations_ = 50;
}

int PoolFitInterface::AddPulse(const std::vector<float> &pulse, const int ch)
{
    if (n_added_ >= n_fits_)
    {
        return 1;
    }

    if (fit_range_.empty())
    {
        std::memcpy(&data_[n_added_ * n_points_], pulse.data(), n_points_ * sizeof(float));
    }
    else
    {
        std::memcpy(&data_[n_added_ * n_points_], ((void*)pulse.data())+sizeof(float)*fit_range_[0], n_points_ * sizeof(float));
    }
    CalculateInitialParameters(pulse, ch);
    ++n_added_;
    return 0;
}

int PoolFitInterface::CallCpufit()
{
    if (!n_added_)
        return 0;
    const int status = cpufit(
        n_added_,
        n_points_,
        data_.data(),
        0,
        model_id_,
        initial_parameters_.data(),
        tolerance_,
        max_n_iterations_,
        parameters_to_fit_.data(),
        estimator_id_,
        0,
        0,
        output_parameters_.data(),
        output_states_.data(),
        output_chi_squares_.data(),
        output_n_iterations_.data());

    if (status != 0)
    {
        std::cout << "[PoolFitInterface]: Error in cpufit: " << cpufit_get_last_error() << std::endl;
    }
    return status;
}

int PoolFitInterface::CallGpufit()
{
    if (!n_added_)
        return 0;
    const int status = gpufit(
        n_added_,
        n_points_,
        data_.data(),
        0,
        model_id_,
        initial_parameters_.data(),
        tolerance_,
        max_n_iterations_,
        parameters_to_fit_.data(),
        estimator_id_,
        0,
        0,
        output_parameters_.data(),
        output_states_.data(),
        output_chi_squares_.data(),
        output_n_iterations_.data());

    if (status != 0)
    {
        std::cout << "[PoolFitInterface]: Error in gpufit: " << gpufit_get_last_error() << std::endl;
    }
    return status;
}

void PoolFitInterface::PoolFit()
{
    std::cout << "PoolFitInterface[" << instance_id_ << "]: submitting Fit to Queue" << std::endl;
    std::future<PoolFitInterface*> result = FitThreadPool::GetInstance().enqueue([](PoolFitInterface *interface){
        auto if_gpu = FitThreadPool::GetInstance().threadToIfgpu[std::this_thread::get_id()];
        std::cout << "if_gpu " << if_gpu << std::endl;
        if (if_gpu)
            interface->CallGpufit();
        else
            interface->CallCpufit();
        return interface;
    }, this);
    auto ptr = result.get();
    std::cout << "PoolFitInterface[" << instance_id_ << "]: Fit done." << std::endl;
}

void PoolFitInterface::Clear()
{
    n_added_ = 0;
    n_read_ = 0;
}

const int PoolFitInterface::ReadResults(int &index, std::vector<float> &parameters, std::vector<float> &init_params, int &states, float &chi_square, int &n_iterations)
{
    if (n_added_ <= n_read_)
    {
        Clear();
        return 1;
    }
    index = n_read_;
    parameters.resize(n_parameters_);
    init_params.resize(n_parameters_);
    for (int i = 0; i < n_parameters_; ++i)
    {
        parameters[i] = output_parameters_[n_parameters_ * n_read_ + i];
        init_params[i] = initial_parameters_[n_parameters_ * n_read_ + i];
    }
    states = output_states_[index];
    chi_square = output_chi_squares_[index];
    n_iterations = output_n_iterations_[index];
    ++n_read_;
    return 0;
}

void PoolFitInterface::SetFitRange(const std::vector<int> &range)
{
    if (range.size() != 2)
    {
        std::cout << "PoolFitInterface::SetFitRange(): Invalid range input" << std::endl;
        return;
    }
    if (range[1]<range[0])
    {
        std::cout << "PoolFitInterface::SetFitRange(): range[0] is grater than range[1]" << std::endl;
        return;
    }
    fit_range_ = range;
    n_points_ = fit_range_[1] - fit_range_[0];
}