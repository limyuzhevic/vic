# NLM System - Enhanced Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) system enhancements, including improvements to the C++ core, Python bindings, configuration system, and user interfaces.

## Overview

The NLM system has been significantly enhanced with the following improvements:

### Key Enhancements

1. **Enhanced Command-Line Interface**
   - Advanced argument parsing with help system
   - Multiple execution modes (demo, test, run)
   - Configuration file management
   - Verbose logging and output control

2. **Advanced Configuration System**
   - Schema-based configuration validation
   - Multiple preset configurations
   - Configuration validation with detailed error messages
   - Schema management and validation utilities

3. **Enhanced Python Bindings**
   - Comprehensive error handling with custom exceptions
   - Convenience factory methods and utilities
   - Statistical analysis tools
   - Machine learning dataset creation
   - Advanced serialization support

4. **New Utility Functions**
   - Neural activity analysis
   - Experiment automation
   - Reproducible experiment creation
   - Configuration validation utilities
   - Statistical analysis tools

5. **Improved Documentation**
   - Comprehensive API documentation
   - Detailed examples and tutorials
   - Best practices and troubleshooting guide
   - Quick start guides

## System Architecture

### Core Components

1. **Configuration System**
   - Config class with validation and schema support
   - Configuration presets (phase2, phase6, learning, development, structural)
   - Schema validation and constraint checking
   - Configuration serialization and persistence

2. **Neural Simulation**
   - Core brain simulation with LIF neurons
   - STDP and Hebbian plasticity
   - Structural plasticity (synaptogenesis/pruning)
   - Neuromodulation systems
   - Memory systems (working, episodic, semantic)

3. **Agent Interface**
   - Agent brain for interaction with environments
   - Sensory processing and motor control
   - Action selection and execution
   - Reward prediction and error signals

4. **Python Bindings**
   - Enhanced C++ to Python interface
   - Comprehensive documentation and examples
   - Advanced utility functions
   - Statistical analysis tools

### Data Flow

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
                                      ↓
                                MEMORY / PREDICTION
                                      ↓
                         MOTIVATION / NEUROMODULATION
                                      ↓
                         ACTION SELECTION
                                      ↓
                         MOTOR OUTPUT → WORLD CONSEQUENCE
                                      ↓
                         REWARD / SURPRISE / ERROR
                                      ↓
                         PLASTICITY → MEMORY / DEVELOPMENT
```

## Configuration System

### Configuration Classes

#### Config
- **Purpose**: Central configuration management with validation
- **Features**:
  - Schema-based validation
  - Multiple preset configurations
  - Detailed error reporting
  - Configuration serialization
  - Type-safe value access

#### ConfigSchema
- **Purpose**: Define configuration schemas with constraints
- **Features**:
  - Type definitions (int, double, bool, string, vector)
  - Constraint validation (range, pattern, length)
  - Default values
  - Required key marking

#### ConfigValidator
- **Purpose**: Validate configurations against schemas
- **Features**:
  - Comprehensive error reporting
  - Constraint checking
  - Schema validation
  - Type validation

### Presets

#### Phase2 Presets
- **Purpose**: Phase 2 neural computation settings
- **Features**:
  - Standard LIF neuron dynamics
  - Basic STDP and Hebbian plasticity
  - Simple structural plasticity
  - Configuration for demonstration and testing

#### Phase6 Presets
- **Purpose**: Phase 6 final integration settings
- **Features**:
  - Complete system integration
  - Advanced neuromodulation
  - Comprehensive memory systems
  - Full cognitive capabilities

#### Learning Presets
- **Purpose**: Settings optimized for learning experiments
- **Features**:
  - Enhanced plasticity rules
  - Increased learning rates
  - Novelty detection
  - Curiosity-driven exploration

#### Development Presets
- **Purpose**: Settings for developmental processes
- **Features**:
  - Age-dependent plasticity
  - Critical period simulation
  - Developmental stage management
  - Excitability dynamics

#### Structural Presets
- **Purpose**: Settings for structural plasticity
- **Features**:
  - Synaptogenesis control
  - Pruning mechanisms
  - Activity-dependent structural changes
  - Network architecture optimization

### Configuration Validation

```python
# Create configuration
config = pynlm.create_preset_config("learning")

