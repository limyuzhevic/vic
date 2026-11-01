# NLM (Neural Learning Machine) - Advanced User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. This guide provides comprehensive documentation for advanced users who want to work with the neural simulation framework at a deeper level.

## Project Architecture

### Core Components

1. **Brain Class** - Central neural simulation engine
   - 500-1000 neurons by default
   - 1-2 regions (configurable)
   - Full integration of memory, prediction, cognition, and neuromodulation systems

2. **AgentBrain Class** - Bridge between brain and environment
   - Handles sensory transduction and motor decoding
   - Implements reward-based learning and neuromodulation
   - Manages developmental processes and adaptation

3. **SimpleWorld Class** - 2D simulation environment
   - Grid-based world with resource and hazard objects
   - Physics simulation for agent movement
   - Reward computation based on actions

### Advanced Systems Integration

The NLM framework integrates multiple advanced brain systems:

#### Memory Systems
- **Working Memory**: Transient active information maintenance
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern association and recall

#### Neuromodulation
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Surprise detection and change detection
- **Prediction Error**: Discrepancy between expected and actual outcomes

#### Cognitive Systems
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Pattern discovery and categorization
- **Attentional Selection**: Competitive focus and attention mechanisms

#### Developmental Systems
- **Structural Plasticity**: Synaptogenesis and pruning
- **Maturation**: Age-dependent plasticity modulation
- **Critical Periods**: Sensitive periods for learning

## Advanced Python Features

### Enhanced Configuration System

The NLM Python bindings provide extensive configuration options:

```python
from pynlm import Config, createBrain, createSimpleWorld

# Create advanced configuration
config = Config()
config.loadFromFile("advanced_config.json")

# Set advanced neural parameters
config.set("neuron_count", 2000)  # Larger network
config.set("region_count", 4)     # Multiple regions
config.set("connection_probability", 0.05)  # Sparse connectivity
config.set("simulation_timestep", 0.0001)  # Higher temporal resolution

# Configure advanced plasticity
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.018)
config.set("stdp_tau", 50.0)  # Slower STDP time constant
config.set("synaptogenesis_rate", 0.0005)
config.set("pruning_rate", 0.00005)

# Configure neuromodulation
config.set("dopamine_baseline", 0.2)
config.set("reward_discount_factor", 0.95)

# Configure memory systems
config.set("replay_interval", 200)
config.set("consolidation_interval", 2000)
config.set("working_memory_capacity", 500)

# Enable advanced features
config.set("enable_global_inhibition", True)
config.set("enable_homeostatic_scaling", True)
config.set("enable_neuromodulation_learning", True)
config.set("enable_replay_for_learning", True)
config.set("enable_concept_formation", True)
config.set("enable_attention_dynamics", True)
```

### Advanced Sensory Processing

The framework supports advanced sensory modalities and processing:

```python
from pynlm import Vision, Audio, InternalSignals, SensoryPercept

# Create advanced sensory inputs
vision = Vision(width=128, height=128, channels=3)
vision.setData([...])  # High-resolution visual data

# Audio processing with spectral features
audio = Audio(sampleRate=44100, numSamples=16384)
audio.setData([...])  # Audio features

# Internal signals from physiological sensors
internal = InternalSignals()
internal.addSignal(0.7)  # Energy level
internal.addSignal(0.3)  # Stress level
internal.addSignal(0.9)  # Attention level

# Combine into percept
sensory_percept = SensoryPercept()
sensory_percept.setVision(vision)
sensory_percept.setAudio(audio)
sensory_percept.setInternal(internal)
sensory_percept.setProprioception([...])  # Proprioceptive data
```

### Advanced Agent Capabilities

#### Custom Motor Actions
```python
from pynlm import Action, ActionType

# Define custom action with parameters
custom_action = Action(
    ActionType.Custom,
    parameters=[0.5, 0.8, 1.2]  # Custom parameters
)

# Define complex action sequences
action_sequence = [
    Action(ActionType.MoveForward, parameters=[1.0]),
    Action(ActionType.TurnLeft, parameters=[0.5]),
    Action(ActionType.Interact, parameters=[0.0]),
    Action(ActionType.Look, parameters=[90.0, 45.0]),
]
```

