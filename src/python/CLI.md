# NLM - Neural Learning Machine Unified CLI Documentation

## Overview

The NLM (Neural Learning Machine) unified CLI provides a comprehensive command-line interface for interacting with the artificial neural substrate. It consolidates all Phase 2-6 functionality into a single, powerful interface with subcommand architecture.

## Installation and Setup

### System Requirements

- C++20 compatible compiler
- Python 3.8+ (for enhanced bindings)
- CMake 3.16+

### Building with CLI Support

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. Configure with CLI support:
   ```bash
   cmake -B build -D NLM_BUILD_PYTHON_BINDINGS=ON -D NLM_BUILD_ENHANCED_PYTHON_BINDINGS=ON
   ```

3. Build the project:
   ```bash
   cmake --build build
   ```

4. Install (optional):
   ```bash
   cmake --install build
   ```

### Using the Enhanced Python Bindings

The enhanced Python bindings require `pybind11` and Python development packages:

```bash
# Ubuntu/Debian
apt-get update
apt-get install python3-dev python3-pip libpybind11-dev

# macOS
brew install python3 pybind11

# Install Python dependencies
pip install pybind11 numpy
```

Enable enhanced bindings at runtime:
```bash
export NLM_ENHANCED=1
python -m nlm
```

## Command Structure

The unified CLI follows the pattern:
```
nlm [global_options] <command> [command_options] [command_args]
```

### Global Options

- `--help, -h`: Show help message for the specified command
- `--version, -v`: Show version information
- `--config FILE, -c FILE`: Load configuration from file
- `--log-level LEVEL, -l LEVEL`: Set logging level (debug, info, warning, error)
- `--verbose, -vv`: Enable verbose output
- `--enhanced, -e`: Use enhanced Python bindings
- `--basic, -b`: Use basic Python bindings (default)

### Command Categories

#### Phase Commands
- `nlm phase2 [options]`: Run Phase 2: Real neural computation
- `nlm phase3 [options]`: Run Phase 3: World interaction
- `nlm phase4 [options]`: Run Phase 4: Cognitive capabilities
- `nlm phase5 [options]`: Run Phase 5: Performance optimizations
- `nlm phase6 [options]`: Run Phase 6: Integration tests

#### Demo Commands
- `nlm demo-phase3 [options]`: Run Phase 3 demonstration
- `nlm demo-phase4 [options]`: Run Phase 4 demonstration
- `nlm demo-phase5 [options]`: Run Phase 5 demonstration
- `nlm demo-phase6 [options]`: Run Phase 6 demonstration
- `nlm demo-phase6-text [options]`: Run Phase 6 text/thought demonstration

#### Core Commands
- `nlm simulate [options]`: Run neural simulation
- `nlm experiment [type] [options]`: Run experimental protocol
- `nlm benchmark [options]`: Run performance benchmarks
- `nlm config [action] [options]`: Configuration management
- `nlm help [command]`: Show help information
- `nlm status`: Show system status

## Command Reference

### Basic Commands

#### nlm simulate
Run a neural simulation with configurable parameters.

**Options:**
```
--config FILE, -c FILE           Configuration file path
--episodes N, -e N              Number of simulation episodes
--steps N, -s N                 Steps per episode
--enhanced, -e                  Use enhanced bindings
--basic, -b                    Use basic bindings
--verbose, -vv                  Enable verbose output
```

**Examples:**
```bash
# Basic simulation with default config
nlm simulate

# Enhanced simulation with custom config
nlm simulate --config myconfig.json --episodes 10 --steps 500 --enhanced

# Short simulation for testing
nlm simulate --episodes 1 --steps 100
```

#### nlm benchmark
Run performance benchmarks to evaluate system capabilities.

**Options:**
```
--type TYPE, -t TYPE            Benchmark type: "quick", "comprehensive"
--iterations N, -i N            Number of iterations (default: 1)
```

