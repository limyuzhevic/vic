# Configuration Management Improvements for NLM

## Current Issues Identified

### 1. Configuration Complexity
- Current: String keys with dot notation (e.g., "brain.neuron_count")
- Problem: Unintuitive for users, requires knowledge of exact parameter names
- Impact: High barrier to entry for new users

### 2. Limited Configuration Options
- Current: Manual parameter setting required
- Problem: No sensible defaults, no configuration templates
- Impact: Inconsistent behavior across users

### 3. Missing Validation
- Current: No schema validation or type checking
- Problem: Runtime errors when invalid values are used
- Impact: Poor developer experience, difficult debugging

### 4. No Presets
- Current: Configuration must be manually created
- Problem: No built-in configurations for common use cases
- Impact: Duplicate configuration code across projects

### 5. Missing Environment Support
- Current: Cannot load configuration from environment variables
- Problem: Inflexible deployment scenarios
- Impact: Limited use in containerized environments

### 6. Poor Developer Experience
- Current: Complex parameter discovery
- Problem: Users don't know what parameters are available
- Impact: Underutilized features, inefficient development

## Proposed Improvements

### 1. Enhanced Configuration API

#### Pythonic Configuration Access
```python
# Current: brain.getTotalNeuronCount()
# Proposed: brain.neuron_count
brain = pynlm.createBrain()
brain.neuron_count = 1000  # Automatic validation
print(f"Current neurons: {brain.neuron_count}")

# Nested attribute access
config = pynlm.createDefaultConfig()
config.neural_plasticity.stdp.learning_rate = 0.01
config.development.curiosity.enabled = True
```

#### Configuration Validation
```python
# Automatic type and range validation
brain.plasticity_strength = 1.5  # Raises ValueError if > 1.0

# Custom validation callbacks
brain.set_validation_callback('neuron_count', lambda x: x > 0)
```

#### Configuration Groups
```python
# Logical grouping of related parameters
config = pynlm.createConfig('learning')
config.learning.rate = 0.001
config.learning.momentum = 0.9
config.training.epochs = 100
config.training.batch_size = 32
```

### 2. Configuration Presets

#### Built-in Configuration Templates
```python
# High-performance configuration
performance_config = pynlm.createPreset('performance')
performance_config.neural.neuron_count = 5000
performance_config.plasticity.enable = True
performance_config.performance.optimizations = 'aggressive'

# Learning configuration  
learning_config = pynlm.createPreset('learning')
learning_config.neural.development.enable = True
learning_config.neural.curiosity.enabled = True
learning_config.memory.episodic.enable = True

# Fast testing configuration
test_config = pynlm.createPreset('test')
config.neural.neuron_count = 100
config.simulation.steps = 1000
config.performance.debug = True
```

### 3. Environment Variable Support
```python
# Environment variable loading
import os

config = pynlm.createConfig()
config.loadFromEnv('NLM_', prefix_validation=True)

# Example environment variables:
# NLM_NEURON_COUNT=2000
# NLM_SIMULATION_STEPS=5000
# NLM_ENABLE_LEARNING=true
```

### 4. Configuration Validation and Schema
```python
# Configuration schema definition
config_schema = {
    'neuron_count': {'type': int, 'min': 1, 'max': 100000},
    'plasticity': {
        'type': dict,
        'schema': {
            'enable': {'type': bool},
            'learning_rate': {'type': float, 'min': 0.0, 'max': 1.0},
            'hebbian_weight': {'type': float, 'min': 0.0, 'max': 1.0}
        }
    },
    'simulation': {
        'steps': {'type': int, 'min': 1},
        'timestep': {'type': float, 'min': 0.0001, 'max': 1.0}
    }
}

# Apply schema
config.setSchema(config_schema)

# Validate configuration
try:
    config.validate()
    print("Configuration is valid!")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

### 5. Enhanced Error Handling
```python
# Better error messages with context
brain = pynlm.createBrain(config)

try:
    brain.simulate(1000)  # Non-existent method
except AttributeError as e:
    print(f"Method not found: {e}. Available methods: {brain.getAvailableMethods()}")

# Parameter validation with helpful messages
try:
    brain.neuron_count = -100  # Invalid
except ValueError as e:
    print(f"Neuron count must be positive. Got: -100")

# Configuration loading errors
config = pynlm.createConfig()
try:
    config.loadFromFile('nonexistent.json')
except FileNotFoundError as e:
    print(f"Configuration file not found: {e}. Using defaults.")
```

### 6. Configuration Documentation
```python
# Automatic configuration documentation
doc = config.getDocumentation()
print(doc)

