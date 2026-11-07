#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    #include "bindings_basic_types.cpp"

    // Import config bindings
    #include "bindings_config.cpp"

    // Import sensor bindings
    #include "bindings_sensor.cpp"

    // Import action and world object bindings
    #include "bindings_action_world.cpp"

    // Import agent and state class bindings
    #include "bindings_agent_state.cpp"

    // Import simulation entity bindings
    #include "bindings_simulation.cpp"

    // Import helper functions
    addHelperFunctions(m);
}