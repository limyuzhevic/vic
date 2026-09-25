# NLM (熙然) API Documentation
# Neural Learning Machine - Phase 6: Integrated Artificial Brain

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

This document provides comprehensive API reference for NLM's neural components, integration systems, and advanced features.

## Core Architecture

NLM implements a fully integrated artificial brain with the following components:

### 1. Neural Core (brain/)

#### Brain Class (Brain.hpp, Brain.cpp)
Primary neural substrate implementing integrated artificial intelligence.

**Key Methods:**
- `Brain(std::shared_ptr<Config> config)` - Constructor with configuration
- `initialize()` - Initialize neural network with configuration parameters
- `step(SimulationStep step, Timestamp time)` - Execute one simulation timestep
- `logStatus() const` - Report comprehensive system status
- `receiveSensoryInput(const SensoryInput& input)` - Process sensory data
- `produceAction()` - Convert neural activity to behavioral actions
- `reset()` - Reset brain state for new episodes

**Important Properties:**
- `getTotalNeuronCount()` - Total neurons in network
- `getTotalSynapseCount()` - Total synaptic connections
- `getFiringNeuronCount()` - Neurons firing in current timestep
- `getAverageFiringRate()` - Network-wide firing rate
- `getExcitationInhibitionRatio()` - E/I balance metric

### 2. Integrated Systems

#### Memory Systems
- **Working Memory**: Short-term memory with competitive encoding
- **Episodic Memory**: Experience recording and replay capability
- **Associative Memory**: Pattern association through Hebbian learning

#### Neuromodulation
- **Dopamine**: Reward prediction error and motivation
- **Curiosity**: Exploration drive and intrinsic motivation
- **Novelty**: Novelty detection and surprise processing
- **Prediction Error**: Mismatch detection and learning signals

#### Cognition
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Abstract pattern extraction and categorization
- **Attentional Selection**: Competitive attention allocation

#### Development
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult
- **Structural Plasticity**: Synaptogenesis and pruning dynamics
- **Maturation**: Network stabilization and efficiency optimization

### 3. Sensory-Motor Interface (agent/)

#### AgentBrain Class (AgentBrain.hpp, AgentBrain.cpp)
Connects NLM brain to external world through sensory transduction and motor decoding.

**Key Methods:**
- `processSensoryInput(const SensoryPercept& percept)` - Convert world observations to neural input
- `decodeMotorCommand()` - Transform neural activity into actionable behaviors
- `applyRewardModulation(float reward, float predictedReward)` - Apply reward-based learning

### 4. Configuration Management (core/Config/)

#### Config Class (Config.hpp, Config.cpp)
Advanced configuration system supporting multiple sources and validation.

**Key Features:**
- **Multiple sources**: File, command-line, runtime, default
- **Type-safe**: Support for int, double, string, bool, and collections
- **Validation**: Comprehensive parameter range checking
- **Convenience methods**: `getOr()`, `has()`, `set()`

**Common Configuration Parameters:**
```
random_seed: uint64_t          # For reproducible experiments
simulation_timestep: double    # Neural simulation step (seconds)
neuron_count: size_t           # Network size
region_count: size_t           # Modular brain regions
connection_probability: float  # Synaptic connectivity density
stdp_ltp_weight: float         # Long-term potentiation strength
stdp_ltd_weight: float         # Long-term depression strength
stdp_tau: float               # STDP eligibility trace time constant
synaptogenesis_rate: float    # Development synaptogenesis
pruning_rate: float           # Development pruning
```

### 5. Performance Monitoring and Benchmarking

#### Benchmark Framework (benchmark.cpp)
Comprehensive performance measurement for neural networks at scale.

**Benchmark Features:**
- Multiple network sizes (100 to 2000 neurons)
- Detailed performance metrics (steps/sec, spikes/step, memory usage)
- Formatted output with comparison tables
- Configurable benchmark suites

#### Performance Metrics (Phase4Experiment.cpp, Phase5Experiment.cpp)
Extensive experimental result tracking for neural development and learning.

