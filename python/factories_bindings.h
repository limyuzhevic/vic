#ifndef PYNLM_FACTORIES_BINDINGS_H
#define PYNLM_FACTORIES_BINDINGS_H

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void bindFactories(py::module_& m);

} // namespace nlm

#endif // PYNLM_FACTORIES_BINDINGS_H
