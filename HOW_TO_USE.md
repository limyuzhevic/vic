# How to Use NLM - Complete Guide

NLM (Neural Learning Machine) is a comprehensive experimental brain simulation framework. This guide covers both the original NLM executable and the new advanced Command Line Interface (CLI).

## Table of Contents

1. [Getting Started](#getting-started)
2. [Original NLM Executable](#original-nlm-executable)
3. [New CLI Interface](#new-cli-interface)
4. [Configuration Files](#configuration-files)
5. [Command Reference](#command-reference)
6. [Advanced Features](#advanced-features)
7. [Troubleshooting](#troubleshooting)

---

## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake 3.16 or higher
- Make or equivalent build system

### Build Instructions

```bash
# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The build will create several executables:

- `./nlm` - Original NLM main executable
- `./nlm_cli` - Advanced CLI interface (NEW)
- `./nlm_phase3_demo` - Phase 3 demonstration
- `./nlm_phase4_demo` - Phase 4 demonstration
- `./nlm_phase6_demo` - Phase 6 integration demo

## Original NLM Executable

### Overview

The original `nlm` executable provides the core NLM functionality with three integrated experiments:

1. **Test 1: Basic Neural Connectivity**
   - Injects current into neurons
   - Tests spike propagation through network
   - Verifies basic functionality

2. **Test 2: Plasticity Learning Experiment**
   - Demonstrates synaptic learning
   - Shows weight changes through experience
   - Tests Hebbian and STDP plasticity

3. **Test 3: STDP Verification**
   - Specifically tests spike-timing dependent plasticity
   - Verifies LTP (long-term potentiation)
   - Tests LTD (long-term depression) mechanisms

### Running the Original NLM

```bash
# Basic usage - runs all three tests
./nlm

# With custom configuration
./nlm --config my_config.cfg

# With custom parameters
./nlm --neurons 1000 --steps 5000

# Help
./nlm --help
```

### Command Line Options

```bash
--config FILE      Load configuration from file
--neurons N        Number of neurons (default: 500)
--steps N          Number of simulation steps (default: 1000)
--timestep F       Simulation timestep (default: 0.001)
--seed N           Random seed (default: 42)
--verbose, -v      Enable verbose output
```

## New CLI Interface

### Overview

The new `nlm_cli` executable provides enhanced functionality with specialized modes for different types of experiments:

1. **Auto Mode** - Run all experiments in sequence
2. **Basic Mode** - Simple connectivity and activity tests
3. **Plasticity Mode** - Learning experiments
4. **STDP Mode** - Spike-timing dependent plasticity verification
5. **Benchmark Mode** - Performance measurement
6. **Integration Mode** - Phase 6 integration test
7. **Script Mode** - Execute automation scripts

### Getting Started with CLI

```bash
# List all available commands
./nlm_cli --list-commands

# Get help for a specific command
./nlm_cli basic --help

# Run basic connectivity test
./nlm_cli basic

# Run learning experiment
./nlm_cli plasticity

# Run STDP verification
./nlm_cli stdp

# Run performance benchmark
./nlm_cli benchmark

# Run integration test
./nlm_cli integration

# Run all experiments in sequence
./nlm_cli auto
```

### CLI Command Reference

#### Auto Mode
```bash
nlm_cli auto [--config file] [--steps N] [--neurons N]
```
Runs all experiments in sequence: basic → plasticity → stdp → benchmark

#### Basic Mode
```bash
nlm_cli basic [--config file] [--steps N] [--neurons N]
```
Tests neural connectivity and spike propagation. Perfect for system verification.

#### Plasticity Mode
```bash
nlm_cli plasticity [--config file] [--steps N] [--neurons N]
```
Demonstrates synaptic learning through Hebbian and STDP mechanisms.

#### STDP Mode
```bash
nlm_cli stdp [--config file] [--steps N] [--neurons N]
```
Specifically tests spike-timing dependent plasticity (pre-before-post).

#### Benchmark Mode
```bash
nlm_cli benchmark [--steps N] [--neurons N] [--iterations M]
```
Measures simulation performance and efficiency. Reports steps/sec.

#### Integration Mode
```bash
nlm_cli integration [--config file] [--steps N] [--neurons N]
```
Runs Phase 6 integration test with all brain systems connected.

#### Script Mode
```bash
nlm_cli script <script_file> [args...]
```
Execute custom automation scripts (Phase 7 feature).

## Configuration Files

### Configuration File Format

NLM uses simple key=value configuration files:

```
# Configuration file example
# Comments start with #

neuron_count = 500
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Memory settings
working_memory_capacity = 100
episodic_memory_capacity = 50
```

### Configuration Files Location

- `configs/default.cfg` - Default configuration
- Custom configs can be created for specific experiment types

### Configuration Loading Priority

1. Command line arguments (highest priority)
2. Config file specified with `--config`
3. Default config file
4. Built-in default values

## Command Reference

### Common Options

All NLM commands support these common options:

```bash
--help, -h              Show help for specific command
--verbose, -v           Enable verbose output
--config FILE           Configuration file
--steps N               Number of simulation steps
--neurons N             Number of neurons
--timestep F            Simulation timestep
--seed N                Random seed
--checkpoint FILE       Enable checkpointing
--enable-replay         Enable replay system
--enable-development    Enable developmental processes
```

### Mode-Specific Options

Each mode has additional options specific to its experiments:

#### Basic Mode
Tests connectivity and basic activity patterns.

#### Plasticity Mode
Focuses on learning and synaptic changes.

#### STDP Mode
Specifically tests timing-dependent plasticity.

#### Benchmark Mode
- `--iterations M` Number of benchmark iterations (default: 3)

#### Integration Mode
- `--enable-checkpoint` Enable checkpoint/save-load
- `--enable-replay` Enable replay system
- `--enable-development` Enable developmental processes

## Advanced Features

### 1. Checkpointing

Enable checkpointing for long experiments:

```bash
./nlm_cli plasticity --enable-checkpoint --checkpoint /tmp/nlm_checkpoint.bin
```

### 2. Replay System

Test memory replay functionality:

```bash
./nlm_cli integration --enable-replay
```

### 3. Development Processes

Enable developmental plasticity:

```bash
./nlm_cli integration --enable-development
```

### 4. Performance Optimization

Benchmark to find optimal settings:

```bash
./nlm_cli benchmark --neurons 1000 --steps 5000 --iterations 5
```

### 5. Custom Experiments

Combine different modes and options for custom experiments:

```bash
# Small connectivity test
./nlm_cli basic --neurons 100 --steps 500

# Large learning experiment
./nlm_cli plasticity --neurons 2000 --steps 10000

# Fast performance test
./nlm_cli benchmark --neurons 500 --steps 1000
```

## Quick Start Examples

### Example 1: System Verification
```bash
# Quick test to verify everything works
./nlm_cli basic
```

### Example 2: Learning Demonstration
```bash
# Demonstrate neural learning
./nlm_cli plasticity --neurons 800 --steps 2000
```

### Example 3: Performance Measurement
```bash
# Measure how fast NLM runs
./nlm_cli benchmark --neurons 1000 --steps 5000
```

### Example 4: Integration Testing
```bash
# Test all brain systems together
./nlm_cli integration --enable-replay --enable-development
```

### Example 5: Complete Workflow
```bash
# Run all experiments in sequence
./nlm_cli auto --neurons 1000 --steps 5000
```

## Comparison: Original vs CLI

| Feature | Original NLM (`nlm`) | CLI (`nlm_cli`) |
|---------|---------------------|-----------------|
| Basic Usage | `nlm` (all 3 tests) | `nlm_cli basic` |
| Help System | Simple `--help` | Comprehensive help for each command |
| Specialized Modes | No dedicated modes | 7 specialized modes |
| Configuration | Limited options | Advanced configuration |
| Benchmarking | No benchmarking | Built-in benchmarking |
| Scripting | No scripting | Script execution |
| Integration | Manual testing | Automated integration tests |
| Learning Focus | Basic learning | Advanced learning experiments |

## Migration Guide

### For Users of Original NLM

If you're currently using the original NLM, you can:

1. **Try the CLI first** - Most users will prefer the new CLI
2. **Access original functionality** - `nlm_cli basic` replaces Test 1
3. **Enhanced learning** - `nlm_cli plasticity` provides more detailed learning experiments
4. **Better testing** - `nlm_cli stdp` gives focused STDP testing
5. **Performance monitoring** - `nlm_cli benchmark` provides performance metrics

### For Developers

The new CLI provides:

1. **Extensible architecture** - Easy to add new command modes
2. **Better separation of concerns** - Each mode has clear purpose
3. **Enhanced testing** - More comprehensive test coverage
4. **Improved documentation** - Comprehensive help system
5. **Future extensibility** - Script execution for Phase 7

## Troubleshooting

### Common Issues

#### "Command not found"
```bash
# Make sure you're in the build directory
cd build
./nlm_cli basic
```

#### "Configuration file not found"
```bash
# Check if file exists
ls configs/

# Create if it doesn't exist
cp configs/default.cfg my_config.cfg
```

#### "Too many neurons"
```bash
# Reduce neuron count for testing
./nlm_cli basic --neurons 100 --steps 100
```

#### "Memory issues"
```bash
# Run with less memory-intensive settings
./nlm_cli basic --neurons 200 --steps 500
```

#### "Help not showing"
```bash
# Rebuild if needed
make -j4
```

### Getting Help

```bash
# General help
./nlm_cli --list-commands
./nlm_cli --show-advanced-help

# Command-specific help
./nlm_cli basic --help
./nlm_cli plasticity --help

# Advanced help with all options
./nlm_cli --show-advanced-help
```

## Next Steps

1. **Try the basic mode** to get familiar with NLM
2. **Experiment with plasticity** to understand learning
3. **Run benchmarks** to optimize performance
4. **Test integration** to see all systems work together
5. **Create custom configs** for your specific needs

The new CLI interface makes it much easier to explore, test, and use NLM for both research and educational purposes. Enjoy experimenting with artificial brain dynamics! 🧠✨

---

*This guide covers both the original NLM functionality and the new advanced CLI interface. The CLI provides enhanced features while maintaining backward compatibility with the original experiment structure.*

For the latest updates and additional features, check the repository documentation and release notes.