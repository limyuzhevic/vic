#!/usr/bin/env python3
"""
Enhanced NLM Python API Documentation

This document provides comprehensive documentation for the enhanced NLM Python API,
demonstrating all the improvements and new features added to the project.

The enhanced API includes:
1. Pythonic wrapper classes with better error handling
2. Convenience functions and factory methods
3. Command-line interface
4. Simulation orchestration and helpers
5. Logging and monitoring utilities
6. Configuration utilities and validation
7. Comprehensive documentation and examples
"""

import sys
import os
from pathlib import Path

def create_documentation_index():
    """Create the main documentation index"""
    
    index_content = """# Enhanced NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API has been completely enhanced to provide a more Pythonic, robust, and user-friendly interface for neural simulation development. This documentation covers all improvements and new features.

## Key Improvements

### 1. Enhanced Python Bindings (`pynlm_enhanced`)
- **Pythonic Wrapper Classes**: `ConfigWrapper`, `BrainWrapper`, `AgentBrainWrapper`, `SimpleWorldWrapper`
- **Better Error Handling**: Specific exception types (NLMError, ConfigurationError, SimulationError, MemoryError)
- **Convenience Functions**: Factory methods and helper functions
- **Enhanced Documentation**: Comprehensive docstrings and examples

### 2. Command-Line Interface (`nlm_cli.py`)
- **Common Operations**: Run simulations, save/load states, run experiments
- **Flexible Configuration**: Command-line arguments for all simulation parameters
- **Progress Reporting**: Real-time progress updates and statistics

### 3. Simulation Helpers (`simulation_helpers.py`)
- **High-Level SimulationRunner**: Advanced simulation orchestration
- **Scenario Management**: Pre-built configurations for different experiment types
- **Checkpoint Management**: Save and resume simulation states
- **Experiment Orchestration**: Run multiple simulations with different configurations

### 4. Logging and Monitoring (`logging_monitoring.py`)
- **Comprehensive Logging**: File and console logging with rotation
- **Metrics Collection**: Track and analyze simulation metrics
- **Performance Monitoring**: Real-time performance monitoring
- **Progress Reporting**: Multiple output formats (text, JSON, CSV)

### 5. Configuration Utilities (`configuration_utils.py`)
- **Schema-Based Validation**: Validate configurations against schemas
- **Template System**: Reusable configuration templates
- **Configuration Migration**: Version management and migration
- **Configuration Comparison**: Compare and diff configurations

## Installation

### Prerequisites
- Python 3.8 or higher
- Build C++ extensions

### Building

```bash
# Build the enhanced Python bindings
cd /path/to/nlm/project
mkdir build
gd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python package
pip install -e .
```

### Quick Start

```python
# Import enhanced API
import pynlm_enhanced as nlm

# Create default configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
```

## Enhanced API Reference

### Module: `pynlm_enhanced`

#### Class: ConfigWrapper
**Purpose**: Pythonic configuration management

**Methods**:
- `get(key, default=None)`: Get configuration value
- `getOr(key, default)`: Get value with default
- `set(key, value)`: Set configuration value
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all configuration
- `loadFromFile(filepath)`: Load from JSON file
- `saveToFile(filepath)`: Save to JSON file
- `loadFromArgs(args)`: Load from command line arguments
- `summary()`: Get configuration summary

**Example**:
```python
config = nlm.createDefaultConfig()
config.set("world_width", 30)
config.set("timestep", 0.05)

learning_rate = config.getOr("learning_rate", 0.001)
print(f"Learning rate: {learning_rate}")
```

#### Class: BrainWrapper
**Purpose**: Enhanced brain interface with better API

**Methods**:
- `initialize()`: Initialize the brain
- `step(step_num)`: Perform simulation step
- `stepWithTime(step_num, time)`: Perform step with timestamp
- `getTotalNeuronCount()`: Get total neuron count
- `getActiveNeuronCount()`: Get active neuron count
- `getFiringNeuronCount()`: Get firing neuron count
- `getTotalSpikeCount()`: Get total spike count
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get E/I balance
- `getDevelopmentalStage()`: Get developmental stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `logStatus()`: Log brain status
- `repr()`: String representation

**Example**:
```python
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Brain: {brain}")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### Class: AgentBrainWrapper
**Purpose**: Enhanced agent brain interface

**Methods**:
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `getSensoryInputSize()`: Get sensory input size
- `getMotorOutputSize()`: Get motor output size
- `applyRewardModulation(reward, predicted_reward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `getDevelopmentalStage()`: Get developmental stage
- `reset()`: Reset agent
- `enableRewardModulation(enable)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enable)`: Enable/disable structural plasticity
- `enableDevelopment(enable)`: Enable/disable development
- `enableCuriosity(enable)`: Enable/disable curiosity

**Example**:
```python
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Class: SimpleWorldWrapper
**Purpose**: Enhanced world interface

**Methods**:
- `configure(width, height, vision_width=8, vision_height=8)`: Configure world
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent start position
- `update(timestep)`: Update world
- `applyMotorCommand(cmd, time)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body
- `addObject(obj)`: Add world object
- `removeObject(x, y)`: Remove object
- `isValidPosition(x, y)`: Check valid position
- `getWidth()`: Get world width
- `getHeight()`: Get world height
- `setMaxEnergy(e)`: Set max energy
- `setEnergyDecayRate(r)`: Set energy decay rate
- `setRandomSeed(seed)`: Set random seed
- `getSimulationTime()`: Get simulation time

**Example**:
```python
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.setAgentStart(2, 2)
world.reset()

for step in range(30):
    world.update(0.1)
    percept = world.getSensoryPercept()
    # Process percept...
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Module: `nlm_cli.py`

Command-line interface for common operations:

#### Commands:

**run**: Run a simulation
```bash
python nlm_cli.py run --steps 100 --seed 42 --width 30 --height 30
```

**save**: Save brain state
```bash
python nlm_cli.py save brain_state.json --steps 50
```

**load**: Load brain state
```bash
python nlm_cli.py load brain_state.json --additional-steps 100
```

**experiment**: Run an experiment
```bash
python nlm_cli.py experiment --type phase6 --duration 60
```

**analyze**: Analyze saved state
```bash
python nlm_cli.py analyze brain_state.json
```

### Module: `simulation_helpers.py`

#### Class: SimulationConfig
Configuration class for simulation runs

**Attributes**:
- `brain_config`: Brain configuration
- `world_width`: World width
- `world_height`: World height
- `vision_width`: Vision width
- `vision_height`: Vision height
- `timestep`: Simulation timestep
- `max_steps`: Maximum steps
- `seed`: Random seed
- `enable_reward_modulation`: Enable reward modulation
- `enable_structural_plasticity`: Enable structural plasticity
- `enable_development`: Enable development
- `enable_curiosity`: Enable curiosity
- `log_interval`: Log interval
- `start_position`: Start position
- `action_callback`: Action callback function
- `reward_callback`: Reward callback function
- `termination_condition`: Termination condition function

#### Class: SimulationRunner
High-level simulation runner

**Methods**:
- `run()`: Run the simulation
- `save_checkpoint(filepath)`: Save checkpoint
- `resume_from_checkpoint(filepath)`: Resume from checkpoint

#### Function: run_scenario(name, config)
Run a named scenario

#### Function: run_reproduction_experiment(base_config, num_repetitions, save_results)
Run reproduction experiment

#### Function: run_comparison_experiment(base_config, variations)
Run comparison experiment

### Module: `logging_monitoring.py`

#### Class: SimulationLogger
Comprehensive logging system

**Methods**:
- `add_console_handler(level, format_string)`: Add console handler
- `add_file_handler(filepath, level, max_size, backup_count)`: Add file handler
- `add_custom_handler(handler)`: Add custom handler
- `log_event(event_type, message, level, **kwargs)`: Log event
- `log_step(step, brain, world, agent, action, reward, novelty, curiosity)`: Log step
- `log_simulation_start(config)`: Log simulation start
- `log_simulation_end(stats)`: Log simulation end
- `log_error(error_type, message, exception, **kwargs)`: Log error
- `log_checkpoint(filepath, brain_stats)`: Log checkpoint
- `clear_handlers()`: Clear all handlers

#### Class: MetricsCollector
Collect and analyze metrics

**Methods**:
- `record(metric_name, value, timestamp, **kwargs)`: Record metric
- `record_step_metrics(step, brain, world, agent, action, reward, novelty, curiosity)`: Record step metrics
- `get_metric_summary(metric_name, last_n)`: Get metric summary
- `get_comprehensive_summary()`: Get comprehensive summary
- `add_callback(callback)`: Add callback
- `save_metrics(filepath)`: Save metrics
- `load_metrics(filepath)`: Load metrics

#### Class: PerformanceMonitor
Performance monitoring

**Methods**:
- `start(brain_getter, world_getter)`: Start monitoring
- `stop()`: Stop monitoring
- `add_callback(callback)`: Add callback
- `get_current_metrics()`: Get current metrics

#### Class: SimulationProgressReporter
Progress reporting

**Methods**:
- `report_progress(step, stats, format_type)`: Report progress

### Module: `configuration_utils.py`

#### Class: ConfigSchema
Configuration schema definition

**Methods**:
- `add_field(name, field_type, required, description, validation_rules, default_value)`: Add field
- `add_int_field(name, required, min_value, max_value, description, default_value)`: Add integer field
- `add_float_field(name, required, min_value, max_value, description, default_value)`: Add float field
- `add_str_field(name, required, choices, description, default_value)`: Add string field
- `add_bool_field(name, required, description, default_value)`: Add boolean field
- `add_list_field(name, item_type, required, min_length, max_length, description, default_value)`: Add list field
- `validate_config(config, strict)`: Validate configuration

#### Class: ConfigValidator
Configuration validator

**Methods**:
- `register_schema(name, schema)`: Register schema
- `register_validator(name, validator)`: Register validator
- `validate_with_schema(name, config, strict)`: Validate with schema
- `validate_config(config, schema_name)`: Validate configuration
- `get_schema_info(name)`: Get schema info

#### Class: ConfigTemplate
Configuration template

**Methods**:
- `apply_variables(variables)`: Apply variables
- `validate_template(config)`: Validate template

#### Class: ConfigManager
Main configuration manager

**Methods**:
- `create_config(name, config)`: Create configuration
- `get_config(name)`: Get configuration
- `update_config(name, config)`: Update configuration
- `validate_config(name)`: Validate configuration
- `apply_template(template_name, variables)`: Apply template
- `save_config(name, filepath)`: Save configuration
- `load_config(name, filepath)`: Load configuration
- `compare_configs(name1, name2)`: Compare configurations
- `export_config(name)`: Export configuration
- `import_config(name, config)`: Import configuration

## Usage Examples

### Example 1: Basic Simulation

```python
import pynlm_enhanced as nlm

# Create simulation components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Initialize
brain.initialize()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.setAgentStart(2, 2)
world.reset()
agent.initialize(world)

# Enable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Print statistics
print(nlm.formatStats(brain.getBrain()))
```

### Example 2: Using Simulation Helpers

```python
from simulation_helpers import SimulationRunner, SimulationConfig, run_scenario, run_comparison_experiment

# Create configuration
config = SimulationConfig(
    world_width=30,
    world_height=30,
    vision_width=10,
    vision_height=10,
    max_steps=500,
    enable_reward_modulation=True,
    enable_curiosity=True,
    log_interval=100
)

# Run simulation using SimulationRunner
runner = SimulationRunner(config)
stats = runner.run()

print(f"Simulation completed: {stats.steps_completed} steps")
print(f"Final energy: {world.getAgentBody().energy}")
```

### Example 3: Running Comparison Experiment

```python
from simulation_helpers import run_comparison_experiment, SimulationConfig

# Base configuration
base_config = SimulationConfig(
    world_width=25,
    world_height=25,
    max_steps=200,
    enable_reward_modulation=True
)

# Variations for comparison
variations = {
    "Basic": {},
    "Enhanced": {
        "enable_curiosity": True,
        "enable_structural_plasticity": True
    },
    "Development": {
        "enable_development": True,
        "enable_curiosity": True
    }
}

# Run comparison
results = run_comparison_experiment(base_config, variations)

for name, stats in results.items():
    if stats:
        avg_energy = (stats.max_energy + stats.min_energy) / 2
        print(f"{name}: {stats.steps_completed} steps, {avg_energy:.2f} avg energy")
```

### Example 4: Using Logging and Monitoring

```python
from logging_monitoring import create_default_logging_config, MetricsCollector

# Create logger
logger = create_default_logging_config()

# Log simulation start
config = {"world_width": 20, "max_steps": 100}
logger.log_simulation_start(config)

# Log some events
logger.log_event("trial_start", "Starting trial", "INFO", trial_id="trial_001")
logger.log_event("action_taken", "Agent moved", "INFO", action="MoveForward", reward=1.0)

# Create metrics collector
metrics = MetricsCollector("experiment_1")

# Record metrics during simulation
for step in range(50):
    metrics.record('step', step)
    metrics.record('temperature', 20.0 + step * 0.5)

# Get summary
step_summary = metrics.get_metric_summary('step')
print(f"Average step: {step_summary['mean']}")

# Save metrics
metrics.save_metrics("experiment_results.json")
```

### Example 5: Configuration Management

```python
from configuration_utils import ConfigManager, create_scenario_config

# Create configuration manager
config_manager = ConfigManager()

# Create configuration
config = config_manager.create_config("my_config", {
    "world_width": 30,
    "world_height": 30,
    "timestep": 0.05,
    "max_steps": 500
})

# Validate configuration
errors = config_manager.validate_config("my_config")
if not errors:
    print("Configuration is valid!")

# Create scenario configuration
phase4_config = create_scenario_config("phase4")
print(f"Phase 4 config: {phase4_config}")

# Compare configurations
diff = config_manager.compare_configs("my_config", phase4_config)
print(f"Differences: {diff}")
```

### Example 6: Command-Line Interface

```bash
# Run simulation with command-line interface
python nlm_cli.py run --steps 100 --width 30 --height 30 \
    --reward-modulation --curiosity

# Save brain state
python nlm_cli.py save brain_state.json --steps 50

# Load and continue simulation
python nlm_cli.py load brain_state.json --additional-steps 100

# Run experiment
python nlm_cli.py experiment --type phase6 --duration 60

# Analyze results
python nlm_cli.py analyze brain_state.json
```

## Advanced Features

### 1. Context Managers

```python
from contextlib import contextmanager

@contextmanager
def brain_session():
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    try:
        brain.initialize()
        world.configure(10, 10, 8, 8)
        world.reset()
        agent.initialize(world)
        yield brain, world, agent
    finally:
        # Cleanup
        pass

with brain_session() as (brain, world, agent):
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
```

### 2. Event-Driven Programming

```python
def on_step(step, action, world, agent, brain):
    print(f"Step {step}: {action}")

def on_reward(step, reward, novelty, curiosity, world, agent, brain):
    if reward > 0:
        print(f"Reward at step {step}: {reward}")

config = SimulationConfig(
    action_callback=on_step,
    reward_callback=on_reward,
    termination_condition=lambda step, world, agent, brain: step >= 100
)

runner = SimulationRunner(config)
stats = runner.run()
```

### 3. Custom Validation

```python
def custom_validator(config):
    errors = []
    
    if config.get("world_width", 0) <= 0:
        errors.append(ConfigValidationError("World width must be positive", "world_width"))
    
    if config.get("timestep", 0) <= 0:
        errors.append(ConfigValidationError("Timestep must be positive", "timestep"))
    
    return errors

config_manager = ConfigManager()
config_manager.validator.register_validator("custom", custom_validator)
```

## Performance Considerations

### 1. Memory Management
- Use `with` statements for resource management
- Clear unnecessary references after simulation
- Monitor memory usage with PerformanceMonitor

### 2. Simulation Optimization
- Adjust timestep for different simulation scales
- Use checkpointing for long simulations
- Monitor performance with profiling tools

### 3. Logging Optimization
- Use appropriate log levels (DEBUG, INFO, WARNING, ERROR)
- Set appropriate log file sizes and rotation
- Use custom handlers for performance-critical applications

## Troubleshooting

### Common Issues

**1. Simulation Doesn't Start**
```python
# Check initialization
brain.initialize()  # This must be called first
world.configure(10, 10)  # Configure world before use
agent.initialize(world)  # Initialize agent with world
```

**2. Brain Not Learning**
```python
# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

**3. Performance Issues**
```python
from logging_monitoring import PerformanceMonitor

# Monitor performance
monitor = PerformanceMonitor(log_interval=1.0)
monitor.start(lambda: brain, lambda: world)
```

**4. Configuration Errors**
```python
from configuration_utils import ConfigManager

# Validate configuration
config_manager = ConfigManager()
errors = config_manager.validator.validate_config(config, "default")
if errors:
    for error in errors:
        print(f"Error: {error}")
```

## Migration Guide

### From Original API to Enhanced API

#### Changes:
1. **Module Import**: `import pynlm` → `import pynlm_enhanced as nlm`
2. **Configuration**: Use `createDefaultConfig()` instead of manual creation
3. **Brain Creation**: Use `createBrain(config)` instead of direct constructor
4. **World Creation**: Use `createSimpleWorld()` instead of direct constructor
5. **Agent Creation**: Use `createAgentBrain(brain)` instead of direct constructor

#### Backward Compatibility:
- Original classes are still available as `ConfigLegacy`, `BrainLegacy`, etc.
- Original functionality is preserved with enhanced error handling
- New features are available through enhanced API

### Migration Example

```python
# OLD API
import pynlm

config = pynlm.Config()
config.loadFromFile("config.json")
brain = pynlm.Brain(config)
brain.initialize()

# NEW API
import pynlm_enhanced as nlm

config = nlm.createDefaultConfig()
config.loadFromFile("config.json")
brain = nlm.createBrain(config)
brain.initialize()
```

## Testing

### Running Tests

```bash
# Run Python tests
python -m pytest tests/ -v

# Run demo script
python demo_enhanced_api.py

# Run CLI tests
python nlm_cli.py run --steps 10
```

### Test Coverage

The enhanced API includes tests for:
- Basic functionality
- Error handling
- Configuration validation
- Simulation helpers
- Logging and monitoring
- Command-line interface

## Future Enhancements

### Planned Features
1. **Web Interface**: REST API for remote simulation control
2. **Visualization**: Built-in plotting and visualization tools
3. **Distributed Computing**: Parallel simulation execution
4. **Machine Learning Integration**: Neural network training
5. **Hardware Acceleration**: GPU support
6. **Real-time Interaction**: WebSocket-based control

### Contribution Guidelines
1. Follow Python Enhancement Proposals (PEPs)
2. Maintain backward compatibility
3. Write comprehensive tests
4. Document new features
5. Update documentation

## References

1. NLM Documentation: https://nlm.readthedocs.io
2. GitHub Repository: https://github.com/nlm-project/nlm
3. Phase 6 Final Audit: docs/PHASE6_FINAL_AUDIT.md
4. Architecture Documentation: docs/ARCHITECTURE.md

## License

MIT License - See LICENSE file for details.

---

*Document generated on 2025-06-16*
*Version: 1.0.0-enhanced*
*Author: Enhanced NLM Development Team*
"""

# Get current date
current_date = "2025-06-16"

# Create documentation index
doc_content = f"""{index_content}"""

# Write to file
Path("ENHANCED_NLM_DOCUMENTATION.md").write_text(doc_content)
print("Enhanced NLM Python API Documentation created!")

if __name__ == "__main__":
    # Print welcome message
    print("=" * 60)
    print("Enhanced NLM Python API - Documentation Generator")
    print("=" * 60)
    print(f"Generated on: {current_date}")
    print(f"Documentation saved to: ENHANCED_NLM_DOCUMENTATION.md")
    print()
    print("This documentation provides comprehensive coverage of:")
    print("1. Enhanced Python bindings with better error handling")
    print("2. Command-line interface for common operations")
    print("3. Simulation orchestration and helpers")
    print("4. Logging and monitoring utilities")
    print("5. Configuration utilities and validation")
    print("6. Usage examples and migration guide")
    print("=" * 60)"