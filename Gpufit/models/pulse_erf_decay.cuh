#include "cuda_runtime.h"

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
    x = point_index;
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
    REAL const ex_rise = erf(argx_rise) + 1.0f;
    REAL const ex_decay = exp(-argx_decay);
    REAL const d_ex_rise = -2.0f * exp(-argx_rise * argx_rise) / (p[2] * sqrt_pi);
    REAL const ex = ex_rise * ex_decay;
    REAL const amp = 0.5f * p[0];

    value[point_index] = amp * ex + p[4];

    // derivative

    REAL *current_derivative = derivative + point_index;

    current_derivative[0 * n_points] = 0.5f * ex;
    current_derivative[1 * n_points] = amp * (d_ex_rise * ex_decay + ex_rise * ex_decay / p[3]);
    current_derivative[2 * n_points] = amp * (x - p[1]) / p[2] * ex_decay * d_ex_rise;
    current_derivative[3 * n_points] = amp * ex_rise * (x - p[1]) * ex_decay / (p[3] * p[3]);
    current_derivative[4 * n_points] = 1;
}
