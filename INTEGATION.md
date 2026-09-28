# NLM Integration Guide - Connecting Systems

## Overview

This guide explains how to connect the NLM (Neural Learning Machine) systems to create a coherent artificial brain. After Phase 6, the core architecture exists but many systems remain disconnected from the main brain loop.

## Current Integration Status

Based on the Phase 6 Final Audit, the following systems are **connected**:

- ✅ Neural Core (LIF neurons, spikes, synapses)
- ✅ Working Memory (connected in Brain::step())
- ✅ Neuromodulation (dopamine, curiosity, novelty connected)
- ✅ Development System (affects plasticity rates)
- ✅ Structural Plasticity (synaptogenesis/pruning)

**NOT CONNECTED**:
- ❌ Episodic Memory (returns nullptr)
- ❌ Semantic Memory (empty implementation)
- ❌ Procedural Memory (empty implementation)  
- ❌ Prediction System (returns nullptr)
- ❌ Associative Memory (disconnected placeholder)
- ❌ Neural Planner (not used)
- ❌ Concept Formation (not used)
- ❌ Attention Selection (not used)
- ❌ Self-Model (not used)
- ❌ Social Learning (not used)

## Integration Strategy

### 1. Memory System Integration

**Working Memory** - Already connected:
```cpp
// Brain.cpp:413-417
if (pImpl->workingMemory) {
    pImpl->workingMemory->update(pImpl->timestep);
}
```

**Episodic Memory** - Must be connected:
```cpp
// Add to Brain.cpp step() method
if (pImpl->episodicMemory) {
    pImpl->episodicMemory->update(pImpl->timestep, currentStep);
}
```

**Associative Memory** - Connect pattern storage:
```cpp
// Connect to working memory for pattern associations
if (pImpl->associativeMemory && pImpl->workingMemory) {
    pImpl->associativeMemory->integrateWithWorkingMemory(
        pImpl->workingMemory->retrieve()
    );
}
```

### 2. Prediction System Integration

**Current**: Prediction system exists but disconnected
**Goal**: Integrate with sensory processing and memory

```cpp
// Add to Brain.cpp step() method  
if (pImpl->predictionSystem) {
    // Update predictions with sensory input
    auto sensoryPattern = getSensoryPattern();
    pImpl->predictionSystem->updatePrediction(
        sensoryPattern, pImpl->timestep
    );
}
```

### 3. Neuromodulation Integration

**Already Enhanced**:
- Dopamine: Reward prediction error affects neural excitability and plasticity
- Curiosity: Drives exploration and synaptic modulation
- Novelty: Promotes structural plasticity changes

**To Add**:
- Serotonin (currently stub) - regulates attention and learning rate
- Norepinephrine (currently stub) - arousal and focus
- Acetylcholine (currently stub) - memory consolidation and attention

```cpp
// Enhanced neuromodulation integration
void Brain::applyNeuromodulation() {
    if (pImpl->dopamine) {
        float error = calculatePredictionError();
        pImpl->dopamine->setRewardPredictionError(error);
        pImpl->dopamine->update(pImpl->timestep);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
}
```

### 4. Cognitive System Integration

**Currently Disconnected**: All cognitive systems exist but are not instantiated or used.

**Integration Sequence**:

1. **Attention Selection** - Gate sensory processing
```cpp
if (pImpl->attentionalSelection) {
    auto selectedNeurons = pImpl->attentionalSelection->selectAttention(
        sensoryNeurons, pImpl->globalInhibition
    );
    processSelectedNeurons(selectedNeurons);
}
```

2. **Concept Formation** - Extract patterns from sensory input
```cpp
if (pImpl->conceptFormation) {
    auto currentPattern = extractPatternFromSensoryInput();
    pImpl->conceptFormation->learnConcept(currentPattern);
}
```

3. **Neural Planner** - Generate action sequences
```cpp
if (pImpl->neuralPlanner) {
    auto goal = determineGoal(pImpl->workingMemory, 
                             pImpl->episodicMemory);
    pImpl->neuralPlanner->setGoal(goal);
    auto plan = pImpl->neuralPlanner->planAction();
    executeActionPlan(plan);
}
```

4. **Self-Model** - Track body schema and agency
```cpp
if (pImpl->selfModel) {
    pImpl->selfModel->update(selfPosition, lastAction);
}
```

### 5. Development Integration

**Current**: Development affects only plasticity rates
**Enhanced**: Development should affect multiple systems

