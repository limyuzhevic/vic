NLM (熙然) - Neural Learning Machine
========================================

A Python package for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

NLM is an experimental artificial developmental brain project. The Python bindings provide access to the C++ neural simulation framework through a simple, intuitive API.

## Features

- **Brain Simulation**: Create and run virtual brains with neurons and synapses
- **World Interaction**: Simulate environments with sensory input and motor output
- **Learning**: Support for Hebbian learning, STDP, and neuromodulation
- **Integration**: Access to all NLM Phase 2+ features
- **Visualization**: Built-in and extensible visualization capabilities

## Installation

```bash
pip install pynlm
```

Or build from source:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
# Then install the Python bindings
pip install python/
```

## Quick Start

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Advanced Usage

### World Simulation

```python
import pynlm

# Create world and brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete agent loop
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning and Plasticity

```python
import pynlm

# Enable learning features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)

agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Stage: {agent.getDevelopmentalStage()}")
        print(f"Neuromodulation: {agent.getNeuromodulationLevel()}")
```

### Configuration and Customization

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Set simulation parameters
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.2)

# STDP parameters
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
config.set("stdp_tau", 20.0)

# Developmental parameters
config.set("critical_period_start", 10.0)
config.set("critical_period_end", 50.0)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory and Persistence

```python
import pynlm
import numpy as np

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Save brain state
brain.save("my_brain.nlm")

# Load brain state back
brain.load("my_brain.nlm")
```

## Core Classes

### Brain

The main neural simulation brain class:

- `initialize()`: Initialize the brain with current configuration
- `step(step, time)`: Perform one simulation step
- `receiveSensoryInput(input)`: Inject sensory data
- `produceAction()`: Generate motor output
- `reset()`: Reset brain to initial state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

### Config

Configuration management:

- `loadFromFile(filepath)`: Load from JSON configuration file
- `loadFromArgs(argc, argv)`: Load from command line
- `saveToFile(filepath)`: Save configuration to JSON
- `has(key)`: Check if configuration key exists
- `getKeys()`: Get all configuration keys

### SimpleWorld

2D simulation world:

- `configure(width, height, visionWidth, visionHeight)`: Set up world dimensions
- `reset()`: Reset world state
- `update(timestep)`: Advance world simulation
- `applyMotorCommand(action, time)`: Execute motor actions
- `getSensoryPercept()`: Get current sensory observations
- `getAgentBody()`: Get agent position and state

### AgentBrain

Bridge between brain and world:

- `initialize(world)`: Connect to world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Convert neural activity to actions
- `applyRewardModulation(reward, predicted)`: Learn from rewards

## Advanced Features

### Neural Network Operations

```python
import pynlm
import numpy as np

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access individual regions
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getPopulationCount()} populations")
    
    for population in region.getPopulations():
        print(f"  Population: {population.getNeuronCount()} neurons")
        print(f"  Type: {population.getNeuronType()}")
```

### Statistical Analysis

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Get statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Batch Processing

```python
import pynlm
import time

def run_simulation(config_file, steps=1000):
    """Run a complete simulation with given configuration"""
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f}s")
    print(f"Results: {brain.getTotalSpikeCount()} spikes over {steps} steps")
    
    return brain

# Run simulation
brain = run_simulation("my_config.json", 2000)
```

## Development API

### Building Custom Experiments

```python
import pynlm
from typing import Callable, List

class CustomExperiment:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = None
        self.world = None
        self.metrics = []
    
    def setup(self, world_config):
        """Setup the experiment environment"""
        self.brain.initialize()
        self.world = pynlm.createSimpleWorld()
        self.world.configure(**world_config)
        self.world.reset()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def run_episode(self, max_steps=1000) -> dict:
        """Run a single episode and collect metrics"""
        for step in range(max_steps):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        return self.get_metrics()
    
    def get_metrics(self) -> dict:
        """Collect and return experiment metrics"""
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'learning_stage': self.agent.getDevelopmentalStage(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
        }

# Use the custom experiment
experiment = CustomExperiment()
experiment.setup({
    'width': 30, 'height': 30,
    'visionWidth': 12, 'visionHeight': 12
})

results = experiment.run_episode(500)
print(results)
```

## Performance Optimization

### Large-Scale Simulations

```python
import pynlm
import threading
import concurrent.futures

class BatchSimulator:
    def __init__(self, num_brains=10):
        self.num_brains = num_brains
        self.brains = []
    
    def create_brain(self, seed):
        """Create a brain with specific seed for reproducibility"""
        config = pynlm.createDefaultConfig()
        config.set("random_seed", seed)
        config.set("neuron_count", 500)  # Smaller for batch processing
        return pynlm.createBrain(config)
    
    def run_brain(self, brain_id):
        """Run a single brain simulation"""
        brain = self.create_brain(brain_id * 1000)
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
        
        return brain_id, brain.getTotalSpikeCount()
    
    def run_batch(self):
        """Run all brains in parallel"""
        with concurrent.futures.ThreadPoolExecutor(max_workers=self.num_brains) as executor:
            futures = [executor.submit(self.run_brain, i) for i in range(self.num_brains)]
            results = [future.result() for future in concurrent.futures.as_completed(futures)]
        
        return results

# Run batch simulation
simulator = BatchSimulator(num_brains=20)
results = simulator.run_batch()
print(results)
```

## Error Handling and Debugging

### Exception Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Simulation error: {e}")
    # Graceful error recovery
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
```

### Logging and Monitoring

```python
import pynlm
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, 
                   format='%(asctime)s - %(levelname)s - %(message)s')

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Add custom logging
brain.logStatus()  # Built-in logging

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    if step % 50 == 0:
        logging.info(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
```

## Installation Troubleshooting

### Common Issues

1. **Missing Dependencies**
```bash
pip install pybind11 numpy matplotlib
```

2. **Build Failures**
```bash
# Check for required system dependencies
# Linux: sudo apt-get install python3-dev cmake build-essential
# macOS: xcode-select --install
```

3. **Import Errors**
```python
# Verify the module is installed correctly
import pynlm
print(pynlm.__version__)  # Should print version info
```

## Examples Directory

The package includes example scripts in the `examples/` directory:

- `simple_brain.py`: Basic brain simulation
- `world_simulation.py`: Complete agent-world interaction
- `learning_experiment.py`: Plasticity and learning demonstration
- `performance_test.py`: Benchmarking and optimization

## Troubleshooting

For issues with the Python bindings, refer to the main NLM documentation for:

- Build configuration options
- Platform-specific issues
- Performance troubleshooting
- API changes between versions

## License

MIT License

Copyright (c) 2026 NLM Project Authors

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
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
