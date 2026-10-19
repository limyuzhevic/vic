# NLM CLI - Command Line Interface for Batch Processing and Automation

## Overview

The NLM CLI (Neural Learning Machine Command Line Interface) is a comprehensive tool for batch processing and automation of NLM simulations. It provides powerful capabilities for:

- **Batch Processing**: Parameter sweeps, hyperparameter optimization, parallel execution
- **Automation**: Automated experiment generation, pipeline execution, error recovery
- **Workflow Management**: Complex workflow definition, dependency management, progress tracking
- **Result Management**: Result storage, analysis, visualization, export
- **Configuration Management**: Configuration templates, validation, comparison

## Installation

### Prerequisites

The NLM CLI requires:

- Python 3.8 or higher
- Enhanced NLM Python bindings
- NLM Advanced Python utilities

### Installation

```bash
# Clone the repository

# Navigate to the python directory
cd python

# Install dependencies (if not already installed)
pip install -r requirements.txt

# Build the enhanced bindings (if needed)
# This is typically done as part of the project setup

# The NLM CLI is available as a module:
python -c "from nlm_cli import main; main()"
```

### Quick Start

```bash
# Run the examples
cd python
python nlm_cli_examples.py

# Test the CLI
python nlm_cli.py --help
```

## CLI Structure

The NLM CLI is organized into the following main components:

### 1. Core CLI Module

The core module provides:

- Command-line argument parsing with comprehensive options
- Subcommand system for different operations (run, batch, config, etc.)
- Configuration management and validation
- Logging and progress reporting

**Main Entry Point**: `nlm_cli.py`

### 2. Batch Processing Module

The batch module provides:

- Parameter sweep and grid search
- Random search and Bayesian optimization
- Parallel execution with configurable workers
- Result aggregation and analysis

**Key Features**:
- Grid search over multiple parameters
- Random parameter sampling
- Parallel execution for performance
- Comprehensive result analysis

### 3. Experiment Management Module

The experiment module provides:

- Experiment definition and configuration
- Experiment tracking and monitoring
- Results storage and organization
- Experiment comparison and analysis

**Key Features**:
- Experiment definition templates
- Parameter validation and defaults
- Experiment status tracking
- Results comparison

### 4. Workflow Engine Module

The workflow module provides:

- Workflow definition and execution
- Dependency management and task scheduling
- Error recovery and retry mechanisms
- Progress tracking and reporting

**Key Features**:
- Directed acyclic graph (DAG) workflows
- Automatic dependency resolution
- Checkpointing and resumption
- Progress monitoring

### 5. Result Analysis Module

The result module provides:

- Result aggregation and statistical analysis
- Visualization generation
- Report generation
- Data export and archiving

**Key Features**:
- Statistical analysis of results
- Multiple visualization formats
- Comprehensive reporting
- Data export support

## CLI Commands

The NLM CLI provides the following commands:

### `nlm batch run`

Run a batch experiment with parameter sweeps.

**Syntax**:
```bash
nlm batch run <experiment_name> [options]
```

**Options**:
- `--ranges <JSON>`: Parameter ranges for the sweep (JSON format)
- `--fixed-params <JSON>`: Fixed parameters (JSON format)
- `--repetitions <int>`: Number of repetitions (default: 1)
- `--randomize`: Randomize parameter order
- `--output-format <format>`: Output format (json, csv, yaml) (default: json)
- `--save-intermediate`: Save intermediate results

### `nlm batch config`

Manage batch configurations.

**Syntax**:
```bash
nlm batch config <action> [options]
```

**Actions**:
- `create`: Create a batch configuration
- `list`: List available configurations
- `delete`: Delete a configuration

### `nlm config`

Manage configuration templates.

**Syntax**:
```bash
nlm config <action> [options]
```

**Actions**:
- `create`: Create a configuration template
- `load`: Load a configuration template
- `validate`: Validate a configuration

### `nlm workflow`

Manage workflows.

**Syntax**:
```bash
nlm workflow <action> [options]
```

