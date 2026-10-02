// Brain System Manager implementation

BrainSystemManager::BrainSystemManager(std::shared_ptr<Brain> brain)
    : brain_(brain) {}

void BrainSystemManager::initializeAllSystems() {
    NLM_LOG_INFO("Initializing all integrated brain systems...");
    
    // Initialize working memory
    if (brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->initialize(brain_.get());
        brain_->getWorkingMemory()->setCapacity(500);  // Default capacity
    }
    
    // Initialize episodic memory
    if (brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->initialize(brain_.get());
        brain_->getEpisodicMemory()->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (brain_->getAssociativeMemory()) {
        brain_->getAssociativeMemory()->initialize(brain_.get());
    }
    
    // Initialize cognition systems
    if (brain_->getPlanner()) {
        brain_->getPlanner()->initialize(brain_.get());
        brain_->getPlanner()->setPlanningDepth(5);
    }
    
    if (brain_->getConceptFormation()) {
        brain_->getConceptFormation()->initialize(brain_.get());
    }
    
    if (brain_->getAttention()) {
        brain_->getAttention()->initialize(brain_.get());
        brain_->getAttention()->setInhibitionStrength(0.5f);
        brain_->getAttention()->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    if (brain_->getNovelty()) {
        brain_->getNovelty()->initialize(brain_.get());
    }
    
    if (brain_->getCuriosity()) {
        brain_->getCuriosity()->initialize(brain_.get());
    }
    
    NLM_LOG_INFO("All integrated systems initialized successfully");
}

void BrainSystemManager::updateWorkingMemory() {
    if (brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->update(brain_->pImpl->timestep);
    }
}

void BrainSystemManager::updateNeuromodulationSystems() {
    // Update novelty detection
    if (brain_->getNovelty()) {
        brain_->getNovelty()->update(brain_->pImpl->timestep);
    }
    
    // Update curiosity
    if (brain_->getCuriosity()) {
        brain_->getCuriosity()->update(brain_->pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (brain_->getDopamine()) {
        brain_->getDopamine()->update(brain_->pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brain_->getDopamine()->getLevel();
        for (auto& region : brain_->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainSystemManager::updatePredictionSystem() {
    if (brain_->getPredictionSystem()) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainSystemManager::updateCognitionSystems() {
    if (brain_->getAttention()) {
        brain_->getAttention()->update(brain_->pImpl->timestep);
        
        // Apply attention to working memory winners
        if (brain_->getWorkingMemory() && !brain_->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain_->getWorkingMemory()->getMemoryNeurons();
            brain_->getAttention()->processCompetition(competitors);
        }
    }
    
    // Note: Concept formation and planning would update here
    // They require more complex state management with sensory input
}

void BrainSystemManager::updateDevelopmentSystem(SimulationStep step) {
    if (brain_->getDevelopmentSystem() && step % 1000 == 0) {
        brain_->getDevelopmentSystem()->update(brain_.get(), 
                                              *brain_->getRandomGenerator(),
                                              brain_->pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brain_->getDevelopmentalStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void BrainSystemManager::applyStructuralPlasticity(SimulationStep step) {
    if (step % 100 == 0) {
        brain_->getStructuralPlasticity()->update(brain_.get(), *brain_->getRandomGenerator());
    }
}

void BrainSystemManager::replayMemories(SimulationStep step) {
    integrationState_.stepsSinceLastEpisode++;
    
    // Store episode every 10 steps
    if (integrationState_.stepsSinceLastEpisode >= 10) {
        integrationState_.stepsSinceLastEpisode = 0;
        
        if (brain_->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : brain_->pImpl->regions) {
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
            
            brain_->getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    // Replay important memories
    if (step % integrationState_.replayInterval == 0 && brain_->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = brain_->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain_->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void BrainSystemManager::consolidateMemories(SimulationStep step) {
    if (step % integrationState_.consolidationInterval == 0 && brain_->getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        brain_->getEpisodicMemory()->consolidate(0.3f);
    }
}

void BrainSystemManager::updateCheckpointManager(SimulationStep step, Timestamp time) {
    if (brain_->pImpl->checkpointManager) {
        brain_->pImpl->checkpointManager->update(step, time);
    }
}

void BrainSystemManager::collectSystemStatistics() {
    // Memory system status
    if (brain_->getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(brain_->getWorkingMemory()->getActiveTraces()));
    }
    
    if (brain_->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(brain_->getEpisodicMemory()->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (brain_->getDopamine()) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(brain_->getDopamine()->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain_->getDevelopmentalStage())));
}