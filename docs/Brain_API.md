# NLM Brain API

## Brain Class

The `Brain` class is the central neural simulation engine in NLM, implementing a complete artificial brain with spiking neurons, plasticity mechanisms, memory systems, and cognitive capabilities.

### Overview

The Brain class implements a biologically-inspired artificial brain with the following key features:

- **Realistic LIF neuron dynamics** with spike-frequency adaptation
- **Multiple plasticity mechanisms** (STDP, Hebbian, structural)
- **Integrated memory systems** (working, episodic, associative)
- **Prediction and planning** capabilities
- **Neuromodulation** for reward and curiosity
- **Developmental plasticity** over time
- **Checkpoint save/load** for long-running simulations

### Key Features

#### Neural Architecture
- **500-5000 neurons** with configurable connectivity
- **Event-driven spike propagation** with synaptic delays
- **Regional organization** for modular processing
- **Multiple neuron types** (excitatory, inhibitory, sensory, motor, modulatory)

#### Learning Mechanisms
- **STDP (Spike-Timing Dependent Plasticity)**: Learning based on spike timing
- **Hebbian Plasticity**: "Neurons that fire together, wire together"
- **Structural Plasticity**: Growth and pruning of synaptic connections
- **Reward-modulated Plasticity**: Learning guided by rewards

#### Cognitive Systems
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Recording and replay of experiences
- **Concept Formation**: Pattern abstraction and generalization
- **Neural Planning**: Action sequence evaluation and planning

#### Neuromodulation
- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Detection of unexpected events
- **Prediction Error**: Learning from prediction mismatches

#### Development
- **Developmental Stages**: Different learning regimes over time
- **Critical Period**: Enhanced learning during specific stages
- **Maturation**: Gradual improvement of capabilities

### Basic Usage

#### Creating and Initializing a Brain

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize brain (required before any simulation)
brain.initialize()

# Verify initialization
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
```

#### Running a Simulation

```python
import pynlm
import time

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Record initial state
start_time = time.time()
initial_spikes = brain.getTotalSpikeCount()
initial_firing = brain.getFiringNeuronCount()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Optional: Print progress
    if step % 100 == 0:
        firing = brain.getFiringNeuronCount()
        spikes = brain.getTotalSpikeCount() - initial_spikes
        print(f"Step {step}: {firing} firing neurons, {spikes} total spikes")

# Record final state
end_time = time.time()
final_spikes = brain.getTotalSpikeCount() - initial_spikes
final_firing = brain.getFiringNeuronCount()

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average firing rate: {final_firing / 1000:.2f} Hz")
print(f"Total spikes generated: {final_spikes}")
```

#### Using Sensory Input

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create sensory input
vision = pynlm.Vision(10, 10, 3)  # 10x10 RGB image
# ... set vision data ...

# Process sensory input
brain.receiveSensoryInput(vision)

# Run simulation to process input
for step in range(100):
    brain.step(step)
```

#### Producing Actions

```python
import pynlm

# Setup with agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable action production
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Memory Systems

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Working memory - temporary storage
working_memory.storePattern("pattern1", [0.1, 0.5, 0.9])
retrieved = working_memory.retrievePattern("pattern1")

# Episodic memory - experience recording
episodic_memory.recordExperience("step_100", "exploration")
replay = episodic_memory.getReplayMemory()
```

#### Plasticity Control

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()

# Configure STDP
stdp.setLTPWeight(0.02)
stdp.setLTDWeight(0.015)
stdp.setTimeConstant(20.0)

# Enable/disable plasticity
stdp.setEnabled(True)
hebbian.setEnabled(True)
structural.setEnabled(True)
```

#### Development Control

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get development system
development = brain.getDevelopmentSystem()

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
# ... run during critical period ...

brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
```

#### Neuromodulation

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Apply reward signals
agent.applyRewardModulation(1.0, 0.5)  # reward, predicted reward
```

#### Checkpointing

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Save checkpoint
brain.save("checkpoint_100.bin")

# ... continue running or load later ...
brain.load("checkpoint_100.bin")
```

### Brain Class Methods

#### Construction and Destruction

```python
# Create brain with configuration
brain = pynlm.createBrain(config)

# Brain is automatically destroyed when out of scope
```

#### Initialization

```python
# Initialize brain with configuration
brain.initialize()

# Re-initialize brain (clears state)
brain.reset()
brain.initialize()
```

#### Simulation Steps

```python
# Perform simulation step (with implicit time)
brain.step(0)
brain.step(1)
brain.step(100)

# Perform simulation step with explicit time
brain.step(0, 0.0)
brain.step(1, 0.001)
brain.step(100, 0.1)
```

#### Input Processing

```python
# Inject current into specific neuron
brain.injectCurrent(neuron_id, 50.0)

# Inject current into all neurons of a type
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)

# Process sensory input
brain.receiveSensoryInput(sensory_input)
```

#### Action Production

```python
# Produce motor action based on current neural activity
action = brain.produceAction()
```