**Examples:**
```bash
# Quick benchmark (default)
nlm benchmark

# Comprehensive benchmark with multiple iterations
nlm benchmark --type comprehensive --iterations 3

# Custom benchmark type
nlm benchmark --type quick
```

#### nlm config
Manage NLM configuration files and settings.

**Actions:**
```
load     Load configuration from file
save     Save configuration to file
validate Validate configuration file
show     Display current configuration
```

**Options:**
```
--file FILE, -f FILE           Configuration file path (default: configs/default.cfg)
--key KEY, -k KEY              Configuration key to set
--value VALUE, -v VALUE        Configuration value to set
```

**Examples:**
```bash
# Load configuration
nlm config load --file configs/default.json

# Save current configuration
nlm config save --file configs/myconfig.json

# Validate configuration
nlm config validate --file configs/myconfig.json

# Show configuration
nlm config show

# Set a specific configuration value
nlm config load --file configs/myconfig.json
nlm config --key neuron_count --value 1000
```

#### nlm experiment
Run specific experimental protocols.

**Available Experiments:**
- `temporal-prediction`: Learn temporal relationships between events
- `working-memory`: Maintain information across delays
- `episodic-recall`: Store and recall experiences
- `concept-formation`: Discover patterns without labels
- `attention`: Selective processing through competition
- `planning`: Use predictions to select actions
- `self-model`: Learn body schema and action consequences
- `social-learning`: Observe and imitate others
- `continual-learning`: Adapt to new tasks

**Examples:**
```bash
# Run temporal prediction experiment
nlm experiment temporal-prediction --trials 50

# Run working memory experiment with custom trials
nlm experiment working-memory --trials 30
```

#### nlm demo
Run pre-defined demonstrations.

**Available Demos:**
- `phase2`: Phase 2 neural computation demonstration
- `phase3`: Phase 3 world interaction demo
- `phase4`: Phase 4 cognitive capabilities demo
- `phase5`: Phase 5 performance optimizations demo
- `phase6`: Phase 6 integration test demo
- `text`: Phase 6 text/thought integration demo
- `benchmark`: Performance benchmark demo

**Examples:**
```bash
# Run Phase 3 demo with default settings
nlm demo-phase3

# Run Phase 4 demo with custom trials
nlm demo-phase4 --trials 25

# Run Phase 6 text demo
nlm demo-phase6-text
```

### Phase-Specific Commands

#### nlm phase2
Run Phase 2: Real neural computation (LIF neurons, event-driven spikes, STDP, Hebbian plasticity).

**Examples:**
```bash
nlm phase2
nlm phase2 --episodes 1 --steps 50
```

#### nlm phase3
Run Phase 3: World interaction demo (experience → neural activity → action → consequence → learning).

**Examples:**
```bash
nlm phase3 --episodes 5 --steps 200
```

#### nlm phase4
Run Phase 4: Cognitive capabilities demo (temporal prediction, working memory, episodic recall, concept formation, attention, planning, self-model, social learning, continual learning).

**Examples:**
```bash
nlm phase4 --trials 50
```

#### nlm phase5
Run Phase 5: Performance optimizations demo (pattern probe, sandbox run, checkpoint round-trip).

**Examples:**
```bash
nlm demo-phase5
```

#### nlm phase6
Run Phase 6: Integration test demo (full integration of all systems).

**Examples:**
```bash
nlm demo-phase6
```

## Configuration Files

### Default Configuration Files

The NLM CLI supports multiple configuration file formats:

#### JSON Configuration (configs/default.json)
```json
{
  "neuron_count": 500,
  "region_count": 2,
  "connection_probability": 0.1,
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "environment_width": 20,
  "environment_height": 20,
  "vision_width": 16,
  "vision_height": 16
}
```

#### YAML Configuration (configs/default.yaml)
```yaml
# NLM Configuration Example
# YAML configuration file for Neural Learning Machine

neuron_count: 500
region_count: 2
connection_probability: 0.1
random_seed: 42
simulation_timestep: 0.001

stdp:
  ltp_weight: 0.01
  ltd_weight: 0.012
  tau: 20.0

structural_plasticity:
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001

environment:
  width: 20
  height: 20
  vision_width: 16
  vision_height: 16
```

