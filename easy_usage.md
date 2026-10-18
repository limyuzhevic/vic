# Enhanced NLM Python Bindings Documentation

This document provides comprehensive examples and documentation for the enhanced NLM (熙然) Python bindings.

## Overview

The enhanced Python bindings provide:
- Complete error handling with specific exception types
- All memory, neuromodulation, and cognition systems
- High-level training APIs (train(), validate(), test())
- Model persistence (save/load)
- Real-time monitoring and statistics
- Context managers for automatic resource cleanup
- Python properties and natural method signatures
- Comprehensive utility functions

## Quick Start

### Basic Usage (Unchanged)
```python
import pynlm

# Create brain with default configuration
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Enhanced Usage (New Capabilities)
```python
import pynlm

# Create brain with error handling
try:
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Training and Learning

### Training API
```python
import pynlm

# Create brain and world
brain = pynlm.create_brain()
brain.initialize()

world = pynlm.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.set_agent_start(10.0, 10.0)

# Create training session
training = pynlm.TrainingSession(brain, world)

# Train with specific parameters
print("Starting training...")
training.train(500, enable_reward_modulation=True, enable_development=True, enable_curiosity=True)

# Get training statistics
print(f"Average reward: {training.get_average_reward():.4f}")
print(f"Steps completed: {training.get_steps_completed()}")
print(f"Average firing rate: {training.get_avg_firing_rate():.4f}")

# Validate the trained agent
print("Validating agent...")
training.validate(200)

# Test agent stability
print("Testing agent stability...")
training.test(100)

# Save trained model
training.save_model("trained_brain.bin")
print("Model saved to trained_brain.bin")
```

### Quick Experiment Templates
```python
import pynlm

# 1. Reinforcement Learning Experiment
print("=== Reinforcement Learning Experiment ===")
experiment = pynlm.run_reinforcement_learning_experiment(1000)
experiment.train(500)
print(f"Final reward: {experiment.get_average_reward():.4f}")

# 2. Memory Integration Experiment  
print("\n=== Memory Integration Experiment ===")
experiment = pynlm.run_memory_integration_experiment(2000)
experiment.train(1000)
print(f"Final reward: {experiment.get_average_reward():.4f}")

# 3. Curiosity-Driven Exploration
print("\n=== Curiosity-Driven Exploration ===")
experiment = pynlm.run_curiosity_experiment(1500)
experiment.train(750)
print(f"Final reward: {experiment.get_average_reward():.4f}")

# 4. Development Experiment
print("\n=== Development Experiment ===")
experiment = pynlm.run_development_experiment(3000)
experiment.train(1500)
print(f"Final reward: {experiment.get_average_reward():.4f}")
```

## Model Management

### Model Persistence
```python
import pynlm

# Create and train a model
brain = pynlm.create_brain(pynlm.create_research_config(1000))
brain.initialize()

# Train for a while
for step in range(500):
    brain.step(step)

# Save model with metadata
pynlm.save_brain_with_metadata("my_brain.bin", "brain_metadata.txt")

# Load model
loaded_brain = pynlm.load_brain_with_metadata("my_brain.bin", "brain_metadata.txt")
if loaded_brain:
    print("Model loaded successfully")
    print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")

# Compare models
difference = pynlm.compare_brains("brain1.bin", "brain2.bin")
print(f"Brain difference: {difference:.4f} (0.0 = identical, 1.0 = very different)")
```

### Configuration Management
```python
import pynlm

# Create different configuration presets
research_config = pynlm.create_research_config(1000)
production_config = pynlm.create_production_config(5000)
development_config = pynlm.create_development_config(2000)

# Configure brains
brain1 = pynlm.create_brain(research_config)
brain2 = pynlm.create_brain(production_config)
brain3 = pynlm.create_brain(development_config)

# Validate configurations
print(f"Research config valid: {pynlm.validate_brain_config(research_config)}")
print(f"Production config valid: {pynlm.validate_brain_config(production_config)}")
print(f"Development config valid: {pynlm.validate_brain_config(development_config)}")
```

## Advanced Features

### Statistics and Monitoring
```python
import pynlm

# Create a brain
brain = pynlm.create_brain()
brain.initialize()

# Collect statistics
stats = pynlm.collect_stats(brain)

# Print formatted statistics
pynlm.print_stats(stats)

# Or get as string
stats_str = pynlm.format_stats(stats)
print(stats_str)

# Access specific statistics
print(f"Neuron count: {stats['neuron_count']}")
print(f"Firing rate: {stats['average_firing_rate']:.4f}")
print(f"E/I ratio: {stats['excitation_inhibition_ratio']:.4f}")
print(f"Working memory traces: {stats['working_memory_traces']}")
print(f"Episodic memory episodes: {stats['episodic_memory_episodes']}")
```

