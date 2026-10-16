// BindingGenerator.cpp - Implementation of the NLM Python binding code generator

#include "BindingGenerator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <regex>
#include <ctime>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace fs = std::filesystem;

BindingGenerator::BindingGenerator(const BindingConfig& config) : config_(config) {
    stats_.classesProcessed = 0;
    stats_.methodsProcessed = 0;
    stats_.linesGenerated = 0;
    stats_.filesGenerated = 0;
    stats_.generationTime = std::chrono::duration<double>::zero();
    
    // Initialize type conversion database
    initializeTypeConversions();
    
    log("BindingGenerator initialized with config: " + std::to_string(static_cast<int>(config_.bindingLevel)));
}

bool BindingGenerator::initializeTypeConversions() {
    // Define type conversions for common NLM types to pybind11 types
    typeConversions_["int64_t"] = {"int64_t", "int", "py::arg", false, false, false, "0"};
    typeConversions_["double"] = {"double", "float", "py::arg", false, false, false, "0.0"};
    typeConversions_["float"] = {"float", "float", "py::arg", false, false, false, "0.0f"};
    typeConversions_["bool"] = {"bool", "bool", "py::arg", false, false, false, "false"};
    typeConversions_["std::string"] = {"std::string", "str", "py::arg", false, false, false, "\"\""};
    typeConversions_["std::vector<float>"] = {"std::vector<float>", "List[float]", "py::arg", false, true, false, "{}"};
    typeConversions_["std::vector<double>"] = {"std::vector<double>", "List[float]", "py::arg", false, true, false, "{}"};
    typeConversions_["std::vector<int64_t>"] = {"std::vector<int64_t>", "List[int]", "py::arg", false, true, false, "{}"};
    typeConversions_["std::shared_ptr<Config>"] = {"std::shared_ptr<Config>", "Config", "py::arg", false, false, true, "nullptr"};
    typeConversions_["std::shared_ptr<Brain>"] = {"std::shared_ptr<Brain>", "Brain", "py::arg", false, false, true, "nullptr"};
    typeConversions_["std::unique_ptr<Action>"] = {"std::unique_ptr<Action>", "Action", "py::arg", false, false, true, "nullptr"};
    
    return true;
}

bool BindingGenerator::generateAllBindings() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    log("Starting binding code generation...");
    log("Output directory: " + config_.outputDirectory);
    log("Binding level: " + std::to_string(static_cast<int>(config_.bindingLevel)));
    
    if (!ensureOutputDirectory()) {
        log("ERROR: Failed to ensure output directory");
        return false;
    }
    
    // Parse header files and extract metadata
    if (!parseHeaderFiles()) {
        log("ERROR: Failed to parse header files");
        return false;
    }
    
    // Generate bindings based on configuration
    switch (config_.bindingLevel) {
        case BindingConfig::Level::SIMPLE:
            if (!generateSimpleBindings()) {
                log("ERROR: Failed to generate simple bindings");
                return false;
            }
            break;
        case BindingConfig::Level::ADVANCED:
            if (!generateAdvancedBindings()) {
                log("ERROR: Failed to generate advanced bindings");
                return false;
            }
            break;
        case BindingConfig::Level::FULL:
            if (!generateSimpleBindings()) {
                log("ERROR: Failed to generate simple bindings");
                return false;
            }
            if (!generateAdvancedBindings()) {
                log("ERROR: Failed to generate advanced bindings");
                return false;
            }
            break;
    }
    
    // Write generated files
    if (!writeGeneratedFiles()) {
        log("ERROR: Failed to write generated files");
        return false;
    }
    
    // Validate generated code
    if (!validateGeneratedCode()) {
        log("ERROR: Generated code validation failed");
        return false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    stats_.generationTime = std::chrono::duration<double>(endTime - startTime);
    
    log("Binding code generation completed successfully");
    log("Processed " + std::to_string(stats_.classesProcessed) + " classes");
    log("Processed " + std::to_string(stats_.methodsProcessed) + " methods");
    log("Generated " + std::to_string(stats_.linesGenerated) + " lines of code");
    log("Generated " + std::to_string(stats_.filesGenerated) + " files");
    log("Generation time: " + std::to_string(stats_.generationTime.count()) + " seconds");
    
    return true;
}

bool BindingGenerator::parseHeaderFiles() {
    log("Parsing header files for NLM classes...");
    
    // Common header paths for NLM classes
    std::vector<std::string> headerPaths = {
        "../src/core/Config/Config.hpp",
        "../src/brain/Brain.hpp",
        "../src/brain/Neuron.hpp",
        "../src/brain/Synapse.hpp",
        "../src/brain/NeuralRegion.hpp",
        "../src/brain/NeuralPopulation.hpp",
        "../src/core/Types/Types.hpp",
        "../src/sensory/SensoryInput.hpp",
        "../src/sensory/Vision.hpp",
        "../src/sensory/Audio.hpp",
        "../src/sensory/InternalSignals.hpp",
        "../src/motor/Action.hpp",
        "../src/world/SimpleWorld.hpp",
        "../src/agent/AgentBrain.hpp",
        "../src/agent/SensoryPercept.hpp"
    };
    
    for (const auto& headerPath : headerPaths) {
        if (parseHeaderFile(headerPath)) {
            log("  Parsed " + headerPath);
        } else {
            log("  WARNING: Failed to parse " + headerPath);
        }
    }
    
    // Extract class metadata from source files
    if (!extractClassMetadata()) {
        log("ERROR: Failed to extract class metadata");
        return false;
    }
    
    return true;
}

