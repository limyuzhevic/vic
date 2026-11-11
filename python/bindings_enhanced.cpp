# Python bindings for NLM - Enhanced Edition

This file provides comprehensive Python bindings for the NLM (Neural Learning Machine) 
framework, with enhanced functionality beyond the basic C++ API.

Key improvements over basic bindings:
- High-level simulation control with automatic timing
- Agent interaction utilities
- Learning and training helpers
- Visualization and plotting support
- Progress monitoring and callbacks
- Configuration presets and templates
- Neural network analysis tools
- Episode management for reinforcement learning
- Serialization utilities
- Debugging and profiling tools

The bindings follow Pythonic conventions with:
- Context managers for resource cleanup
- Method chaining where appropriate
- Type hints and documentation
- Error handling with clear messages
- Progress callbacks for long-running operations

Typical usage patterns:

```python
import nlm

# Create a brain with preset configuration
brain = nlm.create_brain_with_preset("default")
brain.initialize()

# Run simulation with automatic timing
results = nlm.run_simulation(
    brain,
    duration=1000,
    sensory_inputs=sensory_generator,
    action_callback=policy
)

# Train using reinforcement learning
agent = nlm.create_agent(brain, world)
learning_results = nlm.train_agent(
    agent,
    episodes=100,
    max_steps=200,
    exploration_rate=0.1,
    callback=lambda e, r: print(f"Episode {e}: Reward {r}")
)
```