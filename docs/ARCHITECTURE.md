# NLM Architecture (Updated for Phase 6)

## Overview

NLM (熙然, meaning "serene flow") is designed as a modular computational brain with the following major components:

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
│  │            PHASE 6: FINAL INTEGRATION                        ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │ Working │  │ Episodic │  │ Associative│  │   Planner   │       ││
│  │  │ Memory  │  │ Memory  │  │  Memory  │  │           │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Phase 6: Complete Integration

Phase 6 represents the final integration of all NLM systems into a coherent artificial brain:

### Key Achievements

**1. Memory Systems Integration**
- **Working Memory**: Persistent activity with competitive selection
- **Episodic Memory**: Experience encoding with replay during consolidation
- **Associative Memory**: Hebbian pattern associations
- **All memory systems connected to neural processing loops**

**2. Neuromodulation Integration**
- **Dopamine**: Affects neural excitability and plasticity
- **Curiosity**: Drives exploration behavior
- **Novelty**: Detects unexpected events
- **Prediction Error**: Monitors prediction accuracy
- **All neuromodulators connected to plasticity rules**

**3. Prediction System Integration**
- **Temporal Sequence Learning**: Predicts future states
- **Action Consequence Prediction**: Links actions to outcomes
- **Multi-step Planning**: Evaluates action sequences
- **Integrated with working memory and attention**

**4. Cognitive Systems Integration**
- **Working Memory**: Persistent activity for temporary storage
- **Episodic Memory**: Neural pattern encoding of experiences
- **Associative Memory**: Pattern associations from Hebbian learning
- **Planning**: Predictive action selection using learned predictions
- **All cognition systems connected to perception and action loops**

**5. Development Integration**
- **Developmental Stages**: Plasticity rates change with age
- **Structural Plasticity**: Synaptogenesis modulated by stage
- **Neural Excitability**: Changes with developmental progression
- **Stage-based plasticity rules for each developmental phase**

**6. Persistence and Replay**
- **Checkpoint Save/Load**: Brain state serialization working
- **Memory Replay**: Episodic memory replay during rest/sleep
- **Consolidation**: Important memories strengthened through replay

## Core Components

### 1. Core Layer
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system (Phase 2)
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system with levels
- **SimulationClock**: Time management for simulation

### 2. Brain Layer
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator with all integrated systems

### 3. Dynamics Layer
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing with delays
- **LIF Implementation**: Leaky Integrate-and-Fire neuron model

### 4. Plasticity Layer
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule (experience co-activation)
- **STDP**: Spike-Timing-Dependent Plasticity (temporal order)
- **Reward-Modulated**: Plasticity affected by neuromodulators
- **StructuralPlasticity**: Synaptogenesis and pruning

### 5. Development Layer
- **DevelopmentSystem**: Orchestrates developmental stages
- **Synaptogenesis**: New synapse formation rates
- **Pruning**: Weak synapse removal optimization
- **Maturation**: Adult property development
- **Stage-based plasticity modulation**

### 6. Neuromodulation Layer
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration motivation based on novelty
- **Novelty**: Detects unexpected sensory events
- **PredictionError**: Monitors prediction accuracy
- **All neuromodulators connected to neural dynamics**

### 7. Memory Layer
- **Working Memory**: Persistent activity with competitive selection
- **Episodic Memory**: Experience encoding with neural patterns
- **Associative Memory**: Hebbian pattern associations
- **Concept Formation**: Pattern discovery without labels
- **Integration with prediction and attention systems**

### 8. Prediction Layer
- **NeuralPrediction**: Temporal sequence learning from experience
- **ActionConsequencePredictor**: Links actions to outcomes
- **Multi-step Prediction**: Evaluates action sequences
- **Error Signal Integration**: Modulates learning via prediction error

### 9. Cognitive Layer
- **Working Memory**: Persistent activity for temporary storage
- **Episodic Memory**: Neural pattern encoding of experiences
- **Associative Memory**: Pattern associations from Hebbian learning
- **Planning**: Predictive action selection using learned predictions
- **Self-Model**: Sensorimotor self-awareness

### 10. Sensory Layer
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing (16x16 world)
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals
- **Proprioception**: Body position awareness

### 11. Motor Layer
- **Action**: Action representation (MoveForward, TurnLeft, etc.)
- **MotorSystem**: Neural-to-action conversion
- **ActionSelection**: Competitive neural dynamics

### 12. Environment Layer
- **SimpleWorld**: Grid-based world simulation
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper with feedback

### 13. Experiments Layer
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution framework
- **Metrics**: Data analysis and statistics
- **Phase4Experiment**: Cognitive experiments
- **Phase6IntegratedExperiment**: Integration verification

## Data Flow (Phase 6 Complete Integration)

### Main Brain Loop
```
WORLD STATE
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
ATTENTION (competitive selection)
      ↓
NEURAL PROCESSING (LIF dynamics, spikes)
      ↓
INTERNAL STATE (working memory, attention)
      ↓
MEMORY / PREDICTION
      ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity, novelty)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD CONSEQUENCE
      ↓
REWARD / SURPRISE / ERROR
      ↓
NEUROMODULATION
      ↓
PLASTICITY (STDP, Hebbian, reward-modulated)
      ↓
MEMORY / DEVELOPMENT
      ↓
CHANGED BRAIN STATE
      ↓
CHANGED FUTURE BEHAVIOR
```

### Memory Integration Loop
```
EXPERIENCE
      ↓
EPISODIC MEMORY (neural pattern storage)
      ↓
PREDICTION SYSTEM (temporal sequence learning)
      ↓
CONCEPT FORMATION (pattern discovery)
      ↓
WORKING MEMORY (persistent activity)
      ↓
ATTENTION (selective processing)
      ↓
ACTION SELECTION
      ↓
WORLD ACTION
      ↓
REWARD / OUTCOME
      ↓
NEUROMODULATION
      ↓
PLASTICITY
      ↓
NEW EXPERIENCE
      ↓
(repeat)
```

## Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## Key Design Principles (Phase 6)

1. **Integrated Systems**: All brain systems work together in closed loops
2. **No ML Frameworks**: NLM implements its own neural computation
3. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
4. **Biological Inspiration**: Designed to be incrementally more biologically accurate
5. **Modularity**: Clear interfaces between components for maintenance
6. **Determinism**: Reproducible experiments via explicit random seeds
7. **Performance**: Data-oriented design for large-scale simulation
8. **Extensibility**: Clear patterns for adding new components
9. **No Predefined Knowledge**: All learning from experience
10. **Experience-based Development**: Brain develops from primitive state

## Phase 6 Integration Status

### ✅ Completed Integration
- [x] Memory systems connected to neural processing
- [x] Neuromodulation affects plasticity and neural dynamics
- [x] Prediction integrated with working memory
- [x] Development affects plasticity rates
- [x] Checkpoint save/load working
- [x] Replay and consolidation functional

### 🚀 Advanced Features (Phase 6)
- **Experience-based Development**: Brain develops from primitive to mature state
- **Memory Replay**: Replay of important episodes during consolidation
- **Prediction-Driven Behavior**: Actions selected based on learned predictions
- **Integrated Neuromodulation**: Coordinated chemical signaling for learning
- **Persistent Activity**: Working memory through recurrent neural activity

### 📊 Scientific Investigations (Phase 6)
- Can memory systems integrate with neural dynamics?
- Can neuromodulation affect plasticity in a coordinated way?
- Can prediction become a central organizing principle?
- Can developmental stages modulate learning effectively?
- Can replay reinforce memory consolidation?
- Can the complete brain loop function coherently?

## Usage Examples

### Basic Integration Demo
```bash
./nlm_phase6_demo
```

### Integration Verification Only
```bash
./nlm_phase6_demo --validate-only
```

### Full Integration with Custom Settings
```bash
./nlm_phase6_demo --neurons 1000 --steps 5000 --enable-dev --debug
```

### Quick Integration Test
```bash
./nlm_phase6_demo --quick --neurons 200 --steps 1000
```

### Phase 2 Basic Usage (Original demo)
```bash
./nlm_phase2 --neurons 1000 --steps 1000 --debug
```

## Component Dependencies

### High-Level Overview
- **Core → All**: Configuration, logging, random, time
- **Dynamics → Brain**: Spike processing
- **Plasticity → Brain**: Synaptic modification
- **Memory → Brain**: Storage and retrieval
- **Prediction → Brain**: Temporal sequence learning
- **Neuromodulation → Brain**: Learning signals
- **Development → Brain**: Stage progression
- **Sensory → Brain**: Input processing
- **Motor → World**: Action execution
- **Brain → AgentBrain**: Neural control interface

### Component Coupling
- **Loose Coupling**: Abstract interfaces between components
- **Tight Integration**: Functional connections within brain loop
- **Bidirectional Flow**: Forward processing and feedback loops

## Future Extensibility

### Adding New Components
1. **New Neuromodulators**: Add classes following Neuromodulator interface
2. **New Memory Systems**: Implement MemorySystem abstract interface
3. **New Prediction Models**: Extend PredictionSystem base class
4. **New Cognitive Functions**: Follow Planning/Attention patterns

### Extending Learning
1. **New Plasticity Rules**: Implement PlasticityRule interface
2. **New Development Stages**: Add stage-specific behaviors
3. **Advanced Learning**: Add reward-modulated variants

## Performance Characteristics

### Scalability
- **Memory**: Working memory scales with neural population
- **Processing**: Event-driven spike propagation (O(steps))
- **Storage**: Episodic memory stores neural patterns
- **Computation**: Predicts O(steps) for sequence learning

### Limitations
- **Memory**: Working memory limited by persistent activity
- **Computational Complexity**: Prediction learning O(n²) for patterns
- **Developmental Time**: Biological progression simulated
- **Replay Efficiency**: Consolidation O(episodes × replay_steps)

## Technical Details

### Neural Implementation
- **Model**: Leaky Integrate-and-Fire (LIF)
- **Dynamics**: Continuous differential equations discretized
- **Plasticity**: Spike-timing dependent rules
- **Development**: Stage-based parameter modulation

### Memory Implementation
- **Working Memory**: Recurrent neural connections
- **Episodic Memory**: Hebbian synaptic changes
- **Associative Memory**: Pattern completion
- **Replay**: Gradual consolidation during rest

### Learning Mechanisms
- **Supervised**: Prediction error signals
- **Reinforcement**: Reward prediction error
- **Autonomous**: Activity-dependent Hebbian
- **Developmental**: Stage-appropriate plasticity

## References

1. **Phase 6 Final Audit**: `docs/PHASE6_FINAL_AUDIT.md`
2. **Research Methods**: `docs/RESEARCH_METHODS.md`
3. **Limitations**: `docs/LIMITATIONS.md`
4. **Science**: `docs/SCIENCE.md`
5. **Phase 6 Results**: `docs/PHASE6_FINAL_RESULTS.md`

The NLM architecture represents a complete integration of brain-inspired systems into a functioning artificial neural substrate capable of experience-based learning, development, and adaptive behavior.

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