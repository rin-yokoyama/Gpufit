/**
 * @file BGFitInterface.hpp
 * @author Rin Yokoyama (yokoyama@cns.s.u-tokyo.ac.jp)
 * @brief Gpufit interface for background fitting 
 * @version 0.1
 * @date 2023-11-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __BG_FIT_INTERFACE__
#define __BG_FIT_INTERFACE__

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
#include <math.h>

class BGFitInterface : public PoolFitInterface
{
public:
    BGFitInterface(int nfits, int npoints);
    virtual ~BGFitInterface(){};

    void SetInitialCycle(const double &cycle)
    {
        cycle_ = cycle;
    }
    void SetInitialPhase(const double &phase)
    {
        phase_ = phase;
    }
    void SubtractBackground(std::vector<float> &pulse, const std::vector<float> &params);

protected:
    static int static_id_;
    double cycle_;
    double phase_;

    void CalculateInitialParameters(const std::vector<float> &pulse, const int ch) override;
};


#endif /**__BG_FIT_INTERFACE__**/