bool BindingGenerator::parseHeaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    // Extract namespace
    std::regex namespaceRegex(R"(namespace\s+(\w+)\s*{)");
    std::smatch match;
    std::string namespaceName = "nlm"; // Default
    
    if (std::regex_search(source, match, namespaceRegex)) {
        namespaceName = match[1].str();
    }
    
    // Parse classes in the header file
    // For now, we'll do basic parsing - in a full implementation,
    // this would use a proper C++ parser
    
    return true;
}

bool BindingGenerator::extractClassMetadata() {
    log("Extracting class metadata...");
    
    // This is a simplified implementation
    // In a full implementation, we would parse the actual headers
    // and extract method signatures, types, etc.
    
    // Manually define metadata for key NLM classes
    ClassMetadata configMeta;
    configMeta.name = "Config";
    configMeta.namespaceName = "nlm";
    configMeta.headerFile = "../src/core/Config/Config.hpp";
    configMeta.documentation = "Configuration class for NLM system";
    configMeta.hasPimpl = true;
    configMeta.publicMethods = {
        "loadFromFile", "loadFromArgs", "saveToFile", "has", "getKeys",
        "clear", "summary", "setValidator", "setValidationMode", "getValidationMode",
        "validate", "validateEntry", "setErrorHandlingEnabled", "isErrorHandlingEnabled",
        "setErrorStrategy", "setErrorReportLevel", "getErrorStats"
    };
    configMeta.properties = {"errorHandlingEnabled", "errorStats", "errorHandlingEnabled"};
    classMetadata_["Config"] = configMeta;
    stats_.classesProcessed++;
    
    ClassMetadata brainMeta;
    brainMeta.name = "Brain";
    brainMeta.namespaceName = "nlm";
    brainMeta.headerFile = "../src/brain/Brain.hpp";
    brainMeta.documentation = "Central neural simulation brain class";
    brainMeta.hasPimpl = true;
    brainMeta.publicMethods = {
        "initialize", "step", "receiveSensoryInput", "injectCurrent",
        "injectCurrentToNeurons", "produceAction", "applyNeuromodulation",
        "updatePlasticity", "develop", "reset", "save", "load",
        "addRegion", "getRegion", "getRegionCount", "getRegionIds",
        "getRegions", "getTotalNeuronCount", "getTotalSynapseCount",
        "getActiveNeuronCount", "getFiringNeuronCount", "getAverageFiringRate",
        "getExcitationInhibitionRatio", "getTotalSpikeCount",
        "getDevelopmentalStage", "setDevelopmentalStage", "getConfig",
        "logStatus"
    };
    brainMeta.properties = {"pImpl"};
    classMetadata_["Brain"] = brainMeta;
    stats_.classesProcessed++;
    
    ClassMetadata neuronMeta;
    neuronMeta.name = "Neuron";
    neuronMeta.namespaceName = "nlm";
    neuronMeta.headerFile = "../src/brain/Neuron.hpp";
    neuronMeta.documentation = "Neuron class representing a single neuron";
    neuronMeta.hasPimpl = true;
    neuronMeta.publicMethods = {
        "getId", "getType", "setType", "getState", "getMembranePotential",
        "setMembranePotential", "addToMembranePotential", "getThreshold",
        "setThreshold", "isFiring", "isRefractory", "setFiringState",
        "setRefractoryPeriod", "decrementRefractory", "getFiringRate",
        "setFiringRate", "setLeakConductance", "getLeakConductance",
        "getRefractoryPeriod", "setRestingPotential", "getRestingPotential",
        "setResetPotential", "checkThreshold", "getLastSpikeTime",
        "stepLIF", "receiveExcitatoryInput", "receiveInhibitoryInput",
        "receiveModulatoryInput", "injectCurrent", "getTotalCurrent",
        "clearTotalCurrent", "recordSpike", "getSpikeHistory",
        "clearSpikeHistory", "addIncomingSynapse", "addOutgoingSynapse",
        "getIncomingSynapses", "getOutgoingSynapses", "getPlasticityFlags",
        "enablePlasticity", "setRegionId", "getRegionId", "setPopulationId",
        "getPopulationId", "step", "reset", "initializeRandom"
    };
    neuronMeta.properties = {"pImpl"};
    classMetadata_["Neuron"] = neuronMeta;
    stats_.classesProcessed++;
    
    ClassMetadata synapseMeta;
    synapseMeta.name = "Synapse";
    synapseMeta.namespaceName = "nlm";
    synapseMeta.headerFile = "../src/brain/Synapse.hpp";
    synapseMeta.documentation = "Synapse representing a connection between neurons";
    synapseMeta.hasPimpl = true;
    synapseMeta.publicMethods = {
        "getId", "getSourceNeuron", "getDestinationNeuron", "getWeight",
        "setWeight", "addToWeight", "getDelay", "setDelay", "getType",
        "setType", "isExcitatory", "isInhibitory", "recordPreSpike",
        "recordPostSpike", "getPreSpikeHistory", "getPostSpikeHistory",
        "clearHistory", "getPlasticityFlags", "enablePlasticity",
        "getEligibilityTrace", "setEligibilityTrace", "decayEligibilityTrace",
        "getEfficacy", "setEfficacy", "step", "reset", "initializeRandom"
    };
    synapseMeta.properties = {"pImpl"};
    classMetadata_["Synapse"] = synapseMeta;
    stats_.classesProcessed++;
    
    // Extract NeuronId, SynapseId, etc. as value types
    ClassMetadata neuronIdMeta;
    neuronIdMeta.name = "NeuronId";
    neuronIdMeta.namespaceName = "nlm";
    neuronIdMeta.headerFile = "../src/core/Types/Types.hpp";
    neuronIdMeta.documentation = "Unique identifier for a neuron";
    neuronIdMeta.isEnum = false;
    classMetadata_["NeuronId"] = neuronIdMeta;
    stats_.classesProcessed++;
    
    ClassMetadata synapseIdMeta;
    synapseIdMeta.name = "SynapseId";
    synapseIdMeta.namespaceName = "nlm";
    synapseIdMeta.headerFile = "../src/core/Types/Types.hpp";
    synapseIdMeta.documentation = "Unique identifier for a synapse";
    synapseIdMeta.isEnum = false;
    classMetadata_["SynapseId"] = synapseIdMeta;
    stats_.classesProcessed++;
    
    ClassMetadata regionIdMeta;
    regionIdMeta.name = "RegionId";
    regionIdMeta.namespaceName = "nlm";
    regionIdMeta.headerFile = "../src/core/Types/Types.hpp";
    regionIdMeta.documentation = "Unique identifier for a brain region";
    regionIdMeta.isEnum = false;
    classMetadata_["RegionId"] = regionIdMeta;
    stats_.classesProcessed++;
    
    ClassMetadata populationIdMeta;
    populationIdMeta.name = "PopulationId";
    populationIdMeta.namespaceName = "nlm";
    populationIdMeta.headerFile = "../src/core/Types/Types.hpp";
    populationIdMeta.documentation = "Unique identifier for a neuron population";
    populationIdMeta.isEnum = false;
    classMetadata_["PopulationId"] = populationIdMeta;
    stats_.classesProcessed++;
    
    // Extract enums
    std::map<std::string, int> neuronTypeValues = {
        {"Excitatory", 0}, {"Inhibitory", 1}, {"Modulatory", 2},
        {"Sensory", 3}, {"Motor", 4}, {"Internal", 5}
    };
    
    ClassMetadata neuronTypeMeta;
    neuronTypeMeta.name = "NeuronType";
    neuronTypeMeta.namespaceName = "nlm";
    neuronTypeMeta.headerFile = "../src/core/Types/Types.hpp";
    neuronTypeMeta.documentation = "Neuron type enumeration";
    neuronTypeMeta.isEnum = true;
    neuronTypeMeta.enumValues = neuronTypeValues;
    classMetadata_["NeuronType"] = neuronTypeMeta;
    stats_.classesProcessed++;
    
    return true;
}

