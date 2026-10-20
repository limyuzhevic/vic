pynlm
=====================

A Python-based command-line interface for NLM (Neural Learning Machine) with enhanced functionality beyond the basic interface.

## Overview

This CLI provides comprehensive access to NLM neural simulation capabilities, including:

- Brain simulation and management
- World creation and control
- Agent training and evaluation
- Configuration management
- Experiment running and analysis
- Performance profiling
- Interactive and batch processing modes

## Installation

Install the package and its dependencies:

```bash
pip install -e .
```

## Quick Start

```bash
# Start interactive CLI
nlm

# Run basic brain simulation
nlm brain create --neuron-count 1000 --timestep 0.001 nlm brain simulate

# Run with custom config
nlm brain create --config my_config.cfg --output-format json nlm brain simulate --output results.json
```

## Main Commands

- `brain` - Brain creation, simulation, inspection, and benchmarking
- `world` - World creation and simulation
- `agent` - Agent creation, training, and evaluation
- `config` - Configuration management
- `experiment` - Experiment management and analysis
- `profile` - Performance profiling and reporting

## Advanced Options

- `--output-format` (json, yaml, table, csv)
- `--output-file` for saving results
- `--log-level` (debug, info, warn, error, critical)
- `--config-file` for custom configuration
- `--dry-run` for preview operations
- `--force` for overwriting existing files

## Configuration

Configuration can be set via:
1. Config file (default: configs/default.cfg)
2. Command line arguments
3. Environment variables

```bash
nlm --config-file my_config.cfg brain create --neuron-count 2000
```

## Examples

See `docs/cli_examples.md` for detailed examples of common workflows.
