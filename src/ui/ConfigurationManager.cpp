#include "ConfigurationManager.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

namespace nlm {

ConfigurationManager::ConfigurationManager() {}

ConfigurationManager::~ConfigurationManager() {}

bool ConfigurationManager::loadConfiguration(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(configMutex);
    this->filePath = filePath;
    
    // For now, initialize with default values
    configData["brain.neuronCount"] = 1000;
    configData["brain.simulationSpeed"] = 1.0;
    configData["brain.learningRate"] = 0.01;
    configData["world.width"] = 100.0;
    configData["world.height"] = 100.0;
    configData["world.simulationSteps"] = 1000;
    configData["memory.episodicCapacity"] = 100;
    configData["memory.workingMemorySize"] = 50;
    configData["visualization.outputFormat"] = "png";
    configData["visualization.enable3D"] = false;
    configData["validation.strictMode"] = true;
    configData["debug.breakOnError"] = false;
    
    return true;
}

bool ConfigurationManager::saveConfiguration(const std::string& filePath) const {
    std::lock_guard<std::mutex> lock(configMutex);
    this->filePath = filePath;
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toString();
    file.close();
    return true;
}

template<typename T>
T ConfigurationManager::getValue(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = configData.find(key);
    if (it != configData.end()) {
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
    }
    
    throw std::runtime_error("Key not found: " + key);
}

template<typename T>
void ConfigurationManager::setValue(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(configMutex);
    configData[key] = value;
}

std::string ConfigurationManager::toString() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::stringstream ss;
    
    ss << "Configuration Data:" << std::endl;
    for (const auto& pair : configData) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    return ss.str();
}

std::string ConfigurationManager::toXML() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::stringstream ss;
    
    ss << "<?xml version=\"1.0\"?>" << std::endl;
    ss << "<configuration>" << std::endl;
    
    for (const auto& pair : configData) {
        ss << "  <" << pair.first << ">" << pair.second << "</" << pair.first << ">>" << std::endl;
    }
    
    ss << "</configuration>" << std::endl;
    return ss.str();
}

std::string ConfigurationManager::toJSON() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::stringstream ss;
    
    ss << "{" << std::endl;
    
    bool first = true;
    for (const auto& pair : configData) {
        if (!first) ss << "," << std::endl;
        ss << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    
    ss << std::endl << "}" << std::endl;
    return ss.str();
}

ValidationResult ConfigurationManager::validate() const {
    ValidationResult result;
    result.isValid = true;
    
    // Basic validation checks
    try {
        auto neuronCount = getValue<int>("brain.neuronCount");
        if (neuronCount <= 0) {
            result.isValid = false;
            result.errors.push_back("brain.neuronCount must be positive");
        }
    } catch (...) {
        result.isValid = false;
        result.errors.push_back("brain.neuronCount is missing or invalid");
    }
    
    try {
        auto learningRate = getValue<double>("brain.learningRate");
        if (learningRate < 0.0 || learningRate > 1.0) {
            result.isValid = false;
            result.errors.push_back("brain.learningRate must be between 0.0 and 1.0");
        }
    } catch (...) {
        // learningRate is optional
    }
    
    result.confidenceScore = result.isValid ? 1.0 : 0.0;
    return result;
}

std::vector<ConfigComparator::ComparisonResult> ConfigurationManager::compare(const ConfigurationManager& other) const {
    ConfigComparator comparator;
    Configuration tempConfig;
    
    // Create temporary configuration objects for comparison
    ConfigurationManager thisCopy = *this;
    ConfigurationManager otherCopy = other;
    
    return comparator.compare(thisCopy, otherCopy);
}

std::vector<std::string> ConfigurationManager::getKeys() const {
    std::lock_guard<std::mutex> lock(configMutex);
    std::vector<std::string> keys;
    keys.reserve(configData.size());
    
    for (const auto& pair : configData) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

bool ConfigurationManager::createFromTemplate(const std::string& templateName) {
    // Template implementation - load from templates directory
    if (templateName == "default") {
        configData.clear();
        configData["brain.neuronCount"] = 1000;
        configData["brain.simulationSpeed"] = 1.0;
        configData["brain.learningRate"] = 0.01;
        configData["world.width"] = 100.0;
        configData["world.height"] = 100.0;
        configData["world.simulationSteps"] = 1000;
        return true;
    } else if (templateName == "high_performance") {
        configData.clear();
        configData["brain.neuronCount"] = 5000;
        configData["brain.simulationSpeed"] = 2.0;
        configData["brain.learningRate"] = 0.05;
        configData["world.width"] = 200.0;
        configData["world.height"] = 200.0;
        configData["world.simulationSteps"] = 5000;
        configData["visualization.outputFormat"] = "gltf";
        configData["visualization.enable3D"] = true;
        return true;
    } else if (templateName == "research") {
        configData.clear();
        configData["brain.neuronCount"] = 10000;
        configData["brain.simulationSpeed"] = 0.5;
        configData["brain.learningRate"] = 0.001;
        configData["world.width"] = 500.0;
        configData["world.height"] = 500.0;
        configData["world.simulationSteps"] = 10000;
        configData["memory.episodicCapacity"] = 1000;
        configData["memory.workingMemorySize"] = 500;
        configData["validation.strictMode"] = true;
        return true;
    }
    
    return false;
}

bool ConfigurationManager::applyPreset(const std::string& presetName) {
    // Preset configuration implementation
    if (presetName == "quick_test") {
        configData["brain.neuronCount"] = 100;
        configData["world.simulationSteps"] = 100;
        return true;
    } else if (presetName == "standard_run") {
        configData["brain.neuronCount"] = 1000;
        configData["world.simulationSteps"] = 1000;
        configData["visualization.enable3D"] = false;
        return true;
    } else if (presetName == "detailed_analysis") {
        configData["brain.neuronCount"] = 2000;
        configData["world.simulationSteps"] = 5000;
        configData["visualization.enable3D"] = true;
        configData["validation.strictMode"] = true;
        configData["memory.episodicCapacity"] = 500;
        return true;
    }
    
    return false;
}

} // namespace nlm