# Validate configuration
validation_errors = config.validate("learning")
if validation_errors:
    print("Configuration validation failed:")
    for error in validation_errors:
        print(f"  {error.key}: {error.message} ({error.code})")
else:
    print("Configuration is valid")

# Check if configuration is valid
if config.isValid("learning"):
    print("Configuration is valid")
```

## Python Bindings

### Core API

#### Config Class
- **Methods**:
  - `create_default_config()` - Create default configuration
  - `create_preset_config(preset)` - Create configuration with preset
  - `validate_config(config, preset)` - Validate configuration
  - `apply_preset(base_config, preset_name)` - Apply preset to configuration
  - `getSchemaNames()` - Get available schema names
  - `hasSchema(name)` - Check if schema exists

#### Brain Class
- **Methods**:
  - `create_brain(config)` - Create brain with configuration
  - `create_brain_with_validation(config, preset_name)` - Create brain with validation
  - `create_preset_brain(preset)` - Create brain with preset configuration
  - `initialize()` - Initialize brain
  - `step(step, time)` - Perform simulation step
  - `reset()` - Reset brain state
  - `save(filepath)` - Save brain state
  - `load(filepath)` - Load brain state

#### AgentBrain Class
- **Methods**:
  - `create_agent_brain(brain)` - Create agent brain interface
  - `initialize(world)` - Initialize with world
  - `processSensoryInput(percept)` - Process sensory input
  - `decodeMotorCommand()` - Decode motor command
  - `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation

#### SimpleWorld Class
- **Methods**:
  - `create_simple_world()` - Create simple world
  - `configure(width, height, vision_width, vision_height)` - Configure world
  - `update(timestep)` - Update world state
  - `applyMotorCommand(command, time)` - Apply motor command
  - `getSensoryPercept()` - Get sensory percept
  - `getAgentBody()` - Get agent body state

### Enhanced Features

#### Convenience Methods

```python
# Create brain with preset
brain = pynlm.create_preset_brain("learning")
brain.initialize()

# Run complete demo
pynlm.run_complete_demo(brain)

# Analyze neural activity
stats = pynlm.analyze_neural_activity(brain, 1000)

# Create learning experiment
experiment = pynlm.create_and_run_learning_experiment(config, 500)

# Create reproducible experiment
experiment = pynlm.create_reproducible_experiment("learning", 42, 1000)
```

#### Statistical Analysis

```python
# Analyze neural activity
brain = pynlm.create_brain(learning_config)
brain.initialize()

stats = pynlm.analyze_neural_activity(brain, 1000)
print(f"Firing rate mean: {stats['firing_rate']['mean']}")
print(f"Firing rate stddev: {stats['firing_rate']['stddev']}")
print(f"Firing rate min: {stats['firing_rate']['min']}")
print(f"Firing rate max: {stats['firing_rate']['max']}")
```

#### Machine Learning Dataset Creation

```python
# Create training dataset
brain = pynlm.create_brain(learning_config)
dataset = pynlm.create_training_dataset(brain, 10)

print(f"Created dataset with {len(dataset)} episodes")
for i, episode in enumerate(dataset):
    print(f"Episode {i}:")
    print(f"  Initial neurons: {episode['initial_neurons']}")
    print(f"  Final neurons: {episode['final_neurons']}")
    print(f"  Spike change: {episode['spike_change']}")
```

#### Serialization

```python
# Convert objects to dictionaries
brain = pynlm.create_brain(learning_config)
brain.initialize()

brain_dict = brain.to_dict()
print(f"Brain dictionary: {brain_dict}")

# Create object from dictionary
new_brain = pynlm.Brain.from_dict(brain_dict)
```

#### Exception Handling

```python
import pynlm

try:
    config = pynlm.create_preset_config("learning")
    brain = pynlm.create_brain_with_validation(config, "learning")
    brain.initialize()
    
    print("Brain initialized successfully")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e.what()}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e.what()}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e.what()}")
except pynlm.NLMError as e:
    print(f"NLM error: {e.what()}")
```

### Module Attributes

- `__version__` - Version of the bindings
- `__description__` - Description of the bindings
- `__available_presets__` - Available preset names
- `__available_types__` - Available configuration types
- `__validation_errors__` - Available validation error codes

## Command-Line Interface

### Usage

