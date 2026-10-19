#include "ValidationFramework.hpp"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

ValidationFramework::ValidationFramework() : totalValidations(0), successfulValidations(0), errorValidations(0) {}

bool ValidationFramework::initialize() {
    startTime = std::chrono::high_resolution_clock::now();
    totalValidations = 0;
    successfulValidations = 0;
    errorValidations = 0;
    
    // Add default validation rules
    addDefaultRules();
    
    return true;
}

void ValidationFramework::addDefaultRules() {
    // Rule 1: Neuron count validation
    ValidationRule neuronCountRule("NEURON_COUNT_001", "Neuron Count Validation",
                                   "Validates that neuron count is within reasonable limits");
    neuronCountRule.setRangeValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries,
           const ConfigParameter& param, const ConfigEntry& entry) {
            std::vector<ValidationResultDetail> results;
            
            // Convert to appropriate type
            if (auto val = entry.value.if_type<size_t>(); val.has_value()) {
                size_t neuronCount = *val;
                
                // Check if neuron count exceeds reasonable limits
                if (neuronCount > 1000000) {
                    results.emplace_back("neuron_count", "", ValidationLevel::Error,
                                       "Neuron count exceeds maximum limit of 1,000,000",
                                       "NEURON_COUNT_001", 100);
                } else if (neuronCount > 100000) {
                    results.emplace_back("neuron_count", "", ValidationLevel::Warning,
                                       "Neuron count is high, may impact performance",
                                       "NEURON_COUNT_001", 50);
                }
            }
            
            return results;
        }
    );
    engine.addRule(neuronCountRule);
    
    // Rule 2: Memory constraint validation
    ValidationRule memoryRule("MEMORY_002", "Memory Constraint Validation",
                             "Validates memory constraints and available resources");
    memoryRule.setCrossValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries) {
            std::vector<ValidationResultDetail> results;
            
            // Look for neuron count and memory-related parameters
            size_t neuronCount = 0;
            double memoryPerNeuron = 0.0001;  // 0.1 KB per neuron
            
            for (const auto& entry : entries) {
                if (entry.key == "neuron_count" && entry.value.if_type<size_t>()) {
                    neuronCount = *entry.value.if_type<size_t>();
                } else if (entry.key == "memory_per_neuron" && entry.value.if_type<double>()) {
                    memoryPerNeuron = *entry.value.if_type<double>();
                }
            }
            
            double estimatedMemory = neuronCount * memoryPerNeuron;
            
            if (estimatedMemory > 100.0) {  // 100 MB limit
                results.emplace_back("memory", "", ValidationLevel::Error,
                                   "Estimated memory usage exceeds 100 MB limit",
                                   "MEMORY_002", 100);
            } else if (estimatedMemory > 50.0) {
                results.emplace_back("memory", "", ValidationLevel::Warning,
                                   "Estimated memory usage is high",
                                   "MEMORY_002", 50);
            }
            
            return results;
        }
    );
    engine.addRule(memoryRule);
    
    // Rule 3: Plasticity parameter validation
    ValidationRule plasticityRule("PLASTICITY_003", "Plasticity Parameter Validation",
                                   "Validates plasticity parameters and their combinations");
    plasticityRule.setConditionalValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries,
           const ConfigParameter& param, const ConfigEntry& entry) {
            std::vector<ValidationResultDetail> results;
            
            // If plasticity is enabled, validate plasticity parameters
            if (entry.key == "enable_plasticity") {
                bool plasticityEnabled = false;
                if (entry.value.if_type<bool>()) {
                    plasticityEnabled = *entry.value.if_type<bool>();
                }
                
                if (plasticityEnabled) {
                    // Check for required plasticity parameters
                    bool hasSTDP = config.has("stdp_ltp_weight") &&
                                 config.has("stdp_ltd_weight") &&
                                 config.has("stdp_tau");
                    bool hasHebbian = config.has("hebbian_learning_rate");
                    
                    if (!hasSTDP && !hasHebbian) {
                        results.emplace_back("plasticity", "", ValidationLevel::Error,
                                           "Plasticity is enabled but no plasticity rules are configured",
                                           "PLASTICITY_003", 100);
                    } else {
                        if (hasSTDP) {
                            // Validate STDP parameters
                            if (auto ltp = config.get<double>("stdp_ltp_weight"); ltp.has_value() && *ltp <= 0) {
                                results.emplace_back("stdp_ltp_weight", "", ValidationLevel::Error,
                                                   "STDP LTP weight must be positive when plasticity is enabled",
                                                   "PLASTICITY_003", 80);
                            }
                        }
                        if (hasHebbian) {
                            // Validate Hebbian parameters
                            if (auto rate = config.get<double>("hebbian_learning_rate"); rate.has_value() && *rate <= 0) {
                                results.emplace_back("hebbian_learning_rate", "", ValidationLevel::Error,
                                                   "Hebbian learning rate must be positive when plasticity is enabled",
                                                   "PLASTICITY_003", 80);
                            }
                        }
                    }
                }
            }
            
            return results;
        }
    );
    engine.addRule(plasticityRule);
    
    // Rule 4: Configuration conflict detection
    ValidationRule conflictRule("CONFLICT_004", "Configuration Conflict Detection",
                                "Detects incompatible configuration combinations");
    conflictRule.setCrossValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries) {
            std::vector<ValidationResultDetail> results;
            
            // Check for conflicting parameters
            bool fastMode = false;
            bool highPrecision = false;
            
            for (const auto& entry : entries) {
                if (entry.key == "fast_mode" && entry.value.if_type<bool>()) {
                    fastMode = *entry.value.if_type<bool>();
                } else if (entry.key == "high_precision" && entry.value.if_type<bool>()) {
                    highPrecision = *entry.value.if_type<bool>();
                }
            }
            
            if (fastMode && highPrecision) {
                results.emplace_back("fast_mode", "", ValidationLevel::Error,
                                   "'Fast mode' and 'High precision' are incompatible configurations",
                                   "CONFLICT_004", 100);
            }
            
            // Check for memory/performance conflicts
            if (auto neuronCount = config.get<size_t>("neuron_count"); neuronCount.has_value() && *neuronCount > 10000) {
                if (auto timestep = config.get<double>("simulation_timestep"); timestep.has_value() && *timestep < 0.0001) {
                    results.emplace_back("neuron_count", "simulation_timestep", ValidationLevel::Warning,
                                       "Large neuron count with very small timestep may cause performance issues",
                                       "CONFLICT_004", 60);
                }
            }
            
            return results;
        }
    );
    engine.addRule(conflictRule);
    
    // Rule 5: Dynamic range validation
    ValidationRule rangeRule("RANGE_005", "Dynamic Range Validation",
                             "Validates dynamic ranges based on other parameters");
    rangeRule.setRangeValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries,
           const ConfigParameter& param, const ConfigEntry& entry) {
            std::vector<ValidationResultDetail> results;
            
            // Dynamic range validation based on system resources
            if (entry.key == "connection_probability" && entry.value.if_type<double>()) {
                double prob = *entry.value.if_type<double>();
                
                if (auto neuronCount = config.get<size_t>("neuron_count"); neuronCount.has_value()) {
                    size_t neurons = *neuronCount;
                    
                    // If neuron count is very large, connection probability should be adjusted
                    if (neurons > 100000 && prob > 0.5) {
                        results.emplace_back("connection_probability", "", ValidationLevel::Warning,
                                           "High connection probability with large neuron count may cause excessive connections",
                                           "RANGE_005", 70);
                    }
                }
            }
            
            return results;
        }
    );
    engine.addRule(rangeRule);
    
    // Rule 6: Parameter consistency validation
    ValidationRule consistencyRule("CONSISTENCY_006", "Parameter Consistency Validation",
                                    "Validates consistency between related parameters");
    consistencyRule.setCrossValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries) {
            std::vector<ValidationResultDetail> results;
            
            // Check that plasticity is only enabled when certain dependencies are met
            bool plasticityEnabled = false;
            for (const auto& entry : entries) {
                if (entry.key == "enable_plasticity" && entry.value.if_type<bool>()) {
                    plasticityEnabled = *entry.value.if_type<bool>();
                    break;
                }
            }
            
            if (plasticityEnabled) {
                // Check for required connectivity
                bool hasConnectivity = false;
                for (const auto& entry : entries) {
                    if (entry.key == "connection_probability" && entry.value.if_type<double>()) {
                        double prob = *entry.value.if_type<double>();
                        if (prob > 0.0) {
                            hasConnectivity = true;
                            break;
                        }
                    }
                }
                
                if (!hasConnectivity) {
                    results.emplace_back("enable_plasticity", "", ValidationLevel::Warning,
                                       "Plasticity enabled but no connectivity configured",
                                       "CONSISTENCY_006", 50);
                }
            }
            
            return results;
        }
    );
    engine.addRule(consistencyRule);
    
    // Rule 7: Resource constraint validation
    ValidationRule resourceRule("RESOURCE_007", "Resource Constraint Validation",
                                "Validates resource usage constraints");
    resourceRule.setCrossValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries) {
            std::vector<ValidationResultDetail> results;
            
            // Calculate estimated resource usage
            double totalResource = 0.0;
            
            for (const auto& entry : entries) {
                if (entry.key == "neuron_count" && entry.value.if_type<size_t>()) {
                    totalResource += static_cast<double>(*entry.value.if_type<size_t>());  // 1 unit per neuron
                } else if (entry.key == "synapse_count" && entry.value.if_type<size_t>()) {
                    totalResource += static_cast<double>(*entry.value.if_type<size_t>() * 10);  // 10 units per synapse
                } else if (entry.key == "population_size" && entry.value.if_type<size_t>()) {
                    totalResource += static_cast<double>(*entry.value.if_type<size_t>() * 0.1);  // 0.1 unit per population
                }
            }
            
            // Check against resource limits
            if (totalResource > 1000000) {  // 1M units limit
                results.emplace_back("total_resources", "", ValidationLevel::Error,
                                   "Total estimated resource usage exceeds system limits",
                                   "RESOURCE_007", 100);
            } else if (totalResource > 500000) {
                results.emplace_back("total_resources", "", ValidationLevel::Warning,
                                   "High resource usage may impact performance",
                                   "RESOURCE_007", 60);
            }
            
            return results;
        }
    );
    engine.addRule(resourceRule);
    
    // Rule 8: Integration validation
    ValidationRule integrationRule("INTEGRATION_008", "Integration Validation",
                                    "Validates integration with other systems");
    integrationRule.setCrossValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries) {
            std::vector<ValidationResultDetail> results;
            
            // Check for required integration parameters
            bool hasMemorySystem = false;
            bool hasPredictionSystem = false;
            bool hasCognitionSystem = false;
            
            for (const auto& entry : entries) {
                if (entry.key == "enable_memory_system" && entry.value.if_type<bool>()) {
                    if (*entry.value.if_type<bool>()) hasMemorySystem = true;
                } else if (entry.key == "enable_prediction_system" && entry.value.if_type<bool>()) {
                    if (*entry.value.if_type<bool>()) hasPredictionSystem = true;
                } else if (entry.key == "enable_cognition_system" && entry.value.if_type<bool>()) {
                    if (*entry.value.if_type<bool>()) hasCognitionSystem = true;
                }
            }
            
            // If all systems are enabled, check if integration is configured
            if (hasMemorySystem && hasPredictionSystem && hasCognitionSystem) {
                // Check for integration parameters
                bool hasIntegrationConfig = config.has("integration_mode") ||
                                           config.has("coordination_strategy");
                
                if (!hasIntegrationConfig) {
                    results.emplace_back("system_integration", "", ValidationLevel::Warning,
                                       "Multiple systems enabled but integration configuration is minimal",
                                       "INTEGRATION_008", 70);
                }
            }
            
            return results;
        }
    );
    engine.addRule(integrationRule);
    
    // Rule 9: Performance validation
    ValidationRule performanceRule("PERFORMANCE_009", "Performance Validation",
                                    "Validates performance-related parameters");
    performanceRule.setCrossValidator(
        [](const Config& config, const std::vector<ConfigEntry>& entries) {
            std::vector<ValidationResultDetail> results;
            
            // Check for performance-impacting combinations
            double timestep = 0.001;  // Default
            for (const auto& entry : entries) {
                if (entry.key == "simulation_timestep" && entry.value.if_type<double>()) {
                    timestep = *entry.value.if_type<double>();
                }
            }
            
            // Check for timestep too small for performance
            if (timestep < 0.0001) {
                results.emplace_back("simulation_timestep", "", ValidationLevel::Warning,
                                   "Very small timestep may cause performance issues",
                                   "PERFORMANCE_009", 60);
            }
            
            return results;
        }
    );
    engine.addRule(performanceRule);
}

