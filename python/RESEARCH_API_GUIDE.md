# NLM Enhanced Research API Guide

## Overview

This guide provides documentation for the enhanced Python bindings for the Neural Learning Machine (NLM) project. The enhanced bindings transform the low-level C++ interface into a high-level, Pythonic API suitable for scientific research.

## What This Guide Covers

1. **Core API Classes**: High-level brain management and experiment control
2. **Configuration API**: Python-friendly configuration management
3. **Visualization API**: Interactive and static visualizations
4. **Analysis API**: Statistical analysis and data processing
5. **Research Workflows**: Common research patterns and examples
6. **Performance Considerations**: Optimization tips for large-scale research

## Quick Start

```python
import pynlm

# Create a brain manager
brain = pynlm.BrainManager()

# Run a learning experiment
results = brain.run_experiment(steps=1000)

# Visualize results
pynlm.MatplotlibVisualizer(brain).plot_learning_progress()

# Save results
brain.save_state("experiment_results.bin")
```

## Core API Classes

### BrainManager

The `BrainManager` class provides high-level interface for brain lifecycle management.

**Methods:**

- `__init__(config=None, brain_type='standard')` - Initialize with optional config
- `run_experiment(steps, **kwargs)` - Run simulation experiment
- `save_state(path)` - Save brain state
- `load_state(path)` - Load brain state
- `get_metrics(**kwargs)` - Get performance metrics
- `visualize(**kwargs)` - Generate visualization

**Properties:**

- `learning_rate` - Get/set learning rate
- `neuron_count` - Get/set neuron count

**Example:**

```python
import pynlm

# Create brain manager with default configuration
brain = pynlm.BrainManager()

# Configure brain
brain.set_learning_rate(0.01)
brain.set_neuron_count(1000)

# Run experiment
results = brain.run_experiment(steps=1000)

# Get metrics
metrics = brain.get_metrics()
print(f"Total spikes: {metrics['total_spike_count']}")
print(f"Active neurons: {metrics['active_neuron_count']}")
```

### ExperimentManager

The `ExperimentManager` class provides methods for conducting research experiments.

**Methods:**

- `conduct_learning_experiment(**kwargs)` - Conduct learning experiments
- `run_integration_test(**kwargs)` - Run system integration tests
- `parameter_tuning(param_space, **kwargs)` - Perform hyperparameter optimization
- `ablation_study(components_to_disable, **kwargs)` - Test system components
- `cross_validation(**kwargs)` - Validate model performance

**Example:**

```python
import pynlm

# Create experiment manager
exp_manager = pynlm.ExperimentManager()

# Conduct parameter tuning
best_params = exp_manager.parameter_tuning(
    param_space={
        'neuron_count': [500, 1000, 2000],
        'learning_rate': [0.01, 0.001, 0.0001]
    },
    n_trials=10
)

print(f"Best parameters: {best_params}")
```

### ResearchSession

The `ResearchSession` class provides context manager support for experiment sessions.

**Methods:**

- `__enter__()` - Enter context manager
- `__exit__()` - Exit context manager
- `run_step(step)` - Run single simulation step
- `run_steps(steps)` - Run multiple simulation steps
- `collect_metrics()` - Collect current metrics
- `get_results()` - Get session results

**Example:**

```python
import pynlm
from pynlm import ResearchSession

# Use research session with context manager
with ResearchSession() as session:
    for step in range(1000):
        session.run_step(step)
        if step % 100 == 0:
            session.collect_metrics()

# Analyze results
results = session.get_results()
analysis = pynlm.StatisticalAnalysis(results)
analysis.t_test(group1, group2)
```

## Configuration API

### PythonConfig

The `PythonConfig` class provides Python-friendly configuration management.

**Methods:**

- `__init__(**kwargs)` - Initialize with keyword arguments
- `load_json(path)` - Load from JSON file
- `load_yaml(path)` - Load from YAML file
- `save_json(path)` - Save to JSON file
- `get(key, default)` - Get configuration value
- `set(key, value)` - Set configuration value

**Pythonic Property Access:**

```python
import pynlm

# Create configuration
config = pynlm.PythonConfig(
    learning_rate=0.01,
    neuron_count=1000,
    brain_type='standard'
)

# Pythonic property access
print(f"Learning rate: {config.learning_rate}")
config.learning_rate = 0.05
print(f"Updated learning rate: {config.learning_rate}")

# Load from file
config.load_json("config.json")
config.save_json("new_config.json")
```

### Factory Methods

**PythonConfig factory methods:**

- `createDefault()` - Create default configuration
- `createLearningExperiment()` - Create configuration for learning experiments
- `createIntegrationTest()` - Create configuration for integration tests
- `createParameterTuning()` - Create configuration for parameter tuning

