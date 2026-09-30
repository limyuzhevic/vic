.kilo/command/AGENTS.md
Name: AGENTS.md configuration file
Summary: Documentation for Kilo commands and agents

# NLM Python Scripting Interface - AGENTS Configuration

## Overview

This document describes the commands and agents available for the NLM (Neural Learning Machine) Python scripting interface.

## Available Commands

### Core Interface Commands

- `init`: Initialize NLM system with default configuration
- `reset`: Reset the entire NLM system
- `status`: Display current system status
- `pause`: Pause simulation
- `resume`: Resume simulation
- `step`: Run a single simulation step
- `run`: Run simulation for a specified duration

### Brain Control Commands

- `inject <type> [params]`: Inject stimulus into brain
  - Types: `current`, `type`, `sensory`
  - Parameters vary by type
- `save <filepath>`: Save brain state to file
- `load <filepath>`: Load brain state from file
- `state`: Display current brain state

### Visualization Commands

- `plot <type> [save]`: Create visualization
  - Types: `neurons`, `regions`, `firing`
- `visualize`: Start visualization dashboard

### Plugin Commands

- `plugins`: List loaded plugins
- `plugin <action> <name>`: Load or unload plugin

### Configuration Commands

- `config <filepath>`: Load configuration from file
- `config-save <filepath>`: Save configuration to file

### Debug Commands

- `trace`: Enable debug tracing
- `vars`: List variables
- `history`: Show command history

### Control Commands

- `exit`: Exit REPL
- `quit`: Exit REPL

## Advanced Commands

### Script Commands

- `script <filepath>`: Execute Python script
- `batch <file>`: Execute batch of commands
- `replay <filepath>`: Replay simulation from recorded data

### Monitoring Commands

- `monitor <duration>`: Monitor system for specified duration
- `profile <command>`: Profile command execution
- `benchmark`: Run benchmarks

### Integration Commands

- `export <format> <filepath>`: Export data in specified format
  - Formats: `json`, `csv`, `numpy`, `matlab`
- `import <format> <filepath>`: Import data from specified format

### Administration Commands

- `users`: List all users
- `roles`: List all roles
- `permissions <user>`: Show user permissions
- `security-policy`: Show current security policy

## Agent Types

### REPL Agent

The REPL (Read-Eval-Print Loop) agent provides an interactive interface for:
- Direct command execution
- Python code evaluation
- Interactive exploration
- Real-time feedback

### Script Agent

The Script agent executes Python scripts with NLM interface:
- Script-based control
- Batch operations
- Automation
- Custom workflows

### Visualization Agent

The Visualization agent provides:
- Real-time plotting
- Dashboard creation
- Data visualization
- Report generation

### Plugin Agent

The Plugin agent manages plugin lifecycle:
- Plugin discovery
- Plugin loading/unloading
- Plugin configuration
- Plugin dependency management

### Monitoring Agent

The Monitoring agent provides:
- System monitoring
- Resource tracking
- Event logging
- Alert management

## Example Usage

### Basic NLM Control

```python
# Initialize NLM system
pynlm.init()

# Run simulation
pynlm.run(duration=10, dt=0.1)

# Inject stimulus
pynlm.inject(type='current', neuron_id=0, current=1.0)

# Save brain state
pynlm.save(filepath='brain_state.json')
```

### Advanced Scripting

```python
# Create custom script
import pynlm

# Initialize with custom config
pynlm.config(filepath='custom_config.json')

# Run simulation with monitoring
pynlm.init()
pynlm.run(duration=5, dt=0.01)

# Analyze results
pynlm.visualize()
```

### Plugin Usage

```python
# Load a plugin
pynlm.plugins.load(name='custom_visualization')

# Use plugin functionality
pynlm.visualize.plugin('custom_visualization')
```

## Configuration

### Default Configuration

```json
{
    "default_config": {
        "simulation": {
            "dt": 1.0,
            "duration": 10.0,
            "auto": true
        },
        "brain": {
            "neuron_count": 1000,
            "synapse_density": 0.1,
            "learning_rate": 0.1
        },
        "visualization": {
            "backend": "matplotlib",
            "theme": "default"
        }
    }
}
```

### Command Configuration

```json
{
    "commands": {
        "init": {
            "description": "Initialize NLM system",
            "arguments": [],
            "category": "system"
        },
        "run": {
            "description": "Run simulation for duration",
            "arguments": ["duration", "dt"],
            "category": "simulation"
        }
    }
}
```

### Plugin Configuration

```json
{
    "plugins": {
        "custom_visualization": {
            "name": "Custom Visualization",
            "version": "1.0.0",
            "description": "Custom visualization plugin",
            "author": "NLM Authors",
            "type": "visualization",
            "enabled": true,
            "config": {
                "theme": "dark",
                "animation": true
            }
        }
    }
}
```

## Command Line Interface

### Basic Usage

```bash
# Start REPL
python -m pynlm.interface

# Execute command
python -m pynlm.interface command arguments

# Load script
python -m pynlm.interface script filepath
```

### Advanced Usage

```bash
# Start with configuration
python -m pynlm.interface --config config.json

# Run with monitoring
python -m pynlm.interface --monitor 60

# Execute batch commands
python -m pynlm.interface batch commands.txt
```

## Integration Examples

### Jupyter Notebook Integration

```python
# Import pynlm in notebook
import pynlm

# Initialize and run simulation
brain = pynlm.initialize_nlm()
results = pynlm.quick_simulation(duration=5)

# Create visualizations
pynlm.create_plot(brain_state=brain)
```

### Script Integration

```python
# Create standalone script
script = """
import pynlm

pynlm.init()
pynlm.run(duration=10)
pynlm.save('results.json')
"""

# Execute script
pynlm.run_script(script)
```

### Plugin Integration

```python
# Create plugin
plugin = pynlm.create_plugin('custom_tool')
plugin.initialize(config={'setting': 'value'})

# Use plugin
plugin.execute()
```

## Best Practices

### Security

1. Use appropriate access levels for different users
2. Implement proper input validation
3. Use sandboxing for untrusted code
4. Monitor system resources
5. Regular security audits

### Performance

1. Use asynchronous operations for long-running tasks
2. Implement caching for frequently accessed data
3. Use efficient data structures
4. Monitor and optimize resource usage
5. Implement parallel processing where appropriate

### Maintainability

1. Follow consistent coding style
2. Use meaningful variable names
3. Document code and functions
4. Implement error handling
5. Use version control

### Testing

1. Unit test individual components
2. Integration test the entire system
3. Performance test critical paths
4. Security test input validation
5. Stress test resource limits

## Troubleshooting

### Common Issues

1. **Plugin loading errors**: Check plugin dependencies and configuration
2. **Performance issues**: Monitor system resources and optimize code
3. **Security errors**: Review access controls and input validation
4. **Visualization errors**: Check available libraries and dependencies

### Debugging

```bash
# Enable debug mode
python -m pynlm.interface --debug

# Check logs
python -m pynlm.interface --log-level debug

# Profile performance
python -m pynlm.interface --profile
```

### Support

For additional support, refer to:
- Documentation: https://docs.nlm.org
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Community Forum: https://community.nlm.org
- API Reference: https://api.nlm.org
