#include "EnvVarManager.hpp"
#include "../core/Config/AdvancedConfiguration.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {
namespace EnvVarManager {

    bool setSystemEnvironmentVariable(const std::string& name, const std::string& value) {
        try {
            // Use putenv or setenv based on platform
            #ifdef _WIN32
            // Windows API
            if (SetEnvironmentVariableA(name.c_str(), value.c_str())) {
                NLM_LOG_INFO(std::string("Windows environment variable set: ") + name + " = " + value, 
                           "EnvVarManager::setSystemEnvironmentVariable");
                return true;
            }
            #else
            // POSIX systems
            if (setenv(name.c_str(), value.c_str(), 1) == 0) {
                NLM_LOG_INFO(std::string("Environment variable set: ") + name + " = " + value, 
                           "EnvVarManager::setSystemEnvironmentVariable");
                return true;
            }
            #endif
            
            NLM_LOG_ERROR(std::string("Failed to set environment variable: ") + name, 
                         "EnvVarManager::setSystemEnvironmentVariable");
            return false;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error setting environment variable: ") + e.what(), 
                         "EnvVarManager::setSystemEnvironmentVariable");
            return false;
        }
    }

    std::string getSystemEnvironmentVariable(const std::string& name) {
        try {
            #ifdef _WIN32
            // Windows API
            char buffer[32767];
            DWORD result = GetEnvironmentVariableA(name.c_str(), buffer, sizeof(buffer));
            if (result > 0) {
                return std::string(buffer);
            }
            #else
            // POSIX systems
            const char* value = std::getenv(name.c_str());
            if (value) {
                return std::string(value);
            }
            #endif
            
            return "";
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting environment variable: ") + e.what(), 
                         "EnvVarManager::getSystemEnvironmentVariable");
            return "";
        }
    }

    bool isSystemEnvironmentVariableSet(const std::string& name) {
        try {
            return !getSystemEnvironmentVariable(name).empty();
        } catch (const std::exception& e) {
            return false;
        }
    }

    bool unsetSystemEnvironmentVariable(const std::string& name) {
        try {
            #ifdef _WIN32
            // Windows API
            if (SetEnvironmentVariableA(name.c_str(), nullptr)) {
                NLM_LOG_INFO(std::string("Windows environment variable unset: ") + name, 
                           "EnvVarManager::unsetSystemEnvironmentVariable");
                return true;
            }
            #else
            // POSIX systems
            if (unsetenv(name.c_str()) == 0) {
                NLM_LOG_INFO(std::string("Environment variable unset: ") + name, 
                           "EnvVarManager::unsetSystemEnvironmentVariable");
                return true;
            }
            #endif
            
            NLM_LOG_ERROR(std::string("Failed to unset environment variable: ") + name, 
                         "EnvVarManager::unsetSystemEnvironmentVariable");
            return false;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error unsetting environment variable: ") + e.what(), 
                         "EnvVarManager::unsetSystemEnvironmentVariable");
            return false;
        }
    }

    bool validateEnvironmentVariableValue(const std::string& name, 
                                         const std::string& value,
                                         const std::string& validationPattern) {
        if (validationPattern.empty()) {
            return true; // No validation required
        }
        
        try {
            // Escape regex special characters for basic validation
            std::string escapedPattern = validationPattern;
            
            // Basic validation checks
            if (validationPattern == "^[0-9]+$") {
                // Check if value is all digits
                return std::all_of(value.begin(), value.end(), ::isdigit);
            } else if (validationPattern == "^([0-9]*[.])?[0-9]+$") {
                // Check if value is a valid number (integer or float)
                try {
                    double val = std::stod(value);
                    return true;
                } catch (...) {
                    return false;
                }
            } else if (validationPattern == "^(true|false|1|0)$") {
                // Check if value is a boolean
                std::string lowerValue = value;
                std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
                return lowerValue == "true" || lowerValue == "false" || 
                       lowerValue == "1" || lowerValue == "0";
            } else if (validationPattern == "^(DEBUG|INFO|WARN|ERROR)$") {
                // Check if value is a valid log level
                std::string upperValue = value;
                std::transform(upperValue.begin(), upperValue.end(), upperValue.begin(), ::toupper);
                return upperValue == "DEBUG" || upperValue == "INFO" || 
                       upperValue == "WARN" || upperValue == "ERROR";
            } else {
                // Try to compile and use the regex pattern
                std::regex pattern(validationPattern);
                return std::regex_match(value, pattern);
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error validating environment variable pattern: ") + e.what(), 
                         "EnvVarManager::validateEnvironmentVariableValue");
            return false;
        }
    }

    std::string getEnvironmentVariableDocumentation(const std::string& name) {
        try {
            // Get environment variable from advanced configuration
            auto& advancedConfig = AdvancedConfiguration::getInstance();
            
            // Try to get the environment variable
            std::string value;
            const char* envValue = std::getenv(name.c_str());
            if (envValue) {
                value = std::string(envValue);
            }
            
            std::stringstream doc;
            doc << "=== Environment Variable Documentation ===" << std::endl;
            doc << "Name: " << name << std::endl;
            doc << "System Value: " << value << std::endl;
            doc << "System Set: " << (isSystemEnvironmentVariableSet(name) ? "Yes" : "No") << std::endl;
            
            // Check if this is a defined environment variable
            auto envVars = advancedConfig.getEnvironmentVariableDefinitions();
            for (const auto& envVar : envVars) {
                if (envVar.name == name) {
                    doc << "Description: " << envVar.description << std::endl;
                    doc << "Config Key: " << envVar.configKey << std::endl;
                    doc << "Required: " << (envVar.required ? "Yes" : "No") << std::endl;
                    doc << "Current Value: " << envVar.value << std::endl;
                    if (!envVar.validationPattern.empty()) {
                        doc << "Validation Pattern: " << envVar.validationPattern << std::endl;
                    }
                    break;
                }
            }
            
            return doc.str();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting environment variable documentation: ") + e.what(), 
                         "EnvVarManager::getEnvironmentVariableDocumentation");
            return "Error retrieving documentation: " + std::string(e.what());
        }
    }

    std::string getAllEnvironmentVariablesDocumentation() {
        try {
            std::stringstream doc;
            doc << "=== All Environment Variables Documentation ===" << std::endl;
            doc << std::endl;
            
            // Get all environment variable names
            std::vector<std::string> envNames;
            
            // Try to get some common environment variables
            std::vector<std::string> commonVars = {
                "NLM_RANDOM_SEED",
                "NLM_NEURON_COUNT",
                "NLM_CONNECTION_PROBABILITY",
                "NLM_ENABLE_DEVELOPMENT",
                "NLM_LOG_LEVEL",
                "PATH",
                "HOME",
                "USER"
            };
            
            for (const auto& varName : commonVars) {
                if (isSystemEnvironmentVariableSet(varName)) {
                    envNames.push_back(varName);
                }
            }
            
            for (const auto& varName : envNames) {
                doc << "---" << std::endl;
                doc << getEnvironmentVariableDocumentation(varName);
                doc << std::endl;
            }
            
            if (envNames.empty()) {
                doc << "No system environment variables found" << std::endl;
            }
            
            return doc.str();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting all environment variables documentation: ") + e.what(), 
                         "EnvVarManager::getAllEnvironmentVariablesDocumentation");
            return "Error retrieving documentation: " + std::string(e.what());
        }
    }

} // namespace EnvVarManager

