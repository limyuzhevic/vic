# NLM Enhanced Configuration and Usage Guide

This document provides comprehensive guidance for using NLM (Neural Learning Machine) with the enhanced configuration system and command-line interface.

## Overview

NLM is an experimental artificial developmental brain simulator that implements real spiking neural computation with multiple integrated systems including memory, prediction, cognition, and neuromodulation.

## Enhanced Configuration System

### Key Improvements

1. **Multi-format Support**: JSON, YAML, and simple key=value format
2. **Advanced Validation**: Configuration validation with auto-fix capabilities
3. **Source Tracking**: Track configuration sources (default, file, command-line, runtime)
4. **Auto-save**: Automatic configuration saving with customizable intervals
5. **Error Handling**: Comprehensive error handling and reporting
6. **Type Safety**: Strong typing with variant-based configuration values

### Configuration File Formats

#### JSON Configuration

JSON is the recommended format for configuration files due to its structure and tooling support.

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.5
        }
    }
}
```

#### YAML Configuration

YAML is ideal for human-readable configuration files.

```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  initial_weight_mean: 0.5
  initial_weight_std: 0.1

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.01

neuromodulation:
  curiosity:
    enable: true
    exploration_rate: 0.5
```

#### Simple Configuration

Simple key=value format is ideal for quick configuration and scripting.

```
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
neuromodulation.curiosity.enable = true
```

## Configuration Management

### Loading Configuration

#### Command Line Interface

```bash
# Load JSON configuration
nlm-cli config load config.json --format json --validate

# Load YAML configuration
nlm-cli config load config.yaml --format yaml

# Load simple configuration
nlm-cli config load config.cfg
```

#### Python API

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Load from JSON
config.loadFromFile("config.json")

# Load from simple format
config.loadFromSimpleFile("config.cfg")

# Load from command line arguments
config.loadFromArgs(2, ["--neuron-count", "1000", "--connection-probability", "0.15"])
```

### Saving Configuration

#### Command Line Interface

```bash
# Save to JSON
nlm-cli config save config.json --format json

# Save to YAML
nlm-cli config save config.yaml --format yaml

# Save to simple format
nlm-cli config save config.cfg
```

#### Python API

```python
# Save configuration
config.saveToFile("my_config.json")
```

### Setting Configuration Values

#### Command Line Interface

```bash
# Set single value
nlm-cli config set brain.neuron_count 2000

# Set with source specification
nlm-cli config set brain.neuron_count 2000 --source command-line

# Set multiple values
nlm-cli config set brain.neuron_count 2000 plasticity.stdp.learning_rate 0.002
```

#### Python API

```python
# Set configuration value
config.set("brain.neuron_count", 2000)

# Set with source
config.set("brain.neuron_count", 2000, nlm.ConfigSource.CommandLine)
```

### Getting Configuration Values

#### Command Line Interface

```bash
# Get single value
nlm-cli config get brain.neuron_count

# Get with format specification
nlm-cli config get brain.neuron_count --format json
nlm-cli config get brain.neuron_count --format raw
```

#### Python API

```python
# Get configuration value
value = config.get("brain.neuron_count")
if value:
    neuron_count = value.value()

# Get with default value
default_neurons = config.getOr("brain.neuron_count", 1000)
```

### Listing Configuration Keys

#### Command Line Interface

```bash
# List all keys
nlm-cli config list

# List with table format
nlm-cli config list --format table

# List with JSON format
nlm-cli config list --format json

# Filter by source
nlm-cli config list --source command-line
```

#### Python API

```python
# Get all keys
keys = config.getKeys()

# Check if key exists
if config.has("brain.neuron_count"):
    print("Configuration has neuron_count")
```

### Validating Configuration

#### Command Line Interface

```bash
# Validate configuration
nlm-cli config validate

# Validate with strict mode
nlm-cli config validate --strict

# Validate with auto-fix
nlm-cli config validate --fix
```

#### Python API

```python
# Basic validation
def validate_config(config):
    keys = config.getKeys()
    valid_keys = 0
    for key in keys:
        if config.has(key):
            valid_keys += 1
    return valid_keys, len(keys)

valid_count, total_count = validate_config(config)
print(f"Valid keys: {valid_count}/{total_count}")
```

