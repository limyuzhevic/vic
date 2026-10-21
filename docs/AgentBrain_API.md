# NLM AgentBrain API

## AgentBrain Class

The `AgentBrain` class provides a high-level interface for connecting an NLM brain to a world environment, enabling seamless interaction between neural processing and environmental dynamics. It acts as a bridge between the internal brain processes and external world interactions.

### Overview

The AgentBrain class simplifies the integration of NLM brains into interactive simulations by providing:

- **Simplified sensory processing** with automatic input injection
- **Motor command decoding** from neural activity
- **Neuromodulation** integration for reward-based learning
- **Development system** integration for lifespan learning
- **Complete simulation loop** management

It abstracts away the low-level neural operations while maintaining access to the underlying brain's capabilities.

### Key Features

#### Sensory-Motor Integration
- **Automatic sensory input processing**: Direct mapping from world percepts to neural injection
- **Motor command decoding**: Translation of neural activity to actionable commands
- **Feedback loops**: Continuous world-brain interaction

#### Learning and Adaptation
- **Reward modulation**: Integration of reward signals for reinforcement learning
- **Development over time**: Lifespan learning with developmental stages
- **Structural plasticity**: Growing and pruning of neural connections
- **Curiosity-driven exploration**: Motivation for novel experiences

#### Simulation Management
- **Complete episode management**: Setup, execution, and cleanup
- **State management**: Checkpointing and reset capabilities
- **Progress monitoring**: Built-in statistics and logging
- **Error handling**: Robust error management with recovery

### Basic Usage

#### Creating and Initializing an Agent

```python
import pynlm

# Create brain configuration
config = pynlm.createDefaultConfig()

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Create world environment
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Initialize agent with world
agent.initialize(world)

# Verify setup
print(f"Expected sensory input size: {agent.getSensoryInputSize()}")
print(f"Expected motor output size: {agent.getMotorOutputSize()}")
```

#### Running a Complete Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev stage: {agent.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

#### Minimal Agent Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable basic features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Agent with Checkpoints

```python
import pynlm

def run_long_experiment(config, max_steps=5000):
    """Run a long experiment with checkpointing."""
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with checkpoints
    checkpoint_interval = 1000
    
    for step in range(max_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Save checkpoint
        if step % checkpoint_interval == 0:
            brain.save(f"checkpoint_{step}.bin")
            print(f"Checkpoint saved at step {step}")
    
    # Save final result
    brain.save("final_result.bin")
    print("Experiment completed")

# Run experiment
config = pynlm.createDefaultConfig()
run_long_experiment(config, max_steps=5000)
```

### AgentBrain Class Methods

#### Construction and Destruction

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Agent is automatically destroyed when out of scope
```

#### Initialization

```python
# Initialize agent with world
agent.initialize(world)

# Re-initialize for new episode
agent.reset()
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory percept (automatic injection into brain)
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Decode motor command from brain neural activity
action = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
# Apply reward-based neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development

```python
# Update development system
agent.updateDevelopment(timestep)

# Get developmental stage
stage = agent.getDevelopmentalStage()
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Check if subsystem is enabled
reward_enabled = agent.isRewardModulationEnabled()
plasticity_enabled = agent.isStructuralPlasticityEnabled()
```

#### Statistics and Monitoring

```python
# Neuromodulation levels
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

#### Access to Underlying Brain

```python
# Get the underlying brain object
brain = agent.getBrain()

# The brain can be used directly if needed
brain.step(step)
brain.injectCurrent(neuron_id, current)
```

### AgentBrain Configuration

#### Recommended Subsystem Combinations

```python
# Basic learning agent
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
# agent.enableDevelopment(True)  # Optional
# agent.enableCuriosity(True)    # Optional

# Fast reactive agent
agent.enableRewardModulation(True)
agent.disableStructuralPlasticity()  # Fast, less flexible
agent.disableDevelopment()           # Simple, fixed architecture
agent.disableCuriosity()             # Reactive, not exploratory

# Advanced learning agent
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Curiosity-driven exploration agent
agent.disableRewardModulation()     # Learn from curiosity, not rewards
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)
```

### Agent Brain Interface Patterns

#### Pattern 1: Simple Reflex Agent
```python
def run_reflex_agent(config, steps=1000):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Basic setup only
    agent.enableRewardModulation(True)
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 2: Learning Agent with Development
```python
def run_developmental_agent(config, stages=5):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Full learning setup
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run in developmental stages
    steps_per_stage = 500
    for stage in range(stages):
        brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
        
        for step in range(steps_per_stage):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
```

