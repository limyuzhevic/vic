#include "BrainStatistics.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

// ===========================================
// NEURAL STATISTICS
// ===========================================

void BrainStatistics::logNeuralStatistics(const Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("=== NLM Brain Neural Statistics ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
}

void BrainStatistics::logRegionStatistics(const Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("=== Region Statistics ===");
    for (const auto& region : brain->getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

// ===========================================
// MEMORY STATISTICS
// ===========================================

void BrainStatistics::logMemoryStatistics(const Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("=== Memory System Statistics ===");

    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }

    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }

    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        NLM_LOG_INFO("Associative memory patterns: " + std::to_string(associativeMemory->getPatternCount()));
    }
}

// ===========================================
// NEUROMODULATION STATISTICS
// ===========================================

void BrainStatistics::logNeuromodulationStatistics(const Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("=== Neuromodulation System Statistics ===");

    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }

    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity->getLevel()));
    }

    auto* novelty = brain->getNovelty();
    if (novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(novelty->getLevel()));
    }

    auto* predictionError = brain->getPredictionErrorSignal();
    if (predictionError) {
        NLM_LOG_INFO("Prediction error level: " + std::to_string(predictionError->getLevel()));
    }
}

// ===========================================
// DEVELOPMENT STATISTICS
// ===========================================

void BrainStatistics::logDevelopmentStatistics(const Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("=== Development System Statistics ===");
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));

    // Additional development metrics
    // This would include things like synaptic density, plasticity rates, etc.
}

// ===========================================
// COMPLETE BRAIN STATUS LOG
// ===========================================

void BrainStatistics::logCompleteBrainStatus(const Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");

    // Basic neural statistics
    logNeuralStatistics(brain);

    // Memory statistics
    logMemoryStatistics(brain);

    // Neuromodulation statistics
    logNeuromodulationStatistics(brain);

    // Development statistics
    logDevelopmentStatistics(brain);

    // Region statistics
    logRegionStatistics(brain);
}

} // namespace nlm
