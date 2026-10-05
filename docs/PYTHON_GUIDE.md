# NLM Python User Guide

## Overview

This comprehensive guide provides Python users with everything they need to know to effectively use the enhanced NLM neural simulation framework. The guide covers installation, basic usage, advanced features, and best practices for the Python bindings.

## Installation and Setup

### Prerequisites

Before installing NLM Python bindings, ensure you have:

- **Python 3.7 or higher**
- **CMake 3.22 or higher** (for building from source)
- **Compiler with C++20 support** (GCC 9+, Clang 10+, MSVC 2019+)

### Installation Options

#### Option 1: pip (Recommended for most users)

```bash
# Install from PyPI (if available)
pip install pynlm

# Install in development mode
pip install -e .
```

#### Option 2: Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake ..

# Build the project
make -j$(nproc)

# Install Python module
make install
```

#### Option 3: Conda (Cross-platform solution)

```bash
# Create conda environment
conda create -n nlm python=3.9
conda activate nlm

# Install from PyPI or build locally
conda install pynlm
# or
pip install -e .
```

### Verification

After installation, verify that NLM is working:

```python
import pynlm

print("NLM version:", pynlm.__version__)

# Test basic functionality
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
print("Brain created successfully:", brain)

# Test numpy integration
import numpy as np
vision = pynlm.create_vision(10, 10, 3)
data = np.random.rand(300).astype(np.float32)
vision.set_data(data)
print("Numpy integration works:", vision.get_data().shape)
```

## Basic Usage

### 1. Creating Your First Brain

```python
import pynlm

# Create a default configuration
config = pynlm.create_default_config()

# Customize configuration
config.set("simulation_steps", 1000)
config.set("neuron_count", 100)
config.set("synapse_density", 0.1f)

# Create a brain with the configuration
brain = pynlm.create_brain(config)
brain.initialize()

print(f"Created brain with {brain.get_total_neuron_count()} neurons")
```

### 2. Running a Simple Simulation

```python
import pynlm
import time

# Create brain and world
brain = pynlm.create_brain()
world = pynlm.create_simple_world(50, 50)

# Initialize components
brain.initialize()

# Run simulation
print("Starting simulation...")
start_time = time.time()

for step in range(100):
    # Create simple visual input
    vision = pynlm.create_vision(10, 10, 3)
    
    # Create a simple pattern (white square in center)
    data = np.zeros(300, dtype=np.float32)
    for y in range(4, 7):
        for x in range(4, 7):
            idx = y * 30 + x * 3
            data[idx:idx+3] = [1.0, 1.0, 1.0]  # White
    
    vision.set_data(data)
    
    # Process input
    brain.receive_sensory_input(vision)
    
    # Step simulation
    brain.step(step, step * 0.01)
    
    # Print progress
    if step % 20 == 0:
        print(f"Step {step}: {brain.get_active_neuron_count()} active neurons")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### 3. Working with the World

```python
import pynlm

# Create world and agent brain
world = pynlm.create_simple_world(100, 100, visionWidth=20, visionHeight=20)
brain = pynlm.create_brain()
agent_brain = pynlm.create_agent_brain("agent", brain, world)

# Initialize with world
agent_brain.initialize(world)

# Reset world and agent
world.reset()
agent_brain.reset()

# Add some objects to the world
obj1 = pynlm.create_world_object(10, 10, pynlm.WorldObjectType.Resource, 10.0, 1.0)
obj2 = pynlm.create_world_object(30, 30, pynlm.WorldObjectType.Hazard, 5.0, 2.0)

world.add_object(obj1)
world.add_object(obj2)

# Get world information
print(f"World size: {world.get_width()} x {world.get_height()}")
print(f"Max energy: {world.get_max_energy()}")
```

### 4. Agent-Based Learning

```python
import pynlm

# Create agent with brain and world
brain = pynlm.create_brain()
world = pynlm.create_simple_world(80, 80)
agent = pynlm.create_nlm_agent("my_agent", brain, world)

# Run learning episode
print("Starting learning episode...")
episode_stats = agent.run_episode("learning_demo", 200)

print(f"Episode completed!")
print(f"Total reward: {episode_stats['total_reward']}")
print(f"Actions taken: {len(episode_stats['actions'])}")
print(f"Success rate: {episode_stats['success_rate'] * 100:.1f}%")
```

## Advanced Python Features

### 1. Context Manager Support

```python
import pynlm

# Create configuration
config = pynlm.create_brain_config("context_test", 500)

# Use brain as context manager (automatic cleanup)
with pynlm.create_brain(config) as brain:
    brain.initialize()
    
    # Do work with brain
    for step in range(100):
        brain.step(step, step * 0.01)
    
    # Brain is automatically reset when exiting the context

print("Context manager usage completed")
```

### 2. Numpy Integration

```python
import pynlm
import numpy as np

# Create brain
brain = pynlm.create_brain()
brain.initialize()

# Create vision from numpy array
vision = pynlm.create_vision(20, 20, 3)

# Create numpy array with a simple pattern
image_data = np.zeros((20, 20, 3), dtype=np.float32)

# Add a diagonal line
for i in range(20):
    image_data[i, i, :] = [1.0, 0.0, 0.0]  # Red diagonal
    image_data[i, 19-i, :] = [0.0, 0.0, 1.0]  # Blue anti-diagonal

# Set vision data from numpy
vision.set_data(image_data)

# Get data as numpy array
retrieved_data = vision.get_data()
print(f"Vision shape: {retrieved_data.shape}")

# Validate numpy shape
is_valid = vision.is_valid_numpy_shape(retrieved_data)
print(f"Numpy shape is valid: {is_valid}")
```

### 3. Simulation Manager

```python
import pynlm

# Create components
brain = pynlm.create_brain()
world = pynlm.create_simple_world(60, 60)

# Create simulation manager
sim_manager = pynlm.create_simulation_manager(brain, world)

# Run multiple episodes with checkpointing
episode_ids = []
for i in range(5):
    episode_id = f"episode_{i}"
    episode_ids.append(episode_id)
    
    print(f"Running {episode_id}...")
    stats = sim_manager.run_episode(episode_id, 100)
    
    print(f"  Reward: {stats['reward']:.2f}")
    print(f"  Success: {stats['success']}")
    
    # Save checkpoint
    sim_manager.save_checkpoint(episode_id, 99)

# Analyze episode performance
print("\nEpisode Analysis:")
for episode_id in episode_ids:
    stats = sim_manager.get_episode_stats(episode_id)
    print(f"{episode_id}: Reward={stats['reward']:.2f}, "
          f"Success={stats['success']}, Steps={stats['steps']}")

# Restore from checkpoint
print("\nRestoring episode_2 from checkpoint...")
sim_manager.restore_episode("episode_2", 50)

# Continue simulation
stats = sim_manager.run_episode("episode_2", 50, 100)
print(f"Restored episode completed. Reward: {stats['reward']:.2f}")
```

### 4. Advanced Visualization

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create components
brain = pynlm.create_brain()
world = pynlm.create_simple_world(50, 50)
visualizer = pynlm.create_visualizer(brain, world)

# Run simulation
brain.initialize()

for step in range(200):
    brain.step(step, step * 0.01)
    
    if step % 20 == 0:
        # Generate visualization data
        activity = visualizer.visualize_neuron_activity(brain)
        connections = visualizer.visualize_synaptic_connections(brain)
        regions = visualizer.visualize_brain_regions(brain)

# Create comprehensive visualization
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Neuron activity heatmap
im0 = axes[0, 0].imshow(activity, cmap='hot', interpolation='nearest')
axes[0, 0].set_title('Neural Activity Heatmap')
plt.colorbar(im0, ax=axes[0, 0])

# Synaptic connections adjacency matrix
im1 = axes[0, 1].imshow(connections, cmap='Blues', interpolation='nearest')
axes[0, 1].set_title('Synaptic Connections')
plt.colorbar(im1, ax=axes[0, 1])

# Brain regions visualization
im2 = axes[1, 0].imshow(regions[:, :3], cmap='viridis', interpolation='nearest')
axes[1, 0].set_title('Brain Regions')
plt.colorbar(im2, ax=axes[1, 0])

# Spike raster plot
spike_data = visualizer.generate_spike_raster(brain)
axes[1, 1].scatter(spike_data['spike_times'], 
                   [spike_data['spike_times'].shape[0]] * len(spike_data['spike_times']), 
                   s=1, c='red', alpha=0.6)
axes[1, 1].set_title('Spike Raster Plot')
axes[1, 1].set_xlabel('Time (ms)')
axes[1, 1].set_ylabel('Neuron Index')

plt.tight_layout()
plt.savefig('simulation_results.png', dpi=150)
print("Visualization saved to simulation_results.png")
```

### 5. Performance Monitoring

```python
import pynlm
import time
import numpy as np

# Create performance-optimized brain
config = pynlm.create_brain_config("performance_test", 10000)
brain = pynlm.create_brain(config, thread_safe=True, precision_mode="double")
brain.initialize()

# Enable memory pool for large simulations
brain.enable_memory_pool(True)

# Create performance timer
import pynlm
import time

# Create performance timer
timer = pynlm.create_timer("simulation_step")

# Run performance benchmark
print("Starting performance benchmark...")
start_time = time.time()

num_steps = 1000
step_times = []

for step in range(num_steps):
    step_start = time.time()
    
    # Generate and process sensory input
    vision = pynlm.create_vision(20, 20, 3)
    random_data = np.random.rand(1200).astype(np.float32)
    vision.set_data(random_data)
    
    # Process and step
    brain.receive_sensory_input(vision)
    brain.step(step, step * 0.01)
    
    step_time = time.time() - step_start
    step_times.append(step_time)
    
    if step % 200 == 0:
        print(f"Step {step}: {step_time*1000:.2f}ms")

end_time = time.time()
total_time = end_time - start_time

# Performance analysis
avg_step_time = np.mean(step_times)
std_step_time = np.std(step_times)
steps_per_second = num_steps / total_time

print(f"\nPerformance Results:")
print(f"  Total time: {total_time:.2f}s")
print(f"  Average step time: {avg_step_time*1000:.2f} ± {std_step_time*1000:.2f} ms")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Efficiency: {brain.get_average_firing_rate():.2f} firing rate")

# Export results for analysis
state = pynlm.export_brain_state(brain)
np.save('performance_results.npy', state)
print("Results saved to performance_results.npy")
```

### 6. Error Handling and Debugging

```python
import pynlm

# Create timer for debugging
debug_timer = pynlm.create_timer("debug_operation")

# Test configuration validation
print("Testing configuration validation...")
config = pynlm.create_default_config()

# Add invalid configuration
config.set("invalid_key", None)
config.set("empty_key", "")

# Validate
errors = config.validate()
if errors:
    print("Configuration errors found:")
    for key, error in errors.items():
        print(f"  {key}: {error}")
else:
    print("Configuration is valid")

# Test error handling in brain operations
try:
    brain = pynlm.create_brain()
    brain.initialize()
    
    # Try to step before initialization (should raise error)
    brain.step(0, 0.0)
    
except Exception as e:
    print(f"Caught expected error: {type(e).__name__}: {e}")

# Test numpy validation
print("\nTesting numpy validation...")
vision = pynlm.create_vision(10, 10, 3)

# Test invalid numpy array (wrong shape)
wrong_shape = np.random.rand(5, 5, 3)  # Too small
try:
    vision.set_data(wrong_shape)
except Exception as e:
    print(f"Caught shape validation error: {type(e).__name__}: {e}")

# Test correct numpy array
correct_shape = np.random.rand(300).astype(np.float32)
vision.validate_and_convert_numpy(correct_shape)
print("Correct numpy array accepted")
```

### 7. Advanced Features

#### Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.Config.from_dict({
    "simulation_steps": 5000,
    "neuron_count": 500,
    "synapse_density": 0.05,
    "enable_memory_pool": True,
    "thread_safe": True,
    "precision_mode": "double"
})

# Add custom settings
config.set("custom_setting1", 42)
config.set("custom_setting2", "hello")
config.set("custom_setting3", [1.0, 2.0, 3.0])

print("Custom configuration created:", config.to_dict())
```

#### Advanced Agent Features

```python
import pynlm

# Create agent with custom configuration
config = pynlm.create_agent_config(energy=200.0, max_steps=1000)
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world(100, 100)
agent = pynlm.create_nlm_agent("advanced_agent", brain, world, config)

# Configure agent features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

print(f"Agent features enabled:")
print(f"  Reward modulation: {agent.is_reward_modulation_enabled()}")
print(f"  Structural plasticity: {agent.is_structural_plasticity_enabled()}")
print(f"  Development: {agent.is_development_enabled()}")
print(f"  Curiosity: {agent.is_curiosity_enabled()}")

# Run learning with advanced features
print("\nRunning advanced learning episode...")
stats = agent.run_episode("advanced_learning", 300)

print(f"Learning completed:")
print(f"  Final energy: {agent.get_energy()}")
print(f"  Total reward: {stats['total_reward']}")
print(f"  Success rate: {stats['success_rate'] * 100:.1f}%")
```

#### Export/Import Utilities

```python
import pynlm
import numpy as np

# Create and run simulation
brain = pynlm.create_brain()
world = pynlm.create_simple_world(30, 30)

brain.initialize()

for step in range(100):
    brain.step(step, step * 0.01)

# Export brain state
brain_state = pynlm.export_brain_state(brain)
print(f"Exported brain state with keys: {list(brain_state.keys())}")

# Export world state
world_state = pynlm.export_world_state(world)
print(f"Exported world state with keys: {list(world_state.keys())}")

# Save to files
np.save('brain_state.npy', brain_state['neurons'])
np.save('world_state.npy', brain_state['world'])

# Import brain state
import_brain = pynlm.create_brain()
pynlm.import_brain_state(import_brain, brain_state)
print("Brain state imported successfully")
```

