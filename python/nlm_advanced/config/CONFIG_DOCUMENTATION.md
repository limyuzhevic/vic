# Enhanced Configuration System Documentation

## Overview

The Enhanced Configuration System is a comprehensive configuration management system for NLM neural simulations. It provides advanced features beyond the original configuration system, including type safety, JSON/YAML support, template systems, and automated parameter optimization.

## Key Features

### Type Safety and Validation

The enhanced configuration system provides robust type safety with automatic conversion and validation:

1. **Automatic Type Conversion**: Seamless conversion between Python and NLM types
2. **Comprehensive Validation**: Multiple validation rules and type checking
3. **Error Handling**: Detailed error messages with type information
4. **Backward Compatibility**: Maintains compatibility with existing configurations

### Configuration Templates

The template system allows for reusable configuration patterns:

1. **Template Inheritance**: Templates can inherit from other templates
2. **Parameter Search**: Automated hyperparameter optimization
3. **Template Application**: Easy application of templates to simulations
4. **Template Validation**: Comprehensive validation of templates

### JSON and YAML Support

Multiple configuration file formats are supported:

1. **JSON Files**: Simple, human-readable configuration files
2. **YAML Files**: Rich, structured configuration files
3. **Mixed Formats**: Support for both formats
4. **Configuration Loading**: Easy loading of configuration files

### Parameter Search and Optimization

Advanced parameter search capabilities:

1. **Grid Search**: Systematic exploration of parameter space
2. **Random Search**: Random sampling of parameter space
3. **Bayesian Optimization**: Intelligent parameter optimization
4. **Genetic Algorithms**: Evolutionary approach to parameter optimization

## Configuration API

### Core Classes

#### ConfigTemplate

Configuration templates for neural simulations.

```python
from nlm_advanced.config import ConfigTemplate

# Create a configuration template
template = ConfigTemplate(
    name="basic_neural",
    description="Basic neural network configuration",
    parameters={
        "learning_rate": {"type": "float", "required": True, "default": 0.01},
        "batch_size": {"type": "int", "required": True, "default": 32},
        "hidden_layers": {"type": "int", "default": 2},
        "activation_function": {"type": "string", "default": "relu"}
    },
    tags=["basic", "neural"],
    author="NLM Team",
    version="1.0"
)
```

#### ParameterSearcher

Parameter search and optimization tools.

```python
from nlm_advanced.config import ParameterSearcher

# Create parameter searcher
def brain_factory(params):
    # Your brain creation logic here
    return create_brain(params)

config = createDefaultConfig()
searcher = ParameterSearcher(brain_factory, config)

# Perform grid search
parameter_grid = {
    "learning_rate": [0.001, 0.01, 0.1],
    "batch_size": [16, 32, 64],
    "hidden_layers": [1, 2, 3]
}

results = searcher.manual_search(parameter_grid)
for result in results:
    print(f"Params: {result.parameter_set}, Metric: {result.metric}, Fitness: {result.fitness_score}")
```

#### HyperparameterTuner

Automated hyperparameter tuning.

```python
from nlm_advanced.config import HyperparameterTuner

# Create hyperparameter tuner
tuner = HyperparameterTuner(brain_factory, config)

# Tune hyperparameters
base_config = createDefaultConfig()
hyperparameter_space = {
    "learning_rate": (0.001, 0.1),
    "dropout_rate": (0.1, 0.5),
    "hidden_size": (64, 512)
}

results = tuner.tune_hyperparameters(
    base_config=base_config,
    hyperparameter_space=hyperparameter_space,
    optimization_method="bayesian",
    brain_factory=brain_factory
)

print(f"Best configuration: {results['best_configuration']}")
print(f"Best metric: {results['best_metric']}")
```

### Configuration Operations

#### Loading Configuration

```python
from nlm_advanced.config import ConfigTemplate
import yaml

# Load configuration from JSON
with open("config.json", "r") as f:
    config_data = json.load(f)

# Load configuration from YAML
with open("config.yaml", "r") as f:
    config_data = yaml.safe_load(f)

# Create configuration from data
config = createDefaultConfig()
for key, value in config_data.items():
    config.set(key, value)
```

#### Configuration Validation

```python
from nlm_advanced.config import ConfigValidator

# Create configuration validator
validator = ConfigValidator()

# Add validation rules
validator.add_validation_rule({
    "type": "range",
    "parameter": "learning_rate",
    "min": 0.0,
    "max": 1.0
})

validator.add_validation_rule({
    "type": "dependency",
    "parameter": "use_gpu",
    "depends_on": "gpu_available",
    "required_value": True
})

# Validate configuration
validation_result = validator.validate_config(config)

if not validation_result['valid']:
    print("Configuration validation errors:")
    for error in validation_result['errors']:
        print(f"  - {error}")
```

#### Configuration Export

