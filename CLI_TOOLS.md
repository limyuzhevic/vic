// CLI Tools Documentation
//
// This document provides comprehensive documentation for all NLM CLI tools.
// It includes tool descriptions, usage examples, and implementation details.

# NLM CLI Tools Documentation

## Overview

The NLM (Neural Learning Machine) project includes a comprehensive suite of command-line interface tools for managing and working with neural simulation experiments. The CLI tools provide powerful utilities for configuration management, experiment control, brain state management, world/exploration, analysis, visualization, development, and advanced operations.

## Tool Categories

### 1. Configuration Tools (config-*)

#### config-info
**Description:** Show configuration details and summary information
**Usage:** `nlm-cli config-info [options]`
**Example:**
```bash
nlm-cli config-info
nlm-cli config-info --verbose
```

#### config-validate
**Description:** Validate configuration files against schema
**Usage:** `nlm-cli config-validate <config_file>`
**Example:**
```bash
nlm-cli config-validate configs/default.json
nlm-cli config-validate configs/experiment.json --validate-strict
```

#### config-merge
**Description:** Merge multiple configuration files
**Usage:** `nlm-cli config-merge <file1> <file2> [options]`
**Example:**
```bash
nlm-cli config-merge configs/base.json configs/experiment.json merged.json
nlm-cli config-merge configs/*.json merged_config.json
```

#### config-export
**Description:** Export configuration to different formats
**Usage:** `nlm-cli config-export <config_file> <output_file> [options]`
**Example:**
```bash
nlm-cli config-export configs/default.json exported.yaml --format yaml
nlm-cli config-export configs/experiment.json results.json
```

#### config-list
**Description:** List available configurations and presets
**Usage:** `nlm-cli config-list [options]`
**Example:**
```bash
nlm-cli config-list
nlm-cli config-list --preset-only
```

#### config-compare
**Description:** Compare configuration files
**Usage:** `nlm-cli config-compare <file1> <file2>`
**Example:**
```bash
nlm-cli config-compare configs/v1.json configs/v2.json
nlm-cli config-compare config1.cfg config2.cfg --diff-detailed
```

### 2. Experiment Management Tools (exp-*)

#### exp-info
**Description:** Show experiment details and metadata
**Usage:** `nlm-cli exp-info <experiment_name> [options]`
**Example:**
```bash
nlm-cli exp-info learning_experiment
nlm-cli exp-info my_exp --config configs/experiment.json
```

#### exp-run
**Description:** Run experiments with different configurations
**Usage:** `nlm-cli exp-run --experiment <name> --config <file> [options]`
**Example:**
```bash
nlm-cli exp-run --experiment my_exp --config configs/experiment.json
nlm-cli exp-run --experiment test_exp --steps 1000 --save-results
```

#### exp-list
**Description:** List available experiments
**Usage:** `nlm-cli exp-list [options]`
**Example:**
```bash
nlm-cli exp-list
nlm-cli exp-list --detailed
```

#### exp-compare
**Description:** Compare experiment results
**Usage:** `nlm-cli exp-compare <file1> <file2>`
**Example:**
```bash
nlm-cli exp-compare results1.json results2.json
nlm-cli exp-compare exp1.csv exp2.csv --compare-metrics
```

#### exp-export
**Description:** Export experiment results
**Usage:** `nlm-cli exp-export <input_file> <output_file> [options]`
**Example:**
```bash
nlm-cli exp-export results.json exp_results.csv --format csv
nlm-cli exp-export simulation.json visualization_data.json
```

#### exp-merge
**Description:** Merge multiple experiment results
**Usage:** `nlm-cli exp-merge <file1> <file2> [options]`
**Example:**
```bash
nlm-cli exp-merge results1.json results2.json merged_results.json
nlm-cli exp-merge exp1.csv exp2.csv merged.csv
```

### 3. Brain Management Tools (brain-*)

