# NLM Tutorial Guide
# Getting Started with Neural Learning Machine

## Welcome to NLM!

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that implements real spiking neural computation. This guide will help you get started with NLM, from basic setup to advanced usage.

## Table of Contents

1. [Introduction](#introduction)
2. [System Requirements](#system-requirements)
3. [Installation](#installation)
4. [Basic Usage](#basic-usage)
5. [Working with Brain Commands](#working-with-brain-commands)
6. [Configuration Management](#configuration-management)
7. [Experiment Control](#experiment-control)
8. [Advanced Features](#advanced-features)
9. [Troubleshooting](#troubleshooting)
10. [Further Learning](#further-learning)

## Introduction

NLM is a research project investigating computational brain-like systems. Unlike traditional AI systems, NLM:

- **Implements real neural dynamics** using LIF (Leaky Integrate-and-Fire) neurons
- **Uses event-driven computation** with spikes and synaptic delays
- **Features adaptive plasticity** through STDP and Hebbian learning
- **Includes comprehensive memory systems** (working, episodic, associative)
- **Integrates neuromodulation** (dopamine, curiosity, novelty detection)
- **Supports cognitive functions** (planning, attention, concept formation)

### What Makes NLM Special

- **Phase-based architecture**: 6 completed phases from basic computation to full integration
- **Research-oriented**: No claims of human intelligence or consciousness
- **Scientifically rigorous**: Based on computational neuroscience principles
- **Modular design**: 23 specialized brain systems
- **Production-ready**: Professional engineering standards and extensive testing

## System Requirements

### Minimum Requirements

- **Operating System**: Linux, macOS, or Windows (with WSL)
- **Compiler**: C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19+)
- **Memory**: 4GB RAM minimum, 8GB+ recommended
- **Disk Space**: 500MB for source code, 2GB+ for build artifacts

### Recommended Requirements

- **Processor**: Multi-core CPU (4+ cores)
- **Memory**: 16GB+ RAM for full functionality
- **Graphics**: GPU support for visualization (optional)
- **Storage**: SSD recommended for build performance

### Development Environment

- **CMake 3.16+**
- **Git** (for version control)
- **Build tools**: make, ninja, or MSBuild
- **Text editor**: VSCode, CLion, or similar IDE
- **Optional**: Python 3.8+ for Python bindings

## Installation

### Quick Installation

```bash
# Clone the repository
cd /path/to
git clone https://github.com/nlm-research/nlm.git

cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Advanced Installation

```bash
# Configure with specific options
cd nlm
mkdir build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DNLM_BUILD_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_VIZUALIZATION=ON \
    -DNLM_ENABLE_DOCS=ON

# Install dependencies (if available)
./setup.sh install-deps

# Build
./setup.sh build

# Install (optional)
./setup.sh install
```

### Installation Scripts

The project includes convenience scripts for common tasks:

```bash
# Build script
./setup.sh build

# Test script
./setup.sh test

# Install dependencies
./setup.sh install-deps

# Format code
./setup.sh format

# Static analysis
./setup.sh lint

# Generate documentation
./setup.sh docs
```

## Basic Usage

### Quick Start

Let's get started with a simple NLM session:

```bash
# Interactive mode
cd /workspace/app
./nlm
```

You should see the banner and be ready to enter commands:

```
>>> help
```

This will show you all available commands.

### First Commands

Let's try some basic commands:

```bash
>>> brain status
```

This shows the current state of your brain, including:
- Total neurons and synapses
- Active firing neurons
- Spike activity
- Memory system status

```bash
>>> config get neuron_count
```

This shows the current number of neurons (default: 500).

```bash
>>> run 100
```

This runs the simulation for 100 time steps, allowing neural activity to develop.

### Brain Operations

```bash
# Reset the brain to initial state
>>> brain reset

# Inject current into sensory neurons
>>> brain inject sensory 25.0

# Check brain status after injection
>>> brain status

# Run more simulation steps
>>> brain step 50
```

### Experiment Control

```bash
# List available experiments
>>> experiment list

# Run a specific experiment
>>> experiment run phase2_basic 1000

# Run integration experiment
>>> experiment run phase6_integration 5000
```

## Working with Brain Commands

### Brain Command Categories

Brain commands are organized into categories:

1. **Control Commands**: Reset, step, status
2. **Input Commands**: Inject current, apply neuromodulation
3. **System Commands**: Enable/disable brain systems
4. **Query Commands**: Get statistics, check connections

### Brain Command Examples

#### 1. Basic Brain Control

```bash
# Reset brain to clean state
>>> brain reset

# Advance simulation
>>> brain step 10

# Check status after reset
>>> brain status
```

#### 2. Neural Input

```bash
# Excite sensory neurons (simulating sensory input)
>>> brain inject sensory 50.0

# Inhibit motor neurons
>>> brain inject motor -10.0

# Modulate interneurons
>>> brain inject interneuron 15.0

# Apply neuromodulatory signals
>>> brain enable dopamine
>>> brain disable curiosity
```

#### 3. System Management

```bash
# Enable specific brain systems
>>> brain enable working_memory
>>> brain enable prediction
>>> brain enable attention

# Disable unnecessary systems
>>> brain disable novelty
>>> brain disable curiosity

# Check system status
>>> brain status
```

#### 4. Advanced Brain Commands

```bash
# Get detailed statistics
>>> brain step 100
>>> brain status

# Check specific neuron types
>>> brain inject sensory 30.0
>>> brain inject motor 20.0
>>> brain inject interneuron 25.0

# Monitor development
>>> brain enable development
>>> brain step 500
```

## Configuration Management

### Configuration Overview

Configuration controls all aspects of NLM behavior. It's organized into logical groups:

1. **Basic Simulation**: neuron count, timestep, regions
2. **Plasticity**: STDP, Hebbian, structural plasticity
3. **Development**: maturation, stages, structural changes
4. **Memory**: working memory, episodic memory, associative memory
5. **Neuromodulation**: dopamine, curiosity, novelty
6. **Prediction**: predictive processing, confidence
7. **Performance**: checkpointing, replay, optimization

### Basic Configuration Commands

#### Get Configuration Values

```bash
# Basic simulation parameters
>>> config get neuron_count
>>> config get simulation_timestep
>>> config get region_count
>>> config get connection_probability

# Plasticity parameters
>>> config get stdp_ltp_weight
>>> config get stdp_ltd_weight
>>> config get stdp_tau

# Development parameters
>>> config get maturation_rate
>>> config get initial_development_stage

# Neuromodulation parameters
>>> config get dopamine_sensitivity
>>> config get curiosity_strength
```

#### Set Configuration Values

```bash
# Set integer values
>>> config set neuron_count 1000
>>> config set region_count 3

# Set floating-point values
>>> config set simulation_timestep 0.0005
>>> config set connection_probability 0.2

# Set boolean values
>>> config set enable_checkpointing true
>>> config set enable_development false

# Set multiple values at once
>>> config set neuron_count 1000
>>> config set enable_plasticity true
>>> config set enable_memory true
```

### Configuration File Management

#### Save Configuration

```bash
# Save current configuration
>>> config save myconfig.cfg
>>> config save development.cfg

# Save with timestamp (recommended for experiments)
>>> config save config_$(date +%Y%m%d_%H%M%S).cfg
```

#### Load Configuration

```bash
# Load configuration
>>> config load default.cfg
>>> config load myconfig.cfg

# List available configuration files
ls config_*.cfg

# Compare configurations
>>> config load config_old.cfg
>>> config save config_new.cfg
```

### Configuration File Format

NLM uses JSON configuration files:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "enable_checkpointing": true,
    "enable_development": true,
    "enable_replay": true,
    "enable_prediction": true,
    "enable_neuromodulation": true,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "maturation_rate": 0.001,
    "initial_development_stage": 0,
    "dopamine_sensitivity": 1.0,
    "curiosity_strength": 0.5
}
```

## Experiment Control

### Available Experiments

NLM includes several experiment types:

#### 1. Phase 2: Basic Neural Computation

- **Description**: Real neural computation with LIF neurons
- **Focus**: STDP, Hebbian plasticity, structural plasticity
- **Typical Use**: Understanding basic neural dynamics

#### 2. Phase 3: World Interaction

- **Description**: Sensory-motor loop with reward prediction
- **Focus**: Developmental stages, novelty detection
- **Typical Use**: Embodied cognition

#### 3. Phase 4: Full Cognitive System

- **Description**: Complete cognitive architecture
- **Focus**: Memory, prediction, planning
- **Typical Use**: Advanced cognitive research

#### 4. Phase 6: Integration Test

- **Description**: Complete integration of all brain systems
- **Focus**: End-to-end functionality
- **Typical Use**: Final validation

### Running Experiments

#### Interactive Experiment Control

```bash
# List available experiments
>>> experiment list

# Run experiment with default steps
>>> experiment run phase2_basic

# Run experiment with custom steps
>>> experiment run phase6_integration 10000

# Monitor experiment progress
>>> experiment run phase4_cognitive 5000
```

#### Batch Experiment Execution

```bash
# Save experiment configuration
>>> config save experiment.cfg

# Configure for specific experiment
>>> config set neuron_count 1000
>>> config set enable_checkpointing true
>>> config set enable_development true

# Run experiment
>>> experiment run phase6_integration 20000

# Save results
>>> config save post_experiment.cfg
```

### Experiment Parameters

Experiments can be configured with various parameters:

```bash
# Neuron configuration
>>> config set neuron_count 1000

# Memory system configuration
>>> config set enable_working_memory true
>>> config set enable_episodic_memory true
>>> config set enable_associative_memory true

# Neuromodulation configuration
>>> config set enable_dopamine true
>>> config set enable_curiosity true
>>> config set enable_novelty true

# Prediction configuration
>>> config set enable_prediction true
>>> config set prediction_confidence 0.8
```

## Advanced Features

### Checkpointing and Persistence

#### Save and Load Brain State

```bash
# Save brain state for later use
>>> save brain_checkpoint_$(date +%s).bin

# Load saved brain state
>>> load brain_checkpoint_123456789.bin

# Use checkpoints for experimentation
>>> save baseline_checkpoint.bin
>>> brain reset
>>> experiment run phase2_basic 1000
>>> save learning_checkpoint.bin
```

### Performance Monitoring

#### Monitor System Performance

```bash
# Get performance statistics
>>> stats

# Generate performance reports
>>> stats

# Check memory usage
>>> brain status

# Monitor spike activity
>>> brain step 100
>>> brain status
```

### Visualization

#### Generate Visualizations

```bash
# Generate neural activity plots
>>> plot neural_activity.png

# Create brain connectivity diagrams
>>> plot connectivity_diagram.png

# Generate performance charts
>>> plot performance_report.png
```

### Scripting and Automation

#### Create Command Scripts

```bash
#!/bin/bash
# experiment.sh - Run NLM experiments

# Set up configuration
cd /workspace/app
./nlm --batch --command="config set neuron_count 500"

# Run baseline experiment
./nlm --batch --command="experiment run phase2_basic 1000"

# Run learning experiment
./nlm --batch --command="config set enable_plasticity true"
./nlm --batch --command="experiment run phase3_world 2000"

# Save results
./nlm --batch --command="config save post_learning.cfg"
```

#### Interactive Script Editor

```bash
# Create interactive script
cat > my_script.sh << 'EOF'
# My NLM experiment script
./nlm --command="brain reset"
./nlm --command="brain inject sensory 30.0"
./nlm --command="run 500"
./nlm --command="brain status"
EOF

# Make script executable
chmod +x my_script.sh

# Run script
./my_script.sh
```

## Troubleshooting

### Common Issues

#### 1. "Unknown Command" Error

**Problem**: Command not recognized

**Solution**: 
```bash
>>> help
# Check available commands
>>> brain
# Check brain commands
>>> config
# Check config commands
```

#### 2. Configuration Not Found

**Problem**: Configuration file not found

**Solution**:
```bash
>>> config save test.cfg
>>> config load test.cfg
```

#### 3. Brain Not Initialized

**Problem**: Brain system not ready

**Solution**:
```bash
>>> brain reset
>>> brain status
```

#### 4. Performance Issues

**Problem**: Simulation too slow

**Solution**:
```bash
# Reduce complexity
>>> config set neuron_count 100
>>> config set region_count 1

# Disable expensive features
>>> config set enable_replay false
>>> config set enable_development false

# Run shorter simulations
>>> brain step 100
>>> brain status
```

### Getting Help

#### Online Resources

- **Documentation**: `docs/` directory
- **API Reference**: `docs/doxygen/`
- **Examples**: `examples/` directory
- **Community**: GitHub repository

#### Command Line Help

```bash
# Get help for specific commands
>>> help brain
>>> help config
>>> help experiment

# Get usage examples
>>> help brain
>>> brain

# Get configuration information
>>> config
>>> help config
```

#### Debugging

```bash
# Enable debug logging
>>> brain status

# Check system resources
free -h
df -h

# Monitor process
htop

# Check log files
ls -la *.log
```

## Further Learning

### Advanced Topics

1. **Neural Dynamics**: Understanding LIF neuron models
2. **Plasticity Rules**: STDP, Hebbian learning mechanisms
3. **Memory Systems**: Working memory, episodic memory, associative memory
4. **Neuromodulation**: Dopamine, curiosity, novelty detection
5. **Cognitive Architecture**: Planning, attention, concept formation
6. **Performance Optimization**: Memory pools, SIMD, parallel processing
7. **Integration Testing**: Phase 6 experiment design

### Research Applications

#### Educational Applications

- **Neuroscience education**: Demonstrate neural computation
- **Machine learning**: Alternative to traditional neural networks
- **Cognitive modeling**: Study learning and memory
- **Robotics**: Neural control systems

#### Research Applications

- **Brain-computer interfaces**: Neural signal processing
- **Neuroprosthetics**: Neural control algorithms
- **Computational neuroscience**: Modeling brain function
- **Artificial intelligence**: Neuromorphic computing

### Community and Contributions

#### Contributing to NLM

1. **Report Issues**: File bug reports on GitHub
2. **Submit Pull Requests**: Propose improvements
3. **Documentation**: Write guides and tutorials
4. **Testing**: Add unit tests and integration tests
5. **Performance**: Profile and optimize code

#### Development Guidelines

- **Code Quality**: Follow C++20 best practices
- **Documentation**: Keep documentation updated
- **Testing**: Ensure test coverage
- **Performance**: Monitor performance impact
- **Compatibility**: Maintain backward compatibility

## Conclusion

You've completed the NLM tutorial guide! You now have:

- **Working Knowledge**: How to use NLM's unified command-line interface
- **Configuration Skills**: How to manage simulation parameters
- **Experiment Control**: How to run and monitor experiments
- **Brain Commands**: How to interact with neural systems
- **Troubleshooting**: How to solve common issues

NLM is a powerful research tool for studying artificial developmental brains. Continue exploring the documentation, try different experiment configurations, and contribute to this exciting field of computational neuroscience!

### Next Steps

1. **Run Experiments**: Try different experiment configurations
2. **Learn Neuromodulation**: Explore dopamine, curiosity, and novelty
3. **Study Memory**: Understand working memory, episodic memory, and associative memory
4. **Contribute**: Share your findings and improvements with the community

Enjoy your journey into neural computation with NLM!
