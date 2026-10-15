// Python bindings for NLM (Neural Learning Machine)
// This file provides Python bindings using pybind11 for the C++ NLM library
// Generated as part of the NLM Python API improvement project

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

// Include all NLM headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "sensory/SensoryInput.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/MotorSystem.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "experiments/Experiment.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace py = pybind11;

// Forward declarations for Python binding
class PyConfig;
class PyBrain;
class PyAgentBrain;
class PySimpleWorld;
class PyObservation;
class PyAction;
class PySensoryPercept;
class PyMotorCommand;
class PyNeuralWorkingMemory;
class PyNeuralEpisodicMemory;
class PyNeuralAssociativeMemory;
class PyDopamine;
class PyCuriosity;
class PyNovelty;
class PyPredictionError;
class PyVision;
class PyAudio;
class PySensoryInput;
class PyInternalSignals;
class PyMotorSystem;
class PyPredictionSystem;
class PyNeuralPlanner;
class PyConceptFormation;
class PyAttentionalSelection;
class PyDevelopmentSystem;

// Helper functions for Python integration
namespace pybind11 {
    // Allow conversion between std::vector and Python lists for specific types
    template<typename T>
    struct type_caster<std::vector<T>> {
        PYBIND11_TYPE_CASTER(std::vector<T>, _s PyList_<T>);
        
        static handle cast(const std::vector<T> &src, return_value_policy policy, handle parent) {
            return py::cast(src, policy, parent);
        }
    };
    
    // Allow conversion between std::map and Python dicts
    template<typename K, typename V>
    struct type_caster<std::map<K, V>> {
        PYBIND11_TYPE_CASTER(std::map<K, V>, _s PyDict_<K, V>);
        
        static handle cast(const std::map<K, V> &src, return_value_policy policy, handle parent) {
            return py::cast(src, policy, parent);
        }
    };
}