**Actions**:
- `run`: Run a workflow
- `define`: Define a workflow
- `list`: List workflows
- `status`: Show workflow status

### `nlm analysis`

Analyze simulation results.

**Syntax**:
```bash
nlm analysis <action> [options]
```

**Actions**:
- `run`: Run analysis on results
- `compare`: Compare multiple result sets
- `report`: Generate analysis report

### `nlm experiment`

Manage experiments.

**Syntax**:
```bash
nlm experiment <action> [options]
```

**Actions**:
- `create`: Create an experiment
- `run`: Run an experiment
- `list`: List experiments
- `status`: Show experiment status

### `nlm debug`

Debug operations.

**Syntax**:
```bash
nlm debug <action> [options]
```

**Actions**:
- `status`: Show system status
- `logs`: Show system logs
- `config`: Show configuration

## Configuration Files

The NLM CLI supports multiple configuration file formats:

### JSON Configuration

```json
{
  "name": "experiment_name",
  "parameter_ranges": {
    "learning_rate": [0.01, 0.1, 0.5, 1.0],
    "batch_size": [32, 64, 128, 256]
  },
  "fixed_parameters": {
    "time_step_size": 0.01,
    "total_steps": 1000
  },
  "repetitions": 3,
  "randomize_order": true,
  "save_intermediate": true
}
```

### YAML Configuration

```yaml
name: experiment_name
parameter_ranges:
  learning_rate: [0.01, 0.1, 0.5, 1.0]
  batch_size: [32, 64, 128, 256]
fixed_parameters:
  time_step_size: 0.01
  total_steps: 1000
repetitions: 3
randomize_order: true
save_intermediate: true
```

### Template Configuration

Configuration templates can be defined with the following structure:

```json
{
  "name": "template_name",
  "description": "Template description",
  "parameters": {
    "network_type": "LIF",
    "neuron_count": 10000,
    "learning_rate": 0.01,
    "batch_size": 32
  },
  "tags": ["standard", "LIF", "neural_network"],
  "author": "NLM Team",
  "version": "1.0"
}
```

## Workflow Definition

Workflows can be defined using JSON or YAML files with the following structure:

### Simple Workflow

```json
{
  "name": "simple_workflow",
  "tasks": [
    {
      "name": "validate_config",
      "type": "validation",
      "depends_on": []
    },
    {
      "name": "run_sweep",
      "type": "batch",
      "depends_on": ["validate_config"],
      "parameters": {
        "sweep_name": "firing_rate_sweep",
        "repetitions": 5
      }
    },
    {
      "name": "analyze_results",
      "type": "analysis",
      "depends_on": ["run_sweep"],
      "analysis_type": "statistical"
    }
  ],
  "dependencies": {
    "run_sweep": ["validate_config"],
    "analyze_results": ["run_sweep"]
  }
}
```

## Result Formats

### Simulation Results

Simulation results are saved in JSON format with the following structure:

```json
{
  "metadata": {
    "sweep_name": "experiment_name",
    "param_set_name": "param_set_0",
    "parameters": {
      "param1": "value1",
      "param2": 42
    },
    "start_time": "2026-01-01T10:00:00Z",
    "end_time": "2026-01-01T10:05:00Z",
    "duration": 300.0,
    "repetition": 0
  },
  "final_metrics": {
    "final_active_neurons": 8450,
    "final_firing_rate": 4.8,
    "final_e_i_ratio": 1.2,
    "total_spikes": 12345,
    "final_development_stage": 0.85,
    "final_neurons": 10000
  },
  "step_results": [
    {
      "step": 0,
      "time": "2026-01-01T10:00:00Z",
      "active_neurons": 10000,
      "firing_rate": 0.0,
      "e_i_ratio": 1.0,
      "spike_count": 0
    }
  ],
  "config": {
    "param1": "value1",
    "param2": 42
  }
}
```

### Analysis Results

Analysis results are saved in JSON format with the following structure:

