# NLM - Python API Reference

## Overview

This document provides complete reference documentation for the NLM Python bindings (`pynlm`). It covers all available classes, methods, and enumerations with detailed parameter descriptions, return values, and usage examples.

## Module Information

### `pynlm` Module

The main module is imported as `pynlm` and contains:
- Brain simulation classes
- Configuration management
- Agent behavior models
- World simulation environments
- Data structures and enumerations
- Utility functions

#### Version Information

```python
import pynlm

# Get module version
print(pynlm.__version__)  # e.g., "0.1.0"
```

## Core Classes

### Config Class

#### Creation and Management

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config.loadFromFile("my_config.json")
```

#### Query Methods

```python
# Check if parameter exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Get all parameter keys
keys = config.getKeys()

# Check parameter type
if config.isInt("brain.neuron_count"):
    count = config.getInt("brain.neuron_count")
elif config.isFloat("brain.neuron_count"):
    count = config.getFloat("brain.neuron_count")
elif config.isBool("brain.neuron_count"):
    enabled = config.getBool("brain.neuron_count")
```

#### Modification Methods

```python
# Set parameters using string value
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Set parameters with type safety
config.setInt("brain.neuron_count", 1000)
config.setFloat("plasticity.stdp.learning_rate", 0.001)
config.setBool("neuromod.dopamine.scale", True)
```

### Brain Class

#### Creation and Initialization

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain (required before use)
brain.initialize()
```

#### Simulation Control

```python
# Run single simulation step with time
brain.step(step_number)

# Run simulation step with explicit timestamp
brain.step(step_number, time=0.1)

# Reset brain to initial state
brain.reset()

# Save brain state to file
brain.save("checkpoint_001.bin")

# Load brain state from file
brain.load("checkpoint_001.bin")
```

#### Input/Output Methods

```python
# Process sensory input into brain
brain.receiveSensoryInput(sensory_input)

# Get motor output from brain
action = brain.produceAction()
```

#### Statistics and Information

```python
# Basic statistics
brain.getTotalNeuronCount()          # Total number of neurons
brain.getTotalSynapseCount()         # Total number of synapses
brain.getFiringNeuronCount()        # Number of currently firing neurons
brain.getAverageFiringRate()        # Average firing rate across all neurons
brain.getExcitationInhibitionRatio() # E/I balance ratio

# Spike statistics
brain.getTotalSpikeCount()          # Total spikes in current simulation
brain.getRecentSpikeCount(window=100) # Spikes in recent window

# Developmental information
brain.getDevelopmentalStage()       # Current developmental stage
brain.getDevelopmentProgress()      # Progress through development
brain.getMaturationLevel()          # Neural maturation level
```

#### Brain Region Management

```python
# Add new brain region
region_id = brain.addRegion("cortex", neuron_count=1000)

# Get region by ID
region = brain.getRegion(region_id)

# Get all regions
regions = brain.getRegions()

# Remove region
brain.removeRegion(region_id)
```

#### Neural Population Management

```python
# Add neural population to region
population_id = brain.addPopulation("excitatory_cortex", region_id, 500)

# Get population by ID
population = brain.getPopulation(population_id)

# Get populations in region
populations = brain.getPopulations(region_id)
```

#### Neuron and Synapse Operations

```python
# Get neuron by ID
neuron = brain.getNeuron(neuron_id)

# Get synapse by ID
synapse = brain.getSynapse(synapse_id)

# Find neurons by type
neurons = brain.findNeurons(pynlm.NeuronType.Excitatory)

# Find synapses by type
synapses = brain.findSynapses(pynlm.SynapseType.Excitatory)

# Get neuron connections
connections = brain.getOutgoingConnections(neuron_id)
```

### AgentBrain Class

#### Creation and Initialization

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)
```

#### Sensory and Motor Processing

```python
# Process sensory input from environment
agent.processSensoryInput(percept)

# Decode motor command from brain activity
motor_cmd = agent.decodeMotorCommand()

# Get sensory perception for agent
perception = agent.getSensoryPerception()
```

#### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Get current neuromodulation level
neuromod_level = agent.getNeuromodulationLevel()

# Check if neuromodulation subsystem is enabled
agent.isRewardModulationEnabled()
```

#### Learning and Plasticity

```python
# Enable/disable learning subsystems
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
agent.enableNoveltyDetection(True/False)

# Trigger specific learning mechanisms
agent.triggerHebbianLearning(pattern)
agent.triggerSTDPUpdate()
```

