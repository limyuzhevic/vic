# NLM - Enhanced Usage Guide with Modern Python Features

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## New Pythonic Features

This guide documents the enhanced Python API with modern Python features:

### 1. Module Metadata

```python
import pynlm

# Access module information
print(pynlm.__version__)      # "0.1.0"
print(pynlm.__author__)       # "NLM Authors"
print(pynlm.__description__)  # "Python bindings for NLM..."
```

### 2. Custom Exception Hierarchy

```python
import pynlm

# New custom exceptions
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Example: Catch specific NLM errors
try:
    config = pynlm.createDefaultConfig()
    config.loadFromFile("invalid_config.json")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
```

### 3. Pythonic Properties and Context Managers

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use Pythonic properties
print(f"Neurons: {brain.total_neurons}")
print(f"Synapses: {brain.total_synapses}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Average firing rate: {brain.average_firing_rate}")

# Context manager support
with brain:  # Context manager available
    for step in range(100):
        brain.step(step)
    print(f"Final stats: {brain.firing_neurons} neurons firing")
```

### 4. Convenience Functions

```python
import pynlm

# Quick setup for agent simulation
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
agent = pynlm.setup_agent_simulation(config, world)
agent.initialize(world)

# Statistical helper
stats = pynlm.brain_stats(brain)
print(f"Brain stats: {stats}")

# Enable all features at once
pynlm.enable_all_features(agent)

# Get comprehensive agent state
agent_state = pynlm.get_agent_state(agent)
print(f"Agent neuromodulation level: {agent_state['neuromodulation_level']}")
```

### 5. Simulation Utilities

```python
import pynlm

# Create everything with helper functions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.create_world_with_objects(20, 20, 8, 8)
agent = pynlm.createAgentBrain(brain)

# Run complete episode
pynlm.run_episode(brain, agent, world, 500, 0.1)

# Batch operations
pynlm.batch_step_brain(brain, 1000)
```

### 6. Error Handling Helpers

```python
import pynlm

# Safe operation execution
success = pynlm.safe_brain_operation(brain, [](Brain& b) {
    b.step(42);
    // Some risky operation
});

if success:
    print("Operation completed successfully")
else:
    print("Operation failed or was interrupted")
```

## Complete Example: Modern Agent Simulation

```python
import pynlm

class SmartAgent:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Quick setup
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable all features for advanced learning
        pynlm.enable_all_features(self.agent)
    
    def run_simulation(self, steps=1000):
        print(f"Starting agent simulation with {steps} steps")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action from brain
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = self._get_reward(percept, action)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                stats = pynlm.brain_stats(self.brain)
                agent_state = pynlm.get_agent_state(self.agent)
                print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                      f"curiosity: {agent_state['curiosity_level']:.2f}")
    
    def _get_reward(self, percept, action):
        # Example reward function
        return percept.getInternal()[0] if percept.getInternal() else 0.0

def advanced_simulation_example():
    """Example of advanced simulation using all features"""
    print("=== Advanced NLM Simulation Example ===")
    
    agent = SmartAgent()
    agent.run_simulation(1000)
    
    print("=== Simulation Complete ===")
    print(f"Final brain stats: {pynlm.brain_stats(agent.brain)}")
    print(f"Final agent state: {pynlm.get_agent_state(agent.agent)}")

# Run the example
advanced_simulation_example()
```

## Getting Started with Enhanced NLM

### Basic Usage (Unchanged)

```python
import pynlm

# Still works exactly as before
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

### New Efficient Patterns

```python
import pynlm

# Pattern 1: Quick agent setup
config = pynlm.createDefaultConfig()
world = pynlm.createSimpleWorld()
agent = pynlm.setup_agent_simulation(config, world)

# Pattern 2: Batch processing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
pynlm.batch_step_brain(brain, 10000)

# Pattern 3: Statistical analysis
stats = pynlm.brain_stats(brain)
print(f"Total neurons: {stats['neurons']}")
print(f"E/I ratio: {stats['excitation_inhibition_ratio']}")
```

## Migration Guide

### From Old to New API

**Old API:**
```python
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

**New API:**
```python
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.total_neurons}")
```

**Old API:**
```python
stats = {}
stats["neurons"] = brain.getTotalNeuronCount()
stats["synapses"] = brain.getTotalSynapseCount()
```

**New API:**
```python
stats = pynlm.brain_stats(brain)
```

## Error Handling Best Practices

```python
import pynlm

def robust_simulation():
    try:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        if not config.has("brain.neuron_count"):
            raise pynlm.ConfigurationError("Required brain.neuron_count not found")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Safe operation
        success = pynlm.safe_brain_operation(brain, [](Brain& b) {
            b.step(0);
        })
        
        return success
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration problem: {e}")
        return False
    except pynlm.SimulationError as e:
        print(f"Simulation failed: {e}")
        return False
    except pynlm.ValidationError as e:
        print(f"Validation error: {e}")
        return False
    except pynlm.NLMException as e:
        print(f"NLM error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

# Usage
robust_simulation()
```

## Performance Tips

1. **Use batch operations**: `pynlm.batch_step_brain(brain, 10000)` for efficient processing
2. **Leverage convenience functions**: `pynlm.setup_agent_simulation()` for common patterns
3. **Monitor statistics**: Use `pynlm.brain_stats()` for performance tracking
4. **Safe operations**: Use `pynlm.safe_brain_operation()` for robust error handling

## Advanced Features

### Context Managers

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Automatic cleanup
with brain:
    for i in range(1000):
        brain.step(i)
        if i % 100 == 0:
            print(f"Progress: {i} steps")

# Resources automatically cleaned up
```

### Property Access

```python
import pynlm

config = pynlm.createDefaultConfig()

# Property-based configuration
config.neuron_count = 1000
config.synapse_density = 0.1
config.learning_rate = 0.001

print(f"Configured with {config.neuron_count} neurons")
```

## Conclusion

The enhanced NLM Python API now provides:

1. **Modern Python features**: Properties, context managers, type hints
2. **Better error handling**: Custom exception hierarchy
3. **Convenience functions**: Simplified common patterns
4. **Enhanced documentation**: Comprehensive examples and migration guide
5. **Backward compatibility**: Existing code continues to work
6. **Performance improvements**: Batch operations and efficient helpers

These improvements make NLM more accessible to Python developers while maintaining the power of the underlying C++ neural simulation framework.