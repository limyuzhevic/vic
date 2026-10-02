# NLM Python Package Documentation

## Introduction

The NLM (Neural Learning Machine) Python package provides a comprehensive interface to the NLM neural simulation framework. This package enables researchers and developers to create, run, and analyze neural simulations with advanced Python APIs.

## Installation

### Quick Installation

```bash
pip install .
```

### Full Installation with All Dependencies

```bash
pip install .[all]
```

### Development Installation

```bash
pip install -e .
```

## Package Structure

```
pynlm/
├── __init__.py           # Main package interface
├── core.py              # Core neural simulation components
├── experiments.py       # Experiment management
├── research.py          # Research workflows
├── visualization.py     # Results visualization
├── performance.py       # Performance monitoring
├── config.py           # Configuration management
├── types.py            # Type definitions
├── core_ext.py          # C++ bindings interface
├── examples/           # Example scripts
│   ├── basic_simulation.py
│   ├── agent_world.py
│   ├── phase3_experiment.py
│   ├── phase4_experiment.py
│   ├── phase6_experiment.py
│   ├── advanced_research.py
│   └── visualization_examples.py
├── docs/               # Documentation
│   ├── README.md
│   ├── API-Guide.md
│   ├── USER-GUIDE.md
│   └── CONTRIBUTING.md
├── configs/            # Example configurations
│   ├── simple_brain.json
│   ├── large_scale.yaml
│   └── default.cfg
└── data/               # Example data
    ├── sample_results.json
    └── sample_config.yaml
```

## Core Concepts

### 1. Brain and Simulation

The NLM Python package provides classes for neural simulation:

- **Config**: Configuration management with JSON/YAML support
- **Brain**: Neural brain instance with all integrated systems
- **SimpleWorld**: Simulated environment for agents
- **AgentBrain**: Interface between brain and world

### 2. Experiment Management

The package provides comprehensive experiment management:

- **Experiment**: Single experiment run with configuration
- **ParameterSweep**: Automated parameter optimization
- **MultiSeedExperiment**: Statistical validation across seeds
- **ExperimentTemplate**: Pre-configured experiment templates

### 3. Research Workflows

High-level APIs for research automation:

- **ResearchWorkflow**: Sequential experiment execution
- **ScientificExperimentDesigner**: Experiment design with best practices
- **ExperimentReproducibilityManager**: Version control and reproducibility

### 4. Results Visualization

Comprehensive visualization capabilities:

- **ResultsVisualizer**: Main visualization class
- **StatisticalAnalysis**: Statistical plot generation
- **Dashboard**: Interactive web-based dashboards

### 5. Performance Monitoring

Resource monitoring and optimization:

- **ResourceMonitor**: System resource monitoring
- **MemoryProfiler**: Memory usage analysis
- **SimulationProfiler**: Simulation performance profiling

### 6. Configuration Management

Advanced configuration system:

- **ConfigManager**: Configuration file management
- **ConfigPreset**: Preset configuration management
- **Config**: Pythonic configuration interface

## Quick Start Guide

### Step 1: Install the Package

```bash
pip install .[all]
```

### Step 2: Create a Basic Simulation

```python
import pynlm

# Create configuration
config = pynlm.create_default_config()
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("seed", 42)

# Create and initialize brain
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
print(f"Brain initialized with {brain.get_total_neuron_count()} neurons")
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
```

### Step 3: Run an Experiment

```python
import pynlm

# Create experiment configuration
experiment_config = {
    'name': 'learning_experiment',
    'neuron_count': 1000,
    'region_count': 2,
    'max_steps': 200,
    'seed': 123
}

# Run experiment
result = pynlm.run_experiment('learning_experiment', experiment_config)

print(f"Experiment success: {result.success}")
print(f"Performance: {result.average_firing_rate:.2f} firing rate")
```

### Step 4: Parameter Optimization

```python
import pynlm
from pynlm.experiments import ParameterSweep

# Define parameter ranges
parameter_ranges = {
    'learning_rate': [0.001, 0.01, 0.1],
    'exploration_rate': [0.1, 0.5, 1.0],
    'novelty_threshold': [0.2, 0.5, 0.8]
}

# Create parameter sweep
from pynlm.experiments import ParameterSweepConfig
sweep_config = ParameterSweepConfig(
    parameters=parameter_ranges,
    random_search=True,
    num_iterations=50
)

# Run sweep
results = pynlm.run_parameter_sweep('learning_experiment', sweep_config)

# Get best result
best_result = sweep_result.get_best_result()
print(f"Best parameters found")
```

