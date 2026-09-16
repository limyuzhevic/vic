#include "BrainStats.hpp"
#include "Brain.hpp"

namespace nlm {

// Brain::Impl statistics - BrainStats class implementation

BrainStats::BrainStats(Brain::Impl* impl) : pImpl(impl) {}

void BrainStats::collectStatistics() const {
    // Collect all statistics in one method
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Collect all statistics
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(pImpl->getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(pImpl->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(pImpl->getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(pImpl->getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(pImpl->getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(pImpl->getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
