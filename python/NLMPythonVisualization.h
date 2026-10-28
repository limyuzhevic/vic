// Visualization and plotting utilities for Python
#ifndef NLM_PYTHON_VISUALIZATION_H
#define NLM_PYTHON_VISUALIZATION_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace py = pybind11;

void registerVisualizationUtils(py::module_& m);

#endif // NLM_PYTHON_VISUALIZATION_H
