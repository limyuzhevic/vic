pynlm

This is a Python binding for the NLM (Neural Learning Machine) neural simulation framework.
The NLM is an experimental artificial developmental brain that learns through interaction with an environment.

## Quick Start

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Key Classes

- **Brain**: Central neural simulation brain class
- **Config**: Configuration system for NLM parameters
- **AgentBrain**: Interface connecting brain to world
- **SimpleWorld**: Simple 2D world for simulation
- **Vision**, **Audio**, **InternalSignals**: Sensory input types
- **SensoryPercept**: Combined sensory perception data
- **Action**: Motor action representation
- **WorldObject**: Objects in the simulation world
- **AgentBody**: Agent state information

## Advanced Usage

```python
import pynlm

# Create complete simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get statistics
stats = pynlm.getAgentStats(agent)
print(f"Curiosity: {stats['curiosity']:.3f}")
print(f"Novelty: {stats['novelty']:.3f}")
```

## Performance Monitoring

The Python API provides comprehensive performance monitoring capabilities:

```python
# Get detailed agent statistics
stats = pynlm.getAgentStats(agent)
print(f"Agent Performance:")
print(f"  Neurons firing: {stats['firing_neurons']}")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Curiosity level: {stats['curiosity']:.3f}")
print(f"  Novelty level: {stats['novelty']:.3f}")
```

## Error Handling

The bindings provide comprehensive error handling with detailed error messages:

```python
try:
    brain.step(step)
except RuntimeError as e:
    print(f"Error during simulation: {e}")
```

## File Structure

- `bindings/`: C++ binding source files
- `examples/`: Python usage examples
- `tests/`: Python unit tests
- `docs/`: Detailed documentation

## Building

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build from source
pip install -e .

# Or build only
pip install build
python -m build
```

## Getting Help

- Read `docs/ARCHITECTURE.md` for system architecture overview
- Check `docs/SCIENCE.md` for scientific background
- Look at `examples/` for practical usage patterns

The NLM framework provides a complete artificial brain simulation environment with neural dynamics, learning, memory, and developmental capabilities.
