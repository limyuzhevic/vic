# Advanced NLM Python API with Convenience Methods

This document provides comprehensive documentation for the advanced convenience methods added to the NLM Python API. These methods enhance the user experience with better error handling, performance monitoring, and simplified workflows.

## Introduction

The NLM Python API now includes advanced convenience methods that provide:

- **Brain Statistics & Monitoring**: Comprehensive brain status reporting
- **Agent Status Management**: Detailed agent state tracking
- **Configuration Management**: Enhanced configuration validation and management
- **World Monitoring**: Real-time world status and statistics
- **Serialization Convenience**: Simplified save/load operations
- **Error Handling**: Improved error detection and recovery
- **Performance Monitoring**: Built-in performance tracking
- **Validation Framework**: Comprehensive configuration validation

## Part 1: Brain Statistics & Convenience Methods

### Brain.getBrainStats()

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive brain statistics
stats = brain.getBrainStats()

print(stats)
# Output example:
# {
#     "total_neurons": 1000,
#     "total_synapses": 2500,
#     "firing_neurons": 25,
#     "average_firing_rate": 0.015,
#     "spike_count": 1500,
#     "excitatory_inhibition_ratio": 1.2,
#     "developmental_stage": "Adult",
#     "neurons_by_type": {"excitatory": 800, "inhibitory": 200},
#     "regions": ["cortex", "hippocampus"]
# }
```

**Returns**: Dictionary containing comprehensive brain statistics

**Key Statistics**:
- `total_neurons`: Total number of neurons in the brain
- `total_synapses`: Total number of synaptic connections
- `firing_neurons`: Number of currently firing neurons
- `average_firing_rate`: Average firing rate across all neurons
- `spike_count`: Total spike count since initialization
- `excitatory_inhibition_ratio`: E/I balance ratio
- `developmental_stage`: Current developmental stage
- `neurons_by_type`: Breakdown of neurons by type
- `regions`: List of brain regions

### Brain.getPerformanceStats()

```python
# Get performance statistics
perf_stats = brain.getPerformanceStats()

print(f"Average step time: {perf_stats['average_step_time_ms']:.2f} ms")
print(f"Last step time: {perf_stats['last_step_time_ms']:.2f} ms")
print(f"Total steps: {perf_stats['total_steps']}")
print(f"Neuron operations per step: {perf_stats['neuron_operations_per_step']}")
```

**Returns**: Dictionary containing performance metrics

**Performance Metrics**:
- `last_step_time_ms`: Time for most recent step
- `average_step_time_ms`: Average time per simulation step
- `total_steps`: Total number of steps executed
- `neuron_operations_per_step`: Estimated neuron operations per step
- `memory_usage_mb`: Current memory usage (if available)

## Part 2: Agent Status & Convenience Methods

### AgentBrain.getAgentBrainStatus()

```python
import pynlm

# Create agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Get comprehensive agent status
status = agent.getAgentBrainStatus()

print(status)
# Output example:
# {
#     "brain_reference": <brain object>,
#     "sensory_input_size": 320,
#     "motor_output_size": 8,
#     "reward_modulation_enabled": true,
#     "structural_plasticity_enabled": true,
#     "development_enabled": true,
#     "curiosity_enabled": true,
#     "sensory_neuron_groups": {
#         "vision": 256,
#         "touch": 8,
#         "internal": 4,
#         "proprioception": 6
#     },
#     "current_curiosity_level": 0.75,
#     "current_novelty_level": 0.32,
#     "current_prediction_error": 0.15,
#     "developmental_stage": "Adult"
# }
```

**Returns**: Dictionary containing comprehensive agent status

**Agent Status Fields**:
- `brain_reference`: Reference to the underlying brain object
- `sensory_input_size`: Expected sensory input size
- `motor_output_size`: Expected motor output size
- `reward_modulation_enabled`: Whether reward modulation is enabled
- `structural_plasticity_enabled`: Whether structural plasticity is enabled
- `development_enabled`: Whether development is enabled
- `curiosity_enabled`: Whether curiosity is enabled
- `sensory_neuron_groups`: Breakdown of sensory neuron groups
- `current_curiosity_level`: Current curiosity level (0-1)
- `current_novelty_level`: Current novelty level (0-1)
- `current_prediction_error`: Current prediction error
- `developmental_stage`: Current developmental stage

### AgentBrain.getAgentPerformanceStats()

```python
# Get agent performance statistics
perf_stats = agent.getAgentPerformanceStats()

