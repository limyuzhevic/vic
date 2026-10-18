// bindings_main.cpp
// Main pybind11 bindings file that includes all individual bindings

#include <pybind11/pybind11.h>
#include "bindings_ids.hpp"
#include "bindings_enums.hpp"
#include "bindings_config.hpp"
#include "bindings_sensory.hpp"
#include "bindings_action.hpp"
#include "bindings_agent.hpp"
#include "bindings_world.hpp"
#include "bindings_brain.hpp"
#include "bindings_agentbrain.hpp"
#include "bindings_factory.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Bind all components
    bind_ids(m);
    bind_enums(m);
    bind_config(m);
    bind_sensory(m);
    bind_action(m);
    bind_agent(m);
    bind_world(m);
    bind_brain(m);
    bind_agentbrain(m);
    bind_factory(m);
}
