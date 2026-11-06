# NLM - Neural Learning Machine Examples and Tutorials

## Welcome to NLM!

This document provides comprehensive examples and tutorials for getting started with the Neural Learning Machine (NLM) project. NLM is an experimental artificial developmental brain that implements real spiking neural computation with event-driven dynamics, plasticity, and memory systems.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Basic Python Examples](#basic-python-examples)
3. [C++ Examples](#cpp-examples)
4. [Configuration Examples](#configuration-examples)
5. [Tutorial: Building Your First Neural Network](#tutorial-building-your-first-neural-network)
6. [Tutorial: Learning with Plasticity](#tutorial-learning-with-plasticity)
7. [Tutorial: Working Memory and Episodic Memory](#tutorial-working-memory-and-episodic-memory)
8. [Advanced Topics](#advanced-topics)
9. [Troubleshooting](#troubleshooting)
10. [API Reference](#api-reference)

## Quick Start

### Install NLM

NLM is a C++/Python project. You'll need:

1. **C++ compiler** with C++17 support
2. **Python 3.8+** with pybind11
3. **CMake** for building

### Build Instructions

```bash
# Clone the repository
# git clone https://github.com/nlm-project/nlm.git
# cd nlm

# Build the project
mkdir build
cd build
cmake ..
make -j4

# Build Python bindings
# (Usually automatic with CMake, but sometimes needs explicit setup)
python3 -m pip install -e .
```

### Run Demo Programs

The project comes with several demo programs:

```bash
# Phase 2: Real neural computation demo (default)
./nlm

# Phase 3: World interaction demo
./nlm_phase3_demo

# Phase 4: Cognition demo
./nlm_phase4_demo

# Phase 6: Final integration demo
./nlm_phase6_demo

# Benchmark program
./nlm_benchmark
```

## Basic Python Examples

NLM includes comprehensive Python bindings (`pynlm`) that expose the full power of the neural simulation framework. Here are some basic examples:

### Example 1: Creating a Simple Brain

```python
import nlm

# Create a default configuration
config = nlm.createDefaultConfig()

# Customize configuration
config.set("neuron_count", 100)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Create brain
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Has {brain.getRegionCount()} regions")
print(f"Has {brain.getTotalSynapseCount()} synapses")
```

### Example 2: Simulating Neural Activity

```python
import nlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Simulate neural activity
print("Simulating neural activity...")
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} total spikes")

print(f"Final average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
```

### Example 3: Working with Memory Systems

```python
import nlm

# Create brain and world for memory examples
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Get working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    # Store some information
    pattern = [0.5, 0.8, 0.3, 0.9, 0.2]  # Example sensory pattern
    working_memory.store(pattern, strength=1.0)
    
    # Retrieve information
    retrieved = working_memory.retrieve()
    print(f"Stored and retrieved pattern: {retrieved}")
    
    # Get memory statistics
    print(f"Active memory traces: {working_memory.getActiveTraces()}")
    print(f"Memory neurons: {working_memory.getMemoryNeuronCount()}")
else:
    print("Working memory not available")
```

### Example 4: Neural Prediction and Planning

```python
import nlm
import nlm

# Create agent brain for prediction and planning
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Create a simple sensory state
sensory_state = [0.1, 0.5, 0.9, 0.3, 0.7]

# Process sensory input
agent.processSensoryInput(sensory_state)

# Get prediction
prediction = agent.getBrain().getPredictionSystem()
if prediction:
    predicted = prediction.predictNextState(sensory_state)
    print(f"Predicted next state: {predicted.getData()}")

# Plan an action
goal = [0.8, 0.2, 0.6, 0.4, 0.1]  # Example goal state
action = agent.planAction(goal)
print(f"Planned action: {action.getName()}")

# Get neural plan
planner = agent.getBrain().getPlanner()
if planner:
    candidate = planner.evaluateSequence([action.getType()], goal)
    print(f"Action quality: {candidate.expectedReward:.2f}, Confidence: {candidate.confidence:.2f}")
```

### Example 5: Neuromodulation and Learning

```python
import nlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Get neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

print("Neuromodulation System Status:")
if dopamine:
    print(f"  Dopamine level: {dopamine.getLevel():.3f}")
if curiosity:
    print(f"  Curiosity level: {curiosity.getLevel():.3f}")
if novelty:
    print(f"  Novelty level: {novelty.getLevel():.3f}")
if prediction_error:
    print(f"  Prediction error: {prediction_error.getValue():.3f}")

# Simulate learning with reward modulation
print("\nSimulating reward-modulated learning...")
for step in range(50):
    # Inject some sensory input
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 5.0)
    
    # Step the brain
    brain.step(step)
    
    # Apply reward modulation occasionally
    if step % 10 == 0:
        reward = 1.0 - (step % 10) * 0.1  # Decreasing reward
        prediction_error.applyRewardModulation(reward, 0.0)
    
    if step % 10 == 0:
        print(f"Step {step}: Average firing rate = {brain.getAverageFiringRate():.2f} Hz")
```

### Example 6: Memory Replay and Consolidation

```python
import nlm

# Create brain with memory capabilities
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Get episodic memory system
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # Create a test episode
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = 0
    episode.positionX = 5.0
    episode.positionY = 5.0
    episode.orientation = 0.0
    episode.action = nlm.ActionType::MoveForward
    episode.reward = 1.0
    episode.energy = 0.8
    episode.novelty = 0.3
    
    # Add some sensory and resulting states
    episode.sensoryState = [0.1, 0.2, 0.3, 0.4, 0.5]
    episode.resultingSensoryState = [0.2, 0.3, 0.4, 0.5, 0.6]
    
    # Add some active neurons
    for i in range(5):
        episode.activeNeurons.append(nlm.NeuronId(i))
        episode.neuronActivations.append(0.5)
    
    # Store the episode
    episodic_memory.storeEpisode(episode)
    
    # Simulate some time passes
    for step in range(10):
        brain.step(step)
    
    # Get similar episodes
    query_pattern = [0.15, 0.25, 0.35, 0.45, 0.55]
    similar_episodes = episodic_memory.retrieveSimilar(query_pattern, maxResults=3)
    
    print(f"Found {len(similar_episodes)} similar episodes")
    for i, ep in enumerate(similar_episodes):
        print(f"  Episode {i+1}: Position ({ep.positionX:.1f}, {ep.positionY:.1f}), Reward: {ep.reward:.1f}")
    
    # Enable replay and get episodes for replay
    episodic_memory.enableReplay(True)
    replay_episodes = episodic_memory.getEpisodesForReplay(2)
    print(f"Got {len(replay_episodes)} episodes for replay")
```

## C++ Examples

Here are some examples for using NLM in C++:

### Example 1: Basic Brain Simulation in C++

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <iostream>
#include <vector>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    std::cout << "Running brain simulation..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Inject some sensory input every 10 steps
        if (step % 10 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        }
    }
    
    // Report results
    std::cout << "Simulation completed:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Example 2: Memory System Usage in C++

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <nlm/memory/NeuralWorkingMemory.hpp>
#include <nlm/memory/NeuralEpisodicMemory.hpp>
#include <iostream>
#include <vector>

int main() {
    // Create brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Access working memory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working Memory System:" << std::endl;
        
        // Store a pattern
        std::vector<float> pattern = {0.5, 0.8, 0.3, 0.9, 0.2};
        workingMemory->store(pattern, 1.0f);
        
        // Retrieve the pattern
        std::vector<float> retrieved = workingMemory->retrieve();
        std::cout << "  Stored pattern: ";
        for (float val : pattern) std::cout << val << " ";
        std::cout << std::endl;
        
        std::cout << "  Retrieved pattern: ";
        for (float val : retrieved) std::cout << val << " ";
        std::cout << std::endl;
        
        // Get memory statistics
        std::cout << "  Active traces: " << workingMemory->getActiveTraces() << std::endl;
        std::cout << "  Capacity: " << workingMemory->getCapacity() << std::endl;
    }
    
    // Access episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "\nEpisodic Memory System:" << std::endl;
        
        // Create and store an episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.positionX = 1.0f;
        episode.positionY = 2.0f;
        episode.orientation = 0.5f;
        episode.action = nlm::ActionType::MoveForward;
        episode.reward = 1.0f;
        episode.energy = 0.8f;
        episode.novelty = 0.3f;
        episode.resultingReward = 0.9f;
        
        // Add some sensory data
        episode.sensoryState = {0.1f, 0.2f, 0.3f};
        episode.resultingSensoryState = {0.2f, 0.3f, 0.4f};
        
        episodicMemory->storeEpisode(episode);
        std::cout << "  Stored episode at position (" 
                  << episode.positionX << ", " << episode.positionY << ")" << std::endl;
        
        // Retrieve similar episodes
        std::vector<float> query = {0.15f, 0.25f, 0.35f};
        auto similar = episodicMemory->retrieveSimilar(query, 5);
        std::cout << "  Found " << similar.size() << " similar episodes" << std::endl;
    }
    
    return 0;
}
```

### Example 3: Plasticity and Learning in C++

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>
#include <nlm/plasticity/STDP.hpp>
#include <iostream>

int main() {
    // Create brain with plasticity enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Enable plasticity rules
    config->set("enable_stdp", true, nlm::ConfigSource::Default);
    config->set("enable_hebbian", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    if (stdp && hebbian) {
        std::cout << "Plasticity Systems:" << std::endl;
        
        // Configure STDP parameters
        stdp->configure(0.01f, 0.012f, 20.0f); // LTP, LTD, tau
        
        // Create correlated pre-post activity for STDP
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Get a synapse for testing
                auto synapses = region->getSynapses();
                if (!synapses.empty()) {
                    auto& synapse = synapses[0];
                    
                    // Enable STDP on this synapse
                    synapse->enablePlasticity(false, true, false);
                    
                    float initialWeight = synapse->getWeight();
                    std::cout << "Initial synapse weight: " << initialWeight << std::endl;
                    
                    // Apply correlated pre-post activity (potentiation)
                    for (int trial = 0; trial < 50; ++trial) {
                        // Pre-synaptic neuron fires
                        brain.injectCurrent(synapse->getSourceNeuron(), 60.0f);
                        brain.step(trial * 2);
                        
                        // Post-synaptic neuron fires
                        brain.injectCurrent(synapse->getDestinationNeuron(), 60.0f);
                        brain.step(trial * 2 + 1);
                    }
                    
                    float finalWeight = synapse->getWeight();
                    float delta = finalWeight - initialWeight;
                    
                    std::cout << "Final synapse weight: " << finalWeight << std::endl;
                    std::cout << "Weight change (Δ): " << delta << std::endl;
                    
                    if (delta > 0.001f) {
                        std::cout << "✓ STDP produced potentiation (learning occurred)" << std::endl;
                    } else {
                        std::cout << "! No significant weight change" << std::endl;
                    }
                }
            }
        }
    }
    
    return 0;
}
```

## Configuration Examples

NLM uses configuration files to control brain behavior. Here are some example configurations:

### Basic Configuration (`configs/default.cfg`)

```ini
# Basic NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

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

### Fast Configuration (`configs/fast.cfg`)

```ini
# Fast demo configuration for quick testing
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 200
region_count = 1
connection_probability = 0.2

# Reduced plasticity for faster convergence
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018

# Enable memory systems
working_memory_enabled = true
episodic_memory_enabled = true

# High logging for debugging
log_level = DEBUG
log_to_file = true
log_filename = nlm_fast.log
```

### Thorough Configuration (`configs/thorough.cfg`)

```ini
# Thorough configuration for comprehensive testing
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 3
connection_probability = 0.05

# Detailed plasticity settings
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
plasticity_learning_rate = 0.01

# Full development system
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
development_maturation_rate = 0.0005

# Complete neuromodulation
 dopamine_baseline = 0.1
acetylcholine_baseline = 0.05
curiosity_baseline = 0.1
novelty_baseline = 0.2

# Advanced memory systems
working_memory_capacity = 500
episodic_memory_max_episodes = 1000
associative_memory_enabled = true

# Comprehensive prediction system
prediction_system_enabled = true
prediction_horizon = 5

# Full cognition systems
neural_planner_enabled = true
concept_formation_enabled = true
attentional_selection_enabled = true

# Enable all plasticity rules
enable_stdp = true
enable_hebbian = true
enable_reward_modulated = true
enable_structural_plasticity = true

# Intensive logging
log_level = DEBUG
log_to_file = true
log_filename = nlm_thorough.log
```

## Tutorial: Building Your First Neural Network

This tutorial will walk you through creating your first NLM brain and simulating neural activity.

### Step 1: Create a Simple Brain

```python
import nlm

# Create a basic configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 50)  # Start with 50 neurons
config.set("region_count", 2)   # Use 2 regions
config.set("connection_probability", 0.1)

# Create the brain
brain = nlm.createBrain(config)

# Initialize the brain
if brain.initialize():
    print("✓ Brain created and initialized successfully!")
else:
    print("✗ Failed to initialize brain")
```

### Step 2: Explore Brain Structure

```python
# Get information about the brain
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Number of regions: {brain.getRegionCount()}")
print(f"Region IDs: {brain.getRegionIds()}")

# Get statistics for each region
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    if region:
        print(f"Region {region_id.value()}:")
        print(f"  Neurons: {region.getTotalNeuronCount()}")
        print(f"  Populations: {region.getPopulationCount()}")
        print(f"  Synapses: {region.getSynapseCount()}")
```

### Step 3: Simulate Neural Activity

```python
# Simulate neural activity
print("\nSimulating neural activity...")
for step in range(50):
    # Inject some sensory input every 10 steps
    if step % 10 == 0:
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 15.0)
        print(f"Step {step}: Injected sensory input")
    
    # Step the brain
    brain.step(step)
    
    # Print status every 10 steps
    if step % 10 == 0:
        print(f"Step {step}: "
              f"{brain.getTotalSpikeCount()} total spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getAverageFiringRate():.2f} Hz average firing rate")
```

### Step 4: Analyze Results

```python
# Analyze the simulation results
print("\nSimulation Results:")
print(f"Total simulation steps: {50}")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get working memory if available
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"\nWorking Memory:")
    print(f"  Active traces: {working_memory.getActiveTraces()}")
    print(f"  Capacity: {working_memory.getCapacity()}")
    
    # Store and retrieve some information
    pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
    working_memory.store(pattern, 1.0)
    retrieved = working_memory.retrieve()
    print(f"  Stored and retrieved pattern: {retrieved}")

# Get episodic memory if available
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"\nEpisodic Memory:")
    print(f"  Total episodes: {episodic_memory.getEpisodeCount()}")
    print(f"  Replay enabled: {episodic_memory.isReplayEnabled()}")
    
    # Show recent episodes
    recent = episodic_memory.getRecentEpisodes(3)
    print(f"  Recent episodes: {len(recent)}")

# Get neuromodulation levels
print(f"\nNeuromodulation Levels:")
dopamine = brain.getDopamine()
if dopamine:
    print(f"  Dopamine: {dopamine.getLevel():.3f}")

curiosity = brain.getCuriosity()
if curiosity:
    print(f"  Curiosity: {curiosity.getLevel():.3f}")

novelty = brain.getNovelty()
if novelty:
    print(f"  Novelty: {novelty.getLevel():.3f}")
```

## Tutorial: Learning with Plasticity

This tutorial demonstrates how to use NLM's plasticity systems for learning.

### Step 1: Setup Brain with Plasticity

```python
import nlm

# Create brain with plasticity enabled
config = nlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 1)
config.set("connection_probability", 0.1)

# Enable all plasticity rules
config.set("enable_stdp", True)
config.set("enable_hebbian", True)
config.set("enable_reward_modulated", True)

brain = nlm.createBrain(config)
brain.initialize()

print("Brain created with plasticity systems:")
print(f"  STDP enabled: {brain.getSTDP() is not None}")
print(f"  Hebbian enabled: {brain.getHebbian() is not None}")
```

### Step 2: Demonstrate STDP Learning

```python
# Get STDP system
stdp = brain.getSTDP()
if stdp:
    print("\nDemonstrating STDP (Spike-Timing-Dependent Plasticity)...")
    
    # Configure STDP parameters
    stdp.configure(0.01, 0.012, 20.0)  # LTP, LTD, tau
    
    # Create a correlated pre-post activity pattern
    print("Applying correlated pre-post activity (should produce potentiation)...")
    
    # For 50 trials, fire pre-synaptic neuron, then post-synaptic neuron
    for trial in range(50):
        # Pre-synaptic spike
        brain.injectCurrentToNeurons(nlm.NeuronType::Sensory, 60.0)
        brain.step(trial * 2)
        
        # Post-synaptic spike  
        brain.injectCurrentToNeurons(nlm.NeuronType::Internal, 60.0)
        brain.step(trial * 2 + 1)
        
        if trial % 10 == 0:
            print(f"  Trial {trial}: Current STDP weight = {stdp.getCurrentWeight():.4f}")
    
    print("✓ STDP learning demonstrated successfully!")
```

### Step 3: Demonstrate Hebbian Learning

```python
# Get Hebbian system
hebbian = brain.getHebbian()
if hebbian:
    print("\nDemonstrating Hebbian Learning...")
    
    # Configure Hebbian parameters
    hebbian.configure(0.005, 0.0, 0.95)  # learning_rate, ltd_weight, decay
    
    # Create simultaneous pre-post activity
    print("Applying simultaneous pre-post activity (should produce strengthening)...")
    
    for trial in range(30):
        # Simultaneous pre and post activity
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 40.0)
        brain.injectCurrentToNeurons(nlm.NeuronType.Internal, 40.0)
        brain.step(trial)
        
        if trial % 10 == 0:
            print(f"  Trial {trial}: Current Hebbian weight = {hebbian.getCurrentWeight():.4f}")
    
    print("✓ Hebbian learning demonstrated successfully!")
```

### Step 4: Reward-Modulated Learning

```python
# Get reward system
prediction_error = brain.getPredictionErrorSignal()
if prediction_error:
    print("\nDemonstrating Reward-Modulated Learning...")
    
    # Configure reward parameters
    prediction_error.configure(0.1, 0.9, 0.95)  # learning_rate, discount, decay
    
    # Simulate learning with rewards
    print("Applying reward signals...")
    
    for episode in range(20):
        # Reset brain for new episode
        brain.reset()
        brain.initialize()
        
        # Some random activity
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0)
        brain.step(episode)
        
        # Apply reward (decreasing over time)
        reward = 1.0 - (episode * 0.03)
        prediction_error.applyRewardModulation(reward, 0.0)
        
        print(f"  Episode {episode}: Reward = {reward:.2f}")
    
    print("✓ Reward-modulated learning demonstrated successfully!")
