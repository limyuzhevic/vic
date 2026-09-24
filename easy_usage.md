# NLM - Enhanced Usage Guide for Beginners and Advanced Users

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain system. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

## What Makes NLM Better

### ✅ Advanced Configuration Management
- **JSON Support**: Load/save configuration files in modern JSON format
- **Command-line configuration**: Override settings via command line
- **Configuration validation**: Check config files for errors before use
- **Default configuration templates**: Create standardized configs for experiments

### ✅ Enhanced Command-Line Interface
- **Multiple modes**: Demo, debug, performance, interactive, experiment
- **Advanced commands**: Config management, monitoring, profiling, export/import
- **Flexible argument parsing**: Clean and intuitive command structure
- **Help system**: Comprehensive command documentation

### ✅ Better Integration
- **Working memory**: Neural-based memory that maintains information through sustained activity
- **Episodic memory**: Story-like memory of experiences with replay mechanisms
- **Attention system**: Neural attention for selective focus
- **Concept formation**: Automatic categorization from experience

### ✅ Performance Monitoring
- **Real-time monitoring**: Track brain activity, memory usage, performance
- **Performance profiling**: Measure and optimize execution speed
- **Memory profiling**: Monitor memory usage and allocation
- **Debug tools**: Enhanced debugging with detailed diagnostics

## Getting Started

### 1. Install NLM

```bash
# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run basic demo
./nlm demo
```

### 2. Create Configuration

```bash
# Create a default configuration file
./nlm create-config configs/default.cfg

# Or create JSON configuration
./nlm create-config configs/experiment.json --json-config
```

### 3. Run Basic Demo

```python
import nlm

# Simple demonstration
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

print(f"Brain has {brain.getTotalNeuronCount()} neurons!")

# Run simulation
for i in range(100):
    brain.step(i)
```

## Advanced Usage Examples

### Running Different Modes

```bash
# Demo mode - comprehensive integration tests
./nlm demo

# Debug mode - detailed diagnostics
./nlm debug --verbose

# Performance mode - benchmarks and profiling
./nlm performance --max-steps=5000

# Custom experiment - Phase 6 integration
./nlm experiment --enable-checkpoints --enable-development

# Interactive mode - real-time monitoring
./nlm interactive
```

### Configuration Management

```bash
# Create config file
./nlm create-config myconfig.json --json-config

# Validate config file
./nlm validate-config myconfig.json

# Modify config with CLI arguments
./nlm demo --config=myconfig.json --seed=12345 --max-steps=2000
```

### Performance Monitoring

```bash
# Run performance profiler
./nlm profile 1000

# Start system monitor
./nlm monitor

# Show system statistics
./nlm stats
```

### Export and Import Brain State

```bash
# Export brain state for later use
./nlm export saved_brain_state.json

# Import saved brain state
./nlm import saved_brain_state.json
```

## Advanced Python API Examples

### Using Advanced Configuration

```python
import nlm

# Create advanced configuration
config = nlm.createDefaultConfig()

# Modify configuration programmatically
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("learning.rate", 0.001)
config.set("development.enabled", True)
config.set("curiosity.enabled", True)
config.set("reward.modulation", True)

# Save configuration
config.saveToFile("advanced_config.json")

# Create brain with advanced config
brain = nlm.createBrain(config)
brain.initialize()
```

### Memory Integration Example

```python
import nlm

# Create brain with memory systems
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Check if memory systems are integrated
if brain.getWorkingMemory():
    print("Working memory is integrated")

if brain.getEpisodicMemory():
    print("Episodic memory is integrated")

if brain.getPredictionSystem():
    print("Prediction system is integrated")

# Run simulation with memory tracking
for step in range(100):
    brain.step(step)
    
    # Check memory usage every 10 steps
    if step % 10 == 0:
        wm = brain.getWorkingMemory()
        if wm:
            print(f"Step {step}: Working memory traces = {wm.getActiveTraces()}")
        
        em = brain.getEpisodicMemory()
        if em:
            print(f"Step {step}: Episodic memory episodes = {em.getEpisodeCount()}")
```

### Performance Monitoring Example

```python
import nlm
import time

# Create performance monitor
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

brain = nlm.createBrain(config)
brain.initialize()

print("Starting performance benchmark...")
start_time = time.time()

# Run simulation and collect performance data
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {step/elapsed:.1f} steps/sec, "
              f"firing neurons: {brain.getFiringNeuronCount()}, "
              f"avg firing rate: {brain.getAverageFiringRate():.2f}")

end_time = time.time()

print(f"\nPerformance Results:")
print(f"Total time: {end_time - start_time:.2f}s")
print(f"Performance: {1000/(end_time - start_time):.1f} steps/sec")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Debug Mode Example

```bash
# Enable debug mode with verbose output
./nlm debug --verbose

