NLM Documentation
================

This document provides comprehensive information about using the NLM (Neural Learning Machine) framework for Phase 2: Real Neural Computation.

## Overview

NLM is an experimental artificial developmental brain project that simulates:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

## Key Components

### 1. Core Neural Components

#### Neurons (Neuron class)
- **Type**: LIF (Leaky Integrate-and-Fire) neurons
- **Features**:
  - Real membrane potential dynamics
  - Refractory periods
  - Spike-frequency adaptation
  - Synaptic input integration
  - Plasticity state management

#### Synapses (Synapse class)
- **Type**: Event-driven synaptic transmission
- **Features**:
  - Synaptic delays
  - Short-term plasticity (STP)
  - Eligibility traces for reward-modulated learning
  - Weight bounds and clamping
  - Spike history recording

#### Brain (Brain class)
- **Type**: Neural system coordinator
- **Features**:
  - Integration of all neural components
  - Multiple plasticity systems (STDP, Hebbian, structural)
  - Neuromodulation systems (dopamine, curiosity, novelty)
  - Memory systems (working, episodic, semantic)
  - Prediction and cognition systems
  - Development system

### 2. Plasticity Systems

#### STDP (Spike-Timing-Dependent Plasticity)
- **Mechanism**: Pre-before-post spike pairs potentiate synapses (LTP)
- **Formula**: Δw = A⁺ × exp(-Δt/τ⁺) for Δt > 0
- **Formula**: Δw = -A⁻ × exp(Δt/τ⁻) for Δt < 0
- **Parameters**:
  - ltp_weight: Potentiation amplitude (default: 0.02)
  - ltd_weight: Depression amplitude (default: 0.015)
  - timeConstant: Time window (default: 20.0 ms)

#### Hebbian Plasticity
- **Mechanism**: Correlation-based learning
- **Formula**: Δw = η × (coactivity - baseline)
- **Features**:
  - Covariance-based rule
  - Threshold to prevent runaway potentiation
  - Activity-dependent scaling

#### Structural Plasticity
- **Mechanism**: Synaptogenesis and pruning
- **Features**:
  - Activity-dependent synapse creation
  - Weight-based synapse elimination
  - Maximum synapse limits per neuron
  - Global synapse budget

### 3. Neuromodulation Systems

#### Dopamine
- **Function**: Reward prediction and reinforcement learning
- **Effects**:
  - Modulates synaptic plasticity
  - Encodes prediction errors
  - Regulates exploration vs. exploitation

#### Curiosity
- **Function**: Novelty detection and exploration motivation
- **Formula**: Level = wₙ×novelty + wₚ×prediction_error
- **Purpose**: Drive exploration through intrinsic motivation

#### Novelty Detection
- **Function**: Detects novel sensory inputs
- **Mechanism**: Pattern comparison with history
- **Application**: Drives curiosity and exploration

#### Prediction Error
- **Function**: Compares predictions with actual sensory input
- **Application**: Improves forward models and prediction accuracy

### 4. Memory Systems

#### Working Memory
- **Function**: Temporary active information storage
- **Characteristics**:
  - Limited capacity (approximately 7±2 items)
  - Rapid decay without rehearsal
  - Integration with attention

#### Episodic Memory
- **Function**: Experience encoding and replay
- **Features**:
  - Temporal ordering of events
  - Pattern completion during recall
  - Consolidation during sleep/rest cycles

#### Semantic Memory
- **Function**: Conceptual knowledge and patterns
- **Features**:
  - Distributed representation
  - Similarity-based retrieval
  - Continuous learning from experience

### 5. Development System

#### Developmental Stages
- **Initial**: High plasticity, basic connectivity
- **Critical Period**: Activity-dependent refinement
- **Maturation**: Stabilization and optimization
- **Adult**: Limited plasticity, expert performance
- **Aging**: Gradual decline of performance

#### Developmental Mechanisms
- **Synaptogenesis**: Creation of new synapses
- **Pruning**: Elimination of weak connections
- **Maturation**: Refinement of synaptic strengths
- **Experience-dependent**: Plasticity modulated by activity

### 6. Prediction and Cognition Systems

#### Prediction System
- **Function**: Forward models and prediction
- **Applications**:
  - Action planning
  - Error detection
  - Anticipation of sensory input

#### Neural Planner
- **Function**: Action sequence evaluation
- **Features**:
  - Value-based planning
  - Hierarchical action selection
  - Resource allocation

#### Concept Formation
- **Function**: Pattern discovery and categorization
- **Applications**:
  - Object recognition
  - Scene understanding
  - Abstract reasoning

### 7. Performance Optimizations

