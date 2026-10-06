# NLM - Neural Learning Machine CLI Tools

Advanced command-line tools for neuroscience research providing sophisticated functionality
for researchers. This package provides comprehensive experiment management, parameter
exploration, neural data analysis, and visualization capabilities for neuroscience
research.

## Overview

NLM CLI Tools is a research-oriented simulation framework that leverages the existing
NLM (Neural Learning Machine) C++ neural simulation framework through Python bindings.
The tools provide:

- **Experiment Management**: Comprehensive control over neuroscience experiments
- **Parameter Space Exploration**: Systematic parameter space traversal
- **Neural Data Analysis**: Advanced statistical analysis methods
- **Visualization Tools**: Sophisticated visualizations for research results
- **Batch Processing**: Parallel execution and results aggregation
- **Research Workflow**: Experiment pipelines and reproducible research

## Installation

### Prerequisites

- Python 3.8 or higher
- NumPy (>= 1.20)
- pip (Python package installer)

### Installation Steps

```bash
# Clone the repository
cgit clone https://github.com/nlm-project/nlm.git

# Navigate to the tools directory
cd nlm/nlx_tools

# Install Python dependencies
pip install -r requirements.txt

# Build and install the package
pip install -e .

# Verify installation
nlx-tools --version
```

### Quick Start

```bash
# Create a simple experiment
nlx-run --name quick_test --neurons 500 --steps 2000

# Run parameter sweep
nlx-parameter-sweep --base-config config.json --parameters rate:0.5,1.0,1.5;decay:0.9,0.99,0.999

# Analyze neural data
nlx-analyze --file spike_data.json --type advanced

# Create visualization
nlx-visualize --file results.json --type heatmap
```

## Core Tools

### nlx-run

**Purpose**: Run neuroscience experiments with comprehensive control

**Features**:
- Experiment configuration management
- Real-time monitoring and visualization
- Automated experiment parameterization
- Results recording and analysis
- Comparison with baseline experiments

**Usage**:
```bash
nlx-run --name <experiment_name> \
        --neurons <neuron_count> \
        --steps <simulation_steps> \
        --rate <base_firing_rate> \
        --config <config_file> \
        --output-format <json|csv|npy|hdf5>
```

**Example**:
```bash
nlx-run \
  --name excitatory_dynamics \
  --neurons 1000 \
  --steps 5000 \
  --rate 1.2 \
  --config my_config.json \
  --output-format json
```

### nlx-parameter-sweep

**Purpose**: Systematically explore parameter space

**Features**:
- Multi-dimensional parameter space exploration
- Efficient parameter space traversal
- Statistical analysis of parameter effects
- Visualization of parameter-response relationships
- Identification of optimal parameter combinations

**Usage**:
```bash
nlx-parameter-sweep \
  --base-config <base_config.json> \
  --parameters <parameter_definitions> \
  --jobs <num_parallel_jobs>
```

**Parameter Definition Format**:
```
<parameter_name>:<value1>,<value2>,...,<valueN>;<parameter_name2>:<value1>,<value2>,...

Example:
rate:0.5,1.0,1.5;decay:0.9,0.99,0.999;noise:0.01,0.1,1.0
```

**Example**:
```bash
nlx-parameter-sweep \
  --base-config default_config.json \
  --parameters \
    rate:0.5,1.0,1.5;decay:0.9,0.99,0.999 \
  --jobs 4
```

### nlx-analyze

**Purpose**: Analyze neural data with advanced statistical methods

**Features**:
- Spike train analysis
- Synaptic connectivity analysis
- Network topology analysis
- Activity pattern analysis
- Statistical hypothesis testing

**Usage**:
```bash
nlx-analyze --file <data_file> [--type <basic|advanced>]
                     [--output-format <json|csv|npy|hdf5>]
```

**Example**:
```bash
nlx-analyze \
  --file neural_data.json \
  --type advanced \
  --output-format json
```

### nlx-visualize

**Purpose**: Create visualizations of neural data and results

**Features**:
- Neural activity heatmaps
- Connectivity graphs
- Time series analysis plots
- Network dynamics visualization
- Statistical visualization

