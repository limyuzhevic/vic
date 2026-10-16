# NLM (Neural Learning Machine) - Enhanced Python API Guide

## Overview

This document provides comprehensive documentation for the enhanced NLM Python API, which extends the original bindings with additional functionality, better error handling, and high-level utilities.

## Version Information

- **Core API**: `pynlm` (original bindings)
- **Enhanced API**: `pynlm_enhanced` (includes utilities and convenience methods)
- **Version**: Enhanced version 1.0.0

## Key Improvements

### 1. Enhanced Error Handling
- Custom exception classes: `NLMError`, `ConfigError`, `BrainError`, `SimulationError`
- Better error messages and debugging information
- Pythonic exception handling

### 2. Richer API Design
- Python property access patterns
- More intuitive method names
- Comprehensive parameter validation
- Enhanced documentation strings

### 3. Convenience Methods
- Simulation management utilities
- Data analysis tools
- Visualization helpers
- Configuration presets

### 4. Advanced Features
- Plugin system support
- Performance monitoring
- Data export/import
- Multi-simulation management

## Quick Start (Enhanced API)

```python
import pynlm_enhanced as nlm

# Create a simulation with enhanced utilities
sim = nlm.create_simulation("advanced", 100, 100)

# Run episode with automatic statistics collection
stats = sim.run_episode(max_steps=1000, random_seed=42)

print(f"Episode completed: {stats['total_reward']} reward")
print(f"Success: {stats['success']}")
print(f"Final curiosity: {sim.agent.getCuriosityLevel():.3f}")

# Analyze multiple episodes
analysis = nlm.DataAnalyzer.calculate_metrics([stats])
print(f"Mean reward: {analysis['mean_reward']:.3f}")
```

## Quick Reference

### Enhanced Module Contents

#### Core Classes
- `SimulationManager` - High-level simulation management
- `DataAnalyzer` - Statistical analysis tools
- `VisualizationHelper` - Visualization utilities
- `ConfigurationTools` - Configuration management
- `PerformanceMonitor` - Performance monitoring

#### Factory Functions
- `create_simulation(preset, width, height)` - Create simulation with preset
- `create_research_simulation(width, height)` - Create research-grade simulation
- `create_learning_simulation(width, height)` - Create learning-focused simulation
- `get_version()` - Get version information
- `get_system_info()` - Get comprehensive system info

### Comparison: Original vs Enhanced

| Feature | Original API | Enhanced API |
|---------|--------------|--------------|
| Basic brain creation | `createBrain(config)` | Same (plus presets) |
| Error handling | Standard exceptions | Custom exceptions |
| Simulation control | Manual loop | `SimulationManager.run_episode()` |
| Data analysis | Manual | `DataAnalyzer.calculate_metrics()` |
| Visualization | Not available | `VisualizationHelper.plot_brain_metrics()` |
| Configuration | Basic file I/O | `ConfigurationTools.create_preset_config()` |
| Performance monitoring | Not available | `PerformanceMonitor` |

## Creating Simulations with Enhanced API

### 1. Basic Enhanced Simulation

```python
import pynlm_enhanced as nlm

# Create a simple simulation
sim = nlm.create_simulation("basic", 50, 50)
stats = sim.run_episode(max_steps=200)
print(f"Total reward: {stats['total_reward']}")
```

### 2. Research-Grade Simulation

```python
import pynlm_enhanced as nlm

# Create a comprehensive research simulation
sim = nlm.create_research_simulation(200, 200)

# Add environment objects
sim.add_resource(10, 10, value=1.0)
sim.add_resource(20, 20, value=2.0)
sim.add_hazard(5, 5, value=-0.5)

# Run learning simulation
stats = sim.run_episode(max_steps=1000, random_seed=42)

# Get detailed status
status = sim.get_status()
print(f"Brain age: {status['brain_age']:.2f}")
print(f"Success rate: {stats['success']}")
```