### Automatic Configuration Saving

#### Command Line Interface

```bash
# Enable auto-save
nlm-cli config autosave checkpoints/config.json 1000 --enabled

# Check auto-save status
nlm-cli config list --format json
```

#### Python API

```python
# Enable auto-save
config.setAutoSave("checkpoints/config.json", 1000)

# Disable auto-save
config.disableAutoSave()

# Check if should save
def should_auto_save(config, step):
    return config.shouldAutoSave(step)
```

## Brain Management

### Creating Brain Instances

#### Command Line Interface

```bash
# Create brain with default settings
nlm-cli brain create

# Create brain with custom settings
nlm-cli brain create --neurons 500 --regions 2 --density 0.1

# Create and save brain
nlm-cli brain create --neurons 500 --output my_brain.bin
```

#### Python API

```python
import pynlm

# Create brain with default config
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Create brain with custom config
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Loading Brain Instances

#### Command Line Interface

```bash
# Load brain from file
nlm-cli brain load my_brain.bin

# Load and replace current brain
nlm-cli brain load my_brain.bin --replace
```

#### Python API

```python
# Load brain from file
new_brain = pynlm.createBrain(config)
new_brain.initialize()
new_brain.load("my_brain.bin")

# Replace current brain
current_brain = new_brain
```

### Running Brain Simulation

#### Command Line Interface

```bash
# Run simulation with steps
nlm-cli brain step --steps 1000

# Run simulation with time
nlm-cli brain step --steps 100 --time 0.1

# Run simulation with monitoring
nlm-cli brain step --steps 100 --monitor

# Run complete simulation
nlm-cli simulate --config config.json --steps 1000 --brain brain.bin
```

#### Python API

```python
# Run brain simulation
for step in range(1000):
    brain.step(step)
    
    # Monitor progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Run with time steps
for step in range(100):
    time = step * 0.001
    brain.step(step, time)
```

### Saving Brain State

#### Command Line Interface

```bash
# Save brain to file
nlm-cli brain save brain.bin

# Save with compression
nlm-cli brain save brain.bin --compress

# Save with metadata
nlm-cli brain save brain.bin --metadata
```

#### Python API

```python
# Save brain state
brain.save("my_brain.bin")

# Save with compression
# (Compression options vary by implementation)
```

### Brain Statistics and Diagnostics

#### Command Line Interface

```bash
# Get brain statistics
nlm-cli brain stats

# Get statistics in JSON format
nlm-cli brain stats --format json

# Get detailed statistics
nlm-cli brain stats --detailed

# Get statistics in CSV format
nlm-cli brain stats --format csv
```

#### Python API

```python
# Get brain statistics
brain.logStatus()

# Get specific statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "total_spikes": brain.getTotalSpikeCount()
}

# Get developmental stage
development_stage = brain.getDevelopmentalStage()
print(f"Development stage: {development_stage}")
```

## Integration Examples

### Complete Simulation Example

```python
import pynlm
import time

def run_complete_simulation(config_file, output_file, num_steps):
    # Load configuration
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    start_time = time.time()
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    end_time = time.time()
    
    # Save brain state
    brain.save(output_file)
    
    print(f"Simulation complete in {end_time - start_time:.2f} seconds")
    print(f"Results saved to {output_file}")

# Run simulation
run_complete_simulation("config.json", "final_brain.bin", 1000)
```

### Configuration Management Example

```python
import pynlm
import json

def manage_configuration(config_file, operation, options=None):
    config = pynlm.createDefaultConfig()
    
    if operation == "load":
        config.loadFromFile(config_file)
        print("Configuration loaded successfully")
        
    elif operation == "save":
        config.saveToFile(config_file)
        print("Configuration saved successfully")
        
    elif operation == "set":
        for key, value in options.items():
            config.set(key, value)
            print(f"Set {key} = {value}")
            
    elif operation == "get":
        for key in options:
            if config.has(key):
                print(f"{key} = {config.getOr(key, '')}")
            else:
                print(f"Key not found: {key}")
                
    elif operation == "validate":
        keys = config.getKeys()
        valid_keys = [key for key in keys if config.has(key)]
        print(f"Validation: {len(valid_keys)}/{len(keys)} keys valid")
        
    elif operation == "list":
        keys = config.getKeys()
        print(f"Configuration keys ({len(keys)} total):")
        for key in keys:
            print(f"  {key}")

