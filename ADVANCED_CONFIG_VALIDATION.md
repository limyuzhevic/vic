NLM Configuration Advanced Options & Validation
=============================================

## Overview

This document details the advanced configuration options and validation framework for the enhanced NLM configuration system.

## Advanced Configuration Options

### 1. Brain Configuration Advanced Options

#### Neuron and Network Configuration
```json
"brain": {
  "neuron_count": 1000,
  "region_count": 2,
  "population_distribution": {
    "sensory": 0.25,
    "internal": 0.5,
    "motor": 0.25
  },
  "connectivity_pattern": "random",
  "target_connection_density": 0.1,
  "max_in_degree": 1000,
  "max_out_degree": 1000,
  "spike_threshold_variation": 2.0,
  "rest_potential_variation": 5.0,
  "refractory_period_variation": 5,
  "adaptational_excitation": 0.1
},
"neuron_parameters": {
  "initial_membrane_potential": {
    "mean": -70.0,
    "std_dev": 3.0
  },
  "threshold_potential": {
    "mean": -55.0,
    "std_dev": 2.0
  },
  "reset_potential": {
    "mean": -70.0,
    "std_dev": 2.0
  },
  "leak_conductance": {
    "mean": 10.0,
    "std_dev": 2.0
  },
  "refractory_period": {
    "min": 2,
    "max": 10
  },
  "adaptation_strength": {
    "mean": 0.1,
    "std_dev": 0.05
  }
}
```

#### Plasticity Configuration Advanced Options
```json
"plasticity": {
  "stdp": {
    "enable": true,
    "learning_rate": 0.001,
    "tau_plus": 20.0,
    "tau_minus": 20.0,
    "weight_range": {
      "min": -1.0,
      "max": 1.0
    },
    "pre_post_timing": {
      "pre_post_window": 50,
      "post_pre_window": 50
    },
    "learning_stdp_window": true,
    "triplet_interaction": false
  },
  "hebbian": {
    "enable": true,
    "learning_rate": 0.01,
    "global_eligibility": false,
    "local_eligibility": true,
    "weight_change_clamp": {
      "max_delta": 0.1,
      "min_delta": -0.1
    }
  },
  "structural": {
    "enable": true,
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "max_synapses": 50000,
    "min_synapses": 100,
    "growth_threshold": 0.1,
    "pruning_threshold": 0.01,
    "homeostatic_control": true,
    "homeostatic_target": 0.5
  },
  "reward_modulated": {
    "enable": true,
    "learning_rate": 0.1,
    "eligibility_decay": 0.9,
    "credit_assignment": "td_error",
    "prediction_error_window": 100
  }
}
```

### 2. Neuromodulation Advanced Options
```json
"neuromodulation": {
  "dopamine": {
    "baseline": 0.1,
    "scale": 1.0,
    "decay_rate": 0.95,
    "plasticity_modulation": {
      "ltp_modulation": true,
      "ltd_modulation": true,
      "weight_change_factor": 1.0
    },
    "excitability_modulation": {
      "threshold_modulation": true,
      "adaptation_modulation": true,
      "refractory_modulation": false
    }
  },
  "curiosity": {
    "enable": true,
    "baseline": 0.05,
    "exploration_bonus": 0.5,
    "novelty_weight": 0.7,
    "reward_weight": 0.3,
    "expected_value_weight": 0.2,
    "exploration_temperature": 1.0,
    "decay_rate": 0.98,
    "novelty_threshold": 0.1,
    "max_curiosity": 2.0
  },
  "novelty": {
    "enable": true,
    "sensory_channels": ["vision", "touch", "internal"],
    "computation_method": "temporal_differences",
    "decay_rate": 0.95,
    "novelty_decay_rate": 0.9,
    "novelty_threshold": 0.1,
    "novelty_max_value": 1.0
  },
  "prediction_error": {
    "enable": true,
    "learning_rate": 0.1,
    "prediction_horizon": 10,
    "error_calculation": "squared",
    "error_weighting": true,
    "credit_assignment": "eligibility_trace",
    "trace_decay": 0.95,
    "prediction_error_threshold": 0.5
  },
  "acetylcholine": {
    "enable": true,
    "learning_rate": 0.05,
    "attention_modulation": true,
    "working_memory_boost": 0.2,
    "phasic_mode": false
  }
}
```