#### Advanced Motor Command Processing
```python
from pynlm import SimpleWorld

world = SimpleWorld()
world.configure(width=100, height=100, visionWidth=64, visionHeight=64)

# Get sensory percept for decision making
percept = world.getSensoryPercept()

# Advanced motor command decoding with context
motor_cmd = world.decodeMotorCommand(
    percept,  # Current sensory input
    world.getAgentBody(),  # Agent state
    step=1000,  # Current simulation step
    context=[...]  # Additional context
)
```

### Advanced Learning and Development

#### Reward-Based Learning
```python
from pynlm import AgentBrain

# Create brain and agent
brain = createBrain(config)
agent = AgentBrain(brain)
world = createSimpleWorld()
agent.initialize(world)

# Simulate episode with reward tracking
total_reward = 0.0
description = "Advanced agent learning episode"

for step in range(1000):
    # Get motor command
    cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    action_result = world.applyMotorCommand(cmd, step * 0.001)
    
    # Apply reward-based neuromodulation
    agent.applyRewardModulation(
        action_result.reward,  # Actual reward
        agent.getPredictionError(),  # Prediction error
    )
    
    total_reward += action_result.reward
    
    # Update development
    agent.updateDevelopment(0.001)
    
    print(f"Step {step}: Reward = {action_result.reward:.3f}, Total = {total_reward:.3f}")

print(f"Episode completed. Total reward: {total_reward:.3f}")
```

#### Development Tracking
```python
# Monitor developmental progress
brain = agent.getBrain()

print(f"Developmental Stage: {brain.getDevelopmentalStage()}")
print(f"Neuromodulation Level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity Level: {agent.getCuriosityLevel()}")
print(f"Novelty Level: {agent.getNoveltyLevel()}")

# Check specific developmental metrics
if hasattr(brain, 'getDevelopmentalAge'):
    print(f"Developmental Age: {brain.getDevelopmentalAge():.2f}")

if hasattr(brain, 'getPlasticityLevel'):
    print(f"Plasticity Level: {brain.getPlasticityLevel():.3f}")

if hasattr(brain, 'getMemoryConsolidation'):
    print(f"Memory Consolidation: {brain.getMemoryConsolidation():.3f}")
```

### Advanced Simulation Control

#### Multi-Agent Simulation
```python
from pynlm import createBrain, createSimpleWorld, createAgentBrain

# Create multiple agents
brain1 = createBrain(config)
brain2 = createBrain(config)  # Different configuration

agent1 = createAgentBrain(brain1)
agent2 = createAgentBrain(brain2)

world1 = createSimpleWorld()
world2 = createSimpleWorld()

agent1.initialize(world1)
agent2.initialize(world2)

# Multi-agent interaction simulation
def simulate_multi_agent(world1, world2, agent1, agent2):
    for step in range(500):
        # Each agent perceives its own world
        percept1 = world1.getSensoryPercept()
        percept2 = world2.getSensoryPercept()
        
        # Agents act independently but interact through world states
        cmd1 = agent1.decodeMotorCommand()
        cmd2 = agent2.decodeMotorCommand()
        
        # Apply actions to respective worlds
        action1 = world1.applyMotorCommand(cmd1, step * 0.001)
        action2 = world2.applyMotorCommand(cmd2, step * 0.001)
        
        # Apply reward modulation
        agent1.applyRewardModulation(action1.reward, agent1.getPredictionError())
        agent2.applyRewardModulation(action2.reward, agent2.getPredictionError())
        
        print(f"Step {step}: Agent1 Reward = {action1.reward:.3f}, Agent2 Reward = {action2.reward:.3f}")

simulate_multi_agent(world1, world2, agent1, agent2)
```