# Configuration parameter info
param_info = config.getParameterInfo('brain.neuron_count')
print(f"Parameter: {param_info.name}")
print(f"Type: {param_info.type}")
print(f"Default: {param_info.default_value}")
print(f"Description: {param_info.description}")
print(f"Valid range: {param_info.min} - {param_info.max}")
```

### 7. Configuration Copy and Merge
```python
# Configuration copying
base_config = pynlm.createDefaultConfig()
custom_config = base_config.copy()
custom_config.neural.neuron_count = 2000

# Configuration merging
config1 = pynlm.createConfig('fast')
config2 = pynlm.createConfig('thorough')
merged_config = config1.merge(config2)  # config2 overrides config1

# Configuration comparison
if config1 != config2:
    print("Configurations differ:")
    print(f"Config1 neurons: {config1.neural.neuron_count}")
    print(f"Config2 neurons: {config2.neural.neuron_count}")
```

### 8. Configuration Export and Import
```python
# Export configuration to structured formats
config.exportToJSON('my_config.json')
config.exportToYAML('my_config.yaml')
config.exportToDict()  # Returns Python dict

# Import configuration
config = pynlm.createConfig()
config.importFromJSON('saved_config.json')
config.importFromDict({
    'neural': {'neuron_count': 1000},
    'simulation': {'steps': 5000}
})
```

### 9. Advanced Configuration Features

#### Configuration Templates
```python
# Create reusable configuration templates
pynlm.createConfigTemplate('research', {
    'description': 'Configuration for research experiments',
    'neural': {'neuron_count': 10000, 'plasticity': {'enable': True}},
    'simulation': {'steps': 100000, 'timestep': 0.001},
    'memory': {'episodic': {'enable': True, 'capacity': 1000}},
    'learning': {'curiosity': {'enabled': True, 'rate': 0.01}}
})

# Use template
research_config = pynlm.getTemplate('research')
```

#### Configuration Validation Rules
```python
# Define custom validation rules
config.addValidationRule('neuron_count', 
    lambda x: x > 0 and x <= 100000,
    "Neuron count must be between 1 and 100,000"
)

config.addValidationRule('plasticity.learning_rate',
    lambda x: 0.0 <= x <= 1.0,
    "Learning rate must be between 0.0 and 1.0"
)
```

## Implementation Benefits

### 1. Developer Experience
- **Intuitive API**: Natural Python syntax instead of verbose method calls
- **Better documentation**: Automatic parameter descriptions and examples
- **Helpful errors**: Context-rich error messages with suggestions
- **Easy exploration**: Configuration discovery and parameter info

### 2. Productivity
- **Templates**: Pre-configured setups for common use cases
- **Presets**: One-click configuration for performance/accuracy tradeoffs
- **Environment support**: Flexible deployment configurations
- **Import/export**: Easy configuration sharing and persistence

### 3. Reliability
- **Validation**: Automatic type and range checking
- **Schema support**: Structured configuration validation
- **Error recovery**: Graceful handling of missing files and invalid values
- **Consistency**: Standardized configuration management

### 4. Performance
- **Optimized presets**: Pre-tuned configurations for different use cases
- **Memory efficiency**: Lightweight configuration objects
- **Fast validation**: Quick parameter checking during simulation
- **Cached configurations**: Reuse validated configurations

## Backward Compatibility

The enhanced configuration system maintains full backward compatibility:

```python
# Old code still works
config = pynlm.createDefaultConfig()
config.set('brain.neuron_count', 1000)
value = config.get('brain.neuron_count')
keys = config.getKeys()
config.loadFromFile('config.json')

# New Pythonic code available
config.neural.neuron_count = 1000  # Or uses old API if needed
```

## Migration Path

### For New Projects
- Use the new Pythonic API immediately
- Leverage presets and templates
- Take advantage of automatic validation

### For Existing Projects
- Gradually migrate to Pythonic syntax
- Add validation to prevent configuration errors
- Implement configuration templates for consistency
- Use import/export for configuration management

## Testing and Validation

```python
# Configuration validation tests
def test_configuration_validation():
    config = pynlm.createDefaultConfig()
    
    # Test valid configuration
    config.neural.neuron_count = 1000
    assert config.isValid()
    
    # Test invalid configuration
    try:
        config.neural.neuron_count = -100
        assert False, "Should have raised ValueError"
    except ValueError:
        pass  # Expected
    
    # Test configuration schema
    schema = {'neuron_count': {'type': int, 'min': 1}}
    config.setSchema(schema)
    assert config.isValid()
    
    # Test validation errors
    config.setSchema({'neuron_count': {'type': int, 'min': 100}})
    try:
        config.neural.neuron_count = 50
        assert False, "Should have raised validation error"
    except pynlm.ConfigurationError:
        pass  # Expected
```

This enhanced configuration management system provides a significant improvement in developer experience, reliability, and productivity while maintaining full backward compatibility with existing code.