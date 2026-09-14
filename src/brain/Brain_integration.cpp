#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

void Brain::Impl::initializeIntegratedSystems() {
    // Initialize working memory
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    workingMemory->initialize(nullptr);  // Will need brain reference in full implementation
    workingMemory->setCapacity(100);
    
    // Initialize episodic memory  
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    episodicMemory->initialize(nullptr);  // Will need brain reference in full implementation
    episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    associativeMemory->initialize(nullptr);  // Will need brain reference in full implementation
    
    // Initialize prediction system
    predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    planner = std::make_unique<NeuralPlanner>();
    planner->initialize(nullptr);  // Will need brain reference in full implementation
    
    conceptFormation = std::make_unique<ConceptFormation>();
    conceptFormation->initialize(nullptr);  // Will need brain reference in full implementation
    
    attention = std::make_unique<AttentionalSelection>();
    attention->initialize(nullptr);  // Will need brain reference in full implementation
}

void Brain::Impl::integrateMemorySystemsInStep() {
    // Working memory integration
    if (workingMemory) {
        // Update working memory with current neural state
        // This would capture active neural patterns
    }
    
    // Episodic memory integration  
    if (episodicMemory) {
        // Store experiences in episodic memory
        // This would happen during consolidation phases
    }
    
    // Associative memory integration
    if (associativeMemory) {
        // Update pattern associations
        // This would reinforce Hebbian learning
    }
    
    // Prediction system integration
    if (predictionSystem) {
        // Update predictions based on sensory input
        // This would involve temporal sequence learning
    }
    
    // Cognition system integration
    if (conceptFormation) {
        // Form concepts from patterns
        // This would abstract sensory input
    }
    
    if (planner) {
        // Plan actions based on current state
        // This would evaluate action sequences
    }
    
    if (attention) {
        // Select focus areas
        // This would allocate processing resources
    }
}

void Brain::Impl::updateNeuromodulation() {
    // Update all neuromodulators
    if (dopamine) {
        dopamine->update(timestep);
    }
    if (curiosity) {
        curiosity->update(timestep);
    }
    if (novelty) {
        novelty->update(timestep);
    }
    if (predictionError) {
        predictionError->update(timestep);
    }
}

void Brain::Impl::applyIntegratedNeuromodulation() {
    // Apply integrated neuromodulation to systems
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        // Apply to plasticity
        if (stdp) {
            stdp->setLTPWeight(0.01f * dopamineLevel);
            stdp->setLTDWeight(0.012f * dopamineLevel);
        }
    }
    
    // Apply attention modulation to memory
    if (attention && workingMemory) {
        float attentionLevel = attention->getFocusLevel();
        workingMemory->setAttentionLevel(attentionLevel);
    }
    
    // Apply development modulation
    if (developmentSystem) {
        developmentSystem->update(this, timestep);
    }
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    pImpl->interRegionConnections.push_back(InterRegionConnection(source, target, weight, delay));
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(), pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl->workingMemory) {
        // Create working memory if it doesn't exist
        pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
        if (pImpl->config) {
            size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
        }
    }
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (!pImpl->episodicMemory) {
        // Create episodic memory if it doesn't exist
        pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (!pImpl->associativeMemory) {
        // Create associative memory if it doesn't exist
        pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        pImpl->associativeMemory->initialize(this);
    }
    return pImpl->associativeMemory.get();
}

NeuralAttention* Brain::getAttention() {
    if (!pImpl->attention) {
        // Create attention system if it doesn't exist
        pImpl->attention = std::make_unique<AttentionalSelection>();
        pImpl->attention->initialize(this);
    }
    return pImpl->attention.get();
}

void Brain::initializeIntegratedMemorySystems() {
    // Initialize all memory systems to ensure they exist
    getWorkingMemory();
    getEpisodicMemory();
    getAssociativeMemory();
    getAttention();
}

void Brain::integrateMemorySystemsInStep() {
    // Working memory integration
    if (auto* wm = getWorkingMemory()) {
        // Update working memory with current neural state
        // This would capture active neural patterns
    }
    
    // Episodic memory integration  
    if (auto* em = getEpisodicMemory()) {
        // Store experiences in episodic memory
        // This would happen during consolidation phases
    }
    
    // Associative memory integration
    if (auto* am = getAssociativeMemory()) {
        // Update pattern associations
        // This would reinforce Hebbian learning
    }
    
    // Prediction system integration
    if (auto* ps = getPredictionSystem()) {
        // Update predictions based on sensory input
        // This would involve temporal sequence learning
    }
    
    // Cognition system integration
    if (auto* cf = getConceptFormation()) {
        // Form concepts from patterns
        // This would abstract sensory input
    }
    
    if (auto* planner = getPlanner()) {
        // Plan actions based on current state
        // This would evaluate action sequences
    }
    
    if (auto* att = getAttention()) {
        // Select focus areas
        // This would allocate processing resources
    }
}

void Brain::updateNeuromodulation() {
    // Update all neuromodulators
    if (auto* dopamine = getDopamine()) {
        dopamine->update(pImpl->timestep);
    }
    if (auto* curiosity = getCuriosity()) {
        curiosity->update(pImpl->timestep);
    }
    if (auto* novelty = getNovelty()) {
        novelty->update(pImpl->timestep);
    }
    if (auto* predictionError = getPredictionErrorSignal()) {
        predictionError->update(pImpl->timestep);
    }
}

void Brain::applyIntegratedNeuromodulation() {
    // Apply integrated neuromodulation to systems
    if (auto* dopamine = getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        // Apply to plasticity
        if (auto* stdp = getSTDP()) {
            stdp->setLTPWeight(0.01f * dopamineLevel);
            stdp->setLTDWeight(0.012f * dopamineLevel);
        }
    }
    
    // Apply attention modulation to memory
    if (auto* att = getAttention()) {
        if (auto* wm = getWorkingMemory()) {
            float attentionLevel = att->getFocusLevel();
            wm->setAttentionLevel(attentionLevel);
        }
    }
    
    // Apply development modulation
    if (auto* dev = getDevelopmentSystem()) {
        dev->update(this, pImpl->timestep);
    }
}

void Brain::step(SimulationStep currentStep) {
    // ... existing step implementation ...
    
    // Integrate memory systems after main processing
    integrateMemorySystemsInStep();
    
    // Update neuromodulation
    updateNeuromodulation();
    
    // Apply integrated neuromodulation
    applyIntegratedNeuromodulation();
    
    // Log final statistics
    logStatus();
}