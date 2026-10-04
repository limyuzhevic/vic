# NLM Advanced Python Bindings - Installation and Usage Guide

## Overview

The NLM framework now includes advanced Python bindings that provide access to all the new features implemented in this improvement project. These bindings are organized into two modules:

1. **pynlm** - Basic bindings (existing functionality)
2. **pynlm_advanced** - Enhanced bindings with all new features

## Installation

### Prerequisites

- Python 3.8 or higher
- pybind11 development headers
- CMake 3.16 or higher
- C++20 compatible compiler

### Build and Install

```bash
# Clone the NLM repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build the project
make -j$(nproc)

# Install the Python modules
pip install -e .
```

## Quick Start Examples

### Basic Usage (pynlm)

```python
import pynlm as nlm

# Create a default brain
brain = nlm.createDefaultBrain()

# Create a simple world
world = nlm.createSimpleWorld()

# Create an agent
agent = nlm.createAgentBrain(brain)

# Run a simulation for 100 steps
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation completed. Firing neurons: {brain.getFiringNeuronCount()}")
```

### Advanced Usage (pynlm_advanced)

```python
import pynlm_advanced as nlm

# Create an enhanced brain using the factory
config = nlm.BrainFactoryConfig()
config.brainType = nlm.BrainComponentType.EnhancedBrain
config.neuronCount = 5000
config.enablePlasticity = True
config.enableDevelopment = True
config.enableNeuromodulation = True

brain = nlm.BrainFactory.createBrain(config)

# Create an advanced topology (Convolutional Network)
topo_config = nlm.AdvancedTopologyConfig()
topo_config.topologyType = nlm.AdvancedTopologyType.Convolutional
topo_config.layerSizes = [256, 128, 64, 32, 16]
advanced_brain = nlm.AdvancedTopologyManager.createFromTopology(topo_config)

# Set up thread-safe simulation with 4 threads
thread_pool = nlm.ThreadPool(4)
sim_manager = nlm.ThreadSafeSimulationManager(thread_pool)

# Monitor memory usage
memory_allocator = nlm.ThreadSafeAllocator()
allocator_stats = memory_allocator.getStats()
print(f"Memory allocator stats: {allocator_stats}")

# Create simulation with enhanced error handling
for step in range(100):
    sim_manager.addNeuralOperation(lambda: brain.step(step))
    if step % 10 == 0:
        sim_manager.executeOperations()

# Clean shutdown
sim_manager.stopSimulation()

# Create a custom brain from file
custom_brain = nlm.BrainFactory.createBrainFromFile("configs/advanced.cfg")

print(f"Advanced brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Topology layers: {len(topo_config.layerSizes)}")
```

### Command System Usage

```python
import pynlm_advanced as nlm

# Create a command manager
command_manager = nlm.CommandManager()

# Add different types of commands
from pynlm_advanced import SimpleCommand

# Exploration command (higher priority)
explore_cmd = SimpleCommand(
    nlm.Action(nlm.ActionType.MoveForward),
    priority=1.5
)
command_manager.addCommand(explore_cmd)

# Reward-based command
reward_cmd = SimpleCommand(
    nlm.Action(nlm.ActionType.Interact),
    priority=1.2
)
command_manager.addCommand(reward_cmd)

# Execute commands and get the best action
brain = nlm.createEnhancedBrain()
action = command_manager.executeCommands(brain)
print(f"Selected action: {action.getName()}")

# Enable/disable commands
command_manager.setCommandEnabled("explore_cmd", True)
command_manager.setCommandPriority("reward_cmd", 2.0)
```

### Thread-Safe Simulation

```python
import pynlm_advanced as nlm

# Create a brain
brain = nlm.createEnhancedBrain(2000)

# Create thread pool with 4 threads
thread_pool = nlm.ThreadPool(4)

# Create thread-safe simulation manager
sim_manager = nlm.ThreadSafeSimulationManager(thread_pool)

# Get simulation statistics before starting
stats = sim_manager.getStats()
print(f"Initial simulation stats: {stats}")

# Start simulation with multiple neural operations
sim_manager.startSimulation(lambda: brain.step(0))
sim_manager.addNeuralOperation(lambda: brain.injectCurrentToNeurons(nlm.NeuronType::Excitatory, 10.0f))

# Execute all operations
sim_manager.executeOperations()

# Get simulation statistics after execution
stats = sim_manager.getStats()
print(f"Final simulation stats: {stats}")

# Stop simulation
sim_manager.stopSimulation()
```

### Advanced Topology Management

