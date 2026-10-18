// bindings_agentbrain.hpp
// Python bindings for AgentBrain class

#pragma once

#include <pybind11/pybind11.h>
#include "../src/agent/AgentBrain.hpp"

namespace py = pybind11;

void bind_agentbrain(py::module_ &m);
