# NLM/熙然 Phase 6 Architecture Documentation

## Overview

**Phase 6 Status:** NOT COMPLETE. The NLM architecture contains sophisticated components but most cognitive systems are disconnected from the main brain loop. The brain has the *anatomy* of a cognitive system but lacks the *integration* that makes it function as a coherent artificial brain.

This document reflects the **current implementation state** (35.8% integration) rather than the intended Phase 6 design. The architecture follows a closed-loop design where neural activity, plasticity, memory, neuromodulation, and behavior continuously influence each other, but many connections are missing.

## Working Brain Loop Architecture

The current `Brain::step()` implementation includes only:

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
    // ... spike detection and propagation ...
    
    // 4. Apply plasticity rules (STDP and Hebbian) ✅
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // STDP and Hebbian applied
        }
    }
    
    // 5. Apply structural plasticity periodically (every 100 steps) ✅
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}
```

**What's NOT called in the current brain loop:**
- ❌ Working memory update
- ❌ Episodic memory storage
- ❌ Prediction system update
- ❌ Attention update
- ❌ Concept formation
- ❌ Planning
- ❌ Development update (beyond structural plasticity)
- ❌ Neuromodulation update (dopamine, etc.)
- ❌ Memory consolidation
- ❌ Replay

---

## Memory Systems

### Working Memory (NeuralWorkingMemory)

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp`

**Mechanisms:**
- Persistent neural activity for maintaining information
- Recurrent connections for maintenance
- Competition among memory traces
- Decay of weak traces

**CURRENT STATUS:** ⚠️ **DISCONNECTED**
- NeuralWorkingMemory class exists with proper mechanisms
- But `Brain::getWorkingMemory()` returns nullptr
- Never updated in brain loop
- No integration with sensory processing or action selection

### Episodic Memory (NeuralEpisodicMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp`

**Mechanisms:**
- Stores experiences as neural activity patterns
- Temporal ordering through timestamps
- Relevance-weighted recall
- Replay during offline periods

**CURRENT STATUS:** ❌ **NOT USED**
- Class exists but never instantiated
- Brain doesn't call any episodic memory functions
- No experience capture, storage, or replay

### Associative Memory (NeuralAssociativeMemory)

**Location:** `src/memory/NeuralEpisodicMemory.hpp/cpp` (in same file)

**Mechanisms:**
- Hebbian associations between patterns
- Spreading activation for recall
- Strength modification through experience

**CURRENT STATUS:** ❌ **EMPTY IMPLEMENTATION**
- Defined but with minimal functionality
- Never used in the brain
- No pattern associations created or recalled

## Neuromodulation Systems

### Dopamine

**Location:** `src/neuromodulation/Neuromodulator.hpp/cpp`

**Mechanisms:**
- Reward prediction error computation
- Phasic and tonic signaling
- Behavioral vigor modulation

**CURRENT STATUS:** ⚠️ **LIMITED**
- Dopamine class exists
- Affects neural excitability
- **Only**: Scales STDP weight scaling (lines 465-466 in Brain.cpp)
- No connection to memory or attention

### Curiosity

**Location:** `src/neuromodulation/Curiosity.hpp/cpp`

**Mechanisms:**
- Combines novelty and prediction error
- Exploration drive computation
- Decay over time

**CURRENT STATUS:** ✅ **WORKING (Phase 2)**
- Integrated into AgentBrain
- Used by AgentBrain for exploration decisions
- Stored in AgentBrain for action selection
- Affects motor command decoding

### Novelty

**Location:** `src/neuromodulation/Novelty.hpp/cpp`

**Mechanisms:**
- Novelty detection from sensory patterns
- Comparison with previous patterns
- Decay over time

**CURRENT STATUS:** ✅ **WORKING (Phase 2)**
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

**CURRENT STATUS:** ❌ **NOT INTEGRATED**
- PredictionSystem class exists with full functionality
- `Brain::getPredictionSystem()` returns nullptr
- Never updated in brain loop
- Prediction errors computed but not used

## Cognition Systems

### Neural Planner

**Location:** `src/cognition/NeuralPlanner.hpp/cpp`

**Mechanisms:**
- Multi-step action sequence evaluation
- Planning depth configuration
- Action quality from experience
- Plan success tracking

**CURRENT STATUS:** ❌ **NOT USED**
- NeuralPlanner class exists with complete implementation
- Never instantiated or called anywhere in the brain
- Planning mechanisms available but disconnected from action selection

### Concept Formation

**Location:** `src/cognition/ConceptFormation.hpp/cpp`

**Mechanisms:**
- Pattern clustering through similarity
- Prototype formation
- Instance tracking
- Stability measurement

**CURRENT STATUS:** ❌ **NOT USED**
- ConceptFormation class exists with full implementation
- Never instantiated or called
- No concept formation from experiences

### Attentional Selection

**Location:** `src/memory/NeuralWorkingMemory.hpp/cpp` (AttentionalSelection class)

**Mechanisms:**
- Competition between neural populations
- Winner-take-more dynamics
- Top-down and bottom-up bias
- Inhibition and excitation

**CURRENT STATUS:** ❌ **NOT USED**
- AttentionalSelection class exists
- But never processes working memory competitors
- No competitive attention in current brain loop

## Development System

**Location:** `src/development/DevelopmentSystem.hpp/cpp`

**Mechanisms:**
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Plasticity modification by stage
- Auto-advance based on age

**CURRENT STATUS:** ⚠️ **MINIMAL**
- Development system implemented
- **Only**: Modifies structural plasticity rates
- No effect on:
  - Neural excitability
  - Plasticity rules themselves
  - Attention
  - Memory consolidation
  - Neuromodulation levels

## Checkpoint System

**Location:** `src/performance/CheckpointSystem.hpp`

**Mechanisms:**
- Brain state serialization
- Neuron and synapse state saving
- Checkpoint manager for automated saves

**CURRENT STATUS:** ❌ **NOT CONNECTED**
- CheckpointSystem fully implemented with reader/writer/manager
- **But**: `Brain::save()` and `Brain::load()` are stubs
- No state persistence

## Current Working Integration (Phase 3/4)

The **working brain loop** is actually the Phase 3/4 integration:

```cpp
// In experiment/demos:
while (running) {
    // 1. Get observation from world ✅
    SensoryPercept percept = world.observe(agent);
    
    // 2. Process sensory input ✅
    agentBrain.processSensoryInput(percept);
    
    // 3. Brain step ✅
    brain.step(step);
    
    // 4. Decode motor command ✅
    MotorCommand cmd = agentBrain.decodeMotorCommand();
    
    // 5. Apply action to world ✅
    world.applyAction(agent, cmd);
    
    // 6. Compute reward ✅
    float reward = world.computeReward(agent);
    
    // 7. Apply reward modulation ✅
    agentBrain.applyRewardModulation(reward, predictedReward);
}
```

**What's missing after the loop:**
- No memory storage of experiences
- No prediction training
- No concept formation from observations
- No planning integration
- No sleep/rest cycle

## Data Flow (What Works vs What's Missing)

### Sensory Processing (WORKING)
```
World → SensoryPercept → AgentBrain::processSensoryInput()
                               ↓
                         Brain::receiveSensoryInput()
                               ↓
                         Sensory neurons receive current
                               ↓
                         LIF dynamics integrate input
```

### Action Selection (WORKING)
```
Motor neuron activity → AgentBrain::decodeMotorCommand()
                               ↓
                         Curiosity exploration check
                               ↓
                         MotorCommand selected
                               ↓
                         World::applyAction()
```

### Memory Formation (NOT WORKING)
```
Brain step (spikes) → Working memory stores firing neurons (NOT CALLED)
                               ↓
                         Every 10 steps: Episodic memory captures state (NOT CALLED)
                               ↓
                         Every 100 steps: Replay of important episodes (NOT CALLED)
                               ↓
                         Every 1000 steps: Consolidation (NOT CALLED)
```

### Learning (PARTIALLY WORKING)
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

### Actually Loosely Coupled (but not connected):
- Working memory (defined but never called)
- Episodic memory (defined but never instantiated)
- Prediction system (defined but never instantiated)
- Concept formation (defined but never instantiated)

### Actually Tightly Coupled (minimal):
- Neurons ↔ Synapses ↔ Plasticity (✅ working)
- Dopamine ↔ Eligibility traces ↔ Synaptic weights (⚠️ limited)
- Novelty ↔ Curiosity ↔ Exploration (✅ working in Phase 2)
- Development ↔ Plasticity rates ↔ Structural plasticity (⚠️ minimal)

## Extension Points

### Where to add new mechanisms:

1. **Connect Existing Systems:** Priority 1 is connecting what already exists
   - Connect WorkingMemory to sensory processing
   - Connect EpisodicMemory to experience logging
   - Connect PredictionSystem to sensory processing
   - Connect NeuralPlanner to action selection
   - Connect ConceptFormation to experience processing

2. **Implement Missing Mechanisms:**
   - Implement `Brain::save()` and `Brain::load()` using CheckpointSystem
   - Implement sleep/rest cycle with memory consolidation
   - Implement replay mechanism
   - Implement full dopamine effects on plasticity
   - Implement ACh effects on attention/memory

3. **Implement Missing Mechanisms:**
   - Test memory retention over time
   - Test prediction accuracy improvement
   - Test continual learning
   - Test developmental progression
   - Test multi-system interaction

4. **Optimize:**
   - Profile and optimize hot paths
   - Integrate performance infrastructure
   - Enable multithreading where safe

---
