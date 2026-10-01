# NLM Tutorial 2: Brain Configuration

This tutorial provides step-by-step instructions for brain configuration and customization in NLM (Neural Learning Machine). It covers:

- **Brain Configuration Overview**: Brain configuration overview and concepts
- **Brain Configuration Basics**: Basic brain configuration
- **Brain Configuration Details**: Detailed brain configuration options
- **Brain Configuration Examples**: Configuration examples and use cases
- **Brain Configuration Testing**: Configuration testing and validation
- **Brain Configuration Optimization**: Configuration optimization strategies

## 🎯 Why Brain Configuration Matters

Brain configuration is crucial for NLM because:

1. **System Control**: Configuration controls NLM system behavior
2. **Performance**: Configuration affects system performance
3. **Learning**: Configuration affects learning capabilities
4. **Adaptation**: Configuration enables system adaptation
5. **Customization**: Configuration enables system customization

## 🧠 Brain Configuration Overview

### Brain Configuration Overview

NLM brain configuration provides a framework for configuring NLM brain behavior:

1. **Configuration System**: Hierarchical configuration system
2. **Configuration Parameters**: Parameters for brain configuration
3. **Configuration Options**: Options for brain configuration
4. **Configuration Validation**: Validation of configuration parameters
5. **Configuration Optimization**: Optimization of configuration parameters

### Brain Configuration Structure

NLM brain configuration structure:

```
brain
├── neuron_count                 # Total number of neurons
├── synapse_density               # Synapse connectivity density
├── connection_probability       # Probability of connection between neurons
├── initial_weight_mean          # Mean initial synaptic weight
├── initial_weight_std           # Standard deviation of initial weights
├── v_thresh                    # Neuron threshold potential (mV)
├── v_rest                      # Resting potential (mV)
├── v_reset                     # Reset potential after spike (mV)
├── tau_mem                     # Membrane time constant (ms)
├── tau_ref                     # Refractory period (ms)
├── c_m                         # Membrane capacitance (nF)
├── g_L                         # Leak conductance (nS)
├── region_count                # Number of brain regions
├── population_size             # Average population size
├── spatial_radius             # Spatial extent (mm)
├── spatial_exponent           # Spatial connection decay
└── ...                        # Additional configuration options
```

### Brain Configuration Types

Brain configuration types:

1. **Basic Configuration**: Basic brain configuration
2. **Advanced Configuration**: Advanced brain configuration
3. **Custom Configuration**: Custom brain configuration
4. **Optimized Configuration**: Optimized brain configuration
5. **Research Configuration**: Research brain configuration

## ⚙️ Brain Configuration Basics

### Create Basic Configuration

Create basic brain configuration:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

print("Default configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
print(f"  STDP enabled: {config.getBool('plasticity.stdp.enable')}")
```

### Modify Basic Configuration

Modify basic brain configuration:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify neuron count
config.set("brain.neuron_count", 1000)

# Modify synapse density
config.set("brain.synapse_density", 0.1)

# Enable STDP
config.set("plasticity.stdp.enable", True)

# Print configuration
print("Modified configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
print(f"  STDP enabled: {config.getBool('plasticity.stdp.enable')}")
```

### Save Configuration

Save configuration to file:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)

# Save configuration
config.saveToFile("my_config.json")

print("✅ Configuration saved to my_config.json")
```

### Load Configuration

Load configuration from file:

```python
import pynlm

# Load configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")

