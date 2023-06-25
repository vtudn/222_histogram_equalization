#include <iostream>
#include <algorithm>
#include <cmath>
#include "omp.h"
#define COLOR_RANGE 256
#define PAD 16

void _histEqThread(int *output, const int *input, int width, int height)
{
    // Compute the histogram of the input image
    int histogram[COLOR_RANGE] = {0};
    // int histogram[COLOR_RANGE][PAD] = {0};
    // #pragma omp parallel for
    for (int i = 0; i < width * height; i++)
    {
        histogram[input[i]]++;
        // histogram[input[i]][0]++;
    }

    int cdf[COLOR_RANGE] = {0};
    cdf[0] = histogram[0];
    // cdf[0] = histogram[0][0];

    // Compute the cumulative distribution function (CDF) of the histogram
    for (int i = 1; i < COLOR_RANGE; i++)
    {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // Normalize the CDF
    float cdf_min = *std::min_element(cdf, cdf + COLOR_RANGE);
    for (int i = 0; i < COLOR_RANGE; i++)
    {
        cdf[i] = std::round((cdf[i] - cdf_min) * 255.0f / (width * height - cdf_min));
    }

    // Map the intensity values of the input image using the CDF
    for (int i = 0; i < width * height; i++)
    {
        output[i] = cdf[input[i]];
    }
}