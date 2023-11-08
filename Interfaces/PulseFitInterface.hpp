/**
 * @file GpufitInterface.hpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "Gpufit/constants.h"
#include "Gpufit/gpufit.h"
#include "Cpufit/cpufit.h"
#include "FitThreadPool.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <numeric>

class PulseFitInterface
{
public:
    PulseFitInterface(int nfits = 1000, int npoints = 500, int polarity = 1 /*1: positive, 0: negative*/);
    virtual ~PulseFitInterface(){};

    int AddPulse(const std::vector<float> &pulse);
    int CallCpufit();
    int CallGpufit();
    void PoolFit();
    void Clear();
    void SetParametersToFit(const std::vector<int> flags) { parameters_to_fit_ = flags; }
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

    const int GetNAdded() const { return n_added_; }
    const int ReadResults(int &index, std::vector<float> &parameters, std::vector<float> &init_params, int &states, float &chi_square, int &n_iterations);

protected:
    static const int n_parameters_ = 5;
    static const int model_id_ = PULSE_ERF_DECAY;
    static const int estimator_id_ = MLE; // LSE;
    static int static_id_;

    int instance_id_;
    const int n_fits_;
    const int n_points_;
    const int polarity_;
    int n_added_ = 0;
    int n_read_ = 0;
    std::vector<float> data_;
    std::vector<float> initial_parameters_;
    float tolerance_;
    int max_n_iterations_;
    std::vector<int> parameters_to_fit_;
    std::vector<float> output_parameters_;
    std::vector<int> output_states_;
    std::vector<float> output_chi_squares_;
    std::vector<int> output_n_iterations_;

    float prepulse_range_;
    float initial_peak_time_;
    float initial_rise_time_;
    float initial_decay_time_;

    void CalculateInitialParameters(const std::vector<float> &pulse);
};

int PulseFitInterface::static_id_ = 0;
