#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <chrono>
#include <iostream>
#include <sstream>

namespace nlm {

// Validation result levels
enum class ValidationLevel {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3
};

// Validation action types
enum class ValidationAction {
    Allow,
    Warn,
    Error,
    Skip
};

// Detailed validation result with context
struct ValidationResultDetail {
    std::string key;                    ///< Configuration key being validated
    std::string subKey;                 ///< Sub-key for nested parameters (optional)
    ValidationLevel level;              ///< Severity level
    ValidationAction action;            ///< Action to take
    std::string message;                ///< Detailed error/warning message
    std::string ruleId;                 ///< ID of the validation rule that triggered this
    int priority;                       ///< Priority of this validation (higher = more important)
    std::string context;                ///< Additional context information
    bool isCrossParameter;              ///< Whether this is a cross-parameter validation
    bool isConditional;                  ///< Whether this is a conditional validation
    double executionTime;               ///< Time taken for this validation (milliseconds)
    
    ValidationResultDetail()
        : level(ValidationLevel::Info), action(ValidationAction::Allow),
          priority(0), executionTime(0.0), isCrossParameter(false), isConditional(false) {}
    
    ValidationResultDetail(const std::string& k, const std::string& sub, 
                          ValidationLevel lvl, const std::string& msg,
                          const std::string& ruleId = "", int priority = 0,
                          const std::string& ctx = "")
        : key(k), subKey(sub), level(lvl), action(ValidationAction::Error),  // Default to error
          message(msg), ruleId(ruleId), priority(priority), context(ctx),
          executionTime(0.0), isCrossParameter(false), isConditional(false) {}
    
    // Convert to string for output
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(level) << "]";
        if (!ruleId.empty()) oss << "[" << ruleId << "]";
        oss << " Key:" << key;
        if (!subKey.empty()) oss << " SubKey:" << subKey;
        oss << " Priority:" << priority;
        oss << " " << message;
        if (!context.empty()) oss << " Context:" << context;
        return oss.str();
    }
    
    // Check if this result is a success (info level)
    bool isSuccess() const { return level == ValidationLevel::Info && action == ValidationAction::Allow; }
    
    // Check if this result is an error
    bool isError() const { return level >= ValidationLevel::Warning; }
    
    // Check if this result is critical
    bool isCritical() const { return level == ValidationLevel::Critical; }
};

// Validation rule definition
struct ValidationRule {
    std::string id;                      ///< Unique rule identifier
    std::string name;                    ///< Human-readable name
    std::string description;             ///< Description of what this rule validates
    ValidationLevel level;               ///< Default severity level
    ValidationAction defaultAction;      ///< Default action if no specific action is set
    int priority;                        ///< Rule priority (higher = executed first)
    
    // Cross-parameter validation function
    std::function<std::vector<ValidationResultDetail>(const Config& config,
                                                      const std::vector<ConfigEntry>& entries)> crossValidator;
    
    // Conditional validation function
    std::function<std::vector<ValidationResultDetail>(const Config& config,
                                                     const std::vector<ConfigEntry>& entries,
                                                     const ConfigParameter& param,
                                                     const ConfigEntry& entry)> conditionalValidator;
    
    // Range validation function
    std::function<std::vector<ValidationResultDetail>(const Config& config,
                                                      const std::vector<ConfigEntry>& entries,
                                                      const ConfigParameter& param,
                                                      const ConfigEntry& entry)> rangeValidator;
    
    // Dependency validation function
    std::function<std::vector<ValidationResultDetail>(const Config& config,
                                                      const std::vector<ConfigEntry>& entries,
                                                      const ConfigParameter& param,
                                                      const ConfigEntry& entry)> dependencyValidator;
    
    ValidationRule()
        : level(ValidationLevel::Warning), defaultAction(ValidationAction::Warn),
          priority(100) {}
    
    ValidationRule(const std::string& id, const std::string& name,
                   const std::string& desc, int priority = 100)
        : id(id), name(name), description(desc),
          level(ValidationLevel::Warning), defaultAction(ValidationAction::Warn),
          priority(priority) {}
    
    // Add cross-parameter validator
    void setCrossValidator(const std::function<std::vector<ValidationResultDetail>(const Config&, 
                                                                                const std::vector<ConfigEntry>&)>& validator) {
        crossValidator = validator;
    }
    
    // Add conditional validator
    void setConditionalValidator(const std::function<std::vector<ValidationResultDetail>(const Config&,
                                                                                        const std::vector<ConfigEntry>&,
                                                                                        const ConfigParameter&,
                                                                                        const ConfigEntry&)>& validator) {
        conditionalValidator = validator;
    }
    
    // Add range validator
    void setRangeValidator(const std::function<std::vector<ValidationResultDetail>(const Config&,
                                                                                 const std::vector<ConfigEntry>&,
                                                                                 const ConfigParameter&,
                                                                                 const ConfigEntry&)>& validator) {
        rangeValidator = validator;
    }
    
