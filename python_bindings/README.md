# NLM Python Bindings - Improved Documentation

## Overview

The NLM Python bindings provide a comprehensive interface to the Neural Learning Machine (NLM) framework, an experimental artificial developmental brain system.

## Installation

```bash
pip install -e .
```

## Basic Usage

```python
from nlm import Brain, Config, AgentBrain, SimpleWorld

# Create a brain with default configuration
config = Config()
brain = Brain(config)

# Create world
world = SimpleWorld()
world.configure(640, 480, 320, 240)

# Create agent brain
agent_brain = AgentBrain(brain, world)

# Initialize
brain.initialize()
agent_brain.initialize(world)
```

## Key Features

- Direct access to neural simulation systems
- Integration with memory, prediction, and cognition systems
- Simple interface to brain world interaction
- Checkpoint and save/load functionality
- Advanced brain management utilities

## Advanced Usage

See examples directory for advanced usage patterns.
```
```
