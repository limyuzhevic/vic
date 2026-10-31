// BrainIntegration.cpp
#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct BrainIntegration::Impl {
    std::shared_ptr<Brain> brain;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::shared_ptr<NeuralWorkingMemory> workingMemory;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory;
    std::shared_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::shared_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::shared_ptr<NeuralPlanner> planner;
    std::shared_ptr<ConceptFormation> conceptFormation;
    std::shared_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::shared_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::shared_ptr<Dopamine> dopamine;
    std::shared_ptr<Curiosity> curiosity;
    std::shared_ptr<PredictionError> predictionError;
    std::shared_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::shared_ptr<SpikeSystem> spikeSystem;
    std::shared_ptr<STDP> stdp;
    std::shared_ptr<Hebbian> hebbian;
    std::shared_ptr<StructuralPlasticity> structuralPlasticity;
    
    std::shared_ptr<CheckpointManager> checkpointManager;
    
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    size_t totalSpikesTotal;
    
    Impl() : totalNeuronCount(0), totalSynapseCount(0), totalSpikesTotal(0) {}
};

BrainIntegration::BrainIntegration() : pImpl(new Impl()) {}

BrainIntegration::~BrainIntegration() = default;

BrainIntegration::BrainIntegration(BrainIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainIntegration& BrainIntegration::operator=(BrainIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainIntegration::initialize(std::shared_ptr<Brain> brainRef) {
    pImpl->brain = brainRef;
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_shared<SpikeSystem>();
    pImpl->stdp = std::make_shared<STDP>();
    pImpl->hebbian = std::make_shared<Hebbian>();
    pImpl->structuralPlasticity = std::make_shared<StructuralPlasticity>();
    
    // Initialize memory systems
    pImpl->workingMemory = std::make_shared<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_shared<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_shared<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_shared<PredictionSystem>();
    
    // Initialize cognition systems
    pImpl->planner = std::make_shared<NeuralPlanner>();
    pImpl->conceptFormation = std::make_shared<ConceptFormation>();
    pImpl->attention = std::make_shared<AttentionalSelection>();
    
    // Initialize neuromodulation systems
    pImpl->dopamine = std::make_shared<Dopamine>();
    pImpl->curiosity = std::make_shared<Curiosity>();
    pImpl->predictionError = std::make_shared<PredictionError>();
    pImpl->novelty = std::make_shared<Novelty>();
    
    // Initialize development system
    pImpl->developmentSystem = std::make_shared<DevelopmentSystem>();
    
    // Initialize checkpoint system
    pImpl->checkpointManager = std::make_shared<CheckpointManager>();
    
    NLM_LOG_INFO("BrainIntegration initialized with all systems");
    return true;
}

void BrainIntegration::step(SimulationStep currentStep, Timestamp currentTime) {
    // This method would contain the full integration step logic
    // For now, it's a placeholder for the refactored approach
    
    // Update all systems
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    // TODO: Add full integration step logic here
    NLM_LOG_DEBUG("BrainIntegration step called");
}

std::vector<NeuronId> BrainIntegration::getMostActiveNeurons(size_t count) const {
    std::vector<NeuronId> activeNeurons;
    if (pImpl->brain) {
        // This would query the actual neurons from the brain
        // For now return empty vector as this is a simplified implementation
    }
    return activeNeurons;
}

size_t BrainIntegration::getPendingSpikeEventCount() const {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getPendingSpikeCount();
    }
    return 0;
}

} // namespace nlm