#### Legacy Configuration (configs/legacy.cfg)
```
# NLM Configuration File (Legacy Format)
# This file uses the original key=value format

# Configuration for Phase 2 Neural Computation
neuron_count = 500
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001

# STDP parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Environment configuration
environment_width = 20
environment_height = 20
vision_width = 16
vision_height = 16
```

### Configuration File Format

The CLI supports the following configuration file formats:

1. **JSON**: Modern, structured data format with extensive type support
2. **YAML**: Human-readable data format commonly used for configuration
3. **Legacy**: Simple key=value format for backward compatibility

### Configuration Loading Priority

1. Command-line arguments (highest priority)
2. Configuration file specified with `--config` option
3. Default configuration file (configs/default.cfg)
4. Default values (lowest priority)

### Configuration Key Documentation

| Key | Type | Description | Default |
|-----|------|-------------|---------|
| `neuron_count` | Integer | Number of neurons in the brain | 500 |
| `region_count` | Integer | Number of neural regions | 2 |
| `connection_probability` | Float | Probability of synaptic connections | 0.1 |
| `random_seed` | Integer | Random seed for reproducibility | 42 |
| `simulation_timestep` | Float | Simulation time step in seconds | 0.001 |
| `stdp_ltp_weight` | Float | STDP long-term potentiation weight | 0.01 |
| `stdp_ltd_weight` | Float | STDP long-term depression weight | 0.012 |
| `stdp_tau` | Float | STDP time constant | 20.0 |
| `synaptogenesis_rate` | Float | Rate of new synapse formation | 0.0001 |
| `pruning_rate` | Float | Rate of synapse elimination | 0.00001 |
| `environment_width` | Integer | Width of the simulated world | 20 |
| `environment_height` | Integer | Height of the simulated world | 20 |
| `vision_width` | Integer | Width of agent's vision field | 16 |
| `vision_height` | Integer | Height of agent's vision field | 16 |

## Error Handling and Troubleshooting

### Common Errors

1. **Configuration File Not Found**
   - Solution: Ensure the configuration file exists or use `--config` with a valid path
   - Example: `nlm simulate --config configs/myconfig.json`

2. **Invalid Command Arguments**
   - Solution: Check the command syntax and use `--help` for usage information
   - Example: `nlm phase3 --help`

3. **Python Bindings Not Available**
   - Solution: Install Python development packages or use the basic C++ interface
   - On Ubuntu: `sudo apt-get install python3-dev python3-pip libpybind11-dev`
   - On macOS: `brew install python3 pybind11`

4. **Memory or Resource Constraints**
   - Solution: Reduce simulation size or available system resources
   - Example: `nlm simulate --episodes 1 --steps 100`

### Debugging Tips

1. **Verbose Mode**
   ```bash
   nlm simulate --verbose
   ```

2. **Log Level**
   ```bash
   nlm simulate --log-level debug
   ```

3. **Check System Status**
   ```bash
   nlm status
   ```

4. **Help for Specific Commands**
   ```bash
   nlm <command> --help
   ```

### Error Codes

| Code | Description |
|------|-------------|
| 0 | Success |
| 1 | General error |
| 130 | Interrupted by user (Ctrl+C) |
| 2 | Command-line argument parsing error |

## Advanced Usage

### Scripting and Automation

For scripting and automated testing, the NLM CLI supports non-interactive modes:

```bash
#!/bin/bash
# Run a quick test
nlm simulate --episodes 1 --steps 50 --basic > output.txt 2>&1

# Run benchmark for performance testing
nlm benchmark --iterations 5 > benchmark_results.txt 2>&1

# Validate configuration
if nlm config validate --file configs/test.json; then
    echo "Configuration is valid"
else
    echo "Configuration validation failed"
    exit 1
fi
```

### Integration with Python Scripts

