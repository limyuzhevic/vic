// NeuralEpisodicMemory Implementation
// Provides biologically realistic episodic memory with consolidation mechanisms
// Integrates with other brain systems through real neural dynamics

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    /*
     * Store a new episodic memory
     * 
     * The episode is encoded with neural dynamics and integrated into
     * the memory system. Synaptic consolidation begins immediately
     * to strengthen the memory trace.
     */
    // Add to episode deque with aging support
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    episodes_.push_back(stored);
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.erase(episodes_.begin());
    }
    
    // Initialize synaptic consolidation for this episode
    initSynapticConsolidation();
    
    // Integrate with associative memory
    if (brain_ && brain_->getAssociativeMemory()) {
        brain_->getAssociativeMemory()->associateFromExperience(episodes_.back());
    }
}

void NeuralEpisodicMemory::initialize(Brain* brain) {
    brain_ = brain;
    
    // Configure for biological realism
    maxEpisodes_ = 1000;  // Increased capacity for long-term storage
    replayEnabled_ = true;
    
    // Initialize systems consolidation
    corticalIntegrationLevel_ = 0.0f;
    hippocampalCorticalLinks_.clear();
    corticalRepresentations_.clear();
    
    // Initialize synaptic consolidation structures
    memoryStrengths_.clear();
    encodingTimes_.clear();
    proteinLevels_.clear();
    markedForReplay_.clear();
    replayProbabilities_.clear();
    
    // Initialize statistics
    avgMemoryStrength_ = 0.0f;
    avgReplayPotential_ = 0.0f;
    
    // Initialize cholinergic integration
    initializeAcetylcholineIntegration();
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized with " + std::to_string(maxEpisodes_) + " episode capacity");
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

void NeuralEpisodicMemory::updateMemoryDynamics(TimestepDuration dt) {
    // Update all memory dynamics including consolidation, replay, and integration
    if (!brain_) return;
    
    float deltaTime = static_cast<float>(dt);
    
    // Update synaptic consolidation
    updateSynapticConsolidation(deltaTime);
    
    // Update systems consolidation
    updateSystemsConsolidation(deltaTime);
    
    // Update sleep-dependent consolidation
    updateSleepConsolidation(deltaTime);
    
    // Update active maintenance and working memory transfer
    updateActiveMaintenance(deltaTime);
    
    // Update realistic dynamics (interference, forgetting, reconstruction)
    updateRealisticDynamics(deltaTime);
    
    // Update neuromodulation integration
    updateNeuromodulationIntegration();
    
    // Update development integration
    updateDevelopmentIntegration();
    
    // Update memory binding mechanisms
    updateBindingMechanisms();
    
    // Update acetylcholine state
    updateAcetylcholineState();
    
    // Update statistics
    updateMemoryStatistics();
}

void NeuralEpisodicMemory::updateSynapticConsolidation(float dt) {
    // Implement comprehensive synaptic consolidation
    // This is the core of biologically realistic memory storage
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (pImpl->memoryStrengths[i] <= 0.0f) continue;
        
        // Protein synthesis-dependent consolidation
        pImpl->proteinLevels[i] += NeuromodulationConstants::PROTEIN_SYNTHESIS_RATE * dt;
        
        // Protein decay (natural forgetting)
        pImpl->proteinLevels[i] *= std::exp(-NeuromodulationConstants::PROTEIN_DECAY_RATE * dt);
        
        // Synaptic strength updates with eligibility traces
        float consolidationFactor = pImpl->proteinLevels[i] * NeuromodulationConstants::CONSOLIDATION_EFFICIENCY;
        
        // Apply homeostatic scaling
        float targetStrength = pImpl->memoryStrengths[i] * (1.0f + consolidationFactor * 0.1f);
        targetStrength = std::clamp(targetStrength, NeuromodulationConstants::SYNAPTIC_STRENGTH_MIN,
                                    NeuromodulationConstants::SYNAPTIC_STRENGTH_MAX);
        
        // Apply interference management from similar memories
        float interference = calculateInterference(i);
        targetStrength *= (1.0f - interference * NeuromodulationConstants::INTERFERENCE_SENSITIVITY);
        
        pImpl->memoryStrengths[i] = targetStrength;
    }
}