#### brain-info
**Description:** Show brain configuration and status
**Usage:** `nlm-cli brain-info [options]`
**Example:**
```bash
nlm-cli brain-info
nlm-cli brain-info --detailed
```

#### brain-save
**Description:** Save brain state
**Usage:** `nlm-cli brain-save <output_file> [options]`
**Example:**
```bash
nlm-cli brain-save brain_state.json
nlm-cli brain-save current_brain.json --compress
```

#### brain-load
**Description:** Load brain state
**Usage:** `nlm-cli brain-load <input_file> [options]`
**Example:**
```bash
nlm-cli brain-load saved_state.json
nlm-cli brain-load brain_backup.json --validate
```

#### brain-compare
**Description:** Compare brain states
**Usage:** `nlm-cli brain-compare <file1> <file2>`
**Example:**
```bash
nlm-cli brain-compare brain1.json brain2.json
nlm-cli brain-compare current.json saved.json --detailed
```

#### brain-export
**Description:** Export brain state
**Usage:** `nlm-cli brain-export <input_file> <output_file> [options]`
**Example:**
```bash
nlm-cli brain-export brain.json neural_network.json --format nn
nlm-cli brain-export current.json brain_backup.json
```

#### brain-import
**Description:** Import brain state
**Usage:** `nlm-cli brain-import <input_file> [options]`
**Example:**
```bash
nlm-cli brain-import neural_network.json
nlm-cli brain-import imported.json --validate --merge
```

### 4. World/Environment Tools (world-*)

#### world-info
**Description:** Show world configuration
**Usage:** `nlm-cli world-info [options]`
**Example:**
```bash
nlm-cli world-info
nlm-cli world-info --detailed
```

#### world-save
**Description:** Save world state
**Usage:** `nlm-cli world-save <output_file> [options]`
**Example:**
```bash
nlm-cli world-save world_state.json
nlm-cli world-save current_world.json --compressed
```

#### world-load
**Description:** Load world state
**Usage:** `nlm-cli world-load <input_file> [options]`
**Example:**
```bash
nlm-cli world-load saved_world.json
nlm-cli world-load backup.json --validate
```

#### world-test
**Description:** Test world configuration
**Usage:** `nlm-cli world-test --config <file> [options]`
**Example:**
```bash
nlm-cli world-test --config test_config.json
nlm-cli world-test --config custom.json --thorough
```

#### world-explore
**Description:** Explore world state
**Usage:** `nlm-cli world-explore [options]`
**Example:**
```bash
nlm-cli world-explore
nlm-cli world-explore --depth 3 --details
```

### 5. Analysis Tools (analyze-*)

#### analyze-metrics
**Description:** Analyze simulation metrics
**Usage:** `nlm-cli analyze-metrics --input <file> --output <file> [options]`
**Example:**
```bash
nlm-cli analyze-metrics --input results.csv --output analysis.png
nlm-cli analyze-metrics --input exp_results.json --output summary.txt
```

#### analyze-performance
**Description:** Analyze system performance
**Usage:** `nlm-cli analyze-performance --benchmark <file> [options]`
**Example:**
```bash
nlm-cli analyze-performance --benchmark benchmark.json
nlm-cli analyze-performance --benchmark perf_data.json --detailed
```

#### analyze-results
**Description:** Analyze experiment results
**Usage:** `nlm-cli analyze-results --input <file> [options]`
**Example:**
```bash
nlm-cli analyze-results --input exp_results.json
nlm-cli analyze-results --input simulation.json --all-metrics
```

#### analyze-memory
**Description:** Analyze memory usage
**Usage:** `nlm-cli analyze-memory --threshold <value> [options]`
**Example:**
```bash
nlm-cli analyze-memory --threshold 1024
nlm-cli analyze-memory --threshold 512 --detailed
```

#### analyze-neural
**Description:** Analyze neural network properties
**Usage:** `nlm-cli analyze-neural <input_file> [options]`
**Example:**
```bash
nlm-cli analyze-neural brain.json
nlm-cli analyze-neural neural_net.json --properties
```

