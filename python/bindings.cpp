#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Python exception wrapper for better error handling
class PythonRuntimeError : public std::runtime_error {
public:
    PythonRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a Pythonic interface to the NLM brain simulation framework,
        enabling users to create, configure, and run neural simulations in Python with
        access to all NLM's advanced brain features including memory systems,
        neuromodulation, and cognitive functions.
    )pbdoc";

    // Register Python-specific exception
    py::register_exception<PythonRuntimeError>(m, "PythonRuntimeError");

    // Utility functions for common operations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible settings for NLM simulations.");

    // Convenience function to create brain with default config (most common use case)
    m.def("createBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration. Convenience wrapper for most common use case.");

    m.def("createBrainWithConfig", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with the specified configuration.");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple 2D world for simulation.");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface that connects brain to world.");

    // Helper function to run simple simulation loops
    m.def("run_simple_simulation", [](std::shared_ptr<Brain> brain, 
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     int num_steps, double timestep = 0.1) {
        world->reset();
        agent->initialize(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * timestep);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
        py::arg("num_steps"), py::arg("timestep") = 0.1,
        "Run a complete simple simulation loop with brain, world, and agent.");

    // Statistical analysis functions
    m.def("analyze_brain_state", [](std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        oss << "=== Brain State Analysis ===" << std::endl;
        oss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        oss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        oss << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        oss << "E/I Balance Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        oss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        return oss.str();
    }, py::arg("brain"), "Get comprehensive statistical analysis of brain state.");

    // Enhanced error handling wrapper
    m.def("safe_brain_step", [](std::shared_ptr<Brain> brain, int step, double time = 0.0) {
        try {
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            throw PythonRuntimeError(std::string("Brain step failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
        "Safely execute a brain step with error handling.");

    // Batch operations for efficiency
    m.def("inject_current_batch", [](std::shared_ptr<Brain> brain, 
                                   const std::vector<NeuronId>& neuron_ids,
                                   const std::vector<float>& currents) {
        if (neuron_ids.size() != currents.size()) {
            throw PythonRuntimeError("Neuron IDs and currents vectors must have same size");
        }
        
        for (size_t i = 0; i < neuron_ids.size(); ++i) {
            brain->injectCurrent(neuron_ids[i], currents[i]);
        }
    }, py::arg("brain"), py::arg("neuron_ids"), py::arg("currents"),
        "Inject current into multiple neurons in a single batch operation for efficiency.");

    // Memory management helpers
    m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->save(filepath)) {
            throw PythonRuntimeError("Failed to save brain state to file: " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file with error checking.");

    m.def("load_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain->load(filepath)) {
            throw PythonRuntimeError("Failed to load brain state from file: " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file with error checking.");

    // Development utilities
    m.def("get_developmental_summary", [](std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        auto stage = brain->getDevelopmentalStage();
        oss << "Developmental Stage: " << static_cast<int>(stage) << std::endl;
        oss << "Stage Description: ";
        switch (stage) {
            case DevelopmentalStage::Initial: oss << "Initial (start)"; break;
            case DevelopmentalStage::CriticalPeriod: oss << "Critical Period (rapid learning)"; break;
            case DevelopmentalStage::Maturation: oss << "Maturation (fine-tuning)"; break;
            case DevelopmentalStage::Adult: oss << "Adult (stable)"; break;
            case DevelopmentalStage::Aging: oss << "Aging (decline)"; break;
        }
        return oss.str();
    }, py::arg("brain"), "Get developmental stage information as human-readable string.");

    // Validation and diagnostics
    m.def("validate_brain_config", [](std::shared_ptr<Config> config) {
        std::ostringstream oss;
        oss << "=== Configuration Validation ===" << std::endl;
        oss << "Has neuron_count: " << (config->has("brain.neuron_count") ? "Yes" : "No") << std::endl;
        oss << "Has connection_probability: " << (config->has("brain.connection_probability") ? "Yes" : "No") << std::endl;
        oss << "Has simulation_timestep: " << (config->has("simulation_timestep") ? "Yes" : "No") << std::endl;
        oss << "Configuration keys: ";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << keys[i];
        }
        return oss.str();
    }, py::arg("config"), "Validate and summarize configuration settings.");

    // Fast iteration helpers
    m.def("get_region_info", [](std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        oss << "=== Brain Regions Summary ===" << std::endl;
        auto region_ids = brain->getRegionIds();
        oss << "Number of regions: " << region_ids.size() << std::endl;
        for (const auto& id : region_ids) {
            oss << "Region ID: " << id.value << std::endl;
        }
        return oss.str();
    }, py::arg("brain"), "Get information about all brain regions.");

    // Simplified agent creation
    m.def("create_simple_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all major subsystems by default for power users
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize a fully configured agent brain.");

    // Namespace constants for convenience
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_TYPE_INTERNAL") = NeuronType::Internal;
    m.attr("NEURON_TYPE_MODULATORY") = NeuronType::Modulatory;

    m.attr("ACTION_TYPE_MOVE_FORWARD") = ActionType::MoveForward;
    m.attr("ACTION_TYPE_TURN_LEFT") = ActionType::TurnLeft;
    m.attr("ACTION_TYPE_INTERACT") = ActionType::Interact;
    m.attr("ACTION_TYPE_EAT") = ActionType::Eat;
    m.attr("ACTION_TYPE_WAIT") = ActionType::Wait;

    // Version information
    m.attr("__version__") = "0.1.0 (Enhanced Python Bindings)";
    m.attr("__description__") = "Enhanced NLM Python bindings with improved error handling, convenience functions, and batch operations.";

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm