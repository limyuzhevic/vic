// Sensory input bindings for Python
#ifndef NLM_PYTHON_SENSORY_INPUT_H
#define NLM_PYTHON_SENSORY_INPUT_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerSensoryInputBindings(py::module_& m);

#endif // NLM_PYTHON_SENSORY_INPUT_H