**Usage**:
```bash
nlx-visualize --file <results_file> [--type <heatmap|connectivity|time_series>]
                         [--output-format <png|pdf|svg>]
```

**Example**:
```bash
nlx-visualize \
  --file experiment_results.json \
  --type heatmap \
  --output-format pdf
```

## Additional Tools

### nlx-batch-experiments

Run multiple experiments in parallel with resource management, experiment queuing, and results aggregation.

### nlx-workflow

Create and manage research workflows with pipeline scripting, experiment chaining, and reproducible research.

### nlx-journal

Create interactive research notebooks with code documentation, results tracking, and collaborative features.

### nlx-compare

Compare results from different experiments with statistical comparison, visualization comparison, and performance analysis.

### nlx-train

Train neural network models on experimental data with automated training, hyperparameter optimization, and model validation.

### nlx-predict

Make predictions using trained models with uncertainty quantification and confidence intervals.

### nlx-optimize

Optimize brain parameters using machine learning with evolutionary algorithms and constraint handling.

### nlx-share

Share experiments and results with cloud storage integration, version control, and access control.

### nlx-reproduce

Reproduce published experiments with parameter replication, results verification, and methodology documentation.

### nlx-statistics

Statistical analysis of neural data with advanced statistical testing and distribution analysis.

### nlx-metrics

Calculate performance metrics including neural performance, learning metrics, efficiency metrics, and scalability metrics.

### nlx-forecast

Forecast neural activity with time series forecasting, pattern prediction, and uncertainty quantification.

### nlx-convert

Convert data formats and units with format conversion, unit conversion, and data transformation capabilities.

### nlx-validate

Validate data and experiments with comprehensive validation checks, format validation, and quality assurance.

### nlx-monitor

Monitor system performance with real-time monitoring, resource usage tracking, and performance profiling.

## Configuration

### Configuration File Format

Create a configuration file (e.g., `config.json`) with the following structure:

```json
{
  "default_neuron_count": 1000,
  "default_max_steps": 10000,
  "default_connection_probability": 0.1,
  "output_format": "json",
  "progress_interval": 1000,
  "enable_real_time_monitoring": true,
  "auto_save_interval": 5000,
  "base_firing_rate": 1.0,
  "firing_decay": 0.9995,
  "noise_level": 0.1,
  "neural_dynamics": {
    "type": "lif",
    "v_rest": -70.0,
    "v_thresh": -50.0,
    "tau": 20.0
  },
  "plasticity": {
    "type": "stdp",
    "learning_rate": 0.01,
    "time_constant": 20.0
  },
  "memory": {
    "working_memory_enabled": true,
    "episodes_stored": 100,
    "consolidation_threshold": 0.8
  },
  "development": {
    "enabled": true,
    "plasticity_rate": 0.01,
    "maturation_rate": 0.005
  }
}
```

### Configuration Commands

```bash
# Set configuration values
nlx-tools config set default_neuron_count 2000
nlx-tools config set output_format csv

# Get configuration values
nlx-tools config get default_neuron_count
nlx-tools config get all

# Save configuration
nlx-tools config save

# Reset to defaults
nlx-tools config reset
```

## Research Features

### Reproducibility

- **Session Tracking**: Every experiment run is tracked with unique session IDs
- **Configuration Logging**: All experiment parameters are automatically logged
- **Random Seed Management**: Deterministic random number generation for reproducibility
- **Version Control Integration**: Automatic version tracking and backup

### Collaboration

- **Cloud Storage Integration**: Export results to cloud storage platforms
- **Access Control**: Fine-grained permissions for experiment access
- **Data Sharing**: Share results with collaborators securely
- **Version History**: Track all changes to experiments and results

### Scalability

- **Parallel Processing**: Multiple experiments can run simultaneously
- **Memory Management**: Efficient memory usage for large-scale simulations
- **Distributed Computing**: Support for distributed computing across multiple nodes
- **Resource Monitoring**: Real-time resource usage tracking

### Analysis Capabilities

- **Statistical Testing**: Advanced statistical tests for hypothesis validation
- **Pattern Recognition**: Automated pattern detection in neural data
- **Anomaly Detection**: Identify unusual patterns in experimental results
- **Model Comparison**: Compare different models and parameter sets

