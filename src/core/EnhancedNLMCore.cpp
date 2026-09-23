#include "EnhancedNLMCore.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace nlm {

// EnhancedConfigManager implementation
struct EnhancedConfigManager::Impl {
    std::unordered_map<std::string, std::string> configMap;
    std::vector<std::string> errorMessages;
};

bool EnhancedConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter = line.find('=');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            pImpl->configMap[key] = value;
        }
    }

    return true;
}

bool EnhancedConfigManager::loadFromString(const std::string& configString) {
    std::istringstream stream(configString);
    std::string line;
    bool success = true;

    while (std::getline(stream, line)) {
        size_t delimiter = line.find('=');
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
            pImpl->configMap[key] = value;
        }
    }

    return success;
}

bool EnhancedConfigManager::loadFromArgs(int argc, char* argv[]) {
    bool success = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                size_t delimiter = arg.find('=');
                std::string key = arg.substr(7, delimiter - 7);
                std::string value = arg.substr(delimiter + 1);
                pImpl->configMap[key] = value;
            }
        }
    }

    return success;
}

bool EnhancedConfigManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& pair : pImpl->configMap) {
        file << pair.first << "=" << pair.second << std::endl;
    }

    return true;
}

std::string EnhancedConfigManager::saveToString() const {
    std::ostringstream stream;

    for (const auto& pair : pImpl->configMap) {
        stream << pair.first << "=" << pair.second << std::endl;
    }

    return stream.str();
}

template<typename T>
T EnhancedConfigManager::get(const std::string& key, const T& defaultValue) const {
    auto it = pImpl->configMap.find(key);
    if (it == pImpl->configMap.end()) {
        return defaultValue;
    }

    // Simple conversion (in real implementation, use proper type conversion)
    std::string value = it->second;
    // For simplicity, just return default for non-string types
    return defaultValue;
}

template<typename T>
bool EnhancedConfigManager::set(const std::string& key, const T& value) {
    std::ostringstream stream;
    stream << value;
    pImpl->configMap[key] = stream.str();
    return true;
}

EnhancedConfigManager::ValidationResult EnhancedConfigManager::validate() const {
    ValidationResult result;
    result.valid = true;

    // Basic validation
    auto it = pImpl->configMap.find("neuron_count");
    if (it != pImpl->configMap.end()) {
        try {
            size_t neuronCount = std::stoull(it->second);
            if (neuronCount == 0) {
                result.valid = false;
                result.errors.push_back("neuron_count must be greater than 0");
            }
        } catch (const std::exception& e) {
            result.valid = false;
            result.errors.push_back("Invalid neuron_count: " + std::string(e.what()));
        }
    }

    it = pImpl->configMap.find("connection_probability");
    if (it != pImpl->configMap.end()) {
        try {
            float prob = std::stof(it->second);
            if (prob < 0.0f || prob > 1.0f) {
                result.errors.push_back("connection_probability must be between 0.0 and 1.0");
                result.valid = false;
            }
        } catch (const std::exception& e) {
            result.valid = false;
            result.errors.push_back("Invalid connection_probability: " + std::string(e.what()));
        }
    }

    validationResult_ = result;
    return result;
}

bool EnhancedConfigManager::applyPreset(Preset preset) {
    switch (preset) {
        case Preset::MINIMAL:
            pImpl->configMap["neuron_count"] = "100";
            pImpl->configMap["region_count"] = "1";
            pImpl->configMap["connection_probability"] = "0.05";
            pImpl->configMap["simulation_timestep"] = "0.001";
            break;

        case Preset::BALANCED:
            pImpl->configMap["neuron_count"] = "1000";
            pImpl->configMap["region_count"] = "1";
            pImpl->configMap["connection_probability"] = "0.1";
            pImpl->configMap["simulation_timestep"] = "0.001";
            break;

        case Preset::PERFORMANCE:
            pImpl->configMap["neuron_count"] = "500";
            pImpl->configMap["region_count"] = "1";
            pImpl->configMap["connection_probability"] = "0.2";
            pImpl->configMap["simulation_timestep"] = "0.0005";
            break;

        case Preset::RESEARCH:
            pImpl->configMap["neuron_count"] = "5000";
            pImpl->configMap["region_count"] = "2";
            pImpl->configMap["connection_probability"] = "0.15";
            pImpl->configMap["simulation_timestep"] = "0.001";
            break;

        case Preset::REALTIME:
            pImpl->configMap["neuron_count"] = "1000";
            pImpl->configMap["region_count"] = "1";
            pImpl->configMap["connection_probability"] = "0.3";
            pImpl->configMap["simulation_timestep"] = "0.0005";
            break;
    }

    return true;
}

// EnhancedConfigManager constructor and destructor
EnhancedConfigManager::EnhancedConfigManager() : pImpl(std::make_unique<Impl>()) {}
EnhancedConfigManager::~EnhancedConfigManager() = default;

// NLMError implementation
NLMError::NLMError(const std::string& message) : std::runtime_error(message), code_(Code::UNKNOWN) {}
NLMError::NLMError(Code code, const std::string& message) : std::runtime_error(message), code_(code) {}

// ConfigurationException implementation
ConfigurationException::ConfigurationException(const std::string& message) 
    : NLMException(Code::CONFIGURATION, message) {}

// InitializationException implementation
InitializationException::InitializationException(const std::string& message) 
    : NLMException(Code::INITIALIZATION, message) {}

// MemoryException implementation
MemoryException::MemoryException(const std::string& message) 
    : NLMException(Code::MEMORY, message) {}

// PlasticityException implementation
PlasticityException::PlasticityException(const std::string& message) 
    : NLMException(Code::PLASTICITY, message) {}

// IntegrationException implementation
IntegrationException::IntegrationException(const std::string& message) 
    : NLMException(Code::INTEGRATION, message) {}

} // namespace nlm