bool BindingGenerator::generateSimpleBindings() {
    log("Generating simple bindings...");
    
    for (const auto& pair : classMetadata_) {
        const auto& metadata = pair.second;
        
        // Skip excluded classes
        if (config_.excludeClasses.find(metadata.name) != config_.excludeClasses.end()) {
            log("  Skipping excluded class: " + metadata.name);
            continue;
        }
        
        // Skip if only specific classes are included
        if (!config_.includeOnlyClasses.empty() &&
            config_.includeOnlyClasses.find(metadata.name) == config_.includeOnlyClasses.end()) {
            log("  Skipping non-included class: " + metadata.name);
            continue;
        }
        
        std::string bindingCode;
        
        if (metadata.isEnum) {
            bindingCode = generateEnumBinding(metadata);
        } else if (metadata.name.find("Id") != std::string::npos) {
            bindingCode = generateValueTypeBinding(metadata);
        } else {
            bindingCode = generateSimpleBinding(metadata);
        }
        
        if (bindingCode.empty()) {
            log("  WARNING: Failed to generate bindings for " + metadata.name);
            continue;
        }
        
        // Store generated code
        generatedClasses_.push_back(bindingCode);
        updateStats("bindings", 1);
        
        // Apply custom bindings if any
        if (customBindings_.find(metadata.name) != customBindings_.end()) {
            bindingCode = customBindings_[metadata.name];
        }
        
        // Execute extension hooks if configured
        if (!config_.extensionHookPath.empty()) {
            if (!executeCustomExtension(metadata, bindingCode)) {
                log("  WARNING: Custom extension failed for " + metadata.name);
            }
        }
        
        log("  Generated simple bindings for: " + metadata.name);
    }
    
    return true;
}

bool BindingGenerator::generateAdvancedBindings() {
    log("Generating advanced bindings...");
    
    for (const auto& pair : classMetadata_) {
        const auto& metadata = pair.second;
        
        // Skip excluded classes
        if (config_.excludeClasses.find(metadata.name) != config_.excludeClasses.end()) {
            continue;
        }
        
        // Skip if only specific classes are included
        if (!config_.includeOnlyClasses.empty() &&
            config_.includeOnlyClasses.find(metadata.name) == config_.includeOnlyClasses.end()) {
            continue;
        }
        
        std::string advancedBindingCode = generateAdvancedBinding(metadata);
        
        if (advancedBindingCode.empty()) {
            log("  WARNING: Failed to generate advanced bindings for " + metadata.name);
            continue;
        }
        
        // Append to generated classes (separate from simple bindings)
        generatedClasses_.push_back(advancedBindingCode);
        updateStats("bindings", 1);
        
        log("  Generated advanced bindings for: " + metadata.name);
    }
    
    return true;
}

