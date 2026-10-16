# NLM Enhanced Python API Guide

## Overview

This comprehensive guide documents the advanced Python API features available in the NLM (Neural Learning Machine) project. The enhanced Python bindings provide access to sophisticated neural simulation capabilities through the `pynlm` module.

## Quick Start

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(1000):
    brain.step(step)
    
print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes generated")
```

## Core API Classes

### Brain Class

The `Brain` class is the central neural simulation engine. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

#### Creation and Initialization

```python
# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Alternative: Create with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Basic Operations

```python
# Perform a simulation step
brain.step(step_number)  # Simple step with implicit time
brain.step(step_number, current_time)  # Step with explicit timestamp

# Save and load brain state
brain.save("my_brain.bin")
# Later...
brain.load("my_brain.bin")

# Reset brain
brain.reset()

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world, handling sensory transduction and motor decoding.

#### Basic Usage

```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Apply reward modulation
reward = 1.0  # Calculate based on world outcome
agent.applyRewardModulation(reward, predicted_reward)
```

### SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment for the agent.

#### Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Update world
world.update(timestep=0.1)

# Get sensory input
percept = world.getSensoryPercept()

# Get agent state
body = world.getAgentBody()
```

## Enhanced Python API Features

### 1. Batch Operations

The enhanced Python API provides efficient batch processing capabilities for high-throughput simulations.

#### Basic Batch Operations

```python
# Batch step execution
spike_count = pynlm.batchStep(brain, num_steps=1000)
print(f"Batch completed: {spike_count} spikes")

# Batch step with timestamps
spike_count = pynlm.batchStepWithTime(brain, num_steps=1000, time_step=0.1)

# Batch sensor input processing
inputs = [sensory_input1, sensory_input2, sensory_input3, ...]
pynlm.batchSensorInput(brain, inputs)

# Batch action production
actions = pynlm.batchActionProduction(brain, num_actions=100)
print(f"Produced {len(actions)} actions")

# Complete batch processing
actions, spike_count, active_neurons = pynlm.batchProcessing(
    brain, num_steps=500, sensory_inputs=sensory_list
)
```

#### Parallel Processing

```python
# Parallel batch processing with performance monitoring
result = pynlm.parallelBatchStep(
    brain, num_steps=2000, 
    inputs=sensory_batch, 
    enableAsync=True
)

spike_count, processing_time = result
print(f"Processed {spike_count} spikes in {processing_time}ms")
```

### 2. Event/Callback System

The event system enables real-time monitoring and custom processing during simulation.

#### Progress Callbacks

```python
# Register progress callback for simulation monitoring
def progress_callback(current_step, total_steps, progress):
    print(f"Progress: {progress*100:.1f}% ({current_step}/{total_steps})")

pynlm.registerProgressCallback(brain, total_steps=10000, callback=progress_callback)

# Run simulation with progress reporting
pynlm.runWithProgressCallback(
    brain, totalSteps=5000,
    progressCallback=progress_callback,
    logCallback=lambda msg: print(f"LOG: {msg}")
)
```

#### Spike Callbacks

```python
# Register spike event callback
def spike_callback(detailed_spike_event):
    spike = detailed_spike_event.spike
    neuron = detailed_spike_event.neuron
    print(f"Spike! Neuron {neuron} fired at time {spike.timestamp}")

# This would need access to the spike system
# pynlm.registerSpikeCallback(spike_system, spike_callback)
```

#### Plasticity Callbacks

```python
# Register plasticity event callback
def plasticity_callback(event_type, magnitude, step):
    print(f"Plasticity event: {event_type} with magnitude {magnitude} at step {step}")

pynlm.registerPlasticityCallback(brain, plasticity_callback)
```

#### Neuromodulation Callbacks

```python
# Register neuromodulation event callback
def neuromodulation_callback(n_type, level, time):
    print(f"Neuromodulation: {n_type} = {level} at time {time}")

pynlm.registerNeuromodulationCallback(brain, neuromodulation_callback)
```

#### Custom Event System

```python
# Create event hook system for custom processing
event_system = pynlm.createEventHookSystem()

# Add event hooks for custom processing
def custom_processing(event_data):
    # Custom logic here
    return processed_data

pynlm.addEventHook(brain, "custom_event", custom_processing)
```

