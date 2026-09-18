# NLM Phase 6 Integration Plan

## Overview

Phase 6 is intended to be "Final Integration" but the current codebase has a fundamental architectural problem: **all cognitive systems exist but are completely disconnected from the main brain loop**. The system has sophisticated components but functions as a basic neural simulator rather than an integrated artificial brain.

**Core Philosophy:** Integration over new features. We must make existing systems work together as a coherent whole.

## Current State Summary

Based on the Phase 6 Final Audit, the integration score is **43/120 (35.8%)** with these critical gaps:

### Disconnected Systems Map
```
PERCEPTION (SensoryInput)
    ↓
SENSORY NEURONS (working)
    ↓
??? (gap - no working memory integration)
    ↓
??? (gap - no prediction integration)
    ↓
??? (gap - no concept formation integration)
    ↓
ACTION SELECTION (motor neurons - working but basic)
    ↓
MOTOR COMMAND
```

## Integration Priorities

### Priority 1: CONNECT EXISTING SYSTEMS
1. **WorkingMemory ↔ Sensory processing** - Store sensory patterns for pattern completion
2. **EpisodicMemory ↔ Experience logging** - Record all experiences for later recall
3. **PredictionSystem ↔ Sensory processing** - Predict sensory input to drive attention
4. **NeuralPlanner ↔ Action selection** - Plan action sequences based on goals
5. **ConceptFormation ↔ Experience processing** - Extract abstract concepts from experience

### Priority 2: IMPLEMENT MISSING MECHANISMS
1. **Brain::save() / Brain::load()** - Use CheckpointSystem for persistence
2. **Sleep/rest cycle** - Consolidate memories, reset plasticity
3. **Replay mechanism** - Replay important experiences for strengthening
4. **Full dopamine effects** - More than just scaling STDP
5. **ACh effects** - Modulate attention and memory encoding

### Priority 3: VALIDATE INTEGRATION
1. **Memory retention tests** - Verify episodic memory over time
2. **Prediction accuracy** - Test prediction learning and error correction
3. **Continual learning** - Test without catastrophic forgetting
4. **Developmental progression** - Verify stage-dependent behavior
5. **Multi-system interaction** - Test how systems influence each other

## Specific Integration Points

### 1. Sensory Processing → Working Memory Integration

**Current Gap:** Sensory input injects current into neurons but never stores patterns.

**Implementation:**
```cpp
// In AgentBrain::processSensoryInput() after injecting current:
if (brain_->getWorkingMemory()) {
    brain_->getWorkingMemory()->storePattern(percept, brain_->getSimulationTime());
}
```

**Impact:** Working memory can recall patterns to guide attention and decision-making.

### 2. Brain Step Loop Integration

**Current Gap:** Brain::step() only processes spikes, plasticity, and structural changes.

**Modified Brain::step() implementation:**
```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // 1. Process pending delayed spikes
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
    // ... (existing code) ...
    
    // 4. Apply plasticity rules
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // STDP and Hebbian plasticity
        }
    }
    
    // ===== PHASE 6 INTEGRATION POINTS =====
    
    // 5. Update working memory with current neural activity
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(*this, currentStep);
    }
    
    // 6. Update episodic memory with recent experiences
    if (pImpl->episodicMemory && getTotalSpikeCount() > lastSpikeCount) {
        pImpl->episodicMemory->storeExperience(*this, currentStep, currentTime);
        lastSpikeCount = getTotalSpikeCount();
    }
    
    // 7. Update prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(*this, currentStep);
    }
    
    // 8. Update attention
    if (pImpl->attention) {
        pImpl->attention->update(*this, currentStep);
    }
    
    // 9. Update concept formation
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(*this, currentStep);
    }
    
    // 10. Update development
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(*this, timestep);
    }
    
    // 11. Update neuromodulation
    if (pImpl->dopamine) pImpl->dopamine->update(*this, currentStep);
    if (pImpl->curiosity) pImpl->curiosity->update(*this, currentStep);
    if (pImpl->novelty) pImpl->novelty->update(*this, currentStep);
    if (pImpl->predictionError) pImpl->predictionError->update(*this, currentStep);
    
    // 12. Handle structural plasticity periodically
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // 13. Handle sleep/rest cycle
    handleSleepCycle(currentStep);
    
    // 14. Handle replay
    handleReplay(currentStep);
    
    // 15. Handle consolidation
    handleConsolidation(currentStep);
}
```

### 3. AgentBrain Integration with Higher Cognition

**Current Gap:** AgentBrain processes sensory input and outputs motor commands but doesn't connect to planning or concepts.

**Enhanced AgentBrain::processSensoryInput()**: