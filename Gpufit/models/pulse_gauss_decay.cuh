__device__ void calculate_pulse_gauss_decay (               // ... = function name
    REAL const * parameters,
    int const n_fits,
    int const n_points,
    REAL * value,
    REAL * derivative,
    int const point_index,
    int const fit_index,
    int const chunk_index,
    char * user_info,
    std::size_t const user_info_size)
{
    // indices

    REAL * user_info_float = (REAL*)user_info;
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

    REAL const * p = parameters;
    
    // value

    REAL const argx = 0;
    if (x < p[1])
    {
        argx = (x - p[1]) * (x - p[1]) / (2 * p[2] * p[2]);
    }
    else{
        argx = (x - p[1])/p[3];
    }
    REAL const ex = exp(-argx);
    value[point_index] = p[0] * ex + p[4];

    // derivative

    REAL * current_derivative = derivative + point_index;

    current_derivative[0 * n_points]  = ex;
    current_derivative[4 * n_points]  = 1;
    if (x < p[1])
    {
        current_derivative[1 * n_points]  = p[0] * ex * (x - p[1]) / (p[2] * p[2]);
        current_derivative[2 * n_points]  = p[0] * ex * (x - p[1]) * (x - p[1]) / (p[2] * p[2] * p[2]);
        current_derivative[3 * n_points]  = 0;
    }
    else
    {
        current_derivative[1 * n_points]  = p[0] * ex / p[3];
        current_derivative[2 * n_points]  = 0;
        current_derivative[3 * n_points]  = p[0] * ex * (x - p[1]) / (p[3] * p[3]);
    }
    
}