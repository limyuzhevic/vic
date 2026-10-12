# Enhanced NLM Python Bindings

This directory contains the enhanced Python bindings for the Neural Learning Machine (NLM) project, transforming the low-level C++ interface into a high-level, Pythonic API for research.

## Overview

The enhanced bindings provide:

1. **High-level research APIs** for brain management and experiment control
2. **Pythonic configuration management** with property access
3. **Comprehensive visualization** using matplotlib and plotly
4. **Statistical analysis** and data processing tools
5. **Research workflows** for common research patterns
6. **Real-time monitoring** and integration capabilities

## File Structure

```
pynlm/
├── research_api.py          # High-level research API
│   ├── BrainManager         # Brain lifecycle management
│   ├── ExperimentManager    # Research experiment management
│   ├── ResearchSession      # Individual experiment sessions
│   ├── PythonConfig         # Python-friendly configuration
│   └── LearningExperimentAPI, IntegrationTestAPI, etc.
├── viz_api.py              # Visualization API
│   ├── MatplotlibVisualizer # Static matplotlib plots
│   └── PlotlyVisualizer     # Interactive plotly plots
├── analysis_api.py         # Analysis API
│   ├── PandasDataFrameExporter # Data export to pandas
│   ├── StatisticalAnalysis     # Statistical analysis tools
│   └── ResearchDataProcessor  # Data processing
└── examples/               # Example scripts
    ├── basic_example.py
    ├── research_workflow_example.py
    ├── viz_analysis_example.py
    └── integration_realtime_example.py
```

## Getting Started

### Installation

Install the enhanced NLM Python bindings:

```bash
pip install pynlm

# Or install with all dependencies
pip install pynlm[examples,visualization,advanced_ml]
```

### Basic Usage

```python
import pynlm

# Create brain manager
brain = pynlm.BrainManager()

# Configure brain
brain.set_learning_rate(0.01)
brain.set_neuron_count(1000)

# Run experiment
results = brain.run_experiment(steps=1000)

# Get metrics
metrics = brain.get_metrics()
print(f"Total spikes: {metrics['total_spike_count']}")

# Create visualizer
visualizer = pynlm.create_matplotlib_visualizer(brain)
visualizer.plot_neural_activity()
visualizer.show_all_plots()
```

### Research Workflows

```python
import pynlm

# Create research session
with pynlm.create_research_session() as session:
    for step in range(500):
        session.run_step(step)
        if step % 100 == 0:
            session.collect_metrics()

# Get results
results = session.get_results()
analysis = pynlm.create_statistical_analysis(results['step_data'])
```

## Core Classes

### BrainManager

High-level brain management interface for research experiments.

**Key Features:**
- Brain lifecycle management
- Experiment execution
- State save/load
- Metrics collection
- Visualization support

**Usage:**
```python
brain = pynlm.BrainManager(config)
brain.set_learning_rate(0.01)
brain.set_neuron_count(1000)
results = brain.run_experiment(steps=1000)
```

### ExperimentManager

Research experiment management with support for:
- Learning experiments
- Integration tests
- Parameter tuning
- Ablation studies
- Cross-validation

**Usage:**
```python
exp_manager = pynlm.ExperimentManager()
learning_result = exp_manager.conduct_learning_experiment(steps=500)
param_tuning = exp_manager.parameter_tuning(param_space, n_trials=10)
```

### ResearchSession

Individual experiment session with context manager support.

**Key Features:**
- Context manager support (`with` statements)
- Step-by-step execution
- Metrics collection
- State management

**Usage:**
```python
with pynlm.create_research_session() as session:
    for step in range(100):
        session.run_step(step)
        session.collect_metrics()
    
    results = session.get_results()
```

### PythonConfig

Python-friendly configuration management.

**Key Features:**
- JSON/YAML file loading
- Pythonic property access
- Factory methods for common configurations
- Type conversion and validation

**Usage:**
```python
config = pynlm.PythonConfig(
    learning_rate=0.01,
    neuron_count=1000,
    brain_type='standard'
)

print(f"Learning rate: {config.learning_rate}")
config.learning_rate = 0.05

config.load_json("config.json")
config.save_json("new_config.json")
```

## Visualization APIs

### MatplotlibVisualizer

Static visualization using matplotlib.

**Key Features:**
- Neural activity plots
- Memory system metrics
- Learning progress tracking
- Integration status visualization
- Export to various formats

**Usage:**
```python
import pynlm

visualizer = pynlm.create_matplotlib_visualizer(brain_manager)
visualizer.plot_neural_activity()
visualizer.plot_memory_metrics()
visualizer.plot_learning_progress()
visualizer.show_all_plots()
```

### PlotlyVisualizer

Interactive visualization using plotly.

**Key Features:**
- Interactive dashboards
- Real-time metrics
- Parameter exploration
- Web-based sharing
- Responsive design

**Usage:**
```python
import pynlm

visualizer = pynlm.create_plotly_visualizer(brain_manager)
dashboard = visualizer.create_interactive_dashboard()
exploration_plots = visualizer.create_exploration_plots()
visualizer.show_interactive()
```

## Analysis APIs

### PandasDataFrameExporter

Export simulation results to pandas DataFrames.

**Key Features:**
- Export simulation results
- Export step-by-step data
- Export memory data
- Multiple output formats (CSV, JSON, Excel)

**Usage:**
```python
exporter = pynlm.create_pandas_exporter()
results_df = exporter.export_simulation_results(brain_manager)
exporter.export_to_csv(results_df, 'results.csv')
```

### StatisticalAnalysis

Comprehensive statistical analysis tools.

