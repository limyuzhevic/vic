# Configuration Validation Improvements for NLM

## Overview

This document outlines comprehensive configuration validation improvements for the NLM (Neural Learning Machine) system. The current configuration system lacks proper validation, which can lead to:

- Invalid configurations that cause runtime failures
- Difficult-to-debug issues from incompatible parameter combinations
- Poor user experience with unclear error messages
- System instability from invalid parameter values

## Current Configuration Validation Issues

### 1. No Parameter Range Validation
**Current Behavior:**
```cpp
// Any value can be set, regardless of whether it makes sense
config.set("neuron_count", 0);                    // Zero neurons
config.set("neuron_count", -1000);                // Negative neurons
config.set("connection_probability", 2.0);        // > 1.0 probability
config.set("simulation_timestep", 0.0);           // Zero timestep
```

**Issues:**
- Parameters can be set to obviously invalid values
- No validation until runtime (if at all)
- Users get cryptic errors later in execution

**Required Improvements:**
- Range validation for all numeric parameters
- Minimum/maximum value constraints
- Early validation with clear error messages

### 2. No Cross-Parameter Validation
**Current Behavior:**
```cpp
config.set("neuron_count", 1000000);  // Very large
config.set("region_count", 1000);     // Also large
// Result: Memory exhaustion and performance degradation
```

**Issues:**
- Incompatible parameter combinations
- Resource exhaustion scenarios
- Performance degradation from unbalanced configurations

**Required Improvements:**
- Compatibility checks between related parameters
- Resource constraint validation
- Performance impact warnings

### 3. No Schema Validation
**Current Behavior:**
```cpp
// Invalid JSON configuration
{
  "neuron_count": "string_instead_of_int",
  "unknown_parameter": "this_parameter_does_not_exist"
}
```

**Issues:**
- Type mismatches in configuration files
- Unknown parameters ignored silently
- Configuration structure not validated

**Required Improvements:**
- JSON schema validation for configuration files
- Type checking for all configuration parameters
- Parameter existence validation

### 4. No Default Configuration Validation
**Current Behavior:**
```cpp
// Default config may have invalid values
// (duplicate dopamine_baseline was one example)
```

**Issues:**
- Default configurations may be invalid
- No validation of hardcoded defaults
- Potential for systemic issues

**Required Improvements:**
- Validate all default configuration values
- Ensure defaults are sensible
- Provide configuration migration paths

## Required Configuration Validation Improvements

### 1. Parameter Schema Definition
```cpp
struct ConfigParameter {
    std::string name;
    std::string description;
    ConfigSource default_source;
    Any default_value;
    std::function<bool(const Any&)> validator;
    std::function<std::string(const Any&)> formatter;
    bool required;
    std::vector<std::string> dependencies;  // Other parameters this depends on
};

// Example parameter definitions
const std::vector<ConfigParameter> PARAMETER_SCHEMAS = {
    {
        .name = "neuron_count",
        .description = "Number of neurons in the brain",
        .default_source = ConfigSource::Default,
        .default_value = 1000,
        .validator = [](const Any& val) { return std::any_cast<int64_t>(val) > 0; },
        .formatter = [](const Any& val) { return "Neuron count must be positive"; },
        .required = true,
        .dependencies = {}
    },
    {
        .name = "region_count",
        .description = "Number of brain regions",
        .default_source = ConfigSource::Default,
        .default_value = 2,
        .validator = [](const Any& val) { 
            int64_t regions = std::any_cast<int64_t>(val);
            return regions > 0 && regions <= 100;  // Reasonable upper bound
        },
        .formatter = [](const Any& val) { return "Region count must be 1-100"; },
        .required = false,
        .dependencies = {"neuron_count"}
    },
    {
        .name = "connection_probability",
        .description = "Probability of synaptic connections",
        .default_source = ConfigSource::Default,
        .default_value = 0.1,
        .validator = [](const Any& val) {
            float prob = std::any_cast<float>(val);
            return prob >= 0.0f && prob <= 1.0f;
        },
        .formatter = [](const Any& val) { return "Connection probability must be 0.0-1.0"; },
        .required = false,
        .dependencies = {}
    }
    // ... more parameters
};
```

### 2. Comprehensive Validation Framework
```cpp
class ConfigurationValidator {
public:
    ConfigurationValidator(const std::vector<ConfigParameter>& schemas);
    
    bool validate(const Config& config) const;
    std::vector<std::string> validateAndGetErrors(const Config& config) const;
    
    bool validateParameter(const std::string& name, const Any& value) const;
    std::string getParameterError(const std::string& name, const Any& value) const;
    
    bool validateCrossParameterDependencies(const Config& config) const;
    std::vector<std::string> getCrossParameterErrors(const Config& config) const;
    
    void setStrictMode(bool strict) { m_strictMode = strict; }
    bool getStrictMode() const { return m_strictMode; }
    
    void addCustomValidator(const std::string& name, 
                           std::function<bool(const Any&)> validator);
    
private:
    std::unordered_map<std::string, ConfigParameter> m_schemaMap;
    std::unordered_map<std::string, std::function<bool(const Any&)>> m_customValidators;
    bool m_strictMode;
};
```

### 3. Configuration File Validation
```cpp
class ConfigurationFileValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::unordered_map<std::string, std::string> warnings;
    };
    
    ValidationResult validateFile(const std::string& filepath) const;
    ValidationResult validateContent(const std::string& jsonContent) const;
    
    std::vector<std::string> getSupportedParameters() const;
    bool hasParameter(const std::string& name) const;
    
    void setSchemaFile(const std::string& schemaFile) { m_schemaFile = schemaFile; }
    
private:
    std::string m_schemaFile;
    std::unordered_map<std::string, ConfigParameter> m_supportedParameters;
};
```

### 4. Configuration Migration and Fix Suggestions
```cpp
class ConfigurationFixer {
public:
    struct FixResult {
        bool fixed;
        Config correctedConfig;
        std::vector<std::string> changesMade;
        std::vector<std::string> warnings;
    };
    
    FixResult fixConfiguration(const Config& problematicConfig) const;
    std::vector<FixResult> autoFixConfiguration(const Config& config) const;
    
    std::vector<std::string> getFixSuggestions(const Config& config) const;
    
private:
    std::vector<std::pair<std::function<bool(const Config&)>, FixResult>> m_fixRules;
};
```

### 5. Runtime Configuration Monitoring
```cpp
class ConfigurationMonitor {
public:
    struct ConfigHealthStatus {
        bool healthy;
        double healthScore;  // 0.0 - 1.0
        std::vector<std::string> issues;
        std::vector<std::string> warnings;
        std::unordered_map<std::string, double> parameterHealth;
    };
    
    void updateHealth(const Config& config);
    ConfigHealthStatus getHealthStatus() const;
    
    bool isHealthy() const { return getHealthStatus().healthy; }
    double getHealthScore() const { return getHealthStatus().healthScore; }
    
    void addHealthMonitor(const std::string& name,
                         std::function<ConfigHealthStatus(const Config&)> monitor);
    
private:
    std::vector<std::pair<std::string, std::function<ConfigHealthStatus(const Config&)>>> m_monitors;
    ConfigHealthStatus m_currentStatus;
    std::mutex m_mutex;
};
```

## Configuration Validation Test Scenarios

### 1. Parameter Range Validation Tests
```python
def test_parameter_range_validation():
    """Test that configuration parameters are validated for correct ranges."""
    config = pynlm.createDefaultConfig()
    
    # Test neuron count validation
    with pytest.raises(ValueError):
        config.set("neuron_count", 0)  # Zero neurons
    
    with pytest.raises(ValueError):
        config.set("neuron_count", -100)  # Negative neurons
    
    # Valid range should work
    config.set("neuron_count", 500)
    assert config.getOr("neuron_count", 0) == 500
    
    # Test connection probability validation
    with pytest.raises(ValueError):
        config.set("connection_probability", 1.5)  # > 1.0
    
    with pytest.raises(ValueError):
        config.set("connection_probability", -0.5)  # < 0.0
    
    # Valid probability should work
    config.set("connection_probability", 0.2)
    assert config.getOr("connection_probability", 0.0) == 0.2

def test_simulation_timestep_validation():
    """Test simulation timestep validation."""
    config = pynlm.createDefaultConfig()
    
    # Test timestep validation
    with pytest.raises(ValueError):
        config.set("simulation_timestep", 0.0)  # Zero timestep
    
    with pytest.raises(ValueError):
        config.set("simulation_timestep", -0.001)  # Negative timestep
    
    # Valid timestep should work
    config.set("simulation_timestep", 0.001)
    assert config.getOr("simulation_timestep", 0.0) == 0.001
```