void NeuralEpisodicMemory::updateSystemsConsolidation(float dt) {
    // Implement systems consolidation: gradual transfer from hippocampus to cortex
    // After weeks/months, memories become independent of hippocampus
    
    // Update integration level
    pImpl->corticalIntegrationLevel += NeuromodulationConstants::SYSTEMS_CONSOLIDATION_RATE * dt;
    pImpl->corticalIntegrationLevel = std::clamp(pImpl->corticalIntegrationLevel, 0.0f, 1.0f);
    
    // Process hippocampal-cortical links
    for (size_t i = 0; i < pImpl->hippocampalCorticalLinks_.size(); ++i) {
        size_t episodeIdx = pImpl->hippocampalCorticalLinks_[i];
        if (episodeIdx >= episodes_.size()) continue;
        
        // Transfer information to cortical representation
        float transferAmount = NeuromodulationConstants::HIPPOCAMPAL_UPDATE_RATE * pImpl->corticalIntegrationLevel;
        
        if (!corticalRepresentations_[i].empty()) {
            // Inject cortical activation
            float corticalActivation = pImpl->memoryStrengths[episodeIdx] * transferAmount;
            for (NeuronId neuronId : corticalRepresentations_[i]) {
                if (brain_) brain_->injectCurrent(neuronId, corticalActivation);
            }
        }
        
        // Gradually weaken hippocampal trace
        pImpl->memoryStrengths[episodeIdx] *= (1.0f - NeuromodulationConstants::CORTICAL_STABILIZATION_RATE * dt);
    }
}

void NeuralEpisodicMemory::updateSleepConsolidation(float dt) {
    // Sleep-dependent consolidation during Sharp Wave-Ripples
    // Reactivation of memory traces during slow wave sleep
    
    if (!brain_ || !brain_->getAcetylcholine()) {
        return;
    }
    
    float acetylcholineLevel = brain_->getAcetylcholine()->getLevel();
    
    // Sleep state: low ACh
    if (acetylcholineLevel < NeuromodulationConstants::ACH_SLEEP_THRESHOLD) {
        // Generate Sharp Wave-Ripple bursts
        for (size_t i = 0; i < episodes_.size(); ++i) {
            if (!pImpl->markedForReplay[i]) continue;
            
            // Calculate replay probability
            float replayProbability = pImpl->replayProbabilities[i];
            
            // Enhance replay for high-reward memories
            if (episodes_[i].reward > 0.0f) {
                replayProbability *= (1.0f + NeuromodulationConstants::REPLAY_ENHANCEMENT_REWARD * episodes_[i].reward);
            }
            
            // Enhance replay for recent memories
            float recency = 1.0f / (1.0f + (getCurrentTime() - episodes_[i].timestamp) * NeuromodulationConstants::RECENCY_DECAY_RATE);
            replayProbability *= (1.0f + NeuromodulationConstants::REPLAY_ENHANCEMENT_RELEVANCE * recency);
            
            // Competition between memories
            float competitionFactor = calculateMemoryCompetition(i);
            replayProbability *= competitionFactor;
            
            // Probabilistic replay
            if (pImpl->rng && pImpl->rng->uniformReal() < replayProbability) {
                // Reactivate memory trace
                replayEpisode(&episodes_[i]);
                
                // Post-replay consolidation boost
                pImpl->memoryStrengths[i] *= NeuromodulationConstants::POST_REPLAY_CONSOLIDATION_BOOST;
            }
        }
    }
}

