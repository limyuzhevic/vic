# Enhanced NLM Python API Guide

This guide introduces the enhanced Python bindings for NLM (Neural Learning Machine) that provide a modern, Pythonic interface to the neural simulation framework.

## Overview

The enhanced bindings provide two interfaces:
- **Enhanced API (`pynlm`)**: New Pythonic interface with improved usability
- **Original API (`pynlm_original`)**: Backward compatibility with original bindings

## Installation

```bash
# Build from source
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Or use Python wheel (coming soon)
pip install pynlm-enhanced
```

## Quick Start

### Basic Usage

```python
import pynlm as nlm

# Create enhanced configuration with Python dictionary interface
config = nlm.Config()
config.update({
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42
})

# Create brain with context manager (automatic initialization/reset)
with nlm.Brain(config) as brain:
    # Run simulation with detailed results
    results = brain.run_simulation(
        steps=100,
        sensory_inputs=[...],
        with_details=True
    )
    
    # Get brain statistics
    stats = brain.get_stats()
    print(f"Neurons: {stats['total_neurons']}")
    print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
```

### Advanced Usage

```python
import pynlm as nlm
import numpy as np

# Create all components
brain = nlm.create_brain(nlm.create_config())
agent = nlm.create_agent_brain(brain)
world = nlm.create_simple_world()

# Configure the world
world.configure(
    width=20, height=20,
    vision_width=8, vision_height=8
)

# Enable advanced features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run advanced simulation with exploration
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process with detailed tracking
    agent.process_sensory_input(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action with neuromodulation
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Apply rewards (example: +1 for moving)
    if action.type in [nlm.ActionType.MOVE_FORWARD, nlm.ActionType.MOVE_LEFT, nlm.ActionType.MOVE_RIGHT]:
        agent.apply_reward_modulation(reward=1.0, predicted_reward=0.5)
    
    # Log progress every 50 steps
    if step % 50 == 0:
        motor_output = agent.get_detailed_motor_output()
        print(f"Step {step}: Action={motor_output['action']['name']}, "
              f"Neuromodulation={motor_output['neuromodulation_level']:.2f}")
```

## Key Features

### 1. Enhanced Configuration Interface

The new `Config` class provides a Pythonic dictionary-like interface:

```python
import pynlm as nlm

# Method 1: Traditional way
config = nlm.Config()
config.set("neuron_count", 1000)
config.set("random_seed", 42)

# Method 2: Dictionary-like interface
config2 = nlm.Config()
config2["neuron_count"] = 1000
config2["random_seed"] = 42

# Method 3: Update from dictionary
config3 = nlm.Config()
config3.update({
    "neuron_count": 1000,
    "region_count": 2,
    "random_seed": 123
})

# Method 4: Access values
default_neurons = config3.get_or("neuron_count", 500)
print(f"Neuron count: {default_neurons}")
```

### 2. Context Managers

Automatic resource management:

```python
import pynlm as nlm

# Automatic brain lifecycle management
with nlm.Brain(config) as brain:
    # Brain is automatically initialized on entry
    brain.step(0)
    # Brain is automatically reset on exit

# Automatic agent lifecycle management  
with nlm.AgentBrain(brain) as agent:
    agent.initialize(world)
    # Agent is automatically reset on exit
```

### 3. Simulation Runners

Step-by-step simulation with detailed results:

```python
# Run simulation and get detailed results
results = brain.run_simulation(
    steps=100,
    sensory_inputs=sensor_data,
    with_details=True,
    log_interval=10
)

# Access results by step
step_0_results = results['0']
print(f"Step 0: {step_0_results['firing_neurons']} neurons firing")
```

### 4. Enhanced Agent Brain Interface

```python
import pynlm as nlm

# Create agent with enhanced interface
agent = nlm.create_agent_brain(brain)

# Get comprehensive status
status = agent.get_agent_status()
print(f"Neuromodulation: {status['neuromodulation_level']:.2f}")
print(f"Curiosity: {status['curiosity_level']:.2f}")
print(f"Developmental stage: {status['developmental_stage']}")

# Decode motor command with confidence
motor_output = agent.get_detailed_motor_output()
print(f"Selected action: {motor_output['action']['name']}")
print(f"Motor confidence: {motor_output['motor_confidence']:.2f}")
```

### 5. Advanced Visualization and Monitoring

```python
# Get brain metrics for visualization
metrics = brain.get_brain_metrics()

# Create simple report
report = brain.generate_report(metrics)
print(report)
```

