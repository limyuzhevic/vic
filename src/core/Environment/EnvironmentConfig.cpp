// Implementation of Environment-based Configuration System
//
// This file provides the implementation for NLM's environment variable configuration system.

#include "core/Environment/EnvironmentConfig.hpp"
#include <cstdlib>
#include <algorithm>

namespace nlm {

// EnvironmentConfigManager instance
EnvironmentConfigManager EnvironmentConfigManager::instance;

// Convenience functions implementation
void loadEnvironmentConfiguration(std::shared_ptr<Config> config) {
    EnvironmentConfigManager::instance.applyEnvironmentConfig(config);
}

void refreshEnvironmentVariables() {
    EnvironmentConfigManager::instance.loadEnvironmentVariables();
}

// Get environment variable
std::string getEnvironmentVariable(const std::string& varName) {
    return EnvironmentConfigManager::instance.getEnvVar(varName);
}

// Check if environment variable is set
bool isEnvironmentVariableSet(const std::string& varName) {
    return !EnvironmentConfigManager::instance.getEnvVar(varName).empty();
}

// List all environment variables
void listEnvironmentVariables() {
    NLM_LOG_INFO("=== Environment Variables ===");
    for (const auto& pair : EnvironmentConfigManager::instance.envVars) {
        NLM_LOG_INFO(pair.first + " = " + pair.second);
    }
}

} // namespace nlm