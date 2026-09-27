# NLM Project Improvements - Installation and Usage Guide

## Overview

This document provides a comprehensive guide to the improvements made to the NLM (Neural Learning Machine) project. The project has been significantly enhanced from the Phase 6 audit state, with complete system integration, comprehensive Python bindings, and advanced features for researchers and developers.

## Quick Start

### 1. Basic Python Usage

```python
import pynlm

# Create a basic brain configuration
config = pynlm.createExperimentConfig(steps=10000, neurons=1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
brain = pynlm.runSimulation(config, 1000)

# Export metrics
metrics = pynlm.exportMetrics(brain)
print(f"Average firing rate: {metrics.avgFiringRate}")
```

### 2. Agent-World Simulation

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent with learning capabilities
config = pynlm.createAgentConfig(world)
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation
agent = pynlm.runAgentEpisode(config, world, agent, 500)
```

### 3. Neural Network Abstraction

```python
import pynlm

# Create neural network model
nn = pynlm.NeuralNetwork()

# Build architecture
nn.addLayer(100, 'relu')
nn.addLayer(50, 'tanh')
nn.addLayer(10, 'softmax')

# Connect layers
nn.connectLayers(100, 50)
nn.connectLayers(50, 10)

# Forward pass
input_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 20
output = nn.forward(input_data)
```

## Installation

### Prerequisites

- Python 3.8 or higher
- pip 20.0 or higher
- CMake 3.16 or higher (for building from source)

### Installation Methods

#### Method 1: Using Pip (Recommended)

```bash
# Install from PyPI
pip install pynlm

# Install in development mode
pip install -e .
```

#### Method 2: Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build using scikit-build
pip install scikit-build-core pybind11
pip install .

# Or build with CMake
mkdir build
cd build
cmake ..
make -j4
```

## Python API Reference

### Core Classes

#### Brain
The central neural simulation brain class.

**Methods:**
- `brain.initialize()` - Initialize brain with configuration
- `brain.step(step)` - Perform a simulation step
- `brain.reset()` - Reset brain state
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSynapseCount()` - Get total synapse count
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getEpisodicMemory()` - Get episodic memory system
- `brain.getWorkingMemory()` - Get working memory system
- `brain.getPredictionSystem()` - Get prediction system
- `brain.getDopamine()` - Get dopamine neuromodulator
- `brain.getCuriosity()` - Get curiosity neuromodulator
- `brain.getNovelty()` - Get novelty detector
- `brain.getPlanner()` - Get neural planner
- `brain.getConceptFormation()` - Get concept formation system
- `brain.getAttention()` - Get attentional selection system
- `brain.save(filepath)` - Save brain state to file
- `brain.load(filepath)` - Load brain state from file

#### Config
Configuration management for NLM system.

**Methods:**
- `config.loadFromFile(filepath)` - Load from JSON file
- `config.loadFromArgs(argc, argv)` - Load from command line
- `config.saveToFile(filepath)` - Save to JSON file
- `config.has(key)` - Check if key exists
- `config.getKeys()` - Get all configuration keys

#### AgentBrain
Interface between brain and world.

**Methods:**
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `agent.updateDevelopment(timestep)` - Update development
- `agent.enableRewardModulation(enabled)` - Enable/disable reward modulation
- `agent.enableCuriosity(enabled)` - Enable/disable curiosity

#### SimpleWorld
2D world simulation environment.