# Example usage
manage_configuration("config.json", "load")
manage_configuration("config.json", "set", {"brain.neuron_count": 1500})
manage_configuration("config.json", "get", ["brain.neuron_count", "plasticity.stdp.learning_rate"])
manage_configuration("config.json", "validate")
manage_configuration("config.json", "save")
```

### Brain Configuration Example

```python
import pynlm

def create_brain_with_config(config_options):
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Apply configuration options
    for key, value in config_options.items():
        config.set(key, value)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

# Define brain configuration
brain_config = {
    "brain.neuron_count": 1000,
    "brain.region_count": 1,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "neuromod.curiosity.enable": True,
    "neuromod.curiosity.exploration_rate": 0.5,
    "development.enable": True
}

# Create brain with custom configuration
brain = create_brain_with_config(brain_config)

# Run simulation
for step in range(500):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Advanced Usage Patterns

### Configuration Templates

```python
import pynlm
import json

# Define configuration templates
templates = {
    "basic": {
        "brain.neuron_count": 100,
        "brain.region_count": 1,
        "brain.connection_probability": 0.05,
        "plasticity.stdp.enable": True,
        "neuromod.curiosity.enable": False
    },
    "advanced": {
        "brain.neuron_count": 1000,
        "brain.region_count": 2,
        "brain.connection_probability": 0.1,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.enable": True,
        "memory.working_memory.capacity": 100,
        "memory.episodic_memory.max_episodes": 1000
    },
    "minimal": {
        "brain.neuron_count": 50,
        "brain.region_count": 1,
        "brain.connection_probability": 0.02,
        "plasticity.stdp.enable": False
    }
}

def apply_template(template_name):
    config = pynlm.createDefaultConfig()
    
    if template_name in templates:
        for key, value in templates[template_name].items():
            config.set(key, value)
        
        print(f"Applied template: {template_name}")
        return config
    else:
        print(f"Unknown template: {template_name}")
        return config

# Apply template
config = apply_template("advanced")
brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuration Comparison

```python
import pynlm

def compare_configurations(config1, config2):
    keys1 = set(config1.getKeys())
    keys2 = set(config2.getKeys())
    
    only_in_1 = keys1 - keys2
    only_in_2 = keys2 - keys1
    in_both = keys1 & keys2
    
    print("Configuration Comparison:")
    print(f"  Only in config1: {len(only_in_1)} keys")
    print(f"  Only in config2: {len(only_in_2)} keys")
    print(f"  In both: {len(in_both)} keys")
    
    # Compare values for keys in both
    differences = []
    for key in in_both:
        val1 = config1.getOr(key, None)
        val2 = config2.getOr(key, None)
        if val1 != val2:
            differences.append((key, val1, val2))
    
    if differences:
        print("\nValue Differences:")
        for key, val1, val2 in differences:
            print(f"  {key}:")
            print(f"    config1: {val1}")
            print(f"    config2: {val2}")

# Create two configurations
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 500)

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 1000)
config2.set("plasticity.stdp.learning_rate", 0.002)

# Compare them
compare_configurations(config1, config2)
```

### Batch Configuration Operations

```python
import pynlm

def batch_config_operations(config_file, operations):
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    results = []
    
    for operation in operations:
        op_type = operation["type"]
        
        if op_type == "set":
            for key, value in operation["values"].items():
                old_value = config.getOr(key, None)
                config.set(key, value)
                results.append({
                    "operation": "set",
                    "key": key,
                    "old_value": old_value,
                    "new_value": value,
                    "success": True
                })
                
        elif op_type == "get":
            for key in operation["keys"]:
                value = config.getOr(key, None)
                results.append({
                    "operation": "get",
                    "key": key,
                    "value": value,
                    "success": True
                })
                
        elif op_type == "validate":
            is_valid = config.has_all(operation["required_keys"])
            results.append({
                "operation": "validate",
                "valid": is_valid,
                "success": True
            })
    
    return results

