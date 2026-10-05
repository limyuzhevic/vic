# NLM - Enhanced Usage Guide for Modern Python Developers

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this - but now with modern Python features!

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Modern Brain Creation with Context Manager (NEW!)

```python
import nlm

# Create and use a brain with automatic resource cleanup
with nlm.Brain(preset='beginner') as brain:
    brain.initialize()
    
    for i in range(100):
        brain.step(i)  # One step of thinking
    
    print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Simulation with Enhanced Features (NEW!)

```python
import nlm

# Setup simulation with enhanced features
with nlm.SimulationContext(preset='intermediate') as sim:
    brain, world, agent = sim.setup(world_config={
        'width': 15,
        'height': 15,
        'vision_width': 8,
        'vision_height': 8
    })
    
    # Enable learning features automatically
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    # Run simulation with monitoring
    sim.run_simulation(50, dt=0.1)
    
    print(f"Simulation complete!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Configuration Management (NEW!)

```python
import nlm

# Use preset configuration
config = nlm.create_config(preset='intermediate')

# Create brain with validation
brain = nlm.create_brain(config)
brain.initialize()

# Customize configuration
config = nlm.create_config(
    preset='beginner',
    custom_settings={
        'neuron_count': 500,
        'learning_rate': 0.05,
        'plasticity': True
    }
)
```

---

## Modern Python API Features

### 1. Context Managers (NEW!)
```python
# Automatic resource cleanup with context managers
with nlm.Brain(preset='advanced') as brain:
    brain.initialize()
    # ... use brain ...
    # Brain automatically cleaned up here
```

### 2. Property Access (NEW!)
```python
# Read-only property access
brain = nlm.create_brain(preset='intermediate')
print(f"Total neurons: {brain.stats['total_neurons']}")
print(f"Average firing rate: {brain.stats['firing_rate']:.2f} Hz")
print(f"Is initialized: {brain.is_initialized}")
```

### 3. Enhanced Error Handling (NEW!)
```python
try:
    config = nlm.create_config({'neuron_count': -100})
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Helpful error messages with suggestions

with nlm.Brain(preset='beginner') as brain:
    brain.initialize()
    # Your simulation code here
    # Errors are handled gracefully with recovery suggestions
```

### 4. Convenience Functions (NEW!)
```python
# Create and initialize in one call
brain = nlm.createBrainAndInitialize()

# Run complete simulation
results = nlm.run_simple_simulation(steps=100, preset='intermediate')

# Train agent with monitoring
nlm.train_agent(brain, world, agent, episodes=100, max_steps=50)
```

### 5. Performance Monitoring (NEW!)
```python
brain = nlm.create_brain(preset='intermediate')

# Run simulation
with nlm.SimulationContext(preset='intermediate') as sim:
    brain, world, agent = sim.setup()
    sim.run_simulation(100, dt=0.1)
    
    # Get performance metrics
    if hasattr(brain, 'performance_metrics'):
        metrics = brain.performance_metrics
        summary = metrics.get_metrics_summary()
        
        print(f"Performance Summary:")
        print(f"  Average firing rate: {summary['average_firing_rate']:.2f} Hz")
        print(f"  Stability trend: {summary['stability_trend']:.3f}")
```

### 6. Advanced Simulation Control (NEW!)
```python
brain = nlm.create_brain(preset='intermediate')
world = nlm.create_world({'width': 10, 'height': 10})
agent = nlm.create_agent(brain, world)

# Create simulation runner with advanced control
runner = nlm.SimulationRunner(brain, world, agent)

# Run with step-by-step monitoring
def on_step(result):
    print(f"Step {result['step']}: {result['spike_count']} spikes")

results = runner.run_simulation(50, dt=0.1, on_step=on_step)

# Pause/resume simulation
runner.pause()
# ... do something ...
runner.resume()
```

---

## Enhanced Configuration API

### Configuration Presets

```python
# Create configuration with preset
config = nlm.create_config(preset='intermediate')

# Available presets:
# - 'beginner': 100 neurons, 2 regions, learning_rate=0.01
# - 'intermediate': 1000 neurons, 4 regions, learning_rate=0.001
# - 'advanced': 5000 neurons, 8 regions, learning_rate=0.0001
```

### Configuration Validation

```python
# Validate configuration before use
from nlm import _ConfigValidator
validator = _ConfigValidator()

config = {'neuron_count': 100, 'learning_rate': 0.01}
errors = validator.validate_brain_config(config)
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
```

### Advanced Configuration Features

```python
# Create world configuration
world_config = nlm.create_config(preset='intermediate')
world_config.setInt('width', 20)
world_config.setInt('height', 20)
world_config.setInt('vision_width', 8)
world_config.setInt('vision_height', 8)

# Validate world configuration
world_errors = validator.validate_world_config(world_config.get_config_dict())
```

---

## Common Patterns

### Pattern 1: Simple Brain with Context Manager

```python
def run_brain_simulation(steps=100, preset='beginner'):
    """Run a simple brain simulation with automatic cleanup."""
    with nlm.Brain(preset=preset) as brain:
        brain.initialize()
        
        for i in range(steps):
            brain.step(i)
            
        return brain.get_stats()

# Use it
stats = run_brain_simulation(50, 'intermediate')
print(f"Total spikes: {stats['total_spikes']}")
```

### Pattern 2: Complete Agent Simulation

```python
def run_complete_simulation(brain_config=None, world_config=None, 
                           preset='intermediate', enable_features=True):
    """Run complete simulation with agent and monitoring."""
    with nlm.SimulationContext(preset=preset) as sim:
        brain, world, agent = sim.setup(
            config=brain_config,
            world_config=world_config
        )
        
        if enable_features:
            agent.enable_reward_modulation(True)
            agent.enable_curiosity(True)
            agent.enable_development(True)
        
        # Run simulation with monitoring
        sim.run_simulation(100, dt=0.1)
        
        # Get comprehensive results
        stats = brain.get_stats()
        return {
            'brain_stats': stats,
            'world_config': world_config,
            'agent_features': agent.enabled_features if hasattr(agent, 'enabled_features') else {}
        }
```

### Pattern 3: Training with Monitoring

```python
def train_agent(agent, world, brain, episodes=100, max_steps=50):
    """Train agent with reward modulation and monitoring."""
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    training_results = []
    
    for episode in range(episodes):
        world.reset()
        episode_results = {'episode': episode, 'steps': []}
        
        for step in range(max_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simulate reward based on action
            reward = 1.0 if action.type == nlm.ActionType.MoveForward else 0.0
            predicted = 0.5
            agent.apply_reward_modulation(reward, predicted)
            
            episode_results['steps'].append({
                'step': step,
                'spike_count': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'action': action.type.name
            })
        
        training_results.append(episode_results)
    
    return training_results
```

### Pattern 4: Performance Monitoring

```python
def monitor_performance(brain, world, agent, steps=50):
    """Monitor simulation performance with metrics."""
    # Create simulation runner with metrics
    runner = nlm.SimulationRunner(brain, world, agent)
    
    # Define callback for real-time monitoring
    def monitor_callback(result):
        print(f"Step {result['step']}: "
              f"{result['spike_count']} spikes, "
              f"{result['firing_rate']:.2f} Hz")
    
    # Run with monitoring
    results = runner.run_simulation(steps, dt=0.1, on_step=monitor_callback)
    
    # Get performance metrics
    if hasattr(brain, 'performance_metrics'):
        metrics = brain.performance_metrics
        summary = metrics.get_metrics_summary()
        stability = metrics.get_stability_analysis()
        
        print(f"\nPerformance Summary:")
        print(f"  Average firing rate: {summary['average_firing_rate']:.2f} Hz")
        print(f"  Stability trend: {summary['stability_trend']:.3f}")
        print(f"  Performance trend: {summary['performance_trend']:.3f}")
        
        # Check for degradation
        if metrics.detect_degradation():
            print("  WARNING: Performance degradation detected!")
    
    return results
```

---

## Quick Reference

| What you want | Modern API Code |
|--------------|----------------|
| Create brain | `nlm.create_brain(preset='beginner')` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `nlm.create_world(config)` |
| Create agent | `nlm.create_agent(brain, world)` |
| Get brain stats | `brain.get_stats()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

| Use context manager | `with nlm.Brain() as brain:` |
| Monitor performance | `brain.performance_metrics` |
| Use presets | `nlm.create_config(preset='intermediate')` |
| Run simulation | `nlm.run_simple_simulation(steps=100)` |
| Train agent | `nlm.train_agent(brain, world, agent, episodes=100)` |

---

## Troubleshooting

**"Brain isn't responding"**
- **Solution**: Did you call `brain.initialize()`?
- **Tip**: Use context managers for automatic cleanup
- **Check**: Verify configuration is valid

**"Agent won't learn"**
- **Solution**: Enable reward modulation and curiosity
- **Tip**: Use `agent.enable_reward_modulation(True)`
- **Check**: Verify `agent.enable_development(True)`

**"Everything is 0"**
- **Solution**: Brains need time to "warm up"
- **Tip**: Try more steps (100-1000 typical)
- **Check**: Ensure `world.update()` is called

**"Configuration errors"**
- **Solution**: Use preset configurations
- **Tip**: Validate configuration before use
- **Check**: Use `nlm.create_config(preset='beginner')`

**"Performance issues"**
- **Solution**: Monitor performance with metrics
- **Tip**: Use `brain.performance_metrics` for monitoring
- **Check**: Reduce simulation complexity if needed

**"Resource cleanup issues"**
- **Solution**: Use context managers
- **Tip**: Always use `with nlm.Brain() as brain:`
- **Check**: Resources are automatically cleaned up

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md`** for detailed API documentation
2. **Run `examples/comprehensive_examples.py`** for complete examples
3. **Explore `nlm_utils.py`** for advanced utility functions
4. **Experiment with different configurations** and presets
5. **Try advanced features** like neuromodulation and development
6. **Implement custom validation** and error handling
7. **Create your own simulation patterns** based on these examples

---

## Migration Guide

### From Old API to Enhanced API

If you're migrating from the old API:

**Old:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(10)
```

**New:**
```python
import nlm

with nlm.Brain(preset='beginner') as brain:
    brain.initialize()
    for i in range(100):
        brain.step(i)
```

**Key Changes:**

1. **Module**: `pynlm` → `nlm`
2. **Context managers**: Optional but recommended
3. **Configuration**: Use presets or validation
4. **Error handling**: Enhanced with helpful messages
5. **Resource cleanup**: Automatic with context managers
6. **Documentation**: Comprehensive examples and API reference

---

## Best Practices

1. **Use context managers** for automatic resource cleanup
2. **Validate configuration** before use
3. **Use appropriate presets** for your use case
4. **Monitor performance** with metrics
5. **Implement error handling** gracefully
6. **Use simulation helpers** for common workflows
7. **Check safety limits** before running simulations
8. **Implement proper cleanup** in error cases

---

*(Document Version: 2.0.0)
Last Updated: 2026-10-05*
*Enhanced NLM Python API Usage Guide*"
