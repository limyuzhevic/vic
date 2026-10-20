#ifndef NLM_PYTHON_BINDINGS_ERROR_HANDLING_H
#define NLM_PYTHON_BINDINGS_ERROR_HANDLING_H

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void register_type_conversions(py::module_& m);
void register_exception_translators(py::module_& m);
void add_type_conversions(py::module_& m);

} // namespace nlm

#endif // NLM_PYTHON_BINDINGS_ERROR_HANDLING_H