```python
from nlm_advanced.config import ConfigTemplate

# Save configuration to JSON
config.save_to_file("config.json", format="json")

# Save configuration to YAML
config.save_to_file("config.yaml", format="yaml")

# Export configuration to dictionary
config_dict = config.to_dict()
```

## Advanced Configuration Features

### Template Inheritance

Templates can inherit from other templates:

```python
# Create base template
base_template = ConfigTemplate(
    name="base_config",
    parameters={"learning_rate": 0.01, "batch_size": 32}
)

# Create derived template
advanced_template = ConfigTemplate(
    name="advanced_config",
    description="Advanced neural configuration",
    parameters={
        "hidden_layers": 2,
        "dropout_rate": 0.5,
        "use_batch_norm": True
    },
    inheritance=["base_config"]
)

# Apply template with overrides
config_data = advanced_template.apply_template("advanced_config", {"learning_rate": 0.1})
```

### Parameter Search Methods

#### Manual Grid Search

```python
results = searcher.manual_search(parameter_grid)
```

#### Random Search

```python
results = searcher.random_search(parameter_ranges, n_trials=100)
```

#### Bayesian Optimization

```python
results = searcher.bayesian_optimization(parameter_bounds, n_iterations=50)
```

#### Genetic Algorithm

```python
results = searcher.genetic_algorithm(
    parameter_ranges=parameter_ranges,
    population_size=20,
    generations=50
)
```

### Configuration Analysis

```python
# Analyze parameter search results
analysis = searcher.analyze_search_results(results)

print(f"Best metric: {analysis['summary']['best_metric']}")
print(f"Average fitness: {analysis['summary']['average_fitness']}")
print(f"Optimal parameters: {analysis['optimal_configuration']['parameters']}")

# Generate recommendations
recommendations = analysis['recommendations']
for recommendation in recommendations:
    print(f"Recommendation: {recommendation}")
```

## Configuration Best Practices

### 1. Use Templates for Common Configurations

```python
# Create template for common configurations
model_template = ConfigTemplate(
    name="simple_model",
    parameters={
        "architecture": "mlp",
        "input_size": 784,
        "output_size": 10,
        "hidden_layers": 2,
        "hidden_size": 128
    }
)

# Apply template to multiple simulations
for learning_rate in [0.001, 0.01, 0.1]:
    config = model_template.apply_template("simple_model", {"learning_rate": learning_rate})
    run_simulation(config)
```

### 2. Validate Configuration Before Use

```python
# Validate configuration before simulation
if not config.validate():
    print("Configuration is invalid. Please check the following errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")
    sys.exit(1)

# Run simulation with validated configuration
run_simulation(config)
```

### 3. Use Parameter Search for Optimization

```python
# Use parameter search to find optimal configuration
searcher = ParameterSearcher(brain_factory, config)

# Define parameter ranges
parameter_ranges = {
    "learning_rate": (0.001, 0.1),
    "batch_size": (16, 128),
    "hidden_size": (64, 512)
}

# Perform random search
results = searcher.random_search(parameter_ranges, n_trials=200)

# Get best configuration
best_result = max(results, key=lambda x: x.fitness_score)
print(f"Best configuration: {best_result.parameter_set}")
print(f"Best fitness score: {best_result.fitness_score}")
```

### 4. Export Results for Analysis

```python
# Export parameter search results
searcher.export_search_results("search_results.json", results)

# Import results for analysis
imported_results = ParameterSearcher.load_search_results("search_results.json")
```

## Migration Guide

### From Basic Configuration to Enhanced Configuration

The enhanced configuration system maintains backward compatibility while adding new features:

#### Old Configuration Approach

```python
# Old approach: basic configuration
config = {
    "learning_rate": 0.01,
    "batch_size": 32,
    "hidden_layers": 2,
    "use_gpu": False
}

# Use configuration
config_value = config["learning_rate"]
```

#### New Enhanced Configuration Approach

```python
# New approach: enhanced configuration with type safety
config = createDefaultConfig()
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
config.set("hidden_layers", 2)
config.set("use_gpu", False)

# Use configuration with automatic type conversion
config_value = config["learning_rate"]  # Automatically converted to float
print(f"Learning rate: {config_value} (type: {type(config_value).__name__})")
```

#### Key Differences

1. **Type Safety**: Enhanced configuration provides automatic type conversion and validation
2. **Validation**: Enhanced configuration includes comprehensive validation
3. **Templates**: Enhanced configuration supports reusable templates
4. **Parameter Search**: Enhanced configuration includes automated parameter search
5. **Backward Compatibility**: Enhanced configuration maintains compatibility with existing code

### Migration Examples

#### Example 1: Configuration Validation