# Define batch operations
operations = [
    {
        "type": "set",
        "values": {
            "brain.neuron_count": 2000,
            "plasticity.stdp.learning_rate": 0.002
        }
    },
    {
        "type": "get",
        "keys": ["brain.neuron_count", "plasticity.stdp.learning_rate", "neuromod.curiosity.enable"]
    },
    {
        "type": "validate",
        "required_keys": ["brain.neuron_count", "plasticity.stdp.enable"]
    }
]

# Execute batch operations
results = batch_config_operations("config.json", operations)

# Print results
for result in results:
    if result["operation"] == "set":
        print(f"Set {result['key']}: {result['old_value']} -> {result['new_value']}")
    elif result["operation"] == "get":
        print(f"Get {result['key']}: {result['value']}")
    elif result["operation"] == "validate":
        print(f"Validation: {'PASS' if result['valid'] else 'FAIL'}")
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration file not found**

   ```bash
   # Check if file exists
   ls -la config.json
   
   # Create example configuration
   nlm-cli config save config.json
   ```

2. **Invalid configuration values**

   ```bash
   # Validate configuration
   nlm-cli config validate --strict
   
   
   # View configuration errors
   nlm-cli config get brain.neuron_count
   ```

3. **Brain initialization fails**

   ```bash
   # Check brain configuration
   nlm-cli config list --format table
   
   # Create brain with minimal configuration
   nlm-cli brain create --neurons 10
   ```

4. **Simulation runs too slowly**

   ```bash
   # Reduce brain size
   nlm-cli config set brain.neuron_count 100
   
   # Reduce simulation steps
   nlm-cli brain step --steps 100
   ```

5. **Memory issues**

   ```bash
   # Close other applications
   # Check system resources
   # Reduce brain size
   nlm-cli config set brain.neuron_count 500
   ```

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to load configuration from: file" | Configuration file not found or invalid | Check file path and format |
| "Key not found: key" | Configuration key doesn't exist | Check key name or set value first |
| "Invalid configuration key or value" | Configuration validation failed | Check key and value format |
| "Brain initialization failed" | Brain initialization failed | Check configuration and try again |
| "Unknown command: command" | Unknown CLI command | Check command name or type 'help' |

## Further Reading

### Related Documentation

1. **NLM Architecture**: `docs/ARCHITECTURE.md` - System architecture overview
2. **Phase 6 Integration**: `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration details
3. **Performance Guide**: `docs/PERFORMANCE.md` - Performance optimization guide
4. **Experiment Guide**: `docs/EXPERIMENTS.md` - Experiment descriptions and examples
5. **Python API Reference**: `src/python/bindings.cpp` - Python API implementation

### Development Resources

1. **Source Code**: `src/` - Complete source code documentation
2. **Test Suite**: `tests/` - Unit and integration tests
3. **Configuration Examples**: `configs/` - Additional configuration examples
4. **Sample Projects**: `examples/` - Sample projects and demonstrations

### Community Resources

1. **GitHub Repository**: https://github.com/nlm-project/nlm - Source code and issues
2. **Documentation**: https://nlm.readthedocs.io - Online documentation
3. **Discussions**: https://github.com/nlm-project/nlm/discussions - Community discussions
4. **Support**: https://github.com/nlm-project/nlm/issues - Issue tracking

## Conclusion

The enhanced NLM configuration system and command-line interface provide powerful tools for managing neural simulations. This guide covers the essential features and advanced usage patterns for getting the most out of NLM.

Key takeaways:

1. **Use JSON/YAML for structured configuration**: These formats provide better parsing and validation.
2. **Leverage configuration templates**: Templates enable consistent configuration across simulations.
3. **Use the CLI for quick operations**: The command-line interface provides fast access to configuration management.
4. **Validate configuration regularly**: Always validate configuration files to catch errors early.
5. **Monitor simulation progress**: Use the statistics and monitoring features to track simulation performance.

For support or additional questions, refer to the documentation and community resources listed above.

---

*This guide is continuously updated as NLM evolves. Check the NLM documentation for the latest features and improvements.*

Keywords: NLM, Neural Learning Machine, configuration, brain simulation, neural network, CLI, Python API, JSON, YAML, command line, brain management, simulation, development, enhanced configuration, NLM CLI, configuration management, neural simulation, Python bindings