```

### Step 5: Analyze Learning Results

```python
# Analyze the learning results
print("\nLearning Analysis:")

# Get final statistics
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Check working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory active traces: {working_memory.getActiveTraces()}")
    print(f"Working memory capacity: {working_memory.getCapacity()}")

# Check episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")

print("\n✓ Plasticity systems learning demonstration complete!")
```

## Tutorial: Working Memory and Episodic Memory

This tutorial demonstrates NLM's advanced memory systems.

### Step 1: Setup Memory Systems

```python
import nlm

# Create brain with memory capabilities
config = nlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("region_count", 1)
config.set("working_memory_enabled", True)
config.set("episodic_memory_enabled", True)
config.set("associative_memory_enabled", True)

brain = nlm.createBrain(config)
brain.initialize()

print("Memory Systems Initialized:")
print(f"  Working Memory: {brain.getWorkingMemory() is not None}")
print(f"  Episodic Memory: {brain.getEpisodicMemory() is not None}")
print(f"  Associative Memory: {brain.getAssociativeMemory() is not None}")
```

### Step 2: Working Memory Tutorial

```python
# Working memory stores information temporarily using neural activity patterns
working_memory = brain.getWorkingMemory()
if working_memory:
    print("\nWorking Memory Tutorial:")
    
    # Set working memory capacity
    working_memory.setCapacity(100)
    print(f"Set working memory capacity: {working_memory.getCapacity()}")
    
    # Store different patterns
    print("\nStoring patterns...")
    
    # Pattern 1: Sensory input
    pattern1 = [0.1, 0.8, 0.3, 0.9, 0.2, 0.5, 0.7, 0.4, 0.6, 0.8]
    working_memory.store(pattern1, strength=1.0)
    print("  Stored pattern 1")
    
    # Pattern 2: Internal state
    pattern2 = [0.9, 0.2, 0.7, 0.1, 0.8, 0.3, 0.6, 0.4, 0.9, 0.2]
    working_memory.store(pattern2, strength=0.7)
    print("  Stored pattern 2")
    
    # Store directly to specific neurons
    print("\nStoring to specific neurons...")
    for i in range(5):
        working_memory.storeToNeuron(nlm.NeuronId(i), 0.5 + i * 0.1)
        print(f"  Stored activation {0.5 + i * 0.1:.1f} to neuron {i}")
    
    # Retrieve patterns
    print("\nRetrieving patterns...")
    retrieved1 = working_memory.retrieve()
    print(f"  Retrieved pattern 1: {retrieved1}")
    
    # Get neuron activations
    print("\nChecking neuron activations...")
    for i in range(5):
        activation = working_memory.getNeuronActivation(nlm.NeuronId(i))
        print(f"  Neuron {i} activation: {activation:.3f}")
    
    # Get working memory statistics
    print("\nWorking Memory Statistics:")
    print(f"  Active traces: {working_memory.getActiveTraces()}")
    print(f"  Memory neurons: {working_memory.getMemoryNeuronCount()}")
    print(f"  Memory activity level: {working_memory.getMemoryActivity():.3f}")
    
    # Update working memory
    print("\nUpdating working memory...")
    for step in range(10):
        working_memory.update(0.001)  # Small time step
        if step % 5 == 0:
            print(f"  Step {step}: Memory activity = {working_memory.getMemoryActivity():.3f}")
