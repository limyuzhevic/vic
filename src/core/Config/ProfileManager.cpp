#include "ProfileManager.hpp"
#include "../core/Config/AdvancedConfiguration.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {
namespace ProfileManager {

    bool createProfileFromConfig(const std::string& profileName, 
                                const ConfigurationManager& config,
                                const std::string& description) {
        try {
            // Access the advanced configuration system
            auto& advancedConfig = AdvancedConfiguration::getInstance();
            
            AdvancedConfiguration::Profile profile;
            profile.name = profileName;
            profile.description = description;
            profile.type = AdvancedConfiguration::ProfileType::CUSTOM;
            profile.isBuiltIn = false;
            
            // Copy all configuration settings
            auto keys = config.getAllKeys();
            for (const auto& key : keys) {
                auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
                if (value.has_value()) {
                    std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
                    profile.settings[key] = strValue;
                }
            }
            
            // Add profile to advanced configuration
            if (advancedConfig.createProfile(profile)) {
                NLM_LOG_INFO(std::string("Profile created from config: ") + profileName, 
                           "ProfileManager::createProfileFromConfig");
                return true;
            }
            
            return false;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error creating profile from config: ") + e.what(), 
                         "ProfileManager::createProfileFromConfig");
            return false;
        }
    }

} // namespace ProfileManager

// Environment variable implementations
namespace EnvVarManager {

    bool setSystemEnvironmentVariable(const std::string& name, const std::string& value) {
        try {
            // Set environment variable using system call
            if (setenv(name.c_str(), value.c_str(), 1) != 0) {
                NLM_LOG_ERROR(std::string("Failed to set environment variable: ") + name, 
                             "EnvVarManager::setSystemEnvironmentVariable");
                return false;
            }
            
            NLM_LOG_INFO(std::string("Environment variable set: ") + name + " = " + value, 
                        "EnvVarManager::setSystemEnvironmentVariable");
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error setting environment variable: ") + e.what(), 
                         "EnvVarManager::setSystemEnvironmentVariable");
            return false;
        }
    }

    std::string getSystemEnvironmentVariable(const std::string& name) {
        try {
            const char* value = std::getenv(name.c_str());
            if (value) {
                return std::string(value);
            }
            
            return "";
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting environment variable: ") + e.what(), 
                         "EnvVarManager::getSystemEnvironmentVariable");
            return "";
        }
    }

    bool isSystemEnvironmentVariableSet(const std::string& name) {
        try {
            return std::getenv(name.c_str()) != nullptr;
        } catch (const std::exception& e) {
            return false;
        }
    }

    bool unsetSystemEnvironmentVariable(const std::string& name) {
        try {
            if (unsetenv(name.c_str()) != 0) {
                NLM_LOG_ERROR(std::string("Failed to unset environment variable: ") + name, 
                             "EnvVarManager::unsetSystemEnvironmentVariable");
                return false;
            }
            
            NLM_LOG_INFO(std::string("Environment variable unset: ") + name, 
                        "EnvVarManager::unsetSystemEnvironmentVariable");
            
            return true;
            
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
            std::regex pattern(validationPattern);
            return std::regex_match(value, pattern);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error validating environment variable pattern: ") + e.what(), 
                         "EnvVarManager::validateEnvironmentVariableValue");
            return false;
        }
    }

    std::string getEnvironmentVariableDocumentation(const std::string& name) {
        try {
            // Access the advanced configuration system to get environment variable info
            auto& advancedConfig = AdvancedConfiguration::getInstance();
            
            // Get environment variable definitions
            auto envVars = advancedConfig.getEnvironmentVariableDefinitions();
            
            for (const auto& envVar : envVars) {
                if (envVar.name == name) {
                    std::stringstream doc;
                    doc << "=== Environment Variable Documentation ===" << std::endl;
                    doc << "Name: " << envVar.name << std::endl;
                    doc << "Description: " << envVar.description << std::endl;
                    doc << "Config Key: " << envVar.configKey << std::endl;
                    doc << "Required: " << (envVar.required ? "Yes" : "No") << std::endl;
                    doc << "Current Value: " << envVar.value << std::endl;
                    if (!envVar.validationPattern.empty()) {
                        doc << "Validation Pattern: " << envVar.validationPattern << std::endl;
                    }
                    doc << "System Value: " << getSystemEnvironmentVariable(name) << std::endl;
                    doc << "System Set: " << (isSystemEnvironmentVariableSet(name) ? "Yes" : "No") << std::endl;
                    
                    return doc.str();
                }
            }
            
            return "Environment variable not defined: " + name;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting environment variable documentation: ") + e.what(), 
                         "EnvVarManager::getEnvironmentVariableDocumentation");
            return "Error retrieving documentation: " + std::string(e.what());
        }
    }

    std::string getAllEnvironmentVariablesDocumentation() {
        try {
            // Access the advanced configuration system
            auto& advancedConfig = AdvancedConfiguration::getInstance();
            
            // Get environment variable definitions
            auto envVars = advancedConfig.getEnvironmentVariableDefinitions();
            
            std::stringstream doc;
            doc << "=== All Environment Variables Documentation ===" << std::endl;
            doc << std::endl;
            
            for (const auto& envVar : envVars) {
                doc << "---" << std::endl;
                doc << "Name: " << envVar.name << std::endl;
                doc << "Description: " << envVar.description << std::endl;
                doc << "Config Key: " << envVar.configKey << std::endl;
                doc << "Required: " << (envVar.required ? "Yes" : "No") << std::endl;
                doc << "Current Value: " << envVar.value << std::endl;
                if (!envVar.validationPattern.empty()) {
                    doc << "Validation Pattern: " << envVar.validationPattern << std::endl;
                }
                doc << "System Value: " << getSystemEnvironmentVariable(envVar.name) << std::endl;
                doc << "System Set: " << (isSystemEnvironmentVariableSet(envVar.name) ? "Yes" : "No") << std::endl;
                doc << std::endl;
            }
            
            return doc.str();
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error getting all environment variables documentation: ") + e.what(), 
                         "EnvVarManager::getAllEnvironmentVariablesDocumentation");
            return "Error retrieving documentation: " + std::string(e.what());
        }
    }

} // namespace EnvVarManager

