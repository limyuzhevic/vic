# NLM — 熙然 Phase 6 Complete Architecture Documentation

**Phase 6: FINAL INTEGRATION** - Complete artificial brain system

## Executive Summary

NLM (熙然, "serene flow") is an experimental artificial developmental brain system designed to simulate neural computation and cognitive development through interaction with an environment. Phase 6 represents the final integration of all previously separate systems into a coherent artificial brain.

## High-Level Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        ARTIFICIAL BRAIN                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   SENSORY   │───▶│    NEURAL   │───▶│    MOTOR    │          │
│  │   PERCEPT   │    │  PROCESSING │    │   OUTPUT    │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                  │                  │             │
│         ▼                  ▼                  ▼             │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐         │
│  │   WORLD     │    │   MEMORY    │    │  ACTION     │         │
│  │  LOOP       │    │  SYSTEMS   │    │  SELECTION  │         │
│  └─────────────┘    └─────────────┘    └─────────────┘         │
│                            │                                │
│                            ▼                                │
│                    ┌─────────────┐                          │
│                    │ PREDICTION  │                          │
│                    │  SYSTEM    │                          │
│                    └─────────────┘                          │
│                            │                                │
│                            ▼                                │
│                    ┌─────────────┐                          │
│                    │ NEUROMOD.  │                          │
│                    │  SIGNALS   │                          │
│                    └─────────────┘                          │
│                            │                                │
│                            ▼                                │
│                    ┌─────────────┐                          │
│                    │ DEVELOPMENT │                          │
│                    │   SYSTEM    │                          │
│                    └─────────────┘                          │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    DEVELOPMENTAL STAGES                   ││
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────┐│
│  │  │ Initial│  │ Critical│  │Maturation│  │  Adult  │  │Aging││
│  │  │   (0%) │  │  Period│  │   (~50%)│  │ (~80%) │  │(90%)││
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────┘│
│  └─────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
```

## Core Components and Interactions

### 1. Core Layer (Infrastructure)
- **Types**: NeuronId, SynapseId, RegionId, PopulationId - strongly typed identifiers
- **Config**: Configuration management system (JSON/file/command-line args)
- **Random**: Deterministic random number generator for reproducible experiments
- **SimulationClock**: Time management with wall-clock and simulation time synchronization
- **Logger**: Structured logging system for debugging and monitoring

### 2. Neural Layer (Computation)
- **Brain**: Central coordinator implementing LIF neural dynamics
- **NeuralRegion**: Functional brain regions with multiple neural populations
- **NeuralPopulation**: Groups of neurons with shared properties and dynamics
- **Neuron**: Individual spiking neuron model (LIF dynamics)
- **Synapse**: Synaptic connections with plasticity rules

### 3. Memory Systems (Phase 4 Integration)
- **Working Memory**: Persistent neural activity for temporary information storage
- **Episodic Memory**: Neural encoding of experience sequences with temporal ordering
- **Associative Memory**: Hebbian pattern associations and spreading activation
- **Concept Formation**: Pattern discovery without explicit labels or categories

### 4. Prediction System (Phase 4 Integration)
- **Prediction System**: Temporal sequence learning and action consequence prediction
- **Prediction Error Signal**: Error computation for learning and adjustment

### 5. Cognitive Layer (Phase 4 Integration)
- **Attention**: Competitive selection (NOT Transformer QKV attention)
- **Planning**: Neural planner with action sequence evaluation
- **Self-Model**: Sensorimotor self-awareness and capability representation

### 6. Neuromodulation Systems (Phase 3 Integration)
- **Dopamine**: Reward prediction error computation and behavioral vigor modulation
- **Curiosity**: Exploration drive combining novelty and prediction error
- **Novelty**: Novelty detection from sensory pattern changes
- **Prediction Error**: Error signals for learning and adjustment

### 7. Plasticity Systems
- **STDP**: Spike-Timing-Dependent Plasticity for synaptic strengthening/weakening
- **Hebbian**: Activity-dependent synaptic potentiation
- **Structural Plasticity**: Synapse formation and pruning based on activity

### 8. Development System (Phase 3 Integration)
- **DevelopmentalStages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- Modulates plasticity rates and learning capabilities across lifespan

### 9. Environment Interface
- **SimpleWorld**: 2D simulation environment for agent interaction
- **SensoryPercept**: Wrapper for vision, touch, internal, and proprioception signals
- **AgentBrain**: Interface connecting brain to world

### 10. Monitoring and Control
- **Metrics**: Performance monitoring and analysis
- **Checkpoint**: State persistence for long-running simulations

## Detailed Component Descriptions

### Brain Core Processing Loop

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics)
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events
    // ... spike detection and propagation ...
    
    // 4. Update working memory
    pImpl->workingMemory->update(pImpl->timestep);
    
    // 5. Apply neuromodulation effects
    // - Dopamine affects neural excitability
    // - Novelty detection updates
    // - Curiosity computation
    
    // 6. Apply plasticity rules (STDP and Hebbian)
    // with neuromodulation modulation
    
    // 7. Update episodic memory with current experience
    
    // 8. Update prediction system
    
    // 9. Update attention system
    
    // 10. Update concept formation
    
    // 11. Apply structural plasticity (synaptogenesis/pruning)
    
    // 12. Replay important memories
    
    // 13. Apply development effects
    
    // 14. Memory consolidation
    
    // 15. Checkpoint management
}
```

### Data Flow Architecture

#### Phase 2: Neural Computation
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

#### Phase 3: World Interaction
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

#### Phase 4: Emerging Cognition
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

### Agent-Brain Interface

Phase 3/4 introduces the AgentBrain class connecting NLM to the world:

- **SensoryTransduction**: Converts world percepts to neural input
- **MotorDecoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition
- **Planning**: Uses predictions to select actions

## System Integration Details

### Memory System Integration

#### Working Memory (NeuralWorkingMemory)
- Stores firing neurons each step
- Updated each simulation step
- Influences attention competition
- Decays over time if not reinforced

#### Episodic Memory (NeuralEpisodicMemory)
- Captures brain state every 10 steps
- Replays important episodes every 100 steps
- Consolidation every 1000 steps
- Influencing future behavior through replay

### Neuromodulation Integration

#### Dopamine
- Reward prediction error computation
- Phasic and tonic signaling
- Affects neural excitability
- Modulates STDP learning rates

#### Curiosity
- Combines novelty and prediction error
- Drives exploration behavior
- Used by AgentBrain for exploration decisions

#### Novelty
- Detects sensory pattern changes
- Comparison with previous patterns
- Used for curiosity computation

### Development Integration
- Every 1000 steps: Development system updates
- Modifies structural plasticity rates by stage
- Affects neural excitability through plasticity modulation

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

## Technical Implementation Details

### Memory System Structure
```cpp
class NeuralWorkingMemory {
public:
    void update(double timestep);
    void storeFiringNeurons(const std::vector<NeuronId>& firingNeurons);
    const std::vector<NeuronId>& getCurrentActivity() const;
    void decay();
private:
    std::vector<std::vector<NeuronId>> activityHistory;
    std::vector<NeuronId> currentActivity;
    double decayRate;
};
```

### Neuromodulation Integration
```cpp
class Dopamine {
public:
    void update(double reward, double predictionError);
    double getLevel() const;
    void modulatePlasticity(PlasticityRule& rule);
private:
    double currentLevel;
    double baseline;
    double predictionError;
};
```

### Development System
```cpp
enum class DevelopmentalStage {
    Initial = 0,
    CriticalPeriod = 1,
    Maturation = 2,
    Adult = 3,
    Aging = 4
};

class DevelopmentSystem {
public:
    void update(double timestep);
    DevelopmentalStage getStage() const;
    double getPlasticityModulator() const;
private:
    DevelopmentalStage currentStage;
    double age;
    double plasticityRate;
};
```

## Extension and Customization Points

### Where to Add New Components

1. **New plasticity rules**: Add to `Brain::step()` after STDP/Hebbian
2. **New neuromodulators**: Add to `Brain::Impl` and update `Brain::step()`
3. **New memory types**: Add to `Brain::Impl` and update `Brain::step()`
4. **New cognition**: Add to `Brain::Impl` and connect to perception/action

### Key Integration Hooks

1. `receiveSensoryInput()` - Entry point for perception
2. `produceAction()` - Exit point for action
3. `step()` - Main loop where all systems update
4. `applyRewardModulation()` - Where learning signals apply
5. `save()/load()` - State persistence

## Performance Considerations

### Memory Management
- Neural data stored in contiguous arrays for cache efficiency
- Event-driven processing avoids unnecessary computation
- Memory pools for frequent allocations

### Parallelization
- Neural population updates can be parallelized
- Spike processing uses event queues
- Memory systems operate sequentially for consistency

### Scalability
- Configuration-driven neuron/synapse counts
- Modular design allows component-specific scaling
- Checkpoint system enables long-running simulations

## Testing and Validation

Phase 6 includes comprehensive integration tests verifying:
- All brain components are properly connected
- Memory systems function together
- Neuromodulation affects learning
- Development system progression works
- Prediction and cognition systems integrate
- Complete brain loop functions coherently

## Future Directions

1. **Biological Fidelity**: Move toward more realistic neuron models
2. **Network Topology**: Implement cortical column and network structures
3. **Multi-Agent Systems**: Social interaction and communication
4. **Real-world Integration**: Connect to actual sensors and actuators
5. **Higher Cognition**: More complex reasoning and planning capabilities

## References

- NLM Documentation Suite (Phase 1-5)
- Scientific Literature on Neural Computation
- Neuroscience Research on Memory and Learning
- Computational Neuroscience Principles

---

**Document Version**: Phase 6 Architecture Documentation
**Last Updated**: September 2026
**Status**: Complete Integration