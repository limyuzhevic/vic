# NLM CLI Quick Start

## Overview

The NLM CLI provides a comprehensive command-line interface for NLM (Neural Learning Machine) brain simulations and experiments.

## Installation

### From pip

```bash
pip install nlm
```

### From source

```bash
cd nlm-cli
pip install -e .
```

## Quick Examples

### 1. Run a simple simulation

```bash
nlm simulate --steps 500 --neuron-count 1000 --output results.json
```

### 2. Create a brain configuration

```bash
nlm create --neurons 2000 --output mybrain.cfg
```

### 3. Explore brain statistics

```bash
nlm explore --stats --neurons --synapses
```

### 4. Run Phase 6 integration test

```bash
nlm experiment --phase6 --output phase6_results.json
```

### 5. Manage configuration

```bash
nlm config set neuron_count 2000
nlm config get environment_name
nlm config list
```

## Common Commands

| Command | Description |
|---------|-------------|
| `nlm --help` | Show help information |
| `nlm --version` | Show version information |
| `nlm config list` | List all configuration keys |
| `nlm simulate` | Run brain simulation |
| `nlm create` | Create brain configuration |
| `nlm explore` | Explore brain state |
| `nlm experiment` | Run experiments |
| `nlm batch` | Process multiple configurations |
| `nlm benchmark` | Run performance benchmarks |

## Configuration Files

The NLM CLI supports JSON, YAML, and simple .cfg format configuration files.

## Output Formats

- **text**: Human-readable output (default)
- **json**: Structured output for programming
- **yaml**: Structured output for data processing

## Getting Help

- `nlm --help`: Show help for all commands
- `nlm [command] --help`: Show help for specific command
- Visit the documentation website for detailed guides

## Next Steps

1. Install the NLM CLI package
2. Create your first configuration file
3. Run a simple simulation
4. Explore the configuration management commands
5. Try batch processing for multiple simulations