```python
import pynlm_advanced as nlm

# Create different types of advanced topologies

# 1. Convolutional Network
conv_config = nlm.AdvancedTopologyConfig()
conv_config.topologyType = nlm.AdvancedTopologyType.Convolutional
conv_config.layerSizes = [256, 128, 64, 32, 16]
conv_config.activationFunction = nlm.AdvancedTopologyConfig.ActivationFunction.ReLU
conv_config.batchNormalization = True
conv_brain = nlm.AdvancedTopologyManager.createFromTopology(conv_config)

# 2. Residual Network
res_config = nlm.AdvancedTopologyConfig()
res_config.topologyType = nlm.AdvancedTopologyConfig.Residual
topo_config.layerSizes = [128, 128, 128, 128]
res_config.residualConnections = True
res_brain = nlm.AdvancedTopologyManager.createFromTopology(res_config)

# 3. Autoencoder
auto_config = nlm.AdvancedTopologyConfig()
auto_config.topologyType = nlm.AdvancedTopologyType.AutoEncoder
auto_config.layerSizes = [100, 50, 20, 50, 100]
auto_brain = nlm.AdvancedTopologyManager.createFromTopology(auto_config)

# 4. Graph Neural Network
gnn_config = nlm.AdvancedTopologyConfig()
gnn_config.topologyType = nlm.AdvancedTopologyType.GraphNeuralNetwork
gnn_config.layerSizes = [500, 200, 100]
gnn_config.sparsity = 0.9  # Very sparse connections
gnn_brain = nlm.AdvancedTopologyManager.createFromTopology(gnn_config)

# 5. Hybrid Network
hybrid_config = nlm.AdvancedTopologyConfig()
hybrid_config.topologyType = nlm.AdvancedTopologyType.HybridTopology
# Can combine multiple sub-topologies

print(f"Convolutional network: {conv_brain.getTotalNeuronCount()} neurons")
print(f"Residual network: {res_brain.getTotalNeuronCount()} neurons")
print(f"Autoencoder: {auto_brain.getTotalNeuronCount()} neurons")
print(f"Graph neural network: {gnn_brain.getTotalNeuronCount()} neurons")
```

### Memory Management and Monitoring

```python
import pynlm_advanced as nlm
import time

# Create memory allocator
memory_allocator = nlm.ThreadSafeAllocator(2048)

# Allocate some neural memory
neuron_count = 1000
synapse_count = 5000
allocated = memory_allocator.allocate(neuron_count * sizeof(float) + synapse_count * sizeof(float))

# Check memory stats
stats = memory_allocator.getStats()
print(f"Memory stats: {stats}")

# Monitor memory usage over time
for i in range(5):
    memory_allocator.setTrackingEnabled(True)
    time.sleep(0.1)
    current_stats = memory_allocator.getStats()
    print(f"Iteration {i}: {current_stats}")

# Deallocate memory
memory_allocator.deallocate(allocated, neuron_count * sizeof(float) + synapse_count * sizeof(float))

# Final stats
final_stats = memory_allocator.getStats()
print(f"Final memory stats: {final_stats}")
```

### Error Handling and Validation

```python
import pynlm_advanced as nlm

# Demonstrate error handling
try:
    # Try to create an invalid configuration
    invalid_config = nlm.BrainFactoryConfig()
    invalid_config.neuronCount = 0  # Invalid: zero neurons

    if not invalid_config.isValid():
        print(f"Invalid configuration detected: {nlm.BrainFactory.validateConfig(invalid_config)}")

    # Demonstrate error result handling
    error_result = nlm.ErrorResult(False, "Test error message", nlm.ErrorCategory::Validation)

    if error_result.isError():
        print(f"Error: {error_result.message}")
        print(f"Error category: {error_result.category}")

except nlm.NLMException as e:
    print(f"NLM Exception: {e.what()} (Category: {e.getCategory()})")
except std::runtime_error as e:
    print(f"Runtime Error: {e.what()}")
```

## Advanced Factory Methods

### Factory Configuration Options

```python
import pynlm_advanced as nlm

# Get default configuration
from pynlm_advanced import BrainFactoryConfig, BrainComponentType, NeuronTypeConfig, NetworkTopology

config = nlm.BrainFactory.getDefaultConfig()
print(f"Default brain: {config.neuronCount} neurons")

# Get enhanced configuration
enhanced_config = nlm.BrainFactory.getEnhancedConfig()
print(f"Enhanced brain: {enhanced_config.neuronCount} neurons")

# Create custom brain with specific parameters
custom_config = nlm.BrainFactoryConfig()
custom_config.brainType = nlm.BrainComponentType.Custom
custom_config.neuronCount = 3000
custom_config.connectionDensity = 0.15
custom_config.enablePlasticity = True
custom_config.enableDevelopment = False
custom_config.enableNeuromodulation = True
custom_brain = nlm.BrainFactory.createBrain(custom_config)
```