### 3. Development Configuration Advanced Options
```json
"development": {
  "enable": true,
  "stage_duration": {
    "initial": 100,
    "critical_period": 900,
    "maturation": 2000,
    "adult": 0
  },
  "plasticity_curves": {
    "initial": 1.0,
    "critical_period_start": 0.9,
    "critical_period_end": 0.8,
    "maturation_start": 0.8,
    "maturation_end": 0.5
  },
  "structural_plasticity": {
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "homeostatic_growth": true,
    "network_competition": true,
    "regional_specificity": true
  },
  "functional_maturation": {
    "sensory_refinement": true,
    "motor_refinement": true,
    "cognitive_maturation": true,
    "synaptic_pruning_threshold": 0.01,
    "new_connection_benefit": 0.1
  },
  "experience_dependent": {
    "enrichment_enhancement": true,
    "deprivation_depression": true,
    "critical_period_plasticity": true,
    "sensitive_period_length": 100
  },
  "behavioral_maturation": {
    "action_selection_maturity": true,
    "planning_maturity": true,
    "social_learning_maturity": true,
    "problem_solving_maturity": true
  }
}
```

### 4. Simulation Configuration Advanced Options
```json
"simulation": {
  "timestep": 0.001,
  "max_steps": 10000,
  "time_limit": 0.0,
  "log_level": "INFO",
  "log_to_file": true,
  "log_filename": "nlm.log",
  "log_format": "detailed",
  "max_log_size": 10485760,
  "log_rotation_count": 5,
  "performance_monitoring": {
    "enable": true,
    "log_interval": 1000,
    "memory_tracking": true,
    "timestep_tracking": true,
    "cpu_tracking": true,
    "gpu_tracking": true
  },
  "checkpointing": {
    "enable": true,
    "interval": 1000,
    "compression": "balanced",
    "compression_level": 5,
    "format": "binary",
    "encrypt": false,
    "encryption_key_file": ".nlm_encryption_key",
    "verify_checksum": true,
    "backup_count": 3
  },
  "parallel": {
    "enable": true,
    "threads": 4,
    "thread_pool_size": 8,
    "work_distribution": "dynamic",
    "load_balancing": true,
    "thread_affinity": false
  },
  "memory_management": {
    "enable_memory_pools": true,
    "pool_size": 1000000,
    "enable_spare_memory": true,
    "spare_memory_ratio": 0.1,
    "enable_memory_profiling": false,
    "profile_frequency": 1000
  },
  "performance_optimization": {
    "enable_simd": true,
    "enable_vectorization": true,
    "enable_loop_unrolling": true,
    "enable_cache_optimization": true,
    "enable_branch_prediction": false,
    "enable_speculative_execution": false
  }
}
```

## Configuration Validation Framework

### 1. Type Validation
```cpp
class TypeValidator {
public:
    static bool validateString(const std::string& value, 
                               const std::string& pattern = "",
                               size_t min_length = 0,
                               size_t max_length = std::string::max_size);
    
    static bool validateNumber(const std::string& key,
                              double value,
                              double min = std::numeric_limits<double>::lowest(),
                              double max = std::numeric_limits<double>::max());
    
    static bool validateBoolean(const std::string& key, bool value);
    
    static bool validateEnum(const std::string& key,
                            const std::string& value,
                            const std::vector<std::string>& valid_values);
    
    static bool validateArray(const std::string& key,
                             const std::vector<double>& values,
                             size_t min_size = 0,
                             size_t max_size = std::numeric_limits<size_t>::max());
    
    static bool validateObject(const std::string& key,
                              const json& obj,
                              const json& schema);
};
```

