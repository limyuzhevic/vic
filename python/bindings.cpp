// bindings.cpp
// Main entry point for NLM Python bindings
// This file includes all the binding modules

#include "bindings_brain.h"
#include "bindings_agent.h"
#include "bindings_config.h"
#include "bindings_world.h"

PYBIND11_MODULE(pynlm, m) {
    // Configure module with documentation
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register all submodules
    register_bindings_brain(m);
    register_bindings_agent(m);
    register_bindings_config(m);
    register_bindings_world(m);
}