### 3. Advanced Learning Simulation

```python
import pynlm_enhanced as nlm
from pynlm_enhanced import SimulationManager

# Create with custom configuration
sim = nlm.create_learning_simulation(100, 100)

# Enable all subsystems
sim.agent.enableRewardModulation(True)
sim.agent.enableStructuralPlasticity(True)
sim.agent.enableDevelopment(True)
sim.agent.enableCuriosity(True)

# Create custom world objects
for i in range(10):
    sim.add_resource(i * 10 + 5, i * 10 + 5, value=1.0)

# Run training simulation
training_stats = sim.run_episode(max_steps=2000, random_seed=123)

# Analyze learning progress
analysis = nlm.DataAnalyzer.calculate_metrics([training_stats])
print(f"Learning progress: {analysis['success_rate']*100:.1f}% success rate")
print(f"Mean reward: {analysis['mean_reward']:.3f}")
```

## Data Analysis and Visualization

### Working with Episode Data

```python
import pynlm_enhanced as nlm
import matplotlib.pyplot as plt

# Run multiple episodes and collect statistics
all_stats = []
sim = nlm.create_simulation("advanced", 50, 50)

for episode in range(10):
    stats = sim.run_episode(max_steps=500, random_seed=episode)
    all_stats.append(stats)

# Analyze performance
metrics = nlm.DataAnalyzer.calculate_metrics(all_stats)
print("Performance Metrics:")
for key, value in metrics.items():
    print(f"  {key}: {value:.3f}")

# Visualize results
nlm.VisualizationHelper.plot_brain_metrics({
    'region_data': {
        'cortex': {'neuron_count': 500},
        'hippocampus': {'neuron_count': 200},
        'amygdala': {'neuron_count': 150}
    },
    'activity_data': {
        'step': list(range(100)),
        'firing_rate': [0.1 * (i % 10) for i in range(100)]
    },
    'neuromodulation_data': {
        'dopamine': 0.8,
        'curiosity': 0.6,
        'novelty': 0.7,
        'prediction_error': 0.3
    }
}, output_file="brain_metrics.png")
```

### Time Series Analysis

```python
import pynlm_enhanced as nlm

# Create time series data from multiple episodes
training_sessions = []
for session in range(5):
    sim = nlm.create_simulation("research", 100, 100)
    sim.run_episode(max_steps=1000, random_seed=session)
    training_sessions.append(sim.get_status())

# Extract time series (conceptual example)
time_series = nlm.DataAnalyzer.extract_time_series(training_sessions)

# Export data
nlm.DataAnalyzer.export_data(time_series, "simulation_data.json")
```

## Configuration Management

### Using Configuration Presets

```python
import pynlm_enhanced as nlm

# Create different configuration presets
basic_config = nlm.ConfigurationTools.create_preset_config("basic")
advanced_config = nlm.ConfigurationTools.create_preset_config("advanced")
research_config = nlm.ConfigurationTools.create_preset_config("research")

# Save configurations
nlm.ConfigurationTools.export_config(basic_config, "basic_config.json")

# Import configurations
imported_config = nlm.ConfigurationTools.import_config("basic_config.json")
```

### Configuration Comparison

```python
import pynlm_enhanced as nlm

# Compare preset configurations
presets = ["basic", "advanced", "research"]
for preset in presets:
    config = nlm.ConfigurationTools.create_preset_config(preset)
    keys = config.getKeys()
    print(f"{preset} configuration has {len(keys)} keys")
    print(f"  Key count: {len(keys)}")
    print(f"  Neuron count: {config.getInt('brain.neuron_count', 1000)}")
```

## Performance Monitoring

### Monitoring Simulation Performance

