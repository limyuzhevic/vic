# NLM - Complete Usage Guide

## Overview

NLM (Neural Learning Machine) is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

### Core Concepts

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## Quick Start

### The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

### Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Complete Agent Example

```python
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
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Core Classes

### Brain Class

#### Creation
```python
brain = pynlm.createBrain(config)
```

#### Initialization
```python
brain.initialize()
```

#### Simulation
```python
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state
```

#### Input/Output
```python
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action
```

#### Statistics
```python
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
```

#### State
```python
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
```

#### Regions
```python
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### Config Class

#### Create
```python
config = pynlm.createDefaultConfig()
```

#### Query
```python
if config.has("brain.neuron_count"):
    keys = config.getKeys()
```

#### Modify
```python
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
```

#### File I/O
```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### Convenience Methods
```python
# Set multiple values at once
config.setMultiple({
    "brain.neuron_count": 1000,
    "learning.rate": 0.001,
    "plasticity.enabled": True
})

# Get with type conversion
neuron_count = config.get("brain.neuron_count", "int")
learning_rate = config.get("learning.rate", "float")

# Validate key
if config.validateKey("brain.neuron_count", "neuron_*"):
    print("Valid key!")

# Validate type
if config.validateType("brain.neuron_count", "int"):
    print("Value is an integer!")

# Get config info
info = config.getConfigInfo()
for key, type_info in info.items():
    print(f"{key}: {type_info}")
```

### AgentBrain Class

#### Creation
```python
agent = pynlm.createAgentBrain(brain)
```

#### Initialization
```python
agent.initialize(world)
```

#### Sensory Processing
```python
agent.processSensoryInput(percept)
```

#### Motor Decoding
```python
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation
```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development
```python
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
```

#### Subsystem Enable/Disable
```python
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
```

#### Statistics
```python
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class

#### Creation
```python
world = pynlm.createSimpleWorld()
```

#### Configuration
```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### Control
```python
world.reset()
world.update(timestep)
world.setRandomSeed(42)
```

#### Agent Interaction
```python
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)
```

#### State Access
```python
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

#### World Objects
```python
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

#### Properties
```python
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

## Enumerations

### Neuron Types
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more
```

### World Object Types
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

## Advanced Usage Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)              # Explore new things
```

### Pattern 4: Working with Different Sensor Types

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create different sensory inputs
vision = pynlm.Vision(8, 8, 3)  # 8x8 RGB image
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8] * 3)  # Example RGB data

audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setData([0.1, 0.2, 0.3, 0.4] * 256)  # Example audio data

internal = pynlm.InternalSignals()
internal.addSignal(0.5)  # Add some internal signal
internal.addSignal(-0.3)
internal.addSignal(0.9)

# Create a percept with all sensors
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setAudio(audio)
percept.setInternal(internal)

# Process the percept
agent.processSensoryInput(percept)

# Run brain step
brain.step(0)

# Get action
action = agent.decodeMotorCommand()
```

### Pattern 5: Configuration Management

```python
import pynlm
import json

# Create and configure
config = pynlm.createDefaultConfig()

# Set multiple values with validation
config.setMultiple({
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "learning.stdp.enabled": True,
    "learning.hebbian.enabled": True,
    "neuromod.curiosity.enabled": True
})

# Validate configuration
valid_keys = ["brain.neuron_count", "brain.synapse_density", "learning.stdp.enabled"]
for key in valid_keys:
    if config.validateKey(key):
        print(f"✓ Key '{key}' is valid")
    else:
        print(f"✗ Key '{key}' is invalid")

# Check types
int_keys = ["brain.neuron_count"]
float_keys = ["brain.synapse_density"]
str_keys = ["learning.stdp.enabled"]

for key in int_keys:
    if config.validateType(key, "int"):
        value = config.get(key, "int")
        print(f"✓ {key} = {value} (int)")

for key in float_keys:
    if config.validateType(key, "float"):
        value = config.get(key, "float")
        print(f"✓ {key} = {value} (float)")

for key in str_keys:
    if config.validateType(key, "str"):
        value = config.get(key, "str")
        print(f"✓ {key} = {value} (str)")

# Save configuration
save_path = "my_brain_config.json"
if config.saveToFile(save_path):
    print(f"✓ Configuration saved to {save_path}")