### 2. Cross-Parameter Validation Tests
```python
def test_neuron_region_ratio_validation():
    """Test that neuron count and region count are compatible."""
    config = pynlm.createDefaultConfig()
    
    # Test extreme ratios
    config.set("neuron_count", 1000)
    config.set("region_count", 1)  # Should be valid (1 region with 1000 neurons)
    
    config.set("neuron_count", 100)
    config.set("region_count", 1000)  # Should be invalid (1000 regions with 100 neurons)
    # Should raise error or warning
    
    # Test minimum neurons per region
    config.set("neuron_count", 50)
    config.set("region_count", 30)  # 50/30 ≈ 1.67 neurons per region
    # Should raise warning (very few neurons per region)

def test_plasticity_parameters_validation():
    """Test that plasticity parameters are reasonable."""
    config = pynlm.createDefaultConfig()
    
    # Test plasticity learning rate
    with pytest.raises(ValueError):
        config.set("plasticity_learning_rate", 10.0)  # Too high
    
    with pytest.raises(ValueError):
        config.set("plasticity_learning_rate", -1.0)  # Negative
    
    # Test STDP parameters
    with pytest.raises(ValueError):
        config.set("stdp_ltp_weight", 10.0)  # Too high
    
    with pytest.raises(ValueError):
        config.set("stdp_ltd_weight", -10.0)  # Negative
```

### 3. Configuration File Validation Tests
```python
def test_configuration_file_schema_validation():
    """Test validation of configuration file schema."""
    import json
    import tempfile
    import os
    
    config = pynlm.createDefaultConfig()
    
    # Test valid configuration file
    valid_config = {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.1
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(valid_config, f)
        temp_file = f.name
    
    try:
        config.loadFromFile(temp_file)
        assert config.has("neuron_count")
        assert config.getOr("neuron_count", 0) == 500
    finally:
        os.unlink(temp_file)
    
    # Test invalid configuration file (type mismatch)
    invalid_config = {
        "neuron_count": "not_a_number",  # Should be integer
        "region_count": 2
    }
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump(invalid_config, f)
        temp_file = f.name
    
    try:
        # Should raise error for invalid type
        with pytest.raises(ValueError):
            config.loadFromFile(temp_file)
    finally:
        os.unlink(temp_file)

def test_unknown_parameter_handling():
    """Test handling of unknown configuration parameters."""
    config = pynlm.createDefaultConfig()
    
    # Add unknown parameter
    config.set("unknown_parameter", "this_should_be_validated")
    
    # Should either:
    # 1. Raise error when parameter is accessed
    # 2. Ignore silently but log warning
    # 3. Have option to be strict vs permissive
    
    # Check if parameter exists
    assert config.has("unknown_parameter")
```

### 4. Default Configuration Validation Tests
```python
import json

def test_default_configuration_validation():
    """Test that default configuration is valid."""
    config = pynlm.createDefaultConfig()
    
    # Test that default config can be validated without errors
    # This should pass for a well-designed default configuration
    try:
        # Try to save and reload default config
        import tempfile
        import os
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            # This assumes there's a saveToFile method
            # config.saveToFile(f.name)  # Would need to implement this test
            pass
        
        # Validate that defaults are within acceptable ranges
        assert config.getOr("neuron_count", 0) > 0
        assert config.getOr("region_count", 0) > 0
        assert config.getOr("connection_probability", -1) >= 0.0
        assert config.getOr("connection_probability", -1) <= 1.0
        
    except Exception as e:
        pytest.fail(f"Default configuration validation failed: {e}")
```

### 5. Configuration Fixer Tests
```python
def test_configuration_fixer():
    """Test automatic configuration fixing."""
    from config_fixer import ConfigurationFixer
    
    fixer = ConfigurationFixer()
    
    # Test configuration with multiple issues
    problematic_config = {
        "neuron_count": 0,  # Invalid: zero
        "region_count": -5,  # Invalid: negative
        "connection_probability": 1.5  # Invalid: > 1.0
    }
    
    # Create config object and apply fixer
    config = pynlm.createDefaultConfig()
    for key, value in problematic_config.items():
        config.set(key, value)
    
    # Apply fixes
    fix_result = fixer.fixConfiguration(config)
    
    # Should have fixed issues
    assert fix_result.fixed
    assert fix_result.correctedConfig.has("neuron_count")
    assert fix_result.correctedConfig.getOr("neuron_count", 0) > 0
    
    # Should have recorded changes
    assert len(fix_result.changesMade) > 0
```

