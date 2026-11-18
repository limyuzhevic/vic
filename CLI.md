# Command Line Interface

**nlm-cli** is a modern, user-friendly command-line interface for NLM (Neural Learning Machine). It provides comprehensive control over brain simulations, experiments, and configuration management.

## Overview

The CLI features:
- **Modern Argument Parser**: Uses modern C++17 with argparse-like interface
- **Subcommands**: `run`, `benchmark`, `config`, `visualize`, `experiment`, `profile`
- **Configuration Files**: JSON/YAML support for configuration
- **Output Formats**: Human-readable, JSON, CSV, table formats
- **Progress Reporting**: Real-time progress bars and statistics
- **Experiment Management**: Create, run, and manage experiments
- **Advanced Features**: Profiling, batch processing, scripting
- **Help System**: Comprehensive help with examples

## Installation

```bash
# Install nlm-cli
pip install nlm-cli

# Or build from source
cd nlm
mkdir build
cd build
cmake -DNLM_BUILD_PYTHON_BINDINGS=ON ..
make -j$(nproc)

# Install the CLI tool
cp nlm-cli /usr/local/bin/
```

## Getting Started

### Quick Start Examples

```bash
# Run a simple simulation
nlm run --brain-size 500 --steps 100

# Run with a specific configuration
nlm run config.json --output results.json

# Run benchmark for different network sizes
nlm benchmark --neurons 100,500,1000 --steps 1000

# Create and run an experiment
nlm experiment create my_exp --brain-size 500
nlm experiment run my_exp

# Interactive mode
nlm interactive
```

### Help and Usage

```bash
# Show main help
nlm help

# Show help for specific command
nlm help run

# Show all available commands
nlm commands
```

## Commands

### run - Run a brain simulation

Run a neural brain simulation with various options.

**Options:**
```bash
# Basic simulation
nlm run --brain-size 1000 --steps 100

# With configuration file
nlm run my_config.json

# With world simulation
nlm run --brain-size 500 --world-width 20 --world-height 20

# With agent and learning
nlm run --brain-size 500 --with-agent --enable-curiosity

# Progress and output
nlm run --brain-size 500 --steps 1000 --output results.json --verbose
```

**Arguments:**
- `--brain-size N`: Number of neurons (default: 500)
- `--steps N`: Number of simulation steps (default: 100)
- `--config FILE`: Load configuration from file
- `--output FILE`: Save results to file
- `--world-width N`: World width in tiles
- `--world-height N`: World height in tiles
- `--with-agent`: Include agent-brain interface
- `--enable-curiosity`: Enable curiosity-driven exploration
- `--enable-reward`: Enable reward modulation
- `--enable-development`: Enable developmental stages
- `--verbose`: Enable verbose output
- `--quiet`: Suppress output
- `--save-checkpoint`: Save brain state periodically

### benchmark - Performance benchmarking

Run performance benchmarks for different network configurations.

**Options:**
```bash
# Benchmark single configuration
nlm benchmark --neurons 100 --steps 1000

# Benchmark multiple configurations
nlm benchmark --neurons 100,500,1000 --steps 1000 --output benchmark.json

# Detailed benchmark with profiling
nlm benchmark --neurons 1000 --steps 500 --profile --output results.csv

# Custom benchmark
nlm benchmark --neurons 500 --steps 2000 --name "production_test"
```

**Arguments:**
- `--neurons N,K,M`: Neuron counts to benchmark (comma-separated)
- `--steps N`: Number of simulation steps (default: 1000)
- `--output FILE`: Save results to file
- `--profile`: Enable performance profiling
- `--name STRING`: Benchmark name/label
- `--csv`: Output in CSV format
- `--json`: Output in JSON format
- `--table`: Output in human-readable table format

### config - Configuration management

Manage NLM configuration files and settings.