### Random Seed Management
```python
import pynlm

# Set seed for reproducibility
pynlm.set_random_seed(42)

# Generate new seed
seed = pynlm.generate_seed()
print(f"Generated seed: {seed}")

# Get current seed
pynlm.set_random_seed(seed)
print(f"Current seed: {pynlm.get_random_seed()}")
```

### Utility Functions
```python
import pynlm

# Create simple agent world
brain, world, agent = pynlm.create_simple_agent_world(neuron_count=500, world_width=20, world_height=20)

# Run simple simulation
pynlm.run_simple_simulation(brain, world, steps=100)

# Get system status
status = pynlm.get_system_status(brain)
print(status)

# Create training configuration
train_config = pynlm.create_training_config(episodes=10, steps_per_episode=500,
                                           reward_modulation=True, development=True, curiosity=True)

# Create performance config and validate
perf_config = pynlm.create_production_config(3000)
print(f"Production config valid: {pynlm.validate_brain_config(perf_config)}")

# Get performance statistics
perf_stats = pynlm.get_performance_stats(brain, steps=100)
print(perf_stats)
```

## Error Handling

The enhanced bindings provide comprehensive error handling with specific exception types:

```python
import pynlm

try:
    # Create brain
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Do something that might fail
    brain.step(10)
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Handle configuration problems
    
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
    # Handle memory issues
    
except pynlm.TrainingError as e:
    print(f"Training error: {e}")
    # Handle training problems
    
except ValueError as e:
    print(f"Invalid value: {e}")
    # Handle parameter validation
    
except RuntimeError as e:
    print(f"Runtime error: {e}")
    # Handle runtime problems
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle any other errors
```

## Context Managers

Use context managers for automatic resource cleanup:

```python
import pynlm

# Use context manager for automatic cleanup
with pynlm.create_brain() as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    print("Brain automatically cleaned up")

# Check memory system cleanup
with pynlm.create_brain() as brain:
    brain.initialize()
    working_memory = brain.get_working_memory()
    with pynlm.MemoryCleanupManager(working_memory):
        working_memory.store("test_pattern", 1.0)
        print("Memory stored and automatically cleared")
```

## Comparing NLM APIs

### Original API
```python
import pynlm

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
brain.step(10)
brain.getTotalNeuronCount()
```

### Enhanced API
```python
import pynlm

# Original usage still works
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
brain.step(10)
brain.getTotalNeuronCount()

# Enhanced API also available
brain2 = pynlm.create_brain()  # Direct creation with defaults
brain2.initialize()
brain2.step(10)
brain2.total_neuron_count  # Python property instead of getTotalNeuronCount()

# Properties are available for all major classes
stats = pynlm.collect_stats(brain2)
stats.neuron_count  # Property instead of getNeuronCount()
stats.average_firing_rate  # Property instead of getAverageFiringRate()
```

## Performance Monitoring

### Real-time Monitoring
```python
import pynlm
import time

# Create brain
brain = pynlm.create_brain(pynlm.create_production_config(5000))
brain.initialize()

# Monitor performance over time
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Get current statistics every 100 steps
    if step % 100 == 0:
        stats = pynlm.collect_stats(brain)
        print(f"Step {step}: Firing rate = {stats['average_firing_rate']:.4f}, "
              f"E/I ratio = {stats['excitation_inhibition_ratio']:.4f}")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### Performance Comparison
```python
import pynlm

# Create brains with different configurations
research_brain = pynlm.create_brain(pynlm.create_research_config(1000))
production_brain = pynlm.create_brain(pynlm.create_production_config(5000))
development_brain = pynlm.create_brain(pynlm.create_development_config(2000))

# Compare performance
for brain in [research_brain, production_brain, development_brain]:
    brain.initialize()
    
    stats = pynlm.collect_stats(brain)
    print(f"{brain.__class__.__name__}:")
    print(f"  Neurons: {stats['neuron_count']}")
    print(f"  Firing rate: {stats['average_firing_rate']:.4f}")
    print(f"  Processing time: {stats['wall_clock_time']:.2f}s")
    print()
```

## Migration Guide

### From Old to New API

If you're currently using the NLM Python bindings, you can upgrade to the enhanced version without changing your existing code:

```python
# Old usage (still works)
import pynlm

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# New capabilities available
brain2 = pynlm.create_brain()  # Same as old, just with error handling
brain2.initialize()

# High-level APIs for common workflows
training = pynlm.TrainingSession(brain2, world)
training.train(100)

# Better error handling
try:
    # Your existing code here
    pass
except Exception as e:
    # Now you have specific exception types
    if isinstance(e, pynlm.ConfigurationError):
        pass
    elif isinstance(e, pynlm.MemoryError):
        pass
