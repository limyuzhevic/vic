# NLM Architecture

## Overview

NLM (熙然) is an integrated computational brain simulator designed for studying emergent cognitive behaviors through event-driven neural computation. The architecture follows a layered, modular approach that integrates multiple computational systems for a unified representation of biological brain processes.

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Brain                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Sensory    │    │  Memory     │    │   Action    │          │
│  │  Processing │    │  Systems   │    │  Selection  │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │  Prediction │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │Neuromodul. │                              │
│                    │  Signals    │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 6: INTEGRATED BRAIN                        ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │Social  │       ││
│  │  │         │  │         │  │         │  │Learning│       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Component Layers

### Phase 1: Basic Brain Functionality

#### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system with JSON/YAML support
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

#### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics with improved state management)
- **Synapse**: Synaptic connection representation with plasticity flags
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator with integrated memory systems

#### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing with O(1) synaptic delivery

### Phase 2: Learning and Plasticity

#### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule with neuromodulation
- **STDP**: Spike-Timing-Dependent Plasticity with dopamine modulation
- **StructuralPlasticity**: Synaptogenesis and pruning mechanisms

#### 5. Development Layer
- **DevelopmentSystem**: Orchestrates developmental stages
- **Synaptogenesis**: New synapse formation with activity-dependent rules
- **Pruning**: Weak synapse removal with homeostatic regulation
- **Maturation**: Adult property development and stabilization

### Phase 3-6: Advanced Integration

#### 6. Neuromodulation Layer (Phase 5-6)
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward prediction error with plasticity factor modulation
- **Novelty**: Novelty detection and exploration motivation
- **Curiosity**: Goal-directed exploration and intrinsic motivation
- **PredictionError**: Temporal difference learning signals

#### 7. Memory Layer (Phase 4-6)
- **NeuralWorkingMemory**: Persistent activity for temporary storage and working memory
- **NeuralEpisodicMemory**: Experience encoding with replay mechanisms
- **NeuralAssociativeMemory**: Experience-based associations and pattern completion
- **ConceptFormation**: Pattern discovery without explicit labels
- **SpatialRepresentation**: Learned spatial relationships and navigation
- **TemporalRelation**: Before/after/sequence encoding

#### 8. Prediction Layer (Phase 4-6)
- **PredictionSystem**: Temporal sequence learning and forward models
- **NeuralPrediction**: Predictive coding with hierarchical temporal processing

#### 9. Cognitive Layer (Phase 6)
- **NeuralPlanner**: Goal-directed planning with Monte Carlo tree search
- **AttentionalSelection**: Competition-based attention mechanisms
- **SelfModel**: Internal model of agent's capabilities and limitations
- **SocialLearning**: Observational learning and imitation

## Integrated Brain Architecture

### Brain::step() Method Organization

The core simulation loop has been refactored from a monolithic 200+ line method into focused, single-responsibility methods:

#### Core Processing Pipeline
1. **Step Initialization**: Set up current simulation state
2. **Neuron Dynamics**: Update membrane potentials and detect spikes
3. **Spike Processing**: Schedule and deliver synaptic events
4. **Memory Updates**: Working memory and episodic memory
5. **Neuromodulation**: Apply reward, curiosity, and novelty signals
6. **Plasticity**: Apply learning rules (STDP, Hebbian, structural)
7. **Cognition**: Planning, attention, concept formation
8. **Development**: Stage-dependent developmental updates
9. **Replay**: Memory consolidation and replay
10. **Checkpointing**: Periodic state saving

### Configuration System

#### Format Support
- **JSON/YAML**: Structured configuration with type validation
- **Simple Format**: Backward-compatible key=value format
- **Command Line**: Runtime parameter overrides

#### Validation System
- **Type Checking**: Runtime type validation for all parameters
- **Range Validation**: Numeric bounds checking
- **Probability Validation**: Value range [0.0, 1.0]
- **Custom Validators**: User-defined validation rules

### Error Handling and Recovery

#### Error Classification
- **Configuration Errors**: Invalid or missing configuration values
- **File I/O Errors**: Missing files, permission issues
- **Memory Errors**: Allocation failures
- **Simulation Errors**: Invalid simulation state

