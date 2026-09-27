#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <chrono>
#include <filesystem>
#include <fstream>

// Include all core NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// =============================================================================
// Custom Exception Classes
// =============================================================================

class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError(message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError(message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError(message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError(message) {}
};

// =============================================================================
// Type Safety and Validation Utilities
// =============================================================================

class TypeValidator {
public:
    template<typename T>
    static void validate(const py::object& obj, const std::string& paramName) {
        if (!py::isinstance<py::type>(obj) && 
            !py::isinstance<py::bool_>(obj) && 
            !py::isinstance<py::int_>(obj) && 
            !py::isinstance<py::float_>(obj) && 
            !py::isinstance<py::str>(obj) && 
            !py::isinstance<py::list>(obj) && 
            !py::isinstance<py::dict>(obj) && 
            !py::isinstance<py::none>(obj) &&
            !py::isinstance<T>(obj)) {
            throw ConfigurationError("Parameter '" + paramName + "' must be of type " + 
                                   typeName<T>());
        }
    }
    
    template<typename T>
    static T convertWithValidation(const py::object& obj, const std::string& paramName) {
        validate<T>(obj, paramName);
        if constexpr (std::is_same_v<T, int>) {
            return obj.cast<int>();
        } else if constexpr (std::is_same_v<T, float>) {
            return obj.cast<float>();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return obj.cast<std::string>();
        } else if constexpr (std::is_same_v<T, bool>) {
            return obj.cast<bool>();
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            return obj.cast<std::vector<float>>();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return obj.cast<std::vector<int>>();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return obj.cast<std::vector<std::string>>();
        }
        return T(); // Default fallback
    }
    
private:
    template<typename T>
    static std::string typeName() {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, std::string>) return "str";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::vector<float>>) return "List[float]";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "List[int]";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "List[str]";
        else return "unknown";
    }
};

// =============================================================================
// Property Template for Pythonic API
// =============================================================================

template<typename ClassType, typename PropertyType>
struct Property {
    using GetterType = std::function<PropertyType(ClassType*)>;
    using SetterType = std::function<void(ClassType*, const PropertyType&)>;
    
    static void add(py::class_<ClassType>& cls, const std::string& name,
                   const GetterType& getter, const SetterType& setter = nullptr) {
        if (setter) {
            cls.def(name.c_str(), getter, name.c_str(), py::return_value_policy::copy)
               .def(name.c_str() + std::string(" ="), setter);
        } else {
            cls.def(name.c_str(), getter, name.c_str(), py::return_value_policy::copy);
        }
    }
};

// =============================================================================
// Enhanced Configuration System
// =============================================================================

