# NLM (Neural Learning Machine) - Documentation

An Experimental Artificial Developmental Brain System

**NLM (熙然, meaning "serene flow")** is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Table of Contents

- [Quick Start](#quick-start)
- [Building and Installation](#building-and-installation)
- [Python API Reference](#python-api-reference)
- [C++ API Reference](#c-api-reference)
- [Examples and Demos](#examples-and-demos)
- [Configuration Reference](#configuration-reference)
- [Performance and Optimization](#performance-and-optimization)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)

## Quick Start

### Python Usage (Recommended)

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# Run a simple simulation
for i in range(1000):
    brain.step(i)
    
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### C++ Usage

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    return 0;
}
```

## Building and Installation

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Python Installation

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Python API Reference

### Core Classes

#### Brain
The main neural simulation brain class.

**Methods:**
- `createBrain(config)` - Create a new brain with configuration
- `initialize()` - Initialize the brain with configuration
- `step(step_number)` - Perform a simulation step
- `step(step_number, time)` - Perform a simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input into the brain
- `injectCurrent(neuron, current)` - Inject current into a specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current into all neurons of a specific type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add a new neural region
- `getRegion(id)` - Get a region by ID

**Properties:**
- `getTotalNeuronCount()` - Total neuron count across all regions
- `getTotalSynapseCount()` - Total synapse count across all regions
- `getActiveNeuronCount()` - Count of active neurons
- `getFiringNeuronCount()` - Count of currently firing neurons
- `getAverageFiringRate()` - Average firing rate across all neurons
- `getExcitationInhibitionRatio()` - E/I balance ratio
- `getTotalSpikeCount()` - Total spike count
- `getPendingSpikeEventCount()` - Number of pending spike events

#### Config
Configuration class for NLM system.

**Methods:**
- `createDefaultConfig()` - Create a default configuration
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load configuration from command line
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if a configuration key exists
- `getKeys()` - Get all configuration keys
- `set(key, value)` - Set configuration value
- `getOr(key, defaultValue)` - Get value with default

#### AgentBrain
Interface connecting NLM brain to world.

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode motor command from brain
- `applyRewardModulation(reward, predicted_reward)` - Apply reward neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity

#### SimpleWorld
Simple 2D world for NLM simulation.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world simulation
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept from world
- `getAgentBody()` - Get agent body state

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

#### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period stage
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### ActionType
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

## Examples and Demos

### Basic Usage

**File:** `examples/basic_usage.py`

A minimal example showing how to create and run a simple neural simulation.

**Key Features:**
- Create default configuration
- Initialize brain
- Run simulation loop
- Monitor firing activity

### Agent Simulation

**File:** `examples/agent_simulation.py`

Complete example showing integration between brain, agent, and world.

**Key Features:**
- Create and configure brain, world, and agent
- Enable all subsystems (reward, development, curiosity)
- Run full episode simulation
- Track performance metrics

### Visualization

**File:** `examples/visualization.py`

Example showing how to visualize brain activity.

**Key Features:**
- Create visual representation of brain state
- Monitor neural activity patterns
- Show spike timing and connectivity

### Experiment Runner

**File:** `examples/experiment_runner.py`

Framework for running systematic experiments.

**Key Features:**
- Parameter sweep capabilities
- Automated experiment execution
- Result collection and analysis

## Configuration Reference

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Performance and Optimization

### Performance Considerations

1. **Neuron Count**: Performance scales linearly with neuron count
2. **Simulation Speed**: Typical timestep = 0.001ms (1ms simulated)
3. **Memory Usage**: ~100KB per 1000 neurons
4. **CPU Usage**: ~10% per CPU core per 10,000 neurons

### Optimization Strategies

1. **Reduce Neuron Count**: Start with smaller networks for prototyping
2. **Disable Unused Subsystems**: Turn off subsystems you don't need
3. **Adjust Timestep**: Increase timestep for faster simulation
4. **Use Multi-threading**: Enable parallel processing where available

### Benchmark Results

Performance benchmarks show:
- 1000 neurons: ~100ms per 1000ms simulation
- 10,000 neurons: ~1.2s per 1000ms simulation
- 100,000 neurons: ~15s per 1000ms simulation

## Troubleshooting

### Common Issues

**ImportError: No module named 'pynlm'**
- Install with: `pip install pynlm` or `pip install -e .`

**CMake cannot find Python**
- Install dependencies: `pip install pybind11 scikit-build-core pybind11[global]`
- Set Python executable: `cmake .. -DPython_EXECUTABLE=$(which python)`

**Compilation errors**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**Runtime issues**
- Ensure `initialize()` is called before `step()`
- Reset brain state with `reset()` before re-running simulation

### Getting Help

- Check the [NLM documentation](https://nlm.readthedocs.io)
- Visit the [NLM GitHub repository](https://github.com/nlm/nlm)
- Join the [NLM discussion forum](https://community.nlm.org)

## Contributing

### Code Contributions

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests: `ctest --output-on-failure`
5. Submit a pull request

### Documentation Contributions

1. Fork the repository
2. Edit documentation files in the `docs/` directory
3. Submit a pull request

### Testing Contributions

1. Fork the repository
2. Add tests to the `tests/` directory
3. Run the test suite: `ctest --output-on-failure`
4. Submit a pull request

## License

MIT License

Copyright (c) 2026 NLM Project Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
