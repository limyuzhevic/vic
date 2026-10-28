// Simulation utilities for Python
#ifndef NLM_PYTHON_SIMULATION_UTILS_H
#define NLM_PYTHON_SIMULATION_UTILS_H

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

namespace py = pybind11;

void registerSimulationUtils(py::module_& m);

#endif // NLM_PYTHON_SIMULATION_UTILS_H
