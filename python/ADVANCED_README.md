# NLM Python Library - Advanced Features Guide

This document provides an overview of the advanced features available in the NLM Python bindings.

## Quick Access to Advanced Features

### Core Advanced API Functions

| Function | Description | Use Case |
|----------|-------------|----------|
| `pynlm.analyzeBrainConnectivity(brain)` | Analyze brain connectivity and provide detailed statistics | Debugging and optimization |
| `pynlm.getPerformanceStats()` | Get real-time performance statistics | Monitoring and optimization |
| `pynlm.runSimulation(brain, steps, callback)` | Run simulation with progress callback | Batch processing and monitoring |
| `pynlm.createAdvancedConfig()` | Create optimized configuration for complex simulations | Research and experimentation |
| `pynlm.validateBrainState(brain)` | Validate brain state and return validation results | Testing and debugging |
| `pynlm.batchProcessBrains(brains, processor)` | Process multiple brains in batch | High-throughput experiments |
| `pynlm.exportBrainState(brain, path)` | Export brain state for checkpointing | Persistence and backup |
| `pynlm.importBrainState(brain, path)` | Import brain state from checkpoint | Resume and recovery |
| `pynlm.analyzeMemoryUsage(brain)` | Analyze brain memory usage distribution | Memory optimization |

### Advanced Usage Examples

#### 1. Brain Analysis and Introspection

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps to populate the brain
for i in range(500):
    brain.step(i)

# Get detailed brain analysis
analysis = pynlm.analyzeBrainConnectivity(brain)
print("Brain Analysis:")
for item in analysis:
    print(f"  {item}")

# Get memory usage information
memory_info = pynlm.analyzeMemoryUsage(brain)
print(f"\nMemory Usage:")
for key, value in memory_info.items():
    print(f"  {key}: {value}")
```

#### 2. Performance Monitoring

```python
import pynlm
import time

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with timing
start_time = time.time()
for i in range(1000):
    brain.step(i)

# Get performance statistics
stats = pynlm.getPerformanceStats()
end_time = time.time()

print(f"Simulation time: {end_time - start_time:.2f} seconds")
print(f"Performance stats: {stats}")
```

#### 3. Batch Processing

```python
import pynlm

# Create multiple brains
brains = []
for i in range(5):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500 + i * 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Define processing function
def processBrain(brain):
    for i in range(100):
        brain.step(i)
    
    firing = brain.getFiringNeuronCount()
    rate = brain.getAverageFiringRate()
    return firing, rate

# Process all brains
results = []
for brain in brains:
    firing, rate = processBrain(brain)
    results.append((firing, rate))

print(f"Processed {len(brains)} brains")
for i, (firing, rate) in enumerate(results):
    print(f"  Brain {i}: {firing} firing, {rate:.2f} Hz")
```

#### 4. Advanced Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Modify for specific experiment
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.002f)
config.set("memory.episodic.capacity", 5000)

# Save configuration
config.saveToFile("my_advanced_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_advanced_config.json")

brain = pynlm.createBrain(config2)
brain.initialize()
```

## Complete Advanced Agent Example

```python
import pynlm

def run_complete_advanced_simulation(num_steps=2000):
    """Run a complete simulation with all advanced features."""
    
    # 1. Create advanced configuration
    config = pynlm.createAdvancedConfig()
    config.set("brain.neuron_count", 2000)
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation with callback
    def simulation_callback(step, time):
        if step % 200 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"  Step {step}: {firing} firing, {rate:.2f} Hz, "
                  f"curiosity={curiosity:.3f}, novelty={novelty:.3f}")
    
    print("Running advanced simulation...")
    pynlm.runSimulation(brain, num_steps, simulation_callback)
    
    # 7. Analyze results
    analysis = pynlm.analyzeBrainConnectivity(brain)
    memory_info = pynlm.analyzeMemoryUsage(brain)
    validation = pynlm.validateBrainState(brain)
    
    return {
        'analysis': analysis,
        'memory_info': memory_info,
        'validation': validation,
        'final_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate()
        }
    }

# Run the simulation
results = run_complete_advanced_simulation(2000)
print(f"\nSimulation complete!")
print(f"Final analysis length: {len(results['analysis'])} items")
print(f"Memory info keys: {list(results['memory_info'].keys())}")
print(f"Validation: {results['validation']}")
```

## Advanced Configuration Reference

The `createAdvancedConfig()` function creates a configuration optimized for complex simulations:

```python
# Default advanced configuration includes:
- 2000 neurons
- 0.15 synapse density
- 0.05 connection probability
- All plasticity systems enabled (STDP, Hebbian, structural)
- Neuromodulation enabled (dopamine, curiosity, novelty)
- Working memory capacity: 100 traces
- Episodic memory capacity: 1000 episodes
- Development enabled with critical period
```

You can modify this configuration for your specific needs:

```python
# Example: High-performance research configuration
config = pynlm.createAdvancedConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.0015f)
config.set("plasticity.hebbian.learning_rate", 0.0005f)
config.set("memory.working.capacity", 500)
config.set("memory.episodic.capacity", 5000)
config.set("development.critical_period_length", 120.0)
```

## Validation Utilities

Use validation functions to ensure system integrity:

```python
import pynlm

# Validate brain configuration
config = pynlm.createDefaultConfig()
if config.has("brain.neuron_count"):
    print("✓ Configuration has neuron count")
else:
    print("✗ Configuration missing neuron count")

# Validate brain state
brain = pynlm.createBrain(config)
brain.initialize()

validation = pynlm.validateBrainState(brain)
print(f"\nBrain validation results:")
for key, is_valid in validation.items():
    print(f"  {key}: {'✓' if is_valid else '✗'}")
```

## Performance Monitoring

Monitor simulation performance with:

```python
import pynlm

# Get performance statistics
stats = pynlm.getPerformanceStats()
print("Performance statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Use callback for custom monitoring
def progress_callback(step, time):
    # Custom monitoring logic here
    pass

pynlm.runSimulation(brain, 1000, progress_callback)
```

## Error Handling and Validation

For robust simulations, use comprehensive error handling:

```python
import pynlm

def safeSimulation(brain, num_steps):
    """Run simulation with error handling."""
    errors = []
    
    for step in range(num_steps):
        try:
            brain.step(step)
        except Exception as e:
            errors.append(f"Step {step}: {str(e)}")
            break
    
    if errors:
        print(f"Simulation completed with {len(errors)} errors")
        for error in errors[:5]:  # Show first 5 errors
            print(f"  {error}")
    
    return len(errors) == 0

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
success = safeSimulation(brain, 1000)
```

## Further Resources

- **Phase 6 Integration Demo**: Run `./build/nlm_phase6_demo` for complete integration testing
- **Phase 6 Integration Experiment**: See `src/experiments/Phase6IntegratedExperiment.cpp` for advanced integration patterns
- **Advanced Usage Guide**: See `python/ADVANCED_USAGE.md` for comprehensive examples
- **Original Documentation**: See `python/easy_usage.md` for basic usage
- **Detailed HOWTO**: See `python/HOW_TO_USE.md` for complete API documentation

For research papers and detailed technical information, see the `docs/` directory.
```