### 6. Utility Tools

#### version
**Description:** Show version information
**Usage:** `nlm-cli version`
**Example:**
```bash
nlm-cli version
```

#### health-check
**Description:** Perform system health checks
**Usage:** `nlm-cli health-check [options]`
**Example:**
```bash
nlm-cli health-check
nlm-cli health-check --thorough
```

#### stats
**Description:** Show system statistics
**Usage:** `nlm-cli stats [options]`
**Example:**
```bash
nlm-cli stats
nlm-cli stats --performance
```

#### logs
**Description:** Show logs
**Usage:** `nlm-cli logs [options]`
**Example:**
```bash
nlm-cli logs --tail 100
nlm-cli logs --level INFO --since "2025-01-01"
```

#### benchmark
**Description:** Run performance benchmarks
**Usage:** `nlm-cli benchmark [options]`
**Example:**
```bash
nlm-cli benchmark
nlm-cli benchmark --duration 60
```

#### benchmark-config
**Description:** Configure benchmarks
**Usage:** `nlm-cli benchmark-config <preset> [options]`
**Example:**
```bash
nlm-cli benchmark-config standard
nlm-cli benchmark-config custom --parameters "neurons=1000"
```

### 7. Visualization Tools (viz-*)

#### viz-simulation
**Description:** Visualize simulation
**Usage:** `nlm-cli viz-simulation --input <file> [options]`
**Example:**
```bash
nlm-cli viz-simulation --input simulation.json
nlm-cli viz-simulation --input simulation.json --format interactive
```

#### viz-results
**Description:** Visualize experiment results
**Usage:** `nlm-cli viz-results --input <file> [options]`
**Example:**
```bash
nlm-cli viz-results --input results.json
nlm-cli viz-results --input exp_results.json --chart-type bar
```

#### viz-brain
**Description:** Visualize brain structure
**Usage:** `nlm-cli viz-brain --input <file> [options]`
**Example:**
```bash
nlm-cli viz-brain --input brain.json
nlm-cli viz-brain --input brain.json --region-specific
```

#### viz-metrics
**Description:** Visualize metrics
**Usage:** `nlm-cli viz-metrics --input <file> [options]`
**Example:**
```bash
nlm-cli viz-metrics --input metrics.csv
nlm-cli viz-metrics --input metrics.json --interactive
```

#### viz-comparison
**Description:** Visualize comparison data
**Usage:** `nlm-cli viz-comparison <file1> <file2> [options]`
**Example:**
```bash
nlm-cli viz-comparison results1.json results2.json
nlm-cli viz-comparison exp1.csv exp2.csv --diff-mode
```

### 8. Development Tools

#### debug
**Description:** Debug current session
**Usage:** `nlm-cli debug [options]`
**Example:**
```bash
nlm-cli debug
nlm-cli debug --session current --level verbose
```

#### trace
**Description:** Enable/disable tracing
**Usage:** `nlm-cli trace --enable|--disable [options]`
**Example:**
```bash
nlm-cli trace --enable
nlm-cli trace --disable --clear
```

#### profile
**Description:** Start/stop profiling
**Usage:** `nlm-cli profile --start|--stop [options]`
**Example:**
```bash
nlm-cli profile --start --duration 30
nlm-cli profile --stop --output profile.json
```

#### monitor
**Description:** Start/stop monitoring
**Usage:** `nlm-cli monitor --start|--stop [options]`
**Example:**
```bash
nlm-cli monitor --start --interval 1
nlm-cli monitor --stop --output monitoring.json
```

#### snapshot
**Description:** Create system snapshot
**Usage:** `nlm-cli snapshot <output_file> [options]`
**Example:**
```bash
nlm-cli snapshot system_snapshot.json
nlm-cli snapshot backup.json --compress
```

