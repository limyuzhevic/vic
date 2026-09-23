# NLM Python Bindings Tutorial

Welcome to the enhanced NLM Python bindings tutorial! This guide will walk you through the key features and capabilities of the modern Python interface to the NLM neural simulation framework.

## Getting Started

### Installation

The enhanced NLM Python bindings are available through PyPI. Install with:

```bash
pip install pynlm
```

For development or if you want to use the latest features, install from the source:

```bash
pip install -e .
```

### First Import

After installation, you can start using the NLM bindings:

```python
from pynlm import AdvancedBrain, create_simulation, Config

print("NLM Python Bindings loaded successfully!")
```

## Tutorial Overview

This tutorial covers:

1. **Basic Simulation Setup** - Creating your first simulation
2. **Configuration Management** - Setting up simulation parameters
3. **Running Episodes** - Executing simulation episodes
4. **Monitoring and Diagnostics** - Tracking simulation performance
5. **Visualization Integration** - Visualizing simulation results
6. **Agent Implementation** - Building intelligent agents
7. **Advanced Features** - Context managers, callbacks, and async operations

Let's start with the basics!

## 1. Basic Simulation Setup

### Creating a Simple Simulation

The easiest way to get started is by using the `create_simulation()` convenience function:

```python
from pynlm import create_simulation

# Create complete simulation with brain, world, and agent
brain, world, agent_brain = create_simulation()

print(f"Brain: {brain}")
print(f"World: {world}")
print(f"Agent: {agent_brain}")
```

### Configuring Your Simulation

Before running a simulation, you need to configure it. You can configure the brain directly:

```python
from pynlm import AdvancedBrain, Config

# Method 1: Create configuration first
config = Config()
config.configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_monitoring=True,
    enable_visualization=False
)

# Create brain with custom configuration
brain = AdvancedBrain(config=config, verbose=True)

# Method 2: Configure brain directly
brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=500,
    simulation_speed=2.0,
    enable_monitoring=True,
    enable_visualization=True
)
```

### Understanding the Components

- **Brain**: Central neural simulation engine with intelligent configuration management
- **World**: 2D environment where agents interact with objects
- **AgentBrain**: Interface connecting brain to world, handling sensory input and motor output

## 2. Running Your First Episode

### Basic Episode Execution

Now let's run your first simulation episode:

```python
from pynlm import create_simulation

# Create simulation
brain, world, agent_brain = create_simulation()

# Configure for simulation
brain.configure_simulation(
    max_steps=100,
    enable_monitoring=True,
    enable_visualization=False
)

# Initialize agent
agent_brain.initialize(world)

# Run episode
print("Running simulation episode...")
results = brain.run_episode(max_steps=50, verbose=True)

print(f"Episode completed!")
print(f"Steps: {results['steps_completed']}")
print(f"Total Reward: {results['total_reward']}")
print(f"Success: {results['success']}")
```

### Exploring Episode Results

The episode results provide comprehensive information about the simulation:

```python
# Unpack results
steps_completed = results['steps_completed']
total_reward = results['total_reward']
success = results['success']
diagnostics = results['diagnostics']
metrics = results['metrics']

# Check performance
if success:
    print("✅ Episode completed successfully!")
else:
    print("⚠️  Episode did not complete fully")

# Show performance metrics
print(f"Average reward per step: {total_reward/steps_completed:.2f}")
print(f"Performance stats: {diagnostics['performance']}")

# Show monitoring status
monitoring_status = metrics.get('performance_stats', {})
print(f"Memory usage: {monitoring_status.get('current_memory_usage', 0):.1f}%")
print(f"CPU usage: {monitoring_status.get('current_cpu_usage', 0):.1f}%")
```

## 3. Monitoring and Diagnostics

### Setting Up Monitoring

Enable comprehensive monitoring to track simulation performance:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=200,
    enable_monitoring=True,
    monitoring_interval=0.1
)

# Run simulation with monitoring
results = brain.run_episode(max_steps=100, verbose=True)

# Get comprehensive diagnostics
diagnostics = brain.get_diagnostics()
print(f"System Status: {diagnostics['simulation_state']}")
print(f"Performance: {diagnostics['performance']}")

