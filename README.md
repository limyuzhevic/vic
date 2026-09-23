# NLM (熙然) — Neural Learning Machine

**An Experimental Artificial Developmental Brain with Python Interface**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project implemented in C++ with a Python interface. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

**Python Interface**: Use the `pynlm` Python library for easy brain simulation and experimentation.

**Advanced C++ Core**: Complete spiking neural network simulation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Full brain integration (Phase 6)

**What NLM IS NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

**What NLM IS**:
- A neural system that learns from experience
- A brain-inspired architecture with real neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start (Python API)

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

## Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
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
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
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
        
        # Apply reward modulation based on action result
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

## Building (Optional)

If you need to build the C++ core from source:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The Python bindings (`pynlm`) are pre-built and included in the package.

## Installation

Install the Python library:

```bash
pip install .
```

For development:

```bash
pip install -e .
```

## Project Structure

```
NLM/
├── Python Interface
│   └── pynlm/           # Python bindings (main user interface)
├── Core Components (C++)
│   ├── src/core/        # Core utilities
│   ├── src/brain/       # Neural components (integrated)
│   ├── src/neuromodulation/ # Neuromodulators (integrated)
│   ├── src/memory/     # Memory systems (integrated)
│   ├── src/cognition/  # Cognitive mechanisms (integrated)
│   └── ...
├── Documentation
│   ├── docs/           # Technical documentation
│   ├── easy_usage.md   # Beginner guide
│   └── HOW_TO_USE.md   # Comprehensive guide
├── Examples & Demos
│   ├── src/experiments/ # Phase experiments
│   └── examples/       # Python examples
└── Tests
    └── tests/          # Unit tests
```

## Key Classes and Methods

### Brain Class

| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create a new brain |
| `brain.initialize()` | Initialize the brain |
| `brain.step(step)` | Run one simulation step |
| `brain.getTotalSpikeCount()` | Get total spike count |
| `brain.getFiringNeuronCount()` | Get currently firing neurons |
| `brain.getAverageFiringRate()` | Get average firing rate |
| `brain.getDevelopmentalStage()` | Get current developmental stage |

### Config Class

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create default configuration |
| `config.set(key, value)` | Set configuration parameter |
| `config.has(key)` | Check if key exists |

### AgentBrain Class

| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent interface |
| `agent.initialize(world)` | Initialize with world |
| `agent.processSensoryInput(percept)` | Process sensory input |
| `agent.decodeMotorCommand()` | Decode motor command |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward learning |

### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create simple 2D world |
| `world.configure(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `world.reset()` | Reset world state |
| `world.update(timestep)` | Update world |
| `world.applyMotorCommand(cmd, time)` | Apply motor command |

## Advanced Usage

### Custom Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 2.0)

brain = pynlm.createBrain(config)
```

### Multiple Regions

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Add different brain regions
region1_id = brain.addRegion("Sensory")
region2_id = brain.addRegion("Motor")
region3_id = brain.addRegion("Association")

print(f"Created {brain.getRegionCount()} regions")
```

### Checkpoints and Persistence

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Save brain state
brain.save("my_brain_checkpoint.bin")

# Later, load the saved state
new_brain = pynlm.createBrain(config)
new_brain.load("my_brain_checkpoint.bin")
```

## Development and Extension

### For Advanced Users

NLM provides several extension points:

1. **Custom Sensory Inputs**: Create custom sensory modalities
2. **Specialized World Models**: Implement different environment dynamics
3. **New Plasticity Rules**: Add custom learning mechanisms
4. **Advanced Memory Systems**: Implement specialized memory architectures

### Configuration Options

See `HOW_TO_USE.md` for complete configuration documentation including:
- Brain parameters (neuron count, connectivity, etc.)
- Plasticity configuration (STDP, Hebbian, structural)
- Neuromodulation parameters (dopamine, curiosity, novelty)
- Development stages and progression

## Examples and Demos

Check out the examples directory for:

- **Basic brain simulation** - Simple neural activity
- **Agent in world** - Complete perception-action loop
- **Learning experiments** - Demonstrating plasticity
- **Development demos** - Brain maturation over time

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive usage guide
- [easy_usage.md](easy_usage.md) - Beginner-friendly guide
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model
- No realistic ion channel dynamics
- Limited biological fidelity
- Dependent on available computing resources

## License

MIT

## Authors

Research project — See docs for scientific background.

## Key Achievements (Phase 6)

- ✅ All neural systems integrated into coherent brain loop
- ✅ Memory systems connected to neural processing
- ✅ Neuromodulation affects plasticity and dynamics
- ✅ Prediction integrated with learning
- ✅ Development affects plasticity rates
- ✅ Checkpoint save/load working
- ✅ Replay and consolidation functional
- ✅ Complete Phase 6 integration experiment

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

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

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
