#include <iostream>
#include <algorithm>
#include <cmath>

void _histEqSerial(int *output, const int *input, int width, int height)
{
    // Compute the histogram of the input image
    int histogram[256] = {0};
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            histogram[input[y * width + x]]++;
        }
    }

    // Compute the cumulative distribution function (CDF) of the histogram
    int cdf[256] = {0};
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++)
    {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // Normalize the CDF
    float cdf_min = *std::min_element(cdf, cdf + 256);
    for (int i = 0; i < 256; i++)
    {
        cdf[i] = std::round((cdf[i] - cdf_min) * 255.0f / (width * height - cdf_min));
    }

    // Map the intensity values of the input image using the CDF
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            output[y * width + x] = cdf[input[y * width + x]];
        }
    }
}