# NLM Schema Validation

## Overview

Validation rules and algorithms for NLM configuration parameters and command-line arguments.

## Configuration Validation

### Validation Functions

#### validate_parameter_value(parameter_name, value)
Validate that a configuration parameter value is acceptable.

**Parameters**:
- `parameter_name` (str): Name of parameter to validate
- `value`: Value to validate

**Returns**:
- bool: True if valid, False otherwise

**Examples**:
```python
# Validate positive integer
validate_parameter_value("random_seed", 42)  # True
validate_parameter_value("random_seed", -1)  # False

# Validate positive float
validate_parameter_value("simulation_timestep", 0.001)  # True
validate_parameter_value("simulation_timestep", -0.001)  # False

# Validate string enum
validate_parameter_value("learning_algorithm", "stdp")  # True
validate_parameter_value("learning_algorithm", "invalid")  # False
```

#### validate_configuration(config)
Validate entire configuration object.

**Parameters**:
- `config` (Config): Configuration to validate

**Returns**:
- tuple: (bool, list) - (is_valid, list_of_errors)

**Examples**:
```python
errors = []
if not validate_configuration(config, errors):
    for error in errors:
        print(f"Validation error: {error}")
```

### Parameter-Specific Validators

#### Integer Validators
```python
def validate_int64(value, param_name=""):
    """Validate int64 value"""
    return isinstance(value, int) and not isinstance(value, bool)

def validate_positive_int(value, param_name=""):
    """Validate positive int64 value"""
    return validate_int64(value, param_name) and value > 0

def validate_non_negative_int(value, param_name=""):
    """Validate non-negative int64 value"""
    return validate_int64(value, param_name) and value >= 0
```

#### Float Validators
```python
def validate_double(value, param_name=""):
    """Validate double value"""
    return isinstance(value, (int, float)) and not isinstance(value, bool)

def validate_positive_double(value, param_name=""):
    """Validate positive double value"""
    return validate_double(value, param_name) and value > 0.0

def validate_percentage(value, param_name=""):
    """Validate percentage value (0.0 to 1.0)"""
    return validate_double(value, param_name) and 0.0 <= value <= 1.0
```

#### String Validators
```python
def validate_string(value, param_name=""):
    """Validate string value"""
    return isinstance(value, str)

def validate_string_enum(value, allowed_values, param_name=""):
    """Validate string enum value"""
    return validate_string(value, param_name) and value in allowed_values

def validate_non_empty_string(value, param_name=""):
    """Validate non-empty string value"""
    return validate_string(value, param_name) and len(value) > 0
```

### Validation Error Messages

#### Standard Error Format
```python
ERROR_FORMAT = "Config parameter '{param}': {message}"

def format_validation_error(param_name, error_message):
    return ERROR_FORMAT.format(param=param_name, message=error_message)
```

#### Common Error Messages
```python
ERRORS = {
    "invalid_type": "must be {expected_type}, got {actual_type}",
    "out_of_range": "must be {min} to {max}, got {actual}",
    "missing_required": "is required but missing",
    "invalid_value": "has invalid value: {value}",
    "invalid_dependency": "parameter depends on {dependency}",
}
```

### Test Cases

#### Configuration Validation Test Cases
```python
# Test case structure for configuration validation
class ConfigValidationTest:
    def test_valid_configuration(self):
        """Test that valid configuration passes validation"""
        config = create_valid_configuration()
        is_valid, errors = validate_configuration(config)
        assert is_valid
        assert len(errors) == 0
    
    def test_missing_required_parameter(self):
        """Test that missing required parameter fails validation"""
        config = create_configuration_missing_param("neuron_count")
        is_valid, errors = validate_configuration(config)
        assert not is_valid
        assert any("neuron_count" in error for error in errors)
    
    def test_invalid_parameter_type(self):
        """Test that invalid parameter type fails validation"""
        config = create_configuration_with_wrong_type("neuron_count", "string")
        is_valid, errors = validate_configuration(config)
        assert not is_valid
        assert any("type" in error for error in errors)
    
    def test_out_of_range_parameter(self):
        """Test that out-of-range parameter fails validation"""
        config = create_configuration_with_out_of_range(
            "simulation_timestep", -0.001
        )
        is_valid, errors = validate_configuration(config)
        assert not is_valid
        assert any("range" in error for error in errors)
```

## CLI Validation

### CLI Argument Validation

#### validate_command(command_name)
Validate that a command exists.

**Parameters**:
- `command_name` (str): Command name to validate

**Returns**:
- bool: True if command exists

**Example**:
```python
if validate_command("brain simulate"):
    print("Command exists")
```

#### validate_command_arguments(command_name, args)
Validate command arguments.