### Step 5: Multi-Seed Validation

```python
import pynlm

# Run multi-seed experiment
base_config = {
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.1
}

results = pynlm.run_multi_seed_experiment(base_config, num_seeds=10)

# Statistical analysis
from pynlm.experiments import analyze_results
stat_results = analyze_results(results)

print(f"Mean firing rate: {stat_results.mean:.2f} ± {stat_results.std:.2f}")
print(f"95% Confidence interval: {stat_results.percentiles[95]:.2f}")
```

## Advanced Usage

### Research Workflows

```python
import pynlm
from pynlm.research import ResearchWorkflow

# Create research workflow
workflow = ResearchWorkflow('my_research', 'My research workflow')

# Add steps
workflow.add_step('create_config', 'experiment', {
    'experiment_name': 'baseline'
})

workflow.add_step('run_experiment', 'experiment', {
    'experiment_name': 'baseline'
})

workflow.add_step('analyze_results', 'statistical_analysis', {
    'results': 'run_experiment'
})

# Run workflow
results = workflow.run_all_steps()
```

### Visualization

```python
import pynlm
from pynlm.visualization import ResultsVisualizer

# Create visualizer
visualizer = ResultsVisualizer()

# Create and run experiment
experiment_results = pynlm.run_experiment('test_experiment')

# Generate visualizations
fig = visualizer.create_dashboard([experiment_results])
fig.show()  # Interactive dashboard

# Save plots
visualizer.save_all_plots('./results')
```

### Performance Monitoring

```python
import pynlm
from pynlm.performance import PerformanceMonitor

# Create brain and start monitoring
brain = pynlm.create_brain()
monitor = PerformanceMonitor(brain)
monitor.start_monitoring()

# Run simulation
for step in range(100):
    brain.step(step)
    monitor.profile_step(step, 0.0)

# Stop monitoring
metrics = monitor.stop_monitoring()

print(f"Simulation time: {metrics.simulation_time:.2f}s")
print(f"Memory usage: {metrics.memory_usage_mb:.1f} MB")
print(f"CPU usage: {metrics.cpu_usage:.1f}%")
```

### Configuration Management

```python
import pynlm

# Load configuration from file
from pynlm import Config
config = Config()
config.load_from_file('experiment_config.yaml')

# Use preset configuration
config = Config.load_from_preset('research_scientific')

# Save configuration
config.save_to_file('my_experiment.json')
```

## Examples

The package includes comprehensive examples in the `examples/` directory:

### 1. Basic Simulation
- **File**: `examples/basic_simulation.py`
- **Description**: Basic brain simulation with configuration management
- **Features**: Core simulation, checkpointing, performance tracking

### 2. Agent-World Simulation
- **File**: `examples/agent_world.py`
- **Description**: Complete agent-world simulation with sensory input and motor output
- **Features**: Agent-brain interface, world interaction, reward systems

### 3. Phase 3 Experiment
- **File**: `examples/phase3_experiment.py`
- **Description**: World interaction, sensory processing, and motor control
- **Features**: Experimental design, statistical validation

### 4. Phase 4 Experiment
- **File**: `examples/phase4_experiment.py`
- **Description**: Cognitive development, concept formation, and learning
- **Features**: Development stages, learning curves

### 5. Phase 6 Integration
- **File**: `examples/phase6_experiment.py`
- **Description**: Integration of all neural systems
- **Features**: Complete system integration, comprehensive validation

### 6. Advanced Research
- **File**: `examples/advanced_research.py`
- **Description**: Advanced research with parameter sweeps and statistical analysis
- **Features**: Parameter optimization, research workflows

### 7. Visualization Examples
- **File**: `examples/visualization_examples.py`
- **Description**: Comprehensive visualization and results analysis
- **Features**: Statistical plotting, interactive dashboards

## API Reference

### Core Module