```json
{
  "sweep_name": "experiment_name",
  "total_simulations": 45,
  "parameter_names": ["param1", "param2"],
  "metric_analysis": {
    "final_firing_rate": {
      "mean": 5.2,
      "std": 2.1,
      "min": 1.1,
      "max": 8.3,
      "optimal_value": 7.5,
      "optimal_parameters": {
        "param1": "value1",
        "param2": 42
      }
    }
  },
  "correlations": {
    "final_firing_rate": {
      "param1": 0.85,
      "param2": 0.3
    }
  },
  "response_surfaces": {
    "final_firing_rate": {
      "param1": "param1",
      "param2": "param2",
      "metric_values": [1.2, 3.4, 5.6, 7.8],
      "normalized_values": [0.0, 0.3, 0.6, 1.0]
    }
  }
}
```

## Integration with Enhanced Components

The NLM CLI integrates seamlessly with all enhanced NLM components:

### Enhanced Python Bindings

The CLI uses the enhanced Python bindings for:

- Type-safe configuration management
- Better error handling
- Pythonic interfaces
- Automatic type conversion

### Advanced Agent Capabilities

The CLI leverages advanced agent capabilities for:

- Custom agent architectures
- Social learning algorithms
- Hierarchical control systems
- Adaptive behavior modification

### Visualization and Debugging Tools

The CLI integrates with visualization and debugging tools for:

- Real-time neural activity visualization
- Connectivity graph exploration
- Activity profiling
- Performance benchmarking

### Configuration Management System

The CLI uses the configuration management system for:

- Configuration templates
- Parameter search and optimization
- Hyperparameter tuning
- Configuration validation and auditing

### Performance Monitoring Tools

The CLI integrates with performance monitoring tools for:

- Resource usage monitoring
- Simulation progress tracking
- Custom metrics collection
- Auto-tuning suggestions

## User Experience

### Easy to Use

- Intuitive command-line interface with comprehensive help
- Sensible defaults and smart suggestions
- Progressive disclosure of advanced features
- Clear error messages and helpful guidance

### Powerful

- Advanced options for complex workflows
- Configurable parallelization
- Flexible result management
- Extensible plugin architecture

### Robust

- Comprehensive error handling
- Checkpointing and resumption
- Retry mechanisms
- Resource management

### Well-Documented

- Complete command documentation
- Usage examples and tutorials
- Configuration guides
- Troubleshooting sections

## Implementation Plan

The NLM CLI implementation follows these phases:

### Phase 1: Core CLI Framework

- Create core CLI module with argparse
- Implement subcommand system
- Add configuration management and validation
- Implement logging and progress reporting

### Phase 2: Batch Processing Capabilities

- Implement parameter sweep and grid search
- Add random search and Bayesian optimization
- Implement parallel execution
- Add result aggregation and analysis

### Phase 3: Experiment Management System

- Create experiment definition and configuration
- Implement experiment tracking and monitoring
- Add results storage and organization
- Implement experiment comparison and analysis

### Phase 4: Workflow Engine

- Implement workflow definition and execution
- Add dependency management and task scheduling
- Implement error recovery and retry mechanisms
- Add progress tracking and reporting

### Phase 5: Result Analysis

- Implement result aggregation and statistical analysis
- Add visualization generation
- Implement report generation
- Add data export and archiving

### Phase 6: Integration

- Integrate with enhanced NLM components
- Implement configuration management
- Add performance monitoring
- Ensure backward compatibility

### Phase 7: Documentation and Examples

- Create comprehensive documentation
- Add usage examples and tutorials
- Implement configuration guides
- Add troubleshooting sections

### Phase 8: Testing and Validation

- Implement unit tests
- Add integration tests
- Create performance tests
- Validate with real-world scenarios

## Examples

### Basic Batch Processing