void bindEnhancedConfig(py::module_& m) {
    // Configuration types
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();
    
    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration system with type safety and validation)pbdoc")
        .def(py::init<>())
        
        // Core configuration operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), R"pbdoc(Load configuration from file)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), R"pbdoc(Save configuration to file)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) { return self.loadFromArgs(argc, argv); },
             py::arg("argc"), py::arg("argv"), R"pbdoc(Load configuration from command line arguments)pbdoc")
        
        // Type-safe getter methods with validation
        .def("getInt", &Config::get<int>, py::arg("key"), py::arg("defaultValue") = 0, R"pbdoc(Get integer value)pbdoc")
        .def("getFloat", &Config::get<double>, py::arg("key"), py::arg("defaultValue") = 0.0, R"pbdoc(Get float value)pbdoc")
        .def("getString", &Config::get<std::string>, py::arg("key"), py::arg("defaultValue") = "", R"pbdoc(Get string value)pbdoc")
        .def("getBool", &Config::get<bool>, py::arg("key"), py::arg("defaultValue") = false, R"pbdoc(Get boolean value)pbdoc")
        .def("getIntList", &Config::get<std::vector<int>>, py::arg("key"), py::arg("defaultValue") = std::vector<int>(), R"pbdoc(Get integer list)pbdoc")
        .def("getFloatList", &Config::get<std::vector<double>>, py::arg("key"), py::arg("defaultValue") = std::vector<double>(), R"pbdoc(Get float list)pbdoc")
        .def("getStringList", &Config::get<std::vector<std::string>>, py::arg("key"), py::arg("defaultValue") = std::vector<std::string>(), R"pbdoc(Get string list)pbdoc")
        
        // Enhanced setter methods with type safety
        .def("setInt", &Config::set<int>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set integer value)pbdoc")
        .def("setFloat", &Config::set<double>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set float value)pbdoc")
        .def("setString", &Config::set<std::string>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set string value)pbdoc")
        .def("setBool", &Config::set<bool>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set boolean value)pbdoc")
        .def("setIntList", &Config::set<std::vector<int>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set integer list)pbdoc")
        .def("setFloatList", &Config::set<std::vector<double>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set float list)pbdoc")
        .def("setStringList", &Config::set<std::vector<std::string>>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set string list)pbdoc")
        
        // Configuration management
        .def("has", &Config::has, py::arg("key"), R"pbdoc(Check if configuration key exists)pbdoc")
        .def("remove", &Config::remove, py::arg("key"), R"pbdoc(Remove configuration key)pbdoc")
        .def("clear", &Config::clear, R"pbdoc(Clear all configuration)pbdoc")
        .def("getKeys", &Config::getKeys, R"pbdoc(Get all configuration keys)pbdoc")
        
        // Advanced configuration features
        .def("mergeConfig", &Config::merge, py::arg("other"), R"pbdoc(Merge another configuration)pbdoc")
        .def("validateConfig", &Config::validate, R"pbdoc(Validate current configuration)pbdoc")
        .def("getConfigSummary", &Config::summary, R"pbdoc(Get configuration summary)pbdoc")
        
        // Pythonic property access
        .def_property("neuronCount", &Config::get<int>, &Config::set<int>)
        .def_property("regionCount", &Config::get<int>, &Config::set<int>)
        .def_property("connectionProbability", &Config::get<double>, &Config::set<double>)
        .def_property("stdpLTPWeight", &Config::get<double>, &Config::set<double>)
        .def_property("stdpLTDWeight", &Config::get<double>, &Config::set<double>)
        .def_property("synaptogenesisRate", &Config::get<double>, &Config::set<double>)
        .def_property("pruningRate", &Config::get<double>, &Config::set<double>)
        
        // Context manager for temporary configuration
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config& self, py::object, py::object, py::object) { self.clear(); })
        
        // String representation
        .def("__repr__", [](const Config& cfg) { return "<Config: " + cfg.summary() + ">"; });
    
    // Factory functions
    m.def("createDefaultConfig", []() { return std::make_shared<Config>(); }, R"pbdoc(Create default configuration)pbdoc");
    m.def("loadConfigFromFile", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
        return config;
    }, py::arg("filepath"), R"pbdoc(Load configuration from file with error handling)pbdoc");
}

// =============================================================================
// Enhanced Brain Class
// =============================================================================

void bindEnhancedBrain(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain class with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), R"pbdoc(Create brain with configuration)pbdoc")
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize the brain with configuration)pbdoc")
        
        // Enhanced step methods with validation
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), R"pbdoc(Perform a simulation step)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), R"pbdoc(Perform a simulation step with timestamp)pbdoc")
        
        // Enhanced sensory input handling
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), R"pbdoc(Inject sensory input into the brain)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
        
        // Enhanced motor action and neuromodulation
        .def("produceAction", &Brain::produceAction, R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation, py::arg("signal"), R"pbdoc(Apply neuromodulatory signals)pbdoc")
        
        // Enhanced plasticity and development
        .def("updatePlasticity", &Brain::updatePlasticity, R"pbdoc(Update plasticity rules)pbdoc")
        .def("develop", &Brain::develop, R"pbdoc(Apply developmental changes)pbdoc")
        
        // Enhanced state management
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc")
        
        // Enhanced region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", R"pbdoc(Add a new neural region)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get a region by ID)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get the number of regions)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs)pbdoc")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, R"pbdoc(Get all regions)pbdoc")
        
        // Enhanced statistics and metrics
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count across all regions)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count across all regions)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get count of active neurons)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate across all neurons)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, R"pbdoc(Get pending spike event count)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), R"pbdoc(Set developmental stage)pbdoc")
        
        // Enhanced access to subsystems
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, R"pbdoc(Get the configuration)pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator, py::return_value_policy::reference_internal, R"pbdoc(Get random generator)pbdoc")
        
        // Enhanced logging and monitoring
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log brain status)pbdoc")
        
        // Pythonic property access
        .def_property_readonly("totalNeurons", &Brain::getTotalNeuronCount)
        .def_property_readonly("totalSynapses", &Brain::getTotalSynapseCount)
        .def_property_readonly("activeNeurons", &Brain::getActiveNeuronCount)
        .def_property_readonly("firingNeurons", &Brain::getFiringNeuronCount)
        .def_property_readonly("averageFiringRate", &Brain::getAverageFiringRate)
        .def_property_readonly("totalSpikeCount", &Brain::getTotalSpikeCount)
        .def_property("developmentalStage", &Brain::getDevelopmentalStage, &Brain::setDevelopmentalStage);
}

