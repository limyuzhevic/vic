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

### 7. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Real reward prediction error signaling
- **Novelty**: Bayesian surprise detection
- **Curiosity**: Exploration motivation and intrinsic reward
- **PredictionError**: Temporal difference learning signals
- **Reward**: Multimodal reward computation

### 8. Memory Integration (Phase 6)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 9. Prediction Integration (Phase 6)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 10. Cognitive Integration (Phase 6)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 11. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 12. Motor Layer
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 13. Environment Layer
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 14. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase6Experiment**: Phase 6 integration experiment

## Data Flow

### Phase 2: Neural Computation
```
Sensory Input (test signals)
      ↓
Brain.receiveSensoryInput()
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Plasticity (STDP, Hebbian)
      ↓
Structural Plasticity (synaptogenesis/pruning)
```

### Phase 3: World Interaction
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Structural Plasticity (synaptogenesis/pruning)
      ↓
Development System (stage progression)
```

### Phase 5: Performance Optimization
```
Advanced memory management with memory pools
      ↓
Event-driven processing with optimized queues
      ↓
SIMD vectorization for neural computations
      ↓
Parallel processing for large networks
      ↓
Checkpoint system with compression
      ↓
Development-aware plasticity
```

### Phase 6: Final Integration
```
ALL SYSTEMS INTEGRATED:
      ↓
MEMORY SYSTEMS ↔ NEURAL PROCESSING
      ↓
NEUROMODULATION ↔ PLASTICITY
      ↓
DEVELOPMENT ↔ PLASTICITY RATES
      ↓
PREDICTION ↔ LEARNING
      ↓
CHECKPOINT ↔ SAVE/LOAD
      ↓
REPLAY ↔ CONSOLIDATION
      ↓
COHERENT BRAIN LOOP:
WORLD → SENSORY → NEURAL → MEMORY → PREDICTION → MOTIVATION → ACTION → WORLD
```

## Integration Architecture

### Phase 6 Brain Loop
```
╔════════════════════════════════════════════════════════╗
║                    NLM BRAIN LOOP                     ║
╠════════════════════════════════════════════════════════╣
║                                                        ║
║  ┌─────────────────┐    ┌───────────────────────────┐  ║
║  │   WORLD         │    │          WORLD             │  ║
║  │                 │    │                         │  ║
║  │   SimpleWorld   │◀──▶│   Observation/WORLD       │  ║
║  │                 │    │                         │  ║
║  └─────────────────┘    └───────────────────────────┘  ║
║                                                        ║
║  ↓                        ↓                            ║
║  SENSORY INPUT            MOTOR OUTPUT                 ║
║  (Vision, Audio, etc.)    (MotorCommand)                ║
║                                                        ║
║  ↓                        ↓                            ║
║  ┌─────────────────┐    ┌───────────────────────────┐  ║
║  │                 │    │                         │  ║
║  │   AgentBrain    │───▶│          Brain            │  ║
║  │                 │    │                         │  ║
║  │   Interface     │    │   Neural Processing       │  ║
║  │                 │    │  (LIF Dynamics)           │  ║
║  └─────────────────┘    └───────────────────────────┘  ║
║                                                        ║
║  ↓                        ↓                            ║
║  SENSORY INJECTION      ACTION SELECTION               ║
║  (Neurons)              (Motor Neurons)               ║
║                                                        ║
║  ↓                        ↓                            ║
║  ┌─────────────────┐    ┌───────────────────────────┐  ║
║  │                 │    │                         │  ║
║  │   Memory       │    │   Prediction             │  ║
║  │   Systems      │    │   System                  │  ║
║  │                 │    │                         │  ║
║  │   Working      │    │   Temporal Learning       │  ║
║  │   Episodic      │    │   Sequence Prediction     │  ║
║  │   Associative   │    │   Action Consequence     │  ║
║  │                 │    │   Prediction             │  ║
║  └─────────────────┘    └───────────────────────────┘  ║
║                                                        ║
║  ↓                        ↓                            ║
║  NEUROMODULATION         DEVELOPMENT                ║
║  (Dopamine, Cur., etc.)  (Plasticity Rates)           ║
║                                                        ║
║  ↓                        ↓                            ║
║  PLASTICITY UPDATES     DEVELOPMENTAL STATE           ║
║  (STDP, Hebbian)        (Learning Rate Modulation)    ║
║                                                        ║
║  ↓                        ↓                            ║
║  ┌─────────────────┐    ┌───────────────────────────┐  ║
║  │   Memory        │    │   Memory                │  ║
║  │   Consolidation │    │   Replay                │  ║
║  │                 │    │                         │  ║
║  │   Checkpoint    │    │   Persistence           │  ║
║  │   Save/Load     │    │   State Restoration      │  ║
║  └─────────────────┘    └───────────────────────────┘  ║
║                                                        ║
║  ↓                        ↓                            ║
║  ┌─────────────────┐    ┌───────────────────────────┐  ║
║  │   Enhanced      │    │   Advanced             │  ║
║  │   Python API    │    │   Configuration       │  ║
║  │   (pynlm)       │    │   Management           │  ║
║  │                 │    │                         │  ║
║  │   Real-time     │    │   Batch Processing     │  ║
║  │   Monitoring    │    │   Parallel Execution │  ║
║  │   Analysis      │    │   Export/Import       │  ║
║  └─────────────────┘    └───────────────────────────┘  ║
║                                                        ║
║  ↓                        ↓                            ║
║  ┌─────────────────┐    ┌───────────────────────────┐  ║
║  │   RESEARCH      │    │   DEVELOPER TOOLS     │  ║
║  │   CAPABILITIES   │    │   AND UTILITIES       │  ║
║  │   Advanced      │    │   Custom Scripts     │  ║
║  │   Neuroscience   │    │   Configuration     │  ║
║  │   Modeling      │    │   Generators        │  ║
║  └─────────────────┘    └───────────────────────────┘  ║
║                                                        ║
╚════════════════════════════════════════════════════════╝
```

## Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world. Phase 6 enhances this interface with advanced neuromodulation and memory integration:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions
- **Memory Integration**: Connects memory systems to neural processing
- **Replay Control**: Manages memory replay and consolidation

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience
9. **Neuromodulation Integration**: Dopamine and other signals modulate plasticity
10. **Memory Integration**: All memory systems connected to neural processing
11. **Development Integration**: Developmental stages affect learning rates

## Phase 4 Key Innovations

### Prediction System

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Phase 4 Key Innovations

### Prediction System
- Learns temporal sequences from experience
- Predicts action consequences
- Multi-step prediction capability
- Error signals modulate learning

### Working Memory
- Persistent neural activity
- Recurrent connections for maintenance
- Competition for selective retention
- Neural-grounded (not a vector store)

### Episodic Memory
- Experience encoded as neural patterns
- Pattern completion retrieval
- Temporal and spatial indexing
- Replay during consolidation

### Concept Formation
- Discovers patterns without labels
- Prototype formation through averaging
- Stability measurement
- Category hints from properties

### Attention
- Lateral inhibition competition
- Winner-take-more dynamics
- Top-down and bottom-up bias
- NOT Transformer QKV attention

### Planning
- Uses learned predictions
- Evaluates potential futures
- Selects actions leading to goals
- Adapts based on outcomes

### Self-Model
- Learns body schema
- Predicts action consequences
- Self vs external distinction
- Capability representation

### Social Learning
- Observes other agents
- Learns action effects
- Imitation capability
- Simple communication signals