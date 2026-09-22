// Connect cognitive systems to brain
void Brain::connectCognitiveSystems() {
    // Connect NeuralPlanner to action selection
    if (pImpl->planner) {
        // Register planning callback
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
        pImpl->planner->setCurrentGoal(std::vector<float>(256, 0.0f));  // Full vision state
    }
    
    // Connect ConceptFormation to pattern processing
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
        pImpl->conceptFormation->setFormationThreshold(0.7f);
    }
    
    // Connect SelfModel for action prediction
    // SelfModel exists in NeuralPlanner.hpp but not instantiated - would be part of planner
    
    // Connect SocialLearning (would be part of planner or separate)
    // SocialLearning exists in NeuralPlanner.hpp but not instantiated
}

// Process current sensory input through cognitive systems
void Brain::processSensoryThroughCognition(const SensoryInput& input) {
    // Convert sensory input to neural pattern
    std::vector<float> sensoryPattern = input.getData();
    
    // Process through ConceptFormation if available
    if (pImpl->conceptFormation && !sensoryPattern.empty()) {
        // Present to concept formation system
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            sensoryPattern,
            std::vector<float>(sensoryPattern.size()),  // features would be extracted
            0.0f,  // initial reward
            pImpl->currentStep
        );
        
        // Update NeuralPlanner with current concepts
        if (pImpl->planner && conceptId > 0) {
            // Get concept prototype and set as goal
            auto prototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
            if (!prototype.empty()) {
                pImpl->planner->setCurrentGoal(prototype);
            }
        }
    }
    
    // Update prediction system with sensory input
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(pImpl->currentStep, pImpl->currentTime);
    }
}

// Generate action using cognitive systems
MotorCommand Brain::generateCognitionAction() {
    if (!pImpl->planner || pImpl->motorNeurons.empty()) {
        return MotorCommand::Wait;
    }
    
    // Get current state (sensory input, memory, etc.)
    std::vector<float> currentState;
    if (pImpl->workingMemory) {
        // Sample from working memory
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            currentState = std::vector<float>(256, 0.0f);  // Full vision state
        }
    } else {
        // Create simple state from current neuron activity
        currentState = std::vector<float>(256, 0.0f);
        for (size_t i = 0; i < pImpl->sensoryNeurons.size() && i < currentState.size(); ++i) {
            if (pImpl->sensoryNeurons[i]) {
                currentState[i] = std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                         pImpl->sensoryNeurons[i]->getState().restingPotential) / 50.0f;
            }
        }
    }
    
    // Use NeuralPlanner to select action
    ActionType plannedAction = pImpl->planner->planAction(currentState, 0.5f);
    
    // Convert ActionType to MotorCommand
    switch (plannedAction) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::Look: return MotorCommand::MoveForward;  // Simplified
        case ActionType::LookUp: return MotorCommand::MoveBackward;
        case ActionType::LookDown: return MotorCommand::TurnLeft;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Eat: return MotorCommand::MoveForward;
        case ActionType::Drink: return MotorCommand::MoveBackward;
        case ActionType::Rest: return MotorCommand::Wait;
        case ActionType::Wait:
        default: return MotorCommand::Wait;
    }
}

// Store experience in memory systems
void Brain::storeExperience(float reward, float predictionError) {
    // Create episodic memory entry
    if (pImpl->episodicMemory) {
        EpisodicMemoryItem episode;
        episode.timestamp = pImpl->currentStep;
        episode.reward = reward;
        
        // Store current brain state
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
        
        pImpl->episodicMemory->storeEpisode(episode);
    }
    
    // Store in working memory
    if (pImpl->workingMemory) {
        // Store current sensory patterns
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            if (pImpl->sensoryNeurons[i]) {
                float activity = std::abs(pImpl->sensoryNeurons[i]->getState().membranePotential - 
                                         pImpl->sensoryNeurons[i]->getState().restingPotential);
                pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), 
                    activity / 100.0f);
            }
        }
    }
    
    // Update prediction system
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->update(pImpl->currentStep, pImpl->currentTime);
    }
}