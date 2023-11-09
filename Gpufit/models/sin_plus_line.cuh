#include "cuda_runtime.h"

__device__ void calculate_sin_plus_line( // ... = function name
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

    REAL const theta = (x-p[1])/p[2];
    REAL const wave = sin(theta);
    REAL const d_wave = cos(theta);

    value[point_index] = p[0] * wave + p[3] * x + p[4];

    // derivative

    REAL *current_derivative = derivative + point_index;

    current_derivative[0 * n_points] = wave;
    current_derivative[1 * n_points] = - p[0] * d_wave / p[2];
    current_derivative[2 * n_points] = (p[1]-x) * d_wave / (p[2] * p[2]);
    current_derivative[3 * n_points] = x;
    current_derivative[4 * n_points] = 1;
}
