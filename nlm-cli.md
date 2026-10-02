#!/usr/bin/env python3
"""
NLM Command Line Interface

Simple command-line interface for common NLM operations.

Usage:
    nlm-cli <command> [options]

Commands:
    create <brain_name>          Create a new brain
    simulate <brain_name> <steps> Run simulation
    config <file>                Load configuration
    stats <brain_name>           Show brain statistics
    help                         Show this help

Examples:
    nlm-cli create mybrain
    nlm-cli simulate mybrain 1000
    nlm-cli config myconfig.json

Advanced commands:
    create-optimized                      Create optimized brain for speed
    create-balanced                       Create balanced brain (default)
    run-agent <world_size> <steps>        Run complete agent simulation
    print-stats <world_name> <agent_name>  Print detailed simulation stats

## Command Examples

### Basic Usage
    nlm-cli create mybrain
    nlm-cli simulate mybrain 1000
    nlm-cli config myconfig.json

### Agent Simulation
    nlm-cli run-agent "5x5" 500
    nlm-cli run-agent "10x10" 1000 --learning

### Configuration
    nlm-cli config production_config.json
    nlm-cli config development_config.json

## Configuration Files

The CLI supports JSON configuration files. Examples:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "learning": {
        "curiosity_enabled": true,
        "reward_modulation_enabled": true
    }
}
```

## Output Format

The CLI provides consistent output formatting:

- **Progress**: Shows step-by-step progress for long operations
- **Statistics**: Provides detailed brain and simulation statistics
- **Errors**: Clear, actionable error messages
- **Help**: Comprehensive usage documentation

## Integration

The CLI can be integrated into:

### Python Scripts
    ```python
    import subprocess
    result = subprocess.run(['nlm-cli', 'create', 'mybrain'], capture_output=True)
    ```

### Shell Scripts
    ```bash
    #!/bin/bash
    nlm-cli create mybrain
    nlm-cli simulate mybrain 100
    nlm-cli stats mybrain
    ```

### Automated Workflows
    - Batch processing of simulations
    - Configuration testing
    - Performance benchmarking
    - Experiment automation

## File Locations

- **CLI Script**: `nlm-cli.py`
- **Python Module**: `pynlm` (install via `pip install .`)
- **Documentation**: `README.md` for complete usage
- **Examples**: Python examples in documentation

## Installation and Usage

### Install Python Package
```bash
pip install .
```

### Run CLI
```bash
# From project root
./nlm-cli.py <command> [args]

# Make executable
chmod +x nlm-cli.py
# Then run directly
./nlm-cli.py <command> [args]
```

### Common Commands

#### Create Brains
```bash
# Default brain (balanced performance)
nlm-cli create mybrain

# Optimized for speed (fewer neurons, simpler learning)
nlm-cli create-optimized fastbrain

# Balanced configuration (default)
nlm-cli create-balanced normalbrain
```

#### Run Simulations
```bash
# Simple brain simulation
nlm-cli simulate mybrain 1000

# Agent simulation with world
nlm-cli run-agent "8x8" 500

# Long simulation with learning
nlm-cli run-agent "10x10" 2000 --learning
```

#### Configuration
```bash
# Load configuration
nlm-cli config production.json

# Show configuration keys
nlm-cli config myconfig.json | grep brain.neuron_count
```

#### Diagnostics
```bash
# Show brain statistics
nlm-cli stats mybrain

# Show detailed simulation stats
nlm-cli print-stats myworld myagent
```

## Command Reference

### Basic Commands
- `help` - Show this help
- `create <name>` - Create brain with default config
- `simulate <name> <steps>` - Run simulation
- `config <file>` - Load and display configuration
- `stats <name>` - Show brain statistics

### Advanced Commands
- `create-optimized` - Create optimized brain
- `create-balanced` - Create balanced brain  
- `run-agent <world_size> <steps>` - Run agent simulation
- `print-stats <world> <agent>` - Show detailed stats

### Configuration Options
- `--learning` - Enable learning capabilities
- `--no-learning` - Disable learning (faster)
- `--verbose` - Show detailed progress
- `--quiet` - Minimal output

## Troubleshooting

### Command Not Found
```bash
# Ensure Python package is installed
pip install .

# Make CLI executable
chmod +x nlm-cli.py
```

### Import Errors
```bash
# Check Python path
python -c "import sys; print(sys.path)"

# Install dependencies
pip install pybind11 scikit-build-core
```

### Configuration Errors
```bash
# Check JSON syntax
python -m json.tool config.json

# Validate configuration structure
nlm-cli config config.json
```

### Simulation Errors
```bash
# Check available memory
free -h

# Reduce simulation size
nlm-cli simulate mybrain 100
```

## Performance Tips

### For Faster Simulations
- Use `--no-learning` flag to disable neuromodulation
- Reduce `brain.neuron_count` in config
- Use `--quiet` to reduce output overhead

### For Better Learning
- Enable `--learning` flag for exploration
- Use larger world sizes for more complex behavior
- Enable curiosity-based exploration

### For Debugging
- Use `--verbose` to see step-by-step progress
- Use `--quiet` for minimal output in automated scripts

## Advanced Features

### Batch Processing
```bash
#!/bin/bash
for config in configs/*.json; do
    nlm-cli config "$config"
    nlm-cli create "brain_$(basename $config .json)"
    nlm-cli simulate "brain_$(basename $config .json)" 100
    nlm-cli stats "brain_$(basename $config .json)"
done
```

### Parallel Execution
```bash
#!/bin/bash
for brain in brains/*.json; do
    (nlm-cli config "$brain" && nlm-cli simulate "$(basename $brain .json)" 100) &
done
wait
```

The CLI provides a flexible interface for common NLM operations with support for both simple and advanced use cases.