# NLM (Neural Learning Machine) - User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. It implements a neural system that learns from experience through spiking neural networks, memory systems, neuromodulation, and cognitive functions.

### Key Features

- **Spiking Neural Networks**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Cognition**: Attention, planning, concept formation, self-model
- **Plasticity**: STDP, Hebbian learning, structural plasticity
- **Development**: Developmental stages, maturation, critical periods
- **Embodiment**: Sensorimotor loops with world interaction
- **Integration**: All systems work together as a coherent artificial brain

## System Architecture

```
SENSORIAL INPUT → NEURAL PROCESSING → INTERNAL STATE
                ↓                       ↓
            MEMORY/PREDICTION    MOTIVATION/NEUROMODULATION
                ↓                       ↓
           ACTION SELECTION → MOTOR OUTPUT
                ↓
            WORLD CONSEQUENCE → REWARD/SURPRISE
                ↓
             PLASTICITY → CHANGED BRAIN → CHANGED BEHAVIOR
```

## Getting Started

### Prerequisites

- C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+
- pip (Python package installer)

### Quick Start

#### 1. Build from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Run the build script (recommended)
./build.sh build
```

Or build manually:

```bash
# Build C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
cd ..
pip install -e .
```

#### 2. Quick Python Demo

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
```

#### 3. Complete Agent Example

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
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Python API Reference

### Core Classes

#### Config
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Check if configuration key exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")

# Get all configuration keys
keys = config.getKeys()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# Save/load configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")
```

#### Brain
```python
# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Save/load brain state
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### AgentBrain
```python
# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input
agent.processSensoryInput(percept)

# Get action
action = agent.decodeMotorCommand()

# Apply reward
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)
```

#### SimpleWorld
```python
# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Control world
world.reset()
world.update(timestep)
world.applyMotorCommand(action, current_time)

# Get world information
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Set agent start position
world.setAgentStart(x, y)
```

#### SensoryInput (Base Class)
```python
# Get sensory input properties
percept.getType()
percept.getData()
percept.getDimensions()
percept.getTimestamp()
```

#### Vision (Subclass of SensoryInput)
```python
# Create vision input
vision = pynlm.Vision(width, height, channels=3)

# Set vision data
vision.setData([0.1, 0.5, 0.9, ...])

# Get vision properties
vision.getWidth()
vision.getHeight()
vision.getChannels()
vision.getTotalPixels()
vision.getMeanIntensity()
```

#### Audio (Subclass of SensoryInput)
```python
# Create audio input
audio = pynlm.Audio(sampleRate, numSamples)

# Set audio data
audio.setData([0.2, -0.3, 0.1, ...])

# Get audio properties
audio.getSampleRate()
 audio.getNumSamples()
audio.getDuration()
audio.getPeakAmplitude()
```

#### InternalSignals (Subclass of SensoryInput)
```python
# Create internal signals
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.5)
internal.addSignal(-0.2)

# Get signals
internal.getAllSignals()
```

#### Action
```python
# Create action from type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with parameters
action = pynlm.Action(
    pynlm.ActionType.Custom,
    [0.5, 0.8, 0.3]  # Movement parameters
)

# Get action properties
action.getType()
action.getParameters()
action.getName()

# Clone action
clonedaction = action.clone()
```

#### WorldObject
```python
# Create world object
obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)

# Access object properties
obj.x
obj.y
obj.type
obj.value
obj.radius
obj.active
```

#### AgentBody
```python
# Create agent body
body = pynlm.AgentBody()

# Access body state
body.x
body.y
body.orientation
body.velocityX
body.velocityY
body.energy
body.health

# Get body information
body.isMoving
body.isTurning
body.getLastActionTime()
```

#### ActionResult
```python
# Create action result
result = pynlm.ActionResult(reward=0.5, success=True, message="Action completed")

# Access result properties
result.reward
result.success
result.message
```

#### SensoryPercept
```python
# Create sensory percept
percept = pynlm.SensoryPercept()

# Set percept data
percept.setVision(vision_data)
percept.setTouch(touch_data)
percept.setInternal(internal_data)

# Get percept data
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
```

### NeuralWorkingMemory
```python
# Create and initialize working memory
wm = pynlm.NeuralWorkingMemory()
wm.initialize(brain)

# Store and retrieve memory patterns
wm.store([0.5, 0.8, 0.3], 1.0)
retrieved = wm.retrieve()

# Query memory state
active_traces = wm.getActiveTraces()
capacity = wm.getCapacity()
memory_activity = wm.getMemoryActivity()

# Memory lifecycle
wm.update(dt=1.0)           # Maintenance and decay
wm.strengthenMemory(1.2)    # Rehearse memory
wm.clear()                   # Clear all memory
```

### AttentionalSelection
```python
# Create and initialize attention system
attn = pynlm.AttentionalSelection()
attn.initialize(brain)

# Process competing inputs
winners = attn.processCompetition([1, 2, 3, 4, 5], globalInhibition=0.5)

# Focus attention
attn.focusOnRegion(region_id)

# Query attention
attended = attn.getAttendedRegions()
is_attended = attn.isAttended(neuron)

# Apply attention biases
attn.applyTopDownBias(neuron, 0.7)        # Goal-directed
attn.applyBottomUpSalience(neuron, 0.5)   # Stimulus-driven
```

