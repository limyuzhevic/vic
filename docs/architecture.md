# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

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
│  │              PHASE 4: EMERGING COGNITION                    ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Attention│  │Planning │  │Self-Model│ │ Social  │       ││
│  │  │         │  │         │  │         │  │Learning │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Layer Architecture

### Layer 1: Core Layer
The foundation of NLM provides essential infrastructure and data structures:

- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system with schema validation
- **Random**: Deterministic random number generator with explicit seeds
- **Logger**: Structured logging system with performance metrics
- **SimulationClock**: Time management for event-driven simulation

### Layer 2: Brain Layer
Implements the biological neural substrate:

- **Neuron**: Individual neuron representation (LIF dynamics with refractory period)
- **Synapse**: Synaptic connection representation with weight dynamics
- **NeuralPopulation**: Group of neurons with shared properties and SoA layout
- **NeuralRegion**: Brain region with multiple populations and connectivity patterns
- **Brain**: Central coordinator with event queue and spike processing

### Layer 3: Dynamics Layer
Real-time neural computation engine:

- **NeuralDynamics**: Abstract base for neuron dynamics (extensible to other models)
- **SpikeSystem**: Event-driven spike processing with time-bucketed queues

### Layer 4: Plasticity Layer
Learning mechanisms based on biological principles:

- **PlasticityRule**: Abstract base for plasticity rules
- **STDP**: Spike-Timing-Dependent Plasticity (long-term potentiation/depression)
- **Hebbian**: Hebbian learning rule (co-activation strengthening)
- **StructuralPlasticity**: Synapse formation (synaptogenesis) and removal (pruning)

### Layer 5: Development Layer
Ontogenetic processes for brain maturation:

- **DevelopmentSystem**: Orchestrates developmental stages
- **Synaptogenesis**: Activity-dependent new synapse formation
- **Pruning**: Competition-based removal of weak synapses
- **Maturation**: Gradual optimization of synaptic properties

### Layer 6: Neuromodulation Layer
Behavioral control and reinforcement systems:

- **Neuromodulator**: Abstract base for neuromodulators (dopamine, acetylcholine, etc.)
- **Dopamine**: Reward prediction error signaling
- **Novelty**: Novelty detection for exploration
- **Curiosity**: Exploration motivation based on prediction errors
- **PredictionError**: Temporal difference error signals

### Layer 7: Memory Layer (Phase 4)
Cognitive architectures for information storage and retrieval:

- **NeuralWorkingMemory**: Persistent activity for temporary information holding
- **NeuralEpisodicMemory**: Experience encoding in distributed neural patterns
- **NeuralAssociativeMemory**: Experience-based association formation
- **ConceptFormation**: Pattern discovery and category abstraction
- **SpatialRepresentation**: Neural encoding of spatial relationships
- **TemporalRelation**: Sequencing and temporal ordering of events

### Layer 8: Prediction Layer (Phase 4)
Temporal anticipation and prospective cognition:

- **NeuralPrediction**: Temporal sequence learning and prediction
- **ActionConsequencePredictor**: Forward modeling of action outcomes
- **PredictionErrorSignal**: Error computation for learning and adaptation

### Layer 9: Cognitive Layer (Phase 4)
Higher-order cognitive functions:

- **AttentionalSelection**: Competition-based selective attention (lateral inhibition)
- **NeuralPlanner**: Predictive action selection and multi-step planning
- **SelfModel**: Sensorimotor self-awareness and body schema
- **SocialLearning**: Observation-based learning and imitation

### Layer 10: Sensory Layer
Input processing and perception:

- **SensoryInput**: Base class for sensory data abstraction
- **Vision**: Visual input processing with spatial filtering
- **Audio**: Auditory input processing with temporal filtering
- **InternalSignals**: Proprioceptive and internal state monitoring

### Layer 10: Motor Layer
Output generation and action execution:

- **Action**: Action representation and planning
- **MotorSystem**: Neural-to-action conversion through motor decoding

### Layer 12: Environment Layer
Interaction framework for embodied cognition:

- **Environment**: Abstract environment interface with state management
- **Observation**: Sensory observation wrapper with feature extraction
- **Action**: Action wrapper with effect prediction

### Layer 13: Experiments Layer
Research infrastructure and validation:

- **Experiment**: Single experiment recording with metadata
- **ExperimentRunner**: Experiment execution and data collection
- **Metrics**: Statistical analysis and performance evaluation
- **Phase4Experiment**: Specialized experimental paradigms for cognitive functions

## System Data Flow

### Phase 2: Neural Computation Pipeline
```
[External Stimulus] → Sensory Processing → Brain Core
      ↓                        ↓                   ↓
[Sensory Input] → receiveSensoryInput() → Neural Computation
      ↓                        ↓                   ↓
[Neural Potentials] → LIF Dynamics → [Spikes]
      ↓                        ↓                   ↓
[Spike Events] → Event Queue → Spike Processing
      ↓                        ↓                   ↓
[Synaptic Updates] → Plasticity Rules → [Weight Changes]
      ↓                        ↓                   ↓
[Structural Changes] → Synaptogenesis/Pruning → [Network Rewiring]
```

### Phase 3: World Interaction Cycle
```
[World State] → Environment Dynamics
      ↓                                   ↓
[Percepts] → Sensory Transduction
      ↓                                   ↓
[Neural Activity] → Brain Core Processing
      ↓                                   ↓
[Motor Commands] → Motor Decoding
      ↓                                   ↓
[Action Effects] → World Feedback
      ↓                                   ↓
[Rewards/Errors] → Neuromodulation
      ↓                                   ↓
[Learning Signals] → Plasticity Updates
      ↓                                   ↓
[Development] → Network Optimization
```

### Phase 4: Cognitive Integration Flow
```
[Percepts] → Attentional Selection
      ↓                                   ↓
[Selected Features] → Neural Processing
      ↓                                   ↓
[Temporal Patterns] → Prediction Systems
      ↓                                   ↓
[Working Memory] ← Episodic Memory ← Concept Formation
      ↓                                   ↓
[Multi-step Plans] → Neural Planning
      ↓                                   ↓
[Self Models] → Action Selection
      ↓                                   ↓
[Social Observations] → Social Learning
      ↓                                   ↓
[Actions] → Motor Output → World Effects
```

## Agent-Brain Interface Architecture

Phase 3/4 introduces the AgentBrain class that bridges NLM brain to external environments:

```cpp
class AgentBrain {
public:
    // Sensory Processing Interface
    void processSensoryInput(const Observation& observation);
    
    // Motor Output Interface
    MotorCommand decodeMotorCommand();
    
    // Learning Integration Interface
    void applyNeuromodulation(const NeuromodulatorSignal& signal);
    void updateDevelopmentStage(DevelopmentStage stage);
    
    // Cognitive Control Interface
    void applyAttentionalSelection(const AttentionWeights& weights);
    void executeNeuralPlanning(const ActionPlan& plan);
};
```

### AgentBrain Components

- **SensoryTransduction**: Converts world percepts to neural input patterns
- **MotorDecoding**: Converts neural population activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity mechanisms
- **Development**: Manages ontogenetic stage transitions and plasticity modifiers
- **Attention**: Implements competitive selection among neural populations
- **Planning**: Uses predictions to evaluate and select actions

## Key Design Principles

### 1. Anti-Transformer Philosophy
NLM deliberately avoids transformer architectures:
- **Individual Neurons**: Spiking dynamics rather than artificial attention
- **Spike-Based Communication**: Temporal coding instead of weighted sum
- **Biological Plasticity**: Synaptic weight changes based on spike timing
- **No Pretraining**: All knowledge emerges from interaction

### 2. Biological Plausibility
- **LIF Neurons**: Leaky integrate-and-fire dynamics with refractory periods
- **STDP**: Spike-timing dependent plasticity with biological time constants
- **Neuromodulation**: Dopamine-like reward prediction error signals
- **Development**: Activity-dependent synaptic formation and pruning

### 3. Modularity and Extensibility
- **Clear Interfaces**: Abstract base classes for all components
- **Plug-in Architecture**: Easy to add new neuron models or plasticity rules
- **Configuration-Driven**: All parameters set through configuration system
- **Performance-Centric**: Data-oriented design for large-scale simulations

