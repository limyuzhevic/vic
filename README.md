# NLM CLI - Neural Learning Machine Command Line Interface

## Overview

The NLM CLI provides a comprehensive command-line interface for the Neural Learning Machine project. It offers production-ready tools for brain management, simulation control, analysis, configuration management, checkpoint handling, and system monitoring.

## Installation

### Prerequisites

- C++ compiler supporting C++20
- CMake 3.16 or higher
- nlohmann/json library (for JSON output formatting)

### Build Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. Create a build directory and configure:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:
   ```bash
   make -j$(nproc)
   ```

4. Install (optional):
   ```bash
   sudo make install
   ```

## Usage

The main executable is `nlm-cli` located in the `build/bin/` directory.

### Basic Usage

```bash
# Show help
nlm-cli --help

# Show version
nlm-cli --version

# Interactive mode (default)
nlm-cli

# Script mode (non-interactive)
nlm-cli --script brain create mybrain

# With custom output format
nlm-cli --format json brain list

# With custom configuration
nlm-cli --config myconfig.cfg brain load mybrain
```

### Command Structure

The NLM CLI is organized into the following main command groups:

#### Brain Commands
- `brain create <name>` - Create a new brain with configuration
- `brain load <name>` - Load an existing brain from checkpoint
- `brain save <name>` - Save brain state to file
- `brain info <name>` - Display brain information
- `brain list` - List saved brains
- `brain delete <name>` - Delete saved brain

#### Simulation Commands
- `simulation run <steps> [duration]` - Run a simulation episode
- `simulation run-continuous [steps]` - Run continuous simulation
- `simulation step [steps]` - Run a single simulation step
- `simulation pause` - Pause simulation
- `simulation resume` - Resume simulation
- `simulation stop` - Stop simulation

#### Analysis Commands
- `analysis stats` - Display simulation statistics
- `analysis memory` - Analyze memory systems
- `analysis neuromodulation` - Analyze neuromodulation
- `analysis prediction` - Analyze prediction systems
- `analysis cognition` - Analyze cognition systems
- `analysis development` - Analyze development

#### Configuration Commands
- `config show [key]` - Show current configuration
- `config set <key> <value>` - Set configuration value
- `config load <file>` - Load configuration from file
- `config save <file>` - Save configuration to file
- `config validate` - Validate configuration
- `config template <action> [name]` - Manage configuration templates

#### Checkpoint Commands
- `checkpoint create <name>` - Create checkpoint
- `checkpoint list` - List checkpoints
- `checkpoint load <name>` - Load checkpoint
- `checkpoint delete <name>` - Delete checkpoint
- `checkpoint restore <name>` - Restore checkpoint
- `checkpoint compare <name1> <name2>` - Compare checkpoints

#### System Commands
- `system info` - Display system information
- `system logs [lines] [level]` - Display logs
- `system monitor [duration] [interval]` - Start system monitoring
- `system stats` - Display system statistics
- `system performance` - Display performance metrics

#### Help Commands
- `help` - Display general help
- `help examples` - Display examples
- `help tutorial` - Display tutorial
- `help reference` - Display reference

## Scripting Support

The NLM CLI supports script mode for batch processing:

```bash
# Script mode with non-interactive execution
nlm-cli --script brain create mybrain --config configs/default.cfg

# Using the script wrapper
scripts/nlm-cli.sh brain create mybrain --config configs/default.cfg
```

## Configuration Files

Configuration files should be in JSON format and placed in the `configs/` directory:

### Default Configuration Example
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
```

## Brain Files

Brain state files are stored in the `brains/` directory with the `.nlm` extension.

## Checkpoint Files

Checkpoint files are stored in the `checkpoints/` directory with the `.ckpt` extension.

## Output Formats

The NLM CLI supports different output formats:

### Pretty Format (Default)
Human-readable format with clear formatting and color coding:
```
=== Brain Information ===
Brain name: mybrain
Neurons: 1000
Synapses: 250000
Total spikes: 12345
```

### JSON Format
Machine-readable format for scripting and integration:
```json
{
    "name": "mybrain",
    "neurons": 1000,
    "synapses": 250000,
    "spikes": 12345
}
```

### Minimal Format
Compact output for scripting:
```
Brain name: mybrain
Neurons: 1000
Synapses: 250000
Total spikes: 12345
```

## Error Handling

The NLM CLI provides detailed error messages and exit codes:

- Exit code 0: Success
- Exit code 1: Error
- Exit code 2: Usage error (invalid command)
- Exit code 3: Configuration error
- Exit code 4: File not found
- Exit code 5: Permission denied
- Exit code 6: Invalid brain state
- Exit code 7: Simulation error
- Exit code 8: Analysis error
- Exit code 9: Checkpoint error
- Exit code 10: System error

## Logging

The NLM CLI supports logging with different levels:

### Log Levels
- `DEBUG`: Detailed debug information
- `INFO`: General information (default)
- `WARNING`: Warning messages
- `ERROR`: Error messages
- `CRITICAL`: Critical errors

### Log Configuration
```bash
# Set log level
nlm-cli --config config.cfg system logs 1000 DEBUG

# Enable logging to file
# (Configure via config file)
```

## Examples

### Creating and Running a Brain

```bash
# Create a new brain
nlm-cli brain create mybrain --config configs/default.cfg

# Load the brain
nlm-cli brain load mybrain

# Run a simulation
nlm-cli simulation run 1000 10.0

# Analyze the results
nlm-cli analysis stats
nlm-cli analysis memory
nlm-cli analysis cognition

# Save the brain
nlm-cli brain save mybrain
```

### Batch Processing Script

```bash
#!/bin/bash

# NLM batch processing script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOG_FILE="${SCRIPT_DIR}/batch.log"

# Function to log messages
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "${LOG_FILE}"
}

# Main execution
log "Starting NLM batch processing"

# Create brain
if nlm-cli --script brain create batch_brain --config configs/default.cfg; then
    log "Brain created successfully"
else
    log "Failed to create brain"
    exit 1
fi

# Load brain
if nlm-cli --script brain load batch_brain; then
    log "Brain loaded successfully"
else
    log "Failed to load brain"
    exit 1
fi

# Run simulation
if nlm-cli --script simulation run 500 5.0; then
    log "Simulation completed successfully"
else
    log "Simulation failed"
    exit 1
fi

# Save results
if nlm-cli --script brain save batch_brain; then
    log "Results saved successfully"
else
    log "Failed to save results"
    exit 1
fi

log "Batch processing completed successfully"
```

## Advanced Features

### Configuration Templates

The NLM CLI supports configuration templates for common use cases:

```bash
# List available templates
nlm-cli config template list

# Load a template
nlm-cli config template load research

# Create a new template
nlm-cli config template create production configs/production.cfg
```

### Checkpoint Comparison

Compare two checkpoints to see differences:

```bash
nlm-cli checkpoint compare checkpoint1 checkpoint2
```

### System Monitoring

Monitor system performance during simulation:

```bash
# Monitor for 60 seconds with 1-second intervals
nlm-cli system monitor 60 1.0
```

## Troubleshooting

### Common Issues

**Error: "Brain not found"**
- Ensure the brain exists or create it first: `nlm-cli brain create`

**Error: "Invalid configuration file"**
- Check that the configuration file is in valid JSON format
- Verify file permissions

**Error: "Simulation failed"**
- Check system resources
- Verify configuration settings
- Try with smaller simulation steps

**Error: "Permission denied"**
- Check file permissions in the `brains/` and `checkpoints/` directories

### Debug Mode

To enable debug output:

```bash
nlm-cli --config config.cfg --format pretty
```

### Interactive Testing

For interactive testing:

```bash
nlm-cli
```

Then use commands like:
```
brain list
simulation run 100
system info
help examples
```

## Building from Source

### Dependencies

The NLM CLI requires:

- CMake 3.16 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19.2+)
- nlohmann/json library

### Manual Build

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)

# Run tests (if any)
ctest
```

### Development Build

For development with debug symbols:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

## File Structure

```
/workspace/app/
├── CMakeLists.txt                    # CMake configuration
├── pyproject.toml                     # Python bindings configuration
├── configs/                           # Configuration files
│   ├── default.cfg                   # Default configuration
│   └── ...                           # Additional configs
├── brains/                           # Saved brain files
├── checkpoints/                      # Checkpoint files
├── src/                              # Source code
│   ├── nlm-cli.cpp                   # Main CLI entry point
│   ├── cli/                          # CLI module
│   │   ├── NLMCLI.hpp                # CLI interface
│   │   └── NLMCLI.cpp                # CLI implementation
│   ├── core/                         # Core functionality
│   ├── brain/                        # Brain implementation
│   ├── agent/                        # Agent implementation
│   └── ...                           # Other modules
├── scripts/                          # Script utilities
│   └── nlm-cli.sh                    # CLI script wrapper
├── tests/                            # Test files
└── README.md                         # This file
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Copyright

© 2024 NLM Project. All rights reserved.