#### Development and Maturity

```python
# Update development state
agent.updateDevelopment(timestep)

# Get current developmental stage
stage = agent.getDevelopmentalStage()

# Get development progress
progress = agent.getDevelopmentProgress()
```

#### Statistical Monitoring

```python
# Curiosity-driven exploration metrics
agent.getCuriosityLevel()
agent.getCuriosityThreshold()

# Novelty detection metrics
agent.getNoveltyLevel()
agent.getNoveltyHistory(window=100)

# Prediction error (for predictive coding)
agent.getPredictionError()
agent.getPredictionConfidence()

# Reward-related metrics
agent.getLastReward()
agent.getCumulativeReward(steps=1000)
```

### SimpleWorld Class

#### Creation and Configuration

```python
# Create simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions and properties
world.configure(
    width=20,                    # World width
    height=20,                   # World height
    visionWidth=8,               # Agent's visual width
    visionHeight=8,              # Agent's visual height
    timestep=0.1,                # Simulation timestep
    maxEnergy=100.0              # Maximum agent energy
)
```

#### World Control

```python
# Reset world to initial state
world.reset()

# Update world simulation
world.update(timestep)

# Set agent starting position
world.setAgentStart(x, y)

# Set random seed for reproducible simulations
world.setRandomSeed(42)
```

#### Agent Interaction

```python
# Apply motor command to world
world.applyMotorCommand(action, current_time)

# Get sensory percept (what agent sees)
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
```

#### World Object Management

```python
# Add world object (resource, hazard, wall, marker)
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=0.5))

# Remove object from world
world.removeObject(x, y)

# Check if position is valid
if world.isValidPosition(x, y):
    # Can place object here
    pass
```

#### World Information

```python
# World dimensions
world.getWidth()
world.getHeight()

# Simulation state
world.getSimulationTime()
world.getStepsCompleted()

# Agent state
world.getAgentBody()
world.getAgentEnergy()
world.getAgentPosition()
world.getAgentOrientation()

# World properties
world.getMaxEnergy()
world.getTimestep()
world.getRandomSeed()
```

#### World Object Types

```python
# World object type enumeration
pynlm.WorldObjectType.Empty        # Empty space
pynlm.WorldObjectType.Resource     # Resource node (positive value)
pynlm.WorldObjectType.Hazard      # Hazard (negative value)
pynlm.WorldObjectType.Wall        # Impassable barrier
pynlm.WorldObjectType.Marker      # Special marker location
```

### SensoryInput Class

#### Creation and Configuration

```python
# Create sensory input structure
sensory = pynlm.SensoryInput()

# Set various sensory modalities
# Visual
# sensory.setVisualField(x, y, intensity)
# sensory.setVisualEdge(x, y, edge_direction)

# Auditory
sensory.setSound(frequency, amplitude, location)

# Mechanosensory
sensory.setTouch(position, pressure)

# Internal state
sensory.setInternal(state_value, source)
```

#### Access Methods

```python
# Get sensory modality values
visual_data = sensory.getVisual()
auditory_data = sensory.getAuditory()
touch_data = sensory.getTouch()
internal_data = sensory.getInternal()

# Check if sensory data is valid
if sensory.isValid():
    # Process sensory data
    pass
```

### Action Class

#### Creation and Configuration

```python
# Create action structure
action = pynlm.Action()

# Set action type and parameters
action.setType(pynlm.ActionType.MoveForward)
action.setParameter("speed", 1.0)
action.setParameter("duration", 0.5)

# Set target position
action.setTarget(x, y)
```

#### Access Methods

```python
# Get action type
action.getType()

# Get action parameters
action.getParameter("speed")
action.getParameter("duration")

# Get target position
x, y = action.getTarget()

# Check if action is valid
if action.isValid():
    # Execute action
    pass
```

### Enumerations

#### NeuronType

```python
# Neuron type enumeration
pynlm.NeuronType.Excitatory    # Excitatory neuron
pynlm.NeuronType.Inhibitory    # Inhibitory neuron
pynlm.NeuronType.Modulatory    # Neuromodulatory neuron
pynlm.NeuronType.Sensory       # Sensory neuron
pynlm.NeuronType.Motor         # Motor neuron
pynlm.NeuronType.Internal      # Internal neuron
```

#### SynapseType

