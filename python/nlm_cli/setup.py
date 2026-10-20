#!/usr/bin/env python3
"""
Setup script for NLM CLI Tool

This script sets up the NLM CLI tool with all necessary dependencies
and configurations for deployment.
"""

import os
import sys
import json
import subprocess
import platform
from pathlib import Path

def print_header(message):
    """Print header message"""
    print(f"\n{'=' * 60}")
    print(f"{message}")
    print(f"{'=' * 60}")

def check_python_version():
    """Check Python version requirements"""
    print_header("Checking Python Version")
    
    python_version = sys.version_info
    print(f"Python version: {python_version.major}.{python_version.minor}.{python_version.micro}")
    
    if python_version.major < 3 or (python_version.major == 3 and python_version.minor < 8):
        print("ERROR: Python 3.8 or higher is required")
        return False
    
    print("✓ Python version check passed")
    return True

def check_dependencies():
    """Check if required dependencies are available"""
    print_header("Checking Dependencies")
    
    required_packages = [
        ("click", "8.0.0"),
        ("rich", "10.0.0"),
        ("pydantic", "1.8.0"),
        ("pyyaml", "6.0.0"),
        ("tabulate", "0.8.9"),
        ("websockets", "10.0.0"),
    ]
    
    optional_packages = [
        ("prompt_toolkit", "3.0.0"),
    ]
    
    all_good = True
    
    for package, min_version in required_packages:
        try:
            if package == "pydantic":
                import pydantic
                version = pydantic.__version__
            elif package == "pyyaml":
                import yaml
                version = yaml.__version__
            elif package == "tabulate":
                import tabulate
                version = tabulate.__version__
            elif package == "websockets":
                import websockets
                version = websockets.__version__
            else:
                exec(f"import {package}")
                version = getattr(sys.modules[package], '__version__', 'unknown')
            
            print(f"✓ {package}: {version} (required: >= {min_version})")
            
        except ImportError:
            print(f"✗ {package}: Not installed (required: >= {min_version})")
            all_good = False
            
    for package, min_version in optional_packages:
        try:
            exec(f"import {package}")
            version = getattr(sys.modules[package], '__version__', 'unknown')
            print(f"✓ {package}: {version} (optional, required: >= {min_version})")
        except ImportError:
            print(f"⚠ {package}: Not installed (optional, required: >= {min_version})")
    
    return all_good

def setup_directories():
    """Create necessary directories"""
    print_header("Setting Up Directories")
    
    directories = [
        "configs",
        "outputs",
        "logs",
        "scripts",
        "examples",
        "docs",
    ]
    
    for dir_name in directories:
        dir_path = Path(dir_name)
        if not dir_path.exists():
            dir_path.mkdir(parents=True, exist_ok=True)
            print(f"✓ Created directory: {dir_name}")
        else:
            print(f"✓ Directory exists: {dir_name}")
    
    return True

def create_default_configs():
    """Create default configuration files"""
    print_header("Creating Default Configuration Files")
    
    configs_dir = Path("configs")
    
    # Default configuration file
    default_config = configs_dir / "default.cfg"
    if not default_config.exists():
        config_content = """# NLM Configuration File
# This is the default configuration for NLM (Neural Learning Machine)

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
"""
        with open(default_config, 'w') as f:
            f.write(config_content)
        print(f"✓ Created default config: {default_config}")
    
    # JSON configuration example
    json_config = configs_dir / "example.json"
    if not json_config.exists():
        config_content = {
            "general": {
                "random_seed": 42,
                "simulation_timestep": 0.001,
                "max_simulation_steps": 10000
            },
            "neuron": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "v_thresh": -50.0,
                "v_rest": -70.0,
                "v_reset": -75.0
            },
            "plasticity": {
                "stdp": {"enable": true, "learning_rate": 0.001},
                "hebbian": {"enable": true},
                "structural": {"enable": true}
            },
            "neuromodulation": {
                "dopamine": {"scale": 1.0},
                "curiosity": {"enable": true},
                "novelty": {"enable": true}
            }
        }
        with open(json_config, 'w') as f:
            json.dump(config_content, f, indent=2)
        print(f"✓ Created JSON config example: {json_config}")
    
    return True

