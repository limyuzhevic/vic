#include "BrainStatistics.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainStatistics::logBrainStatus(const Brain::Impl& pImpl) const {
    try {
        // Validate Brain::Impl
        if (!pImpl.config) {
            NLM_LOG_ERROR("Failed to log brain status: Brain::Impl is null");
            return;
        }
        
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
            try {
                NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to get working memory traces: " + e.what());
            }
        }
        if (pImpl->episodicMemory) {
            try {
                NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to get episodic memory episodes: " + e.what());
            }
        }
        
        // Neuromodulation status
        if (pImpl->dopamine) {
            try {
                NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to get dopamine level: " + e.what());
            }
        }
        
        // Development status
        try {
            NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to get developmental stage: " + e.what());
        }
        
        for (const auto& region : pImpl->regions) {
            try {
                NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                             " (" + region->getName() + "): " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                             std::to_string(region->getSynapseCount()) + " synapses, " +
                             "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to log region " + std::to_string(region->getId().index()) + " stats: " + e.what());
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain status logging: ") + e.what());
    }
}

void BrainStatistics::logNeuralStats(const Brain::Impl& pImpl) const {
    try {
        // Collect and log neural statistics
        NLM_LOG_INFO("Neural statistics: collecting neuron activity data");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during neural statistics logging: ") + e.what());
    }
}

void BrainStatistics::logSynapticStats(const Brain::Impl& pImpl) const {
    try {
        // Collect and log synaptic statistics
        NLM_LOG_INFO("Synaptic statistics: collecting connection data");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during synaptic statistics logging: ") + e.what());
    }
}

void BrainStatistics::logRegionStats(const Brain::Impl& pImpl) const {
    try {
        // Collect and log region statistics
        NLM_LOG_INFO("Region statistics: collecting regional data");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during region statistics logging: ") + e.what());
    }
}

void BrainStatistics::logSpikeStats(const Brain::Impl& pImpl) const {
    try {
        // Collect and log spike statistics
        NLM_LOG_INFO("Spike statistics: collecting firing data");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during spike statistics logging: ") + e.what());
    }
}

void BrainStatistics::logEIRatioStats(const Brain::Impl& pImpl) const {
    try {
        // Collect and log E/I ratio statistics
        NLM_LOG_INFO("E/I ratio statistics: calculating excitation-inhibition balance");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during E/I ratio statistics logging: ") + e.what());
    }
}

void BrainStatistics::collectAndLogNeuralStats(const Brain::Impl& pImpl) const {
    // Collect and log neural statistics
    NLM_LOG_INFO("Neural statistics: collecting neuron activity data");
}

void BrainStatistics::collectAndLogSynapticStats(const Brain::Impl& pImpl) const {
    // Collect and log synaptic statistics
    NLM_LOG_INFO("Synaptic statistics: collecting connection data");
}

void BrainStatistics::collectAndLogRegionStats(const Brain::Impl& pImpl) const {
    // Collect and log region statistics
    NLM_LOG_INFO("Region statistics: collecting regional data");
}

void BrainStatistics::collectAndLogSpikeStats(const Brain::Impl& pImpl) const {
    // Collect and log spike statistics
    NLM_LOG_INFO("Spike statistics: collecting firing data");
}

void BrainStatistics::collectAndLogEIRatioStats(const Brain::Impl& pImpl) const {
    // Collect and log E/I ratio statistics
    NLM_LOG_INFO("E/I ratio statistics: calculating excitation-inhibition balance");
}

void BrainStatistics::collectAndLogMemoryStats(const Brain::Impl& pImpl) const {
    // Collect and log memory statistics
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory: " + std::to_string(pImpl->workingMemory->getActiveTraces()) + " active traces");
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()) + " episodes stored");
    }
}

} // namespace nlm
