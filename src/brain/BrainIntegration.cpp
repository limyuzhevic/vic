// Add this include to Brain.cpp
#include "../agent/WorkingMemoryIntegrator.hpp"
#include "../agent/Experience.hpp"
#include "../agent/BrainState.hpp"

namespace nlm {

// Enhanced Brain::step() method with full system integration
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Update current step and time
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== PHASE 6: COMPREHENSIVE BRAIN INTEGRATION ==========
    
    // 1. Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // 2. Update all neurons (LIF dynamics) with potential neuromodulation
    applyNeuromodulationToNeurons();
    
    // 3. Update working memory with current sensory state
    updateWorkingMemoryWithCurrentState();
    
    // 4. Detect spikes and schedule spike events
    detectAndScheduleSpikes();
    
    // 5. Apply plasticity rules (STDP and Hebbian) 
    updatePlasticityRules();
    
    // 6. Apply structural plasticity periodically (every 100 steps)
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // 7. Update prediction system
    updatePredictionSystem(currentStep);
    
    // 8. Update cognitive systems (attention, planning, concept formation)
    updateCognitiveSystems(currentStep);
    
    // 9. Update development system
    updateDevelopmentSystem(currentTime);
    
    // 10. Apply reward-based neuromodulation
    applyRewardModulationToPlasticity();
    
    // 11. Update memory consolidation and replay
    updateMemoryConsolidation(currentStep);
    
    // 12. Check for checkpoint saves
    if (currentStep % pImpl->consolidationInterval == 0) {
        saveCheckpoint();
    }
}

// Update neurons with neuromodulation effects
void Brain::applyNeuromodulationToNeurons() {
    if (!pImpl->dopamine) return;
    
    float dopamineLevel = pImpl->dopamine->getCurrentLevel();
    float explorationValue = pImpl->curiosity ? pImpl->curiosity->getCurrentLevel() : 0.0f;
    
    // Apply dopamine effects on neuronal excitability
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        
        // Get all neurons in this region
        auto neurons = region->getAllNeurons();
        
        for (Neuron* neuron : neurons) {
            // Dopamine modulates neuronal response properties
            if (dopamineLevel > 0.0f) {
                // Enhance firing probability for reward-associated neurons
                neuron->modulateFiringProbability(1.0f + dopamineLevel * 0.5f);
            } else if (dopamineLevel < 0.0f) {
                // Suppress firing for punishment-associated neurons
                neuron->modulateFiringProbability(1.0f + dopamineLevel * 0.3f);
            }
            
            // Apply curiosity-driven exploration modulation
            if (explorationValue > 0.3f) {
                neuron->increaseExplorationDrive(explorationValue * 2.0f);
            }
        }
    }
}

// Update working memory with current brain state
void Brain::updateWorkingMemoryWithCurrentState() {
    if (!pImpl->workingMemory) return;
    
    // Create current brain state summary
    BrainState state;
    state.step = pImpl->currentStep;
    state.timestamp = pImpl->currentTime;
    
    // Add current neural activity patterns
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto neurons = region->getAllNeurons();
        
        std::vector<float> activityPattern;
        activityPattern.reserve(neurons.size());
        
        for (Neuron* neuron : neurons) {
            float activity = neuron->getCurrentActivity();
            activityPattern.push_back(activity);
        }
        
        state.neuralActivity[static_cast<size_t>(RegionId(i + 1))] = activityPattern;
    }
    
    // Integrate with working memory
    if (auto* agentBrain = dynamic_cast<AgentBrain*>(this)) {
        // Convert sensory percept to experience
        // Note: In real integration, this would come from agent observation
        Experience experience;
        experience.state = state;
        experience.step = pImpl->currentStep;
        experience.timestamp = pImpl->currentTime;
        
        // Store in working memory
        pImpl->workingMemory->updateWorkingMemory(experience);
    }
}

// Detect spikes and schedule spike events
void Brain::detectAndScheduleSpikes() {
    // Update all neurons
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        
        // Use NeuralRegion::detectSpikes() to detect threshold crossings
        region->detectSpikes();
        
        // Get firing neurons for spike events
        auto firingNeurons = region->getFiringNeurons();
        
        // Schedule spike events for each firing neuron
        for (Neuron* neuron : firingNeurons) {
            if (neuron->hasFired()) {
                SpikeEvent event;
                event.source_neuron = neuron->getId();
                event.source_region = region.getId();
                event.timestamp = pImpl->currentTime;
                event.weight = neuron->getSynapticWeight();
                event.is_excitatory = true; // Simplified for now
                
                pImpl->spikeSystem->scheduleSpikeEvent(event);
                
                // Count for statistics
                ++pImpl->totalSpikesThisStep;
            }
        }
    }
}