#### Advanced Checkpointing and Persistence
```python
from pynlm import createBrain
import json
import time

# Create brain with checkpointing
brain = createBrain(config)
brain.initialize()

# Run simulation for a while
for step in range(100):
    brain.step(step, step * 0.001)
    
    # Save checkpoint periodically
    if step % 100 == 0:
        checkpoint_file = f"checkpoint_step_{step}.bin"
        if brain.save(checkpoint_file):
            print(f"Checkpoint saved: {checkpoint_file}")
            
            # Load checkpoint to test persistence
            brain2 = createBrain(config)
            brain2.initialize()
            
            if brain2.load(checkpoint_file):
                print(f"Checkpoint loaded successfully")
                print(f"Loaded brain spike count: {brain2.getTotalSpikeCount()}")
                print(f"Original brain spike count: {brain.getTotalSpikeCount()}")
        else:
            print(f"Failed to save checkpoint: {checkpoint_file}")
```

### Performance Monitoring and Analysis

#### Brain Performance Metrics
```python
from pynlm import createBrain, createSimpleWorld, createAgentBrain

# Create simulation setup
brain = createBrain(config)
agent = AgentBrain(brain)
world = SimpleWorld()
agent.initialize(world)

# Performance metrics collection
performance_data = {
    'steps': [],
    'rewards': [],
    'firing_rates': [],
    'spike_counts': [],
    'memory_usage': [],
    'novelty_levels': [],
    'curiosity_levels': [],
}

# Run simulation with detailed monitoring
for step in range(1000):
    # Record metrics before action
    firing_rate = brain.getAverageFiringRate()
    spike_count = brain.getTotalSpikeCount()
    novelty_level = agent.getNoveltyLevel()
    curiosity_level = agent.getCuriosityLevel()
    
    performance_data['steps'].append(step)
    performance_data['firing_rates'].append(firing_rate)
    performance_data['spike_counts'].append(spike_count)
    performance_data['novelty_levels'].append(novelty_level)
    performance_data['curiosity_levels'].append(curiosity_level)
    
    # Execute simulation step
    cmd = agent.decodeMotorCommand()
    action_result = world.applyMotorCommand(cmd, step * 0.001)
    
    # Record reward
    reward = action_result.reward
    performance_data['rewards'].append(reward)
    
    # Apply reward modulation
    agent.applyRewardModulation(reward, agent.getPredictionError())
    
    # Store memory metrics if available
    if hasattr(brain, 'getWorkingMemory'):
        working_memory = brain.getWorkingMemory()
        if working_memory:
            performance_data['memory_usage'].append(working_memory.getActiveTraces())

# Analyze performance data
import numpy as np

def analyze_performance(data):
    print("=== Performance Analysis ===")
    
    rewards = np.array(data['rewards'])
    firing_rates = np.array(data['firing_rates'])
    
    print(f"Reward Statistics:")
    print(f"  Mean: {rewards.mean():.4f}")
    print(f"  Std: {rewards.std():.4f}")
    print(f"  Min: {rewards.min():.4f}")
    print(f"  Max: {rewards.max():.4f}")
    print(f"  Total: {rewards.sum():.4f}")
    
    print(f"\nFiring Rate Statistics:")
    print(f"  Mean: {firing_rates.mean():.4f} Hz")
    print(f"  Std: {firing_rates.std():.4f} Hz")
    print(f"  Min: {firing_rates.min():.4f} Hz")
    print(f"  Max: {firing_rates.max():.4f} Hz")
    
    # Check for stability
    reward_stability = np.std(rewards[-100:]) if len(rewards) >= 100 else np.std(rewards)
    firing_stability = np.std(firing_rates[-100:]) if len(firing_rates) >= 100 else np.std(firing_rates)
    
    print(f"\nStability (last 100 steps):")
    print(f"  Reward stability: {reward_stability:.6f}")
    print(f"  Firing stability: {firing_stability:.6f}")
    
    # Check for learning patterns
    if len(rewards) >= 200:
        first_half = rewards[:100]
        second_half = rewards[100:200]
        learning_gain = second_half.mean() - first_half.mean()
        print(f"\nLearning Gain (steps 100-200): {learning_gain:.4f}")

analyze_performance(performance_data)
```

### Configuration File Templates

