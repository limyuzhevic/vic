# NLM Examples Directory

This directory contains example scripts demonstrating various ways to use the NLM (Neural Learning Machine) framework.

## Overview

The NLM Examples directory provides practical demonstrations of the NLM library's capabilities, ranging from simple introductory examples to advanced multi-agent simulations. Each example showcases different aspects of the neural simulation framework.

## Available Examples

### Basic Examples (`basic_demos.py`)

The `basic_demos.py` module contains four demonstration scripts:

1. **Basic Brain Demo** (`basic_brain_demo()`)
   - Shows the simplest way to create and use a neural brain
   - Covers brain creation, initialization, and basic simulation
   - Ideal for absolute beginners

2. **Agent-World Demo** (`agent_world_demo()`)
   - Demonstrates complete agent-world interaction
   - Shows how an agent perceives and acts in an environment
   - Includes learning capabilities (reward modulation, structural plasticity)

3. **Complete Simulation Demo** (`complete_simulation_demo()`
   - Comprehensive demonstration of all major features
   - Shows brain, agent, and world working together
   - Includes world objects, extended simulation, and detailed reporting

4. **Quick Start Demo** (`quick_start_demo()`)
   - Minimal code example for fastest setup
   - Shows the absolute minimum required to get started
   - Perfect for quick tutorials or testing

### Advanced Examples (`advanced_demos.py`)

The `advanced_demos.py` module provides sophisticated demonstrations:

1. **AdvancedBrainTrainer Class**
   - Advanced brain training with comprehensive statistics tracking
   - Multiple episode management and performance metrics
   - Support for reproducible experiments with seeds

2. **AdvancedSimulationRunner Class**
   - Multi-agent simulation capabilities
   - Competitive scenario setup and management
   - Agent ranking and performance comparison

3. **Helper Functions**
   - Convenience functions for common agent configurations
   - `create_reactive_agent()` - Fast-reacting agents
   - `create_creative_agent()` - Exploration-focused agents
   - `create_cooperative_agent()` - Social learning agents

4. **Demo Sequence**
   - Comprehensive demo showcasing different agent types
   - Comparative analysis of agent behaviors
   - Performance metrics across different approaches

## How to Use These Examples

### Running Examples

To run the examples, navigate to the examples directory and execute:

```bash
cd /path/to/nlm/examples
python3 basic_demos.py
python3 advanced_demos.py
```

Individual demos can be run by uncommenting the function calls in the `if __name__ == "__main__":` section of each file.

### Importing Examples

For use in your own projects, import specific functions:

```python
from examples.basic_demos import basic_brain_demo, agent_world_demo
from examples.advanced_demos import create_reactive_agent, AdvancedBrainTrainer

# Use in your own code
brain = basic_brain_demo()
agent = create_reactive_agent()
```

### Customizing Examples

All examples are designed to be easily customizable:

1. **Modify Parameters**: Change simulation steps, world dimensions, agent configurations
2. **Add Logging**: Uncomment logging statements for detailed output
3. **Extend Functionality**: Add new agent behaviors or world interactions
4. **Integrate with Your Code**: Import and adapt examples for specific use cases

## Example Use Cases

### Research and Development

- Use `advanced_demos.py` for testing new neural architectures
- Implement custom reward functions and learning rules
- Compare different agent strategies using the simulation runner

### Education and Training

- Use `basic_demos.py` for teaching neural network concepts
- Demonstrate key NLM features with simple, focused examples
- Create tutorials for different skill levels

### Production Applications

- Extract and adapt example code for specific applications
- Use agent training examples as starting points for custom implementations
- Leverage advanced features for complex multi-agent systems

## Technical Details

### Brain Component

All examples demonstrate:
- Brain creation and initialization
- Neural network simulation (spiking neurons, synaptic connections)
- Plasticity mechanisms (STDP, Hebbian learning)
- Developmental stages and maturation

### Agent Component

Examples showcase:
- Sensory processing and perception
- Motor command decoding and action execution
- Learning through reward modulation
- Neuromodulation (curiosity, novelty detection)

### World Component

World interaction includes:
- 2D environment simulation
- Object creation and interaction
- Sensory input (vision, touch, internal signals)
- Action effects and consequences

## File Structure

```
/examples/
├── basic_demos.py          # Basic demonstration scripts
├── advanced_demos.py       # Advanced demonstration scripts
└── README.md               # This file
```

## Requirements

To run these examples:

1. **Install NLM**: Ensure the NLM Python bindings are installed
   ```bash
   pip install pynlm
   ```

2. **Build C++ Core**: The examples depend on the compiled NLM C++ core
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Python Dependencies**: Basic examples require only standard Python
   Advanced examples use standard libraries (math, time, typing)

## Tips for Getting Started

1. **Start with Basic Examples**: Begin with `basic_demos.py` to understand the fundamentals
2. **Explore Step by Step**: Each example builds on the previous one's concepts
3. **Modify and Experiment**: Change parameters and add new features to learn
4. **Study the Code**: Examine the implementation details to understand NLM architecture
5. **Create Custom Examples**: Develop your own examples based on the patterns shown

## Contribution

These examples are designed to be community resources. Contributions are welcome:

- Add new example scenarios
- Include different use cases and applications
- Provide educational tutorials and walkthroughs
- Share best practices and advanced techniques

## License

These examples are distributed under the same license as the NLM framework (MIT).

---

For more information about NLM or to report issues, visit the NLM documentation or repository.