```

### Key Improvements

1. **Better Error Handling**: Specific exception types for different error conditions
2. **Complete Class Coverage**: All memory, neuromodulation, and cognition systems included
3. **Training APIs**: High-level train/validate/test methods
4. **Model Persistence**: Save/load with metadata
5. **Python Properties**: Natural attribute access instead of get() methods
6. **Context Managers**: Automatic resource cleanup
7. **Utility Functions**: Common operations made easy
8. **Configuration Presets**: Research, production, and development configurations
9. **Statistics API**: Comprehensive statistics collection and formatting
10. **Backward Compatible**: Existing code continues to work unchanged

## Installation and Setup

### Requirements
```bash
pip install pynlm
```

### Building from Source
```bash
# Clone the repository
cd nlm

# Build the Python bindings
pip install -e .

# Or build with scikit-build
pip install scikit-build-core pybind11
pip install .
```

### Quick Test
```python
import pynlm

# Test basic functionality
print("Testing NLM Python bindings...")

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

for step in range(10):
    brain.step(step)

print(f"Test successful: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")
```

## Troubleshooting

### Common Issues

**Import Error**
```
ImportError: cannot import name 'pynlm'
```
**Solution**: Ensure the package is installed correctly:
```bash
pip install pynlm
```

**Runtime Error**
```
RuntimeError: Brain not initialized
```
**Solution**: Always call `initialize()` after creating a brain:
```python
brain = pynlm.create_brain()
brain.initialize()  # Missing in your code
brain.step(0)
```

**Configuration Error**
```
ConfigurationError: Invalid configuration parameters
```
**Solution**: Use configuration validation before use:
```python
if pynlm.validate_brain_config(config):
    brain = pynlm.create_brain(config)
    brain.initialize()
else:
    print("Invalid configuration")
```

**Memory Error**
```
MemoryError: Out of memory
```
**Solution**: Use context managers for automatic cleanup:
```python
with pynlm.create_brain() as brain:
    brain.initialize()
    # Brain automatically cleaned up
```

## Advanced Usage Examples

### Complex Experiment Setup
```python
import pynlm

def run_comprehensive_experiment(config_type="research", steps=2000):
    """Run a comprehensive experiment with different configurations."""
    
    # Select configuration
    if config_type == "research":
        config = pynlm.create_research_config(1000)
    elif config_type == "production":
        config = pynlm.create_production_config(5000)
    else:
        config = pynlm.create_development_config(2000)
    
    # Create brain
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.create_simple_world(config.getOr<int64_t>("neuron_count", 1000) // 20,
                                       config.getOr<int64_t>("neuron_count", 1000) // 20,
                                       config.getOr<int64_t>("neuron_count", 1000) // 40,
                                       config.getOr<int64_t>("neuron_count", 1000) // 40)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run experiment
    start_time = time.time()
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 200 == 0:
            stats = pynlm.collect_stats(brain)
            print(f"Step {step}: Reward {agent.getNeuromodulationLevel():.4f}, "
                  f"Curiosity {agent.getCuriosityLevel():.4f}, "
                  f"Firing rate {stats['average_firing_rate']:.4f}")
    
    end_time = time.time()
    
    # Save model
    brain.save(f"experiment_{config_type}_{steps}.bin")
    
    print(f"Experiment completed in {end_time - start_time:.2f} seconds")
    
    return brain, world, agent

# Run multiple experiments
experiments = {}
for config_type in ["research", "production", "development"]:
    print(f"\nRunning {config_type} experiment...")
    experiments[config_type] = run_comprehensive_experiment(config_type, 1000)
```

### Model Comparison
```python
import pynlm

# Train multiple models
models = {}
for seed in [42, 123, 999]:
    print(f"Training model with seed {seed}...")
    pynlm.set_random_seed(seed)
    
    # Create brain
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Train
    for step in range(500):
        brain.step(step)
    
    # Save model
    model_name = f"model_seed_{seed}.bin"
    brain.save(model_name)
    models[model_name] = brain

# Compare all models
print("\nComparing models...")
model_files = list(models.keys())
for i in range(len(model_files)):
    for j in range(i + 1, len(model_files)):
        diff = pynlm.compare_brains(model_files[i], model_files[j])
        print(f"Difference between {model_files[i]} and {model_files[j]}: {diff:.4f}")
```

## Conclusion

The enhanced NLM Python bindings provide a powerful, user-friendly interface to the NLM neural simulation framework. Key improvements include:

1. **Complete Error Handling**: Specific exception types for different error conditions
2. **Full Feature Coverage**: All memory, neuromodulation, and cognition systems included
3. **High-Level APIs**: Training, model management, and utility functions for common workflows
4. **Pythonic Design**: Properties, context managers, and natural method signatures
5. **Backward Compatibility**: Existing code continues to work without changes
6. **Comprehensive Documentation**: Examples and troubleshooting guides

The enhanced bindings make it easy to:
- Quickly prototype neural simulations
- Train and evaluate agents
- Compare different model configurations
- Monitor system performance
- Save and load trained models
- Handle errors gracefully

These bindings provide a professional-grade Python interface to the NLM framework while maintaining the simplicity and power of the original API.