std::string BindingGenerator::generateSimpleBinding(const ClassMetadata& metadata) const {
    std::string bindingCode;
    
    if (metadata.name == "Config") {
        bindingCode = generateConfigSimpleBinding();
    } else if (metadata.name == "Brain") {
        bindingCode = generateBrainSimpleBinding();
    } else if (metadata.name == "Neuron") {
        bindingCode = generateNeuronSimpleBinding();
    } else if (metadata.name == "Synapse") {
        bindingCode = generateSynapseSimpleBinding();
    } else {
        bindingCode = generateGenericSimpleBinding(metadata);
    }
    
    return bindingCode;
}

std::string BindingGenerator::generateAdvancedBinding(const ClassMetadata& metadata) const {
    std::string bindingCode;
    
    if (metadata.name == "Config") {
        bindingCode = generateConfigAdvancedBinding();
    } else if (metadata.name == "Brain") {
        bindingCode = generateBrainAdvancedBinding();
    } else if (metadata.name == "Neuron") {
        bindingCode = generateNeuronAdvancedBinding();
    } else if (metadata.name == "Synapse") {
        bindingCode = generateSynapseAdvancedBinding();
    } else {
        bindingCode = generateGenericAdvancedBinding(metadata);
    }
    
    return bindingCode;
}

std::string BindingGenerator::generateEnumBinding(const ClassMetadata& metadata) const {
    std::string bindingCode;
    
    if (metadata.name == "NeuronType") {
        bindingCode = generateNeuronTypeEnumBinding();
    } else if (metadata.name == "SynapseType") {
        bindingCode = generateSynapseTypeEnumBinding();
    } else {
        bindingCode = generateGenericEnumBinding(metadata);
    }
    
    return bindingCode;
}

std::string BindingGenerator::generateConfigSimpleBinding() const {
    return R"pbdoc(
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
)pbdoc";
}

std::string BindingGenerator::generateConfigAdvancedBinding() const {
    return R"pbdoc(
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class with error handling)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file", py::call_guard<py::call_guard>())
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments", py::call_guard<py::call_guard>())
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file", py::call_guard<py::call_guard>())
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists", py::call_guard<py::call_guard>())
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys", py::call_guard<py::call_guard>())
        .def("clear", &Config::clear,
             "Clear all configuration entries", py::call_guard<py::call_guard>())
        .def("summary", &Config::summary,
             "Get a summary string of the configuration", py::call_guard<py::call_guard>())
        .def("validate", &Config::validate,
             "Validate the configuration with error handling")
        .def("validateEntry", &Config::validateEntry, py::arg("entry"),
             "Validate a single configuration entry")
        .def("setValidationMode", &Config::setValidationMode, py::arg("mode"),
             "Set the validation mode (Strict, Warning, Off)")
        .def("getValidationMode", &Config::getValidationMode,
             "Get the current validation mode")
        .def("setErrorHandlingEnabled", &Config::setErrorHandlingEnabled,
             py::arg("enabled"), "Enable or disable error handling")
        .def("isErrorHandlingEnabled", &Config::isErrorHandlingEnabled,
             "Check if error handling is enabled")
        .def("getErrorStats", &Config::getErrorStats,
             "Get error statistics")
        .def("setErrorStrategy", &Config::setErrorStrategy, py::arg("strategy"),
             "Set error handling strategy")
        .def("setErrorReportLevel", &Config::setErrorReportLevel, py::arg("level"),
             "Set error reporting level")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"))
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("__contains__", &Config::has, py::arg("key"))
        .def("__len__", &Config::getKeys)
        .def("items", &Config::getKeys)
        .def("keys", &Config::getKeys)
        .def("values", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            std::vector<std::string> values;
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                values.push_back(value ? *value : "");
            }
            return values;
        })
        .def("_get", &Config::get<std::string>, py::arg("key"))
        .def("_set", &Config::set<std::string>, py::arg("key"), py::arg("value"))
        .def("_get_int", &Config::get<int64_t>, py::arg("key"))
        .def("_get_double", &Config::get<double>, py::arg("key"))
        .def("_get_bool", &Config::get<bool>, py::arg("key"));
)pbdoc";
}

std::string BindingGenerator::generateBrainSimpleBinding() const {
    return R"pbdoc(
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");
)pbdoc";
}

