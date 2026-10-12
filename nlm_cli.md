# NLM CLI - Command Line Interface Tools

## Overview

The NLM CLI provides a comprehensive set of command-line tools for working with NLM (Neural Learning Machine) simulations. It includes utilities for:

- Brain simulation control
- Configuration management
- Data export/import
- Performance analysis
- Experiment management
- Debug and diagnostic tools

## Installation

The CLI tools are included with the NLM Python package. You can use them after installing pynlm:

```bash
pip install pynlm
```

## Usage

The main CLI tool is called `nlm-cli`. You can invoke it with various subcommands:

```bash
nlm-cli --help
```

### Basic Usage Examples

```bash
# Run a simple brain simulation
nlm-cli simulate --steps 1000 --neurons 500

# Run with configuration file
nlm-cli simulate -c config.json -s 2000

# Export brain data to file
nlm-cli export --input brain.bin --output brain.json

# Compare two simulation results
nlm-cli compare results1.json results2.json

# Analyze performance
nlm-cli analyze performance.log --stats

# Create backup of brain state
nlm-cli backup brain.bin --output backup_$(date +%Y%m%d).bin
```

## Subcommands

### simulate

Run a brain simulation with specified parameters.

**Usage:**
```bash
nlm-cli simulate [OPTIONS] --steps N
```

**Options:**
```
--steps, -s          Number of simulation steps (required)
--neurons, -n       Number of neurons (default: 1000)
--regions, -r       Number of brain regions (default: 1)
--config, -c        Configuration file path
--checkpoint, -k    Checkpoint file path
--output, -o        Output file path
--verbose, -v       Enable verbose output
--quiet, -q         Quiet mode (minimal output)
--benchmark         Enable benchmarking
```

**Example:**
```bash
nlm-cli simulate --steps 2000 --neurons 2000 --config sim_config.json
```

### config

Manage NLM configurations.

**Usage:**
```bash
nlm-cli config [COMMAND]
```

**Commands:**
```
create    Create a new configuration file
edit      Edit an existing configuration file
view      View configuration file contents
export    Export configuration to different formats
import    Import configuration from different formats
list      List all available configurations
remove    Remove a configuration file
```

**Examples:**
```bash
# Create a new configuration
nlm-cli config create my_config.json

# View configuration
nlm-cli config view my_config.json

# Export configuration to Python format
nlm-cli config export my_config.json --format python
```

### export

Export brain data to various formats.

**Usage:**
```bash
nlm-cli export [OPTIONS] --input INPUT --output OUTPUT
```

**Options:**
```
--input, -i          Input file path (brain binary format)
--output, -o         Output file path
--format, -f         Output format (json, csv, txt, python)
--include-stats      Include performance statistics
--include-neurons    Include detailed neuron information
--include-synapses   Include detailed synapse information
--compress          Compress output files
```

**Example:**
```bash
nlm-cli export --input brain.bin --output brain_analysis.json --format json --include-stats
```

### import

Import data from various formats.

**Usage:**
```bash
nlm-cli import [OPTIONS] --input INPUT --output OUTPUT
```

**Options:**
```
--input, -i          Input file path
--output, -o         Output file path (brain binary format)
--format, -f         Input format (json, csv, txt)
--validate          Validate imported data before saving
```

**Example:**
```bash
nlm-cli import --input brain_data.json --output brain.bin --format json
```

### compare

Compare two simulation results or brain states.

**Usage:**
```bash
nlm-cli compare [OPTIONS] FILE1 FILE2
```

**Options:**
```
--output, -o         Output file path for comparison report
--format, -f         Output format (json, txt, html)
--diff              Show detailed differences
--similarity        Calculate similarity metrics
--stats             Include performance statistics in comparison
--neurons           Compare neuron information
--synapses          Compare synapse information
--memory            Compare memory states
```

**Example:**
```bash
nlm-cli compare simulation1.json simulation2.json --diff --stats
```

### analyze

