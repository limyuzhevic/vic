# NLM CLI Tools

This directory contains the command-line interface tools for the NLM (Neural Learning Machine) project. The CLI tools provide comprehensive utilities for managing neural simulation experiments, configuration management, analysis, and visualization.

## Overview

The NLM CLI tools are organized into several categories:

### Configuration Tools (config-*)
- `config-info`: Show configuration details
- `config-validate`: Validate configuration files
- `config-merge`: Merge multiple configuration files
- `config-export`: Export configuration to different formats
- `config-list`: List available configurations and presets
- `config-compare`: Compare configuration files

### Experiment Management Tools (exp-*)
- `exp-info`: Show experiment details
- `exp-run`: Run experiments with different configurations
- `exp-list`: List available experiments
- `exp-compare`: Compare experiment results
- `exp-export`: Export experiment results
- `exp-merge`: Merge multiple experiment results

### Brain Management Tools (brain-*)
- `brain-info`: Show brain configuration and status
- `brain-save`: Save brain state
- `brain-load`: Load brain state
- `brain-compare`: Compare brain states
- `brain-export`: Export brain state
- `brain-import`: Import brain state

### World/Environment Tools (world-*)
- `world-info`: Show world configuration
- `world-save`: Save world state
- `world-load`: Load world state
- `world-test`: Test world configuration
- `world-explore`: Explore world state

### Analysis Tools (analyze-*)
- `analyze-metrics`: Analyze simulation metrics
- `analyze-performance`: Analyze system performance
- `analyze-results`: Analyze experiment results
- `analyze-memory`: Analyze memory usage
- `analyze-neural`: Analyze neural network properties

### Utility Tools
- `version`: Show version information
- `health-check`: Perform system health checks
- `stats`: Show system statistics
- `logs`: Show logs
- `benchmark`: Run performance benchmarks
- `benchmark-config`: Configure benchmarks

### Visualization Tools (viz-*)
- `viz-simulation`: Visualize simulation
- `viz-results`: Visualize experiment results
- `viz-brain`: Visualize brain structure
- `viz-metrics`: Visualize metrics
- `viz-comparison`: Visualize comparison data

### Development Tools
- `debug`: Debug current session
- `trace`: Enable/disable tracing
- `profile`: Start/stop profiling
- `monitor`: Start/stop monitoring
- `snapshot`: Create system snapshot
- `restore`: Restore from snapshot

### Advanced Tools
- `batch-exec`: Execute batch operations
- `parallel-exec`: Execute parallel operations
- `distributed`: Distributed execution
- `scale-out`: Scale out operations
- `load-balance`: Load balancing

## Getting Started

### Installation

The CLI tools are built as part of the NLM project. To build and install:

```bash
cd /workspace/app
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Basic Usage

To see all available commands:

```bash
./nlm-cli --help
```

To see commands in a specific category:

```bash
./nlm-cli --help | grep "config-"
./nlm-cli --help | grep "exp-"
./nlm-cli --help | grep "brain-"
```

### Configuration

The CLI tools use the same configuration system as the main NLM application. Configuration can be specified:

1. **Command-line options**:
   ```bash
   ./nlm-cli config-info --config configs/experiment.json
   ```

2. **Configuration presets**:
   ```bash
   ./nlm-cli config-info --preset research
   ```

3. **Configuration files**:
   ```bash
   # Traditional format
   random_seed=42
   neuron_count=1000
   
   # Save to file
   ./nlm-cli config-info --config my_config.cfg
   ```

## Configuration File Formats

### Traditional Format (.cfg)
```
key=value
another_key=another_value
```

### JSON Format (.json)
```json
{
  "key": "value",
  "another_key": "another_value"
}
```

## Available Presets

- **research**: Detailed logging, validation enabled
- **performance**: Optimized for speed, reduced logging
- **education**: Simplified configuration, verbose output
- **debug**: Maximum verbosity, all debugging enabled
- **production**: Minimal output, validation enabled

## Tool Examples

### Configuration Tools

```bash
# Show configuration information
./nlm-cli config-info

# Validate configuration file
./nlm-cli config-validate configs/default.json

# Merge configuration files
./nlm-cli config-merge configs/base.json configs/experiment.json merged.json

# Export configuration
./nlm-cli config-export configs/default.json exported.yaml --format yaml
```

### Experiment Tools

```bash
# Show experiment information
./nlm-cli exp-info learning_experiment

# Run experiment with configuration
./nlm-cli exp-run --experiment my_exp --config configs/experiment.json

# Compare experiment results
./nlm-cli exp-compare results1.json results2.json
```

### Brain Tools

```bash
# Show brain information
./nlm-cli brain-info

# Save brain state
./nlm-cli brain-save brain_state.json

# Load brain state
./nlm-cli brain-load saved_state.json
```

### Analysis Tools

```bash
# Analyze simulation metrics
./nlm-cli analyze-metrics --input results.csv --output analysis.png

# Analyze performance
./nlm-cli analyze-performance --benchmark benchmark.json

# Analyze experiment results
./nlm-cli analyze-results --input exp_results.json
```

## Development

### Adding New Tools

To add a new CLI tool:

1. Create a new header file in `src/tools/` with your tool implementation
2. Implement the base `CLITool` class
3. Add your tool to the CLI engine registration
4. Update documentation if needed

### Tool Structure

Each tool should implement:

- `getName()`: Unique command name
- `getDescription()`: Human-readable description
- `getUsage()`: Command usage syntax
- `getType()`: Command category (CONFIG, EXPERIMENT, BRAIN, etc.)
- `getCategory()`: Sub-category within the type
- `execute()`: Main tool logic
- `validateArgs()`: Argument validation

### Testing

Tools can be tested using the existing test framework or by running them directly with various inputs.

## Performance Tips

1. **Use presets**: Apply performance or research presets for optimized behavior
2. **Batch operations**: Use `batch-exec` for running multiple commands
3. **Parallel execution**: Use `parallel-exec` for parallel processing
4. **Configuration files**: Use configuration files for complex setups
5. **Dry run**: Use `--dry-run` for validation without execution

## Troubleshooting

### Common Issues

1. **Tool not found**: Check command name and spelling
2. **Configuration error**: Use `--validate-config` to check configuration
3. **File not found**: Ensure the file exists and path is correct
4. **Permission denied**: Check file permissions

### Getting Help

- `nlm-cli --help`: Show all available commands
- `nlm-cli <command> --help`: Show help for specific command
- `nlm-cli --version`: Show version information

## Integration with Main NLM Application

The CLI tools can be integrated with the main NLM application in several ways:

### Command Line Interface

The main NLM application (`nlm`) can be modified to use the CLI tools for better user experience:

```bash
# Run main simulation
nlm --config experiment.json

# Use CLI tools for configuration
nlm-cli config-validate experiment.json
nlm-cli config-info
```

### API Integration

The CLI tools can be used programmatically through the `CLIEngine` class:

```cpp
#include "src/cli/CLIEngine.hpp"

int main() {
    nlm::CLIEngine engine;
    nlm::Config config;
    
    // Execute a command
    engine.executeCommand("config-info", {}, config);
    
    return 0;
}
```

## Future Enhancements

The CLI tools framework supports easy addition of new tools:

1. **New command types**: Add new `CommandType` enum values
2. **New tool categories**: Extend the `getCategory()` method
3. **Advanced features**: Add new flags and options to tools
4. **Integration**: Integrate with additional NLM components
5. **Documentation**: Generate comprehensive documentation automatically

## License

This CLI tools framework is part of the NLM (Neural Learning Machine) project and is licensed under the same terms as the main NLM application.