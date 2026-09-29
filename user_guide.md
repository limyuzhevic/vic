# NLM User Guide
# Complete Documentation for Neural Learning Machine

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that simulates neural processes and learning. This guide provides comprehensive documentation for using NLM in various scenarios, from basic testing to advanced research applications.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Getting Started with Python API](#getting-started-with-python-api)
3. [Advanced Tutorials](#advanced-tutorials)
4. [Research Applications](#research-applications)
5. [Performance Optimization](#performance-optimization)
6. [Troubleshooting](#troubleshooting)
7. [API Reference](#api-reference)
8. [Configuration Management](#configuration-management)

---

## Quick Start Guide

### 1. Basic Installation

#### System Requirements
- C++20 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher (for Python bindings)
- Optional: pybind11 development headers

#### Build Instructions

```bash
# Clone the repository
mkdir nlm && cd nlm
# Clone from your repository here

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### 2. Python Package Installation

```bash
# Install Python dependencies
pip install pybind11

# Build Python bindings
pip install -e .

# Test installation
python -c "import pynlm; print('NLM imported successfully')"
```

### 3. Running Basic Tests

```bash
# Navigate to build directory
cd build

# Run basic tests
./test_all

# Run Python tests
python -m pytest tests/  # If Python tests exist
```

---

## Getting Started with Python API

### 1. Simple Brain Simulation

```python
import pynlm

# Create a basic brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Run basic simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
```

### 2. Advanced Brain with Agent Setup

```python
import pynlm
import time

# Create a complete agent setup
brain, world, agent = pynlm.createAgentSetup(
    world_width=20,
    world_height=20,
    vision_width=8,
    vision_height=8
)

# Configure simulation
config = pynlm.SimulationConfig(
    max_steps=500,
    enable_reward_modulation=True,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)

# Define callback functions
def step_callback(step, stats):
    if step % 50 == 0:
        print(f"Step {step}: {stats.firing_neurons} firing neurons, "
              f"firing rate: {stats.average_firing_rate:.2f} Hz")

def reward_callback(reward):
    if reward > 0:
        print(f"Received reward: {reward}")

# Run simulation with callbacks
print("Starting advanced simulation...")
result = pynlm.runSimulation(
    brain, world, agent, config,
    step_callback=step_callback,
    reward_callback=reward_callback
)

print(f"\nSimulation Results:")
print(f"Total steps: {result.total_steps}")
print(f"Final spike count: {result.final_spike_count}")
print(f"Total reward: {result.total_reward}")
print(f"Learning detected: {result.learning_detected}")
```

### 3. Multi-Modal Sensory Processing

```python
import pynlm
import numpy as np

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 16, 16)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create multi-modal input
vision = pynlm.Vision(16, 16, 3)
audio = pynlm.Audio(44100, 1024)
internal = pynlm.InternalSignals()

# Set realistic sensory data
vision_data = np.random.randn(16, 16, 3).astype(np.float32) * 0.1
vision.setData(vision_data.flatten())

audio_data = np.random.randn(1024).astype(np.float32) * 0.05
audio.setData(audio_data)

internal.addSignal(0.5)
internal.addSignal(0.3)
internal.addSignal(0.8)

# Process multi-modal input
input_multi = pynlm.MultiModalInput(
    visual_features=vision_data.flatten(),
    auditory_features=audio_data,
    proprioceptive_features=np.array([0.1, 0.2, 0.3]),
    internal_signals=internal.getData(),
    tactile_features=np.array([0.9, 0.7, 0.5]),
    modality="experiment",
    attention_weight=0.8
)

# Process through agent
agent.processSensoryInput(input_multi)

# Run brain simulation
for step in range(50):
    world.update(0.1)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Multi-modal processing simulation completed.")
```

### 4. Learning and Development

```python
import pynlm

# Create brain with advanced features
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(25, 25, 12, 12)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
print("Starting learning and development simulation...")
for episode in range(20):
    # Reset environment
    world.reset()
    
    # Run episode
    for step in range(100):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action with neuromodulation
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(1.0, 0.5)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Print episode status
    if (episode + 1) % 5 == 0:
        print(f"Episode {episode + 1}: "
              f"Firing neurons: {brain.getFiringNeuronCount()}, "
              f"Total spikes: {brain.getTotalSpikeCount()}, "
              f"Development stage: {brain.getDevelopmentalStage()}")

print("Learning and development simulation completed.")
```

### 5. Transfer Learning

```python
import pynlm

# Create multiple task knowledge
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain1.initialize()

brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Store task knowledge
brain1.save("task1.knowledge")
brain2.save("task2.knowledge")

# Create transfer learning system
config = pynlm.createAdvancedConfig()
transfer_system = nlm.createTransferLearningSystem(config)

# Transfer knowledge from task1 to task2
transfer_system.loadKnowledge("task1", brain1.getKnowledge())
transfer_system.transferKnowledge("task1", "task2", transfer_strength=0.7f)

# Load transferred knowledge for task2
transfered_knowledge = transfer_system.getTaskKnowledge("task2")
brain2.setKnowledge(transferred_knowledge)

# Run task2 with transferred knowledge
for step in range(100):
    brain2.step(step)

print("Transfer learning simulation completed.")
```

---

## Advanced Tutorials

### 1. Meta-Learning Tutorial

```python
import pynlm

# Create brain with meta-learning enabled
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Create meta-learning system
meta_params = pynlm.MetaLearningParams(
    learning_rate_adaptation=0.1f,
    exploration_rate_adaptation=0.05f,
    novelty_threshold=0.5f,
    uncertainty_threshold=0.3f,
    transfer_strength=0.7f,
    generalization_factor=0.8f,
    meta_learning_rate=0.01f,
    enable_meta_learning=True
)

meta_system = pynlm.createMetaLearningSystem(meta_params)

# Simulate meta-learning
for step in range(200):
    # Get task performance
    task_performance = simulateTaskPerformance(step)
    
    # Get current loss
    current_loss = calculateCurrentLoss(brain, step)
    
    # Update meta-learning system
    meta_system.update(task_performance, current_loss)
    
    # Print meta-learning state
    if step % 50 == 0:
        print(meta_system.getMetaLearningState())
        print(f"Current learning rate: {meta_system.getCurrentLearningRate()}")
        print(f"Exploration rate: {meta_system.getExplorationRate()}")
        print(f"Uncertainty: {meta_system.getUncertainty()}")
```

### 2. Multi-Modal Learning Tutorial

```python
import pynlm
import numpy as np

# Create multi-modal learning system
multi_modal_system = pynlm.createMultiModalLearningSystem()

# Create training data
training_data = []
for episode in range(10):
    # Generate multi-modal input
    input = generateMultiModalInput(episode)
    training_data.append(input)

# Train the system
multi_modal_system.train(training_data, learning_rate=0.01f)

# Test the system
test_input = generateMultiModalInput(100)
processed_input = multi_modal_system.processMultiModalInput(test_input)

print(f"Original modality: {input.modality}")
print(f"Processed modality: {processed_input.modality}")
print(f"Attention weight: {processed_input.attention_weight}")
print(f"Integration quality: {multi_modal_system.getIntegrationQuality()}")
```

### 3. Advanced Plasticity Tutorial

```python
import pynlm

# Create advanced plasticity system
plasticity_params = pynlm.AdvancedPlasticityParams(
    stdp_learning_rate=0.01f,
    hebbian_learning_rate=0.02f,
    reward_modulated_learning_rate=0.03f,
    homeostatic_learning_rate=0.001f,
    structural_plasticity_rate=0.0001f,
    metaplasticity_strength=0.1f,
    eligibility_trace_decay=0.95f,
    dopamine_modulation_factor=1.0f,
    curiosity_modulation_factor=1.0f,
    novelty_modulation_factor=1.0f,
    enable_advanced_stdp=True,
    enable_homeostatic_plasticity=True,
    enable_metaplasticity=True
)

plasticity_system = pynlm.createAdvancedPlasticitySystem(plasticity_params)

# Create synaptic state
synaptic_state = AdvancedSynapticState()
synaptic_state.weight = 0.5f

# Simulate plasticity updates
for step in range(100):
    # Generate spike data
    pre_spikes = generateSpikeData(10)
    post_spikes = generateSpikeData(10)
    
    # Update synaptic state
    plasticity_system.updateSynapticState(
        synaptic_state, pre_spikes, post_spikes,
        reward=1.0f, prediction_error=0.1f,
        novelty=0.5f, dopamine=0.8f, curiosity=0.3f,
        current_time=step * 0.01f
    )
    
    # Print plasticity stats periodically
    if step % 50 == 0:
        print(plasticity_system.getPlasticityStats())
        print(f"Current synaptic strength: {plasticity_system.calculateSynapticStrength(synaptic_state)}")
```

---

## Research Applications

### 1. Cognitive Architecture Research

```python
import pynlm

# Create advanced brain for cognitive research
brain = pynlm.createBrain(pynlm.createResearchConfig())
brain.initialize()

# Create advanced experiment system
experiment_config = pynlm.createExperimentConfig(
    num_experiments=20,
    max_steps_per_experiment=500,
    enable_advanced_features=True
)

experiment_system = pynlm.createAdvancedExperimentSystem(experiment_config)

# Run cognitive research experiments
experiment_system.runAdvancedExperiments()

# Analyze results
results = experiment_system.getExperimentResults()
for i, result in enumerate(results):
    print(f"Experiment {i + 1}: {result}")
```

### 2. Neuromodulation Research

```python
import pynlm

# Create brain with emphasis on neuromodulation
brain = pynlm.createBrain(pynlm.createNeuromodulationConfig())
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(40, 40, 20, 20)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Focus on neuromodulation aspects
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Run neuromodulation research simulation
for episode in range(15):
    world.reset()
    
    for step in range(100):
        world.update(0.05f)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action with neuromodulation
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record neuromodulation effects
        recordNeuromodulationEffects(brain, step)
    
    print(f"Episode {episode + 1}: "
          f"Avg firing rate: {brain.getAverageFiringRate():.2f} Hz, "
          f"Neuromodulation level: {getNeuromodulationLevel(brain):.2f}")
```

### 3. Memory Systems Research

```python
import pynlm

# Create brain optimized for memory research
brain = pynlm.createBrain(pynlm.createMemoryResearchConfig())
brain.initialize()

# Set up memory recording
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Enable memory features
working_memory->setCapacity(1000);
npisodic_memory->setMaxEpisodes(1000);

# Run memory research simulation
for episode in range(10):
    # Store initial memory state
    initial_state = recordMemoryState(brain)
    
    # Run simulation
    for step in range(200):
        brain.step(step)
        
        # Record episodic memory
        if step % 20 == 0:
            recordEpisodicMemory(brain, step);
        
        # Record working memory
        recordWorkingMemory(brain, step);
        
        # Record associative memory
        recordAssociativeMemory(brain, step);
    
    # Compare memory states
    final_state = recordMemoryState(brain)
    analyzeMemoryChanges(initial_state, final_state);

# Print memory statistics
print(f"Working memory traces: {working_memory->getActiveTraces()}")
print(f"Episodic memory episodes: {episodic_memory->getEpisodeCount()}")
```

---

## Performance Optimization

### 1. Memory Optimization

```python
import pynlm

# Create brain with memory optimization
brain = pynlm.createBrain(pynlm.createMemoryOptimizedConfig())
brain.initialize()

# Enable memory pooling
enableMemoryPooling = True

# Set optimization parameters
optimization_params = {
    "memory_pool_size": 10000,
    "enable_compression": True,
    "enable_optimization": True,
    "threading_level": 4
}

# Apply optimizations
applyMemoryOptimizations(brain, optimization_params)

# Run performance test
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Performance: {1000 / (end_time - start_time):.0f} steps/second")
print(f"Memory usage: {getMemoryUsage(brain)} MB")
```

### 2. Parallel Processing

```python
import pynlm
import threading
import concurrent.futures

# Create brain with parallel processing
brain = pynlm.createBrain(pynlm.createParallelConfig())
brain.initialize()

# Create parallel processing system
parallel_system = pynlm.createParallelProcessingSystem(
    num_threads=4,
    load_balancing=True,
    work_stealing=True
)

# Run parallel simulation
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = []
    for step in range(100):
        future = executor.submit(processStep, brain, step)
        futures.append(future)
    
    # Wait for all steps to complete
    concurrent.futures.wait(futures)
    
    # Collect results
    results = [future.result() for future in futures]

print(f"Parallel processing completed. "
      f"Processed {len(results)} steps with {threading.active_count()} threads")
```

---

## Troubleshooting

### 1. Common Issues and Solutions

#### Issue: "Cannot initialize brain"

**Solution:**
- Check if configuration file is valid
- Ensure all required libraries are installed
- Verify system requirements are met
- Try running with debug mode enabled

```python
# Enable debug logging
brain = pynlm.createBrain(pynlm.createDebugConfig())
brain.enableDebug(True)
brain.initialize()
```

#### Issue: "Simulation performance is poor"

**Solution:**
- Enable performance optimizations
- Reduce simulation complexity
- Use parallel processing
- Monitor memory usage

```python
# Enable performance optimizations
optimization_config = pynlm.getPerformanceOptimizedConfig()
brain = pynlm.createBrain(optimization_config)
```

#### Issue: "Memory usage is too high"

**Solution:**
- Enable memory pooling
- Reduce simulation duration
- Use compression techniques
- Monitor memory leaks

```python
# Enable memory optimization
memory_config = pynlm.createMemoryOptimizedConfig()
brain = pynlm.createBrain(memory_config)
```

### Args and Error Handling

```python
def safe_simulation(brain, max_steps):
    try:
        for step in range(max_steps):
            brain.step(step)
        return True
    except Exception as e:
        print(f"Simulation error: {e}")
        return False

# Run safe simulation
if safe_simulation(brain, 1000):
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

---

## API Reference

### Core Classes

#### Brain
- `Brain(config)`: Create new brain
- `brain.initialize()`: Initialize brain
- `brain.step(step)`: Process simulation step
- `brain.receiveSensoryInput(input)`: Inject sensory input
- `brain.produceAction()`: Generate motor action
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

#### Config
- `Config()`: Create default configuration
- `config.set(key, value)`: Set configuration value
- `config.get(key)`: Get configuration value
- `config.loadFromFile(filepath)`: Load from file
- `config.loadFromArgs()`: Load from command line

#### AgentBrain
- `AgentBrain(brain)`: Create agent brain interface
- `agent.initialize(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode motor command
- `agent.applyRewardModulation(reward, predicted)`: Apply reward modulation

#### SimpleWorld
- `SimpleWorld()`: Create simple world
- `world.configure(width, height, vision_width, vision_height)`: Configure world
- `world.reset()`: Reset world
- `world.update(timestep)`: Update world
- `world.applyMotorCommand(action, time)`: Apply motor command

### Advanced Classes

#### MetaLearningSystem
- `MetaLearningSystem(params)`: Create meta-learning system
- `system.update(performance, loss)`: Update meta-learning state
- `system.getCurrentLearningRate()`: Get current learning rate
- `system.getExplorationRate()`: Get exploration rate
- `system.getUncertainty()`: Get uncertainty estimate

#### TransferLearningSystem
- `TransferLearningSystem(transfer_strength)`: Create transfer learning system
- `system.storeTaskKnowledge(task_id, knowledge)`: Store task knowledge
- `system.getTaskKnowledge(task_id)`: Get task knowledge
- `system.transferKnowledge(source, target)`: Transfer knowledge
- `system.calculateTaskSimilarity(task1, task2)`: Calculate task similarity

#### MultiModalLearningSystem
- `MultiModalLearningSystem()`: Create multi-modal learning system
- `system.processMultiModalInput(input)`: Process multi-modal input
- `system.train(data, learning_rate)`: Train system
- `system.getIntegrationQuality()`: Get integration quality

#### AdvancedPlasticitySystem
- `AdvancedPlasticitySystem(params)`: Create advanced plasticity system
- `system.updateSynapticState(state, ...)`: Update synaptic state
- `system.calculateSynapticStrength(state)`: Calculate synaptic strength
- `system.getPlasticityStats()`: Get plasticity statistics

### Configuration Types

#### SimulationConfig
- `max_steps`: Maximum simulation steps
- `timestep`: Simulation timestep
- `agent_width`: Agent world width
- `agent_height`: Agent world height
- `enable_reward_modulation`: Enable reward modulation
- `enable_structural_plasticity`: Enable structural plasticity
- `enable_development`: Enable development
- `enable_curiosity`: Enable curiosity

#### LearningMetrics
- `weight_change_rate`: Rate of weight changes
- `strengthened_synapses`: Number of strengthened synapses
- `weakened_synapses`: Number of weakened synapses
- `learning_efficiency`: Learning efficiency metric
- `plasticity_strength`: Overall plasticity strength

#### NeuralStats
- `total_neurons`: Total number of neurons
- `total_synapses`: Total number of synapses
- `firing_neurons`: Number of firing neurons
- `active_neurons`: Number of active neurons
- `average_firing_rate`: Average firing rate
- `excitation_inhibition_ratio`: Excitation/inhibition ratio

---

## Configuration Management

### Configuration File Format

NLM uses a simple key=value format for configuration files:

```ini
# NLM Configuration File
# Generated on: 2024-01-01 12:00:00 UTC
# Total entries: 10

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0
curiosity_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Configuration Templates

#### Default Configuration
```python
config = pynlm.createDefaultConfig()
```

#### Minimal Configuration
```python
config = pynlm.createMinimalConfig()
```

#### Research Configuration
```python
config = pynlm.createResearchConfig()
```

#### Performance Optimized Configuration
```python
config = pynlm.createPerformanceOptimizedConfig()
```

#### Memory Optimized Configuration
```python
config = pynlm.createMemoryOptimizedConfig()
```

### Configuration Validation

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
is_valid = config.isValid()
validation_errors = config.validate()

if not is_valid:
    print("Configuration errors:")
    for error in validation_errors:
        print(f"  - {error}")
```

### Configuration Merging

```python
# Create base configuration
base_config = pynlm.createDefaultConfig()
base_config.set("neuron_count", 1000)

# Create override configuration
override_config = pynlm.createDefaultConfig()
override_config.set("neuron_count", 2000)
override_config.set("learning_rate", 0.1f)

# Merge configurations
base_config.merge(override_config, ConfigSource::Override)

# Use merged configuration
brain = pynlm.createBrain(base_config)
```

---

## Conclusion

NLM provides a comprehensive platform for neural simulation and research. This guide provides comprehensive documentation for using NLM in various scenarios, from basic testing to advanced research applications.

Key strengths of NLM include:

1. **Modular Architecture**: Clear separation of concerns for different neural systems
2. **Advanced Learning**: Support for meta-learning, transfer learning, and multi-modal learning
3. **Performance**: Optimized for high-performance simulations
4. **Extensibility**: Easy to add new neural models and learning rules
5. **Research-Grade**: Designed for scientific research and experimentation

For questions not covered in this documentation, please refer to the NLM project documentation or contact the development team.

---

*Last updated: $(date +%Y-%m-%d)*
*Version: 1.0.0*