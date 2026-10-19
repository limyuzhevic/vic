# NLM (Neural Learning Machine) - User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain system. It aims to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## System Architecture

NLM implements a complete closed-loop brain simulation with the following components:

### Core Neural System
- **Brain**: Central coordinator with spiking neurons and event-driven dynamics
- **Neurons**: Leaky Integrate-and-Fire (LIF) neurons with realistic dynamics
- **Synapses**: Event-driven synaptic connections with plasticity
- **Regions**: Neural populations organized into functional regions

### Integrated Systems
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, and prediction error
- **Prediction System**: Sensory prediction and error computation
- **Cognition Systems**: Neural planning, concept formation, and attentional selection
- **Development System**: Developmental stages affecting plasticity and behavior

### Environment Interface
- **SimpleWorld**: 2D grid world for simulation
- **Sensory Processing**: Vision, audio, and internal signal processing
- **Motor Control**: Action selection and execution

## Quick Start Guide

### 1. Using Python Bindings (Recommended for research and prototyping)

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and configure
world = pynlm.createSimpleWorld()
world.configure(16, 16, 8, 8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Run simulation for 100 steps
for step in range(100):
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input and update brain
    agent.processSensoryInput(percept)
    brain.step(step, step * 0.001)
    
    # Get action and execute in world
    cmd = agent.decodeMotorCommand()
    result = world.applyMotorCommand(cmd, step * 0.001)
    
    # Apply reward modulation
    agent.applyRewardModulation(result.reward, 0.0)

# Analyze results
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.4f}")
```

### 2. Using Command Line Interface

Install the NLM CLI tool:
```bash
pip install -e .
```

Run a simple demo:
```bash
nlm_cli.py demo --steps 500
```

Initialize a new simulation:
```bash
nlm_cli.py init --width 20 --height 20
```

Run a simulation with checkpointing:
```bash
nlm_cli.py run --steps 2000 --checkpoint save_after_1000.bin
```

Analyze a brain state:
```bash
nlm_cli.py analyze --output analysis.json
```

### 3. Using C++ API

For maximum performance, use the C++ API directly:

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world
    SimpleWorld world;
    world.configure(16, 16, 8, 8);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run simulation
    for (uint64_t step = 0; step < 1000; ++step) {
        // Get observation
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action
        world.applyAction(cmd, step * 0.001);
    }
    
    // Analyze results
    brain->logStatus();
    return 0;
}
```

## Configuration Reference

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | size_t | 1000 | Number of neurons in the brain |
| `region_count` | size_t | 1 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of synapse formation |
| `simulation_timestep` | double | 0.001 | Simulation time step size |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP Long-Term Potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP Long-Term Depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse removal |

### Integration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `replay_interval` | size_t | 100 | Steps between memory replay |
| `consolidation_interval` | size_t | 1000 | Steps between memory consolidation |

## Building and Running

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with 4 parallel jobs
make -j4
```

### Running Demos

Phase 6 Integration Test:
```bash
./nlm_phase6_demo
```

### Running Tests

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific tests
ctest -R test_brain
ctest -R test_neuron
```

## Python Module Usage

### Available Classes

#### Config
Configuration management for NLM parameters.

**Methods:**
- `loadFromFile(filepath)`: Load configuration from JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save configuration to JSON file
- `has(key)`: Check if configuration key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all configuration entries
- `summary()`: Get configuration summary

#### Brain
Main brain simulation class.

**Methods:**
- `initialize()`: Initialize brain with configuration
- `step(currentStep, currentTime)`: Perform simulation step
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Produce motor action
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- Various getter methods for statistics

#### SimpleWorld
2D simulation world.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get current sensory percept
- `getAgentBody()`: Get agent body state

#### AgentBrain
Interface between brain and world.

**Methods:**
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode brain activity into motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward modulation
- Various enable/disable methods for neuromodulation

### Helper Functions

- `createDefaultConfig()`: Create default configuration
- `createBrain(config)`: Create brain with configuration
- `createSimpleWorld()`: Create simple world
- `createAgentBrain(brain)`: Create agent brain interface

## Advanced Usage

### Custom Sensory Inputs

```python
# Create custom sensory input
sensory = pynlm.Vision(32, 32, 3)  # 32x32 RGB image

# Set pixel data
import numpy as np
data = np.random.rand(32 * 32 * 3).astype(np.float32)
sensory.setData(data.tolist())

# Inject into brain
brain.receiveSensoryInput(sensory)
```

### Checkpoint Management

```python
# Save brain state
brain.save("brain_state.bin")

# Load brain state
brain.load("brain_state.bin")
```

### Analysis and Visualization

```python
# Compare two brain states
comparison = pynlm.compare_brain_states(brain1, brain2)
print(f"Spike ratio: {comparison['spike_ratio']:.4f}")
print(f"Firing rate ratio: {comparison['firing_rate_ratio']:.4f}")

# Export for external analysis
pynlm.export_brain_for_analysis(brain, "brain_for_analysis.bin")
```

## Troubleshooting

### Common Issues

**Issue: "pynlm module not available"**
Solution: Install Python bindings:
```bash
pip install pynlm
```

**Issue: "Brain initialization failed"**
Solution: Check configuration parameters and ensure required libraries are built.

**Issue: Simulation running slowly**
Solution: Reduce `neuron_count` or increase `simulation_timestep`.

**Issue: Memory usage too high**
Solution: Reduce `neuron_count` or `region_count`.

### Debugging

```python
# Enable logging
brain.logStatus()

# Create configuration with debug logging
config = pynlm.createDefaultConfig()
config.set("log_level", "debug")
```

## Performance Optimization

### For High-Performance Applications

1. **Use C++ API directly** for maximum performance
2. **Reduce neuron count** for faster simulations
3. **Increase simulation timestep** for larger time steps
4. **Disable unnecessary features** like replay or development

### For Research and Development

1. **Use Python bindings** for rapid prototyping
2. **Use CLI tool** for easy command-line access
3. **Use comprehensive logging** for debugging
4. **Use checkpointing** for resuming interrupted simulations

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python, follow these guidelines:

1. **Configuration**: Use `createDefaultConfig()` and set parameters with `config.set()`
2. **Brain Creation**: Use `createBrain(config)` instead of `Brain(config)`
3. **World Creation**: Use `createSimpleWorld()` and `configure()`
4. **Agent Creation**: Use `createAgentBrain(brain)` and `initialize(world)`
5. **Simulation Loop**: Use the helper functions in the simulation examples

### From Old to New Python API

If you're using an older version of the Python bindings, note these changes:

1. **Novelty class**: Now available as `pynlm.Novelty`
2. **Dopamine class**: Now available as `pynlm.Dopamine`
3. **Curiosity class**: Now available as `pynlm.Curiosity`
4. **PredictionError class**: Now available as `pynlm.PredictionError`

## Examples Directory

The `examples/` directory contains additional usage examples:

- `simple_demo.py`: Basic simulation example
- `checkpoint_demo.py`: Checkpoint management example
- `analysis_demo.py`: Brain analysis example
- `config_demo.py`: Configuration management example

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
