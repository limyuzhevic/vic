# NLM Enhanced Python Bindings - Examples

This file contains comprehensive examples demonstrating the enhanced NLM Python bindings.

## Installation

```bash
pip install pynlm
# Or build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e .
```

## Quick Start

### Example 1: Basic Neural Simulation

```python
import pynlm as nlm

# Create default configuration
config = nlm.create_default_config()
print(f"Configuration: {config}")

# Create neural brain
brain = nlm.create_brain(config)
brain.initialize()
print(f"Brain initialized with {brain.getRegionCount()} regions")

# Simulate for 10 steps
for i in range(10):
    brain.step(i)
    print(f"Step {i}: {brain.getActiveNeuronCount()} active neurons, "
          f"{brain.getFiringNeuronCount()} firing neurons")

# Produce action
action = brain.produce_action()
print(f"Produced action: {action.getName()}")
```

### Example 2: Complete Agent-Environment Integration

```python
import pynlm as nlm

# Create complete agent-environment system
config, world, brain, agent = nlm.create_complete_agent()

print(f"World: {world}")
print(f"Agent brain: {agent}")

# Run an episode
episode_stats = nlm.run_learning_episode(
    agent, world, 
    max_steps=100, 
    verbose=True
)

print(f"Episode completed:")
print(f"  Total reward: {episode_stats['total_reward']:.2f}")
print(f"  Time steps: {episode_stats['steps']}")
print(f"  Final simulation time: {episode_stats['time']:.2f}")
print(f"  Action distribution: {episode_stats['action_count']}")
```

### Example 3: Advanced Memory Systems

```python
import pynlm as nlm

# Create brain and world
config, world, brain, agent = nlm.create_complete_agent()

# Create and initialize working memory
working_memory = nlm.NeuralWorkingMemory()
working_memory.initialize(brain)

# Store sensory patterns
vision_pattern = world.getSensoryPercept().getVision()
working_memory.store(vision_pattern, strength=1.0)

# Store specific neuron activation
neuron_id = brain.addRegion("memory_test")
working_memory.storeToNeuron(neuron_id, activation=0.8)

# Retrieve and analyze memory
retrieved_pattern = working_memory.retrieve()
print(f"Working memory pattern shape: {len(retrieved_pattern)} elements")
print(f"Memory neurons: {working_memory.getMemoryNeurons()}")
print(f"Memory activity: {working_memory.getMemoryActivity():.3f}")

# Update working memory (maintenance and decay)
working_memory.update(0.1)
print(f"Memory after update: {working_memory.getMemoryActivity():.3f}")
```

### Example 4: Neuromodulation Systems

```python
import pynlm as nlm

# Create brain
config, world, brain, agent = nlm.create_complete_agent()

# Set up neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Get neuromodulation components
brain_ptr = agent.getBrain()
dopamine = brain_ptr.getDopamine()
curiosity = brain_ptr.getCuriosity()
novelty = brain_ptr.getNovelty()
prediction_error = brain_ptr.getPredictionErrorSignal()

print(f"Neuromodulation levels:")
print(f"  Dopamine: {dopamine.getLevel():.3f}")
print(f"  Curiosity: {curiosity.getLevel():.3f}")
print(f"  Novelty: {novelty.getLevel():.3f}")
print(f"  Prediction Error: {prediction_error.getLevel():.3f}")

# Update neuromodulation with sensory input
percept = world.getSensoryPercept()
novelt_y = novelle.update(percept, 0.1)

# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

### Example 5: NumPy Integration

```python
import pynlm as nlm
import numpy as np

# Create brain and world
config, world, brain, agent = nlm.create_complete_agent()

# Get vision data from world
percept = world.getSensoryPercept()
vision_data = percept.getVision()

# Convert to NumPy array (automatic conversion)
vision_array = nlm.to_numpy(vision_data)
print(f"Vision array shape: {vision_array.shape}")
print(f"Vision array type: {type(vision_array)}")