```

### Step 3: Episodic Memory Tutorial

```python
# Episodic memory stores experiences for later recall
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print("\nEpisodic Memory Tutorial:")
    
    # Configure episodic memory
    episodic_memory.setMaxEpisodes(1000)
    episodic_memory.enableReplay(True)
    
    print(f"Set max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"Replay enabled: {episodic_memory.isReplayEnabled()}")
    
    # Create and store episodes
    print("\nCreating and storing episodes...")
    
    for episode_num in range(3):
        # Create episode
        episode = nlm.EpisodicMemoryItem()
        episode.timestamp = episode_num * 100
        episode.positionX = 5.0 + episode_num * 2.0
        episode.positionY = 5.0 + episode_num * 1.5
        episode.orientation = episode_num * 0.5
        
        # Set action based on episode
        if episode_num == 0:
            episode.action = nlm.ActionType::MoveForward
        elif episode_num == 1:
            episode.action = nlm.ActionType::TurnLeft
        else:
            episode.action = nlm.ActionType::Look
        
        # Set reward (decreases for later episodes)
        episode.reward = 1.0 - episode_num * 0.2
        
        # Set energy (varies)
        episode.energy = 0.7 + episode_num * 0.1
        
        # Set novelty (higher for new experiences)
        episode.novelty = 0.3 + episode_num * 0.2
        
        # Add sensory states
        episode.sensoryState = [0.1 * (episode_num + 1), 0.2 * (episode_num + 1), 
                              0.3 * (episode_num + 1), 0.4 * (episode_num + 1), 
                              0.5 * (episode_num + 1)]
        
        # Add resulting sensory states
        episode.resultingSensoryState = [0.15 * (episode_num + 1), 0.25 * (episode_num + 1),
                                       0.35 * (episode_num + 1), 0.45 * (episode_num + 1),
                                       0.55 * (episode_num + 1)]
        
        # Add active neurons
        for i in range(3):
            episode.activeNeurons.append(nlm.NeuronId(i + episode_num * 10))
            episode.neuronActivations.append(0.6 + episode_num * 0.1)
        
        # Set age
        episode.age = episode_num * 50
        
        # Store episode
        episodic_memory.storeEpisode(episode)
        
        print(f"  Stored episode {episode_num + 1}:")
        print(f"    Position: ({episode.positionX:.1f}, {episode.positionY:.1f})")
        print(f"    Action: {episode.action}")
        print(f"    Reward: {episode.reward:.1f}")
        print(f"    Novelty: {episode.novelty:.1f}")
    
    # Retrieve similar episodes
    print("\nRetrieving similar episodes...")
    query_pattern = [0.15, 0.3, 0.45, 0.6, 0.75]
    similar_episodes = episodic_memory.retrieveSimilar(query_pattern, maxResults=2)
    
    print(f"Found {len(similar_episodes)} similar episodes:")
    for i, ep in enumerate(similar_episodes):
        print(f"  Episode {i + 1}:")
        print(f"    Timestamp: {ep.timestamp}")
        print(f"    Position: ({ep.positionX:.1f}, {ep.positionY:.1f})")
        print(f"    Action: {ep.action}")
        print(f"    Reward: {ep.reward:.1f}")
        print(f"    Novelty: {ep.novelty:.1f}")
    
    # Retrieve by time
    print("\nRetrieving episodes by time...")
    temporal_episodes = episodic_memory.retrieveTemporal(50, 200, maxResults=3)
    print(f"Found {len(temporal_episodes)} episodes between steps 50-200")
    
    # Retrieve by action
    print("\nRetrieving episodes by action...")
    action_episodes = episodic_memory.retrieveAfterAction(nlm.ActionType::MoveForward, maxResults=2)
    print(f"Found {len(action_episodes)} episodes after MoveForward action")
    
    # Get episodic memory statistics
    print("\nEpisodic Memory Statistics:")
    print(f"  Total episodes: {episodic_memory.getEpisodeCount()}")
    print(f"  Average reward: {episodic_memory.getAverageReward():.3f}")
    print(f"  Replay enabled: {episodic_memory.isReplayEnabled()}")
    
    # Get episodes for replay
    replay_episodes = episodic_memory.getEpisodesForReplay(2)
    print(f"  Episodes for replay: {len(replay_episodes)}")
```

### Step 4: Associative Memory Tutorial

```python
# Associative memory creates connections between related experiences
associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print("\nAssociative Memory Tutorial:")
    
    # Create pattern pairs
    print("\nCreating pattern associations...")
    
    # Pattern A: Sensory input
    pattern_a = [0.1, 0.2, 0.3, 0.4, 0.5]
    
    # Pattern B: Related internal state
    pattern_b = [0.6, 0.7, 0.8, 0.9, 1.0]  # Higher values = related to A
    
    # Create strong association
    associative_memory.associate(pattern_a, pattern_b, strength=1.0)
    print("  Created strong association between pattern A and B")
    
    # Create another association
    pattern_c = [0.2, 0.4, 0.6, 0.8, 1.0]
    pattern_d = [0.1, 0.3, 0.5, 0.7, 0.9]  # Less related to A
    
    associative_memory.associate(pattern_c, pattern_d, strength=0.5)
    print("  Created moderate association between pattern C and D")
    
    # Test association retrieval
    print("\nTesting association retrieval...")
    
    # Retrieve patterns associated with pattern A
    retrieved_from_a = associative_memory.retrieve(pattern_a, maxResults=3)
    print(f"  Patterns associated with A: {len(retrieved_from_a)}")
    
    # Retrieve patterns associated with pattern C  
    retrieved_from_c = associative_memory.retrieve(pattern_c, maxResults=3)
    print(f"  Patterns associated with C: {len(retrieved_from_c)}")
    
    # Test spreading activation
    print("\nTesting spreading activation...")
    
    # Start with pattern A
    activation_spread = associative_memory.spreadActivation(pattern_a, steps=2)
    print(f"  Patterns activated through spreading from A: {len(activation_spread)}")
    
    # Show some spread results
    for i, pattern in enumerate(activation_spread[:3]):
        print(f"    Pattern {i+1}: {[f'{x:.2f}' for x in pattern]}")
    
    # Test association strength
    print("\nTesting association strength...")
    
    strength_ab = associative_memory.getAssociationStrength(pattern_a, pattern_b)
    strength_cd = associative_memory.getAssociationStrength(pattern_c, pattern_d)
    strength_ac = associative_memory.getAssociationStrength(pattern_a, pattern_c)  # Should be weak
    
    print(f"  Association strength A-B: {strength_ab:.3f}")
    print(f"  Association strength C-D: {strength_cd:.3f}")
    print(f"  Association strength A-C: {strength_ac:.3f} (should be weak)")
    
    # Get associative memory statistics
    print("\nAssociative Memory Statistics:")
    print(f"  Total associations: {associative_memory.getAssociationCount()}")
```

## Advanced Topics

### Working with the Python API

#### Creating Custom Configurations

```python
import nlm
import json

# Create a custom configuration
config_dict = {
    "neuron_count": 500,
    "region_count": 3,
    "connection_probability": 0.05,
    "stdp_ltp_weight": 0.015,
    "stdp_ltd_weight": 0.018,
    "stdp_tau": 25.0,
    "enable_working_memory": True,
    "enable_episodic_memory": True,
    "enable_associative_memory": True,
    "enable_prediction_system": True,
    "enable_planning_system": True,
}

# Create config from dictionary
config = nlm.createDefaultConfig()
for key, value in config_dict.items():
    if key.startswith("enable_"):
        # Handle boolean flags
        if value:
            config.set(key.replace("enable_", ""), "true")
    else:
        # Handle regular values
        config.set(key, str(value))

# Create brain with custom config
brain = nlm.createBrain(config)
brain.initialize()
```

#### Batch Operations

```python
import nlm

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Batch operation: inject current to multiple neurons
neuron_ids = [nlm.NeuronId(i) for i in range(10)]
for neuron_id in neuron_ids:
    brain.injectCurrent(neuron_id, 20.0)

# Batch operation: get multiple neuron states
states = []
for neuron_id in neuron_ids:
    state = brain.getNeuronState(neuron_id)
    if state:
        states.append(state)

print(f"Collected states from {len(states)} neurons")
```

#### Monitoring and Debugging

```python
import nlm
import time

# Create brain with monitoring
config = nlm.createDefaultConfig()
config.set("log_level", "DEBUG")
brain = nlm.createBrain(config)
brain.initialize()

# Enable monitoring
monitor_start = time.time()
step_count = 0

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    step_count += 1
    
    # Log progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: "
              f"{brain.getTotalSpikeCount()} total spikes, "
              f"{brain.getAverageFiringRate():.2f} Hz average rate")