## Visualization API

### MatplotlibVisualizer

The `MatplotlibVisualizer` class provides matplotlib-based visualization.

**Methods:**

- `plot_neural_activity(step_range=None)` - Plot neural activity
- `plot_memory_metrics()` - Plot memory system metrics
- `plot_learning_progress()` - Plot learning progress
- `plot_integration_status()` - Plot integration status
- `show_all_plots()` - Display all generated plots
- `save_plot(name, path)` - Save plot to file

**Example:**

```python
import pynlm

# Create visualizer
visualizer = pynlm.MatplotlibVisualizer()

# Generate plots
visualizer.plot_neural_activity()
visualizer.plot_memory_metrics()
visualizer.plot_learning_progress()

# Display plots
visualizer.show_all_plots()

# Save plots
visualizer.save_plot('neural_activity', 'neural_activity.png')
```

### PlotlyVisualizer

The `PlotlyVisualizer` class provides interactive plotly-based visualization.

**Methods:**

- `create_interactive_dashboard()` - Create interactive dashboard
- `plot_real_time_metrics()` - Plot real-time metrics
- `create_exploration_plots()` - Create parameter exploration plots
- `show_interactive(figure_names=None)` - Display interactive plots
- `save_interactive(name, path)` - Save interactive plot to HTML

**Example:**

```python
import pynlm

# Create interactive visualizer
visualizer = pynlm.PlotlyVisualizer()

# Create dashboard
dashboard = visualizer.create_interactive_dashboard()

# Create exploration plots
plots = visualizer.create_exploration_plots()

# Display plots
visualizer.show_interactive()

# Save dashboard
dashboard.save_html("dashboard.html")
```

## Analysis API

### PandasDataFrameExporter

The `PandasDataFrameExporter` class provides data export to pandas DataFrames.

**Methods:**

- `export_simulation_results(brain_manager, metrics=None)` - Export simulation results
- `export_step_data(brain_manager, steps)` - Export step-by-step data
- `export_memory_data(brain_manager)` - Export memory data
- `export_to_csv(data, path)` - Export DataFrame to CSV
- `export_to_json(data, path)` - Export DataFrame to JSON
- `export_to_excel(data, path)` - Export DataFrame to Excel

### StatisticalAnalysis

The `StatisticalAnalysis` class provides statistical analysis tools.

**Methods:**

- `t_test(group1, group2)` - Perform t-test
- `correlation_analysis(x, y)` - Perform correlation analysis
- `trend_analysis(data, window_size=10)` - Perform trend analysis
- `significance_test(hypothesis, data)` - Perform significance test
- `anova_test(groups)` - Perform ANOVA test
- `chi_square_test(observed, expected=None)` - Perform chi-square test
- `regression_analysis(x, y)` - Perform regression analysis
- `clustering_analysis(data, n_clusters=3)` - Perform clustering analysis
- `time_series_analysis(data, forecast_periods=5)` - Perform time series analysis
- `generate_report()` - Generate statistical report

**Example:**

```python
import pynlm
import pandas as pd

# Create sample data
data = pd.DataFrame({
    'group1': [1.2, 1.5, 1.8, 2.0, 2.2],
    'group2': [1.0, 1.3, 1.6, 1.9, 2.1],
    'time': [0, 10, 20, 30, 40]
})

# Perform statistical analysis
analysis = pynlm.create_statistical_analysis(data)

# Perform t-test
t_test_results = analysis.t_test(data['group1'], data['group2'])
print(f"T-test p-value: {t_test_results['p_value']}")

# Perform correlation analysis
corr_results = analysis.correlation_analysis(data['time'], data['group1'])
print(f"Correlation: {corr_results.correlation}")

# Generate report
report = analysis.generate_report()
print(report)
```

## Research Workflows

### Learning Experiment Workflow

```python
import pynlm
from pynlm import LearningExperimentAPI

# Create learning experiment API
learning_api = LearningExperimentAPI()

# Run different types of learning experiments
basic_learning = learning_api.run_basic_learning(
    steps=1000,
    learning_rate=0.01,
    neuron_count=1000
)

plasticity_learning = learning_api.run_plasticity_learning(
    steps=2000,
    learning_rate=0.05,
    enable_stdp=True,
    enable_hebbian=True
)

 development_learning = learning_api.run_development_learning(
    steps=5000,
    learning_rate=0.001,
    enable_development=True
)
```

### Integration Test Workflow

