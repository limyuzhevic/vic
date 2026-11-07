#include "StatisticsManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

StatisticsManager::StatisticsManager(Brain* brain)
    : brain_(brain) {
}

StatisticsManager::~StatisticsManager() = default;

bool StatisticsManager::initialize() {
    NLM_LOG_INFO("Initializing Statistics Manager...");
    
    NLM_LOG_INFO("Statistics Manager initialized successfully");
    return true;
}

void StatisticsManager::reset() {
    // Statistics are reset on brain reset
}

void StatisticsManager::step(SimulationStep currentStep, Timestamp currentTime) {
    updateStepStatistics(currentStep, currentTime);
}

void StatisticsManager::updateStepStatistics(SimulationStep currentStep, Timestamp currentTime) {
    // Update statistics for logging
}

void StatisticsManager::logStatus() const {
    if (!brain_) return;
    
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain_->getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain_->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain_->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain_->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain_->getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain_->getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain_->getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()));
    
    // Memory system status
    if (brain_->pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(brain_->pImpl->workingMemory->getActiveTraces()));
    }
    if (brain_->pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(brain_->pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (brain_->pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(brain_->pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain_->pImpl->developmentalStage)));
    
    for (const auto& region : brain_->pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
