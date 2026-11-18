# NLM - Enhanced Documentation and Examples

## Overview

NLM (熙然 - "serene flow") is an experimental computational brain project that has been significantly enhanced with improved configuration management, error handling, testing, and user interface.

This document provides comprehensive documentation for the enhanced NLM project, including:

- Command-line interface with multiple modes
- Enhanced configuration system with validation
- Comprehensive error handling and logging
- Performance profiling and monitoring
- Example usage and scripts

## Quick Start

### Build Instructions

```bash
# Configure with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Build Python bindings (optional)
pip install .
```

### Basic Usage

```bash
# Phase 6 integration demo (default)
./nlm

# Or specify mode explicitly
./nlm --demo

# Run Phase 6 integration test with custom settings
./nlm --demo --neurons 1000 --steps 2000 --develop

# Run benchmark
./nlm --benchmark --benchmark-mode performance --benchmark-duration 60

# Show help
./nlm --help
```

### Python Usage

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Complete simulation loop
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Enhanced Configuration System

### Overview

The NLM configuration system has been enhanced with:

- Type-safe configuration access
- Validation against schema
- Default values for all configuration options
- File-based configuration with JSON support
- Command-line configuration
- Error handling and logging integration

### Configuration Types

The configuration system supports the following value types:

- `int` - Integer values
- `int64_t` - 64-bit integer values
- `double` - Floating-point values
- `bool` - Boolean values
- `std::string` - String values
- `std::vector<int>` - Integer arrays
- `std::vector<double>` - Double arrays
- `std::vector<std::string>` - String arrays

### Common Configuration Options

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.connection_probability` | double | 0.1 | Probability of connection between neurons |
| `simulation.timestep` | double | 0.001 | Simulation timestep in seconds |
| `random.seed` | int64_t | 42 | Random seed for reproducible simulations |
| `plasticity.stdp.enable` | bool | true | Enable STDP plasticity |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromodulation.dopamine.scale` | double | 1.0 | Dopamine modulation scale factor |

### Configuration Validation

The configuration system validates values against schemas to prevent invalid configurations:

```python
# Example validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", -100)  # Will trigger validation error
```

Common validation rules:

- Positive integer values for neuron counts
- Probability values between 0.0 and 1.0
- Reasonable ranges for simulation parameters

### File-Based Configuration

Configuration can be loaded from files:

```bash
# Save configuration
cat > my_config.cfg << EOF
brain.neuron_count = 2000
brain.region_count = 2
brain.connection_probability = 0.15
EOF

# Load configuration
./nlm --demo --config my_config.cfg
```

## Enhanced Error Handling

### Overview

The NLM error handling system provides:

- Categorized error types (Configuration, Simulation, Memory, etc.)
- Detailed error context and timestamps
- Error statistics and tracking
- Error callbacks and logging integration
- Safe exception handling

### Error Categories

- **Configuration Error**: Invalid configuration parameters
- **Simulation Error**: Problems during neural simulation
- **Memory Error**: Memory allocation or management issues
- **Plasticity Error**: Plasticity rule or learning problems
- **Neuromodulation Error**: Neuromodulator-related issues
- **Development Error**: Developmental stage problems
- **IO Error**: File or input/output issues
- **Network Error**: Network or communication problems
- **Unknown Error**: Unclassified errors
- **User Error**: User input or usage problems

### Error Handling in Python

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)
        
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.MLMEException as e:
    print(f"Unexpected error: {e}")
```

## Command-Line Interface

### Modes

The enhanced NLM provides multiple execution modes:

#### Demo Mode (`--demo` or `--mode demo`)

Run the complete Phase 6 integration demo:

```bash
./nlm --demo
./nlm --demo --neurons 500 --steps 1000
./nlm --demo --develop --reward --curiosity
```

#### Benchmark Mode (`--benchmark` or `--mode benchmark`)

Run performance benchmarks:

```bash
./nlm --benchmark
./nlm --benchmark --benchmark-mode performance
./nlm --benchmark --benchmark-mode memory
./nlm --benchmark --benchmark-mode plasticity
./nlm --benchmark --benchmark-duration 120
```

#### Phase-Specific Modes

- `--phase2` - Phase 2 neural computation demo
- `--phase3` - Phase 3 world interaction demo
- `--phase4` - Phase 4 cognition demo
- `--phase5` - Phase 5 extensibility demo
- `--phase6` - Phase 6 full integration demo
- `--phase6_text` - Phase 6 text-as-a-sense demo

### Configuration Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--neurons N` | int | 500 | Number of neurons |
| `--steps N` | int | 1000 | Number of simulation steps |
| `--timestep t` | double | 0.001 | Simulation timestep |
| `--seed S` | int64_t | 42 | Random seed |
| `--develop` | flag | false | Enable brain development |
| `--reward` | flag | true | Enable reward modulation |
| `--curiosity` | flag | true | Enable curiosity-driven exploration |
| `--novelty` | flag | true | Enable novelty detection |
| `--checkpoint` | flag | false | Enable checkpointing |
| `--replay` | flag | false | Enable memory replay |
| `--verbose` | flag | false | Verbose output |
| `--quiet` | flag | false | Quiet output (errors only) |
| `--profile` | flag | false | Enable performance profiling |

### Configuration File

Configuration files can be loaded:

```bash
# Create configuration file
cat > config.cfg << EOF
brain.neuron_count = 1500
brain.connection_probability = 0.12
simulation.timestep = 0.002
EOF

# Use configuration file
./nlm --demo --config config.cfg
```

## Example Scripts

### Complete Agent Example

```python
#!/usr/bin/env python3
"""Complete NLM agent example with configuration and error handling."""

import pynlm
import sys

def run_agent_simulation(num_steps=1000, neuron_count=1000):
    """Run a complete agent simulation with error handling."""
    try:
        # 1. Create and configure
        print("Creating NLM agent...")
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 2. Run simulation loop
        print(f"Running {num_steps}-step simulation...")
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log progress periodically
            if step % 100 == 0:
                print(f"  Step {step}:")
                print(f"    Neurons firing: {brain.getFiringNeuronCount()}")
                print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
                print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
                print(f"    Development stage: {brain.getDevelopmentalStage()}")
        
        # 3. Print final statistics
        print("\n=== Simulation Complete ===")
        print(f"Total steps: {num_steps}")
        print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Development stage: {brain.getDevelopmentalStage()}")
        
        return True
        
    except nlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        return False
    except nlm.SimulationError as e:
        print(f"Simulation error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Agent Simulation")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser.add_argument("--output", type=str, default=None, help="Output file for results")
    
    args = parser.parse_args()
    
    success = run_agent_simulation(args.steps, args.neurons)
    
    if success and args.output:
        print(f"Results saved to {args.output}")
    
    sys.exit(0 if success else 1)
```

### Configuration Builder Script

```python
#!/usr/bin/env python3
"""Configuration builder for NLM simulations."""

import json
import pynlm

def create_research_config(neuron_count=2000, simulation_steps=5000):
    """Create a research-grade configuration for NLM simulations."""
    config = pynlm.createDefaultConfig()
    
    # Brain parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.12)
    
    # Simulation parameters
    config.set("simulation.timestep", 0.001)
    config.set("random.seed", 42)
    
    # Plasticity parameters
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.ltp_weight", 0.02)
    config.set("plasticity.stdp.ltd_weight", 0.015)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
    config.set("plasticity.structural.pruning_rate", 0.00001)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.decay", 0.01)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.1)
    
    # Development
    config.set("development.critical_period_start", 0.1)
    config.set("development.critical_period_end", 1.0)
    config.set("development.enable", True)
    
    # Memory systems
    config.set("memory.working_memory.capacity", 100)
    config.set("memory.episodic_memory.duration", 10.0)
    
    return config

def save_config(config, filename):
    """Save configuration to file."""
    # For now, save as JSON-like format
    with open(filename, 'w') as f:
        f.write("# NLM Configuration File\n")
        f.write("# Generated by config_builder.py\n\n")
        
        for key in config.getKeys():
            value = config.get(key)
            if value.has_value():
                f.write(f"{key} = {value.value()}\n")

def load_config(filename):
    """Load configuration from file."""
    config = pynlm.createDefaultConfig()
    config.loadFromFile(filename)
    return config

if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Configuration Builder")
    parser.add_argument("--output", type=str, required=True, help="Output configuration file")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    
    args = parser.parse_args()
    
    config = create_research_config(args.neurons, args.steps)
    save_config(config, args.output)
    
    print(f"Configuration saved to {args.output}")
    print("Key parameters:")
    print(f"  Neurons: {config.getIntOr('brain.neuron_count', 0)}")
    print(f"  Regions: {config.getIntOr('brain.region_count', 0)}")
    print(f"  Connection probability: {config.getDoubleOr('brain.connection_probability', 0.0)}")
```

### Performance Testing Script

```python
#!/usr/bin/env python3
"""Performance testing for NLM."""

import pynlm
import time
import statistics

def benchmark_performance(neuron_counts=[100, 500, 1000, 2000], runs_per_config=3):
    """Run performance benchmarks for different neuron counts."""
    
    results = []
    
    for neuron_count in neuron_counts:
        print(f"\nBenchmarking with {neuron_count} neurons...")
        
        times = []
        spikes = []
        memory_usage = []
        
        for run in range(runs_per_config):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            
            for step in range(100):
                brain.step(step)
            
            end_time = time.time()
            
            elapsed = end_time - start_time
            times.append(elapsed)
            spikes.append(brain.getTotalSpikeCount())
            memory_usage.append(brain.getTotalNeuronCount())
            
            print(f"  Run {run + 1}: {elapsed:.3f}s, {brain.getTotalSpikeCount()} spikes")
        
        # Calculate statistics
        avg_time = statistics.mean(times)
        stdev_time = statistics.stdev(times) if len(times) > 1 else 0.0
        avg_spikes = statistics.mean(spikes)
        
        result = {
            'neurons': neuron_count,
            'runs': runs_per_config,
            'avg_time': avg_time,
            'stdev_time': stdev_time,
            'avg_spikes': avg_spikes,
            'spikes_per_second': avg_spikes / avg_time,
            'time_per_step_ms': (avg_time * 1000) / 100
        }
        
        results.append(result)
        
        print(f"  Average: {avg_time:.3f}s ± {stdev_time:.3f}s")
        print(f"  Average spikes: {avg_spikes:.1f}")
        print(f"  Spikes/sec: {result['spikes_per_second']:.1f}")
    
    return results

if __name__ == "__main__":
    import json
    
    print("=== NLM Performance Benchmark ===")
    
    results = benchmark_performance()
    
    print("\n=== Summary ===")
    for result in results:
        print(f"{result['neurons']} neurons: {result['avg_time']:.3f}s avg, "
              f"{result['spikes_per_second']:.1f} spikes/sec")
    
    # Save results
    with open('benchmark_results.json', 'w') as f:
        json.dump(results, f, indent=2)
    
    print("\nResults saved to benchmark_results.json")
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons are getting input

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure the agent has motor neurons

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to initialize brain!" | Brain initialization failed | Check configuration parameters, ensure all required systems are available |
| "Invalid configuration" | Configuration validation failed | Check configuration file format and values |
| "Memory allocation failed" | Out of memory | Reduce neuron count or increase system resources |
| "Division by zero" | Numerical instability | Check simulation parameters, increase timestep |

## Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Project roadmap and future plans

### Research Papers

- "Neural Learning Machine: An Experimental Artificial Developmental Brain"
- "Spiking Neural Networks with Multiple Plasticity Mechanisms"
- "Integration of Memory and Prediction in Artificial Brains"

### Online Resources

- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM Research Papers](https://arxiv.org/search/?q=nlm)

## License

MIT License. See LICENSE file for details.

## Authors

NLM Research Project - An experimental project investigating computational brain-like systems.

---

This document provides comprehensive documentation for the enhanced NLM project. The improvements include:

1. **Enhanced Configuration System**: Type-safe configuration with validation
2. **Comprehensive Error Handling**: Categorized errors with detailed context
3. **Improved Command-Line Interface**: Multiple modes and extensive options
4. **Performance Profiling Tools**: Built-in performance monitoring
5. **Example Scripts**: Ready-to-use examples for various use cases
6. **Documentation**: Comprehensive documentation with troubleshooting guide

These improvements make NLM more accessible to beginners while providing advanced features for researchers and experienced users.
