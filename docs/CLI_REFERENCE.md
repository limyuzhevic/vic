// Command-line help documentation for NLM
// Comprehensive documentation for all command-line options and modes

# NLM Command-Line Interface Reference

## Overview

The NLM (熙然) - Neural Learning Machine provides a comprehensive command-line interface for running neural learning experiments and demonstrations. The interface supports multiple execution modes, configuration options, advanced features, and flexible test selection.

## Modes

### `--mode basic`
Run basic connectivity and learning tests.

**Description:**
Tests the fundamental neural connectivity and spike propagation capabilities of the brain. This mode verifies that:
- Neurons can receive input and generate spikes
- Spikes propagate through the network
- Basic synaptic dynamics work correctly

**Typical Use:**
Initial testing, debugging, and basic functionality verification.

**Default Configuration:**
- Neuron count: 1000
- Simulation timestep: 0.001 seconds
- Duration: Fast mode (100 steps) or full mode (1000 steps)

### `--mode full`
Run complete integration demo.

**Description:**
Runs a comprehensive integration test that exercises all major brain subsystems:
- Neural processing (LIF dynamics, spikes)
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction systems
- Motor control
- World interaction

**Typical Use:**
Full system validation, performance testing, and comprehensive verification.

**Default Configuration:**
- Enables all subsystems
- Simulates agent-world interaction
- Measures integration metrics

### `--mode phase3`
Run Phase 3 world interaction demo.

**Description:**
Demonstrates the world interaction capabilities developed in Phase 3:
- Sensory input processing from environment
- Motor output generation and application
- Reward prediction error learning
- Novelty and curiosity-driven exploration
- Developmental stage progression

**Typical Use:**
World interaction testing, reinforcement learning validation, exploration behavior analysis.

**Default Configuration:**
- Grid world environment (20x20)
- Agent with vision (8x8 sensory range)
- Reward and curiosity modulation enabled

### `--mode phase4`
Run Phase 4 cognition experiments.

**Description:**
Tests advanced cognitive capabilities from Phase 4:
- Neural prediction systems
- Working and episodic memory
- Concept formation from experience
- Neural attention mechanisms
- Predictive planning
- Self-model development
- Social learning
- Continual learning capabilities

**Typical Use:**
Cognitive testing, complex task validation, learning behavior analysis.

**Default Configuration:**
- All cognitive subsystems enabled
- Rich sensory-motor loop
- Prediction and attention mechanisms active

### `--mode phase6`
Run Phase 6 integration test.

**Description:**
Comprehensive integration test verifying all Phase 6 achievements:
- All memory systems connected to neural processing
- Neuromodulation affecting plasticity and dynamics
- Prediction integrated with learning
- Development affecting plasticity rates
- Checkpoint save/load functionality
- Replay and consolidation systems
- Complete brain loop coherence

**Typical Use:**
Final system validation, regression testing, comprehensive integration verification.

**Default Configuration:**
- Maximum subsystem integration
- Comprehensive metric collection
- Extended simulation duration

### `--mode help`
Show help and available modes.

**Description:**
Displays detailed help information including all available modes, configuration options, and usage examples.

**Typical Use:**
Getting started, learning about available options, debugging command-line issues.

## Configuration Options

### `--config <file>`
Load configuration from JSON/YAML file.

**Description:**
Loads configuration from the specified file. The configuration file can be in JSON or YAML format and will override default settings.

**File Format:**
- JSON (`.json`): Standard JSON format
- YAML (`.yaml`, `.yml`): YAML format with indentation

**Example:**
```bash
nlm --mode basic --config examples/config.json
nlm --mode phase6 --config examples/config.yaml
```

### `--set <key>=<value>`
Set individual configuration values.

**Description:**
Sets specific configuration values directly from the command line. Multiple `--set` options can be used.

**Syntax:**
```
--set key1=value1 --set key2=value2
```

**Examples:**
```bash
nlm --mode basic --set brain.neuron_count=2000 --set simulation.timestep=0.0005
nlm --mode phase6 --set plasticity.stdp.learning_rate=0.01
```

### `--neurons <count>`
Set neuron count.

**Description:**
Sets the number of neurons in the brain. Overrides configuration file settings.

**Example:**
```bash
nlm --mode basic --neurons 5000
```

### `--episodes <count>`
Set number of episodes.