// Update plasticity rules with neuromodulation
void Brain::updatePlasticityRules() {
    if (!pImpl->stdp || !pImpl->hebbian) return;
    
    // Apply STDP with dopamine modulation
    float dopamineLevel = pImpl->dopamine ? pImpl->dopamine->getCurrentLevel() : 0.0f;
    
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto synapses = region->getSynapses();
        
        for (auto* synapse : synapses) {
            // Apply STDP
            float stdpChange = pImpl->stdp->calculateChange(synapse, pImpl->currentTime);
            
            // Apply Hebbian plasticity
            float hebbianChange = pImpl->hebbian->calculateChange(synapse);
            
            // Combine with dopamine modulation
            if (dopamineLevel != 0.0f) {
                float totalChange = (stdpChange + hebbianChange) * (1.0f + std::abs(dopamineLevel) * 0.5f);
                
                if (totalChange != 0.0f) {
                    synapse->addToWeight(totalChange);
                }
            } else {
                // No dopamine modulation
                if (stdpChange + hebbianChange != 0.0f) {
                    synapse->addToWeight(stdpChange + hebbianChange);
                }
            }
        }
    }
}

// Update prediction system
void Brain::updatePredictionSystem(SimulationStep step) {
    if (!pImpl->predictionSystem) return;
    
    // Create prediction target from current sensory context
    PredictionTarget target;
    target.step = step;
    target.timestamp = pImpl->currentTime;
    
    // Generate predictions based on current working memory
    if (pImpl->workingMemory && pImpl->workingMemory->getActiveTraces() > 0) {
        // Get most salient working memory traces
        auto traces = pImpl->workingMemory->getSalientTraces(10);
        
        // Use traces to generate predictions
        for (const auto& trace : traces) {
            Prediction prediction;
            prediction.predictedState = trace.features;
            prediction.confidence = trace.priority;
            prediction.uncertainty = 1.0f - trace.priority;
            
            target.predictions.push_back(prediction);
        }
        
        // Update prediction system with new targets
        pImpl->predictionSystem->updatePredictions(target);
        
        // Compute prediction errors
        auto predictionErrors = pImpl->predictionSystem->computeErrors();
        
        // Use prediction errors for learning
        if (!predictionErrors.empty()) {
            float averageError = 0.0f;
            for (const auto& error : predictionErrors) {
                averageError += error;
            }
            averageError /= static_cast<float>(predictionErrors.size());
            
            // Update prediction error neuromodulator
            if (pImpl->predictionError) {
                pImpl->predictionError->update(averageError);
            }
            
            // Apply prediction error to synaptic plasticity
            applyPredictionErrorToPlasticity(averageError);
        }
    }
}

// Update cognitive systems
void Brain::updateCognitiveSystems(SimulationStep step) {
    if (!pImpl->attention || !pImpl->planner || !pImpl->conceptFormation) return;
    
    // Update attentional selection based on working memory and prediction
    updateAttentionSystem(step);
    
    // Update concept formation based on sensory patterns
    updateConceptFormation(step);
    
    // Update neural planning based on current state
    updateNeuralPlanning(step);
}

// Update attentional selection system
void Brain::updateAttentionSystem(SimulationStep step) {
    // Get current attentional focus from attentional selection system
    auto focus = pImpl->attention->getCurrentFocus();
    
    // Get working memory traces
    auto traces = pImpl->workingMemory ? pImpl->workingMemory->getActiveTraces() : std::vector<WorkingMemoryTrace>();
    
    // Use attention to select most relevant traces
    std::vector<size_t> attendedTraceIndices;
    if (!traces.empty()) {
        // Rank traces by attention priority
        std::vector<std::pair<size_t, float>> rankedTraces;
        for (size_t i = 0; i < traces.size(); ++i) {
            float attentionScore = calculateAttentionScore(traces[i], focus);
            rankedTraces.push_back(std::make_pair(i, attentionScore));
        }
        
        // Sort by attention score (descending)
        std::sort(rankedTraces.begin(), rankedTraces.end(),
                  [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) {
                      return a.second > b.second;
                  });
        
        // Select top N traces for attention
        size_t maxAttended = std::min(static_cast<size_t>(10), rankedTraces.size());
        for (size_t i = 0; i < maxAttended; ++i) {
            attendedTraceIndices.push_back(rankedTraces[i].first);
        }
    }
    
    // Update attentional selection system
    pImpl->attention->setAttendedTraces(attendedTraceIndices);
    
    // Update working memory based on attention
    if (pImpl->workingMemory) {
        pImpl->workingMemory->applyAttention(attendedTraceIndices);
    }
}