### 2. Range Validation
```cpp
class RangeValidator {
public:
    static bool validateNeuronCount(uint64_t count);
    static bool validateConnectionProbability(float prob);
    static bool validateTimestep(double timestep);
    static bool validateWeightRange(float weight);
    static bool validatePlasticityRate(float rate);
    static bool validateLearningRate(float rate);
    static bool validateTimeConstant(float tau);
    static bool validateThreshold(float threshold);
    static bool validatePotential(float potential);
    static bool validateConductance(float conductance);
    static bool validateDelay(uint32_t delay);
    static bool validateActivity(float activity);
    static bool validateNovelty(float novelty);
    static bool validateCuriosity(float curiosity);
    static bool validateDopamine(float dopamine);
    static bool validatePredictionError(float error);
    static bool validateReward(float reward);
    static bool validateMemoryUsage(size_t memory);
    static bool validatePerformance(float performance);
    static bool validateEnergy(float energy);
    static bool validateEntropy(float entropy);
};
```

### 3. Dependency Validation
```cpp
class DependencyValidator {
public:
    static bool validateBrainDependencies(const Config& config);
    static bool validatePlasticityDependencies(const Config& config);
    static bool validateNeuromodulationDependencies(const Config& config);
    static bool validateDevelopmentDependencies(const Config& config);
    static bool validateSimulationDependencies(const Config& config);
    static bool validateWorldDependencies(const Config& config);
    static bool validateVisualizationDependencies(const Config& config);
    
    static std::vector<std::string> getDependencyErrors(const Config& config);
    static std::vector<std::string> getDependencyWarnings(const Config& config);
};
```

### 4. Configuration Consistency Validation
```cpp
class ConsistencyValidator {
public:
    enum ConsistencyCheck {
        BRAIN_INTERNAL,
        PLASTICITY_STDP_HEBBIAN,
        NEUROMODULATION_BALANCE,
        DEVELOPMENT_STAGES,
        SIMULATION_PARAMETERS,
        WORLD_AGENT_COMPATIBILITY,
        PERFORMANCE_CONSTRAINTS,
        MEMORY_CONSTRAINTS,
        SECURITY_CONSTRAINTS
    };
    
    static bool validateConsistency(const Config& config,
                                    ConsistencyCheck check);
    
    static bool validateAllConsistencyChecks(const Config& config,
                                            std::vector<std::string>& errors,
                                            std::vector<std::string>& warnings);
    
    static std::string getConsistencyReport(const Config& config);
    static std::string getConsistencyErrorMessage(ConsistencyCheck check);
};
```

## Error Handling and Robustness

### 1. Error Classification
```cpp
enum class ConfigErrorType {
    FILE_NOT_FOUND,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    INVALID_FORMAT,
    JSON_SYNTAX_ERROR,
    YAML_SYNTAX_ERROR,
    TYPE_MISMATCH,
    RANGE_VIOLATION,
    MISSING_REQUIRED_FIELD,
    INVALID_DEPENDENCY,
    INCONSISTENT_CONFIGURATION,
    VALIDATION_ERROR,
    PARSING_ERROR,
    SERIALIZATION_ERROR,
    CONFIGURATION_TOO_LARGE,
    ENCRYPTION_ERROR,
    NETWORK_ERROR,
    PERMISSION_ERROR,
    RESOURCE_EXHAUSTED,
    UNKNOWN_ERROR
};

struct ConfigError {
    ConfigErrorType type;
    std::string message;
    std::string context;
    std::string suggested_fix;
    int line_number;
    int column_number;
    std::string file_path;
    
    std::string toString() const;
    std::string toDetailedString() const;
    bool isRecoverable() const;
    bool isFatal() const;
};
```

### 2. Error Recovery Framework
```cpp
class ErrorRecovery {
public:
    enum RecoveryStrategy {
        ABORT,
        SKIP_INVALID,
        USE_DEFAULTS,
        USE_PREVIOUS,
        REQUEST_USER_INPUT,
        MODIFY_CONFIGURATION
    };
    
    static std::vector<ConfigError> loadWithRecovery(
        const std::string& filepath,
        RecoveryStrategy strategy = USE_DEFAULTS,
        const Config& fallback_config = Config());
    
    static std::vector<ConfigError> validateWithRecovery(
        const Config& config,
        RecoveryStrategy strategy = SKIP_INVALID);
    
    static std::vector<ConfigError> saveWithRecovery(
        const std::string& filepath,
        const Config& config,
        RecoveryStrategy strategy = ABORT);
    
    static ConfigError attemptRecovery(const ConfigError& error,
                                       RecoveryStrategy strategy);
    
    static bool canRecover(const ConfigError& error,
                          RecoveryStrategy strategy);
};
```

