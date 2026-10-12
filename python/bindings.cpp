#include <pybind11/pybind11.h>

// Include all binding header files
#include "bindings_core.h"
#include "bindings_config.h"
#include "bindings_sensory.h"
#include "bindings_motor.h"
#include "bindings_perception.h"
#include "bindings_world.h"
#include "bindings_brain.h"
#include "bindings_agent.h"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides access to the full NLM neural simulation framework including:
        - Neural computation with spiking neurons
        - Memory systems (working, episodic, associative)
        - Plasticity rules (STDP, Hebbian, structural)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction systems
        - Cognitive systems (planning, concept formation, attention)
        - Development and learning
        - World interaction and action selection
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Import all bindings from header files
    // (All classes and functions are defined in their respective header files)

    // Module attributes
    m.attr("__version__") = "0.6.0";  // Updated version for improvements
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Phase 6: Final Integration Neural Learning Machine";
}

} // namespace nlm
