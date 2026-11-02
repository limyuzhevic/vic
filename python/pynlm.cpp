// pynlm.cpp - Modular Python bindings for NLM
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Include all modular binding components
#include "bindings_utils.h"
#include "bindings_id_types.h"
#include "bindings_enums.h"
#include "bindings_core.h"
#include "bindings_factory.h"

// Include NLM headers
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/SensoryPercept.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register custom exception
    py::register_exception<nlm::bindings::NLMException>(m, "NLMException");

    // Bind all components in logical order
    nlm::bindings::bindIdTypes(m);
    nlm::bindings::bindEnums(m);
    nlm::bindings::bindCoreClasses(m);
    nlm::bindings::bindFactoryFunctions(m);

    // Register constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";
}

// namespace nlm