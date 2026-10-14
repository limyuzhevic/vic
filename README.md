# NLM Python Package - Quick Start Guide

This guide provides a quick introduction to using the enhanced NLM Python API.

## What is NLM?

NLM (Neural Learning Machine) is an experimental artificial developmental brain project. It simulates spiking neural networks with advanced features like:

- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Development and maturation
- Prediction and planning
- Social learning and continual learning

## Quick Start

### 1. Install the Package

```bash
pip install pynlm

# Or for development
pip install -e .
```

### 2. Basic Usage

```python
import nlm

# Create a brain with default configuration
brain = nlm.create_brain()
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Firing neurons: {brain.get_firing_neuron_count()}")
```

### 3. Enhanced Session Management

```python
import nlm

# Use the enhanced context manager
with nlm.BrainSession() as session:
    # Create world
    session.create_world(width=16.0, height=16.0, 
                        vision_width=8, vision_height=8)
    
    # Create agent
    session.create_agent()
    
    # Run simulation with learning
    session.run(1000, enable_learning=True)
    
    # Get statistics
    stats = session.get_stats()
    print(f"Simulation stats: {stats}")
```

### 4. Advanced Configuration

```python
import nlm

# Create enhanced configuration
config = nlm.Config({
    'neuron_count': 2000,
    'region_count': 2,
    'simulation_timestep': 0.001,
    'enable_reward_modulation': True,
    'enable_development': True,
    'enable_curiosity': True,
    'enable_structural_plasticity': True,
})

# Use with brain
brain = nlm.create_brain(config)
brain.initialize()
```

### 5. Memory System Example

```python
import nlm

# Create memory systems
working_memory = nlm.WorkingMemory(capacity=100)
episodic_memory = nlm.EpisodicMemory(max_episodes=1000)

# Store in working memory
working_memory.store("neuron_1", 0.8)
working_memory.store("neuron_2", 0.6)

print(f"Working memory traces: {working_memory.get_active_traces()}")

# Store in episodic memory
observations = [{'x': 1.0, 'y': 2.0}, {'x': 3.0, 'y': 4.0}]
episodic_memory.store_episode(
    reward=1.5,
    action="move",
    observations=observations,
    neurons=["neuron_1", "neuron_2"]
)

print(f"Episodic memory episodes: {episodic_memory.get_episode_count()}")
```

### 6. Agent with Neuromodulation

```python
import nlm

# Create complete simulation
with nlm.BrainSession() as session:
    world = session.create_world(20.0, 20.0, 8, 8)
    agent = session.create_agent()
    
    # Enable all advanced features
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    agent.enable_structural_plasticity(True)
    
    # Run simulation
    for step in range(100):
        # Process sensory input
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Step brain
        session.brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        result = world.apply_motor_command(action, step * 0.1)
        
        # Apply learning
        agent.apply_reward_modulation(result.reward)
        agent.update_development(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}: Dopamine={agent.get_neuromodulation_level():.2f}, "
                  f"Curiosity={agent.get_curiosity_level():.2f}, "
                  f"Firing={session.brain.get_firing_neuron_count()}")
```

### 7. Performance Monitoring

```python
import nlm

# Monitor simulation performance
monitor = nlm.PerformanceMonitor()
monitor.start()

with nlm.BrainSession() as session:
    for step in range(50):
        session.step(step)
        monitor.record_step()

# Get performance statistics
stats = monitor.get_stats()
print(f"Performance statistics:")
print(f"  Total steps: {stats['total_steps']}")
print(f"  Average step time: {stats['avg_step_time']:.3f}s")
print(f"  Steps per second: {stats['steps_per_second']:.1f}")
print(f"  Current memory: {stats['current_memory']}")
```

## Key Improvements

### Enhanced Pythonic API
- **Context managers**: `BrainSession()` for automatic resource cleanup
- **Property access**: `brain.neuron_count` instead of `brain->getTotalNeuronCount()`
- **Dictionary-style**: `config['neuron_count'] = 1000`
- **Rich monitoring**: Access to detailed simulation statistics