**Subcommands:**
```bash
nlm config list                    # List all configuration files
nlm config create <name>           # Create new configuration
nlm config view <name>             # View configuration
nlm config update <name>           # Update configuration
nlm config export <name> <format>  # Export configuration
nlm config import <file>           # Import configuration
nlm config validate <file>         # Validate configuration file
```

**Examples:**
```bash
# Create a basic configuration
nlm config create production --brain-size 1000 --learning-rate 0.01

# View configuration
nlm config view production

# Export configuration
nlm config export production json > config.json
```

### experiment - Experiment management

Create, run, and manage scientific experiments.

**Subcommands:**
```bash
nlm experiment create <name>                    # Create experiment
nlm experiment run <name>                       # Run experiment
nlm experiment list                              # List experiments
nlm experiment view <name>                      # View experiment details
nlm experiment results <name>                    # Show experiment results
nlm experiment compare <name1> <name2>           # Compare experiments
nlm experiment export <name> <format>            # Export results
nlm experiment delete <name>                     # Delete experiment
```

**Options:**
- `--brain-size N`: Number of neurons
- `--learning-rate R`: Learning rate
- `--max-steps N`: Maximum simulation steps
- `--seed N`: Random seed for reproducibility
- `--description TEXT`: Experiment description
- `--tags T1,T2,T3`: Tags for categorization

### visualize - Generate visualizations

Create visual representations of simulation results.

**Options:**
```bash
# Create spike train visualization
nlm visualize --input results.json --type spike-trains --output spike_plot.html

# Create brain connectivity plot
nlm visualize --input results.json --type connectivity --output connectivity.png

# Create activity heatmap
nlm visualize --input results.json --type heatmap --output activity_heatmap.html
```

**Arguments:**
- `--input FILE`: Input results file
- `--type TYPE`: Visualization type (spike-trains, connectivity, heatmap, activity)
- `--output FILE`: Output file
- `--width N`: Image width (default: 1200)
- `--height N`: Image height (default: 800)
- `--format FORMAT`: Output format (html, png, svg)

### profile - Performance profiling

Analyze simulation performance and identify bottlenecks.

**Options:**
```bash
# Profile single simulation
nlm profile --brain-size 500 --steps 1000

# Profile with memory tracking
nlm profile --brain-size 1000 --steps 2000 --memory

# Profile multiple configurations
nlm profile --neurons 100,500,1000 --steps 1000 --output profile.json
```

**Arguments:**
- `--brain-size N`: Number of neurons
- `--steps N`: Number of simulation steps
- `--memory`: Enable memory profiling
- `--output FILE`: Save profiling results
- `--format FORMAT`: Output format (json, csv, text)

### interactive - Interactive mode

Start interactive session for manual control and exploration.

**Features:**
- Run commands interactively
- Inspect brain state
- Modify configuration on the fly
- Start/stop simulations
- View real-time statistics

**Usage:**
```bash
nlm interactive
```

## Configuration Files

NLM supports JSON configuration files for specifying simulation parameters:

### Basic Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "scale": 1.0
        }
    }
}
```

### Command Line Configuration
Command line options override configuration file values.

## Output Formats

The CLI supports multiple output formats:

### Human-readable
- Default format for terminal output
- Tables, progress bars, and statistics
- Interactive display

### JSON
- Structured data format
- Machine-readable
- Suitable for programmatic analysis

### CSV
- Comma-separated values
- Spreadsheet compatible
- Easy to import into analysis tools

### Table
- Formatted text tables
- Good for terminal display
- Supports sorting and filtering

## Configuration File Examples

### Basic Simulation
```json
{
    "simulation": {
        "steps": 1000,
        "timestep": 0.001,
        "brain_size": 500
    },
    "learning": {
        "enable_curiosity": true,
        "enable_reward": true,
        "enable_development": true
    }
}
```

### World Simulation
```json
{
    "world": {
        "width": 30,
        "height": 30,
        "vision_width": 12,
        "vision_height": 12,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    },
    "agent": {
        "enable_reward_modulation": true,
        "enable_structural_plasticity": true,
        "enable_curiosity": true
    }
}
```

### Advanced Configuration
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_count": 100000,
        "development_stage": "adult"
    },
    "memory": {
        "working_memory_size": 100,
        "episodic_memory_capacity": 1000
    },
    "prediction": {
        "enable_prediction_error": true,
        "prediction_window": 10
    }
}
```

