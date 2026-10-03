# NLM (Neural Learning Machine) - Comprehensive Documentation

**Version**: 0.1.0  
**Date**: October 2026  
**Phase**: 6 - Final Integration  

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This documentation provides comprehensive technical documentation for all NLM improvements, organized code modules for clarity and maintainability, and enhanced developer experience with clear documentation and examples.

## Table of Contents

### Part 1: Technical Documentation

1. [Python API Reference](#part-1-python-api-reference)
   - [Core Classes](#python-api-reference-core-classes)
   - [Brain Class](#python-api-reference-brain-class)
   - [Config Class](#python-api-reference-config-class)
   - [AgentBrain Class](#python-api-reference-agentbrain-class)
   - [SimpleWorld Class](#python-api-reference-simpleworld-class)
   - [Enumerations](#python-api-reference-enumerations)

2. [C++ API Documentation](#part-1-c-api-documentation)
   - [Brain Class](#c-api-documentation-brain-class)
   - [AgentBrain Class](#c-api-documentation-agentbrain-class)
   - [Core Components](#c-api-documentation-core-components)

3. [Architecture Documentation](#part-1-architecture-documentation)
   - [System Architecture](#architecture-documentation-system-architecture)
   - [Brain Loop](#architecture-documentation-brain-loop)
   - [Data Flow](#architecture-documentation-data-flow)

### Part 2: Developer Experience

4. [Setup and Installation](#part-2-setup-and-installation)
   - [Prerequisites](#setup-and-installation-prerequisites)
   - [Building from Source](#setup-and-installation-building-from-source)
   - [Installing Python Bindings](#setup-and-installation-installing-python-bindings)

5. [Development Setup Guide](#part-2-development-setup-guide)
   - [Building the Project](#development-setup-guide-building-the-project)
   - [Running Tests](#development-setup-guide-running-tests)
   - [Running Experiments](#development-setup-guide-running-experiments)

6. [Contribution Guidelines](#part-2-contribution-guidelines)
   - [Code Style](#contribution-guidelines-code-style)
   - [Testing Guidelines](#contribution-guidelines-testing-guidelines)
   - [Documentation Guidelines](#contribution-guidelines-documentation-guidelines)

7. [Best Practices](#part-2-best-practices)
   - [Code Organization](#best-practices-code-organization)
   - [API Design](#best-practices-api-design)
   - [Performance Considerations](#best-practices-performance-considerations)

8. [Troubleshooting Guide](#part-2-troubleshooting-guide)
   - [Build Issues](#troubleshooting-guide-build-issues)
   - [Runtime Issues](#troubleshooting-guide-runtime-issues)
   - [Common Problems](#troubleshooting-guide-common-problems)

### Part 3: Examples and Tutorials

9. [Python Examples](#part-3-python-examples)
   - [Basic Brain Usage](#python-examples-basic-brain-usage)
   - [Complete Agent Simulation](#python-examples-complete-agent-simulation)
   - [Advanced Usage](#python-examples-advanced-usage)

10. [C++ Examples](#part-3-c-examples)
    - [Basic Brain Usage](#c-examples-basic-brain-usage)
    - [Integration Examples](#c-examples-integration-examples)

11. [Phase 6 Integration Examples](#part-3-phase-6-integration-examples)

### Part 4: Code Organization

12. [Module Structure](#part-4-module-structure)
    - [Core Layer](#module-structure-core-layer)
    - [Brain Layer](#module-structure-brain-layer)
    - [Memory Systems](#module-structure-memory-systems)
    - [Neuromodulation Layer](#module-structure-neuromodulation-layer)
    - [Prediction System](#module-structure-prediction-system)
    - [Cognition Systems](#module-structure-cognition-systems)

13. [Package Organization](#part-4-package-organization)
    - [Python Package Structure](#package-organization-python-package-structure)
    - [C++ Package Structure](#package-organization-c-package-structure)

14. [Import/Export Conventions](#part-4-importexport-conventions)
    - [Python Import Conventions](#importexport-conventions-python-import-conventions)
    - [C++ Include Conventions](#importexport-conventions-c-include-conventions)

## Part 1: Technical Documentation

### Python API Reference

#### Core Classes

##### pynlm.createDefaultConfig()

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
```

**Description**: Creates a default configuration object with all standard parameters.

**Returns**: `pynlm.Config` object

**Example**:
```python
config = pynlm.createDefaultConfig()
print(f"Config has {config.getKeys().size()} parameters")
```

##### pynlm.createBrain(config)

```python
brain = pynlm.createBrain(config)
```

**Description**: Creates a brain object with the given configuration.

**Parameters**:
- `config`: `pynlm.Config` object containing brain configuration parameters

**Returns**: `pynlm.Brain` object

**Example**:
```python
config = pynlm.createDefaultConfig()
# Modify configuration
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

##### pynlm.createSimpleWorld()

```python
world = pynlm.createSimpleWorld()
```

**Description**: Creates a simple 2D world for NLM simulation.

**Returns**: `pynlm.SimpleWorld` object

**Example**:
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
print(f"World size: {world.getWidth()} x {world.getHeight()}")
```

##### pynlm.createAgentBrain(brain)

```python
agent = pynlm.createAgentBrain(brain)
```

**Description**: Creates an agent brain interface that connects the NLM brain to the world.

**Parameters**:
- `brain`: `pynlm.Brain` object to be connected

**Returns**: `pynlm.AgentBrain` object

**Example**:
```python
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print(f"Agent configured with {agent.getSensoryInputSize()} sensory inputs")
```

#### Brain Class

##### brain.initialize()

```python
brain.initialize()
```

**Description**: Initializes the brain with current configuration parameters. This is required before using the brain in a simulation.

**Example**:
```python
brain.initialize()
print("Brain initialization complete")
```

##### brain.step(step_number)

```python
brain.step(step_number)
```

**Description**: Performs one simulation step with the given step number.

**Parameters**:
- `step_number`: Current simulation step number (integer)

**Example**:
```python
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

##### brain.step(step_number, time)

```python
brain.step(step_number, time)
```

**Description**: Performs one simulation step with the given step number and timestamp.

**Parameters**:
- `step_number`: Current simulation step number (integer)
- `time`: Current simulation time in seconds (float)

**Example**:
```python
for step in range(1000):
    time = step * 0.1  # 100 ms per step
    brain.step(step, time)
```

##### brain.reset()

```python
brain.reset()
```

**Description**: Resets the brain to its initial state. This clears all activity and resets developmental stage.

**Example**:
```python
brain.reset()
print("Brain reset complete")
```

##### brain.receiveSensoryInput(percept)

```python
brain.receiveSensoryInput(percept)
```

**Description**: Injects sensory input into the brain. This is typically called by the AgentBrain class.

**Parameters**:
- `percept`: `pynlm.SensoryInput` or derived object containing sensory data

**Example**:
```python
# Get perceptual input from world
percept = world.getSensoryPercept()
# Process in brain
brain.receiveSensoryInput(percept)
```

##### brain.produceAction()

```python
action = brain.produceAction()
```

**Description**: Produces a motor action based on current neural activity. This analyzes motor neuron populations and produces an appropriate action.

**Returns**: `pynlm.Action` object containing the selected action

**Example**:
```python
action = brain.produceAction()
print(f"Brain produced action: {action.getName()}")
```

#### Config Class

##### config.loadFromFile(filepath)

```python
config.loadFromFile("config.json")
```

**Description**: Loads configuration from a JSON or YAML file.

**Parameters**:
- `filepath`: Path to configuration file (string)

**Example**:
```python
config.loadFromFile("my_config.json")
print("Configuration loaded successfully")
```

##### config.saveToFile(filepath)

```python
config.saveToFile("config.json")
```

**Description**: Saves configuration to a JSON or YAML file.

**Parameters**:
- `filepath`: Path where configuration should be saved (string)

**Example**:
```python
config.saveToFile("my_config.json")
print("Configuration saved successfully")
```

##### config.set(key, value)

```python
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)
```

**Description**: Sets a configuration parameter. For complex types (arrays, matrices), use the appropriate setter methods.

**Parameters**:
- `key`: Configuration key (string)
- `value`: Configuration value (type depends on key)

**Example**:
```python
# Simple types
config.set("brain.neuron_count", 1000)
config.set("brain.v_thresh", -50.0)

# Vector types
config.set("plasticity.stdp.learning_rate", 0.001)
```

##### config.get(key)

```python
value = config.get("brain.neuron_count")
```

**Description**: Gets a configuration parameter value.

**Parameters**:
- `key`: Configuration key (string)

**Returns**: Configuration value (type depends on key)

**Example**:
```python
neuron_count = config.get("brain.neuron_count")
print(f"Brain has {neuron_count} neurons")
```

##### config.has(key)

```python
if config.has("brain.neuron_count"):
    print("Neuron count is configured")
```

**Description**: Checks if a configuration key exists.

**Parameters**:
- `key`: Configuration key to check (string)

**Returns**: True if key exists, False otherwise

**Example**:
```python
if config.has("world.width"):
    width = config.get("world.width")
```

##### config.getKeys()

```python
keys = config.getKeys()
```

**Description**: Gets all configuration keys.

**Returns**: List of all configuration keys (list of strings)

**Example**:
```python
keys = config.getKeys()
print(f"Total parameters: {len(keys)}")
print(f"First few parameters: {keys[:5]}")
```

#### AgentBrain Class

##### agent.initialize(world)

```python
agent.initialize(world)
```

**Description**: Initializes the agent brain interface with the world.

**Parameters**:
- `world`: `pynlm.SimpleWorld` object to be connected

**Example**:
```python
agent.initialize(world)
print("Agent initialization complete")
```

##### agent.processSensoryInput(percept)

```python
agent.processSensoryInput(percept)
```

**Description**: Processes sensory input from the world and injects it into the brain.

**Parameters**:
- `percept`: `pynlm.SensoryPercept` object containing sensory data from world

**Example**:
```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

##### agent.decodeMotorCommand()

```python
motor_cmd = agent.decodeMotorCommand()
```

**Description**: Decodes motor command from brain activity and applies curiosity-based exploration.

**Returns**: `pynlm.MotorCommand` object representing the selected action

**Example**:
```python
motor_cmd = agent.decodeMotorCommand()
print(f"Agent selected: {motor_cmd}")
```

##### agent.applyRewardModulation(reward, predicted_reward)

```python
agent.applyRewardModulation(reward, predicted_reward)
```

**Description**: Applies reward-based neuromodulation to the brain, updating dopamine levels and affecting plasticity.

**Parameters**:
- `reward`: Actual reward received (float)
- `predicted_reward`: Expected reward (float)

**Example**:
```python
agent.applyRewardModulation(1.0, 0.8)  # Got 1.0 reward, expected 0.8
```

##### agent.updateDevelopment(timestep)

```python
agent.updateDevelopment(timestep)
```

**Description**: Updates developmental stage based on elapsed time, affecting plasticity rates and other developmental processes.

**Parameters**:
- `timestep`: Time step size (float, typically 0.1)

**Example**:
```python
agent.updateDevelopment(0.1)
```

#### SimpleWorld Class

##### world.configure(width, height, vision_width, vision_height)

```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

**Description**: Configures the world dimensions and sensory capabilities.

**Parameters**:
- `width`: World width in cells (integer)
- `height`: World height in cells (integer)
- `vision_width`: Vision sensor width in cells (integer)
- `vision_height`: Vision sensor height in cells (integer)

**Example**:
```python
world.configure(
    width=15,
    height=15,
    visionWidth=10,
    visionHeight=10
)
```

##### world.update(timestep)

```python
world.update(timestep)
```

**Description**: Updates the world state by the given time step.

**Parameters**:
- `timestep`: Time step size (float)

**Example**:
```python
world.update(0.1)  # Update by 100 ms
```

##### world.applyMotorCommand(motor_cmd, current_time)

```python
world.applyMotorCommand(motor_cmd, current_time)
```

**Description**: Applies a motor command to the world, updating agent position and triggering world events.

**Parameters**:
- `motor_cmd`: `pynlm.MotorCommand` object to execute
- `current_time`: Current simulation time (float)

**Example**:
```python
world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

#### Enumerations

##### NeuronType Enum

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

**Description**: Neuron type classifications used for organizing neural populations.

**Values**:
- `Excitatory`: Excitatory neurons that promote firing in connected neurons
- `Inhibitory`: Inhibitory neurons that suppress firing in connected neurons
- `Modulatory`: Modulatory neurons that affect plasticity and other processes
- `Sensory`: Sensory neurons that receive input from environment
- `Motor`: Motor neurons that produce output actions
- `Internal`: Internal neurons that represent internal state

##### DevelopmentalStage Enum

```python
# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

**Description**: Developmental stages that affect brain plasticity and learning capabilities.

**Values**:
- `Initial`: Birth state with high plasticity
- `CriticalPeriod`: Sensitive learning period
- `Maturation`: Continued development and refinement
- `Adult`: Stable adult learning capabilities
- `Aging`: Age-related decline in plasticity

##### ActionType Enum

```python
# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom
```

**Description**: High-level action types that can be performed by the agent.

### C++ API Documentation

#### Brain Class

##### Brain::step(SimulationStep currentStep)

```cpp
void Brain::step(SimulationStep currentStep);
```

**Description**: Performs one simulation step using the current step number.

**Parameters**:
- `currentStep`: Current simulation step number

**Note**: This method is called by the AgentBrain class and handles all neural dynamics, plasticity, and system updates.

##### Brain::step(SimulationStep currentStep, Timestamp currentTime)

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime);
```

**Description**: Performs one simulation step with explicit timestamp.

**Parameters**:
- `currentStep`: Current simulation step number
- `currentTime`: Current simulation time in seconds

**Note**: This is the more detailed version of step() that includes time information for systems that need it.

#### AgentBrain Class

##### AgentBrain::processSensoryInput(const SensoryPercept& percept)

```cpp
void AgentBrain::processSensoryInput(const SensoryPercept& percept);
```

**Description**: Processes sensory input from the world and injects it into the brain.

**Parameters**:
- `percept`: Sensory percept containing all sensor data

**Note**: This method handles conversion from world percepts to brain input signals and computes novelty for curiosity modulation.

### Architecture Documentation

#### System Architecture

The NLM architecture follows a modular, layered design with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM BRAIN                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

**Layers:**

1. **Core Layer**: Basic types, configuration, random generation, logging, time management
2. **Brain Layer**: Neural components (LIF dynamics, spikes, populations, regions, coordinator)
3. **Dynamics Layer**: Neural dynamics and spike propagation
4. **Plasticity Layer**: Learning mechanisms (STDP, Hebbian, structural plasticity)
5. **Development Layer**: Developmental stages and processes
6. **Neuromodulation Layer**: Modulators (dopamine, curiosity, novelty, prediction error)
7. **Memory Layer**: Working, episodic, associative, semantic, procedural memories
8. **Prediction Layer**: Temporal sequence learning and prediction
9. **Cognitive Layer**: Attention, planning, self-model, social learning
10. **Sensory Layer**: Vision, audio, internal signals processing
11. **Motor Layer**: Action representation and motor system
12. **Environment Layer**: World interface and interaction
13. **Experiment Layer**: Experiment framework and metrics

## Part 2: Developer Experience

### Setup and Installation

#### Prerequisites

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make, ninja, or MSBuild

#### Building from Source

##### Step 1: Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

##### Step 2: Build the C++ Core

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake (Release build recommended for performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)
```

##### Step 3: Install Python Bindings

```bash
# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

#### Building Python Bindings Only

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Build Python package
pip install build
python -m build

# Install from wheel
pip install dist/*.whl
```

### Development Setup Guide

#### Building the Project

##### Quick Build (Development)

```bash
# For development builds with debug symbols
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

##### Optimized Build (Release)

```bash
# For production builds with optimizations
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -O3
make -j$(nproc)
```

##### Building with Specific Options

```bash
# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
make install
```

#### Running Tests

##### Run Unit Tests

```bash
# Run individual unit tests
./test_config          # Configuration system tests
./test_config_enhanced # Enhanced configuration tests
./test_brain           # Brain integration tests
./test_clock           # Clock tests
```

##### Run Comprehensive Integration Tests

```bash
# Run all integration tests
./nlm_test_runner

# Or use CTest
ctest --output-on-failure
```

##### Run Performance Benchmarks

```bash
# Run performance benchmarks
./nlm_benchmark
```

#### Running Experiments

##### Phase 3 Experiments

```bash
# Run Phase 3 integration demo
./nlm_phase3_demo 5 200
```

Parameters:
- `5`: Number of episodes
- `200`: Maximum steps per episode

##### Phase 4 Experiments

```bash
# Run Phase 4 cognitive experiments
./nlm_phase4_demo 100
```

Parameters:
- `100`: Number of trials

##### Phase 6 Integration Test

```bash
# Run Phase 6 comprehensive integration test
./nlm_phase6_demo
```

### Contribution Guidelines

#### Code Style

##### Naming Conventions

- **Classes**: CamelCase (e.g., `NeuralPopulation`, `AgentBrain`)
- **Methods**: camelCase (e.g., `getSensoryInputSize()`, `initialize()`)
- **Variables**: camelCase (e.g., `simulationStep_`, `neuronCount_`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_SIMULATION_STEPS`, `NEURON_TYPE_COUNT`)
- **Files**: snake_case (e.g., `brain.cpp`, `agent_brain.hpp`)

##### Documentation Style

- **Doxygen comments** for C++ code
- **PEP 257** docstrings for Python code
- **Mathematical formulas** in LaTeX format where appropriate
- **Code examples** in both C++ and Python where possible

##### Code Quality

- **Header guards**: Use `#pragma once` for C++ headers
- **Include order**: System headers first, then local headers
- **Line length**: Maximum 100 characters for code, 120 for documentation
- **Indentation**: 4 spaces (no tabs)
- **Braces**: K&R style for functions, Allman style for structs

#### Testing Guidelines

##### Unit Testing

- **Test naming**: Use descriptive names (e.g., `testConfigVersion`, `testBrainStep`)
- **Test structure**: Arrange-Act-Assert pattern
- **Test coverage**: Aim for >90% code coverage
- **Test isolation**: Each test should be independent

##### Integration Testing

- **Test fixtures**: Use setup/teardown for shared resources
- **Test data**: Use deterministic seeds for reproducibility
- **Performance benchmarks**: Include timing measurements
- **Error handling**: Test edge cases and error conditions

##### Performance Testing

- **Benchmarking**: Use consistent timing methods
- **Memory profiling**: Track memory usage patterns
- **Scalability tests**: Test with increasing system sizes
- **Load testing**: Test with maximum expected loads

#### Documentation Guidelines

##### Documentation Structure

1. **Main Documentation**: Comprehensive overview with examples
2. **API Reference**: Detailed method/parameter documentation
3. **Tutorials**: Step-by-step guides for common tasks
4. **Examples**: Complete, runnable examples
5. **Troubleshooting**: Common issues and solutions

##### Documentation Generation

For Python documentation:
```bash
pip install sphinx sphinx-rtd-theme
make docs
```

For C++ documentation:
```bash
# Configure Doxygen
doxygen Doxyfile
```

### Best Practices

#### Code Organization

##### File Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
│   └── docs/           # Documentation
├── python/             # Python bindings
├── tests/             # Test suite
└── docs/              # Documentation
```

##### Module Dependencies

- **Core modules** should have minimal dependencies
- **Higher-level modules** depend on lower-level modules
- **Avoid circular dependencies** through proper interface design
- **Use forward declarations** to reduce include dependencies

#### API Design

##### Consistent Interfaces

- **Uniform parameter ordering**: Input parameters before output parameters
- **Consistent naming**: Similar concepts use similar names
- **Type safety**: Use strong typing and avoid implicit conversions
- **Error handling**: Provide clear error messages and recovery options

##### Extension Points

- **Plugin architecture**: Allow third-party extensions
- **Configuration system**: Enable runtime customization
- **Hook mechanisms**: Provide callbacks for custom behavior
- **Observer pattern**: Support for event-driven systems

#### Performance Considerations

##### Memory Management

- **Use smart pointers**: Prefer `std::shared_ptr` and `std::unique_ptr`
- **Minimize allocations**: Pre-allocate frequently used objects
- **Use object pools**: For high-frequency allocations
- **Cache frequently accessed data**: Use caching where appropriate

##### Computational Efficiency

- **Profile before optimizing**: Identify bottlenecks through profiling
- **Use SIMD instructions**: Where safe and beneficial
- **Optimize hot paths**: Focus on most frequently executed code
- **Minimize branching**: Use lookup tables where possible

## Part 3: Examples and Tutorials

### Python Examples

#### Basic Brain Usage

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

#### Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Complete agent simulation with brain, world, and learning."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and world interfaces
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # 3. Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode and execute action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Compute reward and learn
        reward = 0.0
        if world.getAgentBody().health > 0:
            reward = 1.0  # Simple reward for survival
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print("Agent simulation complete!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
```

#### Advanced Usage

```python
import pynlm
import numpy as np

def custom_simulation(custom_config=None):
    """Run a simulation with custom configuration."""
    
    # Create configuration
    if custom_config:
        config = pynlm.createDefaultConfig()
        for key, value in custom_config.items():
            config.set(key, value)
    else:
        config = pynlm.createDefaultConfig()
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world with custom settings
    config_width = config.get("world.width", 15)
    config_height = config.get("world.height", 15")
    world.configure(
        width=config_width,
        height=config_height,
        visionWidth=8,
        visionHeight=8
    )
    
    # Initialize
    brain.initialize()
    world.reset()
    world.setAgentStart(config_width/2, config_height/2)
    agent.initialize(world)
    
    # Create data collection
    history = {
        'steps': [],
        'firing_rates': [],
        'curiosity_levels': [],
        'action_counts': {}
    }
    
    # Run simulation
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward (example: based on resources)
        reward = 0.0
        if world.getAgentBody().energy > 50:
            reward = 0.5
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        history['steps'].append(step)
        history['firing_rates'].append(brain.getAverageFiringRate())
        history['curiosity_levels'].append(agent.getCuriosityLevel())
        
        action_name = action.getName()
        history['action_counts'][action_name] = history['action_counts'].get(action_name, 0) + 1
        
        # Optional: Add custom logic based on state
        if brain.getFiringNeuronCount() == 0:
            # Inject random current if no neurons are firing
            for i in range(10):
                neuron_id = pynlm.INVALID_NEURON_ID  # Example: could be implemented
                # brain.injectCurrent(neuron_id, 5.0)
    
    return history

# Run with custom configuration
custom_config = {
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.05,
    "world.width": 10,
    "world.height": 10,
    "plasticity.stdp.learning_rate": 0.005
}

history = custom_simulation(custom_config)

# Analyze results
avg_firing_rate = np.mean(history['firing_rates'])
print(f"Average firing rate: {avg_firing_rate:.3f}")
print(f"Total steps: {len(history['steps'])}")
print(f"Most common action: {max(history['action_counts'].items(), key=lambda x: x[1])}")
```

### C++ Examples

#### Basic Brain Usage

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <iostream>

int main() {
    // Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simple simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        if (step % 10 == 0) {
            std::cout << "Step " << step 
                     << ": " << brain->getFiringNeuronCount() 
                     << " neurons firing\n";
        }
    }
    
    std::cout << "Simulation complete!\n";
    return 0;
}
```

#### Integration Example

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <iostream>

int main() {
    // Create brain with enhanced configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 2000);
    config->set("world.width", 15);
    config->set("world.height", 15);
    config->set("world.vision_width", 8);
    config->set("world.vision_height", 8);
    
    // Create brain and initialize
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create agent interface
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create and configure world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(15, 15, 8, 8);
    world->reset();
    world->setAgentStart(7.5, 7.5);
    
    // Initialize agent
    agent->initialize(*world);
    
    // Enable learning subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // Run agent simulation
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory percept
        auto percept = world->getSensoryPercept();
        
        // Process in agent/brain
        agent->processSensoryInput(percept);
        brain->step(step);
        
        // Decode and execute action
        auto action = brain->produceAction();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Compute reward (example: survival reward)
        double reward = (world->getAgentBody().health > 0) ? 1.0 : 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ":\n";
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << "\n";
            std::cout << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
            std::cout << "  Agent position: (" << world->getAgentBody().x 
                     << ", " << world->getAgentBody().y << ")\n";
        }
    }
    
    std::cout << "Agent simulation complete!\n";
    return 0;
}
```

### Phase 6 Integration Examples

#### Complete Integration Test

```python
import pynlm

def phase6_integration_test():
    """Run a complete Phase 6 integration test."""
    
    print("=== NLM Phase 6 Integration Test ===\n")
    
    # 1. Create brain with all systems
    print("1. Creating brain with enhanced configuration...")
    config = pynlm.createDefaultConfig()
    
    # Configure for comprehensive testing
    config.set("brain.neuron_count", 3000)
    config.set("brain.synapse_density", 0.1)
    config.set("world.width", 20)
    config.set("world.height", 20)
    config.set("world.vision_width", 10)
    config.set("world.vision_height", 10)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   and {brain.getTotalSynapseCount()} synapses\n")
    
    # 2. Create complete agent environment
    print("2. Creating agent environment...")
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 10, 10)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems for integration testing
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    print("   All subsystems enabled\n")
    
    # 3. Run integration test
    print("3. Running integration test (1000 steps)...")
    
    metrics = {
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'action_counts': {},
        'development_stages': [],
        'memory_usage': []
    }
    
    for step in range(1000):
        # World update
        world.update(0.1)
        
        # Perception-action loop
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = 0.0
        # Simple reward: stay alive, explore
        if world.getAgentBody().health > 0:
            reward += 0.1
        if agent.getCuriosityLevel() > 0.5:
            reward += 0.5  # Reward exploration
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['development_stages'].append(int(brain.getDevelopmentalStage()))
        
        action_name = action.getName()
        metrics['action_counts'][action_name] = metrics['action_counts'].get(action_name, 0) + 1
        
        # Progress reporting
        if step % 100 == 0:
            print(f"   Step {step}: Firing rate={brain.getAverageFiringRate():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Stage={brain.getDevelopmentalStage()}")
    
    print("   Integration test complete!\n")
    
    # 4. Analyze results
    print("4. Analyzing results...")
    
    avg_firing_rate = sum(metrics['firing_rates']) / len(metrics['firing_rates'])
    max_firing_rate = max(metrics['firing_rates'])
    min_firing_rate = min(metrics['firing_rates'])
    
    avg_curiosity = sum(metrics['curiosity_levels']) / len(metrics['curiosity_levels'])
    final_stage = metrics['development_stages'][-1]
    
    print(f"   Average firing rate: {avg_firing_rate:.3f} Hz")
    print(f"   Firing rate range: {min_firing_rate:.3f} - {max_firing_rate:.3f} Hz")
    print(f"   Average curiosity level: {avg_curiosity:.3f}")
    print(f"   Final developmental stage: {final_stage}")
    
    print(f"\n   Action distribution:")
    for action, count in sorted(metrics['action_counts'].items(), key=lambda x: x[1], reverse=True):
        percentage = (count / 1000) * 100
        print(f"     {action}: {count} ({percentage:.1f}%)")
    
    # 5. Integration assessment
    print("\n5. Integration Assessment:")
    
    integration_score = 0
    max_score = 100
    
    # Check key integration aspects
    if avg_firing_rate > 0.1:
        integration_score += 20  # Neural activity
        print("   ✅ Neural activity present")
    else:
        print("   ⚠️  Low neural activity (may need more input)")
    
    if avg_curiosity > 0.1:
        integration_score += 20  # Neuromodulation
        print("   ✅ Neuromodulation active")
    else:
        print("   ⚠️  Neuromodulation limited")
    
    if final_stage >= pynlm.DevelopmentalStage.Adult:
        integration_score += 20  # Development
        print("   ✅ Development progressing")
    else:
        print("   ⚠️  Development limited")
    
    if len(metrics['action_counts']) > 2:
        integration_score += 20  # Behavior diversity
        print("   ✅ Behavioral diversity")
    else:
        print("   ⚠️  Limited behavioral diversity")
    
    if avg_firing_rate > 0.5 and avg_curiosity > 0.3:
        integration_score += 20  # System coherence
        print("   ✅ System coherence")
    else:
        print("   ⚠️  System coherence limited")
    
    print(f"\n   Overall Integration Score: {integration_score}/{max_score} ({integration_score}%)\n")
    
    if integration_score >= 80:
        print("   🎉 Excellent integration! All major systems working together.")
    elif integration_score >= 60:
        print("   👍 Good integration with room for improvement.")
    elif integration_score >= 40:
        print("   ⚠️  Basic integration achieved, needs refinement.")
    else:
        print("   ❌ Poor integration. Requires major attention.")
    
    return metrics

# Run the integration test
metrics = phase6_integration_test()
```

## Part 4: Code Organization

### Module Structure

#### Core Layer

The Core layer contains fundamental types, configuration management, and utility systems:

```
NLM/
└── src/core/
    ├── Config/              # Configuration system
    │   ├── Config.hpp
    │   └── Config.cpp
    ├── Random/              # Random number generation
    │   ├── Random.hpp
    │   └── Random.cpp
    ├── Logger/              # Structured logging
    │   ├── Logger.hpp
    │   └── Logger.cpp
    └── SimulationClock/     # Time management
        ├── SimulationClock.hpp
        └── SimulationClock.cpp
```

**Responsibilities:**
- Type definitions (NeuronId, SynapseId, RegionId, etc.)
- Configuration loading, parsing, and validation
- Random number generation for simulations
- Structured logging and debugging
- Simulation time management

#### Brain Layer

The Brain layer implements the core neural simulation components:

```
NLM/
└── src/brain/
    ├── Brain.hpp                    # Main brain coordinator
    ├── Brain.cpp                    # Brain implementation
    ├── Neuron.hpp                   # Individual neuron model
    ├── Neuron.cpp                   # Neuron implementation
    ├── Synapse.hpp                   # Synaptic connections
    ├── Synapse.cpp                   # Synapse implementation
    ├── NeuralPopulation.hpp          # Neuron groups
    ├── NeuralPopulation.cpp          # Population management
    ├── NeuralRegion.hpp              # Brain regions
    └── NeuralRegion.cpp              # Region management
```

**Responsibilities:**
- LIF neuron dynamics and spike generation
- Synaptic transmission with delays
- Neural population management
- Brain region organization
- Spike system coordination

#### Memory Systems

The Memory layer implements various memory systems:

```
NLM/
└── src/memory/
    ├── NeuralWorkingMemory.hpp      # Working memory
    ├── NeuralWorkingMemory.cpp      # Working memory implementation
    ├── NeuralEpisodicMemory.hpp     # Episodic memory
    ├── NeuralEpisodicMemory.cpp     # Episodic memory implementation
    ├── NeuralAssociativeMemory.hpp  # Associative memory
    ├── NeuralAssociativeMemory.cpp  # Associative memory implementation
    ├── NeuralSemanticMemory.hpp     # Semantic memory
    └── NeuralSemanticMemory.cpp     # Semantic memory implementation
```

**Responsibilities:**
- Working memory for temporary information storage
- Episodic memory for experience recording
- Associative memory for pattern associations
- Semantic memory for knowledge storage

#### Neuromodulation Layer

The Neuromodulation layer implements learning signals:

```
NLM/
└── src/neuromodulation/
    ├── Neuromodulator.hpp          # Base neuromodulator class
    ├── Neuromodulator.cpp          # Neuromodulator implementation
    ├── Dopamine.hpp                # Dopamine (reward)
    ├── Dopamine.cpp                 # Dopamine implementation
    ├── Novelty.hpp                 # Novelty detection
    ├── Novelty.cpp                  # Novelty implementation
    ├── Curiosity.hpp               # Curiosity (exploration)
    ├── Curiosity.cpp                # Curiosity implementation
    └── PredictionError.hpp         # Prediction error
        └── PredictionError.cpp      # Prediction error implementation
```

**Responsibilities:**
- Dopamine for reward and reinforcement learning
- Novelty for detecting novel experiences
- Curiosity for driving exploration behavior
- Prediction error for predictive coding

#### Prediction System

The Prediction layer implements temporal sequence learning:

```
NLM/
└── src/prediction/
    ├── PredictionSystem.hpp        # Main prediction system
    ├── PredictionSystem.cpp        # Prediction system implementation
    ├── NeuralPrediction.hpp        # Neural prediction mechanism
    └── NeuralPrediction.cpp        # Neural prediction implementation
```

**Responsibilities:**
- Predict next states from current patterns
- Compute prediction errors
- Track prediction confidence
- Learn temporal sequences

#### Cognition Systems

The Cognition layer implements higher-level cognitive functions:

```
NLM/
└── src/cognition/
    ├── AttentionalSelection.hpp    # Competitive attention
    ├── AttentionalSelection.cpp    # Attention implementation
    ├── NeuralPlanner.hpp           # Action planning
    ├── NeuralPlanner.cpp           # Planning implementation
    ├── ConceptFormation.hpp         # Pattern discovery
    └── ConceptFormation.cpp         # Concept formation
```

**Responsibilities:**
- Competitive attention for focus management
- Multi-step action planning
- Pattern discovery without labels
- Self-model formation

### Package Organization

#### Python Package Structure

```
python/
├── pyproject.toml              # Package metadata
├── setup.py                    # Legacy setup (deprecated)
└── bindings.cpp                # C++ bindings source
```

**Key Files:**

- **pyproject.toml**: Modern Python packaging with scikit-build-core
- **bindings.cpp**: Pybind11 bindings exposing C++ API to Python

#### C++ Package Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── ...             # Other modules
│   └── ...
├── include/            # Public headers
│   └── nlm/           # NLM namespace
├── tests/             # Test suite
├── docs/              # Documentation
└── ...                # Other directories
```

### Import/Export Conventions

#### Python Import Conventions

**Module Organization:**
```python
import pynlm

# Core components
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enumerations
from pynlm import NeuronType, DevelopmentalStage, ActionType

# Types
from pynlm import NeuronId, SynapseId
```

**Import Style Guide:**

1. **Standard Library First**: `import standard_lib`
2. **Third-Party Libraries**: `import third_party_lib`
3. **Local Application Imports**: `from app import module`
4. **Local Package Imports**: `from . import module` (relative)
5. **Avoid Wildcard Imports**: `from module import *` discouraged

#### C++ Include Conventions

**Include Style Guide:**

```cpp
// Standard libraries first
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

// Local system headers second
#include "../core/Config/Config.hpp"
#include "../brain/Neuron.hpp"

// Project headers last
#include "Brain.hpp"
#include "AgentBrain.hpp"
```

**Include Guards:**

Use `#pragma once` for all C++ headers:

```cpp
#pragma once

// Header contents...
```

**Namespace Usage:**

- **Project namespace**: `nlm` for all NLM classes and functions
- **Standard library**: `std` for standard C++ library
- **Nested namespaces**: Use `nlm::detail` for implementation details

## Part 5: Advanced Topics

### Configuration System

#### Configuration Sources

NLM supports multiple configuration sources with the following precedence:

1. **Runtime**: Programmatically set during execution
2. **Command Line**: Parsed from command line arguments
3. **File**: JSON or YAML configuration files
4. **Environment**: Environment variables
5. **Default**: Built-in default values

#### Configuration Types

```cpp
enum class ConfigDataType {
    Int, Int64, Double, Bool, String,
    IntVector, DoubleVector, StringVector,
    Matrix
};
```

**Supported Configuration Parameters:**

- **Brain Parameters**: Neuron count, connection density, threshold values
- **Plasticity Parameters**: STDP rates, Hebbian learning, structural plasticity
- **Neuromodulation Parameters**: Dopamine scaling, curiosity enablement
- **World Parameters**: Size, vision capabilities, physics properties
- **Development Parameters**: Stage transitions, plasticity modifiers

### Performance Optimization

#### Memory Management Strategies

1. **Smart Pointers**: Use `std::shared_ptr` and `std::unique_ptr`
2. **Object Pools**: Pre-allocate frequently used objects
3. **Memory Alignment**: Proper alignment for SIMD operations
4. **Cache Locality**: Optimize data layout for CPU cache

#### Computational Optimization

1. **SIMD Vectorization**: Use SIMD where safe and beneficial
2. **Event-Driven Simulation**: Minimize unnecessary computations
3. **Parallel Processing**: Use threading where appropriate
4. **Profile-Guided Optimization**: Identify and optimize bottlenecks

### Testing Strategies

#### Unit Testing

- **Test-Driven Development**: Write tests before implementing
- **Edge Cases**: Test boundary conditions and error cases
- **Deterministic Results**: Use fixed random seeds for reproducibility
- **Isolation**: Each test should be independent

#### Integration Testing

- **System Integration**: Test all major components working together
- **Performance Testing**: Verify performance requirements
- **Stress Testing**: Test with maximum loads
- **Regression Testing**: Ensure existing functionality remains intact

#### Performance Benchmarking

**Key Metrics:**
- Simulation speed (steps per second)
- Memory usage patterns
- CPU utilization
- I/O performance

**Benchmarking Tools:**
- Google Benchmark (C++)
- pytest-benchmark (Python)
- Custom timing utilities

### Extensibility

#### Adding New Components

**Steps to add a new component:**

1. **Define Interface**: Create abstract base class with clear contract
2. **Implement Core**: Implement concrete class following interface
3. **Integrate**: Add to Brain or AgentBrain for system integration
4. **Test**: Write comprehensive tests
5. **Document**: Add API documentation

#### Plugin Architecture

**Extension Points:**
- **New Plasticity Rules**: Implement `PlasticityRule` interface
- **New Neuromodulators**: Implement `Neuromodulator` interface
- **New Memory Systems**: Implement memory system interfaces
- **New Environment Types**: Implement `Environment` interface

#### Configuration Extensibility

**Schema Evolution:**
- Versioned configuration schemas
- Automatic migration support
- Backward-compatible parameter changes

**Advanced Configuration:**
- Template-based schemas
- Configuration validation
- Runtime configuration updates

### Future Directions

#### Research Areas

1. **Neurally Inspired Learning**: More biologically plausible learning rules
2. **Multi-Agent Systems**: Interaction between multiple agents
3. **Hierarchical Processing**: Multiple levels of processing
4. **Cognitive Architecture**: More comprehensive cognitive models
5. ** Embodied Cognition**: Better integration with physical environments

#### Performance Goals

1. **Real-Time Simulation**: Millisecond-scale simulation speed
2. **Large-Scale Models**: Millions of neurons
3. **Distributed Computing**: Multi-node simulation
4. **Energy Efficiency**: Low power consumption
5. **Adaptive Scaling**: Automatic resource management

## Conclusion

NLM represents a significant step forward in artificial brain simulation, combining biologically inspired neural dynamics with sophisticated cognitive architectures. This comprehensive documentation provides the foundation for understanding, using, and extending the NLM system.

The implementation successfully integrates all Phase 6 systems into a coherent artificial brain capable of:

- Processing sensory information
- Forming and maintaining memories
- Modulating its own learning through neuromodulation
- Adapting through developmental processes
- Persisting across time and experience

The documentation, code organization, and developer experience improvements outlined in this document will ensure that NLM continues to evolve as a powerful platform for artificial intelligence research and development.

---

**License**: MIT  
**Copyright**: NLM Project 2024  
**Version**: 0.1.0  
**Status**: Phase 6 - Final Integration
