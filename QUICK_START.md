# NLM Python Bindings - Quick Start Guide

## Introduction

Welcome to the improved NLM (Neural Learning Machine) Python bindings! This guide will help you get started with the Pythonic interface for the NLM neural simulation framework.

## What Has Been Improved

### 1. Pythonic API Design
- **Context managers** for automatic resource management
- **Property access** for intuitive object interaction
- **Type hints** and comprehensive documentation
- **Convenience methods** for common operations

### 2. Enhanced Functionality
- **Development stage support** for brain lifecycle management
- **Advanced agent creation** with all systems enabled
- **Performance optimization** helpers
- **Memory management** utilities
- **Checkpointing and persistence** support

### 3. Comprehensive Documentation
- **Quick start examples** (9 examples covering all features)
- **Advanced examples** demonstrating sophisticated use cases
- **Python API reference guide** with complete documentation
- **Usage patterns** for different user levels

### 4. Robust Testing
- **Unit tests** for all major components
- **Integration tests** for complex scenarios
- **Performance benchmarks** for validation
- **Example validation** to ensure documentation accuracy

## Installation

### Build the Module

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Install the Module

```bash
# Install the module
pip install python/dist/pynlm*.whl

# Or install in development mode
pip install -e python/
```

## Basic Usage

### 1. Creating Components

```python
import pynlm as nlm

# Create default configuration
config = nlm.create_config()

# Create brain with configuration
brain = nlm.create_brain(config)

# Create world
world = nlm.create_world()
world.configure(100, 100, 20, 20)

# Create agent
agent = nlm.create_agent_brain(brain)
```

### 2. Basic Simulation

```python
# Initialize components
brain.initialize()
agent.initialize(world)

# Run simulation steps
for step in range(100):
    # Get sensory input
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    
    # Generate action
    action = agent.decode_motor_command()
    
    # Update world
    world.update(step)
```

## Quick Start Examples

### Example 1: Basic Usage

This example demonstrates the basic NLM workflow:

```python
import pynlm as nlm

# Create components
config = nlm.create_config()
brain = nlm.create_brain(config)
world = nlm.create_world()
world.configure(50, 50, 10, 10)

# Initialize
brain.initialize()

# Run one step
percept = world.get_sensory_percept()
brain.receive_sensory_input(percept)
action = brain.produce_action()
world.update(1)

print(f"Action: {action.get_name()}")
```

### Example 2: Advanced Agent

This example creates an advanced agent with all systems enabled:

```python
import pynlm as nlm

# Create training environment
world = nlm.create_training_environment(50, 50, 10, 10)

# Create advanced agent with all systems
agent = nlm.create_advanced_agent(
    config=nlm.create_config(),
    world=world
)

# Enable all systems
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run simulation
for step in range(20):
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    action = agent.decode_motor_command()
    world.update(step)
```

### Example 3: Experimental Setup

This example creates a complete experimental setup:

```python
import pynlm as nlm

# Create complete experimental setup
config, world, agent = nlm.create_experiment_setup(80, 80, 15, 15)

# Initialize and run
agent.get_brain().initialize()
agent.initialize(world)

# Run simulation
for step in range(30):
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    action = agent.decode_motor_command()
    world.update(step)
```

### Example 4: Development Stages

This example demonstrates developmental stage support:

```python
import pynlm as nlm

# Create brain for each developmental stage
stages = [
    nlm.DevelopmentalStage.Initial,
    nlm.DevelopmentalStage.Maturation,
    nlm.DevelopmentalStage.Adult,
]

for stage in stages:
    brain = nlm.create_development_stage_brain(stage)
    brain.initialize()
    print(f"Created {stage} brain with {brain.get_total_neuron_count()} neurons")
```

### Example 5: Context Manager

This example demonstrates context manager usage:

```python
import pynlm as nlm

# Use context manager for automatic resource management
with nlm.BrainContext(nlm.create_config()) as brain:
    brain.initialize()
    print(f"Brain initialized with {brain.get_total_neuron_count()} neurons")
    
    # Perform operations
    print(f"Total neurons: {brain.get_total_neuron_count()}")
    
    # Brain will be automatically reset on exit
```

## Advanced Features

### 1. Performance Optimization

```python
import pynlm as nlm

# Create and optimize brain
brain = nlm.create_brain()
optimized_brain = nlm.optimize_brain_for_performance(brain)

# Enable memory optimizations
optimized_brain = nlm.enable_memory_optimization(optimized_brain)
```

### 2. Debugging and Monitoring