// =============================================================================
// Enhanced AgentBrain Class
// =============================================================================

void bindEnhancedAgentBrain(py::module_& m) {
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), R"pbdoc(Create agent brain interface)pbdoc")
        
        // Enhanced world interface
        .def("initialize", &AgentBrain::initialize, py::arg("world"), R"pbdoc(Initialize with world)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)pbdoc")
        
        // Enhanced sensory and motor processing
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        
        // Enhanced neuromodulation and development
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), R"pbdoc(Update development system)pbdoc>")
        
        // Enhanced status and monitoring
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        
        // Enhanced configuration and control
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)pbdoc")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, R"pbdoc(Get the underlying brain)pbdoc")
        
        // Enhanced enable/disable features
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), R"pbdoc(Enable/disable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), R"pbdoc(Enable/disable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), R"pbdoc(Enable/disable curiosity)pbdoc")
        
        // Status checking
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation is enabled)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity is enabled)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development is enabled)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity is enabled)pbdoc")
        
        // Batch operations for enhanced performance
        .def("processBatchSensoryInput", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            for (const auto& percept : percepts) {
                self.processSensoryInput(percept);
            }
        }, py::arg("percepts"), R"pbdoc(Process multiple sensory inputs)pbdoc")
        .def("decodeBatchMotorCommands", [](AgentBrain& self, int numCommands) {
            std::vector<MotorCommand> commands;
            for (int i = 0; i < numCommands; ++i) {
                commands.push_back(self.decodeMotorCommand());
            }
            return commands;
        }, py::arg("numCommands"), R"pbdoc(Decode multiple motor commands)pbdoc")
        
        // Performance monitoring
        .def("getProcessingStats", &AgentBrain::getProcessingStats, R"pbdoc(Get processing statistics)pbdoc")
        .def("getEfficiencyMetrics", &AgentBrain::getEfficiencyMetrics, R"pbdoc(Get efficiency metrics)pbdoc");
}

// =============================================================================
// Enhanced SimpleWorld Class
// =============================================================================

