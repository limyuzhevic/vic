# NLM Python API Reference

This document provides a comprehensive reference for the NLM Python API, including detailed explanations of all classes, methods, and their usage.

## Module Overview

The `pynlm` module provides a Python interface to the NLM (Neural Learning Machine) C++ neural simulation framework. It offers classes for creating and controlling virtual brains, sensory processing, motor actions, and environmental interaction.

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## Core Classes

### 1. Config Class

The `Config` class manages configuration settings for NLM systems.

#### Constructor
```python
config = pynlm.Config()  # Default empty configuration
```

#### Key Methods

**Loading and Saving**
```python
# Load configuration from file
config.loadFromFile("config.json")

# Load configuration from command line arguments  
config.loadFromArgs(2, ["--neuron_count", "1000", "--learning_rate", "0.001"])

# Save configuration to file
config.saveToFile("config.json")
```

**Configuration Management**
```python
# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enabled", True)
config.set("neuromod.dopamine.scale", 1.5)

# Get configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 500)

# Get all keys
keys = config.getKeys()

# Remove a key
config.remove("brain.synapse_density")

# Clear all configuration
config.clear()
```

**Query Methods**
```python
# Check if a key exists
if config.has("brain.neuron_count"):
    count = config.getOr("brain.neuron_count", 1000)

# Get all configuration keys as a list
all_keys = config.getKeys()

# Get summary string
summary = config.summary()
```

#### Configuration Parameters

**Brain Configuration**
```python
# Neuron parameters
config.set("brain.neuron_count", 1000)          # Total number of neurons
config.set("brain.v_thresh", -50.0)            # Threshold potential (mV)
config.set("brain.v_rest", -70.0)              # Resting potential (mV)
config.set("brain.v_reset", -75.0)             # Reset potential (mV)
config.set("brain.tau_mem", 20.0)              # Membrane time constant (ms)
config.set("brain.tau_ref", 2.0)               # Refractory period (ms)

# Network parameters
config.set("brain.synapse_density", 0.1)        # Connection probability
config.set("brain.initial_weight_mean", 0.5)   # Mean initial weight
config.set("brain.initial_weight_std", 0.1)    # Weight standard deviation

# Regional parameters
config.set("brain.region_count", 1)            # Number of brain regions
config.set("brain.connection_probability", 0.05)  # Inter-region connectivity
```

**Plasticity Configuration**
```python
# STDP parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)   # LTP time constant
config.set("plasticity.stdp.tau_minus", 20.0) # LTD time constant

# Hebbian parameters
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.threshold", 0.5)  # Co-activation threshold

# Structural plasticity
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.growth_rate", 0.0001)
config.set("plasticity.structural.pruning_rate", 0.00001)
```

**Neuromodulation Configuration**
```python
# Dopamine system
config.set("neuromod.dopamine.scale", 1.0)       # Dopamine modulation strength
config.set("neuromod.dopamine.baseline", 0.1)    # Baseline dopamine level

# Curiosity system
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.sensitivity", 0.5)  # Novelty sensitivity

# Novelty detection
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.8)     # Novelty threshold

# Prediction error
config.set("neuromod.prediction.error.enabled", True)
config.set("neuromod.prediction.error.weight", 0.3)
```

**Development Configuration**
```python
# Developmental stages
config.set("development.stage.initial.age", 0.0)     # Start of initial stage
config.set("development.stage.critical.period.end", 10.0)  # End of critical period
config.set("development.stage.maturation.begin", 10.0)    # Start of maturation
config.set("development.stage.adult.age", 50.0)         # Start of adulthood

# Development parameters
config.set("development.plasticity.rate", 1.0)      # Baseline plasticity
config.set("development.plasticity.modifier", 1.2)   # Plasticity modifier
config.set("development.synapse.growth.rate", 0.0001)
config.set("development.synapse.pruning.rate", 0.00001)
```

### 2. Sensory Input Classes

Base class for all sensory input types. Subclasses provide specific sensory modalities.

#### Vision Class
```python
# Create vision sensory input
vision = pynlm.Vision(width=8, height=8, channels=3)

# Set vision data
vision_data = [0.1] * 8 * 8 * 3  # 8x8 RGB image
vision.setData(vision_data)

# Access vision properties
vision_width = vision.getWidth()      # 8
vision_height = vision.getHeight()    # 8
vision_channels = vision.getChannels() # 3

# Use vision
percept = pynlm.SensoryPercept()
percept.setVision(vision)
```