// Update concept formation system
void Brain::updateConceptFormation(SimulationStep step) {
    if (!pImpl->conceptFormation) return;
    
    // Get current sensory percepts from working memory
    std::vector<ConceptInput> conceptInputs;
    
    if (pImpl->workingMemory) {
        auto traces = pImpl->workingMemory->getActiveTraces();
        
        for (const auto& trace : traces) {
            ConceptInput input;
            input.features = trace.features;
            input.salience = trace.priority;
            input.timestamp = pImpl->currentTime;
            input.step = step;
            
            conceptInputs.push_back(input);
        }
    }
    
    // Update concept formation with new inputs
    if (!conceptInputs.empty()) {
        pImpl->conceptFormation->updateConcepts(conceptInputs);
    }
}

// Update neural planning system
void Brain::updateNeuralPlanning(SimulationStep step) {
    if (!pImpl->planner) return;
    
    // Get current attentional focus
    auto focus = pImpl->attention ? pImpl->attention->getCurrentFocus() : AttentionFocus();
    
    // Get available motor commands from motor neurons
    std::vector<MotorCommand> availableCommands;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isActive()) {
            availableCommands.push_back(translateNeuronToCommand(neuron));
        }
    }
    
    // Get concept-based goals
    std::vector<Goal> goals;
    if (pImpl->conceptFormation) {
        goals = pImpl->conceptFormation->getCurrentGoals();
    }
    
    // Update planning system
    pImpl->planner->updatePlan(availableCommands, goals, focus);
}

// Update development system
void Brain::updateDevelopmentSystem(double timestep) {
    if (!pImpl->developmentSystem) return;
    
    // Update developmental stage based on current time
    pImpl->developmentSystem->updateDevelopmentStage(pImpl->currentTime);
    
    // Get current developmental stage
    auto stage = pImpl->developmentSystem->getCurrentStage();
    
    // Apply developmental effects based on stage
    applyDevelopmentalEffects(stage, timestep);
    
    // Update neuromodulation based on development
    updateDevelopmentNeuromodulation(stage);
}