# NumPy operations on vision data
vision_mean = np.mean(vision_array)
vision_std = np.std(vision_array)
vision_max = np.max(vision_array)

print(f"Vision statistics: mean={vision_mean:.3f}, std={vision_std:.3f}, max={vision_max:.3f}")

# Create new vision data using NumPy
new_vision = np.random.rand(20, 20) * 0.1
vision = nlm.Vision(20, 20)
vision.setData(new_vision.flatten().tolist())

# Convert back to NumPy
new_vision_array = nlm.to_numpy(vision.getData())
print(f"New vision shape: {new_vision_array.shape}")
```

### Example 6: Development and Plasticity

```python
import pynlm as nlm

# Create brain
config, world, brain, agent = nlm.create_complete_agent()

# Enable plasticity
agent.enableStructuralPlasticity(True)

# Get plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()

print("Plasticity systems initialized:")
print(f"  STDP learning rate: {stdp.getLearningRate()}")
print(f"  Hebbian learning rate: {hebbian.getLearningRate()}")

# Update development system
agent.updateDevelopment(0.1)
print(f"Developmental stage: {agent.getDevelopmentalStage()}")

# Get developmental systems
dev_system = brain.getDevelopmentSystem()
print(f"Development age: {dev_system.getAge()}")
print(f"Plasticity rate: {dev_system.getPlasticityRate()}")
```

### Example 7: Complete Experiment Framework

```python
import pynlm as nlm

# Create experiment
experiment = nlm.NLMExperiment(
    name="Curiosity_Driven_Learning",
    config_dict={
        "brain/neurons": 500,
        "brain/regions": 5,
        "brain/development.enabled": True,
        "brain/neuromodulation.dopamine": True,
        "brain/neuromodulation.curiosity": True,
    }
)

# Run experiment
metrics = experiment.run(max_steps=500)
print(f"Experiment '{experiment.name}' completed:")
print(f"  Total reward: {metrics.get('total_reward', 0):.2f}")
print(f"  Steps: {metrics.get('steps', 0)}")
print(f"  Time: {metrics.get('time', 0):.2f}")

# Save results
experiment.save_results("experiment_results.json")
print("Results saved to experiment_results.json")

# Analyze results
print("\nAction analysis:")
for action, count in metrics.get('action_count', {}).items():
    print(f"  {action}: {count}")
```

### Example 8: Advanced Memory and Prediction

```python
import pynlm as nlm

# Create brain
config, world, brain, agent = nlm.create_complete_agent()

# Set up prediction system
prediction_system = brain.getPredictionSystem()
prediction_system.initialize(brain)

# Set up cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Create concepts from experience
percept = world.getSensoryPercept()
vision_data = percept.getVision()
concept_formation.learnConcept(vision_data, "red_objects")
concept_formation.learnConcept([0.1, 0.2, 0.3], "simple_pattern")

print(f"Learned concepts: {concept_formation.getAllConcepts()}")

# Apply concept to new pattern
new_pattern = [0.15, 0.25, 0.35]
concept_match = concept_formation.applyConcept("simple_pattern", new_pattern)
print(f"Concept application result: {concept_match}")

# Run attention selection
competitors = [brain.addRegion("region1"), brain.addRegion("region2")]
winners = attention.processCompetition(competitors)
print(f"Attention winners: {winners}")
```

### Example 9: Visualization and Analysis

```python
import pynlm as nlm

# Create environment
config, world, brain, agent = nlm.create_complete_agent()

# Create visualizer
visualizer = nlm.NLMVisualizer()

# Run a few simulation steps
for i in range(5):
    percept = world.getSensoryPercept()
    visualizer.plot_vision(percept, f"Step {i} Vision")
    world.update(0.1)

# Analyze neural activity
analysis = nlm.analyze_neural_activity(brain, steps=20)
print(f"Neural activity analysis:")
print(f"  Average active neurons: {analysis['average_active_neurons']:.1f}")
print(f"  Average firing patterns: {analysis['average_firing_patterns']:.1f}")
print(f"  E/I ratio: {analysis['excitatory_inhibitory_ratio']:.3f}")

