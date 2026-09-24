# NLM Convenience Module

## Overview

The NLM convenience module provides a collection of commonly used functions and utilities to simplify common tasks and improve the user experience. This module is designed to be intuitive and easy to use while providing powerful functionality for both beginners and advanced users.

## Module Overview

The convenience module is accessible through `pynlm.conv` and includes:

1. **Configuration Templates**: Quick access to predefined configuration templates
2. **Simulation Utilities**: Functions for common simulation tasks
3. **Data Conversion**: Utilities for converting between different data formats
4. **Validation and Error Handling**: Tools for validating and fixing issues
5. **Educational Examples**: Pre-built examples for learning and testing
6. **Batch Processing**: Utilities for efficient batch operations
7. **Debugging Tools**: Tools for debugging and analyzing simulations

## Configuration Templates

### Overview

The configuration templates provide quick access to commonly used configurations for different use cases.

### Available Templates

#### Demo Configuration (`demo`)

**Purpose**: Quick testing and demonstration
- Small brain size (100 neurons)
- Simplified settings
- Fast execution
- All essential systems enabled

**Usage**:
```python
import pynlm

# Get demo configuration
config = pynlm.conv.create_demo_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Run quick test
brain.runSteps(100)
```

#### Production Configuration (`production`)

**Purpose**: Production-ready simulation
- Medium brain size (1000 neurons)
- Balanced performance and features
- All systems enabled
- Optimized for stability

**Usage**:
```python
import pynlm

# Get production configuration
config = pynlm.conv.create_production_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Run production simulation
brain.runSteps(1000)
```

#### Research Configuration (`research`)

**Purpose**: Advanced research and experimentation
- Large brain size (10000 neurons)
- All advanced features enabled
- Optimized for scientific research
- Comprehensive learning capabilities

**Usage**:
```python
import pynlm

# Get research configuration
config = pynlm.conv.create_research_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Run research simulation
brain.runSteps(5000)
```

### Template Information

```python
import pynlm

# List available templates
templates = pynlm.conv.get_config_suggestions()
print("Available templates:", templates)

# Get template information
for template in templates:
    config = pynlm.conv.create_template_config(template)
    print(f"\n{template} configuration:")
    print(f"  Neuron count: {config.getOr<int64_t>('brain.neuron_count', 0)}")
    print(f"  Connection probability: {config.getOr<float>('brain.connection_probability', 0.0)}")
    print(f"  Simulation timestep: {config.getOr<double>('simulation_timestep', 0.0)}")
```

## Simulation Utilities

### Basic Simulation Functions

#### Run Simple Demo

**Purpose**: Run a basic demo simulation
**Parameters**:
- `num_steps` (size_t): Number of simulation steps to run

**Returns**: None (prints progress)

**Example**:
```python
import pynlm

# Run simple demo
pynlm.conv.run_demo(500)
```

#### Run Simulation with Logging

**Purpose**: Run simulation and collect statistics
**Parameters**:
- `brain` (Brain): Brain to simulate
- `num_steps` (size_t): Number of steps to simulate
- `log_interval` (size_t): Interval between logging points

**Returns**: Vector of spike counts

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

spike_counts = pynlm.conv.run_simulation_with_logging(brain, 1000, 100)
print(f"Spike counts: {spike_counts}")
```

### Episode Management

#### Run Test Episode

**Purpose**: Run a complete test episode
**Parameters**:
- `agent` (AgentBrain): Agent to simulate
- `world` (SimpleWorld): World to simulate in
- `num_steps` (size_t): Maximum number of steps

**Returns**: Dictionary with episode results

**Example**:
```python
import pynlm

# Setup
config = pynlm.conv.create_demo_config()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.create_test_world(20, 20)
agent.initialize(world)

# Run test episode
results = pynlm.conv.run_test_episode(agent, world, 500)
print(f"Episode completed: {results['steps_completed']} steps")
print(f"Total reward: {results['total_reward']:.2f}")
```

### Batch Operations

#### Create Neuron Batch

**Purpose**: Create a batch of neuron IDs
**Parameters**:
- `neuron_ids` (list of size_t): List of neuron IDs

**Returns**: Vector of NeuronId objects

**Example**:
```python
import pynlm

