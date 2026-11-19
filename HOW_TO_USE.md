# NLM - Core API Reference

This is the essential API reference for NLM. For detailed tutorials and examples, see `README.md`.

## Core Classes

### Config Class

```python
# Creation
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Set configuration (auto-converts types)
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Advanced operations
json_str = config.toJson()  # Export to JSON
missing_keys = config.validate()  # Validate configuration
```

### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)      # Run step with timestamp
brain.stepMultiple(100)            # Run multiple steps efficiently
brain.runSimulation(1000, verbose=True)  # Run with progress reporting

# Input/Output
brain.receiveSensoryInput(input)   # Inject sensory data
action = brain.produceAction()     # Get motor action

# State management
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
brain.reset()                      # Reset brain state

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()       # Total synapses
brain.getFiringNeuronCount()       # Currently firing
brain.getAverageFiringRate()       # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# Advanced operations
brain.injectCurrent(neuron_id, current)                    # Inject current
brain.injectCurrentToNeurons(neuron_type, current)         # Inject to all of type
brain.applyNeuromodulation(signal)                         # Apply neuromodulation
brain.updatePlasticity()                                    # Update plasticity
brain.develop()                                             # Apply development
brain.setLearnedPreference(value)                          # Set learned preference
```

### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)                    # Initialize with world
agent.initializeSimple()                    # Simple initialization

# Sensory processing
agent.processSensoryInput(percept)        # Process sensory input

# Motor decoding
cmd = agent.decodeMotorCommand()           # Decode motor command
cmd, activities = agent.decodeWithDetails()  # With activity details

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)  # Apply reward

# Development
agent.updateDevelopment(timestep)          # Update development
agent.getDevelopmentalStage()             # Get current stage

# Subsystem control
agent.enableRewardModulation(True/False)   # Enable/disable
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getNeuromodulationLevel()            # Get neuromodulation
agent.getCuriosityLevel()                  # Get curiosity
agent.getNoveltyLevel()                    # Get novelty
agent.getPredictionError()                 # Get prediction error
agent.getCuriosityLevel()                  # Get curiosity
agent.getSocialObservationCount()          # Get observation count

# Advanced features
agent.observeOtherAgent(observed, before_vision, after_vision)  # Social learning
agent.encodeText("hello world")            # Text encoding
agent.getThoughtTrace()                    # Get thought trace
agent.getMotorGroupActivities()            # Get activity details
```

### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()                              # Reset to initial state
world.update(timestep)                      # Update world
world.setRandomSeed(seed)                   # Set random seed
world.setAgentStart(x, y)                   # Set agent position

# Interaction
world.applyMotorCommand(motor_cmd, current_time)  # Apply action

# State access
percept = world.getSensoryPercept()         # Get sensory data
body = world.getAgentBody()                 # Get agent body

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource))  # Add object
world.removeObject(x, y)                    # Remove object
world.isValidPosition(x, y)                 # Check position validity

# Properties
world.getWidth()                            # Get world width
world.getHeight()                           # Get world height
world.getSimulationTime()                   # Get simulation time
world.getMaxEnergy()                        # Get max energy
world.setMaxEnergy(100.0)                   # Set max energy
```

## Enumerations

### NeuronType
```python
pynlm.NeuronType.Excitatory    # Standard excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons (20% of internal population)
pynlm.NeuronType.Sensory       # Sensory input neurons
pynlm.NeuronType.Motor         # Motor output neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Internal     # Internal computation neurons
```

### ActionType
```python
pynlm.ActionType.MoveForward   # Move forward
pynlm.ActionType.MoveBackward  # Move backward
pynlm.ActionType.TurnLeft      # Turn left
pynlm.ActionType.TurnRight     # Turn right
pynlm.ActionType.Interact      # Interact with objects
pynlm.ActionType.Eat           # Eat food resources
pynlm.ActionType.Drink         # Drink water
pynlm.ActionType.Rest          # Rest/sleep
pynlm.ActionType.Wait          # Do nothing
pynlm.ActionType.Custom        # Custom actions
```

### MotorCommand
```python
pynlm.MotorCommand.MoveForward  # Movement command
pynlm.MotorCommand.MoveBackward # Movement command
pynlm.MotorCommand.TurnLeft     # Turn command
pynlm.MotorCommand.TurnRight    # Turn command
pynlm.MotorCommand.LookLeft     # Look left
pynlm.MotorCommand.LookRight    # Look right
pynlm.MotorCommand.Interact     # Interact command
pynlm.MotorCommand.Wait        # Wait command
```

### WorldObjectType
```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Food/resources
pynlm.WorldObjectType.Hazard    # Dangerous objects
pynlm.WorldObjectType.Wall      # Walls/boundaries
pynlm.WorldObjectType.Marker    # Special markers
```

### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial         # Starting stage (high plasticity)
pynlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
pynlm.DevelopmentalStage.Maturation      # Development toward adulthood
pynlm.DevelopmentalStage.Adult           # Stable adult state
pynlm.DevelopmentalStage.Aging           # Age-related decline
```

## Configuration Parameters

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Example Scripts

### Minimal Example
```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Advanced Features

### Text as Sensory Input

NLM can process text as a special sensory channel:

```python
# Text is automatically encoded and injected as a sensory pattern
percept = world.getSensoryPercept()
text_pattern = percept.getText()  # This is the raw text string

# Agent encodes it internally using a hash-based pattern
encoded_neurons = agent.encodeText("hello world")
```

### Self-Model and Planning

The agent contains sophisticated cognitive systems:

```python
# Self-model records what actions led to current perception
# This allows the agent to predict the consequences of its actions

# Planning uses learned action quality to suggest optimal behavior
planned_action = brain.getPlanner().planAction(percept)
```

### Social Learning

Multiple agents can learn from each other:

```python
# Observe another agent's behavior
agent.observeOtherAgent(
    MotorCommand.MoveForward,
    before_vision,
    after_vision
)
```

## Building and Installation

### From Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install -e .  # Development mode
# or
pip install .     # Release mode

# Or build only
pip install build
python -m build
pip install dist/*.whl
```

### Requirements

- C++20 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake >= 3.16
- Python >= 3.8
- pybind11 >= 2.11.0 (for Python bindings)

### Build Options

```bash
# Debug build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Testing

### Running Tests

```bash
# Unit tests (C++)
cd build
ctest --output-on-failure

# Python tests
pytest tests/  # If any Python tests exist
```

### Test Coverage

The test suite includes:
- Neuron dynamics (LIF model)
- Synapse connectivity and plasticity
- Brain simulation integration
- Agent-world interaction
- Memory systems
- Neuromodulation
- Development stages

## Troubleshooting

### ImportError: No module named 'pynlm'

```bash
pip install --force-reinstall .
```

### Segmentation faults

```bash
# Ensure proper initialization order
brain = pynlm.createBrain(config)
brain.initialize()  # CRITICAL: always call this first
# Then run simulation steps
```

### Performance Issues

```python
# For long simulations, use optimized batch processing
brain.stepMultiple(10000)  # More efficient than loop

# Monitor progress
brain.runSimulation(1000, verbose=True)
```

## Further Reading

- **ARCHITECTURE.md** - System architecture overview
- **SCIENCE.md** - Scientific background and neural modeling
- **EXPERIMENTS.md** - Experiment descriptions and protocols
- **PHASE6_FINAL_AUDIT.md** - Phase 6 integration audit report

## License

MIT License. See LICENSE file for details.
