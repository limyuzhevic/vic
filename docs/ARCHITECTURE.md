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

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer (Phase 4)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

### 8. Prediction Layer (Phase 4)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

### 9. Cognitive Layer (Phase 4)
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

### 10. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 11. Motor Layer
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 12. Environment Layer
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

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

### Phase 4: Emerging Cognition
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (competitive selection)
      ↓
NEURAL PROCESSING
      ↓
PREDICTION (temporal sequence learning)
      ↓
WORKING MEMORY (persistent activity)
      ↓
EPISODIC MEMORY (experience storage)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
PLANNING (multi-step prediction)
      ↓
SELF-MODEL (sensorimotor learning)
      ↓
MOTOR ACTION
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION
      ↓
PLASTICITY
      ↓
SOCIAL LEARNING (observation/imitation)
```

## Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

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

### 1. Core Layer ✅ WORKING
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

### 2. Brain Layer ✅ WORKING
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

### 3. Dynamics Layer ✅ WORKING
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

### 4. Plasticity Layer ✅ WORKING
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

### 5. Development Layer ✅ WORKING
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

### 6. Neuromodulation Layer ✅ WORKING
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

### 7. Memory Layer (Phase 4) ❌ DISCONNECTED
- **NeuralWorkingMemory**: Persistent activity for temporary storage
  - **STATUS**: Implemented but not integrated into brain loop
  - **ACCESSORS**: Return nullptr from Brain::getWorkingMemory()
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
  - **STATUS**: Implemented but not integrated
  - **ACCESSORS**: Return nullptr from Brain::getEpisodicMemory()
- **NeuralAssociativeMemory**: Experience-based associations
  - **STATUS**: Implemented but not integrated
  - **ACCESSORS**: Return nullptr from Brain::getAssociativeMemory()
- **ConceptFormation**: Pattern discovery without labels
  - **STATUS**: Implemented but not integrated
  - **ACCESSORS**: Return nullptr from Brain::getConceptFormation()
- **SpatialRepresentation**: Learned spatial relationships
  - **STATUS**: Implemented but not integrated
- **TemporalRelation**: Before/after/sequence encoding
  - **STATUS**: Implemented but not integrated

### 8. Prediction Layer (Phase 4) ❌ NOT TRAINED
- **NeuralPrediction**: Temporal sequence learning
  - **STATUS**: Implemented but not trained in main brain loop
  - **ACCESSORS**: Return nullptr from Brain::getPredictionSystem()
- **ActionConsequencePredictor**: Action effect prediction
  - **STATUS**: Implemented but not integrated
- **PredictionErrorSignal**: Error computation for learning
  - **STATUS**: Implemented but not integrated

### 9. Cognitive Layer (Phase 4) ❌ DISCONNECTED
- **AttentionalSelection**: Competition-based attention (NOT Transformer QKV)
  - **STATUS**: Implemented but not integrated
  - **ACCESSORS**: Return nullptr from Brain::getAttention()
- **NeuralPlanner**: Predictive action selection
  - **STATUS**: Implemented but not integrated
  - **ACCESSORS**: Return nullptr from Brain::getPlanner()
- **SelfModel**: Sensorimotor self-awareness
  - **STATUS**: Implemented but not integrated
- **SocialLearning**: Observation and imitation
  - **STATUS**: Implemented but not integrated

### 10. Sensory Layer ✅ WORKING
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

### 11. Motor Layer ✅ WORKING
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

### 12. Environment Layer ✅ WORKING
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

### 13. Experiments Layer ✅ WORKING
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Data Flow

### Phase 2: Neural Computation ✅ WORKING
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

### Phase 3: World Interaction ✅ WORKING
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

### Phase 4: Emerging Cognition ❌ DISCONNECTED
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (competitive selection) ❌ NOT CALLED
      ↓
NEURAL PROCESSING ❌ NOT CONNECTED
      ↓
PREDICTION (temporal sequence learning) ❌ NOT TRAINED
      ↓
WORKING MEMORY (persistent activity) ❌ NOT STORED
      ↓
EPISODIC MEMORY (experience storage) ❌ NOT STORED
      ↓
CONCEPT FORMATION (pattern discovery) ❌ NOT APPLIED
      ↓
PLANNING (multi-step prediction) ❌ NOT INTEGRATED
      ↓
SELF-MODEL (sensorimotor learning) ❌ NOT APPLIED
      ↓
MOTOR ACTION
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION ❌ LIMITED TO BASIC DOPAMINE
      ↓
PLASTICITY
      ↓
SOCIAL LEARNING (observation/imitation) ❌ NOT ACTIVATED
```

## Agent-Brain Interface ✅ WORKING

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Phase 4 Key Innovations

### Working Components:
- **Prediction System**: Learns temporal sequences from experience (but not trained)
- **Working Memory**: Persistent neural activity (but not stored)
- **Episodic Memory**: Experience encoded as neural patterns (but not stored)
- **Concept Formation**: Discovers patterns without labels (but not applied)
- **Attention**: Lateral inhibition competition (but not used)
- **Planning**: Uses learned predictions (but not integrated)
- **Self-Model**: Learns body schema (but not applied)
- **Social Learning**: Observes other agents (but not activated)

### Critical Integration Gap:
**Most Phase 4 components exist but are disconnected from the main brain loop.** The NLM brain processes sensory input and produces output, but the higher cognitive functions that would make it truly intelligent remain disconnected.

## Reality Check

**The problem is not missing components, but missing integration.** NLM has >80% of the required components - the challenge is connecting them into a working brain loop. Most missing functionality is due to disconnected systems, not unimplemented features.

**Priority**: Focus on connecting existing systems rather than implementing new disconnected components.