```python
import pynlm_enhanced as nlm

# Create performance monitor
monitor = nlm.PerformanceMonitor()

# Create simulation
sim = nlm.create_simulation("advanced", 50, 50)

# Run simulation with performance monitoring
for step in range(100):
    monitor.start_step()
    sim.world.update(0.1)
    monitor.end_step(sim.brain)

# Get performance report
report = monitor.get_performance_report()
print("Performance Report:")
for key, value in report.items():
    print(f"  {key}: {value}")
```

## Visualization Tools

### Creating World Maps

```python
import pynlm_enhanced as nlm
import numpy as np

# Create a world map
sim = nlm.create_simulation("basic", 100, 100)

# Add some objects
sim.add_resource(20, 20)
sim.add_resource(40, 40)
sim.add_hazard(60, 60)
sim.add_wall(80, 80)

# Create world map
world_map = nlm.VisualizationHelper.create_2d_world_map(
    sim.world.getWidth(),
    sim.world.getHeight(),
    sim.world_objects
)

# The world_map is a numpy array where:
# - 0.0 = empty space
# - 0.8 = resource
# - 0.2 = hazard
# - 0.5 = wall
```

### Exporting Results

```python
import plm_enhanced as nlm

# Create and run simulation
sim = nlm.create_simulation("advanced", 100, 100)
stats = sim.run_episode(max_steps=500)

# Export episode statistics
nlm.DataAnalyzer.export_data(stats, "episode_stats.json")

# Export configuration
nlm.ConfigurationTools.export_config(sim.config, "simulation_config.json")

# Create visualization
nlm.VisualizationHelper.plot_brain_metrics(
    {
        'region_data': {'cortex': {'neuron_count': 500}},
        'activity_data': {'step': list(range(500))},
        'neuromodulation_data': {'dopamine': 0.7, 'curiosity': 0.5}
    },
    "results.png"
)
```

## Error Handling

### Working with Exceptions

```python
import pynlm_enhanced as nlm

try:
    # Create a simulation
    sim = nlm.create_simulation("basic", 50, 50)
    
    # This might raise various exceptions
    sim.run_episode(max_steps=1000)
    
except nlm.NLMError as e:
    print(f"NLM error occurred: {e}")
    
except nlm.ConfigError as e:
    print(f"Configuration error: {e}")
    
except nlm.BrainError as e:
    print(f"Brain error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
    import traceback
    traceback.print_exc()
```

## Integration Examples

### Complete Research Workflow

```python
import pynlm_enhanced as nlm
import json
import time

class ResearchWorkflow:
    def __init__(self):
        self.results = []
        self.monitor = nlm.PerformanceMonitor()
    
    def run_experiment(self, experiment_name, preset="research", num_episodes=10):
        """Run a complete experiment."""
        print(f"Running experiment: {experiment_name}")
        
        experiment_start = time.time()
        
        for episode in range(num_episodes):
            # Create simulation with performance monitoring
            self.monitor = nlm.PerformanceMonitor()
            sim = nlm.create_simulation(preset, 100, 100)
            
            # Enable all subsystems for comprehensive research
            sim.agent.enableRewardModulation(True)
            sim.agent.enableStructuralPlasticity(True)
            sim.agent.enableDevelopment(True)
            sim.agent.enableCuriosity(True)
            
            # Run episode with monitoring
            start_time = time.time()
            stats = sim.run_episode(max_steps=1000, random_seed=episode)
            end_time = time.time()
            
            # Collect performance data
            self.monitor.start_step()
            # Add brain state to monitor
            
            # Store results
            self.results.append({
                'episode': episode,
                'stats': stats,
                'time_taken': end_time - start_time,
                'brain_state': sim.get_status()
            })
        
        experiment_end = time.time()
        
        # Analyze results
        analysis = self.analyze_results()
        
        return {
            'name': experiment_name,
            'duration': experiment_end - experiment_start,
            'episodes': num_episodes,
            'analysis': analysis,
            'results': self.results
        }
    
    def analyze_results(self):
        """Analyze experiment results."""
        if not self.results:
            return {}
        
        all_stats = [r['stats'] for r in self.results]
        
        analysis = {
            'mean_reward': nlm.DataAnalyzer.calculate_metrics(all_stats)['mean_reward'],
            'success_rate': nlm.DataAnalyzer.calculate_metrics(all_stats)['success_rate'],
            'total_objects_collected': sum(r['stats']['objects_collected'] for r in self.results),
            'total_hazards_avoided': sum(r['stats']['hazards_avoided'] for r in self.results),
            'average_steps_per_episode': sum(r['stats']['steps'] for r in self.results) / len(self.results)
        }
        
        return analysis
    
    def export_results(self, filepath):
        """Export experiment results."""
        results_data = {
            'workflow_name': 'NLM Research Workflow',
            'timestamp': time.time(),
            'experiments': self.results,
            'analysis': self.analyze_results()
        }
        
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2)

# Run the research workflow
workflow = ResearchWorkflow()
experiment = workflow.run_experiment("Curiosity-Driven Exploration", "advanced", num_episodes=5)

print(f"Experiment completed in {experiment['duration']:.2f} seconds")
print(f"Mean reward: {experiment['analysis']['mean_reward']:.3f}")
print(f"Success rate: {experiment['analysis']['success_rate']*100:.1f}%")

# Export results
workflow.export_results("research_results.json")
print("Results exported to research_results.json")
```

