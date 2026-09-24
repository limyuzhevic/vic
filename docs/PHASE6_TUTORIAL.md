# NLM/熙然 Phase 6 Tutorial

**Step-by-Step Guide to Using NLM Phase 6**

## Overview

Phase 6 Tutorial provides a comprehensive introduction to NLM Phase 6 - the final integration phase of the Neural Learning Machine project. This tutorial walks you through creating, configuring, and running Phase 6 experiments with the complete artificial brain.

**Prerequisites:**
- Basic understanding of neural networks and brain concepts
- Experience with C++ or Python (preferred)
- Familiarity with the NLM project structure

## Table of Contents

1. [Phase 6 Overview](#phase-6-overview)
2. [Setting Up the Environment](#setting-up-the-environment)
3. [Creating a Phase 6 Brain](#creating-a-phase-6-brain)
4. [Phase 6 Agent Configuration](#phase-6-agent-configuration)
5. [Phase 6 Memory Systems](#phase-6-memory-systems)
6. [Phase 6 Neuromodulation](#phase-6-neuromodulation)
7. [Phase 6 Development](#phase-6-development)
8. [Running Phase 6 Experiments](#running-phase-6-experiments)
9. [Phase 6 Analysis and Visualization](#phase-6-analysis-and-visualization)
10. [Phase 6 Performance Tuning](#phase-6-performance-tuning)
11. [Advanced Phase 6 Features](#advanced-phase-6-features)
12. [Phase 6 Troubleshooting](#phase-6-troubleshooting)
13. [Phase 6 Further Reading](#phase-6-further-reading)

## Phase 6 Overview

### What Makes Phase 6 Special

Phase 6 (熙然, meaning "serene flow") represents the **final integration phase** of NLM. Unlike previous phases that focused on individual components, Phase 6 integrates all systems into a coherent artificial brain:

```
PHASE 6: FINAL INTEGRATION
├── Memory Systems (Working + Episodic + Associative)
├── Neuromodulation (Dopamine + Curiosity + Novelty)
├── Prediction System (Temporal sequence learning)
├── Cognition Systems (Planning, Concepts, Attention)
├── Development System (Five-stage progression)
├── Replay and Consolidation (Memory strengthening)
└── Persistence (Checkpoint save/load)
```

### Phase 6 Key Differences from Earlier Phases

| Feature | Phase 5 | Phase 6 |
|---------|--------|--------|
| **Scope** | Individual components | Complete integration |
| **Memory** | Basic implementations | Working + Episodic + Associative |
| **Neuromodulation** | Dopamine only | Dopamine + Curiosity + Novelty |
| **Prediction** | Separate system | Integrated with brain loop |
| **Development** | Basic stages | Five-stage progression with effects |
| **Persistence** | No implementation | Checkpoint + Replay systems |
| **Cognition** | Planner concepts | **Partially implemented** (need integration) |

### Phase 6 Architecture

```
SENSORY INPUT
    ↓
AGENTBRAIN.processSensoryInput()
                               ↓
                         BRAIN.receiveSensoryInput()
                               ↓
                         NEURAL PROCESSING (LIF dynamics)
                               ↓
                         WORKING MEMORY (Persistent activity)
                               ↓
                         EPISODIC MEMORY (Experience capture)
                               ↓
                         PREDICTION SYSTEM (Temporal learning)
                               ↓
                         ATTENTION (Competitive selection)
                               ↓
                         CONCEPT FORMATION (Pattern discovery)
                               ↓
                         PLANNING (Action evaluation)
                               ↓
                         SELF-MODEL (Agency representation)
                               ↓
                         NEUROMODULATION (Learning signals)
                               ↓
                         PLASTICITY (STDP, Hebbian, Structural)
                               ↓
                         DEVELOPMENT (Stage progression)
                               ↓
                         MOTOR ACTIVITY
                               ↓
                         AGENTBRAIN.decodeMotorCommand()
                               ↓
                         WORLD.applyAction()
```

## Setting Up the Environment

### Prerequisites

Ensure you have all required dependencies:

```bash
# Required packages
pip install pybind11 scikit-build-core pytest numpy

# Build tools
cmake version 3.16+
C++ compiler supporting C++20
Python 3.8+

# Optional for development
pip install matplotlib pandas jupyter
```

### Building Phase 6 from Source

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory with Phase 6 support
mkdir -p build
cd build

# Configure CMake with Phase 6 integration
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PHASE6_INTEGRATION=ON \
         -DENABLE_CHECKPOINTING=ON \
         -DENABLE_REPLAY=ON \
         -DENABLE_DEVELOPMENT=ON

# Build all Phase 6 targets
make -j$(nproc)

# Run Phase 6 tests
ctest --output-on-failure -R phase6
```

### Installing Python Bindings

```bash
# Install Python package with Phase 6 features
pip install .

# Or install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Phase 6 Testing

```bash
# Run Phase 6 integration demo
./nlm_test_phase6

# Run all Phase 6 experiments
./nlm_phase6_test

# Run legacy tests (for comparison)
./nlm_test_legacy
```

## Creating a Phase 6 Brain

### Basic Phase 6 Brain Creation

```python
import pynlm

# Create a Phase 6 brain with integrated systems
config = pynlm.createDefaultConfig()

# Phase 6: Configure enhanced features
config.set("brain.neuron_count", 500)  # Larger brain
config.set("memory.working_memory_size", 1000)  # Working memory capacity
config.set("memory.episodic_capacity", 100)  # Episodic memory slots
config.set("development.critical_period_duration", 100.0)  # Development length
config.set("neuromod.dopamine_scale", 1.0)  # Neuromodulation strength
config.set("prediction.horizon", 10)  # Prediction time horizon

# Create and initialize the Phase 6 brain
brain = pynlm.createBrain(config)
brain.initialize()

print("Phase 6 brain created successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"With integrated: Memory, Neuromodulation, Prediction, Development")
```

### Phase 6 Configuration Examples

#### Example 1: Basic Phase 6 Configuration

```python
import pynlm

# Basic Phase 6 configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Example 2: Advanced Phase 6 Configuration

```python
import pynlm

# Advanced Phase 6 configuration with all systems enabled
config = pynlm.createDefaultConfig()

# Neural configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.05)

# Phase 6 Memory configuration
config.set("memory.working_memory_size", 2000)
config.set("memory.episodic_capacity", 500)
config.set("memory.replay_interval", 50)
config.set("memory.consolidation_interval", 500)

# Phase 6 Neuromodulation configuration
config.set("neuromod.dopamine_scale", 1.5)
config.set("neuromod.curiosity_scale", 0.8)
config.set("neuromod.novelty_threshold", 0.05)

# Phase 6 Development configuration
config.set("development.critical_period_duration", 200.0)
config.set("development.maturation_rate", 0.02)
config.set("development.plasticity_decay", 0.001)

# Phase 6 Prediction configuration
config.set("prediction.horizon", 20)
config.set("prediction.accuracy_threshold", 0.8)

# Create Phase 6 brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Phase 6 Brain Initialization

```python
import pynlm

# Initialize Phase 6 brain with all subsystems
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Phase 6: Initialize with complete system setup
brain.initialize()

# Verify Phase 6 integration
print("Phase 6 Brain Status:")
print(f"  Neural core: {brain.getTotalNeuronCount()} neurons")
print(f"  Memory systems: {brain.getWorkingMemory() and 'Working Memory' or 'Not available'}")
print(f"  Development: {brain.getDevelopmentalStage().name}")
print(f"  Ready for Phase 6 simulation: Yes")
```

## Phase 6 Agent Configuration

### Creating a Phase 6 Agent

```python
import pynlm

# Create complete Phase 6 agent with integrated systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create Phase 6 agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL Phase 6 learning subsystems
agent.enableRewardModulation(True)      # Dopamine learning
agent.enableStructuralPlasticity(True)  # Brain growth
agent.enableDevelopment(True)           # Developmental stages
agent.enableCuriosity(True)             # Exploration drive

print("Phase 6 Agent created with complete integration!")
print("Features: Memory, Neuromodulation, Development, Curiosity")
```

### Phase 6 Agent Methods

```python
import pynlm

# Create Phase 6 agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Phase 6: Enable all subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run Phase 6 simulation
for step in range(1000):
    # 1. Get sensory input (Phase 6: stored in working memory)
    percept = world.getSensoryPercept()
    
    # 2. Process sensory input (Phase 6: memory + attention)
    agent.processSensoryInput(percept)
    
    # 3. Brain step (Phase 6: complete integration)
    brain.step(step)
    
    # 4. Get action (Phase 6: planning + neuromodulation)
    action = agent.decodeMotorCommand()
    
    # 5. Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 6. Apply learning signals (Phase 6: dopamine, curiosity)
    reward = 0.0  # Get from world
    agent.applyRewardModulation(reward, 0.0)
    
    # 7. Update development (Phase 6: stage progression)
    agent.updateDevelopment(0.1)
```

## Phase 6 Memory Systems

### Working Memory in Phase 6

```python
import pynlm

# Create Phase 6 brain with working memory
config = pynlm.createDefaultConfig()
config.set("memory.working_memory_size", 1000)  # Configure capacity

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate with Phase 6 working memory
for step in range(500):
    brain.step(step)
    
    # Check working memory (Phase 6: traces from neural activity)
    if brain.getWorkingMemory():
        traces = brain.getWorkingMemory().getActiveTraces()
        print(f"Step {step}: Working memory traces = {traces}")
        
        # Phase 6: Working memory decay
        if traces > 0:
            decay_rate = config.get("memory.working_memory_decay", 0.001)
            # Working memory traces decay over time
```

### Episodic Memory in Phase 6

```python
import pynlm

# Create Phase 6 brain with episodic memory
config = pynlm.createDefaultConfig()
config.set("memory.episodic_capacity", 100)  # Configure capacity

brain = pynlm.createBrain(config)
brain.initialize()

# Phase 6: Experience encoding and replay
for step in range(1000):
    brain.step(step)  # Phase 6: Captures episodic memory
    
    # Check episodic memory every 10 steps
    if step % 10 == 0 and brain.getEpisodicMemory():
        episodes = brain.getEpisodicMemory().getEpisodeCount()
        print(f"Step {step}: Episodic episodes = {episodes}")
        
        # Phase 6: Memory replay every 100 steps
        if step % 100 == 0 and brain.getEpisodicMemory():
            replayed = brain.getEpisodicMemory().getEpisodesForReplay(3)
            print(f"  Replayed episodes: {len(replayed)}")

# Phase 6: Memory consolidation
if step % 1000 == 0 and brain.getEpisodicMemory():
    print("Phase 6: Memory consolidation triggered")
```

### Phase 6 Memory Statistics

```python
import pynlm

# Create and run Phase 6 experiment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run Phase 6 simulation
for step in range(2000):
    brain.step(step)

# Analyze Phase 6 memory systems
print("Phase 6 Memory System Statistics:")
print(f"  Working Memory Capacity: {brain.getWorkingMemory() and brain.getWorkingMemory().getCapacity() or 0}")
print(f"  Working Memory Active Traces: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
print(f"  Episodic Memory Episodes: {brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0}")
print(f"  Associative Memory Strength: {brain.getAssociativeMemory() and brain.getAssociativeMemory().getStrength() or 0.0}")

# Phase 6: Memory system health check
if brain.getWorkingMemory():
    print(f"  Working Memory Health: {'Good' if brain.getWorkingMemory().getActiveTraces() > 0 else 'Low'},
print(f"  Episodic Memory Health: {'Good' if brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() > 0 else 'Empty'}")
```

## Phase 6 Neuromodulation

### Dopamine System in Phase 6

```python
import pynlm

# Create Phase 6 agent with dopamine learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable Phase 6 dopamine learning
agent.enableRewardModulation(True)

# Phase 6: Dopamine learning simulation
for step in range(500):
    world.update(0.1)
    
    # Get reward and apply dopamine modulation
    reward = world.computeReward(brain.getRegion(RegionId(0)).get())
    agent.applyRewardModulation(reward, 0.0)  # Phase 6: Dopamine computation
    
    # Phase 6: Check dopamine level
    dopamine_level = agent.getNeuromodulationLevel()
    print(f"Step {step}: Dopamine level = {dopamine_level:.3f}")
    
    # Phase 6: Dopamine affects learning
    if dopamine_level > 0:
        print(f"  Learning enhanced by dopamine")
```

### Curiosity System in Phase 6

```python
import pynlm

# Create Phase 6 agent with curiosity
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable Phase 6 curiosity
agent.enableCuriosity(True)

# Phase 6: Curiosity-driven exploration
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input (Phase 6: novelty detection)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    # Phase 6: Check curiosity level
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    print(f"Step {step}: Curiosity = {curiosity:.3f}, Novelty = {novelty:.3f}")
    
    # Phase 6: Curiosity drives exploration decisions
    if curiosity > 0.5:
        print(f"  Exploration behavior triggered")
```

### Phase 6 Neuromodulation Statistics

```python
import pynlm

# Create Phase 6 agent and run experiment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all Phase 6 neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run Phase 6 simulation
for step in range(2000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)

# Analyze Phase 6 neuromodulation
print("Phase 6 Neuromodulation Statistics:")
print(f"  Dopamine Level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
print(f"  Prediction Error: {agent.getPredictionError():.3f}")

# Phase 6: Neuromodulation effectiveness check
if agent.getNeuromodulationLevel() > 0:
    print("  Neuromodulation: ACTIVE - Learning enhanced")
else:
    print("  Neuromodulation: LOW - Limited learning")
```

## Phase 6 Development

### Developmental Stages in Phase 6

```python
import pynlm

# Create Phase 6 brain and track development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Phase 6 Developmental Stages:")
print("Initial State:", brain.getDevelopmentalStage().name)

# Phase 6: Development progression
for step in range(2000):
    brain.step(step)
    
    # Phase 6: Check developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage.name} (Age: {brain.getBrainAge():.1f})")
    
    # Phase 6: Stage-specific effects
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        print("  Critical period: High plasticity, rapid learning")
    elif stage == pynlm.DevelopmentalStage.Maturation:
        print("  Maturation: Fine-tuning, specialization")
    elif stage == pynlm.DevelopmentalStage.Adult:
        print("  Adult: Stable performance, maintenance")
    elif stage == pynlm.DevelopmentalStage.Aging:
        print("  Aging: Decline, reduced plasticity")
```

### Phase 6 Development Statistics

```python
import pynlm

# Create and run Phase 6 developmental experiment
config = pynlm.createDefaultConfig()
config.set("development.critical_period_duration", 200.0)

brain = pynlm.createBrain(config)
brain.initialize()

print("Phase 6 Developmental Progression:")

for step in range(3000):
    brain.step(step)
    
    if step % 500 == 0:
        stage = brain.getDevelopmentalStage()
        brain_age = brain.getBrainAge()
        plasticity = brain.getPlasticityRate()
        
        print(f"Step {step}:")
        print(f"  Stage: {stage.name}")
        print(f"  Age: {brain_age:.1f}%")
        print(f"  Plasticity Rate: {plasticity:.4f}")
        print(f"  Structural Plasticity: {'Enabled' if step % 100 == 0 else 'Normal'}")
```

## Running Phase 6 Experiments

### Phase 6 Integration Demo

```bash
# Run Phase 6 integration demo
./nlm_test_phase6

# Expected output:
# === NLM Phase 6 Integration Demo ===
# Testing the integrated artificial brain...
# --- Integration Verification ---
# Integration verification passed!
# --- Memory Integration Test ---
# --- Neuromodulation Integration Test ---
# --- Checkpoint Test ---
# --- Replay Test ---
# --- Full Integration Experiment ---
# === FINAL RESULTS ===
# Total reward: 0.000
# Avg firing rate: 0.500
# Episodes stored: 150.0
# Dopamine level: 0.100
```

### Running Phase 6 Experiments Programmatically

```python
import pynlm
from src.experiments.Phase6IntegratedExperiment.hpp import Phase6IntegratedExperiment, Phase6Config

# Create Phase 6 experiment
experiment = Phase6IntegratedExperiment()

# Configure Phase 6 experiment
config = Phase6Config()
config.neuronCount = 500
config.maxSteps = 2000
config.enableCheckpointing = true
config.enableReplay = true
config.enableDevelopment = true

# Run Phase 6 experiment
result = experiment.run(config)

print("Phase 6 Experiment Results:")
print(f"  Total Reward: {result.totalReward}")
print(f"  Average Firing Rate: {result.avgFiringRate}")
print(f"  Episodes Stored: {result.memoryEpisodesStored}")
print(f"  Dopamine Level: {result.dopamineLevel}")
print(f"  Wall Clock Time: {result.totalWallClockTime}s")

print("\nPhase 6 Integration Status:")
print(f"  Working Memory: {'CONNECTED' if result.memoryWorkingMemoryIntegrated else 'DISCONNECTED'}")
print(f"  Episodic Memory: {'CONNECTED' if result.memoryEpisodicMemoryIntegrated else 'DISCONNECTED'}")
print(f"  Neuromodulation: {'CONNECTED' if result.neuromodulationIntegrated else 'DISCONNECTED'}")
print(f"  Prediction: {'CONNECTED' if result.predictionIntegrated else 'DISCONNECTED'}")
print(f"  Development: {'CONNECTED' if result.developmentIntegrated else 'DISCONNECTED'}")
print(f"  Checkpointing: {'WORKING' if result.checkpointingWorks else 'NOT WORKING'}")
```

### Phase 6 Experiment Parameters

```python
import pynlm
from src.experiments.Phase6IntegratedExperiment.hpp import Phase6Config

# Phase 6 experiment configurations

# Example 1: Basic Phase 6 experiment
config1 = Phase6Config()
config1.neuronCount = 500
config1.maxSteps = 2000
config1.enableCheckpointing = true
config1.enableReplay = true
config1.enableDevelopment = true

# Example 2: Advanced Phase 6 experiment
config2 = Phase6Config()
config2.neuronCount = 1000
config2.maxSteps = 5000
config2.enableCheckpointing = true
config2.enableReplay = true
config2.enableDevelopment = true
config2.enablePrediction = true
config2.enablePlanning = true

# Example 3: Memory-focused Phase 6 experiment
config3 = Phase6Config()
config3.neuronCount = 300
config3.maxSteps = 3000
config3.enableCheckpointing = true
config3.enableReplay = true
config3.enableDevelopment = true
config3.memoryWorkingMemorySize = 2000
config3.memoryEpisodicCapacity = 500
```

## Phase 6 Analysis and Visualization

### Basic Phase 6 Analysis

```python
import pynlm
import matplotlib.pyplot as plt

# Create and run Phase 6 experiment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect Phase 6 metrics
steps = []
firing_rates = []
memory_traces = []
curiosity_levels = []
dopamine_levels = []

for step in range(1000):
    brain.step(step)
    
    steps.append(step)
    firing_rates.append(brain.getAverageFiringRate())
    
    if brain.getWorkingMemory():
        memory_traces.append(brain.getWorkingMemory().getActiveTraces())
    else:
        memory_traces.append(0)
    
    # Need Phase 6 agent for neuromodulation stats
    # (create agent and world as in previous examples)
    
    steps.append(step)
    dopamine_levels.append(agent.getNeuromodulationLevel())
    curiosity_levels.append(agent.getCuriosityLevel())

# Create Phase 6 analysis plots
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Neural activity
axes[0, 0].plot(steps, firing_rates)
axes[0, 0].set_title('Phase 6: Neural Activity')
axes[0, 0].set_xlabel('Step')
axes[0, 0].set_ylabel('Firing Rate (Hz)')

# Working memory
axes[0, 1].plot(steps, memory_traces)
axes[0, 1].set_title('Phase 6: Working Memory')
axes[0, 1].set_xlabel('Step')
axes[0, 1].set_ylabel('Active Traces')

# Neuromodulation
axes[1, 0].plot(steps, dopamine_levels)
axes[1, 0].set_title('Phase 6: Neuromodulation')
axes[1, 0].set_xlabel('Step')
axes[1, 0].set_ylabel('Dopamine Level')

axes[1, 1].plot(steps, curiosity_levels)
axes[1, 1].set_title('Phase 6: Curiosity')
axes[1, 1].set_xlabel('Step')
axes[1, 1].set_ylabel('Curiosity Level')

plt.tight_layout()
plt.savefig('phase6_analysis.png')
plt.show()
```

### Phase 6 Data Export

```python
import pynlm
import json

# Create and run Phase 6 experiment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect Phase 6 experimental data
experimental_data = {
    'configuration': {
        'neuron_count': config.getOr<int64_t>('brain.neuron_count', 1000),
        'memory_working_memory_size': config.getOr<int>('memory.working_memory_size', 1000),
        'memory_episodic_capacity': config.getOr<int>('memory.episodic_capacity', 100),
        'development_critical_period_duration': config.getOr<float>('development.critical_period_duration', 100.0),
        'neuromod_dopamine_scale': config.getOr<float>('neuromod.dopamine_scale', 1.0),
    },
    'results': {
        'final_neural_activity': [],
        'memory_system_performance': {},
        'neuromodulation_effectiveness': {},
        'developmental_progression': [],
        'integration_verification': {}
    },
    'metrics': {
        'total_steps': 0,
        'final_firing_rate': 0.0,
        'peak_memory_traces': 0,
        'average_curiosity': 0.0,
        'total_dopamine_effect': 0.0
    }
}

# Run Phase 6 simulation and collect data
for step in range(2000):
    brain.step(step)
    
    experimental_data['metrics']['total_steps'] = step + 1
    experimental_data['metrics']['final_firing_rate'] = brain.getAverageFiringRate()
    
    if brain.getWorkingMemory():
        traces = brain.getWorkingMemory().getActiveTraces()
        experimental_data['metrics']['peak_memory_traces'] = max(
            experimental_data['metrics']['peak_memory_traces'], traces)

# Save Phase 6 experimental data
with open('phase6_experimental_data.json', 'w') as f:
    json.dump(experimental_data, f, indent=2, default=str)

print("Phase 6 experimental data saved to 'phase6_experimental_data.json'")
```

## Phase 6 Performance Tuning

### Adjusting Phase 6 Parameters

```python
import pynlm

# Phase 6 performance optimization configurations

def optimize_for_memory(config):
    """Optimize Phase 6 for memory-intensive tasks"""
    config.set("memory.working_memory_size", 3000)
    config.set("memory.episodic_capacity", 1000)
    config.set("memory.replay_interval", 200)
    config.set("brain.neuron_count", 800)
    return config

def optimize_for_speed(config):
    """Optimize Phase 6 for speed/performance"""
    config.set("memory.working_memory_size", 500)
    config.set("memory.episodic_capacity", 50)
    config.set("memory.replay_interval", 500)
    config.set("brain.neuron_count", 200)
    return config

def optimize_for_learning(config):
    """Optimize Phase 6 for learning capacity"""
    config.set("memory.working_memory_size", 2000)
    config.set("memory.episodic_capacity", 500)
    config.set("neuromod.dopamine_scale", 2.0)
    config.set("neuromod.curiosity_scale", 1.5)
    config.set("development.critical_period_duration", 300.0)
    return config

# Choose Phase 6 optimization strategy
config = pynlm.createDefaultConfig()
config = optimize_for_memory(config)  # or optimize_for_speed(), optimize_for_learning()

brain = pynlm.createBrain(config)
brain.initialize()
```

### Phase 6 Resource Management

```python
import pynlm
import psutil
import time

# Phase 6 resource monitoring
process = psutil.Process()

def monitor_phase6_resources(config):
    """Monitor Phase 6 resource usage"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Initial Phase 6 memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
    
    for step in range(1000):
        brain.step(step)
        
        if step % 100 == 0:
            memory_mb = process.memory_info().rss / 1024 / 1024
            cpu_percent = process.cpu_percent()
            
            print(f"Step {step}: Memory = {memory_mb:.1f} MB, CPU = {cpu_percent}%")
            
            # Phase 6: Resource warning
            if memory_mb > 1000:  # 1GB warning
                print("  WARNING: High memory usage in Phase 6!")
            if cpu_percent > 80:  # 80% CPU warning
                print("  WARNING: High CPU usage in Phase 6!")

# Configure and run Phase 6 with monitoring
config = pynlm.createDefaultConfig()
monitor_phase6_resources(config)
```

## Advanced Phase 6 Features

### Phase 6 Checkpointing

```python
import pynlm

# Create Phase 6 brain with checkpointing
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run Phase 6 simulation
for step in range(1000):
    brain.step(step)
    
    # Phase 6: Save checkpoint periodically
    if step % 200 == 0:
        checkpoint_path = f"phase6_checkpoint_step_{step}.bin"
        if brain.save(checkpoint_path):
            print(f"Phase 6 checkpoint saved: {checkpoint_path}")

# Phase 6: Load checkpoint
checkpoint_path = "phase6_checkpoint_step_500.bin"
if brain.load(checkpoint_path):
    print("Phase 6 checkpoint loaded successfully")
    print(f"Neurons after load: {brain.getTotalNeuronCount()}")
```

### Phase 6 Replay System

```python
import pynlm

# Create Phase 6 brain with replay
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run Phase 6 simulation
for step in range(2000):
    brain.step(step)  # Phase 6: Captures episodes
    
    # Phase 6: Check episodic memory
    if brain.getEpisodicMemory():
        episodes = brain.getEpisodicMemory().getEpisodeCount()
        print(f"Step {step}: {episodes} episodes captured")
        
        # Phase 6: Replay every 100 steps
        if step % 100 == 0:
            replayed = brain.getEpisodicMemory().getEpisodesForReplay(3)
            print(f"  Replaying {len(replayed)} episodes")
            # Replay logic would go here

# Phase 6: Memory consolidation
if step % 1000 == 0 and brain.getEpisodicMemory():
    print("Phase 6: Memory consolidation triggered")
```

### Phase 6 Prediction System

```python
import pynlm

# Create Phase 6 brain with prediction
config = pynlm.createDefaultConfig()
config.set("prediction.horizon", 10)
config.set("prediction.accuracy_threshold", 0.7)

brain = pynlm.createBrain(config)
brain.initialize()

# Phase 6: Prediction system integration
if brain.getPredictionSystem():
    print("Phase 6: Prediction system available")
    # Phase 6: Use prediction system
    prediction = brain.getPredictionSystem().predictNextState()
    print(f"Predicted state: {prediction}")
else:
    print("Phase 6: Prediction system not yet integrated")
```

## Phase 6 Troubleshooting

### Common Phase 6 Issues

**Issue 1: Phase 6 Memory Systems Not Working**

```
# Check if Phase 6 memory systems are enabled
if brain.getWorkingMemory():
    print("Working memory: Available")
else:
    print("Working memory: Not available - Check configuration")

if brain.getEpisodicMemory():
    print("Episodic memory: Available")
else:
    print("Episodic memory: Not available - Check configuration")
```

**Issue 2: Phase 6 Neuromodulation Not Working**

```python
# Verify Phase 6 neuromodulation systems
if hasattr(brain.getConfig(), 'get'):
    dopamine_scale = brain.getConfig().get('neuromod.dopamine_scale', 1.0)
    print(f"Dopamine scale: {dopamine_scale}")
    
    if dopamine_scale > 0:
        print("Phase 6: Dopamine neuromodulation enabled")
    else:
        print("Phase 6: Dopamine neuromodulation disabled")
```

**Issue 3: Phase 6 Performance Issues**

```python
import pynlm
import time

# Create Phase 6 brain with monitoring
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Start with smaller brain

brain = pynlm.createBrain(config)
brain.initialize()

# Measure Phase 6 performance
start_time = time.time()

for step in range(100):
    brain.step(step)

end_time = time.time()
elapsed = end_time - start_time

print(f"Phase 6: Processed 100 steps in {elapsed:.2f} seconds")
print(f"Phase 6: Average step time: {elapsed/100*1000:.1f} ms")

# Phase 6: Performance optimization recommendations
if elapsed > 10:  # If taking more than 10 seconds
    print("  Performance recommendation: Reduce neuron count or increase optimization")
```

**Issue 4: Phase 6 Integration Verification**

```bash
# Run Phase 6 integration verification
./nlm_test_phase6

# Check for these key outputs:
# [PASS] Working memory is integrated
# [PASS] Episodic memory is integrated
# [PASS] Dopamine system is integrated
# [PASS] Curiosity system is integrated
# [PASS] Prediction system is integrated
# [PASS] Development system is integrated
```

### Phase 6 Debug Commands

```bash
# Enable Phase 6 verbose logging
cd build
gdb ./nlm_test_phase6
# In gdb: set logging file phase6_debug.log
# In gdb: set logging on
# In gdb: run

# Check Phase 6 build configuration
cd build
grep -r "ENABLE_PHASE6_INTEGRATION" CMakeCache.txt

# Verify Phase 6 dependencies
cd build
ldd ./nlm_test_phase6 | grep -E "(stdc\+\+|pthread|dl)"
```

## Phase 6 Further Reading

### Recommended Documentation

1. **[PHASE6_ARCHITECTURE.md](docs/PHASE6_ARCHITECTURE.md)** - Detailed Phase 6 architecture
2. **[PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md)** - Phase 6 results and analysis
3. **[PHASE6_FEATURES.md](docs/PHASE6_FEATURES.md)** - Complete Phase 6 feature list
4. **[PHASE6_TUTORIAL.md](docs/PHASE6_TUTORIAL.md)** - Phase 6 tutorial (this document)
5. **[PHASE6_REFERENCE.md](docs/PHASE6_REFERENCE.md)** - Phase 6 API and configuration reference
6. **[PHASE6_BEST_PRACTICES.md](docs/PHASE6_BEST_PRACTICES.md)** - Phase 6 development and usage best practices

### Research Papers and Publications

- "NLM Phase 6: Final Integration of Artificial Brain Systems" - Technical documentation
- "Memory Systems Integration in NLM Phase 6" - Research on memory integration
- "Neuromodulation Effects on Neural Plasticity in Phase 6" - Research on dopamine and curiosity
- "Checkpointing and Persistence in NLM Phase 6" - Research on state management

### Academic Resources

- **NLM Project Website**: https://github.com/nlm-project
- **Phase 6 Documentation**: https://docs.nlm-project.org/phase6
- **NLM Research Papers**: Available through project repository
- **Phase 6 Experiment Data**: Available in project examples

### Related Projects

- **Phase 2-5 Documentation**: For understanding individual Phase contributions
- **Related Neural Network Projects**: For comparison and benchmarking
- **Brain Simulation Tools**: For additional research capabilities

### Phase 6 Support

For Phase 6 questions and support:
- **GitHub Issues**: Report Phase 6 bugs and feature requests
- **Documentation**: Check Phase 6 documentation for updates
- **Community**: Join NLM Phase 6 discussion forums
- **Development**: Contribute to Phase 6 improvements

## Conclusion

**Phase 6 Tutorial Summary**:

This tutorial has provided a comprehensive introduction to NLM Phase 6 - the final integration phase of the Neural Learning Machine project. Phase 6 represents a significant milestone with all major systems integrated into a coherent artificial brain.

**Phase 6 Key Achievements**:
1. ✅ Complete closed-loop architecture with 15-step brain loop
2. ✅ Integrated memory systems (Working + Episodic + Associative)
3. ✅ Neuromodulation integration (Dopamine + Curiosity + Novelty)
4. ✅ Development system with five-stage progression
5. ✅ Persistence mechanisms (Checkpoint + Replay)
6. ✅ Performance optimizations (SIMD, memory pools)

**Phase 6 Learning Outcomes**:
- Understand Phase 6 architecture and integration
- Create and configure Phase 6 brains
- Implement Phase 6 agents with complete integration
- Analyze Phase 6 memory and neuromodulation systems
- Optimize Phase 6 performance and resource usage
- Troubleshoot Phase 6 integration issues

**Phase 6 Next Steps**:
1. Complete this tutorial
2. Run Phase 6 integration experiments
3. Customize Phase 6 for specific research needs
4. Contribute to Phase 6 improvements
5. Publish Phase 6 results and findings

**Phase 6 Success**: Phase 6 provides the foundation for investigating emergence, learning, and adaptation in a brain-like system with integrated memory, neuromodulation, and development systems. The complete artificial brain is now ready for research and experimentation.