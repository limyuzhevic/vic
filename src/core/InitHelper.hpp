#pragma once

#include <string>
#include <memory>

namespace nlm {

class Config;

namespace InitHelper {
    // Template function to initialize systems with error handling
    template<typename T>
    bool initializeSystem(std::shared_ptr<Config> config, 
                         std::shared_ptr<T>& system,
                         const std::string& systemName);
    
    // Template function to configure systems from config
    template<typename T>
    void configureFromConfig(T* system, std::shared_ptr<Config> config,
                            const std::string& prefix = "");
    
    // Helper to create and initialize a system
    template<typename T>
    std::shared_ptr<T> createAndInitialize(std::shared_ptr<Config> config,
                                          const std::string& systemName);
    
    // Safe value getter with defaults
    template<typename T>
    T getConfigValue(std::shared_ptr<Config> config, 
                    const std::string& key,
                    const T& defaultValue,
                    bool logWarning = false);
}

} // namespace nlm