## Installation and Setup

### Installing Enhanced API

```bash
# Install the enhanced NLM package
pip install pynlm_enhanced

# Or install in development mode
pip install -e .

# Verify installation
python -c "import pynlm_enhanced as nlm; print(nlm.get_system_info())"
```

### Dependencies

The enhanced API requires:
- `pynlm` - Core NLM bindings
- `numpy` - Numerical operations
- `matplotlib` (optional) - Visualization
- `opencv-python` (optional) - Video export

### Setting Up Environment

```python
import pynlm_enhanced as nlm

# Check if all dependencies are available
system_info = nlm.get_system_info()

if system_info['nlm_available']:
    print("✓ NLM core bindings available")
    
    # Check for optional dependencies
    try:
        import matplotlib.pyplot as plt
        print("✓ Matplotlib available for visualization")
    except ImportError:
        print("⚠ Matplotlib not available - visualization limited")
        
    try:
        import cv2
        print("✓ OpenCV available for video export")
    except ImportError:
        print("⚠ OpenCV not available - video export limited")
else:
    print("✗ NLM core bindings not available - please install pynlm")
```

## Migration Guide

### From Original API to Enhanced API

If you're currently using the original NLM API and want to upgrade, here's how to migrate:

#### Simple Migration

**Original:**
```python
import pynlm as nlm

brain = nlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

**Enhanced (Recommended):**
```python
import pynlm_enhanced as nlm

# Create simulation with basic preset
sim = nlm.create_simulation("basic", 100, 100)

# Run episode with automatic statistics
stats = sim.run_episode(max_steps=100)

print(f"Episode completed: {stats['total_reward']} total reward")
print(f"Success: {stats['success']}")
print(f"Final firing rate: {sim.brain.getAverageFiringRate():.3f} Hz")
```

#### Gradual Migration

For projects that can't be upgraded immediately, you can use both APIs:

```python
import pynlm as nlm_original
import pynlm_enhanced as nlm_enhanced

# Use original API for compatibility
brain = nlm_original.createBrain(config)
brain.initialize()

# Use enhanced utilities for analysis
analysis = nlm_enhanced.DataAnalyzer.calculate_metrics([
    {'total_reward': brain.getTotalSpikeCount(), 'success': True}
])
```

## Best Practices

### 1. Use Enhanced API for New Projects
- The enhanced API provides better error handling and utilities
- Includes comprehensive simulation management
- Better data analysis and visualization tools

### 2. Handle Errors Gracefully
```python
import pynlm_enhanced as nlm

