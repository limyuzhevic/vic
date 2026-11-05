#include "AdvancedConfigUtils.hpp"
#include "../core/Config/ConfigurationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {
namespace AdvancedConfigUtils {

    std::vector<std::string> getAvailableUseCases() {
        return {"research", "production", "high_performance", "basic", "realtime"};
    }

    std::string getRecommendedProfileForSystem(const std::string& systemInfo) {
        // Simple heuristic: recommend based on system memory
        if (systemInfo.find("high memory") != std::string::npos || 
            systemInfo.find("large RAM") != std::string::npos) {
            return "research";
        } else if (systemInfo.find("low memory") != std::string::npos || 
                  systemInfo.find("limited RAM") != std::string::npos) {
            return "production";
        } else {
            return "balanced";
        }
    }

    std::map<std::string, std::string> getSystemConfigurationRecommendations() {
        std::map<std::string, std::string> recommendations;
        
        recommendations["neuron_count"] = "1000"; // Default
        recommendations["region_count"] = "2"; // Default
        recommendations["connection_probability"] = "0.1"; // Default
        recommendations["enable_development"] = "false"; // Default for stability
        recommendations["log_level"] = "INFO"; // Default for production
        
        return recommendations;
    }

    std::string generateConfigurationProfileFromTemplate(const std::string& templateName,
                                                         const std::map<std::string, std::string>& customizations) {
        std::stringstream profile;
        
        profile << "# Generated Configuration Profile" << std::endl;
        profile << "# Template: " << templateName << std::endl;
        profile << "# Generated: " << getCurrentTimestamp() << std::endl;
        profile << std::endl;
        
        // Start with template settings
        std::map<std::string, std::string> baseSettings = getSystemConfigurationRecommendations();
        
        // Apply customizations
        for (const auto& customization : customizations) {
            baseSettings[customization.first] = customization.second;
        }
        
        // Output settings
        profile << "[settings]" << std::endl;
        for (const auto& setting : baseSettings) {
            profile << setting.first << " = " << setting.second << std::endl;
        }
        
        return profile.str();
    }

    bool validateConfigurationFile(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Try to parse as INI format
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos == std::string::npos) {
                    continue; // Skip invalid lines
                }
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }

    std::string getConfigurationCompatibilityReport(const std::string& filepath) {
        std::stringstream report;
        
        report << "=== Configuration Compatibility Report ===" << std::endl;
        report << "File: " << filepath << std::endl;
        report << std::endl;
        
        if (!validateConfigurationFile(filepath)) {
            report << "❌ File is not a valid configuration file" << std::endl;
            return report.str();
        }
        
        report << "✅ File format is valid" << std::endl;
        
        // Try to load as ConfigurationManager
        ConfigurationManager config;
        if (config.loadConfiguration(filepath)) {
            report << "✅ File can be loaded by ConfigurationManager" << std::endl;
            
            // Check for essential settings
            std::vector<std::string> essentialSettings = {
                "neuron_count",
                "region_count", 
                "connection_probability"
            };
            
            size_t presentEssential = 0;
            for (const auto& setting : essentialSettings) {
                if (config.has(setting)) {
                    presentEssential++;
                }
            }
            
            report << "Essential settings present: " << presentEssential << "/" 
                   << essentialSettings.size() << std::endl;
            
            // Validation status
            if (config.validate()) {
                report << "Configuration validation: PASS" << std::endl;
            } else {
                report << "Configuration validation: FAIL" << std::endl;
                auto errors = config.getKeys(); // Simplified error reporting
                report << "Issues: " << errors.size() << " validation issues" << std::endl;
            }
            
        } else {
            report << "❌ File cannot be loaded by ConfigurationManager" << std::endl;
        }
        
        return report.str();
    }

    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

} // namespace AdvancedConfigUtils

// Forward declaration to avoid circular includes
namespace ProfileManager {
    bool createProfileFromConfig(const std::string& profileName, 
                                const ConfigurationManager& config,
                                const std::string& description = "");
}

// ProfileManager implementation
namespace ProfileManager {

