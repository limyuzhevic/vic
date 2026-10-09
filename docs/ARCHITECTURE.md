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
│  │              PHASE 4-6: ADVANCED COGNITION                  ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐ ││
│  │  │Working │  │Episodic │  │Semantic │  │Procedural│  │Social │ ││
│  │  │Memory  │  │Memory  │  │Memory  │  │Memory   │  │Learning│ ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────────┘ ││
│  │                    │                     │                 ││
│  │                    ▼                     ▼                 ││
│  │            ┌─────────────┐     ┌─────────────┐                ││
│  │            │   Planning  │     │   Attention │                ││
│  │            │   System    │     │Selection    │                ││
│  │            └─────────────┘     └─────────────┘                ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Systems (Phase 4)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralSemanticMemory**: Abstract knowledge representation
- **NeuralProceduralMemory**: Learned action sequences
- **NeuralAssociativeMemory**: Experience-based associations

### 8. Prediction Systems (Phase 4)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Mechanisms (Phase 4)
- **AttentionalSelection**: Competition-based attention (NOT Transformer)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 10. Enhanced Integration (Phase 5-6)
- **MotorSystem**: Advanced motor control with planning
- **RealEnvironment**: Physics-based simulation with realistic dynamics
- **Performance Infrastructure**: Memory pools, event queues, SIMD optimization
- **Error Handling**: Comprehensive error management and recovery

## Data Flow Architecture

### Complete Brain Loop (Phase 6 Enhanced)

```
ENVIRONMENT (RealEnvironment)
  ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
  ↓
AGENT BRAIN (Phase 4-6 Enhanced)
  ├── SENSORY TRANSDUCTION
  │   └── Converts percepts to neural input
  ├── NEURAL PROCESSING
  │   ├── Working Memory (Phase 4)
  │   ├── Episodic Memory (Phase 4)
  │   ├── Prediction (Phase 4)
  │   ├── Attention (Phase 4)
  │   └── Concept Formation (Phase 4)
  ├── MOTOR DECODING
  │   ├── Neural Planner (Phase 4)
  │   ├── Self Model (Phase 4)
  │   ├── Social Learning (Phase 4)
  │   └── Motor System (Phase 6)
  └── NEUROMODULATION
      ├── Reward Prediction Error
      ├── Curiosity & Novelty
      └── Developmental Signals

  ↓
WORLD (Action effects)
  ↓
ENVIRONMENT FEEDBACK
  ↓
REWARD / SURPRISE / ERROR
  ↓
NEUROMODULATION (dopamine-like signal)
  ↓
ELIGIBILITY TRACES
  ↓
PLASTICITY (STDP, Hebbian, Reward-Modulated)
  ↓
STRUCTURAL PLASTICITY (synaptogenesis/pruning)
  ↓
DEVELOPMENTAL CHANGE
  ↓
BEHAVIORAL CHANGE
```

### Modern C++17 Implementation Features

- **pImpl Idiom**: Clean implementation separation (MotorSystem)
- **Memory Management**: Smart pointers, RAII, custom memory pools
- **Event-Driven Architecture**: Lock-free rings, time-bucketed processing
- **Parallel Processing**: Thread-safe neural computation with SIMD
- **Error Handling**: Comprehensive error types and recovery mechanisms

### Biological Fidelity Improvements (Phase 5)

- **Adaptive neuron models**: Dynamic parameters based on context
- **Short-term plasticity**: Facilitation/depression mechanisms
- **Configurable inhibitory circuits**: Balanced excitation/inhibition
- **Homeostatic plasticity**: Stability maintenance mechanisms

### Scalability Enhancements (Phase 5)

- **Memory pools**: Pre-allocated neuron/synapse resources
- **Event queues**: Lock-free processing for high concurrency
- **Sparse connectivity**: Efficient representation of neural networks
- **SIMD vectorization**: Parallel neural update computation
- **Parallel processing**: Multi-threaded neural simulation

