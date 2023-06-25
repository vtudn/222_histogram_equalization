#include <iostream>
#include "src/histEqSerial.h"
#define WIDTH 100
#define HEIGHT 100

using namespace std;

int main()
{
    int input[WIDTH * HEIGHT];
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        input[i] = i % 256;
    }

    int output[WIDTH * HEIGHT];

    int histogram[256] = {0};
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            histogram[input[y * WIDTH + x]]++;
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
        cdf[i] = std::round((cdf[i] - cdf_min) * 255.0f / (WIDTH * HEIGHT - cdf_min));
    }

    // Map the intensity values of the input image using the CDF
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            output[y * WIDTH + x] = cdf[input[y * WIDTH + x]];
        }
    }

    // print output
    // for (int i = 0; i < WIDTH * HEIGHT; i++)
    // {
    //     cout << output[i] << " ";
    // }
    // cout << endl;
    return 0;
}