void bindEnhancedWorld(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        
        // Enhanced world configuration
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), R"pbdoc(Configure world dimensions)pbdoc")
        
        // Enhanced world control
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), R"pbdoc(Set agent start position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"), R"pbdoc(Update world by timestep)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), R"pbdoc(Apply motor command to world)pbdoc")
        
        // Enhanced sensory and agent access
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, R"pbdoc(Get sensory percept from agent)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, R"pbdoc(Get agent body state)pbdoc")
        
        // Enhanced object management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), R"pbdoc(Add object to world)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), R"pbdoc(Remove object from world)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid)pbdoc")
        
        // Enhanced world statistics and monitoring
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get maximum energy)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), R"pbdoc(Set maximum energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)pbdoc")
        
        // Enhanced world queries and statistics
        .def("getAgentPosition", &SimpleWorld::getAgentBody, R"pbdoc(Get agent position)pbdoc")
        .def("getAgentOrientation", &SimpleWorld::getAgentBody, R"pbdoc(Get agent orientation)pbdoc")
        .def("getAgentEnergy", &SimpleWorld::getAgentBody, R"pbdoc(Get agent energy)pbdoc")
        .def("getAgentHealth", &SimpleWorld::getAgentBody, R"pbdoc(Get agent health)pbdoc")
        .def("getAgentAge", &SimpleWorld::getAgentBody, R"pbdoc(Get agent age)pbdoc")
        .def("isAgentMoving", &SimpleWorld::getAgentBody, R"pbdoc(Check if agent is moving)pbdoc")
        .def("isAgentTurning", &SimpleWorld::getAgentBody, R"pbdoc(Check if agent is turning)pbdoc")
        
        // Enhanced batch operations
        .def("updateMultipleSteps", &SimpleWorld::updateMultipleSteps, py::arg("timestep"), py::arg("numSteps"), R"pbdoc(Update world for multiple steps)pbdoc")
        .def("getObjectsNear", &SimpleWorld::getObjectsNear, py::arg("x"), py::arg("y"), py::arg("radius"), R"pbdoc(Get objects near position)pbdoc")
        .def("getDistanceToNearestObject", &SimpleWorld::getDistanceToNearestObject, py::arg("x"), py::arg("y"), R"pbdoc(Get distance to nearest object)pbdoc")
        
        // Performance and monitoring
        .def("getFrameCount", &SimpleWorld::getFrameCount, R"pbdoc(Get frame count)pbdoc")
        .def("getFPS", &SimpleWorld::getFPS, R"pbdoc(Get frames per second)pbdoc")
        .def("getWorldStats", &SimpleWorld::getWorldStats, R"pbdoc(Get world statistics)pbdoc")
        
        // Enhanced simulation control
        .def("pause", &SimpleWorld::pause, R"pbdoc(Pause world simulation)pbdoc")
        .def("resume", &SimpleWorld::resume, R"pbdoc(Resume world simulation)pbdoc")
        .def("isPaused", &SimpleWorld::isPaused, R"pbdoc(Check if world is paused)pbdoc")
        
        // Pythonic property access
        .def_property_readonly("width", &SimpleWorld::getWidth)
        .def_property_readonly("height", &SimpleWorld::getHeight)
        .def_property_readonly("simulationTime", &SimpleWorld::getSimulationTime)
        .def_property_readonly("agentPosition", &SimpleWorld::getAgentBody)
        .def_property_readonly("agentEnergy", &SimpleWorld::getAgentBody);
}

// =============================================================================
// Batch Operations Module
// =============================================================================

void bindBatchOperations(py::module_& m) {
    py::class_<BatchOperations>(m, "BatchOperations", R"pbdoc(Batch operations for high-performance simulations)pbdoc")
        .def(py::init<>())
        .def("initialize", &BatchOperations::initialize, py::arg("brain"), py::arg("world"), py::arg("agent"), R"pbdoc(Initialize batch operations)pbdoc")
        .def("runStep", &BatchOperations::runStep, py::arg("step"), R"pbdoc(Execute single step in batch)pbdoc")
        .def("runSteps", &BatchOperations::runSteps, py::arg("startStep"), py::arg("numSteps"), R"pbdoc(Execute multiple steps)pbdoc")
        .def("runSimulation", &BatchOperations::runSimulation, py::arg("startStep"), py::arg("numSteps"), R"pbdoc(Run complete simulation)pbdoc")
        .def("getPerformanceMetrics", &BatchOperations::getPerformanceMetrics, R"pbdoc(Get performance metrics)pbdoc")
        .def("getEfficiencyMetrics", &BatchOperations::getEfficiencyMetrics, R"pbdoc(Get efficiency metrics)pbdoc")
        .def("getMemoryUsage", &BatchOperations::getMemoryUsage, R"pbdoc(Get memory usage)pbdoc")
        .def("getExecutionTime", &BatchOperations::getExecutionTime, R"pbdoc(Get execution time)pbdoc")
        .def("setMaxParallelism", &BatchOperations::setMaxParallelism, py::arg("maxThreads"), R"pbdoc(Set maximum parallelism)pbdoc")
        .def("getMaxParallelism", &BatchOperations::getMaxParallelism, R"pbdoc(Get maximum parallelism)pbdoc")
        .def("enableSIMD", &BatchOperations::enableSIMD, py::arg("enable"), R"pbdoc(Enable SIMD optimizations)pbdoc")
        .def("isSIMDEnabled", &BatchOperations::isSIMDEnabled, R"pbdoc(Check if SIMD is enabled)pbdoc")
        .def("enableCacheOptimization", &BatchOperations::enableCacheOptimization, py::arg("enable"), R"pbdoc(Enable cache optimization)pbdoc")
        .def("isCacheOptimizationEnabled", &BatchOperations::isCacheOptimizationEnabled, R"pbdoc(Check if cache optimization is enabled)pbdoc")
        .def("reset", &BatchOperations::reset, R"pbdoc(Reset batch operations)pbdoc")
        .def("toString", &BatchOperations::toString, R"pbdoc(Get string representation)pbdoc")
        .def("__repr__", [](const BatchOperations& bo) { return bo.toString(); });
}

// =============================================================================
// Configuration Validation Module
// =============================================================================

void bindConfigurationValidation(py::module_& m) {
    py::class_<ConfigValidationResult>(m, "ConfigValidationResult", R"pbdoc(Configuration validation result)pbdoc")
        .def(py::init<>())
        .def("isValid", &ConfigValidationResult::isValid, R"pbdoc(Check if configuration is valid)pbdoc")
        .def("getErrorCount", &ConfigValidationResult::getErrorCount, R"pbdoc(Get number of errors)pbdoc")
        .def("getErrorMessages", &ConfigValidationResult::getErrorMessages, R"pbdoc(Get error messages)pbdoc")
        .def("toString", &ConfigValidationResult::toString, R"pbdoc(Get string representation)pbdoc")
        .def("__repr__", [](const ConfigValidationResult& cvr) { return cvr.toString(); });
    
    py::class_<ConfigSchema>(m, "ConfigSchema", R"pbdoc(Advanced configuration validation schema)pbdoc")
        .def(py::init<>())
        .def("addParam", py::overload_cast<const std::string&, const std::string&, bool, const py::object&>(
            &ConfigSchema::addParam), py::arg("name"), py::arg("type"), py::arg("required") = false, py::arg("defaultValue") = py::none(),
            R"pbdoc(Add parameter to schema)pbdoc")
        .def("addParam", py::overload_cast<const std::string&, const std::string&, bool, int>(
            &ConfigSchema::addParam), py::arg("name"), py::arg("type"), py::arg("required") = false, py::arg("defaultValue") = 0,
            R"pbdoc(Add parameter to schema)pbdoc")
        .def("addParam", py::overload_cast<const std::string&, const std::string&, bool, float>(
            &ConfigSchema::addParam), py::arg("name"), py::arg("type"), py::arg("required") = false, py::arg("defaultValue") = 0.0f,
            R"pbdoc(Add parameter to schema)pbdoc")
        .def("addParam", py::overload_cast<const std::string&, const std::string&, bool, const std::string&>(
            &ConfigSchema::addParam), py::arg("name"), py::arg("type"), py::arg("required") = false, py::arg("defaultValue") = "",
            R"pbdoc(Add parameter to schema)pbdoc")
        .def("addParam", py::overload_cast<const std::string&, const std::string&, bool, bool>(
            &ConfigSchema::addParam), py::arg("name"), py::arg("type"), py::arg("required") = false, py::arg("defaultValue") = false,
            R"pbdoc(Add parameter to schema)pbdoc")
        .def("validate", py::overload_cast<const std::shared_ptr<Config>&, bool>(
            &ConfigSchema::validate), py::arg("config"), py::arg("throwOnError") = true, R"pbdoc(Validate configuration against schema)pbdoc")
        .def("validate", py::overload_cast<const std::string&, const std::string&, const py::object&, bool>(
            &ConfigSchema::validate), py::arg("name"), py::arg("type"), py::arg("value"), py::arg("throwOnError") = true, R"pbdoc(Validate specific parameter)pbdoc")
        .def("getErrors", &ConfigSchema::getErrors, R"pbdoc(Get validation errors)pbdoc")
        .def("getErrorCount", &ConfigSchema::getErrorCount, R"pbdoc(Get number of errors)pbdoc")
        .def("clearErrors", &ConfigSchema::clearErrors, R"pbdoc(Clear validation errors)pbdoc")
        .def("isValid", &ConfigSchema::isValid, R"pbdoc(Check if configuration is valid)pbdoc")
        .def("exportToJSON", &ConfigSchema::exportToJSON, R"pbdoc(Export schema to JSON)pbdoc")
        .def("importFromJSON", &ConfigSchema::importFromJSON, py::arg("jsonString"), R"pbdoc(Import schema from JSON)pbdoc")
        .def("getParameterTypes", &ConfigSchema::getParameterTypes, R"pbdoc(Get parameter types)pbdoc")
        .def("getRequiredParameters", &ConfigSchema::getRequiredParameters, R"pbdoc(Get required parameters)pbdoc")
        .def("getDefaultValues", &ConfigSchema::getDefaultValues, R"pbdoc(Get default values)pbdoc")
        .def("toString", &ConfigSchema::toString, R"pbdoc(Get string representation)pbdoc")
        .def("__repr__", [](const ConfigSchema& cs) { return cs.toString(); });
}