#### SIMD Vectorization
- **Implementation**: Efficient vector operations
- **Benefits**:
  - 4-10x speedup for neural computations
  - Reduced memory bandwidth usage
  - Better cache utilization

#### Memory Pools
- **Implementation**: Pre-allocated memory blocks
- **Benefits**:
  - Reduced memory fragmentation
  - Faster memory allocation
  - Better cache locality

#### Parallel Processing
- **Implementation**: Multi-core support
- **Benefits**:
  - Scalable to many CPU cores
  - Improved simulation throughput
  - Better resource utilization

#### Checkpointing
- **Implementation**: State persistence
- **Features**:
  - Periodic saving
  - Resume capability
  - Fault tolerance

## Configuration System

### Configuration File Format

NLM uses a key=value configuration format with support for:

```
# Comment
key = value
# Nested keys
section.key = value
# Boolean values
flag = true/false
# Numeric values
number = 42
float = 3.14
# String values
string = "example"
```

### Default Configuration Parameters

#### Brain Configuration
- `brain.neuron_count`: Total number of neurons (default: 1000)
- `brain.synapse_density`: Synapse connectivity density (default: 0.1)
- `brain.connection_probability`: Connection probability (default: 0.05)
- `brain.initial_weight_mean`: Initial synaptic weight mean (default: 0.5)
- `brain.initial_weight_std`: Initial synaptic weight standard deviation (default: 0.1)

#### Neuron Parameters
- `brain.v_thresh`: Neuron threshold potential (default: -50.0 mV)
- `brain.v_rest`: Resting potential (default: -70.0 mV)
- `brain.v_reset`: Reset potential (default: -75.0 mV)
- `brain.tau_mem`: Membrane time constant (default: 20.0 ms)
- `brain.tau_ref`: Refractory period (default: 2.0 ms)

#### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP (default: true)
- `plasticity.stdp.learning_rate`: STDP learning rate (default: 0.001)
- `plasticity.stdp.tau_plus`: STDP time constant for LTP (default: 20.0 ms)
- `plasticity.stdp.tau_minus`: STDP time constant for LTD (default: 20.0 ms)
- `plasticity.hebbian.enable`: Enable Hebbian learning (default: true)
- `plasticity.structural.enable`: Enable structural plasticity (default: true)

#### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine modulation scale (default: 1.0)
- `neuromod.curiosity.enable`: Enable curiosity (default: true)
- `neuromod.novelty.enable`: Enable novelty detection (default: true)

### Advanced Configuration

For more advanced usage, see:
- `configs/advanced.cfg` - Advanced configuration
- `configs/advanced_detailed.cfg` - Detailed configuration
- `nlm.sh` - Command line interface

## Programming Interface

### C++ API

The NLM framework provides a comprehensive C++ API for:

#### Basic Usage

```cpp
#include "brain/Brain.hpp"

// Create configuration
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("plasticity.stdp.enable", true);

// Create brain
auto brain = std::make_shared<Brain>(config);

// Initialize
if (brain->initialize()) {
    // Configure for simulation
    // Run simulation loop
}
```

#### Complete Agent Example

```cpp
// Create brain
config = pynlm.createDefaultConfig();
brain = pynlm.createBrain(config);
brain.initialize();

// Create agent brain interface
agent = pynlm.createAgentBrain(brain);

// Create world
world = pynlm.createSimpleWorld();
world.configure(width=20, height=20, visionWidth=8, visionHeight=8);
world.reset();

// Initialize agent with world
agent.initialize(world);

// Enable learning features
agent.enableRewardModulation(true);
agent.enableCuriosity(true);
agent.enableDevelopment(true);

// Run simulation loop
for (step = 0; step < 1000; ++step) {
    world.update(0.1);
    
    percept = world.getSensoryPercept();
    agent.processSensoryInput(percept);
    
    brain.step(step);
    
    action = agent.decodeMotorCommand();
    world.applyMotorCommand(action, world.getSimulationTime());
    
    reward = world.getSensoryPercept().getInternal()[0];
    agent.applyRewardModulation(reward, 0.0);
    
    agent.updateDevelopment(0.1);
}
```

### Python API

The NLM framework provides Python bindings for easier scripting:

#### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Complete Agent Example

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
```

## Performance Considerations

### Memory Usage
- **Default Configuration**: ~100 neurons, ~1,000 synapses
- **Large Configuration**: ~2,000 neurons, ~20,000 synapses
- **Memory Pools**: Reduces memory fragmentation
- **SIMD Vectorization**: Improves cache efficiency

### Computational Complexity
- **Neuron Updates**: O(n) per simulation step
- **Synaptic Transmission**: O(m) where m is number of synapses
- **Plasticity Updates**: O(k) where k is number of active synapses
- **Memory Operations**: O(1) with memory pools

### Scalability
- **Multi-core Support**: Parallel processing for large networks
- **Distributed Computing**: Potential for distributed simulations
- **Hardware Acceleration**: SIMD and GPU support

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain initialization failed"
- **Cause**: Invalid configuration parameters
- **Solution**: Check configuration values and file paths
- **Debug**: Enable verbose logging

#### 2. "Simulation too slow"
- **Cause**: Inefficient implementation or small timestep
- **Solution**:
  - Increase simulation timestep
  - Enable SIMD optimization
  - Use memory pools
  - Reduce network size

#### 3. "Memory allocation failed"
- **Cause**: Out of memory
- **Solution**:
  - Increase system memory
  - Use memory pools
  - Reduce network size
  - Enable memory compression

#### 4. "Plasticity not working"
- **Cause**: Plasticity rules not enabled
- **Solution**:
  - Check plasticity configuration
  - Enable plasticity rules in config
  - Verify spike activity
  - Check neuron membrane potentials

### Error Messages and Meanings

- **"Failed to initialize brain"**: Brain configuration invalid
- **"No neurons found"**: Neuron count too low or region issues
- **"Memory allocation failed"**: Insufficient memory
- **"File not found"**: Configuration file missing
- **"Invalid parameter value"**: Configuration parameter invalid

### Debugging Tips

1. **Enable verbose logging**:
   - Add "verbose = true" to configuration
   - Use command line `-v` flag

2. **Check system resources**:
   - Monitor memory usage
   - Check CPU temperature
   - Verify disk space

3. **Validate configuration**:
   - Use "nlm --config test.cfg start" to test
   - Check configuration file syntax

4. **Use debug mode**:
   - Enable debug mode in configuration
   - Use "nlm debug" command

## Advanced Usage Examples

### 1. Custom Neural Architecture

```cpp
// Create custom network with specific connectivity
config.set("brain.neuron_count", 2000);
config.set("brain.synapse_density", 0.2);
config.set("brain.connection_probability", 0.15);
config.set("network.layer_sizes", "500,300,200");
```

### 2. Learning Rate Scheduling

```cpp
// Dynamic learning rate adjustment
config.set("learning.rate_schedule", "exponential");
config.set("learning.initial_rate", 0.01);
config.set("learning.decay_rate", 0.001);
config.set("learning.min_rate", 0.0001);
```

### 3. Neuromodulation Tuning

```cpp
// Custom neuromodulation parameters
config.set("neuromod.dopamine.baseline", 0.1);
config.set("neuromod.dopamine.peak", 2.0);
config.set("neuromod.curiosity.weight_novelty", 0.7);
config.set("neuromod.curiosity.weight_prediction_error", 0.3);
```

### 4. Development Scheduling

```cpp
// Custom developmental stages
config.set("development.stages.initial.duration", 30);
config.set("development.stages.critical_period.duration", 150);
config.set("development.stages.maturation.duration", 400);
config.set("development.stage_plasticity.initial", 1.0);
config.set("development.stage_plasticity.critical_period", 0.8);
```

## Future Enhancements

### Planned Features

1. **Multi-region Connectivity**
   - Long-range connections between regions
   - Top-down and bottom-up processing
   - Hierarchical processing

2. **Advanced Plasticity Rules**
   - Spike-timing dependent plasticity
   - Reward-modulated plasticity
   - Homeostatic plasticity
   - Metaplasticity

3. **Enhanced Neuromodulation**
   - Additional neuromodulators (acetylcholine, norepinephrine)
   - Complex interactions between neuromodulators
   - Real-time modulation

4. **Improved Memory Systems**
   - Working memory with active maintenance
   - Episodic memory with temporal sequencing
   - Semantic memory with embeddings
   - Procedural memory with reinforcement learning

5. **Advanced Cognition**
   - Theory of mind
   - Causal reasoning
   - Counterfactual thinking
   - Abstract planning

6. **Hardware Acceleration**
   - GPU implementation
   - Neuromorphic hardware support
   - Event-driven simulation

## Conclusion

The NLM framework provides a comprehensive platform for simulating artificial neural systems with:

- **Realistic neuron dynamics** with LIF models
- **Comprehensive plasticity** with multiple rules
- **Integrated neuromodulation** for adaptive behavior
- **Sophisticated memory** systems for information storage
- **Advanced cognition** for intelligent behavior
- **Performance optimizations** for efficient computation

This documentation provides the foundation for using and extending the NLM framework for research and applications in computational neuroscience and artificial intelligence.