# NLM (Neural Learning Machine) - Quick Start Guide

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This guide provides a quick introduction to using NLM with the enhanced Python API and command-line interface.

## Installation

NLM is already installed in this environment. The Python bindings are available via the `pynlm` module.

## Quick Usage Examples

### 1. Minimal Python Example

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
```

### 2. Enhanced Python API

```python
from pynlm.enhanced import ConfigPresets, ExperimentManager

# Use a preset configuration
config = ConfigPresets.minimal()

# Run agent experiment with world
manager = ExperimentManager()
results = manager.run_agent_experiment(
    config,
    world_config={'width': 20, 'height': 20, 'visionWidth': 8, 'visionHeight': 8},
    num_steps=500,
    enable_learning=True,
    enable_development=True
)

print(f"Experiment complete! Total reward: {results['total_reward']:.1f}")
```

### 3. Command-Line Interface

```bash
# Run a simple simulation
./nlm.py run --neurons 1000 --steps 500

# Run with preset configuration
./nlm.py run --preset full --steps 1000

# Run agent experiment
./nlm.py experiment agent --neurons 2000 --world-width 30 --world-height 30 --enable-learning

# Benchmark brain creation
./nlm.py benchmark creation --count 100
```

## Advanced Usage

### Batch Operations

```python
from pynlm.enhanced import BatchSimulator, ConfigPresets

# Run multiple simulations with different configurations
configs = [
    ConfigPresets.minimal(),
    ConfigPresets.full(),
]

for config in configs:
    simulator = BatchSimulator(config, num_simulations=10)
    results = simulator.run(steps_per_simulation=500)
    stats = simulator.get_statistics()
    print(f"Average spikes: {stats['average_total_spikes']:.1f}")
```

### Serialization

```python
from pynlm.enhanced import SerializationUtils

# Save brain state
brain.save("my_brain.bin")

# Load brain state
brain2 = SerializationUtils.load_brain_from_file("my_brain.bin", config)
```

### Benchmarking

```python
from pynlm.enhanced import BenchmarkSuite
import pynlm

config = pynlm.createDefaultConfig()

# Benchmark brain creation
creation_results = BenchmarkSuite.benchmark_creation(100)
print(f"Created 100 brains in {creation_results['total_creation_time']:.2f} seconds")

# Benchmark simulation
simulation_results = BenchmarkSuite.benchmark_simulation(config, 1000, 10)
print(f"Average simulation time: {simulation_results['average_simulation_time']:.4f} seconds")
```

## Configuration Presets

### Minimal

- **Neurons**: 500
- **Connection probability**: 0.05
- **Purpose**: Basic testing and development

### Full

- **Neurons**: 2000
- **Connection probability**: 0.1
- **Plasticity**: All mechanisms enabled
- **Purpose**: Complex simulations and research

## Command-Line Reference

### `nlm run`

```bash
Usage: nlm.py run [OPTIONS]

Options:
  --neurons INTEGER     Number of neurons (default: 1000)
  --steps INTEGER       Number of simulation steps (default: 1000)
  --preset TEXT        Use a preset configuration (minimal, full)
  --output TEXT        Output file for results
```

### `nlm benchmark`

```bash
Usage: nlm.py benchmark <CREATION | SIMULATION> [OPTIONS]

CREATION:
  --count INTEGER      Number of brains to create (default: 100)

SIMULATION:
  --steps INTEGER      Number of steps per simulation (default: 1000)
  --runs INTEGER       Number of simulation runs (default: 10)
```

### `nlm brain`

```bash
Usage: nlm.py brain <SAVE | LOAD> [OPTIONS]

SAVE:
  --neurons INTEGER    Number of neurons (default: 1000)
  FILEPATH             Output file path

LOAD:
  FILEPATH             Input file path
```

### `nlm experiment`

```bash
Usage: nlm.py experiment <AGENT> [OPTIONS]