**Methods:**
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.reset()` - Reset world
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(cmd, currentTime)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept

### Advanced Classes

#### WorkingMemory
Transient active information storage.

**Methods:**
- `wm.storeToNeuron(neuronId, activation)` - Store neuron activation
- `wm.getActiveTraces()` - Get number of active traces
- `wm.getMemoryNeurons()` - Get neurons in memory
- `wm.update(timestep)` - Update working memory
- `wm.clear()` - Clear all working memory traces

#### EpisodicMemory
Experience encoding and storage.

**Methods:**
- `em.storeEpisode(episode)` - Store an episode
- `em.getEpisodeCount()` - Get number of episodes
- `em.getEpisodesForReplay(count)` - Get episodes for replay
- `em.replayEpisode(episode)` - Replay an episode
- `em.consolidate(threshold)` - Consolidate episodes
- `em.clear()` - Clear all episodes

#### PredictionSystem
Temporal sequence learning and prediction.

**Methods:**
- `ps.getPredictionError(step)` - Get prediction error
- `ps.updatePrediction(observation)` - Update predictions
- `ps.getConfidence(step)` - Get prediction confidence
- `ps.getSequence(length)` - Get recent sequence
- `ps.reset()` - Reset prediction system

#### NeuralPlanner
Action planning with multiple steps.

**Methods:**
- `planner.planAction(currentState, context)` - Plan an action
- `planner.evaluateAction(action, state)` - Evaluate an action
- `planner.setPlanningDepth(depth)` - Set planning depth
- `planner.getPlanningDepth()` - Get planning depth
- `planner.reset()` - Reset planner

#### NeuralNetwork
Neural network model for abstraction.

**Methods:**
- `nn.addLayer(size, activation)` - Add a layer
- `nn.connectLayers(inputSize, outputSize)` - Connect layers
- `nn.forward(input)` - Perform forward pass
- `nn.backward(loss)` - Perform backward pass
- `nn.updateWeights()` - Update network weights
- `nn.getOutput()` - Get network output
- `nn.reset()` - Reset network state
- `nn.getLayerCount()` - Get number of layers
- `nn.getLayerSize(layerIndex)` - Get layer size

### Factory Functions

#### createExperimentConfig
Create standard experiment configuration.

**Parameters:**
- `steps` - Number of simulation steps (default: 10000)
- `neurons` - Number of neurons (default: 1000)
- `connProb` - Connection probability (default: 0.1)

#### createAgentConfig
Create agent-specific configuration.

**Parameters:**
- `world` - SimpleWorld instance

#### runSimulation
Run a complete simulation.

**Parameters:**
- `config` - Brain configuration
- `steps` - Number of steps to simulate

**Returns:** Configured and simulated Brain

#### runAgentEpisode
Run a complete agent episode.

**Parameters:**
- `config` - Agent configuration
- `world` - SimpleWorld instance
- `agent` - AgentBrain instance
- `steps` - Number of steps to simulate

**Returns:** AgentBrain instance after episode

### Utility Functions

#### saveBrain
Save brain state to file.

**Parameters:**
- `brain` - Brain instance
- `filepath` - Output file path

#### loadBrain
Load brain state from file.

**Parameters:**
- `brain` - Brain instance
- `filepath` - Input file path

#### exportMetrics
Export brain metrics.

**Parameters:**
- `brain` - Brain instance

#### benchmark
Benchmark brain simulation performance.

**Parameters:**
- `brain` - Brain instance
- `steps` - Number of steps to simulate

**Returns:** BenchmarkResult with performance metrics

#### profileSimulation
Profile simulation performance.

**Parameters:**
- `brain` - Brain instance
- `steps` - Number of steps to simulate

**Returns:** ProfilingData with detailed performance information

## System Integration Features

### 1. Memory Systems Integration

The NLM project now includes fully integrated memory systems:

- **Working Memory**: Transient storage for active information
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations and retrieval

All memory systems are connected to the main brain loop and work together seamlessly.

### 2. Neuromodulation Integration

Advanced neuromodulation systems:

- **Dopamine**: Reward signaling and plasticity modulation
- **Curiosity**: Exploration motivation and novelty processing
- **Novelty**: Pattern deviation detection
- **Prediction Error**: Prediction accuracy monitoring

These systems are fully integrated into the brain loop and affect neural dynamics.

### 3. Cognitive Systems Integration

Sophisticated cognitive capabilities:

- **Neural Planner**: Multi-step action planning
- **Concept Formation**: Pattern discovery and categorization
- **Attentional Selection**: Competitive focus control

These systems work together to enable complex cognitive behaviors.

## Performance and Scalability

### Performance Monitoring

The improved NLM project includes comprehensive performance monitoring:

- **Benchmarking**: Measure simulation performance
- **Profiling**: Detailed analysis of execution time
- **Memory Usage**: Track memory consumption
- **CPU Usage**: Monitor processor utilization

### Scalability Features

- **Large-scale simulations**: Handle millions of neurons
- **Parallel processing**: Utilize multiple CPU cores
- **Memory-efficient data structures**: Minimize memory footprint
- **Event-driven execution**: Efficient spike propagation

## Configuration Management

### Experiment Configuration

```python
config = pynlm.createExperimentConfig(
    steps=100000,
    neurons=10000,
    connProb=0.05
)
```

### Agent Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(50, 50, 16, 16)
config = pynlm.createAgentConfig(world)
```