# Debug with custom configuration
./nlm debug --config=debug_config.json --log-level=debug

# Debug with specific monitoring
./nlm debug --enable-checkpoints
```

### Experiment Configuration Example

```python
import nlm

# Configure for specific experiment
experiment_config = {
    "neuron_count": 800,
    "region_count": 2,
    "connection_probability": 0.08,
    "max_steps": 3000,
    "enable_checkpointing": True,
    "enable_development": True,
    "enable_replay": True,
    "enable_curiosity": True,
    "enable_reward_modulation": True
}

# Create configuration
config = nlm.createDefaultConfig()
for key, value in experiment_config.items():
    config.set(key, value)

# Run experiment
brain = nlm.createBrain(config)
brain.initialize()

# TODO: Implement experiment framework
# experiment = nlm.createExperiment(brain, config)
# result = experiment.run()
# print(f"Experiment result: {result}")
```

## Command Reference

### Core Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `demo` | Run comprehensive demo/test | `./nlm demo` |
| `debug` | Run with enhanced debugging | `./nlm debug [options]` |
| `performance` | Run performance benchmarks | `./nlm performance [options]` |
| `experiment` | Run custom experiment | `./nlm experiment [options]` |
| `interactive` | Run interactive mode | `./nlm interactive` |

### Configuration Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `create-config` | Create default config file | `./nlm create-config <file>` |
| `validate-config` | Validate config file | `./nlm validate-config <file>` |

### System Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `monitor` | Start system monitoring | `./nlm monitor` |
| `profile` | Profile brain performance | `./nlm profile [steps]` |
| `stats` | Show system statistics | `./nlm stats` |
| `export` | Export brain state | `./nlm export <file>` |
| `import` | Import brain state | `./nlm import <file>` |

### General Options

| Option | Description |
|--------|-------------|
| `--config=<file>` | Configuration file path |
| `--output=<file>` | Output file path |
| `--log-level=<level>` | Log level (debug, info, warning, error) |
| `--verbose` | Enable verbose output |
| `--quiet` | Suppress output |
| `--json-config` | Use JSON configuration format |
| `--max-steps=<n>` | Maximum simulation steps |
| `--timestep=<t>` | Simulation timestep |
| `--seed=<n>` | Random seed for reproducibility |

## Advanced Features for Expert Users

### 1. Custom Brain Types

```cpp
// Create custom neuron types
class CustomNeuron : public Neuron {
public:
    CustomNeuron() : Neuron() {
        // Custom neuron properties
        setThreshold(1.5f);  // Different threshold
        setRestingPotential(-65.0f);  // Different resting potential
    }
};
```

### 2. Custom Plasticity Rules

```cpp
// Implement custom STDP rules
class CustomSTDP : public STDP {
public:
    void applyPlasticity(Synapse* synapse, double dt) override {
        // Custom STDP implementation
        // Modify weight update rules based on custom learning objectives
    }
};
```

### 3. Custom Memory Systems

```cpp
// Implement custom memory system
class CustomWorkingMemory : public NeuralWorkingMemory {
public:
    void store(const std::vector<float>& pattern, float strength) override {
        // Custom memory storage implementation
    }
    
    void update(TimestepDuration dt) override {
        // Custom memory update mechanism
    }
};
```

### 4. Custom Environment

```cpp
// Create custom environment
class CustomEnvironment : public SimpleWorld {
public:
    void applyAction(NeuronId agent, const Action& action) override {
        // Custom action application
        // Implement domain-specific behavior
    }
    
    float computeReward(NeuronId agent) const override {
        // Custom reward calculation
        // Implement domain-specific reward signals
    }
};
```

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

### "Configuration file not loading"
- Check file path and permissions
- Validate JSON syntax if using JSON config
- Ensure required configuration keys exist

### "Performance is slow"
- Use `--quiet` flag to reduce logging overhead
- Run performance profiler to identify bottlenecks
- Consider reducing neuron count for testing

### "Memory systems not working"
- Verify memory systems are integrated in brain step
- Check that working memory is properly initialized
- Ensure episodic memory has sufficient storage space

## Next Steps

When you're comfortable with NLM:

1. **Read `HOW_TO_USE.md` for more details** - Comprehensive API reference
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - System architecture and design
3. **Experiment with different configurations!** - Test various parameters and settings
4. **Contribute to NLM!** - Help improve this experimental brain system

That's it! You're now ready to use NLM like an expert with all the advanced features and commands.