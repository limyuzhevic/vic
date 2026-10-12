# NLM Python Troubleshooting

## Common Issues and Solutions

### Issue 1: Import Errors

**Problem:** `ModuleNotFoundError: No module named 'nlm'`

**Solution:**

1. Ensure you're in the correct directory where the `nlm` package is located
2. Install the package in development mode:

```bash
pip install -e .
```

3. Or add the package to your Python path:

```python
import sys
sys.path.insert(0, '/path/to/nlm/package')
from nlm import Brain, World
```

### Issue 2: "Brain not properly initialized"

**Problem:** `RuntimeError: Brain not properly initialized`

**Solution:**

1. Check that you're creating Brain objects correctly:

```python
# Before (may cause error)
brain = Brain(invalid_param="test")

# After
from nlm import create_brain
brain = create_brain(learning_rate=0.01, hidden_size=128)
```

2. Ensure you're passing valid configuration parameters
3. Check that the Brain constructor is being called with proper arguments

### Issue 3: "Agent has no brain set"

**Problem:** `RuntimeError: Agent has no brain set`

**Solution:**

1. Always set a brain before using an agent:

```python
from nlm import create_agent, Brain

brain = Brain()
agent = create_agent("my_agent", brain)  # Set brain during creation

# OR
agent = create_agent("my_agent")  # No brain
agent.set_brain(brain)  # Set brain afterwards
```

2. For high-level functions:

```python
from nlm import create_agent, create_brain

brain = create_brain()
agent = create_agent("my_agent", brain)
```

### Issue 4: World Initialization Problems

**Problem:** "World not properly initialized" errors

**Solution:**

1. Use valid world names:

```python
from nlm import World

# Valid
world = World("my_world")

# Invalid (may cause issues)
world = World("")
world = World(123)  # Must be a string
```

2. Ensure all components are properly added before running simulations
3. Check that you're not removing brains/agents that are in use

### Issue 5: Parameter Type Errors

**Problem:** `TypeError: Parameter name must be a string`

**Solution:**

1. Always use string keys for configuration:

```python
# Valid
brain = create_brain(learning_rate=0.01, hidden_size=128)

# Invalid (causes TypeError)
brain = create_brain(learning_rate=0.01, hidden_size=128, 123="invalid")
```

2. Check all dictionary keys are strings when passing parameters

### Issue 6: Simulation Errors

**Problem:** Simulation fails or hangs

**Solution:**

1. Validate simulation parameters:

```python
from nlm import create_brain, run_simulation

# Valid
brain = create_brain()
run_simulation(brain, steps=100)  # steps is int

# Invalid (causes ValueError)
run_simulation(brain, steps=-10)  # Negative steps
run_simulation(brain, steps="100")  # String steps
```

2. Check that the Brain is in a valid state before running simulation
3. Ensure all required parameters are set

### Issue 7: Configuration Issues

**Problem:** Configuration parameters not being applied

**Solution:**

1. Check configuration parameter names match expected format
2. Verify parameter types are correct:

```python
from nlm import Config

config = Config()
config.set("learning_rate", 0.01)  # Float
config.set("batch_size", 32)      # Int
config.set("optimizer", "adam")   # String
```

3. Use the `get()` method to retrieve configuration values:

```python
learning_rate = config.get("learning_rate", 0.001)
```

### Issue 8: Agent Decision-Making Problems

**Problem:** Agents not making proper decisions

**Solution:**

1. Ensure agents have brains set:

```python
from nlm import create_agent, create_brain

brain = create_brain()
agent = create_agent("my_agent", brain)
```

2. Check that observation data is in the correct format:

```python
# Valid observation
observation = {"sensor_data": [1, 2, 3], "timestamp": 123}

# Invalid observation
observation = None  # May cause issues
```

3. Update agent parameters as needed:

```python
agent.update_parameters(strategy="exploration", epsilon=0.1)
```

### Issue 9: World Agent Management

**Problem:** Agents not being found or managed properly

**Solution:**

1. Use consistent naming:

