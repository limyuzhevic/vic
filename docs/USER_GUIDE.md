# NLM (熙然) - Neural Learning Machine User Guide

**An Experimental Artificial Developmental Brain**

This guide provides comprehensive documentation for using the NLM (Neural Learning Machine) project, covering all phases from basic neural computation to full Phase 6 integration.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Command Line Interface](#command-line-interface)
   - [Phase Selection](#phase-selection)
   - [Configuration Options](#configuration-options)
   - [Help and Version Information](#help-and-version-information)
3. [Phase Overview](#phase-overview)
   - [Phase 2: Basic Neural Computation](#phase-2-basic-neural-computation)
   - [Phase 4: Advanced Brain Systems](#phase-4-advanced-brain-systems)
   - [Phase 5: Performance Optimized](#phase-5-performance-optimized)
   - [Phase 6: Full Integration](#phase-6-full-integration)
4. [System Architecture](#system-architecture)
   - [Brain Systems](#brain-systems)
   - [Memory Systems](#memory-systems)
   - [Neuromodulation Systems](#neuromodulation-systems)
   - [Development System](#development-system)
5. [Advanced Usage](#advanced-usage)
   - [Configuration Files](#configuration-files)
   - [Integration Experiments](#integration-experiments)
   - [Performance Tuning](#performance-tuning)
6. [Examples](#examples)
   - [Basic Usage](#basic-usage)
   - [Integration Testing](#integration-testing)
   - [Custom Configuration](#custom-configuration)
7. [Troubleshooting](#troubleshooting)
8. [References](#references)

## Getting Started

The NLM project is an experimental artificial developmental brain that simulates neural computation with learning capabilities. It has evolved through multiple phases, each adding new capabilities:

### Quick Start

```bash
# Run the default Phase 6 integration demo
./nlm

# Run specific phase (2, 4, 5, or 6)
./nlm --phase 2
./nlm --phase 4
./nlm --phase 5
./nlm --phase 6

# View help information
./nlm --help

# View version information
./nlm --version
```

## Command Line Interface

### Phase Selection

The NLM command line interface supports multiple phases, allowing you to run specific demonstrations:

#### Phase 2: Basic Neural Computation
- Implements Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Best for understanding basic neural mechanisms

#### Phase 4: Advanced Brain Systems
- Neural prediction system for forward modeling
- Working memory for temporary information storage
- Episodic memory for experience storage
- Concept formation for pattern discovery
- Neural attention mechanisms (not Transformer-based)
- Predictive planning capabilities
- Self-model development
- Social learning algorithms
- Continual learning mechanisms

#### Phase 5: Performance Optimized
- Memory pools and event queues for efficient resource management
- SIMD vectorization for high-performance computation
- Parallel processing capabilities
- Checkpoint system for persistence
- Optimized for large-scale simulations

#### Phase 6: Full Integration (Default)
- **All previous phases integrated into a coherent artificial brain**
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load functional
- Replay and consolidation operational
- Comprehensive integration tests
- Advanced agent-based simulation

### Configuration Options

```bash
./nlm [options]

Options:
  --phase 2    Run Phase 2: Basic neural computation
  --phase 4    Run Phase 4: Advanced brain systems
  --phase 5    Run Phase 5: Performance optimized
  --phase 6    Run Phase 6: Full integration (default)
  --config FILE   Load configuration from FILE
  --help        Show this help message
  --version     Show version information
```

### Help and Version Information

```bash
# Show comprehensive help information
./nlm --help

# Show version and build information
./nlm --version
```

## Phase Overview

### Phase 2: Basic Neural Computation

Phase 2 implements the fundamental neural computation mechanisms:

**Key Features:**
- **Leaky Integrate-and-Fire (LIF) neurons**: Biophysical model of neuronal dynamics
- **Event-driven spike propagation**: Efficient transmission of neural signals
- **STDP and Hebbian plasticity**: Synaptic learning rules
- **Structural plasticity**: Formation and elimination of connections

**Use Cases:**
- Understanding basic neural mechanisms
- Testing plasticity algorithms
- Learning neural dynamics

**Example Command:**
```bash
./nlm --phase 2
```

### Phase 4: Advanced Brain Systems

Phase 4 introduces advanced cognitive and memory systems:

**Key Features:**
- **Neural prediction system**: Forward models of environment dynamics
- **Working memory**: Temporary storage and manipulation of information
- **Episodic memory**: Storage of experiences with temporal context
- **Concept formation**: Abstraction and categorization of patterns
- **Neural attention**: Competitive selection of information focus
- **Predictive planning**: Goal-directed behavior planning
- **Self-model**: Representation of the agent's own state

**Use Cases:**
- Cognitive architecture research
- Complex behavior simulation
- Memory system development

**Example Command:**
```bash
./nlm --phase 4
```

### Phase 5: Performance Optimized

Phase 5 focuses on computational efficiency and scalability:

**Key Features:**
- **Memory pools**: Efficient allocation of neural resources
- **Event queues**: Optimized spike event processing
- **SIMD vectorization**: Parallel computation acceleration
- **Parallel processing**: Multi-core CPU utilization
- **Checkpoint system**: State persistence and recovery

**Use Cases:**
- Large-scale neural simulations
- High-performance computing
- Long-running experiments

**Example Command:**
```bash
./nlm --phase 5
```

### Phase 6: Full Integration

Phase 6 represents the culmination of all previous phases, integrating everything into a coherent artificial brain:

**Key Features:**
- **Complete system integration**: All systems work together seamlessly
- **End-to-end functionality**: From sensory input to motor output
- **Lifelong learning**: Continuous adaptation and development
- **Memory consolidation**: Transfer between working and episodic memory
- **Replay and consolidation**: Strengthening important memories
- **Advanced agent behavior**: Goal-directed, adaptive behavior

**Use Cases:**
- Complete artificial brain demonstrations
- Integration testing and validation
- Advanced research scenarios

**Example Command:**
```bash
./nlm --phase 6
```

## System Architecture

### Brain Systems

The NLM brain consists of several interconnected systems:

#### 1. Core Neural Computation
- **LIF neurons**: Biological neuron model with membrane potential dynamics
- **Spike system**: Event-driven signal transmission
- **Neural regions**: Organized brain areas with specialized functions
- **Populations**: Groups of neurons with similar properties

#### 2. Memory Systems
- **Working memory**: Transient active information storage
- **Episodic memory**: Storage of experiences and events
- **Associative memory**: Pattern associations and relationships
- **Semantic memory**: Abstract knowledge and facts
- **Procedural memory**: Learned skills and habits

#### 3. Neuromodulation Systems
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Detection of unexpected sensory input
- **Prediction error**: Difference between expected and actual outcomes
- **Acetylcholine**: Attention and memory consolidation
- **Norepinephrine**: Arousal and vigilance
- **Serotonin**: Mood and impulse regulation

#### 4. Development System
- **Developmental stages**: Maturation from initial to adult stages
- **Plasticity modulation**: Age-dependent learning rates
- **Synaptogenesis**: Formation of new connections
- **Pruning**: Elimination of unused connections
- **Critical periods**: Windows of enhanced plasticity

### Integration Architecture

The NLM brain follows a closed-loop architecture:

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

## Advanced Usage

### Configuration Files

NLM supports configuration files to customize simulation parameters:

**Default Configuration Location:** `configs/default.cfg`

**Example Configuration:**
```ini
[simulation]
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001

[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[memory]
working_memory_capacity = 100
max_episodes = 1000
replay_interval = 100
consolidation_interval = 1000

[neuromodulation]
dopamine_level = 0.5
curiosity_weight = 0.3
novelty_threshold = 0.2

[development]
enable_development = true
initial_plasticity = 1.0
```

**Usage:**
```bash
./nlm --phase 6 --config myconfig.cfg
```

### Integration Experiments

NLM provides several integration experiments to test system functionality:

#### 1. Integration Verification
```bash
# Verifies that all basic brain systems are properly initialized
./nlm --phase 6 --integration-verify
```

#### 2. Memory Integration Test
```bash
# Tests memory system integration
./nlm --phase 6 --memory-test
```

#### 3. Neuromodulation Integration Test
```bash
# Tests neuromodulation system integration
./nlm --phase 6 --neuromodulation-test
```

#### 4. Checkpoint Test
```bash
# Tests checkpoint save/load functionality
./nlm --phase 6 --checkpoint-test
```

#### 5. Replay Test
```bash
# Tests memory replay functionality
./nlm --phase 6 --replay-test
```

### Performance Tuning

For optimal performance, consider these tuning parameters:

**CPU Performance:**
```bash
# Use multiple cores for parallel processing
./nlm --phase 5 --threads 4
```

**Memory Optimization:**
```bash
# Reduce memory usage for large simulations
./nlm --phase 5 --neuron-count 500
```

**Checkpoint Frequency:**
```bash
# More frequent checkpoints
./nlm --phase 6 --checkpoint-frequency 500
```

## Examples

### Basic Usage

#### Example 1: Run Phase 6 Demo
```bash
# This is the recommended way to run the complete integration demo
cd /path/to/nlm
./nlm --phase 6
```

**What happens:**
1. Integration verification passes
2. Memory integration test runs
3. Neuromodulation integration test runs
4. Checkpoint test runs
5. Replay test runs
6. Full 2000-step simulation with 500 neurons
7. Results and integration status displayed

#### Example 2: Custom Configuration
```bash
# Create custom configuration
cat > myconfig.cfg << EOF
[simulation]
neuron_count = 2000
max_steps = 5000

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018

[memory]
working_memory_capacity = 200
max_episodes = 5000
EOF

# Run with custom configuration
./nlm --phase 6 --config myconfig.cfg
```

#### Example 3: Phase 4 Advanced Systems
```bash
# Run Phase 4 to see advanced systems in action
./nlm --phase 4

# This demonstrates:
# - Neural prediction
# - Working memory
# - Episodic memory
# - Concept formation
# - Neural attention
# - Predictive planning
```

### Integration Testing

#### Example: Full Integration Test Script
```bash
#!/bin/bash
# integration_test.sh - Run all NLM integration tests

set -e

echo "=== NLM Integration Test Suite ==="

# Test 1: Basic integration verification
echo "1. Running integration verification..."
./nlm --phase 6 --integration-verify

# Test 2: Memory integration

echo "2. Running memory integration test..."
./nlm --phase 6 --memory-test

# Test 3: Neuromodulation integration

echo "3. Running neuromodulation integration test..."
./nlm --phase 6 --neuromodulation-test

# Test 4: Checkpoint functionality

echo "4. Running checkpoint test..."
./nlm --phase 6 --checkpoint-test

# Test 5: Replay functionality

echo "5. Running replay test..."
./nlm --phase 6 --replay-test

# Test 6: Full integration demo
echo "6. Running full integration demo..."
./nlm --phase 6

echo "=== All tests completed successfully ==="
```

### Custom Configuration

#### Example: Development-Focused Configuration
```bash
# Create development-focused configuration
cat > dev_config.cfg << EOF
[simulation]
neuron_count = 1000
region_count = 2
connection_probability = 0.05
simulation_timestep = 0.001

[development]
enable_development = true
initial_plasticity = 1.0
critical_period_active = true
adult_plasticity = 0.2

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
synaptogenesis_rate = 0.00015
pruning_rate = 0.000005

[memory]
working_memory_capacity = 150
max_episodes = 2000
replay_interval = 50
consolidation_interval = 500

[neuromodulation]
dopamine_level = 0.7
curiosity_weight = 0.4
novelty_threshold = 0.15

[agent]
enable_curiosity = true
enable_reward_modulation = true
planning_depth = 3
EOF

# Run development configuration
./nlm --phase 6 --config dev_config.cfg
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Integration verification failed"
**Problem:** One or more brain systems are not properly integrated.

**Solution:**
```bash
# Run with verbose output to see specific failures
./nlm --phase 6 --verbose

# Check system logs for detailed error messages
cat /tmp/nlm_integration.log
```

#### Issue 2: Performance Issues
**Problem:** Simulation is slow or unresponsive.

**Solutions:**
- Use Phase 5 for better performance
- Reduce neuron count
- Use smaller simulation timestep
- Enable parallel processing

```bash
# Use performance-optimized Phase 5
./nlm --phase 5 --neuron-count 500

# Enable parallel processing
./nlm --phase 5 --threads 4
```

#### Issue 3: Memory Issues
**Problem:** Out of memory errors.

**Solutions:**
- Use Phase 5 memory pools
- Reduce simulation size
- Increase checkpoint frequency
- Use configuration options

```bash
# Use Phase 5 for better memory management
./nlm --phase 5 --neuron-count 200

# Use custom configuration with lower memory requirements
cat > low_memory.cfg << EOF
[simulation]
neuron_count = 200
max_steps = 1000

[memory]
working_memory_capacity = 50
max_episodes = 100
EOF

./nlm --phase 6 --config low_memory.cfg
```

### Error Messages

#### "Failed to save checkpoint"
**Meaning:** Checkpoint system encountered an error.

**Troubleshooting:**
1. Check disk space: `df -h`
2. Check file permissions: `ls -la /tmp`
3. Try different checkpoint path: `--checkpoint-path /other/path`

#### "Brain initialization failed"
**Meaning:** Configuration error or system resources insufficient.

**Troubleshooting:**
1. Check configuration file syntax
2. Verify required libraries are installed
3. Try with smaller simulation parameters
4. Check system resources: `free -h`

## References

### Documentation

- **NLM README**: Main project documentation
- **Phase 6 Integration Demo**: `src/experiments/Phase6Demo.cpp`
- **Phase 6 Experiment**: `src/experiments/Phase6IntegratedExperiment.hpp`

### API References

- **Brain Class**: `src/brain/Brain.hpp`
- **Memory Systems**: `src/memory/Memory.hpp`
- **Neuromodulation Systems**: `src/neuromodulation/Neuromodulator.hpp`
- **Development System**: `src/development/DevelopmentSystem.hpp`

### Research Papers

- "An Experimental Artificial Developmental Brain" - NLM Project Documentation
- "Phase 6 Integration of Memory, Prediction, and Neuromodulation Systems"
- "Event-Driven Neural Dynamics for Efficient Brain Simulation"

### Online Resources

- **NLM GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation Website**: https://nlm.readthedocs.io
- **Project Wiki**: https://github.com/nlm-project/nlm/wiki

### Contact and Support

For questions, issues, or collaboration opportunities:

- **Email**: research@nlm-project.org
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Slack Channel**: https://nlm-project.slack.com

### License

This project is licensed under the MIT License. See `LICENSE` file for details.

---

**Version**: 0.1.0 (Phase 6.0)
**Built**: $(date)
**Authors**: NLM Research Team

*This documentation is continuously updated. For the latest version, visit the NLM documentation website.*
EOF
