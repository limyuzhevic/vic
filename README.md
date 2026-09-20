# NLM (Neural Learning Machine) CLI

## Overview

The NLM (Neural Learning Machine) Command Line Interface (CLI) is a comprehensive tool for managing NLM simulations, brains, experiments, and configurations. This CLI provides a unified interface for interacting with neural network simulations and experiments through a command-line interface.

## Features

### 1. Configuration Management
- Create, edit, validate, backup, and restore configuration files
- List and manage multiple configurations
- Support for JSON parameters and inline configuration

### 2. Brain Management
- Create new brain instances with different parameters
- Load and save brain models from files
- Compare brain models and their statistics
- Inspect brain properties and statistics
- Support for multiple brain formats (JSON, binary, etc.)

### 3. Simulation Control
- Start, pause, resume, and stop simulations
- Control simulation speed and step mode
- Create and manage checkpoints
- Monitor simulation progress and metrics
- Automatic checkpointing support

### 4. Experiment Management
- Create and run experiments with different configurations
- Manage experiment parameters and results
- Compare experiment results
- Generate experiment reports
- Support for automated experiment workflows

### 5. Analysis Tools
- Analyze simulation data and metrics
- Generate performance reports
- Visualize simulation results
- Identify patterns and anomalies
- System resource monitoring

### 6. Plugin Management
- List available plugins
- Install and configure plugins
- Manage plugin settings
- Plugin dependency management

### 7. Error Handling and Logging
- View error logs and diagnostics
- Set up error handling strategies
- Monitor system health
- Alert system for critical events

### 8. System Management
- Check system resources and status
- Manage plugins and dependencies
- Set up monitoring and alerts
- System health checks

## Requirements

### System Requirements
- Python 3.8 or higher
- Operating System: Windows, Linux, or macOS

### Python Dependencies
This project includes all necessary Python code without requiring external dependencies:
- Built-in Python modules (json, time, threading, pathlib, datetime, etc.)
- Standard library modules (typing, argparse, etc.)

## Installation

### Basic Installation
```bash
# Clone the repository
git clone <repository-url>
cd nlm-cli

# The project is ready to use
# No additional installation required
```

### Development Installation
```bash
# Clone the repository
git clone <repository-url>
cd nlm-cli

# Run tests (if any)
python -m pytest

# Run example demonstrations
python -c "from nlm_cli import NLMCLI; cli = NLMCLI(); print('NLM CLI is ready to use')"
```

## Getting Started

### 1. Quick Start
```bash
# Display help information
nlm-cli --help

# List available configurations
nlm-cli config list

# Create your first configuration
nlm-cli config create --name my_config

# Create your first brain
nlm-cli brain create --name mybrain

# Run your first simulation
nlm-cli simulation start --name my_sim --steps 50

# Create and run your first experiment
nlm-cli experiment create --name my_exp
nlm-cli experiment run --name my_exp --steps 100
```

### 2. Configuration Management
```bash
# Create a configuration
nlm-cli config create --name neural_config --parameters "{\"neurons\": 1000, \"layers\": 3}"

# List configurations
nlm-cli config list --all

# Show configuration details
nlm-cli config show --name neural_config

# Validate configuration
nlm-cli config validate --name neural_config

# Backup configuration
nlm-cli config backup --name neural_config --output backup.json
```

### 3. Brain Management
```bash
# Create a brain
nlm-cli brain create --name mybrain --config neural_config

# Load a brain from file
nlm-cli brain load --name loaded_brain --filepath brain.json

# Inspect brain statistics
nlm-cli brain inspect --name mybrain

# Compare brain models
nlm-cli brain compare --brain1 brain_a --brain2 brain_b
```

### 4. Simulation Control
```bash
# Start a simulation
nlm-cli simulation start --name my_sim --steps 100

# Pause a simulation
nlm-cli simulation pause --name my_sim

# Resume a simulation
nlm-cli simulation resume --name my_sim

# Get simulation status
nlm-cli simulation status --name my_sim

# Create a checkpoint
nlm-cli simulation checkpoint --name my_sim --filepath checkpoint.json
```

### 5. Experiment Management
```bash
# Create an experiment
nlm-cli experiment create --name my_exp --description "My first experiment"

# Run an experiment
nlm-cli experiment run --name my_exp --steps 200

# List experiments
nlm-cli experiment list --all

# Compare experiments
nlm-cli experiment compare --name1 exp1 --name2 exp2
```

### 6. Analysis and Visualization
```bash
# Analyze simulation data
nlm-cli analysis analyze_data --simulation_name my_sim

# Generate performance report
nlm-cli analysis generate_report --name performance_report --simulation_name my_sim

# Visualize results
nlm-cli analysis visualize --simulation_name my_sim --plot_type comprehensive

# Find patterns and anomalies
nlm-cli analysis find_patterns --simulation_name my_sim --pattern_type all
```

### 7. System Management
```bash
# Check system status
nlm-cli system status

# Monitor system resources
nlm-cli system resources

# Check system health
nlm-cli system check_system_health

# Get system alerts
nlm-cli system alerts
```

## Usage Examples

### Complete Workflow Example
```bash
#!/bin/bash

# Create configuration
echo "Creating configuration..."
nlm-cli config create --name nn_config --parameters "{\"neurons\": 500, \"regions\": 2}"

# Create brain
echo "Creating brain..."
nlm-cli brain create --name neural_brain --config nn_config

# Create experiment
echo "Creating experiment..."
nlm-cli experiment create --name neural_exp --description "Neural network simulation"

# Run experiment
echo "Running experiment..."
nlm-cli experiment run --name neural_exp --steps 300

# Analyze results
echo "Analyzing results..."
nlm-cli analysis generate_report --name neural_report --simulation_name neural_sim

# Visualize
echo "Generating visualizations..."
nlm-cli analysis visualize --simulation_name neural_sim --plot_type comprehensive

echo "Experiment completed successfully!"
```

### Batch Processing Example
```bash
#!/bin/bash

# Process multiple configurations
for config in configs/*.json; do
    config_name=$(basename "$config" .json)
    echo "Processing configuration: $config_name"
    nlm-cli config import --input "$config" --name "$config_name"
done

# Run experiments for each configuration
for config in configs/*.json; do
    config_name=$(basename "$config" .json)
    echo "Running experiment for: $config_name"
    nlm-cli experiment create --name "exp_${config_name}" --config_name "$config_name"
    nlm-cli experiment run --name "exp_${config_name}" --steps 100
done
```

### Automation Example
```bash
#!/bin/bash

# Monitor system continuously
while true; do
    nlm-cli system check_system_health
    nlm-cli system alerts
    sleep 300  # Check every 5 minutes
    
    # Check if any experiment is still running
    if nlm-cli experiment list --name running_exp; then
        echo "Experiment is still running"
    else
        echo "All experiments completed"
        break
    fi
done
```

## File Structure

```
.
├── nlm_cli.py                    # Main CLI interface
├── config_commands.py            # Configuration management commands
├── brain_commands.py             # Brain management commands
├── simulation_commands.py        # Simulation control commands
├── experiment_commands.py        # Experiment management commands
├── analysis_commands.py          # Analysis tools commands
├── plugin_commands.py            # Plugin management commands
├── system_commands.py            # System management commands
├── cli_examples.py               # Usage examples and demonstrations
├── cli_help.py                   # Comprehensive help and documentation
├── README.md                     # Project documentation
├── pyproject.toml               # Project configuration
└── python/                       # Python bindings for NLM
    ├── enhanced_api.py          # Enhanced Python API for NLM
    └── __init__.py             # Python package initialization
```

## Development

### Setting Up Development Environment
```bash
# Clone the repository
git clone <repository-url>
cd nlm-cli

# Install development dependencies (if any)
# pip install -e .[dev]

# Run tests (if any)
# python -m pytest

# Run the CLI help
nlm-cli help general
```

### Running the CLI
```bash
# Run the main CLI
nlm-cli

# Run specific command
nlm-cli config list

# Get help for a specific command
nlm-cli config --help
```

### Adding New Commands
To add new commands to the NLM CLI:

1. Create a new command file (e.g., `new_feature_commands.py`)
2. Implement the command class with a `handle()` method
3. Import the command class in `nlm_cli.py`
4. Register the command in the CLI dispatch mechanism

### Command Structure
Each command file should follow this structure:

```python
class CommandNameCommands:
    """Handle [command name] commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle [command name] commands."""
        command = args.get('command')
        
        handlers = {
            'subcommand1': self.subcommand1,
            'subcommand2': self.subcommand2,
            ...
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown [command name] command: {command}")
    
    def subcommand1(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle subcommand1."""
        # Implementation here
        ...
    
    def subcommand2(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle subcommand2."""
        # Implementation here
        ...
```

## Configuration

### Configuration File Format
Configuration files are stored in JSON format:

```json
{
    "name": "neural_config",
    "description": "Neural network configuration",
    "created_at": "2024-01-01T00:00:00Z",
    "last_modified": "2024-01-01T00:00:00Z",
    "parameters": {
        "neurons": 1000,
        "regions": 4,
        "synapses_per_neuron": 10,
        "learning_rate": 0.01
    }
}
```

### Default Configuration Path
Configuration files are stored in:
- Unix/Linux: `~/.nlm/config.json`
- Windows: `%APPDATA%\\.nlm\\config.json`

### Plugin Configuration
Plugins are configured in:
- `~/.nlm/plugins/[plugin_name]/plugin.json`

## Advanced Usage

### Scripting
The NLM CLI supports scripting through:
- JSON parameter files
- Batch processing scripts
- Automated experiment workflows
- Integration with other tools

### Automation
Use the NLM CLI for:
- Automated experiment runs
- Performance testing
- Data analysis pipelines
- System monitoring

### Integration
The NLM CLI can be integrated with:
- CI/CD pipelines
- Data analysis tools
- Visualization tools
- Monitoring systems

## Troubleshooting

### Common Issues

#### 1. Command not found
**Solution:** Ensure you're using the correct command path.
```bash
# Use the correct command structure
nlm-cli config list
```

#### 2. Configuration not found
**Solution:** Create the configuration first.
```bash
# Create the configuration
nlm-cli config create --name myconfig
```

#### 3. Brain not loaded
**Solution:** Load or create a brain first.
```bash
# Create or load a brain
nlm-cli brain create --name mybrain
```

#### 4. Simulation not running
**Solution:** Start a simulation first.
```bash
# Start a simulation
nlm-cli simulation start --name mysim --steps 100
```

### Getting Help
```bash
# Get general help
nlm-cli --help

# Get help for specific command category
nlm-cli help config
nlm-cli help brain
nlm-cli help simulation

# Get help for specific command
nlm-cli config --help
nlm-cli brain --help
```

## Contributing

### Contributing Guidelines
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests (if applicable)
5. Submit a pull request

### Code Style
- Follow PEP 8 style guide
- Use descriptive variable names
- Add docstrings for all public functions
- Include type hints
- Add error handling

### Testing
- Write unit tests for new functionality
- Test edge cases
- Ensure backward compatibility
- Run existing tests before submitting

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contact

For questions, issues, or suggestions:
- GitHub: <https://github.com/nlm-project/nlm-cli>
- Documentation: <https://nlm.readthedocs.io>
- Email: <support@nlm.ai>

## Acknowledgements

Special thanks to:
- The NLM project team for their contributions
- All contributors who have helped develop this CLI
- The open-source community for their valuable tools and libraries

## Version

Version: 1.0.0

Last updated: 2024-01-01

---

*This project was created as a comprehensive CLI for the NLM (Neural Learning Machine) neural simulation framework.*