```python
import pynlm
from pynlm import IntegrationTestAPI

# Create integration test API
integration_api = IntegrationTestAPI()

# Run different types of integration tests
complete_integration = integration_api.run_complete_integration(
    steps=10000,
    enable_checkpointing=True,
    enable_replay=True,
    enable_development=True
)

memory_integration = integration_api.run_memory_integration(
    steps=3000
)

neuromodulation_integration = integration_api.run_neuromodulation_integration(
    steps=2000
)
```

### Parameter Tuning Workflow

```python
import pynlm
from pynlm import ParameterTuningAPI

# Create parameter tuning API
param_tuning_api = ParameterTuningAPI()

# Tune different hyperparameters
learning_rate_tuning = param_tuning_api.tune_learning_rate(
    learning_rates=[0.001, 0.005, 0.01, 0.05, 0.1],
    n_trials=20
)

network_size_tuning = param_tuning_api.tune_network_size(
    neuron_counts=[500, 1000, 2000, 5000],
    n_trials=20
)
```

### Ablation Study Workflow

```python
import pynlm
from pynlm import AblationStudyAPI

# Create ablation study API
ablation_api = AblationStudyAPI()

# Run different ablation studies
memory_ablation = ablation_api.disable_memory_systems(
    steps=500
)

neuromodulation_ablation = ablation_api.disable_neuromodulation(
    steps=500
)

plasticity_ablation = ablation_api.disable_plasticity(
    steps=500
)

cognition_ablation = ablation_api.disable_cognition(
    steps=500
)
```

### Cross-Validation Workflow

```python
import pynlm
from pynlm import CrossValidationAPI

# Create cross-validation API
cv_api = CrossValidationAPI()

# Perform cross-validation
learning_cv = cv_api.validate_learning(n_folds=5)

memory_cv = cv_api.validate_memory(n_folds=5)
```

## Examples

### Example 1: Basic Brain Simulation

```python
import pynlm
import time

# Create brain manager
brain = pynlm.BrainManager()

# Configure brain
brain.set_learning_rate(0.01)
brain.set_neuron_count(1000)

# Run experiment with timing
start_time = time.time()
results = brain.run_experiment(steps=1000)
end_time = time.time()

print(f"Experiment completed in {end_time - start_time:.2f} seconds")
print(f"Total spikes: {results.metrics.get('total_spike_count', 'N/A')}")
print(f"Active neurons: {results.metrics.get('active_neuron_count', 'N/A')}")

# Visualize results
visualizer = pynlm.create_matplotlib_visualizer(brain)
visualizer.plot_neural_activity()
visualizer.show_all_plots()
```

### Example 2: Research Session Management

```python
import pynlm
from pynlm import ResearchSession

# Create and use research session
with ResearchSession() as session:
    for step in range(500):
        session.run_step(step)
        
        # Collect metrics every 50 steps
        if step % 50 == 0:
            metrics = session.collect_metrics()
            print(f"Step {step}: {metrics}")

# Get final results
results = session.get_results()
print(f"Final step count: {results['step_count']}")
print(f"Session active: {results['session_active']}")
```

### Example 3: Advanced Analysis

```python
import pynlm
import pandas as pd

# Create sample data for analysis
sample_data = pd.DataFrame({
    'experiment': ['A', 'B', 'C', 'A', 'B', 'C'] * 10,
    'performance': np.random.normal(0.8, 0.1, 60),
    'memory_usage': np.random.uniform(0.5, 0.9, 60),
    'learning_rate': np.random.uniform(0.001, 0.1, 60)
})

# Create statistical analysis
analysis = pynlm.create_statistical_analysis(sample_data)

# Describe data
describe_results = analysis.describe()
for column, stats in describe_results.items():
    print(f"{column}: mean={stats.mean:.3f}, std={stats.std:.3f}")

# Correlation analysis
corr_results = analysis.correlation_analysis(
    sample_data['learning_rate'], 
    sample_data['performance']
)
print(f"Correlation: {corr_results.correlation:.3f} (p={corr_results.p_value:.3f})")

# Generate report
report = analysis.generate_report()
print(report)
```

### Example 4: Parameter Optimization

```python
import pynlm
from pynlm import ExperimentManager

# Create experiment manager
exp_manager = ExperimentManager()

# Define parameter space for optimization
param_space = {
    'learning_rate': [0.001, 0.005, 0.01, 0.05, 0.1],
    'neuron_count': [500, 1000, 2000, 5000],
    'connection_probability': [0.05, 0.1, 0.2]
}

# Convert to format suitable for parameter tuning
param_space_json = {
    'learning_rate': param_space['learning_rate'],
    'neuron_count': param_space['neuron_count'],
    'connection_probability': param_space['connection_probability']
}

# Run parameter tuning
best_params = exp_manager.parameter_tuning(
    param_space=param_space_json,
    n_trials=20,
    metric='performance'
)

print(f"Best parameters found: {best_params}")

# Run experiment with best parameters
brain = exp_manager.get_brain_manager()
if brain:
    brain.set_learning_rate(best_params.get('learning_rate', 0.01))
    brain.set_neuron_count(best_params.get('neuron_count', 1000))
    
    results = brain.run_experiment(steps=1000)
    print(f"Best experiment results: {results.metrics}")
```

