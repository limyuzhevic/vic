# NLM - Advanced User Guide

**For experienced users and developers**

## Advanced Configuration

### Custom Configuration File

Instead of using the default configuration, you can create your own config file:

```python
import pynlm

# Create custom config
config = pynlm.createDefaultConfig()

# Override settings
config.set("neuron_count", 2000)
config.set("connection_probability", 0.05)
config.set("stdp_ltp_weight", 0.05)
config.set("stdp_ltd_weight", 0.04)

# Save to file (optional)
# config.save("my_config.json")

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Performance Optimization

For high-performance simulations, consider these optimization techniques:

```python
import pynlm
import time

# Create optimized configuration
def createOptimizedConfig(neuronCount, connectionProb=0.1):
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuronCount)
    config.set("region_count", 2)  # Multiple regions for better performance
    config.set("connection_probability", connectionProb)
    
    # Optimization settings
    config.set("simulation_timestep", 0.0005)  # Smaller timestep for accuracy
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    
    return config

# Create large brain
config = createOptimizedConfig(5000, 0.08)
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
startTime = time.time()
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes, "
              f"{brain.getFiringNeuronCount()} firing neurons")

endTime = time.time()
print(f"Performance: {1000/(endTime-startTime):.0f} steps/sec")
```

### Multi-threaded Simulation

While NLM is designed for single-threaded execution for reproducibility,
you can run multiple simulations in parallel:

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

def runSimulation(neuronCount, seed):
    """Run a single simulation with given parameters"""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuronCount)
    config.set("random_seed", seed)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    return {
        "neurons": neuronCount,
        "spikes": brain.getTotalSpikeCount(),
        "firing": brain.getFiringNeuronCount(),
        "seed": seed
    }

# Run multiple simulations in parallel
neuronCounts = [100, 500, 1000, 2000]
seeds = [42, 123, 456, 789]

with ThreadPoolExecutor(max_workers=4) as executor:
    futures = [
        executor.submit(runSimulation, neurons, seed)
        for neurons, seed in zip(neuronCounts, seeds)
    ]
    
    results = [future.result() for future in futures]
    
    for result in results:
        print(f"Neuron count: {result['neurons']}, "
              f"Spikes: {result['spikes']}, "
              f"Firing: {result['firing']}%")
```

## Advanced Brain Features

### Custom Neuron Types

You can extend the simulation with custom neuron types by subclassing:

```python
import pynlm

# Note: This is a conceptual example - actual implementation depends on NLM's architecture

class CustomNeuron:
    def __init__(self, neuron_id):
        self.id = neuron_id
        self.customProperty = 0.0
    
    def processInput(self, inputValue):
        # Custom processing logic
        self.customProperty = inputValue * 0.5
        return self.customProperty

# Usage (conceptual)
# customNeuron = CustomNeuron(pynlm.NeuronId(1))
```

### Advanced Plasticity Rules

Create custom plasticity rules by extending existing rules:

```python
import pynlm

class CustomPlasticityRule:
    def __init__(self, learningRate=0.01):
        self.learningRate = learningRate
        self.stdpWeight = 0.0
    
    def updateWeight(self, preSpikeTime, postSpikeTime, currentWeight):
        # Custom STDP implementation
        timeDiff = postSpikeTime - preSpikeTime
        
        if timeDiff > 0:  # Post after pre
            self.stdpWeight += self.learningRate * timeDiff
        else:  # Pre after post
            self.stdpWeight -= self.learningRate * abs(timeDiff)
        
        return min(max(currentWeight + self.stdpWeight, 0.0), 1.0)

# Usage (conceptual)
# rule = CustomPlasticityRule(learningRate=0.02)
# newWeight = rule.updateWeight(preTime, postTime, currentWeight)
```

### Checkpoint and Serialization

Save and restore brain state for long-running experiments:

```python
import pynlm
import os

def saveBrainState(brain, filename):
    """Save brain state to file"""
    if not os.path.exists("checkpoints"):
        os.makedirs("checkpoints")
    
    # Note: Implementation depends on NLM's checkpoint system
    # This is a conceptual example
    brain.save(f"checkpoints/{filename}.bin")
    print(f"Brain state saved to checkpoints/{filename}.bin")

def loadBrainState(filename, config):
    """Load brain state from file"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Note: Implementation depends on NLM's checkpoint system
    # This is a conceptual example
    if brain.load(f"checkpoints/{filename}.bin"):
        print(f"Brain state loaded from checkpoints/{filename}.bin")
        return brain
    else:
        print(f"Failed to load brain state")
        return None

# Usage example
config = pynlm.createDefaultConfig()

# After running simulation for a while
saveBrainState(brain, "experiment_1")

# Later...
restoredBrain = loadBrainState("experiment_1", config)
```

## Advanced World Integration

### Custom Worlds

Create custom environments by extending the SimpleWorld:

```python
import pynlm

class CustomWorld:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.data = [[0.0 for _ in range(width)] for _ in range(height)]
        self.time = 0.0
    
    def update(self, dt):
        """Update world state"""
        self.time += dt
        # Custom world dynamics
        self.updateDynamics(dt)
    
    def updateDynamics(self, dt):
        """Implement custom world dynamics"""
        # Example: simple diffusion
        for y in range(1, self.height - 1):
            for x in range(1, self.width - 1):
                self.data[y][x] = (
                    self.data[y-1][x] * 0.25 +
                    self.data[y+1][x] * 0.25 +
                    self.data[y][x-1] * 0.25 +
                    self.data[y][x+1] * 0.25
                ) * 0.9
    
    def getSensoryPercept(self):
        """Get sensory input from world"""
        # Convert world state to sensory input
        percept = []
        for y in range(self.height):
            for x in range(self.width):
                percept.append(self.data[y][x])
        return percept

# Usage (conceptual)
# world = CustomWorld(20, 20)
```

## Build System Integration

### Building with Different Build Tools

#### Using CMake Directly

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

#### Using Ninja (Faster)

```bash
mkdir build
cd build
cmake .. -G Ninja -DNLM_BUILD_PYTHON_BINDINGS=ON
ninja
```

#### Using scikit-build (for Python bindings)

```bash
pip install -e .
```

### Cross-compilation

For building on different platforms:

```bash
mkdir build
cd build
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake \
    -DNLM_BUILD_PYTHON_BINDINGS=OFF
```

## Debugging and Troubleshooting

### Common Issues and Solutions

#### "ImportError: cannot import name 'pynlm'"

**Cause**: Python bindings not built correctly.

**Solution**:
```bash
# Ensure Python bindings are built
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make

# Or install via pip
pip install -e .
```

#### "Brain initialization failed"

**Cause**: Configuration error or insufficient memory.

**Solution**:
```python
import pynlm

# Create smaller configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Start small
config.set("region_count", 1)

brain = pynlm.createBrain(config)
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Brain initialization failed - check configuration")
```

#### "Performance too low"

**Cause**: Suboptimal configuration or too many neurons.

**Solutions**:
1. Use smaller timesteps: `config.set("simulation_timestep", 0.001)`
2. Reduce neuron count for testing
3. Use fewer regions
4. Enable optimizations if available

```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)  # Reasonable starting point
config.set("simulation_timestep", 0.001)
config.set("region_count", 2)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Debug Output

Enable verbose logging for debugging:

```python
import pynlm

# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run with debug output
for step in range(10):
    brain.step(step)
    # Debug output will be printed
```

## Advanced APIs

### Direct Brain Access

Access brain components directly for advanced control:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access internal components (if exposed by NLM)
# Note: Implementation depends on NLM's API
brain.getSpikeSystem()
brain.getSTDP()
brain.getHebbian()
brain.getStructuralPlasticity()
```

### Configuration Management

Manage configuration files and settings:

```python
import pynlm
import json
import os

class ConfigManager:
    def __init__(self, configDir="configs/"):
        self.configDir = configDir
        if not os.path.exists(configDir):
            os.makedirs(configDir)
    
    def saveConfig(self, config, name):
        """Save config to JSON file"""
        # Note: Implementation depends on NLM's config save method
        pass
    
    def loadConfig(self, name):
        """Load config from JSON file"""
        # Note: Implementation depends on NLM's config load method
        pass

# Usage
manager = ConfigManager()
```

## Performance Monitoring

### Track Simulation Metrics

```python
import pynlm
import time
import statistics

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.stepTimes = []
        self.spikeCounts = []
        self.firingRates = []
    
    def recordStep(self, step):
        """Record metrics for a single step"""
        startTime = time.time()
        self.brain.step(step)
        endTime = time.time()
        
        self.stepTimes.append(endTime - startTime)
        self.spikeCounts.append(self.brain.getTotalSpikeCount())
        self.firingRates.append(self.brain.getAverageFiringRate())
    
    def getStats(self):
        """Get performance statistics"""
        if not self.stepTimes:
            return {}
        
        return {
            "steps_per_second": len(self.stepTimes) / sum(self.stepTimes),
            "avg_step_time": statistics.mean(self.stepTimes),
            "max_step_time": max(self.stepTimes),
            "min_step_time": min(self.stepTimes),
            "total_spikes": self.spikeCounts[-1],
            "avg_firing_rate": statistics.mean(self.firingRates),
            "spike_count_std": statistics.stdev(self.spikeCounts) if len(self.spikeCounts) > 1 else 0.0
        }

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

monitor = PerformanceMonitor(brain)
for step in range(100):
    monitor.recordStep(step)

stats = monitor.getStats()
print(f"Performance: {stats['steps_per_second']:.0f} steps/sec")
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
```

## Integration with Other Tools

### Using with Visualization Tools

Export brain state for visualization:

```python
import pynlm

# Export neuron data for visualization
def exportForVisualization(brain, outputFile):
    """Export brain state for visualization tools"""
    # Note: Implementation depends on NLM's export capabilities
    pass

# Usage (conceptual)
# exportForVisualization(brain, "brain_state.json")
```

### Using with Analysis Tools

Analyze brain dynamics:

```python
import pynlm
import numpy as np
from scipy import signal

def analyzeBrainDynamics(brain, steps):
    """Analyze brain dynamics over time"""
    spikeCounts = []
    firingRates = []
    
    for step in range(steps):
        brain.step(step)
        spikeCounts.append(brain.getTotalSpikeCount())
        firingRates.append(brain.getAverageFiringRate())
    
    # Convert to numpy arrays for analysis
    spikeCounts = np.array(spikeCounts)
    firingRates = np.array(firingRates)
    
    # Analyze patterns
    results = {
        "spike_rate": spikeCounts,
        "firing_rate": firingRates,
        "mean_spike_count": np.mean(spikeCounts),
        "std_spike_count": np.std(spikeCounts),
        "mean_firing_rate": np.mean(firingRates),
        "std_firing_rate": np.std(firingRates)
    }
    
    return results

# Usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

analysis = analyzeBrainDynamics(brain, 1000)
print(f"Average firing rate: {analysis['mean_firing_rate']:.2f} Hz")
print(f"Spike count std: {analysis['std_spike_count']:.0f}")
```

## Next Steps

1. **Master the basics**: Start with `easy_usage.md` and `HOW_TO_USE.md`
2. **Explore the API**: Study the Python bindings using `pynlm` module
3. **Build for performance**: Use Ninja build system and optimize configuration
4. **Create custom simulations**: Extend NLM with custom world and neuron types
5. **Contribute**: If you have improvements, consider contributing to the project

This advanced guide provides a foundation for experienced users to get the most out of NLM. The actual API surface depends on the specific implementation of the NLM framework.