**Parameters**:
- `command_name` (str): Command name
- `args` (list): Arguments to validate

**Returns**:
- bool: True if arguments are valid

**Example**:
```python
if validate_command_arguments("brain simulate", ["--steps", "100"]):
    print("Arguments valid")
```

### CLI Option Validators

```python
def validate_cli_option(option, value):
    """Validate CLI option and value"""
    # Option-specific validation logic
    return True

def validate_cli_file_path(path):
    """Validate file path for CLI operations"""
    import os
    if not os.path.exists(path):
        return False
    if not os.access(path, os.R_OK):
        return False
    return True
```

## Validation Integration Points

### Configuration Validation Integration

#### CLI Integration
```python
def handle_config_validation(args):
    """Handle config validation command"""
    if args.get("validate"):
        config = get_current_config()
        is_valid, errors = validate_configuration(config)
        if not is_valid:
            print("Configuration validation errors:")
            for error in errors:
                print(f"  - {error}")
        return is_valid
    return True
```

#### API Integration
```python
def validate_config_api(config):
    """API wrapper for configuration validation"""
    is_valid, errors = validate_configuration(config)
    if not is_valid:
        raise ValueError(f"Invalid configuration: {errors}")
    return is_valid
```

### CLI Validation Integration

#### Command Registration
```python
def register_validated_commands():
    """Register commands with validation"""
    commands = [
        ("config", ["list", "get", "set", "validate", "export"]),
        ("brain", ["load", "save", "reset", "info", "simulate"]),
        ("debug", ["validate", "log", "info"]),
        ("experiment", ["run", "list"]),
        ("system", ["status", "stop", "restart"]),
    ]
    
    for command_group, subcommands in commands:
        for subcommand in subcommands:
            full_command = f"{command_group} {subcommand}"
            command_registry.register(full_command, validate_command)
```

#### Validation Middleware
```python
def validate_command_middleware(command_name, args):
    """Middleware to validate commands before execution"""
    if not validate_command(command_name):
        print(f"Error: '{command_name}' is not a valid command")
        return False
    
    if not validate_command_arguments(command_name, args):
        print(f"Error: Invalid arguments for '{command_name}'")
        return False
    
    return True
```

## Validation Test Framework

### Test Configuration
```python
def create_validation_test_suite():
    """Create test suite for validation"""
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Add parameter-specific tests
    suite.addTests(unittest.makeSuite(ParameterValidationTests))
    suite.addTests(unittest.makeSuite(ConfigValidationTests))
    suite.addTests(unittest.makeSuite(CLIValidationTests))
    
    return suite
```

### Parameter Test Cases
```python
class ParameterValidationTests(unittest.TestCase):
    def test_int64_parameters(self):
        """Test all int64 parameters"""
        # Test each int64 parameter
        int64_params = [
            "random_seed", "neuron_count", "region_count", "max_simulation_steps",
            "environment_width", "environment_height", "prediction_window_size",
            "max_pending_spikes", "event_queue_size", "memory_pool_size",
            "checkpoint_format_version"
        ]
        
        for param in int64_params:
            with self.subTest(param=param):
                # Test valid values
                self.assertTrue(validate_parameter_value(param, 1))
                self.assertTrue(validate_parameter_value(param, 1000))
                self.assertFalse(validate_parameter_value(param, -1))
                self.assertFalse(validate_parameter_value(param, "string"))
    
    def test_double_parameters(self):
        """Test all double parameters"""
        double_params = [
            "simulation_timestep", "stdp_time_constant", "reward_discount_factor",
            "reward_prediction_error_gamma", "reward_prediction_error_lambda",
            "development_stage_transition_age", "working_memory_capacity",
            "episodic_memory_memory_strength", "prediction_confidence_threshold",
            "concept_formation_threshold", "self_model_confidence_decay",
            "social_learning_weight", "dopamine_sensitivity", "curiosity_decay_rate",
            "novelty_threshold", "touch_sensitivity", "vision_range",
            "motor_action_smoothness"
        ]
        
        for param in double_params:
            with self.subTest(param=param):
                # Test valid values
                self.assertTrue(validate_parameter_value(param, 0.001))
                self.assertTrue(validate_parameter_value(param, 1.0))
                self.assertFalse(validate_parameter_value(param, -0.001))
                self.assertFalse(validate_parameter_value(param, "string"))
    
    def test_float_parameters(self):
        """Test all float parameters"""
        float_params = [
            "connection_probability", "plasticity_learning_rate",
            "stdp_ltp_weight", "stdp_ltd_weight", "development_synaptogenesis_rate",
            "development_pruning_rate", "dopamine_baseline", "neuromodulation_strength",
            "learning_rate", "learning_momentum", "development_plasticity_decay_rate",
            "episodic_memory_max_episodes", "prediction_window_size",
            "prediction_confidence_threshold", "concept_formation_threshold",
            "self_model_confidence_decay", "social_learning_weight",
            "max_pending_spikes", "event_queue_size", "memory_pool_size"
        ]
        
        for param in float_params:
            with self.subTest(param=param):
                # Test valid values
                self.assertTrue(validate_parameter_value(param, 0.1))
                self.assertTrue(validate_parameter_value(param, 1.0))
                self.assertFalse(validate_parameter_value(param, -0.1))
                self.assertFalse(validate_parameter_value(param, "string"))
    
    def test_string_parameters(self):
        """Test all string parameters"""
        string_params = [
            ("environment_name", ["GridWorld", "RandomWorld", "MazeWorld"]),
            ("log_level", ["DEBUG", "INFO", "WARN", "ERROR"]),
            ("neural_connectivity_pattern", ["random", "grid", "ring", "small_world"]),
            ("learning_algorithm", ["stdp", "hebbian", "reward_modulated", "mixed"]),
            ("experiment_reward_function", ["exponential", "linear", "logarithmic"])
        ]
        
        for param, allowed_values in string_params:
            with self.subTest(param=param):
                # Test valid values
                for value in allowed_values:
                    self.assertTrue(validate_parameter_value(param, value))
                
                # Test invalid values
                self.assertFalse(validate_parameter_value(param, "invalid"))
                self.assertFalse(validate_parameter_value(param, ""))
    
    def test_bool_parameters(self):
        """Test boolean parameters"""
        bool_params = ["log_to_file", "visualization_enabled", "checkpoint_compression"]
        
        for param in bool_params:
            with self.subTest(param=param):
                self.assertTrue(validate_parameter_value(param, True))
                self.assertTrue(validate_parameter_value(param, False))
```

