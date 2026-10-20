#ifndef NLM_PYTHON_BINDINGS_MAIN_BINDINGS_H
#define NLM_PYTHON_BINDINGS_MAIN_BINDINGS_H

#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace nlm {

void register_type_conversions(py::module_& m);
void include_basic_types(py::module_& m);
void include_config(py::module_& m);
void include_sensory(py::module_& m);
void include_action(py::module_& m);
void include_agent(py::module_& m);
void include_world(py::module_& m);
void include_brain(py::module_& m);
void include_agentbrain(py::module_& m);
void include_factories(py::module_& m);

} // namespace nlm

#endif // NLM_PYTHON_BINDINGS_MAIN_BINDINGS_H