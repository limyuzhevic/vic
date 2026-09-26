# NLM Phase 6 Integration Examples

This document provides examples of the new advanced features in the Phase 6 integrated NLM brain system.

## 1. Concept Formation Example

### Overview
Concept formation allows the brain to abstract patterns from experiences and form stable concept prototypes. In Phase 6, concepts are continuously learned from neural activity patterns.

### Basic Concept Formation
```cpp
// In Brain::step(), concept formation is integrated:
if (pImpl->conceptFormation && pImpl->dopamine) {
    // Extract neural activity pattern from working memory
    std::vector<float> pattern;
    const auto& memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
    
    for (NeuronId neuronId : memoryNeurons) {
        // Extract activation level for each neuron
        float activation = 0.0f;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        const auto& state = neuron->getState();
                        activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
                        break;
                    }
                }
            }
        }
        pattern.push_back(activation);
    }
    
    // Extract features from the pattern
    std::vector<float> features;
    if (!pattern.empty()) {
        // Feature 1: Pattern entropy (variation in neural activity)
        float mean = 0.0f;
        for (float val : pattern) mean += val;
        mean /= pattern.size();
        
        float variance = 0.0f;
        for (float val : pattern) variance += (val - mean) * (val - mean);
        variance /= pattern.size();
        features.push_back(std::sqrt(variance));
        
        // Feature 2: Proportion of neurons with high activity (> 0.5)
        size_t highActivityCount = 0;
        for (float val : pattern) {
            if (val > 0.5f) highActivityCount++;
        }
        features.push_back(static_cast<float>(highActivityCount) / pattern.size());
        
        // Feature 3: Global brain activity (firing rate)
        float globalFiringRate = static_cast<float>(pImpl->totalSpikesThisStep) / 
                                 std::max(1.0f, static_cast<float>(pImpl->getTotalNeuronCount()));
        features.push_back(globalFiringRate);
    }
    
    // Present experience to concept formation system
    float reward = pImpl->dopamine->getLevel();
    size_t conceptId = pImpl->conceptFormation->presentExperience(
        pattern, features, reward, currentStep);
}
```

### Advanced Concept Merging
```cpp
// Concepts can merge when they become similar
void ConceptFormation::mergeConcepts(size_t conceptA, size_t conceptB) {
    auto& protoA = concepts[conceptA].prototype;
    auto& protoB = concepts[conceptB].prototype;
    
    // Create new merged prototype
    std::vector<float> mergedPrototype;
    mergedPrototype.reserve(protoA.size() + protoB.size());
    mergedPrototype.insert(mergedPrototype.end(), protoA.begin(), protoA.end());
    mergedPrototype.insert(mergedPrototype.end(), protoB.begin(), protoB.end());
    
    // Normalize the merged prototype
    float sum = 0.0f;
    for (float val : mergedPrototype) sum += val;
    if (sum > 0.0f) {
        for (float& val : mergedPrototype) val /= sum;
    }
    
    // Update concept properties
    concepts[conceptA].prototype = std::move(mergedPrototype);
    concepts[conceptA].stability = (concepts[conceptA].stability + concepts[conceptB].stability) * 0.5f;
    concepts[conceptA].totalObservations += concepts[conceptB].totalObservations;
    
    // Remove merged concept
    concepts.erase(concepts.begin() + conceptB);
    
    // Update all concept IDs
    for (size_t i = conceptB; i < concepts.size(); ++i) {
        if (concepts[i].id > conceptB) {
            concepts[i].id--;
        }
    }
}
```

### Concept-Based Prediction
```cpp
// Concepts influence the prediction system
if (pImpl->predictionSystem && conceptId > 0) {
    // Get concept influence to guide prediction
    std::vector<float> conceptInfluence = pImpl->conceptFormation->getConceptInfluence(conceptId);
    if (!conceptInfluence.empty()) {
        // Use concept influence to update prediction system
        float conceptStability = pImpl->conceptFormation->getConceptStability(conceptId);
        pImpl->predictionSystem->updateParameters(
            pImpl->predictionSystem->getPredictionError(), // Use current error as learning rate
            pImpl->predictionSystem->getConfidence() * conceptStability, // Use concept stability as confidence
            pImpl->predictionSystem->getPredictionError(), // Novelty threshold based on error
            conceptInfluence.size() / 100.0f); // Concept influence weight based on influence magnitude
    }
}
```

## 2. Prediction System Example

### Overview
The prediction system anticipates future states based on current input and learned patterns. In Phase 6, it's integrated with concept formation for enhanced predictive capabilities.

### Basic Prediction Integration
```cpp
// In Brain::step(), prediction system is updated:
// ========== STEP 8: Update prediction system ==========
if (pImpl->predictionSystem) {
    // Update prediction system with current state
    auto currentInput = std::make_unique<InternalSignals>();
    
    // Get concept prototypes as prediction targets
    if (pImpl->conceptFormation) {
        auto conceptPrototypes = pImpl->conceptFormation->getConcepts();
        float totalConceptInfluence = 0.0f;
        
        // Integrate concept prototypes into predictions
        for (const auto& concept : conceptPrototypes) {
            // Get concept influence based on stability and observation count
            float conceptStability = conceptFormation->getConceptStability(concept.id);
            float influence = conceptStability * (concept.totalObservations / 10.0f);
            totalConceptInfluence += influence;
            
            // Use concept prototype as prediction target
            std::vector<float> conceptInfluence = conceptFormation->getConceptInfluence(concept.id);
            if (!conceptInfluence.empty()) {
                // Blend concept prototype with prediction system
                for (size_t i = 0; i < std::min(conceptInfluence.size(), currentInput->getData().size()); ++i) {
                    currentInput->getData()[i] = currentInput->getData()[i] * (1.0f - pImpl->predictionSystem->getConceptUpdateSignal()) + 
                                                 conceptInfluence[i] * pImpl->predictionSystem->getConceptUpdateSignal() * influence;
                }
            }
        }
    }
    
    // Update prediction system with current input
    pImpl->predictionSystem->updatePredictions(*currentInput, *currentInput);
    
    // Log prediction with concept integration
    float predictionError = pImpl->predictionSystem->getPredictionError();
    float confidence = pImpl->predictionSystem->getConfidence();
    NLM_LOG_DEBUG("PredictionSystem: Error={}, Confidence={}, Concepts={}", 
                 predictionError, confidence, totalConceptInfluence);
}
```

### Prediction Error Integration
```cpp
// Prediction error affects concept formation
// In ConceptFormation::presentExperience():
if (pImpl->predictionSystem && pImpl->conceptFormation) {
    float predictionError = pImpl->predictionSystem->getPredictionError();
    
    // Update all concepts based on prediction error
    // High prediction error indicates the concept may need refinement
    if (predictionError > 0.1f && predictionError < 0.9f) {
        const auto& concepts = pImpl->conceptFormation->getConcepts();
        for (const auto& concept : concepts) {
            if (pImpl->conceptFormation->getConceptStability(concept.id) > 0.5f) {
                // Update concept prototype using prediction error as guidance
                std::vector<float> dummyPattern(concept.prototype.size(), predictionError);
                pImpl->conceptFormation->updateConceptFromError(concept.id, 
                                                             concept.prototype, 
                                                             dummyPattern, 
                                                             predictionError);
            }
        }
    }
}
```

### Concept-to-Prediction Integration
```cpp
// Concepts provide targets for prediction learning
void PredictionSystem::updateFromConcepts(const ConceptFormation& conceptFormation) {
    const auto& concepts = conceptFormation.getConcepts();
    
    // Create prediction targets from stable concepts
    for (const auto& concept : concepts) {
        if (conceptFormation.getConceptStability(concept.id) > 0.7f) {
            // Add concept as a prediction target
            addConceptTarget(concept);
            
            // Use concept characteristics to adjust prediction parameters
            float conceptEntropy = calculateConceptEntropy(concept.prototype);
            adjustPredictionParameters(conceptEntropy, concept.totalObservations);
        }
    }
}
```

## 3. Neural Planning Example

### Overview
The neural planner generates action sequences based on predicted outcomes and current goals. In Phase 6, planning is integrated with attention and prediction systems.

### Basic Planning Integration
```cpp
// In Brain::step(), planning is enabled through the NeuralPlanner:
// ========== STEP 10: Update concept formation ==========
// (Planning is accessed through the concept system and attention)
if (pImpl->conceptFormation && pImpl->dopamine) {
    // Extract neural activity pattern from working memory
    std::vector<float> pattern;
    // ... (pattern extraction code as in concept formation example)
    
    // Present experience to concept formation system
    float reward = pImpl->dopamine->getLevel();
    size_t conceptId = pImpl->conceptFormation->presentExperience(
        pattern, features, reward, currentStep);
    
    // Neural planning can use concepts to evaluate action sequences
    if (conceptId > 0 && pImpl->planner) {
        // Get concept as a planning target
        auto conceptProto = pImpl->conceptFormation->getConceptPrototype(conceptId);
        
        // Use concept to guide action selection
        pImpl->planner->setGoal(conceptProto);
        
        // Plan action sequence
        std::vector<ActionSequence> plans = pImpl->planner->planActions(
            currentInput, conceptProto, 
            pImpl->predictionSystem, 
            currentStep);
    }
}
```