// Apply developmental effects to brain systems
void Brain::applyDevelopmentalEffects(DevelopmentalStage stage, double timestep) {
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High plasticity, rapid learning
            if (pImpl->workingMemory) {
                pImpl->workingMemory->setCapacity(1000);
                pImpl->workingMemory->setTraceDecayRate(0.5f);
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for certain learning
            if (pImpl->conceptFormation) {
                pImpl->conceptFormation->setLearningRate(1.5f);
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Refining learned concepts
            if (pImpl->workingMemory) {
                pImpl->workingMemory->setTraceDecayRate(0.8f);
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Stabilized learning, memory consolidation
            if (pImpl->workingMemory) {
                pImpl->workingMemory->setTraceDecayRate(0.95f);
            }
            break;
    }
}

// Update neuromodulation based on development
void Brain::updateDevelopmentNeuromodulation(DevelopmentalStage stage) {
    if (!pImpl->dopamine) return;
    
    float currentDopamine = pImpl->dopamine->getCurrentLevel();
    
    // Modulate neuromodulation based on developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Higher exploration drive, dopamine baseline elevated
            pImpl->dopamine->setBaseline(0.3f);
            if (pImpl->curiosity) {
                pImpl->curiosity->setExplorationDrive(0.8f);
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Peak learning, high reward sensitivity
            pImpl->dopamine->setBaseline(0.2f);
            if (pImpl->curiosity) {
                pImpl->curiosity->setExplorationDrive(0.6f);
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Refining, moderate neuromodulation
            pImpl->dopamine->setBaseline(0.1f);
            if (pImpl->curiosity) {
                pImpl->curiosity->setExplorationDrive(0.4f);
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Stable, low neuromodulation for consolidation
            pImpl->dopamine->setBaseline(0.05f);
            if (pImpl->curiosity) {
                pImpl->curiosity->setExplorationDrive(0.2f);
            }
            break;
    }
}

// Apply reward modulation to plasticity
void Brain::applyRewardModulationToPlasticity() {
    if (!pImpl->dopamine) return;
    
    float dopamineLevel = pImpl->dopamine->getCurrentLevel();
    
    // Use agent brain to apply reward modulation if available
    if (auto* agentBrain = dynamic_cast<AgentBrain*>(this)) {
        agentBrain->applyRewardModulation(0.0f, 0.0f); // Will be called from experiment loop
    }
}

// Apply prediction error to plasticity
void Brain::applyPredictionErrorToPlasticity(float predictionError) {
    if (!pImpl->predictionError) return;
    
    float errorLevel = pImpl->predictionError->getCurrentLevel();
    
    // Apply prediction error to synaptic plasticity
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto synapses = region->getSynapses();
        
        for (auto* synapse : synapses) {
            // Use prediction error to modulate synaptic change
            float predictionErrorModulation = calculatePredictionErrorModulation(
                errorLevel, synapse->getWeight()
            );
            
            if (predictionErrorModulation != 0.0f) {
                synapse->addToWeight(predictionErrorModulation);
            }
        }
    }
}

// Update memory consolidation and replay
void Brain::updateMemoryConsolidation(SimulationStep step) {
    if (!pImpl->episodicMemory) return;
    
    // Check if it's time for memory consolidation
    if (step % pImpl->consolidationInterval == 0) {
        // Perform memory consolidation
        consolidateMemories(step);
        
        // Perform memory replay if enabled
        if (step % pImpl->replayInterval == 0) {
            replayMemories(step);
        }
    }
}

// Consolidate memories from working memory to episodic memory
void Brain::consolidateMemories(SimulationStep step) {
    if (!pImpl->workingMemory || !pImpl->episodicMemory) return;
    
    // Get most stable working memory traces
    auto stableTraces = pImpl->workingMemory->getStableTraces(5);
    
    for (const auto& trace : stableTraces) {
        // Create episodic memory entry from stable working memory trace
        EpisodicMemoryEntry entry;
        entry.step = step;
        entry.timestamp = pImpl->currentTime;
        entry.features = trace.features;
        entry.importance = trace.priority;
        entry.context = extractContextFromTrace(trace);
        
        // Store in episodic memory
        pImpl->episodicMemory->storeEpisode(entry);
    }
    
    // Update episodic memory consolidation metrics
    pImpl->episodicMemory->consolidateRecentEpisodes();
}

// Replay memories to strengthen connections
void Brain::replayMemories(SimulationStep step) {
    if (!pImpl->episodicMemory) return;
    
    // Get episodes suitable for replay
    auto replayEpisodes = pImpl->episodicMemory->getEpisodesForReplay(3);
    
    if (replayEpisodes.empty()) return;
    
    NLM_LOG_INFO("Memory replay: Replaying " + std::to_string(replayEpisodes.size()) + 
                 " episodic memories");
    
    // Replay each episode by reconstructing neural activity patterns
    for (const auto& episode : replayEpisodes) {
        replayEpisode(episode);
    }
    
    // Update episodic memory replay statistics
    pImpl->episodicMemory->updateReplayStats(step);
}

// Replay a specific episodic memory
void Brain::replayEpisode(const EpisodicMemoryEntry& episode) {
    if (!pImpl->workingMemory) return;
    
    // Reconstruct working memory traces from episodic memory
    WorkingMemoryTrace reconstructedTrace;
    reconstructedTrace.features = episode.features;
    reconstructedTrace.importance = episode.importance;
    reconstructedTrace.step = episode.step;
    
    // Store reconstructed trace in working memory
    pImpl->workingMemory->addActiveTrace(reconstructedTrace);
    
    // Apply replay effects to synaptic connections
    applyReplayEffects(episode);
}

// Apply replay effects to synaptic connections
void Brain::applyReplayEffects(const EpisodicMemoryEntry& episode) {
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto synapses = region->getSynapses();
        
        for (auto* synapse : synapses) {
            // Enhance synaptic strength based on episode importance
            float replayBoost = calculateReplayBoost(episode.importance, synapse->getWeight());
            
            if (replayBoost != 0.0f) {
                synapse->addToWeight(replayBoost);
            }
        }
    }
}

// Save checkpoint
void Brain::saveCheckpoint() {
    if (!pImpl->checkpointManager) return;
    
    // Create checkpoint with current brain state
    Checkpoint checkpoint;
    checkpoint.step = pImpl->currentStep;
    checkpoint.timestamp = pImpl->currentTime;
    checkpoint.brainState = collectBrainState();
    
    // Save checkpoint
    std::string checkpointPath = "./checkpoint_" + std::to_string(pImpl->currentStep) + ".bin";
    bool saved = pImpl->checkpointManager->save(checkpoint, checkpointPath);
    
    if (saved) {
        NLM_LOG_INFO("Checkpoint saved at step " + std::to_string(pImpl->currentStep));
    } else {
        NLM_LOG_WARN("Failed to save checkpoint at step " + std::to_string(pImpl->currentStep));
    }
}

// Collect current brain state for checkpoint
BrainState Brain::collectBrainState() {
    BrainState state;
    state.step = pImpl->currentStep;
    state.timestamp = pImpl->currentTime;
    
    // Collect synaptic weights
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto synapses = region->getSynapses();
        
        for (auto* synapse : synapses) {
            SynapseState synapseState;
            synapseState.source_neuron = synapse->getSourceNeuronId();
            synapseState.destination_neuron = synapse->getDestinationNeuronId();
            synapseState.weight = synapse->getWeight();
            synapseState.region_id = region.getId();
            
            state.synapses.push_back(synapseState);
        }
    }
    
    // Collect neuron states
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto neurons = region->getAllNeurons();
        
        for (Neuron* neuron : neurons) {
            NeuronState neuronState;
            neuronState.neuron_id = neuron->getId();
            neuronState.region_id = region.getId();
            neuronState.membrane_potential = neuron->getMembranePotential();
            neuronState.spike_history = neuron->getRecentSpikeHistory();
            
            state.neurons.push_back(neuronState);
        }
    }
    
    return state;
}

