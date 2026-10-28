// Main NLM Python bindings module
// This file combines all the specialized binding modules

#include <pybind11/pybind11.h>

// Include all the binding modules
#include "bindings_basic_types.cpp"
#include "bindings_core_classes.cpp"
#include "bindings_sensory_motor.cpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive bindings for:
        - Basic types and enumerations (NeuronId, SynapseId, etc.)
        - Core classes (Config, Brain, RandomGenerator)
        - Sensory systems (Vision, Audio, InternalSignals)
        - Motor systems (Action, AgentBody)
        - World interaction (SimpleWorld)
        - Agent brain interface (AgentBrain, SensoryPercept)
        
        The NLM framework implements a complete artificial brain system
        with neural dynamics, memory systems, prediction, cognition, and
        neuromodulation.
    )pbdoc";

    // Import all submodules
    bindBasicTypes(m);
    bindCoreClasses(m);
    bindSensoryMotorSystems(m);
}

} // namespace nlm