## Agent Brain Features

### Neuromodulation Control

```python
# Enable/disable neuromodulation systems
agent.enable_reward_modulation(True)  # Dopamine learning
agent.enable_structural_plasticity(True)  # Structural changes
agent.enable_development(True)  # Developmental changes
agent.enable_curiosity(True)  # Exploration behavior
```

### Motor Decoding

```python
# Get raw motor command
action = agent.decode_motor_command()

# Get detailed motor analysis
motor_analysis = agent.get_motor_analysis()
print(f"Motor group activities: {motor_analysis['motor_activities']}")
print(f"Recommended action: {motor_analysis['recommended_action']}")
```

## Configuration Options

### Core Parameters

```python
# Neuron and network configuration
{
    "neuron_count": 1000,           # Total neurons
    "region_count": 1,              # Number of brain regions
    "connection_probability": 0.1,  # How connected the network is
    "random_seed": 42,              # Random number seed
    "simulation_timestep": 0.001    # Time step for simulation
}

# Memory system configuration
{
    "working_memory_capacity": 100,     # Size of working memory
    "episodic_memory_max_episodes": 1000,  # Max stored episodes
    "synaptic_memory_trace_decay": 0.9    # Memory decay rate
}

# Neuromodulation parameters
{
    "dopamine_learning_rate": 0.01,
    "curiosity_strength": 0.5,
    "novelty_threshold": 0.3
}

# Development parameters
{
    "developmental_duration": 1000,    # Total development time
    "critical_period_strength": 1.0,
    "maturation_rate": 0.5
}
```

## Best Practices

### 1. Configuration Management

```python
# Create a configuration factory
def create_experiment_config(experiment_type="default"):
    configs = {
        "default": {"neuron_count": 500, "region_count": 2},
        "large": {"neuron_count": 5000, "region_count": 5},
        "sparse": {"neuron_count": 200, "connection_probability": 0.05},
        "dense": {"neuron_count": 1000, "connection_probability": 0.3},
    }
    
    config = nlm.Config()
    config.update(configs.get(experiment_type, configs["default"]))
    return config
```

### 2. Simulation Management

```python
class NLMSimulation:
    def __init__(self, config, world_params):
        self.config = config
        self.world_params = world_params
        self.brain = None
        self.agent = None
        self.world = None
        self.step_count = 0
        
    def __enter__(self):
        # Initialize all components
        self.brain = nlm.create_brain(self.config)
        self.agent = nlm.create_agent_brain(self.brain)
        self.world = nlm.create_simple_world()
        self.world.configure(**self.world_params)
        
        # Enable features
        self.agent.enable_reward_modulation(True)
        self.agent.enable_curiosity(True)
        
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Cleanup
        if self.brain:
            self.brain.reset()
        
    def run_episode(self, max_steps=1000):
        for step in range(max_steps):
            self.step()
            if self.agent.get_curiosity_level() > 0.8:
                break
                
    def step(self):
        # Run one simulation step
        self.world.update(0.1)
        
        percept = self.world.get_sensory_percept()
        self.agent.process_sensory_input(percept)
        self.brain.step(self.step_count)
        
        action = self.agent.decode_motor_command()
        self.world.apply_motor_command(action, self.world.get_simulation_time())
        
        self.step_count += 1
```

### 3. Error Handling and Recovery

```python
def safe_simulation(config, world, agent):
    try:
        # Main simulation loop
        for step in range(1000):
            world.update(0.1)
            agent.process_sensory_input(world.get_sensory_percept())
            agent.get_brain().step(step)
            
            # Check for errors
            if agent.get_brain().get_pending_spike_event_count() > 10000:
                print("Warning: Too many pending spike events")
                
            if agent.get_brain().get_total_spike_count() > 1000000:
                print("Warning: Spike count exceeded safe limit")
                
    except Exception as e:
        print(f"Simulation error: {e}")
        # Attempt recovery
        try:
            agent.get_brain().reset()
            world.reset()
            print("Recovery successful")
        except:
            print("Recovery failed")
            raise
```

## Performance Optimization

### 1. Batch Processing

```python
# Process multiple sensory inputs efficiently
def batch_process(brain, world, percepts):
    results = []
    for i, percept in enumerate(percepts):
        brain.step(i)
        results.append({
            'step': i,
            'firing_rate': brain.get_average_firing_rate(),
            'action': decode_action(percept)
        })
    return results
```

