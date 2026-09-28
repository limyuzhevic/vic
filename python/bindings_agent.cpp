// bindings_agent.cpp - AgentBrain and related bindings
#include "bindings_agent.h"
#include "../src/world/SimpleWorld.hpp"
#include <memory>

namespace nlm {

// Factory function for creating agent brain
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

void register_bindings_agent(py::module& m) {
    // Factory function for agent brain
    m.def("createAgentBrain", &createAgentBrain,
          py::arg("brain"), "Create an agent brain interface with the given brain");
}