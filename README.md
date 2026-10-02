# NLM Python Package - Quickstart Guide

## Overview

The NLM Python package provides a comprehensive interface to the NLM (Neural Learning Machine) neural simulation framework. This package enables researchers and developers to:

- Run neural simulations with advanced Python APIs
- Perform experimental research with parameter sweeps and statistical analysis
- Visualize and analyze experiment results
- Monitor and optimize simulation performance
- Manage complex experiment workflows

## Installation

### Basic Installation

```bash
pip install .
```

### With All Dependencies

```bash
pip install .[all]
```

### With Development Tools

```bash
pip install .[dev]
```

## Quick Start

### 1. Basic Brain Simulation

```python
import pynlm

# Create default configuration
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
    if step % 20 == 0:
        print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")

print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
```

### 2. Experiment Management

```python
import pynlm

# Create experiment with preset
experiment_result = pynlm.run_experiment(
    "learning_experiment",
    seed=123,
    max_steps=200
)

# Access results
print(f"Experiment success: {experiment_result.success}")
print(f"Steps completed: {experiment_result.steps_completed}")
print(f"Final performance: {experiment_result.average_firing_rate:.2f}")
```

### 3. Parameter Sweeps

```python
import pynlm

# Define parameter ranges
parameter_ranges = {
    'learning_rate': [0.001, 0.01, 0.1],
    'exploration_rate': [0.1, 0.5, 1.0],
    'novelty_threshold': [0.2, 0.5, 0.8]
}

# Create sweep configuration
from pynlm.experiments import ParameterSweepConfig
sweep_config = ParameterSweepConfig(
    parameters=parameter_ranges,
    random_search=True,
    num_iterations=50
)

# Create experiment template
from pynlm.experiments import ExperimentTemplate
template = ExperimentTemplate(
    name='learning_experiment',
    description='Learning experiment with parameter optimization',
    config={'neuron_count': 1000, 'seed': 42}
)

# Run parameter sweep
from pynlm.experiments import run_parameter_sweep
results = run_parameter_sweep(template, sweep_config)

# Analyze results
best_result = sweep_result.get_best_result()
print(f"Best parameters: {best_result.experiment_name}")
print(f"Best performance: {best_result.average_firing_rate:.2f}")
```

### 4. Multi-Seed Experiments

```python
import pynlm

# Run multi-seed experiment
from pynlm.experiments import run_multi_seed_experiment
base_config = {
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.1
}

results = run_multi_seed_experiment(base_config, num_seeds=10)

# Statistical analysis
from pynlm.experiments import analyze_results, StatisticalResults
stat_results = analyze_results(results)

print(f"Mean firing rate: {stat_results.mean:.2f}")
print(f"Standard deviation: {stat_results.std:.2f}")
print(f"Confidence interval: {stat_results.percentiles[95]:.2f}")
```

### 5. Advanced Research Workflow

```python
import pynlm
from pynlm.research import (
    ResearchWorkflow, create_research_workflow, 
    get_research_designer, run_research_workflow
)

# Create research workflow
designer = get_research_designer()
workflow = designer.create_workflow_from_template(
    'learning_curves',
    'Advanced_Learning_Research',
    max_steps=300,
    enable_parameter_sweeps=True
)

# Add additional steps
workflow.add_step('statistical_analysis', 'statistical_analysis', {
    'results': 'run_experiment',
    'metric': 'average_firing_rate'
})

workflow.add_step('save_results', 'data_management', {
    'results': ['run_experiment'],
    'format': 'json'
})

# Run workflow
results = run_research_workflow(workflow)
print(f"Workflow completed: {len(results)} steps executed")
```

### 6. Visualization

```python
import pynlm
from pynlm.visualization import ResultsVisualizer, VisualizationConfig

# Create visualizer
config = VisualizationConfig(
    figure_size=(12, 8),
    style='seaborn-v0_8',
    save_plots=True,
    output_format='png'
)

visualizer = ResultsVisualizer(config)

# Create and run experiment
experiment_results = pynlm.run_experiment('test_experiment')

# Generate visualizations
fig = visualizer.create_dashboard([experiment_results])
fig.show()  # Interactive dashboard

# Save visualizations
visualizer.save_all_plots('./results')
```

### 7. Performance Monitoring

```python
import pynlm
from pynlm.performance import PerformanceMonitor, start_performance_monitoring

# Create brain and start monitoring
brain = pynlm.create_brain()
monitor = PerformanceMonitor(brain)
monitor.start_monitoring()

# Run simulation
for step in range(100):
    brain.step(step)
    monitor.profile_step(step, 0.0)  # Step timing would go here

# Stop monitoring and get metrics
metrics = monitor.stop_monitoring()

print(f"Simulation time: {metrics.simulation_time:.2f}s")
print(f"Memory usage: {metrics.memory_usage_mb:.1f} MB")
print(f"CPU usage: {metrics.cpu_usage:.1f}%")
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
└── examples/           # Example scripts
    ├── basic_simulation.py
    ├── agent_world.py
    ├── phase3_experiment.py
    ├── phase4_experiment.py
    ├── phase6_experiment.py
    ├── advanced_research.py
    └── visualization_examples.py
```

