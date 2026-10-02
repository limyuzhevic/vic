# NLM CLI - Command Line Interface for NLM Brain Simulation Project

## Overview

The NLM CLI (Neural Learning Machine Command Line Interface) provides a comprehensive command-line interface for managing brain simulations, experiments, configuration, and analysis in the NLM project.

## Features

### 1. Brain Simulation Commands
- **brain create**: Create new brain simulations
- **brain load**: Load saved brain states
- **brain save**: Save brain states to files
- **brain run**: Run simulations with customizable parameters
- **brain status**: Show current brain status and statistics
- **brain stats**: Get detailed metrics and analysis
- **brain compare**: Compare multiple brain states

### 2. Configuration Management
- **config list**: List all configuration options
- **config get <key>**: Get configuration values
- **config set <key> <value>**: Set configuration values
- **config save <file>**: Save configuration to files
- **config load <file>**: Load configuration from files
- **config validate**: Validate configuration settings

### 3. Experiment Management
- **experiment run <name>**: Run named experiments
- **experiment list**: List available experiments
- **experiment create <name>**: Create new experiment configurations
- **experiment export <name> <file>**: Export experiments to files
- **experiment import <file>**: Import experiments from files

### 4. Brain State Exploration
- **explore visualize**: Generate visualizations of brain states
- **explore neurons**: Analyze neuron population statistics
- **explore synapses**: Analyze synapse connectivity and weights
- **explore memory**: Analyze memory system contents
- **explore simulate**: Run simulations with integrated analysis

### 5. Python Scripting
- **python run <script>**: Execute Python scripts with pynlm bindings
- **python interactive**: Start Python interactive session
- **python benchmark**: Run performance benchmarks

### 6. Utility and Debugging Tools
- **tools benchmark**: Run performance benchmarks
- **tools profile**: Profile simulation performance
- **tools compare**: Compare different configurations
- **tools analyze**: Analyze simulation results
- **tools replay**: Replay saved simulation data
- **tools stats**: Generate summary statistics

### 7. Help and Documentation
- **help**: Show general help and available commands
- **help <topic>**: Get detailed help for specific topics

## Installation and Usage

### Prerequisites

1. **Python 3.10+** (tested with Python 3.10.12)
2. **pyyaml** package (for YAML configuration support)
3. **NLM Python bindings (pynlm)** - The C++ NLM framework with Python bindings

### Installation

```bash
# Install required Python packages
pip3 install pyyaml

# Install NLM Python bindings (if not already installed)
# The NLM C++ framework must be built first
pip3 install -e .  # From the NLM project root directory
```

### Basic Usage

#### Start the CLI

```bash
# Run the CLI from any directory
cd /path/to/nlm/project
./cli.py --help

# Or run directly with python
python3 cli.py brain create --name mybrain
```

#### Create and run a brain simulation

```bash
# Create a new brain simulation
./cli.py brain create --name test_brain

# Run simulation with 1000 steps
./cli.py brain run --steps 1000

# Check brain status
./cli.py brain status
```

#### Configure the simulation

```bash
# List all configuration options
./cli.py config list

# Get specific configuration values
./cli.py config get neuron_count
./cli.py config get simulation_timestep

# Set configuration values
./cli.py config set neuron_count 1000
./cli.py config set simulation_timestep 0.001

# Save configuration
./cli.py config save config.yaml

# Load configuration
./cli.py config load config.yaml
```

#### Run experiments

```bash
# List available experiments
./cli.py experiment list

# Run an experiment
./cli.py experiment run learning_test
```

#### Explore brain states

```bash
# Analyze neuron populations
./cli.py explore neurons --type all

# Analyze synapse connectivity
./cli.py explore synapses --type all

# Generate visualizations
./cli.py explore visualize --type all --output brain_viz.png
```

#### Use Python scripting

```bash
# Run a Python script with pynlm bindings
./cli.py python run my_script.py

# Start interactive Python session
./cli.py python interactive

# Run performance benchmarks
./cli.py python benchmark --steps 1000 --repeats 5
```