## Implementation Plan

### Phase 1: Basic Validation (Low Priority)
1. Add basic parameter range validation
2. Add type validation for configuration parameters
3. Add basic file format validation
4. Implement basic validation tests

### Phase 2: Advanced Validation (Medium Priority)
1. Implement parameter schema definition
2. Add cross-parameter dependency validation
3. Add configuration file schema validation
4. Implement configuration fixing capabilities
5. Add advanced validation tests

### Phase 3: Production-Grade Validation (High Priority)
1. Implement comprehensive parameter validation
2. Add real-time configuration monitoring
3. Implement automatic configuration fixing
4. Add health monitoring and alerting
5. Implement production-ready validation tests

## Testing Strategy

### Unit Tests
- Test each validation rule individually
- Verify proper error messages
- Test edge cases and boundary conditions

### Integration Tests
- Test validation in real usage scenarios
- Test validation with complete configurations
- Test validation with partial configurations

### System Tests
- Test validation under performance constraints
- Test validation with large configurations
- Test validation in production-like environments

## Configuration Validation Documentation

### Configuration Parameter Reference
```markdown
| Parameter | Description | Valid Range | Default | Notes |
|-----------|-------------|--------------|---------|-------|
| neuron_count | Number of neurons | 1-1000000 | 1000 | Higher values increase memory usage |
| region_count | Number of brain regions | 1-100 | 2 | More regions enable specialized processing |
| connection_probability | Connection probability | 0.0-1.0 | 0.1 | Higher values increase connectivity |
| simulation_timestep | Time step size | 0.0001-0.1 | 0.001 | Smaller values increase accuracy |
```

### Configuration Error Messages
```python
class ConfigValidationError(Exception):
    """Base exception for configuration validation errors."""
    pass

class InvalidParameterValueError(ConfigValidationError):
    """Exception raised for invalid parameter values."""
    
    def __init__(self, parameter_name, invalid_value, expected_range):
        self.parameter_name = parameter_name
        self.invalid_value = invalid_value
        self.expected_range = expected_range
        super().__init__(f"Invalid value for parameter '{parameter_name}': {invalid_value}. Expected range: {expected_range}")

class InvalidParameterTypeError(ConfigValidationError):
    """Exception raised for invalid parameter types."""
    
    def __init__(self, parameter_name, expected_type, actual_type):
        self.parameter_name = parameter_name
        self.expected_type = expected_type
        self.actual_type = actual_type
        super().__init__(f"Invalid type for parameter '{parameter_name}': expected {expected_type}, got {actual_type}")

class InvalidParameterCombinationError(ConfigValidationError):
    """Exception raised for invalid parameter combinations."""
    
    def __init__(self, parameter_name, dependent_parameters, reason):
        self.parameter_name = parameter_name
        self.dependent_parameters = dependent_parameters
        self.reason = reason
        super().__init__(f"Invalid combination for parameter '{parameter_name}': {reason}. Dependencies: {dependent_parameters}")
```

## Conclusion

The current NLM configuration system lacks proper validation, which can lead to:

1. **Runtime failures**: Invalid configurations cause crashes or incorrect behavior
2. **Difficult debugging**: Users get cryptic errors without clear guidance
3. **Poor user experience**: Users need to understand all parameter constraints
4. **System instability**: Invalid configurations can cause resource exhaustion
5. **Reduced trust**: Users may not trust the system without proper validation

Implementing comprehensive configuration validation will:

1. **Improve reliability**: Invalid configurations are caught early
2. **Enhance user experience**: Clear error messages guide users to correct configurations
3. **Enable better defaults**: Validated default configurations ensure stable operation
4. **Reduce support burden**: Fewer bugs mean less user support needed
5. **Increase confidence**: Proper validation shows the system is robust and well-designed

The implementation should be prioritized based on impact and effort, with basic validation as a minimum requirement and production-grade validation as the ultimate goal.