Analyze simulation data and performance.

**Usage:**
```bash
nlm-cli analyze [OPTIONS] INPUT_FILE
```

**Options:**
```
--output, -o         Output file path for analysis report
--stats, -s         Analyze performance statistics
--neurons, -n       Analyze neuron dynamics
--synapses, -y      Analyze synapse dynamics
--memory, -m        Analyze memory systems
--plot, -p          Generate plots (requires matplotlib)
--all               Analyze all available data
```

**Example:**
```bash
nlm-cli analyze simulation_stats.json --stats --neurons --synapses --output analysis_report.txt
```

### backup

Create backups of brain states and configurations.

**Usage:**
```bash
nlm-cli backup [OPTIONS] INPUT_FILE
```

**Options:**
```
--output, -o         Output file path
--compress          Compress backup file
--encrypt           Encrypt backup file (requires encryption key)
--verify            Verify backup integrity
--list, -l          List backup files
--remove, -r        Remove backup file
```

**Examples:**
```bash
# Create simple backup
nlm-cli backup brain.bin

# Create compressed backup
nlm-cli backup brain.bin --compress

# List backup files
nlm-cli backup --list
```

### benchmark

Benchmark NLM performance.

**Usage:**
```bash
nlm-cli benchmark [OPTIONS]
```

**Options:**
```
--steps, -s         Number of steps to benchmark (default: 1000)
--neurons, -n       Number of neurons (default: 1000)
--repeats, -r       Number of benchmark runs (default: 3)
--output, -o        Output file path
--detailed         Enable detailed performance analysis
--compare          Compare with reference performance
```

**Example:**
```bash
nlm-cli benchmark --steps 5000 --neurons 2000 --repeats 5 --output benchmark_results.json
```

### train

Train or fine-tune neural networks using NLM.

**Usage:**
```bash
nlm-cli train [OPTIONS] DATASET
```

**Options:**
```
--model, -m         Model file path
--epochs, -e        Number of training epochs (default: 10)
--learning-rate, -l Learning rate (default: 0.01)
--batch-size, -b    Batch size (default: 32)
--output, -o        Output model file path
--validation        Run validation during training
--early-stop        Enable early stopping
```

**Example:**
```bash
nlm-cli train dataset.json --epochs 50 --learning-rate 0.001 --batch-size 64
```

### test

Test a trained model.

**Usage:**
```bash
nlm-cli test [OPTIONS] MODEL_FILE DATASET
```

**Options:**
```
--output, -o        Output file path for test results
--metrics, -m       Metrics to compute (accuracy, loss, etc.)
--batch-size, -b    Batch size (default: 32)
--verbose, -v       Enable verbose output
```

**Example:**
```bash
nlm-cli test trained_model.bin test_dataset.json --metrics accuracy loss --output test_results.json
```

### validate

Validate NLM models and configurations.

**Usage:**
```bash
nlm-cli validate [OPTIONS]
```

**Options:**
```
--config, -c        Configuration file to validate
--model, -m         Model file to validate
--format, -f        Validation format (json, yaml, xml)
--output, -o        Output file path for validation report
--strict           Enable strict validation mode
```

**Example:**
```bash
nlm-cli validate --config sim_config.json --model trained_model.bin --output validation_report.txt
```

### debug

Debug NLM simulations and issues.

**Usage:**
```bash
nlm-cli debug [OPTIONS]
```

**Options:**
```
--input, -i         Input file path
--output, -o        Output file path for debug information
--trace, -t         Enable detailed tracing
--memory, -m       Enable memory analysis
--performance, -p  Enable performance analysis
--log-level, -l    Logging level (debug, info, warn, error)
```

**Example:**
```bash
nlm-cli debug --input problem_simulation.json --trace --memory --performance
```

### help

Show help information for all commands.

**Usage:**
```bash
nlm-cli help [COMMAND]
```

**Example:**
```bash
nlm-cli help simulate
```

## Configuration File

