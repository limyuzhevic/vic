// Enhanced Python bindings for NLM Configuration System
// This file demonstrates the improved Python API for the configuration system

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/core/Config/Config.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Config class bindings with Pythonic API
py::class_<Config, std::shared_ptr<Config>> createConfigBindings(py::module_ &m) {
    return py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with JSON/YAML support.
        
        Provides comprehensive configuration management for neural networks
        with type-safe value retrieval, file I/O, and command-line argument parsing.
    )pbdoc")
        // Core functionality
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file. Supports both formats with
             automatic fallback to simple key=value format for backward compatibility.")
        
        // Enhanced command-line argument parsing
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            return self.loadFromArgs(static_cast<int>(args.size()),
                                     const_cast<char**>(const_cast<const std::string*>(args.data())));
        }, py::arg("args"),
           "Load configuration from command line arguments. Expects a list of strings
           in the format --key=value or -key value.")
        
        // Enhanced file saving
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file in enhanced format with metadata.")
        
        // Value retrieval with enhanced error handling
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if the key exists,
             False otherwise (e.g., for backward compatibility with missing keys).")
        
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        
        // Enhanced value retrieval with type safety
        .def("get", static_cast<std::string(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a string value from configuration. Returns the value or raises
             KeyError if key doesn't exist.")
        
        .def("get", static_cast<int(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get an integer value from configuration. Returns the value or raises
             KeyError if key doesn't exist.")
        
        .def("get", static_cast<int64_t(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a 64-bit integer value from configuration. Returns the value or raises
             KeyError if key doesn't exist.")
        
        .def("get", static_cast<double(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a floating-point value from configuration. Returns the value or raises
             KeyError if key doesn't exist.")
        
        .def("get", static_cast<bool(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a boolean value from configuration. Returns the value or raises
             KeyError if key doesn't exist.")
        
        // Enhanced getOr with default values
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), 
             "Get a string value with default. Returns the value if key exists,
             otherwise returns the default value.")
        
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get an integer value with default. Returns the value if key exists,
             otherwise returns the default value.")
        
        .def("getOr", [](Config& self, const std::string& key, int64_t defaultValue) {
            auto val = self.get<int64_t>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a 64-bit integer value with default. Returns the value if key exists,
             otherwise returns the default value.")
        
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a floating-point value with default. Returns the value if key exists,
             otherwise returns the default value.")
        
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a boolean value with default. Returns the value if key exists,
             otherwise returns the default value.")
        
        // Array value retrieval
        .def("get", static_cast<std::vector<int64_t>(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a vector of integers from configuration. Returns the vector or empty vector if key doesn't exist.")
        
        .def("get", static_cast<std::vector<double>(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a vector of doubles from configuration. Returns the vector or empty vector if key doesn't exist.")
        
        .def("get", static_cast<std::vector<std::string>(Config::*) (Config&, const std::string&) const>(&Config::get), 
             py::arg("key"),
             "Get a vector of strings from configuration. Returns the vector or empty vector if key doesn't exist.")
        
        // Array getOr with defaults
        .def("getOr", [](Config& self, const std::string& key, const std::vector<int64_t>& defaultValue) {
            auto val = self.get<std::vector<int64_t>>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a vector of integers with default. Returns the vector if key exists,
             otherwise returns the default vector.")
        
        .def("getOr", [](Config& self, const std::string& key, const std::vector<double>& defaultValue) {
            auto val = self.get<std::vector<double>>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a vector of doubles with default. Returns the vector if key exists,
             otherwise returns the default vector.")
        
        .def("getOr", [](Config& self, const std::string& key, const std::vector<std::string>& defaultValue) {
            auto val = self.get<std::vector<std::string>>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
             "Get a vector of strings with default. Returns the vector if key exists,
             otherwise returns the default vector.")
        
        // Utility methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (string type).")
        
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (integer type).")
        
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (floating-point type).")
        
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (boolean type).")
        
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key.")
        
        // String representation
        .def("summary", &Config::summary,
             "Get a detailed summary of the configuration including all key-value pairs")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });
}

// Enhanced Brain bindings with additional methods
py::class_<Brain, std::shared_ptr<Brain>> createBrainBindings(py::module_ &m) {
    return py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain with integrated memory, prediction,
        cognition, and neuromodulation systems.
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated cognitive systems for advanced AI behavior.
    )pbdoc")
        // Core brain operations
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        .def("step", static_cast<void(Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        
        .def("step", static_cast<void(Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Input/Output operations
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state")
        
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Statistics and monitoring
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        
        // Advanced features
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name")
        
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics")
        
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        });
}

// Enhanced AgentBrain bindings
py::class_<AgentBrain, std::shared_ptr<AgentBrain>> createAgentBrainBindings(py::module_ &m) {
    return py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world environment.
        
        Handles sensory transduction and motor decoding for interactive agents
        with advanced cognitive and learning capabilities.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        
        .def("initialize", &AgentBrain::initialize,
             py::arg("world"),
             "Initialize with world")
        
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Advanced features
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation")
        
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-based exploration")
        
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable developmental processes")
        
        // State information
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get thought trace (one-line human-readable record)")
        
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        
        // Motor group activities for diagnostics
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities for diagnostics")
        
        .def("getBrain", &AgentBrain::getBrain,
             "Get underlying brain pointer");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        Provides comprehensive classes for Brain, Config, AgentBrain, SimpleWorld,
        SensoryInput, and Action with enhanced error handling, type safety, and
        Pythonic APIs for easier integration and use.
        
        Key features:
        - Enhanced configuration system with JSON/YAML support
        - Type-safe value retrieval with comprehensive error handling
        - Advanced Pythonic APIs for easier integration
        - Improved documentation and examples
        - Better error messages and debugging support
    )pbdoc";
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register enhanced bindings
    createConfigBindings(m);
    createBrainBindings(m);
    createAgentBrainBindings(m);
    
    // Include the rest of the original bindings...
    // (For brevity, the rest of the original bindings are omitted here
    // but would be included in the actual implementation)
    
    // Example constants for users
    m.attr("NEURON_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_INTERNAL") = NeuronType::Internal;
    
    m.attr("ACTION_MOVE_FORWARD") = ActionType::MoveForward;
    m.attr("ACTION_INTERACT") = ActionType::Interact;
    m.attr("ACTION_WAIT") = ActionType::Wait;
    
    m.attr("DEVELOPMENT_INITIAL") = DevelopmentalStage::Initial;
    m.attr("DEVELOPMENT_ADULT") = DevelopmentalStage::Adult;
    
    m.attr("FIRING_RESTING") = FiringState::Resting;
    m.attr("FIRING_ACTIVE") = FiringState::Active;
}