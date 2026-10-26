# Enhanced NLM Python Interface

This is the enhanced Python interface for NLM (Neural Learning Machine) neural simulation framework.

## Installation

```bash
pip install ./  # Install from this directory

# Or for development
pip install -e .
```

## Quick Start

### Example 1: Simple Brain (Classic API)

```python
import nlm

# Create a simple brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print("Simulation complete!")
```

### Example 2: Learning Agent (Enhanced Interface)

```python
import nlm

# Use high-level NLM class
nlm_instance = nlm.NLM()
nlm_instance.setup(world_width=30.0, world_height=30.0, enable_learning=True)
stats = nlm_instance.run(1000, verbose=True)

print(f"Simulation completed: {stats['steps_completed']} steps")
print(f"Efficiency score: {stats['efficiency_score']:.4f}")
```

### Example 3: Configuration Builder (New Feature)

```python
import nlm

# Create custom configuration with validation
config = (nlm.ConfigBuilder()
         .set_neuron_count(1000)
         .set_region_count(2)
         .set_connection_probability(0.15)
         .enable_curiosity(True)
         .enable_structural_plasticity(True)
         .build())

brain = nlm.create_brain(config)
brain.initialize()
```

## Features

### Enhanced Python Interface

- **NLM class**: High-level simulation control
- **ConfigBuilder**: Fluent API for configuration with validation
- **PerformanceMonitor**: Performance tracking and optimization
- **Context managers**: Automatic resource management
- **Enhanced exceptions**: Better error messages and recovery

### Factory Functions

- **create_brain()**: Create NLM brain with optional configuration
- **create_educational_simulation()**: Create complete educational simulation
- **create_simulation_environment()**: Create simulation world
- **create_agent()**: Create agent interface

### Configuration Presets

- **create_research_config()**: Configuration optimized for research
- **create_educational_config()**: Configuration for educational use
- **create_simple_config()**: Minimal configuration for quick tests

### Performance Tools

- **PerformanceMonitor**: Track and optimize simulation performance
- **Context managers**: `brain_context()`, `simulation_episode()`

### Backward Compatibility

All existing code continues to work with the classic/simple API:

```python
import pynlm  # Old interface still works
```

## Quick Examples

### Learning with Performance Monitoring

```python
import nlm

# Create educational simulation
brain, world, agent = nlm.create_educational_simulation()

# Setup performance monitoring
monitor = nlm.create_performance_monitor()

# Run simulation with monitoring
for step in range(500):
    monitor.start_step()
    
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    monitor.end_step(brain, world)

# Get performance summary
perf_stats = monitor.get_summary()
print(f"Performance: {perf_stats['avg_step_time_ms']:.1f} ms/step")
print(f"Efficiency: {perf_stats['efficiency_score']:.4f}")
```

### Complete Educational Simulation

```python
import nlm

def run_complete_simulation(steps=1000):
    """Run a complete educational simulation with learning."""
    
    # Create educational simulation
    brain, world, agent = nlm.create_educational_simulation()
    
    # Enable learning subsystems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    
    # Run simulation with progress tracking
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Process sensory input
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        
        # Make and execute action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Curiosity={agent.get_curiosity_level():.2f}, "
                  f"Novelty={agent.get_novelty_level():.2f}, "
                  f"Spikes={brain.get_firing_neuron_count()}")
    
    # Get final statistics
    return {
        'total_spikes': brain.get_total_spike_count(),
        'final_curiosity': agent.get_curiosity_level(),
        'final_neuromodulation': agent.get_neuromodulation_level(),
        'efficiency_score': agent.get_curiosity_level() * agent.get_neuromodulation_level(),
        'steps_completed': steps
    }

# Run the simulation
results = run_complete_simulation(1000)
print(f"Simulation results: {results}")
```

## Configuration Builder Examples

### Custom Configuration

```python
import nlm

# Create research-grade configuration
config = (nlm.ConfigBuilder()
         .set_neuron_count(2000)
         .set_region_count(4)
         .set_connection_probability(0.2)
         .enable_stdp(True, ltp_weight=0.015, ltd_weight=0.013, tau=25.0)
         .enable_hebbian(True)
         .enable_structural_plasticity(True)
         .enable_curiosity(True)
         .enable_novelty(True)
         .enable_reward_modulation(True)
         .set_replay_interval(50)
         .set_development_interval(500)
         .build())

# Use the configuration
brain = nlm.create_brain(config)
brain.initialize()
print(f"Created brain with {brain.get_total_neuron_count()} neurons")
```

### Load from File

```python
import nlm

# Load configuration from file and customize
config = (nlm.ConfigBuilder()
         .load_from_file("my_config.cfg")
         .set_neuron_count(500)
         .set_simulation_timestep(0.001)
         .build())

brain = nlm.create_brain(config)
```

## Performance Monitoring

### Monitor Simulation Performance