// Restore brain state from checkpoint
bool Brain::restoreBrainState(const BrainState& state) {
    // Clear current brain state
    clearCurrentBrainState();
    
    // Restore synaptic weights
    for (const auto& synapseState : state.synapses) {
        restoreSynapseState(synapseState);
    }
    
    // Restore neuron states
    for (const auto& neuronState : state.neurons) {
        restoreNeuronState(neuronState);
    }
    
    return true;
}

// Calculate attention score for a working memory trace
float Brain::calculateAttentionScore(const WorkingMemoryTrace& trace, const AttentionFocus& focus) {
    float score = 0.0f;
    
    // Consider trace importance
    score += trace.priority * 0.5f;
    
    // Consider match with attentional focus
    if (!focus.targetFeatures.empty()) {
        float featureMatch = calculateFeatureMatch(trace.features, focus.targetFeatures);
        score += featureMatch * 0.3f;
    }
    
    // Consider trace age (older traces have less attention)
    float ageScore = 1.0f - std::min(trace.step / 1000.0f, 1.0f);
    score += ageScore * 0.2f;
    
    return score;
}

// Calculate feature match between two feature sets
float Brain::calculateFeatureMatch(const std::vector<FeatureVector>& features1,
                                   const std::vector<FeatureVector>& features2) {
    if (features1.empty() || features2.empty()) return 0.0f;
    
    float totalMatch = 0.0f;
    float maxPossible = 0.0f;
    
    for (const auto& feature1 : features1) {
        for (const auto& feature2 : features2) {
            float match = calculateFeatureSimilarity(feature1, feature2);
            totalMatch += match;
        }
    }
    
    maxPossible = static_cast<float>(features1.size() * features2.size());
    
    return totalMatch / maxPossible;
}

// Calculate similarity between two features
float Brain::calculateFeatureSimilarity(const FeatureVector& feature1, const FeatureVector& feature2) {
    float valueSimilarity = 1.0f - std::abs(feature1.featureValue - feature2.featureValue);
    float salienceSimilarity = 1.0f - std::abs(feature1.salience - feature2.salience);
    
    // Spatial proximity factor
    float spatialSimilarity = 1.0f - calculateSpatialDistance(feature1.spatialCoordinates, feature2.spatialCoordinates);
    
    return (valueSimilarity + salienceSimilarity + spatialSimilarity) / 3.0f;
}

