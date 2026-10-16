# NLM Command Line Interface (CLI) Help

# NLM Command Line Interface (CLI) Help

## Overview

The NLM (Neural Learning Machine) project provides a comprehensive command-line interface for neural network simulation and experimentation. The main executable `nlm` supports multiple operational modes, configuration options, and runtime controls.

## Available Modes

### Basic Modes
- **`test`** - Comprehensive test suite (runs all neural computation tests)
- **`demo`** - Interactive demonstration with visualization
- **`benchmark`** - Performance benchmarking mode
- **`continuous`** - Continuous real-time simulation
- **`runtime_config_test`** - Tests runtime configuration modifications
- **`runtime_config_demo`** - Demonstrates runtime configuration changes
- **`runtime_config_cli`** - Interactive command-line interface for runtime configuration

## Usage Examples

### Basic Test Mode
```bash
./nlm --mode test --neurons 500 --region 1 --steps 1000
```

### Demo Mode with Visualization
```bash
./nlm --mode demo --neurons 2000 --region 2 --steps 5000 --pause
```

### Runtime Configuration Test
```bash
./nlm --mode runtime_config_test --neurons 100
```

### Runtime Configuration CLI
```bash
./nlm --mode runtime_config_cli
```

### Benchmark with Scaling
```bash
./nlm --mode benchmark --benchmark-scale --scaling-levels 100,500,1000,2000,5000
```

### Continuous Mode with Time Limit
```bash
./nlm --mode continuous --neurons 1000 --steps 10000 --max-time 300
```

## Command Line Options

### Core Options
- `-h`, `--help` - Display help information and exit
- `-m`, `--mode` - Operational mode (default: "test")
- `-c`, `--config` - Configuration file path (default: "configs/default.cfg")
- `-s`, `--steps` - Number of simulation steps (default: 1000)
- `-n`, `--neurons` - Number of neurons (default: 500)
- `-r`, `--region` - Number of brain regions (default: 1)
- `-t`, `--timestep` - Simulation timestep (default: 0.001)
- `-S`, `--seed` - Random seed for reproducible experiments (default: 42)
- `-v`, `--verbose` - Verbosity level (0-3, default: 1)

### Runtime Control Options
- `-p`, `--pause` - Enable pause in demo/continuous modes
- `--max-time`, `--max` - Maximum simulation time in seconds (default: 0, unlimited)
- `--save-results`, `-save` - Save results to file at completion
- `--save-interval`, `--si` - Interval for automatic checkpoint saves (default: 0, disabled)

### Benchmark Options
- `--benchmark-scale` - Enable scaling benchmark mode
- `--scaling-levels` - Comma-separated list of scale factors (e.g., "100,500,1000")

### Configuration File
- `--config=FILE` - Path to configuration file (or use `-c FILE`)

## Mode-Specific Options

### Demo Mode
- `--pause` - Start with simulation paused
- `--visualization` - Enable visualization (if available)

### Continuous Mode
- `--pause` - Start with simulation paused
- `--max-time` - Maximum simulation duration in seconds

### Runtime Config Modes
- `--pause` - Pause demo modes
- `--save-results` - Save runtime configuration changes

## Configuration File Format

Configuration files use a simple `key = value` format with optional comments.

```ini
# Example configuration section
brain.neuron_count = 1000
brain.region_count = 2
plasticity.stdp.enable = true
neuromod.dopamine.scale = 0.5
log.level = INFO
```

## Error Handling

The CLI provides the following error codes:
- **0** - Successful execution
- **1** - Runtime error or execution failure
- **2** - Invalid command line arguments

## Help Display

To display basic help information:
```bash
./nlm
or
./nlm --help
```

For detailed mode-specific help, run the executable with the desired mode and `--help` flag (if supported by the mode).