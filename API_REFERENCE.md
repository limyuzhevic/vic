# NLM API Reference

## Core Architecture

NLM (Neural Learning Machine) is an experimental computational brain project implementing Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation and multiple plasticity mechanisms.

## Module Overview

### Core Module
- **Config**: Configuration management with JSON support
- **Types**: Fundamental data types for neural simulation
- **Random**: Random number generation for stochastic processes
- **Logger**: Thread-safe logging system
- **SimulationClock**: Accurate time management

### Brain Module
- **Neuron**: LIF neuron model with spike-frequency adaptation
- **Synapse**: Synaptic connections with plasticity mechanisms
- **NeuralRegion**: Collections of neurons for modular organization
- **NeuralPopulation**: Groups of neurons with shared properties
- **Brain**: Main brain class integrating all subsystems

### Plasticity Module
- **STDP**: Spike-Timing Dependent Plasticity
- **Hebbian**: Hebbian learning rule
- **StructuralPlasticity**: Growth and pruning of connections
- **PlasticityRule**: Abstract base class for plasticity rules

### Neuromodulation Module
- **Dopamine**: Reward-based reinforcement learning
- **Curiosity**: Exploration motivation system
- **Novelty**: Novelty detection for learning
- **PredictionError**: Prediction error signals

### Memory Module
- **WorkingMemory**: Transient active information storage
- **EpisodicMemory**: Experience recording and replay
- **AssociativeMemory**: Pattern association storage

### Cognition Module
- **NeuralPlanner**: Action planning with evaluation
- **ConceptFormation**: Pattern discovery and abstraction

### Environment Module
- **SimpleWorld**: 2D world simulation
- **SensoryInput**: Base class for sensory modalities

### Agent Module
- **AgentBrain**: Interface connecting brain to world
- **SensoryPercept**: Collected sensory information

## Installation

### Prerequisites
```bash
# C++ compiler with C++20 support
gcc 10+, clang 12+, or MSVC 2019+

# CMake 3.16 or higher

# Python 3.8+ (for Python bindings)

# Build dependencies
pip install scikit-build-core pybind11 pytest numpy
```

### Building from Source
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

### Installing Python Bindings
```bash
# Install from source (recommended for development)
pip install .

# Or install in development mode
pip install -e .
```

## Basic Usage

### Creating a Brain
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Get basic statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Running a Simulation
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Complete Agent Example
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
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

## Configuration

### Brain Configuration Parameters
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
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Class Documentation

### Config
Manages configuration values with JSON and command-line support.

**Methods:**
- `loadFromFile(filepath)`: Load configuration from JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save configuration to JSON file
- `has(key)`: Check if configuration key exists
- `get(key)`: Get configuration value
- `set(key, value)`: Set configuration value
- `getKeys()`: Get all configuration keys
- `summary()`: Get configuration summary

### Brain
Central neural simulation brain class integrating all subsystems.

**Methods:**
- `initialize()`: Initialize brain with configuration
- `step(step)`: Perform simulation step
- `step(step, time)`: Perform simulation step with timestamp
- `receiveSensoryInput(input)`: Inject sensory input
- `injectCurrent(neuron, current)`: Inject current into specific neuron
- `injectCurrentToNeurons(type, current)`: Inject to all neurons of type
- `produceAction()`: Produce motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file
- `addRegion(name)`: Add neural region
- `getRegion(id)`: Get region by ID

### AgentBrain
Interface connecting NLM brain to world environment.

**Methods:**
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory percept
- `decodeMotorCommand()`: Decode brain activity to motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward signals
- `updateDevelopment(timestep)`: Update development system

### SimpleWorld
Simple 2D world for NLM simulation.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `reset()`: Reset to initial state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world state
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get agent's sensory input
- `getAgentBody()`: Get agent state

## Enumerations

### NeuronType
- `Excitatory`: Excitatory neurons
- `Inhibitory`: Inhibitory neurons
- `Modulatory`: Neuromodulatory neurons
- `Sensory`: Sensory neurons
- `Motor`: Motor neurons
- `Internal`: Internal neurons

### ActionType
- `MoveForward`, `MoveBackward`: Movement actions
- `TurnLeft`, `TurnRight`: Rotation actions
- `Look`, `LookUp`, `LookDown`: Vision actions
- `Interact`: Interaction actions
- `Eat`, `Drink`: Consumption actions
- `Rest`, `Wait`: Idle actions
- `Custom`: Custom actions

### DevelopmentalStage
- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period for learning
- `Maturation`: Maturation stage
- `Adult`: Adult stage
- `Aging`: Aging stage

## Error Handling

NLM provides comprehensive error handling:

### Common Exceptions
- `RuntimeError`: General runtime errors
- Configuration errors (invalid values, missing files)
- Simulation errors (invalid state, out of bounds)
- Memory errors (allocation failures)

### Error Handling Best Practices
1. Always check return values of `initialize()` and `load()`
2. Use try-catch blocks for Python binding errors
3. Handle configuration validation before simulation
4. Implement proper cleanup with `reset()` between episodes

## Performance Considerations

### Memory Management
- Use `reset()` between episodes to free memory
- Monitor memory usage during long simulations
- Consider smaller brain configurations for real-time applications

### Simulation Speed
- Adjust `simulation_timestep` for trade-offs between accuracy and speed
- Use configuration files to optimize for specific use cases
- Consider parallel processing for large networks

### Configuration Optimization
```python
# Optimize for performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Smaller network
config.set("simulation_timestep", 0.01)  # Larger timestep
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected

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

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Advanced Features

### Custom Plasticity Rules
```python
# Example: Custom STDP with asymmetric time constants
import pynlm

config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.ltp_weight", 0.02)
config.set("plasticity.stdp.ltd_weight", 0.01)
config.set("plasticity.stdp.time_constant", 15.0)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory Checkpointing
```python
# Save and load brain state for long experiments
brain.save("checkpoint_1000.bin")
# ... run many steps ...
brain.load("checkpoint_1000.bin")
```

### Development Stages
```python
# Control developmental stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
# ... run during critical period ...
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
```

## References

1. [NLM Documentation](https://nlm.readthedocs.io)
2. [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
3. [Architecture Overview](docs/ARCHITECTURE.md)
4. [Scientific Background](docs/SCIENCE.md)

## License

MIT License. See LICENSE file for details.