### CLI Test Cases
```python
class CLIValidationTests(unittest.TestCase):
    def test_valid_commands(self):
        """Test that all valid commands pass validation"""
        valid_commands = [
            "config list", "config get random_seed", "config set neuron_count 1000",
            "config validate", "config export test.cfg",
            "brain load brain.nml", "brain save brain.nml", "brain reset",
            "brain info", "brain simulate",
            "debug validate", "debug log", "debug info",
            "experiment run config.cfg", "experiment list",
            "system status", "system stop", "system restart"
        ]
        
        for command in valid_commands:
            with self.subTest(command=command):
                # Split command into name and args
                parts = command.split()
                cmd_name = parts[0]
                cmd_args = parts[1:] if len(parts) > 1 else []
                
                self.assertTrue(validate_command(cmd_name))
                self.assertTrue(validate_command_arguments(cmd_name, cmd_args))
    
    def test_invalid_commands(self):
        """Test that invalid commands fail validation"""
        invalid_commands = [
            "invalid_command",
            "config invalid_command",
            "brain invalid_command",
            "debug invalid_command"
        ]
        
        for command in invalid_commands:
            with self.subTest(command=command):
                parts = command.split()
                cmd_name = parts[0]
                
                self.assertFalse(validate_command(cmd_name))
    
    def test_cli_options_validation(self):
        """Test CLI option validation"""
        # Test --verbose option
        self.assertTrue(validate_cli_option("--verbose", "true"))
        
        # Test --help option
        self.assertTrue(validate_cli_option("--help", ""))
        
        # Test --config option
        self.assertTrue(validate_cli_option("--config", "config.cfg"))
        
        # Test invalid option
        self.assertFalse(validate_cli_option("--invalid", "value"))

## Validation Algorithms

### Configuration Validation Algorithm
```python
def validate_configuration_full(config):
    """Complete configuration validation algorithm"""
    errors = []
    
    # Get all keys from configuration
    keys = config.getKeys()
    
    # Validate each parameter
    for key in keys:
        value = config.get(key)  # Get raw value
        
        # Get parameter metadata
        param_info = get_parameter_metadata(key)
        
        # Validate type
        if not validate_parameter_type(key, value, param_info):
            errors.append(f"{key}: {param_info['error_type']}")
        
        # Validate range
        elif not validate_parameter_range(key, value, param_info):
            errors.append(f"{key}: {param_info['error_range']}")
        
        # Validate enum
        elif not validate_parameter_enum(key, value, param_info):
            errors.append(f"{key}: {param_info['error_enum']}")
        
        # Validate dependencies
        elif not validate_parameter_dependencies(key, value, param_info):
            errors.append(f"{key}: {param_info['error_dependency']}")
    
    return not errors, errors