**Description:**
Sets the number of learning episodes to run. Useful for controlled experiments.

**Example:**
```bash
nlm --mode full --episodes 50
```

### `--trials <count>`
Set number of trials.

**Description:**
Sets the number of experimental trials to run. Different from episodes in some contexts.

**Example:**
```bash
nlm --mode phase3 --trials 100
```

### `--timestep <value>`
Set simulation timestep.

**Description:**
Sets the simulation timestep in seconds. Controls simulation speed and accuracy.

**Example:**
```bash
nlm --mode basic --timestep 0.001
```

## Advanced Features

### `--verbose` / `-v`
Enable verbose logging.

**Description:**
Enables detailed logging output showing step-by-step progress, metrics, and internal state changes.

**Example:**
```bash
nlm --mode full --verbose
```

### `--log-level <level>`
Set log level.

**Description:**
Sets the minimum log level to display. Options are:
- DEBUG: Detailed debug information
- INFO: Standard information messages
- WARNING: Warning and error messages
- ERROR: Error messages only
- CRITICAL: Critical system errors only

**Example:**
```bash
nlm --mode phase6 --log-level DEBUG
```

### `--save-state <file>`
Save brain state to file.

**Description:**
Saves the current brain state (neurons, synapses, memory, etc.) to the specified file. Can be loaded later with `--load-state`.

**File Format:**
- Binary format for efficient storage
- Includes neural weights, memory contents, and development state

**Example:**
```bash
nlm --mode phase6 --save-state checkpoint1
nlm --mode phase6 --load-state checkpoint1
```

### `--load-state <file>`
Load brain state from file.

**Description:**
Loads a previously saved brain state from the specified file. Useful for resuming interrupted experiments.

**Example:**
```bash
nlm --mode phase6 --load-state checkpoint1 --mode basic
```

### `--benchmark`
Run performance benchmark.

**Description:**
Runs the selected mode while measuring and reporting performance metrics including execution time, memory usage, and throughput.

**Output:**
- Console: Human-readable benchmark results
- JSON: Structured benchmark data for automated analysis

**Example:**
```bash
nlm --mode phase6 --benchmark
nlm --mode full --benchmark --json
```

### `--version`
Show version information.

**Description:**
Displays version information including NLM version and phase.

**Example:**
```bash
nlm --version
```

## Flexible Test Selection

### `--tests <test1>,<test2>,...`
Specify which tests to run.

**Description:**
Runs only the specified tests, separated by commas. If no tests are specified or "all" is used, all available tests run.

**Available Tests:**
- `basic`: Basic connectivity test
- `plasticity`: Plasticity learning experiment
- `stdp`: STDP verification test
- `all`: All tests (default)

**Examples:**
```bash
nlm --mode basic --tests basic
nlm --mode full --tests basic,plasticity,stdp
nlm --mode phase6 --tests all
```

### `--fast`
Run reduced test suite for quick verification.

**Description:**
Runs tests with reduced duration and fewer steps for quick verification. Saves time while still validating basic functionality.

**Effects:**
- Reduces simulation steps by ~10x
- Uses smaller brain sizes
- Skips some extended validation

**Example:**
```bash
nlm --mode phase6 --fast
```

### `--thorough`
Run extended test suite for comprehensive validation.

**Description:**
Runs tests with extended duration and additional validation for comprehensive testing. Ensures thorough coverage of edge cases.

**Effects:**
- Increases simulation steps by ~10x
- Adds additional validation checks
- Tests more extreme conditions

**Example:**
```bash
nlm --mode phase6 --thorough
```

## Output and Reporting

### `--quiet` / `-q`
Minimal output.

**Description:**
Suppresses most informational output, showing only essential information and errors.

**Example:**
```bash
nlm --mode basic --quiet
```

### `--json`
Output results in JSON format.

**Description:**
Formats all output as JSON for easy parsing by automated tools and scripts.

**Output Structure:**
```json
{
  "mode": "basic",
  "steps": 1000,
  "successful_spikes": 450,
  "success_rate": 45.0,
  "duration_ms": 1234,
  "metrics": {...}
}
```

**Example:**
```bash
nlm --mode phase6 --json --report results.json
```

### `--report <file>`
Save detailed report to file.

**Description:**
Saves comprehensive execution report to the specified file, including all metrics, timing information, and detailed results.

**Report Format:**
- Human-readable summary
- Detailed metrics
- Configuration used
- Performance benchmarks