// PerformanceOptimizer implementations
namespace PerformanceOptimizer {

    std::vector<PerformanceRecommendation> getPerformanceRecommendations(
        const ConfigurationManager& config,
        const std::string& useCase) {
        std::vector<PerformanceRecommendation> recommendations;
        
        // Basic recommendations based on configuration
        auto keys = config.getAllKeys();
        
        for (const auto& key : keys) {
            auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
            if (!value.has_value()) {
                continue;
            }
            
            // Convert to string for analysis
            std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
            
            // Check for performance issues
            if (key == "neuron_count") {
                try {
                    size_t count = std::stoull(strValue);
                    if (count > 10000) {
                        PerformanceRecommendation rec;
                        rec.setting = key;
                        rec.currentValue = strValue;
                        rec.recommendedValue = "5000";
                        rec.expectedImprovement = 25.0;
                        rec.reason = "Large neuron count significantly impacts simulation performance";
                        recommendations.push_back(rec);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            } else if (key == "connection_probability") {
                try {
                    double prob = std::stod(strValue);
                    if (prob > 0.3) {
                        PerformanceRecommendation rec;
                        rec.setting = key;
                        rec.currentValue = strValue;
                        rec.recommendedValue = "0.15";
                        rec.expectedImprovement = 20.0;
                        rec.reason = "High connection probability increases memory and computation";
                        recommendations.push_back(rec);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
        
        // Use case specific recommendations
        if (useCase == "research") {
            PerformanceRecommendation rec;
            rec.setting = "log_level";
            rec.currentValue = "INFO"; // Default
            rec.recommendedValue = "WARN";
            rec.expectedImprovement = 15.0;
            rec.reason = "Debug logging in research can significantly slow down simulations";
            recommendations.push_back(rec);
        } else if (useCase == "production") {
            PerformanceRecommendation rec;
            rec.setting = "visualization_enabled";
            rec.currentValue = "false"; // Default
            rec.recommendedValue = "false";
            rec.expectedImprovement = 30.0;
            rec.reason = "Visualization disabled for optimal production performance";
            recommendations.push_back(rec);
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
                    size_t oldValue = 1000; // Default
                    impact += std::log2(static_cast<double>(newValue) / oldValue) * 20.0;
                } catch (...) {
                    impact += 10.0;
                }
            } else if (change.first == "connection_probability") {
                try {
                    double newValue = std::stod(change.second);
                    double oldValue = 0.1; // Default
                    impact += std::abs(newValue - oldValue) * 30.0;
                } catch (...) {
                    impact += 5.0;
                }
            } else if (change.first == "max_simulation_steps") {
                try {
                    size_t newValue = std::stoull(change.second);
                    size_t oldValue = 10000; // Default
                    impact += std::log2(static_cast<double>(newValue) / oldValue) * 5.0;
                } catch (...) {
                    impact += 3.0;
                }
            }
        }
        
        // Apply use case modifier
        if (useCase == "research") {
            impact *= 1.2; // Research is more computationally intensive
        } else if (useCase == "production") {
            impact *= 0.8; // Production is more performance-sensitive
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
        
        // Apply optimizations based on use case
        if (useCase == "high_performance") {
            // Optimize for speed and efficiency
            if (optimizedSettings.find("neuron_count") != optimizedSettings.end()) {
                optimizedSettings["neuron_count"] = "2000";
            }
            if (optimizedSettings.find("connection_probability") != optimizedSettings.end()) {
                optimizedSettings["connection_probability"] = "0.12";
            }
            if (optimizedSettings.find("enable_development") != optimizedSettings.end()) {
                optimizedSettings["enable_development"] = "false";
            }
            if (optimizedSettings.find("visualization_enabled") != optimizedSettings.end()) {
                optimizedSettings["visualization_enabled"] = "false";
            }
        } else if (useCase == "research") {
            // Optimize for accuracy and completeness
            if (optimizedSettings.find("neuron_count") != optimizedSettings.end()) {
                optimizedSettings["neuron_count"] = "5000";
            }
            if (optimizedSettings.find("connection_probability") != optimizedSettings.end()) {
                optimizedSettings["connection_probability"] = "0.15";
            }
            if (optimizedSettings.find("enable_development") != optimizedSettings.end()) {
                optimizedSettings["enable_development"] = "true";
            }
            if (optimizedSettings.find("visualization_enabled") != optimizedSettings.end()) {
                optimizedSettings["visualization_enabled"] = "true";
            }
        }
        
        // Check memory usage target
        auto memoryKey = optimizedSettings.find("memory_limit");
        if (memoryKey != optimizedSettings.end()) {
            try {
                double currentMemory = std::stod(memoryKey->second);
                if (currentMemory > targetMemoryUsage) {
                    optimizedSettings["memory_limit"] = std::to_string(targetMemoryUsage);
                }
            } catch (...) {
                // Ignore parsing errors
            }
        }
        
        return optimizedSettings;
    }

} // namespace PerformanceOptimizer

} // namespace nlm
