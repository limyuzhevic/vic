# Episodic Memory Integration Implementation

## Current Status

The episodic memory system is **partially integrated** but could be enhanced:

### What's Working (GOOD)
1. **Episodic memory storage** - Episodes are created every 10 steps
2. **Memory content** - Stores active neurons, neuron activations, and rewards
3. **Replay mechanism** - Episodes can be replayed for strengthening
4. **Consolidation** - Weak memories are consolidated periodically

### What's Missing (TO CONNECT)
1. **Sensory input integration** - Episodes should include sensory patterns
2. **Action integration** - Episodes should store actions taken
3. **Prediction integration** - Episodes should include predictions and prediction errors
4. **Working memory connection** - Current activity should be stored in working memory
5. **Sleep/rest cycle** - Need proper consolidation during rest

## Implementation Plan

### 1. Enhanced Episode Structure

The current EpisodicMemoryItem struct should be enhanced:

```cpp
struct EpisodicMemoryItem {
    Timestamp timestamp;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    float reward;
    
    // ADDITIONAL FIELDS FOR ENHANCED INTEGRATION:
    std::vector<float> sensoryPattern;      // From sensory input
    std::vector<MotorCommand> actions;       // Actions taken
    float predictionError;                   // Prediction error
    float novelty;                           // Novelty level
    
    // Meta information
    size_t episodeAge;                       // How old the episode is
    float importance;                        // Signal importance
};
```

### 2. Enhanced Episode Capture

Enhance `Brain::step()` episodic memory capture:

```cpp
// In Brain::step(), after episodic memory logic:

// ===== STEP 5: ENHANCED EPISODIC MEMORY CAPTURE =====
pImpl->stepsSinceLastEpisode++;
if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
    pImpl->stepsSinceLastEpisode = 0;
    
    if (pImpl->episodicMemory) {
        // Capture enhanced episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // 1. STORE NEURAL ACTIVITY (already implemented)
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
        
        // 2. STORE SENSORY INPUT (NEW - ENHANCED INTEGRATION)
        // Get sensory input from agent if available
        if (pImpl->sensoryNeurons.size() > 0) {
            // Create a simple sensory pattern representation
            std::vector<float> sensoryPattern;
            for (size_t i = 0; i < pImpl->sensoryNeurons.size() && i < 64; ++i) {
                float activation = getNeuronActivation(pImpl->sensoryNeurons[i]);
                sensoryPattern.push_back(activation);
            }
            episode.sensoryPattern = sensoryPattern;
        }
        
        // 3. STORE ACTIONS TAKEN (NEW - ENHANCED INTEGRATION)
        // Get motor command from brain
        auto action = produceAction();
        if (action) {
            episode.actions.push_back(action->getType());
        }
        
        // 4. STORE PREDICTION ERROR (NEW - ENHANCED INTEGRATION)
        if (pImpl->predictionSystem) {
            // Prediction error could be stored here
            episode.predictionError = pImpl->predictionError ? 
                pImpl->predictionError->getPredictionError() : 0.0f;
        }
        
        // 5. STORE NOVELTY (NEW - ENHANCED INTEGRATION)
        episode.novelty = pImpl->novelty ? 
            pImpl->novelty->getNoveltyLevel() : 0.0f;
        
        // 6. CALCULATE EPISODE IMPORTANCE (NEW)
        calculateEpisodeImportance(episode);
        
        // Store episode with enhanced content
        pImpl->episodicMemory->storeEpisode(episode);
    }
}
```

### 3. Enhanced Episodic Memory Class

Enhance `NeuralEpisodicMemory` class:

```cpp
class NeuralEpisodicMemory {
public:
    // ... existing methods ...
    
    // NEW: Enhanced storage with additional metrics
    void storeEpisode(const EpisodicMemoryItem& episode) {
        // Call base implementation
        storeEpisodeInternal(episode);
        
        // Update statistics
        totalEpisodesStored++;
        
        // Update importance tracking
        updateEpisodeImportance(episode);
        
        // Trigger replay if important
        if (episode.importance > importanceThreshold) {
            addToReplayQueue(episode);
        }
    }
    
    // NEW: Get episodes by type (sensory, action, etc.)
    std::vector<const EpisodicMemoryItem*> getEpisodesByType(
        EpisodeType type) const;
    
    // NEW: Calculate episode coherence
    float calculateEpisodeCoherence(const EpisodicMemoryItem& episode) const;
    
    // NEW: Sleep/rest consolidation
    void consolidateDuringSleep(float consolidationFactor);
    
    // NEW: Get memory statistics
    struct MemoryStats {
        float totalStorage;
        float replayFrequency;
        float consolidationEfficiency;
        float temporalCoherence;
    };
    
    MemoryStats getMemoryStats() const;
    
private:
    // Helper methods
    void updateEpisodeImportance(const EpisodicMemoryItem& episode);
    void addToReplayQueue(const EpisodicMemoryItem& episode);
};
```

### 4. Integration with Working Memory

Enhance working memory integration:

```cpp
// In Brain::step() after working memory update:

// Connect working memory to episodic memory
if (pImpl->workingMemory && pImpl->episodicMemory) {
    // Store working memory state in episodic memory
    auto workingMemoryPattern = pImpl->workingMemory->retrieve();
    if (!workingMemoryPattern.empty()) {
        // Create memory binding
        workingMemoryBinding binding;
        binding.timestamp = currentStep;
        binding.pattern = workingMemoryPattern;
        binding.memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        
        // Store binding
        storeWorkingMemoryBinding(binding);
    }
}
```

### 5. Integration with Prediction System

Enhanced prediction system integration:

```cpp
// In Brain::step() prediction system update:

if (pImpl->predictionSystem) {
    // Update prediction system with current state
    pImpl->predictionSystem->update(*this, currentStep);
    
    // Store prediction error in episodic memory
    float predictionError = pImpl->predictionError ? 
        pImpl->predictionError->getPredictionError() : 0.0f;
    
    if (std::abs(predictionError) > predictionErrorThreshold) {
        // Create prediction error episode
        EpisodicMemoryItem errorEpisode;
        errorEpisode.timestamp = currentStep;
        errorEpisode.predictionError = predictionError;
        errorEpisode.importance = std::abs(predictionError) * errorImportanceFactor;
        
        pImpl->episodicMemory->storeEpisode(errorEpisode);
    }
}
```

### 6. Sleep/Rest Cycle Integration

Implement sleep/rest cycle with memory consolidation:

```cpp
// Add sleep/rest cycle handler
void Brain::handleSleepCycle(SimulationStep currentStep) {
    if (!pImpl->isResting) return;
    
    // Check if sleep period completed
    if (currentStep - pImpl->sleepStartStep >= pImpl->sleepDuration) {
        // End sleep
        endSleepCycle();
        return;
    }
    
    // During sleep, consolidate memories
    if (pImpl->episodicMemory) {
        // Strong consolidation during sleep
        pImpl->episodicMemory->consolidate(0.7f);  // Higher consolidation rate
        
        // Replay important episodes during sleep
        if (currentStep % pImpl->sleepReplayInterval == 0) {
            auto importantEpisodes = pImpl->episodicMemory->getImportantEpisodes(5);
            for (const auto* episode : importantEpisodes) {
                pImpl->episodicMemory->replayEpisode(episode, true);  // Replay during sleep
            }
        }
    }
    
    // Decay neuromodulation levels
    if (pImpl->dopamine) {
        pImpl->dopamine->decayDuringSleep();
    }
}
```

### 7. Replay Enhancement

Enhanced replay mechanism:

```cpp
// In NeuralEpisodicMemory::replayEpisode:
void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode, bool duringSleep) {
    if (!episode) return;
    
    // Inject episode content into neural network
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = episode->neuronActivations[i];
        
        // Stronger replay during sleep
        float replayStrength = duringSleep ? replayStrengthSleep : replayStrengthAwake;
        
        // Inject current to reactivate episode
        brain_->injectCurrent(neuron, activation * replayStrength);
    }
    
    // Update synaptic connections based on replay
    if (duringSleep) {
        // Weaker synaptic changes during sleep (consolidation only)
        replaySynapticChanges(episode, false);
    } else {
        // Stronger synaptic changes during awake replay
        replaySynapticChanges(episode, true);
    }
}
```

### 8. Attention Integration

Connect episodic memory to attention:

```cpp
// In AgentBrain::processSensoryInput:
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // ... existing sensory processing ...
    
    // Connect to episodic memory for attention
    if (brain_ && brain_->getEpisodicMemory()) {
        // Get recent episodes related to this sensory input
        auto relatedEpisodes = brain_->getEpisodicMemory()->
            getRelatedEpisodes(percept, currentTime, timeWindow);
        
        if (!relatedEpisodes.empty()) {
            // Adjust attention based on episode relevance
            for (const auto* episode : relatedEpisodes) {
                float relevance = calculateEpisodeRelevance(episode, percept);
                applyAttentionBias(episode->activeNeurons, relevance);
            }
        }
    }
}
```

## Implementation Priority

1. ✅ Current episodic memory storage (needs enhancement)
2. 🔄 Add sensory pattern storage
3. 🔄 Add action storage
4. 🔄 Add prediction error storage
5. 🔄 Add novelty storage
6. 🔄 Calculate episode importance
7. 🔄 Integrate with working memory
8. 🔄 Implement sleep/rest cycle
9. 🔄 Enhance replay mechanism
10. 🔄 Connect to attention system

## Testing Implementation

```bash
# Test enhanced episodic memory
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase6_demo

# Expected output:
# - Enhanced episodic memory storage
# - Sleep/rest cycle functionality
# - Memory consolidation during sleep
# - Replay of important episodes
# - Integration with working memory
```

## Expected Improvements

1. **Memory Retention**: Episodic memory stores rich, detailed experiences
2. **Temporal Coherence**: Episodes maintain temporal relationships
3. **Sleep Consolidation**: Memories strengthened during rest
4. **Replay Efficiency**: Important episodes replayed for consolidation
5. **Attention Guidance**: Past experiences guide attention
6. **Prediction Learning**: Prediction errors stored for learning

This enhancement makes episodic memory a true integration hub, connecting all other systems together.