#### Advanced Configuration Template
```ini
# Advanced NLM Configuration

# Basic simulation settings
random_seed = 42
simulation_timestep = 0.0001
neuron_count = 2000
region_count = 4
connection_probability = 0.05

# Enhanced plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
stdp_time_constant = 50.0

# Development settings
development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.00005

# Neuromodulation settings
dopamine_baseline = 0.2

# Reward settings
reward_discount_factor = 0.95

# Environment settings
environment_name = GridWorld
environment_width = 200
environment_height = 200

# Advanced neural dynamics
enable_global_inhibition = true
firing_rate_threshold = 0.15
inhibition_strength_scale = 8.0
enable_homeostatic_scaling = true
homeostatic_scaling_interval = 200
homeostatic_target_excitatory_weight = 5.0

# Memory systems
replay_interval = 200
consolidation_interval = 2000
working_memory_capacity = 1000
enable_replay_for_learning = true
enable_concept_formation = true
enable_attention_dynamics = true

# Cognitive development
enable_advanced_planning = true
planning_depth = 8
enable_concept_learning = true
concept_formation_threshold = 0.8
attention_inhibition_strength = 0.7
attention_excitation_strength = 2.0

# Simulation monitoring
max_simulation_steps = 50000
simulation_time_limit = 0.0
checkpoint_dir = ./checkpoints
checkpoint_save_interval = 1000
max_checkpoints = 20
checkpoint_compress = true

# Visualization and debugging
visualization_enabled = true
visualization_update_rate = 30.0
debug_logging_enabled = true
log_level = DEBUG
log_to_file = true
log_filename = nlm_debug.log

# Advanced performance settings
enable_parallel_processing = true
enable_vectorized_operations = true
optimization_level = 3
memory_pool_size = 100000
```

### Example Advanced Applications

#### 1. Resource Management Agent
```python
# Create specialized agent for resource management
class ResourceManagementAgent(AgentBrain):
    def __init__(self, config):
        brain = createBrain(config)
        super().__init__(brain)
        self.resource_threshold = config.getOrFloat("resource_threshold", 0.8)
        self.hazard_avoidance = config.getOrFloat("hazard_avoidance", 0.9)
    
    def getOptimalAction(self, percept):
        # Custom decision logic for resource management
        vision = percept.getVision()
        # Analyze visual scene for resources
        # Calculate approach strategy
        # Return optimal action
        pass

# Create and use the specialized agent
agent = ResourceManagementAgent(config)
```

#### 2. Social Learning Agent
```python
# Agent that learns from observing other agents
class SocialLearningAgent(AgentBrain):
    def __init__(self, config):
        super().__init__(createBrain(config))
        self.social_memory = {}
        self.teacher_mode = False
    
    def observeOtherAgent(self, other_agent, action, outcome):
        # Store observed experiences for social learning
        self.social_memory[(other_agent, action)] = outcome
        
        # Update concept formation with social observations
        if hasattr(self.getBrain(), 'getConceptFormation'):
            # Process observed outcome for concept formation
            pass
    
    def getActionFromObservation(self, context):
        # Select actions based on social observations
        if context in self.social_memory:
            return self.social_memory[context]
        return self.decodeMotorCommand()

# Create and use social learning agent
social_agent = SocialLearningAgent(config)
```

#### 3. Adaptive Controller
```python
# Agent that adapts its parameters based on performance
class AdaptiveControllerAgent(AgentBrain):
    def __init__(self, config):
        super().__init__(createBrain(config))
        self.performance_history = []
        self.adaptation_rate = config.getOrFloat("adaptation_rate", 0.01)
    
    def updateBasedOnPerformance(self, reward, step):
        # Adapt neural parameters based on recent performance
        self.performance_history.append((step, reward))
        
        # Keep history manageable
        if len(self.performance_history) > 1000:
            self.performance_history.pop(0)
        
        # Calculate recent performance
        if len(self.performance_history) >= 10:
            recent_rewards = [r for _, r in self.performance_history[-10:]]
            avg_reward = sum(recent_rewards) / len(recent_rewards)
            
            # Adapt parameters based on performance
            self.adaptParameters(avg_reward)
    
    def adaptParameters(self, target_reward):
        # Adapt various parameters based on target reward
        current_reward = self.getRecentAverageReward()
        error = target_reward - current_reward
        
        # Adjust neuromodulation sensitivity
        if hasattr(self.getBrain(), 'getDopamine'):
            dopamine = self.getBrain().getDopamine()
            if dopamine:
                current_level = dopamine.getLevel()
                adapted_level = current_level + self.adaptation_rate * error
                dopamine.setLevel(adapted_level)

# Create and use adaptive controller
adaptive_agent = AdaptiveControllerAgent(config)
```

