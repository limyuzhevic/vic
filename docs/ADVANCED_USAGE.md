# NLM Advanced Usage Guide
# Phase 2: Advanced Neural Computation

This document provides comprehensive guidance for using NLM Phase 2 with advanced features and optimizations.

## Introduction

NLM Phase 2 introduces significant improvements over previous phases, including:
- **Advanced Command-line Interface**: Extensive configuration options
- **Performance Monitoring**: Built-in benchmarking and statistics
- **Modular Architecture**: Separated concerns for better maintainability
- **Enhanced Configuration**: More flexible configuration system
- **Performance Optimizations**: Benchmark mode and reduced-precision execution

## Quick Start with Advanced Features

### Example 1: Basic Advanced Usage

```python
import pynlm

# Create configuration with advanced options
config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("brain.neuron_count", 2000)      # Larger brain
config.set("brain.synapse_density", 0.2)   # Higher connectivity
config.set("plasticity.stdp.enable", True)  # Enable STDP
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Example 2: Advanced Simulation with Monitoring

```python
import pynlm
import time

def runAdvancedSimulation(numSteps=5000, enableBenchmark=False):
    """Run simulation with performance monitoring and statistics."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Enable advanced features
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with monitoring
    startTime = time.time()
    
    for step in range(numSteps):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world->getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get and apply motor action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        reward = calculateReward(world.getSensoryPercept())
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 500 == 0:
            logProgress(step, brain, agent, world)
    
    endTime = time.time()
    duration = endTime - startTime
    
    # Generate statistics report
    generateStatisticsReport(brain, agent, duration, numSteps)
    
    return brain, agent, world
```

### Example 3: Command-Line Advanced Usage

```bash
# Run with benchmark mode (faster, reduced precision)
nlm --benchmark --steps 2000 --neurons 1000 --output benchmark_results.json

# Run with verbose output for debugging
nlm --verbose --config custom_config.cfg --steps 5000

# Run quiet mode (errors only)
nlm --quiet --neurons 500

# Run with specific timestep and logging
nlm --timestep 0.01 --log-level DEBUG --steps 1000

# Save performance statistics to file
nlm --steps 3000 --save-stats --output full_stats.txt
```

## Configuration Reference

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 500 | Total number of neurons |
| `brain.synapse_density` | float | 0.15 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.15 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synapse_growth_rate` | float | 0.0001 | Rate of new synapse formation |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction.error_threshold` | float | 0.1 | Prediction error threshold |

## Performance Optimization

### Benchmark Mode

Benchmark mode reduces precision and uses optimizations for faster execution:

```python
# Run in benchmark mode
agent.enableBenchmarkMode(True)
world.enableFastMode(True)

# Features in benchmark mode:
# - Reduced precision floating point operations
# - Simplified plasticity calculations
# - Fewer logging statements
# - Optimized memory access patterns
# - Parallel processing where possible
```

### Command-Line Benchmark Options

```bash
# Fast benchmark run
nlm --benchmark --steps 2000 --neurons 500

# Reduced precision simulation
nlm --benchmark --timestep 0.005 --steps 3000

# Memory-efficient mode
nlm --benchmark --neurons 100 --steps 1000
```

## Advanced Monitoring and Statistics

### Performance Metrics

The system provides comprehensive performance metrics:

```python
# Get performance statistics
stats = brain.getPerformanceStatistics()

# Key metrics:
stats.totalSimulationTime    # Total execution time in seconds
stats.stepsPerSecond         # Computational throughput
stats.spikesPerSecond        # Neural activity rate
stats.efficiencyScore        # Normalized efficiency measure
stats.memoryUsage           # Estimated memory usage in MB
```

### Learning Metrics

Track learning progress and plasticity:

```python
# Get learning statistics
learning = brain.getLearningStatistics()

# Important metrics:
learning.learningDetected     # Whether learning occurred
learning.initialSynapseWeightMean    # Initial average weight
learning.finalSynapseWeightMean      # Final average weight
learning.weightChangeMagnitude       # Total weight change
learning.strengthenedConnections    # Number of strengthened synapses
learning.weakenedConnections        # Number of weakened synapses
learning.synapticPlasticityLevel    # Overall plasticity level
```

### Statistics Report Generation

Generate comprehensive reports:

```python
# Generate full statistics report
brain.generateStatisticsReport("full_stats.txt", "html")

# Generate performance-only report
brain.generateStatisticsReport("perf_summary.txt", "plain")

# Generate learning analysis report
brain.generateStatisticsReport("learning_analysis.txt", "json")
```

## Advanced Agent Configuration

### Custom Agent Behavior

```python
class AdvancedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.setupAdvancedFeatures()
    
    def setupAdvancedFeatures(self):
        # Enable all learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Configure neuromodulation levels
        self.agent.setNeuromodulationLevel("dopamine", 0.5)
        self.agent.setNeuromodulationLevel("curiosity", 0.8)
        self.agent.setNeuromodulationLevel("novelty", 0.3)
    
    def processEnvironmentStep(self, percept, world):
        # Advanced sensory processing
        processedPercept = self.processSensoryInput(percept)
        
        # Neural computation
        self.agent.processSensoryInput(processedPercept)
        
        # Action selection with exploration/exploitation balance
        action = self.selectAction(processedPercept)
        
        # Environment interaction
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning from results
        reward = self.calculateReward(percept, action, world)
        predictedReward = self.predictReward(action)
        
        self.agent.applyRewardModulation(reward, predictedReward)
        
        # Development updates
        self.agent.updateDevelopment(0.1)
        
        return action
```

## Configuration File Management

### Custom Configuration Files

Create custom configuration files for specialized simulations:

```ini
# custom_simulation.cfg
[neuron_settings]
neuron_count = 3000
connection_probability = 0.12
initial_weight_mean = 0.6

[plasticity_settings]
stdp_enable = true
stdp_learning_rate = 0.002
stdp_tau_plus = 25.0
stdp_tau_minus = 22.0

hebbian_enable = true
hebbian_learning_rate = 0.0015

structural_enable = true
synapse_growth_rate = 0.0002

[neuromodulation_settings]
dopamine_scale = 1.2
curiosity_enable = true
novelty_enable = true

[environment_settings]
environment_name = CustomWorld
environment_width = 30
environment_height = 30
vision_width = 10
vision_height = 10
```

### Configuration API

```python
import pynlm

# Load custom configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("custom_simulation.cfg")

# Modify configuration at runtime
config.set("brain.neuron_count", 4000)
config.set("plasticity.stdp.learning_rate", 0.003)

# Save modified configuration
config.saveToFile("modified_config.cfg")
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### Issue: Brain not initializing properly

**Solution:**
```python
# Check configuration
if not brain.initialize():
    print("Brain initialization failed!")
    config = brain.getConfig()
    print(config.summary())
```

#### Issue: Low performance

**Solution:**
```python
# Run in benchmark mode
brain.enableBenchmarkMode(True)

# Check performance statistics
stats = brain.getPerformanceStatistics()
print(f"Steps per second: {stats.stepsPerSecond}")
print(f"Efficiency score: {stats.efficiencyScore}")
```

#### Issue: Memory usage high

**Solution:**
```python
# Reduce neuron count
config.set("brain.neuron_count", 1000)

# Disable unused features
agent.enableStructuralPlasticity(False)
agent.enableDevelopment(False)
agent.enableCuriosity(False)
```

### Debugging Tools

```python
# Enable verbose logging
brain.enableVerboseLogging(True)

# Get detailed status
brain.logStatus()

# Check configuration validity
config.validate()

# Monitor memory usage
brain.monitorMemoryUsage()
```

## Next Steps

1. **Read the Science Documentation** - Learn about the neural computation models
2. **Experiment with Different Configurations** - Find optimal settings for your use case
3. **Benchmark Your System** - Measure performance on your target hardware
4. **Explore Phase 3-6 Features** - Access advanced functionality from other phases
5. **Contribute to the Project** - Share your improvements and optimizations

## Resources

- **GitHub Repository**: https://github.com/nlm-project
- **Documentation**: docs/ARCHITECTURE.md, docs/SCIENCE.md
- **Examples**: examples/ directory
- **Community**: Join the NLM discussion forum

## Version Information

- **NLM Version**: Phase 2.1.0 (Advanced Features)
- **API Version**: 2.0
- **Build Date**: $(date)
- **Compiler**: C++20 with optimizations

This advanced usage guide provides comprehensive documentation for using NLM Phase 2 with all available features and optimizations. For more specific guidance, consult the individual API documentation or project examples.