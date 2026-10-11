// Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

void AgentBrain::processWithWorkingMemory(const std::vector<NeuronId>& activeNeurons) {
    // Integrate working memory with cognitive processing
    auto* workingMemory = getWorkingMemory();
    if (!workingMemory) return;
    
    // Store active neural patterns in working memory
    for (const auto& neuronId : activeNeurons) {
        workingMemory->storeToNeuron(neuronId, 1.0f);  // Full activation
    }
    
    // Update working memory state
    workingMemory->update(0.001);  // Use simulation timestep
    
    NLM_LOG_INFO("Working memory updated with " + std::to_string(activeNeurons.size()) + 
                 " active neurons");
}

void AgentBrain::updateWithEpisodicMemory() {
    // Update episodic memory with current experience
    auto* episodicMemory = getEpisodicMemory();
    if (!episodicMemory) return;
    
    // Create an episodic memory item with current brain state
    EpisodicMemoryItem episode;
    episode.timestamp = brain_->getCurrentStep();
    episode.reward = getNeuromodulationLevel();
    
    // Record current neural activity patterns
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto* neuron : region->getAllNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
        }
    }
    
    // Store the episode
    episodicMemory->storeEpisode(episode);
    NLM_LOG_INFO("Episodic memory updated with episode containing " + 
                 std::to_string(episode.activeNeurons.size()) + " active neurons");
}

void AgentBrain::updateWithPredictionSystem() {
    // Update prediction system with current sensory input
    auto* predictionSystem = getPredictionSystem();
    if (!predictionSystem) return;
    
    // Get current sensory state (simplified: use current brain state)
    if (brain_) {
        // Extract sensory activity patterns
        std::vector<float> sensoryPattern;
        
        // Get sensory neuron activity levels
        auto* workingMemory = getWorkingMemory();
        if (workingMemory) {
            std::vector<NeuronId> memoryNeurons = workingMemory->getMemoryNeurons();
            // Convert to activity pattern
            for (const auto& neuronId : memoryNeurons) {
                // Get actual activation level (simplified)
                sensoryPattern.push_back(0.5f);  // Placeholder for actual activation
            }
        }
        
        if (!sensoryPattern.empty()) {
            // Update prediction system with new sensory data
            predictionSystem->train(sensoryPattern);
            NLM_LOG_INFO("Prediction system updated with " + std::to_string(sensoryPattern.size()) + 
                         " sensory features");
        }
    }
}

void AgentBrain::updateWithPlanning() {
    // Update neural planner for action selection
    auto* planner = getPlanner();
    if (!planner) return;
    
    // Get current working memory contents for planning
    std::vector<NeuronId> currentPlan;
    auto* workingMemory = getWorkingMemory();
    if (workingMemory) {
        currentPlan = workingMemory->getMemoryNeurons();
    }
    
    // Evaluate current plan using neural planner
    if (!currentPlan.empty()) {
        // Plan action sequence based on current neural patterns
        planner->planAction(currentPlan, getNeuromodulationLevel());
        NLM_LOG_INFO("Neural planner updated with " + std::to_string(currentPlan.size()) + 
                     " candidate actions");
    }
}

void AgentBrain::updateWithConceptFormation() {
    // Update concept formation with current experiences
    auto* conceptFormation = getConceptFormation();
    if (!conceptFormation) return;
    
    // Get current sensory patterns for concept formation
    std::vector<float> sensoryPattern;
    auto* workingMemory = getWorkingMemory();
    if (workingMemory) {
        std::vector<NeuronId> memoryNeurons = workingMemory->getMemoryNeurons();
        for (const auto& neuronId : memoryNeurons) {
            sensoryPattern.push_back(0.5f);  // Placeholder for actual activation
        }
    }
    
    if (!sensoryPattern.empty()) {
        // Form concepts from sensory patterns
        conceptFormation->processPattern(sensoryPattern);
        NLM_LOG_INFO("Concept formation updated with " + std::to_string(sensoryPattern.size()) + 
                     " features for concept extraction");
    }
}

void AgentBrain::updateWithAttention() {
    // Update attentional selection system
    auto* attention = getAttention();
    if (!attention) return;
    
    // Get current competing neural representations
    std::vector<NeuronId> competitors;
    
    // Get working memory contents for attentional competition
    auto* workingMemory = getWorkingMemory();
    if (workingMemory) {
        competitors = workingMemory->getMemoryNeurons();
    }
    
    if (!competitors.empty()) {
        // Update attentional selection through competitive dynamics
        attention->processCompetition(competitors);
        
        // Get attentional focus
        NeuronId focus = attention->getCurrentFocus();
        NLM_LOG_INFO("Attention system updated - focus on neuron " + std::to_string(focus.index()) + 
                     " from " + std::to_string(competitors.size()) + " competitors");
    }
}