```python
from nlm import World, create_agent, Brain

world = World("production_world")
brain = Brain()
agent1 = create_agent("agent_1", brain)
agent2 = create_agent("agent_2", brain)

world.add_agent(agent1)
world.add_agent(agent2)
```

2. Check agent existence before operations:

```python
agents = world.get_agents()
if "agent_1" in [a.name for a in agents]:
    # Agent exists
    pass
```

3. Handle errors gracefully:

```python
try:
    world.remove_brain(brain)
except ValueError as e:
    print(f"Error removing brain: {e}")
```

### Issue 10: Memory and Performance Issues

**Problem:** High memory usage or slow performance

**Solution:**

1. Limit simulation steps for testing:

```python
# For testing, use fewer steps
brain.run_simulation(steps=10)  # Instead of 1000
```

2. Clean up resources when done:

```python
# Delete references to free memory
brain = None
world = None
agent = None
```

3. Monitor memory usage during long simulations

## Debugging Techniques

### Technique 1: Print Statements

Add print statements to debug your code:

```python
from nlm import create_brain, World, create_agent

# Debug brain creation
brain = create_brain(learning_rate=0.01, hidden_size=128)
print(f"Brain created: {brain}")
print(f"Brain parameters: {brain.get_parameters()}")

# Debug world operations
world = World("debug_world")
print(f"World created: {world}")

# Debug agent operations
agent = create_agent("debug_agent", brain)
print(f"Agent created: {agent}")
```

### Technique 2: Exception Handling

Use try-except blocks to catch and handle errors:

```python
from nlm import create_brain, World, create_agent

try:
    brain = create_brain(learning_rate=0.01, hidden_size=128)
    world = World("test_world")
    agent = create_agent("test_agent", brain)
    
    # Run simulation
    result = world.run_simulation(steps=100)
    print("Simulation completed successfully")
    
except ValueError as e:
    print(f"ValueError: {e}")
except TypeError as e:
    print(f"TypeError: {e}")
except RuntimeError as e:
    print(f"RuntimeError: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Technique 3: Use Examples

Run the provided examples to understand the correct usage:

```bash
python nlm/examples/quick_start.py
python nlm/examples/simulation.py
python nlm/examples/agent_brain.py
```

### Technique 4: Check Documentation

Always refer to the latest documentation:

1. **API Reference**: `nlm/docs/api.md`
2. **Tutorials**: `nlm/docs/tutorials.md`
3. **Package README**: Check for additional documentation

## Error Code Reference

| Error Type | Message | Cause | Solution |
|------------|---------|-------|----------|
| ValueError | "Steps must be a positive integer" | Invalid steps parameter | Use positive integer for steps |
| TypeError | "Parameter name must be a string" | Invalid parameter name | Use string keys for parameters |
| RuntimeError | "Brain not properly initialized" | Invalid Brain state | Check Brain creation |
| RuntimeError | "Agent has no brain set" | No brain for agent | Set brain for agent |
| ValueError | "Brain already exists in the world" | Duplicate brain | Check for duplicates |
| TypeError | "brain must be a Brain instance" | Wrong type | Use Brain instances |
| RuntimeError | "World not properly initialized" | Invalid World state | Check World creation |

## Support Resources

### Getting Help

1. **Documentation**: Read `nlm/docs/api.md` and `nlm/docs/tutorials.md`
2. **Examples**: Run examples in `nlm/examples/` directory
3. **Error Messages**: Use detailed error messages to identify issues
4. **Community**: Join NLM community for support

### Common Patterns for Success

1. **Always validate inputs** before using NLM functions
2. **Use high-level functions** for simple usage
3. **Use low-level classes** for advanced control
4. **Handle exceptions** gracefully
5. **Test with small simulations** before running large ones
6. **Use examples** as reference for correct usage

## Version-Specific Information

Current version: 1.0.0

This troubleshooting guide is for NLM Python package version 1.0.0. For older versions, refer to version-specific documentation.

## Last Updated

2026-10-12

For the latest troubleshooting information, check the documentation or contact support.