try:
    sim = nlm.create_simulation("research", 100, 100)
    stats = sim.run_episode(max_steps=1000)
except nlm.NLMError as e:
    print(f"Simulation error: {e}")
    # Implement recovery logic
```

### 3. Monitor Performance
```python
import pynlm_enhanced as nlm

monitor = nlm.PerformanceMonitor()
sim = nlm.create_simulation("advanced", 100, 100)

for step in range(100):
    monitor.start_step()
    sim.run_episode(max_steps=1)
    monitor.end_step(sim.brain)

report = monitor.get_performance_report()
if report['average_step_time'] > 0.1:  # >100ms per step
    print("Warning: Simulation running slowly")
```

### 4. Export Results for Analysis
```python
import pynlm_enhanced as nlm

# Run experiment
sim = nlm.create_simulation("research", 200, 200)
stats = sim.run_episode(max_steps=2000)

# Export comprehensive data
nlm.DataAnalyzer.export_data(stats, "experiment_results.json")
nlm.ConfigurationTools.export_config(sim.config, "experiment_config.json")
```

### 5. Use Configuration Presets Wisely
```python
import pynlm_enhanced as nlm

# Choose preset based on experimental needs
if experiment_type == "quick_test":
    sim = nlm.create_simulation("basic", 50, 50)
elif experiment_type == "learning_study":
    sim = nlm.create_simulation("advanced", 100, 100)
elif experiment_type == "comprehensive_research":
    sim = nlm.create_research_simulation(200, 200)
```

## Troubleshooting

### Common Issues and Solutions

**Issue: "NLM not available" error**
```python
# Solution: Check installation
import pynlm_enhanced as nlm
system_info = nlm.get_system_info()
print(system_info)

# If not available:
# pip install pynlm_enhanced
# pip install pynlm
```

**Issue: Missing matplotlib for visualization**
```python
# Solution: Install optional dependency
pip install matplotlib
```

**Issue: Slow simulation performance**
```python
# Solution: Check performance monitoring
sim = nlm.create_simulation("basic", 50, 50)  # Start with smaller world
stats = sim.run_episode(max_steps=100)  # Run shorter episodes
monitor = nlm.PerformanceMonitor()
# ... add monitoring
```

**Issue: Visualization not working**
```python
# Solution: Check matplotlib installation
try:
    import matplotlib.pyplot as plt
    # Try to create a simple plot
    plt.plot([1, 2, 3], [1, 2, 3])
    plt.show()
except ImportError:
    print("Matplotlib not available - please install: pip install matplotlib")
```

### Getting Help

For additional support:

1. **Check system information**
```python
import pynlm_enhanced as nlm
print(nlm.get_system_info())
```

2. **View documentation**
```python
help(pynlm_enhanced)
help(pynlm_enhanced.SimulationManager)
help(pynlm_enhanced.DataAnalyzer)
```

3. **Run examples**
```python
# Run example scripts
python examples/basic_example.py
python examples/research_workflow.py
```

4. **Check logs**
```python
import logging
logging.basicConfig(level=logging.DEBUG)
```

## Version Compatibility

### Supported Python Versions
- Python 3.8+
- Python 3.9+
- Python 3.10+
- Python 3.11+
- Python 3.12+

### NLM C++ Version Compatibility
- NLM Phase 6 (Final Integration)
- Compatible with core NLM bindings

### Recommended Dependencies
```bash
# Core dependencies
pip install pynlm numpy

# Optional for enhanced features
pip install matplotlib opencv-python pandas scipy
```

## License

This enhanced Python API is provided under the same MIT license as the core NLM project.

## Acknowledgments

- Original NLM developers and researchers
- pybind11 team for binding framework
- Python community for excellent ecosystem
- All NLM contributors and Phase 6 integration team

## Contact

For issues or questions:
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Examples: https://github.com/nlm-project/nlm/tree/master/examples