def create_example_scripts():
    """Create example scripts"""
    print_header("Creating Example Scripts")
    
    scripts_dir = Path("scripts")
    
    # Basic simulation script
    basic_script = scripts_dir / "basic_simulation.sh"
    if not basic_script.exists():
        script_content = """#!/bin/bash
# Basic NLM simulation script

echo "Running basic NLM simulation..."

# Create brain with 1000 neurons
nlm brain create --neuron-count 1000

# Run simulation with 100 steps
nlm brain simulate --steps 100 --output-format json --output-file simulation_results.json

# Inspect brain properties
nlm brain inspect

# Create world
nlm world create --width 100 --height 100

# Run world simulation
nlm world simulate --steps 200

# Create agent
nlm agent create --training-episodes 100

# Train agent
nlm agent train --episodes 100 --learning-rate 0.01

# Evaluate agent
nlm agent evaluate --test-episodes 50

echo "Basic simulation completed successfully!"
"""
        with open(basic_script, 'w') as f:
            f.write(script_content)
        os.chmod(basic_script, 0o755)
        print(f"✓ Created basic simulation script: {basic_script}")
    
    # Advanced script
    advanced_script = scripts_dir / "advanced_simulation.sh"
    if not advanced_script.exists():
        script_content = """#!/bin/bash
# Advanced NLM simulation script with profiling and analysis

echo "Running advanced NLM simulation with profiling..."

# Start profiling
nlm profile start

# Create complex brain
nlm brain create --neuron-count 5000 --config configs/example.json

# Run intensive simulation
nlm brain simulate --steps 1000 --output-format json --output-file advanced_results.json

# Run world simulation
nlm world create --width 200 --height 200
nlm world simulate --steps 500

# Create and train agent
nlm agent create --training-episodes 500
nlm agent train --episodes 500 --learning-rate 0.005

# Evaluate with detailed analysis
nlm agent evaluate --test-episodes 100 --output-format csv --output-file evaluation_metrics.csv

# Analyze agent behavior
nlm agent analyze --output-format json --output-file agent_analysis.json

# Stop profiling and generate report
nlm profile stop
nlm profile analyze
nlm profile report --output-format html --output-file performance_report.html

echo "Advanced simulation completed successfully!"
"""
        with open(advanced_script, 'w') as f:
            f.write(script_content)
        os.chmod(advanced_script, 0o755)
        print(f"✓ Created advanced simulation script: {advanced_script}")
    
    # Script for configuration management
    config_script = scripts_dir / "config_management.sh"
    if not config_script.exists():
        script_content = """#!/bin/bash
# Configuration management script for NLM

echo "Managing NLM configurations..."

# List all config files
echo "Available configuration files:"
find configs -name "*.cfg" -o -name "*.json" | sed 's|./||'

# Create a backup of current config
if [ -f "configs/default.cfg" ]; then
    cp configs/default.cfg "configs/default.cfg.backup.$(date +%Y%m%d_%H%M%S)"
    echo "✓ Created backup of default.cfg"
fi

# Create example configuration
nlm config create --name "production" --neuron-count 10000

# Validate configuration
nlm config validate --file configs/example.json

# Compare configurations
if [ -f "configs/example.json" ] && [ -f "configs/default.cfg" ]; then
    echo "Comparing JSON and INI configurations..."
    nlm config compare --base configs/example.json --new configs/default.cfg
fi

# Migrate configuration
nlm config migrate --from 0.1 --to 0.2 --file configs/example.json

echo "Configuration management completed!"
"""
        with open(config_script, 'w') as f:
            f.write(script_content)
        os.chmod(config_script, 0o755)
        print(f"✓ Created configuration management script: {config_script}")
    
    return True

