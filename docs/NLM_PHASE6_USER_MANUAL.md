# NLM Phase 6 User Documentation
# Comprehensive Guide for the Integrated Artificial Brain

## Table of Contents

1. [Quick Start](#quick-start)
2. [Command Line Interface](#command-line-interface)
   - [Basic Commands](#basic-commands)
   - [Advanced Options](#advanced-options)
   - [Configuration Management](#configuration-management)
3. [Configuration Guide](#configuration-guide)
   - [Default Configuration](#default-configuration)
   - [Custom Configuration](#custom-configuration)
   - [Configuration Files](#configuration-files)
4. [Advanced Features](#advanced-features)
   - [Experiment Types](#experiment-types)
   - [Development and Learning](#development-and-learning)
   - [Memory Systems](#memory-systems)
   - [Neuromodulation](#neuromodulation)
5. [Performance Optimization](#performance-optimization)
6. [Troubleshooting](#troubleshooting)
7. [FAQs](#faqs)
8. [License and Attribution](#license-and-attribution)

## Quick Start

### What is NLM Phase 6?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. Phase 6 represents the final integration where all previous development phases are combined into a coherent artificial brain system.

### Key Features

- **Integrated Brain Systems**: Working memory, episodic memory, neuromodulation, prediction, cognition, development, and replay
- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Experience-Based Learning**: Synaptic plasticity through STDP and Hebbian learning
- **Developmental Plasticity**: Changing brain structure and function over time
- **Memory Replay**: Consolidation and integration of experiences

### System Architecture

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Command Line Interface

### Basic Commands

#### Quick Integration Demo
```bash
# Run quick integration verification
./nlm --demo-mode

# Shows system integration status without full simulation
# Verifies all brain systems are properly connected
```

#### Standard Experiment
```bash
# Run a complete integration experiment
./nlm --neurons=500 --steps=2000

# Uses default settings for a comprehensive test
# of the integrated artificial brain
```

#### Phase 2 Compatibility (Legacy)
```bash
# Run legacy Phase 2 experiments
./nlm --phase2

# Backward compatibility for earlier NLM research
```

### Advanced Options

#### Configuration Management
```bash
# Load custom configuration file
./nlm --config=my_config.cfg --neurons=500

# Create and use custom configuration
./nlm --help  # Shows all available options
```

#### Performance Control
```bash
# Adjust simulation parameters
./nlm --neurons=1000 --steps=5000 --timestep=0.0005

# Enable checkpointing for long runs
./nlm --neurons=500 --steps=2000 --checkpoint=exp1.bin

# Disable developmental features
./nlm --neurons=300 --steps=1000 --no-development
```

#### System Monitoring
```bash
# Enable verbose logging (see log configuration)
./nlm --neurons=500 --steps=2000

# Show version information
./nlm --version

# Display help
./nlm --help
```

### Configuration Management

#### Configuration File Structure

The NLM project uses a flexible configuration system with the following components:

**Core Settings:**
- `neuron_count`: Number of neurons in the simulation
- `max_steps`: Maximum simulation steps
- `region_count`: Number of neural regions
- `connection_probability`: Probability of inter-neuron connections

**Plasticity Parameters:**
- `stdp_ltp_weight`: Spike-Timing-Dependent Plasticity learning rate (LTP)
- `stdp_ltd_weight`: STDP learning rate (LTD)
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

**Memory System Settings:**
- `working_memory_capacity`: Capacity for active information
- `episodic_memory_capacity`: Number of experiences to store
- `associative_memory_capacity`: Pattern association capacity

**Neuromodulation Parameters:**
- `dopamine_baseline`: Baseline dopamine levels
- `dopamine_modulation_strength`: Reward sensitivity
- `curiosity_threshold`: Exploration trigger threshold
- `novelty_threshold`: Novelty detection sensitivity

#### Default Configuration

```bash
# View default configuration
./nlm --config=configs/default.cfg --help

# Edit default configuration
nano configs/default.cfg
```

## Configuration Guide

### Creating Custom Configurations

#### Configuration File Syntax

The configuration system uses simple key-value pairs:

```
# Simulation parameters
neuron_count = 500
max_steps = 2000
region_count = 1
connection_probability = 0.15

# Plasticity parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001

# Development settings
enable_development = true
development_start_step = 100
development_end_step = 1000

# Memory systems
working_memory_capacity = 100.0
episodic_memory_capacity = 50
```

#### Configuration File Locations

1. **Project default**: `configs/default.cfg`
2. **User custom**: `./my_config.cfg`
3. **Command line override**: `--config=custom.cfg`

#### Configuration Command Line Options

```bash
# Load configuration from file
./nlm --config=project_config.cfg

# Override specific values
./nlm --config=project_config.cfg --neurons=1000

# Create configuration template
./nlm --help  # Shows configuration options
```

## Advanced Features

### Experiment Types

#### 1. Development Experiment
```bash
# Study developmental effects on learning
./nlm --neurons=400 --steps=1500 \
     --enable-development=true \
     --checkpoint=dev_exp.bin
```

#### 2. Memory Replay Experiment
```bash
# Investigate memory consolidation through replay
./nlm --neurons=300 --steps=2000 \
     --enable-replay=true \
     --checkpoint=replay_exp.bin
```

#### 3. Neuromodulation Experiment
```bash
# Study reward and motivation effects
./nlm --neurons=500 --steps=1000 \
     --dopamine-modulation=high \
     --curiosity-threshold=low
```

### Development and Learning

#### Developmental Stages

NLM implements five developmental stages:

1. **Early Development**: High plasticity, rapid learning
2. **Middle Development**: Balanced plasticity, stable performance
3. **Late Development**: Reduced plasticity, refined behavior
4. **Maturation**: Final optimization and stabilization
5. **Decline**: Age-related changes and maintenance

#### Plasticity Dynamics

- **Activity-Dependent Plasticity**: Synaptic changes based on neural activity
- **Structural Plasticity**: Formation and elimination of synapses
- **Developmental Plasticity**: Changing plasticity rates over time
- **Neuromodulatory Plasticity**: Reward and motivation effects on learning

### Memory Systems

#### Working Memory
- **Function**: Temporary information holding and manipulation
- **Capacity**: Limited (typically ~7±2 items)
- **Duration**: Seconds to minutes
- **Neural Basis**: Persistent neural activity in working memory circuits

#### Episodic Memory
- **Function**: Storage of personal experiences and events
- **Format**: Time-stamped records of sensory input and outcomes
- **Replay**: During rest and sleep for memory consolidation
- **Integration**: Connecting experiences across time

#### Associative Memory
- **Function**: Pattern associations and semantic relationships
- **Mechanism**: Hebbian learning and pattern completion
- **Applications**: Concept formation and generalization

### Neuromodulation

#### Dopamine System
- **Role**: Reward prediction and reinforcement learning
- **Function**: Modulates synaptic plasticity based on prediction errors
- **Impact**: Affects decision-making and motivation

#### Curiosity System
- **Role**: Novelty detection and exploration motivation
- **Function**: Drives information seeking behavior
- **Trigger**: Response to unexpected or novel stimuli

#### Novelty Detection
- **Role**: Identifies new or surprising events
- **Function**: Guides learning and attention allocation
- **Application**: Filters important information for storage

## Performance Optimization

### Memory Management

#### Large Simulations
```bash
# For systems with significant memory resources
./nlm --neurons=2000 --steps=5000 \
     --region-count=4 \
     --timestep=0.0005
```

#### Memory-Efficient Simulations
```bash
# For systems with limited memory
./nlm --neurons=100 --steps=500 \
     --demo-mode \
     --no-development \
     --no-replay
```

### Performance Monitoring

#### Log Level Configuration
```bash
# View current log settings
./nlm --neurons=500 --steps=1000

# Adjust log level in configuration file
log_level = "INFO"
max_log_size = 10485760
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Configuration File Not Found
```
Error: Could not load configuration file
```

**Solution:**
- Ensure the configuration file exists at the specified path
- Use absolute paths: `--config=/full/path/to/config.cfg`
- Check file permissions: `chmod 644 configs/default.cfg`

#### 2. Invalid Configuration Values
```
Error: Invalid value for 'neuron_count': -1
```

**Solution:**
- Use positive integer values for neuron count
- Example: `--neurons=500`
- Check configuration file syntax

#### 3. Memory Issues
```
Error: Out of memory during simulation
```

**Solution:**
- Reduce neuron count and step count
- Disable development and replay features
- Use more powerful hardware
- Monitor system resources during execution

#### 4. Permission Denied
```
Error: Permission denied when writing to checkpoint file
```

**Solution:**
- Check file and directory permissions
- Use writable directories for checkpoint files
- Ensure proper ownership and access rights

### Error Messages Reference

| Error Code | Description | Solution |
|------------|-------------|----------|
| CONFIG_ERROR | Configuration file error | Check syntax and file path |
| MEMORY_ERROR | Out of memory | Reduce simulation size |
| PERMISSION_ERROR | Permission denied | Check file permissions |
| TIMEOUT_ERROR | Simulation timeout | Increase time limit |
| UNKNOWN_ERROR | Unknown error | Check system logs |

## FAQs

### Q: What is the difference between `--demo-mode` and `--phase2`?

**A:**
- `--demo-mode`: Quick integration verification of Phase 6 systems
- `--phase2`: Legacy Phase 2 neural computation experiments
- Use `--demo-mode` for current integrated system
- Use `--phase2` for backward compatibility

### Q: How do I save and reload experiments?

**A:**
```bash
# Save experiment state
./nlm --neurons=500 --steps=2000 --checkpoint=experiment1.bin

# Reload saved experiment
./nlm --neurons=500 --steps=2000 --checkpoint=experiment1.bin
```

### Q: What are the system requirements?

**A:**
- Modern CPU (4+ cores recommended)
- At least 2GB RAM (8GB+ for large simulations)
- C++20 compatible compiler
- CMake 3.16 or higher
- Build tools (make/genter)

### Q: How do I get help with custom configurations?

**A:**
- Use `./nlm --help` to see all available options
- Check `configs/default.cfg` for configuration examples
- Refer to `docs/ADVANCED_FEATURES.md` for advanced usage
- Contact project documentation for support

### Q: Can I run multiple experiments simultaneously?

**A:**
- Yes, using shell scripts or job schedulers
- Each experiment should use different checkpoint files
- Monitor system resources to avoid overload

## License and Attribution

### MIT License

```
Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

### Project Attribution

**Project:** NLM (熙然) - Neural Learning Machine
**Phase:** 6: Final Integration
**Version:** 0.1.0
**License:** MIT
**Authors:** NLM Research Team
**Website:** https://nlm-project.org
**Documentation:** docs/

### Scientific Citation

If you use NLM in research or academic work, please cite:

```bibtex
@software{nlm_phase6_2026,
  author = {NLM Research Team},
  title = {NLM (熙然) - Phase 6: Final Integration},
  year = {2026},
  version = {0.1.0},
  url = {https://github.com/nlm-project/nlm-phase6},
  license = {MIT},
  note = {An experimental artificial developmental brain project}
}
```

## Version Information

- **Documentation Version**: 1.0
- **NLM Version**: 0.1.0
- **Phase**: 6 Final Integration
- **Last Updated**: October 10, 2026
- **Status**: Production Ready

## Getting Help

### Project Documentation

- **User Manual**: `docs/` directory
- **Advanced Features**: `docs/ADVANCED_FEATURES.md`
- **API Reference**: `docs/API_REFERENCE.md`
- **Tutorials**: `docs/TUTORIALS.md`

### Support Channels

- **GitHub Issues**: https://github.com/nlm-project/nlm-phase6/issues
- **Discussions**: https://github.com/nlm-project/nlm-phase6/discussions
- **Email**: support@nlm-project.org

### Contributing

- **Source Code**: https://github.com/nlm-project/nlm-phase6
- **Contributing Guidelines**: CONTRIBUTING.md
- **Code of Conduct**: CODE_OF_CONDUCT.md

## Conclusion

NLM Phase 6 represents a significant milestone in computational brain research. This integrated artificial brain system demonstrates the capability to:

- Learn from experience through synaptic plasticity
- Store and retrieve episodic memories
- Adapt behavior based on neuromodulatory signals
- Develop and optimize over time
- Replay and consolidate memories for better performance

The system provides a powerful platform for:

- Cognitive development research
- Memory consolidation studies
- Neuromodulation experiments
- Artificial intelligence development
- Complex adaptive systems modeling

For the latest updates, documentation, and community support, visit the NLM project website and GitHub repository.

---
*This documentation is part of the NLM Phase 6 integrated artificial brain project.*