The enhanced Python bindings allow for programmatic control:

```python
#!/usr/bin/env python3
import sys
sys.path.insert(0, '.')

from pynlm_enhanced import EnhancedBrain, ConfigManager, BrainContext

# Create configuration
config = ConfigManager()
config.set("neuron_count", 100)
config.set("region_count", 1)

# Create brain with context manager
with BrainContext(EnhancedBrain(config.getConfig())) as brain:
    # Run simulation
    for step in range(100):
        brain.step_with_context(step, step * 0.001)
    
    # Get results
    print(f"Total spikes: {brain.getBrain().getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getBrain().getAverageFiringRate()}")
```

### Performance Optimization

For optimal performance, consider these strategies:

1. **Use Basic Bindings for Production**
   ```bash
   nlm simulate --basic
   ```

2. **Run Benchmarks**
   ```bash
   nlm benchmark --type comprehensive
   ```

3. **Optimize Configuration**
   - Use JSON/YAML for complex configurations
   - Keep `neuron_count` and `region_count` appropriate for your hardware

4. **Parallel Processing**
   ```bash
   # Run multiple simulations in parallel
   nlm simulate --episodes 1 --steps 100 &
   nlm simulate --episodes 1 --steps 100 &
   wait
   ```

## Migration Guide

### From Legacy CLI to Unified CLI

If you're migrating from the legacy separate executables (`./nlm`, `./nlm_phase3_demo`, etc.) to the unified CLI, follow these changes:

#### Old Commands → New Commands

| Old Command | New Command |
|-------------|-------------|
| `./nlm` | `nlm phase2` |
| `./nlm_phase3_demo` | `nlm demo-phase3` |
| `./nlm_phase4_demo` | `nlm demo-phase4` |
| `./nlm_phase5_demo` | `nlm demo-phase5` |
| `./nlm_phase6_demo` | `nlm demo-phase6` |
| `./nlm_phase6_text_demo` | `nlm demo-phase6-text` |
| `./nlm_benchmark` | `nlm benchmark` |

#### Configuration Loading

| Old Method | New Method |
|------------|------------|
| `./nlm --config myconfig.cfg` | `nlm simulate --config myconfig.cfg` |
| `./nlm_phase3_demo 5 200` | `nlm demo-phase3 --episodes 5 --steps 200` |

#### Argument Style

| Old Style | New Style |
|-----------|-----------|
| `./nlm --neuron_count 1000` | `nlm simulate --config myconfig.json` |
| `./nlm --stdp_ltp_weight 0.02` | Set in configuration file |

## Additional Resources

### Online Documentation

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/yourorg/nlm)
- [Python Bindings API Reference](https://nlm.readthedocs.io/en/latest/python-api)

### Examples and Tutorials

1. **Basic Usage Tutorial**: `nlm --help`
2. **Phase 3 Example**: `nlm demo-phase3 --help`
3. **Configuration Examples**: Check the `configs/` directory
4. **Python Integration Examples**: Check the `examples/` directory

### Support and Community

- **GitHub Issues**: [Report bugs and feature requests](https://github.com/yourorg/nlm/issues)
- **Discussions**: [Community discussions](https://github.com/yourorg/nlm/discussions)
- **Email**: support@nlm.org

## Acknowledgments

This unified CLI framework was developed as part of the NLM Phase 5 enhancements, focusing on:

1. **User Experience**: Simplified command-line interface with subcommand architecture
2. **Extensibility**: Plugin system for adding new commands and functionality
3. **Documentation**: Comprehensive help system and reference materials
4. **Performance**: Efficient implementation with minimal overhead
5. **Compatibility**: Backward compatibility with existing configuration files

The development team would like to acknowledge all contributors who have helped improve and extend the NLM command-line interface.

---

*Last Updated: November 16, 2026*
*NLM Version 0.1.0*

For more information, visit the [NLM Documentation](https://nlm.readthedocs.io) or [GitHub Repository](https://github.com/yourorg/nlm).