#### Use debugging tools

```bash
# Run performance benchmarks
./cli.py tools benchmark --steps 1000

# Profile simulation performance
./cli.py tools profile --steps 1000

# Compare different configurations
./cli.py tools compare config1.yaml config2.yaml

# Analyze simulation results
./cli.py tools analyze results1.bin results2.bin
```

## Configuration File Formats

The CLI supports multiple configuration file formats:

### JSON
```json
{
  "neuron_count": 1000,
  "region_count": 10,
  "simulation_timestep": 0.001,
  "connection_probability": 0.15
}
```

### YAML
```yaml
neuron_count: 1000
region_count: 10
simulation_timestep: 0.001
connection_probability: 0.15
```

### INI (planned for future)
```ini
[neuron]
count = 1000
region_count = 10

simulation
 timestep = 0.001
connection_probability = 0.15
```

## Command Reference

### Brain Commands

#### brain create
Create a new brain simulation.

**Options:**
- `--name <name>`: Name for the brain (required)
- `--config <file>`: Configuration file path

**Example:**
```bash
./cli.py brain create --name mybrain
```

#### brain run
Run simulation with parameters.

**Options:**
- `--steps <n>`: Number of simulation steps (default: 1000)
- `--config <file>`: Configuration file path

**Example:**
```bash
./cli.py brain run --steps 500
```

#### brain compare
Compare multiple brain states.

**Arguments:**
- `filepaths`: Paths to brain files to compare (at least 2 required)

**Example:**
```bash
./cli.py brain compare brain1.bin brain2.bin
```

### Configuration Commands

#### config set
Set configuration values.

**Arguments:**
- `key`: Configuration key
- `value`: Configuration value (automatically parsed as int, float, or string)

**Example:**
```bash
./cli.py config set neuron_count 1000
./cli.py config set simulation_timestep 0.001
```

#### config get
Get configuration values.

**Arguments:**
- `key`: Configuration key

**Example:**
```bash
./cli.py config get neuron_count
./cli.py config get simulation_timestep
```

### Explore Commands

#### explore neurons
Analyze neuron population statistics.

**Options:**
- `--type <type>`: Neuron type (all, sensory, motor, interneuron)
- `--format <format>`: Output format (json, table, text)

**Example:**
```bash
./cli.py explore neurons --type all --format table
```

#### explore visualize
Generate visualizations of brain state.

**Options:**
- `--type <type>`: Visualization type (neurons, synapses, memory, all)
- `--output <file>`: Output file path
- `--format <format>`: Output format (png, svg, pdf)

**Example:**
```bash
./cli.py explore visualize --type all --output brain.png
```

### Experiment Commands

#### experiment run
Run a named experiment.

**Arguments:**
- `name`: Name of the experiment to run

**Options:**
- `--config <file>`: Experiment configuration file path

**Example:**
```bash
./cli.py experiment run learning_test
```

### Python Commands

#### python run
Execute Python script with pynlm bindings.

**Arguments:**
- `script`: Path to Python script

**Options:**
- `--args <args>`: Additional arguments for the script

**Example:**
```bash
./cli.py python run my_experiment.py
```

### Tools Commands

#### tools benchmark
Run performance benchmarks.

**Options:**
- `--steps <n>`: Number of simulation steps (default: 1000)
- `--format <format>`: Output format (json, table, text)

**Example:**
```bash
./cli.py tools benchmark --steps 500
```

#### tools analyze
Analyze simulation results.

**Arguments:**
- `filepaths`: Paths to simulation result files

**Example:**
```bash
./cli.py tools analyze results1.bin results2.bin
```

## Scripting and Automation

### Script Examples

#### Basic Simulation Script
```bash
#!/bin/bash
# Run a complete simulation
./cli.py brain create --name simulation1
./cli.py brain run --steps 1000
./cli.py brain stats --format json > stats.json
./cli.py brain save simulation1.bin
```