```python
import nlm
from nlm import create_performance_monitor

# Create simulation
brain, world, agent = nlm.create_educational_simulation()

# Create performance monitor
monitor = create_performance_monitor()

# Run simulation with performance tracking
for step in range(1000):
    monitor.start_step()
    
    # Run simulation step
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    # End step and record metrics
    monitor.end_step(brain, world)

# Get performance summary
perf_stats = monitor.get_summary()
print(f"Performance summary:")
print(f"  Steps completed: {perf_stats['steps_completed']}")
print(f"  Average step time: {perf_stats['avg_step_time_ms']:.1f} ms")
print(f"  Step time consistency (std): {perf_stats['std_step_time_ms']:.1f} ms")
print(f"  Neurons per step: {perf_stats['avg_neurons']:.0f}")
print(f"  Spike generation rate: {perf_stats['spikes_per_second']:.1f} spikes/sec")
print(f"  Overall efficiency score: {perf_stats['efficiency_score']:.4f}")

if perf_stats['efficiency_score'] > 0.1:
    print("✅ Simulation shows good efficiency and stability")
elif perf_stats['efficiency_score'] > 0.05:
    print("⚠️  Simulation is functional but could be optimized")
else:
    print("❌ Simulation may need configuration adjustments")
```

## Context Managers

### Automatic Resource Management

```python
import nlm
from nlm import brain_context, simulation_episode

# Using brain_context for automatic cleanup
with brain_context() as brain:
    for step in range(1000):
        brain.step(step)
    print(f"Simulation completed: {brain.get_total_spike_count()} spikes")

# Using simulation_episode for complete episode management
brain, world, agent = nlm.create_educational_simulation()

with simulation_episode(brain, world, agent, 500) as stats:
    # Simulation runs with automatic cleanup
    # stats is available for monitoring
    pass
```

## Configuration Presets

```python
import nlm

# Research configuration (large, detailed)
research_config = nlm.create_research_config()

# Educational configuration (medium complexity)
educational_config = nlm.create_educational_config()

# Simple configuration (quick tests)
simple_config = nlm.create_simple_config()

# Compare configurations
for config_name, config in [
    ("research", research_config),
    ("educational", educational_config), 
    ("simple", simple_config)
]:
    brain = nlm.create_brain(config)
    brain.initialize()
    print(f"{config_name}: {brain.get_total_neuron_count()} neurons, "
          f"{brain.get_total_synapse_count()} synapses")
```

## Enhanced Exception Handling

The enhanced interface provides better error messages and recovery suggestions:

```python
try:
    brain = nlm.create_brain(config)
    brain.initialize()
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    print("Try checking your config parameters")
except nlm.InitializationError as e:
    print(f"Initialization error: {e}")
    print("Ensure all required subsystems are enabled")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
    print("Check that brain.initialize() was called before brain.step()")
```

## Quick Reference

### Quick Functions

| Function | Description |
|----------|-------------|
| `create_brain(config)` | Create NLM brain with optional configuration |
| `create_educational_simulation()` | Create complete educational simulation |
| `create_simulation_environment()` | Create simulation world |
| `create_agent(brain, world)` | Create agent interface |
| `ConfigBuilder()` | Fluent API for configuration |
| `NLM()` | High-level simulation control |
| `create_performance_monitor()` | Performance tracking tool |
| `brain_context()` | Automatic brain lifecycle management |
| `simulation_episode()` | Complete episode management |

### Configuration Presets

| Function | Use Case |
|----------|----------|
| `create_research_config()` | Research simulations |
| `create_educational_config()` | Educational demonstrations |
| `create_simple_config()` | Quick tests |

### Exception Types

| Exception | When raised |
|-----------|-------------|
| `NLMError` | Base error for all NLM issues |
| `ConfigurationError` | Invalid or missing configuration |
| `InitializationError` | Brain/system initialization failure |
| `SimulationError` | Runtime simulation error |

## Performance Optimization Tips

1. **Use simple configurations for testing**: `create_simple_config()`
2. **Disable learning for speed**: `enable_reward_modulation(False)`
3. **Monitor performance**: Use `PerformanceMonitor()`
4. **Use context managers**: `with simulation_episode(...)`
5. **Leverage presets**: Use predefined configurations

## Examples Directory

The repository includes an `examples/` directory with comprehensive demonstration scripts.

## Installation Requirements

```bash
pip install numpy>=1.20
```

Optional development dependencies:

```bash
pip install pytest>=7.0
```

## Project Structure

```
nlm/
├── __init__.py              # Main interface
├── core.py                  # Core NLM classes
├── examples/                # Example scripts
└── docs/                    # Documentation (to be added)
```

## License

MIT License - See `LICENSE` file for details.

## Contact

For questions, issues, or contributions:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io

Enjoy exploring the enhanced NLM Python interface! 🚀

---

*Enhanced Python interface version 0.1.0*
*Backward compatible with existing code*
*New features for easier simulation control*