# Print monitoring summary
monitor_end = time.time()
print(f"\nMonitoring Summary:")
print(f"  Total steps: {step_count}")
print(f"  Duration: {monitor_end - monitor_start:.2f} seconds")
print(f"  Steps per second: {step_count / (monitor_end - monitor_start):.1f}")
print(f"  Final total spikes: {brain.getTotalSpikeCount()}")
print(f"  Final average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Working with Checkpoints

```python
import nlm
import os

# Create brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step)

# Save checkpoint
checkpoint_file = "test_brain_checkpoint.bin"
if brain.save(checkpoint_file):
    print(f"✓ Checkpoint saved to {checkpoint_file}")
    
    # Get checkpoint size
    if os.path.exists(checkpoint_file):
        size = os.path.getsize(checkpoint_file)
        print(f"  Checkpoint size: {size} bytes")
else:
    print("✗ Failed to save checkpoint")

# Create a new brain and load checkpoint
new_brain = nlm.createBrain(config)
new_brain.initialize()

if new_brain.load(checkpoint_file):
    print("✓ Checkpoint loaded successfully")
    
    # Verify loaded state
    print(f"  Loaded total neurons: {new_brain.getTotalNeuronCount()}")
    print(f"  Loaded total synapses: {new_brain.getTotalSynapseCount()}")
else:
    print("✗ Failed to load checkpoint")

# Clean up
if os.path.exists(checkpoint_file):
    os.remove(checkpoint_file)
    print(f"Cleaned up checkpoint file")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: "Failed to initialize brain"

**Possible causes:**
- Insufficient system resources
- Invalid configuration values
- Missing dependencies

**Solutions:**
1. Check system resources (memory, CPU)
2. Verify configuration values are valid
3. Ensure all dependencies are installed
4. Check for configuration file syntax errors

#### Issue: Memory usage grows too high

**Possible causes:**
- Memory systems not properly managed
- Unbounded growth in episodic memory
- Working memory capacity not set appropriately

**Solutions:**
1. Set appropriate memory capacities:
```python
working_memory.setCapacity(100)
episodic_memory.setMaxEpisodes(1000)
```

2. Enable memory consolidation:
```python
episodic_memory.consolidate(0.5)  # Consolidate episodes with relevance < 0.5
```

3. Enable replay for consolidation:
```python
episodic_memory.enableReplay(True)
```

#### Issue: Performance is too slow

**Possible causes:**
- Too many neurons or synapses
- Complex simulation steps
- Inefficient operations

**Solutions:**
1. Reduce network size:
```python
config.set("neuron_count", 100)  # Instead of 1000
```

2. Use faster simulation:
```python
# Use smaller time steps
config.set("simulation_timestep", 0.0005)
```

3. Optimize operations:
- Use batch operations
- Reduce logging level
- Disable unnecessary systems

#### Issue: Python bindings not working

**Possible causes:**
- Pybind11 compilation issues
- Missing Python dependencies
- Incorrect installation

**Solutions:**
1. Reinstall Python bindings:
```bash
pip install --force-reinstall -e .
```

2. Check Pybind11 installation:
```bash
pip list | grep pybind11
```

3. Check Python bindings compilation:
```bash
python3 -c "import nlm; print('Import successful')"
```

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Configuration key cannot be empty" | Empty key in config | Check config file for empty keys |
| "Invalid configuration value" | Wrong type or value | Check config file syntax and types |
| "Memory capacity exceeded" | Working memory full | Increase capacity or clear memory |
| "File not found" | Missing configuration file | Ensure file exists or provide default |
| "Invalid neuron ID" | Reference to non-existent neuron | Check neuron IDs before use |

## API Reference

This section provides detailed API documentation for the NLM Python bindings.

### Core Brain API

#### `createDefaultConfig()`
```python
config = nlm.createDefaultConfig()
```
Creates a default configuration object.

#### `createBrain(config)`
```python
brain = nlm.createBrain(config)
```
Creates a brain with the given configuration.

#### `Brain.initialize()`
```python
brain.initialize()
```
Initializes the brain with the current configuration.

#### `Brain.step(step)`
```python
brain.step(step)
```
Performs one simulation step.

#### `Brain.injectCurrent(neuron_id, current)`
```python
brain.injectCurrent(neuron_id, current)
```
Injects current into a specific neuron.

#### `Brain.injectCurrentToNeurons(neuron_type, current)`
```python
brain.injectCurrentToNeurons(neuron_type, current)
```
Injects current into all neurons of a specific type.

### Memory Systems API

#### Working Memory

```python
working_memory = brain.getWorkingMemory()
working_memory.store(pattern, strength=1.0)
working_memory.storeToNeuron(neuron_id, activation)
retrieved = working_memory.retrieve()
working_memory.clear()
working_memory.setCapacity(capacity)
working_memory.setDecayRate(rate)
```

#### Episodic Memory

```python
episodic_memory = brain.getEpisodicMemory()
episodic_memory.storeEpisode(episode)
retrieved = episodic_memory.retrieveSimilar(query_pattern, max_results)
retrieved = episodic_memory.retrieveTemporal(start_time, end_time)
retrieved = episodic_memory.retrieveByLocation(x, y, radius)
retrieved = episodic_memory.retrieveAfterAction(action)
episodic_memory.replayEpisode(episode)
episodic_memory.enableReplay(enable)
```

#### Associative Memory

```python
associative_memory = brain.getAssociativeMemory()
associative_memory.associate(pattern_a, pattern_b, strength=1.0)
associative_memory.associateFromExperience(episode)
retrieved = associative_memory.retrieve(query_pattern)
associative_memory.spreadActivation(cue_pattern, steps=2)
```

### Prediction and Planning API

#### Prediction System

```python
prediction_system = brain.getPredictionSystem()
if prediction_system:
    predicted = prediction_system.predictNextState(current_state)
    prediction_system.updatePredictions(predicted, actual)
    error = prediction_system.getPredictionError()
    confidence = prediction_system.getConfidence()
```

#### Neural Planner

```python
planner = brain.getPlanner()
if planner:
    action = planner.planAction(current_state, target_reward=0.5)
    candidate = planner.evaluateSequence([action_type], current_state)
    planner.updatePlanQuality(planned_actions, actual_actions, reward)
```

### Neuromodulation API

#### Dopamine System

```python
dopamine = brain.getDopamine()
if dopamine:
    level = dopamine.getLevel()
    prediction_error = dopamine.getPredictionError()
    dopamine.applyModulation()
```

#### Curiosity System

```python
curiosity = brain.getCuriosity()
if curiosity:
    level = curiosity.getLevel()
    curiosity.update()
```

#### Novelty System

```python
novelty = brain.getNovelty()
if novelty:
    level = novelty.getLevel()
    novelty.update(current_state)
```

### Configuration API

#### Config Operations

```python
config = nlm.createDefaultConfig()
config.set("key", "value")
config.set("neuron_count", 100)
config.set("random_seed", 42)
value = config.get("key")
default_value = config.getOr("key", "default")
config.has("key")
config.remove("key")
config.clear()
config.loadFromFile("file.cfg")
config.loadFromArgs(args)
config.saveToFile("output.cfg")
```

### Agent Brain API

```python
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
sensory_input = agent.processSensoryInput(percept)
motor_command = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
```

## License and Attribution

NLM is licensed under the MIT License. See the LICENSE file for details.

## Further Reading

For more information about NLM, visit the official documentation:
- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [NLM Research Papers](https://github.com/nlm-project/nlm/papers)

## Acknowledgements

This project builds on decades of research in computational neuroscience and artificial intelligence. Special thanks to the open source community for their contributions.

---

*This documentation is continuously updated. For the latest information, please check the official repository.*

---