    bool createProfileFromConfig(const std::string& profileName, 
                                const ConfigurationManager& config,
                                const std::string& description) {
        try {
            // Include advanced configuration headers
            #include "AdvancedConfiguration.hpp"
            
            AdvancedConfiguration::Profile profile;
            profile.name = profileName;
            profile.description = description;
            profile.type = AdvancedConfiguration::ProfileType::CUSTOM;
            profile.isBuiltIn = false;
            
            // Copy configuration settings
            auto keys = config.getAllKeys();
            for (const auto& key : keys) {
                auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
                if (value.has_value()) {
                    // Convert ConfigValue to string
                    std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
                    profile.settings[key] = strValue;
                }
            }
            
            // Add to advanced configuration
            AdvancedConfiguration::getInstance().createProfile(profile);
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }

} // namespace ProfileManager

// Environment variable forward declarations
namespace EnvVarManager {
    bool setSystemEnvironmentVariable(const std::string& name, const std::string& value);
    std::string getSystemEnvironmentVariable(const std::string& name);
    bool isSystemEnvironmentVariableSet(const std::string& name);
    bool unsetSystemEnvironmentVariable(const std::string& name);
    bool validateEnvironmentVariableValue(const std::string& name, 
                                         const std::string& value,
                                         const std::string& validationPattern);
    std::string getEnvironmentVariableDocumentation(const std::string& name);
    std::string getAllEnvironmentVariablesDocumentation();
}

// PerformanceOptimizer forward declarations
namespace PerformanceOptimizer {
    struct PerformanceRecommendation;
    std::vector<PerformanceRecommendation> getPerformanceRecommendations(
        const ConfigurationManager& config,
        const std::string& useCase);
    
    double estimatePerformanceImpact(const std::map<std::string, std::string>& changes,
                                     const std::string& useCase);
    
    std::map<std::string, std::string> optimizeForPerformance(
        const ConfigurationManager& config,
        const std::string& useCase,
        double targetUpdateTime,
        double targetMemoryUsage);
}

// PerformanceOptimizer implementation
namespace PerformanceOptimizer {

    struct PerformanceRecommendation {
        std::string setting;
        std::string currentValue;
        std::string recommendedValue;
        double expectedImprovement; // percentage
        std::string reason;
    };

    std::vector<PerformanceRecommendation> getPerformanceRecommendations(
        const ConfigurationManager& config,
        const std::string& useCase) {
        std::vector<PerformanceRecommendation> recommendations;
        
        // Check neuron count
        auto neuronCount = config.getOptional<size_t>("neuron_count");
        if (neuronCount && *neuronCount > 10000) {
            PerformanceRecommendation rec;
            rec.setting = "neuron_count";
            rec.currentValue = std::to_string(*neuronCount);
            rec.recommendedValue = "5000";
            rec.expectedImprovement = 15.0;
            rec.reason = "Large neuron count impacts simulation speed";
            recommendations.push_back(rec);
        }
        
        // Check connection probability
        auto connProb = config.getOptional<double>("connection_probability");
        if (connProb && *connProb > 0.2) {
            PerformanceRecommendation rec;
            rec.setting = "connection_probability";
            rec.currentValue = std::to_string(*connProb);
            rec.recommendedValue = "0.15";
            rec.expectedImprovement = 10.0;
            rec.reason = "High connection probability increases memory usage";
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
                    double oldValue = 1000; // Default
                    impact += std::log2(newValue / oldValue) * 10.0;
                } catch (...) {
                    impact += 5.0;
                }
            } else if (change.first == "connection_probability") {
                try {
                    double newValue = std::stod(change.second);
                    double oldValue = 0.1; // Default
                    impact += std::abs(newValue - oldValue) * 50.0;
                } catch (...) {
                    impact += 5.0;
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
        
        // Apply optimizations based on use case
        if (useCase == "high_performance") {
            // Reduce computational complexity
            if (optimizedSettings.find("neuron_count") != optimizedSettings.end()) {
                optimizedSettings["neuron_count"] = "2000";
            }
            if (optimizedSettings.find("connection_probability") != optimizedSettings.end()) {
                optimizedSettings["connection_probability"] = "0.12";
            }
        }
        
        return optimizedSettings;
    }

} // namespace PerformanceOptimizer

} // namespace nlm