**Metrics Categories:**
- **Learning Metrics**: Prediction accuracy, reward accumulation, learning efficiency
- **Memory Metrics**: Working memory capacity, episodic recall, associative strength
- **Cognitive Metrics**: Concept formation, planning accuracy, attention selectivity
- **Social Metrics**: Imitation accuracy, social prediction, theory of mind

## Integration Patterns

### 1. Basic Neural Network Setup

```cpp
// Create configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);
config->set("connection_probability", 0.1f);

// Initialize brain
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Run simulation for 1000 steps
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step, step * 0.001);
}
```

### 2. Sensory Integration with AgentBrain

```cpp
// Create agent with brain
auto agentBrain = std::make_shared<AgentBrain>(brain);

// Initialize with environment
SimpleWorld world(10, 10);
agentBrain->initialize(world);

// Process sensory input and get actions
while (simulation_running) {
    // Get sensory percept from environment
    SensoryPercept percept = world.getPercept();
    
    // Process input through agent brain
    agentBrain->processSensoryInput(percept);
    
    // Get motor command
    MotorCommand action = agentBrain->decodeMotorCommand();
    
    // Execute action in environment
    world.executeAction(action);
}
```

### 3. Developmental Learning

```cpp
// Enable developmental learning
agentBrain->enableDevelopment(true);
agentBrain->enableStructuralPlasticity(true);
agentBrain->enableCuriosity(true);

// Run developmental simulation
for (double age = 0.0; age < 300.0; age += 10.0) {
    // Apply age-dependent development
    agentBrain->updateDevelopment(10.0);
    
    // Learn from experience
    brain->step(current_step, current_time);
    
    // Occasionally save developmental state
    if (age > 0 && std::fmod(age, 50.0) < 10.0) {
        std::string filename = "checkpoint_age_" + std::to_string((int)age) + ".nlm";
        brain->save(filename);
    }
}
```

### 4. Advanced Configuration with Command-Line Interface

```bash
// Basic usage
./nlm --config=configs/production.cfg

// Custom parameters via command line
./nlm \
  --config=configs/default.cfg \
  --neuron_count=2000 \
  --region_count=4 \
  --timestep=0.002 \
  --log_level=Info \
  --seed=12345

// Enable specific features
./nlm \
  --plasticity=enable \
  --speed=2.0 \
  --checkpoint=./checkpoints \
  --log_colors=true
```

## Advanced Features

### 1. Experimental Mode
NLM provides comprehensive experimental capabilities for studying neural development and learning.

**Available Experiments:**
- **Phase 3**: Basic neural computation demonstration
- **Phase 4**: Cognitive capabilities testing
- **Phase 5**: Lifetime developmental learning
- **Phase 6**: Full integration testing

**Experimental Features:**
- Customizable environments and tasks
- Detailed metrics collection and analysis
- Statistical comparison across conditions
- Automated experiment pipelines

### 2. Configuration Management
Advanced configuration system with validation, defaults, and override capabilities.

**Configuration Sources (highest priority to lowest):**
1. Command-line arguments
2. Configuration file
3. Runtime overrides
4. Built-in defaults

**Configuration Validation:**
- Parameter range checking
- Inter-parameter consistency validation
- Resource requirement analysis
- Performance impact estimation

### 3. Performance Monitoring
Real-time monitoring and benchmarking for neural network optimization.

**Monitoring Features:**
- Memory usage tracking
- Computational complexity analysis
- Spike rate monitoring
- Learning progress tracking
- Network stability metrics

### 4. Logging and Diagnostics
Comprehensive logging system with multiple levels and output formats.

**Log Levels:**
- DEBUG: Detailed debugging information
- INFO: General system information
- WARNING: Potential issues or suboptimal configurations
- ERROR: Actual errors or failures
- CRITICAL: Critical system failures

**Log Output Options:**
- Console output with colors
- File logging with rotation
- Structured logging formats
- Real-time filtering and routing

## API Reference Summary

