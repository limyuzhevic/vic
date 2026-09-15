# NLM (Neural Learning Machine) - Phase 6: Integrated Artificial Brain

NLM is a brain-inspired spiking neural network simulator written in C++20 with Python bindings. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

## Features

- **Real spiking neural computation** with event-driven dynamics
- **Phase 6: Fully integrated brain with memory, prediction, cognition, neuromodulation**
- **Memory systems**: Working memory, episodic memory, associative memory
- **Prediction system**: Forward models and prediction errors
- **Cognitive functions**: Planning, attention, concept formation
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Development**: Critical periods, maturation, aging
- **Plasticity**: STDP, Hebbian learning, structural plasticity
- **Full Python bindings** for easy integration and experimentation

## Installation and Setup

### Prerequisites
- C++ compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake: Version 3.16 or higher
- Python: 3.8+ (for Python bindings)
- pybind11: Version 2.11.0+

### Build from Source

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Install Python Library

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode for easier editing
pip install -e .
```

### Quick Verification

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Python API Reference

### Core Classes

#### Brain
Represents the central neural system with all integrated components.

**Creation:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

**Initialization:**
```python
brain.initialize()
```

**Key Methods:**
- `step(step_number)`: Run one simulation step
- `step(step_number, time)`: Run step with timestamp
- `receiveSensoryInput(input)`: Inject sensory data into brain
- `produceAction()`: Get motor action based on neural activity
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to checkpoint
- `load(filepath)`: Load brain state from checkpoint

**Statistics:**
- `getTotalNeuronCount()`: Total neurons
- `getTotalSynapseCount()`: Total synapses
- `getFiringNeuronCount()`: Currently firing neurons
- `getAverageFiringRate()`: Average firing rate
- `getTotalSpikeCount()`: Total spike count

#### Config
Configuration system for the NLM brain.

**Creation:**
```python
config = pynlm.createDefaultConfig()
```

**Configuration Options:**
- `brain.neuron_count`: Total number of neurons (default: 1000)
- `brain.synapse_density`: Synapse connectivity density (default: 0.1)
- `brain.connection_probability`: Connection probability (default: 0.05)
- `brain.v_thresh`: Neuron threshold potential (default: -50.0 mV)
- `brain.v_rest`: Resting potential (default: -70.0 mV)
- `plasticity.stdp.enable`: Enable STDP (default: true)
- `plasticity.hebbian.enable`: Enable Hebbian learning (default: true)
- `neuromod.dopamine.scale`: Dopamine modulation scale (default: 1.0)

#### AgentBrain
Bridge connecting NLM brain to environment.

**Creation:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Key Methods:**
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory percept
- `decodeMotorCommand()`: Get action from brain activity
- `applyRewardModulation(reward, predicted)`: Apply reward
- `updateDevelopment(timestep)`: Update developmental stage

#### SimpleWorld
Simple 2D simulation environment.

**Creation:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

**Key Methods:**
- `reset()`: Reset world to initial state
- `update(timestep)`: Update world dynamics
- `applyMotorCommand(action, time)`: Apply agent's action
- `getSensoryPercept()`: Get what agent sees
- `getAgentBody()`: Get agent state

### Enumerations

#### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

#### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
```

#### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Adult
```

## Complete Example: Agent in Environment

```python
import pynlm
import time

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
    
    # 6. Enable learning and development
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
        
        # Apply reward modulation based on world result
        action_result = world.getAgentBody()
        reward = action_result.energy / 100.0  # Simple reward based on energy
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Agent position: ({action_result.x:.1f}, {action_result.y:.1f})")
            print(f"  Agent energy: {action_result.energy:.1f}")

# Run the simulation
if __name__ == "__main__":
    run_agent_simulation(1000)
```

## Advanced Usage

### Using Checkpoints

```python
# Save brain state
brain.save("my_brain_checkpoint.bin")

# Load brain state
brain.load("my_brain_checkpoint.bin")
```

### Custom Configuration

```python
config = pynlm.createDefaultConfig()

# Configure custom parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Working with Different Worlds

```python
# Create multiple worlds with different properties
world1 = pynlm.createSimpleWorld()
world1.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world1.setAgentStart(5.0, 5.0)

world2 = pynlm.createSimpleWorld()
world2.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world2.setAgentStart(15.0, 15.0)
```

## Performance Tips

1. **Batch simulation steps**: For multiple steps, use `brain.step(step, time)` with time
2. **Enable/disable systems**: Only enable learning/subsystems when needed
3. **Memory management**: Monitor brain statistics with `logStatus()`
4. **Configuration optimization**: Adjust neuron count and density for performance
5. **Use checkpoints**: Save brain states to resume simulations

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Brain not firing:**
- Ensure `brain.initialize()` is called before `step()`
- Check that sensory input is connected to brain
- Try increasing number of simulation steps

**Agent not moving:**
- Ensure `world.applyMotorCommand()` is called
- Check that `world.update()` is being called
- Verify motor neuron population is created

## Examples and Experiments

The repository includes several example scripts demonstrating different aspects of the system:

1. **Minimal example**: Brain without environment
2. **Environment interaction**: Complete agent simulation
3. **Phase experiments**: Different developmental phases
4. **Benchmark scripts**: Performance testing

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [PHASE6_ARCHITECTURE.md](docs/PHASE6_ARCHITECTURE.md) - Phase 6 specific architecture
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Current status and features
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Advanced Python Features

### Custom Neural Networks

```python
import pynlm

# Create multiple brains and connect them
config1 = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config1)
brain1.initialize()

config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)
brain2.initialize()

# Create agent connections
agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

# Connect brains via inter-region connections (internal API)
# This would require access to internal C++ API
```

### Performance Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor performance
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        # Get performance metrics
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"Step {step}: {total_neurons} neurons, {total_synapses} synapses")
        print(f"Firing rate: {firing_rate:.2f} Hz")
```

## Conclusion

NLM provides a complete, integrated artificial brain simulation platform that combines realistic neural dynamics with cognitive functions, learning, and development. The Python bindings make it accessible for research, education, and experimentation.

This is Phase 6 of the NLM project, featuring a fully integrated system with memory, prediction, cognition, and neuromodulation systems working together to create a biologically plausible artificial brain.

---

**Version:** 0.1.0 (Phase 6)
**License:** MIT
**Documentation:** https://nlm.readthedocs.io