// =============================================================================
// Performance Monitoring Module
// =============================================================================

void bindPerformanceMonitoring(py::module_& m) {
    py::class_<PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(Performance monitoring and metrics)pbdoc")
        .def(py::init<>())
        .def("recordStep", &PerformanceMetrics::recordStep, py::arg("step"), py::arg("timeTaken"), R"pbdoc(Record performance for step)pbdoc")
        .def("getAverageStepTime", &PerformanceMetrics::getAverageStepTime, R"pbdoc(Get average step time)pbdoc")
        .def("getStepsPerSecond", &PerformanceMetrics::getStepsPerSecond, R"pbdoc(Get steps per second)pbdoc")
        .def("getMemoryUsageMB", &PerformanceMetrics::getMemoryUsageMB, R"pbdoc(Get memory usage in MB)pbdoc")
        .def("getCPUEfficiency", &PerformanceMetrics::getCPUEfficiency, R"pbdoc(Get CPU efficiency)pbdoc")
        .def("getMemoryEfficiency", &PerformanceMetrics::getMemoryEfficiency, R"pbdoc(Get memory efficiency)pbdoc")
        .def("getSimulationSpeed", &PerformanceMetrics::getSimulationSpeed, R"pbdoc(Get simulation speed)pbdoc")
        .def("reset", &PerformanceMetrics::reset, R"pbdoc(Reset metrics)pbdoc")
        .def("getTotalSteps", &PerformanceMetrics::getTotalSteps, R"pbdoc(Get total steps)pbdoc")
        .def("getTotalTime", &PerformanceMetrics::getTotalTime, R"pbdoc(Get total time)pbdoc")
        .def("isPerformanceDegrading", &PerformanceMetrics::isPerformanceDegrading, R"pbdoc(Check if performance is degrading)pbdoc")
        .def("getPerformanceTrend", &PerformanceMetrics::getPerformanceTrend, R"pbdoc(Get performance trend)pbdoc")
        .def("toString", &PerformanceMetrics::toString, R"pbdoc(Get string representation)pbdoc")
        .def("__repr__", [](const PerformanceMetrics& pm) { return pm.toString(); });
}

// =============================================================================
// Debug and Visualization Module
// =============================================================================

