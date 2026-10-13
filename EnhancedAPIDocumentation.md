# NLM Enhanced Python API Reference

This document describes the enhanced Python API for NLM (Neural Learning Machine), which provides advanced features for both beginners and experienced users.

## Overview

The enhanced Python API builds upon the original NLM bindings with additional convenience methods, advanced features, and improved usability while maintaining full backward compatibility.

## Quick Start (Enhanced)

### Basic Usage

```python
import pynlm

# Create a brain using preset configurations
brain = pynlm.create_brain_for_task("learning")
brain._initialize_default()  # Convenience method

# Run a simulation with validation
stats = brain.run_simulation(1000, verbose=True)
print(f"Average firing rate: {stats[2]:.2f}")

# Check brain health
health = brain.check_health()
if health.isHealthy:
    print("✓ Brain is healthy and ready for simulation")
```

### Complete Agent Setup

```python
import pynlm

# Create a complete agent with all systems
brain, world, agent = pynlm.setup_complete_agent("learning", "adaptive")

# Run the agent in the world
results = pynlm.run_agent_simulation(brain, world, agent, 500, verbose=True)

# Analyze performance
metrics = pynlm.profile_brain_performance(brain)
print(f"Stability: {metrics.stabilityClassification}")
print(f"Avg firing rate: {metrics.avgFiringRate:.2f}")
```

## Enhanced API Documentation

### Brain Class Enhancements

#### Convenience Methods

| Method | Description | Example |
|--------|-------------|---------|
| `_initialize_default()` | Initialize with default config | `brain._initialize_default()` |
| `step_with_validation()` | Step with optional validation | `brain.step_with_validation(step)` |
| `batch_step()` | Perform multiple steps efficiently | `brain.batch_step(0, 100)` |
| `save_checkpoint()` | Save with timestamp | `path = brain.save_checkpoint()` |
| `run_simulation()` | Complete simulation with stats | `stats = brain.run_simulation(1000)` |
| `check_health()` | Brain health diagnostics | `health = brain.check_health()` |

#### Advanced Methods

| Method | Description | Example |
|--------|-------------|---------|
| `inject_current_batch()` | Inject current to multiple neurons | `brain.inject_current_batch(injections)` |
| `skip_development()` | Advance development stages | `stage = brain.skip_development(5)` |
| `clear_memory()` | Reset brain state | `brain.clear_memory()` |
| `get_detailed_stats()` | Comprehensive brain metrics | `stats = brain.get_detailed_stats()` |

### Config Class Enhancements

#### Presets

| Preset | Description |
|--------|-------------|
| `"simple"` | Minimal configuration for basic testing |
| `"complex"` | High-performance configuration for demanding tasks |
| `"learning"` | Enhanced learning-focused configuration |

#### Convenience Methods

| Method | Description | Example |
|--------|-------------|---------|
| `set_safe()` | Set with validation | `config.set_safe("neuron_count", "500")` |
| `_create_preset()` | Create preset configuration | `config._create_preset("learning")` |
| `diagnose()` | Configuration health check | `diag = config.diagnose()` |
| `batch_set()` | Set multiple values | `config.batch_set(settings)` |

### AgentBrain Class Enhancements

#### Behavior Modes

| Mode | Description |
|------|-------------|
| `"exploratory"` | High curiosity, maximum exploration |
| `"conservative"` | Stable, reliable operation |
| `"adaptive"` | Dynamic adaptation to environment |

#### Convenience Methods

| Method | Description | Example |
|--------|-------------|---------|
| `enable_all_systems()` | Enable all subsystems | `agent.enable_all_systems()` |
| `reset_complete()` | Complete agent reset | `agent.reset_complete()` |
| `get_status_report()` | Comprehensive status | `status = agent.get_status_report()` |

#### Analysis Methods

| Method | Description | Example |
|--------|-------------|---------|
| `process_sensory_batch()` | Batch sensory processing | `agent.process_sensory_batch(percepts)` |
| `analyze_motor_command()` | Analyze motor command | `analysis = agent.analyze_motor_command()` |
| `apply_reward_modulation_analyzed()` | Apply reward with analysis | `agent.apply_reward_modulation_analyzed(reward, pred, True)` |
| `get_comprehensive_state()` | Complete agent state | `state = agent.get_comprehensive_state()` |

