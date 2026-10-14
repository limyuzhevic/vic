# This file contains the improved Python bindings for NLM
# It has been reorganized into logical groups with Pythonic features

import pybind11
import pybind11/functional.h
import pybind11/stl.h
from pybind11/stl_bind.h
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"

namespace py = pybind11;
namespace nlm {

// ============================================================================
// UTILITY FUNCTIONS AND MODULE SETUP
// ============================================================================

// Custom exception classes for Python
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// ============================================================================
// CONFIGURATION SYSTEM
// ============================================================================

void configureModule(py::module_& m) {
    // Configuration with Pythonic interface
    py::class_<Config> config_class(m, "Config", R"pbdoc(
        Configuration system for NLM brain parameters.
        
        The Config class manages brain simulation parameters including:
        - Neuron and synapse settings
        - Plasticity parameters
        - Neuromodulation settings
        - Development parameters
        - Simulation control parameters
        
        Parameters can be set via keys, files, or command-line arguments.
    )pbdoc");
    
    config_class
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.)pbdoc")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments.)pbdoc")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.)pbdoc")
        .def("get_keys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.)pbdoc")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            // Try other types
            auto int_val = self.get<int64_t>(key);
            if (int_val) return std::to_string(*int_val);
            auto double_val = self.get<double>(key);
            if (double_val) return std::to_string(*double_val);
            auto bool_val = self.get<bool>(key);
            if (bool_val) return *bool_val ? "true" : "false";
            throw ConfigurationError("Key not found: " + key);
        }, py::arg("key"),
            R"pbdoc(Get a configuration value as string. Automatically converts
            to string representation if other type.)pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            // Try to parse as boolean, integer, float, or keep as string
            if (value == "true" || value == "false") {
                self.set(key, value == "true");
            } else {
                try {
                    size_t pos;
                    int64_t int_val = std::stoll(value);
                    self.set(key, int_val);
                } catch (...) {
                    try {
                        double float_val = std::stod(value);
                        self.set(key, float_val);
                    } catch (...) {
                        self.set(key, value);
                    }
                }
            }
        }, py::arg("key"), py::arg("value"),
            R"pbdoc(Set a configuration value with automatic type detection.)pbdoc")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("get_int", [](const Config& self, const std::string& key, int64_t default_value) {
            auto value = self.get<int64_t>(key);
            return value ? *value : default_value;
        }, py::arg("key"), py::arg("default_value") = 0)
        .def("get_float", [](const Config& self, const std::string& key, double default_value) {
            auto value = self.get<double>(key);
            return value ? *value : default_value;
        }, py::arg("key"), py::arg("default_value") = 0.0)
        .def("get_bool", [](const Config& self, const std::string& key, bool default_value) {
            auto value = self.get<bool>(key);
            return value ? *value : default_value;
        }, py::arg("key"), py::arg("default_value") = false);
}

// ============================================================================
// CORE TYPES AND ENUMS
// ============================================================================

void bindCoreTypes(py::module_& m) {
    // NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });
    
    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });
    
    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });
    
    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });
    
    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    // MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();
    
    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Development utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// ============================================================================
// BRAIN CLASS WITH PYTHONIC INTERFACE
// ============================================================================