```bash
# Show help
./nlm --help
./nlm help

# Run demo mode
./nlm demo --demo-mode=plasticity --demo-steps=1000

# Run test mode
./nlm test --test-mode=integration --verbose

# Run continuous simulation
./nlm run --episode-length=10000 --enable-plasticity --enable-neuromodulation

# Load custom configuration
./nlm --config=my_config.cfg --output=simulation.log

# Dry run (parse arguments only)
./nlm --dry-run --config=test.cfg
```

### Options

#### Common Options
- `--help, -h` - Show help message
- `--verbose, -v` - Enable verbose logging
- `--quiet, -q` - Disable informational messages
- `--log-level=LEVEL` - Set log level (trace, debug, info, warn, error, critical)
- `--random-seed=SEED` - Set random seed for reproducibility
- `--timestep=DT` - Set simulation timestep
- `--dry-run` - Parse arguments but don't execute
- `--config=FILE` - Load configuration from file
- `--output=FILE` - Save output to file

#### Demo Mode Options
- `--demo-mode=MODE` - Demo mode: 'basic', 'plasticity', 'stdp', 'comprehensive'
- `--demo-steps=STEPS` - Number of steps per demo
- `--reset-between-tests` - Reset brain between demo tests

#### Test Mode Options
- `--test-mode=MODE` - Test mode: 'unit', 'integration', 'stress'
- `--skip-tests=LIST` - Comma-separated list of tests to skip
- `--test-timeout=SECS` - Timeout for individual tests
- `--continuous` - Run tests continuously

#### Run Mode Options
- `--episode-length=LEN` - Length of simulation episode
- `--episode-count=COUNT` - Number of episodes to run
- `--enable-plasticity` - Enable synaptic plasticity
- `--enable-development` - Enable developmental processes
- `--enable-neuromodulation` - Enable neuromodulation
- `--save-interval=INT` - Save brain state every N steps
- `--auto-restart` - Auto-restart after episode completion

## Utility Functions

### Neural Activity Analysis

```python
# Analyze neural activity over time
brain = pynlm.create_brain(learning_config)
brain.initialize()

stats = pynlm.analyze_neural_activity(brain, 1000)
print(f"Firing rate statistics: {stats['firing_rate']}")
print(f"Overall statistics: {stats['overall']}")
```

### Experiment Automation

```python
# Run complete demo sequence
brain = pynlm.create_brain(learning_config)
brain.initialize()
pynlm.run_complete_demo(brain)

# Setup complete experiment
brain = pynlm.create_brain(learning_config)
brain.initialize()
pynlm.setup_complete_experiment(brain, "plasticity", 1000)
```

### Reproducible Experiments

```python
# Create reproducible experiment with fixed seed
experiment = pynlm.create_reproducible_experiment("learning", 12345, 1000)
print(f"Reproducible experiment completed")
print(f"Total spikes: {experiment.brain->getTotalSpikeCount()}")
```

### Configuration Utilities

```python
# Create advanced configuration with overrides
overrides = {"neuron_count": "2000", "learning_rate": "0.05"}
advanced_config = pynlm.create_advanced_config("phase2", overrides)

# Print example usage
pynlm.print_example_usage()
```

## Best Practices

### 1. Use Presets Appropriately

```python
# Use appropriate presets for different use cases
if you_need_learning:
    config = pynlm.create_preset_config("learning")
elif you_need_development:
    config = pynlm.create_preset_config("development")
elif you_need_structural:
    config = pynlm.create_preset_config("structural")
else:
    config = pynlm.create_preset_config("phase2")
```

### 2. Validate Before Use

```python
# Always validate your configuration before using it
config = pynlm.create_preset_config("learning")
if not config.isValid("learning"):
    errors = config.validate("learning")
    # Handle errors appropriately
    raise ValueError("Invalid configuration")

brain = pynlm.create_brain_with_validation(config, "learning")
```

### 3. Use Reproducible Seeds

```python
# Use fixed seeds for reproducible experiments
experiment = pynlm.create_reproducible_experiment("learning", 42, 1000)
```

### 4. Handle Errors Gracefully

```python
try:
    brain = pynlm.create_brain_with_validation(config, "learning")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e.what()}")
    # Fix configuration and retry
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e.what()}")
    # Check brain state and try again
```

### 5. Use Statistical Analysis

