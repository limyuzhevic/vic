# NLM Python Examples

This directory contains example Python scripts for the NLM (Neural Learning Machine) framework.

## Overview

These examples demonstrate different aspects of the NLM Python API, from simple basic usage to advanced multi-agent systems with memory integration and learning capabilities.

## Directory Structure

```
examples/
├── simple_examples.py          # Basic examples for beginners
├── advanced_examples.py       # Advanced examples for power users
├── README.md                  # This file
└── README_quickstart.md       # Quick start guide
```

## Simple Examples (for beginners)

The `simple_examples.py` file contains 5 basic examples:

### Example 1: Basic Brain Operations
- Demonstrates the simplest way to create and use a brain
- Covers brain creation, initialization, and basic simulation
- **Key concepts**: Brain creation, initialization, stepping, basic statistics

### Example 2: Brain and World Interaction
- Shows how a brain interacts with an environment
- Demonstrates the full agent-world cycle: sense-think-act
- **Key concepts**: Sensory input, motor output, world interaction

### Example 3: Learning and Adaptation
- Demonstrates how brains learn and adapt through experience
- Covers reward modulation, curiosity, and development
- **Key concepts**: Learning, neuromodulation, development

### Example 4: Saving and Loading Brain State
- Shows how to save and load brain state for persistence
- Demonstrates checkpoint functionality
- **Key concepts**: Serialization, persistence, state management

### Example 5: Advanced Features and Configuration
- Demonstrates advanced configuration options and features
- Shows comprehensive brain configuration
- **Key concepts**: Configuration, customization, advanced settings

## Advanced Examples (for power users)

The `advanced_examples.py` file contains sophisticated examples:

### Advanced Multi-Agent System
- Multiple brains with different specializations (explorer, worker, social)
- Complex world interactions with multiple object types
- Memory system integration and coordination
- Pattern recognition and concept formation

### Pattern Learning Experiment
- Deep dive into pattern learning and concept formation
- Training on multiple patterns with recall testing
- Monitoring learning progress and performance metrics

## Running Examples

### Prerequisites

1. Install the NLM Python bindings:
   ```bash
   pip install -e .
   ```

2. Build the C++ library (if not already built):
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

### Running Examples

Run the simple examples:
```bash
cd /path/to/nlm/examples
python3 simple_examples.py
```

Run the advanced examples:
```bash
cd /path/to/nlm/examples
python3 advanced_examples.py
```

### Example Scripts

You can also run individual examples:
```bash
# Run just example 1
python3 -c "from simple_examples import example1_basic_brain; example1_basic_brain()"

# Run just example 3
python3 -c "from simple_examples import example3_learning_features; example3_learning_features()"
```

## Quick Start Guide

### Installation (Quick)

```bash
# Clone the repository
https://github.com/nlm-project/nlm.git
cd nlm

# Install Python bindings
pip install -e .

# Run examples
python3 examples/simple_examples.py
```

### First Brain (5 lines of code)

```python
import pynlm

# Create a default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Complete Agent Example

```python
import pynlm

def run_simulation(num_steps=1000):
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(0.0, 0.0)  # Simplified reward
    
    return brain, agent, world

# Run simulation
brain, agent, world = run_simulation(500)
print("Simulation complete!")
```

## Key Concepts

### Brain Lifecycle

1. **Create**: `brain = pynlm.createBrain(config)`
2. **Initialize**: `brain.initialize()`
3. **Simulate**: `brain.step(step_number)`
4. **Interact**: `brain.receiveSensoryInput(percept)`
5. **Control**: `brain.produceAction()`

### Agent Brain Interface

The `AgentBrain` class provides a convenient interface for brain-world interaction:

- `agent.processSensoryInput(percept)` - Inject sensory data into brain
- `agent.decodeMotorCommand()` - Get action from brain activity
- `agent.applyRewardModulation(reward, predicted_reward)` - Learn from rewards
- `agent.updateDevelopment(timestep)` - Update developmental state

### Configuration

Use the `Config` class to customize brain behavior:

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)
```

### Memory and Learning

```python
# Check brain statistics
stats = brain.getBrainStatistics()
print(f"Working memory size: {stats.memory.workingMemorySize}")
print(f"Learning rate: {stats.learning.learningRate}")
print(f"Exploration rate: {stats.learning.explorationRate}")
```

## Advanced Features

### Python Integration

```python
# Export brain state to Python dictionary
state_dict = brain.toPythonDict()

# Import brain state from Python dictionary
brain.fromPythonDict(state_dict)

# Execute Python code in brain context
brain.simulatePythonCode("print('Hello from brain!')")
```

### Expert Modes

```python
# Set expert mode for specific tasks
brain.setExpertMode("fast")      # For speed
brain.setExpertMode("accurate")   # For accuracy
brain.setExpertMode("balanced")   # For general purpose
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
- Some neurons need input to fire - make sure sensory input is connected

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Debugging

Enable verbose logging:
```python
import pynlm
import logging

# Enable logging
logging.basicConfig(level=logging.DEBUG)

# Create brain with logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

### Performance

For large simulations:
- Use batch operations when possible
- Monitor performance metrics with `brain.getPerformanceMetrics()`
- Use expert modes for specific optimization goals

## Additional Resources

### Documentation

- [NLM Documentation](https://nlm.readthedocs.io) - Comprehensive API documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

### Research and Development

- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report
- [docs/PERFORMANCE.md](docs/PERFORMANCE.md) - Performance analysis
- [docs/LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations

## Contributing

### Adding New Examples

1. Create new example files in the `examples/` directory
2. Follow the existing code style and conventions
3. Include comprehensive comments and documentation
4. Add examples to the main README files

### Improving Examples

1. Report issues with existing examples
2. Suggest new examples based on use cases
3. Provide feedback on example clarity and completeness

### Bug Reports and Feature Requests

1. Create issues on GitHub
2. Provide clear reproduction steps
3. Include example code when possible

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contact

For questions, issues, or collaboration:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io

---

**Enjoy exploring the NLM framework! The examples are designed to help you get started and then guide you toward more advanced usage.**
