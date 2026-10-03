NLM (熙然) - Neural Learning Machine
Version 0.1.0

This is a package for the NLM C++ neural simulation framework. It provides Python bindings for creating and running neural brains with learning, memory, and adaptive capabilities.

### Core Concepts

NLM is designed around three main components:

1. **Brain**: The neural network that processes information, learns, and makes decisions
2. **World**: The environment the brain interacts with
3. **Agent**: The interface that connects the brain to the world

### Quick Example

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Package Contents

- `pynlm`: Main Python module with all bindings
- `pynlm/brain.py`: High-level brain abstractions
- `pynlm/world.py`: World simulation utilities
- `pynlm/agent.py`: Agent-brain interface
- `examples/`: Example scripts
- `docs/`: API documentation

### Installation

```bash
pip install pynlm
```

### Usage

See `examples/` for detailed usage examples, or read `API.md` for the complete API reference.

### License

MIT
