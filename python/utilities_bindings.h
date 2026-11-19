#ifndef PYNLM_UTILITIES_BINDINGS_H
#define PYNLM_UTILITIES_BINDINGS_H

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void bindUtilities(py::module_& m);

} // namespace nlm

#endif // PYNLM_UTILITIES_BINDINGS_H
