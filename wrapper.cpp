#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "src/pyFunc.h"

namespace py = pybind11;

PYBIND11_MODULE(myHist, m)
{
    m.def("RGBtoYCbCr", &RGBtoYCbCr, "Convert RGB nparray to YCbCr");
    m.def("histEq_serial", &histEq_serial, "Histogram Equalization for 1D np array");
    m.def("histEq_thread", &histEq_thread, "Histogram Equalization for 1D np array");
    m.def("YCbCrtoRGB", &YCbCrtoRGB, "Convert YCbCr nparray to RGB");
    m.def("bilateralFilter", &bilateralFilter, "Bilateral Filter for 3D np array");
}