```cpp
void Brain::applyDevelopmentEffects() {
    if (!pImpl->developmentSystem) return;
    
    // Neural excitability changes with development
    float excitability = pImpl->developmentSystem->getNeuralExcitability();
    adjustNeuralExcitability(excitability);
    
    // Memory consolidation affected by development
    if (pImpl->developmentSystem->isCriticalPeriod()) {
        consolidateMemoryWithEnhancedRehearsal();
    }
    
    // Attention modulation
    if (pImpl->attentionalSelection) {
        float attentionBias = pImpl->developmentSystem->getAttentionBias();
        pImpl->attentionalSelection->setTopDownBias(attentionBias);
    }
}
```

### 6. Replay and Consolidation

**Currently**: Episodic memory replay stub exists
**To Implement**:

```cpp
void Brain::performMemoryConsolidation() {
    if (pImpl->episodicMemory) {
        // Sleep/rest cycle simulation
        performReplayOfImportantEpisodes();
        consolidateRecentlyLearnedInformation();
        strengthenStrongConnections();
    }
}

void Brain::performReplayOfImportantEpisodes() {
    auto episodes = pImpl->episodicMemory->getEpisodesForReplay(10);
    for (auto& episode : episodes) {
        // Replay episode to strengthen connections
        replayEpisodeToConsolidate(episode);
    }
}
```

## Integration Checklist

### Priority 1: Connect Existing Systems
- [ ] Connect working memory to sensory processing
- [ ] Connect episodic memory to experience logging
- [ ] Connect prediction system to sensory processing
- [ ] Connect neural planner to action selection
- [ ] Connect concept formation to experience processing
- [ ] Integrate neuromodulators with attention systems
- [ ] Connect self-model to motor output tracking

### Priority 2: Enhance Development Effects
- [ ] Development affects neural excitability
- [ ] Development affects attention mechanisms
- [ ] Development modulates memory consolidation
- [ ] Development tunes neuromodulation baseline
- [ ] Development regulates synaptic pruning criteria

### Priority 3: Complete Integration
- [ ] Implement full sleep/rest cycle
- [ ] Connect all neuromodulators to multiple systems
- [ ] Ensure prediction errors modulate plasticity globally
- [ ] Integrate social learning observation mechanisms
- [ ] Connect developmental stage transitions

## Integration Example

### Complete Brain Loop with All Systems

```cpp
void Brain::step(SimulationStep step, Timestamp time) {
    // 1. Process neural dynamics (unchanged)
    pImpl->spikeSystem->processDelayedSpikes(step, time);
    updateNeurons(step, time);
    detectSpikes(step);
    
    // 2. Apply working memory (already connected)
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(time);
    }
    
    // 3. Apply neuromodulation (already enhanced)
    applyNeuromodulation();
    
    // 4. Apply plasticity rules (enhanced)
    applyPlasticityWithNeuromodulation();
    
    // 5. Apply associative memory (new integration)
    if (pImpl->associativeMemory) {
        integrateWithWorkingMemory();
    }
    
    // 6. Apply prediction system (new integration)
    if (pImpl->predictionSystem) {
        updatePredictions();
    }
    
    // 7. Apply cognitive systems (new integration)
    applyCognitiveSystems();
    
    // 8. Apply development effects (enhanced)
    applyDevelopmentEffects();
    
    // 9. Perform memory consolidation (new integration)
    performMemoryConsolidation();
    
    // 10. Apply structural plasticity (unchanged)
    if (step % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}
```

## Integration Testing

To verify successful integration:

### Test 1: Memory Retention
```bash
cat << EOF > test_memory_integration.cpp
// Test that working memory maintains information across steps
// Test that episodic memory stores experiences
// Test that replay mechanisms work
EOF
```

### Test 2: Learning Progress
```bash
cat << EOF > test_learning_progress.cpp
// Test that prediction accuracy improves with integration
// Test that concept formation stabilizes over time
// Test that planning accuracy increases with experience
EOF
```

### Test 3: Developmental Effects
```bash
cat << EOF > test_development_integration.cpp
// Test that development stages affect multiple systems
// Test that critical periods enable learning
// Test that maturation changes system parameters
EOF
```

## Key Integration Points

1. **Sensory → Working Memory**: Store raw sensory patterns for later use
2. **Working Memory → Attention**: Attention selects what's processed next
3. **Attention → Concept Formation**: Form abstract representations
4. **Concept Formation → Planning**: Use concepts to evaluate action sequences
5. **Planning → Action Selection**: Execute chosen actions
6. **Action → Self-Model**: Update understanding of self effects
7. **Experience → Episodic Memory**: Store complete event traces
8. **Episodic Memory → Replay**: Consolidate important experiences
9. **Replay → Memory Integration**: Strengthen relevant connections
10. **Neuromodulation → Plasticity**: Guide learning based on reward and prediction error

This integration guide should help you connect the disconnected NLM systems into a coherent artificial brain that learns, remembers, and acts intelligently.