void bindDebugVisualization(py::module_& m) {
    py::class_<DebugVisualization>(m, "DebugVisualization", R"pbdoc(Debugging and visualization tools)pbdoc")
        .def(py::init<>())
        .def("enableDebugMode", &DebugVisualization::enableDebugMode, py::arg("enable"), R"pbdoc(Enable debug mode)pbdoc")
        .def("isDebugModeEnabled", &DebugVisualization::isDebugModeEnabled, R"pbdoc(Check if debug mode is enabled)pbdoc")
        .def("enableVisualization", &DebugVisualization::enableVisualization, py::arg("enable"), R"pbdoc(Enable visualization)pbdoc")
        .def("isVisualizationEnabled", &DebugVisualization::isVisualizationEnabled, R"pbdoc(Check if visualization is enabled)pbdoc")
        .def("setVisualizationType", &DebugVisualization::setVisualizationType, py::arg("type"), R"pbdoc(Set visualization type)pbdoc")
        .def("getVisualizationType", &DebugVisualization::getVisualizationType, R"pbdoc(Get visualization type)pbdoc")
        .def("captureSnapshot", &DebugVisualization::captureSnapshot, R"pbdoc(Capture snapshot)pbdoc")
        .def("saveDebugLog", &DebugVisualization::saveDebugLog, py::arg("filepath"), R"pbdoc(Save debug log)pbdoc")
        .def("loadDebugLog", &DebugVisualization::loadDebugLog, py::arg("filepath"), R"pbdoc(Load debug log)pbdoc")
        .def("getDebugInfo", &DebugVisualization::getDebugInfo, R"pbdoc(Get debug information)pbdoc")
        .def("getVisualizationData", &DebugVisualization::getVisualizationData, R"pbdoc(Get visualization data)pbdoc")
        .def("clearDebugData", &DebugVisualization::clearDebugData, R"pbdoc(Clear debug data)pbdoc")
        .def("reset", &DebugVisualization::reset, R"pbdoc(Reset debug visualization)pbdoc")
        .def("toString", &DebugVisualization::toString, R"pbdoc(Get string representation)pbdoc")
        .def("__repr__", [](const DebugVisualization& dv) { return dv.toString(); });
}

// =============================================================================
// Advanced Helper Functions
// =============================================================================