#### Configuration Management Script
```bash
#!/bin/bash
# Manage configurations
cat > config.yaml << EOF
neuron_count: 500
region_count: 5
simulation_timestep: 0.001
connection_probability: 0.2
EOF

./cli.py config load config.yaml
./cli.py brain run --steps 500
./cli.py config save final_config.yaml
```

#### Experiment Batch Script
```bash
#!/bin/bash
# Run multiple experiments
for i in {1..5}; do
    echo "Running experiment iteration $i"
    ./cli.py experiment run learning_test_$i
    ./cli.py tools analyze results_$i.bin
    mv results_$i.bin results_$i.completed.bin
    rm -f results_$i.bin
    echo "Completed iteration $i"
    sleep 1
    echo ""
done
```

## Advanced Features

### Configuration Templates

Create reusable configuration templates for common experiment setups:

```bash
# Create a basic neuron configuration template
cat > templates/neural_config.yaml << EOF
neuron_count: 1000
region_count: 10
simulation_timestep: 0.001
connection_probability: 0.15

# Learning parameters
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015

# Memory parameters
working_memory_size: 100
episodic_memory_capacity: 1000

# Neuromodulation
enable_dopamine: true
enable_curiosity: true
enable_novelty: true
EOF
```

### Session Management

Save and restore complete simulation sessions:

```bash
#!/bin/bash
# Save session
session_id="session_$(date +%Y%m%d_%H%M%S)"
./cli.py config save "$session_id.config.yaml"
./cli.py brain save "$session_id.brain.bin"
./cli.py explore neurons --output "$session_id.neuron_stats.txt"

# Restore session
./cli.py config load "$session_id.config.yaml"
./cli.py brain load "$session_id.brain.bin"
```

### Automated Experiment Management

Create automated experiment runners:

```python
# experiment_runner.py
#!/usr/bin/env python3
import subprocess
import time
import sys

def run_experiment(experiment_name, steps=1000, config_file=None):
    """Run a single experiment."""
    cmd = ["cli.py", "brain", "run", "--steps", str(steps)]
    
    if config_file:
        cmd.extend(["--config", config_file])
    
    # Add experiment-specific arguments
    cmd.extend(["--experiment", experiment_name])
    
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        return result.returncode == 0
    except subprocess.CalledProcessError as e:
        print(f"Experiment failed: {e}")
        print(f"Error output: {e.stderr}")
        return False

def run_experiment_suite(experiments, steps=1000):
    """Run a suite of experiments."""
    results = []
    
    for experiment in experiments:
        print(f"Running experiment: {experiment}")
        success = run_experiment(experiment, steps)
        results.append((experiment, success))
        
        if not success:
            print(f"Experiment {experiment} failed!")
        
        time.sleep(1)  # Brief pause between experiments
    
    # Generate summary report
    print("\nExperiment Summary:")
    for experiment, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"  {experiment}: {status}")
    
    return results

if __name__ == "__main__":
    experiments = ["learning_test", "plasticity_test", "cognition_test"]
    run_experiment_suite(experiments)
```

## Development and Extension

### Adding New Commands

To add new commands to the NLM CLI:

1. **Create a new command class** that extends `BaseCommand`
2. **Implement the setup_args() method** to define command arguments
3. **Implement the execute() method** to handle command logic
4. **Register the command** in the `NLMCLI` class

Example command structure:

```python
class CustomCommand(BaseCommand):
    def __init__(self):
        super().__init__("custom", "Custom command for NLM CLI")
        self.setup_args()
    
    def setup_args(self):
        self.args.add_argument('--param', help='Parameter value')
        self.args.add_argument('--flag', action='store_true', help='Flag option')
    
    def execute(self, args):
        # Implement command logic
        result = CommandResult(success=True, message="Command executed")
        return result
```

### Extending Configuration

To add new configuration options:

1. **Update the configuration system** in `config.py`
2. **Add validation logic** for new configuration options
3. **Update default configuration** values

