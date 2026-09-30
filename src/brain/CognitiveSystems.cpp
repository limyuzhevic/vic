// Cognitive systems implementation for Brain.cpp
// Contains: NeuralPlanner, ConceptFormation, AttentionalSelection

#pragma once

#include "CognitiveSystems.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void CognitiveSystems::initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    config_ = config;
    brain_ = brain;
    
    // Initialize cognitive subsystems
    planner_ = std::make_unique<NeuralPlanner>();
    conceptFormation_ = std::make_unique<ConceptFormation>();
    attention_ = std::make_unique<AttentionalSelection>();
    
    planner_->initialize(brain.get());
    conceptFormation_->initialize(brain.get());
    attention_->initialize(brain.get());
    
    // Configure cognitive parameters
    planningDepth_ = config->getOr<size_t>("planning_depth", 5);
    attentionInhibitionStrength_ = config->getOr<float>("attention_inhibition_strength", 0.5f);
    attentionExcitationStrength_ = config->getOr<float>("attention_excitation_strength", 1.5f);
    
    planner_->setPlanningDepth(planningDepth_);
    attention_->setInhibitionStrength(attentionInhibitionStrength_);
    attention_->setExcitationStrength(attentionExcitationStrength_);
    
    NLM_LOG_INFO("CognitiveSystems initialized");
}

void CognitiveSystems::processStep(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_) return;
    
    // Update attention system
    if (attention_) {
        attention_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply attention to working memory winners
        if (brain_->getWorkingMemory() && !brain_->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain_->getWorkingMemory()->getMemoryNeurons();
            attention_->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (conceptFormation_) {
        // Process current neural activity patterns to form concepts
        // This requires sensory state encoding and feature extraction
        updateConceptFormation();
    }
    
    // Update prediction system
    if (brain_->getPredictionSystem()) {
        updatePredictionSystem();
    }
}

void CognitiveSystems::updateConceptFormation() {
    if (!conceptFormation_ || !brain_) return;
    
    // Extract current neural activity patterns
    std::vector<float> activityPattern;
    extractNeuralActivityPattern(activityPattern);
    
    // Process patterns to form new concepts
    if (!activityPattern.empty()) {
        conceptFormation_->processPattern(activityPattern);
    }
}

void CognitiveSystems::updatePredictionSystem() {
    if (!brain_ || !brain_->getPredictionSystem()) return;
    
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
}

void CognitiveSystems::extractNeuralActivityPattern(std::vector<float>& pattern) {
    if (!brain_) return;
    
    // Extract activity from all neural regions
    for (auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                const auto& state = neuron->getState();
                float activation = std::abs(state.membranePotential - state.restingPotential);
                pattern.push_back(activation);
            }
        }
    }
}

NeuralPlanner* CognitiveSystems::getPlanner() {
    return planner_.get();
}

ConceptFormation* CognitiveSystems::getConceptFormation() {
    return conceptFormation_.get();
}

AttentionalSelection* CognitiveSystems::getAttention() {
    return attention_.get();
}

} // namespace nlm
