#ifndef PYNLM_BINDINGS_FACTORY_H
#define PYNLM_BINDINGS_FACTORY_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void bindFactoryFunctions(py::module_& m);

#endif // PYNLM_BINDINGS_FACTORY_H