std::string BindingGenerator::generateBrainAdvancedBinding() const {
    return R"pbdoc(
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced central neural simulation brain class with full API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration", py::call_guard<py::call_guard>())
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step", py::call_guard<py::call_guard>())
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp", py::call_guard<py::call_guard>())
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain", py::call_guard<py::call_guard>())
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron", py::call_guard<py::call_guard>())
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type", py::call_guard<py::call_guard>())
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity", py::call_guard<py::call_guard>())
        .def("reset", &Brain::reset,
             "Reset brain state", py::call_guard<py::call_guard>())
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file", py::call_guard<py::call_guard>())
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file", py::call_guard<py::call_guard>())
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region", py::call_guard<py::call_guard>())
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID", py::call_guard<py::call_guard>())
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions", py::call_guard<py::call_guard>())
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs", py::call_guard<py::call_guard>())
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions", py::call_guard<py::call_guard>())
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions", py::call_guard<py::call_guard>())
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions", py::call_guard<py::call_guard>())
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons", py::call_guard<py::call_guard>())
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons", py::call_guard<py::call_guard>())
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons", py::call_guard<py::call_guard>())
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio", py::call_guard<py::call_guard>())
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count", py::call_guard<py::call_guard>())
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage", py::call_guard<py::call_guard>())
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage", py::call_guard<py::call_guard>())
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration", py::call_guard<py::call_guard>())
        .def("logStatus", &Brain::logStatus,
             "Log brain status", py::call_guard<py::call_guard>())
        // Convenience methods
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (convenience method)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (convenience method)")
        .def("getFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate (convenience method)")
        .def("stepWithTime", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Step the simulation with timestamp (convenience method)")
        .def("injectToType", [](Brain& self, NeuronType type, MembranePotential current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
           "Inject current to all neurons of a type (convenience method)")
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["neuron_count"] = self.getTotalNeuronCount();
            stats["synapse_count"] = self.getTotalSynapseCount();
            stats["active_neuron_count"] = self.getActiveNeuronCount();
            stats["firing_neuron_count"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["spike_count"] = self.getTotalSpikeCount();
            return stats;
        }, "Get brain statistics as dictionary (convenience method)")
        .def("to_dict", [](const Brain& self) {
            py::dict result;
            result["total_neuron_count"] = self.getTotalNeuronCount();
            result["total_synapse_count"] = self.getTotalSynapseCount();
            result["active_neuron_count"] = self.getActiveNeuronCount();
            result["firing_neuron_count"] = self.getFiringNeuronCount();
            result["average_firing_rate"] = self.getAverageFiringRate();
            result["spike_count"] = self.getTotalSpikeCount();
            result["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            return result;
        }, "Convert brain state to dictionary (convenience method)")
        .def("from_dict", [](Brain& self, const py::dict& data) {
            if (data.contains("developmental_stage")) {
                self.setDevelopmentalStage(static_cast<DevelopmentalStage>(data["developmental_stage"].cast<int>()));
            }
        }, py::arg("data"),
           "Update brain state from dictionary (convenience method)")
        .def("__repr__", [](const Brain& brain) {
            std::stringstream ss;
            ss << "<Brain: " << "Total Neurons: " << brain.getTotalNeuronCount() << ", "
               << "Total Synapses: " << brain.getTotalSynapseCount() << ", "
               << "Regions: " << brain.getRegionCount() << ">";
            return ss.str();
        });
)pbdoc";
}

std::string BindingGenerator::generateNeuronSimpleBinding() const {
    return R"pbdoc(
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Neuron class representing a single neuron)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId,
             "Get neuron identifier")
        .def("getType", &Neuron::getType,
             "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"),
             "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             "Add to membrane potential")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached firing threshold")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform Leaky Integrate-and-Fire step")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike at the given timestamp")
        .def("reset", &Neuron::reset,
             "Reset neuron to initial state");
)pbdoc";
}