#### Audio Class
```python
# Create audio sensory input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set audio data
audio_samples = [0.0] * 1024  # Audio waveform
audio.setData(audio_samples)

# Configure audio properties
audio.setSampleRate(48000)
sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()
```

#### InternalSignals Class
```python
# Create internal signals sensory input
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.5)  # Energy level
internal.addSignal(0.8)  # Stress level
internal.addSignal(0.2)  # Hunger level

# Clear all signals
internal.clearSignals()
```

### 3. Action Class

Represents motor actions that can be executed in the environment.

#### Constructor
```python
# Create action with type only
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with type and parameters
action = pynlm.Action(
    pynlm.ActionType.Custom,
    [0.5, 0.8, 1.2]  # Custom parameters
)
```

#### Key Methods
```python
# Get and set action type
action_type = action.getType()
action.setType(pynlm.ActionType.TurnLeft)

# Get and set parameters
parameters = action.getParameters()
action.setParameters([0.3, 0.7, 1.5])

# Get action name
name = action.getName()  # "MoveForward", "TurnLeft", etc.

# Clone action (for creating variations)
action_copy = action.clone()
```

### 4. SensoryPercept Class

Container for all sensory inputs from the environment.

#### Constructor
```python
percept = pynlm.SensoryPercept()
```

#### Key Methods
```python
# Set and get individual sensory modalities
percept.setVision(vision)
vision = percept.getVision()

percept.setTouch(touch)
touch = percept.getTouch()

percept.setInternal(internal)
internal = percept.getInternal()

percept.setProprioception(proprioception)
proprioception = percept.getProprioception()

percept.setAudio(audio)
audio = percept.getAudio()

# Get all sensory data at once
all_signals = percept.getAllSignals()

# Set and get timestamp
percept.setTimestamp(1.5)
timestamp = percept.getTimestamp()
```

### 5. SimpleWorld Class

Represents a 2D simulation environment.

#### Constructor
```python
world = pynlm.createSimpleWorld()
```

#### Configuration
```python
# Configure world dimensions and sensory capabilities
world.configure(
    width=20,           # World width in cells
    height=20,          # World height in cells
    visionWidth=8,      # Agent vision width
    visionHeight=8      # Agent vision height
)
```

#### Control Methods
```python
# Reset world to initial state
world.reset()

# Update world simulation
world.update(timestep=0.1)  # Update for 0.1 seconds

# Set agent starting position
world.setAgentStart(x=10.0, y=10.0)
```

#### Interaction Methods
```python
# Apply motor command to world
world.applyMotorCommand(
    pynlm.MotorCommand(pynlm.MotorCommand.MoveForward),
    1.5  # Current simulation time
)
```

#### State Access
```python
# Get sensory percept observed by agent
sensory_percept = world.getSensoryPercept()

# Get agent body state
agent_body = world.getAgentBody()

# Add or remove world objects
world_object = pynlm.WorldObject(
    x=5.0,
    y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=1.0
)
world.addObject(world_object)

# Check if position is valid
is_valid = world.isValidPosition(x=10.0, y=10.0)
```

#### Properties
```python
# Get world dimensions
width = world.getWidth()
height = world.getHeight()

# Get agent state
agent_body = world.getAgentBody()
agent_x = agent_body.x
agent_y = agent_body.y
agent_orientation = agent_body.orientation
agent_velocity_x = agent_body.velocityX
agent_velocity_y = agent_body.velocityY
agent_energy = agent_body.energy
agent_health = agent_body.health
agent_age = agent_body.age

# Get simulation time
simulation_time = world.getSimulationTime()

# Get configuration properties
max_energy = world.getMaxEnergy()
energy_decay_rate = world.getEnergyDecayRate()

# Set configuration properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
```

### 6. AgentBrain Class

Connects NLM brain to world interface, handling sensory processing and motor decoding.

#### Constructor
```python
agent = pynlm.createAgentBrain(brain)
```

#### Initialization
```python
# Initialize agent with world
agent.initialize(world)
```

#### Sensory Processing
```python
# Process sensory input and inject into brain
agent.processSensoryInput(sensory_percept)
```

#### Motor Decoding
```python
# Decode brain motor activity into motor command
motor_command = agent.decodeMotorCommand()

# Use motor command to interact with world
world.applyMotorCommand(motor_command, world.getSimulationTime())
```

#### Neuromodulation
```python
# Apply reward-based modulation to brain
agent.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

#### Development
```python
# Update developmental state
agent.updateDevelopment(timestep=0.1)

# Get developmental stage
stage = agent.getDevelopmentalStage()
```

#### Configuration Flags
```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check if subsystems are enabled
is_reward_enabled = agent.isRewardModulationEnabled()
is_development_enabled = agent.isDevelopmentEnabled()
is_curiosity_enabled = agent.isCuriosityEnabled()
```

#### Statistics and State
```python
# Get neuromodulation levels
neuromodulation_level = agent.getNeuromodulationLevel()
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()

# Get brain reference (for direct access if needed)
brain = agent.getBrain()
```

### 7. Brain Class

Central neural simulation brain implementing spiking neural computation.

#### Constructor
```python
brain = pynlm.createBrain(config)
```

#### Initialization
```python
# Initialize brain with configuration
brain.initialize()
```

#### Simulation Control
```python
# Run simulation steps
brain.step(step_number)                    # Simple step with default timestep
brain.step(step_number, time)              # Step with specific timestamp
brain.reset()                              # Reset brain to initial state
```

#### Input/Output
```python
# Inject sensory input
brain.receiveSensoryInput(sensory_input)

# Inject current into specific neuron
brain.injectCurrent(neuron_id, current_value)

# Inject current into all neurons of a specific type
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, current_value)

# Get motor action based on neural activity
action = brain.produceAction()
```

#### Statistics
```python
# Population statistics
brain.getTotalNeuronCount()        # Total number of neurons
brain.getTotalSynapseCount()       # Total number of synapses
brain.getActiveNeuronCount()       # Neurons currently active
brain.getFiringNeuronCount()      # Neurons currently firing
brain.getAverageFiringRate()       # Average firing rate across all neurons
brain.getExcitationInhibitionRatio()  # E/I balance ratio

# Temporal statistics
brain.getTotalSpikeCount()         # Total spikes generated
brain.getPendingSpikeEventCount()  # Number of pending spike events
```

#### State Management
```python
# Save brain state to file
brain.save("checkpoint.bin")

# Load brain state from file
brain.load("checkpoint.bin")
```

#### Regional Management
```python
# Add a new brain region
region_id = brain.addRegion("cortex")

# Get region by ID
region = brain.getRegion(region_id)

# Get all regions
regions = brain.getRegions()

# Get region statistics
region_count = brain.getRegionCount()
all_region_ids = brain.getRegionIds()
```

#### Development
```python
# Get current developmental stage
stage = brain.getDevelopmentalStage()

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
```

#### Configuration Access
```python
# Get configuration used by this brain
brain_config = brain.getConfig()

# Get random generator (for reproducibility)
random_generator = brain.getRandomGenerator()
```

#### Logging
```python
# Get detailed brain status
brain.logStatus()
```

### 8. Utility Functions

#### Factory Functions
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create new brain
brain = pynlm.createBrain(config)

# Create new simple world
world = pynlm.createSimpleWorld()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
```

#### Constants
```python
# Neuron identifiers
invalid_neuron_id = pynlm.INVALID_NEURON_ID

# Synapse identifiers  
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID

# Region identifiers
invalid_region_id = pynlm.INVALID_REGION_ID

# Population identifiers
invalid_population_id = pynlm.INVALID_POPULATION_ID
```

#### Enumerations
```python
# Neuron types
pynlm.NeuronType.Excitatory    # Positive, depolarizing
pynlm.NeuronType.Inhibitory    # Negative, hyperpolarizing
pynlm.NeuronType.Sensory       # Receives sensory input
pynlm.NeuronType.Motor         # Generates motor output
pynlm.NeuronType.Modulatory    # Modulates other neurons
pynlm.NeuronType.Internal      # Internal brain processing

# Developmental stages
pynlm.DevelopmentalStage.Initial       # Newborn, high plasticity
pynlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
pynlm.DevelopmentalStage.Maturation    # Growing into adulthood
pynlm.DevelopmentalStage.Adult         # Full adult capabilities
pynlm.DevelopmentalStage.Aging         # Age-related decline

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest

# Motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Advanced Usage Examples

### Example 1: Custom Environment with Learning

```python
import pynlm

def custom_environment():
    """Create a custom environment with resource management."""
    
    # Create world with resource distribution
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    
    # Add resources at random positions
    import random
    for i in range(10):
        x = random.uniform(1, 14)
        y = random.uniform(1, 14)
        value = random.uniform(0.5, 2.0)
        world_object = pynlm.WorldObject(
            x=x,
            y=y,
            type=pynlm.WorldObjectType.Resource,
            value=value
        )
        world.addObject(world_object)
    
    # Add hazards
    for i in range(5):
        x = random.uniform(0, 15)
        y = random.uniform(0, 15)
        hazard = pynlm.WorldObject(
            x=x,
            y=y,
            type=pynlm.WorldObjectType.Hazard,
            value=1.0
        )
        world.addObject(hazard)
    
    return world

def agent_behavior_loop(world, agent, num_steps):
    """Custom agent behavior with memory."""
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        agent.getBrain().step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute custom reward
        reward = compute_environmental_reward(world, agent)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log performance
        if step % 100 == 0:
            brain = agent.getBrain()
            print(f"Step {step}: Reward={reward:.3f}, "
                  f"Neurons={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")

def compute_environmental_reward(world, agent):
    """Compute reward based on environment interaction."""
    brain = agent.getBrain()
    world_obj = world.getAgentBody()
    
    # Reward for collecting resources
    reward = 0.0
    percept = world.getSensoryPercept()
    if percept.getTouch():
        # Check if close to resources
        for i in range(len(percept.getTouch())):
            if percept.getTouch()[i] > 0.7:  # Close to object
                reward += 1.0
    
    # Penalty for collisions with hazards
    if percept.getInternal():
        hazard_signals = [s for s in percept.getInternal() if s > 0.8]
        reward -= len(hazard_signals) * 2.0
    
    # Curiosity bonus
    reward += agent.getCuriosityLevel() * 0.5
    
    # Development bonus (better as brain matures)
    if brain.getDevelopmentalStage() >= pynlm.DevelopmentalStage.Adult:
        reward += 0.5
    
    return max(0.0, reward)
```

### Example 2: Transfer Learning Between Tasks

```python
import pynlm
import pickle

class TransferLearningAgent:
    """Agent that can transfer knowledge between tasks."""
    
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Memory for transferred knowledge
        self.transferred_weight = 0.0
        self.task_memory = {}
    
    def train_on_task(self, world, task_name, num_steps):
        """Train on a specific task."""
        
        # Run task training
        for step in range(num_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Compute task-specific reward
            reward = self.compute_task_reward(world, task_name)
            self.agent.applyRewardModulation(reward, 0.0)
            
            self.agent.updateDevelopment(0.1)
            
            # Store task-specific experience
            self.task_memory[task_name].append({
                'step': step,
                'reward': reward,
                'brain_state': self.save_brain_state()
            })
        
        # After training, transfer knowledge
        self.transfer_knowledge(task_name)
    
    def transfer_knowledge(self, source_task):
        """Transfer knowledge from source task to new tasks."""
        if source_task in self.task_memory:
            # Get key experiences from source task
            experiences = self.task_memory[source_task]
            
            # Compute average performance
            avg_reward = sum(exp['reward'] for exp in experiences) / len(experiences)
            
            # Apply to new tasks as initial bias
            self.transferred_weight = avg_reward
            
            # Adjust configuration based on learned parameters
            self.adjust_configuration_for_task(source_task, avg_reward)
    
    def adjust_configuration_for_task(self, task_name, avg_performance):
        """Adjust configuration based on task performance."""
        
        # If performance is good, increase related parameters
        if avg_performance > 0.5:
            # Increase learning rate for similar tasks
            self.config.set("plasticity.stdp.learning_rate", 0.002)
            # Increase exploration for novel tasks
            self.config.set("neuromod.curiosity.sensitivity", 0.8)
        else:
            # If performance is poor, be more conservative
            self.config.set("plasticity.stdp.learning_rate", 0.0005)
            # Reduce exploration to focus on known tasks
            self.config.set("neuromod.curiosity.sensitivity", 0.3)
    
    def save_brain_state(self):
        """Save current brain state for transfer."""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
    
    def load_brain_state(self, state):
        """Load brain state from transfer memory."""
        # This would typically involve actual brain state serialization
        pass

def example_transfer_learning():
    """Example of transfer learning between navigation and foraging tasks."""
    
    # Create base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.set("brain.neuron_count", 800)
    
    # Create navigation agent
    nav_agent = TransferLearningAgent(base_config)
    
    # Create foraging agent (shares configuration knowledge)
    foraging_agent = TransferLearningAgent(base_config)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Train on navigation task
    print("Training on navigation task...")
    nav_agent.train_on_task(world, "navigation", 1000)
    
    # Transfer knowledge to foraging task
    print("Transferring knowledge to foraging task...")
    foraging_agent.train_on_task(world, "foraging", 1000)
    
    # Evaluate both agents
    print("Evaluating agents...")
    nav_performance = evaluate_agent(nav_agent, world, "navigation")
    foraging_performance = evaluate_agent(foraging_agent, world, "foraging")
    
    print(f"Navigation performance: {nav_performance:.3f}")
    print(f"Foraging performance: {foraging_performance:.3f}")
```

### Example 3: Neural Architecture Search

```python
import pynlm
import numpy as np

class NeuralArchitectureSearch:
    """Neural architecture search for NLM configurations."""
    
    def __init__(self, config_space):
        self.config_space = config_space
        self.search_history = []
        self.best_config = None
        self.best_score = -float('inf')
    
    def search_config_space(self, world, num_trials=50):
        """Search for optimal configuration."""
        
        for trial in range(num_trials):
            # Generate random configuration
            config = self.suggest_config()
            
            # Evaluate configuration
            score = self.evaluate_config(config, world, num_steps=500)
            
            # Record search result
            self.search_history.append({
                'trial': trial,
                'config': config,
                'score': score
            })
            
            # Update best configuration
            if score > self.best_score:
                self.best_score = score
                self.best_config = config
                
                # Save best configuration
                self.save_best_config()
            
            # Print progress
            if trial % 10 == 0:
                print(f"Trial {trial}: Score = {score:.3f}, Best = {self.best_score:.3f}")
        
        return self.best_config
    
    def suggest_config(self):
        """Suggest a configuration from the search space."""
        config = pynlm.createDefaultConfig()
        
        # Sample from configuration space
        config.set("brain.neuron_count", np.random.choice([500, 800, 1000, 1500]))
        config.set("brain.synapse_density", np.random.uniform(0.05, 0.2))
        config.set("plasticity.stdp.learning_rate", np.random.uniform(0.0001, 0.01))
        config.set("plasticity.hebbian.threshold", np.random.uniform(0.1, 1.0))
        config.set("neuromod.curiosity.sensitivity", np.random.uniform(0.1, 1.0))
        config.set("development.plasticity.rate", np.random.uniform(0.5, 2.0))
        
        # Randomly enable/disable features
        if np.random.random() < 0.8:
            config.set("plasticity.stdp.enable", True)
        if np.random.random() < 0.8:
            config.set("neuromod.curiosity.enable", True)
        if np.random.random() < 0.8:
            config.set("development.enable", True)
        
        return config
    
    def evaluate_config(self, config, world, num_steps):
        """Evaluate a configuration."""
        
        # Create brain with configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        total_reward = 0.0
        for step in range(num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Compute task reward
            reward = compute_task_reward(world, agent)
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            total_reward += reward
        
        # Compute final score
        avg_reward = total_reward / num_steps
        
        # Consider exploration (curiosity bonus)
        curiosity_bonus = agent.getCuriosityLevel() * 0.5
        
        # Consider learning efficiency
        learning_efficiency = avg_reward / (brain.getTotalNeuronCount() / 1000)
        
        # Combine metrics
        score = avg_reward * 0.6 + curiosity_bonus * 0.2 + learning_efficiency * 0.2
        
        return score
    
    def save_best_config(self):
        """Save the best configuration found."""
        if self.best_config:
            summary = self.best_config.summary()
            with open("best_config.txt", "w") as f:
                f.write(summary)
            print(f"Saved best configuration to best_config.txt")

def example_neural_architecture_search():
    """Example of neural architecture search for NLM."""
    
    # Define configuration space
    config_space = {
        'brain.neuron_count': [500, 800, 1000, 1500, 2000],
        'brain.synapse_density': [0.05, 0.1, 0.15, 0.2],
        'plasticity.stdp.learning_rate': [0.0001, 0.001, 0.01],
        'neuromod.curiosity.sensitivity': [0.1, 0.5, 1.0],
        'development.plasticity.rate': [0.5, 1.0, 2.0]
    }
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    
    # Create NAS instance
    nas = NeuralArchitectureSearch(config_space)
    
    # Run search
    print("Starting neural architecture search...")
    best_config = nas.search_config_space(world, num_trials=50)
    
    # Show best configuration
    print(f"\nBest configuration found:")
    print(best_config.summary())
    print(f"Best score: {nas.best_score:.3f}")
```