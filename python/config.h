#ifndef NLM_PYTHON_BINDINGS_CONFIG_H
#define NLM_PYTHON_BINDINGS_CONFIG_H

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void include_config(py::module_& m);

} // namespace nlm

#endif // NLM_PYTHON_BINDINGS_CONFIG_H