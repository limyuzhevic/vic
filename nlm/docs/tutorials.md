# NLM Python Tutorials

## Tutorial 1: Quick Start

### Introduction

This tutorial will get you started with NLM in just a few lines of code. It's perfect for beginners who want to quickly experiment with the framework.

### What You'll Learn

- How to create a Brain instance
- How to run a simulation
- Basic error handling

### Steps

#### Step 1: Import the Package

```python
from nlm import create_brain, run_simulation, create_agent
```

#### Step 2: Create a Brain

```python
# Simple one-line usage
brain = create_brain(learning_rate=0.01, hidden_size=128)
```

#### Step 3: Run a Simulation

```python
# Run a simple simulation
run_simulation(brain, steps=100)
```

#### Step 4: Create an Agent (Optional)

```python
# Create an agent with your brain
agent = create_agent("my_agent", brain)
result = agent.run_simulation(steps=50)
```

#### Step 5: Complete Example

```python
from nlm import create_brain, run_simulation, create_agent

# Create brain
brain = create_brain(learning_rate=0.01, hidden_size=128)

# Run simulation
run_simulation(brain, steps=100)

# Create agent
agent = create_agent("my_agent", brain)
result = agent.run_simulation(steps=50)

print("Quick start completed!")
```

## Tutorial 2: World Simulation

### Introduction

This tutorial covers how to create and manage simulation worlds using NLM.

### What You'll Learn

- How to create a World instance
- How to add brains and agents to a world
- How to run world simulations

### Steps

#### Step 1: Set Up the Environment

```python
from nlm import World, Brain, create_agent
```

#### Step 2: Create a World

```python
world = World("my_simulation", environment="test")
```

#### Step 3: Create Brains and Agents

```python
brain = Brain(learning_rate=0.001, hidden_size=256)
agent = create_agent("simulation_agent", brain)
```

#### Step 4: Add to World

```python
world.add_brain(brain)
world.add_agent(agent)
```

#### Step 5: Run World Simulation

```python
result = world.run_simulation(steps=100)
print(f"Simulation completed with {result['total_steps']} steps")
```

#### Step 6: World Management

```python
# Get brains and agents
brains = world.get_brains()
agents = world.get_agents()

# Remove brain
world.remove_brain(brain)
```

## Tutorial 3: Agent-Brain Interaction

### Introduction

This tutorial focuses on how agents interact with brains, making decisions and performing actions.

### What You'll Learn

- How agents use brains for decision-making
- How to perform actions
- How to update parameters

### Steps

#### Step 1: Set Up

```python
from nlm import create_brain, create_agent
```

#### Step 2: Create Brain and Agent

```python
brain = create_brain(learning_rate=0.01, hidden_size=128)
agent = create_agent("decision_maker", brain)
```

#### Step 3: Think (Make Decisions)

```python
# Make decisions based on observations
observation = "observation_data"
decision = agent.think(observation)
print(f"Agent decided to: {decision['action']}")
```

#### Step 4: Act (Perform Actions)

```python
# Perform actions
parameters = {"direction": "forward", "speed": 1.0}
result = agent.act("move", parameters)
print(f"Action completed: {result['status']}")
```

#### Step 5: Update Parameters

```python
# Update brain parameters
brain.update_parameters(learning_rate=0.005)

# Update agent parameters
agent.update_parameters(strategy="exploration", epsilon=0.1)
```

## Tutorial 4: Learning and Training

### Introduction

This tutorial covers how to set up and manage learning scenarios with NLM.

### What You'll Learn

- How to configure learning parameters
- How to run training simulations
- How to track progress

### Steps

#### Step 1: Configure Learning Brain

```python
from nlm import create_brain, World, create_agent

brain = create_brain(
    learning_rate=0.001,
    batch_size=32,
    epochs=10,
    validation_split=0.2,
    optimizer="adam",
    loss="cross_entropy"
)
```

#### Step 2: Set Up Training World

```python
world = World("learning_world")
trainer = create_agent("trainer", brain)
validator = create_agent("validator", brain)

world.add_agent(trainer)
world.add_agent(validator)
```

#### Step 3: Run Training Loop

```python
# Training loop
for epoch in range(brain.get("epochs", 10)):
    # Training step
    train_result = trainer.think({"mode": "train", "epoch": epoch})
    train_action = trainer.act("train", {"loss": 0.5 - (epoch * 0.05)})
    
    # Validation (every 5 epochs)
    if epoch % 5 == 0:
        val_result = validator.think({"mode": "validate"})
        print(f"Epoch {epoch}: Validation accuracy = {val_result.get('accuracy', 0.0):.4f}")
```

## Tutorial 5: Visualization and Monitoring

### Introduction