# Create video (if OpenCV available)
# visualizer.save_to_video(world, "simulation.mp4", fps=10, duration=5)
```

### Example 10: Reproducible Research

```python
import pynlm as nlm
import random

# Create reproducible simulation with fixed seed
seed = 42
config, world, brain, agent = nlm.create_reproducible_simulation(seed)

print(f"Reproducible simulation with seed: {seed}")

# Run multiple experiments with same seed
for i in range(3):
    config2, world2, brain2, agent2 = nlm.create_reproducible_simulation(seed)
    
    # Run episode
    stats = nlm.run_learning_episode(agent2, world2, max_steps=50)
    
    print(f"Experiment {i+1}: Reward={stats['total_reward']:.2f}, "
          f"Steps={stats['steps']}")

print("All experiments are reproducible due to fixed seed!")
```

## Advanced Usage

### Custom Configuration

```python
import pynlm as nlm

# Custom configuration
config_dict = {
    "brain/neurons": 1000,
    "brain/regions": 10,
    "brain/development.enabled": True,
    "brain/neuromodulation.dopamine": True,
    "brain/neuromodulation.curiosity": True,
    "brain/neuromodulation.novelty": True,
    "brain/neuromodulation.prediction_error": True,
    "simulation/timestep": 0.05,
    "simulation/max_steps": 2000,
}

config, world, brain, agent = nlm.create_complete_agent(config_dict)
```

### Batch Processing

```python
import pynlm as nlm
import numpy as np

def run_batch_experiment(num_experiments=10, steps_per_episode=100):
    """Run multiple experiments in batch."""
    results = []
    
    for i in range(num_experiments):
        config, world, brain, agent = nlm.create_complete_agent()
        
        # Run episode
        stats = nlm.run_learning_episode(agent, world, steps_per_episode)
        
        # Add experiment metadata
        stats['experiment_id'] = i
        stats['seed'] = i + 42  # Deterministic variation
        
        results.append(stats)
        
        print(f"Experiment {i+1}/{num_experiments} completed")
    
    # Calculate batch statistics
    total_rewards = [r['total_reward'] for r in results]
    avg_reward = np.mean(total_rewards)
    std_reward = np.std(total_rewards)
    
    print(f"Batch results:")
    print(f"  Average reward: {avg_reward:.2f} ± {std_reward:.2f}")
    print(f"  Reward range: {min(total_rewards):.2f} - {max(total_rewards):.2f}")
    
    return results

# Run batch experiment
batch_results = run_batch_experiment(5, 100)
```

### Performance Benchmarking

```python
import pynlm as nlm
import time

def benchmark_all_systems(brain, iterations=100):
    """Benchmark different neural systems."""
    print(f"Benchmarking {iterations} iterations...")
    
    # Brain step benchmark
    start_time = time.time()
    for i in range(iterations):
        brain.step(i)
    brain_time = time.time() - start_time
    
    # Memory system benchmark
    working_memory = nlm.NeuralWorkingMemory()
    working_memory.initialize(brain)
    
    start_time = time.time()
    for i in range(iterations):
        working_memory.update(0.1)
        pattern = working_memory.retrieve()
    memory_time = time.time() - start_time
    
    # Neuromodulation benchmark
    dopamine = brain.getDopamine()
    
    start_time = time.time()
    for i in range(iterations):
        dopamine.update(0.5, 0.1, 0.1)
    neuromod_time = time.time() - start_time
    
    print(f"Benchmark results ({iterations} iterations):")
    print(f"  Brain steps: {brain_time:.3f}s ({brain_time/iterations*1000:.1f}ms per step)")
    print(f"  Memory updates: {memory_time:.3f}s ({memory_time/iterations*1000:.1f}ms per update)")
    print(f"  Neuromodulation updates: {neuromod_time:.3f}s ({neuromod_time/iterations*1000:.1f}ms per update)")
    
    return {
        'brain_time': brain_time,
        'memory_time': memory_time,
        'neuromod_time': neuromod_time,
        'brain_per_step': brain_time/iterations,
        'memory_per_update': memory_time/iterations,
        'neuromod_per_update': neuromod_time/iterations,
    }

