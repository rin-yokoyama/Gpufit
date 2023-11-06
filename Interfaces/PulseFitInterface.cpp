/**
 * @file PulseFitInterface.cpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "Interfaces/PulseFitInterface.hpp"

PulseFitInterface::PulseFitInterface(int nfits, int npoints, int polarity) : n_fits_(nfits),
                                                                             n_points_(npoints),
                                                                             polarity_(polarity),
                                                                             data_(n_fits_ * n_points_),
                                                                             initial_parameters_(n_fits_ * n_parameters_),
                                                                             parameters_to_fit_(n_parameters_, 1),
                                                                             output_parameters_(n_fits_ * n_parameters_),
                                                                             output_states_(n_fits_),
                                                                             output_chi_squares_(n_fits_),
                                                                             output_n_iterations_(n_fits_)
{
    if (n_points_ < 10)
        std::cout << "[PulseFitInterface]: n_points_ has to be grater than 10!" << std::endl;
    tolerance_ = 0.0001;
    max_n_iterations_ = 20;
    prepulse_range_ = 10;
    initial_peak_time_ = -1;
    initial_rise_time_ = 1;
    initial_decay_time_ = 100;
}

int PulseFitInterface::AddPulse(const std::vector<float> &pulse)
{
    if (n_added_ >= n_fits_)
    {
        return 1;
    }

    std::memcpy(&data_[n_added_ * n_points_], pulse.data(), n_points_ * sizeof(float));
    CalculateInitialParameters(pulse);
    ++n_added_;
    return 0;
}

int PulseFitInterface::CallCpufit()
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
        std::cout << "[PulseFitInterface]: Error in cpufit: " << cpufit_get_last_error() << std::endl;
    }
    return status;
}

int PulseFitInterface::CallGpufit()
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
        std::cout << "[PulseFitInterface]: Error in gpufit: " << gpufit_get_last_error() << std::endl;
    }
    return status;
}

void PulseFitInterface::Clear()
{
    n_added_ = 0;
    n_read_ = 0;
}

void PulseFitInterface::CalculateInitialParameters(const std::vector<float> &pulse)
{
    // const float baseline = std::reduce(pulse.begin(), pulse.begin() + prepulse_range_) / prepulse_range_;
    const float baseline = std::accumulate(pulse.begin(), pulse.begin() + prepulse_range_, 0.0) / prepulse_range_;

    std::vector<float>::const_iterator itr;
    if (polarity_)
    {
        itr = std::max_element(pulse.begin(), pulse.end());
    }
    else
    {
        itr = std::min_element(pulse.begin(), pulse.end());
    }
    initial_parameters_[n_parameters_ * n_added_ + 0] = *itr - baseline;
    if (initial_peak_time_ < 0)
        initial_parameters_[n_parameters_ * n_added_ + 1] = std::distance(pulse.begin(), itr);
    else
        initial_parameters_[n_parameters_ * n_added_ + 1] = initial_peak_time_;
    initial_parameters_[n_parameters_ * n_added_ + 2] = initial_rise_time_;
    initial_parameters_[n_parameters_ * n_added_ + 3] = initial_decay_time_;
    initial_parameters_[n_parameters_ * n_added_ + 4] = baseline;
}

const int PulseFitInterface::ReadResults(int &index, std::vector<float> &parameters, std::vector<float> &init_params, int &states, float &chi_square, int &n_iterations)
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