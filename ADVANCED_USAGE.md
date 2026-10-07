# Advanced NLM Usage Guide

This guide demonstrates advanced usage patterns and features for power users working with the NLM (Neural Learning Machine) brain simulator.

## Context Managers and Resource Management

### Context Manager API

The NLM provides context managers for automatic resource cleanup and better error handling:

```python
import pynlm

# Basic context manager usage
with pynlm.BrainContext(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Simulation loop
    for step in range(100):
        brain.step(step)
        
    print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")

# Context manager with error handling
from pynlm import BrainContext

def run_simulation(steps, config=None):
    try:
        with BrainContext(config or pynlm.createAdvancedConfig()) as brain:
            brain.initialize()
            brain.stepBatch(steps)  # Advanced batch processing
            return brain.getStatsSummary()
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None

# Nested resource management
with pynlm.BrainContext(config1) as brain1:
    with pynlm.BrainContext(config2) as brain2:
        # Run experiments in parallel
        result1 = brain1.getStatsSummary()
        result2 = brain2.getStatsSummary()
        # Compare results
```

## Batch Processing and Performance Optimization

### Batch Operations

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Batch processing for better performance
print("Running 1000 simulation steps...")
stats = brain.stepBatch(1000)
print(f"Average step time: {stats['avg_step_time_ms']:.2f}ms")

# Neural network manipulation
brain_api = pynlm.NeuralNetworkAPI(brain)

# Add regions with specific patterns
region_id = brain_api.addRegionWithPattern(
    name="SensoryCortex",
    neuronCount=500,
    neuronType=pynlm.NeuronType.Excitatory,
    connectionDensity=0.15
)

# Create inter-region connections
brain_api.connectRegions(
    source=region_id,
    target=2,
    weight=1.5,
    delay=5,
    plasticity=pynlm.PlasticityFlags.STDP | pynlm.PlasticityFlags.Hebbian
)

# Get detailed region statistics
region_stats = brain_api.getRegionStats()
print("Region statistics:", region_stats)
```

## Advanced Configuration Management

### Configuration Validation

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.12)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 12345)

# Validate configuration
is_valid, errors = pynlm.validateBrainConfig(config)
if not is_valid:
    print("Configuration errors:")
    for key, error in errors.items():
        print(f"  {key}: {error}")
else:
    print("Configuration is valid")

# Create brain with validated configuration
try:
    brain = pynlm.createValidatedBrain(config)
    brain.initialize()
    print("Brain created successfully with validated configuration")
except pybind11.error::value_error as e:
    print(f"Failed to create brain: {e}")
```

### JSON Configuration Support

```python
import pynlm
import json
import os

# Save configuration as JSON
config = pynlm.createAdvancedConfig()
config.saveToFile("my_config.json")

# Load configuration from JSON
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Modify and save again
new_config.set("neuron_count", 1500)
new_config.saveToFile("updated_config.json")

# Read JSON content directly
with open("my_config.json", 'r') as f:
    json_data = json.load(f)
    print("JSON config entries:", len(json_data.get("configuration", {}).get("entries", [])))
```

## Advanced Agent Brain Operations

### Neural Network Operations

```python
import pynlm

# Create brain and agent
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with operation history
operation_history = []
for step in range(200):
    # Validate operation before execution
    agent.validateOperation("step")
    
    # Update world
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get operation history for debugging
    history = agent.getOperationHistory()
    if history:
        operation_history.append(history)

# Get comprehensive stats
brain_stats = brain.getStatsSummary()
agent_stats = {
    'developmental_stage': agent.getDevelopmentalStage(),
    'neuromodulation_level': agent.getNeuromodulationLevel(),
    'curiosity_level': agent.getCuriosityLevel(),
    'novelty_level': agent.getNoveltyLevel(),
    'prediction_error': agent.getPredictionError()
}

print("Brain stats:", brain_stats)
print("Agent stats:", agent_stats)
```

## Performance Profiling and Optimization

### Simulation Profiling

```python
import pynlm
import time

# Create brain
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Profile different simulation scenarios
profile_results = []

for scenario_name, steps in [("short", 100), ("medium", 1000), ("long", 5000)]:
    print(f"Profiling {scenario_name} scenario ({steps} steps)...")
    start_time = time.time()
    
    # Run simulation with profiling
    profile = pynlm.profileSimulation(brain, steps)
    
    end_time = time.time()
    profile["wall_clock_time"] = end_time - start_time
    profile["scenario"] = scenario_name
    profile_results.append(profile)
    
    print(f"  Performance: {profile['steps_per_second']:.0f} steps/sec")

# Analyze performance results
print("\nPerformance Analysis:")
for profile in profile_results:
    print(f"  {profile['scenario']}: {profile['steps_per_second']:.0f} steps/sec, "
          f"{profile['avg_step_time_ms']:.2f}ms per step")
```

## Advanced Neural Network Manipulation

### Custom Network Architectures

```python
import pynlm

# Create a custom network architecture
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create specialized regions
brain_api = pynlm.NeuralNetworkAPI(brain)

# Visual processing region
visual_region = brain_api.addRegionWithPattern(
    name="VisualCortex",
    neuronCount=400,
    neuronType=pynlm.NeuronType.Sensory,
    connectionDensity=0.1
)

# Motor control region
motor_region = brain_api.addRegionWithPattern(
    name="MotorCortex",
    neuronCount=300,
    neuronType=pynlm.NeuronType.Motor,
    connectionDensity=0.2
)

# Memory consolidation region
memory_region = brain_api.addRegionWithPattern(
    name="MemoryCortex",
    neuronCount=600,
    neuronType=pynlm.NeuronType.Internal,
    connectionDensity=0.05
)

# Create specific connection patterns
brain_api.connectRegions(visual_region, motor_region, weight=2.0)
brain_api.connectRegions(motor_region, memory_region, weight=1.5)
brain_api.connectRegions(memory_region, visual_region, weight=1.2)

# Get network statistics
network_stats = brain_api.getRegionStats()
print("Custom network architecture created successfully")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Error Handling and Recovery

### Robust Error Handling

```python
import pynlm
from pynlm import pybind11 as pybind11

# Create brain with error handling
config = pynlm.createAdvancedConfig()

# Validate configuration before use
is_valid, errors = pynlm.validateBrainConfig(config)
if not is_valid:
    print("Configuration validation failed:")
    for key, error in errors.items():
        print(f"  {key}: {error}")
    exit(1)

# Safe brain creation with try-catch
try:
    brain = pynlm.createValidatedBrain(config)
    brain.initialize()
    
    # Run simulation with error recovery
    for step in range(1000):
        try:
            brain.step(step)
            
            # Check for error conditions
            if brain.getFiringNeuronCount() == 0 and step > 100:
                print("Warning: No neurons firing - possible connectivity issue")
                
        except pybind11.error as e:
            print(f"Error at step {step}: {e}")
            brain.reset()  # Recover by resetting
            continue
            
    # Save brain state if simulation completes successfully
    brain.save("successful_brain_state.nlm")
    
except Exception as e:
    print(f"Fatal error: {e}")
    # Try to save partial state
    try:
        brain.save("error_brain_state.nlm")
    except:
        pass
```

## Multi-Agent Systems

### Running Multiple Brains

```python
import pynlm

# Create multiple independent brains for comparison
brains = []
for i in range(3):
    config = pynlm.createAdvancedConfig()
    config.set("random_seed", i * 1000 + 42)
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run experiments in parallel
results = []
for i, brain in enumerate(brains):
    print(f"Running experiment {i+1}/3...")
    
    # Enable different features for different brains
    if i == 0:
        brain.enableDevelopment(True)
        brain.enableCuriosity(True)
    elif i == 1:
        brain.enableRewardModulation(True)
        brain.enableStructuralPlasticity(True)
    else:
        brain.enableDevelopment(True)
        brain.enableRewardModulation(True)
    
    # Run simulation
    start_time = time.time()
    brain.stepBatch(500)
    end_time = time.time()
    
    # Collect results
    result = {
        'brain_id': i,
        'final_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'simulation_time': end_time - start_time,
        'features_enabled': {
            'development': brain.getDevelopmentSystem() is not None,
            'reward': brain.getDopamine() is not None,
            'curiosity': brain.getCuriosity() is not None,
            'memory': brain.getWorkingMemory() is not None
        }
    }
    results.append(result)

# Compare results
print("\nExperiment Results:")
for result in results:
    print(f"  Brain {result['brain_id']}:")
    print(f"    Spikes: {result['final_spikes']}")
    print(f"    Firing rate: {result['avg_firing_rate']:.3f}")
    print(f"    E/I ratio: {result['excitation_inhibition_ratio']:.3f}")
    print(f"    Time: {result['simulation_time']:.2f}s")
```

## Integration with External Systems

### Brain State Serialization

```python
import pynlm
import json

# Create and run brain
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
brain.stepBatch(1000)

# Save brain state to JSON for external analysis
save_path = "brain_state.json"
if brain.save(save_path):
    print(f"Brain state saved to {save_path}")
    
    # Load brain state back
    new_brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    if new_brain.load(save_path):
        print("Brain state loaded successfully")
        
        # Get statistics from loaded brain
        stats = new_brain.getStatsSummary()
        print("Loaded brain statistics:", stats)
    else:
        print("Failed to load brain state")
else:
    print("Failed to save brain state")

# Export data for machine learning
export_data = {
    'neuron_count': brain.getTotalNeuronCount(),
    'region_count': brain.getRegionCount(),
    'spike_count': brain.getTotalSpikeCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'avg_firing_rate': brain.getAverageFiringRate(),
    'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
    'developmental_stage': brain.getDevelopmentalStage(),
    'timestamp': time.time()
}

with open("brain_metrics.json", 'w') as f:
    json.dump(export_data, f, indent=2)

print("Brain metrics exported for analysis")
```

## Advanced Features Summary

### Key Features for Power Users

1. **Context Managers**: Automatic resource cleanup and error recovery
2. **Batch Processing**: Efficient simulation of multiple steps
3. **Configuration Validation**: Comprehensive error checking
4. **JSON Support**: Modern configuration file format
5. **Performance Profiling**: Detailed performance analysis
6. **Neural Network APIs**: Custom architecture creation
7. **Error Handling**: Robust exception handling and recovery
8. **Multi-Agent Systems**: Parallel experiment comparison
9. **State Serialization**: Brain state import/export
10. **Advanced Metrics**: Comprehensive performance tracking

### Performance Considerations

- Use batch operations for long simulations
- Profile performance to optimize simulation time
- Validate configurations before running experiments
- Implement proper error handling for long-running processes
- Monitor brain statistics for system health

### Best Practices

1. Always validate configuration before creating brains
2. Use context managers for resource management
3. Implement error recovery strategies
4. Profile performance regularly
5. Monitor brain statistics for system stability
6. Use appropriate simulation timesteps for your use case
7. Consider using batch operations for long simulations
8. Implement comprehensive logging for debugging

This guide provides comprehensive examples for advanced NLM usage. Start with the basic examples and gradually incorporate more advanced features as you become familiar with the system.