#### Plasticity

```python
# Update plasticity rules (called automatically in step)
brain.updatePlasticity()
```

#### Development

```python
# Apply developmental changes (called automatically in step)
brain.develop()
```

#### State Management

```python
# Reset brain state (keeps configuration)
brain.reset()

# Save brain state to file
brain.save("checkpoint.bin")

# Load brain state from file
brain.load("checkpoint.bin")
```

#### Region Management

```python
# Add neural region
region_id = brain.addRegion("cortex")

# Get region by ID
region = brain.getRegion(region_id)

# Get region statistics
region_count = brain.getRegionCount()
region_ids = brain.getRegionIds()
```

#### Statistics and Monitoring

```python
# Neural statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Pending spikes: {brain.getPendingSpikeEventCount()}")

# Developmental stage
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### Memory Systems Access

```python
# Working memory
working_memory = brain.getWorkingMemory()

# Episodic memory  
episodic_memory = brain.getEpisodicMemory()

# Associative memory
associative_memory = brain.getAssociativeMemory()

# Prediction system
prediction_system = brain.getPredictionSystem()

# Cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()
```

#### Neuromodulation Systems Access

```python
# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()
```

### Advanced Brain Usage

#### Using the Agent Interface

```python
import pynlm

# Create complete agent setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complete agent simulation
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

#### Complex Plasticity Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure complex plasticity setup
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.stdp.tau_plus", 15.0)
config.set("plasticity.stdp.tau_minus", 15.0)

config.set("plasticity.hebbian.enable", true)
config.set("plasticity.hebbian.learning_rate", 0.005)

config.set("plasticity.structural.enable", true)
config.set("plasticity.structural.growth_rate", 0.001)
config.set("plasticity.structural.pruning_rate", 0.0001)

config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.curiosity.rate", 0.2)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Multi-region Brain

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create different brain regions for different functions
motor_region_id = brain.addRegion("motor")
sensory_region_id = brain.addRegion("sensory")
associative_region_id = brain.addRegion("associative")

# Add inter-region connections
brain.addInterRegionConnection(sensory_region_id, associative_region_id, 0.5)
brain.addInterRegionConnection(associative_region_id, motor_region_id, 0.3)
```

### Error Handling

```python
def safe_brain_simulation(config, max_steps=1000):
    """Safely run a brain simulation with error handling."""
    brain = None
    
    try:
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        # Run simulation
        for step in range(max_steps):
            brain.step(step)
            
            # Optional: Check for early termination
            if brain.getTotalSpikeCount() > 1000000:
                print("Warning: High spike count, simulation may be unstable")
                break
                
        print("Simulation completed successfully")
        
    except Exception as e:
        print(f"Simulation error: {e}")
        if brain:
            # Reset brain on error
            brain.reset()
        raise
        
    finally:
        # Cleanup
        if brain:
            brain.reset()
```

### Performance Tips

#### Memory Management
```python
# Use reset() between episodes to free memory
for episode in range(100):
    brain.reset()
    brain.initialize()
    
    # ... run episode ...
    
    # Save checkpoint if needed
    if episode % 10 == 0:
        brain.save(f"checkpoint_episode_{episode}.bin")
```

#### Configuration Optimization
```python
# Optimize configuration for specific use case
config = pynlm.createDefaultConfig()

# For real-time applications
config.set("brain.neuron_count", 100)
config.set("simulation_timestep", 0.01)
config.set("plasticity.stdp.enable", false)  # Disable for speed

# For research and exploration
config.set("brain.neuron_count", 5000)
config.set("simulation_timestep", 0.001)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.structural.enable", true)
```

### Best Practices

1. **Always initialize before use**: Call `brain.initialize()` before any simulation steps
2. **Reset between episodes**: Use `brain.reset()` to clear state between simulations
3. **Monitor performance**: Track firing rates and spike counts
4. **Use checkpoints for long experiments**: Save intermediate states regularly
5. **Validate configuration**: Check configuration before running simulations
6. **Handle errors gracefully**: Implement proper error handling
7. **Monitor memory usage**: Use `reset()` to free memory

### Troubleshooting

#### "Brain isn't firing any neurons"
```python
# Check if neurons are properly initialized
brain.reset()
brain.initialize()

# Verify configuration
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")

# Inject current to test
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)
```

#### "Simulation is unstable"
```python
# Check configuration parameters
config = brain.getConfig()
print(f"Neuron threshold: {config.get('brain.v_thresh')}")
print(f"Resting potential: {config.get('brain.v_rest')}")
print(f"Time constant: {config.get('brain.tau_mem')}")

# Consider reducing neuron count or timestep
config.set("brain.neuron_count", 100)
config.set("simulation_timestep", 0.01)
```

#### "Memory usage is high"
```python
# Use checkpointing and reset
brain.save("checkpoint.bin")
brain.reset()
brain.initialize()
```

This comprehensive documentation provides everything needed to effectively use the Brain class in NLM simulations, from basic usage to advanced features and best practices.