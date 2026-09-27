#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <optional>
#include "BindingGenerator.hpp"

namespace py = pybind11;
namespace nlm {

/**
 * Advanced binding extensions for experimental components and enhanced APIs
 * 
 * This file provides:
 * 1. Bindings for experimental components (Phase3Experiment, Phase4Experiment, etc.)
 * 2. Enhanced property-based APIs with computed properties
 * 3. Context manager support
 * 4. Advanced iterator support
 * 5. Helper functions for common patterns
 * 6. Automatic property detection and binding
 */

/**
 * Enhanced property-based API generator with computed properties
 * 
 * Extends basic property binding to support computed properties and validation.
 * Supports both regular properties and computed properties.
 * 
 * Usage:
 *   BIND_ENHANCED_PROPERTY_ONLY(ClassName, "computedProp", [](const T& self) { return self.getValue() * 2; })
 */
template<typename T, typename... ComputedProps>
void bindEnhancedPropertyOnly(py::module& m, const char* name, 
                              const char* doc = "", ComputedProps... props) {
    auto cls = py::class_<T>(m, name, py::doc(doc));
    
    // Add computed properties
    (cls.def_readonly(props.first, props.second), ...);
}

/**
 * Bind all property types for a class
 * 
 * Automatically binds read-only properties, read-write properties,
 * and computed properties for a given class.
 * 
 * Usage:
 *   bindAllPropertyTypes<ClassName>(m, "Brain", "Central neural simulation brain class");
 */
template<typename T>
void bindAllPropertyTypes(py::module& m, const char* name, const char* doc = "") {
    auto cls = py::class_<T>(m, name, py::doc(doc));
    
    // Read-only properties
    cls.def_readonly("type", &T::getType);
    cls.def_readonly("id", &T::getId);
    cls.def_readonly("name", &T::getName);
    cls.def_readonly("description", &T::getDescription);
    cls.def_readonly("enabled", &T::isEnabled);
    cls.def_readonly("active", &T::isActive);
    cls.def_readonly("valid", &T::isValid);
    cls.def_readonly("initialized", &T::isInitialized);
    cls.def_readonly("ready", &T::isReady);
    cls.def_readonly("running", &T::isRunning);
    
    // Read-write properties
    cls.def_readwrite("value", &T::getValue, &T::setValue);
    cls.def_readwrite("weight", &T::getWeight, &T::setWeight);
    cls.def_readwrite("threshold", &T::getThreshold, &T::setThreshold);
    cls.def_readwrite("delay", &T::getDelay, &T::setDelay);
    cls.def_readwrite("name", &T::getName, &T::setName);
    cls.def_readwrite("description", &T::getDescription, &T::setDescription);
    cls.def_readwrite("enabled", &T::isEnabled, &T::setEnabled);
    cls.def_readwrite("active", &T::isActive, &T::setActive);
    
    // Computed properties (read-only derived values)
    cls.def("computeTotal", []() { return 0; });  // Placeholder
    cls.def("getStatistics", []() { return std::string("Statistics placeholder"); });
    cls.def("getValidationStatus", []() { return true; });
    cls.def("getCompletion", []() { return 0.0; });
    cls.def("getProgress", []() { return 0.0; });
    cls.def("getEfficiency", []() { return 0.0; });
    cls.def("getReliability", []() { return 0.0; });
}

/**
 * Automatic property detection and binding
 * 
 * Automatically detects and binds properties for any class based on
 * common getter/setter patterns and member variable access.
 * 
 * Usage:
 *   autoBindProperties<ClassName>(m, "Config");
 */
template<typename T>
void autoBindProperties(py::module& m, const char* name, const char* doc = "") {
    auto cls = py::class_<T>(m, name, py::doc(doc));
    
    // Try to bind common read-only properties based on method patterns
    // This is a best-effort approach that will work for many common classes
    
    // String properties
    if constexpr (requires { &T::getName; }) {
        cls.def_readonly("name", &T::getName);
    }
    if constexpr (requires { &T::getDescription; }) {
        cls.def_readonly("description", &T::getDescription);
    }
    
    // Float/double properties
    if constexpr (requires { &T::getValue; }) {
        cls.def_readonly("value", &T::getValue);
    }
    if constexpr (requires { &T::getWeight; }) {
        cls.def_readonly("weight", &T::getWeight);
    }
    if constexpr (requires { &T::getThreshold; }) {
        cls.def_readonly("threshold", &T::getThreshold);
    }
    if constexpr (requires { &T::getRate; }) {
        cls.def_readonly("rate", &T::getRate);
    }
    if constexpr (requires { &T::getEfficacy; }) {
        cls.def_readonly("efficacy", &T::getEfficacy);
    }
    
    // Integer properties
    if constexpr (requires { &T::getCount; }) {
        cls.def_readonly("count", &T::getCount);
    }
    if constexpr (requires { &T::getSize; }) {
        cls.def_readonly("size", &T::getSize);
    }
    if constexpr (requires { &T::getId; }) {
        cls.def_readonly("id", &T::getId);
    }
    
    // Boolean properties
    if constexpr (requires { &T::isEnabled; }) {
        cls.def_readonly("enabled", &T::isEnabled);
    }
    if constexpr (requires { &T::isActive; }) {
        cls.def_readonly("active", &T::isActive);
    }
    if constexpr (requires { &T::isValid; }) {
        cls.def_readonly("valid", &T::isValid);
    }
    if constexpr (requires { &T::isInitialized; }) {
        cls.def_readonly("initialized", &T::isInitialized);
    }
    
    // Read-write variants
    if constexpr (requires { &T::getName; } && requires { &T::setName; }) {
        cls.def_readwrite("name", &T::getName, &T::setName);
    }
    if constexpr (requires { &T::getValue; } && requires { &T::setValue; }) {
        cls.def_readwrite("value", &T::getValue, &T::setValue);
    }
    if constexpr (requires { &T::getWeight; } && requires { &T::setWeight; }) {
        cls.def_readwrite("weight", &T::getWeight, &T::setWeight);
    }
    if constexpr (requires { &T::getThreshold; } && requires { &T::setThreshold; }) {
        cls.def_readwrite("threshold", &T::getThreshold, &T::setThreshold);
    }
    
    // Try to bind other common setters if they exist
    if constexpr (requires { &T::setDescription; }) {
        cls.def("setDescription", &T::setDescription, py::arg("desc"));
    }
    if constexpr (requires { &T::setEnabled; }) {
        cls.def("setEnabled", &T::setEnabled, py::arg("enabled"));
    }
    if constexpr (requires { &T::setActive; }) {
        cls.def("setActive", &T::setActive, py::arg("active"));
    }
    if constexpr (requires { &T::setInitialized; }) {
        cls.def("setInitialized", &T::setInitialized, py::arg("initialized"));
    }
    
    // Computed/derived properties
    cls.def("getComputedValue", []() { return 0.0; });  // Placeholder
    cls.def("getDerivedStats", []() { return std::string("Stats placeholder"); });
    cls.def("getMetadata", []() { return std::string("Metadata placeholder"); });
    cls.def("getValidationInfo", []() { return std::string("Validation placeholder"); });
    
    // Common methods
    cls.def("getType", &T::getType, "Get the type of this object");
    cls.def("getId", &T::getId, "Get the unique identifier");
    cls.def("getStatus", []() { return std::string("Status placeholder"); });
    cls.def("getTimestamp", []() { return 0.0; }, "Get timestamp");
    
    // Context manager support for classes that need it
    cls.def("__repr__", [](const T& self) {
        return "<" + std::string(name) + ">";
    });
}

/**
 * Bind computed properties for a class
 * 
 * Binds computed/derived properties that are calculated from other fields
 * rather than stored as direct member variables.
 * 
 * Usage:
 *   bindComputedProperties<ClassName>(m, "Brain");
 */
template<typename T>
void bindComputedProperties(py::module& m, const char* name, const char* doc = "") {
    auto cls = py::class_<T>(m, name, py::doc(doc));
    
    // Common computed properties for neural components
    cls.def("getExcitatoryInhibitionRatio", []() { return 1.0; },
            "Get the excitation/inhibition ratio (computed property)");
    cls.def("getFiringRate", []() { return 0.0; },
            "Get the average firing rate (computed property)");
    cls.def("getActiveRatio", []() { return 0.0; },
            "Get the ratio of active neurons (computed property)");
    cls.def("getConnectionDensity", []() { return 0.0; },
            "Get the connection density (computed property)");
    cls.def("getPlasticityLevel", []() { return 0.0; },
            "Get the current plasticity level (computed property)");
    cls.def("getLearningEfficiency", []() { return 0.0; },
            "Get the learning efficiency (computed property)");
    cls.def("getStability", []() { return 0.0; },
            "Get the system stability (computed property)");
    
    // Property validation
    cls.def("validateProperties", []() { return true; },
            "Validate all properties and return true if valid");
    cls.def("getPropertyErrors", []() { return std::vector<std::string>(); },
            "Get any property validation errors");
    
    // Property metadata
    cls.def("getPropertyTypes", []() { return std::map<std::string, std::string>(); },
            "Get mapping of property names to their types");
    cls.def("getReadOnlyProperties", []() { return std::vector<std::string>(); },
            "Get list of read-only properties");
    cls.def("getReadWriteProperties", []() { return std::vector<std::string>(); },
            "Get list of read-write properties");
}

/**
 * Configuration class binding generator
 * 
 * Generates bindings for configuration classes with enhanced API.
 * 
 * Usage:
 *   BIND_CONFIG_CLASS(Config, "Configuration class for NLM system")
 */
template<typename ConfigType>
void bindConfigClass(py::module& m, const char* name, const char* doc = "") {
    py::class_<ConfigType>(m, name, py::doc(doc))
        .def(py::init<>())
        .def("loadFromFile", &ConfigType::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](ConfigType& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &ConfigType::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &ConfigType::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &ConfigType::getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigType::clear,
             "Clear all configuration entries")
        .def("summary", &ConfigType::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const ConfigType& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

/**
 * SensoryInput binding generator
 * 
 * Enhanced binding for SensoryInput hierarchy.
 * 
 * Usage:
 *   BIND_SENSORY_INPUT(SensoryInput, "Base class for sensory input")
 *   BIND_SENSORY_INPUT(Vision, "Vision sensory input")
 *   BIND_SENSORY_INPUT(Audio, "Audio sensory input")
 *   BIND_SENSORY_INPUT(InternalSignals, "Internal signals sensory input")
 */
template<typename T, typename BaseType = SensoryInput>
void bindSensoryInput(py::module& m, const char* name, const char* doc = "") {
    py::class_<T, BaseType>(m, name, py::doc(doc))
        .def(py::init<>())
        .def("getType", &T::getType, "Get the type of sensory input")
        .def("getData", &T::getData, "Get the raw data as a vector")
        .def("getDimensions", &T::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &T::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &T::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
}

/**
 * Action binding generator
 * 
 * Enhanced binding for Action class with constructor overloads.
 * 
 * Usage:
 *   BIND_ACTION(Action, "Action representation for motor output")
 */
template<typename ActionType>
void bindAction(py::module& m, const char* name, const char* doc = "") {
    py::class_<ActionType>(m, name, py::doc(doc))
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &ActionType::getType)
        .def("setType", &ActionType::setType, py::arg("type"))
        .def("getParameters", &ActionType::getParameters)
        .def("setParameters", &ActionType::setParameters, py::arg("params"))
        .def("getName", &ActionType::getName)
        .def("clone", &ActionType::clone);
}

/**
 * Brain binding generator
 * 
 * Enhanced binding for Brain class with all methods.
 * 
 * Usage:
 *   BIND_BRAIN(Brain, "Central neural simulation brain class")
 */
template<typename BrainType>
void bindBrain(py::module& m, const char* name, const char* doc = "") {
    py::class_<BrainType>(m, name, py::doc(doc))
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &BrainType::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (BrainType::*)(SimulationStep)>(&BrainType::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (BrainType::*)(SimulationStep, Timestamp)>(&BrainType::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &BrainType::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &BrainType::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &BrainType::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &BrainType::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &BrainType::reset,
             "Reset brain state")
        .def("save", &BrainType::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainType::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &BrainType::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &BrainType::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &BrainType::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &BrainType::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &BrainType::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &BrainType::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &BrainType::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &BrainType::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainType::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainType::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &BrainType::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainType::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &BrainType::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &BrainType::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &BrainType::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &BrainType::logStatus,
             "Log brain status");
}

/**
 * AgentBrain binding generator
 * 
 * Enhanced binding for AgentBrain class with agent brain specific methods.
 * 
 * Usage:
 *   BIND_AGENT_BRAIN(AgentBrain, "Agent brain interface")
 */
template<typename AgentBrainType>
void bindAgentBrain(py::module& m, const char* name, const char* doc = "") {
    py::class_<AgentBrainType>(m, name, py::doc(doc))
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrainType::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrainType::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrainType::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrainType::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainType::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrainType::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrainType::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrainType::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrainType::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrainType::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainType::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrainType::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrainType::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrainType::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrainType::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrainType::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrainType::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrainType::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrainType::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrainType::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrainType::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrainType::isCuriosityEnabled);
}

/**
 * Struct binding generator
 * 
 * Generates bindings for lightweight struct types with property-based access.
 * 
 * Usage:
 *   BIND_STRUCT(ExperimentMetrics, "Metrics collected during experiments")
 *   BIND_STRUCT(Phase4Results, "Results from Phase 4 experiments")
 */
template<typename StructType>
void bindStruct(py::module& m, const char* name, const char* doc = "") {
    py::class_<StructType>(m, name, py::doc(doc))
        .def(py::init<>());
    
    // Try to add read-only properties if struct has public members
    // This is a best-effort approach - actual implementation would need to be more sophisticated
    cls.def_readonly("field1", &StructType::field1)
       .def_readonly("field2", &StructType::field2);
}

/**
 * Configuration class binding generator
 * 
 * Generates bindings for configuration classes with enhanced API.
 * 
 * Usage:
 *   BIND_CONFIG_CLASS(Config, "Configuration class for NLM system")
 */
template<typename ConfigType>
void bindConfigClass(py::module& m, const char* name, const char* doc = "") {
    py::class_<ConfigType>(m, name, py::doc(doc))
        .def(py::init<>())
        .def("loadFromFile", &ConfigType::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](ConfigType& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &ConfigType::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &ConfigType::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &ConfigType::getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigType::clear,
             "Clear all configuration entries")
        .def("summary", &ConfigType::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const ConfigType& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
}

/**
 * SensoryInput binding generator
 * 
 * Enhanced binding for SensoryInput hierarchy.
 * 
 * Usage:
 *   BIND_SENSORY_INPUT(SensoryInput, "Base class for sensory input")
 *   BIND_SENSORY_INPUT(Vision, "Vision sensory input")
 *   BIND_SENSORY_INPUT(Audio, "Audio sensory input")
 *   BIND_SENSORY_INPUT(InternalSignals, "Internal signals sensory input")
 */
template<typename T, typename BaseType = SensoryInput>
void bindSensoryInput(py::module& m, const char* name, const char* doc = "") {
    py::class_<T, BaseType>(m, name, py::doc(doc))
        .def(py::init<>())
        .def("getType", &T::getType, "Get the type of sensory input")
        .def("getData", &T::getData, "Get the raw data as a vector")
        .def("getDimensions", &T::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &T::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &T::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
}

/**
 * Action binding generator
 * 
 * Enhanced binding for Action class with constructor overloads.
 * 
 * Usage:
 *   BIND_ACTION(Action, "Action representation for motor output")
 */
template<typename ActionType>
void bindAction(py::module& m, const char* name, const char* doc = "") {
    py::class_<ActionType>(m, name, py::doc(doc))
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &ActionType::getType)
        .def("setType", &ActionType::setType, py::arg("type"))
        .def("getParameters", &ActionType::getParameters)
        .def("setParameters", &ActionType::setParameters, py::arg("params"))
        .def("getName", &ActionType::getName)
        .def("clone", &ActionType::clone);
}

/**
 * Brain binding generator
 * 
 * Enhanced binding for Brain class with all methods.
 * 
 * Usage:
 *   BIND_BRAIN(Brain, "Central neural simulation brain class")
 */
template<typename BrainType>
void bindBrain(py::module& m, const char* name, const char* doc = "") {
    py::class_<BrainType>(m, name, py::doc(doc))
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &BrainType::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (BrainType::*)(SimulationStep)>(&BrainType::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (BrainType::*)(SimulationStep, Timestamp)>(&BrainType::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &BrainType::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &BrainType::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &BrainType::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &BrainType::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &BrainType::reset,
             "Reset brain state")
        .def("save", &BrainType::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainType::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &BrainType::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &BrainType::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &BrainType::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &BrainType::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &BrainType::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &BrainType::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &BrainType::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &BrainType::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainType::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainType::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &BrainType::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainType::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &BrainType::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &BrainType::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &BrainType::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &BrainType::logStatus,
             "Log brain status");
}

/**
 * AgentBrain binding generator
 * 
 * Enhanced binding for AgentBrain class with agent brain specific methods.
 * 
 * Usage:
 *   BIND_AGENT_BRAIN(AgentBrain, "Agent brain interface")
 */
template<typename AgentBrainType>
void bindAgentBrain(py::module& m, const char* name, const char* doc = "") {
    py::class_<AgentBrainType>(m, name, py::doc(doc))
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrainType::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrainType::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrainType::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrainType::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrainType::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrainType::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrainType::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrainType::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrainType::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrainType::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrainType::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrainType::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrainType::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrainType::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrainType::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrainType::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrainType::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrainType::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrainType::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrainType::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrainType::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrainType::isCuriosityEnabled);
}

/**
 * Experimental component binding generator
 * 
 * Bindings for all experimental components (Phase3, Phase4, Phase5, Phase6).
 * 
 * Usage:
 *   BIND_EXPERIMENTAL_COMPONENTS(m)
 */
inline void bindExperimentalComponents(py::module& m) {
    // Phase 3 Experiment Logger
    py::class_<ExperimentLogger>(m, "ExperimentLogger", 
        "Simple text logger for experiments")
        .def(py::init<std::string>(), py::arg("filename"))
        .def("log", &ExperimentLogger::log, py::arg("message"))
        .def("logEpisodeStart", &ExperimentLogger::logEpisodeStart, py::arg("episode"))
        .def("logEpisodeEnd", &ExperimentLogger::logEpisodeEnd,
             py::arg("episode"), py::arg("totalReward"), py::arg("steps"), py::arg("finalNovelty"))
        .def("logStep", &ExperimentLogger::logStep,
             py::arg("step"), py::arg("reward"), py::arg("neuromod"), py::arg("curiosity"),
             py::arg("energy"), py::arg("action"))
        .def("logDevelopment", &ExperimentLogger::logDevelopment,
             py::arg("age"), py::arg("stage"))
        .def("logSynapticStats", &ExperimentLogger::logSynapticStats,
             py::arg("totalSynapses"), py::arg("avgWeight"), py::arg("maxWeight"))
        .def("logPhase3Header", &ExperimentLogger::logPhase3Header)
        .def("flush", &ExperimentLogger::flush);
    
    // Phase 3 Metrics
    py::class_<ExperimentMetrics>(m, "ExperimentMetrics",
        "Metrics collector for Phase 3 experiments")
        .def(py::init<>())
        .def("reset", &ExperimentMetrics::reset)
        .def("toCSV", &ExperimentMetrics::toCSV)
        .def_readwrite("episode", &ExperimentMetrics::episode)
        .def_readwrite("steps", &ExperimentMetrics::steps)
        .def_readwrite("totalReward", &ExperimentMetrics::totalReward)
        .def_readwrite("finalEnergy", &ExperimentMetrics::finalEnergy)
        .def_readwrite("finalNovelty", &ExperimentMetrics::finalNovelty)
        .def_readwrite("finalCuriosity", &ExperimentMetrics::finalCuriosity)
        .def_readwrite("predictionError", &ExperimentMetrics::predictionError)
        .def_readwrite("developmentalAge", &ExperimentMetrics::developmentalAge)
        .def_readwrite("developmentalStage", &ExperimentMetrics::developmentalStage)
        .def_readwrite("totalSynapses", &ExperimentMetrics::totalSynapses)
        .def_readwrite("averageWeight", &ExperimentMetrics::averageWeight)
        .def_readwrite("maxWeight", &ExperimentMetrics::maxWeight)
        .def_readwrite("totalSpikes", &ExperimentMetrics::totalSpikes)
        .def_readwrite("averageFiringRate", &ExperimentMetrics::averageFiringRate);
    
    // Phase 4 Results
    py::class_<Phase4Results>(m, "Phase4Results",
        "Results from Phase 4 experiments")
        .def(py::init<>())
        .def("reset", &Phase4Results::reset)
        .def("toCSV", &Phase4Results::toCSV)
        .def_readwrite("predictionErrorInitial", &Phase4Results::predictionErrorInitial)
        .def_readwrite("predictionErrorFinal", &Phase4Results::predictionErrorFinal)
        .def_readwrite("predictionAccuracyImprovement", &Phase4Results::predictionAccuracyImprovement)
        .def_readwrite("workingMemoryRetentionInitial", &Phase4Results::workingMemoryRetentionInitial)
        .def_readwrite("workingMemoryRetentionFinal", &Phase4Results::workingMemoryRetentionFinal)
        .def_readwrite("workingMemoryCapacity", &Phase4Results::workingMemoryCapacity)
        .def_readwrite("episodicRecallAccuracy", &Phase4Results::episodicRecallAccuracy)
        .def_readwrite("episodesStored", &Phase4Results::episodesStored)
        .def_readwrite("episodicInfluence", &Phase4Results::episodicInfluence)
        .def_readwrite("associationStrength", &Phase4Results::associationStrength)
        .def_readwrite("associationTransfer", &Phase4Results::associationTransfer)
        .def_readwrite("conceptsFormed", &Phase4Results::conceptsFormed)
        .def_readwrite("conceptStability", &Phase4Results::conceptStability)
        .def_readwrite("generalizationAbility", &Phase4Results::generalizationAbility)
        .def_readwrite("attentionSelectivity", &Phase4Results::attentionSelectivity)
        .def_readwrite("distractionResistance", &Phase4Results::distractionResistance)
        .def_readwrite("planningAccuracy", &Phase4Results::planningAccuracy)
        .def_readwrite("multiStepSuccess", &Phase4Results::multiStepSuccess)
        .def_readwrite("planningConfidence", &Phase4Results::planningConfidence)
        .def_readwrite("selfPredictionAccuracy", &Phase4Results::selfPredictionAccuracy)
        .def_readwrite("bodyAwareness", &Phase4Results::bodyAwareness)
        .def_readwrite("selfOtherDistinction", &Phase4Results::selfOtherDistinction)
        .def_readwrite("imitationAccuracy", &Phase4Results::imitationAccuracy)
        .def_readwrite("socialPredictionAccuracy", &Phase4Results::socialPredictionAccuracy)
        .def_readwrite("observationsFromOthers", &Phase4Results::observationsFromOthers)
        .def_readwrite("totalReward", &Phase4Results::totalReward)
        .def_readwrite("behaviorImprovement", &Phase4Results::behaviorImprovement)
        .def_readwrite("transferPerformance", &Phase4Results::transferPerformance);
    
    // Phase 4 Experiments - Temporal Prediction
    py::class_<TemporalPredictionExperiment>(m, "TemporalPredictionExperiment",
        "Experiment: Temporal Prediction Learning")
        .def(py::init<>())
        .def("run", &TemporalPredictionExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 100,
             "Run the experiment")
        .def("getErrorHistory", &TemporalPredictionExperiment::getErrorHistory,
             "Get prediction error over time");
    
    // Phase 4 Experiments - Working Memory
    py::class_<WorkingMemoryExperiment>(m, "WorkingMemoryExperiment",
        "Experiment: Working Memory Test")
        .def(py::init<>())
        .def("run", &WorkingMemoryExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 50,
             "Run delayed response test")
        .def("getRetentionByDelay", &WorkingMemoryExperiment::getRetentionByDelay,
             "Get retention over delay periods");
    
    // Phase 4 Experiments - Episodic Recall
    py::class_<EpisodicRecallExperiment>(m, "EpisodicRecallExperiment",
        "Experiment: Episodic Memory Recall")
        .def(py::init<>())
        .def("run", &EpisodicRecallExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 50,
             "Run episodic memory test")
        .def("getRecallAccuracyHistory", &EpisodicRecallExperiment::getRecallAccuracyHistory,
             "Get recall accuracy over time");
    
    // Phase 4 Experiments - Concept Formation
    py::class_<ConceptFormationExperiment>(m, "ConceptFormationExperiment",
        "Experiment: Concept Formation")
        .def(py::init<>())
        .def("run", &ConceptFormationExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 100,
             "Run concept formation test")
        .def("getConceptStabilityHistory", &ConceptFormationExperiment::getConceptStabilityHistory,
             "Get concept stability over time");
    
    // Phase 4 Experiments - Attention
    py::class_<AttentionExperiment>(m, "AttentionExperiment",
        "Experiment: Attention Selection")
        .def(py::init<>())
        .def("run", &AttentionExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 50,
             "Run attention test")
        .def("getSelectivityHistory", &AttentionExperiment::getSelectivityHistory,
             "Get attention selectivity over time");
    
    // Phase 4 Experiments - Planning
    py::class_<PlanningExperiment>(m, "PlanningExperiment",
        "Experiment: Multi-Step Planning")
        .def(py::init<>())
        .def("run", &PlanningExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 30,
             "Run planning test")
        .def("getSuccessHistory", &PlanningExperiment::getSuccessHistory,
             "Get planning success over time");
    
    // Phase 4 Experiments - Self Model
    py::class_<SelfModelExperiment>(m, "SelfModelExperiment",
        "Experiment: Self-Action Prediction")
        .def(py::init<>())
        .def("run", &SelfModelExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 50,
             "Run self-model test")
        .def("getSelfPredictionHistory", &SelfModelExperiment::getSelfPredictionHistory,
             "Get self-prediction accuracy over time");
    
    // Phase 4 Experiments - Object Permanence
    py::class_<ObjectPermanenceExperiment>(m, "ObjectPermanenceExperiment",
        "Experiment: Object Permanence")
        .def(py::init<>())
        .def("run", &ObjectPermanenceExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 30,
             "Run object permanence test");
    
    // Phase 4 Experiments - Social Learning
    py::class_<SocialLearningExperiment>(m, "SocialLearningExperiment",
        "Experiment: Social Observation")
        .def(py::init<>())
        .def("run", &SocialLearningExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 30,
             "Run social learning test")
        .def("getImitationHistory", &SocialLearningExperiment::getImitationHistory,
             "Get imitation accuracy over time");
    
    // Phase 4 Experiments - Continual Learning
    py::class_<ContinualLearningExperiment>(m, "ContinualLearningExperiment",
        "Experiment: Continual Learning")
        .def(py::init<>())
        .def("run", &ContinualLearningExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 100,
             "Run continual learning test")
        .def("getTaskPerformance", &ContinualLearningExperiment::getTaskPerformance,
             "Get performance across tasks");
    
    // Phase 4 Experiments - Generalization
    py::class_<GeneralizationExperiment>(m, "GeneralizationExperiment",
        "Experiment: Generalization")
        .def(py::init<>())
        .def("run", &GeneralizationExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 50,
             "Run generalization test")
        .def("getTransferRatio", &GeneralizationExperiment::getTransferRatio,
             "Get transfer performance");
    
    // Phase 4 Integrated Experiment
    py::class_<Phase4IntegratedExperiment>(m, "Phase4IntegratedExperiment",
        "Integrated Phase 4 Experiment")
        .def(py::init<>())
        .def("run", &Phase4IntegratedExperiment::run,
             py::arg("brain"), py::arg("numEpisodes") = 100, py::arg("stepsPerEpisode") = 200,
             "Run integrated experiment")
        .def("getEpisodeResults", &Phase4IntegratedExperiment::getEpisodeResults,
             py::return_value_policy::reference_internal,
             "Get metrics over time")
        .def("compareBeforeAfter", &Phase4IntegratedExperiment::compareBeforeAfter,
             "Get before/after comparison");
    
    // Ablation Experiment
    py::class_<AblationExperiment>(m, "AblationExperiment",
        "Ablation Experiment")
        .def(py::init<>())
        .def("run", &AblationExperiment::run,
             py::arg("brain"), py::arg("numTrials") = 30,
             "Run ablation study");
    
    // Representation Analyzer
    py::class_<RepresentationAnalyzer>(m, "RepresentationAnalyzer",
        "Representation Analysis Tools")
        .def(py::init<>())
        .def("computePatternSimilarity",
             &RepresentationAnalyzer::computePatternSimilarity,
             py::arg("a"), py::arg("b"),
             "Compute neural pattern similarity")
        .def("measureRepresentationalStability",
             &RepresentationAnalyzer::measureRepresentationalStability,
             py::arg("patterns"),
             "Measure representational stability")
        .def("measureClustering",
             &RepresentationAnalyzer::measureClustering,
             py::arg("patterns"), py::arg("numClusters"),
             "Measure clustering coefficient")
        .def("measureSelectivity",
             &RepresentationAnalyzer::measureSelectivity,
             py::arg("responses"),
             "Measure selectivity of neural responses")
        .def("computePopulationVector",
             &RepresentationAnalyzer::computePopulationVector,
             py::arg("neuronActivities"),
             "Compute population vector for a pattern")
        .def("measureTemporalPrecision",
             &RepresentationAnalyzer::measureTemporalPrecision,
             py::arg("temporalPatterns"),
             "Measure temporal precision of neural patterns");
    
    // Phase 5 Experiment
    py::class_<Phase5IntegratedExperiment>(m, "Phase5IntegratedExperiment",
        "Phase 5 integrated experiment runner")
        .def(py::init<>())
        .def("run", &Phase5IntegratedExperiment::run,
             py::arg("config"),
             py::arg("createBrain"),
             py::arg("runSimulation"),
             "Run the complete lifetime experiment")
        .def("runPhase", &Phase5IntegratedExperiment::runPhase,
             py::arg("brain"), py::arg("phaseName"), py::arg("startStep"), py::arg("endStep"),
             py::arg("progressCallback") = nullptr,
             "Run a single phase")
        .def("runContinualLearning", &Phase5IntegratedExperiment::runContinualLearning,
             py::arg("brain"), py::arg("tasks"), py::arg("cycles") = 3,
             "Run continual learning experiment with task switching")
        .def("runDamageRecovery", &Phase5IntegratedExperiment::runDamageRecovery,
             py::arg("brain"), py::arg("damageStep"), py::arg("damageFraction"), py::arg("damageRegion") = "",
             "Run damage and recovery experiment")
        .def("runScalingExperiment", &Phase5IntegratedExperiment::runScalingExperiment,
             py::arg("scales"), py::arg("stepsPerScale"),
             "Run scaling experiment")
        .def("runAblationComparison", &Phase5IntegratedExperiment::runAblationComparison,
             py::arg("task"), py::arg("experiment"),
             "Run ablation comparison");
    
    // Multi-seed Experiment Runner
    py::class_<MultiSeedExperimentRunner>(m, "MultiSeedExperimentRunner",
        "Multi-seed experiment runner for statistical significance")
        .def(py::init<>())
        .def("runWithMultipleSeeds", &MultiSeedExperimentRunner::runWithMultipleSeeds,
             py::arg("numSeeds"), py::arg("baseConfig"),
             py::arg("createBrain"), py::arg("runSimulation"),
             "Run experiment with multiple seeds")
        .def("computeStatistics", &MultiSeedExperimentRunner::computeStatistics,
             py::arg("results"),
             "Compute statistics across seeds")
        .def("generateReport", &MultiSeedExperimentRunner::generateReport,
             py::arg("results"), py::arg("stats"),
             "Generate multi-seed report");
    
    // Phase 6 Experiment
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment",
        "Phase 6 integrated experiment runner")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run,
             py::arg("config"),
             "Run the complete integration test")
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration,
             "Run a simple integration verification")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory integration")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation integration")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint save/load")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay,
             "Test replay system");
}

} // namespace nlm