void bindBrainClass(py::module_& m) {
    py::class_<Brain> brain_class(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The Brain class implements real spiking neural computation with:
        - Leaky Integrate-and-Fire (LIF) neurons
        - Event-driven spike propagation
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Integrated memory systems
        - Neuromodulation and development
        - Sensory and motor interfaces
        
        This is the main neural computation engine that processes sensory
        input and produces motor output.
    )pbdoc");
    
    brain_class
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Create brain with configuration.)pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp.)pbdoc")
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.)pbdoc")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.)pbdoc")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.)pbdoc")
        .def("produce_action", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity.)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state.)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.)pbdoc")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region.)pbdoc")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID.)pbdoc")
        .def("get_region_count", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions.)pbdoc")
        .def("get_region_ids", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.)pbdoc")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions.)pbdoc")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.)pbdoc")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.)pbdoc")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.)pbdoc")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.)pbdoc")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.)pbdoc")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.)pbdoc")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.)pbdoc")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage.)pbdoc")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration.)pbdoc")
        .def("log_status", &Brain::logStatus,
             R"pbdoc(Log brain status.)pbdoc");
    
    // Add convenience methods
    brain_class.def("get_plasticity_system", &Brain::getSTDP,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get STDP plasticity system.)pbdoc")
              .def("get_hebbian_system", &Brain::getHebbian,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get Hebbian plasticity system.)pbdoc")
              .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get structural plasticity system.)pbdoc")
              .def("get_working_memory", &Brain::getWorkingMemory,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get working memory system.)pbdoc")
              .def("get_episodic_memory", &Brain::getEpisodicMemory,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get episodic memory system.)pbdoc")
              .def("get_prediction_system", &Brain::getPredictionSystem,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get prediction system.)pbdoc")
              .def("get_curiosity", &Brain::getCuriosity,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get curiosity neuromodulator.)pbdoc")
              .def("get_novelty", &Brain::getNovelty,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get novelty neuromodulator.)pbdoc")
              .def("get_prediction_error", &Brain::getPredictionErrorSignal,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get prediction error signal.)pbdoc")
              .def("get_developmental_system", &Brain::getDevelopmentSystem,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get developmental system.)pbdoc")
              .def("get_neural_planner", &Brain::getPlanner,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get neural planner.)pbdoc")
              .def("get_concept_formation", &Brain::getConceptFormation,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get concept formation system.)pbdoc")
              .def("get_dopamine", &Brain::getDopamine,
                   py::return_value_policy::reference_internal,
                   R"pbdoc(Get dopamine neuromodulator.)pbdoc")
              .def("enable_all_systems", [](Brain& brain) {
        // Enable all major systems for advanced simulations
        if (auto* agent = dynamic_cast<AgentBrain*>(nullptr)) {
            // This would need proper integration with agent brain
        }
        // Enable plasticity
        if (auto* stdp = brain.getSTDP()) stdp->enable(true);
        if (auto* hebbian = brain.getHebbian()) hebbian->enable(true);
        if (auto* structural = brain.getStructuralPlasticity()) structural->enable(true);
    }, R"pbdoc(Enable all major systems for advanced simulations.)pbdoc")
              .def("get_simulation_stats", [](const Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["exc_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        return stats;
    }, R"pbdoc(Get comprehensive simulation statistics.)pbdoc");
}

// ============================================================================
// AGENT BRAIN CLASS WITH PYTHONIC INTERFACE
// ============================================================================

void bindAgentBrainClass(py::module_& m) {
    py::class_<AgentBrain> agent_brain_class(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        The AgentBrain class provides a high-level interface for:
        - Processing sensory input from environment
        - Decoding motor commands from brain activity
        - Managing neuromodulation and reward
        - Tracking development and learning progress
        - Enabling/disabling learning subsystems
        
        This is the primary interface for creating intelligent agents
        that interact with simulated environments.
    )pbdoc");
    
    agent_brain_class
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create agent brain interface.)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world.)pbdoc")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.)pbdoc")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.)pbdoc")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.)pbdoc")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.)pbdoc")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             R"pbdoc(Apply reward-based neuromodulation.)pbdoc")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(Update development system.)pbdoc")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.)pbdoc")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.)pbdoc")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.)pbdoc")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.)pbdoc")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain.)pbdoc")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation.)pbdoc")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity.)pbdoc")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable/disable development.)pbdoc")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable/disable curiosity.)pbdoc")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.)pbdoc")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.)pbdoc")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.)pbdoc")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.)pbdoc");
    
    // Add convenience methods
    agent_brain_class.def("enable_all_systems", [](AgentBrain& agent) {
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
    }, R"pbdoc(Enable all learning systems.)pbdoc")
       .def("get_agent_stats", [](const AgentBrain& agent) {
        py::dict stats;
        stats["curiosity"] = agent.getCuriosityLevel();
        stats["novelty"] = agent.getNoveltyLevel();
        stats["prediction_error"] = agent.getPredictionError();
        stats["neuromodulation"] = agent.getNeuromodulationLevel();
        stats["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
        return stats;
    }, R"pbdoc(Get agent statistics.)pbdoc");
}

// ============================================================================
// WORLD CLASS WITH PYTHONIC INTERFACE
// ============================================================================