#### Config Class
- `Config()`: Create configuration object
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file
- `set(key, value)`: Set configuration value
- `get(key, default)`: Get configuration value
- `has(key)`: Check if key exists
- `keys()`: Get all keys
- `clear()`: Clear configuration
- `summary()`: Get configuration summary

#### Brain Class
- `Brain(config)`: Create brain with configuration
- `initialize()`: Initialize brain
- `step(step)`: Perform simulation step
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `reset()`: Reset brain
- `logStatus()`: Log brain status
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getAverageFiringRate()`: Get average firing rate

#### SimpleWorld Class
- `SimpleWorld()`: Create world
- `configure(width, height, vision_width, vision_height)`: Configure world
- `update(timestep)`: Update world
- `applyMotorCommand(command, current_time)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body

#### AgentBrain Class
- `AgentBrain(brain)`: Create agent brain
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development

### Experiments Module

#### Experiment Class
- `Experiment(name, config)`: Create experiment
- `run()`: Run experiment
- `getResults()`: Get experiment results

#### ParameterSweep Class
- `ParameterSweep(config)`: Create parameter sweep
- `runSweep(template)`: Run parameter sweep
- `getBestResult()`: Get best result
- `getWorstResult()`: Get worst result

#### MultiSeedExperiment Class
- `MultiSeedExperiment(base_config, num_seeds)`: Create multi-seed experiment
- `run(template)`: Run multi-seed experiment
- `getStatisticalAnalysis()`: Get statistical analysis

### Research Module

#### ResearchWorkflow Class
- `ResearchWorkflow(name, description)`: Create research workflow
- `addStep(name, type, config)`: Add workflow step
- `runAllSteps()`: Run all steps
- `getResultsSummary()`: Get results summary

#### ScientificExperimentDesigner Class
- `ScientificExperimentDesigner()`: Create experiment designer
- `getTemplate(name)`: Get experiment template
- `createWorkflowFromTemplate(name, workflow_name, **overrides)`: Create workflow from template
- `validateExperimentConfig(config)`: Validate experiment configuration

### Visualization Module

#### ResultsVisualizer Class
- `ResultsVisualizer(config)`: Create visualizer
- `plotTimeSeries(data, title, save_path)`: Plot time series
- `plotComparisonChart(results, metrics, title, save_path)`: Plot comparison chart
- `plotHistogram(data, title, save_path)`: Plot histogram
- `createDashboard(results, metrics)`: Create interactive dashboard
- `saveAllPlots(output_dir)`: Save all plots

### Performance Module

#### PerformanceMonitor Class
- `PerformanceMonitor(brain)`: Create performance monitor
- `startMonitoring()`: Start monitoring
- `stopMonitoring()`: Stop monitoring
- `profileStep(step, step_time)`: Profile step
- `getResourceUsage()`: Get resource usage
- `getMemoryProfile()`: Get memory profile

### Configuration Module

#### ConfigManager Class
- `ConfigManager(config_dir)`: Create configuration manager
- `loadPreset(preset_name)`: Load configuration from preset
- `createConfigFromPreset(preset_name)`: Create configuration from preset
- `saveConfig(config, config_name, format, description)`: Save configuration
- `loadConfig(config_name, format)`: Load configuration
- `validateConfig(config)`: Validate configuration

## Configuration Files

### Example Configurations

#### Basic Configuration
```yaml
# configs/simple_brain.yaml
neuron_count: 1000
region_count: 3
connection_probability: 0.1
seed: 42
simulation_timestep: 0.001
enable_parameter_sweeps: false
enable_multi_objective_optimization: false
```

#### Large Scale Configuration
```json
{
  "neuron_count": 10000,
  "region_count": 5,
  "connection_probability": 0.15,
  "seed": 123,
  "enable_multithreading": true,
  "thread_count": 8,
  "enable_SIMD": true,
  "enable_sparse_connectivity": true
}
```

#### Preset Configuration
```yaml
# configs/default.cfg
neuron_count = 500
region_count = 2
connection_probability = 0.12
seed = 42
enable_parameter_sweeps = true
enable_multi_objective_optimization = true
enable_adaptive_experimentation = true
```

## Data Formats