The NLM CLI uses a configuration file (`~/.nlm-cli/config.json`) to store default settings:

```json
{
  "default_steps": 1000,
  "default_neurons": 1000,
  "default_regions": 1,
  "output_directory": "./output",
  "checkpoint_directory": "./checkpoints",
  "log_level": "info",
  "enable_benchmarking": false
}
```

You can override these defaults by setting environment variables:

```bash
export NLM_DEFAULT_STEPS=2000
export NLM_OUTPUT_DIR=/path/to/output
```

## Advanced Features

### Script Mode

For automated processing, you can use the CLI in script mode:

```bash
#!/bin/bash
# Run simulation with logging
nlm-cli simulate --steps 1000 --neurons 500 --verbose > simulation.log 2>&1

# Export results
nlm-cli export --input brain.bin --output analysis.json --format json

# Run analysis
nlm-cli analyze analysis.json --stats --neurons > analysis_report.txt
```

### Pipeline Mode

Chain multiple commands together:

```bash
nlm-cli simulate --steps 500 --neurons 1000 | \
nlm-cli export --format json --output results.json | \
nlm-cli analyze --stats --neurons
```

### Custom Commands

You can create custom commands by creating scripts in the `~/.nlm-cli/commands/` directory:

```bash
# ~/.nlm-cli/commands/my-custom-command
#!/bin/bash
# Custom NLM command implementation
nlm-cli simulate --steps 1000
```

Make the script executable and it will be available as a subcommand:

```bash
nlm-cli my-custom-command
```

## Error Handling

The CLI provides detailed error messages and exit codes:

- Exit code 0: Success
- Exit code 1: General error
- Exit code 2: Invalid arguments
- Exit code 3: File not found
- Exit code 4: Permission denied

## Installation and Setup

### System Installation

For system-wide installation:

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install python3-dev python3-pip cmake build-essential

# Install NLM CLI globally
pip install pynlm

# Install CLI tools
pip install nlm-cli
```

### Development Installation

For development installation:

```bash
# Clone the repository
cd /path/to/nlm

# Install in development mode
pip install -e .

# Install additional development dependencies
pip install pytest pytest-cov mypy black flake8
```

### Environment Setup

Create a virtual environment for NLM development:

```bash
# Create virtual environment
python3 -m venv ~/.venvs/nlm

# Activate virtual environment
source ~/.venvs/nlm/bin/activate

# Install NLM
pip install -e .

# Install CLI tools
pip install nlm-cli
```

## Troubleshooting

### Common Issues

**Issue: Command not found**
```bash
# Ensure NLM CLI is in your PATH
export PATH="$PATH:/usr/local/bin"

# Or use full path
/usr/local/bin/nlm-cli simulate --steps 1000
```

**Issue: Missing dependencies**
```bash
# Install required dependencies
pip install pyyaml pandas matplotlib seaborn
```

**Issue: Performance problems**
```bash
# Reduce simulation complexity
nlm-cli simulate --steps 500 --neurons 500 --quiet

# Use benchmark mode to identify bottlenecks
nlm-cli benchmark --steps 100 --detailed
```

**Issue: Memory issues**
```bash
# Reduce memory usage
nlm-cli simulate --steps 100 --neurons 100 --no-checkpoint

# Use smaller brain configurations
nlm-cli simulate --steps 100 --neurons 100
```

### Getting Help

For help with specific commands:

```bash
nlm-cli help simulate
nlm-cli help config
nlm-cli help export
```

For general help:

```bash
nlm-cli --help
nlm-cli help
```

### Support

If you encounter issues, you can:

1. Check the documentation: `nlm-cli help` or visit the NLM documentation
2. Report issues: Visit the NLM GitHub repository
3. Get help: Join the NLM community forum or mailing list

## License

This CLI tool is part of the NLM project and is licensed under the MIT License.

## Authors

The NLM CLI was developed by the NLM project team to provide comprehensive command-line utilities for working with NLM simulations.