## Advanced Features

### Batch Processing

Run multiple simulations with different parameters:

```bash
nlm batch run --config-list configs/*.json --output-dir results
nlm batch benchmark --neurons 100,200,500 --steps 1000
```

### Scripting

Create scripts for automated experiments:

```bash
#!/bin/bash
# run_experiment.sh

echo "Starting experiment"
nlm experiment create my_experiment \
    --brain-size 1000 \
    --learning-rate 0.01 \
    --max-steps 5000 \
    --seed 42

nlm experiment run my_experiment

if [ $? -eq 0 ]; then
    nlm experiment export my_experiment results.csv
    echo "Experiment completed successfully"
else
    echo "Experiment failed"
    exit 1
fi
```

### Progress Monitoring

Monitor long-running simulations:

```bash
nlm run --brain-size 1000 --steps 10000 --progress --interval 1000
```

The CLI will show real-time progress with:
- Progress bar
- Current step
- Estimated time remaining
- Key statistics (spikes, firing neurons, etc.)

## Installation and Setup

### From Source

```bash
# Clone repository
cd /path/to/nlm

# Build with Python bindings
mkdir build
cd build
cmake -DNLM_BUILD_PYTHON_BINDINGS=ON ..
make -j$(nproc)

# Install CLI tool
install -m 755 nlm-cli /usr/local/bin/
```

### From Package Manager

```bash
# Using pip (if available)
pip install nlm-cli

# Using package manager
# (Add package manager-specific commands here)
```

### Development Setup

```bash
# Install development dependencies
pip install -r requirements-dev.txt

# Install the package in development mode
pip install -e .

# Test the CLI
nlm help
```

## Troubleshooting

### Common Issues

**Issue: Command not found**
```bash
# Add nlm-cli to PATH
export PATH=$PATH:/path/to/nlm/build

# Or install system-wide
 sudo cp nlm-cli /usr/local/bin/
 sudo chmod +x /usr/local/bin/nlm-cli
```

**Issue: Missing dependencies**
```bash
pip install pybind11 scikit-build-core numpy
```

**Issue: Build errors**
```bash
# Check CMake version
cmake --version

# Ensure Python development packages are installed
# Ubuntu/Debian:
 sudo apt-get install python3-dev python3-pip

# macOS:
brew install python3
```

**Issue: Configuration file not found**
```bash
# Create configuration file
nlm config create my_config

# Edit configuration
nlm config update my_config
```

### Getting Help

For more information:

1. **Online Documentation**: https://nlm.readthedocs.io
2. **GitHub Repository**: https://github.com/nlm-project/nlm
3. **Issues**: https://github.com/nlm-project/nlm/issues
4. **Discussions**: https://github.com/nlm-project/nlm/discussions

### Examples and Templates

Check the `examples/` directory for:
- `minimal.py` - Minimal working example
- `agent_basic.py` - Basic agent implementation
- `world_basic.py` - World-only simulation
- `training.py` - Complete training loop
- `experiment.py` - Experiment template
- `visualization.py` - Visualization script

## Contributing

### Code Contributions
1. Fork the repository
2. Create your feature branch
3. Make your changes
4. Test your changes
5. Submit a pull request

### Documentation Contributions
1. Fork the repository
2. Update documentation files
3. Add examples
4. Improve CLI help text
5. Submit a pull request

### Bug Reports
1. Check existing issues
2. Create new issue if none exists
3. Provide reproduction steps
4. Include relevant logs and output

## License

MIT License - See LICENSE file for details.

---

*This documentation is continuously updated. Check for the latest version in the repository.*