void ValidationFramework::setConfig(const ValidationConfig& cfg) {
    config = cfg;
}

const ValidationConfig& ValidationFramework::getConfig() const {
    return config;
}

std::vector<ValidationResultDetail> ValidationFramework::validate(const Config& config,
                                                                 const ConfigSchema& schema) const {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Get all entries from config
    std::vector<ConfigEntry> entries;
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        // Try to get the value based on type
        if (auto val = config.get<int>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        } else if (auto val = config.get<double>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        } else if (auto val = config.get<bool>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        } else if (auto val = config.get<std::string>(key); val.has_value()) {
            entries.emplace_back(key, *val);
        }
    }
    
    // Validate with engine
    std::vector<ValidationResultDetail> results = engine.validate(config, entries);
    
    // Update timing
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Update results with timing information
    for (auto& result : results) {
        result.executionTime = duration.count();
    }
    
    return results;
}

std::vector<ValidationResultDetail> ValidationFramework::filterResultsByLevel(
    const std::vector<ValidationResultDetail>& results, ValidationLevel minLevel) const {
    std::vector<ValidationResultDetail> filtered;
    for (const auto& result : results) {
        if (result.level >= minLevel) {
            filtered.push_back(result);
        }
    }
    return filtered;
}

std::unordered_map<std::string, std::vector<ValidationResultDetail>> ValidationFramework::groupResultsByKey(
    const std::vector<ValidationResultDetail>& results) const {
    std::unordered_map<std::string, std::vector<ValidationResultDetail>> grouped;
    
    for (const auto& result : results) {
        grouped[result.key].push_back(result);
    }
    
    return grouped;
}