**Key Features:**
- Descriptive statistics
- Hypothesis testing (t-test, ANOVA, chi-square)
- Correlation analysis
- Trend analysis
- Regression analysis
- Clustering analysis
- Time series analysis

**Usage:**
```python
analysis = pynlm.create_statistical_analysis(data)
describe_results = analysis.describe()
t_test_results = analysis.t_test(group1, group2)
corr_results = analysis.correlation_analysis(x, y)
```

## Research Workflows

### Learning Experiment API

Conduct learning experiments with different strategies.

```python
learning_api = pynlm.LearningExperimentAPI()
basic_learning = learning_api.run_basic_learning(steps=1000)
plasticity_learning = learning_api.run_plasticity_learning(steps=2000)
development_learning = learning_api.run_development_learning(steps=5000)
```

### Integration Test API

Run system integration tests.

```python
integration_api = pynlm.IntegrationTestAPI()
complete_integration = integration_api.run_complete_integration(steps=10000)
memory_integration = integration_api.run_memory_integration(steps=3000)
neuromodulation_integration = integration_api.run_neuromodulation_integration(steps=2000)
```

### Parameter Tuning API

Hyperparameter optimization using various search strategies.

```python
param_tuning_api = pynlm.ParameterTuningAPI()
learning_rate_tuning = param_tuning_api.tune_learning_rate([0.001, 0.005, 0.01, 0.05])
network_size_tuning = param_tuning_api.tune_network_size([500, 1000, 2000, 5000])
```

### Ablation Study API

Test system components by disabling them individually or in combination.

```python
ablation_api = pynlm.AblationStudyAPI()
memory_ablation = ablation_api.disable_memory_systems(steps=500)
neuromodulation_ablation = ablation_api.disable_neuromodulation(steps=500)
plasticity_ablation = ablation_api.disable_plasticity(steps=500)
```

### Cross-Validation API

Validate model performance using cross-validation.

```python
cv_api = pynlm.CrossValidationAPI()
learning_cv = cv_api.validate_learning(n_folds=5)
memory_cv = cv_api.validate_memory(n_folds=5)
```

## Examples

The `examples/` directory contains comprehensive examples:

### Basic Example

Demonstrates basic usage of the enhanced bindings.

### Research Workflow Example

Shows a complete research workflow with learning experiments, parameter tuning, and analysis.

### Visualization and Analysis Example

Comprehensive visualization and statistical analysis examples.

### Integration and Real-Time Monitoring Example

Real-time monitoring and integration with multiple sessions.

To run an example:

```bash
python examples/basic_example.py
python examples/research_workflow_example.py
python examples/viz_analysis_example.py
python examples/integration_realtime_example.py
```

## Documentation

### API Reference

- **Research API Guide:** `RESEARCH_API_GUIDE.md`
- **Visualization API Guide:** `VISUALIZATION_API.md`
- **Analysis API Guide:** `ANALYSIS_API.md`
- **Examples Guide:** `EXAMPLES.md`

### Usage Examples

- **Basic Usage:** See examples in the `examples/` directory
- **Research Workflows:** Follow the research workflow examples
- **Visualization:** Use the visualization APIs for plotting
- **Analysis:** Use the analysis APIs for statistical analysis

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

### Installation Options

```bash
# Install core package
pip install pynlm

# Install with all dependencies
pip install pynlm[examples,visualization,advanced_ml]

# Install with dev dependencies
pip install pynlm[dev]

# Install with visualization only
pip install pynlm[visualization]

# Install with advanced ML
pip install pynlm[advanced_ml]
```

## Building from Source

If you want to build from source, use scikit-build:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

## Testing

Run the test suite:

```bash
python test_enhanced_bindings.py
```

## Troubleshooting

### Common Issues

1. **Import Errors:** Ensure all dependencies are installed
2. **Plot Display Issues:** Use Agg backend for headless environments
3. **Memory Issues:** Monitor memory usage for large datasets
4. **Performance Issues:** Use vectorized operations and efficient algorithms

### Tips for Best Performance

1. **For Large Datasets:** Use data sampling and efficient algorithms
2. **For Real-Time Applications:** Use background threads and efficient plot updates
3. **For Complex Analysis:** Use appropriate statistical tests and data preprocessing
4. **For Visualization:** Cache plots and use hardware acceleration

## Acknowledgments

The enhanced NLM Python bindings are built on the contributions of:

- **NumPy:** Numerical computing library
- **Pandas:** Data manipulation library
- **Matplotlib:** Plotting library
- **Plotly:** Interactive visualization library
- **scikit-learn:** Machine learning library
- **SciPy:** Scientific computing library
- **statsmodels:** Statistical modeling library

We thank them for providing these powerful tools for scientific research.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Version Information

- **Version:** 0.5.0
- **Release Date:** October 12, 2026
- **Python Support:** >= 3.8
- **Documentation:** Available in the docs/ directory

## Getting Help

### Online Resources

- **Documentation:** https://nlm.readthedocs.io
- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Issues:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

### Support

For support with the enhanced NLM Python bindings:

1. **Check Documentation:** Review the examples and API guides
2. **Report Issues:** Create GitHub issues with detailed error messages
3. **Ask Questions:** Post questions on GitHub Discussions
4. **Provide Feedback:** Share your experiences and suggestions

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface for conducting research with the Neural Learning Machine framework. Whether you're running simple simulations, conducting complex experiments, or performing advanced statistical analysis, the enhanced bindings provide the tools you need.

The enhanced bindings maintain backward compatibility with the existing C++ bindings while providing new, high-level APIs that leverage Python's strengths for scientific research and analysis. This makes NLM more accessible to researchers and scientists who prefer Python for their work.

Start exploring the enhanced NLM Python bindings today and transform your research workflows!