```python
# Synapse type enumeration
pynlm.SynapseType.Excitatory    # Excitatory synapse
pynlm.SynapseType.Inhibitory    # Inhibitory synapse
pynlm.SynapseType.Modulatory    # Neuromodulatory synapse
pynlm.SynapseType.Electrical   # Electrical synapse
pynlm.SynapseType.GapJunction  # Gap junction
```

#### DevelopmentalStage

```python
# Developmental stage enumeration
pynlm.DevelopmentalStage.Initial        # Initial developmental stage
pynlm.DevelopmentalStage.CriticalPeriod # Critical period of plasticity
pynlm.DevelopmentalStage.Maturation     # Maturation phase
pynlm.DevelopmentalStage.Adult          # Adult stage
pynlm.DevelopmentalStage.Aging          # Aging phase
```

#### ActionType

```python
# Action type enumeration
pynlm.ActionType.Idle              # No action
pynlm.ActionType.MoveForward       # Move forward
pynlm.ActionType.MoveBackward      # Move backward
pynlm.ActionType.TurnLeft          # Turn left
pynlm.ActionType.TurnRight         # Turn right
pynlm.ActionType.Interact          # Interact with object
pynlm.ActionType.Eat               # Consume resource
pynlm.ActionType.Sleep             # Rest/recover
pynlm.ActionTypeExplore           # Explore environment
pynlm.ActionTypeLearn              # Learn from experience
```

#### WorldObjectType

```python
# World object type enumeration
pynlm.WorldObjectType.Empty        # Empty space
pynlm.WorldObjectType.Resource     # Resource node
pynlm.WorldObjectType.Hazard      # Hazardous area
pynlm.WorldObjectType.Wall        # Physical barrier
pynlm.WorldObjectType.Marker      # Special location
```

#### NeuronId, SynapseId, RegionId, PopulationId

```python
# ID types for referencing neural components
neuron_id = pynlm.NeuronId(123)
synapse_id = pynlm.SynapseId(456)
region_id = pynlm.RegionId(789)
population_id = pynlm.PopulationId(999)

# Access ID values
print(neuron_id.value)  # Raw ID value
print(neuron_id.index())  # Index in brain
```

## Utility Functions

### Factory Functions

```python
# Configuration factory
pynlm.createDefaultConfig()      # Create default configuration

# Brain factory  
brain = pynlm.createBrain(config)  # Create brain with config

# Agent factory
agent = pynlm.createAgentBrain(brain)  # Create agent interface

# World factory
world = pynlm.createSimpleWorld()  # Create simple 2D world

# Sensory factory
sensory = pynlm.createSensoryInput()  # Create sensory input

# Action factory
action = pynlm.createAction()  # Create action
```

### Error Classes

```python
# Custom exception types
pynlm.RuntimeError  # General runtime error
pynlm.ConfigurationError  # Configuration-related errors
pynlm.SimulationError  # Simulation state errors
pynlm.ValidationError  # Parameter validation errors
```

### Constants

```python
# Physical and simulation constants
pynlm.PI                # Mathematical constant
pynlm.EPSILON           # Small value for comparisons
pynlm.DEFAULT_TIMESTEP # Default simulation timestep
pynlm.MAX_ENERGY        # Maximum agent energy
pynlm.MIN_TEMPERATURE   # Minimum temperature for physical properties
```

## Configuration Reference