### Core Classes
| Class | Purpose | Key Methods |
|-------|---------|-------------|
| Brain | Neural substrate | initialize(), step(), logStatus() |
| AgentBrain | World interface | processSensoryInput(), decodeMotorCommand() |
| Config | Configuration management | getOr(), set(), validate() |
| RandomGenerator | Deterministic randomness | uniformReal(), uniformInt(), normal() |

### Memory Systems
| Class | Function | Capacity |
|-------|---------|----------|
| WorkingMemory | Short-term storage | Configurable |
| EpisodicMemory | Experience recording | 1000+ episodes |
| AssociativeMemory | Pattern association | Scale with network |

### Neuromodulation
| Signal | Function | Role |
|--------|---------|------|
| Dopamine | Reward prediction error | Motivation, learning |
| Curiosity | Intrinsic motivation | Exploration |
| Novelty | Surprise detection | Adaptability |
| Prediction Error | Mismatch detection | Learning correction |

## Migration Guide

### From Earlier Phases
**Phase 3 → Phase 6 Migration:**
- Replace simple neuron models with LIF dynamics
- Add integrated memory systems
- Implement neuromodulation signals
- Include developmental stages
- Add attention and planning systems

**Key Changes:**
- LIF neurons replace threshold neurons
- Spike-based plasticity with STDP and Hebbian rules
- Working/episodic memory for experience tracking
- Developmental trajectory with plasticity modulation

### Configuration Updates
**Phase 1 → Phase 6 Configuration:**
- New required parameters (e.g., `replay_interval`, `consolidation_interval`)
- Additional optional parameters for advanced features
- Updated validation rules for neural constraints
- Performance considerations for large networks

## Best Practices

### 1. Configuration Management
```cpp
// Use validation before initialization
bool validateConfig(const Config& config) {
    if (!config.has("neuron_count")) return false;
    if (config.getOr<size_t>("neuron_count", 0) == 0) return false;
    // Additional validation...
    return true;
}

// Apply configuration safely
if (validateConfig(config)) {
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
} else {
    std::cerr << "Invalid configuration!" << std::endl;
    return 1;
}
```

### 2. Error Handling
```cpp
// Use brain status checking
if (!brain->getRandomGenerator()) {
    std::cerr << "Random generator not available!" << std::endl;
    return 1;
}

// Check system resources
if (brain->getTotalNeuronCount() > MAX_NEURONS) {
    std::cerr << "Network too large for available memory!" << std::endl;
    return 1;
}
```

### 3. Performance Optimization
```cpp
// Configure for performance
config->set("simulation_timestep", 0.001);  // Accurate but slower
// OR
config->set("simulation_timestep", 0.01);   // Faster but less accurate

// Monitor performance during initialization
brain->logStatus();  // Shows resource usage
```

## Common Issues and Solutions

### 1. Null Pointer Dereference
**Problem**: Random generator or brain pointers are null
**Solution**: Check pointer validity before use:
```cpp
if (brain_ && brain_->getRandomGenerator()) {
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
}
```

### 2. Configuration Validation Errors
**Problem**: Invalid configuration parameters
**Solution**: Validate configuration before use:
```cpp
if (!config->has("required_parameter")) {
    std::cerr << "Missing required configuration!" << std::endl;
    return 1;
}
```

### 3. Performance Issues
**Problem**: Slow simulation for large networks
**Solution**: Adjust configuration for performance:
```cpp
// For very large networks
config->set("simulation_timestep", 0.01);  // Larger time steps
config->set("connection_probability", 0.05);  // Fewer synapses
```

## Conclusion

NLM's Phase 6 integration provides a complete, production-ready artificial brain implementation with comprehensive monitoring, advanced configuration, and experimental capabilities. The API supports everything from simple demonstrations to large-scale neural network research.

Key innovations include:
- Fully integrated memory systems for experience tracking
- Neuromodulation for adaptive behavior
- Developmental learning trajectories
- Comprehensive performance monitoring and benchmarking
- Advanced configuration management with validation

NLM serves as both a functional AI system and a flexible platform for neural research and experimentation.
