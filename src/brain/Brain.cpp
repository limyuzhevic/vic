NeuralSemanticMemory* Brain::getSemanticMemory() {
    return pImpl->semanticMemory.get();
}

NeuralProceduralMemory* Brain::getProceduralMemory() {
    return pImpl->proceduralMemory.get();
}

// Brain step implementation - integrate cognitive systems into the brain loop
void Brain::step(SimulationStep currentStep) {
    step(currentStep, currentStep * 0.001); // Call overload with timestamp
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // NEURAL DYNAMICS - Process spiking activity
    if (auto* spikeSystem = getSpikeSystem()) {
        spikeSystem->step(currentTime);
    }
    
    // INTEGRATE NEURAL REGIONS - Process neural populations and connections
    for (const auto& region : getRegions()) {
        region->step(currentTime);
    }
    
    // PLASTICITY - Update synaptic weights based on activity
    updatePlasticity();
    
    // DEVELOPMENT - Apply developmental changes
    if (developmentEnabled_) {
        develop();
    }
    
    // NEUROMODULATION - Apply neuromodulatory signals
    if (auto* dopamine = getDopamine()) {
        dopamine->step(currentTime);
    }
    
    if (auto* curiosity = getCuriosity()) {
        curiosity->step(currentTime);
    }
    
    if (auto* novelty = getNovelty()) {
        novelty->step(currentTime);
    }
    
    // INTEGRATE COGNITIVE SYSTEMS - Phase 6 cognitive integration
    
    // 1. NEURAL PLANNER Integration: Use learned predictions to evaluate potential action sequences
    if (auto* planner = getPlanner()) {
        // Get current brain state for planning
        std::vector<float> currentState;
        if (auto* workingMemory = getWorkingMemory()) {
            currentState = workingMemory->getActivePattern();
        }
        
        // Plan optimal action based on learned predictions
        ActionType plannedAction = planner->planAction(currentState);
        
        // Log planning activity
        NLM_LOG_INFO("NeuralPlanner: Selected action " + std::to_string(static_cast<int>(plannedAction)) +
                     " based on learned predictions");
    }
    
    // 2. CONCEPT FORMATION Integration: Learn abstract patterns from working and episodic memory
    if (auto* conceptForm = getConceptFormation()) {
        // Get patterns from working memory
        std::vector<float> workingPattern;
        if (auto* workingMemory = getWorkingMemory()) {
            workingPattern = workingMemory->getActivePattern();
        }
        
        // Get episodic memories
        std::vector<float> episodicPattern;
        if (auto* episodicMemory = getEpisodicMemory()) {
            episodicPattern = episodicMemory->getLastExperience();
        }
        
        // Present experiences to concept formation system
        if (!workingPattern.empty()) {
            conceptForm->presentExperience(workingPattern, {}, 0.0f, currentStep);
        }
        if (!episodicPattern.empty()) {
            conceptForm->presentExperience(episodicPattern, {}, 0.0f, currentStep);
        }
        
        NLM_LOG_INFO("ConceptFormation: Integrated working and episodic memory patterns");
    }
    
    // 3. SELF MODEL Integration: Update self-model with new experiences and modulate attention
    if (auto* selfModel = getSelfModel()) {
        // Get current sensory input for self-model update
        std::vector<float> sensoryState;
        if (auto* predictionSystem = getPredictionSystem()) {
            sensoryState = predictionSystem->getCurrentSensoryState();
        }
        
        // Record self-generated actions
        selfModel->recordSelfAction(ActionType::Wait, sensoryState, sensoryState);
        
        // Use self-model to modulate action selection
        ActionType selfModelAction = selfModel->getPreferredAction(sensoryState);
        
        NLM_LOG_INFO("SelfModel: Updated with new experiences, preferred action = " +
                     std::to_string(static_cast<int>(selfModelAction)));
    }
    
    // 4. SOCIAL LEARNING Integration: Observe agent interactions and learn from them
    if (auto* socialLearning = getSocialLearning()) {
        // In a full implementation, this would observe other agents' actions
        // For now, we record potential social interactions
        std::vector<float> socialObservation;
        if (auto* workingMemory = getWorkingMemory()) {
            socialObservation = workingMemory->getSocialMemory();
        }
        
        if (!socialObservation.empty()) {
            socialLearning->observeAgentAction(ActionType::Interact, socialObservation, socialObservation);
        }
        
        // Update social knowledge based on interaction rewards
        socialLearning->updateSocialKnowledge(0.5f); // Default neutral reward
        
        NLM_LOG_INFO("SocialLearning: Observed interactions and updated social knowledge");
    }
    
    // 5. SPATIAL REPRESENTATION Integration: Maintain and update environment knowledge
    if (auto* spatialRep = getSpatialRepresentation()) {
        // Update spatial knowledge based on movement and sensory input
        // In a full implementation, this would integrate with world state
        spatialRep->integrateMovement(0.1f, 0.0f); // Small movement step
        
        NLM_LOG_INFO("SpatialRepresentation: Updated environmental knowledge");
    }
    
    // 6. TEMPORAL RELATION Integration: Learn and maintain temporal sequences and patterns
    if (auto* temporalRel = getTemporalRelation()) {
        // Record temporal sequences based on recent activities
        std::vector<float> currentEvent = {static_cast<float>(currentStep) % 100.0f};
        
        if (!currentEvent.empty()) {
            temporalRel->recordSequence(currentEvent, currentEvent, 0.0f);
        }
        
        NLM_LOG_INFO("TemporalRelation: Maintained temporal sequences");
    }
    
    // Apply neuromodulation based on cognitive system activity
    applyNeuromodulation(dopamineLevel_);
}

// Helper variables for integration (would be in Brain::Impl in full implementation)
float Brain::dopamineLevel_ = 0.0f;
bool Brain::developmentEnabled_ = true;

} // namespace nlm