std::string BindingGenerator::generateNeuronAdvancedBinding() const {
    return R"pbdoc(
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Advanced neuron class with full LIF dynamics and comprehensive state management)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId,
             "Get neuron identifier")
        .def("getType", &Neuron::getType,
             "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"),
             "Set neuron type")
        .def("getState", &Neuron::getState,
             "Get complete neuron state")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold,
             "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             "Set firing threshold")
        .def("isFiring", &Neuron::isFiring,
             "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory,
             "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"),
             "Set neuron firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"),
             "Set refractory period duration")
        .def("decrementRefractory", &Neuron::decrementRefractory,
             "Decrement refractory period counter")
        .def("getFiringRate", &Neuron::getFiringRate,
             "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"),
             "Set firing rate")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"),
             "Set leak conductance")
        .def("getLeakConductance", &Neuron::getLeakConductance,
             "Get leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod,
             "Get refractory period duration")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"),
             "Set resting potential")
        .def("getRestingPotential", &Neuron::getRestingPotential,
             "Get resting potential")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"),
             "Set reset potential")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             "Get timestamp of last spike")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached firing threshold")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform Leaky Integrate-and-Fire step", py::call_guard<py::call_guard>())
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"),
             "Receive excitatory input (post-synaptic potential)", py::call_guard<py::call_guard>())
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"),
             "Receive inhibitory input (hyperpolarization)", py::call_guard<py::call_guard>())
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"),
             "Receive modulatory input (neuromodulation)", py::call_guard<py::call_guard>())
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             "Inject current from external source", py::call_guard<py::call_guard>())
        .def("getTotalCurrent", &Neuron::getTotalCurrent,
             "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent,
             "Clear injected current accumulator")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike at the given timestamp", py::call_guard<py::call_guard>())
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             "Get spike history (timestamps)")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"),
             "Add incoming synapse handle")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"),
             "Add outgoing synapse handle")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses,
             "Get incoming synapse handles")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses,
             "Get outgoing synapse handles")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable/disable plasticity rules")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"),
             "Set neuron region membership")
        .def("getRegionId", &Neuron::getRegionId,
             "Get neuron region membership")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"),
             "Set neuron population membership")
        .def("getPopulationId", &Neuron::getPopulationId,
             "Get neuron population membership")
        .def("step", &Neuron::step, py::arg("currentTime"),
             "Update neuron for one simulation step", py::call_guard<py::call_guard>())
        .def("reset", &Neuron::reset,
             "Reset neuron to initial state", py::call_guard<py::call_guard>())
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"),
             "Initialize neuron with random parameters", py::call_guard<py::call_guard>())
        // Convenience methods
        .def("fire", &Neuron::checkThreshold,
             "Check and handle neuron firing (convenience method)")
        .def("getMembranePotentialMV", &Neuron::getMembranePotential,
             "Get membrane potential in mV (convenience method)")
        .def("getThresholdMV", &Neuron::getThreshold,
             "Get threshold in mV (convenience method)")
        .def("getRestingPotentialMV", &Neuron::getRestingPotential,
             "Get resting potential in mV (convenience method)")
        .def("getResetPotentialMV", &Neuron::getResetPotential,
             "Get reset potential in mV (convenience method)")
        .def("getFiringRateHz", &Neuron::getFiringRate,
             "Get firing rate in Hz (convenience method)")
        .def("isActive", &Neuron::isFiring,
             "Check if neuron is active (convenience method)")
        .def("getStateDict", [](const Neuron& self) {
            py::dict state;
            const auto& neuronState = self.getState();
            state["membrane_potential"] = neuronState.membranePotential;
            state["resting_potential"] = neuronState.restingPotential;
            state["threshold"] = neuronState.threshold;
            state["reset_potential"] = neuronState.resetPotential;
            state["firing_rate"] = neuronState.firingRate;
            state["firing_state"] = static_cast<int>(neuronState.firingState);
            state["refractory_remaining"] = neuronState.refractoryRemaining;
            state["refractory_period"] = neuronState.refractoryPeriod;
            state["adaptation_variable"] = neuronState.adaptationVariable;
            state["last_spike_time"] = neuronState.lastSpikeTime;
            return state;
        }, "Get neuron state as dictionary (convenience method)")
        .def("from_dict", [](Neuron& self, const py::dict& state) {
            auto& neuronState = self.getState();
            if (state.contains("threshold")) {
                neuronState.threshold = state["threshold"].cast<MembranePotential>();
            }
            if (state.contains("reset_potential")) {
                neuronState.resetPotential = state["reset_potential"].cast<MembranePotential>();
            }
            if (state.contains("firing_rate")) {
                neuronState.firingRate = state["firing_rate"].cast<FiringRate>();
            }
            if (state.contains("refractory_period")) {
                neuronState.refractoryPeriod = state["refractory_period"].cast<uint32_t>();
            }
        }, py::arg("state"),
           "Update neuron state from dictionary (convenience method)")
        .def("simulateStep", [](Neuron& self, Timestamp time, TimestepDuration dt) {
            self.stepLIF(time, dt);
            self.step(time);
        }, py::arg("time"), py::arg("dt"),
           "Perform a complete neuron simulation step (convenience method)")
        .def("__repr__", [](const Neuron& neuron) {
            std::stringstream ss;
            ss << "<Neuron: " << neuron.getId().value << ", Type: " << static_cast<int>(neuron.getType()) 
               << ", V: " << neuron.getMembranePotential() << "mV>";
            return ss.str();
        });
)pbdoc";
}

std::string BindingGenerator::generateSynapseSimpleBinding() const {
    return R"pbdoc(
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synapse representing a connection between neurons)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId,
             "Get synapse identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron,
             "Get source neuron identifier")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron,
             "Get destination neuron identifier")
        .def("getWeight", &Synapse::getWeight,
             "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"),
             "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"),
             "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay,
             "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"),
             "Set synaptic delay")
        .def("getType", &Synapse::getType,
             "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"),
             "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory,
             "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory,
             "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"),
             "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"),
             "Record postsynaptic spike")
        .def("reset", &Synapse::reset,
             "Reset synapse to initial state");
)pbdoc";
}