#### Pattern 3: Exploration Agent
```python
def run_exploration_agent(config, exploration_steps=2000):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Curiosity-driven exploration
    agent.disableRewardModulation()
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    # Exploration phase
    for step in range(exploration_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Exploration learning
        novelty = agent.getNoveltyLevel()
        curiosity = agent.getCuriosityLevel()
        agent.applyRewardModulation(novelty, curiosity)
        agent.updateDevelopment(0.1)
        
        if step % 500 == 0:
            print(f"Exploration step {step}: Novelty {novelty:.3f}, Curiosity {curiosity:.3f}")
```

### AgentBrain Best Practices

#### 1. Subsystem Management
```python
# Enable only what you need for performance
agent.enableRewardModulation(True)      # Essential for learning
agent.enableStructuralPlasticity(True)  # Essential for adaptation
agent.enableDevelopment(True)           # For lifespan learning
agent.enableCuriosity(True)             # For exploration (optional)
```

#### 2. Memory Management
```python
# Reset between episodes to free memory
for episode in range(100):
    agent.reset()
    agent.initialize(world)
    
    # Run episode
    for step in range(500):
        # ... simulation steps ...
    
    # Save checkpoint if needed
    if episode % 10 == 0:
        agent.getBrain().save(f"episode_{episode}.bin")
```

#### 3. Configuration Optimization
```python
# Optimize for your use case
config = pynlm.createDefaultConfig()

# Fast simulation
config.set("brain.neuron_count", 100)
config.set("simulation_timestep", 0.01)

# Research simulation
config.set("brain.neuron_count", 5000)
config.set("simulation_timestep", 0.001)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.structural.enable", true)
```

#### 4. Error Handling
```python
def safe_agent_simulation(config, max_steps=1000):
    """Safely run an agent simulation."""
    brain = None
    agent = None
    world = None
    
    try:
        # Create components
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        
        # Initialize
        world.reset()
        agent.initialize(world)
        
        # Run simulation
        for step in range(max_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
    except Exception as e:
        print(f"Simulation error: {e}")
        # Reset on error
        if agent:
            agent.reset()
        raise
        
    finally:
        # Cleanup
        if brain:
            brain.reset()
```

### AgentBrain Performance Tips

#### 1. For Real-time Applications
```python
# Minimize overhead for real-time
config.set("brain.neuron_count", 50)           # Small network
config.set("simulation_timestep", 0.05)        # Larger timestep
agent.disableStructuralPlasticity()            # No connection changes
agent.disableDevelopment()                     # Fixed architecture
agent.disableCuriosity()                       # No exploration
```

#### 2. For Long-term Learning
```python
# Optimize for long learning periods
config.set("brain.neuron_count", 500)          # Medium size
config.set("simulation_timestep", 0.001)       # Accurate timestep
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)         # Growing connections
agent.enableDevelopment(True)                  # Changing capabilities
agent.enableCuriosity(True)                    # Encouraging exploration
```

### AgentBrain Troubleshooting

#### "Agent isn't learning"
```python
# Check if reward modulation is enabled
if not agent.isRewardModulationEnabled():
    agent.enableRewardModulation(True)

# Check if structural plasticity is enabled
if not agent.isStructuralPlasticityEnabled():
    agent.enableStructuralPlasticity(True)
```

#### "Agent isn't exploring"
```python
# Check if curiosity is enabled
if not agent.isCuriosityEnabled():
    agent.enableCuriosity(True)
```

#### "Agent performance is poor"
```python
# Check developmental stage
stage = agent.getDevelopmentalStage()
print(f"Current developmental stage: {stage}")

# Consider adjusting configuration
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)
```

### Advanced AgentBrain Features

#### Custom Agent Classes
```python
class CustomAgent:
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.configure_world()
        
        # Custom setup
        self.setup_agent()
    
    def configure_world(self):
        self.world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    
    def setup_agent(self):
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_episode(self, steps=1000):
        self.agent.reset()
        self.agent.initialize(self.world)
        
        for step in range(steps):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Custom learning logic
            self.custom_learning(step)
    
    def custom_learning(self, step):
        # Implement custom learning algorithms
        reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        
        # Custom learning rule
        custom_reward = reward + curiosity * 0.1 + novelty * 0.05
        self.agent.applyRewardModulation(custom_reward, reward)
        self.agent.updateDevelopment(0.1)
```

This comprehensive AgentBrain documentation provides everything needed to effectively use the AgentBrain interface in NLM simulations, from basic usage to advanced features and best practices.