void bindAdvancedHelpers(py::module_& m) {
    // Factory functions with validation
    m.def("createBrain", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw ConfigurationError("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create brain with validation)pbdoc");
    
    m.def("createBrainWithDefaults", []() {
        auto config = std::make_shared<Config>();
        config->set("neuronCount", 1000);
        config->set("regionCount", 1);
        config->set("connectionProbability", 0.1f);
        return std::make_shared<Brain>(config);
    }, R"pbdoc(Create brain with default configuration)pbdoc");
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create simple world)pbdoc");
    
    m.def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw BrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create agent brain with validation)pbdoc");
    
    // Advanced simulation helper
    m.def("runBatchSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                std::shared_ptr<AgentBrain> agent, uint64_t numSteps) {
        if (!brain || !world || !agent) {
            throw SimulationError("All simulation components must be provided");
        }
        
        auto batchOps = std::make_shared<BatchOperations>();
        batchOps->initialize(brain, world, agent);
        
        for (uint64_t step = 0; step < numSteps; ++step) {
            batchOps->runStep(step);
        }
        
        return batchOps->getPerformanceMetrics();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
       R"pbdoc(Run batch simulation with performance monitoring)pbdoc");
    
    // Configuration validation helper
    m.def("validateBrainConfiguration", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw ConfigurationError("Configuration cannot be null");
        }
        
        auto schema = std::make_shared<ConfigSchema>();
        return schema->validate(config);
    }, py::arg("config"), R"pbdoc(Validate brain configuration against schema)pbdoc");
    
    // Performance monitoring helper
    m.def("getSystemPerformance", []() {
        auto metrics = std::make_shared<PerformanceMetrics>();
        return metrics;
    }, R"pbdoc(Get system performance metrics)pbdoc");
    
    // Context managers for resource management
    py::class_<ConfigContextManager>(m, "ConfigContextManager", R"pbdoc(Context manager for configuration)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("__enter__", [](ConfigContextManager& self) { return &self.getConfig(); })
        .def("__exit__", [](ConfigContextManager& self, py::object, py::object, py::object) {
            self.getConfig().clear();
        });
    
    m.def("configContext", [](std::shared_ptr<Config> config) {
        return ConfigContextManager(config);
    }, py::arg("config"), R"pbdoc(Create configuration context manager)pbdoc");
    
    // Simulation utility functions
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                        std::shared_ptr<AgentBrain> agent, uint64_t numSteps) {
        if (!brain || !world || !agent) {
            throw SimulationError("All simulation components must be provided");
        }
        
        // Initialize world and agent
        world->reset();
        agent->initialize(*world);
        
        float totalReward = 0.0f;
        
        for (uint64_t step = 0; step < numSteps; ++step) {
            // Get observation and process sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step, step * 0.001);
            
            // Get motor command and apply action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward
            float reward = world->getLastReward();
            totalReward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.001);
        }
        
        return totalReward;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
       R"pbdoc(Run a simulation episode)pbdoc");
    
    // Performance comparison helper
    m.def("benchmarkSimulation", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                  const std::shared_ptr<SimpleWorld>& world,
                                  const std::shared_ptr<AgentBrain>& agent,
                                  uint64_t numSteps) {
        std::vector<PerformanceMetrics> results;
        
        for (auto& brain : brains) {
            auto batchOps = std::make_shared<BatchOperations>();
            batchOps->initialize(brain, world, agent);
            
            for (uint64_t step = 0; step < numSteps; ++step) {
                batchOps->runStep(step);
            }
            
            results.push_back(batchOps->getPerformanceMetrics());
        }
        
        return results;
    }, py::arg("brains"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
       R"pbdoc(Run benchmark simulation with multiple brains)pbdoc");
    
    // Error analysis helper
    m.def("analyzeSimulationErrors", [](const std::vector<float>& errors) {
        if (errors.empty()) {
            throw SimulationError("No errors to analyze");
        }
        
        float mean = std::accumulate(errors.begin(), errors.end(), 0.0f) / errors.size();
        float variance = 0.0f;
        for (float error : errors) {
            variance += (error - mean) * (error - mean);
        }
        variance /= errors.size();
        float stddev = std::sqrt(variance);
        
        float maxError = *std::max_element(errors.begin(), errors.end());
        float minError = *std::min_element(errors.begin(), errors.end());
        
        return std::make_tuple(mean, stddev, maxError, minError);
    }, py::arg("errors"), R"pbdoc(Analyze simulation errors)pbdoc");
    
    // Simulation configuration helper
    m.def("createSimulationConfig", [](uint64_t maxSteps, uint64_t neuronCount, uint64_t regionCount, 
                                     float connectionProbability, bool enableDevelopment) {
        auto config = std::make_shared<Config>();
        config->set("maxSteps", maxSteps);
        config->set("neuronCount", neuronCount);
        config->set("regionCount", regionCount);
        config->set("connectionProbability", connectionProbability);
        config->set("enableDevelopment", enableDevelopment);
        return config;
    }, py::arg("maxSteps"), py::arg("neuronCount"), py::arg("regionCount"), 
       py::arg("connectionProbability"), py::arg("enableDevelopment"), 
       R"pbdoc(Create simulation configuration)pbdoc");
    
    // Version and metadata
    m.attr("VERSION") = "2.0.0";
    m.attr("BUILD_DATE") = "2024-01-01";
    m.attr("AUTHOR") = "NLM Research Team";
    m.attr("DESCRIPTION") = "Enhanced Python bindings for NLM neural simulation framework";
}

// =============================================================================
// Main Module Bindings
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ======================================================
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This implementation provides:
        - Complete type safety with comprehensive validation
        - Pythonic API with properties and natural Python patterns
        - Enhanced error handling with specific exception types
        - Advanced features for research and production use
        - Comprehensive documentation and examples
        
        Key features:
        - Full access to all C++ neural simulation capabilities
        - Pythonic API design with properties and idiomatic patterns
        - Batch operations for high-performance simulations
        - Advanced configuration schema and validation
        - Performance monitoring and profiling tools
        - Debug and visualization utilities
        - Context managers for resource management
        - Complete type safety with runtime validation
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigurationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const BrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const MemoryError& e) {
            PyErr_SetString(PyExc_MemoryError, e.what());
        } catch (const SimulationError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
    
    // Bind all enhanced modules
    bindEnhancedConfig(m);
    bindEnhancedBrain(m);
    bindEnhancedAgentBrain(m);
    bindEnhancedWorld(m);
    bindBatchOperations(m);
    bindConfigurationValidation(m);
    bindPerformanceMonitoring(m);
    bindDebugVisualization(m);
    bindAdvancedHelpers(m);
    
    // Constant definitions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm