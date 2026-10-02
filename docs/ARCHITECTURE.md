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

---

## Phase 6: Final Integration

Phase 6 represents the culmination of all previous phases, integrating memory, prediction, cognition, development, and neuromodulation into a coherent artificial brain system. This represents the transition from component-wise development to system-level intelligence.

### Phase 6 Integration Achievements

#### Memory Systems Integration
- **Working Memory**: NeuralWorkingMemory provides persistent activity for maintaining information during tasks
- **Episodic Memory**: NeuralEpisodicMemory stores experiences as neural patterns with temporal and spatial indexing
- **Associative Memory**: NeuralAssociativeMemory enables experience-based associations and pattern completion
- **All memory systems connected to neural processing and plasticity**

#### Neuromodulation Integration
- **Dopamine**: Modulates neural excitability and plasticity rules based on reward prediction errors
- **Curiosity**: Drives exploration behavior based on novelty detection
- **Novelty**: Integrated with sensory processing for discovery of new patterns
- **All neuromodulators connected to plasticity rules in a coordinated manner**

#### Prediction System Integration
- **PredictionSystem**: Integrated into brain loop with temporal sequence learning
- **PredictionError**: Error signals affect learning through prediction errors
- **Confidence tracking**: Implemented for reliable action selection and planning
- **Multi-step prediction**: Capability to anticipate future states several steps ahead

#### Cognition Systems Integration
- **NeuralPlanner**: Predictive planning with action sequence evaluation based on learned predictions
- **Concept Formation**: Discovers patterns from experience without predefined categories
- **AttentionalSelection**: Competitive attention with neural dynamics (NOT Transformer QKV)
- **SelfModel**: Sensorimotor self-awareness and body schema development
- **SocialLearning**: Observation and imitation of other agents

#### Development Integration
- **DevelopmentalStages**: Structured progression from Initial → CriticalPeriod → Maturation → Adult
- **StructuralPlasticity**: Modulated by age and developmental stage
- **NeuralExcitability**: Changes with development affecting learning rates and plasticity

#### Persistence and Memory Consolidation
- **Checkpoint System**: Save/load brain state for long simulations
- **Replay Mechanisms**: Episodic memory replay during rest/sleep cycles
- **MemoryConsolidation**: Integration of important experiences into stable representations
- **ReplayIntegration**: Connected with developmental processes for adaptive learning

### Integrated Brain Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    NLM Brain (Phase 6)                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Sensory   │───▶│    Brain    │───▶│    Motor    │          │
│  │   Input    │    │   Core      │    │    Output   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │                 │
│         ▼                  ▼                  ▼                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │  Memory    │    │  Prediction │    │ Cognition  │          │
│  │  Systems   │    │   System    │    │  Systems   │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Neuromod.  │                              │
│                    │  Signals   │                              │
│                    └─────────────┘                              │
│                            │                                    │
│                            ▼                                    │
│                    ┌─────────────┐                              │
│                    │ Development │                              │
│                    │   System    │                              │
│                    └─────────────┘                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │              PHASE 6: COHERENT BRAIN LOOP                   ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │Memory  │  │Predict  │  │Cognition│  │Social  │       ││
│  │  │Systems │  │ionSys  │  │Systems │  │Learning│       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

### Integrated Data Flow

#### Phase 6: Complete Brain Loop

```
WORLD (2D environment with objects)
      ↓
SENSORY PERCEPT (vision, touch, internal, proprioception)
      ↓
ATTENTION (competitive selection of relevant inputs)
      ↓
NEURAL PROCESSING (LIF dynamics, spikes)
      ↓
PREDICTION (learn temporal relationships, anticipate future states)
      ↓
WORKING MEMORY (maintain information for planning)
      ↓
EPISODIC MEMORY (store experiences as neural patterns)
      ↓
CONCEPTS (discover patterns without labels)
      ↓
PLANNING (simulate potential futures, select best actions)
      ↓
SELF-MODEL (predict self-generated vs external effects)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success, error)
      ↓
REWARD / ERROR
      ↓
NEUROMODULATION (dopamine, curiosity, novelty)
      ↓
PLASTICITY UPDATE (STDP, Hebbian, Reward-Modulated)
      ↓
DEVELOPMENTAL CHANGE (stage progression, structural changes)
      ↓
BEHAVIORAL CHANGE (adapted action selection)
      ↓
UPDATED WORLD STATE
      ↓
NEW SENSORY PERCEPT
```

### Advanced Integration Features

#### 1. Memory-Prediction Integration
- **Working Memory ↔ Prediction System**: Prediction system updates working memory content
- **Episodic Memory ↔ Prediction System**: Past experiences inform predictions of future states
- **Memory ↔ Planning**: Planning uses stored memories to evaluate potential futures

#### 2. Neuromodulation Coordination
- **Dopamine ↔ Prediction Error**: Reward prediction errors modulate prediction system learning
- **Curiosity ↔ Prediction System**: Novelty detection drives exploration and prediction refinement
- **Novelty ↔ Attention**: Novel stimuli attract attentional resources
- **All Neuromodulators ↔ Plasticity**: Coordinated modulation of synaptic changes