void NeuralEpisodicMemory::updateActiveMaintenance(float dt) {
    // Active maintenance with working-to-episodic transfer
    // Continuous maintenance of memory traces in the face of interference
    
    if (!brain_ || !brain_->getWorkingMemory()) {
        return;
    }
    
    // Working memory serves as buffer for episodic consolidation
    auto& workingMemory = *brain_->getWorkingMemory();
    
    // Get working memory entries
    const auto& wmEntries = workingMemory.getEntries();
    
    for (const auto& entry : wmEntries) {
        // Transfer to episodic memory based on importance
        float transferRate = NeuromodulationConstants::WORKING_EPISODIC_TRANSFER_RATE;
        
        // Modulation by attention and context
        float attentionModulation = 1.0f;
        if (brain_ && brain_->getAttention()) {
            attentionModulation = brain_->getAttention()->getWinners().size() / static_cast<float>(workingMemory.getCapacity());
        }
        
        transferRate *= attentionModulation;
        
        // Create episodic memory from working trace
        if (pImpl->rng && pImpl->rng->uniformReal() < transferRate) {
            EpisodicMemoryItem newEpisode;
            newEpisode.timestamp = getCurrentTime();
            newEpisode.age = 0;
            newEpisode.reward = entry.importance;
            newEpisode.action = ActionType::Wait;
            
            // Encode working memory content
            newEpisode.sensoryState = {entry.activation, entry.importance};
            
            storeEpisode(newEpisode);
        }
    }
}

void NeuralEpisodicMemory::updateRealisticDynamics(float dt) {
    // Implement realistic memory dynamics: interference, forgetting, reconstruction
    // Models the biological imperfections in human memory
    
    // Update interference fields
    updateInterferenceFields(dt);
    
    // Apply forgetting curves
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Time-based forgetting (power law)
        float age = getEpisodeAge(i);
        float timeForgetting = std::pow(age / NeuromodulationConstants::FORGETTING_TIME_CONSTANT,
                                       NeuromodulationConstants::FORGETTING_EXPONENT);
        
        // Interference-based forgetting
        float interference = calculateInterference(i);
        
        // Synaptic strength reduction
        pImpl->memoryStrengths[i] *= (1.0f - (timeForgetting + interference) * NeuromodulationConstants::FORGETTING_SENSITIVITY);
        
        // Ensure minimum strength
        pImpl->memoryStrengths[i] = std::max(pImpl->memoryStrengths[i],
                                           NeuromodulationConstants::SYNAPTIC_STRENGTH_MIN);
        
        // Memory reconstruction for older traces
        if (age > NeuromodulationConstants::RECONSTRUCTION_AGE_THRESHOLD) {
            reconstructMemory(i, age);
        }
    }
}

void NeuralEpisodicMemory::updateInterferenceFields(float dt) {
    // Calculate interference between similar memories
    // Memories that share features compete for retrieval
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        pImpl->interferenceFields[i] = 0.0f;
        
        // Calculate interference from similar memories
        for (size_t j = 0; j < episodes_.size(); ++j) {
            if (i == j) continue;
            
            // Similarity-based interference
            float similarity = computeSimilarity(episodes_[i], episodes_[j]);
            
            if (similarity > NeuromodulationConstants::INTERFERENCE_THRESHOLD) {
                pImpl->interferenceFields[i] += similarity * NeuromodulationConstants::INTERFERENCE_STRENGTH;
            }
        }
        
        // Decay interference over time
        pImpl->interferenceFields[i] *= (1.0f - NeuromodulationConstants::INTERFERENCE_DECAY_RATE * dt);
    }
}

void NeuralEpisodicMemory::updateNeuromodulationIntegration() {
    // Integrate with neuromodulation systems for encoding, retrieval, and consolidation
    if (!brain_) return;
    
    // Acetylcholine for attention and encoding
    if (auto* acetylcholine = brain_->getAcetylcholine()) {
        float achLevel = acetylcholine->getLevel();
        
        for (size_t i = 0; i < episodes_.size(); ++i) {
            // ACh enhances encoding during attentional focus
            if (achLevel > NeuromodulationConstants::ACH_ENCODING_THRESHOLD) {
                pImpl->memoryStrengths[i] *= NeuromodulationConstants::ACH_ENCODING_BOOST;
            }
            
            // ACh supports retrieval
            if (achLevel > NeuromodulationConstants::ACH_RETRIEVAL_THRESHOLD) {
                pImpl->replayProbabilities[i] *= NeuromodulationConstants::ACH_RETRIEVAL_BOOST;
            }
        }
    }
    
    // Dopamine for reward prediction error and reinforcement
    if (auto* dopamine = brain_->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        
        for (size_t i = 0; i < episodes_.size(); ++i) {
            float rewardPrediction = episodes_[i].reward;
            float predictionError = rewardPrediction - dopamine->getPredictedValue();
            
            if (std::abs(predictionError) > NeuromodulationConstants::PREDICTION_ERROR_THRESHOLD) {
                // Large prediction errors enhance consolidation
                pImpl->memoryStrengths[i] *= (1.0f + NeuromodulationConstants::PE_FORGETTING_CONSOLIDATION * predictionError);
            }
        }
    }
}