print(f"Steps processed: {perf_stats['steps_processed']}")
print(f"Sensory inputs processed: {perf_stats['sensory_inputs_processed']}")
print(f"Motor commands generated: {perf_stats['motor_commands_generated']}")
print(f"Average decision_time_ms: {perf_stats['average_decision_time_ms']:.2f} ms")
```

**Returns**: Dictionary containing agent performance metrics

**Agent Performance Metrics**:
- `steps_processed`: Number of steps processed
- `sensory_inputs_processed`: Number of sensory inputs processed
- `motor_commands_generated`: Number of motor commands generated
- `average_decision_time_ms`: Average decision time per step
- `exploration_rate`: Rate of exploratory actions

## Part 3: Enhanced Configuration Management

### Config.getConfigStats()

```python
import pynlm

config = pynlm.createDefaultConfig()

# Add some configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("learning.rate", 0.001)
config.set("test.bool_value", True)
config.set("test.float_value", 3.14)
config.set("test.string_value", "test")

# Get configuration statistics
stats = config.getConfigStats()

print(stats)
# Output example:
# {
#     "total_keys": 6,
#     "sources": {
#         "Default": 0,
#         "File": 0,
#         "CommandLine": 0,
#         "Runtime": 6
#     },
#     "default_values": [],
#     "runtime_values": ["brain.neuron_count", "brain.synapse_density", "learning.rate", "test.bool_value", "test.float_value", "test.string_value"]
# }
```

**Returns**: Dictionary containing configuration statistics

**Configuration Statistics**:
- `total_keys`: Total number of configuration keys
- `sources`: Count of keys by source (Default, File, CommandLine, Runtime)
- `default_values`: List of keys with default values
- `runtime_values`: List of keys set at runtime

### Config.validateConfig()

```python
# Validate configuration and get errors
errors = config.validateConfig()

if errors:
    print("Configuration errors found:")
    for error in errors:
        print(f"  - {error}")
else:
    print("✓ Configuration is valid")
```

**Returns**: List of validation error strings (empty if valid)

**Validation Rules**:
- Checks for valid data types (int, float, bool, string)
- Validates numeric ranges (neuron count: 100-10000)
- Validates configuration key formats
- Checks for duplicate keys
- Validates inter-parameter dependencies

### Config.getConfigSummary()

```python
# Get human-readable configuration summary
summary = config.getConfigSummary()

print(summary)
# Output example:
# Configuration Summary (6 entries):
#   brain.neuron_count = "2000" (Runtime)
#   brain.synapse_density = "0.15" (Runtime)
#   learning.rate = "0.001" (Runtime)
#   test.bool_value = "true" (Runtime)
#   test.float_value = "3.14" (Runtime)
#   test.string_value = "\"test\"" (Runtime)
```

**Returns**: Human-readable configuration summary string

## Part 4: Enhanced World Management

### SimpleWorld.getWorldStatus()

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20.0, height=20.0, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Get comprehensive world status
status = world.getWorldStatus()

print(status)
# Output example:
# {
#     "width": 20.0,
#     "height": 20.0,
#     "vision_width": 8,
#     "vision_height": 8,
#     "agent_position": {"x": 10.0, "y": 10.0},
#     "agent_orientation": 0.0,
#     "agent_energy": 1.0,
#     "agent_health": 1.0,
#     "simulation_time": 0.0,
#     "objects_count": 0,
#     "resources_count": 0,
#     "hazards_count": 0,
#     "walls_count": 0,
#     "vision_data": [0.1, 0.2, ...],  # Flattened vision array
#     "collision_detected": false
# }
```

**Returns**: Dictionary containing comprehensive world status