## Installation

### Requirements

The enhanced NLM Python bindings require:

- Python >= 3.8
- NumPy >= 1.20
- Pandas >= 1.3.0
- Matplotlib >= 3.5.0
- Plotly >= 5.0.0
- SciPy >= 1.7.0
- scikit-learn >= 1.0.0
- statsmodels >= 0.13.0

### Installation from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install the enhanced bindings
pip install -e .
```

### Installation with Dev Dependencies

```bash
pip install -e .[dev]
```

### Installation with Examples

```bash
pip install -e .[examples]
```

### Installation with Visualization

```bash
pip install -e .[visualization]
```

### Installation with Advanced ML

```bash
pip install -e .[advanced_ml]
```

## Building from Source

### Prerequisites

- CMake >= 3.16
- Python development headers
- pybind11 >= 2.11.0

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Install
make install
```

## Testing

### Run Tests

```bash
# Run unit tests
pytest tests/

# Run integration tests
pytest tests/integration/

# Run performance tests
pytest tests/performance/
```

### Test Coverage

To run tests with coverage:

```bash
pytest --cov=pynlm --cov-report=html tests/
```

## Performance Considerations

### Large-Scale Simulations

For large-scale research simulations, consider these optimization tips:

1. **Batch Processing**: Use batch processing for multiple experiments
2. **Memory Management**: Monitor memory usage during long-running simulations
3. **Parallel Execution**: Consider running multiple simulations in parallel
4. **Checkpointing**: Use checkpointing for long experiments
5. **Data Sampling**: Sample data periodically to reduce I/O overhead

### Visualization Performance

For interactive visualizations:

1. **Lazy Loading**: Load data lazily for large datasets
2. **Downsampling**: Downsample data for better performance
3. **Caching**: Cache generated plots
4. **Hardware Acceleration**: Use hardware acceleration for rendering

### Analysis Performance

For statistical analysis of large datasets:

1. **Vectorized Operations**: Use vectorized operations where possible
2. **Parallel Processing**: Use parallel processing for independent computations
3. **Memory Efficiency**: Use memory-efficient data structures
4. **Algorithm Selection**: Choose appropriate algorithms for dataset size

## Troubleshooting

### Common Issues

1. **Import Errors**: Ensure all dependencies are installed
2. **Build Errors**: Check CMake configuration and compiler settings
3. **Performance Issues**: Monitor system resources and optimize code
4. **Memory Issues**: Check for memory leaks and optimize memory usage

### Getting Help

- **Documentation**: Check the online documentation at https://nlm.readthedocs.io
- **Issues**: Report issues at https://github.com/nlm-project/nlm/issues
- **Community**: Join the NLM community on GitHub Discussions

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contributing

We welcome contributions to the NLM project! Please see the CONTRIBUTING.md file for guidelines on how to contribute.

## Acknowledgments

The NLM project is built on the contributions of many researchers and developers. We thank our community for their support and contributions.

## Version History

- **Version 0.1.0**: Initial release of enhanced Python bindings
- **Version 0.2.0**: Added advanced visualization APIs
- **Version 0.3.0**: Added comprehensive analysis APIs
- **Version 0.4.0**: Added research workflow APIs
- **Version 0.5.0**: Added performance optimizations and bug fixes

## Future Roadmap

1. **Machine Learning Integration**: Integration with TensorFlow and PyTorch
2. **Real-time Monitoring**: Real-time monitoring and visualization
3. **Cloud Computing**: Support for cloud-based simulations
4. **Distributed Computing**: Distributed computing for large-scale simulations
5. **Advanced Analytics**: Advanced analytics and AI-driven insights

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface for conducting research with the Neural Learning Machine framework. Whether you're running simple simulations, conducting complex experiments, or performing advanced statistical analysis, the enhanced bindings provide the tools you need to conduct cutting-edge research in neuroscience and artificial intelligence.

The enhanced bindings maintain backward compatibility with the existing C++ bindings while providing new, high-level APIs that leverage Python's strengths for scientific research and analysis. This makes NLM more accessible to researchers and scientists who prefer Python for their work.

## References

1. NLM Documentation: https://nlm.readthedocs.io
2. NLM GitHub Repository: https://github.com/nlm-project/nlm
3. Related Publications: See docs/RESEARCH_METHODS.md for research papers and references