# NLM - Neural Learning Machine
# Python API Documentation (Phase 6: Final Integration)

This is the official documentation for using NLM (Neural Learning Machine) with the Python bindings. NLM is an experimental computational brain project that simulates spiking neural networks with event-driven dynamics, multiple memory systems, and integrated neuromodulation.

## Overview

NLM provides a brain-inspired neural simulation platform that implements:

- **Leaky Integrate-and-Fire (LIF) neurons** with real membrane dynamics
- **Event-driven spike propagation** with synaptic delays
- **Multiple memory systems** (working, episodic, associative)
- **Neuromodulation systems** (dopamine, curiosity, novelty detection)
- **Prediction and planning capabilities**
- **Developmental processes** that shape learning over time
- **Real-time plasticity rules** (STDP, Hebbian, structural)

## Quick Start Guide

### Installation

```bash
# Install the package
pip install pynlm

# Or build from source (for development)
pip install -e .
```

### First Example: Simple Brain Simulation

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Core Concepts

### The Three Components

1. **Brain** - The virtual brain that thinks and learns
2. **World** - The environment the brain interacts with  
3. **Agent** - The interface connecting brain to world

### Basic Workflow

```python
# 1. Create everything
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 2. Enable learning features (optional but recommended)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# 3. Run simulation loop
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process and think
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Act
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

## Core Classes

### Brain Class

```python
# Creation and Configuration
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one step
brain.step(step_number, time)     # Run with specific time
brain.reset()                     # Reset to initial state

# Sensory Processing
brain.receiveSensoryInput(input_data)  # Inject sensory data
brain.injectCurrent(neuron_id, current)  # Direct current injection
brain.injectCurrentToNeurons(neuron_type, current)  # Inject to all neurons of type

# Motor Output
brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()      # Total neurons
brain.getTotalSynapseCount()     # Total synapses  
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()     # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()  # E/I balance

# State Management
brain.save("checkpoint.bin")     # Save state to file
brain.load("checkpoint.bin")     # Load state from file

# Access Subsystems
brain.getWorkingMemory()         # Working memory system
brain.getEpisodicMemory()        # Episodic memory system
brain.getAssociativeMemory()     # Associative memory
brain.getPredictionSystem()      # Prediction system
brain.getPlanner()              # Neural planner
brain.getConceptFormation()      # Concept formation
brain.getAttention()             # Attention system
brain.getDevelopmentSystem()     # Development system
brain.getDopamine()             # Dopamine neuromodulator
brain.getCuriosity()            # Curiosity neuromodulator
brain.getNovelty()              # Novelty detection
brain.getPredictionErrorSignal() # Prediction error
```

### Config Class

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Get and set configuration values
if config.has("brain.neuron_count"):
    keys = config.getKeys()

config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

```python
# Creation and Initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory Processing
agent.processSensoryInput(percept)

# Motor Decoding
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem Control
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent Interaction
world.setAgentStart(x, y)
world.applyMotorCommand(action, current_time)

# State Access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World Objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
```

## Enumerations

### NeuronType
```python
pynlm.NeuronType.Excitatory  # Excitatory neurons
pynlm.NeuronType.Inhibitory  # Inhibitory neurons
pynlm.NeuronType.Sensory     # Sensory input neurons
pynlm.NeuronType.Motor       # Motor output neurons
pynlm.NeuronType.Modulatory  # Neuromodulatory neurons
pynlm.NeuronType.Internal   # Internal processing neurons
```

### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial       # Initial brain state
pynlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
pynlm.DevelopmentalStage.Maturation    # Maturation phase
pynlm.DevelopmentalStage.Adult        # Adult brain
pynlm.DevelopmentalStage.Aging        # Aging phase
```

### ActionType
```python
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.Interact    # Interact with object
pynlm.ActionType.Eat         # Consume resource
```

### WorldObjectType
```python
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource   # Resource object
pynlm.WorldObjectType.Hazard     # Hazardous object
pynlm.WorldObjectType.Wall       # Wall/obstacle
pynlm.WorldObjectType.Marker     # Marker/landmark
```

## Configuration Options

