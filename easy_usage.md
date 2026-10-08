# NLM - Enhanced Usage Guide

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that simulates neural systems using spiking neural networks. This enhanced edition provides:

- **Phase-based brain simulation** with expert control
- **Advanced configuration management** for research
- **Visualization and analysis tools** for neuroscience
- **Performance monitoring and profiling**
- **Batch simulation for research experiments**
- **Neural topology analysis**
- **Memory management utilities**

## Quick Start (Enhanced)

### Example 1: Basic Enhanced Brain Simulation

```python
import pynlm

# Create brain with advanced configuration
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Run simulation with detailed monitoring
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
              f"{brain.getTotalSpikeCount()} total spikes")

print("Simulation complete!")
```

### Example 2: Complete Research Experiment

```python
import pynlm

# Setup advanced experiment configuration
config = pynlm.createAdvancedConfig()
config.setupStandardResearchConfig()  # Pre-configured for research

# Create simulation components
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)  # 20x20 world with 8x8 vision
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features for research
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run detailed experiment with custom callback
metrics = []
def recordStep(step, reward, percept):
    metrics.append({
        'step': step,
        'reward': reward,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'development_stage': agent.getDevelopmentalStage(),
        'dopamine': agent.getNeuromodulationLevel(),
        'curiosity': agent.getCuriosityLevel()
    })

print("Running research experiment...")
pynlm.runEpisode(brain, world, agent, 1000, recordStep, True)

# Analyze results
print(f"Experiment completed with {len(metrics)} recorded steps")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Peak development stage: {max(m['development_stage'] for m in metrics)}")
```

### Example 3: Batch Processing for Large-Scale Research

```python
import pynlm

# Run multiple experiments in batch
print("Starting batch simulation...")

def progressCallback(episode, total):
    print(f"Episode {episode}/{total} completed")

# Configuration for batch processing
batch_config = pynlm.createAdvancedConfig()
batch_config.setupHighPerformanceConfig()  # Optimized for speed

# Run 20 episodes of 500 steps each
pynlm.runBatchSimulation(
    numEpisodes=20,
    stepsPerEpisode=500,
    config=batch_config.getConfig(),
    progressCallback=progressCallback
)

print("Batch simulation complete!")
```

### Example 4: Neural Topology Analysis

```python
import pynlm

# Create and simulate a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add some activity
for i in range(200):
    brain.step(i)

# Generate detailed connectivity analysis
print("=== Neural Topology Analysis ===")
topology_report = pynlm.generateNeuralTopologyReport(brain)
print(topology_report)

# Save connectivity map for external tools
pynlm.saveNeuralConnectivityMap(brain, "brain_connectivity.json")
print("\nConnectivity map saved to 'brain_connectivity.json'")
```

### Example 5: Performance Profiling

```python
import pynlm

# Create brain for profiling
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

print("Profiling simulation performance...")
pynlm.profileSimulation(brain, 1000, "simulation_profile.txt")

print("Profile saved to 'simulation_profile.txt'")
```

## Advanced Configuration

### 1. Basic vs Advanced Configuration

```python
# Basic configuration (simple setup)
basic_config = pynlm.createDefaultConfig()

# Advanced configuration (expert control)
advanced_config = pynlm.createAdvancedConfig()

# Use advanced configuration for research
brain = pynlm.createBrain(advanced_config)
```

### 2. Configuration Presets

```python
# Setup different research configurations
config = pynlm.createAdvancedConfig()

# Research configuration (standard for neuroscience)
config.setupStandardResearchConfig()

# High-performance configuration (speed over features)
config.setupHighPerformanceConfig()

# Developmental configuration (studies brain development)
config.setupDevelopmentalConfig()
```

## Key Classes and Methods

### Brain Class

| Method | Description | Example |
|--------|-------------|---------|
| `initialize()` | Initialize brain with configuration | `brain.initialize()` |
| `step(step)` | Perform one simulation step | `brain.step(i)` |
| `step(step, time)` | Perform step with specific timestamp | `brain.step(i, t)` |
| `receiveSensoryInput(input)` | Inject sensory data | `brain.receiveSensoryInput(percept)` |
| `injectCurrent(neuron, current)` | Inject current into neuron | `brain.injectCurrent(neuron_id, 5.0)` |
| `save(filepath)` | Save brain state | `brain.save('brain_state.json')` |
| `load(filepath)` | Load brain state | `brain.load('brain_state.json')` |
| `logStatus()` | Print brain statistics | `brain.logStatus()` |

### AgentBrain Class

| Method | Description | Example |
|--------|-------------|---------|
| `initialize(world)` | Initialize with world | `agent.initialize(world)` |
| `processSensoryInput(percept)` | Process sensory input | `agent.processSensoryInput(percept)` |
| `decodeMotorCommand()` | Decode action from neural activity | `action = agent.decodeMotorCommand()` |
| `applyRewardModulation(reward, predicted)` | Apply reward-based learning | `agent.applyRewardModulation(1.0, 0.5)` |
| `updateDevelopment(timestep)` | Update developmental processes | `agent.updateDevelopment(0.1)` |
| `getNeuromodulationLevel()` | Get current neuromodulation | `dopamine = agent.getNeuromodulationLevel()` |
| `getCuriosityLevel()` | Get curiosity level | `curiosity = agent.getCuriosityLevel()` |

### SimpleWorld Class

| Method | Description | Example |
|--------|-------------|---------|
| `configure(width, height, vision_w, vision_h)` | Setup world dimensions | `world.configure(20, 20, 8, 8)` |
| `reset()` | Reset world state | `world.reset()` |
| `update(timestep)` | Advance simulation | `world.update(0.1)` |
| `applyMotorCommand(cmd, time)` | Apply action to world | `world.applyMotorCommand(action, time)` |
| `getSensoryPercept()` | Get current sensory data | `percept = world.getSensoryPercept()` |
| `getAgentBody()` | Get agent physical state | `body = world.getAgentBody()` |

## Advanced Features

### 1. Experimental Control

```python
# Run predefined neuroscience experiments
experiments = pynlm.getAvailableExperiments()
for exp in experiments:
    print(f"Available: {exp}")

# Run specific experiment with custom parameters
brain, world, agent, results = pynlm.runExperiment("plasticity", config, 1000)
```

### 2. Callback System

```python
def myCallback(step, reward, percept):
    print(f"Step {step}: Reward {reward:.2f}")
    # Custom processing or logging

# Run episode with custom callback
pynlm.runEpisode(brain, world, agent, 500, myCallback)
```

### 3. System Information

```python
# Get version and build information
version = pynlm.getVersionInfo()
print(version)

# Get detailed system information
system_info = pynlm.getSystemInfo()
print(f"Version: {system_info['version']}")
print(f"Build type: {system_info['build_type']}")
```

## Usage Patterns

### Pattern 1: Continuous Learning

```python
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(15, 15, 6, 6)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Continuous learning loop
for episode in range(10):
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # End of episode: reset for new episode
    agent.reset()
    
    if episode % 5 == 0:
        print(f"Episode {episode} complete. Firing rate: {brain.getAverageFiringRate():.2f}")
```

### Pattern 2: Batch Research

```python
import pynlm

# Configuration for research
research_config = pynlm.createAdvancedConfig()
research_config.setupStandardResearchConfig()

# Define experiment parameters
experiments = [
    {"name": "plasticity_study", "steps": 1000, "episodes": 10},
    {"name": "development_study", "steps": 1500, "episodes": 5},
    {"name": "curiosity_study", "steps": 800, "episodes": 15}
]

# Run multiple experiments
all_results = []
for exp in experiments:
    print(f"Running {exp['name']}...")
    
    config = pynlm.createAdvancedConfig()
    config.setupStandardResearchConfig()
    
    # Use advanced batch processing
    pynlm.runBatchSimulation(
        numEpisodes=exp['episodes'],
        stepsPerEpisode=exp['steps'],
        config=config.getConfig(),
        progressCallback=lambda e, t: print(f"  Episode {e}/{t}")
    )
    
    all_results.append(f"Completed {exp['name']}")
```

### Pattern 3: Neural Analysis

```python
import pynlm

# Create and run simulation
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Add diverse neural activity
for i in range(500):
    brain.step(i)
    if i % 100 == 0:
        print(f"Progress: {i}/500 steps")

# Comprehensive neural analysis
print("\n=== Neural Analysis ===")

# 1. Save connectivity map for visualization
pynlm.saveNeuralConnectivityMap(brain, "analysis_connectivity.json")

# 2. Generate detailed topology report
report = pynlm.generateNeuralTopologyReport(brain)
print(report)

# 3. Performance profiling
pynlm.profileSimulation(brain, 500, "neural_analysis_profile.txt")

# 4. Get system information
system_info = pynlm.getSystemInfo()
print(f"Analysis completed with system: {system_info}")
```

## Troubleshooting

### "Brain isn't doing anything"

**Solutions:**
1. Always call `brain.initialize()` before using the brain
2. Ensure simulation steps are being called
3. Check if sensory input is being connected properly
4. Verify configuration parameters

```python
# Correct usage
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()  # Must call this!

for i in range(100):
    brain.step(i)  # Now the brain will process
```

### "Agent isn't moving"

**Solutions:**
1. Call `world.applyMotorCommand()` to execute actions
2. Ensure `world.update()` is being called
3. Check agent motor neuron distribution
4. Verify action decoding is working

```python
# Correct agent-world interaction
for step in range(100):
    world.update(0.1)  # Update world state
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)  # Brain computation
    action = agent.decodeMotorCommand()  # Agent decision
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute action!
```

### "Everything is 0 or showing no activity"

**Solutions:**
1. Brains need time to "warm up" - try more steps (200-500 recommended)
2. Ensure sensory input connections are active
3. Check neuron parameters are reasonable
4. Verify plasticity is enabled if expected learning

```python
# Give brain time to develop activity
for i in range(500):  # More steps for warmup
    brain.step(i)
    if i % 100 == 0:
        firing = brain.getFiringNeuronCount()
        if firing > 0:
            print(f"Activity detected at step {i}: {firing} neurons firing")
```

### Performance Issues

**Solutions:**
1. Use `pynlm.createAdvancedConfig().setupHighPerformanceConfig()` for speed
2. Reduce world size or vision resolution
3. Use batch processing instead of single episodes
4. Profile with `pynlm.profileSimulation()`

```python
# For better performance
config = pynlm.createAdvancedConfig()
config.setupHighPerformanceConfig()
```

## Best Practices

### 1. Configuration Management

```python
# Use appropriate configuration for your use case
if "research" in sys.argv:
    config = pynlm.createAdvancedConfig()
    config.setupStandardResearchConfig()
elif "performance" in sys.argv:
    config = pynlm.createAdvancedConfig()
    config.setupHighPerformanceConfig()
else:
    config = pynlm.createAdvancedConfig()
```

### 2. Memory Management

```python
# Use explicit cleanup for large simulations
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

# Cleanup (Python handles this automatically, but good practice)
del brain
```

### 3. Error Handling

```python
import pynlm
import traceback

try:
    brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
    traceback.print_exc()
except Exception as e:
    print(f"Unexpected error: {e}")
    traceback.print_exc()
```

### 4. Progress Monitoring

```python
import pynlm

def createProgressTracker(total_steps):
    def tracker(step, reward, percept):
        if step % 100 == 0:
            elapsed = pynlm.currentTime()
            rate = (step / elapsed * 1000) if elapsed > 0 else 0
            print(f"Progress: {step}/{total_steps} steps, "
                  f"Rate: {rate:.1f} steps/sec, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}")
    return tracker

brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

tracker = createProgressTracker(1000)
pynlm.runEpisode(brain, world, agent, 1000, tracker)
```

## API Reference Summary

### Core Functions

- `pynlm.createDefaultConfig()` - Basic configuration
- `pynlm.createAdvancedConfig()` - Advanced configuration
- `pynlm.createBrain(config)` - Create brain instance
- `pynlm.createSimpleWorld()` - Create simulation world
- `pynlm.createAgentBrain(brain)` - Create agent interface

### Simulation Control

- `pynlm.runEpisode()` - Run single simulation episode
- `pynlm.runBatchSimulation()` - Run multiple episodes
- `pynlm.profileSimulation()` - Performance profiling

### Analysis and Visualization

- `pynlm.saveNeuralConnectivityMap()` - Save connectivity data
- `pynlm.generateNeuralTopologyReport()` - Generate analysis report
- `pynlm.showExamples()` - Display usage examples

### System Information

- `pynlm.getVersionInfo()` - Get version information
- `pynlm.getSystemInfo()` - Get system details
- `pynlm.getAvailableExperiments()` - Get available experiment types

## Next Steps

When you're comfortable with the enhanced NLM API:

1. **Explore Experiment Types**: Try different neuroscience experiments
2. **Custom Callbacks**: Implement your own monitoring and analysis
3. **Performance Optimization**: Profile and optimize your simulations
4. **Advanced Configuration**: Master the advanced configuration options
5. **Batch Processing**: Run large-scale research experiments efficiently
6. **Neural Analysis**: Deep dive into connectivity and topology analysis
7. **Integration**: Integrate NLM with other neuroscience tools

The enhanced NLM Python bindings provide everything you need for sophisticated neural simulation research, from simple demos to large-scale experimental studies.

## Examples Directory

For additional examples and templates, see:
- `examples/basic/` - Basic simulation examples
- `examples/research/` - Research experiment templates
- `examples/advanced/` - Advanced usage patterns
- `examples/batch/` - Batch processing examples
- `examples/analysis/` - Neural analysis examples

These examples demonstrate best practices and provide starting points for your own NLM projects.
