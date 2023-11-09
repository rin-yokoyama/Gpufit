/**
 * @file GpufitInterface.hpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief Pulse fit interface for waveform fitting with ERF + exp decay
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __PULSE_FIT_INTERFACE__
#define __PULSE_FIT_INTERFACE__

#include "PoolFitInterface.hpp"
#include "Gpufit/constants.h"
#include "Gpufit/gpufit.h"
#include "Cpufit/cpufit.h"
#include "FitThreadPool.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

class PulseFitInterface : public PoolFitInterface
{
public:
    PulseFitInterface(int nfits, int npoints, const std::map<int, int> &polarity /*1: positive, 0: negative*/);
    virtual ~PulseFitInterface(){};

    void SetPrepulseRange(const float &range)
    {
        if (range > n_points_)
        {
            std::cout << "[PulseFitInterface]: prepulse_range_ is larger than n_points_!" << std::endl;
            return;
        }
        prepulse_range_ = range;
    }
    void SetInitialPeakTime(const float &peak)
    {
        initial_peak_time_ = peak;
    }
    void SetInitialRiseTime(const float &rise)
    {
        initial_rise_time_ = rise;
    }
    void SetInitialDecayTime(const float &decay)
    {
        initial_decay_time_ = decay;
    }

protected:
    static int static_id_;

    const std::map<int, int> polarity_;

    float prepulse_range_;
    float initial_peak_time_;
    float initial_rise_time_;
    float initial_decay_time_;

    void CalculateInitialParameters(const std::vector<float> &pulse, const int ch) override;
};


#endif /**__PULSE_FIT_INTERFACE__**/