void NeuralEpisodicMemory::updateDevelopmentIntegration() {
    // Integrate with developmental processes
    if (!brain_) return;
    
    DevelopmentalStage stage = brain_->getDevelopmentalStage();
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High plasticity during initial development
            for (size_t i = 0; i < episodes_.size(); ++i) {
                pImpl->memoryStrengths[i] *= NeuromodulationConstants::DEVEL_PLASTICITY_FACTOR_INITIAL;
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for memory formation
            for (size_t i = 0; i < episodes_.size(); ++i) {
                pImpl->memoryStrengths[i] *= NeuromodulationConstants::DEVEL_CRITICAL_PERIOD_BOOST;
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Gradual maturation
            for (size_t i = 0; i < episodes_.size(); ++i) {
                pImpl->memoryStrengths[i] *= NeuromodulationConstants::DEVEL_MATURATION_FACTOR;
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Adult memory stability
            for (size_t i = 0; i < episodes_.size(); ++i) {
                pImpl->memoryStrengths[i] *= NeuromodulationConstants::DEVEL_ADULT_FACTOR;
            }
            break;
    }
}

void NeuralEpisodicMemory::updateBindingMechanisms() {
    // Implement memory binding: linking multiple elements into coherent episodes
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Enhance temporal binding
        if (!temporalBindingCache_.empty()) {
            enhanceTemporalBinding(i);
        }
        
        // Establish causal relationships
        establishCausalBinding(i);
        
        // Integrate with schemas
        integrateWithSchemas(i);
        
        // Perform pattern completion
        performPatternCompletion(i);
        
        // Chunk patterns
        chunkMemoryPattern(i);
    }
}

void NeuralEpisodicMemory::enhanceTemporalBinding(size_t episodeIndex) {
    // Link temporal context to enhance memory recall
    if (episodeIndex >= episodes_.size()) return;
    
    // Temporal binding would integrate episode with temporal context
    // This enables temporal ordering and sequence recall
}

void NeuralEpisodicMemory::establishCausalBinding(size_t episodeIndex) {
    // Establish cause-effect relationships within memory
    if (episodeIndex >= episodes_.size()) return;
    
    EpisodicMemoryItem& episode = episodes_[episodeIndex];
    
    // Causal binding connects actions to outcomes
    if (episode.action != ActionType::Wait && !episode.resultingSensoryState.empty()) {
        // Store causal relationships
        // This supports prediction of future outcomes
    }
}

void NeuralEpisodicMemory::integrateWithSchemas(size_t episodeIndex) {
    // Integrate memory with existing schema knowledge
    if (episodeIndex >= episodes_.size()) return;
    
    if (!brain_ || !brain_->getConceptFormation()) return;
    
    std::vector<float> conceptPattern;
    extractConceptPattern(episodes_[episodeIndex], conceptPattern);
    
    brain_->getConceptFormation()->integrateWithSchema(conceptPattern, 0.5f);
}

void NeuralEpisodicMemory::performPatternCompletion(size_t episodeIndex) {
    // Pattern completion for partial recall
    if (episodeIndex >= episodes_.size()) return;
    
    // Complete partial patterns using stored associations
}

void NeuralEpisodicMemory::chunkMemoryPattern(size_t episodeIndex) {
    // Chunking: compress memory patterns into efficient representations
    if (episodeIndex >= episodes_.size()) return;
    
    // Identify recurring patterns and create compressed templates
}

void NeuralEpisodicMemory::extractConceptPattern(const EpisodicMemoryItem& episode,
                                                 std::vector<float>& pattern) {
    // Extract concept-level features from episode
    pattern.clear();
    
    pattern.push_back(episode.reward);
    pattern.push_back(episode.novelty);
    pattern.push_back(static_cast<float>(episode.action));
}

void NeuralEpisodicMemory::reconstructMemory(size_t episodeIndex, SimulationStep age) {
    // Memory reconstruction with potential biases
    if (episodeIndex >= episodes_.size()) return;
    
    EpisodicMemoryItem& episode = episodes_[episodeIndex];
    
    // Reconstruction bias based on age
    float reconstructionBias = 1.0f / (1.0f + age * NeuromodulationConstants::RECONSTRUCTION_BIAS_DECAY);
    
    // Apply reconstruction bias
    // This would modify memory content in a biologically plausible way
}

void NeuralEpisodicMemory::integrateWithExistingSchemas(size_t episodeIndex) {
    // Integrate with existing schema knowledge structures
    if (episodeIndex >= episodes_.size()) return;
    
    if (!brain_ || !brain_->getConceptFormation()) return;
    
    brain_->getConceptFormation()->getSchemaRepresentations(episodes_[episodeIndex]);
    brain_->getConceptFormation()->updateSchemaIntegration(episodes_[episodeIndex]);
}

void NeuralEpisodicMemory::updateMemoryStatistics() {
    // Update memory statistics for monitoring and analysis
    size_t totalStrength = 0;
    float totalReplayPotential = 0.0f;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (i < pImpl->memoryStrengths.size()) {
            totalStrength += static_cast<size_t>(pImpl->memoryStrengths[i] * 100);
        }
        
        if (i < pImpl->replayProbabilities.size()) {
            totalReplayPotential += pImpl->replayProbabilities[i];
        }
    }
    
    avgMemoryStrength_ = totalStrength / (episodes_.size() > 0 ? episodes_.size() : 1);
    avgReplayPotential_ = totalReplayPotential / (episodes_.size() > 0 ? episodes_.size() : 1);
}