    // Add dependency validator
    void setDependencyValidator(const std::function<std::vector<ValidationResultDetail>(const Config&,
                                                                                       const std::vector<ConfigEntry>&,
                                                                                       const ConfigParameter&,
                                                                                       const ConfigEntry&)>& validator) {
        dependencyValidator = validator;
    }
    
    // Execute all applicable validators
    std::vector<ValidationResultDetail> validate(const Config& config,
                                                 const std::vector<ConfigEntry>& entries) const {
        std::vector<ValidationResultDetail> results;
        
        // Execute cross-parameter validator if set
        if (crossValidator) {
            auto crossResults = crossValidator(config, entries);
            results.insert(results.end(), crossResults.begin(), crossResults.end());
        }
        
        // Execute conditional validators for each parameter
        for (const auto& entry : entries) {
            if (auto param = schema.getParameter(entry.key); param) {
                if (conditionalValidator) {
                    auto condResults = conditionalValidator(config, entries, *param, entry);
                    results.insert(results.end(), condResults.begin(), condResults.end());
                }
                if (rangeValidator) {
                    auto rangeResults = rangeValidator(config, entries, *param, entry);
                    results.insert(results.end(), rangeResults.begin(), rangeResults.end());
                }
                if (dependencyValidator) {
                    auto depResults = dependencyValidator(config, entries, *param, entry);
                    results.insert(results.end(), depResults.begin(), depResults.end());
                }
            }
        }
        
        return results;
    }
};

// Validation rules engine
class ValidationRulesEngine {
private:
    std::vector<ValidationRule> rules;
    std::unordered_map<std::string, ValidationRule> rulesById;
    bool isInitialized;
    
public:
    ValidationRulesEngine() : isInitialized(false) {}
    
    // Add a validation rule
    void addRule(const ValidationRule& rule) {
        rules.push_back(rule);
        rulesById[rule.id] = rule;
        // Sort rules by priority (higher priority first)
        std::sort(rules.begin(), rules.end(),
                 [](const ValidationRule& a, const ValidationRule& b) {
                     return a.priority > b.priority;  // Higher priority first
                 });
        isInitialized = true;
    }
    