Options:
  --neurons INTEGER               Number of neurons (default: 1000)
  --world-width FLOAT             World width (default: 20.0)
  --world-height FLOAT            World height (default: 20.0)
  --steps INTEGER                 Number of steps (default: 1000)
  --enable-learning               Enable reward modulation
  --enable-development            Enable development system
  --enable-curiosity              Enable curiosity system
```

## Performance Tips

### For Large-Scale Simulations

1. **Use minimal configurations** for testing
2. **Enable batch operations** for multiple runs
3. **Use checkpointing** to save intermediate results
4. **Monitor performance** with the benchmark suite

### For Research Experiments

1. **Use full configurations** for complex behaviors
2. **Enable all plasticity mechanisms** (STDP, Hebbian, structural)
3. **Use agent experiments** with world interaction
4. **Compare different configurations** with the experiment manager

### For Production Use

1. **Use command-line tools** for automation
2. **Leverage serialization** for state persistence
3. **Monitor system resources** with benchmarks
4. **Use presets** for consistency across runs

## Troubleshooting

### Common Issues

1. **"ModuleNotFoundError: No module named 'pynlm'"**
   - Ensure the Python bindings are installed
   - Try: `pip install -e .` from the project root

2. **"ImportError: cannot import name 'enhanced' from 'pynlm'"**
   - The enhanced module may need to be built
   - Check if the module is properly packaged

3. **Command not found: nlm**
   - Ensure the CLI script is executable
   - Try: `python nlm.py run --help`

4. **Simulation takes too long**
   - Reduce the number of neurons or steps
   - Use the `--preset minimal` option
   - Run benchmarks to optimize parameters

### Getting Help

- **Quick help**: `python nlm.py help`
- **Command help**: `python nlm.py <command> --help`
- **List presets**: `python nlm.py list presets`
- **Quick test**: `python nlm.py run --steps 10`

## Example Scripts

### Simple Agent Example

```python
#!/usr/bin/env python3
"""
Simple agent example for NLM
"""

import pynlm
from pynlm.enhanced import ConfigPresets

def main():
    # Create configuration
    config = ConfigPresets.full()
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    print("Running simple agent experiment...")
    for step in range(500):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(0.0, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}: Reward={world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0:.2f}")
    
    print("Experiment complete!")
    print(f"Final reward: {world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0:.2f}")

if __name__ == "__main__":
    main()
```

### Configuration Comparison Example

```python
#!/usr/bin/env python3
"""
Configuration comparison example for NLM
"""

import pynlm
from pynlm.enhanced import ExperimentManager, ConfigPresets

def main():
    # Create two different configurations
    config1 = ConfigPresets.minimal()
    config2 = ConfigPresets.full()
    
    world_config = {
        'width': 20,
        'height': 20,
        'visionWidth': 8,
        'visionHeight': 8,
    }
    
    # Run comparison experiment
    manager = ExperimentManager()
    results = manager.run_comparison_experiment(
        config1, config2, world_config,
        steps=200,
        num_trials=3
    )
    
    # Print results
    print("\nConfiguration Comparison Results:")
    print("=" * 50)
    
    print(f"Config 1 (Minimal):")
    for key, value in results['config1_average'].items():
        print(f"  {key}: {value:.3f}")
    
    print(f"Config 2 (Full):")
    for key, value in results['config2_average'].items():
        print(f"  {key}: {value:.3f}")
    
    print("\nImprovements:")
    for key, value in results['improvements'].items():
        print(f"  {key}: {value:+.1f}%")

if __name__ == "__main__":
    main()
```

## Conclusion

NLM provides a powerful framework for brain simulation with comprehensive Python API support. The enhanced features include:

- **Configuration presets** for easy setup
- **Batch operations** for efficiency
- **Experiment management** for complex research
- **Benchmarking** for performance optimization
- **Serialization** for state persistence
- **Command-line interface** for automation

These improvements make NLM more accessible to researchers, educators, and developers while maintaining the scientific rigor and complexity needed for advanced neural network research.