**Example:**
```bash
nlm --mode full --report experiment_report.txt
```

### `--metrics <file>`
Save metrics to file.

**Description:**
Saves only the numerical metrics to the specified file, suitable for automated analysis and graphing.

**Metrics Format:**
- CSV format for easy import into analysis tools
- Includes all measured quantities
- Timestamps for temporal analysis

**Example:**
```bash
nlm --mode phase6 --metrics metrics.csv
```

## Plugin and Extension Support

### `--plugins <dir>`
Load plugins from directory.

**Description:**
Loads plugins from the specified directory. Plugins can extend NLM functionality with custom behaviors, sensors, actuators, and analysis tools.

**Plugin Architecture:**
- Shared libraries (.so, .dll)
- Standard interface for integration
- Dynamic loading at runtime

**Example:**
```bash
nlm --mode basic --plugins /path/to/custom/plugins
```

### `--experiment <name>`
Run specific experiment.

**Description:**
Runs a predefined named experiment. Experiments are collections of configuration settings and specific test scenarios.

**Available Experiments:**
- `learning_curve`: Track learning progress over time
- `plasticity_analysis`: Analyze plasticity effects
- `novelty_driven_exploration`: Study curiosity-driven behavior

**Example:**
```bash
nlm --mode phase3 --experiment learning_curve
```

### `--validate-config`
Validate configuration.

**Description:**
Validates the current configuration (from file or command line) against all constraints and requirements. Reports any issues found.

**Validation Checks:**
- Parameter ranges
- File existence
- Logical consistency
- System requirements

**Example:**
```bash
nlm --validate-config --config examples/config.json
```

## Integration with Existing System

### Backward Compatibility

This command-line interface maintains full backward compatibility with existing demo programs:

- `nlm_phase3_demo`: Can be invoked via `nlm --mode phase3`
- `nlm_phase4_demo`: Can be invoked via `nlm --mode phase4`
- Existing configuration files (`configs/default.cfg`): Supported via `--config`

**Migration Example:**
```bash
# Old way (still supported)
./nlm_phase3_demo

# New unified way
nlm --mode phase3
```

### Configuration System

Uses the new JSON/YAML configuration system with full support for:
- JSON configuration files (`.json`)
- YAML configuration files (`.yaml`, `.yml`)
- Command-line overrides (`--set`)
- Configuration inheritance
- Validation and error handling

### Modular Architecture

The CLI is designed with extensibility in mind:
- Plugin system for custom modes
- Configurable test runners
- Extensible output formats
- Modular option handling

### Advanced User Features

Power users can leverage:
- **Plugin directories**: Add custom functionality
- **Experiment presets**: Reproduce complex experimental setups
- **JSON output**: Integrate with automation pipelines
- **Benchmarking**: Performance analysis and optimization
- **State management**: Save/load experiment states
- **Configuration validation**: Ensure system integrity

## Quick Reference

### Common Commands

```bash
# Quick basic test
nlm --mode basic

# Quick full integration test
nlm --mode full --fast

# Quick Phase 3 demo
nlm --mode phase3 --fast

# Quick Phase 6 integration test
nlm --mode phase6 --fast

# Verbose full integration test
nlm --mode full --verbose

# Benchmark Phase 6 integration
nlm --mode phase6 --benchmark

# JSON output for automation
nlm --mode phase6 --benchmark --json --metrics metrics.csv
```

### Performance Tips

- Use `--fast` for rapid testing
- Use `--thorough` for comprehensive validation
- Use `--benchmark` for performance analysis
- Use `--json` for automated processing
- Use `--quiet` for production runs

### Troubleshooting

**Common Issues:**
- **Configuration file not found**: Use `--config` with full path or ensure file exists
- **Unknown mode**: Check available modes with `--mode help` or `--help`
- **Performance issues**: Use `--fast` mode or reduce `--neurons`
- **Memory errors**: Reduce `--neurons` or `--trials`

**Help Commands:**
```bash
nlm --help                    # General help
nlm --mode help               # Mode-specific help
nlm --validate-config         # Validate current config
```

## Version Information

NLM Phase 6 (熙然) - Neural Learning Machine
Version: 0.6.0 (Phase 6 Final Integration)
Build: $(date +%Y-%m-%d)

For more information, visit the NLM documentation or GitHub repository.