std::string ValidationFramework::generateReport(const std::vector<ValidationResultDetail>& results) const {
    std::ostringstream oss;
    
    oss << "=== Configuration Validation Report ===" << std::endl;
    oss << "Generated: " << getCurrentTimestamp() << std::endl;
    oss << "Total validations: " << totalValidations << std::endl;
    oss << "Successful validations: " << successfulValidations << std::endl;
    oss << "Error validations: " << errorValidations << std::endl;
    oss << "Validation engine status: " << (engine.isInitialized() ? "Initialized" : "Not initialized") << std::endl;
    oss << std::endl;
    
    // Summary by level
    int infoCount = 0, warningCount = 0, errorCount = 0, criticalCount = 0;
    for (const auto& result : results) {
        switch (result.level) {
            case ValidationLevel::Info: infoCount++; break;
            case ValidationLevel::Warning: warningCount++; break;
            case ValidationLevel::Error: errorCount++; break;
            case ValidationLevel::Critical: criticalCount++; break;
        }
    }
    
    oss << "=== Summary ===" << std::endl;
    oss << "  Info: " << infoCount << std::endl;
    oss << "  Warnings: " << warningCount << std::endl;
    oss << "  Errors: " << errorCount << std::endl;
    oss << "  Critical: " << criticalCount << std::endl;
    oss << std::endl;
    
    // Detailed results
    oss << "=== Detailed Results ===" << std::endl;
    for (const auto& result : results) {
        oss << result.toString() << std::endl;
    }
    
    return oss.str();
}

std::string ValidationFramework::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool ValidationFramework::saveResultsToFile(const std::string& filepath,
                                          const std::vector<ValidationResultDetail>& results) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateReport(results);
    file.close();
    return true;
}

bool ValidationFramework::loadRulesFromFile(const std::string& filepath) {
    // This is a simplified implementation
    // In practice, you would parse a JSON or other format containing validation rules
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, just initialize with default rules
    initialize();
    return true;
}

std::string ValidationFramework::getStatistics() const {
    std::ostringstream oss;
    
    oss << "=== Validation Framework Statistics ===" << std::endl;
    oss << "  Total validations: " << totalValidations << std::endl;
    oss << "  Successful validations: " << successfulValidations << std::endl;
    oss << "  Error validations: " << errorValidations << std::endl;
    oss << "  Engine statistics:" << std::endl;
    oss << engine.getStatistics() << std::endl;
    
    return oss.str();
}

} // namespace nlm