### 3. Graceful Degradation
```cpp
class GracefulDegradation {
public:
    static Config loadWithFallback(const std::vector<std::string>& filepaths,
                                  const Config& fallback_config);
    
    static bool isConfigurationUsable(const Config& config,
                                     bool require_all_valid = false);
    
    static Config getMinimalWorkingConfiguration();
    
    static bool validateCriticalParameters(const Config& config);
    
    static std::vector<std::string> getCriticalParameterKeys();
    
    static std::string getConfigurationHealthReport(const Config& config);
};
```

## Validation Examples

### 1. Configuration Validation Example
```cpp
bool validateConfiguration(const Config& config) {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Type validation
    if (!TypeValidator::validateNumber("brain.neuron_count", 
                                       config.get<uint64_t>("brain.neuron_count"),
                                       100, 1000000)) {
        errors.push_back("brain.neuron_count must be between 100 and 1,000,000");
    }
    
    // Range validation
    if (!RangeValidator::validateConnectionProbability(
        config.get<float>("brain.connection_probability"))) {
        errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
    }
    
    // Dependency validation
    if (!DependencyValidator::validateBrainDependencies(config)) {
        auto dep_errors = DependencyValidator::getDependencyErrors(config);
        errors.insert(errors.end(), dep_errors.begin(), dep_errors.end());
    }
    
    // Consistency validation
    if (!ConsistencyValidator::validateConsistency(config, 
                                                   ConsistencyValidator::BRAIN_INTERNAL)) {
        auto consistency_errors = ConsistencyValidator::getConsistencyErrors(config);
        errors.insert(errors.end(), consistency_errors.begin(), consistency_errors.end());
    }
    
    if (!errors.empty()) {
        std::cout << "Configuration validation failed:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
        return false;
    }
    
    if (!warnings.empty()) {
        std::cout << "Configuration validation warnings:" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    
    return true;
}
```

### 2. Error Handling Example
```cpp
void processConfiguration(const std::string& filepath) {
    try {
        Config config;
        
        // Load configuration with error recovery
        std::vector<ConfigError> errors = 
            ErrorRecovery::loadWithRecovery(filepath, RecoveryStrategy::USE_DEFAULTS);
        
        if (!errors.empty()) {
            std::cout << "Configuration loaded with errors:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << error.toString() << std::endl;
                if (error.suggested_fix.empty()) {
                    std::cout << "    Suggested fix: " << error.suggested_fix << std::endl;
                }
            }
        }
        
        // Validate configuration
        std::vector<std::string> validation_errors;
        std::vector<std::string> validation_warnings;
        
        if (!ConfigValidator::validate(config, ConfigValidator::STRICT, 
                                      validation_errors, validation_warnings)) {
            std::cout << "Configuration validation failed:" << std::endl;
            for (const auto& error : validation_errors) {
                std::cout << "  - " << error << std::endl;
            }
            return;
        }
        
        if (!validation_warnings.empty()) {
            std::cout << "Configuration validation warnings:" << std::endl;
            for (const auto& warning : validation_warnings) {
                std::cout << "  - " << warning << std::endl;
            }
        }
        
        // Save configuration with error recovery
        std::vector<ConfigError> save_errors = 
            ErrorRecovery::saveWithRecovery("enhanced_config.cfg", config);
        
        if (!save_errors.empty()) {
            std::cout << "Failed to save configuration:" << std::endl;
            for (const auto& error : save_errors) {
                std::cout << "  - " << error.toString() << std::endl;
            }
        }
        
        std::cout << "Configuration processed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Fatal error processing configuration: " << e.what() 
                  << std::endl;
        return;
    }
}
```

## Validation Reports