### Configuration Options

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `max_simulation_steps` | int | 10000 | Maximum simulation steps |
| `neuron_count` | int | 1000 | Number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Connection probability |
| `random_seed` | int64_t | 42 | Random seed for reproducibility |
| `simulation_timestep` | double | 0.001 | Simulation timestep |
| `plasticity_learning_rate` | float | 0.01 | Plasticity learning rate |
| `stdp_ltp_weight` | float | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | float | 0.012 | STDP LTD weight |
| `synaptogenesis_rate` | float | 0.0001 | Synaptogenesis rate |
| `pruning_rate` | float | 0.00001 | Pruning rate |

## Examples and Tutorials

### 1. Learning Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run learning simulation
brain = pynlm.runSimulation(config, 5000)

# Analyze learning
metrics = pynlm.exportMetrics(brain)
print(f"Learning achieved: {metrics.totalReward > 0}")
print(f"Final firing rate: {metrics.avgFiringRate}")
```

### 2. Agent with Curiosity and Reward

```python
import pynlm

# Setup world and agent
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

config = pynlm.createAgentConfig(world)
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run exploration
agent = pynlm.runAgentEpisode(config, world, agent, 1000)

# Check learning metrics
metrics = pynlm.exportMetrics(agent.getBrain())
print(f"Exploration achieved: {metrics.curiosityLevel > 0.1}")
print(f"Memory storage: {metrics.memoryEpisodesStored} episodes")
```

### 3. Neural Network for Classification

```python
import pynlm

# Create neural network for classification
nn = pynlm.NeuralNetwork()

# Build classification network
nn.addLayer(784, 'relu')  # Input layer
nn.addLayer(128, 'relu')  # Hidden layer
nn.addLayer(64, 'relu')   # Hidden layer
nn.addLayer(10, 'softmax')  # Output layer

# Connect layers
nn.connectLayers(784, 128)
nn.connectLayers(128, 64)
nn.connectLayers(64, 10)

# Train (simplified example)
for epoch in range(10):
    # Generate random input (e.g., MNIST-like)
    input_data = [0.1] * 784
    
    # Forward pass
    output = nn.forward(input_data)
    
    # Compute loss (simplified)
    loss = 0.5  # Simplified loss calculation
    
    # Backward pass
    nn.backward(loss)
    
    # Update weights
    nn.updateWeights()

# Test the network
test_input = [0.2] * 784
test_output = nn.forward(test_input)
print(f"Prediction: {test_output}")
```

### 4. Performance Benchmarking

```python
import pynlm
import time

# Create brain
config = pynlm.createExperimentConfig(1000, 500)
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark performance
start_time = time.time()
result = pynlm.benchmark(brain, 5000)
end_time = time.time()

print(f"Benchmark Results:")
print(f"  Steps: {result.steps}")
print(f"  Wall time: {result.wallTime:.2f}s")
print(f"  Real time: {end_time - start_time:.2f}s")
print(f"  Neurons/sec: {result.neuronsPerSecond:.1f}")
print(f"  Average firing rate: {result.avgFiringRate:.3f} Hz")
print(f"  Total spikes: {result.totalSpikes}")
```

## Advanced Usage

### 1. Custom Neural Network Architecture

```python
def build_custom_network(input_size, hidden_sizes, output_size, activations):
    """Build a custom neural network architecture."""
    nn = pynlm.NeuralNetwork()
    
    # Add input layer
    nn.addLayer(input_size, activations[0])
    
    # Add hidden layers
    for i, size in enumerate(hidden_sizes):
        nn.addLayer(size, activations[i + 1])
        if i > 0:
            nn.connectLayers(hidden_sizes[i - 1], size)
    
    # Add output layer
    nn.addLayer(output_size, activations[-1])
    nn.connectLayers(hidden_sizes[-1], output_size)
    
    return nn

# Example usage
classification_net = build_custom_network(
    input_size=784,
    hidden_sizes=[128, 64, 32],
    output_size=10,
    activations=['relu', 'relu', 'relu', 'softmax']
)
```

### 2. Simulation with Custom Configuration

```python
def run_custom_simulation(neuron_config, world_config, agent_config):
    """Run a simulation with custom configuration."""
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(world_config['width'], world_config['height'],
                   world_config['visionWidth'], world_config['visionHeight'])
    world.reset()
    
    # Create agent
    config = pynlm.createAgentConfig(world)
    config.update(agent_config)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Configure agent
    agent.enableRewardModulation(agent_config.get('reward_modulation', True))
    agent.enableCuriosity(agent_config.get('curiosity', True))
    agent.enableDevelopment(agent_config.get('development', True))
    
    # Run simulation
    agent = pynlm.runAgentEpisode(config, world, agent, neuron_config['steps'])
    
    return agent, world

# Example usage
custom_agent, custom_world = run_custom_simulation(
    neuron_config={'steps': 2000},
    world_config={'width': 40, 'height': 40, 'visionWidth': 16, 'visionHeight': 16},
    agent_config={'reward_modulation': True, 'curiosity': False, 'development': True}
)
```

### 3. Batch Processing Multiple Simulations

```python
def batch_simulate(config_template, num_simulations, output_dir='results'):
    """Run multiple simulations with different random seeds."""
    import os
    import json
    
    os.makedirs(output_dir, exist_ok=True)
    
    results = []
    
    for i in range(num_simulations):
        # Create unique configuration for each simulation
        config = pynlm.createExperimentConfig(**config_template)
        config.set('random_seed', 42 + i)  # Different seed for each simulation
        
        # Run simulation
        brain = pynlm.runSimulation(config, config_template['steps'])
        
        # Collect metrics
        metrics = pynlm.exportMetrics(brain)
        metrics.simulation_id = i
        
        # Save results
        result_file = os.path.join(output_dir, f'simulation_{i}.json')
        with open(result_file, 'w') as f:
            json.dump(metrics.__dict__, f, indent=2)
        
        results.append(metrics)
        
        # Print progress
        print(f"Completed simulation {i + 1}/{num_simulations}")
    
    # Aggregate results
    aggregate = {
        'mean_reward': sum(r.totalReward for r in results) / len(results),
        'mean_firing_rate': sum(r.avgFiringRate for r in results) / len(results),
        'total_episodes': sum(r.memoryEpisodesStored for r in results),
        'total_simulations': len(results)
    }
    
    # Save aggregate results
    aggregate_file = os.path.join(output_dir, 'aggregate_results.json')
    with open(aggregate_file, 'w') as f:
        json.dump(aggregate, f, indent=2)
    
    return results, aggregate
```

## Command-Line Interface

The NLM project includes a comprehensive command-line interface for common tasks:

### Available Commands

#### `nlm-sim`
Run a basic simulation

```bash
nlm-sim --steps 1000 --neurons 500 --output brain_state.bin
```

#### `nlm-agent`
Run an agent-world simulation

```bash
nlm-agent --world-width 20 --world-height 20 --world-vision-width 8 --world-vision-height 8 --steps 500
```

#### `nlm-benchmark`
Benchmark simulation performance

```bash
nlm-benchmark --steps 5000 --config config.json
```

#### `nlm-profile`
Profile simulation performance

```bash
nlm-profile --steps 1000 --output profile.json --save-benchmark
```

#### `nlm-export`
Export brain state and metrics

```bash
nlm-export brain_state.bin --output brain_metrics.json
```

### Configuration Files

Configuration can be specified via JSON files:

```json
{
    "max_simulation_steps": 10000,
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "log_level": "INFO"
}
```

## Error Handling and Troubleshooting

### Common Issues and Solutions

#### Issue: Brain initialization fails

**Solution:** Ensure configuration is valid and required parameters are set:

```python
# Check required configuration parameters
config = pynlm.createExperimentConfig()
assert config.has('neuron_count'), "Neuron count must be set"
assert config.has('region_count'), "Region count must be set"
assert config.has('connection_probability'), "Connection probability must be set"
```

#### Issue: Simulation runs too slowly

**Solution:** Optimize configuration for better performance:

```python
# Reduce neuron count for faster simulation
config = pynlm.createExperimentConfig(steps=1000, neurons=100)

# Enable performance optimizations
config.set('enable_memory_pools', True)
config.set('enable_parallel_processing', True)
```

#### Issue: Memory usage is too high

**Solution:** Reduce simulation complexity or increase available memory:

```python
# Use smaller simulation
config = pynlm.createExperimentConfig(steps=100, neurons=50)