```python
import pynlm as nlm

# Create and initialize brain
brain = nlm.create_brain()
brain.initialize()

# Debug brain connections
nlm.debug_brain_connections(brain)

# Analyze efficiency
efficiency = nlm.analyze_brain_efficiency(brain)
print(f"Efficiency: {efficiency * 100:.1f}%")

# Get status
status = nlm.monitor_brain_health(brain)
print(f"Status: {status}")
```

### 3. Batch Processing

```python
import pynlm as nlm

# Create multiple scenarios
scenarios = [
    {"name": "Small World", "width": 50, "height": 50, "vision": 10},
    {"name": "Medium World", "width": 100, "height": 100, "vision": 20},
    {"name": "Large World", "width": 200, "height": 200, "vision": 40},
]

# Process each scenario
for scenario in scenarios:
    world = nlm.create_world()
    world.configure(scenario["width"], scenario["height"], 
                   scenario["vision"], scenario["vision"])
    
    agent = nlm.create_advanced_agent(
        config=nlm.create_config(),
        world=world
    )
    
    # Run simulation
    for step in range(20):
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        action = agent.decode_motor_command()
        world.update(step)
```

## Testing

### Run Python Tests

```bash
# Run Python bindings tests
python -m pytest python/test_pynlm.py -v
```

### Run Examples

```bash
# Run quick start examples
python examples/quick_start.py

# Run advanced examples
python examples/advanced_examples.py
```

## API Reference

### Key Classes

#### Config
- `create_config()` - Create default configuration
- `load_from_file(filepath)` - Load configuration from JSON
- `save_to_file(filepath)` - Save configuration to JSON

#### Brain
- `create_brain(config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(currentStep, currentTime=None)` - Perform simulation step
- `receive_sensory_input(input)` - Inject sensory input
- `produce_action()` - Generate motor action

#### AgentBrain
- `create_agent_brain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `process_sensory_input(percept)` - Process sensory percept
- `decode_motor_command()` - Decode motor command

#### SimpleWorld
- `create_world()` - Create new world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `get_sensory_percept()` - Get current sensory input
- `get_agent_body()` - Get agent physical state
- `update(timestep)` - Update world state

### Key Functions

#### Factory Functions
- `create_advanced_agent(config, world)` - Create advanced agent
- `create_training_environment(width, height, vision_width, vision_height)` - Create training environment
- `create_experiment_setup(width, height, vision_width, vision_height)` - Create experimental setup

#### Utilities
- `create_development_stage_brain(stage)` - Create brain with specific stage
- `quick_prototype()` - Quick prototyping setup
- `optimize_brain_for_performance(brain)` - Optimize brain for performance
- `enable_memory_optimization(brain)` - Enable memory optimizations

## Troubleshooting

### Common Issues

1. **Module not found**
   ```bash
   pip install pynlm
   ```

2. **Import errors**
   - Ensure all dependencies are installed
   - Check that the module was built successfully

3. **Performance issues**
   - Profile your simulation to identify bottlenecks
   - Consider using optimized brain configurations

4. **Memory issues**
   - Use context managers to ensure proper cleanup
   - Enable memory optimizations for large simulations

### Getting Help

- **Documentation**: Refer to the PYTHON_BINDINGS_GUIDE.md for detailed API reference
- **Examples**: Run the examples in the examples/ directory
- **Debugging**: Use `nlm.debug_brain_connections()` for debugging
- **Monitoring**: Use `nlm.monitor_brain_health()` for monitoring

## Next Steps

### For Beginners
1. **Install the module** using the provided build instructions
2. **Run the quick start examples** to get familiar with the basics
3. **Explore the advanced examples** for more sophisticated use cases
4. **Consult the documentation** for detailed API reference

### For Advanced Users
1. **Extend the bindings** with new functionality as needed
2. **Add new examples** for common use cases
3. **Contribute to the test suite** with additional test cases
4. **Provide feedback** on API improvements and bug fixes

## Conclusion

The improved NLM Python bindings provide:

1. **A Pythonic API** that follows Python best practices
2. **Production-ready functionality** with robust error handling
3. **Comprehensive documentation** with extensive examples
4. **Thorough testing** with full test coverage
5. **Performance optimization** for efficient resource usage
6. **Advanced features** for sophisticated use cases

The bindings are now ready for production use and provide a solid foundation for building complex neural simulation applications.

---

*Version: 0.1.0*
*Date: 2026-09-23*
*Status: Production Ready*

For more information, visit the NLM documentation or contact the NLM authors.