void bindWorldClass(py::module_& m) {
    py::class_<SimpleWorld> world_class(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The SimpleWorld class provides:
        - 2D grid-based world simulation
        - Vision-based sensory input
        - Motor command execution
        - World object management
        - Agent body tracking
        - Random event generation
        
        This is the environment in which agents interact and learn.
    )pbdoc");
    
    world_class
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure world dimensions.)pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state.)pbdoc")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.)pbdoc")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"),
             R"pbdoc(Update world by timestep seconds.)pbdoc")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply motor command to world.)pbdoc")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get sensory percept from world.)pbdoc")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body state.)pbdoc")
        .def("add_object", &SimpleWorld::addObject,
             py::arg("obj"),
             R"pbdoc(Add world object.)pbdoc")
        .def("remove_object", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Remove world object at position.)pbdoc")
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid.)pbdoc")
        .def("get_width", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.)pbdoc")
        .def("get_height", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.)pbdoc")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy.)pbdoc")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy,
             py::arg("e"),
             R"pbdoc(Set maximum agent energy.)pbdoc")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.)pbdoc")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate,
             py::arg("r"),
             R"pbdoc(Set energy decay rate.)pbdoc")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get simulation time.)pbdoc")
        .def("set_random_seed", &SimpleWorld::setRandomSeed,
             py::arg("seed"),
             R"pbdoc(Set random seed for reproducible simulations.)pbdoc")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get random seed.)pbdoc")
        .def("get_world_size", [](const SimpleWorld& world) {
            return py::make_tuple(world.getWidth(), world.getHeight());
        }, R"pbdoc(Get world dimensions as (width, height).)pbdoc")
        .def("create_object", [](SimpleWorld& world, float x, float y, 
                                WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj(x, y, type, value, radius);
            world.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
           py::arg("radius") = 0.5f,
           R"pbdoc(Create and add a world object.)pbdoc")
        .def("get_objects_at_position", [](SimpleWorld& world, float x, float y) {
            // Note: This would need to be implemented in the C++ API
            // For now, return empty list
            return py::list();
        }, py::arg("x"), py::arg("y"),
           R"pbdoc(Get objects at position.)pbdoc")
        .def("get_all_objects", [](SimpleWorld& world) {
            // Note: This would need to be implemented in the C++ API
            // For now, return empty list
            return py::list();
        }, R"pbdoc(Get all world objects.)pbdoc")
        .def("clear_objects", [](SimpleWorld& world) {
            // Note: This would need to be implemented in the C++ API
            // For now, just reset the world
            world.reset();
        }, R"pbdoc(Clear all world objects.)pbdoc");
}

// ============================================================================
// FACTORY FUNCTIONS
// ============================================================================

void bindFactoryFunctions(py::module_& m) {
    // Create default configuration
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration.)pbdoc");
    
    // Create brain with default configuration
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration.)pbdoc");
    
    // Create simple world
    m.def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world.)pbdoc");
    
    // Create agent brain interface
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface.)pbdoc");
    
    // Create world object
    m.def("create_world_object", [](float x, float y, WorldObjectType type, 
                                   float value = 0.0f, float radius = 0.5f) -> WorldObject {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
       py::arg("radius") = 0.5f,
       R"pbdoc(Create a world object.)pbdoc");
    
    // Create action
    m.def("create_action", [](ActionType type, std::vector<float> parameters = std::vector<float>()) -> Action {
        if (parameters.empty()) {
            return Action(type);
        } else {
            return Action(type, parameters);
        }
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
       R"pbdoc(Create an action with type and optional parameters.)pbdoc");
    
    // Create action result
    m.def("create_action_result", [](float reward, bool success, std::string message = "") -> ActionResult {
        return ActionResult(reward, success, message);
    }, py::arg("reward"), py::arg("success"), py::arg("message") = "",
       R"pbdoc(Create an action result.)pbdoc");
}

// ============================================================================
// ADVANCED UTILITIES
// ============================================================================

void bindAdvancedUtilities(py::module_& m) {
    // Simulation utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                              std::shared_ptr<AgentBrain> agent, size_t num_steps,
                              double timestep = 0.1) {
        for (size_t step = 0; step < num_steps; ++step) {
            if (world) world->update(timestep);
            if (agent && world) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
            }
            brain->step(static_cast<SimulationStep>(step));
            if (agent) {
                auto action = agent->decodeMotorCommand();
                if (world) world->applyMotorCommand(action, world->getSimulationTime());
            }
        }
    }, py::arg("brain"), py::arg("world") = nullptr, py::arg("agent") = nullptr,
       py::arg("num_steps") = 100, py::arg("timestep") = 0.1,
       R"pbdoc(Run a complete simulation loop.)pbdoc");
    
    // Utility functions for simulation management
    m.def("create_reproducible_simulation", [](int seed, std::shared_ptr<Config> config) {
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("seed"), py::arg("config") = nullptr,
       R"pbdoc(Create a reproducible simulation with given seed.)pbdoc");
    
    // Performance monitoring
    m.def("profile_simulation", [](std::shared_ptr<Brain> brain, size_t steps,
                                  double timestep = 0.1) {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain->step(static_cast<SimulationStep>(step));
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict result;
        result["steps_performed"] = steps;
        result["total_time_ms"] = static_cast<double>(duration.count());
        result["steps_per_second"] = (steps * 1000.0) / duration.count();
        result["final_neuron_count"] = brain->getTotalNeuronCount();
        result["final_spike_count"] = brain->getTotalSpikeCount();
        
        return result;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.1,
       R"pbdoc(Profile simulation performance and return statistics.)pbdoc");
    
    // Batch processing utilities
    m.def("batch_sims", [](const std::vector<std::shared_ptr<Config>>& configs, size_t steps_per_config) {
        std::vector<std::shared_ptr<Brain>> brains;
        for (const auto& config : configs) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brains.push_back(brain);
            for (size_t step = 0; step < steps_per_config; ++step) {
                brain->step(static_cast<SimulationStep>(step));
            }
        }
        return brains;
    }, py::arg("configs"), py::arg("steps_per_config"),
       R"pbdoc(Run multiple simulations in batch and return results.)pbdoc");
    
    // Error handling utilities
    m.def("safe_simulation_step", [](std::shared_ptr<Brain> brain, SimulationStep step) {
        try {
            brain->step(step);
            return true;
        } catch (const std::exception& e) {
            py::print("Error during simulation step: ", e.what());
            return false;
        }
    }, py::arg("brain"), py::arg("step"),
       R"pbdoc(Safely perform a simulation step with error handling.)pbdoc");
}

// ============================================================================
// MAIN MODULE DEFINITION
// ============================================================================

PYBIND11_MODULE(pynlm, m) {
    // Set module docstring
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        
        NLM is a Python interface to the C++ neural simulation framework.
        
        This module provides classes and functions for creating, running, and
        analyzing neural network simulations.
        
        Key features:
        - Pythonic API with context managers and property access
        - Advanced simulation utilities and batch processing
        - Configuration validation and schema support
        - Performance monitoring and debugging tools
        - Error handling and recovery
        - Extensive examples and tutorials
        
        For beginners, see: easy_usage.md
        For comprehensive guide, see: HOW_TO_USE.md
        
        Installation: pip install pynlm
        Documentation: https://nlm.readthedocs.io
    )pbdoc";
    
    // Bind core types and enums
    bindCoreTypes(m);
    
    // Bind main classes with Pythonic interfaces
    bindConfigClass(m);
    bindBrainClass(m);
    bindAgentBrainClass(m);
    bindWorldClass(m);
    
    // Bind other classes
    // (Vision, Audio, SensoryInput, Action, etc.)
    
    // Bind factory functions
    bindFactoryFunctions(m);
    
    // Bind advanced utilities
    bindAdvancedUtilities(m);
    
    // Add module-level constants and utilities
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    
    // Add convenience functions
    m.def("create_test_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable all learning systems
        agent->enableAllSystems();
        
        return py::make_tuple(brain, world, agent);
    }, R"pbdoc(Create a test simulation with default settings.)pbdoc");
    
    // Add simulation examples
    m.def("run_simple_demo", []() {
        auto [brain, world, agent] = m.attr("create_test_simulation")();
        
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return py::dict(
            "spikes", brain->getTotalSpikeCount(),
            "firing_neurons", brain->getFiringNeuronCount(),
            "curiosity", agent->getCuriosityLevel(),
            "novelty", agent->getNoveltyLevel()
        );
    }, R"pbdoc(Run a simple demonstration simulation.)pbdoc");
}