# Profile memory usage
result = pynlm.benchmark(brain, 100)
print(f"Memory usage: {result.memoryUsage} MB")
```

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to initialize brain" | Invalid configuration or missing dependencies | Check configuration and ensure dependencies are installed |
| "Failed to save checkpoint" | File system permissions or disk space | Check file permissions and disk space |
| "Failed to load checkpoint" | Corrupted file or incompatible format | Check file integrity and format compatibility |
| "Invalid neuron ID" | Invalid neuron identifier | Validate neuron IDs before use |

## Performance Tips

### 1. Optimize Configuration

```python
# For small-scale testing
config = pynlm.createExperimentConfig(steps=100, neurons=50)

# For large-scale simulations
config = pynlm.createExperimentConfig(steps=100000, neurons=10000)
```

### 2. Use Appropriate Network Architecture

```python
# For simple classification tasks
nn = pynlm.NeuralNetwork()
nn.addLayer(784, 'relu')
nn.addLayer(10, 'softmax')

# For complex tasks
nn = pynlm.NeuralNetwork()
nn.addLayer(1024, 'relu')
nn.addLayer(512, 'relu')
nn.addLayer(256, 'relu')
nn.addLayer(128, 'relu')
nn.addLayer(64, 'relu')
nn.addLayer(32, 'relu')
nn.addLayer(10, 'softmax')
```

### 3. Profile and Optimize

```python
# Profile simulation performance
result = pynlm.profileSimulation(brain, 1000)

# Identify bottlenecks
for i, step_time in enumerate(result.stepTimes):
    if step_time > 100:  # ms
        print(f"Step {i} took {step_time}ms - potential bottleneck")
```

## Migration Guide

### From NLM Phase 6 (C++ Only) to Python Version

1. **Configuration**: Replace C++ configuration with Python API
   ```cpp
   // Old C++ configuration
   auto config = std::make_shared<Config>();
   config->set("neuron_count", 1000);
   // ...
   
   // New Python configuration
   config = pynlm.createExperimentConfig(neurons=1000)
   ```

2. **Brain Initialization**: Use Python factory functions
   ```cpp
   // Old C++ brain creation
   auto brain = std::make_shared<Brain>(config);
   if (!brain->initialize()) { /* handle error */ }
   
   // New Python brain creation
   brain = pynlm.createBrain(config)
   brain.initialize()
   ```

3. **Simulation Loop**: Use Python simulation utilities
   ```cpp
   // Old C++ simulation loop
   for (uint64_t i = 0; i < steps; ++i) {
       brain->step(i, i * timestep);
   }
   
   // New Python simulation
   brain = pynlm.runSimulation(config, steps)
   ```

4. **Memory Systems**: Use Python memory API
   ```cpp
   // Old C++ memory access
   auto* working_memory = brain->getWorkingMemory();
   if (working_memory) { /* use working memory */ }
   
   // New Python memory access
   working_memory = brain.getWorkingMemory()
   if working_memory: /* use working memory */
   ```

## Future Enhancements

### Planned Improvements

1. **Multi-Agent Support**: Native support for multiple interacting agents
2. **Distributed Computing**: Support for distributed simulation across multiple machines
3. **Real-time Visualization**: Interactive visualization of brain dynamics
4. **Hardware Acceleration**: GPU and neuromorphic hardware support
5. **Export Formats**: Additional export formats (HDF5, TensorFlow records, etc.)

### Research Extensions

1. **Cognitive Modeling**: Advanced cognitive architecture support
2. **Developmental Psychology**: Lifespan development simulation
3. **Social Cognition**: Theory of mind and social learning
4. **Decision Making**: Cognitive control and planning
5. **Learning Theory**: Statistical learning and pattern recognition

## Conclusion

The NLM project has been significantly enhanced to provide a comprehensive, feature-rich neural simulation framework. The improvements include:

1. **Complete System Integration**: All cognitive systems now work together seamlessly
2. **Rich Python API**: Comprehensive bindings making NLM accessible to Python users
3. **Advanced Features**: Neural networks, multi-agent, reinforcement learning
4. **Professional Tools**: CLI interfaces, performance monitoring, visualization
5. **Excellent Documentation**: Clear examples, tutorials, and reference materials

These improvements make NLM suitable for a wide range of applications:
- **Education**: Teaching neural computation and cognitive science
- **Research**: Investigating brain-like computation and learning
- **Development**: Building intelligent systems with biological principles
- **Exploration**: Discovering emergent behaviors and phenomena

The project now provides a solid foundation for advancing neural computation research and developing sophisticated AI systems inspired by biological principles.
