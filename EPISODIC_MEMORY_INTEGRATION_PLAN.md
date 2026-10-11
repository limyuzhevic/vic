# Episodic Memory Integration Implementation Plan

## Overview

The Episodic Memory Integration Gap is a critical issue in NLM. While episodic memory successfully stores experiences (episodes) and provides retrieval mechanisms (by similarity, time, location, action), it has no influence back on neural dynamics or other cognitive systems.

This plan implements integration between episodic memory and neural processing, prediction systems, and cognitive functions.

## Integration Strategy

### 1. Episodic Memory → Neural Activation
Episodic retrieval should reactivate relevant neural patterns for pattern completion and memory consolidation.

### 2. Neural Activity → Episodic Memory Formation
Sensory input and neural activity should seed episodic memory formation when experiences are significant.

### 3. Episodic Memory → Prediction System
Episodic patterns should inform predictive models and prediction error signals.

### 4. Episodic Memory → Concept Formation
Repeated episodic patterns should extract higher-level concepts and abstractions.

## Files to Modify

### 1. src/brain/Brain.cpp - Main Integration

**Changes to Brain::step() method (around line 480-510):**

```cpp
// ========== STEP 7: Update episodic memory ==========
pImpl->stepsSinceLastEpisode++;
if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
    pImpl->stepsSinceLastEpisode = 0;
    
    if (pImpl->episodicMemory) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store reward in episode
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Store sensory state if available
        // (This would require capturing sensory input during the step)
        
        // Store in episodic memory
        pImpl->episodicMemory->storeEpisode(episode);
        
        // NEW: Episodic memory integration - retrieve similar episodes and influence neural dynamics
        integrateEpisodicMemory(currentStep, currentTime);
    }
}
```

**Add new method `integrateEpisodicMemory()`:**
```cpp
void Brain::integrateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->episodicMemory || !pImpl->attention) return;
    
    // Get last sensory input for pattern matching
    // (This would require storing the last sensory input received)
    std::vector<float> lastSensoryInput = getLastSensoryInput();
    
    if (!lastSensoryInput.empty()) {
        // Retrieve similar episodes
        auto similarEpisodes = pImpl->episodicMemory->retrieveSimilar(lastSensoryInput, 3);
        
        // Reactivate neurons from similar episodes
        for (const auto* episode : similarEpisodes) {
            // Reactivate pattern
            pImpl->episodicMemory->replayEpisode(episode);
            
            // Use episode content to influence attention
            if (!episode->activeNeurons.empty()) {
                pImpl->attention->focusOnRegion(getRegion(RegionId(1))); // Default region
            }
        }
        
        // Use episodic memory to update prediction system
        if (pImpl->predictionSystem) {
            // Extract prediction targets from episodic patterns
            updatePredictionsFromEpisodicMemory(lastSensoryInput, similarEpisodes);
        }
        
        // Use episodic memory for concept formation
        if (pImpl->conceptFormation) {
            // Extract concepts from episodic patterns
            extractConceptsFromEpisodicMemory(lastSensoryInput, similarEpisodes);
        }
    }
}
```

**Add supporting methods:**
```cpp
// Store last sensory input for episodic memory integration
void Brain::storeSensoryInput(const std::vector<float>& input) {
    lastSensoryInput_ = input;
}

std::vector<float> Brain::getLastSensoryInput() const {
    return lastSensoryInput_;
}

// Update prediction system from episodic memory
void Brain::updatePredictionsFromEpisodicMemory(
    const std::vector<float>& currentInput,
    const std::vector<const EpisodicMemoryItem*>& episodes) {
    
    if (episodes.empty()) return;
    
    // Get prediction system
    auto* predictionSystem = getPredictionSystem();
    if (!predictionSystem) return;
    
    // Use similar episodes to generate predictions
    // This would involve:
    // 1. Extracting temporal sequences from episodes
    // 2. Building prediction models based on episode patterns
    // 3. Updating prediction error signals
    
    // Simplified implementation: use episode sequences to influence predictions
    for (size_t i = 0; i < episodes.size(); ++i) {
        const auto* episode = episodes[i];
        
        // If we have multiple similar episodes, check for temporal patterns
        if (i < episodes.size() - 1) {
            const auto* nextEpisode = episodes[i + 1];
            
            // Create prediction based on temporal relationship
            // This would update the prediction system with:
            // - Expected next sensory state
            // - Prediction error based on differences
            // - Confidence based on episode similarity
        }
    }
}

// Extract concepts from episodic memory
void Brain::extractConceptsFromEpisodicMemory(
    const std::vector<float>& currentInput,
    const std::vector<const EpisodicMemoryItem*>& episodes) {
    
    if (episodes.empty()) return;
    
    // Get concept formation system
    auto* conceptFormation = getConceptFormation();
    if (!conceptFormation) return;
    
    // Extract recurring patterns and abstractions
    // This would involve:
    // 1. Finding common activation patterns across episodes
    // 2. Identifying statistical regularities
    // 3. Forming higher-level concepts
    
    // Simplified implementation: apply bottom-up salience based on episode patterns
    for (const auto* episode : episodes) {
        // Use episode's active neurons to form concepts
        for (NeuronId neuron : episode->activeNeurons) {
            // Apply concept formation based on episode activation
            if (neuron < 10000) { // Assuming concept neurons have specific IDs
                conceptFormation->applyConcept(neuron, episode->neuronActivations.size() / 20.0f);
            }
        }
    }
}
```

### 2. src/memory/NeuralEpisodicMemory.cpp - Enhanced Integration

**Add new method to NeuralEpisodicMemory class:**

```cpp
// Retrieve episodes and influence neural dynamics
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAndReactivate(
    const std::vector<float>& pattern,
    Brain* brain) const {
    
    std::vector<const EpisodicMemoryItem*> similarEpisodes = retrieveSimilar(pattern, 3);
    
    // Reactivate neurons from similar episodes
    for (const auto* episode : similarEpisodes) {
        replayEpisode(episode, brain);
    }
    
    return similarEpisodes;
}

// Enhanced replay that also reinforces working memory
void NeuralEpisodicMemory::replayEpisode(
    const EpisodicMemoryItem* episode,
    Brain* brain) const {
    if (!episode || !brain) return;
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Inject current to reawaken this pattern
        brain->injectCurrent(neuron, activation * 3.0f);
        
        // Also store in working memory for maintenance
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->storeToNeuron(neuron, activation);
        }
    }
    
    // Also inject sensory pattern if available
    if (!episode->sensoryState.empty()) {
        // Send through sensory channels for pattern completion
        brain->receiveSensoryInput(episode->sensoryState);
    }
}
```

**Add method to get episode count:**
```cpp
// Get number of episodes
 size_t NeuralEpisodicMemory::getEpisodeCount() const {
    return episodes_.size();
}
```

### 3. src/agent/AgentBrain.cpp - Integration Support

**Add episodic memory awareness to AgentBrain:**

```cpp
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // ... existing code ...
    
    // NEW: Store sensory input for episodic memory
    if (brain_) {
        // Convert percept to vector for episodic storage
        std::vector<float> sensoryVector;
        
        // Add vision data
        const auto& vision = percept.getVision();
        sensoryVector.insert(sensoryVector.end(), vision.begin(), vision.end());
        
        // Add touch data
        const auto& touch = percept.getTouch();
        sensoryVector.insert(sensoryVector.end(), touch.begin(), touch.end());
        
        // Add internal data
        const auto& intern = percept.getInternal();
        sensoryVector.insert(sensoryVector.end(), intern.begin(), intern.end());
        
        // Store for episodic memory integration
        if (!sensoryVector.empty()) {
            brain_->storeSensoryInput(sensoryVector);
        }
    }
    
    // ... rest of existing code ...
}
```

### 4. Configuration Updates

**Add episodic memory configuration options:**

Update configs/default.cfg with:

```ini
# Episodic memory configuration
max_episodes=1000
similar_similarity_threshold=0.3f
replay_enabled=true
concept_extraction=true
```

## Implementation Steps

### Step 1: Core Episodic Memory Integration
1. Modify Brain::step() to call integrateEpisodicMemory()
2. Implement integrateEpisodicMemory() in Brain.cpp
3. Add supporting methods for sensory input storage
4. Test episodic memory retrieval and neural reactivation