### Enumerations

#### NeuronType
- `NeuronType.Excitatory`
- `NeuronType.Inhibitory`
- `NeuronType.Modulatory`
- `NeuronType.Sensory`
- `NeuronType.Motor`
- `NeuronType.Internal`

#### SynapseType
- `SynapseType.Excitatory`
- `SynapseType.Inhibitory`
- `SynapseType.Modulatory`
- `SynapseType.Electrical`
- `SynapseType.GapJunction`

#### DevelopmentalStage
- `DevelopmentalStage.Initial`
- `DevelopmentalStage.CriticalPeriod`
- `DevelopmentalStage.Maturation`
- `DevelopmentalStage.Adult`
- `DevelopmentalStage.Aging`

#### ActionType
- `ActionType.MoveForward`
- `ActionType.MoveBackward`
- `ActionType.TurnLeft`
- `ActionType.TurnRight`
- `ActionType.Look`
- `ActionType.Interact`
- `ActionType.Eat`
- `ActionType.Drink`
- `ActionType.Rest`
- `ActionType.Wait`
- `ActionType.Custom`

#### MotorCommand
- `MotorCommand.MoveForward`
- `MotorCommand.MoveBackward`
- `MotorCommand.TurnLeft`
- `MotorCommand.TurnRight`
- `MotorCommand.LookLeft`
- `MotorCommand.LookRight`
- `MotorCommand.Interact`
- `MotorCommand.Wait`

#### WorldObjectType
- `WorldObjectType.Empty`
- `WorldObjectType.Resource`
- `WorldObjectType.Hazard`
- `WorldObjectType.Wall`
- `WorldObjectType.Marker`

## Configuration Options

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Examples and Demos

The NLM project includes several demonstration scripts:

### 1. Simple Brain Demo
Shows a basic neural network with just neurons and synapses.

### 2. Agent-World Demo
Demonstrates an agent interacting with a world environment.

### 3. Learning Demo
Shows how the brain learns through experience and plasticity.

### 4. Memory Demo
Demonstrates working memory functionality.

### 5. Attention Demo
Shows attention and selection mechanisms.

You can run these demos using the `demo.sh` script:

```bash
# Run simple brain demo
./demo.sh simple

# Run agent-world demo
./demo.sh agent_world

# Run learning demo
./demo.sh learning

# Run all demos
./demo.sh all
```

## Troubleshooting

### Common Issues

**1. "My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**2. "The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**3. "Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**4. "ImportError: No module named 'pynlm'"**
```bash
pip install pynlm
# or
pip install -e .
```

**5. "Build errors"**
```bash
# Ensure you have CMake >= 3.16
git clone https://github.com/nlm-project/nlm.git
cd nlm
./build.sh build
```

### Getting Help

1. **Documentation**: Check the `docs/` directory for detailed documentation
2. **GitHub**: Visit the NLM project website
3. **Issues**: Report bugs and feature requests
4. **Community**: Join the discussion forums

## License

MIT License. See `LICENSE` file for details.

## Authors

NLM Authors - Research project investigating computational brain-like systems.

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Final integration audit

## Version Information

- **NLM Version**: 0.1.0
- **Python Bindings**: 1.0.0
- **Phase**: 6 - Final Integration
- **Build Date**: $(date)

## Installation

### Install from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build using the build script (recommended)
./build.sh build

# Or build manually
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
pip install -e .
```

### Quick Install (if available)

```bash
pip install pynlm
```

## Development

### Building for Development

```bash
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Running Tests

```bash
# Run C++ tests
./build/nlm_test

# Run Python tests
python3 -m pytest tests/ -v
```

### Adding New Features

1. Add C++ classes in `src/`
2. Add Python bindings in `python/bindings.cpp`
3. Update `python/CMakeLists.txt` if needed
4. Write tests in `tests/`
5. Update documentation

## Performance

### Recommended Settings for Production

- **Neuron Count**: 500-1000 for basic simulations
- **Time Step**: 0.001 (1ms)
- **Max Steps**: 1000-10000 for typical episodes
- **Memory Capacity**: Auto (brain sets appropriate capacity)

### Performance Tips

1. **Use Release Build**: For production, use `Release` build type
2. **Parallel Processing**: Enable SIMD vectorization
3. **Memory Optimization**: Use memory pools for large networks
4. **Event-Driven Processing**: Take advantage of spike event system

## Customization

### Custom Configuration

Create your own configuration file:

```json
{
    "brain": {
        "neuron_count": 500,
        "connection_probability": 0.1,
        "synapse_density": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        },
        "hebbian": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": true
        }
    }
}
```

### Custom Worlds

Create custom world classes by implementing the `Environment` interface.

### Custom Neurons

Create custom neuron dynamics by extending the `NeuralDynamics` class.

## License

MIT License. See LICENSE file for details.

---

**Note**: NLM is a research project. We make NO claims that NLM accurately reproduces biological brains. This is a computational framework for investigating artificial developmental systems.

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently