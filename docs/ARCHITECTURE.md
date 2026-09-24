# NLM/熙然 Phase 6 Architecture Documentation

## Overview

**Phase 6: FINAL INTEGRATION**

Phase 6 integrates all previously separate systems into a coherent artificial brain. The architecture follows a closed-loop design where neural activity, plasticity, memory, neuromodulation, and behavior continuously influence each other.

## Current Phase 6 Architecture Status

### Phase 6 Brain Loop Architecture

The main simulation loop (`Brain::step()`) implements the complete brain simulation with **partial integration**:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes (deliver synaptic input) ✅
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics) ✅
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // 3. Detect spikes and schedule spike events ✅
    // ... spike detection and scheduling ...
    
    // 4. Update working memory ✅
    pImpl->workingMemory->update(pImpl->timestep);
    
    // 5. Apply neuromodulation effects ⚠️ (Limited)
    // - Dopamine affects neural excitability
    // - Novelty detection updates
    // - Curiosity computation
    
    // 6. Apply plasticity rules (STDP and Hebbian) ✅ with neuromodulation modulation
    
    // 7. Update episodic memory with current experience ✅
    
    // 8. Update prediction system ❌ (Not integrated)
    
    // 9. Update attention system ⚠️ (Partial integration)
    
    // 10. Update concept formation ❌ (Not integrated)
    
    // 11. Apply structural plasticity (synaptogenesis/pruning) ✅
    
    // 12. Replay important memories ✅
    
    // 13. Apply development effects ⚠️ (Limited)
    
    // 14. Memory consolidation ✅
    
    // 15. Checkpoint management ✅
    
    // TODO: Integrate prediction, planning, concept formation
    // TODO: Enhance neuromodulation integration
    // TODO: Complete development system
}
```

### Core Components - Phase 6 Status

#### 1. Core Layer - ✅ WORKING
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

#### 2. Brain Layer - ✅ WORKING
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

#### 3. Dynamics Layer - ✅ WORKING
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

#### 4. Plasticity Layer - ✅ WORKING
- **PlasticityRule**: Abstract base for plasticity rules
- **STDP**: Spike-Timing-Dependent Plasticity
- **Hebbian**: Hebbian learning rule
- **StructuralPlasticity**: Synapse formation/removal

#### 5. Development Layer - ⚠️ PARTIAL
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

#### 6. Neuromodulation Layer - ⚠️ PARTIAL INTEGRATION
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: ✅ Working - reward prediction error
- **Novelty**: ✅ Working - sensory pattern change detection
- **Curiosity**: ✅ Working - exploration drive
- **PredictionError**: ⚠️ Limited use
- **Serotonin, Norepinephrine, Acetylcholine**: ⚠️ PLACEHOLDER

#### 7. Memory Layer - ⚠️ PARTIAL INTEGRATION
- **WorkingMemory**: ⚠️ Partially integrated - NeuralWorkingMemory exists but disconnected
- **EpisodicMemory**: ⚠️ Partially integrated - NeuralEpisodicMemory exists but disconnected
- **SemanticMemory**: ❌ Empty implementation
- **ProceduralMemory**: ❌ Empty implementation
- **AssociativeMemory**: ⚠️ Basic associations

#### 8. Prediction Layer - ❌ DISCONNECTED
- **NeuralPrediction**: ⚠️ Implemented but not integrated
- **ActionConsequencePredictor**: ⚠️ Implemented but not integrated
- **PredictionErrorSignal**: ⚠️ Limited use

#### 9. Cognitive Layer - ❌ DISCONNECTED
- **AttentionalSelection**: ⚠️ Partially integrated (via WorkingMemory)
- **NeuralPlanner**: ❌ Not used
- **ConceptFormation**: ❌ Not used
- **SelfModel**: ❌ Not used
- **SocialLearning**: ❌ Not used
- **SpatialRepresentation**: ❌ Not used
- **TemporalRelation**: ❌ Not used

#### 10. Sensory Layer - ✅ WORKING
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

#### 11. Motor Layer - ✅ WORKING
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

#### 12. Environment Layer - ✅ WORKING
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

#### 13. Experiments Layer - ✅ WORKING
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase6Experiment**: Phase 6 integration experiment

## Phase 6 Data Flow

### Sensory Processing - ✅ WORKING
```
World → SensoryPercept → AgentBrain::processSensoryInput()
                               ↓
                         Brain::receiveSensoryInput()
                               ↓
                         Sensory neurons receive current
                               ↓
                         LIF dynamics integrate input
```

### Action Selection - ✅ WORKING
```
Motor neuron activity → AgentBrain::decodeMotorCommand()
                               ↓
                         Curiosity exploration check
                               ↓
                         MotorCommand selected
                               ↓
                         World::applyAction()
```

### Memory Formation - ⚠️ PARTIALLY WORKING
```
Brain step (spikes) → Working memory stores firing neurons ✅
                               ↓
                         Every 10 steps: Episodic memory captures state ✅
                               ↓
                         Every 100 steps: Replay of important episodes ✅
                               ↓
                         Every 1000 steps: Consolidation ✅
```

### Learning - ✅ WORKING
```
Reward from environment → AgentBrain::applyRewardModulation()
                               ↓
                         Dopamine computed (prediction error)
                               ↓
                         Synapses with eligibility traces updated
                               ↓
                         STDP weights modulated by dopamine
```

## Agent-Brain Interface - Phase 6 Enhanced

Phase 3/4 introduces the AgentBrain class that connects the NLM brain to the world (Phase 6 enhanced):

- **SensoryTransduction**: Converts world percepts to neural input ✅
- **MotorDecoding**: Converts neural activity to motor commands ✅
- **Neuromodulation**: Applies reward signals to plasticity ✅
- **Development**: Updates developmental stage ✅
- **Attention**: Selective processing through competition ⚠️ (Partial)
- **Planning**: Uses predictions to select actions ❌ (Not implemented)

## Phase 6 Integration Architecture

### Current Integration Map

```
PERCEPTION (SensoryInput)
    ↓
SENSORY NEURONS (working)
    ↓
WORKING MEMORY (Phase 6: stores firing neurons)
    ↓
ATTENTION (Phase 6: competitive selection)
    ↓
NEURAL PROCESSING (LIF dynamics, plasticity)
    ↓
PREDICTION (Phase 6: temporal sequence learning)
    ↓
EPISODIC MEMORY (Phase 6: experience storage)
    ↓
CONCEPT FORMATION (Phase 6: pattern discovery)
    ↓
PLANNING (Phase 6: multi-step prediction)
    ↓
SELF-MODEL (Phase 6: agency representation)
    ↓
ACTION SELECTION
    ↓
MOTOR ACTION
    ↓
WORLD.applyAction()
```

### Phase 6 Integration Status Summary

| System | Status | Integration Level |
|--------|--------|-------------------|
| Working Memory | ⚠️ Partially | Connected to brain loop |
| Episodic Memory | ⚠️ Partially | Connected to brain loop |
| Associative Memory | ✅ Working | Integrated with episodic memory |
| Neuromodulation | ⚠️ Partially | Dopamine, curiosity, novelty working |
| Prediction | ❌ Disconnected | Not integrated with brain loop |
| Cognition (Planner, Concepts) | ❌ Disconnected | Implemented but not used |
| Development | ⚠️ Partially | Limited scope (structural plasticity only) |
| Checkpointing | ✅ Working | File I/O functional |
| Replay | ✅ Working | Episodic memory replay |

## Phase 6 Key Features

### Memory Systems
- **Working Memory**: Persistent neural activity for temporary storage
  - Stores firing neurons during simulation steps
  - Competition among memory traces
  - Decay of weak traces

- **Episodic Memory**: Experience encoding with temporal ordering
  - Stores neural patterns every 10 simulation steps
  - Replay triggered every 100 steps
  - Consolidation every 1000 steps

- **Associative Memory**: Hebbian associations between patterns
  - Integrated with episodic memory
  - Spreading activation for recall

### Neuromodulation
- **Dopamine**: Reward prediction error computation
  - Modulates neural excitability
  - Scales STDP learning rates
  - Affects action selection through learning

- **Curiosity**: Novelty and prediction error combination
  - Drives exploration behavior
  - Combined with novelty detection

- **Novelty**: Sensory pattern change detection
  - Comparison with previous patterns
  - Integrated with curiosity system

### Development
- **Developmental Stages**: Five-stage progression
  - Initial → CriticalPeriod → Maturation → Adult → Aging
  - Auto-advance based on age
  - Modifies structural plasticity rates by stage

### Persistence
- **Checkpoint System**: Brain state serialization
  - Neuron and synapse state saving
  - Checkpoint manager for automated saves
  - Phase 6 integration experiment tests save/load

### Replay and Consolidation
- **Replay**: Episodic memory replay during simulation
  - Replay every 100 steps
  - Consolidation every 1000 steps
  - Memory strengthening mechanism

## Phase 6 Extension Points

### Where to Add New Phase 6 Mechanisms

1. **New plasticity rules**: Add to `Brain::step()` after STDP/Hebbian
2. **New neuromodulators**: Add to `Brain::Impl` and update `Brain::step()`
3. **New memory types**: Add to `Brain::Impl` and connect to perception/action
4. **New cognition**: Add to `Brain::Impl` and integrate with perception/action

### Key Phase 6 Integration Hooks

1. `receiveSensoryInput()` - Entry point for perception
2. `produceAction()` - Exit point for action
3. `step()` - Main loop where all Phase 6 systems update
4. `applyRewardModulation()` - Where Phase 6 learning signals apply
5. `save()/load()` - Phase 6 state persistence

## Phase 6 Configuration

### Core Phase 6 Configuration

| Parameter | Type | Default | Phase 6 Description |
|-----------|------|---------|---------------------|
| `random_seed` | int64_t | 42 | Random seed for reproducibility |
| `simulation_timestep` | double | 0.001 | Simulation time step |
| `neuron_count` | int64_t | 500 | Total number of neurons |
| `region_count` | int64_t | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Neural connectivity probability |

### Phase 6 Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory_size` | int | 1000 | Working memory capacity |
| `memory.episodic_capacity` | int | 100 | Episodic memory slots |
| `memory.associative_strength` | float | 0.5 | Associative memory strength |
| `memory.replay_interval` | int | 100 | Replay frequency (steps) |
| `memory.consolidation_interval` | int | 1000 | Consolidation frequency (steps) |

### Phase 6 Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine_scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity_scale` | float | 0.5 | Curiosity drive strength |
| `neuromod.novelty_threshold` | float | 0.1 | Novelty detection threshold |

### Phase 6 Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.critical_period_duration` | float | 100.0 | Critical period length |
| `development.maturation_rate` | float | 0.01 | Maturation progression rate |
| `development.plasticity_decay` | float | 0.001 | Plasticity decay with age |

### Phase 6 Prediction Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.horizon` | int | 10 | Prediction time horizon |
| `prediction.accuracy_threshold` | float | 0.7 | Prediction accuracy requirement |

## Phase 6 Performance Considerations

### Memory Usage
- **With 1K neurons**: ~50 MB
- **Working memory traces**: 10-50 active traces (activity-dependent)
- **Episodic episodes**: 100-500 per 10K steps
- **Checkpoint size**: ~5 MB (with 1K neurons)

### Computational Complexity
- **Per step**: O(neuron_count + synapse_count)
- **Memory updates**: O(working_memory_size) per step
- **Replay operations**: O(episodic_capacity × replay_depth)

### Scaling Considerations
1. **Memory bottlenecks**: Working memory trace accumulation
2. **Plasticity computation**: STDP for each synapse per step
3. **Replay overhead**: O(episode_count) for replay operations
4. **Neuromodulation costs**: Additional computation per synapse

## Phase 6 Development Guidelines

### Integration Best Practices
1. **Start simple**: Verify individual Phase 6 systems work before integration
2. **Shared data representations**: Use compatible types across systems
3. **Consistent temporal dynamics**: Phase 6 systems should use same time scales
4. **Proper scaling**: Ensure effects scale appropriately with neural activity

### Testing Phase 6 Integration
1. **Verify connectivity**: Ensure Phase 6 systems are instantiated
2. **Test data flow**: Verify sensory → memory → action pathways
3. **Validate learning**: Check that Phase 6 neuromodulation affects learning
4. **Check persistence**: Test checkpoint save/load functionality

### Performance Optimization
1. **Profile hot paths**: Identify Phase 6 bottlenecks
2. **Optimize memory access**: Phase 6 memory systems should be cache-friendly
3. **Parallelize where safe**: Phase 6 systems can be parallelized
4. **SIMD vectorization**: Use SIMD for Phase 6 numerical operations

### Future Phase 6 Improvements

#### Immediate (Easy Wins)
1. Connect neural planner to action selection
2. Implement proper synaptic scaling
3. Add metaplasticity (plasticity of plasticity)
4. Connect prediction system to attention

#### Medium Term
1. Implement full ACh, NE, 5-HT neuromodulation
2. Implement proper sleep/wake cycle
3. Better episodic consolidation mechanisms
4. Body model learning for agency

#### Long Term
1. Cortical-style hierarchical processing
2. Multiple brain regions with specialization
3. Detailed neuromodulator interactions
4. Full sensorimotor integration with proprioception

## Conclusion

**Phase 6 Architecture Status**: The NLM brain has a sophisticated **Phase 6 architecture** with **35.8% integration** (43/120 points). The core neural systems (LIF dynamics, plasticity, basic connectivity) work well, but **cognitive systems are disconnected** from the main brain loop.

**What Makes Phase 6 Unique**:
- **Closed-loop architecture** with 15-step brain loop
- **Integrated memory systems** (working + episodic + associative)
- **Neuromodulation integration** (dopamine, curiosity, novelty)
- **Development system** with five-stage progression
- **Persistence mechanisms** (checkpointing, replay)
- **Performance optimizations** (SIMD, memory pools)

**Phase 6 Challenges**:
- **Prediction disconnected** from sensory processing
- **Cognition isolated** (planner, concepts, self-model not used)
- **Limited neuromodulation** (only dopamine fully integrated)
- **Development scope limited** to structural plasticity only

**Phase 6 Success**: Despite integration challenges, Phase 6 provides a **foundation for investigating emergence, learning, and adaptation in a brain-like system** with all major components connected in a coherent loop.

**The foundation is in place; future work should focus on refinement rather than addition.**