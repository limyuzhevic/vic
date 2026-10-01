# NLM - Enhanced Usage Guide

## What's New in Enhanced pynlm

This is the enhanced version of the NLM Python bindings with significant improvements:

### 🚀 Major New Features

1. **Batch Processing**: Run multiple simulation steps at once with `runSteps()`
2. **Configuration Presets**: Pre-built configs for different use cases
3. **Progress Tracking**: Monitor long simulations with progress callbacks
4. **Error Handling**: Validation and safe initialization
5. **Benchmarking**: Performance measurement tools
6. **Advanced Agent Features**: All neuromodulation and plasticity systems accessible
7. **Action Callbacks**: Custom action providers for batch operations

---

## Quick Start (Updated)

### Example 1: Simplest Brain (Unchanged)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Example 2: Using Configuration Presets (NEW!)

```python
import pynlm

# Quick start with preset configurations
config = pynlm.createSmallConfig()  # Perfect for testing
brain = pynlm.createBrain(config)
brain.initialize()
print("Test brain ready:", brain.getTotalNeuronCount(), "neurons")

# Or for production
config = pynlm.createLargeConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print("Production brain ready:", brain.getTotalNeuronCount(), "neurons")

# Or optimized for learning
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print("Learning brain ready:", brain.getTotalNeuronCount(), "neurons")
```

### Example 3: Batch Processing (NEW!)

```python
import pynlm
from pynlm import Vision, SensoryPercept

# Create brain and agent
brain = pynlm.createBrain(pynlm.createSmallConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

world.configure(10, 10, 8, 8)
agent.initialize(world)

# Create multiple percepts
percepts = []
for i in range(20):
    vision = Vision(8, 8, 3)
    data = [0.1f] * (8 * 8 * 3)
    vision.setData(data)
    
    percept = SensoryPercept()
    percept.setVision(vision)
    percepts.append(percept)

# Process all at once!
agent.runSteps(percepts, 0.001)
print("Processed", len(percepts), "percepts in one call")
```

### Example 4: Progress Monitoring (NEW!)

```python
import pynlm
from pynlm import Vision, SensoryPercept

# Create progress tracking
progress_data = []
def progress_callback(step, firing_rate):
    progress_data.append((step, firing_rate))
    if step % 25 == 0:
        print(f"Step {step}/{100}: {firing_rate:.2f} Hz firing rate")

tracker = pynlm.createProgressTracker(100, progress_callback)
tracker.setMessage("Running simulation...")

print(f"Progress: {tracker.getProgress():.1%}")
```

### Example 5: Configuration Management (NEW!)

```python
import pynlm

# Create custom configuration
config = pynlm.Config()
config.setFloat("learning_rate", 0.01)
config.setInt("max_steps", 1000)
config.setBool("enable_curiosity", True)

print(f"Learning rate: {config.getFloat('learning_rate', 0.0f)}")
print(f"Max steps: {config.getInt('max_steps', 0)}")
print(f"Curiosity enabled: {config.getBool('enable_curiosity', False)}")

# Validate configuration
errors, warnings = pynlm.validateConfig(config)
print(f"Warnings: {len(warnings)}")
```

---

## Advanced Features

### Agent with All Features Enabled

```python
import pynlm

# Create learning-focused brain
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Agent features status:")
print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"  Development enabled: {agent.isDevelopmentEnabled()}")
print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Benchmarking Performance

```python
import pynlm

# Benchmark different configurations
configs = [
    ("Small", pynlm.createSmallConfig()),
    ("Default", pynlm.createDefaultConfig()),
    ("Large", pynlm.createLargeConfig())
]

for name, config in configs:
    spike_count, firing_rate, duration_ms, steps_per_sec = pynlm.runBenchmark(config, 100)
    print(f"{name}: {steps_per_sec:.1f} steps/sec, {duration_ms:.1f}ms duration")