std::string BindingGenerator::generateSynapseAdvancedBinding() const {
    return R"pbdoc(
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Advanced synapse class with full synaptic dynamics and plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId,
             "Get synapse identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron,
             "Get source neuron identifier")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron,
             "Get destination neuron identifier")
        .def("getWeight", &Synapse::getWeight,
             "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"),
             "Set synaptic weight", py::call_guard<py::call_guard>())
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"),
             "Add to synaptic weight", py::call_guard<py::call_guard>())
        .def("getDelay", &Synapse::getDelay,
             "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"),
             "Set synaptic delay", py::call_guard<py::call_guard>())
        .def("getType", &Synapse::getType,
             "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"),
             "Set synapse type", py::call_guard<py::call_guard>())
        .def("isExcitatory", &Synapse::isExcitatory,
             "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory,
             "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"),
             "Record presynaptic spike", py::call_guard<py::call_guard>())
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"),
             "Record postsynaptic spike", py::call_guard<py::call_guard>())
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory,
             "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory,
             "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory,
             "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags,
             "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable/disable plasticity rules", py::call_guard<py::call_guard>())
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace,
             "Get eligibility trace for reward-modulated learning")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"),
             "Set eligibility trace", py::call_guard<py::call_guard>())
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"),
             "Decay eligibility trace", py::call_guard<py::call_guard>())
        .def("getEfficacy", &Synapse::getEfficacy,
             "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"),
             "Set synaptic efficacy", py::call_guard<py::call_guard>())
        .def("step", &Synapse::step, py::arg("currentTime"),
             "Update synapse for one simulation step", py::call_guard<py::call_guard>())
        .def("reset", &Synapse::reset,
             "Reset synapse to initial state", py::call_guard<py::call_guard>())
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"),
             "Initialize synapse with random parameters", py::call_guard<py::call_guard>())
        // Convenience methods
        .def("getStrength", &Synapse::getWeight,
             "Get synaptic strength (convenience method)")
        .def("setStrength", &Synapse::setWeight, py::arg("strength"),
             "Set synaptic strength (convenience method)")
        .def("addStrength", &Synapse::addToWeight, py::arg("delta"),
             "Modify synaptic strength (convenience method)")
        .def("getDelaySteps", &Synapse::getDelay,
             "Get delay in simulation steps (convenience method)")
        .def("getDelayTime", [](const Synapse& self) {
            return static_cast<double>(self.getDelay());
        }, "Get delay in seconds (convenience method)")
        .def("isElectrical", [](const Synapse& self) {
            return self.getType() == SynapseType::Electrical;
        }, "Check if synapse is electrical (convenience method)")
        .def("isGapJunction", [](const Synapse& self) {
            return self.getType() == SynapseType::GapJunction;
        }, "Check if synapse is a gap junction (convenience method)")
        .def("getHistory", [](const Synapse& self) {
            py::dict history;
            auto preSpikes = self.getPreSpikeHistory();
            auto postSpikes = self.getPostSpikeHistory();
            history["presynaptic_spikes"] = preSpikes;
            history["postsynaptic_spikes"] = postSpikes;
            return history;
        }, "Get spike history as dictionary (convenience method)")
        .def("getSynapticState", [](const Synapse& self) {
            py::dict state;
            state["weight"] = self.getWeight();
            state["type"] = static_cast<int>(self.getType());
            state["is_excitatory"] = self.isExcitatory();
            state["is_inhibitory"] = self.isInhibitory();
            state["eligibility_trace"] = self.getEligibilityTrace();
            state["efficacy"] = self.getEfficacy();
            return state;
        }, "Get synaptic state as dictionary (convenience method)")
        .def("updateState", [](Synapse& self, const py::dict& state) {
            if (state.contains("weight")) {
                self.setWeight(state["weight"].cast<SynapticWeight>());
            }
            if (state.contains("delay")) {
                self.setDelay(state["delay"].cast<Delay>());
            }
            if (state.contains("type")) {
                self.setType(static_cast<SynapseType>(state["type"].cast<int>()));
            }
        }, py::arg("state"),
           "Update synapse state from dictionary (convenience method)")
        .def("simulateStep", [](Synapse& self, Timestamp time) {
            self.step(time);
        }, py::arg("time"),
           "Perform a synapse simulation step (convenience method)")
        .def("__repr__", [](const Synapse& synapse) {
            std::stringstream ss;
            ss << "<Synapse: " << synapse.getId().value << ", "
               << "Source: " << synapse.getSourceNeuron().value << ", "
               << "Dest: " << synapse.getDestinationNeuron().value << ", "
               << "Weight: " << synapse.getWeight() << ", "
               << "Type: " << static_cast<int>(synapse.getType()) << ">";
            return ss.str();
        });
)pbdoc";
}

std::string BindingGenerator::generateValueTypeBinding(const ClassMetadata& metadata) const {
    if (metadata.name == "NeuronId" || metadata.name == "SynapseId" || 
        metadata.name == "RegionId" || metadata.name == "PopulationId") {
        return generateIdBinding(metadata);
    }
    return "";
}

std::string BindingGenerator::generateIdBinding(const ClassMetadata& metadata) const {
    std::string bindingCode;
    
    if (metadata.name == "NeuronId") {
        bindingCode = generateNeuronIdBinding();
    } else if (metadata.name == "SynapseId") {
        bindingCode = generateSynapseIdBinding();
    } else if (metadata.name == "RegionId") {
        bindingCode = generateRegionIdBinding();
    } else if (metadata.name == "PopulationId") {
        bindingCode = generatePopulationIdBinding();
    }
    
    return bindingCode;
}

std::string BindingGenerator::generateNeuronIdBinding() const {
    return R"pbdoc(
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });
)pbdoc";
}

std::string BindingGenerator::generateSynapseIdBinding() const {
    return R"pbdoc(
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });
)pbdoc";
}

std::string BindingGenerator::generateRegionIdBinding() const {
    return R"pbdoc(
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
)pbdoc";
}

std::string BindingGenerator::generatePopulationIdBinding() const {
    return R"pbdoc(
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
)pbdoc";
}

std::string BindingGenerator::generateNeuronTypeEnumBinding() const {
    return R"pbdoc(
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
)pbdoc";
}

std::string BindingGenerator::generateSynapseTypeEnumBinding() const {
    return R"pbdoc(
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
)pbdoc";
}

