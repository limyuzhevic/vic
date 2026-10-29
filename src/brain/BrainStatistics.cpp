#include "BrainHeaders.hpp"
#include "../core/Logger/Logger.hpp"
#include <iomanip>

namespace nlm {

void BrainStatistics::collectAndLogStatistics(Brain* brain) {
    if (!brain) return;
    
    // Calculate all statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float avgFiringRate = brain->getAverageFiringRate();
    float eIRatio = brain->getExcitationInhibitionRatio();
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t pendingSpikeEvents = brain->getPendingSpikeEventCount();
    
    // Log statistics
    NLM_LOG_INFO("=== BRAIN STATISTICS ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(totalNeurons) + " (Active: " + std::to_string(activeNeurons) + ", Firing: " + std::to_string(firingNeurons) + ")");
    NLM_LOG_INFO("Synapses: " + std::to_string(totalSynapses));
    NLM_LOG_INFO("Spikes: " + std::to_string(totalSpikes) + " (Pending: " + std::to_string(pendingSpikeEvents) + ")");
    NLM_LOG_INFO("Firing Rate: " + std::to_string(avgFiringRate) + " Hz");
    NLM_LOG_INFO("E/I Ratio: " + std::to_string(eIRatio));
    
    // Log memory statistics
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        NLM_LOG_INFO("Working Memory: " + std::to_string(workingMem->getActiveNeurons()) + " active neurons");
    }
    
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        NLM_LOG_INFO("Episodic Memory: " + std::to_string(episodicMem->getEpisodeCount()) + " episodes");
    }
    
    auto* associativeMem = brain->getAssociativeMemory();
    if (associativeMem) {
        NLM_LOG_INFO("Associative Memory: " + std::to_string(associativeMem->getAssociationCount()) + " associations");
    }
    
    // Log neuromodulation statistics
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("Dopamine Level: " + std::to_string(dopamine->getLevel()));
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        NLM_LOG_INFO("Curiosity Level: " + std::to_string(curiosity->getLevel()));
    }
    
    auto* novelty = brain->getNovelty();
    if (novelty) {
        NLM_LOG_INFO("Novelty Level: " + std::to_string(novelty->getLevel()));
    }
}

void BrainStatistics::logBrainStatus(const Brain* brain) const {
    if (!brain) return;
    
    NLM_LOG_INFO("=== BRAIN STATUS ===");
    NLM_LOG_INFO("Step: " + std::to_string(brain->pImpl->currentStep));
    NLM_LOG_INFO("Time: " + std::to_string(brain->pImpl->currentTime) + " seconds");
    NLM_LOG_INFO("Development Stage: " + std::to_string(static_cast<int>(brain->pImpl->developmentalStage)));
    NLM_LOG_INFO("Neuron Count: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Synapse Count: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Region Count: " + std::to_string(brain->getRegionCount()));
}

void BrainStatistics::calculateExcitatoryInhibitionRatio(Brain* brain) {
    if (!brain) return;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getType() == SynapseType::Excitatory) {
                totalExcitatory += syn->getWeight();
            } else {
                totalInhibitory += syn->getWeight();
            }
        }
    }
    
    // The E/I ratio is excitatory / (inhibitory + small epsilon to avoid division by zero)
    float eIRatio = totalExcitatory / (totalInhibitory + 0.001f);
    
    NLM_LOG_INFO("Calculated E/I ratio: " + std::to_string(eIRatio));
}

void BrainStatistics::trackSpikeStatistics(Brain* brain) {
    if (!brain) return;
    
    size_t currentStep = brain->pImpl->currentStep;
    size_t totalSpikes = brain->getTotalSpikeCount();
    
    // Log spike rate for this step
    float spikeRate = static_cast<float>(brain->pImpl->totalSpikesThisStep) / brain->pImpl->timestep;
    NLM_LOG_INFO("Step " + std::to_string(currentStep) + " spike rate: " + std::to_string(spikeRate) + " Hz");
}

void BrainStatistics::trackMemoryStatistics(Brain* brain) {
    if (!brain) return;
    
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        size_t workingMemoryActivity = workingMem->getActiveNeurons();
        NLM_LOG_INFO("Working Memory Activity: " + std::to_string(workingMemoryActivity));
    }
    
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        size_t episodicMemorySize = episodicMem->getEpisodeCount();
        NLM_LOG_INFO("Episodic Memory Size: " + std::to_string(episodicMemorySize));
    }
}

void BrainStatistics::trackPerformanceMetrics(Brain* brain) {
    if (!brain) return;
    
    // Track performance metrics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSpikes = brain->getTotalSpikeCount();
    
    if (totalNeurons > 0) {
        float efficiency = static_cast<float>(totalSpikes) / totalNeurons;
        NLM_LOG_INFO("Neural Efficiency: " + std::to_string(efficiency) + " spikes/neuron");
    }
}

} // namespace nlm