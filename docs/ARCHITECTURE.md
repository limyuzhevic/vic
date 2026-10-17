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

**Types**
- **NeuronId**: Strongly-typed neuron identifier (32-bit unsigned integer)
- **SynapseId**: Strongly-typed synapse identifier
- **RegionId**: Strongly-typed brain region identifier
- **PopulationId**: Strongly-typed neural population identifier

These type-safe identifiers prevent logical errors and provide better debugging
than raw integers. Each uses a struct-like approach with implicit conversions
for convenience while maintaining type safety.

**Config**
The configuration system uses a variant-based approach to store heterogeneous
values while maintaining type safety through template accessors. Configuration
keys use dot notation for hierarchical organization:

- `neuron.count`: Number of neurons to create
- `plasticity.stdp.learningRate`: STDP learning rate
- `simulation.dt`: Time step duration
- `memory.working.capacity`: Working memory size

**Random**
Deterministic random number generator with explicit seed management for
reproducible experiments. Supports various distributions for different simulation
needs.

**Logger**
Structured logging system with multiple output formats (console, file, JSON)
and configurable verbosity levels. Supports performance timing and memory usage
tracking for analysis.

**SimulationClock**
Monotonic time tracking for discrete-event simulation. Supports:
- Wall-clock timing for real-time applications
- Step-based timing for algorithmic analysis
- Relative and absolute time queries

### 2. Brain Layer

**Neuron**
- **LIF Dynamics**: Leaky Integrate-and-Fire model with biologically plausible parameters
- **State Structure**: Contains membrane potential, refractory state, firing history
- **Synapse Management**: Maintains incoming and outgoing connection lists
- **Plasticity Support**: Tracks eligibility traces and supports multiple plasticity rules

**Synapse**
- **Connectivity**: Stores pre- and post-synaptic neuron references
- **Weight Management**: Maintains synaptic strength with bounds checking
- **Eligibility Traces**: Supports reward-modulated learning
- **Efficacy Tracking**: Computes connection effectiveness for learning

**NeuralPopulation**
- **Neuron Organization**: Groups neurons with similar properties
- **Type Specialization**: Excitatory/inhibitory/neutral classification
- **Function Specialization**: Processes different types of information
- **Performance Optimization**: Vectorized operations for population-wide computations

**NeuralRegion**
- **Anatomical Organization**: Logical grouping of neural populations
- **Connectivity Management**: Handles inter-regional connections with delays
- **Computational Specialization**: Implements different cognitive functions

**Brain**
- **Central Coordinator**: Manages all neural subsystems
- **Event-Driven Processing**: Spike-based dynamics for efficiency
- **Development Integration**: Orchestrates developmental plasticity
- **Memory Integration**: Coordinates multiple memory systems
- **Neuromodulation Integration**: Applies top-down control signals

### 3. Dynamics Layer

**NeuralDynamics**
Abstract base class defining interface for neuron dynamics implementations.
Enables different models (LIF, Izhikevich, Hodgkin-Huxley) with consistent interface.

**SpikeSystem**
- **Event Queue**: Manages spike timing with millisecond precision
- **Vectorization**: SIMD optimizations for parallel spike processing
- **Memory Pool**: Efficient spike storage and reuse
- **Sparse Connectivity**: Only stores active connections for efficiency

### 4. Plasticity Layer

**PlasticityRule**
Abstract base class defining interface for all synaptic learning rules.
Provides common infrastructure for eligibility traces and reward modulation.

**Hebbian**
- **Correlation-Based Learning**: Strengthens co-active synapses
- **Covariance Rule**: Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
- **Stability Mechanisms**: Weight bounds and learning rate limits
- **Biological Plausibility**: Models AMPA receptor trafficking

**STDP**
- **Spike-Timing-Dependent Plasticity**: learns precise timing relationships
- **Asymmetric Learning Window**: Different rules for pre-before-post vs post-before-pre
- **Mathematical Formulation**: Δw = A+ × exp(-Δt/τ+) or A- × exp(Δt/τ-)
- **Biological Basis**: NMDA receptor and backpropagating action potential modeling

**StructuralPlasticity**
- **Synaptogenesis**: Probabilistic new synapse formation
- **Pruning**: Activity-dependent synapse elimination
- **Developmental Regulation**: Stage-specific rates and thresholds

### 5. Development Layer

**DevelopmentSystem**
- **Stage-Based Learning**: Four developmental stages with different plasticity regimes
- **Plasticity Modulation**: Gradually decreasing learning rates from ~1.0 to ~0.2
- **Critical Periods**: Sensitive windows for activity-dependent refinement
- **Structural Maturation**: Synaptogenesis → pruning trajectory

**Synaptogenesis**
- **Probabilistic Formation**: New synapses created with developmental-stage-dependent probability
- **Hebbian Initialization**: Initial weights set based on co-activity patterns
- **Stabilization**: Weight constraints and homeostatic mechanisms

**Pruning**
- **Activity-Dependent**: Weak synapses eliminated based on recent activity
- **Competition**: Synapses compete for limited resources
- **Experience-Dependent**: Shapes neural circuits based on environmental input

**Maturation**
- **Electrical Refinement**: Increases in firing precision and reliability
- **Computational Optimization**: Streamlines network connectivity
- **Functional Specialization**: Develops adult-level computational capabilities

### 6. Neuromodulation Layer

**Neuromodulator**
Abstract base class defining interface for neuromodulatory signals.
Provides framework for top-down control of neural computation.

**Dopamine**
- **Reward Prediction Error**: Computes δ = reward - prediction
- **Temporal Difference Learning**: Updates predictions based on prediction errors
- **Plasticity Modulation**: Multiplies learning rates by prediction error