std::string BindingGenerator::generateGenericSimpleBinding(const ClassMetadata& metadata) const {
    std::stringstream bindingCode;
    bindingCode << "    // Generated simple binding for " << metadata.name << "\n";
    bindingCode << "    // TODO: Implement custom binding for " << metadata.name << "\n";
    return bindingCode.str();
}

std::string BindingGenerator::generateGenericAdvancedBinding(const ClassMetadata& metadata) const {
    std::stringstream bindingCode;
    bindingCode << "    // Generated advanced binding for " << metadata.name << "\n";
    bindingCode << "    // TODO: Implement custom binding for " << metadata.name << "\n";
    return bindingCode.str();
}

std::string BindingGenerator::generateGenericEnumBinding(const ClassMetadata& metadata) const {
    std::stringstream bindingCode;
    bindingCode << "    // Generated enum binding for " << metadata.name << "\n";
    bindingCode << "    // Values: ";
    for (const auto& pair : metadata.enumValues) {
        bindingCode << pair.first << " (" << pair.second << "), ";
    }
    return bindingCode.str();
}

std::string BindingGenerator::getPybindType(const std::string& cppType) const {
    if (typeConversions_.find(cppType) != typeConversions_.end()) {
        return typeConversions_.at(cppType).pyType;
    }
    return cppType; // Return original if not found
}

std::string BindingGenerator::getConversionFunction(const std::string& cppType) const {
    if (typeConversions_.find(cppType) != typeConversions_.end()) {
        return typeConversions_.at(cppType).conversionFunction;
    }
    return "py::arg"; // Default
}

std::string BindingGenerator::escapeString(const std::string& str) const {
    std::string escaped = str;
    // Replace backslashes
    size_t pos = 0;
    while ((pos = escaped.find('\\', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\\");
        pos += 2;
    }
    // Replace quotes
    pos = 0;
    while ((pos = escaped.find('\"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\"");
        pos += 2;
    }
    return escaped;
}

std::string BindingGenerator::generateDocstring(const std::string& documentation) const {
    if (documentation.empty()) {
        return "";
    }
    
    // Escape doc comment and convert to pbdoc format
    std::string doc = documentation;
    // Replace newlines with spaces and ensure it's properly formatted
    doc = escapeString(doc);
    return "R\"pbdoc(" + doc + ")pbdoc\"";
}

bool BindingGenerator::isStandardType(const std::string& type) const {
    static const std::vector<std::string> standardTypes = {
        "int", "int64_t", "double", "float", "bool", "char", "const char*", "std::string"
    };
    
    return std::find(standardTypes.begin(), standardTypes.end(), type) != standardTypes.end();
}

bool BindingGenerator::isContainerType(const std::string& type) const {
    return type.find("std::vector<") != std::string::npos ||
           type.find("std::map<") != std::string::npos ||
           type.find("std::list<") != std::string::npos ||
           type.find("std::unordered_") != std::string::npos;
}

bool BindingGenerator::isSmartPointerType(const std::string& type) const {
    return type.find("std::shared_ptr<") != std::string::npos ||
           type.find("std::unique_ptr<") != std::string::npos ||
           type.find("std::weak_ptr<") != std::string::npos;
}

bool BindingGenerator::ensureOutputDirectory() const {
    if (config_.outputDirectory.empty()) {
        return true; // Use current directory
    }
    
    try {
        fs::create_directories(config_.outputDirectory);
        return true;
    } catch (const fs::filesystem_error& e) {
        log("ERROR: Failed to create output directory: " + std::string(e.what()));
        return false;
    }
}

bool BindingGenerator::writeFile(const std::string& path, const std::string& content) const {
    std::ofstream file(path);
    if (!file.is_open()) {
        log("ERROR: Failed to open file for writing: " + path);
        return false;
    }
    
    file << content;
    file.close();
    
    log("  Written: " + path);
    return true;
}

std::string BindingGenerator::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void BindingGenerator::updateStats(const std::string& operation, size_t count) const {
    if (operation == "bindings") {
        stats_.filesGenerated++;
    }
    if (count > 0) {
        stats_.linesGenerated += count;
    }
}

void BindingGenerator::log(const std::string& message) const {
    std::cout << "[BindingGenerator] " << message << std::endl;
}

bool BindingGenerator::executeCustomExtension(const ClassMetadata& metadata, std::string& output) {
    // In a full implementation, this would execute the custom extension script
    // For now, just log that we would execute it
    log("  Executing custom extension for " + metadata.name);
    return true;
}

bool BindingGenerator::validateGeneratedCode(const std::string& generatedCode) const {
    // Basic validation - check for syntax errors
    // In a full implementation, this would use a more sophisticated validator
    
    // Check for balanced parentheses
    int parenCount = 0;
    for (char c : generatedCode) {
        if (c == '(') parenCount++;
        else if (c == ')') parenCount--;
        if (parenCount < 0) return false;
    }
    
    // Check for common pybind11 patterns
    if (generatedCode.find("py::class_") == std::string::npos &&
        generatedCode.find("py::enum_") == std::string::npos) {
        return false; // No binding found
    }
    
    return true;
}

bool BindingGenerator::checkForConflicts(const std::string& generatedCode) const {
    // Check for naming conflicts with custom bindings
    for (const auto& pair : customBindings_) {
        if (generatedCode.find(pair.first) != std::string::npos) {
            log("  WARNING: Potential conflict with custom binding: " + pair.first);
        }
    }
    return true;
}