## Key Features

### Core Features
- **Neural Simulation**: Brain initialization, stepping, and state management
- **Configuration Management**: JSON/YAML support with presets
- **Experiment Templates**: Pre-built templates for common research patterns
- **Parameter Sweeps**: Grid search, random search, Bayesian optimization
- **Multi-Seed Experiments**: Statistical validation across multiple runs

### Advanced Features
- **Research Workflows**: High-level APIs for automated experiments
- **Statistical Analysis**: Built-in statistical tests and confidence intervals
- **Results Visualization**: Interactive dashboards and publication-ready plots
- **Performance Monitoring**: Resource usage tracking and optimization
- **Reproducibility Tools**: Seed management and checkpointing
- **Integration APIs**: Direct access to all C++ features

### Research Capabilities
- **Learning Experiments**: Learning curve analysis and optimization
- **Memory Studies**: Memory capacity and retention experiments
- **Social Dynamics**: Multi-agent interaction studies
- **Development Studies**: Cognitive development and maturation
- **Parameter Optimization**: Automatic parameter tuning and optimization

## Configuration Management

### File Formats
- **JSON**: `config.json`
- **YAML**: `config.yaml`
- **Simple**: `config.cfg`

### Preset Categories
- **Research**: Scientific, Speed, Reproducibility
- **Performance**: Small, Medium, Large
- **Research Questions**: Learning Curves, Memory Capacity, Social Dynamics
- **Parameter Optimization**: Grid Search, Random Search, Bayesian Optimization

### Usage Examples

```python
# Load configuration from file
from pynlm import Config
config = Config()
config.load_from_file('experiment_config.yaml')

# Save configuration
config.save_to_file('my_experiment.json')

# Use preset configuration
config = Config.load_from_preset('research_scientific')
```

## Experiment Management

### Basic Experiment
```python
from pynlm import Experiment

experiment = Experiment('my_experiment', {
    'neuron_count': 1000,
    'max_steps': 200,
    'seed': 42
})

result = experiment.run()
```

### Experiment Templates
```python
from pynlm import get_research_templates

# Access all templates
research_templates = get_research_templates()

# Create experiment from template
template = research_templates['learning_curves']
experiment = template.create_experiment(
    learning_rate=0.01,
    exploration_rate=0.5
)
```

### Parameter Sweeps
```python
from pynlm import ParameterSweep, ParameterSweepConfig

sweep_config = ParameterSweepConfig(
    parameters={'learning_rate': [0.001, 0.01, 0.1]},
    random_search=True,
    num_iterations=20
)

# Run sweep
results = run_parameter_sweep(research_template, sweep_config)
```

## Visualization

### Available Plot Types
- **Time Series**: Performance over time
- **Scatter Plots**: Experiment comparison
- **Histograms**: Distribution analysis
- **Box Plots**: Statistical summaries
- **Comparison Charts**: Multi-experiment analysis
- **Heatmaps**: Correlation matrices
- **Interactive Dashboards**: Real-time monitoring

### Usage Examples

```python
from pynlm import ResultsVisualizer

visualizer = ResultsVisualizer()

# Plot experiment results
visualizer.plot_comparison_chart(
    results=experiment_results,
    metrics=['average_firing_rate', 'final_spike_count']
)

# Create interactive dashboard
dashboard = visualizer.create_dashboard(experiment_results)
dashboard.show()
```

## Performance Monitoring

### Resource Monitoring
```python
from pynlm import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.start_monitoring()

# Run simulation
brain.step(step)

# Get metrics
metrics = monitor.stop_monitoring()
print(f"CPU: {metrics.cpu_usage:.1f}%")
print(f"Memory: {metrics.memory_usage_mb:.1f} MB")
```

### Memory Profiling
```python
from pynlm import MemoryProfiler

profiler = MemoryProfiler()
profiler.take_snapshot("before_simulation")

# Run simulation
brain.step(step)

profiler.take_snapshot("after_simulation")

# Analyze memory usage
comparison = profiler.compare_snapshots("before_simulation", "after_simulation")
```

## Research Workflows

### Building Workflows
```python
from pynlm import ResearchWorkflow

# Create workflow
workflow = ResearchWorkflow('my_research', 'Research workflow description')

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

### Workflow Templates
```python
from pynlm import ScientificExperimentDesigner

designer = ScientificExperimentDesigner()

