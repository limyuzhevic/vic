// Brain bindings for Python
#ifndef NLM_PYTHON_BRAIN_H
#define NLM_PYTHON_BRAIN_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerBrainBindings(py::module_& m);

#endif // NLM_PYTHON_BRAIN_H