print("Loaded configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
```

## 📋 Brain Configuration Details

### Brain Configuration Parameters

Brain configuration parameters:

#### Neuron Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `brain.neuron_count` | int | 1000 | Total number of neurons | 100-100000 |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density | 0.01-0.5 |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons | 0.001-0.2 |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight | 0.1-1.0 |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights | 0.01-0.5 |

#### Neuron Dynamics Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) | -100 to 0 |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) | -90 to -60 |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) | -85 to -65 |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) | 1-100 |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) | 0.5-10 |
| `brain.c_m` | float | 1.0 | Membrane capacitance (nF) | 0.1-10 |
| `brain.g_L` | float | 10.0 | Leak conductance (nS) | 1-100 |

#### Network Structure Parameters

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `brain.region_count` | int | 1 | Number of brain regions | 1-10 |
| `brain.population_size` | int | 100 | Average population size | 10-1000 |
| `brain.spatial_radius` | float | 10.0 | Spatial extent (mm) | 1-100 |
| `brain.spatial_exponent` | float | 2.0 | Spatial connection decay | 1.0-3.0 |

### Brain Configuration Options

Brain configuration options:

#### Plasticity Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `plasticity.stdp.enable` | bool | true | Enable STDP | - |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | 0.0001-0.01 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation (ms) | 5-50 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression (ms) | 5-50 |
| `plasticity.stdp.max_weight` | float | 1.0 | Maximum synaptic weight | 0.5-2.0 |
| `plasticity.stdp.min_weight` | float | 0.0 | Minimum synaptic weight | 0.0-0.5 |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description | Range |
|-----------|------|---------|-------------|-------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.5-2.0 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | - |
| `neuromod.curiosity.novelty_sensitivity` | float | 1.0 | Sensitivity to novel stimuli | 0.5-2.0 |
| `neuromod.curiosity.exploration_bonus` | float | 0.1 | Bonus for exploration behavior | 0.01-0.5 |
| `neuromod.curiosity.exploration_cost` | float | 0.01 | Cost of exploration | 0.001-0.1 |

### Brain Configuration Examples

#### Example 1: Basic Configuration

Basic brain configuration:

```python
import pynlm

# Create basic configuration
config = pynlm.createDefaultConfig()

# Modify basic parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

print("Basic configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
```

#### Example 2: High Performance Configuration

High performance brain configuration:

```python
import pynlm

# Create configuration for high performance
config = pynlm.createDefaultConfig()

# Optimize for performance
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("memory.working.enable", True)
config.set("performance.memory_pools.enable", True)
config.set("visualization.enabled", False)

print("High performance configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
```

#### Example 3: Research Configuration

Research brain configuration:

```python
import pynlm

# Create configuration for research
config = pynlm.createDefaultConfig()

# Optimize for research
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.3)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("memory.working.enable", True)
config.set("memory.episodic.enable", True)
config.set("memory.semantic.enable", True)
config.set("development.maturation.enable", True)
config.set("visualization.enabled", True)

print("Research configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
```

### Brain Configuration Testing

#### Test Basic Configuration

Test basic brain configuration:

```python
import pynlm

def test_basic_configuration():
    """Test basic configuration"""
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Test basic parameters
    assert config.getInt('brain.neuron_count') == 1000
    assert config.getFloat('brain.synapse_density') == 0.1
    assert config.getBool('plasticity.stdp.enable') == True
    
    print("✅ Basic configuration test passed")

# Run test
test_basic_configuration()
```

#### Test Modified Configuration

Test modified brain configuration:

```python
import pynlm

def test_modified_configuration():
    """Test modified configuration"""
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Modify configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.2)
    
    # Test modified parameters
    assert config.getInt('brain.neuron_count') == 2000
    assert config.getFloat('brain.synapse_density') == 0.2
    
    print("✅ Modified configuration test passed")

# Run test
test_modified_configuration()
```

#### Test Configuration Validation

Test configuration validation:

```python
import pynlm

def test_configuration_validation():
    """Test configuration validation"""
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Test validation
    assert config.isValid()
    
    print("✅ Configuration validation test passed")

# Run test
test_configuration_validation()
```

### Brain Configuration Optimization

#### Optimize for Performance

Optimize brain configuration for performance:

```python
import pynlm

def optimize_for_performance(config):
    """Optimize configuration for performance"""
    # Optimize neuron count for performance
    config.set("brain.neuron_count", 1000)
    
    # Optimize synapse density for performance
    config.set("brain.synapse_density", 0.1)
    
    # Enable memory pools
    config.set("performance.memory_pools.enable", True)
    config.set("performance.memory_pools.neuron_pool_size", 10000)
    
    # Disable visualization for performance
    config.set("visualization.enabled", False)
    
    return config

# Optimize configuration
config = pynlm.createDefaultConfig()
optimized_config = optimize_for_performance(config)

print("✅ Configuration optimized for performance")
```

#### Optimize for Accuracy

Optimize brain configuration for accuracy:

```python
import pynlm

def optimize_for_accuracy(config):
    """Optimize configuration for accuracy"""
    # Optimize neuron count for accuracy
    config.set("brain.neuron_count", 10000)
    
    # Optimize synapse density for accuracy
    config.set("brain.synapse_density", 0.2)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Enable all memory systems
    config.set("memory.working.enable", True)
    config.set("memory.episodic.enable", True)
    config.set("memory.semantic.enable", True)
    
    # Enable development
    config.set("development.maturation.enable", True)
    
    # Enable visualization for debugging
    config.set("visualization.enabled", True)
    
    return config

# Optimize configuration
config = pynlm.createDefaultConfig()
optimized_config = optimize_for_accuracy(config)

print("✅ Configuration optimized for accuracy")
```

## 📊 Brain Configuration Metrics

### Configuration Size Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Configuration Size** | < 10 MB | - |
| **Parameter Count** | < 100 | - |
| **Configuration Depth** | < 5 levels | - |

### Configuration Validation Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Validation Speed** | < 1ms | - |
| **Validation Accuracy** | > 95% | - |
| **Error Detection** | > 90% | - |

### Configuration Performance Metrics

| Metric | Target | Current |
|--------|--------|---------|
| **Configuration Loading** | < 10ms | - |
| **Configuration Saving** | < 10ms | - |
| **Configuration Modification** | < 1ms | - |

## 🏆 Brain Configuration Success Criteria

Brain configuration is successful when:

1. **Configuration Correctness**: Configuration parameters are correct
2. **Configuration Validation**: Configuration is valid
3. **Configuration Performance**: Configuration performance is acceptable
4. **Configuration Optimization**: Configuration is optimized
5. **Configuration Testing**: Configuration is tested
6. **Configuration Documentation**: Configuration is documented

## 📚 Brain Configuration Summary

This completes Brain Configuration Tutorial 2. It provides comprehensive step-by-step instructions for brain configuration and customization, including:

- **Brain Configuration Overview**: Brain configuration overview and concepts
- **Brain Configuration Basics**: Basic brain configuration
- **Brain Configuration Details**: Detailed brain configuration options
- **Brain Configuration Examples**: Configuration examples and use cases
- **Brain Configuration Testing**: Configuration testing and validation
- **Brain Configuration Optimization**: Configuration optimization strategies

## 🔗 Related Resources

### Documentation
- **README.md**: Project overview and installation
- **getting-started.md**: Getting started guide
- **api-reference.md**: Complete API documentation
- **configuration.md**: Configuration guide
- **best-practices.md**: Best practices

### Examples
- **examples/visualization_example.py**: Visualization example

### Code
- **src/brain/**: Core neural components
- **src/agent/**: Agent integration layer
- **src/world/**: Environment simulation

## 📖 Next Tutorial

For more advanced tutorials:

1. **Tutorial 3**: Learn about simulation control and monitoring
2. **Tutorial 4**: Learn about visualization and debugging
3. **Tutorial 5**: Learn about plugin development and usage

Following these steps will help you successfully complete Tutorial 2 and continue learning NLM.🧠✨