// Calculate spatial distance between two points
float Brain::calculateSpatialDistance(const Vector2D& point1, const Vector2D& point2) {
    float dx = point1.x - point2.x;
    float dy = point1.y - point2.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Calculate prediction error modulation for a synapse
float Brain::calculatePredictionErrorModulation(float errorLevel, float currentWeight) {
    // Prediction error modulates synaptic learning
    float learningRate = std::min(std::abs(errorLevel) * 0.1f, 0.1f);
    
    if (errorLevel > 0.0f) {
        // Positive prediction error (surprise) enhances learning
        return currentWeight * learningRate;
    } else {
        // Negative prediction error (expected outcome) reduces learning
        return -currentWeight * learningRate * 0.5f;
    }
}

// Calculate replay boost for synaptic enhancement
float Brain::calculateReplayBoost(float episodeImportance, float currentWeight) {
    // Replay effects are stronger for more important episodes
    float boostFactor = episodeImportance * 0.1f;
    
    // Boost is proportional to current weight (heavier weights get stronger boost)
    return currentWeight * boostFactor;
}

// Extract context from working memory trace
std::string Brain::extractContextFromTrace(const WorkingMemoryTrace& trace) {
    std::string context = "Trace_";
    
    // Add feature type information
    for (const auto& feature : trace.features) {
        switch (feature.featureType) {
            case FeatureType::Visual: context += "_V"; break;
            case FeatureType::Tactile: context += "_T"; break;
            case FeatureType::Internal: context += "_I"; break;
            case FeatureType::Proprioceptive: context += "_P"; break;
        }
    }
    
    // Add importance level
    context += "_" + std::to_string(static_cast<int>(trace.priority * 10));
    
    return context;
}

// Clear current brain state
void Brain::clearCurrentBrainState() {
    // Clear working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    
    // Clear episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clear();
    }
    
    // Reset synaptic weights to initial values
    resetSynapsesToInitialState();
}

// Reset synapses to initial state
void Brain::resetSynapsesToInitialState() {
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto synapses = region->getSynapses();
        
        for (auto* synapse : synapses) {
            // Reset to initial weight (0.1 for now)
            synapse->setWeight(0.1f);
        }
    }
}

// Restore synapse state from checkpoint
void Brain::restoreSynapseState(const SynapseState& synapseState) {
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        
        // Find the source and destination neurons
        auto neurons = region->getAllNeurons();
        
        for (Neuron* sourceNeuron : neurons) {
            if (sourceNeuron->getId() == synapseState.source_neuron) {
                // Find destination neuron in the same region or other regions
                for (auto& otherRegion : pImpl->regions) {
                    auto otherNeurons = otherRegion.getAllNeurons();
                    
                    for (Neuron* destNeuron : otherNeurons) {
                        if (destNeuron->getId() == synapseState.destination_neuron) {
                            // Set synaptic weight
                            destNeuron->setSynapticWeightFromSource(
                                sourceNeuron->getId(), synapseState.weight
                            );
                            return;
                        }
                    }
                }
            }
        }
    }
}

// Restore neuron state from checkpoint
void Brain::restoreNeuronState(const NeuronState& neuronState) {
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto& region = pImpl->regions[i];
        auto neurons = region.getAllNeurons();
        
        for (Neuron* neuron : neurons) {
            if (neuron->getId() == neuronState.neuron_id) {
                // Restore membrane potential
                neuron->setMembranePotential(neuronState.membrane_potential);
                
                // Restore spike history
                neuron->setRecentSpikeHistory(neuronState.spike_history);
                
                return;
            }
        }
    }
}

// Translate neuron activity to motor command
MotorCommand Brain::translateNeuronToCommand(Neuron* neuron) {
    if (!neuron) return MotorCommand::Wait;
    
    float activity = neuron->getCurrentActivity();
    
    // Threshold-based motor command selection
    if (activity > 0.8f) {
        // Determine which motor neuron based on neuron ID or type
        // This is a simplified version - real implementation would need mapping
        return MotorCommand::MoveForward; // Default to forward
    } else if (activity > 0.5f) {
        return MotorCommand::MoveBackward;
    } else if (activity > 0.3f) {
        return MotorCommand::TurnLeft;
    } else if (activity > 0.1f) {
        return MotorCommand::TurnRight;
    } else {
        return MotorCommand::Wait;
    }
}

} // namespace nlm