This tutorial covers how to monitor and visualize NLM operations.

### What You'll Learn

- How to enable visualization
- How to monitor simulation steps
- How to create custom visualizations

### Steps

#### Step 1: Enable Visualization

```python
from nlm import create_brain, World, create_agent

brain = create_brain(
    learning_rate=0.01,
    hidden_size=64,
    visualize=True
)
```

#### Step 2: Create Visualization World

```python
world = World("visualization_world")
agent = create_agent("visualizer", brain)
world.add_agent(agent)
world.add_brain(brain)
```

#### Step 3: Run Visualization

```python
# Run step-by-step visualization
for i in range(5):
    step_info = {
        'step_number': i + 1,
        'world_name': world.name,
        'brains_count': len(world.get_brains()),
        'agents_count': len(world.get_agents()),
        'status': 'completed'
    }
    
    print(f"\nStep {i + 1}:")
    print(f"  World: {step_info['world_name']}")
    print(f"  Brains: {step_info['brains_count']}")
    print(f"  Agents: {step_info['agents_count']}")
    
    # Run actual step
    world.run_step()
```

## Common Patterns

### Pattern 1: Reusable Brain Factory

```python
from nlm import create_brain

def create_learning_brain(learning_rate=0.001, hidden_size=128):
    """Create a brain configured for learning."""
    return create_brain(
        learning_rate=learning_rate,
        hidden_size=hidden_size,
        optimizer="adam",
        loss="cross_entropy"
    )

# Usage
brain1 = create_learning_brain()
brain2 = create_learning_brain(learning_rate=0.01)
```

### Pattern 2: Agent Manager

```python
from nlm import create_agent, World

class AgentManager:
    def __init__(self, world_name="default"):
        self.world = World(world_name)
        self.agents = {}
    
    def add_agent(self, name, brain=None, **kwargs):
        """Add an agent to the manager."""
        agent = create_agent(name, brain, **kwargs)
        self.world.add_agent(agent)
        self.agents[name] = agent
        return agent
    
    def run_all_agents(self, steps=100):
        """Run simulation for all agents."""
        for agent in self.agents.values():
            agent.run_simulation(steps)

# Usage
manager = AgentManager("production")
manager.add_agent("worker_1", brain1)
manager.add_agent("worker_2", brain2)
manager.run_all_agents()
```

### Pattern 3: Configuration Manager

```python
from nlm import Config

class NLMConfig:
    def __init__(self):
        self.global_config = Config()
    
    def set_global(self, **kwargs):
        """Set global configuration."""
        self.global_config.update(**kwargs)
    
    def create_brain(self, **kwargs):
        """Create a brain with global configuration."""
        brain_config = self.global_config.get_parameters()
        brain_config.update(kwargs)
        return Brain(**brain_config)

# Usage
config = NLMConfig()
config.set_global(learning_rate=0.001, batch_size=32)
brain1 = config.create_brain(hidden_size=128)
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: "Brain not properly initialized"

**Solution:** Ensure the Brain is created with valid configuration parameters.

```python
# Before
brain = Brain(invalid_param="test")

# After
brain = Brain(learning_rate=0.01, hidden_size=128)
```

#### Issue: "Agent has no brain set"

**Solution:** Set the agent's brain before using it for decision-making.

```python
from nlm import create_agent, Brain

brain = Brain()
agent = create_agent("test_agent")
agent.set_brain(brain)  # Set brain before using
```

#### Issue: "World not properly initialized"

**Solution:** Ensure the World is created with valid parameters.

```python
from nlm import World

world = World("valid_world")  # Default valid name
```

#### Issue: Invalid parameter types

**Solution:** Check parameter types before passing to NLM functions.

```python
from nlm import create_brain

# Invalid (causes TypeError)
brain = create_brain(steps="string")  # steps must be int

# Valid
brain = create_brain(steps=100)  # steps is int
```

### Error Messages

NLM provides detailed error messages for common issues:

- `"Brain not properly initialized"` - Check Brain initialization
- `"Agent has no brain set"` - Set brain before using
- `"World not properly initialized"` - Check World initialization
- `"Parameter name must be a string"` - Use string parameter names
- `"Steps must be a positive integer"` - Use positive integers for steps

## Next Steps

### Advanced Features

1. **Direct C++ Access**: Use advanced API for low-level control
2. **Multi-threading**: Run multiple simulations in parallel
3. **Distributed Training**: Set up distributed learning across multiple machines

### Community Resources

- Check the `examples/` directory for more use cases
- Visit the documentation in `docs/` for detailed API reference
- Join the NLM community for support and collaboration

## Version Information

Current version: 1.0.0

This tutorial is designed to be run with Python 3.7 or higher. Make sure you have the latest version of the NLM package installed.