**World Status Fields**:
- `width`: World width
- `height`: World height
- `vision_width`: Vision width
- `vision_height`: Vision height
- `agent_position`: Current agent position
- `agent_orientation`: Current agent orientation
- `agent_energy`: Current energy level (0-1)
- `agent_health`: Current health level (0-1)
- `simulation_time`: Current simulation time
- `objects_count`: Total objects in world
- `resources_count`: Number of resources
- `hazards_count`: Number of hazards
- `walls_count`: Number of walls
- `vision_data`: Current vision data (flattened)
- `collision_detected`: Whether collision was detected

## Part 5: Enhanced Serialization

### Brain.save() Enhanced

```python
import pynlm
import tempfile

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save brain state
with tempfile.NamedTemporaryFile(delete=False, suffix='.bin') as f:
    brain_file = f.name

if brain.save(brain_file):
    print(f"✓ Brain saved to {brain_file}")
else:
    print("✗ Failed to save brain")
```

**Returns**: True if save successful, False otherwise

### Config.saveToFile() Enhanced

```python
import pynlm
import tempfile
import json

config = pynlm.createDefaultConfig()

# Add some configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save configuration
with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.json') as f:
    config_file = f.name

if config.saveToFile(config_file):
    print(f"✓ Configuration saved to {config_file}")
    
    # Load and verify
    new_config = pynlm.createDefaultConfig()
    if new_config.loadFromFile(config_file):
        print(f"✓ Configuration loaded successfully")
        print(f"  Neuron count: {new_config.getOr('brain.neuron_count', 0)}")
else:
    print("✗ Failed to save configuration")
```

**Returns**: True if save successful, False otherwise

## Part 6: Error Handling & Recovery

### Enhanced Error Detection

```python
import pynlm

# Create brain with error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("✓ Brain created and initialized successfully")
    
except Exception as e:
    print(f"✗ Error: {e}")
    raise

# Safe brain operations with error handling
try:
    # Multiple operations with error checking
    brain.step(0)
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
    brain.save("backup.bin")
    
    print("✓ All operations completed successfully")
    
except Exception as e:
    print(f"✗ Operation failed: {e}")
    # Recovery logic here
    pass

# Agent error handling
try:
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation with error handling
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"  Step {step} completed")
    
    print("✓ Simulation completed successfully")
    
except Exception as e:
    print(f"✗ Simulation failed: {e}")
    # Error recovery and cleanup
    agent.reset()
    brain.reset()
```

## Part 7: Performance Monitoring

### Performance Monitoring Usage

```python
import pynlm
import time
import statistics

class PerformanceMonitor:
    """Monitor NLM performance over time"""
    
    def __init__(self, brain, agent=None):
        self.brain = brain
        self.agent = agent
        self.step_times = []
        self.performance_stats = []
    
    def record_step(self, step_number):
        """Record performance for a single step"""
        start_time = time.perf_counter()
        
        # Perform step
        self.brain.step(step_number)
        
        elapsed = time.perf_counter() - start_time
        self.step_times.append(elapsed)
        
        # Record performance stats
        if self.brain:
            perf_stats = self.brain.getPerformanceStats()
            self.performance_stats.append(perf_stats)
        
        return elapsed
    
    def get_performance_summary(self):
        """Get performance summary"""
        if not self.step_times:
            return None
        
        return {
            "total_steps": len(self.step_times),
            "avg_step_time_ms": statistics.mean(self.step_times) * 1000,
            "min_step_time_ms": min(self.step_times) * 1000,
            "max_step_time_ms": max(self.step_times) * 1000,
            "stddev_step_time_ms": statistics.stdev(self.step_times) * 1000 if len(self.step_times) > 1 else 0,
            "throughput_steps_per_second": len(self.step_times) / statistics.mean(self.step_times) if self.step_times else 0
        }

# Usage example
monitor = PerformanceMonitor(brain, agent)

for step in range(100):
    monitor.record_step(step)
    
    if step % 20 == 0:
        summary = monitor.get_performance_summary()
        print(f"Step {step}: {summary['avg_step_time_ms']:.2f} ms avg, "
              f"{summary['throughput_steps_per_second']:.1f} steps/sec")
```

## Part 8: Validation Framework

### Configuration Validation Examples

```python
import pynlm

def validate_config(config):
    """Comprehensive configuration validation"""
    
    errors = []
    warnings = []
    
    # Validate brain parameters
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr("brain.neuron_count", 0)
        if neuron_count < 100:
            errors.append(f"Neuron count {neuron_count} is below minimum 100")
        elif neuron_count > 10000:
            warnings.append(f"Neuron count {neuron_count} is above recommended maximum 10000")
    
    if config.has("brain.synapse_density"):
        density = config.getOr("brain.synapse_density", 0.0)
        if density < 0.0 or density > 1.0:
            errors.append(f"Synapse density {density} must be between 0.0 and 1.0")
    
    if config.has("brain.v_thresh"):
        thresh = config.getOr("brain.v_thresh", 0.0)
        if thresh < -100.0 or thresh > 0.0:
            warnings.append(f"Threshold {thresh} is outside typical range (-100 to 0)")
    
    # Validate plasticity parameters
    if config.has("plasticity.stdp.learning_rate"):
        rate = config.getOr("plasticity.stdp.learning_rate", 0.0)
        if rate < 0.0 or rate > 1.0:
            warnings.append(f"STDP learning rate {rate} should be between 0.0 and 1.0")
    
    return errors, warnings

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 50)  # Invalid - too low
config.set("brain.synapse_density", 0.5)

errors, warnings = validate_config(config)

if errors:
    print("Validation errors:")
    for error in errors:
        print(f"  ✗ {error}")

if warnings:
    print("Validation warnings:")
    for warning in warnings:
        print(f"  ⚠ {warning}")
```

## Part 9: Best Practices

### 1. Error Handling Best Practices

```python
# Always validate before using
if not brain or not agent:
    raise ValueError("Brain and agent must be properly initialized")

# Use try-catch blocks for operations that can fail
for step in range(steps):
    try:
        # Your operations here
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
    except Exception as e:
        print(f"Step {step} failed: {e}")
        # Recovery logic
        brain.reset()
        agent.reset()
        world.reset()
        break

# Check results after operations
if brain.getTotalSpikeCount() == 0:
    print("⚠ Warning: No spikes detected - brain may not be active")
```

### 2. Performance Monitoring Best Practices

```python
# Monitor performance regularly
monitor = PerformanceMonitor(brain)

for step in range(total_steps):
    monitor.record_step(step)
    
    # Check for performance degradation
    if step % 100 == 0:
        summary = monitor.get_performance_summary()
        if summary and summary['avg_step_time_ms'] > 100:  # Threshold
            print(f"⚠ Performance degradation detected: {summary['avg_step_time_ms']:.2f} ms avg")
```

### 3. Configuration Management Best Practices

```python
# Save configurations for reproducibility
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.saveToFile("my_config.json")

# Load configurations
new_config = pynlm.createDefaultConfig()
if new_config.loadFromFile("my_config.json"):
    print("✓ Configuration loaded successfully")
else:
    print("✗ Failed to load configuration")
```

### 4. Serialization Best Practices

```python
# Use temporary files for backup
import tempfile

# Create backup before long operations
backup_file = "brain_backup_" + time.strftime("%Y%m%d_%H%M%S") + ".bin"
if brain.save(backup_file):
    print(f"✓ Backup created: {backup_file}")

# Cleanup old backups
cleanup_old_backups("brain_backup_", days=7)
```

## Conclusion

The enhanced NLM Python API provides comprehensive convenience methods that significantly improve:

1. **Developer Experience**: Simplified access to brain and agent status
2. **Error Handling**: Robust error detection and recovery
3. **Performance Monitoring**: Built-in performance tracking
4. **Configuration Management**: Enhanced validation and management
5. **Serialization**: Simplified save/load operations
6. **Debugging**: Comprehensive status reporting

These convenience methods make it easier to:

- Debug complex simulations
- Monitor performance over time
- Validate configuration parameters
- Recover from errors
- Analyze brain and agent behavior
- Compare different configurations

The enhanced API maintains full backward compatibility while providing powerful new tools for advanced NLM development.

---

*For more information, refer to the source code and examples in the repository.*
