# NLM Python API - Comprehensive Guide

## Overview

The NLM Python API (`pynlm`) provides Python bindings for the Neural Learning Machine (NLM), an experimental artificial developmental brain. This guide covers the complete Python API with examples from simple usage to advanced Phase 6 integration.

## Installation

### From PyPI (Stable)
```bash
pip install pynlm
```

### From Source (Development)
```bash
pip install -e .
# or
pip install scikit-build-core pybind11 pytest numpy
pip install -e .
```

## Quick Start Examples

### Example 1: Silent Brain (Phase 2 Basics)
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for a few steps
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Agent in World (Phase 3 Integration)
```python
import pynlm

# Setup brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Phase 6 Complete Integration
```python
import pynlm
import time

def run_phase6_integration(num_steps=5000):
    """Phase 6 integrated brain with all subsystems working together."""
    
    # Create brain with Phase 6 configuration
    config = pynlm.createDefaultConfig()
    
    # Configure for Phase 6 integration
    config.set("brain.neuron_count", 2000)  # More neurons for integration
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("replay_interval", 50)
    config.set("consolidation_interval", 200)
    
    # Create and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Phase 6 Integration Starting")
    print(f"Neurons: {brain.getTotalNeuronCount()}")
    print(f"Synapses: {brain.getTotalSynapseCount()}")
    print(f"Initial Development Stage: {brain.getDevelopmentalStage()}")
    
    # Phase 6 integrated simulation loop
    start_time = time.time()
    
    for step in range(num_steps):
        # Environment update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Motor output
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation (reward learning)
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Development update
        agent.updateDevelopment(0.1)
        
        # Progress reporting
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            elapsed = time.time() - start_time
            print(f"  Speed: {step/elapsed:.1f} steps/sec")
    
    elapsed = time.time() - start_time
    print(f"\nPhase 6 Integration Complete!")
    print(f"Total time: {elapsed:.1f}s ({num_steps/elapsed:.1f} steps/sec)")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Total learning episodes: {brain.getTotalSpikeCount()}")

if __name__ == "__main__":
    run_phase6_integration(5000)
```

## Class Reference

### Brain Class

The `Brain` class represents the central neural simulation core.

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
brain.step(step)                    # Step with timestamp
brain.step(step, time)              # Step with explicit time
```

#### I/O
```python
brain.receiveSensoryInput(input)    # Inject sensory data
brain.produceAction()               # Get motor action
```

#### State Management
```python
brain.save("checkpoint.bin")       # Save brain state
brain.load("checkpoint.bin")       # Load brain state
brain.reset()                       # Reset simulation
```

#### Statistics
```python
brain.getTotalNeuronCount()        # Total neurons
brain.getTotalSynapseCount()       # Total synapses
brain.getFiringNeuronCount()       # Currently firing
brain.getAverageFiringRate()       # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()         # Total spikes generated
brain.getPendingSpikeEventCount()  # Pending events
brain.getDevelopmentalStage()     # Current development stage
```

#### Regions
```python
brain.addRegion("cortex")          # Add neural region
region = brain.getRegion(region_id) # Get region by ID
brain.getRegionCount()             # Number of regions
brain.getRegionIds()               # All region IDs
```

### AgentBrain Class

The `AgentBrain` connects the NLM brain to the environment.

#### Creation
```python
agent = pynlm.createAgentBrain(brain)
```

#### Initialization
```python
agent.initialize(world)             # Connect to world
```

#### Sensory Processing
```python
agent.processSensoryInput(percept)  # Process sensory input
```

#### Motor Decoding
```python
action = agent.decodeMotorCommand() # Get action from brain
```

#### Neuromodulation
```python
agent.applyRewardModulation(reward, predicted_reward)  # Reward learning
agent.enableRewardModulation(True/False)               # Enable/disable
agent.enableCuriosity(True/False)                      # Enable/disable
```

#### Development
```python
agent.updateDevelopment(timestep)    # Update development
agent.getDevelopmentalStage()       # Get current stage
```

#### Statistics
```python
agent.getCuriosityLevel()           # Current curiosity
agent.getNoveltyLevel()              # Current novelty
agent.getPredictionError()           # Prediction error
agent.getNeuromodulationLevel()      # Overall neuromodulation
```

### Config Class

The `Config` class manages NLM system parameters.

#### Creation
```python
config = pynlm.createDefaultConfig()
```

#### Configuration
```python
config.set("brain.neuron_count", 1000)      # Set parameter
if config.has("brain.neuron_count"):         # Check existence
    value = config.get("brain.neuron_count") # Get value
```

#### File I/O
```python
config.loadFromFile("config.json")          # Load from JSON
config.saveToFile("config.json")            # Save to JSON
config.loadFromArgs(argc, argv)             # From command line
```

### SimpleWorld Class

The `SimpleWorld` represents the simulation environment.

#### Creation
```python
world = pynlm.createSimpleWorld()
```

#### Configuration
```python
world.configure(width, height, visionWidth, visionHeight)
```

#### Control
```python
world.reset()                               # Reset world
world.update(timestep)                       # Update environment
world.applyMotorCommand(cmd, time)           # Apply action
```