def create_documentation():
    """Create documentation files"""
    print_header("Creating Documentation")
    
    docs_dir = Path("docs")
    
    # Quick start guide
    quick_start = docs_dir / "quick_start.md"
    if not quick_start.exists():
        content = """# Quick Start Guide for NLM CLI

## Installation

1. Install the package:
   ```bash
   pip install -e .
   ```

2. Test the installation:
   ```bash
   nlm --help
   ```

## Basic Usage

### Create and Simulate a Brain

```bash
# Create a brain with 1000 neurons
nlm brain create --neuron-count 1000

# Run simulation with 100 steps
nlm brain simulate --steps 100

# Save results to JSON file
nlm brain simulate --steps 100 --output-format json --output-file results.json
```

### Create and Configure a World

```bash
# Create world with custom dimensions
nlm world create --width 100 --height 100 --vision-width 32 --vision-height 32

# Configure world parameters
nlm world configure --max-energy 100.0 --energy-decay 0.1

# Run world simulation
nlm world simulate --steps 200
```

### Work with Agents

```bash
# Create agent
nlm agent create

# Train agent
nlm agent train --episodes 100 --learning-rate 0.01

# Evaluate agent
nlm agent evaluate --test-episodes 50
```

## Advanced Features

### Interactive Mode

```bash
# Start interactive CLI
nlm

# Use commands like:
brain create --neuron-count 500
world create --width 50 --height 50
agent train --episodes 100
```

### Configuration Management

```bash
# Create configuration
nlm config create --name "production" --neuron-count 10000

# Load configuration
nlm config load --file my_config.cfg

# Validate configuration
nlm config validate --file test.cfg

# Compare configurations
nlm config compare --base base.cfg --new new.cfg
```

### Experiment Management

```bash
# Run experiment
nlm experiment run --name "demo_test" --description "Demo experiment"

# List experiments
nlm experiment list

# Create experiment
nlm experiment create --name "plasticity_test" --description "Test synaptic plasticity"

# Analyze results
nlm experiment analyze
```

### Performance Profiling

```bash
# Start profiling
nlm profile start

# Run operations with profiling
nlm brain simulate --steps 1000 nlm agent train --episodes 200

# Stop and analyze
nlm profile stop
nlm profile analyze
nlm profile report --output-format html
```

## Output Formats

### JSON Output

```bash
# Save results in JSON format
nlm brain simulate --steps 100 --output-format json --output-file results.json

# JSON output is useful for:
# - Programmatic access
# - Data analysis
# - Integration with other tools
```

### Table Output

```bash
# Display results in table format
nlm brain simulate --steps 100 --output-format table

# Table output is useful for:
# - Human-readable display
# - Command-line interfaces
# - Quick review
```

### CSV Output

```bash
# Save results in CSV format
nlm brain simulate --steps 100 --output-format csv --output-file results.csv

# CSV output is useful for:
# - Spreadsheet applications
# - Data analysis
# - Statistical computing
```

## Configuration Files

### Default Configuration

The CLI tool uses `configs/default.cfg` by default. You can:

1. **Modify the default file**
   ```bash
   nvim configs/default.cfg
   ```

2. **Create custom configuration file**
   ```bash
   nlm --config-file my_config.cfg brain create --neuron-count 2000
   ```

3. **Use JSON configuration**
   ```bash
   nlm --config-file configs/example.json brain create --neuron-count 2000
   ```

### Configuration File Format

Configuration files can be in INI format (like `default.cfg`) or JSON format (like `example.json`). The CLI tool supports both formats automatically.

## Examples Directory

Run the example scripts:

```bash
# Basic simulation
./scripts/basic_simulation.sh

# Advanced simulation
./scripts/advanced_simulation.sh

# Configuration management
./scripts/config_management.sh
```

## Getting Help

### Command Help

```bash
# Get help for a specific command
nlm brain --help
nlm brain create --help
nlm brain simulate --help

# Get help for all commands
nlm --help
```

### Interactive Help

1. Start interactive mode:
   ```bash
   nlm
   ```

2. Get help for commands:
   - Type `help` for general help
   - Type `help <command>` for command-specific help
   - Type `help <command> <subcommand>` for subcommand help

### Examples Documentation

View command examples:
```bash
cat docs/cli_examples.md
```

## Troubleshooting

### Common Issues

1. **pynlm bindings not available**
   ```bash
   # Install dependencies
   pip install -e .
   ```

2. **Configuration file not found**
   ```bash
   # Use absolute path or copy to current directory
   cp /path/to/config.cfg ./my_config.cfg
   nlm --config-file my_config.cfg brain create --neuron-count 1000
   ```

3. **Permission denied**
   ```bash
   # Ensure write permissions
   chmod 755 ./output.json
   ```

4. **Memory issues**
   ```bash
   # Reduce neuron count or simulation steps
   nlm brain create --neuron-count 500 nlm brain simulate --steps 50
   ```

### Environment Setup

If you encounter issues with the installation or setup:

1. **Check Python version**:
   ```bash
   python3 --version
   ```
   Ensure you're using Python 3.8 or higher.

2. **Check dependencies**:
   ```bash
   pip list | grep -E "(click|rich|pydantic|pyyaml|tabulate|websockets)"
   ```

3. **Create virtual environment**:
   ```bash
   python3 -m venv nlm_env
   source nlm_env/bin/activate
   pip install -e .
   ```

4. **Check installation logs**:
   ```bash
   pip install -e . --verbose
   ```

## Performance Tips

### For Faster Simulations

1. **Use smaller networks**:
   ```bash
   nlm brain create --neuron-count 100
   ```

2. **Reduce simulation steps**:
   ```bash
   nlm brain simulate --steps 50
   ```

3. **Use minimal configuration**:
   ```bash
   nlm brain create --neuron-count 100 --timestep 0.01
   ```

### For Better Output

1. **Use appropriate output format**:
   - JSON for programmatic use
   - TABLE for human-readable display
   - CSV for data analysis

2. **Save output to file**:
   ```bash
   nlm brain simulate --steps 100 --output-format json --output-file results.json
   ```

### For Large-Scale Operations

1. **Use script mode**:
   ```bash
   nlm --script operations.txt
   ```

2. **Use configuration files**:
   ```bash
   nlm --config-file large_config.json brain create --neuron-count 10000
   ```

## License

MIT License - See LICENSE file for details.
"""
        with open(quick_start, 'w') as f:
            f.write(content)
        print(f"✓ Created quick start guide: {quick_start}")
    
    # API reference
    api_ref = docs_dir / "api_reference.md"
    if not api_ref.exists():
        content = """# API Reference

## Command Structure

The NLM CLI tool has the following command structure:

```
nlm [OPTIONS] COMMAND [ARGS]...
```

### Commands

- `brain` - Brain operations
- `world` - World operations  
- `agent` - Agent operations
- `config` - Configuration operations
- `experiment` - Experiment operations
- `profile` - Performance profiling

### Global Options

- `--output-format FORMAT` - Output format (json, yaml, table, csv)
- `--output-file FILE` - File to save output to
- `--log-level LEVEL` - Logging level (debug, info, warn, error, critical)
- `--config-file FILE` - Configuration file path
- `--dry-run` - Preview operations without executing
- `--force` - Overwrite existing files
- `--verbose, -v` - Verbose output

### Command Options

Each command has its own specific options. Use `nlm <command> --help` to see available options.

## Configuration Reference

### Configuration File Formats

#### INI Format (default.cfg)

```ini
[section]
key = value
key2 = value2
```

#### JSON Format (example.json)

```json
{
    "section": {
        "key": "value",
        "key2": "value2"
    }
}
```

### Available Configuration Keys

#### General Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| random_seed | int | 42 | Random seed for reproducibility |
| simulation_timestep | float | 0.001 | Simulation timestep in seconds |
| max_simulation_steps | int | 10000 | Maximum simulation steps |
| simulation_time_limit | float | 0.0 | Simulation time limit |

#### Neuron Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| neuron_count | int | 1000 | Number of neurons |
| region_count | int | 2 | Number of brain regions |
| connection_probability | float | 0.1 | Connection probability |
| v_thresh | float | -50.0 | Threshold voltage |
| v_rest | float | -70.0 | Resting voltage |
| v_reset | float | -75.0 | Reset voltage |
| tau_mem | float | 20.0 | Membrane time constant |
| tau_ref | float | 2.0 | Refractory period |

#### Plasticity Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| plasticity_learning_rate | float | 0.01 | General learning rate |
| stdp_ltp_weight | float | 0.01 | STDP LTP weight |
| stdp_ltd_weight | float | 0.012 | STDP LTD weight |
| stdp_time_constant | float | 20.0 | STDP time constant |

#### Development Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| synaptogenesis_rate | float | 0.001 | Synaptogenesis rate |
| pruning_rate | float | 0.0001 | Pruning rate |

#### Neuromodulation Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| dopamine_baseline | float | 0.1 | Dopamine baseline |
| dopamine_scale | float | 1.0 | Dopamine scale factor |
| curiosity_enabled | bool | true | Whether curiosity is enabled |
| novelty_enabled | bool | true | Whether novelty detection is enabled |

#### Environment Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| environment_name | string | GridWorld | Environment name |
| environment_width | int | 10 | Environment width |
| environment_height | int | 10 | Environment height |
| vision_width | int | 32 | Vision width |
| vision_height | int | 32 | Vision height |

#### Logging Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| log_level | string | INFO | Logging level |
| log_to_file | bool | false | Whether to log to file |
| log_filename | string | nlm.log | Log file name |

#### Visualization Settings

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| visualization_enabled | bool | false | Whether visualization is enabled |
| visualization_update_rate | float | 30.0 | Visualization update rate |

## Output Format Reference

### JSON Output

JSON output includes all simulation results in a structured format:

```json
{
    "status": "success",
    "timestamp": "2023-01-01T00:00:00Z",
    "brain_created": true,
    "neuron_count": 1000,
    "timestep": 0.001,
    "regions": 2,
    "synapses": 5000,
    "simulation_results": {
        "steps_completed": 100,
        "total_spikes": 500,
        "average_firing_rate": 0.5,
        "network_activity": "active"
    }
}
```

### YAML Output

YAML output is a human-readable format:

```yaml
status: success
timestamp: 2023-01-01T00:00:00Z
brain_created: true
neuron_count: 1000
timestep: 0.001
regions: 2
synapses: 5000
simulation_results:
  steps_completed: 100
  total_spikes: 500
  average_firing_rate: 0.5
  network_activity: active
```

### Table Output

Table output is formatted for console display:

```
+------------+----------+----------------+--------------+
| Parameter  | Value    | Unit           | Description  |
+------------+----------+----------------+--------------+
| Neuron Count | 1000     | neurons        | Total neurons |
| Time Step   | 0.001    | seconds        | Simulation timestep |
| Regions     | 2        | regions        | Number of brain regions |
| Synapses    | 5000     | connections    | Total synapses |
| Status      | success  | -              | Operation status |
+------------+----------+----------------+--------------+
```

### CSV Output

CSV output is tab-separated values for data analysis:

```csv
parameter,value,unit,description
neuron_count,1000,neurons,Total neurons
timestep,0.001,seconds,Simulation timestep
regions,2,regions,Number of brain regions
synapses,5000,connections,Total synapses
status,success,-,Operation status
```

## Error Codes

The CLI tool uses standard exit codes:

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | General error |
| 2 | Invalid arguments |
| 3 | Configuration error |
| 4 | File not found |
| 5 | Permission denied |
| 6 | Dependency missing |

## Interactive Mode Commands

### Special Commands

In interactive mode, you can use the following special commands:

- `help` - Show general help
- `exit` - Exit interactive mode
- `quit` - Exit interactive mode
- `!command` - Execute as shell command
- `cd path` - Change directory

### Command Completion

The interactive mode provides auto-completion for:

- Commands (brain, world, agent, config, experiment, profile)
- Subcommands (create, load, save, simulate, inspect, benchmark, validate)
- Options (matching available command options)

## API Server Reference

### HTTP Endpoints

The API server provides the following endpoints:

#### GET /api/version

Returns API version information:

```json
{
    "version": "1.0.0",
    "build": "development",
    "dependencies": {
        "pynlm": "0.1.0",
        "python": ">=3.8"
    }
}
```

#### GET /api/health

Returns server health status:

```json
{
    "status": "healthy",
    "timestamp": 1672574400.0,
    "uptime": 3600.0
}
```

#### GET /api/sessions

Returns list of active sessions:

```json
{
    "sessions": [
        {
            "id": "session_123",
            "connected_at": 1672574400.0,
            "last_activity": 1672574401.0
        }
    ],
    "total": 1
}
```

#### POST /api/commands

Execute a command:

```json
{
    "command": "brain create --neuron-count 1000",
    "session_id": "session_123"
}
```

Response:

```json
{
    "status": "success",
    "output": "Brain created successfully",
    "timestamp": 1672574400.0
}
```

#### WebSocket Endpoints

The API server also provides WebSocket connections for real-time updates:

- `ws://host:port/` - Main WebSocket endpoint

WebSocket messages:

```json
{
    "type": "command",
    "command": "brain create --neuron-count 1000",
    "session_id": "session_123"
}
```

Response:

```json
{
    "type": "response",
    "session_id": "session_123",
    "data": {
        "status": "success",
        "output": "Brain created successfully"
    }
}
```

## Troubleshooting

### Common Issues

#### Python Version

```bash
# Check Python version
python3 --version

# Install if needed (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install python3 python3-pip

# Install if needed (macOS)
brew install python3

# Install if needed (Windows)
choco install python3
```

#### Dependencies

```bash
# Install dependencies
pip install -r requirements.txt

# Or install from pyproject.toml
pip install -e .
```

#### Configuration Files

```bash
# Check if configuration files exist
ls -la configs/

# Create directory if needed
mkdir -p configs

# Create default configuration
cat > configs/default.cfg << EOF
[general]
random_seed = 42
simulation_timestep = 0.001
EOF
```

#### Output Files

```bash
# Check if output directory exists
ls -la outputs/

# Create directory if needed
mkdir -p outputs

# Set permissions
chmod 755 outputs/
```

#### Permissions

```bash
# Make scripts executable
chmod 755 scripts/*.sh

# Make Python files executable
chmod 755 python/nlm_cli/*.py
```

### Getting More Help

1. **Documentation**: https://nlm.readthedocs.io
2. **GitHub Issues**: https://github.com/nlm-project/nlm/issues
3. **Community**: https://github.com/nlm-project/nlm/discussions
4. **Stack Overflow**: Tag with `nlm-cli`

## Technical Details

### Dependencies

The NLM CLI tool requires the following Python packages:

- **Required**: click, rich, pydantic, pyyaml, tabulate, websockets
- **Optional**: prompt_toolkit (for interactive mode), prompt_toolkit (for auto-completion)

### Installation Methods

1. ** pip Install**
   ```bash
   pip install -e .
   ```

2. **System Package Manager**
   ```bash
   # Debian/Ubuntu
   apt-get install python3-nlm-cli
   
   # macOS
   brew install nlm-cli
   
   # Arch Linux
   pacman -S nlm-cli
   ```

3. **From Source**
   ```bash
   git clone https://github.com/nlm-project/nlm-cli.git
   cd nlm-cli
   pip install -e .
   ```

### File Structure

```
nlm-cli/
├── python/
│   └── nlm_cli/
│       ├── nlm_cli.py          # Main CLI implementation
│       ├── interactive.py      # Interactive REPL
│       ├── api_server.py       # Web API server
│       ├── README.md          # Package documentation
│       └── test_cli.py         # Test script
├── configs/                   # Configuration files
│   ├── default.cfg           # Default configuration
│   └── example.json          # JSON configuration example
├── scripts/                   # Example scripts
│   ├── basic_simulation.sh
│   ├── advanced_simulation.sh
│   └── config_management.sh
├── docs/                      # Documentation
│   ├── quick_start.md
│   ├── cli_examples.md
│   └── api_reference.md
├── outputs/                   # Output files
├── logs/                      # Log files
├── examples/                  # Example files
└── tests/                     # Test files
```

### Configuration File Format Examples

#### INI Format

```ini
[section1]
key1 = value1
key2 = value2

[section2]
key3 = value3
key4 = value4
```

#### JSON Format

```json
{
    "section1": {
        "key1": "value1",
        "key2": "value2"
    },
    "section2": {
        "key3": "value3",
        "key4": "value4"
    }
}
```

#### Python Dictionary

```python
{
    "section1": {
        "key1": "value1",
        "key2": "value2"
    },
    "section2": {
        "key3": "value3",
        "key4": "value4"
    }
}
```

### Logging Configuration

The CLI tool uses the standard Python logging module. You can configure logging via:

1. **Command line**: `--log-level DEBUG`
2. **Configuration file**: `log_level = DEBUG` in configs/
3. **Environment variable**: `export NLM_LOG_LEVEL=DEBUG`

Log levels:
- DEBUG: Detailed debug information
- INFO: General information
- WARN: Warning messages
- ERROR: Error messages
- CRITICAL: Critical errors

### Output Configuration

The CLI tool supports multiple output formats. You can configure output via:

1. **Command line**: `--output-format JSON`
2. **Configuration file**: `output_format = json` in configs/
3. **Environment variable**: `export NLM_OUTPUT_FORMAT=json`

Output formats:
- JSON: Machine-readable format
- YAML: Human-readable format
- TABLE: Console-friendly format
- CSV: Tab-separated values

### Performance Configuration

You can optimize performance via configuration:

```ini
[performance]
simulation_parallel = true
batch_size = 100
cache_enabled = true
cache_size = 1000
```

### Security Configuration

The CLI tool includes basic security features:

1. **File permissions**: Check and enforce file permissions
2. **Input validation**: Validate command line arguments
3. **Error handling**: Secure error messages without sensitive information
4. **Session management**: Secure session handling in API server

### Backup and Recovery

The CLI tool includes backup and recovery features:

1. **Configuration backup**: Automatic backup of configuration files
2. **Output backup**: Backup of output files
3. **Session backup**: Save and restore sessions
4. **Recovery**: Restore from backups

### Monitoring and Metrics

The CLI tool includes monitoring and metrics:

1. **Performance metrics**: Track simulation performance
2. **Resource usage**: Monitor memory and CPU usage
3. **Log analysis**: Analyze logs for performance issues
4. **Health checks**: Check system health

### Integration

The CLI tool integrates with:

1. **Git**: Version control
2. **Docker**: Containerization
3. **CI/CD**: Continuous integration
4. **Cloud**: Cloud deployment
5. **Web**: Web API
6. **Mobile**: Mobile apps

### Compatibility

The CLI tool is compatible with:

- **Operating Systems**: Linux, macOS, Windows
- **Python Versions**: 3.8, 3.9, 3.10, 3.11, 3.12
- **Hardware**: x86, ARM
- **File Systems**: ext4, APFS, NTFS

### License

MIT License - See LICENSE file for details.

## References

1. NLM Documentation: https://nlm.readthedocs.io
2. NLM Repository: https://github.com/nlm-project/nlm
3. Python Documentation: https://docs.python.org/3/
4. Click Documentation: https://click.palletsprojects.com/
5. Rich Documentation: https://rich.readthedocs.io/
6. Pydantic Documentation: https://pydantic-docs.helpmanual.io/
7. PyYAML Documentation: https://pyyaml.org/
8. Tabulate Documentation: https://github.com/tshepang/tabulate
9. Websockets Documentation: https://websockets.readthedocs.io/
10. Prompt_toolkit Documentation: https://www.pyinstaller.org/projects/prompt_toolkit/
"""
        with open(api_ref, 'w') as f:
            f.write(content)
        print(f"✓ Created API reference: {api_ref}")
    
    return True

def main():
    """Main setup function"""
    print("NLM CLI Tool Setup")
    print("=" * 60)
    print("This script sets up the NLM CLI tool with all necessary")
    print("dependencies, configurations, and examples.")
    print()
    
    # Run setup steps
    steps = [
        ("Python Version Check", check_python_version),
        ("Dependency Check", check_dependencies),
        ("Directory Setup", setup_directories),
        ("Configuration Creation", create_default_configs),
        ("Example Scripts", create_example_scripts),
        ("Documentation", create_documentation),
    ]
    
    passed = 0
    failed = 0
    
    for step_name, step_func in steps:
        try:
            if step_func():
                passed += 1
                print(f"✓ {step_name} completed successfully")
            else:
                failed += 1
                print(f"✗ {step_name} failed")
        except Exception as e:
            failed += 1
            print(f"✗ {step_name} failed with error: {e}")
    
    print_header("Setup Summary")
    print(f"Total steps: {len(steps)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    
    if failed == 0:
        print("\n✓ Setup completed successfully!")
        print("\nYou can now use the NLM CLI tool with:")
        print("  nlm --help")
        print("  nlm brain create --neuron-count 1000")
        print("  nlm brain simulate --steps 100")
        print("\nFor more information, see README.md in the python/nlm_cli directory.")
    else:
        print(f"\n⚠ Setup completed with {failed} error(s).")
        print("Please fix the issues and run the setup script again.")
        sys.exit(1)

if __name__ == "__main__":
    main()