#### Recovery Mechanisms
- **Graceful Degradation**: Fall back to defaults on errors
- **State Restoration**: Checkpoint-based error recovery
- **Validation Warnings**: Non-fatal validation issues logged

## Performance Optimizations

### Memory Efficiency
- **Smart Pointers**: RAII memory management
- **O(1) Lookup**: Neuron and synapse lookup by ID
- **Sparse Connectivity**: Efficient storage of sparse synaptic matrices
- **Memory Pools**: Object reuse for high-frequency allocations

### Computational Efficiency
- **Event-Driven Processing**: Spike delivery only when needed
- **SIMD Instructions**: Vectorized operations where possible
- **Parallel Processing**: Multi-region simulation support
- **Cache Locality**: Memory access pattern optimization

### Scalability Features
- **Hierarchical Regions**: Large-scale brain organization
- **Asynchronous Processing**: Staged simulation steps
- **Checkpoint Intervals**: Configurable state saving

## Extensibility and Plugin Architecture

### Plugin Points
- **Plasticity Rules**: Add new learning mechanisms
- **Neuromodulators**: Introduce new neuromodulation systems
- **Memory Systems**: Extend with custom memory types
- **Sensory Modalities**: Add new input modalities

### Configuration Plugin System
- **Schema Definition**: JSON Schema validation for configuration
- **Runtime Modification**: Hot-swapped configuration values
- **Validation Plugins**: Extensible validation framework

## Experimental Framework

### Experiment Management
- **Configurable Experiments**: Experiment parameters from configuration
- **Result Aggregation**: Standardized experiment result collection
- **Statistical Analysis**: Built-in statistical analysis tools
- **Reproducibility**: Deterministic experiment execution

### Experiment Types
- **Connectivity Testing**: Basic neural network functionality
- **Learning Experiments**: Synaptic plasticity verification
- **STDP Verification**: Spike-timing dependent plasticity tests
- **Integration Testing**: End-to-end system validation

## Development Workflow

### Code Organization
- **Layered Architecture**: Clear separation of concerns
- **Test-Driven Development**: Comprehensive unit and integration tests
- **Continuous Integration**: Automated testing and validation
- **Documentation Generation**: Automated API documentation

### Best Practices
- **Single Responsibility**: Each method has one clear purpose
- **Dependency Injection**: Configurable system dependencies
- **Immutability**: Immutable data structures where appropriate
- **Observer Pattern**: Event-driven system updates

## Documentation and API

### Generated Documentation
- **API Reference**: Auto-generated C++/Python API documentation
- **Architecture Diagrams**: UML and flow diagrams
- **Configuration Schema**: JSON Schema for configuration validation
- **Tutorial Examples**: Step-by-step usage guides

### User Documentation
- **Quick Start Guide**: Getting started with NLM
- **Python API Reference**: Complete Python API documentation
- **Configuration Guide**: Advanced configuration options
- **Troubleshooting**: Common issues and solutions

## Future Extensions

### Planned Features
- **Multi-Regional Connectivity**: Inter-region synaptic connections
- **External Inputs**: Real-world sensory integration
- **Self-Repair**: Fault detection and recovery mechanisms
- **Evolution**: Evolutionary optimization of brain parameters

### Research Integration
- **Biological Plausibility**: Enhanced biological fidelity
- **Computational Models**: Interface with computational neuroscience models
- **Hybrid Systems**: Integration with traditional AI systems

## Technical Specifications

### System Requirements
- **Memory**: 1GB minimum, 4GB recommended for large simulations
- **CPU**: Modern multi-core processor with SIMD support
- **Storage**: 500MB for checkpoints and logs
- **Network**: Optional for distributed computing

### Performance Benchmarks
- **Simulation Speed**: 10^4-10^6 simulation steps per second
- **Memory Efficiency**: <1MB per 1000 neurons with sparse connectivity
- **Scalability**: Linear scaling with number of regions
- **Real-time Capability**: Configurable timestep for real-time applications

## Conclusion

The NLM architecture represents a comprehensive, integrated approach to artificial brain simulation. By combining modular design with advanced computational techniques, NLM provides a flexible platform for studying emergent cognitive behaviors while maintaining biological plausibility and computational efficiency.

The refactored codebase demonstrates improved maintainability, testability, and extensibility while preserving the core functionality and performance characteristics of the original system.