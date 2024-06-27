/**
 * @file GaussFitInterface.cpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief
 * @version 0.1
 * @date 2024-06-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "Interfaces/GaussFitInterface.hpp"

int GausFitInterface::static_id_ = 0;

GausFitInterface::GausFitInterface(int nfits, int npoints, const std::map<int, int> &polarity)
    : PoolFitInterface(nfits,npoints,4,GAUSS_1D,LSE),
      polarity_(polarity)
{
    instance_id_ = static_id_++;
    prepulse_range_ = 10;
    initial_peak_time_ = -1;
    initial_rise_time_ = 1;
}

void GausFitInterface::CalculateInitialParameters(const std::vector<float> &pulse, const int ch)
{
    const float baseline = std::accumulate(pulse.begin(), pulse.begin() + prepulse_range_, 0.0) / prepulse_range_;

    std::vector<float>::const_iterator itr;
    if (polarity_.at(ch))
    {
        itr = std::max_element(pulse.begin(), pulse.end());
    }
    else
    {
        itr = std::min_element(pulse.begin(), pulse.end());
    }
    initial_parameters_[n_parameters_ * n_added_ + 0] = *itr - baseline;
    if (initial_peak_time_ < 0)
    {
        initial_parameters_[n_parameters_ * n_added_ + 1] = std::distance(pulse.begin(), itr);
        if (!fit_range_.empty())
        {
            initial_parameters_[n_parameters_ * n_added_ + 1] -= fit_range_[0];
        }
    }
    else
    {
        initial_parameters_[n_parameters_ * n_added_ + 1] = initial_peak_time_;
    }
    initial_parameters_[n_parameters_ * n_added_ + 2] = initial_rise_time_;
    initial_parameters_[n_parameters_ * n_added_ + 3] = baseline;
}