## Performance and Optimization

### Efficient Algorithms

- Optimized neural simulation algorithms for large-scale experiments
- Parallel processing for parameter sweeps
- Memory-efficient data structures
- Fast I/O operations for large datasets

### Monitoring and Profiling

- Real-time performance monitoring
- Resource usage tracking
- Performance profiling tools
- Automated optimization suggestions

## Integration

### Integration with Other Tools

- **Python Libraries**: Integration with NumPy, SciPy, and other scientific libraries
- **Jupyter Notebooks**: Direct integration with Jupyter for interactive analysis
- **MATLAB**: Export results for MATLAB analysis
- **R**: R-compatible data formats for statistical analysis

### APIs and Scripting

- **Programmatic API**: Python API for programmatic access
- **Command-line Interface**: Rich command-line interface for automation
- **REST API**: Web API for remote access and integration
- **Configuration Files**: JSON-based configuration for easy automation

## Examples

### Example 1: Basic Neural Simulation

```python
from nlx_tools import NLXTools

tools = NLXTools()

# Create an experiment
experiment = tools.create_experiment(
    name="basic_simulation",
    config={
        "neuron_count": 500,
        "max_steps": 2000,
        "base_firing_rate": 1.0,
        "connection_probability": 0.1
    }
)

# Run the experiment
result = tools.run_experiment(experiment)
```

### Example 2: Parameter Optimization

```python
from nlx_tools import NLXTools

tools = NLXTools()

# Define base configuration
base_config = {
    "neuron_count": 1000,
    "max_steps": 5000,
    "base_firing_rate": 1.0
}

# Define parameter space for optimization
parameter_space = {
    "rate": [0.5, 1.0, 1.5, 2.0],
    "decay": [0.9, 0.95, 0.99, 0.999],
    "noise": [0.01, 0.1, 1.0]
}

# Run parameter sweep
results = tools.parameter_sweep(base_config, parameter_space, n_jobs=4)
```

### Example 3: Advanced Neural Analysis

```python
from nlx_tools import NLXTools

tools = NLXTools()

# Analyze neural data
analysis_result = tools.analyze_neural_data(
    data_file="spike_train_data.json",
    analysis_type="advanced"
)

# Visualize results
viz_result = tools.visualize_results(
    results_file="analysis_results.json",
    plot_type="heatmap"
)
```

## Development

### Building from Source

```bash
# Clone the repository
cgit clone https://github.com/nlm-project/nlm.git

# Navigate to the project directory
cd nlm

# Build using CMake
mkdir build
cd build
cmake ..
make -j4
```

### Running Tests

```bash
# Run unit tests
cd build
make test

# Or run specific tests
ctest -R <test_name>
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a pull request

### Code Style

- Follow PEP 8 for Python code
- Use type hints for better code documentation
- Write comprehensive docstrings
- Include unit tests for new functionality
- Use meaningful variable and function names

## Troubleshooting

### Common Issues

1. **Installation Failed**
   - Ensure all dependencies are installed
   - Check Python version compatibility
   - Try installing with `pip install --user -e .`

2. **Command Not Found**
   - Ensure the package is installed in your environment
   - Check PATH environment variable
   - Use `pip show nlx-tools` to verify installation

3. **Configuration Errors**
   - Check configuration file format
   - Validate JSON syntax
   - Use `nlx-tools config validate` command

4. **Performance Issues**
   - Reduce experiment size
   - Use parallel processing
   - Check system resources
   - Monitor memory usage

5. **Data Format Errors**
   - Check file format compatibility
   - Convert data to required format
   - Use appropriate file extensions

## Support

### Getting Help

- **Documentation**: Visit [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed information
- **Examples**: Check the examples directory for practical usage examples
- **Community**: Join the NLM research community for collaboration
- **Issues**: Report bugs and feature requests at GitHub issues

### Contact

- **Email**: research@nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

- The NLM (Neural Learning Machine) framework for the underlying neural simulation capabilities
- Python community for excellent libraries and tools
- Research collaborators for feedback and insights
- Contributors to the open-source community

---

© 2026 NLM Project. All rights reserved.