### Integrated Planning with Prediction
```cpp
// NeuralPlanner uses prediction system for outcome evaluation
std::vector<ActionSequence> NeuralPlanner::planActions(
    const InternalSignals& input,
    const std::vector<float>& goal,
    const PredictionSystem* predictionSystem,
    SimulationStep currentStep) {
    
    std::vector<ActionSequence> plans;
    
    // Generate possible action sequences
    std::vector<ActionSequence> candidates = generateActionCandidates(input);
    
    // Evaluate each candidate using prediction system
    for (auto& sequence : candidates) {
        // Predict outcome of this action sequence
        InternalSignals predictedOutcome;
        if (predictionSystem->predictOutcome(sequence.actions, predictedOutcome)) {
            // Calculate sequence utility
            float utility = calculateSequenceUtility(
                sequence, predictedOutcome, goal,
                predictionSystem->getPredictionError());
            
            sequence.utility = utility;
            plans.push_back(sequence);
        }
    }
    
    // Select best plan
    std::sort(plans.begin(), plans.end(), 
              [](const ActionSequence& a, const ActionSequence& b) {
                  return a.utility > b.utility;
              });
    
    return plans;
}
```

### Attention-Guided Planning
```cpp
// Attention system prioritizes planning targets
void NeuralPlanner::applyAttention(const AttentionalSelection& attention) {
    // Get attentional focus from attention system
    AttentionWeights focus = attention.getCurrentFocus();
    
    // Prioritize planning for attended concepts
    for (auto& sequence : actionSequences) {
        float attentionWeight = focus.getWeightForTarget(sequence.target);
        sequence.utility *= attentionWeight;
    }
    
    // Update planning goals based on attention
    AttentionTargets attendedTargets = attention.getAttendedTargets();
    for (const auto& target : attendedTargets) {
        setGoalPriority(target, 2.0f); // High priority for attended targets
    }
}
```

## 4. Memory System Usage Example

### Overview
Phase 6 memory systems provide episodic storage, working memory with competition, and associative connections. This example shows how these systems integrate.

### Working Memory Competition Example
```cpp
// Working memory maintains active traces through competition
void NeuralWorkingMemory::update(TimestepDuration dt) {
    // Decay inactive traces
    for (auto& trace : traces) {
        if (!trace.isActive()) {
            trace.decay(dt);
        }
    }
    
    // Competition for limited capacity
    std::vector<Trace>& activeTraces = getActiveTraces();
    if (activeTraces.size() > capacity) {
        // Select strongest traces through competition
        std::partial_sort(activeTraces.begin(), activeTraces.begin() + capacity,
                         activeTraces.end(),
                         [](const MemoryTrace& a, const MemoryTrace& b) {
                             return a.strength > b.strength;
                         });
        
        // Remove weakest traces
        activeTraces.resize(capacity);
    }
    
    // Update trace strengths based on neural activity
    for (auto& trace : activeTraces) {
        updateTraceFromNeuralActivity(trace);
    }
}
```

### Episodic Memory Storage Example
```cpp
// Episodic memory stores experienced episodes
void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create new episode entry
    EpisodeEntry entry;
    entry.timestamp = episode.timestamp;
    entry.reward = episode.reward;
    entry.activeNeurons = episode.activeNeurons;
    entry.neuronActivations = episode.neuronActivations;
    entry.features = extractFeatures(episode);
    
    // Store in memory
    episodes.push_back(entry);
    
    // Update statistics
    totalEpisodes++;
    
    // Trigger replay for important episodes
    if (entry.reward > importanceThreshold || 
        entry.neuronActivations.size() > highActivityThreshold) {
        markForReplay(&entry);
    }
}
```

### Memory Replay Example
```cpp
// Memory replay reinforces important experiences
void NeuralEpisodicMemory::replayEpisode(const EpisodeEntry* episode) {
    // Reactivate neural patterns from the episode
    for (NeuronId neuronId : episode->activeNeurons) {
        // Find the neuron and reactivate it
        for (auto& region : brainRegions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        // Reactivate with original activation level
                        float activation = episode->neuronActivations[neuronId] * 2.0f; // Enhance during replay
                        neuron->injectCurrent(activation);
                        
                        // Update working memory traces
                        if (workingMemory) {
                            workingMemory->updateTrace(neuronId, activation);
                        }
                        break;
                    }
                }
            }
        }
    }
    
    // Consolidate the episode
    consolidateEpisode(episode);
}
```

### Associative Memory Example
```cpp
// Associative memory connects related patterns
void NeuralAssociativeMemory::associate(const std::vector<float>& pattern1,
                                       const std::vector<float>& pattern2,
                                       float strength) {
    // Create connection between similar patterns
    size_t pattern1Id = addPattern(pattern1);
    size_t pattern2Id = addPattern(pattern2);
    
    // Create association
    Association assoc;
    assoc.pattern1Id = pattern1Id;
    assoc.pattern2Id = pattern2Id;
    assoc.strength = strength;
    assoc.lastAccess = currentStep;
    
    associations.push_back(assoc);
    
    // Update pattern statistics
    patterns[pattern1Id].connections.push_back(pattern2Id);
    patterns[pattern2Id].connections.push_back(pattern1Id);
}
```