### 3. Context Managers

Context managers provide automatic resource management and cleanup.

#### Brain Context Manager

```python
# Use brain with automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
    print(f"Brain processed {brain.getTotalSpikeCount()} spikes")
    # Brain automatically reset on exit
```

#### World Context Manager

```python
# Use world with automatic cleanup
with pynlm.createSimpleWorld() as world:
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # World operations
    for step in range(100):
        world.update(0.1)
        # World automatically reset on exit
```

#### AgentBrain Context Manager

```python
# Use agent brain with automatic cleanup
with pynlm.createAgentBrain(brain) as agent:
    agent.initialize(world)
    for step in range(500):
        # Agent operations
        pass
    # Agent automatically reset on exit
```

### 4. Advanced Configuration

Advanced configuration features enable sophisticated parameter management.

#### Configuration Templates

```python
# Create configuration templates
base_config = pynlm.createDefaultConfig()

# Create specialized template for high-performance simulation
high_perf_config = base_config.createTemplate(
    name="high_performance",
    learning_rate=0.01,
    neuron_count=10000,
    synapse_density=0.2,
    temperature=0.5,
    exploration_boost=0.1
)

# Create template for learning-focused simulation
learning_config = base_config.createTemplate(
    name="learning_focused",
    dopamine_scale=2.0,
    curiosity_enabled=True,
    novelty_detection=True,
    prediction_error_weight=0.5
)
```

#### Configuration Validation

```python
# Validate configuration
def validate_config(config):
    is_valid, errors = config.validate()
    if is_valid:
        print("Configuration is valid!")
    else:
        print("Configuration errors:")
        for error in errors:
            print(f"  - {error}")

validate_config(high_perf_config)
```

#### Configuration Comparison and Merging

```python
# Compare two configurations
diff = config1.compare(config2)
for change in diff:
    print(f"Difference: {change}")

# Merge configurations
merged = config1.merge(config2)
print(f"Merged configuration has {len(merged.getKeys())} parameters")

# Compare values using Python types
int_val, float_val, str_val, bool_val = config.get("brain.parameter_name")
```

### 5. Statistics and Analysis

Comprehensive statistics and analysis functions for brain network evaluation.

#### Network Analysis

```python
# Analyze brain network structure
regions, network_stats = pynlm.analyzeNetwork(brain)

print(f"Network contains {network_stats['total_regions']} regions")
print(f"Total neurons: {network_stats['total_neurons']}")
print(f"Total synapses: {network_stats['total_synapses']}")
print(f"Active neurons: {network_stats['active_neurons']}")
print(f"Firing rate: {network_stats['average_firing_rate']:.2f}")
print(f"E/I ratio: {network_stats['e_i_ratio']:.2f}")

for region in regions:
    print(f"Region {region['name']}: {region['neuron_count']} neurons, "
          f"firing rate {region['firing_rate']:.2f}")
```

#### Pattern Detection

```python
# Detect patterns from spike data
spike_system = brain.getSpikeSystem()  # If available
detected_patterns, pattern_scores, message = pynlm.detectPatterns(
    spike_system, step=1000
)

print(f"Detected patterns: {detected_patterns}")
print(f"Pattern scores: {pattern_scores}")
print(f"Analysis complete: {message}")
```

#### Time-Series Statistics

```python
# Compute statistics over a time window
step_stats, summary = pynlm.computeStatistics(
    brain, start_step=0, end_step=1000
)

print("Summary statistics:")
print(f"  Mean firing neurons: {summary['mean_firing_neurons']:.1f}")
print(f"  Mean active neurons: {summary['mean_active_neurons']:.1f}")
print(f"  Mean firing rate: {summary['mean_firing_rate']:.2f}")
print(f"  Max firing neurons: {summary['max_firing_neurons']}")
print(f"  Firing rate variance: {summary['firing_rate_variance']:.2f}")

# Step-by-step statistics (sample every 100 steps)
for i, stats in enumerate(step_stats[::100]):
    print(f"Step {i*100}: {stats['firing_neurons']} firing, "
          f"{stats['active_neurons']} active, "
          f"rate {stats['average_firing_rate']:.2f}")
```

#### Connectivity Analysis