### Advanced Features
- **Memory systems**: Working and episodic memory with Python interfaces
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Development**: Developmental stages and maturation
- **Configuration**: Advanced configuration with validation
- **Error handling**: Comprehensive custom exception classes

### Development Tools
- **Progress bars**: Visual progress indicators for long simulations
- **Performance monitoring**: Track simulation timing and memory usage
- **Testing utilities**: Comprehensive test suite and examples
- **Documentation**: Detailed docstrings and examples

## Examples Directory

The package includes several example scripts:

### `examples/basic_usage.py`
- Complete working example showing enhanced API
- Demonstrates context manager usage
- Shows configuration and monitoring features

### `examples/config_tutorial.py`
- Configuration management tutorial
- Shows advanced configuration options
- Demonstrates validation and error handling

### `examples/advanced_simulation.py`
- Complex simulation example
- Shows agent-world interaction
- Demonstrates learning and development

### `tests/test_basic_api.py`
- Comprehensive test suite for new Python API
- Tests for all new classes and methods
- Validation and error handling tests

## Installation from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install with pip
pip install .

# Or for development
pip install -e .

# Build documentation
git clone https://github.com/nlm-project/nlm-docs.git docs
```

## Documentation

For comprehensive documentation, see:

- **API Reference**: `docs/api.md`
- **User Guide**: `docs/user_guide.md`
- **Examples**: `examples/` directory
- **Tutorials**: `docs/tutorials/` directory
- **Enhancements Summary**: `ENHANCEMENTS_SUMMARY.md`

## Getting Help

### Common Issues

1. **Installation problems**: Ensure you have CMake and a C++ compiler
2. **Import errors**: Check that the package is installed correctly
3. **Memory issues**: Use `nlm.get_memory_usage()` for monitoring
4. **Performance issues**: Use `nlm.set_thread_pool_size()` for multi-threading

### Getting Help

- **Documentation**: Check the `docs/` directory
- **Examples**: Look at the `examples/` directory
- **Community**: Join the NLM community for support
- **Issues**: Report bugs on GitHub

## License

MIT License. See `LICENSE` file for details.

## Authors

NLM Research Team

---

# Quick Troubleshooting

## Installation Issues

### Missing Dependencies
```bash
# On Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake python3-dev python3-pip

# On macOS
brew install cmake python3
```

### Build Issues

If the build fails, try:

```bash
# Clean build
rm -rf build
pip install -e . --no-build-isolation
```

## Usage Issues

### "ImportError: cannot import name 'Brain'"

This means the C++ extension wasn't built properly. Try rebuilding:

```bash
pip install --force-reinstall .
```

### Memory Issues

If you run out of memory:

```python
import nlm

# Monitor memory usage
while True:
    memory = nlm.get_memory_usage()
    print(f"Memory usage: {memory}")
    time.sleep(1.0)
```

### Performance Issues

For slow simulations:

```python
import nlm

# Configure for performance
config = nlm.Config({
    'neuron_count': 500,  # Smaller network
    'simulation_timestep': 0.01,  # Larger timestep
})

brain = nlm.create_brain(config)
brain.initialize()
```

## Performance Tips

### Optimize Configuration
```python
# Use sensible defaults
config = nlm.Config({
    'neuron_count': 1000,
    'simulation_timestep': 0.001,
    'enable_reward_modulation': True,
    'enable_curiosity': True,
})
```

### Monitor Performance
```python
import nlm

monitor = nlm.PerformanceMonitor()
monitor.start()

# Run simulation
with nlm.BrainSession() as session:
    for step in range(num_steps):
        session.step(step)
        monitor.record_step()

stats = monitor.get_stats()
print(f"Performance: {stats['steps_per_second']:.1f} steps/sec")
```

### Use Checkpoints
```python
import nlm

# Save simulation state
brain.save("./simulation_checkpoint.bin")

# Load later
brain.load("./simulation_checkpoint.bin")
```

---

# Version Information

**Current Version**: 0.1.0
**Python Compatibility**: 3.8+
**Build System**: CMake + scikit-build-core
**License**: MIT

**Build Date**: 2026-10-14
**Author**: NLM Research Team

---

For the most up-to-date information and documentation, visit:
https://github.com/nlm-project/nlm

Happy coding with NLM!