# Create neuron batch
neuron_ids = pynlm.conv.create_neuron_batch([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
print(f"Created {len(neuron_ids)} neuron IDs")
```

#### Create Current Batch

**Purpose**: Create a batch of current values
**Parameters**:
- `currents` (list of float): List of current values

**Returns**: Vector of current values

**Example**:
```python
import pynlm

# Create current batch
currents = pynlm.conv.create_current_batch([1.0, 2.0, 3.0, 4.0, 5.0])
print(f"Created {len(currents)} current values")
```

## Data Conversion Utilities

### Statistical Functions

#### Calculate Mean

**Purpose**: Calculate mean of values
**Parameters**:
- `values` (list of float): Input values

**Returns**: Mean value

**Example**:
```python
import pynlm

values = [1.0, 2.0, 3.0, 4.0, 5.0]
mean = pynlm.conv.mean(values)
print(f"Mean: {mean}")
```

#### Calculate Standard Deviation

**Purpose**: Calculate standard deviation of values
**Parameters**:
- `values` (list of float): Input values

**Returns**: Standard deviation

**Example**:
```python
import pynlm

values = [1.0, 2.0, 3.0, 4.0, 5.0]
std = pynlm.conv.std(values)
print(f"Standard deviation: {std}")
```

#### Calculate Min and Max

**Purpose**: Get min and max of values
**Parameters**:
- `values` (list of float): Input values

**Returns**: Pair of (min, max) values

**Example**:
```python
import pynlm

values = [1.0, 2.0, 3.0, 4.0, 5.0]
minmax = pynlm.conv.minmax(values)
print(f"Min: {minmax.first}, Max: {minmax.second}")
```

### Data Normalization

#### Normalize Value

**Purpose**: Normalize value to [0, 1] range
**Parameters**:
- `value` (float): Input value
- `min` (float): Minimum value in range
- `max` (float): Maximum value in range

**Returns**: Normalized value

**Example**:
```python
import pynlm

value = pynlm.conv.normalize(5.0, 0.0, 10.0)
print(f"Normalized value: {value}")
```

#### Denormalize Value

**Purpose**: Denormalize value from [0, 1] range
**Parameters**:
- `normalized` (float): Normalized value
- `min` (float): Minimum value in range
- `max` (float): Maximum value in range

**Returns**: Denormalized value

**Example**:
```python
import pynlm

value = pynlm.conv.denormalize(0.5, 0.0, 10.0)
print(f"Denormalized value: {value}")
```

### Linear Interpolation

#### Linear Interpolation

**Purpose**: Linearly interpolate between two values
**Parameters**:
- `a` (float): Start value
- `b` (float): End value
- `t` (float): Interpolation factor (0 = a, 1 = b)

**Returns**: Interpolated value

**Example**:
```python
import pynlm

value = pynlm.conv.lerp(1.0, 10.0, 0.5)
print(f"Interpolated value: {value}")
```

### Value Clamping

#### Clamp Value

**Purpose**: Clamp value between minimum and maximum
**Parameters**:
- `value` (float): Input value
- `min` (float): Minimum allowed value
- `max` (float): Maximum allowed value

**Returns**: Clamped value

**Example**:
```python
import pynlm

value = pynlm.conv.clamp(5.0, 0.0, 10.0)
print(f"Clamped value: {value}")
```

## Validation and Error Handling

### Configuration Validation

#### List Configuration Errors

**Purpose**: Find and list potential errors in configuration
**Parameters**:
- `config` (Config): Configuration to validate

**Returns**: Vector of error messages

**Example**:
```python
import pynlm

config = pynlm.createDefaultConfig()
errors = pynlm.conv.list_config_errors(config)
if errors:
    print("Configuration errors found:")
    for error in errors:
        print(f"  - {error}")
else:
    print("No configuration errors found")
```

#### Validate Configuration

**Purpose**: Validate configuration and suggest fixes
**Parameters**:
- `config` (Config): Configuration to validate

**Returns**: Dictionary with validation results

**Example**:
```python
import pynlm

config = pynlm.createDefaultConfig()
validation = pynlm.conv.validate_config(config)
print(f"Is valid: {validation['is_valid']}")
if 'issues' in validation:
    print(f"Issues: {validation['issues']}")
```

### Brain Validation

#### Validate Brain State

**Purpose**: Validate brain state and report issues
**Parameters**:
- `brain` (Brain): Brain to validate

**Returns**: Dictionary with validation results

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

validation = pynlm.conv.validate_brain_state(brain)
print(f"Is valid: {validation['is_valid']}")
print(f"Issues: {validation['issues']}")
```

#### Validate and Fix Brain

**Purpose**: Validate brain and automatically fix issues
**Parameters**:
- `brain` (Brain): Brain to validate and fix

**Returns**: Dictionary with fixes applied and validation results

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

# Reset to clear any issues
brain.reset()
brain.initialize()

# Validate and fix
result = pynlm.conv.validate_and_fix_brain(brain)
print(f"Fixes applied: {result['fixes_applied']}")
print(f"Is valid: {result['validation_result']['is_valid']}")
```

### Error Handling

#### Safe Brain Step

**Purpose**: Safely step brain with error handling
**Parameters**:
- `brain` (Brain): Brain to step
- `step` (size_t): Step number
- `time` (double): Current time

**Returns**: Dictionary with step results and error information

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

result = pynlm.conv.safe_brain_step(brain, 100, 0.1)
if result['success']:
    print(f"Step {result['step']} completed successfully")
else:
    print(f"Step {result['step']} failed: {result['error']}")
```

## Educational Examples

### Create Educational Example

**Purpose**: Create educational example of specified type
**Parameters**:
- `exampleType` (string): Type of example to create

**Returns**: Dictionary with example components

**Example Types**:
- `basic`: Basic neural network example
- `learning`: Learning example with reward modulation
- `plasticity`: Plasticity learning example
- `development`: Development example
- `curiosity`: Curiosity-driven exploration example

**Example**:
```python
import pynlm

# Create basic educational example
example = pynlm.conv.create_educational_example("basic")
print(f"Created {example['type']} example")
print(f"Description: {example['description']}")

# Create learning educational example
example = pynlm.conv.create_educational_example("learning")
print(f"Created {example['type']} example")
print(f"Brain neurons: {example['brain'].getTotalNeuronCount()}")
print(f"Agent has reward modulation: {example['agent'].isRewardModulationEnabled()}")
```

### Get Example Suggestions

**Purpose**: Get list of available educational example types
**Parameters**: None

**Returns**: Vector of example type strings

**Example**:
```python
import pynlm

examples = pynlm.conv.get_example_suggestions()
print("Available educational examples:")
for example in examples:
    print(f"  - {example}")
```

## Debugging and Analysis Tools

### Brain Analysis

#### Debug Brain State

**Purpose**: Debug brain state with validation and suggestions
**Parameters**:
- `brain` (Brain): Brain to debug

**Returns**: Dictionary with debug information

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

# Run simulation to generate activity
brain.runSteps(100)

# Debug brain state
debug_info = pynlm.conv.debug_brain_state(brain)
print(f"Statistics: {debug_info['statistics']}")
print(f"Is valid: {debug_info['is_valid']}")
print(f"Issues: {debug_info['issues']}")
```

#### Check Brain Health

**Purpose**: Check brain health and return detailed report
**Parameters**:
- `brain` (Brain): Brain to check

**Returns**: String with detailed health report

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

health_report = pynlm.conv.check_brain_health(brain)
print(health_report)
```

### World Analysis

#### Debug World State

**Purpose**: Debug world state
**Parameters**:
- `world` (SimpleWorld): World to debug

**Returns**: Dictionary with world state information

**Example**:
```python
import pynlm

world = pynlm.create_test_world(20, 20)
world.reset()
world.setAgentStart(10.0, 10.0)

debug_info = pynlm.conv.debug_world_state(world)
print(f"World dimensions: {debug_info['width']} x {debug_info['height']}")
print(f"Agent position: ({debug_info['agent_x']}, {debug_info['agent_y']})")
print(f"Agent orientation: {debug_info['agent_orientation']}")
```

## Batch Processing Utilities

### Neuron and Current Management

#### Create Test Components

**Purpose**: Create test brain, agent, and world for quick testing
**Parameters**: None (uses default sizes)

**Returns**: Dictionary with test components

**Example**:
```python
import pynlm

# Create test components
test_components = pynlm.conv.create_test_components()
brain = test_components['brain']
agent = test_components['agent']
world = test_components['world']

# Setup
agent.initialize(world)

# Run test episode
results = pynlm.conv.run_test_episode(agent, world, 200)
print(f"Test completed: {results['steps_completed']} steps")
```

#### Create Test Brain

**Purpose**: Create a test brain with specified number of neurons
**Parameters**:
- `neuronCount` (size_t): Number of neurons (default: 100)

**Returns**: Brain object

**Example**:
```python
import pynlm

# Create test brain with 500 neurons
brain = pynlm.conv.create_test_brain(500)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

#### Create Test World

**Purpose**: Create a test world with specified dimensions
**Parameters**:
- `width` (size_t): World width (default: 20)
- `height` (size_t): World height (default: 20)

**Returns**: World object

**Example**:
```python
import pynlm

# Create test world
world = pynlm.conv.create_test_world(30, 30)
print(f"Created world {world.getWidth()} x {world.getHeight()}")
```

#### Create Test Agent

**Purpose**: Create a test agent for a given brain
**Parameters**:
- `brain` (Brain): Brain to create agent for

**Returns**: AgentBrain object

**Example**:
```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.conv.create_demo_config())
agent = pynlm.conv.create_test_agent(brain)
print(f"Created agent for brain with {brain.getTotalNeuronCount()} neurons")
```

## Data Export and Conversion

### Configuration Export

#### Convert Configuration to Dictionary

**Purpose**: Convert configuration to dictionary format
**Parameters**:
- `config` (Config): Configuration to convert

**Returns**: Dictionary with configuration data

**Example**:
```python
import pynlm

config = pynlm.createDefaultConfig()
config_dict = pynlm.conv.config_to_dict(config)
print(f"Configuration has {len(config_dict)} settings")
for key, value in list(config_dict.items())[:5]:  # Show first 5
    print(f"  {key}: {value}")
```

#### Load Configuration from Dictionary

**Purpose**: Load configuration from dictionary format
**Parameters**:
- `configDict` (dict): Dictionary with configuration data

**Returns**: Configuration object

**Example**:
```python
import pynlm

# Create configuration dictionary
config_dict = {
    "brain.neuron_count": 500,
    "brain.connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "plasticity.stdp.enable": True
}

# Load configuration
config = pynlm.conv.load_config_from_dict(config_dict)
print(f"Loaded configuration with {config.getKeys().size()} settings")
```

## Advanced Utilities

### Specialized Functions

#### Get Neuron Types

**Purpose**: Get list of all available neuron types
**Parameters**: None

**Returns**: Vector of NeuronType objects

**Example**:
```python
import pynlm

types = pynlm.conv.get_neuron_types()
print("Available neuron types:")
for neuron_type in types:
    print(f"  - {neuron_type}")
```

#### Get Action Types

**Purpose**: Get list of common action types
**Parameters**: None

**Returns**: Vector of ActionType objects

**Example**:
```python
import pynlm

actions = pynlm.conv.get_action_types()
print("Available action types:")
for action in actions:
    print(f"  - {action}")
```

#### Get Developmental Stages

**Purpose**: Get list of all developmental stages
**Parameters**: None

**Returns**: Vector of DevelopmentalStage objects

**Example**:
```python
import pynlm

stages = pynlm.conv.get_developmental_stages()
print("Available developmental stages:")
for stage in stages:
    print(f"  - {stage}")
```

#### Get World Object Types

**Purpose**: Get list of all world object types
**Parameters**: None

**Returns**: Vector of WorldObjectType objects

**Example**:
```python
import pynlm

types = pynlm.conv.get_world_object_types()
print("Available world object types:")
for obj_type in types:
    print(f"  - {obj_type}")
```

### Simulation Control

#### Run Silent Simulation

**Purpose**: Run simulation without any output
**Parameters**:
- `brain` (Brain): Brain to simulate
- `steps` (size_t): Number of steps to run

**Returns**: Total spike count

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.conv.create_demo_config())
brain.initialize()

spike_count = pynlm.conv.run_silent_simulation(brain, 1000)
print(f"Simulation completed with {spike_count} total spikes")
```

## Plugin System

### Utility Plugin Registration

#### Register Utility Plugin

**Purpose**: Register a utility plugin function
**Parameters**:
- `plugin` (function): Plugin function to register

**Returns**: True if registration successful

**Example**:
```python
def my_utility_plugin(data):
    return f"Processed: {data}"

import pynlm
pynlm.conv.register_utility_plugin(my_utility_plugin)
print("Plugin registered successfully")
```

## Summary

The NLM convenience module provides a comprehensive set of utilities for:

1. **Quick Setup**: Configuration templates and test component creation
2. **Simulation Control**: Batch processing, episode management, and logging
3. **Data Management**: Statistical analysis, normalization, and conversion
4. **Validation and Error Handling**: Configuration and brain validation
5. **Debugging and Analysis**: Brain health checks and world debugging
6. **Educational Support**: Pre-built examples and learning tools
7. **Plugin System**: Extensible plugin registration
8. **Advanced Utilities**: Specialized functions for common tasks

The convenience module makes it easy to:
- Get started quickly with demo configurations
- Run simulations efficiently with batch operations
- Analyze results with statistical tools
- Debug issues with validation and diagnostic tools
- Create educational examples for learning

This module complements the core NLM API by providing higher-level utilities that simplify common tasks and reduce boilerplate code.