```

### CLI Validation Algorithm
```python
def validate_cli_full(command_name, args):
    """Complete CLI validation algorithm"""
    # Check if command exists
    if not command_registry.has_command(command_name):
        return False, f"Command '{command_name}' not found"
    
    # Get command information
    command = command_registry.get_command(command_name)
    
    # Validate required arguments
    for required_arg in command.required_args:
        if not args or required_arg not in args:
            return False, f"Missing required argument: {required_arg}"
    
    # Validate option types
    for option, value in args.items():
        if option in command.options:
            if not validate_option_type(option, value, command.options[option]):
                return False, f"Invalid type for option '{option}'"
    
    return True, ""
```

## Validation Configuration

### Validation Rules Configuration
```python
VALIDATION_CONFIG = {
    "strict_mode": True,
    "warn_on_deprecation": True,
    "validate_dependencies": True,
    "validate_optional_parameters": False,
    "include_experimental_params": False,
    "error_reporting_level": "detailed"
}
```

### Parameter Validation Rules
```python
PARAMETER_VALIDATION_RULES = {
    "int64": {
        "validator": validate_int64,
        "error_template": "must be integer, got {actual_type}"
    },
    "double": {
        "validator": validate_double,
        "error_template": "must be number, got {actual_type}"
    },
    "float": {
        "validator": validate_float,
        "error_template": "must be float, got {actual_type}"
    },
    "string": {
        "validator": validate_string,
        "error_template": "must be string, got {actual_type}"
    },
    "bool": {
        "validator": validate_bool,
        "error_template": "must be boolean, got {actual_type}"
    },
    "list<int>": {
        "validator": validate_list_int,
        "error_template": "must be list of integers, got {actual_type}"
    },
    "list<double>": {
        "validator": validate_list_double,
        "error_template": "must be list of doubles, got {actual_type}"
    },
    "list<string>": {
        "validator": validate_list_string,
        "error_template": "must be list of strings, got {actual_type}"
    }
}
```

## Integration Examples

### Configuration Validation in CLI
```python
def main(args):
    """Main CLI function with validation"""
    parser = argparse.ArgumentParser()
    parser.add_argument("command")
    parser.add_argument("args", nargs='*')
    parser.add_argument("--config", help="Configuration file")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    
    parsed_args = parser.parse_args(args)
    
    # Load configuration if specified
    if parsed_args.config:
        if not validate_cli_file_path(parsed_args.config):
            print(f"Error: Cannot read configuration file: {parsed_args.config}")
            return False
        
        config = load_config(parsed_args.config)
    else:
        config = create_default_config()
    
    # Validate configuration
    if not validate_configuration(config)[0]:
        print("Configuration validation failed")
        return False
    
    # Execute command
    return execute_command(parsed_args.command, parsed_args.args)
```

### Validation in Python API
```python
def validate_config_with_api(config):
    """API wrapper for configuration validation"""
    try:
        # Check if config is valid
        is_valid, errors = validate_configuration(config)
        
        if not is_valid:
            raise ValueError(f"Invalid configuration: {errors}")
        
        return True
    except Exception as e:
        print(f"Validation error: {e}")
        return False

# Usage
if validate_config_with_api(config):
    print("Configuration is valid")
    # Proceed with simulation
else:
    print("Configuration is invalid")
    # Fix configuration
```

## Performance Considerations

### Efficient Validation
```python
def optimize_validation_performance():
    """Optimize validation performance"""
    
    # Cache validation results
    validation_cache = {}
    
    def validate_cached(key, value):
        cache_key = (key, str(value))
        if cache_key in validation_cache:
            return validation_cache[cache_key]
        
        result = validate_parameter_value(key, value)
        validation_cache[cache_key] = result
        return result
    
    # Batch validation
    def validate_batch(config):
        """Validate configuration in batch"""
        errors = []
        
        for key, value in config.items():
            is_valid = validate_cached(key, value)
            if not is_valid:
                errors.append(f"{key}: validation failed")
        
        return not errors, errors
    
    return validate_batch
```

## Validation Testing Framework

### Automated Validation Testing
```python
def run_validation_tests():
    """Run comprehensive validation tests"""
    
    test_results = {
        "parameter_validation": run_parameter_tests(),
        "cli_validation": run_cli_tests(),
        "integration_tests": run_integration_tests(),
        "performance_tests": run_performance_tests()
    }
    
    # Generate report
    report = generate_validation_report(test_results)
    
    # Save report
    with open("validation_report.txt", "w") as f:
        f.write(report)
    
    return test_results

if __name__ == "__main__":
    results = run_validation_tests()
    print(f"Validation tests completed: {results}")
```

---
*Validation documentation generated from source code analysis*