### 1. Configuration Health Report
```
Configuration Health Report:
===========================

Status: HEALTHY

Parameters Validated: 127/127
Warnings: 3
Errors: 0

Validation Summary:
- Type Validation: PASSED (127/127)
- Range Validation: PASSED (85/85)
- Dependency Validation: PASSED (12/12)
- Consistency Validation: PASSED (32/32)

Detailed Validation Results:

1. Brain Configuration:
   - Neuron Count: 1000 (valid)
   - Connection Probability: 0.1 (valid)
   - Population Distribution: [0.25, 0.5, 0.25] (valid)
   - Connectivity Pattern: "random" (valid)

2. Plasticity Configuration:
   - STDP: Enabled (valid)
   - Hebbian: Enabled (valid)
   - Structural: Enabled (valid)
   - Reward-Modulated: Enabled (valid)

3. Neuromodulation Configuration:
   - Dopamine: Baseline=0.1, Scale=1.0 (valid)
   - Curiosity: Enabled (valid)
   - Novelty: Enabled (valid)
   - Prediction Error: Enabled (valid)

4. Development Configuration:
   - Enabled: true (valid)
   - Stage Duration: [100, 900, 2000, 0] (valid)
   - Critical Period: [100, 1000] (valid)

5. Simulation Configuration:
   - Timestep: 0.001 (valid)
   - Max Steps: 10000 (valid)
   - Performance Monitoring: Enabled (valid)

Warnings:
1. brain.connection_probability is below optimal for network efficiency
2. plasticity.stdp.learning_rate is below recommended minimum for stable learning
3. development.stage_duration.adult is 0 (brain may not reach adult stage)

Recommendations:
1. Consider increasing connection_probability to 0.2 for better network connectivity
2. Consider increasing stdp.learning_rate to 0.005 for more robust learning
3. Consider setting development.stage_duration.adult to a positive value
```

### 2. Error Report Example
```
Configuration Error Report:
===========================

Error Summary:
Total Errors: 12
Fatal Errors: 3
Recoverable Errors: 9

Error Details:

1. FATAL ERROR (File Read Error)
   File: "configs/default.cfg"
   Line: 1, Column: 1
   Message: Failed to open configuration file
   Suggested Fix: Check file permissions and path

2. FATAL ERROR (JSON Syntax Error)
   File: "configs/enhanced.json"
   Line: 42, Column: 5
   Message: Unexpected character ',' in object
   Suggested Fix: Remove trailing comma or escape properly

3. FATAL ERROR (Type Mismatch)
   Key: "brain.neuron_count"
   Value: "invalid"
   Expected: Integer
   Suggested Fix: Provide a numeric value

4. RECOVERABLE ERROR (Range Violation)
   Key: "brain.connection_probability"
   Value: 1.5
   Valid Range: [0.0, 1.0]
   Suggested Fix: Clamp value to [0.0, 1.0] or specify new value

5. RECOVERABLE ERROR (Missing Required Field)
   Key: "plasticity.stdp.tau_plus"
   Suggested Fix: Provide a value for tau_plus (recommended: 20.0)

Validation Errors:
- brain.neuron_count must be between 100 and 1,000,000
- brain.connection_probability must be between 0.0 and 1.0
- plasticity.stdp.learning_rate must be between 0.0001 and 0.01
- development.stage_duration.initial must be positive
- simulation.timestep must be positive

Recovery Actions Taken:
1. Applied default values for missing required fields
2. Clamped invalid values to valid ranges
3. Warned about suboptimal configuration parameters
4. Skipped invalid configuration entries

Recommended Actions:
1. Fix fatal errors first (file access and syntax errors)
2. Update recoverable errors to avoid suboptimal behavior
3. Run validation again to confirm configuration health
```

## Testing and Validation

