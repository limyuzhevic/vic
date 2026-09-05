# NLM/熙然 Phase 6 Architecture Documentation

## Overview

Phase 6 integrates all previously separate systems into a coherent artificial brain. The architecture follows a closed-loop design where neural activity, plasticity, memory, neuromodulation, and behavior continuously influence each other.

## Brain Loop Architecture

The main simulation loop (`Brain::step()`) implements the complete brain simulation:

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

## Memory Systems

### Working Memory (NeuralWorkingMemory)

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp`

**Mechanisms:**
- Persistent neural activity for maintaining information
- Recurrent connections for maintenance
- Competition among memory traces
- Decay of weak traces

**Integration:**
- Stores firing neurons each step
- Updated each simulation step
- Influences attention competition
- Decays over time if not reinforced

### Episodic Memory (NeuralEpisodicMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp`

**Mechanisms:**
- Stores experiences as neural activity patterns
- Temporal ordering through timestamps
- Relevance-weighted recall
- Replay during offline periods

**Integration:**
- Episodes captured every 10 simulation steps
- Replay triggered every 100 steps
- Consolidation every 1000 steps
- Influences future behavior through replay

### Associative Memory (NeuralAssociativeMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp` (in same file)

**Mechanisms:**
- Hebbian associations between patterns
- Spreading activation for recall
- Strength modification through experience

**Integration:**
- Associated with episodic memory
- Updated during experience encoding

## Neuromodulation Systems

### Dopamine

**Location:** `src/neuromodulation/Neuromodulator.hpp/cpp`

**Mechanisms:**
- Reward prediction error computation
- Phasic and tonic signaling
- Behavioral vigor modulation

**Integration:**
- Affects neural excitability (dopamine level modulates membrane potential)
- Scales STDP learning rates
- Modulates plasticity through eligibility traces
- Affects action selection indirectly through learning

### Curiosity

**Location:** `src/neuromodulation/Curiosity.hpp/cpp`

**Mechanisms:**
- Combines novelty and prediction error
- Exploration drive computation
- Decay over time

**Integration:**
- Used by AgentBrain for exploration decisions
- Stored in AgentBrain for action selection
- Affects motor command decoding

### Novelty

**Location:** `src/neuromodulation/Novelty.hpp/cpp`

**Mechanisms:**
- Novelty detection from sensory patterns
- Comparison with previous patterns
- Decay over time

**Integration:**
- Computed from vision changes in AgentBrain
- Used for curiosity computation
- Stored for later use

## Prediction System

**Location:** `src/prediction/PredictionSystem.hpp/cpp`

**Mechanisms:**
- Next state prediction
- Prediction error computation
- Confidence tracking
- History tracking

**Integration:**
- Called in brain loop (placeholder for full implementation)
- Prediction error affects curiosity
- Future: will affect attention and planning

## Cognition Systems

### Neural Planner

**Location:** `src/cognition/NeuralPlanner.hpp/cpp`

**Mechanisms:**
- Multi-step action sequence evaluation
- Planning depth configuration
- Action quality from experience
- Plan success tracking

**Integration:**
- Initialized in Brain
- Future: will be used for goal-directed behavior

### Concept Formation

**Location:** `src/cognition/ConceptFormation.hpp/cpp`

**Mechanisms:**
- Pattern clustering through similarity
- Prototype formation
- Instance tracking
- Stability measurement

**Integration:**
- Initialized in Brain
- Future: will process sensory patterns for generalization

### Attentional Selection

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp` (AttentionalSelection class)

**Mechanisms:**
- Competition between neural populations
- Winner-take-more dynamics
- Top-down and bottom-up bias
- Inhibition and excitation

**Integration:**
- Processes working memory competitors
- Updated each step
- Applies excitation to winners, inhibition to losers

## Development System

**Location:** `src/development/DevelopmentSystem.hpp/cpp`

**Mechanisms:**
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Plasticity modification by stage
- Auto-advance based on age

**Integration:**
- Updated every 1000 steps
- Modifies structural plasticity rates
- Affects neural excitability indirectly through plasticity

## Checkpoint System

**Location:** `src/performance/CheckpointSystem.hpp`

**Mechanisms:**
- Brain state serialization
- Neuron and synapse state saving
- Checkpoint manager for automated saves

**Integration:**
- `Brain::save()` writes checkpoint
- `Brain::load()` reads checkpoint
- CheckpointManager handles periodic saves

## Data Flow

### Sensory Processing
```
World → SensoryPercept → AgentBrain::processSensoryInput()
                              ↓
                        Brain::receiveSensoryInput()
                              ↓
                        Sensory neurons receive current
                              ↓
                        LIF dynamics integrate input
```

### Action Selection
```
Motor neuron activity → AgentBrain::decodeMotorCommand()
                              ↓
                        Curiosity exploration check
                              ↓
                        MotorCommand selected
                              ↓
                        World::applyAction()
```

### Memory Formation
```
Brain step (spikes) → Working memory stores firing neurons
                              ↓
                        Every 10 steps: Episodic memory captures state
                              ↓
                        Every 100 steps: Replay of important episodes
                              ↓
                        Every 1000 steps: Consolidation
```

### Learning
```
Reward from environment → AgentBrain::applyRewardModulation()
                              ↓
                        Dopamine computed (prediction error)
                              ↓
                        Synapses with eligibility traces updated
                              ↓
                        STDP weights modulated by dopamine
```

## System Independence and Coupling

### Loose Coupling (can function somewhat independently):
- Working memory (stores current activity)
- Episodic memory (stores experiences)
- Prediction system (computes errors)
- Concept formation (organizes patterns)

### Tight Coupling (strongly interconnected):
- Neurons ↔ Synapses ↔ Plasticity
- Dopamine ↔ Eligibility traces ↔ Synaptic weights
- Novelty ↔ Curiosity ↔ Exploration
- Development ↔ Plasticity rates ↔ Structural plasticity

## Extension Points

### Where to add new mechanisms:

1. **New plasticity rules**: Add to `Brain::step()` after STDP/Hebbian
2. **New neuromodulators**: Add to `Brain::Impl` and update `Brain::step()`
3. **New memory types**: Add to `Brain::Impl` and update `Brain::step()`
4. **New cognition**: Add to `Brain::Impl` and connect to perception/action

### Key integration hooks:

1. `receiveSensoryInput()` - Entry point for perception
2. `produceAction()` - Exit point for action
3. `step()` - Main loop where all systems update
4. `applyRewardModulation()` - Where learning signals apply
5. `save()/load()` - State persistence
