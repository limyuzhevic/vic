# NLM Advanced Usage Guide

This document covers advanced features and usage patterns for the NLM (Neural Learning Machine) neural simulation framework.

## Table of Contents

1. [Enhanced Configuration Management](#enhanced-configuration-management)
2. [Advanced Python API Features](#advanced-python-api-features)
3. [Performance Benchmarking](#performance-benchmarking)
4. [Integration Testing Framework](#integration-testing-framework)
5. [Error Handling and Recovery](#error-handling-and-recovery)
6. [Command Line Interface Advanced Usage](#command-line-interface-advanced-usage)
7. [Real-world Multi-Agent Environment](#real-world-multi-agent-environment)
8. [Schema Validation and Type Safety](#schema-validation-and-type-safety)

## Enhanced Configuration Management

### JSON and YAML Configuration Support

NLM now supports JSON and YAML configuration files, which provide more powerful features than the simple key=value format:

```python
import pynlm
import json

# Create configuration from JSON
json_config = {
    "brain": {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.15,
        "synapse_density": 0.05
    },
    "plasticity": {
        "stdp": {
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "learning_rate": 0.01,
            "baseline": 0.5
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0,
            "baseline": 0.1
        }
    }
}

# Create config from JSON dict
config = pynlm.createDefaultConfig()
config.from_dict(json_config)

# Save config to JSON and load from file
config.saveToFile("my_experiment.json")
config = pynlm.createDefaultConfig()
config.loadFromFile("my_experiment.json")
```

### Configuration Schema Validation

Validate configuration before use with schema:

```python
import pynlm

# Create configuration with schema
config = pynlm.createDefaultConfig()

# Set schema for validation
config.setSchema("brain.neuron_count", "int", 1000, "Number of neurons")
config.setSchema("brain.connection_probability", "float", 0.1, "Connection probability")
config.setSchema("brain.synapse_density", "float", 0.05, "Synapse density")
config.setSchema("environment.width", "int", 10, "World width")
config.setSchema("environment.height", "int", 10, "World height")

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    errors = config.getValidationErrors()
    print("Validation errors:", errors)

# Test individual parameter validation
if config.has("brain.neuron_count"):
    neuron_count = config.getInt("brain.neuron_count", 100)
    print(f"Neuron count: {neuron_count}")
```

## Advanced Python API Features

### Batch Operations

Perform multiple configuration operations efficiently:

```python
import pynlm

# Create brain configuration
config = pynlm.createDefaultConfig()

# Batch set multiple values using dictionary
batch_config = {
    "brain.neuron_count": 3000,
    "brain.region_count": 5,
    "plasticity.stdp.learning_rate": 0.002,
    "plasticity.hebbian.learning_rate": 0.015,
    "neuromod.dopamine.scale": 1.2,
    "environment.width": 50,
    "environment.height": 50
}

config.batchSet(batch_config)

# Batch operations with direct C++ values
from pynlm import Config
config = Config()
config.batchSet([
    ("brain.neuron_count", 2000),
    ("plasticity.stdp.learning_rate", 0.001),
    ("neuromod.curiosity.enable", True),
    ("environment.maxEnergy", 100.0)
])
```

### Filter and Transform Operations

Work with subsets of configuration:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("environment.width", 20)

# Filter to only essential parameters
essential_params = ["brain.neuron_count", "brain.synapse_density", "environment.width"]
brain_config = config.filter(essential_params)

# Transform all float values to scientific notation
def scientific_notation(value):
    if isinstance(value, float):
        return f"{value:.2e}"
    return value

scientific_config = config.transform(scientific_notation)
```

### Advanced Getters with Type Safety

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.15)
config.set("brain.synapse_density", 0.05)
config.set("environment.width", 30)
config.set("environment.height", 30)

# Type-safe getters with defaults
neuron_count = config.getInt("brain.neuron_count", 100)
connection_prob = config.getFloat("brain.connection_probability", 0.1)
width = config.getInt("environment.width", 10)
height = config.getInt("environment.height", 10)

# Vector getter
vector_value = config.getVector("some_vector_param", [1.0, 2.0, 3.0])

# Boolean getter
enabled = config.getBool("feature.enabled", False)
```

## Performance Benchmarking

Measure and compare performance across different configurations:

```python
import pynlm
import time
import statistics

class PerformanceBenchmark:
    def __init__(self):
        self.results = []
    
    def run_benchmark(self, config, steps=1000):
        """Run a performance benchmark with given configuration"""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Warm-up run
        for _ in range(100):
            brain.step(_)
        
        # Benchmark run
        start_time = time.perf_counter()
        for step in range(steps):
            brain.step(step)
        end_time = time.perf_counter()
        
        duration = end_time - start_time
        steps_per_second = steps / duration
        total_spikes = brain.getTotalSpikeCount()
        avg_firing_rate = brain.getAverageFiringRate()
        
        result = {
            "steps_per_second": steps_per_second,
            "duration_seconds": duration,
            "total_spikes": total_spikes,
            "avg_firing_rate": avg_firing_rate,
            "config_summary": {
                "neuron_count": config.getInt("brain.neuron_count", 1000),
                "region_count": config.getInt("brain.region_count", 1)
            }
        }
        
        self.results.append(result)
        return result
    
    def compare_configurations(self, configs, steps=1000):
        """Compare multiple configurations"""
        print("Performance Comparison:")
        print("=" * 60)
        
        for i, config in enumerate(configs):
            result = self.run_benchmark(config, steps)
            print(f"Configuration {i + 1}:")
            print(f"  Performance: {result['steps_per_second']:.2f} steps/sec")
            print(f"  Total spikes: {result['total_spikes']}")
            print(f"  Avg firing rate: {result['avg_firing_rate']:.3f} Hz")
            print()
        
        # Statistical analysis
        print("Statistical Analysis:")
        steps_values = [r['steps_per_second'] for r in self.results]
        print(f"  Mean: {statistics.mean(steps_values):.2f} steps/sec")
        print(f"  Median: {statistics.median(steps_values):.2f} steps/sec")
        print(f"  Std Dev: {statistics.stdev(steps_values):.2f} steps/sec")

# Example usage
benchmark = PerformanceBenchmark()
configs = []
for neurons in [500, 1000, 2000]:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    configs.append(config)

benchmark.compare_configurations(configs, steps=500)
```

## Integration Testing Framework

Test complete brain-world-agent integration:

```python
import pynlm

class IntegrationTest:
    def __init__(self):
        self.test_results = []
    
    def test_complete_simulation(self, steps=1000):
        """Test complete simulation with all systems"""
        # Create components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        # Verify results
        result = {
            "steps_completed": steps,
            "final_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "development_stage": agent.getDevelopmentalStage(),
            "success": brain.getTotalNeuronCount() > 0
        }
        
        self.test_results.append(result)
        return result

# Run integration tests
integration_test = IntegrationTest()
if integration_test.run_all_tests():
    print("\n🎉 All integration tests passed!")
else:
    print("\n❌ Some integration tests failed!")
```

## Error Handling and Recovery

Handle errors gracefully in production code:

```python
import pynlm
import traceback
import os

def safe_simulation_run(config_path, steps=1000):
    """Run simulation with comprehensive error handling"""
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        if not config.loadFromFile(config_path):
            raise ValueError(f"Failed to load configuration from {config_path}")
        
        # Validate configuration
        if not config.validate():
            errors = config.getValidationErrors()
            raise ValueError(f"Configuration validation failed: {errors}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        # Initialize brain
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        print(f"Simulation started with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation with progress tracking
        for step in range(steps):
            try:
                brain.step(step)
                
                # Save checkpoint periodically
                if step % 100 == 0:
                    checkpoint_file = f"checkpoint_step_{step}.bin"
                    if brain.save(checkpoint_file):
                        print(f"Checkpoint saved: {checkpoint_file}")
                
            except Exception as step_error:
                print(f"Step {step} failed: {step_error}")
                # Try recovery from checkpoint
                checkpoint_file = f"checkpoint_step_{max(0, step-100)}.bin"
                if os.path.exists(checkpoint_file):
                    print(f"Attempting recovery from checkpoint...")
                    brain.load(checkpoint_file)
                    brain.initialize()
                else:
                    raise
        
        print(f"Simulation completed successfully! Total spikes: {brain.getTotalSpikeCount()}")
        return brain
        
    except Exception as e:
        print(f"Simulation failed: {str(e)}")
        traceback.print_exc()
        return None

# Usage example
brain = safe_simulation_run("experiment_config.json", steps=1000)
```

## Command Line Interface Advanced Usage

Use NLM from command line with advanced options:

```bash
# Basic usage with enhanced CLI options
nlm --config experiment.json --verbose --steps 5000 --neurons 2000

# Performance profiling
nlm --profile --output-prefix perf_results --steps 2000 --seed 42

# GPU acceleration (if supported)
nlm --enable-gpu --gpu-device 0 --steps 3000

# Custom experiment
nlm --experiment learning --custom brain.synapse_density=0.15 --custom environment.maxEnergy=150

# Help and information
nlm --help
nlm --version
```

## Real-world Multi-Agent Environment

Complete example combining multiple advanced features:

```python
import pynlm
import json
from pathlib import Path
import time
from datetime import datetime

class MultiAgentEnvironment:
    def __init__(self, config_path="config.json"):
        self.config_path = config_path
        self.experiments = []
        self.results = []
    
    def setup_environment(self):
        """Setup complete environment with all systems"""
        print("Setting up NLM environment...")
        
        # Load advanced configuration
        config = pynlm.createDefaultConfig()
        if Path(self.config_path).exists():
            config.loadFromFile(self.config_path)
        else:
            # Create comprehensive configuration
            self.create_default_config(config)
        
        # Validate configuration
        if not config.validate():
            print(f"Configuration validation errors: {config.getValidationErrors()}")
            return False
        
        # Create brain with high performance settings
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=config.getInt("environment.width", 50),
            height=config.getInt("environment.height", 50),
            visionWidth=config.getInt("environment.visionWidth", 16),
            visionHeight=config.getInt("environment.visionHeight", 16)
        )
        self.world.reset()
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        print(f"Environment setup complete: {self.brain.getTotalNeuronCount()} neurons")
        return True
    
    def create_default_config(self, config):
        """Create comprehensive default configuration"""
        config.batchSet({
            "brain.neuron_count": 2000,
            "brain.region_count": 3,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.hebbian.learning_rate": 0.01,
            "neuromod.curiosity.threshold": 0.3,
            "neuromod.novelty.threshold": 0.5,
            "environment.width": 50,
            "environment.height": 50,
            "environment.visionWidth": 16,
            "environment.visionHeight": 16,
            "environment.maxEnergy": 100.0,
            "simulation.max_steps": 5000
        })
    
    def run_experiment(self, episodes=10, steps_per_episode=1000):
        """Run complete experiment with multiple episodes"""
        print(f"Starting experiment: {episodes} episodes, {steps_per_episode} steps each")
        
        # Save initial configuration
        initial_config = self.brain.getConfig()
        initial_config.saveToFile(f"experiment_{episodes}ep_config.json")
        
        # Run episodes
        experiment_results = []
        for episode_id in range(episodes):
            episode_result = self.run_episode(steps_per_episode, episode_id)
            experiment_results.append(episode_result)
            
            # Save checkpoint after each episode
            checkpoint_file = f"experiment_checkpoint_{episode_id}.bin"
            if self.brain.save(checkpoint_file):
                print(f"Checkpoint saved: {checkpoint_file}")
        
        # Save final results
        self.save_experiment_results()
        
        # Calculate experiment statistics
        stats = self.calculate_experiment_statistics(experiment_results)
        
        return experiment_results, stats

# Usage example
if __name__ == "__main__":
    env = MultiAgentEnvironment("experiment_config.json")
    
    if env.setup_environment():
        results, stats = env.run_experiment(episodes=5, steps_per_episode=500)
        print(f"\nExperiment complete: {len(results)} episodes, {stats['total_steps']} total steps")
    else:
        print("Failed to setup environment")
```

## Schema Validation and Type Safety

### Complete Schema Validation Example

```python
import pynlm

# Define configuration schema
class ConfigSchema:
    def __init__(self):
        self.schemas = {
            "brain.neuron_count": {
                "type": int,
                "min": 100,
                "max": 10000,
                "required": True,
                "description": "Number of neurons in the brain"
            },
            "brain.connection_probability": {
                "type": float,
                "min": 0.0,
                "max": 1.0,
                "required": False,
                "description": "Probability of synaptic connections"
            }
        }
    
    def validate_config(self, config):
        errors = []
        for key, schema in self.schemas.items():
            if schema.get("required", False) and not config.has(key):
                errors.append(f"Missing required parameter: {key}")
        return errors

# Usage
schema = ConfigSchema()
print("Schema validation examples completed")
```

## Installation and Setup

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+
- pybind11 2.11.0+

### Building from Source

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Installing Python Bindings

```bash
pip install nlohmann-json pyyaml rapidjson
pip install .
```

## Advanced Configuration Examples

```python
# High-performance configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("brain.region_count", 5)
config.set("neuromod.curiosity.threshold", 0.1)
config.set("environment.maxEnergy", 1000.0)

# Real-time simulation configuration
config.set("simulation.timestep", 0.0001)
config.set("brain.region_count", 10)
config.set("brain.synapse_density", 0.05)
```

## Troubleshooting

**Common Issues:**
- ImportError: Install with `pip install --force-reinstall .`
- Build errors: Ensure CMake 3.16+ and C++20 compiler
- Runtime errors: Check initialization order

**Performance Optimization:**
- Use `--enable-gpu` for large networks
- Use `--checkpoint-interval` for long simulations
- Configure appropriately sized networks for your use case

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## License

MIT License - See LICENSE file for details.

*This advanced usage guide provides comprehensive examples for leveraging the enhanced NLM features, including JSON/YAML configuration, performance benchmarking, integration testing, and real-world multi-agent environments.*