### 2. Memory Management

```python
# Clear memory periodically to prevent accumulation
def managed_simulation(brain, agent, steps, memory_cleanup_interval=100):
    for step in range(steps):
        # Run simulation
        brain.step(step)
        
        # Clean up memory periodically
        if step % memory_cleanup_interval == 0:
            if hasattr(brain, 'cleanup_memory'):
                brain.cleanup_memory()
                
        # Optional: Save checkpoint
        if step % 500 == 0:
            if hasattr(brain, 'save_checkpoint'):
                brain.save_checkpoint(f"checkpoint_{step}.nlm")
```

## Advanced Topics

### 1. Custom Configurations

```python
# Create custom configuration class
class CustomConfig(nlm.Config):
    def __init__(self, **kwargs):
        super().__init__()
        self.defaults.update({
            "neuron_count": 1000,
            "development_enabled": True,
            "reward_modulation_enabled": True,
            "curiosity_enabled": True
        })
        self.update(kwargs)
        
    def add_custom_rule(self, rule_name, rule_function):
        # Add custom brain configuration rules
        self.set(f"custom_rule_{rule_name}", rule_function)
```

### 2. Custom Agent Behaviors

```python
class CustomAgent(nlm.AgentBrain):
    def __init__(self, brain, behavior_strategy="balanced"):
        super().__init__(brain)
        self.behavior_strategy = behavior_strategy
        
    def decode_motor_command(self):
        if self.behavior_strategy == "exploratory":
            # Always explore when curiosity is high
            if self.get_curiosity_level() > 0.7:
                return self.exploration_action()
        elif self.behavior_strategy == "exploitive":
            # Always exploit when confidence is high
            if self.get_neuromodulation_level() > 0.5:
                return self.exploitation_action()
                
        # Fall back to default behavior
        return super().decode_motor_command()
```

## Migration Guide

### From Original API to Enhanced API

| Original API | Enhanced API |
|-------------|-------------|
| `pynlm.Config()` | `nlm.Config()` |
| `config.set("key", "value")` | `config["key"] = "value"` |
| `config.has("key")` | `"key" in config` |
| `config.get("key")` | `config.get("key")` |
| `brain.initialize()` | `with nlm.Brain(config) as brain:` |
| `brain.step(i)` | `brain.run_simulation(steps)` |
| `agent.decode_motor_command()` | `agent.get_detailed_motor_output()` |
| Manual reset | Automatic context manager |

### Example Migration

```python
# OLD WAY
import pynlm_original as nlm

config = nlm.Config()
config.set("neuron_count", 1000)
brain = nlm.Brain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

brain.reset()

# NEW WAY
import pynlm as nlm

config = nlm.Config()
config.update({"neuron_count": 1000})

with nlm.Brain(config) as brain:
    results = brain.run_simulation(100)
```

## FAQ

### Q: Which API should I use?

**Use the enhanced API (`pynlm`) for new projects** - it provides better usability, error handling, and features.

**Use the original API (`pynlm_original`) for existing projects** to maintain backward compatibility.

### Q: What are the performance differences?

The enhanced API has minimal overhead (typically < 5%) and provides better memory management and error recovery.

### Q: Can I mix enhanced and original components?

Yes, but it's not recommended. Use one API consistently for your application.

### Q: What Python versions are supported?

Python 3.8 and above.

## Troubleshooting

### Common Issues

1. **Memory errors**: Use context managers and periodic cleanup
2. **Performance issues**: Profile with `py-spy` or similar tools
3. **Compatibility issues**: Check version requirements and ensure all dependencies are installed

### Error Messages

- `RuntimeError`: Check for invalid configuration values
- `ImportError`: Ensure the module is properly built and installed
- `AttributeError`: Check for correct API usage

## Future Enhancements

The enhanced API is designed to be extensible. Planned features include:

1. **Distributed computing**: Multi-GPU and distributed simulation support
2. **Real-time visualization**: Live monitoring and plotting
3. **Plugin system**: Custom neural dynamics and plasticity rules
4. **Cloud integration**: AWS/GCP deployment support
5. **API documentation**: Auto-generated API documentation

## License

This enhanced API is released under the same MIT license as the original NLM project.

## Acknowledgments

This enhanced Python interface builds upon the excellent work of the NLM project team and the pybind11 community. Special thanks to the developers who created the original bindings and the community contributors who have made NLM more accessible to Python users.
