#include "Interfaces/BGFitInterface.hpp"

int BGFitInterface::static_id_ = 0;

BGFitInterface::BGFitInterface(int nfits, int npoints)
    : PoolFitInterface(nfits,npoints,5,SIN_PLUS_LINE,MLE),
    cycle_(0.0), phase_(0.0)
{
    instance_id_ = static_id_++;
}

void BGFitInterface::CalculateInitialParameters(const std::vector<float> &pulse, const int ch)
{
    const float baseline = std::accumulate(pulse.begin(), pulse.end(), 0.0) / pulse.size();
    const auto itr_max = std::max_element(pulse.begin(), pulse.end());
    const auto itr_min = std::min_element(pulse.begin(), pulse.end());

    float prev_sample = baseline;
    std::vector<int> counter = {0, 0};
    for(const auto &sample: pulse)
    {
        if(prev_sample < baseline && sample > baseline)
        {
            ++counter[0];
        }
        if(prev_sample > baseline && sample < baseline)
        {
            ++counter[1];
        }
        prev_sample = sample;
    }
    const auto itr = std::min_element(counter.begin(),counter.end());
    float cycle = 0.;
    if(cycle_==0)
        cycle = (float)pulse.size() / (float)(*itr) / 2.0f;
    else
        cycle = cycle_;

    initial_parameters_[n_parameters_ * n_added_ + 0] = *itr_max - baseline;
    initial_parameters_[n_parameters_ * n_added_ + 1] = phase_;
    initial_parameters_[n_parameters_ * n_added_ + 2] = cycle;
    initial_parameters_[n_parameters_ * n_added_ + 3] = 0;
    initial_parameters_[n_parameters_ * n_added_ + 4] = baseline;
}

void BGFitInterface::SubtractBackground(std::vector<float> &pulse, const std::vector<float> &params)
{
    auto f = [&params](const double &x) -> float {
        return params[0] * sin((x-params[1])/params[2]) + params[3] * x + params[4];
    };

    for(int x = 0; x < pulse.size(); ++x)
    {
        pulse[x] = pulse[x] - f(x);
    }
}
