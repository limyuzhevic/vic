# NLM - HOW TO USE

## Overview

NLM (熙然, "serene flow") is an experimental artificial developmental brain. This document provides comprehensive instructions for using the NLM C++ neural simulation framework and its Python bindings.

## Building from Source

### Prerequisites

```bash
# Ubuntu/Debian
apt-get update
apt-get install -y g++ cmake python3 python3-dev python3-pip

# macOS
brew install gcc cmake python3

# Windows
choco install cmake g++ python
```

### Build Steps

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Installation

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install the Python package
pip install .

# Or install in development mode for editing
pip install -e .
```

### Verify Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Quick Start Guide

### 1. Basic Brain Simulation (Phase 2)

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Agent-World Interaction (Phase 3)

```python
import pynlm

# Setup brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### 3. Complete Integration (Phase 6)

```python
import pynlm
import time

def run_phase6_simulation(steps=2000):
    """Run a complete Phase 6 integration experiment."""
    
    # Configure for Phase 6
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create and initialize all components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        # Environment update
        world.update(0.1)
        
        # Sensory processing
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Brain processing
        brain.step(step)
        
        # Motor output
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Development update
        agent.updateDevelopment(0.1)
        
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Firing: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    print("Phase 6 simulation complete!")

run_phase6_simulation(2000)
```

## Configuration Management

### Default Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### File-Based Configuration

```python
import pynlm
import json

# Create config and save to file
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.saveToFile("my_config.json")

# Load config from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Save configuration summary
print(config.summary())
```

### Command Line Configuration

```bash
# Run executable with config options
./nlm --config "brain.neuron_count=1500" --config "plasticity.stdp.enable=true"
```

## Brain Operations

### Initialization

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this before using!
```

### Simulation

```python
brain.step(step)                    # Step with timestamp
brain.step(step, time)              # Step with explicit time
brain.reset()                       # Reset to initial state
```

### State Management

```python
brain.save("checkpoint.bin")       # Save brain state
brain.load("checkpoint.bin")       # Load brain state
```

### Statistics

```python
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Stage: {brain.getDevelopmentalStage()}")
```

## Agent Operations

### Initialization

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Connect to world
```

### Sensory-Motor Loop

```python
# In your main loop:
world.update(0.1)                              # Update environment
percept = world.getSensoryPercept()            # What does agent see?
agent.processSensoryInput(percept)            # Brain processes input
brain.step(step)                              # Brain thinks
action = agent.decodeMotorCommand()           # Get action
world.applyMotorCommand(action, time)         # Agent acts
```

### Neuromodulation

```python
# Enable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore novel things
agent.enableDevelopment(True)           # Brain matures
agent.enableStructuralPlasticity(True)   # Grow new connections

# Apply neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
```

### Development

```python
agent.updateDevelopment(0.1)           # Update development
agent.getDevelopmentalStage()          # Current developmental stage
```

### Statistics

```python
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

## World Operations

### Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=12, visionHeight=12)
world.setAgentStart(50.0, 50.0)        # Starting position
```

### Control

```python
world.reset()                           # Reset to start
world.update(timestep)                  # Update environment
world.applyMotorCommand(cmd, time)      # Execute action
```

### State Access

```python
percept = world.getSensoryPercept()     # Vision, touch, internal signals
body = world.getAgentBody()            # Agent position, velocity, energy
world.getSimulationTime()              # Current simulation time
```

## Configuration Parameters

### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stage.transitions.enabled` | bool | true | Enable developmental stages |
| `development.critical_period.duration` | float | 100.0 | Critical period length (steps) |

## Advanced Usage Examples

### Example 1: Multi-Agent Simulation

```python
import pynlm

def create_multi_agent_simulation(num_agents=3):
    """Create simulation with multiple interacting agents."""
    brains = []
    agents = []
    
    for i in range(num_agents):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        brains.append(brain)
        agents.append(agent)
    
    return brains, agents

# Create and run
brains, agents = create_multi_agent_simulation(3)
world = pynlm.createSimpleWorld()

for agent in agents:
    agent.initialize(world)

# Run simulation loop
for step in range(1000):
    world.update(0.1)
    
    for agent in agents:
        # Each agent senses and acts
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        agent.decodeMotorCommand()
```