```python
# Old approach: manual validation
if "learning_rate" in config and not isinstance(config["learning_rate"], (int, float)):
    print("Error: learning_rate must be a number")
    sys.exit(1)

# New approach: automatic validation
config = createDefaultConfig()
config.set("learning_rate", 0.01)

if not config.validate():
    print("Configuration validation errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")
    sys.exit(1)
```

#### Example 2: Configuration Templates

```python
# Old approach: duplicate configurations
config1 = {"learning_rate": 0.01, "batch_size": 32, "hidden_layers": 2}
config2 = {"learning_rate": 0.001, "batch_size": 64, "hidden_layers": 3}

# New approach: template-based configurations
from nlm_advanced.config import ConfigTemplate

model_template = ConfigTemplate(
    name="basic_model",
    parameters={
        "learning_rate": 0.01,
        "batch_size": 32,
        "hidden_layers": 2
    }
)

# Apply template with variations
config1 = model_template.apply_template("basic_model", {"learning_rate": 0.01})
config2 = model_template.apply_template("basic_model", {"learning_rate": 0.001, "batch_size": 64})
```

#### Example 3: Parameter Search

```python
# Old approach: manual parameter testing
best_config = {"learning_rate": 0.01, "batch_size": 32}
best_score = run_experiment(best_config)

for learning_rate in [0.001, 0.01, 0.1]:
    config = best_config.copy()
    config["learning_rate"] = learning_rate
    score = run_experiment(config)
    if score > best_score:
        best_score = score
        best_config = config

# New approach: automated parameter search
from nlm_advanced.config import ParameterSearcher

searcher = ParameterSearcher(brain_factory, config)
parameter_ranges = {
    "learning_rate": (0.001, 0.1),
    "batch_size": (16, 128),
    "hidden_layers": (1, 5)
}

results = searcher.random_search(parameter_ranges, n_trials=100)
best_result = max(results, key=lambda x: x.fitness_score)

print(f"Best configuration: {best_result.parameter_set}")
print(f"Best fitness score: {best_result.fitness_score}")
```

## Performance Considerations

### 1. Use Templates for Repeated Configurations

```python
# Good: Use templates for repeated configurations
model_template = ConfigTemplate(name="model", parameters={...})
for learning_rate in learning_rates:
    config = model_template.apply_template("model", {"learning_rate": learning_rate})
    run_simulation(config)
```

### 2. Validate Configuration Before Simulation

```python
# Good: Validate configuration before simulation
if not config.validate():
    print("Configuration is invalid. Exiting.")
    sys.exit(1)

run_simulation(config)
```

### 3. Use Parameter Search Wisely

```python
# Good: Use parameter search for optimization
searcher = ParameterSearcher(brain_factory, config)
results = searcher.random_search(parameter_ranges, n_trials=50)
```

### 4. Export Results for Analysis

```python
# Good: Export results for future analysis
searcher.export_search_results("results.json", results)
```

## Common Issues and Solutions

### Issue 1: Configuration Validation Errors

**Symptoms**: Configuration validation fails with errors about missing or invalid parameters.

**Solution**: Check validation errors and fix configuration parameters:

```python
from nlm_advanced.config import ConfigValidator

validator = ConfigValidator()
validator.add_validation_rule({
    "type": "range",
    "parameter": "learning_rate",
    "min": 0.0,
    "max": 1.0
})

validation_result = validator.validate_config(config)
if not validation_result['valid']:
    for error in validation_result['errors']:
        print(f"Error: {error}")
```

### Issue 2: Type Conversion Errors

**Symptoms**: Type conversion errors when accessing configuration values.

**Solution**: Use proper type conversion:

```python
config = createDefaultConfig()
config.set("learning_rate", 0.01)  # Set as float

# Access with automatic type conversion
learning_rate = config["learning_rate"]  # Returns float
print(f"Learning rate: {learning_rate} (type: {type(learning_rate).__name__})")
```

### Issue 3: Parameter Search Performance

**Symptoms**: Parameter search takes too long or consumes too much memory.

**Solution**: Use efficient parameter search methods:

```python
# Use random search instead of grid search for large parameter spaces
results = searcher.random_search(parameter_ranges, n_trials=50)

# Use early stopping for long-running optimizations
results = searcher.random_search(parameter_ranges, n_trials=50, early_stopping=True)
```

## Conclusion

The enhanced configuration system provides powerful features for NLM neural simulations:

1. **Type Safety**: Automatic type conversion and validation
2. **Template System**: Reusable configuration templates
3. **Parameter Search**: Automated hyperparameter optimization
4. **Validation**: Comprehensive validation rules
5. **Backward Compatibility**: Maintains compatibility with existing code

By using the enhanced configuration system, you can:

- Write cleaner, more maintainable configuration code
- Use templates for common configurations
- Automate parameter search and optimization
- Validate configurations before simulation
- Export results for future analysis

Start using the enhanced configuration system today to improve your NLM neural simulations!
