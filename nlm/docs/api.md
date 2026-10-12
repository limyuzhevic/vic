# NLM Python API Reference

## Overview

The NLM Python package provides a simplified interface to the Neural Language Model (NLM) framework, with both simple "one-line" usage and advanced programmatic control.

## Core Classes

### Brain

The `Brain` class represents a neural network model with simplified configuration and control.

#### Methods

- `__init__(config=None, **kwargs)` - Initialize a new Brain instance
- `run_simulation(steps=100, **kwargs)` - Run a simulation with the brain
- `get_parameters()` - Get current brain parameters
- `update_parameters(**kwargs)` - Update brain parameters

#### Example

```python
from nlm import Brain

brain = Brain(learning_rate=0.01, hidden_size=128)
result = brain.run_simulation(steps=100)
```

### World

The `World` class represents a simulation environment for NLM operations.

#### Methods

- `__init__(name="default_world", **kwargs)` - Initialize a new World instance
- `add_brain(brain)` - Add a brain to the world
- `remove_brain(brain)` - Remove a brain from the world
- `add_agent(agent)` - Add an agent to the world
- `get_brains()` - Get all brains in the world
- `get_agents()` - Get all agents in the world
- `run_step()` - Run a single world step
- `run_simulation(steps=100)` - Run a simulation in the world

#### Example

```python
from nlm import World, Brain

world = World("my_world")
brain = Brain()
world.add_brain(brain)
result = world.run_simulation(steps=50)
```

### Agent

The `Agent` class represents an autonomous entity in the NLM framework.

#### Methods

- `__init__(name, brain=None, **kwargs)` - Initialize a new Agent instance
- `set_brain(brain)` - Set the agent's brain
- `get_parameters()` - Get agent parameters
- `update_parameters(**kwargs)` - Update agent parameters
- `think(observation=None)` - Make a decision based on observation
- `act(action, parameters=None)` - Perform an action
- `run_simulation(steps=100)` - Run a simulation with the agent

#### Example

```python
from nlm import Agent, Brain

brain = Brain()
agent = Agent("my_agent", brain)
decision = agent.think("observation")
result = agent.act("move", {"direction": "forward"})
```

### Config

The `Config` class provides configuration management with validation.

#### Methods

- `__init__(**kwargs)` - Initialize a new Config instance
- `get(key, default=None)` - Get a configuration value
- `set(key, value)` - Set a configuration value
- `get_parameters()` - Get all configuration parameters
- `update(**kwargs)` - Update configuration parameters

#### Example

```python
from nlm import Config

config = Config(learning_rate=0.01, batch_size=32)
value = config.get("learning_rate")
config.set("batch_size", 64)
```

## High-Level Functions

### create_brain

Create a new Brain instance with simplified API.

```python
from nlm import create_brain

brain = create_brain(learning_rate=0.01, hidden_size=128)
```

### run_simulation

Run a simulation with the given brain.

```python
from nlm import Brain, run_simulation

brain = Brain()
run_simulation(brain, steps=100)
```

### create_agent

Create a new Agent instance.

```python
from nlm import create_agent, Brain

brain = Brain()
agent = create_agent("my_agent", brain)
```

### get_version

Get the NLM package version.

```python
from nlm import get_version

version = get_version()
```

## Error Handling

NLM uses standard Python exceptions with detailed error messages:

- `ValueError`: Invalid parameter values
- `TypeError`: Invalid parameter types
- `RuntimeError`: Invalid state operations

## Advanced Usage

### Direct C++ Access

For advanced users who need direct access to the C++ implementation, the package provides:

```python
from nlm import Brain

# Direct C++ access (advanced usage)
brain = Brain()
cpp_model = brain.get_cpp_model()  # Advanced API
```

## Type Hints

All public methods include comprehensive type hints for better IDE support.

## Package Structure

```
nlm/
├── __init__.py          # Core exports, easy functions
├── brain.py             # Brain class with simplified API
├── world.py             # World class
├── agent.py             # Agent wrapper
├── config.py            # Configuration wrapper
├── examples/            # Usage examples
└── docs/                # Python documentation
```