## Configuration Presets

### Simple Preset

```python
brain = pynlm.create_brain_for_task("simple")
# 200 neurons, 1 region, 5% connectivity
```

### Complex Preset

```python
brain = pynlm.create_brain_for_task("complex")
# 2000 neurons, 3 regions, 15% connectivity, all plasticity enabled
```

### Learning Preset

```python
brain = pynlm.create_brain_for_task("learning")
# 1000 neurons, enhanced neuromodulation, all plasticity enabled
```

## Advanced Usage Patterns

### Pattern 1: Learning Agent with Monitoring

```python
def run_learning_experiment(num_episodes=100):
    # Setup learning agent
    brain = pynlm.create_brain_for_task("learning")
    brain._initialize_default()
    
    for episode in range(num_episodes):
        # Create agent with adaptive behavior
        agent = pynlm.create_agent_for_environment(brain, "adaptive")
        
        # Setup world
        world = pynlm.create_simple_world_with_settings(30, 30, 12, 12, 200.0)
        world.setAgentStart(15.0, 15.0)
        
        # Run episode
        results = pynlm.run_agent_simulation(brain, world, agent, 200, verbose=False)
        
        # Check if learning is occurring
        if episode % 20 == 0:
            print(f"Episode {episode}: Avg firing = {results[2]:.2f}")
    
    # Profile final performance
    metrics = pynlm.profile_brain_performance(brain)
    print(f"Final stability: {metrics.stabilityClassification}")
    
    return brain
```

### Pattern 2: Comparative Analysis

```python
def compare_behaviors(task_type="learning"):
    behaviors = ["exploratory", "conservative", "adaptive"]
    results = {}
    
    for behavior in behaviors:
        brain = pynlm.create_brain_for_task(task_type)
        brain._initialize_default()
        
        world = pynlm.create_simple_world_with_settings(20, 20, 8, 8, 100.0)
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.create_agent_for_environment(brain, behavior)
        
        # Run simulation
        simulation_results = pynlm.run_agent_simulation(
            brain, world, agent, 500, verbose=False
        )
        
        # Get performance metrics
        metrics = pynlm.profile_brain_performance(brain)
        
        results[behavior] = {
            'avg_firing': simulation_results[2],
            'total_spikes': simulation_results[1],
            'stability': metrics.stabilityClassification,
            'confidence': metrics.confidence
        }
    
    return results
```

### Pattern 3: Development Tracking

```python
def track_development_over_time(brain, time_steps=100):
    developmental_stages = []
    firing_rates = []
    neuromodulation_levels = []
    
    for step in range(time_steps):
        # Record current state
        developmental_stages.append(brain.getDevelopmentalStage())
        firing_rates.append(brain.getAverageFiringRate())
        neuromodulation_levels.append(brain.getExcitationInhibitionRatio())
        
        # Advance development
        brain.develop()
        
        # Simulate neural activity
        brain.step(step)
    
    return {
        'developmental_stages': developmental_stages,
        'firing_rates': firing_rates,
        'neuromodulation_levels': neuromodulation_levels,
        'final_stage': developmental_stages[-1]
    }
```

## Debugging and Error Analysis

### Health Checks

```python
# Check brain health
brain = pynlm.create_brain_for_task("learning")
health = brain.check_health()
if not health.isHealthy:
    print(f"Issue: {health.statusMessage}")
```

### Error Analysis

```python
# Analyze brain for issues
brain = pynlm.create_brain_for_task("complex")
analysis = pynlm.analyze_brain_errors(brain)
print(f"Primary issue: {analysis.primaryIssue}")
print(f"Recommendations: {analysis.recommendations}")
print(f"Confidence: {analysis.confidence * 100:.1f}%")
```

### Agent Status Reporting

```python
# Get comprehensive agent status
agent = pynlm.create_agent_for_environment(brain, "adaptive")
status = agent.get_status_report()

print(f"Initialized: {status.isInitialized}")
print(f"Curiosity Level: {status.curiosityLevel}")
print(f"Status: {status.statusMessage}")
```

## Performance Optimization

### Batch Operations

```python
# Efficient batch current injection
injections = [(neuron_id_1, current_1), (neuron_id_2, current_2), ...]
brain.inject_current_batch(injections)

# Multiple simulation steps
brain.batch_step(0, 1000)
```

