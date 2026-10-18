// bindings_agent.hpp
// Python bindings for agent classes

#pragma once

#include <pybind11/pybind11.h>
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/ActionResult.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;

void bind_agent(py::module_ &m);