### Example 2: Development Tracking

```python
import pynlm

def track_development(brain, steps=500):
    """Track brain development over time."""
    development_log = []
    
    for step in range(steps):
        stage = brain.getDevelopmentalStage()
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        
        development_log.append({
            'step': step,
            'stage': stage,
            'firing': firing,
            'rate': rate
        })
        
        brain.step(step)
    
    return development_log

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

development_data = track_development(brain, 500)

# Analyze development
for entry in development_data:
    if entry['stage'].value > 1:  # If past initial stage
        print(f"Step {entry['step']}: Developed to {entry['stage'].name}")
```

### Example 3: Performance Monitoring

```python
import pynlm
import time

def profile_performance(brain, world, agent, steps=1000):
    """Profile simulation performance and collect metrics."""
    start_time = time.time()
    
    metrics = {
        'steps_completed': 0,
        'total_spikes': 0,
        'avg_firing_rate': 0,
        'total_time': 0,
        'steps_per_second': 0,
        'memory_usage': 0
    }
    
    for step in range(steps):
        step_start = time.time()
        
        # Simulation loop
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        
        step_time = time.time() - step_start
        metrics['steps_completed'] += 1
        metrics['total_spikes'] += brain.getTotalSpikeCount()
        
        # Progress logging
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Progress: {step}/{steps} steps, {step/elapsed:.1f} steps/sec")
    
    # Calculate final metrics
    metrics['total_time'] = time.time() - start_time
    metrics['avg_firing_rate'] = metrics['total_spikes'] / steps
    metrics['steps_per_second'] = steps / metrics['total_time']
    
    return metrics

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Run performance test
results = profile_performance(brain, world, agent, 1000)
print(f"Performance: {results['steps_per_second']:.1f} steps/sec")
print(f"Average firing rate: {results['avg_firing_rate']:.2f} Hz")
```

### Example 4: Checkpoint Management

```python
import pynlm
import os

def run_with_checkpoints(steps=1000, checkpoint_interval=200):
    """Run simulation with checkpoint saving."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Create checkpoint directory
    os.makedirs("checkpoints", exist_ok=True)
    
    # Run simulation with checkpoints
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Save checkpoint
        if step % checkpoint_interval == 0 and step > 0:
            checkpoint_path = f"checkpoints/checkpoint_step_{step}.bin"
            if brain.save(checkpoint_path):
                print(f"Checkpoint saved: {checkpoint_path}")
            else:
                print(f"Failed to save checkpoint: {checkpoint_path}")
    
    # Save final state
    brain.save("checkpoints/final_state.bin")
    print("Final state saved!")
    
    return brain

# Usage
final_brain = run_with_checkpoints(1000, 250)
```

## Error Handling

### Common Errors and Solutions

**Error: Initialization failed**
```python
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Failed to initialize brain")
    # Check configuration parameters
    print("Neurons:", config.get("brain.neuron_count"))
    print("Regions:", config.get("brain.region_count"))
```

**Error: Step without initialization**
```python
# Wrong - will cause issues
brain.step(0)  # Crashes!

# Right - always initialize first
brain.initialize()
brain.step(0)  # Works!
```

**Error: Invalid configuration**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", -100)  # Invalid!

# Fix
config.set("brain.neuron_count", 1000)  # Valid
```

### Robust Error Handling Template

```python
import pynlm