#### restore
**Description:** Restore from snapshot
**Usage:** `nlm-cli restore <input_file> [options]`
**Example:**
```bash
nlm-cli restore system_snapshot.json
nlm-cli restore backup.json --validate --merge
```

### 9. Advanced Tools

#### batch-exec
**Description:** Execute batch operations
**Usage:** `nlm-cli batch-exec <command_file> [options]`
**Example:**
```bash
nlm-cli batch-exec commands.txt
nlm-cli batch-exec batch.txt --parallel
```

#### parallel-exec
**Description:** Execute parallel operations
**Usage:** `nlm-cli parallel-exec <command> [options]`
**Example:**
```bash
nlm-cli parallel-exec exp-run --experiment exp1
nlm-cli parallel-exec viz-simulation --input data.json
```

#### distributed
**Description:** Distributed execution
**Usage:** `nlm-cli distributed --host <host> [options]`
**Example:**
```bash
nlm-cli distributed --host cluster1
nlm-cli distributed --host cluster2 --mode sync
```

#### scale-out
**Description:** Scale out operations
**Usage:** `nlm-cli scale-out --nodes <count> [options]`
**Example:**
```bash
nlm-cli scale-out --nodes 10
nlm-cli scale-out --nodes 5 --mode distributed
```

#### load-balance
**Description:** Load balancing
**Usage:** `nlm-cli load-balance [options]`
**Example:**
```bash
nlm-cli load-balance
nlm-cli load-balance --algorithm round-robin
```

## Command Line Syntax

### Basic Usage
```bash
nlm-cli <command> [options]
nlm-cli --help          # Show help information
nlm-cli --version       # Show version information
```

### Command Options
- `--help, -h`           Show help information
- `--version`           Show version information
- `--verbose, -v`       Enable verbose output
- `--quiet, -q`         Suppress output
- `--config, -c`        Load configuration file
- `--json-config, -j`   Load JSON configuration file
- `--preset`            Apply configuration preset
- `--show-config`       Show current configuration
- `--validate-config`    Validate configuration

### Configuration File Formats

#### Traditional Format (key=value)
```
random_seed=42
simulation_timestep=0.001
neuron_count=500
region_count=1
```

#### JSON Format
```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 500,
  "region_count": 1
}
```

## Configuration Presets

- **research**: Detailed logging, validation enabled
- **performance**: Optimized for speed, reduced logging
- **education**: Simplified configuration, verbose output
- **debug**: Maximum verbosity, all debugging enabled
- **production**: Minimal output, validation enabled

## Installation

### Build from Source
```bash
cd /workspace/app
mkdir build
cd build
cmake ..
make
```

### Running CLI Tools
```bash
./nlm-cli --help          # Show available commands
./nlm-cli config-info     # Show configuration information
./nlm-cli exp-info test   # Show experiment information
```

## Configuration

CLI tools are configured using standard NLM configuration files. Configuration can be specified:
- Via command-line arguments (--config, --json-config)
- Via configuration presets (--preset)
- Via configuration overrides (--config-value)

## Performance Considerations

- Use `--preset performance` for faster execution
- Use `--quiet` for non-interactive environments
- Use `--dry-run` for validation without execution
- Use `--parallel-exec` for parallel processing

## Troubleshooting

### Common Issues

1. **Command not found**: Check command name and available commands with `--help`
2. **File not found**: Ensure the file exists and the path is correct
3. **Configuration error**: Use `--validate-config` to check configuration
4. **Permission denied**: Check file permissions

### Getting Help

- `nlm-cli --help`: Show general help
- `nlm-cli <command> --help`: Show help for specific command
- `nlm-cli <command> --version`: Show version information

## Changelog

### Version 0.1.0
- Initial release of CLI tools framework
- Core configuration and experiment tools
- Basic help and documentation system

### Future Releases
- Additional experiment management tools
- Advanced brain and world tools
- Comprehensive visualization tools
- Distributed computing support