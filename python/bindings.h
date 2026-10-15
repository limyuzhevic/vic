#ifndef NLM_BINDINGS_MODULAR_H
#define NLM_BINDINGS_MODULAR_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations for all modular binding functions
namespace nlm {

void include_types(py::module_& m);
void include_enums(py::module_& m);
void include_config(py::module_& m);
void include_sensory(py::module_& m);
void include_world(py::module_& m);
void include_brain(py::module_& m);
void include_agent(py::module_& m);
void include_functions(py::module_& m);
void include_performance(py::module_& m);

} // namespace nlm

#endif // NLM_BINDINGS_MODULAR_H