# Generate performance report
report = brain.monitoring.generate_report()
print(f"\nPerformance Report:\n{report}")
```

### Understanding Monitoring Data

The monitoring system collects various metrics:

- **Neurons Active**: Number of neurons currently active
- **Neurons Firing**: Number of neurons currently firing spikes
- **Total Spikes**: Cumulative spike count
- **Memory Usage**: System memory consumption
- **CPU Usage**: Processor utilization
- **Simulation Time**: Total simulation duration

## 4. Visualization Integration

### Setting Up Visualization

Add visual representation to your simulations:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)
brain.configure_simulation(
    max_steps=100,
    enable_visualization=True,
    visualization_backend='matplotlib'
)

# Setup visualization
if brain.visualization.setup_plotting('matplotlib'):
    print("✓ Visualization backend setup successful")
    
    # Create brain network visualization
    network_plot = brain.visualization.create_brain_network_plot(brain)
    if network_plot:
        print("✓ Brain network plot created")
    
    # Create training monitor
    monitor = brain.visualization.create_training_monitor(brain)
    if monitor:
        print("✓ Training monitoring interface created")
else:
    print("✗ Visualization setup failed")
```

### Available Visualization Backends

The enhanced bindings support multiple visualization backends:

- **Matplotlib**: Standard Python plotting library
- **Plotly**: Interactive web-based visualizations
- **Bokeh**: Advanced web plotting library

## 5. Agent Implementation

### Creating Intelligent Agents

Build sophisticated agents that can interact with the simulation world:

```python
from pynlm import create_simulation, AgentBrain, NLMTypes

# Create simulation
brain, world, agent_brain = create_simulation()

# Configure agent capabilities
agent_brain.enable_reward_modulation(True)
agent_brain.enable_development(True)
agent_brain.enable_curiosity(True)

# Initialize agent with world
agent_brain.initialize(world)

# Configure world
world.configure(width=100.0, height=100.0, vision_width=20, vision_height=20)

# Create objects in world
from pynlm import WorldObject
resource = WorldObject(50, 50, NLMTypes.WorldObjectType.RESOURCE, value=100.0)
world.add_object(resource)

print(f"Agent initialized with {len(world.objects)} world objects")
```

### Agent-Environment Interaction

Agents interact with the world through sensory input and motor output:

```python
# Get sensory percept from world
percept = world.get_sensory_percept()
print(f"Vision data: {percept.vision}")

# Process sensory input (simplified - would need actual implementation)
# agent_brain.process_sensory_input(percept)

# Create and apply motor command
action = agent_brain.decode_motor_command()
print(f"Generated action: {action.get_name()}")

# Apply action to world
world.apply_motor_command(action, 0.0)

# Update world state
world.update(0.1)
```

## 6. Advanced Features

### Context Managers for Resource Management

Use context managers to ensure proper cleanup:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)

# Use context manager for automatic cleanup
with brain.simulation_context(duration=5.0) as sim:
    print(f"Simulation started: {sim._simulation_running}")
    
    # Run simulation within context
    results = sim.run_episode(max_steps=50, verbose=True)
    
    print(f"Episode completed: {results['success']}")

# Simulation automatically stopped after context
print(f"Simulation after context: {brain._simulation_running}")
```

### Callbacks for Event-Driven Control

Use callbacks to respond to simulation events:

```python
from pynlm import AdvancedBrain

brain = AdvancedBrain(verbose=True)

# Define callback functions
def before_episode_callback(brain):
    print("  [CALLBACK] Before episode: Resetting diagnostics")
    brain.reset_diagnostics()

def step_callback(state):
    if state.step % 10 == 0:
        print(f"  [CALLBACK] Step {state.step}: "
              f"Active={state.neurons_active}, Reward={state.reward}")

def after_episode_callback(brain, state):
    print(f"  [CALLBACK] After episode: "
          f"Total steps={state.step + 1}, Success={state.success}")

# Add callbacks
brain.add_callback('before_episode', before_episode_callback)
brain.add_callback('step', step_callback)
brain.add_callback('after_episode', after_episode_callback)

# Run simulation with callbacks
results = brain.run_episode(max_steps=30, verbose=True)
```

### Asynchronous Operations

Use async operations for non-blocking execution:

```python
import asyncio
from pynlm import AdvancedBrain

async def async_workflow_example():
    brain = AdvancedBrain(verbose=True)
    brain.configure_simulation(max_steps=100)
    
    print("Starting async simulation...")
    
    # Run multiple steps asynchronously
    results = await brain.async_step(5)
    
    print(f"Async step results: {len(results)} batches")
    return results

# Execute async workflow
results = asyncio.run(async_workflow_example())
```

## 7. Custom Configuration

### Environment-Specific Configurations

Create configurations for different use cases:

```python
from pynlm import Config

