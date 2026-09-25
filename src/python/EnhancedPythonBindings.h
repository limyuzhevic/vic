#ifndef ENHANCED_PYTHON_BINDINGS_H
#define ENHANCED_PYTHON_BINDINGS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for Python API
std::string format_simulation_step(SimulationStep step, Timestamp time);
std::vector<float> get_neuron_activations(const Brain& brain);
void set_default_config_values(std::shared_ptr<Config> config);

// Enhanced Config class with additional functionality
void enhanceConfigAPI(py::class_<Config>& cfgClass);

// Enhanced Brain class with advanced simulation control
void enhanceBrainAPI(py::class_<Brain>& brainClass);

// Enhanced AgentBrain with more control methods
void enhanceAgentBrainAPI(py::class_<AgentBrain>& agentBrainClass);

// Enhanced SimpleWorld with better control
void enhanceSimpleWorldAPI(py::class_<SimpleWorld>& worldClass);

// Enhanced visualization interface
void enhanceVisualizationAPI(py::class_<VisualizationInterface>& vizClass);

// New utility functions for batch operations
void addBatchOperations(py::module_& m);

// New utility functions for simulation management
void addSimulationManagement(py::module_& m);

// New utility functions for advanced neuromodulation
void addAdvancedNeuromodulation(py::module_& m);

// New utility functions for memory management
void addMemoryManagement(py::module_& m);

// New utility functions for experiment configuration
void addExperimentConfiguration(py::module_& m);

// New utility functions for performance monitoring
void addPerformanceMonitoring(py::module_& m);

// New utility functions for debugging and development
void addDebugDevelopment(py::module_& m);

} // namespace nlm

#endif // ENHANCED_PYTHON_BINDINGS_H
