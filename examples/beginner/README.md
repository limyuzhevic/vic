# Beginner Examples for NLM Python Bindings

This directory contains beginner-friendly examples to help you get started with
NLM Python bindings. Each example demonstrates a specific concept or capability.

## Quick Start

1. **Install NLM Python bindings** (if not already installed)
2. **Navigate to this directory**: `cd examples/beginner`
3. **Run an example**: `python example_name.py`
4. **Try the interactive examples**: `python main.py`

## Available Examples

### 1. example_1_basic_brain.py
**What it shows**: Basic brain creation and initialization
- Create a brain with default configuration
- Initialize and run simple simulation
- Get basic statistics about the brain

### 2. example_2_sensorimotor_loop.py
**What it shows**: Complete sensorimotor loop
- World perception → brain processing → action selection → world interaction
- Sensory input processing and motor command decoding
- Complete cycle of interaction with environment

### 3. example_3_memory_and_development.py
**What it shows**: Memory systems and developmental stages
- Working memory and episodic memory functionality
- Developmental stages over time
- Curiosity and neuromodulation

### 4. example_4_checkpointing.py
**What it shows**: Checkpoint save/load functionality
- Save brain state to file
- Load brain state from file
- Comparison of brain states

### 5. example_5_advanced_configuration.py
**What it shows**: Advanced configuration options
- Custom simulation configuration
- Configuration validation
- Using specific parameter settings

### 6. example_6_neuromodulation.py
**What it shows**: Neuromodulation and learning
- Reward modulation
- Curiosity-driven exploration
- Learning from experience

### 7. example_7_multiple_agents.py
**What it shows**: Multiple agents in the same world
- Multi-agent simulation
- Concurrent agent processing
- Shared environment interaction

## Running Examples

### Individual Examples
Run any example directly:

```bash
python example_1_basic_brain.py
python example_2_sensorimotor_loop.py
python example_3_memory_and_development.py
# ... and so on
```

### Run All Examples
Run all examples in sequence:

```bash
python main.py
```

### Interactive Mode
For a more interactive experience:

```bash
# The examples are designed to be run individually
# Each example prints detailed output about what it's doing
```

## Learning Path

### For Complete Beginners
1. **Start with `example_1_basic_brain.py`** - The simplest example
2. **Try `example_2_sensorimotor_loop.py`** - Understand the sensorimotor loop
3. **Move to `example_3_memory_and_development.py`** - Learn about memory

### For Intermediate Users
1. **Explore `example_4_checkpointing.py`** - Save/load functionality
2. **Study `example_5_advanced_configuration.py`** - Configuration options
3. **Understand `example_6_neuromodulation.py`** - Learning mechanisms

### For Advanced Users
1. **Try `example_7_multiple_agents.py`** - Complex multi-agent scenarios
2. **Create your own examples** using the patterns from these examples
3. **Combine techniques** from different examples

## Example Code Patterns

Each example follows this pattern:

1. **Setup**: Create brain, world, and agent
2. **Configuration**: Set up parameters and enable features
3. **Simulation**: Run the simulation loop
4. **Analysis**: Print results and statistics
5. **Cleanup**: (Optional) Reset or cleanup

### Common Code Patterns

#### 1. Basic Brain Setup
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

#### 2. Complete Sensorimotor Loop
```python
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(num_steps):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### 3. Configuration
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("enable_development", True)
config.set("enable_curiosity", True)
```

#### 4. Statistics and Monitoring
```python
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing: {brain.getFiringNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Development: {brain.getDevelopmentalStage().name}")
```

## Troubleshooting

### Common Issues and Solutions

1. **"Brain is not initialized"**
   - Solution: Always call `brain.initialize()` before using the brain
   - Ensure you have a valid configuration

2. **"World not configured"**
   - Solution: Always call `world.configure()` before using the world
   - Check that world dimensions are valid

3. **"Agent not initialized"**
   - Solution: Always call `agent.initialize()` before using the agent
   - Ensure world is initialized first

4. **"Import error"**
   - Solution: Ensure NLM Python bindings are installed
   - Check that you're running from the correct directory

### Debugging Tips

1. **Print debug information** in your examples
2. **Use smaller simulation sizes** for testing
3. **Check error messages** for specific details
4. **Start with the simplest example** and gradually add complexity

## Extending Examples

You can create your own examples by:

1. **Copying an existing example**
2. **Modifying parameters** to test different scenarios
3. **Adding new functionality** using the NLM API
4. **Creating custom configurations** for specific research questions

### Example Template

Here's a template for creating your own examples:

```python
import pynlm
import numpy as np

# Your example implementation here

def main():
    print("=== Your Example ===")
    
    # Setup
    # Configure
    # Simulate
    # Analyze
    # Print results
    
    print("Example completed successfully!")

if __name__ == "__main__":
    main()
```

## Conclusion

These examples provide a comprehensive introduction to NLM Python bindings.
They cover the most common use cases and demonstrate the key capabilities
of the NLM framework.

Start with the basic examples and gradually work your way up to more complex
scenarios. Don't hesitate to modify the examples to explore new possibilities
and create your own experiments!

For more advanced documentation and API reference, see the main README
files in the NLM repository.
