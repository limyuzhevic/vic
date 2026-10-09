# NLM Architecture

## Overview

NLM (熙然) is designed as a modular computational brain with the following major components:

```
──────────────────────────────────────────────────────────────────
│                        NLM Brain                                 │
──────────────────────────────────────────────────────────────────
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
│  │              PHASE 6: COMPLETE INTEGRATION                   ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       ││
│  │  │   All   │  │  Systems │  │  Work    │  │   Together│       ││
│  │  │ Integrated│ │ Coherent  │  │  Cohesion│  │   Loop   │       ││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       ││
│  └─────────────────────────────────────────────────────────────┘│
──────────────────────────────────────────────────────────────────
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

### 7. Memory Layer (Phase 6 - Integrated)
- **NeuralWorkingMemory**: Persistent activity with competition (Phase 6)
- **NeuralEpisodicMemory**: Experience encoding with replay (Phase 6)
- **NeuralAssociativeMemory**: Pattern associations (Phase 6)
- **ConceptFormation**: Pattern discovery from experience (Phase 6)
- **SpatialRepresentation**: Learned spatial relationships (Phase 6)
- **TemporalRelation**: Temporal sequence encoding (Phase 6)

### 8. Prediction Layer (Phase 6 - Integrated)
- **NeuralPrediction**: Temporal sequence learning (Phase 6)
- **ActionConsequencePredictor**: Action effect prediction (Phase 6)
- **PredictionErrorSignal**: Error computation for learning (Phase 6)

### 9. Cognitive Layer (Phase 6 - Integrated)
- **AttentionalSelection**: Competitive attention with working memory integration (Phase 6)
- **NeuralPlanner**: Predictive action selection with concept formation (Phase 6)
- **SelfModel**: Sensorimotor self-awareness with prediction (Phase 6)
- **SocialLearning**: Observation and imitation with temporal relations (Phase 6)

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

### Phase 6: Complete Integration
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
ATTENTION (competitive selection, working memory integration)
      ↓
PREDICTION (temporal sequence, prediction error)
      ↓
WORKING MEMORY (persistent activity, attention-gated)
      ↓
EPISODIC MEMORY (experience encoding, replay)
      ↓
CONCEPT FORMATION (pattern discovery, category discovery)
      ↓
NEURAL PLANNING (action selection, self-model integration)
      ↓
SPATIAL/TEMPORAL RELATION (relationship encoding)
      ↓
ACTION SELECTION (integrated cognitive-motivational output)
      ↓
MOTOR OUTPUT
      ↓
WORLD CONSEQUENCE
      ↓
REWARD / PREDICTION ERROR
      ↓
NEUROMODULATION (dopamine, curiosity, novelty)
      ↓
NEURAL DYNAMICS (plasticity, learning)
      ↓
MEMORY SYSTEMS (updating, consolidation)
      ↓
DEVELOPMENT SYSTEM (plasticity rate adjustment)
      ↓
CHANGED BRAIN STATE
      ↓
CHANGED FUTURE BEHAVIOR
```

## Advanced Integration Features (Phase 6)

### 1. Complete System Integration
- **Memory Systems**: Working memory connected to attention, episodic memory integrated with prediction
- **Prediction System**: Integrated with sensory processing and concept formation
- **Cognition Systems**: Neural planner connected to action selection, self-model linked to prediction
- **Neuromodulation**: Dopamine affects plasticity, curiosity drives exploration, novelty detection

### 2. Advanced Feedback Loops
- **Sensory → Working Memory**: Current sensory patterns stored in working memory
- **Working Memory → Attention**: Competition-based attention selection
- **Attention → Neural Processing**: Selective processing of attended information
- **Prediction → Memory**: Prediction errors update episodic memory
- **Memory → Planning**: Concepts inform action planning
- **Planning → Action**: Planned actions execute in world
- **Action → Prediction**: Action outcomes update prediction models
- **Prediction → Attention**: Prediction errors direct attention
- **Development → All Systems**: Developmental stages affect plasticity across all systems

### 3. Learning Mechanisms
- **STDP**: Spike-timing dependent plasticity (baseline)
- **Hebbian**: Correlation-based learning (baseline)
- **Reward-Modulated**: Dopamine-dependent plasticity (reward prediction error)
- **Structural Plasticity**: Synaptogenesis and pruning
- **Developmental Plasticity**: Age-dependent plasticity rates
- **Memory Consolidation**: Sleep/rest cycle with episodic memory replay

### 4. Performance Optimizations
- **Memory Pools**: Efficient allocation for neural components
- **Event Queues**: Asynchronous spike processing
- **Sparse Connectivity**: Reduced memory footprint
- **SIMD Vectorization**: Parallel processing capabilities
- **Checkpoint System**: State persistence and recovery

## Key Improvements in Phase 6

### Before Phase 6:
- Systems implemented but disconnected
- Neural simulation working
- Individual components created
- No integration between systems

### After Phase 6:
- **All systems integrated** into coherent brain loop
- **Real artificial brain** that learns from experience
- **Embodied intelligence** with sensory-motor loops
- **Memory-driven cognition** with episodic storage
- **Prediction-based behavior** with forward models
- **Developmental trajectory** with plasticity modulation
- **Neuromodulated learning** with reward, curiosity, novelty
- **Complete lifecycle** with development, learning, consolidation

## Advanced Usage Patterns

### 1. Research Configuration
```cpp
// Research-grade configuration
config.set("neuron_count", 3000);
config.set("region_count", 6);
config.set("connection_probability", 0.12);
config.set("development_enabled", 1);
config.set("plasticity_enabled", 1);
config.set("replay_interval", 100);
config.set("consolidation_interval", 1000);
```

### 2. Multi-Stage Development
```python
# Multi-stage development
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
# ... run critical period learning ...
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
# ... run maturation learning ...
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
# ... run adult learning ...
```

### 3. Integrated Experiment
```python
# Create comprehensive experiment
experiment = Phase6IntegratedExperiment()
config = Phase6Config()
config.neuronCount = 1000
config.maxSteps = 5000
config.enableCheckpointing = True
config.enableReplay = True
config.enableDevelopment = True

# Run complete integration test
result = experiment.run(config)

# Analyze results
print(f"Integration Score: {result.getIntegrationScore()}")
print(f"Memory Storage: {result.memoryEpisodesStored} episodes")
print(f"Learning Progress: {result.totalReward / result.maxSteps}")
```

### 4. Advanced Analysis
```python
# Perform detailed analysis
brain.printSystemStatus()
stats = agent.getAllStats()
integration = agent.getIntegrationStatus()
memory_stats = agent.getMemoryStats()

# Export for research
agent.exportBrainState("research_brain.bin")
```

## Research Capabilities

Phase 6 enables advanced research in:

1. **Memory Systems**: How working memory integrates with attention and episodic storage
2. **Prediction Systems**: How forward models guide behavior and learning
3. **Cognition Integration**: How concept formation emerges from neural dynamics
4. **Neuromodulation**: How dopamine, curiosity, and novelty coordinate learning
5. **Development**: How developmental stages affect learning capabilities
6. **Embodied Intelligence**: How sensory-motor loops enable adaptive behavior
7. **Memory Consolidation**: How sleep/rest cycles consolidate experiences
8. **Multi-System Interaction**: How different cognitive systems influence each other

## Performance Characteristics

- **Scalability**: Handles 1000-10000+ neurons efficiently
- **Real-time Simulation**: Supports 10000x simulation time in reasonable time
- **Memory Usage**: Optimized with sparse representations
- **Parallel Processing**: SIMD and multi-threading support
- **Persistence**: Checkpoint/restore capabilities for long simulations

## Conclusion

Phase 6 represents a complete artificial brain architecture where all components work together as a coherent system. Unlike previous phases that focused on individual components, Phase 6 emphasizes **integration over new features**, ensuring that existing systems work together to create genuinely intelligent behavior emerging from neural dynamics and plasticity.