# NLM CLI Dependencies

This document describes the dependencies needed for the NLM CLI.

## Python Dependencies

The NLM CLI requires the following Python packages:

### Required Dependencies
- `pybind11` - Required for Python bindings of NLM
- `click` - Command line interface framework
- `argcomplete` - Shell tab completion
- `colorama` - Colored terminal output
- `configparser` - Configuration file parsing
- `datetime` - Date and time handling
- `json` - JSON serialization/deserialization
- `pathlib2` - Cross-platform path handling

### Optional Dependencies
- `pyyaml` - YAML file support
- `toml` - TOML file support
- `numpy` - Numerical computations
- `pandas` - Data analysis
- `matplotlib` - Plotting capabilities

## Installation Instructions

### Using pip (if Python is available)
```bash
pip install pynlm[all]
pip install -r requirements.txt
```

### From source
```bash
# Clone the repository
# Build NLM C++ library
# Build Python bindings
# Install Python module

# Or use the provided build script
bash build.sh
```

## Environment Variables

### NLM_CONFIG_PATH
- Default: `~/.config/nlm/` or `./.nlm/`
- Description: Path to NLM configuration directory
- Example: `export NLM_CONFIG_PATH="/path/to/config"`

### NLM_LOG_LEVEL
- Default: `info`
- Description: Logging level
- Options: `debug`, `info`, `warning`, `error`
- Example: `export NLM_LOG_LEVEL="debug"`

### NLM_OUTPUT_FORMAT
- Default: `text`
- Description: Default output format
- Options: `text`, `json`, `yaml`
- Example: `export NLM_OUTPUT_FORMAT="json"`

## Configuration Files

### .nlm_config (Global)
Global configuration for NLM CLI:

```ini
[cli]
name = "nlm"
version = "0.1.0"
author = "NLM Team"
description = "Enhanced Command Line Interface for NLM"

[cli.output]
format = "text"
verbose = false
debug = false
quiet = false
progress = true

[cli.history]
enabled = true
max_entries = 1000
persistent = false
filepath = ".nlm_history"
```

### project.nlm_config (Project-specific)
Project-specific configuration:

```ini
[project]
name = "My NLM Project"
description = "Project description"
version = "1.0.0"

[project.config]
config_file = "configs/default.cfg"
load_on_start = true

[project.brain]
auto_initialize = true
brain_type = "standard"

[project.world]
auto_reset = true
reset_interval = 1000
```

## Scripting

### Script File Format (.nlm)
NLM script files support:

```bash
# Comments
nlm command arg1 arg2

# Multiple commands separated by newlines
nml config set neurons 100
nml brain initialize
nml run

# Conditional execution
nml config get neurons > 500 && nml brain step 10
```

### Script Variables
Script variables can be set:

```bash
# Set variable
nml script var MY_VAR "value"

# Use variable
nml brain step $MY_VAR

# Export variables for subprocess
export MY_VAR="value"
nlm run
```

## Commands Reference

### Basic Commands
- `nlm` - Start interactive CLI
- `nml --help` - Show help
- `nml --version` - Show version
- `nml --config file` - Load configuration
- `nml --script file.nlm` - Execute script

### Configuration Commands
- `nml config list` - List all configuration keys
- `nml config get <key>` - Get config value
- `nml config set <key> <value>` - Set config value
- `nml config validate` - Validate configuration
- `nml config save <file>` - Save to file
- `nml config load <file>` - Load from file
- `nml config schema` - Show schema
- `nml config info` - Show info

### Brain Commands
- `nml brain status` - Show brain status
- `nml brain reset` - Reset brain
- `nml brain load <file>` - Load brain state
- `nml brain save <file>` - Save brain state
- `nml brain info` - Show brain info
- `nml brain step <steps>` - Run brain steps
- `nml brain initialize` - Initialize brain
- `nml brain terminate` - Terminate brain

### Experiment Commands
- `nml experiment run` - Run experiments
- `nml experiment list` - List experiments
- `nml experiment info <name>` - Show experiment info
- `nml experiment metrics <name>` - Show metrics
- `nml experiment status` - Show status

### World Commands
- `nml world info` - Show world state
- `nml world reset` bibliotecas
- `nml world step` - Take single step

### Monitor Commands
- `nml monitor start` - Start monitoring
- `nml monitor stop` - Stop monitoring
- `nml monitor status` - Show status
- `nml monitor logs` - Show logs
- `nml monitor metrics` - Show metrics

### Script Commands
- `nml script execute <file>` - Execute script
- `nml script list` - List scripts
- `nml script save <file>` - Save script
- `nml script info <file>` - Show script info
- `nml script validate <file>` - Validate script

## Error Codes

The NLM CLI uses the following exit codes:

- `0` - Success
- `1` - General error
- `2` - Usage error (invalid arguments)
- `3` - Command not found
- `4` - Configuration error
- `5` - Brain error
- `6` - Experiment error
- `7` - World error
- `8` - Monitor error
- `9` - Script error

## Troubleshooting

### Common Issues

1. **Python Module Not Available**
   - Ensure the NLM Python module is properly built and installed
   - Check: `python -c "import pynlm; print(pynlm.__version__)"`

2. **Configuration File Not Found**
   - Ensure the configuration file exists
   - Check file permissions: `ls -la ~/.config/nlm/`

3. **Brain Not Initialized**
   - Use `nml brain initialize` before running simulations

4. **Script Execution Failed**
   - Check script syntax
   - Ensure script file exists and is readable

5. **Tab Completion Not Working**
   - Ensure `argcomplete` is installed
   - Run: `nml completion install bash`

## Development

### Adding New Commands

1. Create a new command file in `cli/commands/`
2. Implement the command class
3. Register the command in `cli/cli.py`
4. Add documentation in `docs/commands/`
5. Update `cli/__init__.py`

### Adding New Plugins

1. Create a plugin in `cli/plugins/`
2. Implement the plugin interface
3. Configure in `.nlm_config`
4. Ensure plugin follows the NLM CLI plugin conventions

### Testing

Run tests:
```bash
python -m pytest tests/
python -m unittest discover tests/
```

Run integration tests:
```bash
nml test integration
nml test performance
```

## Migration Guide

### From Old CLI

If migrating from the old NLM CLI:

```bash
# Old commands
nlm --help
nlm --config file.cfg

# New commands
nml --help
nml --config file.cfg
```

### Configuration Migration

The new configuration format is compatible with the old format:

```ini
# Old format (still supported)
[section]
key = value

# New format (recommended)
[config.section]
key = value
```

All existing configurations will be automatically migrated.

## Support

For support, please:
1. Check the documentation
2. Report issues on GitHub
3. Visit the NLM website
4. Contact the development team

## License

This project is licensed under the MIT License.
