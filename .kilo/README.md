# Kilo Configuration for NLM Neural Learning Machine

This directory contains Kilo-specific configuration files for the NLM (熙然) Neural Learning Machine project.

## Overview

The NLM project has been transformed into a production-ready neural simulation framework with comprehensive improvements across CLI interface, configuration management, performance monitoring, debugging tools, and logging systems.

## Configuration Structure

This `.kilo/` directory contains:

### Commands (`.kilo/command/*.md`)

Command-line tools for working with NLM neural simulations:

#### `/nlm-explore`
Analyze and explore the NLM codebase

#### `/nlm-experiments`
Run and manage neural experiments

#### `/nlm-config`
Manage NLM configuration files and settings

#### `/nlm-diagnostics`
Run diagnostics and performance analysis

#### `/nlm-benchmark`
Benchmark NLM performance and capabilities

#### `/nlm-help`
Get comprehensive help and documentation

### Agents (`.kilo/agent/*.md`)

AI assistants specialized in NLM work:

#### `/nlm-research`
Research NLM capabilities and improvements

#### `/nlm-improve`
Improve NLM code and features

#### `/nlm-debug`
Debug NLM simulations and experiments

#### `/nlm-optimize`
Optimize NLM performance and efficiency

### Skills (`.kilo/skill/*.md`)

Specialized skills for NLM development:

#### `/nlm-cli-analyzer`
Analyze NLM command-line interface

#### `/nlm-performance-monitor`
Monitor NLM performance metrics

#### `/nlm-config-manager`
Manage NLM configuration

#### `/nlm-debug-tools`
Access NLM debugging tools

#### `/nlm-experimental-tools`
Access experimental NLM features

### Workflows (`.kilo/workflows/*.md`)

Automated workflows for NLM tasks:

#### `/run-demo`
Run comprehensive NLM demonstration

#### `/run-integration-test`
Run Phase 6 integration tests

#### `/run-learning-experiment`
Run plasticity and learning experiments

#### `/analyze-performance`
Analyze NLM performance metrics

#### `/debug-issues`
Debug NLM simulation issues

#### `/optimize-configuration`
Optimize NLM configuration

## Key Features

### Enhanced CLI Interface

The NLM project now features a comprehensive command-line interface:

- **26+ command-line options**
- **Detailed help documentation with examples**
- **Version information display**
- **Experiment selection and filtering**
- **Custom parameter support (key=value)**
- **Multiple output formats**

**Available experiments:**
```bash
./nlm learning       # Plasticity and STDP experiments
./nlm plasticity     # Plasticity-focused experiments
./nlm stdp          # STDP verification
./nlm connectivity  # Neural connectivity tests
./nlm integration   # Full Phase 6 integration
```

**Key command-line options:**
- `--help, -h` - Show comprehensive help
- `--version, -v` - Show version information
- `--config=FILE` - Configuration file
- `--output-file=FILE` - Save results to file
- `--log-file=FILE` - Log output to file
- `--log-level=LEVEL` - Logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)
- `--seed=NUMBER` - Random seed
- `--timestep=SECONDS` - Simulation timestep
- `--max-steps=NUMBER` - Maximum simulation steps
- `--verbose` - Enable verbose output
- `--quiet` - Quiet mode (minimal output)
- `--debug` - Enable debug mode
- `--profile` - Enable performance profiling
```