### Experiment Results Format
```json
{
  "experiment_name": "learning_experiment",
  "seed": 123,
  "success": true,
  "steps_completed": 200,
  "final_spike_count": 45678,
  "average_firing_rate": 0.456,
  "performance_metrics": {
    "simulation_time": 45.2,
    "memory_usage": 234.5,
    "cpu_usage": 67.8
  },
  "custom_metrics": {
    "accuracy": 0.89,
    "learning_speed": 0.67
  },
  "error_message": null,
  "timestamp": "2024-01-01T12:00:00Z"
}
```

## Troubleshooting

### Common Issues

#### 1. Package Not Found
```bash
# Check if installed in current environment
pip list | grep pynlm

# Install if not present
pip install .
```

#### 2. Missing Dependencies
```bash
# Install with all dependencies
pip install .[all]
```

#### 3. Configuration File Not Found
```python
# Check current directory
import os
print(os.getcwd())

# Use absolute path
config.load_from_file('/path/to/config.yaml')
```

#### 4. Simulation Too Slow
```python
# Reduce neuron count for testing
config.set('neuron_count', 100)

# Disable unnecessary features
config.set('enable_parameter_sweeps', False)
config.set('enable_multi_objective_optimization', False)
```

#### 5. Visualization Not Working
```python
# Check matplotlib backend
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

# Install system dependencies
# Ubuntu: sudo apt-get install libfreetype6-dev libpng-dev
# macOS: brew install freetype libpng
```

### Getting Help

#### Documentation
- **User Guide**: `docs/USER-GUIDE.md`
- **API Reference**: `docs/API-Guide.md`
- **Examples**: `examples/` directory

#### Community
- **GitHub Issues**: https://github.com/nlm/pynlm/issues
- **Discussion Forum**: https://forum.nlm.ai
- **Discord**: https://discord.gg/nlm

#### Support
- **Email**: info@nlm.ai
- **Slack**: https://slack.nlm.ai

## Contributing

### Contributing Guidelines
See `docs/CONTRIBUTING.md` for detailed guidelines on:

- Code style and formatting
- Testing and validation
- Documentation requirements
- Pull request process

### Code Style
- Python 3.8+ syntax
- PEP 8 compliant
- Type hints for all public APIs
- Comprehensive docstrings

### Testing
- Unit tests for all modules
- Integration tests for end-to-end functionality
- Performance tests for optimization validation
- Example-based tests for documentation

### Documentation
- API documentation with examples
- User guides with step-by-step instructions
- Tutorial notebooks
- Quickstart guides

## License

This package is licensed under the MIT License. See `LICENSE` file for details.

## Copyright

Copyright (c) 2024 NLM Research Team

All rights reserved.

## Acknowledgements

This package is built on the NLM C++ framework (Phase 6 implementation). It provides a comprehensive Python interface to access all neural simulation capabilities while maintaining high performance and research-grade functionality.

### Key Dependencies

- **pybind11**: C++ Python bindings
- **NumPy**: Numerical operations
- **Pandas**: Data analysis
- **Matplotlib**: Visualization
- **Scikit-learn**: Machine learning algorithms
- **Plotly**: Interactive visualization
- **Seaborn**: Statistical plotting
- **PyYAML**: YAML parsing
- **PSUtil**: System monitoring

### Special Thanks

- The NLM research team for their vision and support
- Contributors to the C++ NLM framework
- Community members for feedback and support
- Documentation contributors for their efforts

## Version History

### Version 1.0.0 (Current)
- Initial release with core functionality
- Comprehensive experiment management
- Advanced visualization capabilities
- Performance monitoring and optimization
- Research workflow automation

### Future Plans
- **Version 1.1.0**: Enhanced machine learning integration
- **Version 1.2.0**: Distributed computing support
- **Version 2.0.0**: Major architecture improvements

## Contact

### For Questions
- **Email**: info@nlm.ai
- **GitHub**: https://github.com/nlm/pynlm/issues

### For Contributions
- **Pull Requests**: https://github.com/nlm/pynlm/pulls
- **Code Reviews**: https://github.com/nlm/pynlm/pulls?q=is%3Apr+is%3Aopen+label%3A%22good+first+issue%22

### For Collaboration
- **Discussion**: https://github.com/nlm/pynlm/discussions
- **Team**: https://team.nlm.ai

---

*Created with ❤️ by the NLM Research Team for scientific neural simulation and research.*

*Phase 6: Final integration of all neural systems into a coherent artificial brain.*