// Python bindings module
PYBIND11_MODULE(_pynlm_core, m) {
    m.doc() = R"pbdoc(
    Python bindings for NLM (Neural Learning Machine) Core Library
    
    This module provides Python bindings for the C++ NLM brain simulation framework,
    exposing the core neural computation capabilities including:
    
    - LIF neuron dynamics
    - Event-driven spike propagation
    - STDP and Hebbian plasticity
    - Structural plasticity (synaptogenesis/pruning)
    - Working memory systems
    - Neuromodulation (dopamine, curiosity, novelty)
    - Prediction systems
    - Development systems
    
    The bindings are designed to be intuitive and Pythonic while maintaining
    access to the full power of the C++ implementation.
    )pbdoc";
    
    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__doc__") = "NLM Python bindings for neural simulation";
    
    // Exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<EnvironmentError>(m, "EnvironmentError", PyExc_RuntimeError);
    
    // Configuration classes
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def(py::init<const std::string&>())  // Load from file
        
        // Load from file
        .def("loadFromFile", &nlm::Config::loadFromFile,
             py::arg("filepath"), "Load configuration from JSON file")
        
        // Save to file
        .def("saveToFile", &nlm::Config::saveToFile,
             py::arg("filepath"), "Save configuration to JSON file")
        
        // Get values
        .def("get", py::overload_cast<const std::string&>(&nlm::Config::get),
             py::arg("key"), "Get configuration value")
        .def("getOr", py::overload_cast<const std::string&, const int&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get value or default")
        .def("getOr", py::overload_cast<const std::string&, const double&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get value or default")
        .def("getOr", py::overload_cast<const std::string&, const bool&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get value or default")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get value or default")
        
        // Set values
        .def("set", py::overload_cast<const std::string&, const int>(&nlm::Config::set),
             py::arg("key"), py::arg("value"), "Set integer value")
        .def("set", py::overload_cast<const std::string&, const double>(&nlm::Config::set),
             py::arg("key"), py::arg("value"), "Set double value")
        .def("set", py::overload_cast<const std::string&, const bool>(&nlm::Config::set),
             py::arg("key"), py::arg("value"), "Set boolean value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&nlm::Config::set),
             py::arg("key"), py::arg("value"), "Set string value")
        .def("set", py::overload_cast<const std::string&, const nlm::ConfigValue&>(&nlm::Config::set),
             py::arg("key"), py::arg("value"), "Set value from ConfigValue")
        
        // Check existence
        .def("has", &nlm::Config::has, py::arg("key"), "Check if key exists")
        
        // Get all keys
        .def("getKeys", &nlm::Config::getKeys, "Get all configuration keys")
        
        // Clear all
        .def("clear", &nlm::Config::clear, "Clear all configuration")
        
        // Get configuration summary
        .def("summary", &nlm::Config::summary, "Get configuration summary")
        
        // Statistics and utilities
        .def("getKeys", &nlm::Config::getKeys)
        .def("has", &nlm::Config::has)
        .def("getOr", py::overload_cast<const std::string&, const int64_t&, const int64_t&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get int64_t or default")
        .def("getOr", py::overload_cast<const std::string&, const float&, const float&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get float or default")
        .def("getOr", py::overload_cast<const std::string&, const std::vector<int>&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get int vector or default")
        .def("getOr", py::overload_cast<const std::string&, const std::vector<double>&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get double vector or default")
        .def("getOr", py::overload_cast<const std::string&, const std::vector<std::string>&>(&nlm::Config::getOr),
             py::arg("key"), py::arg("default_value"), "Get string vector or default")
        
        .def(py::self == py::self)
        .def(py::self != py::self)
        
        .def("__str__", &nlm::Config::summary)
        .def("__repr__", [](const nlm::Config& config) {
            return "Config(keys=" + std::to_string(config.getKeys().size()) + ")";
        });
    
    // Create default configuration function
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<nlm::Config>();
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("plasticity_learning_rate", 0.01f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_time_constant", 20.0f);
        config->set("development_synaptogenesis_rate", 0.001f);
        config->set("development_pruning_rate", 0.0001f);
        config->set("dopamine_baseline", 0.1f);
        config->set("reward_discount_factor", 0.99f);
        config->set("environment_name", std::string("GridWorld"));
        config->set("environment_width", 10);
        config->set("environment_height", 10);
        config->set("log_level", std::string("INFO"));
        config->set("log_to_file", false);
        config->set("log_filename", std::string("nlm.log"));
        config->set("max_simulation_steps", 10000);
        config->set("simulation_time_limit", 0.0);
        config->set("visualization_enabled", false);
        config->set("visualization_update_rate", 30.0f);
        return config;
    }, "Create a default configuration with sensible values");
    
    // Brain class
    py::class_<nlm::Brain, std::shared_ptr<nlm::Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"), "Create brain with configuration")
        
        .def("initialize", &nlm::Brain::initialize, "Initialize brain with configuration")
        .def("step", py::overload_cast<nlm::SimulationStep>(&nlm::Brain::step),
             py::arg("current_step"), "Main simulation step")
        .def("step", py::overload_cast<nlm::SimulationStep, nlm::Timestamp>(&nlm::Brain::step),
             py::arg("current_step"), py::arg("current_time"), "Main simulation step with time")
        
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"), "Receive sensory input from environment")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current directly into neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current to all neurons of specific type")
        
        .def("getSpikeSystem", &nlm::Brain::getSpikeSystem, "Get spike system")
        .def("getSTDP", &nlm::Brain::getSTDP, "Get STDP plasticity system")
        .def("getHebbian", &nlm::Brain::getHebbian, "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &nlm::Brain::getStructuralPlasticity, "Get structural plasticity system")
        
        .def("getExcitationInhibitionRatio", &nlm::Brain::getExcitationInhibitionRatio, "Get excitation/inhibition ratio")
        .def("getTotalSpikeCount", &nlm::Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &nlm::Brain::getPendingSpikeEventCount, "Get pending spike event count")
        .def("produceAction", &nlm::Brain::produceAction, "Produce motor/action output")
        .def("applyNeuromodulation", &nlm::Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signals")
        .def("updatePlasticity", &nlm::Brain::updatePlasticity, "Update plasticity rules")
        .def("develop", &nlm::Brain::develop, "Apply developmental changes")
        
        .def("reset", &nlm::Brain::reset, "Reset brain state")
        .def("save", &nlm::Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"), "Load brain state from file")
        
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name"), "Add brain region")
        .def("getRegion", py::overload_cast<nlm::RegionId>(&nlm::Brain::getRegion, py::const_),
             py::arg("id"), "Get brain region by ID")
        .def("getRegionCount", &nlm::Brain::getRegionCount, "Get region count")
        .def("getRegionIds", &nlm::Brain::getRegionIds, "Get all region IDs")
        
        .def("addInterRegionConnection", &nlm::Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add inter-region connection")
        .def("removeInterRegionConnection", &nlm::Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"), "Remove inter-region connection")
        
        .def("getTotalNeuronCount", &nlm::Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &nlm::Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &nlm::Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &nlm::Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &nlm::Brain::getAverageFiringRate, "Get average firing rate")
        
        .def("getWorkingMemory", &nlm::Brain::getWorkingMemory, "Get working memory")
        .def("getEpisodicMemory", &nlm::Brain::getEpisodicMemory, "Get episodic memory")
        .def("getAssociativeMemory", &nlm::Brain::getAssociativeMemory, "Get associative memory")
        
        .def("getPredictionSystem", &nlm::Brain::getPredictionSystem, "Get prediction system")
        
        .def("getPlanner", &nlm::Brain::getPlanner, "Get neural planner")
        .def("getConceptFormation", &nlm::Brain::getConceptFormation, "Get concept formation")
        .def("getAttention", &nlm::Brain::getAttention, "Get attentional selection")
        
        .def("getDevelopmentSystem", &nlm::Brain::getDevelopmentSystem, "Get development system")
        .def("getDevelopmentalStage", &nlm::Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        
        .def("getDopamine", &nlm::Brain::getDopamine, "Get dopamine system")
        .def("getCuriosity", &nlm::Brain::getCuriosity, "Get curiosity system")
        .def("getNovelty", &nlm::Brain::getNovelty, "Get novelty system")
        .def("getPredictionErrorSignal", &nlm::Brain::getPredictionErrorSignal, "Get prediction error signal")
        
        .def("getConfig", &nlm::Brain::getConfig, "Get configuration")
        .def("getRandomGenerator", &nlm::Brain::getRandomGenerator, "Get random generator")
        
        .def("logStatus", &nlm::Brain::logStatus, "Log brain status")
        
        .def("__str__", [](const nlm::Brain& brain) {
            return "Brain(neurons=" + std::to_string(brain.getTotalNeuronCount()) + 
                   ", spikes=" + std::to_string(brain.getTotalSpikeCount()) + ")";
        });
    
    // AgentBrain class
    py::class_<nlm::AgentBrain, std::shared_ptr<nlm::AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"), "Create agent brain")
        
        .def("initialize", &nlm::AgentBrain::initialize,
             py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize, "Get sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize, "Get motor output size")
        
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand, "Decode motor command")
        
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"), "Apply reward modulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        
        .def("getDevelopmentalStage", &nlm::AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError, "Get prediction error")
        
        .def("reset", &nlm::AgentBrain::reset, "Reset agent")
        .def("getBrain", &nlm::AgentBrain::getBrain, "Get underlying brain")
        
        // Configuration setters
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        // Configuration getters
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled,
             "Check if development enabled")
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled,
             "Check if curiosity enabled");
    
    // SimpleWorld class
    py::class_<nlm::SimpleWorld, std::shared_ptr<nlm::SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        
        .def("configure", &nlm::SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and agent vision")
        .def("reset", &nlm::SimpleWorld::reset, "Reset world to initial state")
        
        .def("update", &nlm::SimpleWorld::update,
             py::arg("time"), "Update world by given time")
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept, "Get sensory percept")
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("action"), py::arg("time"), "Apply motor command")
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime, "Get simulation time")
        
        .def("getWidth", &nlm::SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &nlm::SimpleWorld::getHeight, "Get world height")
        .def("getVisionWidth", &nlm::SimpleWorld::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &nlm::SimpleWorld::getVisionHeight, "Get vision height");
    
    // SensoryPercept class
    py::class_<nlm::SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVisionData", &nlm::SensoryPercept::getVisionData, "Get vision data")
        .def("getTouchData", &nlm::SensoryPercept::getTouchData, "Get touch data")
        .def("getInternalData", &nlm::SensoryPercept::getInternalData, "Get internal data")
        .def("getProprioceptionData", &nlm::SensoryPercept::getProprioceptionData, "Get proprioception data")
        .def("getAudioData", &nlm::SensoryPercept::getAudioData, "Get audio data")
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp, "Get timestamp");
    
    // MotorCommand class
    py::class_<nlm::MotorCommand>(m, "MotorCommand")
        .def(py::init<>())
        .def("getForward", &nlm::MotorCommand::getForward, "Get forward command")
        .def("getBackward", &nlm::MotorCommand::getBackward, "Get backward command")
        .def("getTurnLeft", &nlm::MotorCommand::getTurnLeft, "Get turn left command")
        .def("getTurnRight", &nlm::MotorCommand::getTurnRight, "Get turn right command")
        .def("getInteract", &nlm::MotorCommand::getInteract, "Get interact command")
        .def("getWait", &nlm::MotorCommand::getWait, "Get wait command")
        .def("setForward", &nlm::MotorCommand::setForward, "Set forward command")
        .def("setBackward", &nlm::MotorCommand::setBackward, "Set backward command")
        .def("setTurnLeft", &nlm::MotorCommand::setTurnLeft, "Set turn left command")
        .def("setTurnRight", &nlm::MotorCommand::setTurnRight, "Set turn right command")
        .def("setInteract", &nlm::MotorCommand::setInteract, "Set interact command")
        .def("setWait", &nlm::MotorCommand::setWait, "Set wait command")
        .def("getCommand", &nlm::MotorCommand::getCommand, "Get command vector")
        .def("setCommand", &nlm::MotorCommand::setCommand, "Set command vector")
        .def("normalize", &nlm::MotorCommand::normalize, "Normalize command vector");
    
    // Action class
    py::class_<nlm::Action, std::shared_ptr<nlm::Action>>(m, "Action")
        .def(py::init<>())
        .def("getForward", &nlm::Action::getForward, "Get forward action")
        .def("getBackward", &nlm::Action::getBackward, "Get backward action")
        .def("getTurnLeft", &nlm::Action::getTurnLeft, "Get turn left action")
        .def("getTurnRight", &nlm::Action::getTurnRight, "Get turn right action")
        .def("getInteract", &nlm::Action::getInteract, "Get interact action")
        .def("getWait", &nlm::Action::getWait, "Get wait action")
        .def("setForward", &nlm::Action::setForward, "Set forward action")
        .def("setBackward", &nlm::Action::setBackward, "Set backward action")
        .def("setTurnLeft", &nlm::Action::setTurnLeft, "Set turn left action")
        .def("setTurnRight", &nlm::Action::setTurnRight, "Set turn right action")
        .def("setInteract", &nlm::Action::setInteract, "Set interact action")
        .def("setWait", &nlm::Action::setWait, "Set wait action");
    
    // Observation class
    py::class_<nlm::Observation, std::shared_ptr<nlm::Observation>>(m, "Observation")
        .def(py::init<>())
        .def("getSensoryPercept", &nlm::Observation::getSensoryPercept, "Get sensory percept")
        .def("setSensoryPercept", &nlm::Observation::setSensoryPercept,
             py::arg("percept"), "Set sensory percept")
        .def("getTimestamp", &nlm::Observation::getTimestamp, "Get timestamp")
        .def("setTimestamp", &nlm::Observation::setTimestamp,
             py::arg("timestamp"), "Set timestamp");
    
    // Vision class
    py::class_<nlm::Vision, std::shared_ptr<nlm::Vision>>(m, "Vision")
        .def(py::init<>())
        .def("configure", &nlm::Vision::configure,
             py::arg("width"), py::arg("height"), "Configure vision dimensions")
        .def("process", &nlm::Vision::process, "Process visual input")
        .def("getWidth", &nlm::Vision::getWidth, "Get vision width")
        .def("getHeight", &nlm::Vision::getHeight, "Get vision height")
        .def("getData", &nlm::Vision::getData, "Get vision data");
    
    // Audio class
    py::class_<nlm::Audio, std::shared_ptr<nlm::Audio>>(m, "Audio")
        .def(py::init<>())
        .def("configure", &nlm::Audio::configure,
             py::arg("sample_rate"), py::arg("channels"), "Configure audio settings")
        .def("process", &nlm::Audio::process, "Process audio input")
        .def("getSampleRate", &nlm::Audio::getSampleRate, "Get sample rate")
        .def("getChannels", &nlm::Audio::getChannels, "Get channels")
        .def("getData", &nlm::Audio::getData, "Get audio data");
    
    // InternalSignals class
    py::class_<nlm::InternalSignals, std::shared_ptr<nlm::InternalSignals>>(m, "InternalSignals")
        .def(py::init<>())
        .def("configure", &nlm::InternalSignals::configure, "Configure internal signals")
        .def("process", &nlm::InternalSignals::process, "Process internal signals")
        .def("getHeartRate", &nlm::InternalSignals::getHeartRate, "Get heart rate")
        .def("getRespiratoryRate", &nlm::InternalSignals::getRespiratoryRate, "Get respiratory rate")
        .def("getMetabolicRate", &nlm::InternalSignals::getMetabolicRate, "Get metabolic rate")
        .def("getData", &nlm::InternalSignals::getData, "Get internal signals data");
    
    // SensoryInput class
    py::class_<nlm::SensoryInput, std::shared_ptr<nlm::SensoryInput>>(m, "SensoryInput")
        .def(py::init<>())
        .def("addVision", &nlm::SensoryInput::addVision,
             py::arg("vision"), "Add vision input")
        .def("addAudio", &nlm::SensoryInput::addAudio,
             py::arg("audio"), "Add audio input")
        .def("addInternal", &nlm::SensoryInput::addInternal,
             py::arg("internal"), "Add internal signals")
        .def("addTouch", &nlm::SensoryInput::addTouch,
             py::arg("touch"), "Add touch input")
        .def("addProprioception", &nlm::SensoryInput::addProprioception,
             py::arg("proprioception"), "Add proprioception input")
        .def("getVision", &nlm::SensoryInput::getVision, "Get vision input")
        .def("getAudio", &nlm::SensoryInput::getAudio, "Get audio input")
        .def("getInternal", &nlm::SensoryInput::getInternal, "Get internal signals")
        .def("getTouch", &nlm::SensoryInput::getTouch, "Get touch input")
        .def("getProprioception", &nlm::SensoryInput::getProprioception, "Get proprioception input");
    
    // MotorSystem class
    py::class_<nlm::MotorSystem, std::shared_ptr<nlm::MotorSystem>>(m, "MotorSystem")
        .def(py::init<>())
        .def("configure", &nlm::MotorSystem::configure, "Configure motor system")
        .def("process", &nlm::MotorSystem::process, "Process motor output")
        .def("addAction", &nlm::MotorSystem::addAction,
             py::arg("action"), "Add action")
        .def("getActions", &nlm::MotorSystem::getActions, "Get actions")
        .def("clearActions", &nlm::MotorSystem::clearActions, "Clear actions");
    
    // Neuromodulation classes
    py::class_<nlm::Dopamine, std::shared_ptr<nlm::Dopamine>>(m, "Dopamine")
        .def(py::init<>())
        .def("configure", &nlm::Dopamine::configure,
             py::arg("baseline"), py::arg("learning_rate"), "Configure dopamine")
        .def("process", &nlm::Dopamine::process, "Process dopamine signal")
        .def("getLevel", &nlm::Dopamine::getLevel, "Get dopamine level")
        .def("setLevel", &nlm::Dopamine::setLevel,
             py::arg("level"), "Set dopamine level")
        .def("getLearningRate", &nlm::Dopamine::getLearningRate, "Get learning rate");
    
    py::class_<nlm::Curiosity, std::shared_ptr<nlm::Curiosity>>(m, "Curiosity")
        .def(py::init<>())
        .def("configure", &nlm::Curiosity::configure,
             py::arg("baseline"), py::arg("exploration_rate"), "Configure curiosity")
        .def("process", &nlm::Curiosity::process, "Process curiosity signal")
        .def("getLevel", &nlm::Curiosity::getLevel, "Get curiosity level")
        .def("setLevel", &nlm::Curiosity::setLevel,
             py::arg("level"), "Set curiosity level")
        .def("getExplorationRate", &nlm::Curiosity::getExplorationRate, "Get exploration rate");
    
    py::class_<nlm::Novelty, std::shared_ptr<nlm::Novelty>>(m, "Novelty")
        .def(py::init<>())
        .def("configure", &nlm::Novelty::configure,
             py::arg("decay_rate"), "Configure novelty detection")
        .def("process", &nlm::Novelty::process, "Process novelty signal")
        .def("getLevel", &nlm::Novelty::getLevel, "Get novelty level")
        .def("setLevel", &nlm::Novelty::setLevel,
             py::arg("level"), "Set novelty level")
        .def("getDecayRate", &nlm::Novelty::getDecayRate, "Get decay rate");
    
    py::class_<nlm::PredictionError, std::shared_ptr<nlm::PredictionError>>(m, "PredictionError")
        .def(py::init<>())
        .def("configure", &nlm::PredictionError::configure,
             py::arg("sensitivity"), "Configure prediction error")
        .def("process", &nlm::PredictionError::process, "Process prediction error")
        .def("getLevel", &nlm::PredictionError::getLevel, "Get prediction error level")
        .def("setLevel", &nlm::PredictionError::setLevel,
             py::arg("level"), "Set prediction error level")
        .def("getSensitivity", &nlm::PredictionError::getSensitivity, "Get sensitivity");
    
    // Memory classes
    py::class_<nlm::WorkingMemory, std::shared_ptr<nlm::WorkingMemory>>(m, "WorkingMemory")
        .def(py::init<>())
        .def("configure", &nlm::WorkingMemory::configure, "Configure working memory")
        .def("store", &nlm::WorkingMemory::store,
             py::arg("pattern"), py::arg("value"), "Store pattern and value")
        .def("retrieve", &nlm::WorkingMemory::retrieve,
             py::arg("pattern"), "Retrieve value for pattern")
        .def("has", &nlm::WorkingMemory::has, "Check if pattern exists")
        .def("clear", &nlm::WorkingMemory::clear, "Clear memory")
        .def("getSize", &nlm::WorkingMemory::getSize, "Get memory size")
        .def("getPatterns", &nlm::WorkingMemory::getPatterns, "Get all patterns");
    
    py::class_<nlm::EpisodicMemory, std::shared_ptr<nlm::EpisodicMemory>>(m, "EpisodicMemory")
        .def(py::init<>())
        .def("configure", &nlm::EpisodicMemory::configure, "Configure episodic memory")
        .def("store", &nlm::EpisodicMemory::store,
             py::arg("episode"), py::arg("timestamp"), "Store episode")
        .def("retrieve", &nlm::EpisodicMemory::retrieve,
             py::arg("pattern"), "Retrieve episode by pattern")
        .def("replay", &nlm::EpisodicMemory::replay,
             py::arg("start_time"), py::arg("duration"), "Replay episodes")
        .def("getSize", &nlm::EpisodicMemory::getSize, "Get memory size")
        .def("getTimestampRange", &nlm::EpisodicMemory::getTimestampRange, "Get timestamp range");
    
    py::class_<nlm::AssociativeMemory, std::shared_ptr<nlm::AssociativeMemory>>(m, "AssociativeMemory")
        .def(py::init<>())
        .def("configure", &nlm::AssociativeMemory::configure, "Configure associative memory")
        .def("associate", &nlm::AssociativeMemory::associate,
             py::arg("input_pattern"), py::arg("output_pattern"), py::arg("strength"),
             "Associate input with output pattern")
        .def("recall", &nlm::AssociativeMemory::recall,
             py::arg("input_pattern"), "Recall output for input pattern")
        .def("getStrength", &nlm::AssociativeMemory::getStrength,
             py::arg("input_pattern"), py::arg("output_pattern"), "Get association strength")
        .def("getAssociations", &nlm::AssociativeMemory::getAssociations,
             py::arg("input_pattern"), "Get all associations for input pattern")
        .def("clear", &nlm::AssociativeMemory::clear, "Clear memory")
        .def("getSize", &nlm::AssociativeMemory::getSize, "Get memory size");
    
    // NeuralWorkingMemory class
    py::class_<nlm::NeuralWorkingMemory, std::shared_ptr<nlm::NeuralWorkingMemory>>(m, "NeuralWorkingMemory")
        .def(py::init<>())
        .def("configure", &nlm::NeuralWorkingMemory::configure, "Configure neural working memory")
        .def("store", &nlm::NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("value"), "Store neural pattern")
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve,
             py::arg("pattern"), "Retrieve neural pattern")
        .def("getFiringNeurons", &nlm::NeuralWorkingMemory::getFiringNeurons, "Get firing neurons")
        .def("clear", &nlm::NeuralWorkingMemory::clear, "Clear neural working memory")
        .def("getSize", &nlm::NeuralWorkingMemory::getSize, "Get memory size")
        .def("getPatterns", &nlm::NeuralWorkingMemory::getPatterns, "Get all patterns");
    
    // NeuralEpisodicMemory class
    py::class_<nlm::NeuralEpisodicMemory, std::shared_ptr<nlm::NeuralEpisodicMemory>>(m, "NeuralEpisodicMemory")
        .def(py::init<>())
        .def("configure", &nlm::NeuralEpisodicMemory::configure, "Configure neural episodic memory")
        .def("store", &nlm::NeuralEpisodicMemory::store,
             py::arg("episode"), py::arg("timestamp"), "Store neural episode")
        .def("retrieve", &nlm::NeuralEpisodicMemory::retrieve,
             py::arg("pattern"), "Retrieve neural episode")
        .def("replay", &nlm::NeuralEpisodicMemory::replay,
             py::arg("start_time"), py::arg("duration"), "Replay neural episodes")
        .def("getSize", &nlm::NeuralEpisodicMemory::getSize, "Get memory size")
        .def("getTimestampRange", &nlm::NeuralEpisodicMemory::getTimestampRange, "Get timestamp range");
    
    // PredictionSystem class
    py::class_<nlm::PredictionSystem, std::shared_ptr<nlm::PredictionSystem>>(m, "PredictionSystem")
        .def(py::init<>())
        .def("configure", &nlm::PredictionSystem::configure, "Configure prediction system")
        .def("predict", &nlm::PredictionSystem::predict,
             py::arg("state"), "Make prediction")
        .def("update", &nlm::PredictionSystem::update,
             py::arg("state"), py::arg("reward"), "Update prediction")
        .def("getPredictionError", &nlm::PredictionSystem::getPredictionError, "Get prediction error")
        .def("getConfidence", &nlm::PredictionSystem::getConfidence, "Get prediction confidence")
        .def("getSize", &nlm::PredictionSystem::getSize, "Get prediction system size");
    
    // NeuralPrediction class
    py::class_<nlm::NeuralPrediction, std::shared_ptr<nlm::NeuralPrediction>>(m, "NeuralPrediction")
        .def(py::init<>())
        .def("configure", &nlm::NeuralPrediction::configure, "Configure neural prediction")
        .def("storePattern", &nlm::NeuralPrediction::storePattern,
             py::arg("pattern"), py::arg("state"), "Store prediction pattern")
        .def("recallPattern", &nlm::NeuralPrediction::recallPattern,
             py::arg("pattern"), "Recall prediction pattern")
        .def("getPatternCount", &nlm::NeuralPrediction::getPatternCount, "Get pattern count")
        .def("clear", &nlm::NeuralPrediction::clear, "Clear predictions");
    
    // DevelopmentSystem class
    py::class_<nlm::DevelopmentSystem, std::shared_ptr<nlm::DevelopmentSystem>>(m, "DevelopmentSystem")
        .def(py::init<>())
        .def("configure", &nlm::DevelopmentSystem::configure, "Configure development system")
        .def("update", &nlm::DevelopmentSystem::update,
             py::arg("brain"), py::arg("timestep"), "Update development")
        .def("getStage", &nlm::DevelopmentSystem::getStage, "Get current stage")
        .def("setStage", &nlm::DevelopmentSystem::setStage,
             py::arg("stage"), "Set developmental stage")
        .def("getMaturationProgress", &nlm::DevelopmentSystem::getMaturationProgress, "Get maturation progress")
        .def("isMature", &nlm::DevelopmentSystem::isMature, "Check if mature");
    
    // Utility functions
    m.def("getVersion", []() {
        return "NLM Python Bindings 0.1.0";
    }, "Get NLM version string");
    
    m.def("getBuildInfo", []() {
        return py::dict(
            "version", "0.1.0",
            "build_type", "Release",
            "compiler", "C++20",
            "features", py::dict(
                "lif_neurons", true,
                "stdp_plasticity", true,
                "hebbian_learning", true,
                "structural_plasticity", true,
                "working_memory", true,
                "episodic_memory", true,
                "associative_memory", true,
                "neuromodulation", true,
                "prediction_system", true,
                "cognition", true,
                "development", true,
                "world_interaction", true
            )
        );
    }, "Get build information");
    
    m.def("validateConfig", [](std::shared_ptr<nlm::Config> config) {
        if (!config) return false;
        
        auto keys = config->getKeys();
        if (keys.empty()) return false;
        
        // Check for required keys
        std::vector<std::string> required_keys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : required_keys) {
            if (!config->has(key)) return false;
        }
        
        return true;
    }, py::arg("config"), "Validate configuration object");
    
    // Type conversion helpers
    py::class_<nlm::ConfigValue>(m, "ConfigValue")
        .def(py::init<>())
        .def("__str__", [](const nlm::ConfigValue& value) {
            return "ConfigValue";
        });
    
    py::class_<nlm::ConfigSource>(m, "ConfigSource")
        .def(py::init<>())
        .def("__str__", [](nlm::ConfigSource source) {
            switch (source) {
                case nlm::ConfigSource::Default: return "Default";
                case nlm::ConfigSource::File: return "File";
                case nlm::ConfigSource::CommandLine: return "CommandLine";
                case nlm::ConfigSource::Runtime: return "Runtime";
                default: return "Unknown";
            }
        });
    
    // Neuron type enum
    py::enum_<nlm::NeuronType>(m, "NeuronType")
        .value("Input", nlm::NeuronType::Input)
        .value("Hidden", nlm::NeuronType::Hidden)
        .value("Output", nlm::NeuronType::Output)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .export_values();
    
    // Developmental stage enum
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Embryo", nlm::DevelopmentalStage::Embryo)
        .value("Fetal", nlm::DevelopmentalStage::Fetal)
        .value("Infant", nlm::DevelopmentalStage::Infant)
        .value("Juvenile", nlm::DevelopmentalStage::Juvenile)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .export_values();
    
    // Neuron ID type
    py::class_<nlm::NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>())
        .def("getId", &nlm::NeuronId::getId)
        .def("toString", &nlm::NeuronId::toString)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self);
    
    // Synapse ID type
    py::class_<nlm::SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>())
        .def("getId", &nlm::SynapseId::getId)
        .def("toString", &nlm::SynapseId::toString)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self);
    
    // Region ID type
    py::class_<nlm::RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>())
        .def("getId", &nlm::RegionId::getId)
        .def("toString", &nlm::RegionId::toString)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self < py::self);
    
    // Plasticity flags
    py::enum_<nlm::PlasticityFlags>(m, "PlasticityFlags")
        .value("None", nlm::PlasticityFlags::None)
        .value("Hebbian", nlm::PlasticityFlags::Hebbian)
        .value("STDP", nlm::PlasticityFlags::STDP)
        .value("RewardModulated", nlm::PlasticityFlags::RewardModulated)
        .value("All", nlm::PlasticityFlags::All)
        .export_values();
    
    // Error types
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Set docstrings
    m.attr("__doc__") = R"pbdoc(
    Python bindings for NLM (Neural Learning Machine) Core Library
    
    This module provides Python bindings for the C++ NLM brain simulation framework,
    exposing the core neural computation capabilities including:
    
    - LIF neuron dynamics
    - Event-driven spike propagation
    - STDP and Hebbian plasticity
    - Structural plasticity (synaptogenesis/pruning)
    - Working memory systems
    - Neuromodulation (dopamine, curiosity, novelty)
    - Prediction systems
    - Development systems
    
    The bindings are designed to be intuitive and Pythonic while maintaining
    access to the full power of the C++ implementation.
    )pbdoc";
    
    // Print success message
    py::call_guard<py::print>(m);
}