```

---

## Pattern Updates

### Pattern 1: Run a Simulation (Enhanced)

```python
def run_simulation_with_progress(brain, world, agent, num_steps=1000):
    progress_data = []
    
    def progress_callback(step, firing_rate):
        progress_data.append((step, firing_rate))
        if step % 100 == 0:
            print(f"Step {step}: {firing_rate:.2f} Hz")
    
    tracker = pynlm.createProgressTracker(num_steps, progress_callback)
    tracker.setMessage("Running simulation...")
    
    # Create percepts for batch processing
    percepts = []
    for step in range(num_steps):
        vision = pynlm.Vision(8, 8, 3)
        data = [0.1f] * (8 * 8 * 3)
        vision.setData(data)
        
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        percepts.append(percept)
    
    # Run with progress tracking
    agent.runSteps(percepts, 0.001)
    
    return progress_data
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
# Get comprehensive brain statistics
def get_brain_stats(brain):
    if brain.getTotalNeuronCount() == 0:
        return "Brain not initialized"
    
    stats = {
        'neuron_count': brain.getTotalNeuronCount(),
        'firing_neuron_count': brain.getFiringNeuronCount(),
        'active_neuron_count': brain.getActiveNeuronCount(),
        'total_spike_count': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'region_count': brain.getRegionCount(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'config_summary': brain.getConfig().summary() if brain.getConfig() else "No config"
    }
    
    return stats

# Format as human-readable string
stats_str = pynlm.formatBrainStats(brain)
print(stats_str)
```

### Pattern 3: Enable Brain Features (Enhanced)

```python
# All optional features with individual enable calls
def enable_all_features(agent):
    features = {
        'reward_modulation': agent.enableRewardModulation,
        'structural_plasticity': agent.enableStructuralPlasticity,
        'development': agent.enableDevelopment,
        'curiosity': agent.enableCuriosity
    }
    
    for name, enable_func in features.items():
        enable_func(True)
        print(f"Enabled {name}")

enable_all_features(agent)
```

---

## New Utility Functions

### Validation Functions

```python
# Validate configuration
errors, warnings = pynlm.validateConfig(config)
print(f"Config valid: {len(errors) == 0}")
print(f"Warnings: {len(warnings)}")

# Validate brain state
brain_errors, brain_warnings = pynlm.validateBrain(brain)
print(f"Brain valid: {len(brain_errors) == 0}")

# Check for NaN values
nan_issues = pynlm.checkForNaN(brain)
print(f"NaN issues found: {len(nan_issues)}")

# Compare two brains
differences = pynlm.compareBrains(brain1, brain2)
print("Brain differences:", differences)
```

### Configuration Presets

```python
# Available presets
configs = {
    "test": pynlm.createSmallConfig(),      # Fast testing
    "demo": pynlm.createDefaultConfig(),   # Balanced
    "production": pynlm.createLargeConfig(), # Production
    "learning": pynlm.createLearningConfig(), # Learning focused
    "exploration": pynlm.createExplorationConfig() # Exploration focused
}
```

---

## Error Handling

The enhanced bindings provide better error handling:

```python
# Safe initialization
success = pynlm.safeInitialize(brain)
if success:
    print("Brain initialized successfully")
else:
    print("Brain initialization failed")

# Safe stepping
success = pynlm.safeStep(brain, 42, 0.042)
if success:
    print("Step completed successfully")
else:
    print("Step failed")
```

---

## Quick Reference

| New Feature | Function | Description |
|-------------|----------|-------------|
| Batch Processing | `runSteps(percepts, timestep)` | Process multiple steps at once |
| Progress Tracking | `createProgressTracker(total_steps, callback)` | Monitor long simulations |
| Configuration Presets | `createSmallConfig()`, `createLargeConfig()`, etc. | Quick config creation |
| Brain Validation | `validateConfig()`, `validateBrain()` | Configuration and state validation |
| Benchmarking | `runBenchmark(config, steps)` | Performance measurement |
| Advanced Stats | `formatBrainStats()`, `getBrainStats()` | Comprehensive statistics |
| Error Handling | `safeInitialize()`, `safeStep()` | Robust error handling |

---

## Next Steps

1. **Read `HOW_TO_USE.md`** - See the original documentation for basic concepts
2. **Read `docs/ARCHITECTURE.md`** - Understand how all the brain systems work together
3. **Experiment with Examples** - Try the examples in `examples/advanced_usage.py`
4. **Run Tests** - Run the test suite with `python -m pytest tests/`
5. **Explore Features** - Try all the new convenience functions and features

---

That's it! You're now ready to use NLM with all its enhanced features.
