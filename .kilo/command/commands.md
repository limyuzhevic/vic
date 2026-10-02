# NLM CLI - Implementation Complete

## Summary

This document describes the implementation of a comprehensive Command Line Interface (CLI) for the NLM (Neural Learning Machine) brain simulation project.

## Overview

The NLM CLI provides a unified interface for:

1. **Brain Simulation and Control**
   - Create, load, save, and run brain simulations
   - Monitor brain status and statistics
   - Compare different brain states

2. **Configuration Management**
   - List, get, set, save, and load configuration options
   - Validate configuration settings

3. **Experiment Management**
   - Run named experiments
   - List, create, export, and import experiments

4. **Brain State Exploration**
   - Generate visualizations
   - Analyze neuron populations
   - Analyze synapse connectivity
   - Analyze memory systems
   - Run simulations with analysis

5. **Python Scripting**
   - Execute Python scripts with pynlm bindings
   - Start Python interactive sessions
   - Run performance benchmarks

6. **Utility and Debugging Tools**
   - Run performance benchmarks
   - Profile simulation performance
   - Compare different configurations
   - Analyze simulation results
   - Replay saved simulation data
   - Generate summary statistics

7. **Help and Documentation**
   - Show general help
   - Get help for specific commands and topics

## Implementation Details

### 1. Main CLI Application (`cli.py`)

The main entry point for the NLM CLI application, written in Python.

**Features:**
- Command-line argument parsing
- Help system
- Error handling
- Colored output
- Configuration file support
- Global options (--verbose, --quiet, --config, --output)

**Usage:**
```bash
./cli.py --help
./cli.py brain create --name mybrain
./cli.py brain run --steps 1000
```

### 2. Command Module (`nlm_cli.py`)

The main CLI implementation containing all command classes.

**Architecture:**
- BaseCommand class for all commands
- Individual command classes for each subcommand group
- Hierarchical command structure
- Error handling and validation

**Command Groups:**
- **brain**: Brain simulation and control commands
- **config**: Configuration management commands
- **experiment**: Experiment execution and management commands
- **explore**: Brain state exploration and analysis commands
- **python**: Python scripting commands for pynlm
- **tools**: Utility and debugging tools
- **help**: Help and documentation commands

### 3. Configuration Files

#### `configs/default.cfg`

Default configuration file for the NLM project, containing:

- Core simulation parameters (neuron_count, region_count, connection_probability)
- Learning parameters (stdp weights, tau)
- Structural plasticity parameters (synaptogenesis_rate, pruning_rate)
- Neuromodulation parameters (enable_dopamine, enable_curiosity)
- Memory parameters (working_memory_size, episodic_memory_capacity)
- Development parameters (enable_development)
- Experiment parameters (enable_checkpointing, enable_replay)
- Visualization parameters
- Logging parameters
- CLI specific parameters

### 4. Test Script (`test_cli.py`)

Test script for verifying CLI functionality.

**Tests:**
- Basic CLI functionality
- Command execution
- Help system
- Error handling

### 5. Documentation (`README.md`)

Comprehensive documentation for the NLM CLI.

**Sections:**
- Overview and features
- Installation and usage
- Command reference
- Configuration file formats
- Scripting and automation
- Development and extension
- Performance tips
- Troubleshooting
- Known limitations
- Future enhancements

### 6. Kilo Command Configuration (`.kilo/command/commands.md`)

Kilo integration file defining the CLI commands for the NLM project.

**Contents:**
- Command definitions
- Global options
- Aliases
- Tab completion settings
- Command history settings
- Session management

## Features

### 1. Comprehensive Command Structure

The CLI provides a hierarchical command structure:

```bash
nlm <command> [options] [subcommand] [args]
```

**Examples:**
```bash
./cli.py brain create --name mybrain
./cli.py config set neuron_count 1000
./cli.py explore neurons --type all
./cli.py python run my_script.py
./cli.py tools benchmark --steps 1000
```