## Thread Safety and Parallel Processing

```python
import pynlm_advanced as nlm
import threading

# Create multiple thread pools
pool1 = nlm.ThreadPool(2)
pool2 = nlm.ThreadPool(4)

# Create simulation managers with different thread pools
sim1 = nlm.ThreadSafeSimulationManager(pool1)
sim2 = nlm.ThreadSafeSimulationManager(pool2)

# Create brains for each simulation
brain1 = nlm.createEnhancedBrain(1000)
brain2 = nlm.createMinimalBrain(500)

# Start parallel simulations
def run_simulation(sim_manager, brain, brain_id):
    sim_manager.startSimulation(lambda: brain.step(0))
    time.sleep(0.1)  # Simulate some work
    sim_manager.stopSimulation()
    print(f"Simulation {brain_id} completed")

thread1 = threading.Thread(target=run_simulation, args=(sim1, brain1, 1))
thread2 = threading.Thread(target=run_simulation, args=(sim2, brain2, 2))

thread1.start()
thread2.start()

thread1.join()
thread2.join()

print("Both simulations completed successfully")
```

## Performance Monitoring

```python
import pynlm_advanced as nlm
import time

# Create enhanced brain
brain = nlm.createEnhancedBrain(3000)

# Create thread pool
thread_pool = nlm.ThreadPool(4)

# Create simulation manager
sim_manager = nlm.ThreadSafeSimulationManager(thread_pool)

# Monitor performance over time
print("Starting performance monitoring...")
for i in range(10):
    start_time = time.time()
    sim_manager.startSimulation(lambda: brain.step(i))
    sim_manager.executeOperations()
    end_time = time.time()

    stats = sim_manager.getStats()
    print(f"Iteration {i}:")
    print(f"  Simulation speed: {stats.simulationSpeed:.2f} steps/sec")
    print(f"  Active threads: {stats.activeThreads}")
    print(f"  Average task time: {stats.averageTaskTime:.4f} sec")
    print(f"  Memory usage: {stats.memoryUsage} MB")
    print(f"  CPU usage: {stats.cpuUsage:.1f}%")

    sim_manager.stopSimulation()
    time.sleep(0.1)

print("Performance monitoring completed")
```

## Migration Guide

### From pynlm to pynlm_advanced

If you're upgrading from the basic pynlm bindings to the advanced bindings, you only need to change the import statement:

```python
# Old code
import pynlm as nlm

# New code
import pynlm_advanced as nlm
```

Most existing code will continue to work with the enhanced bindings, as all original functionality is preserved while new features are added.

## Troubleshooting

### Common Issues

1. **Module not found**: Ensure you've installed the package with `pip install -e .`

2. **Missing dependencies**: Install pybind11 and other dependencies

3. **Build errors**: Ensure CMake version is 3.16 or higher

4. **Memory issues**: Monitor memory usage with the provided memory allocator

### Error Messages

- **Validation Error**: Check configuration parameters using `BrainFactory.validateConfig()`
- **Memory Allocation Error**: Monitor memory usage and consider reducing memory pool size
- **Thread Pool Error**: Check thread pool initialization parameters
- **Topology Error**: Validate topology configuration before creation

## Best Practices

### Memory Management

1. **Use thread-safe allocation**: Always use `ThreadSafeAllocator` for memory management in multi-threaded environments
2. **Monitor memory usage**: Regularly check memory stats to prevent memory leaks
3. **Proper cleanup**: Always stop simulation managers and deallocate memory when done

### Simulation Management

1. **Use thread pools**: Leverage `ThreadPool` for parallel processing
2. **Batch operations**: Group neural operations together for better performance
3. **Monitor performance**: Regularly check simulation statistics

### Error Handling

1. **Validate configurations**: Always validate configuration before creating brains or topologies
2. **Handle exceptions**: Use try-catch blocks for robust error handling
3. **Log errors**: Use the logging system provided by the framework

### Performance Optimization

1. **Adjust thread counts**: Experiment with different thread pool sizes
2. **Optimize topologies**: Choose appropriate topology types for your use case
3. **Monitor bottlenecks**: Regularly check simulation statistics for bottlenecks

## Conclusion

The NLM Advanced Python bindings provide a comprehensive set of tools for advanced neural network simulation. Whether you're building sophisticated brain models, implementing complex topologies, or running parallel simulations, the enhanced bindings offer everything you need for advanced NLM usage.

All existing functionality is preserved, so existing code will continue to work while new features provide additional capabilities for advanced users.
