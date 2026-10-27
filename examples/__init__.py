# NLM Python API Examples

This directory contains comprehensive examples for the NLM (Neural Learning Machine)
Python API. These examples demonstrate the full range of NLM functionality from basic
brain creation to advanced learning scenarios.

## Quick Start

To run the examples:

1. Build NLM with Python bindings:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   pip install -e .
   ```

2. Run examples:
   ```bash
   python examples/quick_start.py        # Simple example
   python examples/getting_started.py    # Tutorial
   python examples/comprehensive_examples.py  # Advanced examples
   python examples/usage_examples.py    # Practical workflows
   python examples/test_examples.py      # Test suite
   ```

## Examples Included

### 1. Quick Start (`quick_start.py`)
The simplest possible example to get you started with NLM.

**Code:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
```

### 2. Getting Started Tutorial (`getting_started.py`)
Step-by-step tutorial covering all major NLM concepts.

**Sections:**
1. Basic brain creation and operation
2. World simulation with agents
3. Learning system enablement
4. Advanced usage patterns

### 3. Comprehensive Examples (`comprehensive_examples.py`)
Full-scale examples demonstrating advanced features:

- `example_basic_brain()`: Basic neural simulation
- `example_world_interaction()`: Agent in simulated world
- `example_advanced_learning()`: All learning subsystems
- `example_neural_planning()`: Planning and decision making
- `example_checkpoints_and_persistence()`: Brain state saving/loading
- `example_development_stages()`: Brain development over time

### 4. Usage Examples (`usage_examples.py`)
Practical examples for common workflows:

- Multiple agents with shared brain
- Batch processing of brains
- Checkpointing
- Configuration management

### 5. Test Suite (`test_examples.py`)
Automated test suite to verify example functionality.

## API Reference

### Core Classes

- **Brain**: Central neural simulation engine
- **Config**: Configuration management
- **AgentBrain**: Interface between brain and environment
- **SimpleWorld**: 2D simulation environment
- **SensoryPercept**: Sensory input representation
- **Action**: Motor output representation

### Key Methods Used

```python
# Brain operations
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step_number)
brain.getTotalNeuronCount()
brain.getTotalSpikeCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()

# Agent operations
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# World operations
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)
world.reset()
world.update(timestep)
world.getSensoryPercept()
world.applyMotorCommand(command, time)
```

## Best Practices Demonstrated

### 1. Configuration Management
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
```

### 2. Error Handling
```python
if not brain.initialize():
    print("Brain initialization failed")
    return
```

### 3. Checkpointing
```python
# Save progress
brain.save("checkpoint.bin")

# Reset and reload
brain.reset()
brain.initialize()
brain.load("checkpoint.bin")
```

### 4. Performance Monitoring
```python
if step % 100 == 0:
    print(f"Step {step}: "
          f"Neurons={brain.getFiringNeuronCount()}, "
          f"Curiosity={agent.getCuriosityLevel():.3f}")
```

## Example Output

Running `examples/quick_start.py` produces:

```
Your brain has 1000 neurons!
Your brain has made 3 spikes so far!
That's it! Your brain is working!
```

Running `examples/getting_started.py` produces a structured tutorial with visual output showing progress through each section.

## Advanced Features

### Learning Systems
The examples demonstrate all major learning subsystems:

- **Reward Modulation**: Reinforcement learning from rewards
- **Curiosity**: Exploration drives to discover novel experiences
- **Development**: Structural changes over time (maturation)
- **Structural Plasticity**: Growing and pruning neural connections

### Memory Systems
Examples show how brains can:

- Use working memory for active information
- Store episodic memories from experiences
- Form associative memories between patterns

### Prediction and Planning
Advanced examples demonstrate:

- Sensory prediction and error computation
- Neural planning for action selection
- Concept formation from patterns

## Extending Examples

You can modify these examples for:

1. **Custom worlds**: Create custom world types with different physics
2. **Specialized agents**: Create agents with custom perception/action systems
3. **Experiment automation**: Run parameter sweeps
4. **Data logging**: Record simulation data for analysis
5. **Visualization**: Plot brain activity and agent behavior

## Testing

Run the test suite to verify all examples work:

```bash
python examples/test_examples.py
```

Or run the full NLM API test suite:

```bash
python test_nlm_api.py
```

## Dependencies

- Python 3.8+
- NLM with Python bindings built and installed
- NumPy (optional, for data analysis)

## License

These examples are part of the NLM project, licensed under the MIT License.

## Author

NLM Development Team

This examples directory is designed to help users quickly get started with the NLM
Python API and progressively learn more advanced features through structured examples.