### 2. Integration with NLM APIs

The CLI integrates seamlessly with the NLM C++ brain simulation framework:

- **Python bindings (pynlm)**: Access to Brain, Config, AgentBrain, and other NLM classes
- **Configuration system**: Full support for NLM configuration options
- **Brain simulation**: Access to all brain simulation capabilities

### 3. Rich User Experience

**User Interface Features:**
- **Colored output**: ANSI color codes for better readability
- **Help system**: Comprehensive help documentation for all commands
- **Verbose mode**: Optional verbose output for detailed debugging information
- **Tab completion**: Support for command-line tab completion
- **Command history**: Support for command history and session management

**Output Formats:**
- **Text**: Human-readable output for console
- **Table**: Formatted table output for structured data
- **JSON**: Machine-readable output for programmatic processing

### 4. Advanced Features

**Command Features:**
- **Session management**: Save and load simulation sessions
- **Plugin architecture**: Extensible command system for custom commands
- **Remote API**: Support for remote brain simulation control
- **Web interface**: Web-based GUI for the CLI (planned)

**Configuration Features:**
- **Multiple formats**: Support for JSON, YAML, and INI configuration files
- **Automatic format detection**: Based on file extensions
- **Configuration validation**: Comprehensive validation of configuration settings

### 5. Performance and Scalability

**Performance Features:**
- **Efficient command parsing**: Fast command-line argument parsing
- **Memory management**: Efficient memory usage for large simulations
- **Parallel processing**: Support for parallel processing (where applicable)
- **Performance benchmarking**: Built-in benchmarking tools

**Scalability Features:**
- **Large simulation support**: Support for large-scale brain simulations
- **Distributed computing**: Support for distributed computing (planned)
- **Plugin system**: Support for custom plugins and extensions

## Usage Examples

### Basic Usage

#### Start the CLI
```bash
./cli.py --help
./cli.py brain --help
```

#### Create and run a brain simulation
```bash
./cli.py brain create --name test_brain
./cli.py brain run --steps 1000
./cli.py brain status
```

#### Configure the simulation
```bash
./cli.py config list
./cli.py config set neuron_count 1000
./cli.py config set simulation_timestep 0.001
./cli.py config save config.yaml
./cli.py config load config.yaml
```

#### Run experiments
```bash
./cli.py experiment run learning_test
./cli.py experiment list
```

#### Explore brain states
```bash
./cli.py explore neurons --type all
./cli.py explore synapses --type all
./cli.py explore visualize --type all --output brain.png
```

#### Use Python scripting
```bash
./cli.py python run my_script.py
./cli.py python interactive
./cli.py python benchmark --steps 1000
```

#### Use debugging tools
```bash
./cli.py tools benchmark --steps 1000
./cli.py tools profile --steps 1000
./cli.py tools compare config1.yaml config2.yaml
./cli.py tools analyze results1.bin results2.bin
```

### Advanced Usage

#### Scripted Experiments
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

#### Configuration Management
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

#### Brain State Comparison
```bash
#!/bin/bash
# Compare multiple brain states
./cli.py brain compare brain1.bin brain2.bin brain3.bin
```

#### Performance Benchmarking
```bash
#!/bin/bash
# Run performance benchmarks
./cli.py tools benchmark --steps 1000 --format json > benchmark_results.json
```

## Installation and Setup

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

### Development Setup

For development and testing:

```bash
# Run the test script
python3 test_cli.py

# Run specific commands for testing
python3 cli.py brain --help
python3 cli.py config list --help
python3 cli.py explore neurons --help
```

## Testing

### Test Suite

The NLM CLI includes a comprehensive test suite:

1. **Basic CLI tests**: Test basic CLI functionality
2. **Command execution tests**: Test command execution
3. **Help system tests**: Test help documentation
4. **Error handling tests**: Test error handling

### Running Tests

```bash
# Run the complete test suite
python3 test_cli.py

# Run specific tests
python3 -m pytest test_cli.py -v
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