### Plugin System

The CLI supports a plugin architecture for extending functionality:

1. **Create plugin modules** in the `plugins/` directory
2. **Implement plugin interfaces** for command registration
3. **Load plugins dynamically** in the CLI

## Performance Tips

### For Large Simulations

1. **Use smaller configurations for testing:**
```bash
./cli.py brain create --name test_small --config small_config.yaml
./cli.py brain run --steps 100  # Smaller number of steps
```

2. **Use configuration files for parameter management:**
```bash
./cli.py config load production_config.yaml
./cli.py brain run --steps 10000
```

3. **Batch multiple operations:**
```bash
#!/bin/bash
# Batch experiment execution
for config in configs/*.yaml; do
    name=$(basename "$config" .yaml)
    echo "Running experiment with config: $name"
    ./cli.py config load "$config"
    ./cli.py brain run --steps 1000
    echo "Completed"
done
```

### For Real-time Analysis

1. **Use analysis commands during simulation:**
```bash
# Create a custom analysis script
./cli.py explore neurons --type all --format json > neuron_analysis.json
./cli.py explore synapses --type all --format json > synapse_analysis.json
```

2. **Use monitoring tools:**
```bash
# Monitor simulation performance
./cli.py tools profile --steps 1000
```

## Troubleshooting

### Common Issues

#### Issue: "Command not found"
**Solution:** Check the command name and ensure it's spelled correctly.
```bash
./cli.py brain --help  # Shows available brain subcommands
./cli.py config --help # Shows available config subcommands
```

#### Issue: "pynlm is not available"
**Solution:** Install the NLM Python bindings.
```bash
pip3 install pynlm
```

#### Issue: "Configuration file not found"
**Solution:** Check the file path and ensure it exists.
```bash
# List files in the current directory
ls -la

# Check if configuration file exists
cat config.yaml  # Should show configuration contents
```

#### Issue: "No brain available"
**Solution:** Create a brain first.
```bash
./cli.py brain create --name mybrain
```

#### Issue: "Permission denied"
**Solution:** Check file permissions and ensure write access.
```bash
chmod +x cli.py
./cli.py brain save brain_state.bin
```

### Debugging Tips

1. **Use verbose mode:**
```bash
./cli.py --verbose brain run --steps 100
```

2. **Check error messages:**
```bash
./cli.py config load invalid_config.yaml 2>&1
```

3. **Test with simple commands:**
```bash
./cli.py help
./cli.py config list --help
```

## Known Limitations

1. **Configuration format support:** YAML and JSON are fully supported. INI format support is planned for future releases.

2. **Python script execution:** Scripts must be compatible with the NLM Python bindings (pynlm).

3. **File format support:** Brain files and configuration files must be in compatible formats.

4. **Plugin system:** The plugin system is basic and may require additional configuration for advanced use.

## Future Enhancements

### Planned Features

1. **Web-based GUI:** Web interface for the CLI
2. **Advanced visualization:** 3D brain visualization and interactive graphs
3. **Machine learning integration:** Integration with ML frameworks
4. **Distributed computing:** Support for distributed brain simulations
5. **Advanced configuration:** Automatic configuration optimization
6. **Real-time monitoring:** Real-time simulation monitoring and control

### Open Source Contributions

We welcome contributions to the NLM CLI project! To contribute:

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement the feature**
4. **Add tests**
5. **Update documentation**
6. **Create a pull request**

### Contact and Support

For issues, questions, or support:

- **GitHub Issues:** Report bugs and request features
- **Documentation:** Check the official documentation
- **Community:** Join the NLM community forums

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Copyright

Copyright © 2026 NLM Project Authors. All rights reserved.

---

**Project:** NLM CLI - Command Line Interface for NLM Brain Simulation Project  
**Version:** 1.0.0  
**Author:** NLM Project Team  
**License:** MIT  
**Repository:** https://github.com/nlm-project/nlm  
**Documentation:** https://nlm.readthedocs.io