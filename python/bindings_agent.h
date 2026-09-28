// bindings_agent.h - Agent module bindings
#pragma once
#include <pybind11/pybind11.h>
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;

void register_bindings_agent(py::module& m);

// Forward declarations for Agent types
namespace nlm {
    class AgentBrain;
    class AgentBody;
    class SensoryPercept;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class ActionResult;
}