```python
# Analyze brain activity for insights
brain = pynlm.create_brain(learning_config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Analyze activity
stats = pynlm.analyze_neural_activity(brain, 1000)
print(f"Average firing rate: {stats['firing_rate']['mean']}")
print(f"Firing rate stddev: {stats['firing_rate']['stddev']}")
```

## Troubleshooting

### Common Issues

**Issue: Configuration validation fails**
```python
# Solution: Check validation errors
config = pynlm.create_preset_config("learning")
errors = config.validate("learning")
for error in errors:
    print(f"Error: {error.message}")
```

**Issue: Brain initialization fails**
```python
# Solution: Check brain configuration
config = pynlm.create_preset_config("learning")
try:
    brain = pynlm.create_brain_with_validation(config, "learning")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e.what()}")
```

**Issue: Simulation runs too slowly**
```python
# Solution: Use smaller configurations
config = pynlm.create_preset_config("small")
brain = pynlm.create_brain(config)
```

**Issue: Neural activity analysis fails**
```python
# Solution: Check brain state before analysis
brain = pynlm.create_brain(learning_config)
brain.initialize()
stats = pynlm.analyze_neural_activity(brain, 100)
```

### Getting Help

For more information and examples:

1. **Documentation**: See this document for comprehensive examples
2. **Source Code**: Examine the Python bindings source code
3. **Repository**: Visit the NLM repository for more examples
4. **Community**: Join the NLM community for support

## Performance Optimization

### Tips for Better Performance

1. **Use Appropriate Presets**
   - Use "small" preset for testing
   - Use "phase2" preset for standard experiments
   - Use "phase6" preset for full system testing

2. **Optimize Configuration**
   - Use preset configurations instead of custom ones
   - Validate configuration before use
   - Use appropriate neuron counts for your use case

3. **Memory Management**
   - Clear brain state when not needed
   - Use configuration validation to prevent memory issues
   - Monitor memory usage during long simulations

4. **Simulation Speed**
   - Use appropriate timesteps
   - Limit simulation duration for testing
   - Use appropriate neuron counts

## Migration Guide

### From Old to New API

#### Configuration Management

**Old API**:
```python
# Old configuration management
config = {}
config['random_seed'] = 42
config['neuron_count'] = 500
config['simulation_timestep'] = 0.001

# Basic brain creation
brain = Brain(config)
brain.initialize()
```

**New API**:
```python
# New configuration management with validation
config = pynlm.create_preset_config("learning")
brain = pynlm.create_brain_with_validation(config, "learning")
brain.initialize()
```

#### Error Handling

**Old API**:
```python
# Basic error handling
try:
    brain = Brain(config)
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
```

**New API**:
```python
# Enhanced error handling with specific exception types
import pynlm

try:
    config = pynlm.create_preset_config("learning")
    brain = pynlm.create_brain_with_validation(config, "learning")
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e.what()}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e.what()}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e.what()}")
except pynlm.NLMError as e:
    print(f"NLM error: {e.what()}")
```

#### Experiment Automation

**Old API**:
```python
# Manual experiment setup
brain = Brain(config)
brain.initialize()

# Run basic connectivity test
run_basic_connectivity_test(brain)

# Reset and run plasticity experiment
brain.reset()
brain.initialize()
run_plasticity_experiment(brain)
```

**New API**:
```python
# Automated experiment setup
brain = pynlm.create_preset_brain("learning")
pynlm.run_complete_demo(brain)
```

## Conclusion

The enhanced NLM system provides a comprehensive set of tools for neural network simulation and analysis. With improved configuration management, error handling, and automation, users can quickly set up and run complex neural experiments.

Key improvements:

- **Enhanced Configuration System**: Schema-based validation, multiple presets, comprehensive error reporting
- **Improved Python API**: Convenience methods, statistical analysis tools, machine learning utilities
- **Better Error Handling**: Custom exception types with detailed error messages
- **Automation Tools**: Complete experiment automation, reproducible experiments
- **Comprehensive Documentation**: Detailed examples, best practices, troubleshooting guide

These enhancements make the NLM system more accessible to users of all levels, from beginners to advanced researchers, while providing powerful tools for complex neural simulation tasks.

## References

- [NLM Project Documentation](https://github.com/nlm-project/nlm)
- [NLM Python Bindings](https://pypi.org/project/pynlm/)
- [NLM Scientific Papers](https://scholar.google.com)

---

*Last updated: October 30, 2026*
*NLM Project Version 2.0.0-enhanced*