## 5. Neuromodulation Example

### Overview
Neuromodulation systems (Dopamine, Serotonin, Norepinephrine, Acetylcholine) affect neural dynamics, plasticity, and behavior. In Phase 6, they're fully integrated.

### Dopamine Integration Example
```cpp
// Dopamine affects neural excitability and plasticity
void Dopamine::update(TimestepDuration dt) {
    // Update dopamine level based on reward prediction error
    float rewardError = calculateRewardPredictionError();
    level_ = std::clamp(level_ + rewardError * learningRate, -1.0f, 1.0f);
    
    // Apply effects to neural excitability
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine increases excitability (lower effective threshold)
                    float excitabilityMod = level_ * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Apply effects to plasticity
    float plasticityFactor = getPlasticityFactor();
    if (brain_ && plasticitySystem_) {
        // Modulate STDP parameters
        plasticitySystem_->setLTPWeight(plasticitySystem_->getLTPWeight() * plasticityFactor);
        plasticitySystem_->setLTDWeight(plasticitySystem_->getLTDWeight() * plasticityFactor);
    }
}
```

### Curiosity Integration Example
```cpp
// Curiosity drives exploration behavior
void Curiosity::update(TimestepDuration dt) {
    // Curiosity is a function of novelty and prediction error
    float curiosityLevel = noveltyLevel_ * 2.0f + predictionError_ * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel, 0.0f, 1.0f);
    
    // Apply curiosity effects to behavior
    if (brain_ && agentBrain_) {
        // High curiosity increases exploration
        if (curiosityLevel_ > explorationThreshold) {
            // Increase random action selection
            agentBrain_->enableCuriosityExploration(true);
        } else {
            agentBrain_->enableCuriosityExploration(false);
        }
        
        // Apply curiosity to working memory search
        if (workingMemory_) {
            // Curiosity enhances exploration in working memory
            workingMemory_->setSearchIntensity(curiosityLevel_);
        }
    }
}
```

### Novelty Integration Example
```cpp
// Novelty detection identifies new experiences
void Novelty::update(TimestepDuration dt) {
    // Calculate novelty as difference from expectations
    float currentNovelty = calculateCurrentNovelty();
    
    // Decay novelty over time
    noveltyLevel_ = noveltyLevel_ * decayFactor + currentNovelty * (1.0f - decayFactor);
    
    // Novelty affects memory encoding
    if (brain_ && episodicMemory_) {
        // High novelty enhances episodic memory encoding
        if (noveltyLevel_ > encodingThreshold) {
            // Increase memory consolidation
            episodicMemory_->setConsolidationBoost(noveltyLevel_);
        }
    }
    
    // Novelty affects attention
    if (attention_) {
        // High novelty captures attention
        attention_->setNoveltyBias(noveltyLevel_);
    }
}
```

### Prediction Error Integration Example
```cpp
// Prediction error signals learning opportunities
void PredictionError::update(TimestepDuration dt) {
    // Calculate prediction error
    predictionError_ = calculatePredictionError();
    
    // Apply prediction error to neuromodulation
    if (dopamine_) {
        // Prediction error drives dopamine release
        dopamine_->signalRewardPredictionError(predictionError_);
    }
    
    // Apply prediction error to attention
    if (attention_) {
        // High prediction error increases attentional focus
        attention_->setPredictionErrorBias(std::abs(predictionError_));
    }
    
    // Apply prediction error to memory
    if (brain_ && workingMemory_) {
        // Prediction error enhances working memory updating
        workingMemory_->setLearningRate(std::min(1.0f, std::abs(predictionError_)));
    }
}
```

## Integration Examples Summary

These examples demonstrate how Phase 6 systems integrate:

1. **Concept Formation**: Creates abstract patterns from neural activity for prediction targets
2. **Prediction System**: Uses concepts to improve predictions and generates prediction errors for learning
3. **Neural Planning**: Plans action sequences using concepts as goals and predictions as outcomes
4. **Memory Systems**: Provide storage for experiences, maintain active traces, and replay important episodes
5. **Neuromodulation**: Coordinate learning, attention, and behavior through dopamine, curiosity, novelty, and prediction error

The integration enables the brain to:
- Learn abstract concepts from experience
- Predict future states for better planning
- Maintain working memory of current goals
- Store and replay episodic memories
- Adapt behavior through neuromodulation

All systems work together in the brain loop to create a coherent artificial brain that develops and learns from interaction with the environment.

<document>
<document>
<document>