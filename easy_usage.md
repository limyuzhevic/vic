# NLM - Enhanced Usage Guide

## What's New

This enhanced version of NLM provides:

### 1. Enhanced Configuration System
- Configuration templates for common use cases
- Configuration validation and suggestions
- Parameter sweeps for experiments
- Advanced configuration options

### 2. Advanced Brain Features
- Neural population management
- Regional connectivity control
- Advanced plasticity configuration
- Custom plasticity rules

### 3. Enhanced Agent Behaviors
- Curiosity-driven exploration
- Reward modulation
- Structural plasticity
- Development system integration

### 4. Advanced World Environments
- Dynamic world with moving objects
- Environmental factors
- Complex object interactions
- Dynamic object management

### 5. Visualization and Debugging
- Built-in visualization tools
- Performance monitoring
- Debug output utilities
- State inspection tools

### 6. Resource Management
- Context managers for automatic cleanup
- Memory management utilities
- Batch processing support

### 7. Performance Tools
- Performance monitoring
- Statistical analysis
- Bottleneck detection
- Optimization suggestions

## Quick Start (Enhanced Examples)

### Example 1: Enhanced Brain with Configuration Template

```python
import pynlm

# Create configuration template
config = pynlm.createDefaultConfig()

# Apply template settings
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Example 2: Advanced Agent with Enhanced Behaviors

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(15.0, 15.0)

agent.initialize(world)

# Enable advanced agent behaviors
agent.enableCuriosity(True, curiosity_strength=2.0)
agent.enableRewardModulation(True, reward_decay=0.95)
agent.enableStructuralPlasticity(True, growth_rate=0.001)
agent.enableDevelopment(True, development_rate=0.1)

# Set exploration strategy
agent.setExplorationStrategy("epsilon_greedy", epsilon=0.1)

# Configure curiosity-driven exploration
agent.configureCuriosity(
    novelty_threshold=0.5,
    exploration_bonus=1.0,
    learning_rate=0.01
)

# Run enhanced simulation
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply motor command to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

### Example 3: Neural Population Management

```python
import pynlm

# Create brain with multiple regions
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create neural populations
region_id = brain.addRegion("cortex")

# Create different types of populations
sensory_pop = brain.createPopulation("sensory", pynlm.NeuronType.Sensory, 100)
motor_pop = brain.createPopulation("motor", pynlm.NeuronType.Motor, 50)
excitatory_pop = brain.createPopulation("excitatory", pynlm.NeuronType.Excitatory, 500)
inhibitory_pop = brain.createPopulation("inhibitory", pynlm.NeuronType.Inhibitory, 100)

# Set connectivity between populations
brain.connectRegions(region_id, "sensory", "motor", strength=0.5)
brain.connectRegions(region_id, "excitatory", "inhibitory", strength=0.8)

# Configure synaptic properties
brain.setSynapticProperties("sensory", "motor", {
    "weight_range": [0.1, 0.5],
    "delay": 2,
    "plasticity": True
})

# Get population statistics
stats = brain.getPopulationStats("sensory")
print(f"Sensory population: {stats['neuron_count']} neurons")
print(f"Average firing rate: {stats['average_firing_rate']:.3f}")

# Run simulation with population monitoring
for step in range(200):
    brain.step(step)
    
    if step % 50 == 0:
        sensory_stats = brain.getPopulationStats("sensory")
        motor_stats = brain.getPopulationStats("motor")
        print(f"Step {step}:")
        print(f"  Sensory firing rate: {sensory_stats['average_firing_rate']:.3f}")
        print(f"  Motor firing rate: {motor_stats['average_firing_rate']:.3f}")
```

### Example 4: Advanced Plasticity Control

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable and configure plasticity
brain.enablePlasticity("stdp", True)
brain.setPlasticityParams("stdp", {
    "learning_rate": 0.001,
    "tau_plus": 20.0,
    "tau_minus": 20.0,
    "lambda": 0.5
})

brain.enablePlasticity("hebbian", True)
brain.setPlasticityParams("hebbian", {
    "learning_rate": 0.0005,
    "threshold": 0.1
})

brain.enablePlasticity("structural", True)
brain.setPlasticityParams("structural", {
    "growth_rate": 0.001,
    "pruning_rate": 0.0001
})

# Get current plasticity state
plasticity_state = brain.getPlasticityState()
print(f"STDP enabled: {plasticity_state['stdp']['enabled']}")
print(f"Hebbian enabled: {plasticity_state['hebbian']['enabled']}")
print(f"Structural enabled: {plasticity_state['structural']['enabled']}")

# Add custom plasticity rule
brain.addCustomPlasticityRule("my_rule", 
    condition=lambda synapse: synapse.weight > 0.5,
    update=lambda synapse: synapse.weight *= 0.99)

# Run simulation with plasticity monitoring
for step in range(300):
    brain.step(step)
    
    if step % 50 == 0:
        plastic_state = brain.getPlasticityState()
        print(f"Step {step} - Plasticity state:")
        for rule_type, state in plastic_state.items():
            print(f"  {rule_type}: enabled={state['enabled']}")
```

### Example 5: Dynamic World Environment

```python
import pynlm
import numpy as np

# Create dynamic world
world = pynlm.createDynamicWorld()
world.configure(width=40, height=40, visionWidth=10, visionHeight=10)

# Add moving objects with different properties
world.addMovingObject(5.0, 5.0, pynlm.WorldObjectType.Resource, velocity_x=0.3, velocity_y=0.1)
world.addMovingObject(35.0, 35.0, pynlm.WorldObjectType.Hazard, velocity_x=-0.2, velocity_y=-0.3)
world.addMovingObject(20.0, 10.0, pynlm.WorldObjectType.Wall, velocity_x=0.0, velocity_y=0.0)

# Create dynamic objects with various properties
for i in range(15):
    x = np.random.uniform(5, 35)
    y = np.random.uniform(5, 35)
    obj_type = np.random.choice([
        pynlm.WorldObjectType.Resource,
        pynlm.WorldObjectType.Hazard,
        pynlm.WorldObjectType.Wall,
        pynlm.WorldObjectType.Marker
    ])
    radius = np.random.uniform(0.3, 0.8)
    velocity_x = np.random.uniform(-0.5, 0.5)
    velocity_y = np.random.uniform(-0.5, 0.5)
    
    world.addDynamicObject(x, y, obj_type, radius, dynamic=True, velocity_x=velocity_x, velocity_y=velocity_y)

# Configure environmental factors
world.setEnvironmentalFactor("gravity", 9.8)
world.setEnvironmentalFactor("friction", 0.1)
world.setEnvironmentalFactor("wind", (1.0, 0.5))
world.setEnvironmentalFactor("temperature", 25.0)
world.setEnvironmentalFactor("humidity", 0.7)

# Run dynamic world simulation
for step in range(200):
    world.update(0.1)
    
    # Get world statistics
    stats = world.getWorldStats()
    
    # Update dynamic object positions
    for i in range(min(5, len(stats['dynamic_objects']))):
        world.updateObject(i, stats['dynamic_objects'][i]['x'], stats['dynamic_objects'][i]['y'])
    
    if step % 50 == 0:
        print(f"Step {step} - World stats:")
        print(f"  Objects in world: {stats['object_count']}")
        print(f"  Dynamic objects: {stats['dynamic_object_count']}")
        print(f"  Agent position: ({stats['agent_x']:.1f}, {stats['agent_y']:.1f})")
        print(f"  Simulation time: {stats['simulation_time']:.1f}")
```

### Example 6: Parameter Sweep

```python
import pynlm
from pynlm import ParameterSweep

# Define parameter space for exploration
param_space = {
    "brain.neuron_count": [500, 1000, 2000],
    "brain.synapse_density": [0.05, 0.1, 0.2],
    "plasticity.stdp.learning_rate": [0.0005, 0.001, 0.01],
    "agent.curiosity.enabled": [True, False]
}

# Create parameter sweep
from pynlm import ParameterSweep
sweep = ParameterSweep(param_space)

# Define objective function
def learning_efficiency(params):
    """Objective function: measure learning efficiency."""
    config = pynlm.createDefaultConfig()
    
    # Apply parameters
    for key, value in params.items():
        config.set(key, value)
        
    # Create and run simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(200):
        brain.step(step)
        
    # Calculate efficiency metric (combine spike count and learning)
    total_spikes = brain.getTotalSpikeCount()
    efficiency = total_spikes / max(config.get("brain.neuron_count", 1000), 1)
    
    return efficiency

# Run parameter sweep
best_result = sweep.runSweep(
    objective_function=learning_efficiency,
    max_evaluations=30,
    parallel=False
)

print(f"Best parameters: {best_result['best_parameters']}")
print(f"Best score: {best_result['best_score']:.4f}")

# Visualize results
import matplotlib.pyplot as plt
sweep.plotResults()
```

### Example 7: Performance Monitoring and Analysis

```python
import pynlm
import time

# Create performance monitor
performance_monitor = pynlm.PerformanceMonitor()
performance_monitor.configure(
    monitor_cpu=True,
    monitor_memory=True,
    sampling_rate=0.01
)

# Create simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Start monitoring
performance_monitor.startMonitoring()

# Run performance test
sim_start = time.time()
for step in range(500):
    brain.step(step)
    
    if step % 50 == 0:
        performance_monitor.recordStep(step)

sim_end = time.time()

# Stop monitoring
performance_monitor.stopMonitoring()

# Get performance report
report = performance_monitor.getReport()
print(f"Simulation completed in {sim_end - sim_start:.2f} seconds")
print(f"Performance score: {report['performance_score']:.3f}")
print(f"Memory usage: {report['memory_usage_mb']:.1f} MB")
print(f"CPU utilization: {report['cpu_utilization_percent']:.1f}%")

# Get bottlenecks
bottlenecks = performance_monitor.getBottlenecks()
print(f"Performance bottlenecks: {len(bottlenecks)}")
for bottleneck in bottlenecks:
    print(f"  {bottleneck['area']}: {bottleneck['severity']:.2f} impact")
```

### Example 8: Statistical Analysis

```python
import pynlm
import numpy as np

# Create statistical analyzer
stat_analyzer = pynlm.StatisticalAnalyzer()

# Collect simulation data
simulations = []
for i in range(5):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    spike_counts = []
    for step in range(100):
        brain.step(step)
        spike_counts.append(brain.getTotalSpikeCount())
        
    simulations.append({
        'simulation_id': i,
        'total_spikes': spike_counts[-1],
        'spike_counts': spike_counts,
        'mean_firing_rate': np.mean(spike_counts),
        'std_firing_rate': np.std(spike_counts)
    })

# Convert to DataFrame for analysis
sim_df = pynlm.convertToDataFrame(simulations)

# Perform statistical analysis
stat_results = stat_analyzer.analyzeDataFrame(sim_df)

print("Statistical Analysis Results:")
print(f"Mean total spikes: {stat_results['mean']['total_spikes']:.0f}")
print(f"Std total spikes: {stat_results['std']['total_spikes']:.0f}")
print(f"Skewness: {stat_results['skewness']['total_spikes']:.3f}")
print(f"Kurtosis: {stat_results['kurtosis']['total_spikes']:.3f}")

# Check for outliers
outliers = stat_analyzer.detectOutliers(sim_df['total_spikes'])
print(f"Outliers detected: {len(outliers)}")
for outlier in outliers:
    print(f"  Simulation {outlier['simulation_id']}: {outlier['value']:.0f} (z-score: {outlier['z_score']:.2f})")

# Correlation analysis
correlation = sim_df['mean_firing_rate'].corr(sim_df['std_firing_rate'])
print(f"Firing rate correlation: {correlation:.3f}")
```

## Enhanced Configuration

### Configuration Templates

```python
import pynlm

# Create configuration from template
config = pynlm.createConfigTemplate("agent_training")
config.set("brain.neuron_count", 1500)
config.set("agent.reward_shaping", 0.1)
config.set("agent.curiosity.enabled", True)
config.set("world.environment.type", "dynamic")

# Validate configuration
is_valid, errors = config.validate()
if not is_valid:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid!")

# Get configuration suggestions
suggestions = config.getSuggestions()
for suggestion in suggestions:
    print(f"Suggestion: {suggestion}")
```

### Parameter Sweeps

```python
import pynlm
from pynlm import ParameterSweep

# Create parameter sweep for hyperparameter optimization
sweep = ParameterSweep()
sweep.addParameter("brain.neuron_count", [100, 500, 1000])
sweep.addParameter("brain.synapse_density", [0.05, 0.1, 0.2])
sweep.addContinuousParameter("plasticity.stdp.learning_rate", 0.0001, 0.1, 10)

# Set objective function
def objective_function(params):
    # Implementation of your objective function
    pass

sweep.setObjectiveFunction(objective_function)

# Run sweep
results = sweep.runSweep(steps_per_parameter=100)

# Save results
sweep.saveResults("sweep_results.json")

# Get best parameters
best_params = sweep.getBestParameters()
print(f"Best parameters: {best_params}")
```

## Advanced Usage

### Resource Management with Context Managers

```python
import pynlm

# Use context manager for automatic resource cleanup
with pynlm.SimulationContext() as sim:
    # Create and configure brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createDynamicWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    # Create and configure agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Context manager automatically cleans up resources
print("Simulation completed and resources cleaned up")
```

### Batch Processing

```python
import pynlm
import numpy as np

def run_multiple_simulations(configs, num_steps=100):
    """Run multiple simulations with resource management."""
    results = []
    
    with pynlm.BatchSimulationContext() as batch:
        for i, config in enumerate(configs):
            print(f"Running simulation {i+1}/{len(configs)}")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(num_steps):
                brain.step(step)
                
            results.append({
                'config': config,
                'final_spike_count': brain.getTotalSpikeCount(),
                'final_firing_rate': brain.getAverageFiringRate()
            })
            
            batch.recordProgress(i + 1, len(configs))
            
    return results

# Example usage
configs = []
for neuron_count in [100, 500, 1000]:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    configs.append(config)

results = run_multiple_simulations(configs)
for result in results:
    print(f"Neuron count: {result['config'].get('brain.neuron_count')}, "
          f"Final spikes: {result['final_spike_count']}")
```

## Key Enhancements

### 1. Neural Population Management
- Create and manage neural populations with shared properties
- Configure connectivity between populations
- Monitor population statistics

### 2. Advanced Plasticity Control
- Enable/disable plasticity rules
- Configure rule parameters
- Add custom plasticity rules
- Monitor plasticity state

### 3. Enhanced Agent Behaviors
- Curiosity-driven exploration
- Reward modulation with shaping
- Structural plasticity
- Development system integration
- Exploration strategies

### 4. Dynamic World Environments
- Moving objects with physics
- Environmental factors (gravity, wind, etc.)
- Dynamic object management
- World object interactions

### 5. Visualization and Debugging
- Built-in visualization tools
- Performance monitoring
- Debug output utilities
- State inspection tools

### 6. Resource Management
- Context managers for automatic cleanup
- Memory management utilities
- Batch processing support

### 7. Performance Tools
- Performance monitoring
- Statistical analysis
- Bottleneck detection
- Optimization suggestions

## Conclusion

The enhanced NLM Python bindings provide:

- **Better Developer Experience**: More Pythonic API with convenience methods
- **Advanced Features**: Neural populations, plasticity control, dynamic worlds
- **Resource Management**: Automatic cleanup with context managers
- **Performance Tools**: Monitoring, analysis, optimization
- **Backward Compatibility**: All existing functionality preserved

These enhancements make NLM more powerful for both beginners and advanced users, enabling complex neural simulations with ease.

For more detailed examples and advanced usage patterns, refer to the original `HOW_TO_USE.md` documentation.

---

**Quick Reference**

| Feature | New? | Description |
|---------|------|-------------|
| Configuration Templates | ✅ | Pre-configured settings for common use cases |
| Neural Populations | ✅ | Manage groups of neurons with shared properties |
| Advanced Plasticity | ✅ | Fine-grained control over learning mechanisms |
| Dynamic Worlds | ✅ | Moving objects and environmental factors |
| Curiosity-Driven Agents | ✅ | Intelligent exploration and learning |
| Performance Monitoring | ✅ | Track and optimize simulation performance |
| Context Managers | ✅ | Automatic resource cleanup |
| Parameter Sweeps | ✅ | Automated experiment optimization |
| Statistical Analysis | ✅ | Built-in statistical tools |
| Visualization Tools | ✅ | Real-time visualization and debugging |
