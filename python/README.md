# NLM Python Package - pynlm

## Overview

pynlm is the Python interface to the NLM (Neural Learning Machine) neural simulation framework. It provides access to advanced brain simulation capabilities through a high-level Python API, enabling researchers and developers to create, simulate, and study artificial neural systems.

The package implements a complete brain architecture inspired by biological neural systems, including:
- **Learning**: STDP, Hebbian, and reward-based plasticity
- **Development**: Age-dependent structural changes and maturation
- **Memory**: Working memory, episodic memory, and associative memory
- **Cognition**: Planning, attention, and concept formation
- **Neuromodulation**: Dopamine, curiosity, and novelty signals

## Installation

### From PyPI

Install the latest stable release:

```bash
pip install pynlm
```

### From Source

Clone the repository and build from source:

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e python/
```

### Build Dependencies

- Python 3.8+
- pip
- Build tools (for source installation)

## Basic Usage

### Simple Brain Simulation

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation loop
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
        reward = 0.0  # Get from your actual task
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

## API Reference

### Core Classes

#### Config
Configuration management for NLM systems.

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### Brain
The central neural simulation brain class.

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, time)
brain.reset()

# Input/Output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Systems
brain.getWorkingMemory()
brain.getEpisodicMemory()
brain.getPredictionSystem()
brain.getPlanner()
brain.getConceptFormation()
brain.getAttention()
brain.getDevelopmentSystem()
brain.getDopamine()
```

#### SimpleWorld
2D world for NLM simulation with sensory experience.

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Control
world.update(timestep)
world.applyMotorCommand(motor_cmd, time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addWorldObject(x, y, type, value=0.0)
world.isValidPosition(x, y)
```

#### AgentBrain
Agent brain interface connecting NLM brain to world.

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### Sensory Data Classes

##### SensoryInput (Base)
Base class for all sensory inputs.

##### Vision
Vision sensory input representing visual field.

```python
vision = pynlm.Vision(width, height, channels=3)
vision.setData([0.1, 0.2, 0.3, ...])
data = vision.getData()
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

##### Audio
Audio sensory input representing sound signals.

```python
audio = pynlm.Audio(sample_rate, num_samples)
audio.setData([0.5, -0.3, 0.8, ...])
sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()
```

##### InternalSignals
Internal signals representing homeostatic state.

```python
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
signals = internal.getSignal(index)
num = internal.getNumSignals()
```

##### SensoryPercept
Complete sensory data packet received by agent.

```python
percept = world.getSensoryPercept()
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
proprioception = percept.getProprioception()
audio = percept.getAudio()
all_signals = percept.getAllSignals()
```

#### Action Classes

##### Action
Action representation for motor output.

```python
action = pynlm.Action(pynlm.ActionType.MoveForward)
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.2])
action_type = action.getType()
params = action.getParameters()
name = action.getName()
```

##### ActionResult
Result of applying a motor command to the world.

```python
result = world.applyMotorCommand(motor_cmd, time)
reward = result.reward
success = result.success
message = result.message
```

### Data Types

#### ID Types
```python
neuron_id = pynlm.NeuronId(123)
synapse_id = pynlm.SynapseId(456)
region_id = pynlm.RegionId(789)
population_id = pynlm.PopulationId(999)
```

#### Enumerations
```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
pynlm.NeuronType.Modulatory

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Examples

### Learning Agent

```python
import pynlm

# Setup learning agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = calculate_reward()  # Your reward function
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

brain.save("learned_brain.bin")
```

### Developmental Learning

```python
import pynlm

# Create brain with development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)
agent.enableDevelopment(True)

# Run through developmental stages
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Observe developmental stage
    stage = brain.getDevelopmentalStage()
    if stage == pynlm.DevelopmentalStage.Adult:
        print("Brain has reached adulthood!")
    
    # Enable other learning systems
    if step % 200 == 0:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)

print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
```

### Memory Systems

```python
import pynlm

# Create brain with memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Store in working memory
working_memory.storeToNeuron(neuron_id, activation)
active_traces = working_memory.getActiveTraces()

# Store episodes
episodic_memory.storeEpisode(episode)
episodes = episodic_memory.getEpisodes()

# Learn associations
associative_memory.associate(pattern1, pattern2)
pattern = associative_memory.retrieve(pattern1)
```

## Troubleshooting

### Common Issues

1. **Brain not responding**: Ensure `brain.initialize()` is called before simulation
2. **No activity**: Brains need time to warm up - try more steps
3. **World not updating**: Make sure `world.update()` is called in your loop
4. **Agent not moving**: Check that `world.applyMotorCommand()` is being called
5. **Import errors**: Ensure pynlm is properly installed with all dependencies

### Installation Issues

**Missing dependencies:**
```bash
# Install system dependencies for C++ extensions
# Ubuntu/Debian:
sudo apt-get install build-essential python3-dev

# CentOS/RHEL:
sudo yum groupinstall "Development Tools"
sudo yum install python3-devel

# Then reinstall:
pip install --force-reinstall pynlm
```

**Build errors:**
```bash
# Build with verbose output
pip install pynlm -v

# Or install from binary wheel
pip install --only-binary=pynlm pynlm
```

## Performance Tips

1. **Use checkpoints**: Save brain state periodically to avoid re-running long simulations
2. **Monitor stats**: Use `brain.getFiringNeuronCount()` and `brain.getTotalSpikeCount()`
3. **Profile slowly**: Start with small simulations to debug issues
4. **Batch operations**: Process multiple inputs when possible
5. **Enable parallel processing**: Configure for large-scale simulations

## License

MIT License. See LICENSE file for details.

## Authors

NLM Project Contributors

## Project Resources

- **Documentation**: docs/ARCHITECTURE.md, docs/SCIENCE.md
- **Examples**: examples/ directory
- **Tests**: tests/ directory
- **Issues**: GitHub Issues
- **Discussion**: NLM Forum

## Further Reading

1. **ARCHITECTURE.md** - System architecture overview
2. **SCIENCE.md** - Scientific background and principles
3. **EXPERIMENTS.md** - Experiment descriptions and results
4. **HOW_TO_USE.md** - Detailed usage guide
5. **easy_usage.md** - Beginner-friendly usage guide

## API Changes

For changes to the Python API between versions, see the CHANGELOG.md file in the root directory.

---

Generated with Python and love for neural simulation!