### Core Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `simulation_timestep` | float | 0.001 | Simulation timestep (seconds) |
| `random_seed` | int | 42 | Random seed for reproducibility |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |
| `synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.0001 | Rate of synapse elimination |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_scale` | float | 1.0 | Dopamine modulation scale |
| `curiosity_enable` | bool | true | Enable curiosity-driven exploration |
| `novelty_enable` | bool | true | Enable novelty detection |
| `prediction_error_sensitivity` | float | 1.0 | Sensitivity to prediction errors |

### Memory System Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | int | 100 | Capacity of working memory |
| `episodic_memory_max_episodes` | int | 1000 | Maximum episodic memory episodes |
| `associative_memory_pattern_limit` | int | 10000 | Pattern storage limit |

### Cognitive Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `planning_depth` | int | 5 | Depth of action planning |
| `attention_inhibition_strength` | float | 0.5 | Strength of attentional inhibition |
| `attention_excitation_strength` | float | 1.5 | Strength of attentional excitation |

## Advanced Examples

### Complete Agent with Learning

```python
def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with learning."""
    
    # 1. Setup configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("region_count", 2)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent interface
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
    
    return brain, agent, world
```

### Developmental Learning

```python
def run_development_simulation():
    """Simulate brain development over time."""
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Simulating brain development...")
    
    # Track developmental stage
    current_stage = brain.getDevelopmentalStage()
    stage_changes = []
    
    # Run simulation with development
    for step in range(5000):
        brain.step(step)
        
        # Apply development updates periodically
        if step % 500 == 0:
            brain.develop()
            
            # Check if developmental stage changed
            new_stage = brain.getDevelopmentalStage()
            if new_stage != current_stage:
                stage_changes.append((step, new_stage))
                current_stage = new_stage
                
                # Log development milestone
                print(f"Stage change at step {step}: {new_stage}")
    
    print(f"Development simulation complete. Stage changes: {len(stage_changes)}")
    return brain
```

### Memory Integration Test

```python
def test_memory_integration():
    """Test integration of memory systems."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Generate varied activity to create memories
    for step in range(100):
        brain.step(step)
        
        # Inject different input patterns
        if step % 10 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 
                                        float(step % 5 + 1))
    
    # Test memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print("Memory system status:")
    print(f"  Working memory traces: {working_memory.getActiveTraces() if working_memory else 'N/A'}")
    print(f"  Episodic memory episodes: {episodic_memory.getEpisodeCount() if episodic_memory else 'N/A'}")
    print(f"  Associative memory patterns: {associative_memory.getPatternCount() if associative_memory else 'N/A'}")
    
    return brain
```

## Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Ensure neurons are receiving input

**"The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify action decoder is working

**"Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that neurons are receiving excitatory input

**"ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

**"Segfaults"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid

## Additional Resources

- **Architecture Documentation**: `docs/ARCHITECTURE.md` - System architecture overview
- **Science Documentation**: `docs/SCIENCE.md` - Scientific background
- **Experiments Documentation**: `docs/EXPERIMENTS.md` - Experiment descriptions
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Final integration audit report

## Building and Running from Source

### Using CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running Examples

```bash
# Phase 6 Integration Demo
./nlm_phase6_demo

# Basic neural computation demo  
./nlm

# Phase 4 cognitive demo
./nlm_phase4_demo

# Run tests
ctest --output-on-failure
```

## Advanced Usage

### Custom Configuration

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.018)
config.set("dopamine_scale", 2.0)

brain = pynlm.createBrain(config)
```

### Subclassing and Extension

The NLM framework is designed for extensibility:

1. **Custom Brain Types**: Subclass the Brain class for specialized architectures
2. **Custom Neurons**: Create new neuron models with different dynamics
3. **Custom Synapses**: Implement novel synaptic mechanisms
4. **Custom Memory Systems**: Extend memory with new storage and retrieval mechanisms
5. **Custom Neuromodulators**: Add new neuromodulatory signals and effects

### Performance Considerations

- **Large Networks**: Use efficient data structures and parallel processing
- **Long Simulations**: Implement checkpoint/restart for long runs
- **Memory Management**: Clear memory traces when not needed
- **Plasticity Tuning**: Adjust plasticity rates for desired learning speed

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See docs for scientific background and acknowledgments.

---

For more detailed examples and advanced usage, refer to the source code comments and documentation in the `docs/` directory.

Remember: Start simple with the examples, then gradually add complexity as you understand the NLM architecture better!