#### 3. Development-Driven Integration
- **DevelopmentalStage ↔ Plasticity**: Plasticity rates change with developmental stage
- **CriticalPeriods ↔ Learning**: Sensitive periods for specific types of learning
- **StructuralPlasticity ↔ Memory**: Memory systems influence neural structure
- **Maturation ↔ Cognition**: Cognitive capabilities develop with neural maturation

#### 4. Social Learning Integration
- **SocialLearning ↔ Prediction System**: Observing others updates predictive models
- **SocialLearning ↔ Self-Model**: Understanding self through social comparison
- **SocialLearning ↔ Planning**: Learning from others' successful strategies
- **Communication ↔ Memory**: Social signals stored in memory systems

#### 5. Attention as Integrator
- **Attention ↔ Prediction**: Selective processing of predicted relevant inputs
- **Attention ↔ Working Memory**: Winner-take-all selection of memory traces
- **Attention ↔ Planning**: Goal-directed attention for action planning
- **Attention ↔ Memory**: Competition between memory traces for retrieval

### Key Design Principles for Phase 6

1. **Coherent Architecture**: All systems work together as a unified brain
2. **Emergent Intelligence**: Complex behavior arises from simple component interactions
3. **Experience-Dependent Development**: Brain matures through interaction with environment
4. **Predictive Processing**: Future states drive current behavior
5. **Adaptive Learning**: Multiple learning mechanisms work in coordination
6. **Self-Organization**: Structure emerges from experience and interaction
7. **Scalable Complexity**: System can handle increasingly complex tasks
8. **Biological Plausibility**: Inspired by brain organization and function

### Phase 6 Research Questions

The Phase 6 system enables investigation of:

1. **Memory Integration**: Whether memory systems can integrate with neural dynamics
2. **Neuromodulation Coordination**: Whether neuromodulation can affect plasticity in a coordinated way
3. **Prediction as Organizer**: Whether prediction can become a central organizing principle
4. **Developmental Modulation**: Whether developmental stages can modulate learning
5. **Replay and Consolidation**: Whether replay can reinforce memory consolidation
6. **Complete Brain Loop**: Whether the entire system functions coherently

### Experimental Capabilities

Phase 6 enables sophisticated experiments:

1. **Cognitive Task Performance**: Complex problem-solving with multiple cognitive demands
2. **Social Interaction Studies**: Multi-agent learning and cooperation
3. **Developmental Studies**: How learning changes with age/developmental stage
4. **Memory Research**: Different memory systems and their interactions
5. **Prediction Studies**: How predictive coding supports behavior
6. **Neuromodulation Research**: Role of different neuromodulators in learning

### Technical Implementation Notes

#### Memory System Integration
- **WorkingMemory Integration**: NeuralWorkingMemory provides persistent activity for planning and decision-making
- **EpisodicMemory Integration**: NeuralEpisodicMemory stores experiences for retrieval during similar future situations
- **AssociativeMemory Integration**: NeuralAssociativeMemory enables transfer of learning across contexts

#### Prediction System Integration
- **Forward Models**: Predicts sensory consequences of actions
- **Temporal Sequence Learning**: Learns patterns in time-series data
- **Action Consequence Prediction**: Associates actions with their typical outcomes
- **Multi-step Prediction**: Plans several steps ahead based on learned sequences

#### Cognitive System Integration
- **Attention Mechanisms**: Competitive selection of relevant information
- **Planning Systems**: Evaluation of potential action sequences
- **Self-Models**: Understanding of one's own capabilities and limitations
- **SocialLearning**: Learning from observation of others

#### Development and Neuromodulation
- **DevelopmentalStages**: Structured progression affecting all systems
- **StructuralPlasticity**: Experience-dependent changes in brain connectivity
- **Neuromodulation**: Coordinated regulation of learning across systems

### Future Directions

Phase 6 provides foundation for:

1. **Higher Cognitive Functions**: More complex reasoning and abstract thinking
2. **Language Processing**: Natural language understanding and generation
3. **Theory of Mind**: Understanding others' mental states
4. **Emotions**: Affective processing and regulation
5. **Consciousness**: Emergence of subjective experience
6. **Transfer Learning**: Generalization across tasks and domains

### Limitations and Open Questions

Phase 6 does NOT claim:
- Human-like intelligence or consciousness
- Accurate reproduction of biological brains
- Solving all cognitive science challenges
- Immediate real-world applicability

Phase 6 DOES investigate:
- Whether coherent cognitive architectures can emerge from spiking neural dynamics
- How multiple learning systems can cooperate effectively
- Whether prediction can serve as a unifying principle
- How development shapes lifelong learning capabilities

### Conclusion

Phase 6 represents a significant step toward creating artificial systems that exhibit brain-like cognitive abilities. By integrating memory, prediction, cognition, development, and neuromodulation into a coherent architecture, NLM Phase 6 demonstrates how complex intelligent behavior can emerge from relatively simple neural components working together.

The Phase 6 system provides a powerful platform for investigating the fundamental questions of how cognition emerges from neural dynamics, how learning develops over time, and whether artificial systems can approach the flexibility and adaptability of biological intelligence.

---

## Legacy Documentation (Phase 4)

The documentation above represents the complete Phase 4 cognitive architecture. Phase 6 extends this foundation by adding comprehensive integration of all systems into a unified brain loop.

For complete Phase 4 documentation, refer to the original specifications and implementation details in the source code headers and Phase 4 experiment documentation.