This section provides detailed documentation of all configuration parameters available for the NLM system.

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons in the brain |
| `brain.synapse_density` | float | 0.1 | Probability of connection between neurons |
| `brain.connection_probability` | float | 0.05 | Additional connection probability factor |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuronal firing threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant (ms) |
| `plasticity.stdp.voltage_dependence` | bool | false | Enable voltage-dependent STDP |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.threshold` | float | 0.5 | Correlation threshold for Hebbian learning |
| `plasticity.hebbian.decay_rate` | float | 0.001 | Weight decay rate for Hebbian synapses |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.001 | Rate of structural synapse formation |
| `plasticity.structural.retraction_rate` | float | 0.0005 | Rate of structural synapse elimination |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale factor |
| `neuromod.dopamine.decay_rate` | float | 0.1 | Dopamine signal decay rate |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.threshold` | float | 0.5 | Novelty threshold for curiosity activation |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 1.0 | Novelty detection sensitivity |
| `neuromod.novelty.decay_rate` | float | 0.05 | Novelty signal decay rate |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.critical_period.enabled` | bool | true | Enable critical period of high plasticity |
| `development.critical_period.duration` | float | 100.0 | Duration of critical period (time steps) |
| `development.critical_period.plasticity_multiplier` | float | 10.0 | Plasticity multiplier during critical period |
| `development.maturation.enabled` | bool | true | Enable developmental maturation |
| `development.maturation.rate` | float | 0.001 | Rate of maturation process |
| `development.plasticity_decay.enabled` | bool | true | Enable age-related plasticity decay |
| `development.plasticity_decay.rate` | float | 0.0001 | Rate of plasticity decay with age |

### World Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `world.width` | int | 20 | World width in units |
| `world.height` | int | 20 | World height in units |
| `world.timestep` | float | 0.1 | Simulation timestep (seconds) |
| `world.max_energy` | float | 100.0 | Maximum agent energy |
| `world.vision_width` | int | 8 | Agent's visual field width |
| `world.vision_height` | int | 8 | Agent's visual field height |

### Performance Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `performance.optimization.enable` | bool | true | Enable performance optimizations |
| `performance.optimization.vectorization` | bool | true | Enable SIMD vectorization |
| `performance.optimization.parallel` | bool | true | Enable parallel processing |
| `performance.memory_pool.enable` | bool | true | Enable memory pool allocation |
| `performance.event_queue.size` | int | 10000 | Size of event processing queue |
| `performance.checkpoint.frequency` | int | 100 | Checkpoint save frequency |

## Complete Example: Full Brain-Environment-Agent System

```python
"""
Complete example demonstrating NLM Python API usage.
This example creates a full brain-environment-agent system and runs a simulation.
"""

import pynlm