#### State
```python
world.getSensoryPercept()                   # What agent sees
world.getAgentBody()                        # Agent position/state
```

### Enumerations

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### ActionType
- `MoveForward`, `MoveBackward`
- `TurnLeft`, `TurnRight`
- `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`
- `Rest`, `Wait`, `Custom`

#### DevelopmentalStage
- `Initial`
- `CriticalPeriod`
- `Maturation`
- `Adult`
- `Aging`

## Configuration Parameters

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `brain.connection_probability` | float | 0.05 | Synapse formation probability |
| `brain.v_thresh` | float | -50.0 | Neuron threshold (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Hebbian learning |
| `plasticity.structural.enable` | bool | true | Structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine effect |
| `neuromod.curiosity.enable` | bool | true | Curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Novelty detection |

### Development Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage.transitions.enabled` | bool | true | Enable stage transitions |
| `development.critical_period.duration` | float | 100.0 | Critical period length |
| `development.plasticity.modulation.enabled` | bool | true | Development effects on plasticity |

## Advanced Usage Patterns

### Pattern 1: Multi-Agent Environment
```python
def create_multi_agent_environment(num_agents=3, steps=1000):
    """Create environment with multiple agents."""
    brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(num_agents)]
    agents = [pynlm.createAgentBrain(brain) for brain in brains]
    
    for brain in brains:
        brain.initialize()
    
    for agent in agents:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    return brains, agents

# Usage
brains, agents = create_multi_agent_environment()
for step in range(steps):
    for agent in agents:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        agent.decodeMotorCommand()
```

### Pattern 2: Memory-Augmented Agent
```python
def create_memory_agent(config):
    """Create agent with memory capabilities."""
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all memory systems
    agent.enableMemory()  # If available
    agent.enableEpisodicMemory()
    agent.enableSemanticMemory()
    
    return brain, agent

# Advanced configuration with memory
config = pynlm.createDefaultConfig()
config.set("memory.working.enabled", True)
config.set("memory.episodic.capacity", 1000)
config.set("memory.associative.enabled", True)
```

### Pattern 3: Development Tracking
```python
def run_development_experiment(brain, num_steps=10000):
    """Track brain development over time."""
    developmental_stages = []
    firing_rates = []
    learning_rates = []
    
    for step in range(num_steps):
        current_stage = brain.getDevelopmentalStage()
        current_firing = brain.getAverageFiringRate()
        
        developmental_stages.append(current_stage)
        firing_rates.append(current_firing)
        
        brain.step(step)
        
        # Track learning rate
        if step > 0:
            prev_rate = firing_rates[step-1] if step > 1 else 0
            learning_rate = current_firing - prev_rate
            learning_rates.append(learning_rate)
    
    return {
        'stages': developmental_stages,
        'firing_rates': firing_rates,
        'learning_rates': learning_rates,
        'final_stage': developmental_stages[-1]
    }
```

### Pattern 4: Performance Profiling
```python
def profile_brain_performance(brain, world, agent, num_steps=1000):
    """Profile brain performance metrics."""
    import time
    
    start_time = time.time()
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    metrics = {
        'steps_per_second': num_steps / elapsed,
        'total_spikes': brain.getTotalSpikeCount(),
        'spike_rate': brain.getTotalSpikeCount() / elapsed,
        'neurons_firing': brain.getFiringNeuronCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity_level': agent.getCuriosityLevel(),
        'memory_usage': brain.getTotalNeuronCount() * 128  # Estimate
    }
    
    return metrics
```

## Troubleshooting

### ImportError: No module named 'pynlm'
```bash
pip install --force-reinstall .
```

### Segmentation Fault
- Ensure `brain.initialize()` is called before `brain.step()`
- Check configuration values are reasonable
- Verify neural connectivity parameters

### Slow Performance
- Increase simulation timestep for testing
- Reduce neuron count for development
- Disable expensive plasticity rules during testing

### No Learning Observed
- Check if plasticity is enabled: `config.set("plasticity.stdp.enable", True)`
- Verify reward modulation is working
- Ensure neurons are receiving input

## Migration Guide

### From NLM v0.1 to v0.2
- New `AgentBrain` class for environment interaction
- Enhanced `SimpleWorld` with better sensory processing
- Improved developmental stages and neuromodulation
- Better Python error handling and type safety

### Upgrading from Example 1 to Phase 6
The example scripts have been updated to use Phase 6 integration:

1. Simple brain simulation → `run_phase6_integration()`
2. Agent-world loop → Complete integrated brain loop
3. Basic statistics → Comprehensive Phase 6 metrics

## Next Steps

1. Read `docs/ARCHITECTURE.md` for technical details
2. Read `docs/SCIENCE.md` for scientific background
3. Experiment with `nlm_phase6_demo` executable
4. Run NLM tests to verify implementation
5. Create custom configurations for your application

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Phase 6 Audit Report](docs/PHASE6_FINAL_AUDIT.md)
- [Phase 6 Results](docs/PHASE6_FINAL_RESULTS.md)

---
*This documentation is for NLM Phase 6: Final Integration. The Python API provides access to all integrated brain systems for experimentation and research.*