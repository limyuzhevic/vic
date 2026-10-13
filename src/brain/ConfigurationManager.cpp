#include "ConfigurationManager.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

struct ConfigurationManager::Impl {
    std::string configFilepath;
    std::string configData;
    size_t parameterCount;
    
    Impl() : parameterCount(0) {}
};

ConfigurationManager::ConfigurationManager() : pImpl(new Impl()) {}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::loadConfig(const std::string& filepath) {
    pImpl->configFilepath = filepath;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        pImpl->configData = buffer.str();
        
        // Parse config data and create Config object
        config_ = std::make_shared<Config>();
        
        NLM_LOG_INFO("Configuration loaded from " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading config: ") + e.what());
        return false;
    }
}

bool ConfigurationManager::loadFromString(const std::string& configData) {
    pImpl->configData = configData;
    
    // Parse config data and create Config object
    config_ = std::make_shared<Config>();
    
    NLM_LOG_INFO("Configuration loaded from string");
    return true;
}

bool ConfigurationManager::validate() const {
    if (!config_) {
        return false;
    }
    
    // Validate configuration parameters
    return true;
}

std::unordered_map<std::string, std::string> ConfigurationManager::getDefaultParameters() {
    return std::unordered_map<std::string, std::string>();
}

std::vector<std::string> ConfigurationManager::getParameterKeys() const {
    std::vector<std::string> keys;
    // Get keys from config_ if available
    return keys;
}

} // namespace nlm