## Troubleshooting Guide

### Common Issues and Solutions

#### Issue 1: High Spike Counts
**Problem**: Network is generating too many spikes, causing instability.

**Solution**:
```python
# Increase inhibitory control
config.set("enable_global_inhibition", True)
config.set("firing_rate_threshold", 0.10)  # Lower threshold
config.set("inhibition_strength_scale", 10.0)

# Enable homeostatic scaling
config.set("enable_homeostatic_scaling", True)
config.set("homeostatic_scaling_interval", 50)
config.set("homeostatic_target_excitatory_weight", 2.0)
```

#### Issue 2: Lack of Selective Learning
**Problem**: Network learns general patterns but lacks selectivity.

**Solution**:
```python
# Enable advanced learning mechanisms
config.set("enable_concept_formation", True)
config.set("concept_formation_threshold", 0.7)
config.set("enable_attention_dynamics", True)
config.set("attention_inhibition_strength", 0.8)
config.set("attention_excitation_strength", 2.0)

# Increase plasticity modulation
config.set("enable_neuromodulation_learning", True)
config.set("learning_rate_modulation", 2.0)
```

#### Issue 3: Reward Signal Issues
**Problem**: Rewards are too small or not properly integrated.

**Solution**:
```python
# Increase reward sensitivity
config.set("reward_discount_factor", 0.9)  # Less discounting
config.set("dopamine_baseline", 0.5)  # Higher baseline

# Enable reward-based learning
agent.enableRewardModulation(True)
```

#### Issue 4: Memory Management
**Problem**: Memory systems are overwhelmed or not working properly.

**Solution**:
```python
# Adjust memory parameters
config.set("working_memory_capacity", 2000)
config.set("replay_interval", 100)
config.set("consolidation_interval", 1000)
config.set("enable_replay_for_learning", True)
```

### Debug Commands

```python
# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create brain with debug configuration
config = Config()
config.set("debug_logging_enabled", True)
config.set("log_level", "DEBUG")

brain = createBrain(config)
brain.initialize()

# Monitor brain status
brain.logStatus()

# Check specific internal states
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
print(f"Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.4f}")

# Check developmental progress
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
if hasattr(brain, 'getDevelopmentalAge'):
    print(f"Developmental age: {brain.getDevelopmentalAge():.2f}")
```

## Best Practices

### 1. Configuration Management
- Use configuration files for different experimental setups
- Keep configuration objects simple and modular
- Document all configuration parameters

### 2. Simulation Management
- Always initialize components before use
- Use checkpointing for long-running simulations
- Monitor performance metrics regularly

### 3. Learning and Development
- Start with simple configurations and gradually increase complexity
- Use appropriate reward scaling for different tasks
- Balance exploration and exploitation

### 4. Memory Management
- Monitor memory usage during long simulations
- Use appropriate replay intervals for memory consolidation
- Balance working memory capacity with task requirements

### 5. Error Handling
- Implement proper error handling for all external interactions
- Use try-catch blocks for critical operations
- Implement graceful shutdown procedures

## Conclusion

The NLM framework provides a powerful platform for building advanced artificial neural systems. The Python bindings offer extensive functionality for researchers and developers working with complex neural simulations. By following the best practices outlined in this guide, you can create sophisticated agents that learn, adapt, and interact with their environment in realistic ways.

The framework continues to evolve with new features and capabilities, making it an excellent choice for cutting-edge research in artificial intelligence and computational neuroscience.