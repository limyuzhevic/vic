# NLM Architecture Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment. It is NOT a transformer or LLM, but rather a brain-inspired architecture with neurons and synapses where cognition emerges from neural dynamics.

## System Architecture

### Phase 6: Final Integration (Current Phase)

```mermaid
graph TD
    A[ENVIRONMENT] --> B[SENSORY INPUT]
    B --> C[NEURAL PROCESSING]
    C --> D[INTERNAL STATE]
    D --> E[MEMORY / PREDICTION]
    E --> F[NEUROMODULATION]
    F --> G[ACTION SELECTION]
    G --> H[MOTOR OUTPUT]
    H --> A

    subgraph "NEURAL CORE"
        C --> C1[SPIKING NEURONS]
        C1 --> C2[PLASTICITY]
        C2 --> C3[DEVELOPMENT]
    end

    subgraph "COGNITIVE SYSTEMS"
        D --> D1[WORKING MEMORY]
        D --> D2[EPISODIC MEMORY]
        D --> D3[ASSOCIATIVE MEMORY]
        D1 --> D4[ATTENTION]
        D2 --> D5[PREDICTION]
        D3 --> D6[CONCEPT FORMATION]
        D5 --> D7[PLANNING]
        D5 --> D8[SELF-MODEL]
        D5 --> D9[SOCIAL LEARNING]
    end

    subgraph "BEHAVIORAL SYSTEMS"
        F --> F1[DOPAMINE]
        F --> F2[CURIOSITY]
        F --> F3[NOVELTY]
        F --> F4[PREDICTION_ERROR]
        G --> G1[ACTION SELECTION]
        G1 --> G2[ACTION EXECUTION]
    end
end
```

### Core Component Architecture

NLM is organized into 13 major layers, each with specific responsibilities:

#### 1. Core Layer (Foundation)
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system with JSON/YAML support
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system with error handling
- **SimulationClock**: Time management for simulation

#### 2. Brain Layer (Neural Computation)
- **Neuron**: Individual LIF neuron with spiking dynamics
- **Synapse**: Synaptic connection with event-driven propagation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator with comprehensive error handling

#### 3. Dynamics Layer (Neural Physics)
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing with delays

#### 4. Plasticity Layer (Learning Mechanisms)
- **PlasticityRule**: Abstract base for plasticity rules
- **STDP**: Spike-Timing-Dependent Plasticity
- **Hebbian**: Hebbian learning rule
- **StructuralPlasticity**: Synapse formation and pruning

#### 5. Development Layer (Developmental Programming)
- **DevelopmentSystem**: Orchestrates developmental stages
- **Synaptogenesis**: New synapse formation based on activity
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

#### 6. Neuromodulation Layer (Internal State)
- **Dopamine**: Reward and reinforcement signaling
- **Novelty**: Novelty detection for exploration
- **Curiosity**: Exploration motivation system
- **PredictionError**: Error signals for learning

#### 7. Memory Layer (Phase 4 - Emerging Cognition)
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **NeuralAssociativeMemory**: Pattern associations between experiences
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

#### 8. Prediction Layer (Phase 4 - Temporal Processing)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

#### 9. Cognitive Layer (Phase 4 - Higher Functions)
- **AttentionalSelection**: Competitive selection (NOT Transformer QKV)
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

#### 10. Sensory Layer (Perception)
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals (e.g., hunger, thirst)

#### 11. Motor Layer (Action)
- **Action**: Action representation with timing
- **MotorSystem**: Neural-to-action conversion

#### 12. Environment Layer (Interaction)
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper for environment application

#### 13. Experiments Layer (Validation)
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution with metrics
- **Metrics**: Data analysis and statistical computation
- **Phase4Experiment**: Phase 4 cognitive experiments

## Data Flow Architecture

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

### Phase 6: Final Integration
```
AGENT-BRAIN INTERFACE (Closed Loop)
      ↓
ENVIRONMENT (World Simulation)
      ↓
SENSORY TRANSDUCTION → NEURAL PROCESSING
      ↓
BRAIN CORE (LIF, spikes, synapses, plasticity)
      ↓
MEMORY SYSTEMS (Working, Episodic, Associative)
      ↓
PREDICTION SYSTEM (Temporal, Causal)
      ↓
COGNITIVE SYSTEMS (Attention, Planning, Self-model)
      ↓
NEUROMODULATION (Dopamine, Curiosity, Novelty)
      ↓
DEVELOPMENTAL SYSTEM (Stages, Maturation)
      ↓
BEHAVIORAL OUTPUT → MOTOR ACTION
      ↓
ENVIRONMENT FEEDBACK (Reward, Error, Novelty)
      ↓
NEURAL UPDATE (Plasticity, Development)
      ↓
CHANGED BRAIN STATE
      ↓
FUTURE BEHAVIOR
```

## Design Decisions and Rationale

### 1. Biological Inspiration vs. Computational Efficiency
- **Decision**: Balance biological plausibility with computational tractability
- **Rationale**: Real neurons are too complex for large-scale simulation, so simplified models enable exploration of cognitive principles

### 2. No ML Frameworks
- **Decision**: Implement neural computation from scratch rather than using ML frameworks
- **Rationale**: Direct control over neural dynamics is essential for understanding computation

### 3. No Transformer Architecture
- **Decision**: Use spiking dynamics and competitive attention instead of Transformer QKV attention
- **Rationale**: Transformer architecture is biologically implausible for brain modeling

### 4. Modularity
- **Decision**: Clear interfaces between components for independent development and testing
- **Rationale**: Complex system requires component isolation to manage development complexity

### 5. Determinism
- **Decision**: Explicit random seeds for reproducible experiments
- **Rationale**: Scientific reproducibility is essential for research

### 6. Performance
- **Decision**: Data-oriented design for large-scale simulation
- **Rationale**: Performance bottlenecks limit scalability and practical use

### 7. Extensibility
- **Decision**: Clear patterns for adding new components
- **Rationale**: Research requires adding new cognitive mechanisms over time

### 8. No Predefined Knowledge
- **Decision**: All learning from experience, no pre-trained models
- **Rationale**: Developmental learning is central to the research question

## Key Components and Integration

### The Missing Connections (Phase 6 Challenge)
According to the **Phase 6 Final Audit**, most cognitive systems are disconnected from the main brain loop:

**Disconnected Systems:**
- Working Memory → No integration with sensory processing
- Episodic Memory → Never updated with experiences
- Prediction System → Never trained with data
- Attention → Not used in processing
- Concept Formation → Never processes experiences
- Planning → Not used in action selection
- Self-Model → Never updates
- Social Learning → Never observes others

**What Phase 6 Must Do:** Connect existing systems into a coherent brain loop rather than adding more disconnected components.

### Agent-Brain Interface
The `AgentBrain` class connects NLM brain to the world:

```cpp
class AgentBrain {
public:
    // Sensory transduction
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development
    void updateDevelopment(double timestep);
    
    // Attention and Planning (to be integrated)
    void applyAttention(const std::vector<float>& saliency);
    ActionResult planAction();
};
```

## Performance Architecture

### Phase 5 Performance Infrastructure

```mermaid
graph TD
    A[Performance Infrastructure] --> B[Event Queues]
    A --> C[Memory Pools]
    A --> D[SIMD Vectorization]
    A --> E[Parallel Processing]
    A --> F[Sparse Connectivity]
    A --> G[Benchmarking]
    
    B --> B1[Lock-free Ring Buffer]
    B --> B2[Time-bucketed Delays]
    
    C --> C1[SoA Neuron Storage]
    C --> C2[Free List Allocation]
    
    D --> D1[AVX-512 Support]
    D --> D2[Vectorized STDP]
    
    E --> E1[Thread Pool]
    E --> E2[Parallel Neural Processing]
    
    F --> F1[Adjacency Lists]
    F --> F2[Sparse Synapse Storage]
end
```

### Scaling Targets (Phase 5)

| Scale | Neurons | Synapses | Memory | Steps/sec |
|-------|---------|----------|--------|-----------|
| Micro | 1,000 | ~10,000 | ~10 KB | 10,000+ |
| Small | 10,000 | ~100,000 | ~100 KB | 5,000+ |
| Medium | 100,000 | ~1,000,000 | ~1 MB | 1,000+ |
| Large | 1,000,000 | ~10,000,000 | ~100 MB | 100+ |

## Current Status and Limitations

### What Works (Phase 6 Audit Score: 43/120 - 35.8%)

**Neural Core (Score: 18/20)**:
- ✅ LIF Neuron dynamics (Working)
- ✅ Synaptic transmission (Working)
- ✅ Spike propagation (Working)
- ✅ STDP plasticity (Working)
- ✅ Hebbian plasticity (Working)
- ✅ Structural plasticity (Working)

**Embodiment (Score: 8/10)**:
- ✅ Sensory input and motor output (Working)
- ✅ Reward prediction error (Working)
- ⚠️ Development stages (Minimal integration)
- ❌ Memory systems (Disconnected placeholders)
- ❌ Prediction systems (Disconnected)
- ❌ Attention (Disconnected)
- ❌ Planning (Disconnected)
- ❌ Self-model (Disconnected)
- ❌ Social learning (Disconnected)

### Critical Gaps to Address in Phase 6

1. **Memory Integration**: Connect WorkingMemory to sensory processing, EpisodicMemory to experience logging
2. **Cognitive Integration**: Connect NeuralPlanner to action selection, ConceptFormation to experience processing
3. **Neuromodulation Integration**: Implement full dopamine effects, ACh effects on attention
4. **Prediction Integration**: Train prediction system with sensory data
5. **Persistence**: Implement Brain::save() and Brain::load() using CheckpointSystem
6. **Replay and Consolidation**: Implement sleep/rest cycle with memory consolidation

## Integration Testing and Verification

### Phase 6 Integration Test Suite

```cpp
class Phase6IntegrationTest {
public:
    bool testNeuralCoreIntegration();
    bool testMemorySystemIntegration();
    bool testPredictionSystemIntegration();
    bool testCognitiveSystemIntegration();
    bool testNeuromodulationIntegration();
    bool testDevelopmentSystemIntegration();
    bool testPersistenceIntegration();
    bool testReplayIntegration();
};
```

### Integration Metrics

- **Memory Retention**: Does the system retain information over time?
- **Learning Progress**: Does performance improve across developmental stages?
- **Continual Learning**: Can the system learn without catastrophic forgetting?
- **Generalization**: Can learned behaviors transfer to new contexts?
- **Prediction Accuracy**: Does prediction error decrease with experience?
- **Developmental Progression**: Does the system follow expected developmental trajectories?

## Future Directions

### Beyond Phase 6

1. **Full Biological Fidelity**: More realistic neuron models, synaptic physiology
2. **Large-Scale Simulation**: Millions of neurons with detailed connectivity
3. **Multi-Agent Systems**: Social learning and cultural transmission
4. **Complex Behavior**: Goal-directed behavior with hierarchical planning
5. **Neuroscientific Validation**: Comparison with biological findings

### Research Questions

NLM aims to investigate:
1. Can memory systems integrate with neural dynamics?
2. Can neuromodulation coordinate plasticity in a biologically plausible way?
3. Can prediction become a central organizing principle?
4. Can developmental stages modulate learning effectively?
5. Can replay reinforce memory consolidation?
6. Can the complete brain loop function coherently?

## Conclusion

NLM has sophisticated **anatomy** but lacks **integration**. Phase 6's primary goal is to make existing systems work together as a coherent artificial brain rather than adding more disconnected components. The key challenge is connecting the dots between all the implemented systems to create a functioning cognitive architecture.