### 1. Configuration Validation Tests
```cpp
class ConfigValidationTests {
public:
    static void testTypeValidation() {
        // Test string validation
        assert(TypeValidator::validateString("test", "^[a-zA-Z]+$", 1, 10));
        assert(!TypeValidator::validateString("test", "^[a-zA-Z]+$", 3));
        
        // Test number validation
        assert(TypeValidator::validateNumber("test", 5.0, 0.0, 10.0));
        assert(!TypeValidator::validateNumber("test", 15.0, 0.0, 10.0));
        
        // Test boolean validation
        assert(TypeValidator::validateBoolean("test", true));
        assert(TypeValidator::validateBoolean("test", false));
    }
    
    static void testRangeValidation() {
        // Test neuron count validation
        assert(RangeValidator::validateNeuronCount(100));
        assert(RangeValidator::validateNeuronCount(1000000));
        assert(!RangeValidator::validateNeuronCount(10));
        assert(!RangeValidator::validateNeuronCount(10000000));
        
        // Test connection probability validation
        assert(RangeValidator::validateConnectionProbability(0.0));
        assert(RangeValidator::validateConnectionProbability(1.0));
        assert(!RangeValidator::validateConnectionProbability(-0.1));
        assert(!RangeValidator::validateConnectionProbability(1.1));
    }
    
    static void testDependencyValidation() {
        // Test brain dependency validation
        Config config;
        config.set("brain.neuron_count", 1000);
        config.set("brain.connection_probability", 0.1);
        assert(DependencyValidator::validateBrainDependencies(config));
        
        // Test invalid dependency (neuron_count too small for connection probability)
        config.set("brain.neuron_count", 10);
        assert(!DependencyValidator::validateBrainDependencies(config));
    }
    
    static void testConsistencyValidation() {
        // Test brain internal consistency
        Config config;
        config.set("brain.neuron_count", 1000);
        config.set("brain.connection_probability", 0.1);
        assert(ConsistencyValidator::validateConsistency(config, 
                                                         ConsistencyValidator::BRAIN_INTERNAL));
        
        // Test inconsistent configuration
        config.set("brain.neuron_count", 0);
        assert(!ConsistencyValidator::validateConsistency(config,
                                                         ConsistencyValidator::BRAIN_INTERNAL));
    }
};
```

### 2. Error Handling Tests
```cpp
class ConfigErrorHandlingTests {
public:
    static void testErrorClassification() {
        // Test file not found error
        ConfigError error;
        error.type = ConfigErrorType::FILE_NOT_FOUND;
        assert(error.isFatal());
        assert(!error.isRecoverable());
        
        // Test syntax error
        ConfigError syntax_error;
        syntax_error.type = ConfigErrorType::JSON_SYNTAX_ERROR;
        assert(syntax_error.isRecoverable());
        assert(!syntax_error.isFatal());
        
        // Test type mismatch error
        ConfigError type_error;
        type_error.type = ConfigErrorType::TYPE_MISMATCH;
        assert(type_error.isRecoverable());
        assert(!type_error.isFatal());
    }
    
    static void testErrorRecovery() {
        // Test recovery strategy
        ConfigError error;
        error.type = ConfigErrorType::RANGE_VIOLATION;
        
        assert(ErrorRecovery::canRecover(error, ErrorRecovery::RecoveryStrategy::USE_DEFAULTS));
        assert(ErrorRecovery::canRecover(error, ErrorRecovery::RecoveryStrategy::SKIP_INVALID));
        assert(!ErrorRecovery::canRecover(error, ErrorRecovery::RecoveryStrategy::ABORT));
        
        // Test recovery action
        ConfigError recovered_error = 
            ErrorRecovery::attemptRecovery(error, ErrorRecovery::RecoveryStrategy::USE_DEFAULTS);
        assert(recovered_error.type == ConfigErrorType::RANGE_VIOLATION);
    }
    
    static void testGracefulDegradation() {
        // Test configuration with fallback
        Config fallback_config;
        fallback_config.set("brain.neuron_count", 1000);
        
        Config degraded_config = GracefulDegradation::loadWithFallback(
            {"nonexistent.json", "another_nonexistent.json"}, fallback_config);
        
        assert(degraded_config.has("brain.neuron_count"));
        assert(degraded_config.get<uint64_t>("brain.neuron_count") == 1000);
        
        // Test configuration health check
        assert(GracefulDegradation::validateCriticalParameters(fallback_config));
    }
};
```

This comprehensive configuration validation and error handling framework ensures that NLM configurations are robust, validated, and provide clear feedback to users when issues are encountered. The system supports both strict validation and graceful degradation, making it suitable for both development and production environments.