#include "SystemInitializer.hpp"
#include "LoggerHelper.hpp"

namespace nlm {

// InitHelper template implementations

template<typename T>
bool InitHelper::initializeSystem(std::shared_ptr<Config> config, 
                                  std::shared_ptr<T>& system,
                                  const std::string& systemName) {
    LoggerHelper::info("Initializing system: " + systemName);
    
    try {
        system = std::make_shared<T>();
        
        if (!system->initialize(config)) {
            LoggerHelper::error("Failed to initialize system: " + systemName);
            return false;
        }
        
        LoggerHelper::info("Successfully initialized system: " + systemName);
        return true;
    } catch (const std::exception& e) {
        LoggerHelper::error("Exception initializing system " + systemName + ": " + std::string(e.what()));
        return false;
    }
}

template<typename T>
void InitHelper::configureFromConfig(T* system, std::shared_ptr<Config> config,
                                    const std::string& prefix) {
    if (!system) {
        LoggerHelper::warning("Cannot configure null system");
        return;
    }
    
    LoggerHelper::info("Configuring system from config: " + prefix);
    
    // In a real implementation, this would configure the system based on config values
}

template<typename T>
std::shared_ptr<T> InitHelper::createAndInitialize(std::shared_ptr<Config> config,
                                                 const std::string& systemName) {
    std::shared_ptr<T> system = std::make_shared<T>();
    
    if (!initializeSystem(config, system, systemName)) {
        LoggerHelper::error("Failed to create and initialize system: " + systemName);
        return nullptr;
    }
    
    return system;
}

template<typename T>
T InitHelper::getConfigValue(std::shared_ptr<Config> config, 
                           const std::string& key,
                           const T& defaultValue,
                           bool logWarning) {
    if (config->has(key)) {
        return config->get<T>(key).value_or(defaultValue);
    } else {
        if (logWarning) {
            LoggerHelper::warning("Using default value for config key: " + key);
        }
        return defaultValue;
    }
}

// Explicit template instantiations for common types

} // namespace nlm
