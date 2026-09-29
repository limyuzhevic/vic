#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all the binding headers
#include "bindings_types.hpp"
#include "bindings_enums.hpp"
#include "bindings_config.hpp"
#include "bindings_sensory.hpp"
#include "bindings_objects.hpp"
#include "bindings_agent.hpp"
#include "bindings_world.hpp"
#include "bindings_main.cpp"

#include "../src/brain/Brain.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Define PYBIND11_MODULE
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Bind all components using the modular functions
    nlm::bind_types(m);
    nlm::bind_enums(m);
    nlm::bind_config(m);
    nlm::bind_sensory(m);
    nlm::bind_objects(m);
    nlm::bind_agent(m);
    nlm::bind_world(m);
    nlm::bind_main(m);
}

} // namespace nlm
