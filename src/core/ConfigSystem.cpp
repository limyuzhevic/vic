#include "ConfigSystem.hpp"

namespace nlm {

bool ConfigSystem::initialize(const std::string& configFile, int argc, char** argv) {
    // Initialize manager if not already initialized
    if (!manager_) {
        manager_ = std::make_shared<ConfigManager>();
    }
    
    return manager_->initialize(configFile, argc, argv);
}

bool ConfigSystem::initializeWithBuilder(const ConfigBuilder& builder, 
                                        const std::map<std::string, ConfigValue>& customValues) {
    if (!manager_) {
        manager_ = std::make_shared<ConfigManager>();
    }
    
    // This would need to be implemented based on specific requirements
    // For now, just create config from builder
    auto config = builder.buildConfig(customValues);
    
    // Apply config to manager
    // manager_->setConfig(config);
    
    return true;
}

void ConfigSystem::initializeManager(const std::string& configFile, int argc, char** argv) {
    // This is called when the first initialize() is called
    // Manager is initialized in initialize() method
    if (!manager_) {
        manager_ = std::make_shared<ConfigManager>();
    }
}

} // namespace nlm