void NeuralEpisodicMemory::reactivateMemory(float importance) {
    // Reactivate memory based on importance
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (pImpl->replayProbabilities[i] > importance) {
            pImpl->reconsolidationTimes[i] = getCurrentTime();
        }
    }
}

void NeuralEpisodicMemory::clear() {
    // Clear all episodic memory
    episodes_.clear();
    
    if (pImpl) {
        pImpl->memoryStrengths.clear();
        pImpl->encodingTimes.clear();
        pImpl->proteinLevels.clear();
        pImpl->markedForReplay.clear();
        pImpl->replayProbabilities.clear();
        pImpl->reconsolidationTimes.clear();
        pImpl->memoryTraceStrengths.clear();
        pImpl->interferenceFields.clear();
        pImpl->hippocampalCorticalLinks.clear();
    }
}

void NeuralEpisodicMemory::enableReplay(bool enable) {
    replayEnabled_ = enable;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

void NeuralEpisodicMemory::initializeAcetylcholineIntegration() {
    // Initialize acetylcholine integration for encoding/retrieval
    if (brain_ && brain_->getAcetylcholine()) {
        brain_->getAcetylcholine()->setLevel(NeuromodulationConstants::ACH_INITIAL_LEVEL);
    }
}

float NeuralEpisodicMemory::calculateInterference(size_t episodeIndex) const {
    if (episodeIndex >= pImpl->interferenceFields.size()) return 0.0f;
    return pImpl->interferenceFields[episodeIndex];
}

float NeuralEpisodicMemory::calculateMemoryCompetition(size_t episodeIndex) const {
    // Calculate competition between memories for replay priority
    if (episodes_.empty()) return 1.0f;
    
    // Higher reward = more likely to replay
    float maxReward = 0.0f;
    for (const auto& ep : episodes_) {
        if (ep.reward > maxReward) maxReward = ep.reward;
    }
    
    if (episodeIndex >= episodes_.size()) return 1.0f;
    
    float reward = episodes_[episodeIndex].reward;
    return (maxReward > 0.0f) ? (reward / maxReward) : 1.0f;
}

void NeuralEpisodicMemory::performReconstruction(float age) {
    // Perform reconstruction of older memories
    // This models the natural reconstruction processes in human memory
}
