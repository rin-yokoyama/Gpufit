/**
 * @file PoolFitInterface.hpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief pure virtual class for Gpufit with thread pooling 
 * @version 0.1
 * @date 2023-11-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __POOL_FIT_INTERFACE__
#define __POOL_FIT_INTERFACE__

#include "Gpufit/constants.h"
#include "Gpufit/gpufit.h"
#include "Cpufit/cpufit.h"
#include "FitThreadPool.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <numeric>

class PoolFitInterface
{
public:
    PoolFitInterface(int nfits, int npoints, int n_parameters, int model_id, int estimator_id);
    virtual ~PoolFitInterface(){};

    int AddPulse(const std::vector<float> &pulse, const int ch);
    int CallCpufit();
    int CallGpufit();
    void PoolFit();

    void Clear();
    void SetParametersToFit(const std::vector<int> flags) { parameters_to_fit_ = flags; }
    void SetFitRange(const std::vector<int> &range);

    const int GetNAdded() const { return n_added_; }
    const int ReadResults(int &index, std::vector<float> &parameters, std::vector<float> &init_params, int &states, float &chi_square, int &n_iterations);
    const float* GetDataPtr() const { return data_.data();}
    void SetInitialParameters(int i, const std::vector<float> &params)
    { 
        int index = 0;
        for(const auto &param: params)
        {
            initial_parameters_[i+(index++)] = param;
        }
    }
    const int GetNParameters() const {return n_parameters_;}

protected:
    const int n_parameters_;
    const int model_id_;
    const int estimator_id_;

    int instance_id_;
    const int n_fits_;
    int n_points_;
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
    std::vector<int> fit_range_;

    virtual void CalculateInitialParameters(const std::vector<float> &pulse, const int ch){}
};


#endif /**__POOL_FIT_INTERFACE__**/