def safe_run_simulation(steps=100):
    """Robust simulation with error handling."""
    
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        # Create brain
        brain = pynlm.createBrain(config)
        
        # Initialize (essential!)
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        # Run simulation
        for step in range(steps):
            try:
                brain.step(step)
                
                # Check for any issues
                if brain.getFiringNeuronCount() == 0 and step > 10:
                    print(f"Warning: No firing neurons at step {step}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                break
        
        print("Simulation completed successfully")
        return brain
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None

# Usage
brain = safe_run_simulation(200)
if brain:
    print(f"Success: Generated {brain.getTotalSpikeCount()} spikes")
```

## Python API Reference

### Core Classes

#### Brain
- `createBrain(config)` - Create new brain
- `initialize()` - Initialize brain
- `step(step, time=None)` - Run simulation step
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- All getter methods for statistics

#### AgentBrain
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Connect to world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get action from brain
- `applyRewardModulation(reward, predicted)` - Apply reward learning
- `enableRewardModulation(bool)` - Enable/disable reward learning
- `enableCuriosity(bool)` - Enable/disable curiosity

#### SimpleWorld
- `createSimpleWorld()` - Create world
- `configure(width, height, visionWidth, visionHeight)` - Set dimensions
- `reset()` - Reset world
- `update(timestep)` - Update environment
- `applyMotorCommand(cmd, time)` - Apply action
- `getSensoryPercept()` - Get sensory data
- `getAgentBody()` - Get agent state

#### Config
- `createDefaultConfig()` - Create default config
- `set(key, value)` - Set parameter
- `get(key)` - Get parameter
- `has(key)` - Check if parameter exists
- `loadFromFile(filepath)` - Load from JSON
- `saveToFile(filepath)` - Save to JSON
- `summary()` - Get configuration summary

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

#### ActionType
- `MoveForward`, `MoveBackward`
- `TurnLeft`, `TurnRight`
- `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`
- `Rest`, `Wait`

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

## Troubleshooting

### Installation Issues

**Q: `ImportError: No module named 'pynlm'`**
**A:**
```bash
pip install --force-reinstall .
pip install scikit-build-core pybind11 numpy
```

**Q: Build errors**
**A:**
```bash
# Clean build
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Runtime Issues

**Q: Segmentation fault**
**A:**
- Ensure `brain.initialize()` is called before `brain.step()`
- Check configuration parameters are valid
- Reduce neuron count for testing

**Q: No learning observed**
**A:**
- Verify plasticity is enabled: `config.set("plasticity.stdp.enable", True)`
- Check if reward modulation is working
- Ensure neurons are receiving sufficient input

**Q: Memory issues**
**A:**
- Monitor memory usage with `agent.getMemoryUsage()` if available
- Reduce neuron count or simulation time
- Use checkpointing for long runs

### Performance Issues

**Q: Simulation too slow**
**A:**
- Reduce neuron count
- Increase simulation timestep
- Disable expensive plasticity rules during testing
- Profile with `profile_performance()` function

## Migration Guide

### From NLM v0.1 to v0.2

1. **New AgentBrain class** - Use `pynlm.createAgentBrain(brain)` for environment interaction
2. **Enhanced SimpleWorld** - Updated with better sensory processing
3. **Improved development stages** - New developmental stage system
4. **Better error handling** - More robust initialization and validation
5. **Python API improvements** - Better type hints and documentation

### Upgrading from Basic to Advanced Usage

1. **Start simple:** Use basic examples for initial setup
2. **Add learning:** Enable reward modulation and curiosity
3. **Monitor development:** Track brain developmental stages
4. **Performance tune:** Use profiling and checkpointing
5. **Multi-agent:** Scale to multiple interacting agents

## Next Steps

1. **Read `HOW_TO_USE.md`** - More detailed usage instructions
2. **Read `docs/ARCHITECTURE.md`** - System architecture overview
3. **Build demos:** Run `./nlm_phase6_demo` executable
4. **Experiment:** Create custom configurations
5. **Run tests:** Execute NLM test suite

## Further Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Phase 6 Audit Report](docs/PHASE6_FINAL_AUDIT.md)
- [Phase 6 Results](docs/PHASE6_FINAL_RESULTS.md)
- [Architecture Documentation](docs/ARCHITECTURE.md)

## Conclusion

NLM provides a powerful framework for creating artificial developmental brains. The Python API makes it accessible for rapid prototyping while maintaining the computational depth needed for serious research.

The key to success is:
1. Always call `brain.initialize()` before using the brain
2. Start with simple simulations and gradually add complexity
3. Monitor brain statistics to understand system behavior
4. Use checkpointing for long-running simulations

You now have everything needed to start experimenting with brain-like learning systems! 🎉

---
*This documentation covers NLM Phase 6: Final Integration. The framework implements a complete artificial brain with memory, neuromodulation, development, and learning capabilities.*