# Quick test configuration
quick_test_config = Config().configure(
    max_steps=50,
    simulation_speed=2.0,
    enable_monitoring=False,
    enable_visualization=False
)

# Research configuration
research_config = Config().configure(
    max_steps=1000,
    simulation_speed=1.0,
    enable_monitoring=True,
    enable_development=True,
    enable_curiosity=True,
    enable_structural_plasticity=True
)

# Production configuration
production_config = Config().configure(
    max_steps=2000,
    simulation_speed=1.5,
    enable_monitoring=True,
    enable_visualization=True,
    visualization_backend='matplotlib'
)
```

### Configuration Validation

Ensure your configurations are valid:

```python
from pynlm import Config

# Create configuration with custom validation rules
config = Config()

# Add validation rule
def validate_positive_int(value):
    errors = []
    if not isinstance(value, int) or value <= 0:
        errors.append("Must be a positive integer")
    return errors

config.add_validation_rule('max_steps', validate_positive_int)

# Test configuration
try:
    valid_config = Config().configure(max_steps=100)
    print("✓ Valid configuration accepted")
except Exception as e:
    print(f"✗ Invalid configuration: {e}")
```

## Complete Example: Simulation with All Features

Here's a complete example demonstrating all the key features:

```python
from pynlm import AdvancedBrain, World, AgentBrain, NLMTypes, WorldObject
import asyncio

async def comprehensive_simulation():
    print("=== Comprehensive NLM Simulation Example ===\n")
    
    # Create simulation
    brain = AdvancedBrain(verbose=True)
    world = World()
    agent_brain = AgentBrain(brain)
    
    # Configure all features
    brain.configure_simulation(
        max_steps=200,
        enable_visualization=True,
        enable_monitoring=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    # Setup visualization
    brain.visualization.setup_plotting('matplotlib')
    
    # Initialize agent
    agent_brain.initialize(world)
    
    # Configure world
    world.configure(width=100.0, height=100.0)
    
    # Add objects to world
    resource = WorldObject(30, 30, NLMTypes.WorldObjectType.RESOURCE, value=100.0)
    hazard = WorldObject(70, 70, NLMTypes.WorldObjectType.HAZARD, value=-50.0)
    world.add_object(resource)
    world.add_object(hazard)
    
    # Add callbacks
    def step_monitor(state):
        if state.step % 20 == 0:
            print(f"  Step {state.step}: "
                  f"Active={state.neurons_active}, Reward={state.reward:.2f}")
    
    brain.add_callback('step', step_monitor)
    
    # Run simulation
    print("Running comprehensive simulation...")
    results = brain.run_episode(max_steps=100, verbose=True)
    
    # Show results
    print(f"\n=== Simulation Results ===")
    print(f"Success: {results['success']}")
    print(f"Steps: {results['steps_completed']}")
    print(f"Total Reward: {results['total_reward']:.2f}")
    
    # Show diagnostics
    diagnostics = brain.get_diagnostics()
    print(f"\n=== Diagnostics ===")
    print(f"Performance: {diagnostics['performance']}")
    
    # Show monitoring report
    report = brain.monitoring.generate_report()
    print(f"\n=== Performance Report ===\n{report}")
    
    return results

# Run the comprehensive example
if __name__ == "__main__":
    results = asyncio.run(comprehensive_simulation())
```

## Next Steps

Now that you have the basics covered, you can:

1. **Explore Examples**: Check out the `examples/` directory for more complex use cases
2. **Advanced Features**: Learn about custom agents, path planning, and learning
3. **Visualization**: Master different visualization backends
4. **Production Use**: Implement error handling and monitoring for production systems
5. **Performance Tuning**: Optimize simulations for specific use cases

## Additional Resources

- **API Reference**: Complete documentation in `documentation/API_REFERENCE.md`
- **Examples Directory**: All examples in `examples/`
- **Configuration Examples**: Advanced configurations in `examples/configuration.py`
- **Custom Agents**: Agent implementation examples in `examples/custom_agents.py`
- **Advanced Techniques**: Complex simulations in `examples/advanced_simulation.py`
- **Visualization Guide**: Visualization examples in `examples/visualization.py`

## Getting Help

If you encounter issues or have questions:

1. **Check Examples**: Look at existing examples for common patterns
2. **Documentation**: Refer to the API reference for detailed method information
3. **Community**: Join the NLM community for support
4. **Issues**: Report bugs or feature requests in the issue tracker

Happy simulating! 🎯