### 4. Determinism and Reproducibility
- **Explicit Random Seeds**: Reproducible experiments
- **Deterministic Plasticity**: Same spike patterns yield same weight changes
- **Checkpointing**: State preservation for interrupted simulations
- **Comprehensive Logging**: Full experiment recording

### 5. Performance Optimization
- **Event-Driven Processing**: Only active neurons consume computation
- **Memory Pools**: Pre-allocated neuron/synapse storage with SoA layout
- **Sparse Connectivity**: Adjacency list representation for large networks
- **Parallel Processing**: Multi-threading support for multi-core CPUs
- **SIMD Vectorization**: Vectorized neural computations using AVX-512/AVX2

## Performance Architecture (Phase 5)

### Optimized Data Layouts
```
Neuron State (SoA Layout):
- membrane_potentials[neuron_count]  // Contiguous float array
- refractory_counters[neuron_count]   // Contiguous uint32 array  
- firing_states[neuron_count]        // Contiguous uint8 array

Synapse Storage:
- synapse_records[synapse_count]     // Contiguous struct array
- outgoing_adjacency[source_count][max_out]  // Adjacency lists
- incoming_adjacency[dest_count][max_in]      // Adjacency lists
```

### Event Processing Pipeline
```
Spike Detection → Event Queue → Time Buckets → Batch Processing
      ↓                    ↓              ↓              ↓
Neuron Update → Membrane Potential → Refractory Reset → Eligibility Traces
```

## Component Integration

### Configuration-Component Mapping
- **Core Layer**: Configuration system initialization and validation
- **Brain Layer**: Neuron/synapse creation based on configuration
- **Dynamics Layer**: Spike system initialization with time-step settings
- **Plasticity Layer**: Plasticity rule selection and parameter configuration
- **Development Layer**: Stage progression and timing parameters
- **Memory/Cognitive Layers**: Algorithm selection and network topology

### Performance Optimization Strategies
1. **Memory Efficiency**: Pre-allocation of all data structures
2. **Cache Locality**: SoA layouts for vectorized operations
3. **Parallelization**: Task-based parallelism with thread pools
4. **Vectorization**: SIMD-accelerated neural computations
5. **Event-Driven**: Minimal computation for inactive neurons
6. **Hierarchical**: Multi-level connectivity for scalability

## Anti-Transformer Implementation

### NLM vs Traditional ML
| Feature | NLM | Transformer/Neural Network |
|---------|-----|----------------------------|
| **Neurons** | Biological spiking units | Artificial activation functions |
| **Communication** | Spike timing | Weighted sum of inputs |
| **Learning** | STDP, Hebbian | Backpropagation |
| **Memory** | Working/episodic systems | Attention context window |
| **Attention** | Competitive lateral inhibition | Self-attention mechanisms |
| **Knowledge** | Emergent from experience | Pre-trained on datasets |

### Why This Matters
- **Cognitive Realism**: Implements actual neural mechanisms
- **Scalable Learning**: Can learn from continuous interaction
- **Developmental Trajectory**: Starts naive and matures
- **No Data Dependencies**: Works without large training datasets
- **Explainable**: Each component has biological basis

## Future Extensibility

### Pluggable Components
- **Neuron Models**: Easy to add Izhikevich, Hodgkin-Huxley, conductance-based
- **Plasticity Rules**: Novel learning rules can be implemented
- **Neuromodulators**: Additional chemical signals can be modeled
- **Memory Systems**: New memory architectures can be integrated

### Scaling Strategies
- **Distributed Simulation**: Multi-machine scaling for millions of neurons
- **GPU Acceleration**: Custom kernels for neural operations
- **Approximate Computing**: Trading precision for performance
- **Hierarchical Models**: Multi-scale cognitive architectures

## Conclusion

NLM's architecture represents a fundamentally different approach to artificial intelligence:

1. **Biologically-Inspired**: Implements actual neurons, synapses, and cognitive processes
2. **Developmentally-Grounded**: Starts as a naive brain and matures through interaction
3. **Self-Organizing**: Network structure emerges from activity-dependent processes
4. **Cognition-First**: Cognitive functions emerge from neural dynamics
5. **Performance-Optimized**: Efficient implementation for large-scale simulations

This architecture enables the study of emergent cognitive abilities without the architectural biases of traditional deep learning systems, providing a platform for true artificial general intelligence research.