```python
# Analyze neural connectivity patterns
connectivity = pynlm.analyzeConnectivity(brain)

print(f"Total regions: {connectivity['total_regions']}")
print(f"Clustering coefficient: {connectivity['clustering_coefficient']:.3f}")
print("\nConnections:")
for conn in connectivity['connections'][:10]:  # Show first 10
    print(f"  {conn['source']} -> {conn['target']}: "
          f"strength {conn['strength']:.3f}")
```

#### Visualization Data Preparation

```python
# Prepare data for visualization tools
viz_data = pynlm.prepareVisualizationData(brain, step=1000)

# Regional visualization data
for region in viz_data['regions']:
    activity_level = region['activity_level']
    if activity_level > 0.8:
        print(f"Region {region['name']}: High activity ({activity_level:.2f})")
    elif activity_level > 0.5:
        print(f"Region {region['name']}: Medium activity ({activity_level:.2f})")

# Development stage
print(f"Development stage: {viz_data['development']['stage']}")
print(f"Plasticity modifier: {viz_data['development']['plasticity_modifier']:.2f}")
```

### 6. Profiling and Monitoring

Performance tracking and monitoring capabilities.

#### Basic Profiling

```python
# Create profiler
start_time, initial_spikes, initial_neurons = pynlm.createProfiler(brain)

# Run simulation
for step in range(100):
    brain.step(step)
    
# Profile after simulation
profile, new_time, final_spikes = pynlm.profileStep(
    brain, start_time, initial_spikes, initial_neurons
)

print(f"Step time: {profile['step_time_us']} microseconds")
print(f"Spikes generated: {profile['spikes_generated']}")
print(f"Active neurons: {profile['neurons_active']}")
print(f"Firing neurons: {profile['firing_neurons']}")
```

#### Performance Monitoring

```python
# Start performance monitoring
start_time, initial_spikes, metrics = pynlm.startPerformanceMonitoring(brain)

# Run simulation with periodic monitoring
for step in range(100):
    brain.step(step)
    metric, current_spikes = pynlm.monitorPerformance(
        brain, start_time, initial_spikes, metrics
    )

# Stop monitoring and generate report
report = pynlm.stopPerformanceMonitoring(metrics)

print(f"Performance report:")
print(f"  Total steps: {report['total_steps']}")
print(f"  Total spikes: {report['total_spikes']}")
print(f"  Average spikes per step: {report['average_spikes_per_step']:.2f}")
print(f"  Max firing neurons: {report['max_firing_neurons']}")
```

#### Memory Monitoring

```python
# Monitor memory usage
memory_info = pynlm.monitorMemoryUsage()

print(f"Memory usage:")
print(f"  Allocated: {memory_info['allocated_mb']:.1f} MB")
print(f"  Current: {memory_info['current_mb']:.1f} MB")
print(f"  Peak: {memory_info['peak_mb']:.1f} MB")
print(f"  Threads: {memory_info['thread_count']}")
```

### 7. Learning Progress Tracking

Track learning progress through neuromodulation and development metrics.

```python
# Track learning progress over time
for step in range(0, 10000, 100):
    progress = pynlm.trackLearningProgress(brain, step)
    
    print(f"\nStep {step} Learning Progress:")
    print(f"  Dopamine level: {progress.get('dopamine_level', 0):.3f}")
    print(f"  Curiosity level: {progress.get('curiosity_level', 0):.3f}")
    print(f"  Novelty level: {progress.get('novelty_level', 0):.3f}")
    print(f"  Prediction error: {progress.get('prediction_error', 0):.3f}")
    print(f"  Spike efficiency: {progress.get('spike_efficiency', 0):.3f}")
    print(f"  Developmental stage: {progress.get('developmental_stage')}")
    print(f"  Plasticity active: {progress.get('plasticity_active', False)}")
```

### 8. Advanced AgentBrain Features

The enhanced AgentBrain provides additional motor control capabilities.

#### Advanced Motor Decoding

```python
# Configure advanced motor decoding
agent.setActionTemperature(0.5)  # Softer exploration
agent.setQLearningParameters(alpha=0.1, gamma=0.99, epsilon=0.1)

# Get advanced motor command with neural activity
neural_activity = brain.produceAction()  # Get neural activity
advanced_action = agent.getAdvancedMotorCommand(neural_activity)
```

#### Motor Planning

```python
# Set high-level goal
agent.setGoal("find_food", parameters=[x=5.0, y=5.0])

# Get action sequence for current goal
sequence = agent.getCurrentActionSequence()
if sequence:
    print(f"Goal: {sequence.goalDescription}")
    print(f"Total expected reward: {sequence.totalExpectedReward:.2f}")
    print(f"Total duration: {sequence.totalDuration:.2f}")
    
    # Execute next action
    while not sequence.isComplete():
        action = sequence.executeNext()
        print(f"Executing: {action}")
```

#### Reinforcement Learning

```python
# Configure reinforcement learning
agent.enableReinforcementLearning(True)
agent.setDiscountFactor(0.95)
agent.setExplorationRate(0.2)

# Get learned skill strength for association
skill_strength = agent.getSkillStrength(sensory_input, action)
print(f"Skill strength: {skill_strength:.3f}")
```

#### Sensory-Motor Integration

```python
# Enable sensory-motor integration
agent.enableSensoryMotorIntegration(True)

# Get predicted sensory consequences
predicted = agent.getPredictedSensory(action)
print(f"Predicted sensory: {predicted[:5]}...")  # First 5 values

# Get internal world model
internal_model = agent.getInternalWorldModel()
print(f"Internal state dimension: {len(internal_model)}")
```

## Complete Example: Learning Agent

```python
import pynlm
import time

def run_learning_agent(episode_length=1000, enable_callbacks=True):
    """Complete example of a learning agent using enhanced features."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableAdvancedMotorDecoding(True)
    agent.enableMotorPlanning(True)
    agent.enableReinforcementLearning(True)
    agent.enableSkillLearning(True)
    agent.enableSensoryMotorIntegration(True)
    
    # Create progress tracking
    if enable_callbacks:
        def progress_callback(current, total, progress):
            if int(progress * 100) % 10 == 0:  # Every 10%
                print(f"Episode progress: {progress*100:.0f}%")
        
        pynlm.registerProgressCallback(brain, total_steps=episode_length, 
                                      callback=progress_callback)
    
    # Create profiler
    start_time, initial_spikes, initial_neurons = pynlm.createProfiler(brain)
    
    # Run episode
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    print(f"Starting learning episode with {episode_length} steps...")
    
    for step in range(episode_length):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getEnergy()  # Example reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log periodically
        if step % 100 == 0:
            stats = pynlm.trackLearningProgress(brain, step)
            print(f"Step {step}: "
                  f"Spike eff={stats.get('spike_efficiency', 0):.3f}, "
                  f"Dopamine={stats.get('dopamine_level', 0):.3f}, "
                  f"Curiosity={stats.get('curiosity_level', 0):.3f}")
    
    # Generate performance report
    profile, final_time, final_spikes = pynlm.profileStep(
        brain, start_time, initial_spikes, initial_neurons
    )
    
    print(f"\nEpisode completed!")
    print(f"Performance: {profile['spikes_generated']} spikes in {profile['step_time_us']:.0f}μs per step")
    print(f"Final state: {brain.getTotalSpikeCount()} total spikes, "
          f"{brain.getActiveNeuronCount()} active neurons")
    
    return brain, agent, world

# Run the learning agent
brain, agent, world = run_learning_agent(episode_length=2000, enable_callbacks=True)
```

## Configuration Reference

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Migration Guide

### From Basic to Advanced Usage

1. **Enable Context Managers**: Use `with` statements for automatic resource management
2. **Add Progress Callbacks**: Use `registerProgressCallback` for long simulations
3. **Enable Advanced Features**: Configure motor decoding, planning, and learning
4. **Use Batch Operations**: Leverage `batchStep`, `batchSensorInput`, etc. for performance
5. **Monitor Performance**: Use profiling and performance monitoring for optimization
6. **Implement Event Handling**: Use callbacks for custom processing and event handling

### Common Patterns

#### Pattern 1: Real-time Agent Simulation

```python
def run_real_time_agent(world_width=20, world_height=20, 
                       episode_length=1000, enable_learning=True):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(world_width, world_height, 8, 8)
    world.reset()
    agent.initialize(world)
    
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    # Run episode
    for step in range(episode_length):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"neurons={brain.getFiringNeuronCount()}, "
                  f"curiosity={agent.getCuriosityLevel():.2f}")
    
    return brain, agent, world
```

