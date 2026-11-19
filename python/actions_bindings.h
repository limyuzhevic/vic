#ifndef PYNLM_ACTIONS_BINDINGS_H
#define PYNLM_ACTIONS_BINDINGS_H

#include <pybind11/pybind11.h>
#include "../src/agent/AgentBrain.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

void bindAction(py::module_& m);
void bindAgentBody(py::module_& m);
void bindActionResult(py::module_& m);
void bindSensoryPercept(py::module_& m);
void bindAgentBrain(py::module_& m);

} // namespace nlm

#endif // PYNLM_ACTIONS_BINDINGS_H