# Load configuration
load_path = "my_brain_config.json"
if config.loadFromFile(load_path):
    print(f"✓ Configuration loaded from {load_path}")
    
    # Verify loaded values
    print("Loaded values:")
    for key in valid_keys:
        value = config.get(key)
        print(f"  {key}: {value}")
```

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure the agent's motor neurons are active

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that the brain has been initialized

### "Configuration file not found"
- Use absolute paths or ensure the file is in the current directory
- Check file permissions
- Use `config.validate_key()` before loading

### "Invalid configuration value"
- Use `config.validate_type()` to check value types
- Use `config.get_with_type()` for type-safe retrieval
- Check configuration schema

## Performance Tips

### For Faster Simulation
```python
# Increase neuron count for more realistic behavior
config.set("brain.neuron_count", 5000)

# Enable parallel processing
config.set("performance.enable_parallel", True)

# Optimize memory usage
config.set("memory.enable_pools", True)
```

### For Better Development
```python
# Enable development for learning
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Use appropriate simulation time step
world.update(0.01)  # Smaller time steps for better accuracy
```

### For Large Scale Simulations
```python
# Use checkpointing for long simulations
brain.save("checkpoint_001.bin")

# Resume from checkpoint
brain.load("checkpoint_001.bin")

# Monitor performance
brain.logStatus()
```

## Advanced Features

### Working with Multiple Brains
```python
def run_multiple_agents(num_agents, world, steps):
    brains = []
    agents = []
    
    # Create multiple brains with different configurations
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        # Different configurations for each brain
        config.set("brain.neuron_count", 1000 + i * 100)
        config.set("brain.region_count", 1 + i)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        brains.append(brain)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents.append(agent)
    
    # Run simulation with all agents
    for step in range(steps):
        world.update(0.1)
        
        for i, (brain, agent) in enumerate(zip(brains, agents)):
            # Each agent can have different behavior
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step + i)  # Different step numbers
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
```

### Custom Configuration Schemas
```python
# Define your own configuration schema
def create_brain_config(width, height, vision_width, vision_height):
    config = pynlm.createDefaultConfig()
    
    # Set basic brain parameters
    config.set("brain.neuron_count", width * height * 10)
    config.set("brain.region_count", 2)
    config.set("brain.synapse_density", 0.1)
    
    # Set world parameters
    config.set("world.width", width)
    config.set("world.height", height)
    config.set("world.vision_width", vision_width)
    config.set("world.vision_height", vision_height)
    
    # Set learning parameters
    config.set("learning.stdp.enabled", True)
    config.set("learning.stdp.learning_rate", 0.01)
    config.set("learning.hebbian.enabled", True)
    
    # Set neuromodulation parameters
    config.set("neuromod.curiosity.enabled", True)
    config.set("neuromod.novelty.enabled", True)
    
    # Set development parameters
    config.set("development.enable", True)
    config.set("development.critical_period", 100)
    
    return config

# Use the custom configuration
config = create_brain_config(20, 20, 8, 8)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Error Handling
```python
import pynlm

def safe_simulation(config, world, max_steps):
    """Run a simulation with comprehensive error handling."""
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(max_steps):
            try:
                # Update world
                world.update(0.1)
                
                # Get and process sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode and apply action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Check for errors
                if brain.getFiringNeuronCount() == 0 and step > 100:
                    print("Warning: No neurons firing")
                
                # Log progress
                if step % 100 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                    
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Decide whether to continue or stop
                if step < 10:  # Too early in simulation
                    raise
                else:
                    print("Continuing simulation despite error")
        
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        # Try to save current state
        try:
            brain.save("failed_simulation_checkpoint.bin")
            print("Checkpoint saved")
        except:
            print("Failed to save checkpoint")
        return False

# Run safe simulation
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

if safe_simulation(config, world, 1000):
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

## Next Steps

When you're comfortable with the basics:

1. **Read `docs/ARCHITECTURE.md`** to understand how it all works
2. **Read `docs/SCIENCE.md`** to understand the scientific background
3. **Experiment with different configurations** to see how they affect behavior
4. **Try the demo executables** (`nlm_phase3_demo`, `nlm_phase4_demo`) to see real examples
5. **Explore the C++ API** for more advanced features and customizations
6. **Join the community** to share your experiments and get help

That's it! You're now ready to use NLM with confidence and explore all its advanced features!