### Step 2: Episodic Memory → Neural Influence
1. Implement enhanced replayEpisode() in NeuralEpisodicMemory.cpp
2. Add methods for working memory integration during replay
3. Test pattern completion through episodic reactivation
4. Test attention system integration with episodic memory

### Step 3: Episodic Memory → Prediction System
1. Implement updatePredictionsFromEpisodicMemory() in Brain.cpp
2. Add temporal pattern extraction from episodes
3. Test prediction system updates from episodic memory
4. Test prediction error signal integration

### Step 4: Episodic Memory → Concept Formation
1. Implement extractConceptsFromEpisodicMemory() in Brain.cpp
2. Add concept extraction from episode patterns
3. Test concept formation from episodic experiences
4. Test higher-level abstraction formation

### Step 5: Testing and Validation
1. Create episodic memory integration tests
2. Test episodic memory → neural dynamics coupling
3. Test episodic memory → prediction system integration
4. Test episodic memory → concept formation

## Expected Outcomes

After episodic memory integration, NLM should achieve:

1. **Functional Episodic Memory Integration**: Episodic memory content influences neural dynamics
2. **Pattern Completion**: Episodic retrieval reactivates relevant neural patterns
3. **Predictive Processing**: Episodic memory informs prediction system
4. **Concept Formation**: Episodic patterns extract higher-level concepts
5. **Memory-Episodic Integration**: All episodic memory components work together

## Performance Impact

- **Computational Overhead**: Low to moderate (adds ~O(n) operations per step)
- **Memory Usage**: Minimal (only stores additional references)
- **Learning Improvement**: Expected 15-25% improvement in episodic learning
- **Behavioral Adaptability**: Expected 20-30% improvement in complex task performance

This implementation addresses the Episodic Memory Integration Gap and provides a solid foundation for integrating other cognitive systems (Prediction System, Cognitive Systems, Neuromodulation Systems). The bidirectional nature of this integration ensures that episodic memory becomes truly integrated into the brain's cognitive architecture.

Once episodic memory integration is complete, the next priorities should be Prediction System Integration, followed by Cognitive Systems Integration, and finally Neuromodulation System Integration.

## Technical Details for Implementation

### Neural Reactivation Algorithm
The integration uses a similarity-based approach for neural reactivation:

1. **Pattern Matching**: Retrieve similar episodes based on current sensory input
2. **Pattern Completion**: Reactivate neurons from similar episodes
3. **Working Memory Integration**: Store reactivated patterns in working memory
4. **Attention Modulation**: Use episodic content to guide attention

### Integration Architecture
The integration uses a multi-layered architecture:

1. **Storage Layer**: Episodic memory stores experiences
2. **Retrieval Layer**: Episodic memory retrieves similar patterns
3. **Neural Influence Layer**: Retrieved patterns reactivate neural dynamics
4. **Cognitive Influence Layer**: Episodic patterns influence prediction and concept formation

This architecture allows for flexible integration without requiring major changes to existing systems.

## Testing Strategy

### Unit Tests
1. **EpisodicMemoryToNeural**: Test that episodic retrieval reactivates neural patterns
2. **NeuralToEpisodicMemory**: Test that neural activity strengthens episodic memory
3. **PredictionSystemIntegration**: Test episodic memory influence on predictions
4. **ConceptFormationIntegration**: Test episodic memory influence on concepts

### Integration Tests
1. **EpisodicMemoryDynamics**: Test overall episodic memory-neural dynamics coupling
2. **MemoryTasks**: Test episodic memory-dependent cognitive tasks
3. **BehavioralTests**: Test improvements in memory-based behavior
4. **PerformanceBenchmarks**: Compare before/after integration performance

### Validation Criteria
1. **Memory Influence**: Episodic retrieval should reactivate 50-100% of relevant neurons
2. **Pattern Accuracy**: Pattern completion should achieve >80% accuracy
3. **Prediction Improvement**: Prediction accuracy should improve by 15-25%
4. **Concept Formation**: Concept extraction should achieve >70% of potential concepts
5. **Integration Score**: Overall integration score should exceed 75%

This comprehensive episodic memory integration plan provides a solid foundation for Phase 6 NLM integration and addresses the second most critical integration gap in the current system.

The implementation is designed to be incremental, testable, and maintainable, ensuring that we can verify each integration point before moving to the next phase of integration.