## Best Practices

### 1. Configuration Management

```python
# Best practice: Create configuration early and consistently
from typing import Dict, Any

def create_standard_config() -> Dict[str, Any]:
    """Create a standard configuration for all simulations."""
    return {
        "simulation_steps": 1000,
        "neuron_count": 100,
        "synapse_density": 0.1,
        "enable_memory_pool": True,
        "thread_safe": False,
        "precision_mode": "single"
    }

# Use standard configuration
config_dict = create_standard_config()
config = pynlm.Config.from_dict(config_dict)
```

### 2. Resource Management

```python
# Best practice: Use context managers for resource cleanup
def run_simulation_safely(config_dict):
    """Run simulation with proper resource management."""
    config = pynlm.Config.from_dict(config_dict)
    
    try:
        with pynlm.create_brain(config) as brain:
            brain.initialize()
            
            # Run simulation
            for step in range(config_dict["simulation_steps"]):
                # ... simulation code ...
                pass
                
    except Exception as e:
        print(f"Simulation failed: {e}")
        raise

# Use with custom configuration
config = {"simulation_steps": 500, "neuron_count": 50}
run_simulation_safely(config)
```

### 3. Error Handling

```python
# Best practice: Comprehensive error handling
def safe_simulation_step(brain, step, time):
    """Safely step brain with error handling."""
    try:
        brain.step(step, time)
        return True, None
    except RuntimeError as e:
        return False, f"Runtime error in step {step}: {e}"
    except MemoryError as e:
        return False, f"Memory error in step {step}: {e}"
    except Exception as e:
        return False, f"Unexpected error in step {step}: {type(e).__name__}: {e}"

# Use in simulation loop
for step in range(total_steps):
    success, error = safe_simulation_step(brain, step, step * 0.01)
    if not success:
        print(f"Simulation failed at step {step}: {error}")
        break
```

### 4. Performance Optimization

```python
# Best practice: Monitor and optimize performance
def benchmark_performance(brain, num_steps=100):
    """Benchmark brain performance."""
    import time
    import numpy as np
    
    # Warm up
    for i in range(10):
        brain.step(i, i * 0.01)
    
    # Benchmark
    start_time = time.time()
    step_times = []
    
    for step in range(num_steps):
        step_start = time.time()
        brain.step(step, step * 0.01)
        step_times.append(time.time() - step_start)
    
    end_time = time.time()
    
    # Analyze results
    avg_time = np.mean(step_times)
    std_time = np.std(step_times)
    steps_per_second = num_steps / (end_time - start_time)
    
    return {
        "total_time": end_time - start_time,
        "avg_step_time": avg_time,
        "std_step_time": std_time,
        "steps_per_second": steps_per_second
    }

# Use for performance tuning
results = benchmark_performance(brain)
print(f"Performance: {results['steps_per_second']:.1f} steps/second")
```

### 5. Data Management

```python
# Best practice: Proper data management and cleanup
def cleanup_simulation_resources():
    """Clean up simulation resources."""
    import gc
    
    # Clear numpy arrays from memory
    gc.collect()
    
    # Close any open files
    # Close any open network connections
    # Reset global state if any

# Use in cleanup
import atexit
atexit.register(cleanup_simulation_resources)
```

## Common Use Cases

### Use Case 1: Visual Processing Simulation

```python
import pynlm
import numpy as np

# Create visual processing simulation
config = pynlm.create_brain_config("visual_processing", 1000)
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world(100, 100, visionWidth=20, visionHeight=20)

brain.initialize()

# Create visual inputs
for pattern in ["edge", "circle", "square"]:
    for angle in [0, 45, 90, 135, 180]:
        # Create rotated visual pattern
        vision = pynlm.create_vision(20, 20, 3)
        
        # Generate pattern based on type and angle
        if pattern == "edge":
            # Create edge detection pattern
            pass
        elif pattern == "circle":
            # Create circle pattern
            pass
        elif pattern == "square":
            # Create square pattern
            pass
        
        # Process with brain
        brain.receive_sensory_input(vision)
        brain.step(0, 0.01)
        
        # Record response
        activity = brain.visualize_neuron_activity()
```

### Use Case 2: Agent Navigation

```python
import pynlm

# Create navigation simulation
world = pynlm.create_simple_world(200, 200)
brain = pynlm.create_brain()
agent = pynlm.create_nlm_agent("navigator", brain, world)

# Set agent starting position
agent_body = agent.get_brain().get_agent_body()
agent_body.set_position(10, 10)
world.set_agent(agent_body)

# Run navigation episode
stats = agent.run_episode("navigation", 500)

# Analyze path
print(f"Navigation completed in {stats['steps']} steps")
print(f"Total reward: {stats['total_reward']}")
print(f"Success rate: {stats['success_rate'] * 100:.1f}%")
```

### Use Case 3: Learning with Exploration

```python
import pynlm
import numpy as np

# Create exploration learning simulation
config = pynlm.create_agent_config(energy=100.0, max_steps=1000)
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world(50, 50)
agent = pynlm.create_nlm_agent("explorer", brain, world, config)

# Enable curiosity for exploration
agent.enable_curiosity(True)

# Run exploration episode
print("Starting exploration...")
stats = agent.run_episode("exploration", 300)

# Analyze exploration behavior
print(f"Exploration completed:")
print(f"  Steps taken: {stats['steps']}")
print(f"  Novel actions: {stats['novel_actions']}")
print(f"  Success rate: {stats['success_rate'] * 100:.1f}%")
print(f"  Total reward: {stats['total_reward']}")
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python, here are some key differences:

1. **Memory Management**: Python handles memory automatically, but be careful with large arrays
2. **Error Handling**: Python uses exceptions, C++ may use return codes
3. **Type Hints**: Python provides type hints, C++ has strong static typing
4. **Configuration**: Python uses dictionaries, C++ uses Config class
5. **Context Management**: Python has context managers for automatic cleanup

### Example Migration

```cpp
// C++ version
Config config;
config.set("steps", 100);
config.set("neurons", 50);

Brain brain(config);
brain.initialize();

for (int i = 0; i < 100; ++i) {
    brain.step(i, i * 0.01f);
}
```

```python
# Python equivalent
config = pynlm.create_brain_config("migration_test", 100)

brain = pynlm.create_brain(config)
brain.initialize()

for step in range(100):
    brain.step(step, step * 0.01)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: "ModuleNotFoundError: No module named 'pynlm'"
**Solution**: Install NLM Python bindings:
```bash
pip install pynlm
# or
pip install -e .
```

#### Issue: "ImportError: libpynlm.so: cannot open shared object file"
**Solution**: Set LD_LIBRARY_PATH:
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/nlm/build
```

#### Issue: "RuntimeError: Brain not initialized"
**Solution**: Always initialize brain before use:
```python
brain = pynlm.create_brain()
brain.initialize()  # Required!
```

#### Issue: Memory issues with large simulations
**Solution**: Enable memory pool and use efficient data structures:
```python
brain.enable_memory_pool(True)
# Use smaller arrays when possible
```

#### Issue: Slow simulation performance
**Solution**: Enable optimizations:
```python
config = pynlm.create_brain_config("optimized", 1000)
config.set("enable_memory_pool", True)
config.set("thread_safe", True)
```

### Getting Help

If you need additional help:

1. **Documentation**: Check this guide for detailed API documentation
2. **Examples**: Try the examples in the examples directory
3. **Community**: Join the NLM community for support
4. **Issues**: Report bugs and feature requests at GitHub
5. **Academic**: Contact academic team for research collaborations

## Further Reading

### Related Documentation
- [C++ API Reference](docs/API_REFERENCE.md)
- [Performance Guide](docs/PERFORMANCE_GUIDE.md)
- [Examples and Demos](docs/EXAMPLES.md)
- [Contribution Guidelines](docs/CONTRIBUTION_GUIDE.md)

### Academic Papers
- Maass, W., Natschläger, T., & Markram, H. (2002). "Real-time computing with spiking neurons." TICAM.
- Vogl, T. P., et al. (1995). "NeuroPhys: A neural simulation environment." Neural Networks.

### External Resources
- [PyBind11 Documentation](https://pybind11.readthedocs.io/)
- [NumPy Documentation](https://numpy.org/doc/)
- [Matplotlib Documentation](https://matplotlib.org/)

## Conclusion

The NLM Python bindings provide a powerful and flexible interface to the NLM neural simulation framework. Whether you're doing simple simulations, complex learning experiments, or performance-critical applications, this guide provides everything you need to get started and succeed.

The Python API maintains the full power and flexibility of NLM while providing:
- Simple, intuitive interface
- NumPy integration for scientific computing
- Context managers for resource management
- Comprehensive error handling
- Performance optimizations
- Extensive documentation and examples

Happy simulating with NLM!
