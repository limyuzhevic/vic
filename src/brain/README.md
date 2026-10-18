# Brain Module

## Overview

The Brain module contains the core neural computation components that implement the biological brain simulation. It includes neurons, synapses, neural populations, and the overall brain integration system.

## Components

### 1. Neuron (`src/brain/Neuron/`)

**Leaky Integrate-and-Fire (LIF) Model**

Implements the fundamental neuron dynamics:
- **Membrane potential dynamics** with leak current
- **Spike generation** when threshold is reached
- **Refractory period** after spiking
- **Synaptic integration** from incoming connections
- **Adaptation mechanisms** for firing rate regulation

**Key Features:**
- High-performance implementation with C++20
- Event-driven spike processing
- Multiple neuron types (Sensory, Motor, Internal)
- State management with structured NeuronState

**Neuron Types:**
- `NeuronType::Sensory`: Receives external input
- `NeuronType::Motor`: Produces output actions
- `NeuronType::Internal`: Internal brain processing

### 2. Synapse (`src/brain/Synapse/`)

**Connection Model**

Represents synaptic connections between neurons:
- **Weighted connections** with dynamic strength
- **Delay-based transmission** for spike propagation
- **Plasticity mechanisms** (Hebbian and STDP)
- **Directionality** (pre- -> post-synaptic)
- **Enable/disable plasticity** per synapse

**Plasticity Types:**
- **Hebbian Learning**: Co-activation strengthens connections
- **Spike-Timing-Dependent Plasticity (STDP)**: Precise spike timing matters
- **Structural Plasticity**: Formation and elimination of connections

### 3. Neural Population (`src/brain/NeuralPopulation/`)

**Group Management**

Manages groups of similar neurons:
- **Efficient storage** using contiguous memory
- **Population-based operations** for batch processing
- **Type-specific optimizations** (sensory, motor, internal)
- **Activity monitoring** and statistics collection

### 4. Neural Region (`src/brain/NeuralRegion/`)

**Substrate Organization**

Organizes neurons into functional regions:
- **Hierarchical structure** (World → Brain → Regions)
- **Local connectivity** within regions
- **Region-specific dynamics** and parameters
- **Boundary management** between regions

### 5. Brain (`src/brain/Brain/`)

**Integration System**

The central brain simulation orchestrator:
- **Main event loop** for neural computation
- **Spike propagation** with delays and priorities
- **Memory system integration** (working, episodic, semantic)
- **Neuromodulation coordination**
- **Development system management**
- **Checkpoint/save-load capabilities**
- **Performance monitoring** and profiling

## Neural Dynamics

### LIF Equation Implementation

The neuron model follows:
```
dV/dt = (I_leak + I_syn - I_adapt) / C_m
I_leak = g_leak * (V - E_leak)
I_adapt = w_adapt * (V - V_adapt)
```

### Key Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| membrane_capacitance | float | 1.0 nF | Membrane capacitance |
| time_constant | float | 20.0 ms | Membrane time constant |
| resting_potential | float | -70.0 mV | Resting membrane potential |
| threshold | float | -55.0 mV | Firing threshold |
| reset_potential | float | -70.0 mV | Post-spike reset value |
| leak_conductance | float | 10.0 nS | Leak conductance |

### Spike Propagation

1. **Pre-synaptic neuron** reaches threshold
2. **Spike event** generated with precise timing
3. **Synaptic delay** based on connection strength
4. **Post-synaptic neuron** receives current injection
5. **Membrane potential** updated based on synaptic input

## Integration with Other Modules

### Memory Systems

- **Working Memory**: Persistent activity through recurrent connections
- **Episodic Memory**: Experience encoding and replay
- **Semantic Memory**: Pattern associations and concept formation
- **Procedural Memory**: Motor skill learning and refinement

### Neuromodulation

- **Dopamine**: Reward prediction error and learning modulation
- **Curiosity**: Novelty detection and exploration drive
- **Novelty**: Unexpected event detection
- **Prediction Error**: Discrepancy between expected and actual

### Development System

- **Synaptogenesis**: Formation of new connections based on activity
- **Pruning**: Elimination of unused connections
- **Maturation**: Age-dependent plasticity rate changes
- **Critical Periods**: Time windows for specific learning

### Prediction Systems

- **Forward Models**: Predicting sensory consequences of actions
- **Inverse Models**: Planning motor commands for goals
- **Confidence Estimation**: Uncertainty in predictions

## Performance Optimizations

### Memory Management

- **Memory pools** for frequently allocated objects
- **Contiguous storage** for neural populations
- **Cache-friendly layouts** for data locality
- **Pre-allocated buffers** for spike processing

### Computational Efficiency

- **Event-driven processing** instead of time-stepping
- **Vectorized operations** using SIMD instructions
- **Parallel processing** for population operations
- **Spiky compression** for sparse connectivity

### Memory Hierarchy Optimization

- **L1 cache**: Frequently accessed neuron states
- **L2 cache**: Larger neural population data
- **Main memory**: Large-scale connectivity matrices
- **NUMA awareness** for multi-core systems

## Development Guidelines

### Code Quality Standards

1. **Naming Conventions**:
   - Snake_case for functions and variables
   - CamelCase for classes and types
   - UPPER_SNAKE_CASE for constants

2. **Memory Safety**:
   - Use smart pointers for heap allocation
   - RAII for resource management
   - Avoid raw pointers where possible

3. **Performance**:
   - Prefer const references for large objects
   - Use value categories appropriately
   - Minimize dynamic allocations in hot paths

### Testing Recommendations

1. **Unit Tests**:
   - Individual neuron dynamics validation
   - Synapse plasticity rule testing
   - Memory system integration tests

2. **Integration Tests**:
   - Full brain loop verification
   - Memory-neuromodulation interactions
   - Development system coordination

3. **Performance Tests**:
   - Scaling analysis with increasing neuron counts
   - Memory usage profiling
   - Spike processing throughput

## Next Steps

- Implement advanced neuron models (HH, Izhikevich)
- Add dendritic computation capabilities
- Implement neuromorphic hardware targeting
- Add learning rule variations

## Related Documentation

- [Architecture Deep Dive](docs/PHASE6_ARCHITECTURE.md)
- [Performance Optimization](docs/PERFORMANCE.md)
- [Phase 6 Integration Results](docs/PHASE6_FINAL_RESULTS.md)