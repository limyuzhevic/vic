#ifndef NLM_PYTHON_BINDINGS_WORLD_H
#define NLM_PYTHON_BINDINGS_WORLD_H

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void include_world(py::module_& m);

} // namespace nlm

#endif // NLM_PYTHON_BINDINGS_WORLD_H