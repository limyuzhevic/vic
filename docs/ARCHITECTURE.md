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

## Integration Flow

### Brain-Loop Integration (Phase 6)

The Phase 6 architecture integrates all components into a unified brain loop:

```
SENSORY INPUT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput() → Sensory Neurons
      ↓
Neural Computation (LIF dynamics)
      ↓
Spike Processing (event-driven)
      ↓
Working Memory Update (persistent activity)
      ↓
Episodic Memory Storage (experience encoding)
      ↓
Prediction System Update (temporal learning)
      ↓
Attention System Update (competitive selection)
      ↓
Concept Formation (pattern discovery)
      ↓
Planning (action sequence evaluation)
      ↓
Self-Model Update (sensorimotor learning)
      ↓
Motor Decoding → MotorCommand
      ↓
Motor System → Action
      ↓
World.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine, curiosity)
      ↓
Eligibility Trace Update
      ↓
Plasticity (STDP, Hebbian, Reward-Modulated)
      ↓
Structural Plasticity (synaptogenesis, pruning)
      ↓
Development System (stage progression)
      ↓
Memory Consolidation & Replay
      ↓
CHANGED BRAIN STATE
```

### Memory System Integration

#### Working Memory Integration
- **Storage**: Active neural patterns stored in working memory during processing
- **Competition**: Lateral inhibition selects dominant representations
- **Maintenance**: Persistent activity through recurrent connections
- **Access**: Attention selects memories for use

#### Episodic Memory Integration
- **Encoding**: Experiences stored as neural activity patterns
- **Indexing**: Temporal and spatial indexing for retrieval
- **Replay**: Memory consolidation during rest cycles
- **Completion**: Pattern completion from partial cues

### Neuromodulation Integration

#### Dopamine Effects
- **Learning**: Modulates plasticity rule strength based on reward prediction error
- **Excitability**: Increases neural excitability, lowering firing thresholds
- **Memory**: Influences memory consolidation and strengthening

#### Curiosity Integration
- **Exploration**: Driven by novelty prediction error
- **Attention**: Biases attention toward novel stimuli
- **Memory**: Encourages storage of new experiences

#### Novelty Detection
- **Sensory**: Detects changes in sensory input
- **Prediction**: Compares input to expected patterns
- **Memory**: Signals new experience for storage

### Prediction System Integration

#### Forward Models
- **Learning**: Builds predictive models of sensory consequences
- **Error**: Computation of prediction errors
- **Planning**: Uses predictions to simulate futures
- **Selection**: Chooses actions with expected positive outcomes

### Cognitive System Integration

#### Attention Mechanism
- **Selection**: Competitive selection of neural representations
- **Integration**: Combines bottom-up and top-down signals
- **Gate**: Controls information flow to higher systems
- **Stability**: Maintains focus on relevant representations

#### Planning Integration
- **Evaluation**: Uses prediction system to assess action outcomes
- **Sequencing**: Creates and evaluates action sequences
- **Selection**: Chooses actions based on expected value
- **Adaptation**: Updates plans based on feedback

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Phase 4 Integration

Phase 4 introduced cognitive systems that must be integrated with the core brain loop:

### Required Integration Steps

1. **Sensory → Working Memory**: Convert sensory input to working memory traces
2. **Working Memory → Attention**: Feed working memory contents to attention system
3. **Attention → Concept Formation**: Pass attended content to concept formation
4. **Concept Formation → Planning**: Use concepts in action planning
5. **Planning → Motor System**: Convert plans to motor commands
6. **Motor → Self-Model**: Update self-model based on actions
7. **Action → Episodic Memory**: Store experiences with outcomes
8. **Episodic → Prediction**: Use episodes to train prediction system

### Integration Targets

- **WorkingMemory**: Connected to sensory processing, attention, and motor systems
- **EpisodicMemory**: Updated with experiences, replayed during consolidation
- **PredictionSystem**: Trained on sensory sequences and action outcomes
- **NeuralPlanner**: Uses predictions for action selection
- **ConceptFormation**: Processes working memory contents to form stable representations
- **SelfModel**: Learns body schema and action consequences
- **SocialLearning**: Observes other agents for imitation learning
- **SpatialRepresentation**: Builds metric representations of environment

## Performance Considerations

### Memory Efficiency
- Working memory uses sparse coding (active neurons only)
- Episodic memory stores compressed neural patterns
- Efficient retrieval through pattern completion

### Computational Efficiency
- Event-driven processing (spike events only)
- Sparse connectivity (probabilistic synapses)
- Parallel processing of independent regions

### Scalability
- Modular architecture allows adding regions
- Hierarchical organization (populations → regions → brain)
- Configurable population sizes

## Future Integration Directions

### Advanced Integration
1. **Sleep Cycles**: Consolidated memory replay during rest
2. **Development Effects**: Age-dependent changes in all systems
3. **Social Integration**: Multi-agent interaction and learning
4. **Energy Efficiency**: Metabolic constraints on neural activity

### Research Applications
- **Memory Studies**: Testing memory consolidation mechanisms
- **Learning Studies**: Investigating developmental trajectories
- **Attention Studies**: Studying competition and selection dynamics
- **Decision Making**: Understanding action selection processes

## System Status Summary

| Component | Status | Integration |
|-----------|--------|-------------|
| Neural Core | ✅ Working | Fully operational |
| Working Memory | ✅ Implemented | ❌ Not connected |
| Episodic Memory | ✅ Implemented | ❌ Not connected |
| Prediction System | ✅ Implemented | ❌ Not connected |
| Neuromodulation | ⚠️ Partial | Partial integration |
| Attention | ✅ Implemented | ❌ Not connected |
| Planning | ✅ Implemented | ❌ Not connected |
| Concept Formation | ✅ Implemented | ❌ Not connected |
| Self-Model | ✅ Implemented | ❌ Not connected |
| Social Learning | ✅ Implemented | ❌ Not connected |

**Phase 6 Goal**: Connect all disconnected systems into a coherent brain loop.