// PerformanceOptimizer implementation
namespace PerformanceOptimizer {

    std::vector<PerformanceRecommendation> getPerformanceRecommendations(
        const ConfigurationManager& config,
        const std::string& useCase) {
        std::vector<PerformanceRecommendation> recommendations;
        
        // Get current configuration values
        auto keys = config.getAllKeys();
        
        for (const auto& key : keys) {
            auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
            if (!value.has_value()) {
                continue;
            }
            
            std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
            
            // Analyze each setting for performance impact
            if (key == "neuron_count") {
                try {
                    size_t count = std::stoull(strValue);
                    if (count > 5000) {
                        PerformanceRecommendation rec;
                        rec.setting = key;
                        rec.currentValue = strValue;
                        rec.recommendedValue = "2000";
                        rec.expectedImprovement = 20.0;
                        rec.reason = "Large neuron count significantly impacts simulation speed and memory usage";
                        recommendations.push_back(rec);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            } else if (key == "max_simulation_steps") {
                try {
                    size_t steps = std::stoull(strValue);
                    if (steps > 20000) {
                        PerformanceRecommendation rec;
                        rec.setting = key;
                        rec.currentValue = strValue;
                        rec.recommendedValue = "10000";
                        rec.expectedImprovement = 15.0;
                        rec.reason = "Excessive simulation steps slow down completion";
                        recommendations.push_back(rec);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
        
        return recommendations;
    }

    double estimatePerformanceImpact(const std::map<std::string, std::string>& changes,
                                     const std::string& useCase) {
        double impact = 0.0;
        
        for (const auto& change : changes) {
            if (change.first == "neuron_count") {
                try {
                    size_t newValue = std::stoull(change.second);
                    size_t oldValue = 1000;
                    impact += std::log2(static_cast<double>(newValue) / oldValue) * 15.0;
                } catch (...) {
                    impact += 8.0;
                }
            } else if (change.first == "connection_probability") {
                try {
                    double newValue = std::stod(change.second);
                    double oldValue = 0.1;
                    impact += std::abs(newValue - oldValue) * 20.0;
                } catch (...) {
                    impact += 5.0;
                }
            } else if (change.first == "max_simulation_steps") {
                try {
                    size_t newValue = std::stoull(change.second);
                    size_t oldValue = 10000;
                    impact += std::log2(static_cast<double>(newValue) / oldValue) * 2.0;
                } catch (...) {
                    impact += 3.0;
                }
            }
        }
        
        return std::max(0.0, impact);
    }

    std::map<std::string, std::string> optimizeForPerformance(
        const ConfigurationManager& config,
        const std::string& useCase,
        double targetUpdateTime,
        double targetMemoryUsage) {
        std::map<std::string, std::string> optimizedSettings;
        
        // Get current settings
        auto keys = config.getAllKeys();
        for (const auto& key : keys) {
            auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
            if (value.has_value()) {
                std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
                optimizedSettings[key] = strValue;
            }
        }
        
        // Apply optimizations based on target constraints
        if (targetUpdateTime > 0.0) {
            if (optimizedSettings.find("simulation_timestep") != optimizedSettings.end()) {
                try {
                    double currentStep = std::stod(optimizedSettings["simulation_timestep"]);
                    if (currentStep > targetUpdateTime) {
                        optimizedSettings["simulation_timestep"] = std::to_string(targetUpdateTime);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
        
        if (targetMemoryUsage > 0.0) {
            if (optimizedSettings.find("neuron_count") != optimizedSettings.end()) {
                try {
                    size_t currentCount = std::stoull(optimizedSettings["neuron_count"]);
                    if (currentCount > targetMemoryUsage * 100) { // Rough estimate
                        optimizedSettings["neuron_count"] = std::to_string(static_cast<size_t>(targetMemoryUsage * 50));
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
        
        return optimizedSettings;
    }

} // namespace PerformanceOptimizer

} // namespace nlm
