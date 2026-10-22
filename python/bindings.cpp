#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

#include "bindings_core_types.cpp"
#include "bindings_config.cpp"
#include "bindings_sensory.cpp"
#include "bindings_motor.cpp"
#include "bindings_world.cpp"
#include "bindings_percept.cpp"
#include "bindings_simpleworld.cpp"
#include "bindings_brain.cpp"
#include "bindings_agentbrain.cpp"
#include "bindings_module.cpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all components
    bindCoreTypes(m);
    bindConfig(m);
    bindSensoryInput(m);
    bindMotor(m);
    bindWorld(m);
    bindSensoryPercept(m);
    bindSimpleWorld(m);
    bindBrain(m);
    bindAgentBrain(m);
    bindModuleFunctions(m);
}