### Memory Management

```python
# Clean reset with re-initialization
brain.clear_memory()  # Resets and re-initializes if config exists
```

## Mathematical Constants and Utilities

```python
import pynlm

# Access mathematical constants
pi = pynlm.PI
e = pynlm.E
golden_ratio = pynlm.GOLDEN_RATIO

# Get available presets
presets = pynlm.get_available_presets()
print(f"Available presets: {presets}")

# Get behavior modes
modes = pynlm.get_behavior_modes()
print(f"Available modes: {modes}")
```

## Error Handling

The enhanced API provides better error handling with:

1. **Validation**: Methods validate inputs before processing
2. **Descriptive errors**: Clear error messages with specific guidance
3. **Graceful degradation**: Fallback behaviors when operations fail
4. **Health checks**: Early detection of configuration and runtime issues

### Common Error Scenarios

```python
# Example of safe configuration
config = pynlm.createDefaultConfig()

try:
    config.set_safe("neuron_count", "1000")  # Will validate
    config.set_safe("connection_probability", "0.05")  # Will validate
    print("Configuration applied successfully")
except Exception as e:
    print(f"Configuration error: {e}")

# Example of safe simulation
try:
    brain = pynlm.create_brain_for_task("learning")
    stats = brain.run_simulation(1000, verbose=True)
    print("Simulation completed successfully")
except Exception as e:
    print(f"Simulation error: {e}")
```

## Backward Compatibility

The enhanced API maintains full backward compatibility:

```python
# These original APIs still work exactly as before
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enhanced APIs provide additional functionality
brain = pynlm.create_brain_for_task("learning")  # New convenience method
world = pynlm.create_simple_world_with_settings(20, 20, 8, 8)  # New convenience method
```

## Advanced User Features

### For Researchers

1. **Performance Profiling**: `pynlm.profile_brain_performance()`
2. **Error Analysis**: `pynlm.analyze_brain_errors()`
3. **Development Tracking**: Built-in development stage monitoring
4. **Comparative Studies**: Easy setup for behavior comparison

### For Developers

1. **Batch Operations**: Efficient processing of multiple operations
2. **Memory Management**: Clean reset and cleanup utilities
3. **Configuration Presets**: Standardized configurations for different use cases
4. **Debugging Tools**: Comprehensive status reporting and health checks

### For Experimenters

1. **Simulation Runners**: High-level simulation management
2. **Result Analysis**: Built-in statistical analysis and reporting
3. **Agent Lifecycle**: Complete agent creation and management
4. **Environment Setup**: Pre-configured world environments

## Migration Guide

### From Original to Enhanced API

#### Configuration

**Original:**
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")
config.set("brain.neuron_count", 1000)
```

**Enhanced:**
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")
config.set_safe("neuron_count", "1000")  # Better validation
```

#### Brain Creation

**Original:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

**Enhanced:**
```python
brain = pynlm.create_brain_for_task("learning")  # Preset for learning
brain._initialize_default()  # Convenience method
stats = brain.run_simulation(100)  # Single call with statistics
```

#### Agent Setup

**Original:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
```

**Enhanced:**
```python
agent = pynlm.create_agent_for_environment(brain, "adaptive")
world = pynlm.create_simple_world_with_settings(20, 20, 8, 8)
agent.initialize(*pynlm.setup_complete_agent("learning", "adaptive")[1:])
# Single method handles initialization and setup
```

## Version Information

- **API Version**: 0.1.0-enhanced
- **Minimum Compatible Version**: 0.1.0
- **New Features**: 25+ new convenience methods and advanced features
- **Backward Compatibility**: 100% maintained

## Future Enhancements

Planned enhancements for future releases:

1. **Visualization Integration**: Direct integration with plotting libraries
2. **Machine Learning Interfaces**: Scikit-learn compatible APIs
3. **Distributed Computing**: Multi-process and multi-GPU support
4. **Cloud Integration**: AWS/GCP integration for distributed simulations
5. **Advanced Statistics**: Statistical hypothesis testing and confidence intervals

## Acknowledgments

This enhanced API builds upon the original NLM Python bindings and introduces significant improvements for research and development productivity.

- Original NLM team for the foundational C++ implementation
- Contributors for testing and validation
- Community feedback for feature requests
- Open source contributors for shared improvements