# NLM Python Examples

This directory contains example scripts demonstrating the use of NLM Python bindings.
These examples cover a range of use cases from basic to advanced.

## Quick Start

### Prerequisites

1. Install NLM Python bindings:
   ```bash
   pip install -e .
   ```

2. Ensure you have a working C++ environment to build the NLM C++ core

### Running Examples

```bash
# Run all examples
python example_script.py

# Run specific examples
python simple_brain_example.py
python agent_example.py
python learning_example.py
```

## Examples

### 1. Basic Brain Example
**File**: `simple_brain_example.py`

This example demonstrates the most basic usage of NLM - creating a brain and running a simulation.

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Agent in World Example
**File**: `agent_example.py`

This example shows how to create a complete agent with brain, world, and sensory-motor loops.

```python
import pynlm

def run_agent_simulation():
    # Create components
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize
    brain.initialize()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

run_agent_simulation()
```

### 3. Learning Experiment Example
**File**: `learning_example.py`

This example demonstrates how to create a learning environment with plasticity mechanisms.

```python
import pynlm

def run_learning_experiment():
    # Create learning-optimized brain
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    
    # Configure for learning
    pynlm.configureBrainForLearning(brain)
    
    # Run learning simulation
    for step in range(300):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step)

run_learning_experiment()
```

### 4. Brain Comparison Example
**File**: `comparison_example.py`

This example shows how to compare different brain configurations.

```python
import pynlm

def compare_brain_configurations():
    # Create different configurations
    config1 = pynlm.createLearningConfig()
    config2 = pynlm.createExplorationConfig()
    
    brain1 = pynlm.createBrain(config1)
    brain2 = pynlm.createBrain(config2)
    
    brain1.initialize()
    brain2.initialize()
    
    # Compare performance
    for step in range(100):
        brain1.step(step)
        brain2.step(step)
        
        print(f"Step {step}:")
        print(f"  Brain1: {brain1.getTotalSpikeCount()} spikes")
        print(f"  Brain2: {brain2.getTotalSpikeCount()} spikes")

compare_brain_configurations()
```

### 5. Advanced Features Example
**File**: `advanced_example.py`

This example demonstrates advanced features like state management and performance optimization.

```python
import pynlm

def demonstrate_advanced_features():
    # Create brain
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    
    # Optimize for performance
    pynlm.optimizeBrainForLearning(brain)
    
    # Save brain state
    pynlm.saveBrainWithMetadata(brain, "advanced_example.bin", "Advanced demo")
    
    # Get detailed statistics
    stats = pynlm.getBrainStats(brain)
    print(stats)

demonstrate_advanced_features()
```

## Configuration Examples

### Basic Configuration
```python
# Default configuration - good for testing
config = pynlm.createDefaultConfig()

# Learning configuration - optimized for learning
config = pynlm.createLearningConfig()

# Exploration configuration - optimized for exploration
config = pynlm.createExplorationConfig()

# Custom configuration with specific neuron count
config = pynlm.createConfigWithNeuronCount(2000)
```

### Configuration Templates
```python
# Quick presets for common use cases

templates = {
    'small': pynlm.createConfigWithNeuronCount(500),
    'medium': pynlm.createConfigWithNeuronCount(1000),
    'large': pynlm.createConfigWithNeuronCount(5000),
    'learning': pynlm.createLearningConfig(),
    'exploration': pynlm.createExplorationConfig(),
}

# Use template
brain = pynlm.createBrain(templates['learning'])
```

## Development Guide

### Creating New Examples

1. **Start simple**: Begin with basic brain creation and simulation
2. **Add complexity**: Gradually introduce world, agent, and learning
3. **Demonstrate features**: Show specific NLM features in action
4. **Include output**: Provide meaningful output to demonstrate results

### Best Practices

1. **Always initialize**: Call `initialize()` before using brain or agent
2. **Use meaningful names**: Choose descriptive variable names
3. **Add comments**: Explain complex parts of the code
4. **Include error handling**: Handle potential exceptions gracefully
5. **Add timing**: Measure and report performance metrics

### Testing Examples

To test that examples work correctly:

```bash
# Install required dependencies
pip install pytest numpy

# Run pytest on examples (if they have tests)
pytest examples/

# Run example script directly
python examples/example_script.py
```

## Troubleshooting

### Common Issues

1. **Brain not firing**:
   - Check if `initialize()` was called
   - Try increasing simulation steps
   - Check brain configuration parameters

2. **Agent not moving**:
   - Ensure world updates are happening
   - Check motor command decoding
   - Verify action application

3. **Memory usage**:
   - Reduce brain size with `createConfigWithNeuronCount()`
   - Clear references when done

4. **Performance issues**:
   - Use `optimizeBrainForSpeed()` for faster performance
   - Reduce simulation steps for testing

### Getting Help

1. **Check the documentation**: See the main NLM documentation for detailed API reference
2. **Run examples**: Examples demonstrate correct usage patterns
3. **Consult source code**: Look at the Python bindings source for implementation details
4. **Join community**: Connect with other NLM users for support

## Contributors

This examples directory was created to help users get started with NLM Python bindings.
Contributions are welcome to add new examples and improve existing ones.

### Example Template

To create a new example, copy and modify an existing example:

```bash
cp examples/agent_example.py examples/new_feature_example.py
# Edit new_feature_example.py to demonstrate your feature
```

## License

This examples directory is part of the NLM project and follows the same licensing terms.

See the main NLM repository for full license information.

---

*Last updated: $(date)*
*Version: 0.1.0*