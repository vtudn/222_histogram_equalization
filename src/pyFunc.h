#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "histEqSerial.h"
#include "histEqThread.h"

namespace py = pybind11;

py::array_t<int> RGBtoYCbCr(py::array_t<int> input)
{
    py::buffer_info input_buf = input.request();
    if (input_buf.ndim != 3)
        throw std::runtime_error("Number of dimensions must be 3");
    int *input_ptr = static_cast<int *>(input_buf.ptr);

    int row = input_buf.shape[0], col = input_buf.shape[1], channel = input_buf.shape[2];
    auto result = py::array_t<int>(row * col * channel);
    py::buffer_info result_buf = result.request();
    int *result_ptr = static_cast<int *>(result_buf.ptr);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
        {
            int B = input_ptr[(i * col + j) * channel + 0];
            int G = input_ptr[(i * col + j) * channel + 1];
            int R = input_ptr[(i * col + j) * channel + 2];

            result_ptr[(i * col + j) * channel + 0] = std::round(0.299 * R + 0.587 * G + 0.114 * B);
            result_ptr[(i * col + j) * channel + 1] = std::round(0.5 * R - 0.4187 * G - 0.0813 * B + 128);
            result_ptr[(i * col + j) * channel + 2] = std::round(-0.1687 * R - 0.3313 * G + 0.5 * B + 128);
        }

    result.resize({row, col, channel});
    return result;
}

py::array_t<int> histEq_serial(py::array_t<int> input)
{
    py::buffer_info buf = input.request();

    if (buf.ndim != 2)
        throw std::runtime_error("Number of dimensions must be 2");

    int *img = static_cast<int *>(buf.ptr);
    int size = buf.shape[0] * buf.shape[1];
    int *hist = new int[size];
    _histEqSerial(hist, img, size, 1);

    auto result = py::array_t<int>(size);
    int *result_buf = static_cast<int *>(result.request().ptr);
    for (int idx = 0; idx < size; idx++)
        result_buf[idx] = hist[idx];

    result.resize({buf.shape[0], buf.shape[1]});
    return result;
}

py::array_t<int> histEq_thread(py::array_t<int> input)
{
    py::buffer_info buf = input.request();

    if (buf.ndim != 2)
        throw std::runtime_error("Number of dimensions must be 2");

    int *img = static_cast<int *>(buf.ptr);
    int size = buf.shape[0] * buf.shape[1];
    int *hist = new int[size];
    _histEqThread(hist, img, size, 1);

    auto result = py::array_t<int>(size);
    int *result_buf = static_cast<int *>(result.request().ptr);
    for (int idx = 0; idx < size; idx++)
        result_buf[idx] = hist[idx];

    result.resize({buf.shape[0], buf.shape[1]});
    return result;
}

py::array_t<int> YCbCrtoRGB(py::array_t<int> input)
{
    py::buffer_info input_buf = input.request();
    if (input_buf.ndim != 3)
        throw std::runtime_error("Number of dimensions must be 3");
    int *input_ptr = static_cast<int *>(input_buf.ptr);

    int row = input_buf.shape[0], col = input_buf.shape[1], channel = input_buf.shape[2];
    auto result = py::array_t<int>(row * col * channel);
    py::buffer_info result_buf = result.request();
    int *result_ptr = static_cast<int *>(result_buf.ptr);

    for (int i = 0; i < row; i++)
        for (int j = 0; j < col; j++)
        {
            int Y = input_ptr[(i * col + j) * channel + 0];
            int Cr = input_ptr[(i * col + j) * channel + 1];
            int Cb = input_ptr[(i * col + j) * channel + 2];

            result_ptr[(i * col + j) * channel + 0] = std::round(Y + 1.772 * (Cb - 128));
            result_ptr[(i * col + j) * channel + 1] = std::round(Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128));
            result_ptr[(i * col + j) * channel + 2] = std::round(Y + 1.402 * (Cr - 128));
        }

    result.resize({row, col, channel});
    return result;
}

py::array_t<int> bilateralFilter(py::array_t<int> input, int kernelSize, double sigmaColor, double sigmaSpace)
{
    // Allocate memory for the result_ptr input
    py::buffer_info input_buf = input.request();
    int width = input_buf.shape[0], height = input_buf.shape[1], channel = input_buf.shape[2];
    int *input_ptr = static_cast<int *>(input_buf.ptr);

    auto output = py::array_t<int>(width * height * channel);
    py::buffer_info result_buf = output.request();
    int *result_ptr = static_cast<int *>(result_buf.ptr);

    // Apply the bilateral filter
    for (int c = 0; c < channel; c++)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                // Compute the spatial and range weights
                double ws = 0, wr = 0, w = 0;
                // Determine the kernel limits
                int xMin = std::max(0, x - kernelSize);
                int xMax = std::min(width - 1, x + kernelSize);
                int yMin = std::max(0, y - kernelSize);
                int yMax = std::min(height - 1, y + kernelSize);
                // Iterate over the kernel
                for (int yy = yMin; yy <= yMax; yy++)
                {
                    for (int xx = xMin; xx <= xMax; xx++)
                    {
                        int ds = (x - xx) * (x - xx) + (y - yy) * (y - yy);
                        int dr = std::abs(input_ptr[(xx * height + yy) * channel + c] - input_ptr[(x * height + y) * channel + c]);
                        ws += std::exp(-ds / (2 * sigmaSpace * sigmaSpace));
                        wr += std::exp(-dr * dr / (2 * sigmaColor * sigmaColor));
                        w += input_ptr[(xx * height + yy) * channel + c] * ws * wr;
                    }
                }
                // Compute the result_ptr value
                result_ptr[(x * height + y) * channel + c] = (int)(w / (ws * wr));
            }
        }
    }

    output.resize({width, height, channel});
    return output;
}