**Novelty**
- **Unexpected Change Detection**: Identifies surprising sensory input
- **Exploration Promotion**: Increases attention to novel stimuli
- **Learning Rate Modulation**: Enhances plasticity for novel experiences

**Curiosity**
- **Exploration Motivation**: Drives exploration of unknown states
- **Intrinsic Reward**: Provides reward signal for information gain
- **Policy Optimization**: Balances exploration vs. exploitation

**PredictionError**
- **Sensory Prediction**: Compares expected vs. actual sensory input
- **Error Signal**: Provides teaching signal for learning
- **Learning Modulation**: Adjusts plasticity based on prediction accuracy

### 7. Memory Layer (Phase 4)

**NeuralWorkingMemory**
- **Persistent Activity**: Recurrent circuits maintain information over time
- **Competitive Selection**: Winner-take-all dynamics for selective retention
- **Decay Mechanisms**: Gradual forgetting without reinforcement
- **Capacity Limits**: Finite storage with competitive allocation

**NeuralEpisodicMemory**
- **Pattern Storage**: Experiences stored as distributed neural patterns
- **Pattern Completion**: Partial cues trigger full memory retrieval
- **Temporal Context**: Memories include temporal information
- **Replay Mechanisms**: Reactivation during consolidation

**NeuralAssociativeMemory**
- **Association Formation**: Links between co-occurring patterns
- **Generalization**: Similar patterns map to similar associations
- **Stability-Plasticity Balance**: Maintains learned associations while allowing modification

**ConceptFormation**
- **Prototype Learning**: Clusters similar patterns into stable prototypes
- **Hebbian Averaging**: Prototype moves toward new instances
- **Stability Measurement**: Tracks how consistent patterns are
- **Similarity Matching**: Maps new patterns to existing concepts

### 8. Prediction Layer (Phase 4)

**NeuralPrediction**
- **Temporal Pattern Discovery**: Learns statistical regularities in spike trains
- **Forward Modeling**: Predicts sensory consequences of neural activity
- **Multi-Scale Processing**: Discovers patterns at different temporal resolutions
- **Error-Driven Learning**: Uses prediction errors to refine temporal models

**ActionConsequencePrediction**
- **Causal Inference**: Discovers action-effect relationships from experience
- **Outcome Expectancy**: Learns expected values of actions
- **Temporal Planning**: Sequences actions to achieve goals
- **Value Learning**: Computes action quality based on predicted rewards

### 9. Cognitive Layer (Phase 4)

**AttentionalSelection**
- **Lateral Inhibition**: Competitive dynamics between neural populations
- **Winner-Take-More**: Selected representations receive excitation, others inhibition
- **Top-Down Bias**: Goal information modulates competition
- **Bottom-Up Salience**: Stimulus strength influences selection

**NeuralPlanner**
- **Predictive Coding**: Simulates future outcomes of actions
- **Action Evaluation**: Rates actions based on predicted rewards
- **Policy Search**: Explores action sequences for optimal behavior

**SelfModel**
- **Body Schema**: Internal representation of physical body
- **Sensorimotor Mapping**: Learns relationship between actions and effects
- **Forward Models**: Predicts self-generated and external events
- **Agency Detection**: Distinguishes self-caused from external changes

**SocialLearning**
- **Observation**: Learns from observing other agents
- **Imitation**: Reproduces observed actions and their effects
- **Communication Signals**: Discovers meaning in social signals

### 10. Sensory Layer

**SensoryInput**
Abstract base class defining interface for sensory modalities. Provides
framework for sensor-specific processing while maintaining common interface.

**Vision**
- **Pixel Processing**: Extracts visual features from screen input
- **Feature Detection**: Identifies edges, shapes, patterns
- **Saliency Map**: Prioritizes important visual information

**Audio**
- **Spectral Analysis**: Processes sound frequency content
- **Pattern Recognition**: Identifies acoustic patterns
- **Temporal Integration**: Maintains auditory stream continuity

**InternalSignals**
- **Proprioception**: Sense of body position and movement
- **Interoception**: Internal physiological states
- **Homeostatic Monitoring**: Tracks metabolic needs and drives

### 11. Motor Layer

**Action**
- **Motor Command**: Specifies movement parameters and target
- **Trajectory Planning**: Generates smooth, achievable movements
- **Error Correction**: Adjusts plans based on sensory feedback

**MotorSystem**
- **Neural-to-Action Mapping**: Decodes neural activity into motor commands
- **Inverse Kinematics**: Computes motor commands for desired movements
- **Adaptation**: Refines motor output based on experience

### 12. Environment Layer

**Environment**
Abstract base class defining interface for simulated or real environments.
Supports different world models and interaction paradigms.

**Observation**
- **Sensor Fusion**: Combines multiple sensory inputs
- **State Estimation**: Inferring world state from noisy sensors
- **Representation**: Compact, efficient world model

**Action**
- **Command Interface**: Standardized action specification
- **Validation**: Checks action feasibility and legality
- **Execution**: Applies actions to environment

### 13. Experiments Layer

**Experiment**
- **Protocol Definition**: Specifies experimental conditions and measurements
- **Data Collection**: Records neural and behavioral measures
- **Analysis Framework**: Provides statistical analysis tools

**ExperimentRunner**
- **Trial Management**: Executes experimental protocols
- **Data Logging**: Records all experimental measurements
- **Statistical Analysis**: Computes results and error measures

**Metrics**
- **Neural Measures**: Spike counts, firing rates, connectivity changes
- **Behavioral Measures**: Reaction times, accuracy, choice patterns
- **Learning Metrics**: Acquisition curves, retention, generalization

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
EPISODAL MEMORY (experience storage)
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