```bash
# Run a parameter sweep
nlm batch run firing_rate_sweep --ranges "target_firing_rate:[1.0,2.0,5.0,10.0,20.0]" --repetitions 3

# Run with fixed parameters
nlm batch run my_experiment --ranges "learning_rate:[0.01,0.1],batch_size:[32,64]" \
                           --fixed-params "total_steps:1000,visualization_enabled:false"

# Run with intermediate saves
nlm batch run sweep_experiment --ranges "param1:[1,2,3],param2:[4,5,6]" \
                              --save-intermediate --output-format json
```

### Configuration Management

```bash
# Create a configuration template
nlm config create neural_template --parameters "network_type:LIF,learning_rate:0.01,epochs:100" \
                                 --description "Standard neural network configuration" \
                                 --tags "standard,LIF,neural_network"

# Load a configuration template
nlm config load template_file.json --name my_template

# Validate a configuration
nlm config validate --parameters "learning_rate:0.01,epochs:100"
```

### Workflow Management

```bash
# Define a workflow
nlm workflow define complete_workflow --file workflow_config.json

# Run a workflow
nlm workflow run my_workflow

# Check workflow status
nlm workflow status my_workflow
```

### Result Analysis

```bash
# Analyze simulation results
nlm analysis run results.json --output-format json

# Compare multiple result sets
nlm analysis compare results1.json results2.json

# Generate analysis report
nlm analysis report results.json --format pdf --output my_report.pdf
```

### Experiment Management

```bash
# Create an experiment
nlm experiment create my_experiment --config experiment_config.json

# Run an experiment
nlm experiment run my_experiment

# List experiments
nlm experiment list

# Check experiment status
nlm experiment status my_experiment
```

### Debugging

```bash
# Check system status
nlm debug status --format json

# Check system logs
nlm debug logs --level INFO --tail 100

# Check configuration
nlm debug config --format yaml
```

## Advanced Features

### Parallel Execution

The NLM CLI supports parallel execution for batch processing:

```bash
# Run with 4 workers
nlm batch run my_sweep --ranges "param1:[1,2,3],param2:[4,5,6]" --workers 4

# Run with job queue
nlm batch run my_sweep --ranges "param1:[1,2,3],param2:[4,5,6]" --queue high_priority
```

### Checkpointing and Resumption

The NLM CLI supports checkpointing and resumption:

```bash
# Run with checkpointing
nlm batch run my_sweep --ranges "param1:[1,2,3],param2:[4,5,6]" --checkpoint-interval 100

# Resume interrupted sweep
nlm batch run my_sweep --resume checkpoint_file.json
```

### Custom Plugins

The NLM CLI supports custom plugins for extending functionality:

```bash
# Load custom plugin
nlm --plugin custom_plugin.py batch run my_sweep

# Install custom plugin
pip install custom_nlm_plugin
```

## Troubleshooting

### Common Issues

1. **Enhanced bindings not found**
   - Build the enhanced bindings: `mkdir build && cd build && cmake .. && make -j4`
   - Install dependencies: `pip install numpy matplotlib pandas networkx h5py psutil memory_profiler pyyaml`

2. **Configuration file errors**
   - Check JSON/YAML syntax
   - Validate parameter types
   - Ensure required fields are present

3. **Memory errors**
   - Reduce parallelism
   - Increase system memory
   - Use checkpointing for large experiments

4. **Timeout errors**
   - Increase timeout settings
   - Optimize simulation parameters
   - Reduce batch size

### Getting Help

For help with specific commands:

```bash
nlm batch run --help
nlm config create --help
nlm workflow run --help
nlm analysis run --help
nlm experiment run --help
nlm debug status --help
```

For general help:

```bash
nlm --help
```

## Support and Feedback

### Support

For support, please:

1. Check the documentation for common issues
2. Report bugs at the GitHub repository
3. Join the community for discussions and examples

### Feedback

To provide feedback, please:

1. Report bugs and feature requests at the GitHub repository
2. Share your experiences and best practices
3. Contribute to the documentation and examples

## License

The NLM CLI is part of the NLM (Neural Learning Machine) project and is licensed under the MIT License.

## Copyright

Copyright (c) 2026 NLM Team

All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