# Create workflow from template
workflow = designer.create_workflow_from_template(
    'learning',
    'my_learning_research',
    max_steps=300,
    enable_parameter_sweeps=True
)
```

## Examples Directory

The `examples/` directory contains comprehensive examples for different use cases:

1. **basic_simulation.py** - Basic brain simulation
2. **agent_world.py** - Complete agent-world simulation
3. **phase3_experiment.py** - Phase 3 world interaction experiment
4. **phase4_experiment.py** - Phase 4 cognitive development experiment
5. **phase6_experiment.py** - Phase 6 integration experiment
6. **advanced_research.py** - Advanced research with parameter sweeps
7. **visualization_examples.py** - Visualization and results analysis

Each example demonstrates specific features and can be run independently.

## Getting Started with Research

### Step 1: Set Up Environment
```bash
# Install the package
pip install .[all]

# Create a project directory
mkdir my_research_project
cd my_research_project

# Initialize a Python environment
python -m venv env
source env/bin/activate  # On Windows: env\Scripts\activate
pip install -e .
```

### Step 2: Design Your Experiment
1. **Define Research Question**: What are you studying?
2. **Choose Template**: Use existing templates or create custom ones
3. **Configure Parameters**: Set up neural and simulation parameters
4. **Plan Sweeps**: Decide which parameters to optimize
5. **Design Analysis**: Plan statistical validation methods

### Step 3: Run Your Experiment
```python
# Example experiment script
import pynlm
from pynlm import run_multi_seed_experiment, analyze_results

# Define base configuration
base_config = {
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.1
}

# Run multi-seed experiment for statistical validation
results = run_multi_seed_experiment(base_config, num_seeds=10)

# Analyze results
stat_results = analyze_results(results)
print(f"Mean firing rate: {stat_results.mean:.2f} ± {stat_results.std:.2f}")

# Visualize results
from pynlm import ResultsVisualizer
visualizer = ResultsVisualizer()
visualizer.plot_statistical_analysis(results, 'average_firing_rate')
```

### Step 4: Interpret and Iterate
1. **Analyze Results**: Check for expected patterns and anomalies
2. **Validate Hypotheses**: Test your research questions
3. **Optimize Parameters**: Use parameter sweeps for improvement
4. **Document Findings**: Record all configurations and results
5. **Share Results**: Use built-in export functions for sharing

## Advanced Topics

### Machine Learning Integration
```python
# Use scikit-learn for advanced analysis
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import StandardScaler

# Prepare data for analysis
X = np.array([[r.average_firing_rate, float(r.final_spike_count)]
              for r in results])
y = np.array([r.seed for r in results])

# Apply regression
model = LinearRegression()
model.fit(X, y)
score = model.score(X, y)
print(f"Model R² score: {score:.3f}")
```

### Parallel Processing
```python
from pynlm import run_parameter_sweep
import multiprocessing

# Configure parallel execution
sweep_config = ParameterSweepConfig(
    parameters={'learning_rate': [0.001, 0.01, 0.1]},
    parallel_execution=True,
    parallel_workers=4
)

# Run parallel sweep
results = run_parameter_sweep(research_template, sweep_config)
```

### Reproducibility
```python
from pynlm import ExperimentReproducibilityManager

# Create reproducibility manager
manager = ExperimentReproducibilityManager('./reproducible_research')

# Register experiment
manager.register_experiment(
    'advanced_learning',
    {
        'parameters': {'learning_rate': 0.01, 'seed': 42},
        'timestamp': time.time()
    },
    {'description': 'Advanced learning experiment'}
)

# Log results
manager.register_result('advanced_learning', experiment_result)
```

## Troubleshooting

### Common Issues

**Issue: Missing dependencies**
```bash
pip install .[all]
```

**Issue: Simulation too slow**
```python
# Use performance optimization
from pynlm import start_performance_monitoring
monitor = start_performance_monitoring()

# Reduce neuron count for testing
config.set('neuron_count', 100)
```

**Issue: Visualization not working**
```python
# Check matplotlib backend
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend

# Install system dependencies
# Ubuntu: sudo apt-get install libfreetype6-dev libpng-dev
# macOS: brew install freetype libpng
```

**Issue: Configuration loading errors**
```python
from pynlm import Config

# Try different formats
try:
    config.load_from_file('config.yaml')
except Exception as e:
    print(f"YAML error: {e}")
    config.load_from_file('config.json')
```

### Getting Help

- **Documentation**: See `docs/` directory for detailed documentation
- **Examples**: Check `examples/` directory for working examples
- **Community**: Join the NLM research community
- **GitHub**: Report issues on GitHub repository

## License

This package is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgements

This package is built on the NLM C++ framework (Phase 6 implementation). It provides a comprehensive Python interface to access all neural simulation capabilities while maintaining high performance and research-grade functionality.

## Version History

- **Version 1.0.0**: Initial release with core functionality
- **Features**: Core simulation, experiment management, research workflows
- **Future versions**: Enhanced machine learning integration, distributed computing

## Contributing

Contributions are welcome! Please see the `CONTRIBUTING.md` file for guidelines on contributing to this package.

## Contact

For questions, suggestions, or issues:
- Email: info@nlm.ai
- GitHub: https://github.com/nlm/pynlm
- Discord: https://discord.gg/nlm

---

*Created by the NLM Research Team for scientific neural simulation and research.*
