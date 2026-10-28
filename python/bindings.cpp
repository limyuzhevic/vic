#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core types and forward declarations
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Python binding modules
#include "NLMPythonTypes.h"
#include "NLMPythonEnums.h"
#include "NLMPythonConfig.h"
#include "NLMPythonSensoryInput.h"
#include "NLMPythonMotor.h"
#include "NLMPythonWorld.h"
#include "NLMPythonAgent.h"
#include "NLMPythonBrain.h"
#include "NLMPythonConvenience.h"
#include "NLMPythonSimulationUtils.h"
#include "NLMPythonPersistence.h"
#include "NLMPythonVisualization.h"
#include "NLMPythonMultithreading.h"
#include "NLMPythonAdvancedConfig.h"
#include "NLMPythonHighLevelAPI.h"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Real spiking neural network simulation
        - Leaky Integrate-and-Fire (LIF) neurons
        - Event-driven spike propagation
        - STDP and Hebbian plasticity
        - Structural plasticity
        - Neuromodulation
        - Memory systems
        - Developmental stages
        - Agent-world interaction
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register all binding modules
    registerTypeDefinitions(m);
    registerEnumDefinitions(m);
    registerConfigBindings(m);
    registerSensoryInputBindings(m);
    registerMotorBindings(m);
    registerWorldBindings(m);
    registerAgentBindings(m);
    registerBrainBindings(m);
    registerConvenienceFunctions(m);
    registerSimulationUtils(m);
    registerPersistenceUtils(m);
    registerVisualizationUtils(m);
    registerMultithreadingUtils(m);
    registerAdvancedConfigUtils(m);
    registerHighLevelAPI(m);
}

} // namespace nlm