## Phase Integration Analysis

### Phase 1 → 2: Neural Core
**Achievement**: Real spiking dynamics, event-driven processing, basic plasticity
**Status**: ✅ Complete

### Phase 2 → 3: World Interaction
**Achievement**: Environment interface, sensory processing, motor output
**Status**: ✅ Complete

### Phase 3 → 4: Emerging Cognition
**Achievement**: Memory systems, prediction capabilities, cognitive mechanisms
**Status**: ✅ Implemented (Enhanced in Phase 6)

### Phase 4 → 5: Performance & Research
**Achievement**: Scalability infrastructure, biological fidelity improvements
**Status**: ✅ Implemented (Enhanced in Phase 6)

### Phase 5 → 6: Final Integration
**Achievement**: Advanced motor system, real environment, complete integration
**Status**: ✅ Complete

## Key Design Principles

### Modern C++17 Best Practices

1. **Zero-Cost Abstractions**: Templates for performance-critical code
2. **Resource Management**: Smart pointers, RAII, custom allocators
3. **Concurrency**: Thread-safe operations, lock-free data structures
4. **Type Safety**: Strong typing, compile-time validation
5. **Performance Orientation**: Cache-friendly layouts, SIMD optimization

### Biological Inspiration

1. **Embodied Cognition**: Agent-world interaction drives learning
2. **Neural Plasticity**: STDP, Hebbian learning, structural changes
3. **Developmental Trajectory**: Critical periods, maturation stages
4. **Neuromodulation**: Reward, curiosity, novelty signals

### Architectural Innovation

1. **No Transformers**: Competitive attention, spike-based communication
2. **No Pretrained Knowledge**: All learning from experience
3. **Modular Design**: Clear interfaces between components
4. **Extensibility**: Easy addition of new subsystems

## Anti-Transformer Commitment

Throughout all phases, NLM maintains:

### Strict Adherence
- ✅ **No transformer architecture**
- ✅ **No LLM replacement**
- ✅ **No pretrained knowledge**
- ✅ **No PyTorch/TF/JAX dependencies**
- ✅ **No RNN/LSTM/GRU structures**
- ✅ **No CNN for cognition**

### Biological Focus
- ✅ **Individual neurons** with realistic dynamics
- ✅ **Spike-based communication**
- ✅ **Neural plasticity** mechanisms
- ✅ **Embodied interaction** with environment
- ✅ **Experience-driven learning**

### Architectural Principles
- ✅ **Data-oriented design** for performance
- ✅ **Event-driven processing** for efficiency
- ✅ **Memory pool allocation** for predictability
- ✅ **Clear subsystem interfaces** for modularity
- ✅ **Deterministic behavior** for scientific validity

If at any point the architecture begins to resemble a transformer, it should be redirected or the project renamed to clarify it is no longer NLM.

## Documentation Structure

The architecture documentation serves as the foundation for understanding:

1. **Core neural mechanisms** (Phase 2)
2. **World interaction capabilities** (Phase 3)
3. **Emergent cognitive systems** (Phase 4)
4. **Performance optimization** (Phase 5)
5. **Complete integration** (Phase 6)

### Documentation Integration

- **README.md**: High-level overview and getting started
- **HOW_TO_USE.md**: Detailed implementation and usage
- **easy_usage.md**: Beginner-friendly introduction
- **ARCHITECTURE.md**: Technical architecture and design
- **SCIENCE.md**: Biological background and inspiration
- **ROADMAP.md**: Development timeline and future directions
- **NEW_FEATURES.md**: Comprehensive Phase 4-6 improvements (TO BE CREATED)
- **TUTORIALS.md**: Step-by-step implementation guides (TO BE CREATED)
- **API_REFERENCE.md**: Complete API documentation (TO BE CREATED)

---

**The NLM architecture represents a commitment to brain-inspired, experience-driven artificial intelligence that emerges from neural dynamics rather than being programmed with explicit algorithms.**