    // Get rule by ID
    const ValidationRule* getRule(const std::string& ruleId) const {
        auto it = rulesById.find(ruleId);
        if (it != rulesById.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    // Get all rules
    std::vector<ValidationRule> getAllRules() const {
        return rules;
    }
    
    // Validate configuration with all rules
    std::vector<ValidationResultDetail> validate(const Config& config,
                                                const std::vector<ConfigEntry>& entries) const {
        std::vector<ValidationResultDetail> results;
        
        for (const auto& rule : rules) {
            auto ruleResults = rule.validate(config, entries);
            results.insert(results.end(), ruleResults.begin(), ruleResults.end());
        }
        
        return results;
    }
    
    // Get rules by category
    std::vector<ValidationRule> getRulesByCategory(const std::string& category) const {
        std::vector<ValidationRule> categoryRules;
        for (const auto& rule : rules) {
            // Extract category from rule ID or description for filtering
            if (rule.id.find(category) != std::string::npos || 
                rule.description.find(category) != std::string::npos) {
                categoryRules.push_back(rule);
            }
        }
        return categoryRules;
    }
    
    // Get rules by priority range
    std::vector<ValidationRule> getRulesByPriority(int minPriority, int maxPriority) const {
        std::vector<ValidationRule> filteredRules;
        for (const auto& rule : rules) {
            if (rule.priority >= minPriority && rule.priority <= maxPriority) {
                filteredRules.push_back(rule);
            }
        }
        return filteredRules;
    }
    
    // Get validation statistics
    std::string getStatistics() const {
        std::ostringstream oss;
        oss << "Validation Rules Engine Statistics:" << std::endl;
        oss << "  Total rules: " << rules.size() << std::endl;
        oss << "  Initialized: " << (isInitialized ? "Yes" : "No") << std::endl;
        
        // Count rule types based on validator functions set
        int crossParamCount = 0;
        int conditionalCount = 0;
        int rangeCount = 0;
        int dependencyCount = 0;
        
        for (const auto& rule : rules) {
            if (rule.crossValidator) crossParamCount++;
            if (rule.conditionalValidator) conditionalCount++;
            if (rule.rangeValidator) rangeCount++;
            if (rule.dependencyValidator) dependencyCount++;
        }
        
        oss << "  Cross-parameter validators: " << crossParamCount << std::endl;
        oss << "  Conditional validators: " << conditionalCount << std::endl;
        oss << "  Range validators: " << rangeCount << std::endl;
        oss << "  Dependency validators: " << dependencyCount << std::endl;
        
        return oss.str();
    }
    
    // Check if engine is initialized
    bool isInitialized() const { return isInitialized; }
    
    // Reset engine
    void reset() {
        rules.clear();
        rulesById.clear();
        isInitialized = false;
    }
};

// Validation configuration for dynamic settings
struct ValidationConfig {
    bool enableCrossParameterValidation;      ///< Enable cross-parameter validations
    bool enableConditionalValidation;        ///< Enable conditional validations
    bool enableRangeValidation;               ///< Enable range validations
    bool enableDependencyValidation;         ///< Enable dependency validations
    bool enableConflictDetection;             ///< Enable conflict detection
    double maxMemoryUsage;                   ///< Maximum memory usage for validation (MB)
    int maxValidationTime;                   ///< Maximum validation time (seconds)
    bool reportAllWarnings;                   ///< Whether to report all warnings, not just errors
    bool validateOnLoad;                     ///< Whether to validate when loading configuration
    bool validateOnSet;                      ///< Whether to validate when setting values
    bool validateOnChange;                   ///< Whether to validate when configuration changes
    std::string validationFile;               ///< Path to validation rules file
    std::unordered_map<std::string, std::string> externalValidatorPaths; ///< External validator plugins
    
    ValidationConfig()
        : enableCrossParameterValidation(true),
          enableConditionalValidation(true),
          enableRangeValidation(true),
          enableDependencyValidation(true),
          enableConflictDetection(true),
          maxMemoryUsage(100.0),  // 100 MB
          maxValidationTime(10), // 10 seconds
          reportAllWarnings(false),
          validateOnLoad(true),
          validateOnSet(true),
          validateOnChange(true) {}
};

// Main validation framework
class ValidationFramework {
private:
    ValidationRulesEngine engine;
    ValidationConfig config;
    std::vector<ValidationResultDetail> currentResults;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    size_t totalValidations;
    size_t successfulValidations;
    size_t errorValidations;
    
public:
    ValidationFramework() : totalValidations(0), successfulValidations(0), errorValidations(0) {}
    
    // Initialize framework with default rules
    bool initialize() {
        startTime = std::chrono::high_resolution_clock::now();
        totalValidations = 0;
        successfulValidations = 0;
        errorValidations = 0;
        
        // Add default validation rules
        addDefaultRules();
        
        return true;
    }
    
    // Add default validation rules
    void addDefaultRules() {
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
    
    // Set validation configuration
    void setConfig(const ValidationConfig& cfg) {
        config = cfg;
    }
    
    // Get validation configuration
    const ValidationConfig& getConfig() const {
        return config;
    }
    
    // Validate configuration
    std::vector<ValidationResultDetail> validate(const Config& config,
                                                 const ConfigSchema& schema) const {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Get all entries from config
        auto entries = getAllConfigEntries(config);
        
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
    
    // Get all configuration entries from a Config object
    std::vector<ConfigEntry> getAllConfigEntries(const Config& config) const {
        std::vector<ConfigEntry> entries;
        
        // This is a simplified version - in practice, we would need access to internal config data
        // For now, we'll create a mock implementation
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
        
        return entries;
    }
    
    // Filter validation results by level
    std::vector<ValidationResultDetail> filterResultsByLevel(const std::vector<ValidationResultDetail>& results,
                                                             ValidationLevel minLevel) const {
        std::vector<ValidationResultDetail> filtered;
        for (const auto& result : results) {
            if (result.level >= minLevel) {
                filtered.push_back(result);
            }
        }
        return filtered;
    }
    
    // Group validation results by key
    std::unordered_map<std::string, std::vector<ValidationResultDetail>> groupResultsByKey(
        const std::vector<ValidationResultDetail>& results) const {
        std::unordered_map<std::string, std::vector<ValidationResultDetail>> grouped;
        
        for (const auto& result : results) {
            grouped[result.key].push_back(result);
        }
        
        return grouped;
    }
    
    // Generate validation report
    std::string generateReport(const std::vector<ValidationResultDetail>& results) const {
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
    
    // Get current timestamp
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Save validation results to file
    bool saveResultsToFile(const std::string& filepath,
                          const std::vector<ValidationResultDetail>& results) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(results);
        file.close();
        return true;
    }
    
    // Load validation rules from file
    bool loadRulesFromFile(const std::string& filepath) {
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
    
    // Get validation statistics
    std::string getStatistics() const {
        std::ostringstream oss;
        
        oss << "=== Validation Framework Statistics ===" << std::endl;
        oss << "  Total validations: " << totalValidations << std::endl;
        oss << "  Successful validations: " << successfulValidations << std::endl;
        oss << "  Error validations: " << errorValidations << std::endl;
        oss << "  Engine statistics:" << std::endl;
        oss << engine.getStatistics() << std::endl;
        
        return oss.str();
    }
}; 

} // namespace nlm
