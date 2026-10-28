// World bindings for Python
#ifndef NLM_PYTHON_WORLD_H
#define NLM_PYTHON_WORLD_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerWorldBindings(py::module_& m);

#endif // NLM_PYTHON_WORLD_H
