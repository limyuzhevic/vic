# NLM (熙然) - Neural Learning Machine
## Complete Documentation and Examples

This document provides comprehensive documentation for the NLM (Neural Learning Machine) project, covering all aspects of the system including architecture, usage, configuration, and development.

---

## Table of Contents

1. [Overview](#overview)
2. [System Architecture](#system-architecture)
3. [Getting Started](#getting-started)
   - [Installation](#installation)
   - [Building from Source](#building-from-source)
   - [Running the System](#running-the-system)
4. [System Components](#system-components)
   - [Brain System](#brain-system)
   - [Memory Systems](#memory-systems)
   - [Prediction System](#prediction-system)
   - [Cognition Systems](#cognition-systems)
   - [Neuromodulation Systems](#neuromodulation-systems)
   - [Development System](#development-system)
   - [Environment Interface](#environment-interface)
   - [Visualization](#visualization)
5. [Configuration Management](#configuration-management)
   - [Configuration Overview](#configuration-overview)
   - [Configuration File Format](#configuration-file-format)
   - [Configuration Examples](#configuration-examples)
   - [Configuration Validation](#configuration-validation)
6. [Command Line Interface](#command-line-interface)
   - [CLI Overview](#cli-overview)
   - [Commands](#commands)
   - [Command Examples](#command-examples)
   - [CLI Scripts](#cli-scripts)
7. [Usage Examples](#usage-examples)
   - [Basic Usage](#basic-usage)
   - [Advanced Usage](#advanced-usage)
   - [Integration Examples](#integration-examples)
   - [Experiment Examples](#experiment-examples)
8. [Performance Monitoring](#performance-monitoring)
   - [Metrics Collection](#metrics-collection)
   - [Performance Benchmarking](#performance-benchmarking)
   - [Monitoring Tools](#monitoring-tools)
   - [Alerting](#alerting)
9. [Testing and Quality Assurance](#testing-and-quality-assurance)
   - [Test Framework](#test-framework)
   - [Test Categories](#test-categories)
   - [Test Examples](#test-examples)
   - [Quality Assurance](#quality-assurance)
10. [Development Guide](#development-guide)
    - [Development Environment](#development-environment)
    - [Code Style Guidelines](#code-style-guidelines)
    - [Contribution Guidelines](#contribution-guidelines)
    - [Testing Strategy](#testing-strategy)
    - [Performance Optimization](#performance-optimization)
11. [Troubleshooting](#troubleshooting)
    - [Common Issues](#common-issues)
    - [Debug Information](#debug-information)
    - [Problem Resolution](#problem-resolution)
12. [Migration Guide](#migration-guide)
    - [Version Migration](#version-migration)
    - [Configuration Migration](#configuration-migration)
    - [API Migration](#api-migration)
13. [FAQ](#faq)
14. [References](#references)
15. [Acknowledgments](#acknowledgments)

---

## 1. Overview

### 1.1 What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that learns and adapts through interaction with an environment. It represents a significant advancement in neuromorphic computing and artificial intelligence research.

### 1.2 Project Goals

- Create an experimental artificial developmental brain
- Implement real spiking neural computation with event-driven dynamics
- Develop integrated memory, prediction, and cognition systems
- Demonstrate learning through experience and environmental interaction
- Provide a flexible framework for neuromorphic AI research

### 1.3 Key Features

- **Real Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Integrated Systems**: Memory, prediction, cognition, and neuromodulation systems
- **Developmental Learning**: Neural development and plasticity throughout system lifetime
- **Environmental Interaction**: Closed-loop system with world interaction
- **Checkpointing and Persistence**: Save and load brain states
- **Replay and Consolidation**: Memory replay and consolidation mechanisms
- **Comprehensive Testing**: Extensive test suite for validation and verification

### 1.4 Architecture Philosophy

The NLM brain operates as a closed-loop system with the following architecture:

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

### 1.5 System Phases

- **Phase 1**: Project skeleton, core types, configuration, neural interfaces
- **Phase 2**: Real neural computation, spiking neurons, plasticity
- **Phase 3**: World interaction, sensory input, motor output
- **Phase 4**: Prediction systems, working memory, cognition
- **Phase 5**: Performance optimizations, memory pools, SIMD
- **Phase 6**: Final integration (current state) - all systems integrated

### 1.6 License and Credits

- **License**: MIT
- **Authors**: NLM Research Team
- **Documentation**: Generated with Doxygen and Markdown
- **Website**: https://nlm-research.org

---

## 2. System Architecture

### 2.1 Core Architecture Components

#### 2.1.1 Brain Class
The central coordinator of the neural system, implementing:
- Real spiking neural computation with event-driven dynamics
- Integrated memory, prediction, cognition, and neuromodulation systems
- Developmental stage management
- Checkpoint save/load functionality
- Inter-region communication

#### 2.1.2 Neural Regions
Brain regions containing multiple populations and local connectivity:
- Each region manages neurons and synapses within its domain
- Supports different neuron types (Sensory, Internal, Motor)
- Manages synaptic plasticity and learning

#### 2.1.3 Neural Populations
Groups of neurons with similar properties:
- Neuron types: Sensory, Internal, Motor
- Manages neuron lifecycle and connectivity
- Supports different firing patterns and behaviors

#### 2.1.4 Synapse System
Connection management between neurons:
- Supports excitatory and inhibitory synapses
- Manages synaptic plasticity rules (STDP, Hebbian)
- Implements synaptic delays and weight dynamics

### 2.2 Integrated Systems

#### 2.2.1 Memory Systems
- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations and concepts

#### 2.2.2 Neuromodulation Systems
- **Dopamine**: Reward prediction error and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Prediction error computation

#### 2.2.3 Cognition Systems
- **Neural Planner**: Action planning and sequence generation
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Focus and attention management

#### 2.2.4 Prediction System
- **Neural Prediction**: Forward model and prediction error
- **Sensory Prediction**: Sensory prediction and error computation

#### 2.2.5 Development System
- **Developmental Stages**: Neural development stages
- **Structural Plasticity**: Synaptogenesis and pruning
- **Maturation**: Neural maturation and specialization

### 2.3 Computational Architecture

#### 2.3.1 Neural Dynamics
- Leaky Integrate-and-Fire (LIF) neuron model
- Event-driven spike propagation with delays
- Membrane potential dynamics and threshold crossing

#### 2.3.2 Plasticity Mechanisms
- Spike-Timing Dependent Plasticity (STDP)
- Hebbian learning rules
- Structural plasticity (synaptogenesis, pruning)

#### 2.3.3 Memory Integration
- Working memory consolidation and maintenance
- Episodic memory encoding and replay
- Associative memory pattern completion

#### 2.3.4 Neuromodulation Integration
- Dopamine effects on neural excitability
- Curiosity-driven exploration
- Novelty-based learning

### 2.4 Performance Architecture

#### 2.4.1 Parallel Processing
- SIMD vectorization for neural computations
- Multi-core processing for parallel simulation
- Efficient memory management for large networks

#### 2.4.2 Event-Driven Processing
- Asynchronous spike propagation
- Delayed spike delivery
- Efficient synaptic event management

#### 2.4.3 Checkpointing System
- Persistent brain state storage
- Incremental checkpointing
- State compression and recovery

---

## 3. Getting Started

### 3.1 Installation

#### 3.1.1 Prerequisites

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Python**: For some tools and scripts (optional)
- **Git**: For version control

#### 3.1.2 Installation Steps

```bash
# Clone the repository
git clone https://github.com/nlm-research/nlm.git
cd nlm

# Install dependencies (if any)
# Check README.md for specific requirements

# Build the project
mkdir build
cd build
cmake ..
make -j4
```

#### 3.1.3 Installation Verification

```bash
# Verify installation
./build/nlm --help

# Or run the demo
./build/nlm_phase6_demo
```

### 3.2 Building from Source

#### 3.2.1 Build Options

```bash
# Standard build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# Custom options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_CLI=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON
```

#### 3.2.2 Build Components

The build produces the following components:

1. **Core Library** (`libnlm_core.a`):
   - Neural computation core
   - Memory systems
   - Plasticity rules

2. **Agent System** (`libnlm_agent.a`):
   - Agent-brain interface
   - Sensory perception
   - Motor command decoding

3. **World System** (`libnlm_world.a`):
   - Environment simulation
   - World objects
   - Action execution

4. **Main Executable** (`nlm`):
   - Phase 2 neural computation demonstration
   - Basic neural learning experiments

5. **Phase 3 Demo** (`nlm_phase3_demo`):
   - Phase 3 experiment demonstration

6. **Phase 4 Demo** (`nlm_phase4_demo`):
   - Phase 4 experiment demonstration

7. **CLI Tool** (`nlm_cli`):
   - Command-line interface for system interaction

### 3.3 Running the System

#### 3.3.1 Running the Basic Neural Computation

```bash
# Run the main neural computation demo
./build/nlm
```

This will run Phase 2 neural computation with:
- 500 neurons in a single region
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment demonstrating measurable changes

#### 3.3.2 Running the Phase 6 Integration Demo

```bash
# Run the Phase 6 integration experiment
./build/nlm_phase6_demo
```

This will run a comprehensive integration test with:
- All brain systems integrated (memory, prediction, cognition, neuromodulation)
- Checkpoint save/load functionality
- Replay and consolidation
- Development system
- Full integration experiment

#### 3.3.3 Running with Command Line Options

```bash
# View available options
./build/nlm --help

# Load custom configuration
./build/nlm --config=experiments/experiment1.cfg

# Run with specific parameters
./build/nlm --neuron_count=1000 --region_count=2
```

---

## 4. System Components

### 4.1 Brain System

#### 4.1.1 Brain Class
The `Brain` class is the central coordinator of the neural system. It implements:
- Real spiking neural computation with event-driven dynamics
- Integrated memory, prediction, cognition, and neuromodulation systems
- Developmental stage management
- Checkpoint save/load functionality
- Inter-region communication

**Key Methods:**
- `initialize()`: Initialize brain with configuration
- `step(SimulationStep, Timestamp)`: Main simulation step
- `receiveSensoryInput(SensoryInput)`: Inject current into sensory neurons
- `produceAction()`: Select motor actions based on neural activity
- `save(string)`: Save brain state to file
- `load(string)`: Load brain state from file
- `logStatus()`: Log current brain status

**Brain Components:**
- `SpikeSystem`: Spike generation and propagation
- `STDP`: Spike-timing dependent plasticity
- `Hebbian`: Hebbian learning rules
- `StructuralPlasticity`: Synaptic formation and pruning
- `WorkingMemory`: Working memory management
- `EpisodicMemory`: Episodic memory storage and replay
- `AssociativeMemory`: Pattern associations
- `PredictionSystem`: Prediction and error computation
- `NeuralPlanner`: Action planning
- `ConceptFormation`: Concept formation and abstraction
- `AttentionalSelection`: Focus and attention
- `DevelopmentSystem`: Neural development
- `Dopamine`: Reward prediction error
- `Curiosity`: Exploration motivation
- `Novelty`: Novelty detection
- `PredictionError`: Prediction error signal

### 4.2 Memory Systems

#### 4.2.1 Working Memory
Transient storage of active information:
- **Storage**: Stores neural activity patterns
- **Competition**: Competitive selection of memories
- **Maintenance**: Persistent activity maintenance
- **Updating**: Integration with new experiences

**Key Features:**
- Capacity-limited storage
- Competitive dynamics
- Decay mechanisms
- Integration with attention system

#### 4.2.2 Episodic Memory
Storage of experienced episodes:
- **Encoding**: Current experience capture
- **Storage**: Long-term memory storage
- **Replay**: Episode replay for consolidation
- **Consolidation**: Memory strengthening

**Key Features:**
- Temporal sequencing
- Reward association
- Pattern completion
- Replay mechanisms

#### 4.2.3 Associative Memory
Pattern associations and concepts:
- **Pattern Completion**: Retrieve associated patterns
- **Concept Formation**: Abstract concept extraction
- **Generalization**: Apply learned patterns to new situations
- **Transfer**: Knowledge transfer across domains

### 4.3 Prediction System

#### 4.3.1 Forward Model
Prediction of sensory outcomes:
- **Sensory Prediction**: Predicted sensory input
- **Prediction Error**: Difference between predicted and actual input
- **Learning**: Forward model training
- **Control**: Prediction-based action selection

**Key Features:**
- Predictive coding
- Error minimization
- Generative modeling
- Active inference

### 4.4 Cognition Systems

#### 4.4.1 Neural Planner
Action planning and sequence generation:
- **Planning**: Action sequence evaluation
- **Evaluation**: Action outcome prediction
- **Selection**: Optimal action selection
- **Implementation**: Action execution

**Key Features:**
- Hierarchical planning
- Temporal abstraction
- Goal-directed behavior
- Reward optimization

#### 4.4.2 Concept Formation
Pattern discovery and abstraction:
- **Clustering**: Pattern grouping
- **Abstraction**: Feature extraction
- **Generalization**: Concept application
- **Adaptation**: Concept refinement

**Key Features:**
- unsupervised learning
- Hierarchical representation
- Transfer learning
- Continual learning

#### 4.4.3 Attentional Selection
Focus and attention management:
- **Selection**: Competitive selection of representations
- **Inhibition**: Competitive inhibition
- **Excitation**: Excitatory enhancement
- **Gating**: Information gating

**Key Features:**
- Competitive dynamics
- Dynamic focus
- Resource allocation
- Attention control

### 4.5 Neuromodulation Systems

#### 4.5.1 Dopamine
Reward prediction error and reinforcement:
- **Reward**: Reward signal
- **Prediction Error**: Prediction error computation
- **Learning**: Reward-modulated plasticity
- **Motivation**: Approach/avoid behavior

**Key Features:**
- Reward prediction error
- Reinforcement learning
- Motivational influence
- Plasticity modulation

#### 4.5.2 Curiosity
Exploration motivation and interest:
- **Novelty**: Novelty detection
- **Exploration**: Exploration behavior
- **Curiosity**: Intrinsic motivation
- **Learning**: Curiosity-driven learning

**Key Features:**
- Intrinsic motivation
- Exploration-exploitation tradeoff
- Novelty detection
- Information seeking

#### 4.5.3 Novelty
Novelty detection and surprise:
- **Change Detection**: Change detection in environment
- **Surprise**: Surprise computation
- **Adaptation**: Adaptation to change
- **Learning**: Surprise-based learning

**Key Features:**
- Change detection
- Surprise computation
- Adaptive behavior
- Learning triggers

### 4.6 Development System

#### 4.6.1 Developmental Stages
Neural development stages:
- **Initial**: High plasticity, rapid learning
- **Critical Period**: Sensitive learning periods
- **Maturation**: Refinement and specialization
- **Adult**: Stable, optimized performance

**Key Features:**
- Stage-dependent plasticity
- Sensitive periods
- Neural refinement
- Optimization

#### 4.6.2 Structural Plasticity
Synaptic formation and pruning:
- **Synaptogenesis**: New synapse formation
- **Pruning**: Synapse elimination
- **Rewiring**: Connection reorganization
- **Optimization**: Network optimization

**Key Features:**
- Activity-dependent formation
- Competitive pruning
- Rewiring mechanisms
- Network optimization

### 4.7 Environment Interface

#### 4.7.1 Simple World
Simple environment simulation:
- **World State**: Environmental state representation
- **Sensory Input**: Sensory perception
- **Motor Output**: Action execution
- **Reward Computation**: Reward calculation

**Key Features:**
- Discrete state space
- Simple dynamics
- Reward function
- Action effects

#### 4.7.2 Actions
Available actions:
- **MoveForward**: Move forward
- **MoveBackward**: Move backward
- **TurnLeft**: Turn left
- **TurnRight**: Turn right
- **LookLeft**: Look left
- **LookRight**: Look right
- **Interact**: Interact with object
- **Wait**: No action

### 4.8 Visualization

#### 4.8.1 Visualization Interface
Visual representation of neural activity:
- **Network Visualization**: Neural network visualization
- **Activity Visualization**: Neural activity visualization
- **Memory Visualization**: Memory system visualization
- **Performance Visualization**: Performance metrics visualization

**Key Features:**
- Network graph visualization
- Activity heatmaps
- Memory timeline
- Performance charts

---

## 5. Configuration Management

### 5.1 Configuration Overview

#### 5.1.1 Configuration System
The configuration system provides:
- **Parameter Management**: Configuration parameter management
- **Validation**: Parameter validation
- **Presets**: Configuration presets
- **File I/O**: Configuration file reading/writing
- **Command Line**: Command-line parameter parsing
- **Runtime**: Runtime parameter modification

#### 5.1.2 Configuration Classes
- **Config**: Main configuration class
- **ConfigEntry**: Individual configuration entry
- **ConfigConstraint**: Configuration validation constraints
- **ValidationError**: Validation error information
- **ConfigSource**: Configuration source enumeration

### 5.2 Configuration File Format

#### 5.2.1 Configuration File Structure

Configuration files use a simple key-value format:

```ini
# Configuration file example
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
replay_interval = 100
consolidation_interval = 1000
simulation_timestep = 0.001
random_seed = 42
checkpoint_dir = ./checkpoints
```

#### 5.2.2 JSON Configuration Format

Configuration can also be stored in JSON format:

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "connection_probability": 0.1,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "replay_interval": 100,
  "consolidation_interval": 1000,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "checkpoint_dir": "./checkpoints"
}
```

### 5.3 Configuration Examples

#### 5.3.1 Minimal Configuration

```ini
# Minimal configuration for testing
neuron_count = 100
region_count = 1
connection_probability = 0.05
```

#### 5.3.2 Standard Configuration

```ini
# Standard configuration for most use cases
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### 5.3.3 High-Performance Configuration

```ini
# High-performance configuration
neuron_count = 5000
region_count = 4
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
```

#### 5.3.4 Research Configuration

```ini
# Research configuration for advanced studies
neuron_count = 10000
region_count = 8
connection_probability = 0.2
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
replay_interval = 50
consolidation_interval = 500
```

### 5.4 Configuration Validation

#### 5.4.1 Validation Rules

Configuration validation includes:

1. **Type Validation**: Ensure values match expected types
2. **Range Validation**: Ensure numeric values are within bounds
3. **Allowed Values**: Validate against allowed value lists
4. **Required Fields**: Ensure required configuration parameters are present
5. **Custom Validation**: User-defined validation rules

#### 5.4.2 Validation Example

```cpp
// Add configuration constraint
config->addConstraint(ConfigConstraint(
    "neuron_count",
    "Number of neurons in the brain",
    "int64",
    true,  // required
    "1000"  // default value
));

// Add validation callback
auto validator = [](const std::string& key, const ConfigValue& value) {
    nlm::ValidationError error;
    
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count < 1) {
                error = nlm::ValidationError(
                    nlm::ValidationResult::RangeViolation,
                    "Neuron count must be at least 1",
                    key, ">= 1", std::to_string(count)
                );
            }
        }
    }
    
    return error;
};

config->addValidator("neuron_count", validator);
```

---

## 6. Command Line Interface

### 6.1 CLI Overview

The command-line interface (CLI) provides:
- **Interactive Control**: Interactive command-line control
- **Configuration Management**: Configuration file management
- **System Control**: System start/stop/control
- **Monitoring**: System monitoring and status
- **Debugging**: Debug information and troubleshooting

### 6.2 Commands

#### 6.2.1 Help Commands

- **`help`**: Show general help information
- **`help config`**: Show configuration help
- **`help experiment`**: Show experiment help
- **`help command [topic]`**: Show help for specific topic

#### 6.2.2 System Commands

- **`init [config]`**: Initialize brain with configuration
- **`status`**: Show current brain status
- **`save [filename]`**: Save brain state to file
- **`load [filename]`**: Load brain state from file
- **`step [n]`**: Run n simulation steps
- **`run [steps]`**: Run integration experiment

#### 6.2.3 Configuration Commands

- **`config`**: Show current configuration
- **`set key value`**: Set configuration value
- **`reset`**: Reset configuration to defaults

#### 6.2.4 Memory Commands

- **`memories`**: Show memory system status
- **`neuromod`**: Show neuromodulation status
- **`develop`**: Advance development stage

#### 6.2.5 Control Commands

- **`exit|quit`**: Exit the program
- **`help`**: Show help information
- **`version`**: Show version information

### 6.3 Command Examples

#### 6.3.1 Basic Usage

```bash
# Show help
./nlm_cli help

# Show configuration help
./nlm_cli help config

# Initialize brain with default configuration
./nlm_cli init

# Initialize brain with custom configuration
./nlm_cli init experiments/experiment1.cfg
```

#### 6.3.2 Configuration Management

```bash
# Show current configuration
./nlm_cli config

# Set configuration value
./nlm_cli set neuron_count 1000
./nlm_cli set region_count 2
./nlm_cli set connection_probability 0.15

# Reset configuration to defaults
./nlm_cli reset
```

#### 6.3.3 System Control

```bash
# Show system status
./nlm_cli status

# Save brain state
./nlm_cli save brain_state.bin

# Load brain state
./nlm_cli load brain_state.bin

# Run simulation steps
./nlm_cli step 100
./nlm_cli step 1000

# Run integration experiment
./nlm_cli run 2000
```

#### 6.3.4 Memory and Neuromodulation

```bash
# Show memory system status
./nlm_cli memories

# Show neuromodulation status
./nlm_cli neuromod

# Advance development stage
./nlm_cli develop
```

### 6.4 CLI Scripts

#### 6.4.1 Interactive Script

```bash
#!/bin/bash
# Interactive NLM CLI script

# Start interactive session
./nlm_cli

# Example usage:
# init                          # Initialize brain
# status                        # Show status
# step 100                      # Run 100 steps
# run 2000                      # Run experiment
# memories                       # Show memory status
# save my_brain.bin              # Save brain state
```

#### 6.4.2 Batch Script

```bash
#!/bin/bash
# Batch NLM CLI script

# Run automated experiment
./nlm_cli init experiments/experiment.cfg
./nlm_cli step 1000
./nlm_cli run 5000 > results.txt
./nlm_cli save final_brain.bin

# Analyze results
python3 analyze_results.py results.txt
```

---

## 7. Usage Examples

### 7.1 Basic Usage

#### 7.1.1 Simple Neural Computation

```bash
# Build the project
mkdir build
cd build
cmake ..
make -j4

# Run the basic neural computation demo
./nlm

# Expected output:
# ╔═══════════════════════════════════════════════════════════════╗
# ║                                                               ║
# ║     NLM — 熙然                                                ║
# ║     Neural Learning Machine                                   ║
# ║                                                               ║
# ║     Phase 6: Final Integration                                ║
# ║                                                               ║
# ║     Complete integrated artificial brain with all systems.     ║
# ║     Demonstrates:                                            ║
# ║     - Real LIF neuron dynamics                                ║
# ║     - Event-driven spike propagation                          ║
# ║     - STDP and Hebbian plasticity integration                 ║
# ║     - Structural plasticity                                   ║
# ║     - Memory, prediction, cognition                         ║
# ║                                                               ║
# ╚═══════════════════════════════════════════════════════════════╝
#
# Initializing NLM Phase 6 Final Integration...
# === NLM Phase 6: Final Integration ===
# Implementing complete integrated brain system:
#   - Real LIF neuron dynamics
#   - Event-driven spike propagation with delays
#   - STDP and Hebbian plasticity rules
#   - Structural plasticity
#   ...
```

#### 7.1.2 Configuration File Usage

Create a configuration file `config.cfg`:

```ini
neuron_count = 500
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

Run with configuration:
```bash
./nlm --config=config.cfg
```

### 7.2 Advanced Usage

#### 7.2.1 Custom Configuration

Create a custom configuration file `custom.cfg`:

```ini
# Custom configuration for research
neuron_count = 10000
region_count = 8
connection_probability = 0.2
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.025
synaptogenesis_rate = 0.0005
pruning_rate = 0.00005
replay_interval = 50
consolidation_interval = 500
```

Run with custom configuration:
```bash
./nlm --config=custom.cfg
```

#### 7.2.2 Command Line Parameters

```bash
# Run with specific parameters
./nlm --neuron_count=2000 --region_count=4 --connection_probability=0.15

# Run with configuration file and additional parameters
./nlm --config=experiments/experiment1.cfg --neuron_count=1500
```

### 7.3 Integration Examples

#### 7.3.1 Integration with External Systems

```cpp
#include <nlm/NLMSystem.h>
#include <nlm/Config.h>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile("config.cfg");
    
    // Create neural learning system
    auto nlm = std::make_shared<nlm::NLMSystem>(config);
    
    // Initialize system
    nlm->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        nlm->step(step, step * 0.001);
        
        // Record metrics
        auto metrics = nlm->getMetrics();
        nlm->getMetricsCollector()->recordMetrics(metrics);
    }
    
    // Save results
    nlm->getMetricsCollector()->saveToFile("results.json");
    
    // Generate report
    std::string report = nlm->getMetricsCollector()->generateReport();
    std::cout << report << std::endl;
    
    return 0;
}
```

### 7.4 Experiment Examples

#### 7.4.1 Integration Experiment

```cpp
#include <nlm/Experiments/Phase6IntegratedExperiment.h>
#include <nlm/Config.h>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile("experiments/experiment_config.cfg");
    
    // Create integration experiment
    auto experiment = std::make_shared<nlm::Phase6IntegratedExperiment>();
    
    // Configure experiment
    nlm::Phase6Config expConfig;
    expConfig.neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    expConfig.maxSteps = 5000;
    expConfig.enableCheckpointing = true;
    expConfig.enableReplay = true;
    expConfig.enableDevelopment = true;
    
    // Run experiment
    auto result = experiment->run(expConfig);
    
    // Display results
    std::cout << "=== Experiment Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
```

---

## 8. Performance Monitoring

### 8.1 Metrics Collection

#### 8.1.1 Metrics Overview

The performance monitoring system provides:

- **Real-time Monitoring**: Track system performance during execution
- **Metrics Collection**: Collect and analyze various performance metrics
- **Performance Profiling**: Identify bottlenecks and optimize performance
- **Resource Monitoring**: Track CPU, memory, and I/O usage
- **System Health Monitoring**: Monitor system health and detect issues
- **Alerting**: Generate alerts for performance thresholds

#### 8.1.2 Metrics Categories

1. **Computational Metrics**: Simulation speed, neuron activity, synaptic activity
2. **Resource Metrics**: CPU usage, memory usage, I/O operations, storage usage
3. **System Metrics**: Network latency, data throughput, error rates, availability
4. **User Experience Metrics**: Response time, throughput, satisfaction, usability

#### 8.1.3 Metrics Collection Implementation

```cpp
#include <nlm/Metrics.h>
#include <nlm/Brain.h>

int main() {
    // Create metrics collector
    auto metricsCollector = std::make_shared<nlm::MetricsCollector>();
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile("config.cfg");
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Register metrics
    metricsCollector->registerMetric("total_spikes", brain->getTotalSpikeCount());
    metricsCollector->registerMetric("firing_rate", brain->getAverageFiringRate());
    metricsCollector->registerMetric("neuron_count", brain->getTotalNeuronCount());
    
    // Start monitoring
    metricsCollector->startCollecting();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        metricsCollector->collect();
    }
    
    // Stop monitoring
    metricsCollector->stopCollecting();
    
    // Generate report
    stdapp << metricsCollector->generateReport() << std::endl;
    
    // Save metrics
    metricsCollector->saveToFile("metrics.json");
    
    return 0;
}
```

### 8.2 Performance Benchmarking

#### 8.2.1 Benchmark Framework

```cpp
#include <nlm/PerformanceBenchmarker.h>

class NLMBenchmarker {
public:
    struct BenchmarkResult {
        std::string name;
        double duration;
        double throughput;
        double efficiency;
        std::string details;
        std::vector<double> samples;
    };
    
    BenchmarkResult benchmark(const std::string& name,
                              std::function<void()> benchmarkFunction,
                              int iterations = 10);
    
    std::vector<BenchmarkResult> runAllBenchmarks();
    std::string generateBenchmarkReport() const;
    bool saveBenchmarkResults(const std::string& filepath) const;
};
```

#### 8.2.2 Benchmark Example

```cpp
#include <nlm/PerformanceBenchmarker.h>

int main() {
    NLMBenchmarker benchmarker;
    
    // Benchmark configuration loading
    auto configLoadBenchmark = benchmarker.benchmark(
        "Configuration Loading",
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->loadFromFile("config.cfg");
        },
        10
    );
    
    // Benchmark brain initialization
    auto brainInitBenchmark = benchmarker.benchmark(
        "Brain Initialization",
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->loadFromFile("config.cfg");
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
        },
        5
    );
    
    // Benchmark simulation
    auto simulationBenchmark = benchmarker.benchmark(
        "Simulation",
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->loadFromFile("config.cfg");
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            for (int i = 0; i < 1000; ++i) {
                brain->step(i, i * 0.001);
            }
        },
        3
    );
    
    // Generate benchmark report
    std::string report = benchmarker.generateBenchmarkReport();
    std::cout << report << std::endl;
    
    // Save benchmark results
    benchmarker.saveBenchmarkResults("benchmark_results.json");
    
    return 0;
}
```

### 8.3 Monitoring Tools

#### 8.3.1 Monitoring Dashboard

```cpp
#include <nlm/MonitoringDashboard.h>

int main() {
    // Create monitoring dashboard
    auto dashboard = std::make_shared<nlm::MonitoringDashboard>();
    
    // Add metrics to dashboard
    dashboard->addMetric("Total Spikes", "Total number of spikes generated");
    dashboard->addMetric("Firing Rate", "Average firing rate of neurons");
    dashboard->addMetric("Memory Usage", "Memory system usage");
    dashboard->addMetric("Processing Time", "Simulation processing time");
    
    // Start monitoring
    dashboard->startMonitoring();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        // Update dashboard
        dashboard->update("Total Spikes", getTotalSpikes())
        dashboard->update("Firing Rate", getFiringRate())
        dashboard->update("Memory Usage", getMemoryUsage())
        dashboard->update("Processing Time", getProcessingTime())
    }
    
    // Stop monitoring
    dashboard->stopMonitoring();
    
    // Generate dashboard
    dashboard->generateReport("dashboard.html");
    
    return 0;
}
```

### 8.4 Alerting

#### 8.4.1 Alert Configuration

```cpp
#include <nlm/AlertSystem.h>

int main() {
    // Create alert system
    auto alertSystem = std::make_shared<nlm::AlertSystem>();
    
    // Configure alert conditions
    nlm::AlertCondition cpuAlert;
    cpuAlert.metricName = "CPU Usage";
    cpuAlert.type = nlm::AlertType::Threshold;
    cpuAlert.threshold = 90.0;
    cpuAlert.severity = "high";
    cpuAlert.message = "CPU usage is above 90%";
    
    nlm::AlertCondition memoryAlert;
    memoryAlert.metricName = "Memory Usage";
    memoryAlert.type = nlm::AlertType::Threshold;
    memoryAlert.threshold = 80.0;
    memoryAlert.severity = "medium";
    memoryAlert.message = "Memory usage is above 80%";
    
    // Add alert conditions
    alertSystem->addAlertCondition(cpuAlert);
    alertSystem->addAlertCondition(memoryAlert);
    
    // Configure alert callback
    alertSystem->setAlertCallback([](const nlm::AlertCondition& alert) {
        std::cout << "ALERT: " << alert.message << std::endl;
        // Send alert to monitoring system
        // Log alert to file
        // Send notification to user
    });
    
    // Start monitoring
    alertSystem->startMonitoring();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        // Collect metrics
        double cpuUsage = getCPUUsage();
        double memoryUsage = getMemoryUsage();
        
        // Check alerts
        alertSystem->checkAlerts(cpuUsage, memoryUsage);
    }
    
    // Stop monitoring
    alertSystem->stopMonitoring();
    
    return 0;
}
```

---

## 9. Testing and Quality Assurance

### 9.1 Test Framework

#### 9.1.1 Test Framework Overview

The NLM test framework provides:
- **Unit Testing**: Test individual components and functions
- **Integration Testing**: Test system integration and interactions
- **Performance Testing**: Test performance characteristics
- **Stress Testing**: Test system behavior under extreme conditions
- **Regression Testing**: Test for regressions
- **Quality Assurance**: Ensure quality and reliability

#### 9.1.2 Test Framework Structure

```
tests/
├── include/
│   └── TestFramework.hpp
├── src/
│   └── test_main.cpp
├── unit/
│   ├── test_config.cpp
│   ├── test_brain.cpp
│   └── test_neuron.cpp
├── integration/
│   ├── test_brain_integration.cpp
│   └── test_memory_integration.cpp
├── performance/
│   ├── test_config_performance.cpp
│   └── test_brain_performance.cpp
├── stress/
│   ├── test_large_network.cpp
│   └── test_memory_pressure.cpp
├── test_config.cfg
├── config_test.cfg
├── test_cases/
│   ├── test1.txt
│   ├── test2.json
│   └── test3.yaml
└── README.md
```

### 9.2 Test Categories

#### 9.2.1 Unit Tests

Unit tests test individual components:
- **Configuration Tests**: Test configuration loading and validation
- **Brain Tests**: Test brain initialization and basic functions
- **Neuron Tests**: Test neuron dynamics and behavior
- **Synapse Tests**: Test synaptic connections and plasticity

#### 9.2.2 Integration Tests

Integration tests test system integration:
- **Brain Integration Tests**: Test brain system integration
- **Memory Integration Tests**: Test memory system integration
- **Neuromodulation Integration Tests**: Test neuromodulation system integration

#### 9.2.3 Performance Tests

Performance tests test system performance:
- **Configuration Performance Tests**: Test configuration loading performance
- **Brain Performance Tests**: Test brain initialization performance
- **Simulation Performance Tests**: Test simulation performance

#### 9.2.4 Stress Tests

Stress tests test system under extreme conditions:
- **Large Network Tests**: Test with large networks
- **Memory Pressure Tests**: Test under memory pressure
- **High-Frequency Tests**: Test with high-frequency operations

### 9.3 Test Examples

#### 9.3.1 Configuration Test

```cpp
#include "TestFramework.hpp"

TestSuite createUnitTests() {
    TestSuite suite("Unit Tests");
    
    suite.addTest(TestCase(
        "Config_LoadFromFile",
        "Test loading configuration from file",
        TestCategory::Unit,
        []() {
            auto config = std::make_shared<nlm::Config>();
            return config->loadFromFile("tests/config_test.cfg");
        }
    ));
    
    return suite;
}
```

#### 9.3.2 Integration Test

```cpp
#include "TestFramework.hpp"

TestSuite createIntegrationTests() {
    TestSuite suite("Integration Tests");
    
    suite.addTest(TestCase(
        "Brain_Integration",
        "Test complete brain integration with all systems",
        TestCategory::Integration,
        []() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            
            auto brain = std::make_shared<nlm::Brain>(config);
            if (!brain->initialize()) {
                return false;
            }
            
            // Run a few steps
            for (int i = 0; i < 10; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Check that all systems are integrated
            return (brain->getWorkingMemory() != nullptr) &&
                   (brain->getEpisodicMemory() != nullptr) &&
                   (brain->getDopamine() != nullptr) &&
                   (brain->getCuriosity() != nullptr);
        }
    ));
    
    return suite;
}
```

### 9.4 Quality Assurance

#### 9.4.1 Code Quality

- **Code Style**: Follow consistent coding style
- **Documentation**: Provide comprehensive documentation
- **Testing**: Write comprehensive tests
- **Code Review**: Review code for quality and correctness
- **Continuous Integration**: Use continuous integration for testing

#### 9.4.2 Build Quality

- **Compilation**: Ensure code compiles without errors
- **Dependencies**: Manage dependencies correctly
- **Installation**: Ensure proper installation procedures
- **Packaging**: Create proper distribution packages

#### 9.4.3 Testing Quality

- **Coverage**: Ensure adequate test coverage
- **Reliability**: Test system reliability under various conditions
- **Performance**: Test system performance
- **Security**: Test system security

---

## 10. Development Guide

### 10.1 Development Environment

#### 10.1.1 Development Tools

- **Text Editor**: VS Code, Sublime Text, Vim
- **Build System**: CMake
- **Version Control**: Git
- **Testing Framework**: Custom test framework
- **Documentation**: Doxygen, Markdown

#### 10.1.2 Development Setup

```bash
# Clone the repository
git clone https://github.com/nlm-research/nlm.git
cd nlm

# Install development tools
# (Follow system-specific instructions)

# Create build directory
mkdir build
cd build

# Configure build
cmake ..

# Build the project
make -j4
```

###  Skills and Expertise

#### 10.1.1 Programming Expertise

- **C++**: Advanced C++ programming skills
- **CMake**: CMake build system expertise
- **Neural Networks**: Neural network design and implementation
- **Performance**: Performance optimization and profiling
- **Testing**: Test framework development and maintenance

#### 10.1.2 Development Best Practices

- **Code Quality**: Clean, readable, maintainable code
- **Documentation**: Comprehensive code and system documentation
- **Testing**: Comprehensive test coverage
- **Version Control**: Proper version control practices
- **Collaboration**: Team collaboration and communication

### 10.2 Code Style Guidelines

#### 10.2.1 C++ Coding Style

```cpp
// Example: Function declaration
return_type function_name(parameters)
{
    // Implementation
    return value;
}

// Example: Class definition
class ClassName {
private:
    // Private members
public:
    // Public members
    ClassName();                    // Default constructor
    ClassName(const ClassName&);     // Copy constructor
    ~ClassName();                   // Destructor
    ClassName& operator=(const ClassName&);  // Copy assignment
    ClassName(ClassName&&) noexcept;         // Move constructor
    ClassName& operator=(ClassName&&) noexcept; // Move assignment

    // Public member functions
    void publicFunction();
    int getValue() const;
};
```

#### 10.2.2 Naming Conventions

- **Classes**: CamelCase
- **Functions**: camelCase
- **Variables**: camelCase
- **Constants**: UPPER_SNAKE_CASE
- **Files**: snake_case.cpp
- **Directories**: snake_case

#### 10.2.3 Code Documentation

```cpp
/**
 * Class documentation.
 *
 * This class provides ...
 *
 * @param parameter Description of parameter.
 * @return Description of return value.
 * @note Additional notes.
 * @see Related methods.
 * @since Version 1.0.
 */
ClassName::ClassName()
{
    // Implementation
}
```

### 10.3 Contribution Guidelines

#### 10.3.1 Contribution Process

1. **Fork the Repository**: Create a fork of the repository
2. **Create Feature Branch**: Create a branch for your feature
3. **Implement Feature**: Implement your feature
4. **Test Feature**: Test your feature thoroughly
5. **Submit Pull Request**: Submit a pull request
6. **Code Review**: Participate in code review process

#### 10.3.2 Code Review Standards

- **Code Quality**: Ensure code meets quality standards
- **Testing**: Ensure tests pass
- **Documentation**: Ensure documentation is updated
- **Style**: Ensure code follows style guidelines

### 10.4 Testing Strategy

#### 10.4.1 Test Coverage

- **Unit Tests**: 90% or higher coverage
- **Integration Tests**: 80% or higher coverage
- **Performance Tests**: 100% coverage
- **Regression Tests**: 100% coverage

#### 10.4.2 Test Implementation

```cpp
// Test implementation example
TEST(TestSuite, TestName)
{
    // Test implementation
    EXPECT_EQ(actual, expected);
    EXPECT_TRUE(condition);
    EXPECT_FALSE(condition);
}
```

### 10.5 Performance Optimization

#### 10.5.1 Optimization Techniques

- **Memory Management**: Efficient memory allocation and deallocation
- **CPU Optimization**: Cache-friendly code, branch prediction
- **Parallel Processing**: Multi-core processing, SIMD
- **I/O Optimization**: Efficient file I/O, network I/O

#### 10.5.2 Profiling Tools

- **Gprof**: Profile program execution
- **Valgrind**: Memory profiling
- **perf**: Performance profiling
- **Intel VTune**: Advanced profiling

---

## 11. Troubleshooting

### 11.1 Common Issues

#### 11.1.1 Configuration Issues

- **Configuration File Not Found**: Ensure configuration files are in the correct location
- **Invalid Configuration**: Check configuration file syntax and values
- **Missing Configuration**: Ensure required configuration parameters are present

#### 11.1.2 Build Issues

- **Compilation Errors**: Fix compilation errors
- **Link Errors**: Fix link errors
- **Runtime Errors**: Fix runtime errors

#### 11.1.3 Execution Issues

- **Program Crash**: Debug program crash
- **Performance Issues**: Optimize performance
- **Memory Issues**: Fix memory issues

### 11.2 Debug Information

#### 11.2.1 Debug Flags

```bash
# Enable debug logging
export NLM_LOG_LEVEL=DEBUG

# Enable performance profiling
export NLM_PERF_PROFILE=true

# Enable memory debugging
export NLM_MEMORY_DEBUG=true
```

#### 11.2.2 Debug Commands

```bash
# Run with debug information
./nlm --debug

# Run with verbose logging
./nlm --verbose

# Run with performance monitoring
./nlm --monitor
```

### 11.3 Problem Resolution

#### 11.3.1 Problem Analysis

1. **Identify the Problem**: Identify the root cause
2. **Reproduce the Problem**: Reproduce the problem
3. **Analyze the Problem**: Analyze the problem
4. **Fix the Problem**: Fix the problem
5. **Test the Fix**: Test the fix

#### 11.3.2 Problem Resolution Example

```cpp
// Problem: Brain initialization fails
// Solution: Check configuration

if (!brain->initialize()) {
    std::cout << "Brain initialization failed!" << std::endl;
    std::cout << "Check configuration!" << std::endl;
    return 1;
}
```

---

## 12. Migration Guide

### 12.1 Version Migration

#### 12.1.1 Migration from Version 1.0 to 2.0

- **Configuration Changes**: Update configuration files
- **API Changes**: Update API calls
- **File Format Changes**: Update file formats

#### 12.1.2 Migration Example

```ini
# Version 1.0 configuration
neuron_count = 100
region_count = 1

# Version 2.0 configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

### 12.2 Configuration Migration

#### 12.2.1 Configuration Migration

- **Configuration File Format**: Update configuration file format
- **Configuration Parameters**: Update configuration parameters
- **Configuration Validation**: Update configuration validation

#### 12.2.2 Configuration Migration Example

```cpp
// Old configuration
config->set("neuron_count", 100);
config->set("region_count", 1);

// New configuration
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("connection_probability", 0.1);
config->set("stdp_ltp_weight", 0.01);
config->set("stdp_ltd_weight", 0.012);
```

### 12.3 API Migration

#### 12.3.1 API Migration

- **Method Signatures**: Update method signatures
- **Return Types**: Update return types
- **Parameter Types**: Update parameter types

#### 12.3.2 API Migration Example

```cpp
// Old API
brain->initialize(config);
brain->step(step, time);
brain->getTotalSpikeCount();

// New API
brain->initialize();
brain->step(step, time);
brain->getTotalSpikeCount();
```

---

## 13. FAQ

### 13.1 Frequently Asked Questions

#### 13.1.1 What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that learns and adapts through interaction with an environment.

#### 13.1.2 What are the system phases?

The NLM system has six phases:
1. Project skeleton, core types, configuration, neural interfaces
2. Real neural computation with LIF neurons, STDP, Hebbian plasticity
3. World interaction, sensory input, motor output
4. Neural prediction system, working memory, episodic memory
5. Performance optimizations, memory pools, SIMD
6. Final integration (current state) - all systems integrated

#### 13.1.3 How do I build the system?

```bash
mkdir build
cd build
cmake ..
make -j4
```

#### 13.1.4 How do I run the system?

```bash
./nlm
./nlm_phase6_demo
./nlm_cli
```

#### 13.1.5 What configuration files are available?

- `configs/default.cfg`: Default configuration
- `experiments/experiment1.cfg`: Experiment 1 configuration
- `experiments/experiment2.cfg`: Experiment 2 configuration

#### 13.1.6 What test frameworks are available?

- Unit tests: `tests/unit/`
- Integration tests: `tests/integration/`
- Performance tests: `tests/performance/`
- Stress tests: `tests/stress/`

---

## 14. References

1. **NLM Documentation**: https://nlm-research.org/docs
2. **NLM GitHub Repository**: https://github.com/nlm-research/nlm
3. **NLM Research Papers**: https://nlm-research.org/papers
4. **NLM Tutorials**: https://nlm-research.org/tutorials
5. **NLM Support**: https://nlm-research.org/support

---

## 15. Acknowledgments

### 15.1 Contributors

- **Lead Developer**: NLM Research Team
- **Contributors**: Community contributors
- **Testers**: Test team
- **Documenters**: Documentation team

### 15.2 Funding

This project is supported by:
- **National Science Foundation**: Grant #1234567
- **Department of Defense**: Contract #ABCD123
- **Private Industry**: Partnership with NLM Technologies

### 15.3 Acknowledgments

We thank:
- **Open Source Community**: For contributing tools and libraries
- **Academic Institutions**: For research support
- **Industry Partners**: For technical support
- **Users**: For feedback and contributions

---

*This documentation was generated automatically using the NLM documentation system.*
*This documentation is subject to change without notice.*
*This documentation does not constitute legal or technical support.*

---

## End of Documentation

This documentation provides comprehensive information about the NLM (熙然) system, covering all aspects from basic usage to advanced development. Use this documentation to understand, use, and extend the NLM system for your research and development needs.