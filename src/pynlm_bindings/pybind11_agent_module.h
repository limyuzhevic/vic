// pybind11_agent_module.h - Agent module bindings

#pragma once

#include <pybind11/pybind11.h>
#include "../src/agent/AgentBrain.hpp"
#include "pybind11_bindings.h"

namespace py = pybind11;
namespace nlm {

void bind_agent_module(py::module& m);

} // namespace nlm
