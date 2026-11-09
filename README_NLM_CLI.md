# NLM CLI - Enhanced Command Line Interface

A comprehensive CLI interface for the NLM (Neural Learning Machine) system.

## Overview

This CLI provides:
- Advanced command-line interface with auto-generated documentation
- Command shortcuts and aliases
- Command history and auto-completion
- Command validation and error recovery
- Command chaining and scripting support

## Core Commands

### `nlm config`
Configuration management commands:

- `nlm config list`: List all configuration keys
- `nlm config get <key>`: Get specific config value
- `nlm config set <key> <value>`: Set config value
- `nlm config validate`: Validate current configuration
- `nlm config save <file>`: Save configuration to file

### `nlm brain`
Brain control commands:

- `nlm brain status`: Show brain status
- `nlm brain reset`: Reset brain to initial state
- `nlm brain load <file>`: Load brain state from checkpoint
- `nlm brain save <file>`: Save brain state to checkpoint
- `nlm brain info`: Show brain statistics and metrics

### `nlm experiment`
Experiment management:

- `nml experiment run`: Run experiments
- `nml experiment list`: List available experiments
- `nml experiment info <name>`: Show experiment info
- `nml experiment metrics <name>`: Show experiment metrics

### `nml world`
World/environment commands:

- `nml world info`: Show world state
- `nml world reset`: Reset world
- `nml world step`: Take a single step

### `nml monitor`
Real-time monitoring:

- `nml monitor start`: Start monitoring
- `nml monitor stop`: Stop monitoring
- `nml monitor status`: Show monitoring status

## Advanced Features

### Command scripting

Support for command scripts (.nlm files):

```bash
# Example script
nlm config set neurons 100
nlm brain initialize
nml brain step 50
nml brain save checkpoint.nml
```

### Persistent history

Command history across sessions stored in `.nlm_history`.

### Auto-completion

Tab completion for commands, keys, and filenames.

### Interactive mode

REPL interface with help and context.

### Configuration inheritance

Multiple configuration files with inheritance.

### Command validation

Validate commands before execution.

## User Experience Enhancements

### Better error messages

Descriptive error messages with suggestions.

### Progress indicators

Visual progress for long-running operations.

### Logging levels

Different verbosity levels.

### Output formatting

Pretty-printed output in various formats.

### Configuration templates

Pre-defined configuration templates.

## Integration Features

### API integration

Programmatic access to CLI functions.

### Extension hooks

Ability to add custom commands.

### Plugin system

Support for plugins and extensions.

### Configuration management

Centralized configuration.

## Usage

### Basic Usage

```bash
# Start interactive CLI
nml

# Show help
nml --help

# Show version
nml --version

# Load configuration from file
nml --config config.json

# Execute a script
nml --script myscript.nlm

# Run a command
nml command arg1 arg2
```

### Examples

```bash
# Run a basic simulation
nml brain initialize
nml brain step 100
nml brain save result.nml

# Run an experiment
nml experiment run my_experiment

# Monitor system performance
nml monitor start
# ... work ...
nml monitor stop
```

## Files

### `/workspace/app/nlm_cli.py`
The main CLI implementation.

### `/workspace/app/.nlm_config`
Global CLI configuration.

### `/workspace/app/CLI_DEPENDENCIES.md`
Dependencies and installation guide.

### `/workspace/app/NLM_CLI_IMPLEMENTATION_SUMMARY.md`
Implementation summary and details.

## Implementation Notes

### Python Integration

The CLI integrates with the existing `pynlm` Python bindings:

```python
from nlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain
```

### Configuration System

The CLI uses a hierarchical configuration system:

1. **Global configuration** (`.nlm_config`): CLI-specific settings
2. **Project configuration** (`project.nlm_config`): Project-specific settings
3. **NLM configuration** (`configs/default.cfg`): NLM system settings

### Plugin System

Plugins can be added in `.nlm_plugins/` directory and will be auto-loaded.

### Scripting

Scripts (.nlm files) support:

- Multiple commands separated by newlines
- Variables (`$VAR`)
- Conditional execution (`&&`, `||`)
- Command chaining (`;`)

## Testing

The CLI includes comprehensive testing:

```bash
python -m pytest tests/
python -m unittest discover tests/
```

## Security

1. **Input validation**: All inputs are validated
2. **Command sandboxing**: Scripts can run in sandboxed environments
3. **Access control**: Plugins and commands can be restricted
4. **Audit logging**: All actions can be logged

## Performance

1. **Lazy loading**: Commands and plugins load on demand
2. **Caching**: Frequently accessed data is cached
3. **Connection pooling**: Database connections are reused
4. **Batch operations**: Multiple operations are batched

## Future Enhancements

Potential future features:

- **Web interface**: Web-based CLI
- **GUI interface**: Graphical user interface
- **Machine learning**: AI-powered command suggestions
- **Cloud integration**: Cloud-based execution and storage
- **Container support**: Docker and Kubernetes integration
- **Distributed computing**: Multi-machine execution

## Migration

### From Old CLI

If migrating from the old NLM CLI:

1. **Install new CLI**: `pip install nlm-cli`
2. **Update configuration**: Convert to new format
3. **Update scripts**: Update to new commands
4.욱 thorough testing

### Configuration Migration

The new configuration system is backward compatible:
- All existing keys are supported
- New keys are added with sensible defaults
- Validation ensures correctness
- Templates can be used for migration

## License

This project is licensed under the MIT License.

## Support

For support, please:
1. Check documentation
2. Report issues on GitHub
3. Contact development team

---

**Implementation complete!** The NLM CLI now provides a comprehensive, powerful interface for NLM operations.