#### Pattern 2: Research Experiment

```python
def run_research_experiment(experiment_type="curiosity_driven", 
                            num_trials=100, steps_per_trial=500):
    """Research experiment with comprehensive monitoring and analysis."""
    
    # Setup with profiling
    start_time, initial_spikes, metrics = pynlm.startPerformanceMonitoring()
    
    # Run experiments
    results = []
    for trial in range(num_trials):
        brain, agent, world = run_real_time_agent(episode_length=steps_per_trial)
        
        # Collect research data
        trial_data = {
            'trial': trial,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_prediction_error': agent.getPredictionError(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        
        results.append(trial_data)
        
        # Log progress
        if (trial + 1) % 10 == 0:
            print(f"Completed {trial + 1}/{num_trials} trials")
    
    # Generate final report
    report = pynlm.stopPerformanceMonitoring(metrics)
    
    # Analyze results
    avg_spikes = sum(r['total_spikes'] for r in results) / len(results)
    print(f"\nResearch Results:")
    print(f"  Average spikes per trial: {avg_spikes:.0f}")
    print(f"  Performance: {report['total_steps']} steps completed")
    
    return results, report
```

#### Pattern 3: Batch Processing

```python
def batch_process_scenarios(scenarios, enable_profiling=True):
    """Process multiple scenarios efficiently using batch operations."""
    
    results = []
    
    for scenario_name, scenario_config in scenarios.items():
        print(f"Processing scenario: {scenario_name}")
        
        # Create brain with scenario configuration
        config = pynlm.createDefaultConfig()
        config.update(scenario_config)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Batch process if supported
        if hasattr(brain, 'batchStep'):
            spike_count = brain.batchStep(1000)
        else:
            # Fallback to loop
            spike_count = 0
            for step in range(1000):
                brain.step(step)
                spike_count += brain.getFiringNeuronCount()
        
        results.append({
            'scenario': scenario_name,
            'spike_count': spike_count,
            'config': scenario_config
        })
        
        # Clean up
        brain.reset()
    
    return results
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: "Brain not responding"

**Cause**: Brain not initialized before use.

**Solution**:
```python
# Wrong
brain.step(0)  # Will fail

# Right
brain.initialize()
brain.step(0)  # Works
```

#### Issue: "Progress callback not called"

**Cause**: Progress callback registered after simulation started.

**Solution**:
```python
# Register callback BEFORE starting simulation
pynlm.registerProgressCallback(brain, total_steps=1000, callback=my_callback)

# Then run simulation
pynlm.runWithProgressCallback(brain, totalSteps=1000, 
                              progressCallback=my_callback)
```

#### Issue: Memory usage high

**Cause**: Large neuron count without cleanup.

**Solution**:
```python
# Use context managers for automatic cleanup
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Do work
    # Brain automatically cleaned up

# Or manually reset
brain.reset()
```

#### Issue: Batch operations not working

**Cause**: Using old API without enhanced features.

**Solution**:
```python
# Check if batch operations are available
if hasattr(pynlm, 'batchStep'):
    # Use enhanced API
    result = pynlm.batchStep(brain, 1000)
else:
    # Use traditional loop
    for step in range(1000):
        brain.step(step)
```

## Next Steps

1. **Study Advanced Motor Control**: Read `AgentBrain Motor Control Guide` for detailed motor planning and learning
2. **Explore Neuromodulation**: Read `Neuromodulation Guide` to understand dopamine, curiosity, and novelty systems
3. **Deepen Brain Understanding**: Read `Brain Architecture Guide` for the refactored brain structure
4. **Create Custom Applications**: Use the patterns above to build your own NLM applications
5. **Performance Optimization**: Use profiling and monitoring to optimize your simulations

## References

- NLM Architecture Documentation: `docs/ARCHITECTURE.md`
- Scientific Background: `docs/SCIENCE.md`
- Experiment Examples: `docs/EXPERIMENTS.md`
- API Design Principles: This document section
- Troubleshooting Guide: This document section

---

*This guide is part of the comprehensive NLM documentation suite. For additional information, refer to related documentation files.*