# Run benchmark
config = nlm.create_default_config()
brain = nlm.create_brain(config)
brain.initialize()

benchmark_results = benchmark_all_systems(brain, 50)
```

## Installation Issues

### NumPy Installation

```bash
# Install NumPy for enhanced features
pip install numpy

# For enhanced visualization
pip install matplotlib
pip install opencv-python
```

### Build from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .
```

### Troubleshooting

If you encounter compilation issues:

1. **Missing dependencies**:
```bash
# Install required system packages
apt-get update
apt-get install build-essential cmake python3-dev
```

2. **Python binding issues**:
```bash
# Try building with verbose output
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3
make VERBOSE=1
```

3. **NumPy integration**:
```python
# Check if NumPy is working
import pynlm as nlm
status = nlm.validate_installation()
print("Installation status:", status)
```

## API Reference

### Core Classes

- **Config**: Configuration management for NLM systems
- **Brain**: Central neural simulation brain
- **AgentBrain**: Agent interface for brain-world interaction
- **SimpleWorld**: 2D world for NLM development

### Memory Systems

- **NeuralWorkingMemory**: Working memory using neural dynamics
- **NeuralEpisodicMemory**: Episodic memory for experience storage
- **NeuralAssociativeMemory**: Associative memory for pattern associations

### Prediction and Cognition

- **PredictionSystem**: Sensory prediction and error computation
- **NeuralPrediction**: Neural prediction module
- **NeuralPlanner**: Action planning based on neural activity
- **ConceptFormation**: Pattern discovery and concept learning
- **AttentionalSelection**: Competitive attention mechanism

### Neuromodulation

- **Dopamine**: Reward and reinforcement processing
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **PredictionError**: Prediction error signal

### Development and Plasticity

- **DevelopmentSystem**: Neural development and learning
- **Synaptogenesis**: Synapse formation during development
- **Maturation**: Neural maturation and specialization
- **Pruning**: Neural pruning and elimination
- **STDP**: Spiking-based spike timing dependent plasticity
- **Hebbian**: Hebbian plasticity rule
- **StructuralPlasticity**: Structural connectivity changes

### Sensory and Motor

- **Vision**: Vision sensory input
- **Audio**: Audio sensory input
- **InternalSignals**: Internal homeostatic signals
- **SensoryInput**: Base class for sensory inputs
- **Action**: Action representation for motor output
- **AgentBody**: Agent physical properties and state

### Utilities

- **NLMExperiment**: High-level experiment framework
- **NLMVisualizer**: Visualization utilities
- **create_demo_environment**: Create demo environment
- **run_episode**: Run complete episode
- **benchmark_performance**: Performance benchmarking
- **validate_configuration**: Configuration validation

## License

This enhanced Python bindings are licensed under the MIT License. The original NLM project is also under the MIT License.

For more information about the NLM project, visit: https://github.com/nlm-project/nlm

## Version

Enhanced Python bindings version: 2.0.0
NLM core version: 0.1.0

## Notes

1. **Performance Considerations**: The enhanced bindings provide a balance between ease of use and performance. For maximum performance, consider using the native C++ API directly.

2. **Memory Management**: The bindings use smart pointers and reference counting for automatic memory management.

3. **Thread Safety**: The NLM framework is not thread-safe by default. Use appropriate synchronization if running multiple threads.

4. **Debugging**: Enable verbose logging by setting appropriate configuration options.

5. **Dependencies**: Some features require additional Python packages (NumPy, matplotlib, OpenCV). These are optional dependencies.

## Getting Help

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Examples**: Check the examples directory in the repository

Enjoy exploring the enhanced NLM Python bindings! 🚀