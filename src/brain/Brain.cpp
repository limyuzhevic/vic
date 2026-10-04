// ============================================================================
// ADVANCED FEATURE ACCESS IMPLEMENTATIONS
// ============================================================================

// Memory system diagnostics

size_t Brain::getWorkingMemoryCapacity() const {
    return pImpl->workingMemory ? pImpl->workingMemory->getCapacity() : 0;
}

size_t Brain::getEpisodicMemoryCapacity() const {
    return pImpl->episodicMemory ? pImpl->episodicMemory->getMaxEpisodes() : 0;
}

size_t Brain::getAssociativeMemoryCapacity() const {
    return pImpl->associativeMemory ? pImpl->associativeMemory->getMaxAssociations() : 0;
}

// Neuromodulation diagnostics

float Brain::getDopamineLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

float Brain::getCuriosityLevel() const {
    return pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
}

float Brain::getNoveltyLevel() const {
    return pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
}

// Prediction system diagnostics

float Brain::getPredictionAccuracy() const {
    return pImpl->predictionSystem ? pImpl->predictionSystem->getPredictionAccuracy() : 0.0f;
}

size_t Brain::getPredictionErrorCount() const {
    return pImpl->predictionSystem ? pImpl->predictionSystem->getPredictionErrorCount() : 0;
}

// Cognition system diagnostics

size_t Brain::getPlannerDepth() const {
    return pImpl->planner ? pImpl->planner->getPlanningDepth() : 0;
}

size_t Brain::getConceptCount() const {
    return pImpl->conceptFormation ? pImpl->conceptFormation->getConceptCount() : 0;
}

float Brain::getAttentionStrength() const {
    return pImpl->attention ? pImpl->attention->getAttentionStrength() : 0.0f;
}

// Plasticity system diagnostics

bool Brain::isSTDPActive() const {
    return pImpl->stdp && pImpl->stdp->isEnabled();
}

bool Brain::isHebbianActive() const {
    return pImpl->hebbian && pImpl->hebbian->isEnabled();
}

bool Brain::isStructuralPlasticityActive() const {
    return pImpl->structuralPlasticity && pImpl->structuralPlasticity->isEnabled();
}

// Development system diagnostics

DevelopmentalStage Brain::getCurrentDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

float Brain::getPlasticityModulation() const {
    return pImpl->developmentalStage == DevelopmentalStage::Initial ? 1.0f :
           pImpl->developmentalStage == DevelopmentalStage::CriticalPeriod ? 0.8f :
           pImpl->developmentalStage == DevelopmentalStage::Maturation ? 0.5f :
           pImpl->developmentalStage == DevelopmentalStage::Adult ? 0.2f :
           0.0f;
}

// Environment and interaction

void Brain::enableAllSubsystems() {
    NLM_LOG_INFO("Enabling all brain subsystems...");
    
    if (pImpl->workingMemory) pImpl->workingMemory->enable(true);
    if (pImpl->episodicMemory) pImpl->episodicMemory->enable(true);
    if (pImpl->associativeMemory) pImpl->associativeMemory->enable(true);
    if (pImpl->predictionSystem) pImpl->predictionSystem->enable(true);
    if (pImpl->planner) pImpl->planner->enable(true);
    if (pImpl->conceptFormation) pImpl->conceptFormation->enable(true);
    if (pImpl->attention) pImpl->attention->enable(true);
    if (pImpl->developmentSystem) pImpl->developmentSystem->enable(true);
    if (pImpl->dopamine) pImpl->dopamine->enable(true);
    if (pImpl->curiosity) pImpl->curiosity->enable(true);
    if (pImpl->novelty) pImpl->novelty->enable(true);
    
    NLM_LOG_INFO("All subsystems enabled successfully");
}

void Brain::disableAllSubsystems() {
    NLM_LOG_INFO("Disabling all brain subsystems...");
    
    if (pImpl->workingMemory) pImpl->workingMemory->enable(false);
    if (pImpl->episodicMemory) pImpl->episodicMemory->enable(false);
    if (pImpl->associativeMemory) pImpl->associativeMemory->enable(false);
    if (pImpl->predictionSystem) pImpl->predictionSystem->enable(false);
    if (pImpl->planner) pImpl->planner->enable(false);
    if (pImpl->conceptFormation) pImpl->conceptFormation->enable(false);
    if (pImpl->attention) pImpl->attention->enable(false);
    if (pImpl->developmentSystem) pImpl->developmentSystem->enable(false);
    if (pImpl->dopamine) pImpl->dopamine->enable(false);
    if (pImpl->curiosity) pImpl->curiosity->enable(false);
    if (pImpl->novelty) pImpl->novelty->enable(false);
    
    NLM_LOG_INFO("All subsystems disabled successfully");
}

void Brain::optimizeForPerformance() {
    NLM_LOG_INFO("Optimizing brain for performance...");
    
    // Optimize memory systems for performance
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(pImpl->workingMemory->getCapacity());
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setConsolidationInterval(500);
    }
    
    // Optimize neuromodulation for performance
    if (pImpl->dopamine) {
        pImpl->dopamine->setModulationStrength(2.0f);
    }
    
    // Optimize plasticity for performance
    if (pImpl->stdp) {
        pImpl->stdp->setLearningRate(0.002f);
    }
    
    NLM_LOG_INFO("Performance optimization complete");
}

void Brain::optimizeForLearning() {
    NLM_LOG_INFO("Optimizing brain for learning...");
    
    // Optimize memory systems for learning
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setCapacity(pImpl->workingMemory->getCapacity() * 2);
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setConsolidationInterval(100);
    }
    
    // Optimize neuromodulation for learning
    if (pImpl->curiosity) {
        pImpl->curiosity->setExplorationStrength(1.5f);
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->setNoveltySensitivity(1.5f);
    }
    
    // Optimize plasticity for learning
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(0.003f);
    }
    
    NLM_LOG_INFO("Learning optimization complete");
}

void Brain::pauseSimulation() {
    NLM_LOG_INFO("Pausing simulation...");
    pImpl->isResting = true;
}

void Brain::resumeSimulation() {
    NLM_LOG_INFO("Resuming simulation...");
    pImpl->isResting = false;
}

bool Brain::isSimulationPaused() const {
    return pImpl->isResting;
}

void Brain::enableLearning() {
    NLM_LOG_INFO("Enabling learning mechanisms...");
    
    if (pImpl->stdp) pImpl->stdp->enable(true);
    if (pImpl->hebbian) pImpl->hebbian->enable(true);
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->enable(true);
    if (pImpl->dopamine) pImpl->dopamine->enable(true);
    if (pImpl->curiosity) pImpl->curiosity->enable(true);
    
    NLM_LOG_INFO("Learning enabled successfully");
}

void Brain::disableLearning() {
    NLM_LOG_INFO("Disabling learning mechanisms...");
    
    if (pImpl->stdp) pImpl->stdp->enable(false);
    if (pImpl->hebbian) pImpl->hebbian->enable(false);
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->enable(false);
    if (pImpl->dopamine) pImpl->dopamine->enable(false);
    if (pImpl->curiosity) pImpl->curiosity->enable(false);
    
    NLM_LOG_INFO("Learning disabled successfully");
}

bool Brain::isLearningEnabled() const {
    return (pImpl->stdp && pImpl->stdp->isEnabled()) ||
           (pImpl->hebbian && pImpl->hebbian->isEnabled()) ||
           (pImpl->structuralPlasticity && pImpl->structuralPlasticity->isEnabled()) ||
           (pImpl->dopamine && pImpl->dopamine->isEnabled()) ||
           (pImpl->curiosity && pImpl->curiosity->isEnabled());
}

void Brain::clearWorkingMemory() {
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Clearing working memory...");
        pImpl->workingMemory->clear();
        NLM_LOG_INFO("Working memory cleared successfully");
    }
}

void Brain::clearEpisodicMemory() {
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Clearing episodic memory...");
        pImpl->episodicMemory->clear();
        NLM_LOG_INFO("Episodic memory cleared successfully");
    }
}

void Brain::clearAllCaches() {
    NLM_LOG_INFO("Clearing all brain caches...");
    
    clearWorkingMemory();
    clearEpisodicMemory();
    
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->clear();
    }
    
    if (pImpl->attention) {
        pImpl->attention->reset();
    }
    
    NLM_LOG_INFO("All caches cleared successfully");
}

void Brain::emergencyShutdown() {
    NLM_LOG_ERROR("EMERGENCY SHUTDOWN INITIATED");
    
    // Disable all learning mechanisms
    disableAllSubsystems();
    
    // Stop all spike processing
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Clear memory
    clearAllCaches();
    
    NLM_LOG_ERROR("EMERGENCY SHUTDOWN COMPLETE");
}

void Brain::emergencyReset() {
    NLM_LOG_WARNING("EMERGENCY RESET INITIATED");
    
    // Reset all systems
    reset();
    
    // Re-enable subsystems
    enableAllSubsystems();
    
    NLM_LOG_WARNING("EMERGENCY RESET COMPLETE");
}

bool Brain::isEmergencyMode() const {
    // Check if in emergency state by checking system integrity
    return (pImpl->spikeSystem && pImpl->spikeSystem->getPendingSpikeCount() > 1000) ||
           (pImpl->workingMemory && pImpl->workingMemory->getActiveTraces() > 1000);
}

// ============================================================================
// INTERNAL SYSTEM ACCESS IMPLEMENTATIONS
// ============================================================================

// Direct access to internal systems (advanced usage)

NeuralWorkingMemory* Brain::getWorkingMemoryInternal() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemoryInternal() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemoryInternal() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystemInternal() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlannerInternal() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormationInternal() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttentionInternal() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystemInternal() {
    return pImpl->developmentSystem.get();
}

Dopamine* Brain::getDopamineInternal() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosityInternal() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNoveltyInternal() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignalInternal() {
    return pImpl->predictionError.get();
}

STDP* Brain::getSTDPInternal() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbianInternal() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticityInternal() {
    return pImpl->structuralPlasticity.get();
}

// Implementation details (for debugging)

const Brain::Impl* Brain::getImplementation() const {
    return pImpl;
}

Brain::Impl* Brain::getImplementation() {
    return pImpl;
}

} // namespace nlm