def runCompleteSimulation(num_steps=1000):
    """Run a complete simulation with all components."""
    
    # 1. Create and configure brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create and configure world
    print("Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=12,
        visionHeight=12,
        timestep=0.05,
        maxEnergy=150.0
    )
    
    # Add environmental resources
    for x in range(0, 30, 6):
        for y in range(0, 30, 6):
            if (x + y) % 8 == 0:
                world.addObject(
                    pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0)
                )
    
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 3. Create and initialize agent
    print("Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all agent subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Run simulation
    print(f"Running simulation for {num_steps} steps...")
    
    simulation_data = {
        'steps': list(range(num_steps)),
        'firing_rates': [],
        'spike_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'actions_taken': [],
        'rewards_received': []
    }
    
    for step in range(num_steps):
        # World update
        world.update(0.05)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Agent processes sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes input
        brain.step(step)
        
        # Agent decodes action
        action = agent.decodeMotorCommand()
        simulation_data['actions_taken'].append(action.getType())
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        simulation_data['rewards_received'].append(reward)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Record metrics
        simulation_data['firing_rates'].append(brain.getAverageFiringRate())
        simulation_data['spike_counts'].append(brain.getTotalSpikeCount())
        simulation_data['curiosity_levels'].append(agent.getCuriosityLevel())
        simulation_data['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Progress output
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print(f"  Action: {action.getType()}")
    
    # 5. Save final state
    print("Saving final state...")
    brain.save("final_brain_state.bin")
    
    return simulation_data

if __name__ == "__main__":
    # Run complete simulation
    results = runCompleteSimulation(500)
    
    # Basic analysis
    print("\nSimulation Complete!")
    print(f"Final firing rate: {results['firing_rates'][-1]:.2f}")
    print(f"Final curiosity: {results['curiosity_levels'][-1]:.3f}")
    print(f"Final novelty: {results['novelty_levels'][-1]:.3f}")
    print(f"Total actions taken: {len([a for a in results['actions_taken'] if a != pynlm.ActionType.Idle])}")
```

## Advanced API Usage

### Complex Agent Behavior Patterns

```python
# Custom agent with goal-directed behavior
class GoalDirectedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.goals = []
        self.goal_weights = []
    
    def addGoal(self, goal_type, weight=1.0):
        """Add a learning goal with priority weight."""
        self.goals.append(goal_type)
        self.goal_weights.append(weight)
    
    def getActionWithGoals(self, percept):
        """Get action considering multiple goals."""
        self.agent.processSensoryInput(percept)
        
        # Get base action from brain
        base_action = self.agent.decodeMotorCommand()
        
        # Apply goal-based modifications
        for goal, weight in zip(self.goals, self.goal_weights):
            base_action = self.applyGoalBias(base_action, goal, weight)
        
        return base_action
```

### Performance Monitoring and Optimization

```python
import time

class PerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.timings = []
        self.memory_usage = []
        self.metrics_history = []
    
    def stepWithMonitoring(self, step_number):
        """Run simulation step with comprehensive monitoring."""
        start_time = time.time()
        
        # Record pre-step metrics
        before_spikes = self.brain.getTotalSpikeCount()
        before_curiosity = self.agent.getCuriosityLevel()
        
        # Execute simulation step
        self.world.update(0.1)
        self.agent.processSensoryInput(self.world.getSensoryPercept())
        self.brain.step(step_number)
        action = self.agent.decodeMotorCommand()
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Record post-step metrics
        end_time = time.time()
        
        self.timings.append(end_time - start_time)
        self.metrics_history.append({
            'step': step_number,
            'time': end_time - start_time,
            'spikes': self.brain.getTotalSpikeCount() - before_spikes,
            'firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'action': action.getType(),
            'world_time': self.world.getSimulationTime()
        })
    
    def getPerformanceReport(self):
        """Generate performance report."""
        if not self.timings:
            return None
        
        return {
            'average_step_time': sum(self.timings) / len(self.timings),
            'min_step_time': min(self.timings),
            'max_step_time': max(self.timings),
            'steps_per_second': len(self.timings) / sum(self.timings) if sum(self.timings) > 0 else 0,
            'total_steps': len(self.timings),
            'final_metrics': self.metrics_history[-1] if self.metrics_history else None
        }
```

### Checkpoint and Recovery System

```python
import os
import json

class CheckpointManager:
    def __init__(self, brain, agent, world, checkpoint_dir):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.checkpoint_dir = checkpoint_dir
        os.makedirs(checkpoint_dir, exist_ok=True)
    
    def saveCheckpoint(self, step_number, metadata=None):
        """Save complete simulation state."""
        checkpoint_file = f"{self.checkpoint_dir}/nlm_checkpoint_{step_number:06d}.bin"
        metadata_file = f"{self.checkpoint_dir}/nlm_metadata_{step_number:06d}.json"
        
        # Save brain state
        self.brain.save(checkpoint_file)
        
        # Save metadata
        metadata = {
            'step': step_number,
            'timestamp': time.time(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'world_time': self.world.getSimulationTime(),
            'additional_metadata': metadata or {}
        }
        
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        return checkpoint_file, metadata_file
    
    def loadCheckpoint(self, checkpoint_file):
        """Load simulation state from checkpoint."""
        # Extract step number from filename
        import re
        match = re.search(r'nlm_checkpoint_(\d+)\.bin', checkpoint_file)
        if not match:
            raise ValueError("Invalid checkpoint filename")
        
        step_number = int(match.group(1))
        
        # Load brain state
        self.brain.load(checkpoint_file)
        
        # Find and load metadata
        metadata_file = checkpoint_file.replace('.bin', '.json')
        if os.path.exists(metadata_file):
            with open(metadata_file, 'r') as f:
                metadata = json.load(f)
        
        return step_number, metadata
```

## Migration Guide

### From v0.0.x to v0.1.0

This section provides migration guidance for upgrading from previous versions.

#### API Changes

1. **Configuration Parameters**: New parameters added for enhanced control
   - Added: `performance.optimization.*` parameters
   - Added: `neuromod.*` parameters
   - Added: `development.*` parameters

2. **Method Signatures**: Some method signatures changed for type safety
   - `brain.step(step)` → `brain.step(step, time=None)` (time is optional)
   - New methods: `brain.getRecentSpikeCount(window)`, `brain.getDevelopmentProgress()`

3. **Agent Subsystems**: New subsystems added
   - Added: `enablePrediction()` and `getPredictionError()`
   - Added: `enableWorkingMemory()` and related methods

#### Code Migration Examples

```python
# Old code
brain.step(step_number)

# New code (backward compatible)
brain.step(step_number)  # time parameter is optional

# New usage with explicit time
brain.step(step_number, time=step_number * 0.1)

# Old code
config.set("brain.neuron_count", 1000)

# New code (type-safe alternatives)
config.setInt("brain.neuron_count", 1000)

# Old code
agent.enableRewardModulation(True)

# New code (with additional validation)
agent.enableRewardModulation(True)
# New parameter validation added
```

#### Behavior Changes

1. **Default Configuration**: Some default values changed for better performance
   - `brain.synapse_density`: 0.1 → 0.05 (less dense by default)
   - `brain.connection_probability`: 0.05 → 0.03 (reduced connection probability)

2. **Memory Management**: Improved memory efficiency
   - Reduced memory footprint for large simulations
   - Added memory pool optimization

3. **Plasticity Rules**: Enhanced learning dynamics
   - Added voltage-dependent STDP option
   - Improved Hebbian learning with decay

## Troubleshooting and Common Issues

### API Usage Errors

#### "AttributeError: module 'pynlm' has no attribute 'createSensoryInput'"

**Issue**: The factory function `createSensoryInput` is not available.

**Solution**:
```python
# Create sensory input directly
sensory = pynlm.SensoryInput()
# Or use pynlm.createDefaultConfig() for configuration
```

#### "TypeError: createBrain() takes 1 positional argument but 2 were given"

**Issue**: Configuration parameter not properly set before brain creation.

**Solution**:
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Set before creating brain
brain = pynlm.createBrain(config)
```

#### "RuntimeError: Brain not initialized"

**Issue**: Brain step() called before initialize().

**Solution**:
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Must be called before any steps
brain.step(0)  # Now safe to step
```

### Performance Issues

#### High Memory Usage

**Issue**: Memory usage exceeds available resources.

**Solution**:
```python
config = pynlm.createDefaultConfig()

# Configure for memory efficiency
config.set("performance.optimization.enable", True)
config.set("performance.memory_pool.enable", True)
config.set("brain.synapse_density", 0.01)  # Sparse connections
config.set("brain.neuron_count", 1000)    # Limit neuron count

# Monitor performance
monitor = PerformanceMonitor(brain, agent, world)
```

#### Slow Simulation Speed

**Issue**: Simulation running slower than expected.

**Solution**:
```python
config = pynlm.createDefaultConfig()

# Enable performance optimizations
config.set("performance.optimization.vectorization", True)
config.set("performance.optimization.parallel", True)
config.set("performance.optimization.threads", 4)

# Use larger timesteps where appropriate
config.set("world.timestep", 0.05)  # Larger timestep = faster simulation
```

### State Management Issues

#### Checkpoint File Errors

**Issue**: Checkpoint saving/loading fails.

**Solution**:
```python
# Ensure directory exists
os.makedirs("checkpoints", exist_ok=True)

# Use absolute paths
checkpoint_file = os.path.abspath("checkpoints/simulation_001.bin")
brain.save(checkpoint_file)

# Verify file exists after saving
if os.path.exists(checkpoint_file):
    print("Checkpoint saved successfully")
```

#### Configuration Serialization

**Issue**: Configuration not properly serialized to file.

**Solution**:
```python
# Use absolute file paths
config.saveToFile("/path/to/config.json")

# Verify configuration loaded
config.loadFromFile("/path/to/config.json")
if config.has("brain.neuron_count"):
    print(f"Loaded neuron count: {config.get('brain.neuron_count')}")
```

## Best Practices

### Programming Guidelines

1. **Always initialize components**: Call `initialize()` before using brain or agent
2. **Use type-safe configuration**: Prefer `setInt()`, `setFloat()`, `setBool()`
3. **Monitor performance**: Use `PerformanceMonitor` for production simulations
4. **Implement checkpointing**: Save state regularly for long simulations
5. **Validate parameters**: Check configuration before creating brain
6. **Clean up resources**: Reset and clean up between simulation episodes

### Simulation Design Patterns

1. **Episode-based Learning**: Reset brain/world between learning episodes
2. **Curriculum Learning**: Gradually increase difficulty
3. **Transfer Learning**: Save/load brain states for transfer
4. **Multi-environment Testing**: Test agents across different environments

### Performance Optimization

1. **Profile before optimizing**: Identify bottlenecks with monitoring
2. **Use appropriate timestep**: Balance accuracy and speed
3. **Enable parallel processing**: Utilize multiple CPU cores
4. **Configure memory pools**: Reduce memory allocation overhead
5. **Batch operations**: Group related operations when possible

## References

- [NLM Documentation Home](/HOW_TO_USE.md)
- [Advanced Features Guide](/Advanced_Features_Guide.md)
- [Easy Usage Guide](/easy_usage.md)
- [Configuration Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)

## Version Information

- **Module Version**: 0.1.0
- **API Compatibility**: Compatible with v0.0.x (backward compatible with migration notes)
- **Python Version**: Requires Python 3.8+
- **Build System**: scikit-build-core with pybind11

## License

MIT License - See LICENSE file for details.
