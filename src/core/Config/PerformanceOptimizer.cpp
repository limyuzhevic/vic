#include "PerformanceOptimizer.hpp"
#include "../core/Config/AdvancedConfiguration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {
namespace PerformanceOptimizer {

    std::vector<PerformanceRecommendation> getPerformanceRecommendations(
        const ConfigurationManager& config,
        const std::string& useCase) {
        std::vector<PerformanceRecommendation> recommendations;
        
        // Get all configuration keys
        auto keys = config.getAllKeys();
        
        for (const auto& key : keys) {
            auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
            if (!value.has_value()) {
                continue;
            }
            
            std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
            
            // Analyze for performance optimization opportunities
            if (key == "neuron_count") {
                try {
                    size_t count = std::stoull(strValue);
                    if (count > 3000) {
                        PerformanceRecommendation rec;
                        rec.setting = key;
                        rec.currentValue = strValue;
                        rec.recommendedValue = "1500";
                        rec.expectedImprovement = 25.0;
                        rec.reason = "Large neuron count impacts simulation speed exponentially";
                        recommendations.push_back(rec);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            } else if (key == "max_simulation_steps") {
                try {
                    size_t steps = std::stoull(strValue);
                    if (steps > 15000) {
                        PerformanceRecommendation rec;
                        rec.setting = key;
                        rec.currentValue = strValue;
                        rec.recommendedValue = "8000";
                        rec.expectedImprovement = 20.0;
                        rec.reason = "Excessive simulation steps significantly increase runtime";
                        recommendations.push_back(rec);
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            } else if (key == "enable_development") {
                bool devEnabled = (strValue == "true" || strValue == "1");
                if (devEnabled) {
                    PerformanceRecommendation rec;
                    rec.setting = key;
                    rec.currentValue = strValue;
                    rec.recommendedValue = "false";
                    rec.expectedImprovement = 30.0;
                    rec.reason = "Development features add computational overhead for plasticity updates";
                    recommendations.push_back(rec);
                }
            } else if (key == "visualization_enabled") {
                bool vizEnabled = (strValue == "true" || strValue == "1");
                if (vizEnabled) {
                    PerformanceRecommendation rec;
                    rec.setting = key;
                    rec.currentValue = strValue;
                    rec.recommendedValue = "false";
                    rec.expectedImprovement = 15.0;
                    rec.reason = "Visualization consumes system resources and can slow down simulations";
                    recommendations.push_back(rec);
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
                    double ratio = static_cast<double>(newValue) / oldValue;
                    impact += std::log2(ratio) * 20.0;
                } catch (...) {
                    impact += 10.0;
                }
            } else if (change.first == "connection_probability") {
                try {
                    double newValue = std::stod(change.second);
                    double oldValue = 0.1;
                    impact += std::abs(newValue - oldValue) * 25.0;
                } catch (...) {
                    impact += 8.0;
                }
            } else if (change.first == "max_simulation_steps") {
                try {
                    size_t newValue = std::stoull(change.second);
                    size_t oldValue = 10000;
                    double ratio = static_cast<double>(newValue) / oldValue;
                    impact += std::log2(ratio) * 5.0;
                } catch (...) {
                    impact += 4.0;
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
        
        // Copy current settings
        auto keys = config.getAllKeys();
        for (const auto& key : keys) {
            auto value = config.getOptional<AdvancedConfiguration::ConfigValue>(key);
            if (value.has_value()) {
                std::string strValue = AdvancedConfiguration::ConfigUtils::toString(*value);
                optimizedSettings[key] = strValue;
            }
        }
        
        // Apply optimizations based on use case and targets
        if (useCase == "research") {
            // For research, maintain some flexibility but optimize for reasonable performance
            if (optimizedSettings.find("neuron_count") != optimizedSettings.end()) {
                try {
                    size_t currentCount = std::stoull(optimizedSettings["neuron_count"]);
                    if (currentCount > 2000) {
                        optimizedSettings["neuron_count"] = "1500";
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        } else if (useCase == "production") {
            // For production, prioritize stability and efficiency
            if (optimizedSettings.find("neuron_count") != optimizedSettings.end()) {
                try {
                    size_t currentCount = std::stoull(optimizedSettings["neuron_count"]);
                    if (currentCount > 500) {
                        optimizedSettings["neuron_count"] = "500";
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
            
            if (optimizedSettings.find("enable_development") != optimizedSettings.end()) {
                optimizedSettings["enable_development"] = "false";
            }
            
            if (optimizedSettings.find("visualization_enabled") != optimizedSettings.end()) {
                optimizedSettings["visualization_enabled"] = "false";
            }
        }
        
        return optimizedSettings;
    }

} // namespace PerformanceOptimizer

} // namespace nlm
