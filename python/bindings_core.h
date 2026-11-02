#ifndef PYNLM_BINDINGS_CORE_H
#define PYNLM_BINDINGS_CORE_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void bindCoreClasses(py::module_& m);

#endif // PYNLM_BINDINGS_CORE_H