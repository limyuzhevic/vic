// High-level simplified APIs for common use cases
#ifndef NLM_PYTHON_HIGH_LEVEL_API_H
#define NLM_PYTHON_HIGH_LEVEL_API_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <random>

namespace py = pybind11;

void registerHighLevelAPI(py::module_& m);

#endif // NLM_PYTHON_HIGH_LEVEL_API_H
