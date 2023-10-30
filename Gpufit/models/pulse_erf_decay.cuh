#include "cuda_runtime_api.h"

__device__ void calculate_pulse_erf_decay( // ... = function name
    REAL const *parameters,
    int const n_fits,
    int const n_points,
    REAL *value,
    REAL *derivative,
    int const point_index,
    int const fit_index,
    int const chunk_index,
    char *user_info,
    std::size_t const user_info_size)
{
    // indices

    REAL *user_info_float = (REAL *)user_info;
    REAL x = 0;
    if (!user_info_float)
    {
        x = point_index;
    }
    else if (user_info_size / sizeof(REAL) == n_points)
    {
        x = user_info_float[point_index];
    }
    else if (user_info_size / sizeof(REAL) > n_points)
    {
        int const chunk_begin = chunk_index * n_fits * n_points;
        int const fit_begin = fit_index * n_points;
        x = user_info_float[chunk_begin + fit_begin + point_index];
    }

    // parameters

    REAL const *p = parameters;

    // value

    REAL const sqrt_pi = 1.77245385091f;
    REAL const argx_rise = (x - p[1]) / p[2];
    REAL const argx_decay = (x - p[1]) / p[3];
    REAL const ex_rise = 0.5f * erf(-argx_rise) + 0.5f;
    REAL const ex_decay = exp(-argx_decay);
    REAL const d_ex_rise = 2.0f / sqrt_pi * exp(-argx_rise * argx_rise);
    REAL const ex = ex_rise * ex_decay;

    value[point_index] = p[0] * ex + p[4];

    // derivative

    REAL *current_derivative = derivative + point_index;

    current_derivative[0 * n_points] = ex;
    current_derivative[1 * n_points] = p[0] / (p[2]) * ex_decay * d_ex_rise;
    current_derivative[2 * n_points] = p[0] / (p[2] * p[2] * p[2]) * ex_decay * 0.5f * d_ex_rise * (x - p[1]) * (x - p[1]);
    current_derivative[3 * n_points] = p[0] * ex_rise * (x - p[1]) * ex_decay / (p[3] * p[3]);
    current_derivative[4 * n_points] = 1;
}
