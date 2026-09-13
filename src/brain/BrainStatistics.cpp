#include "BrainStatistics.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include <sstream>

namespace nlm {

void BrainStatistics::update(class Brain* brain) {
    brainRef = brain;
    totalSpikesThisStep = 0;
    neuronsProcessed = 0;
    synapsesProcessed = 0;
    
    if (!brainRef) return;
    
    // Gather statistics from brain
    totalSpikesThisStep = brainRef->getTotalSpikeCount();
    neuronsProcessed = brainRef->getTotalNeuronCount();
    synapsesProcessed = brainRef->getTotalSynapseCount();
}

void BrainStatistics::reset() {
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    neuronsProcessed = 0;
    synapsesProcessed = 0;
}

size_t BrainStatistics::getTotalSpikeCount() const {
    if (!brainRef) return 0;
    return brainRef->getTotalSpikeCount();
}

size_t BrainStatistics::getPendingSpikeEventCount() const {
    if (!brainRef) return 0;
    return brainRef->getPendingSpikeEventCount();
}

size_t BrainStatistics::getTotalNeuronCount() const {
    if (!brainRef) return 0;
    return brainRef->getTotalNeuronCount();
}

size_t BrainStatistics::getTotalSynapseCount() const {
    if (!brainRef) return 0;
    return brainRef->getTotalSynapseCount();
}

size_t BrainStatistics::getActiveNeuronCount() const {
    if (!brainRef) return 0;
    return brainRef->getActiveNeuronCount();
}

size_t BrainStatistics::getFiringNeuronCount() const {
    if (!brainRef) return 0;
    return brainRef->getFiringNeuronCount();
}

float BrainStatistics::getAverageFiringRate() const {
    if (!brainRef) return 0.0f;
    return brainRef->getAverageFiringRate();
}

float BrainStatistics::getExcitationInhibitionRatio() const {
    if (!brainRef) return 0.0f;
    return brainRef->getExcitationInhibitionRatio();
}

size_t BrainStatistics::getWorkingMemoryActiveTraces() const {
    if (!brainRef || !brainRef->pImpl) return 0;
    if (!brainRef->pImpl->workingMemory) return 0;
    return brainRef->pImpl->workingMemory->getActiveTraces();
}

size_t BrainStatistics::getEpisodicMemoryEpisodeCount() const {
    if (!brainRef || !brainRef->pImpl) return 0;
    if (!brainRef->pImpl->episodicMemory) return 0;
    return brainRef->pImpl->episodicMemory->getEpisodeCount();
}

float BrainStatistics::getDopamineLevel() const {
    if (!brainRef || !brainRef->pImpl) return 0.0f;
    if (!brainRef->pImpl->dopamine) return 0.0f;
    return brainRef->pImpl->dopamine->getLevel();
}

float BrainStatistics::getCuriosityLevel() const {
    if (!brainRef || !brainRef->pImpl) return 0.0f;
    if (!brainRef->pImpl->curiosity) return 0.0f;
    return brainRef->pImpl->curiosity->getLevel();
}

float BrainStatistics::getNoveltyLevel() const {
    if (!brainRef || !brainRef->pImpl) return 0.0f;
    if (!brainRef->pImpl->novelty) return 0.0f;
    return brainRef->pImpl->novelty->getLevel();
}

float BrainStatistics::getPredictionErrorLevel() const {
    if (!brainRef || !brainRef->pImpl) return 0.0f;
    if (!brainRef->pImpl->predictionError) return 0.0f;
    return brainRef->pImpl->predictionError->getLevel();
}

DevelopmentalStage BrainStatistics::getDevelopmentalStage() const {
    if (!brainRef || !brainRef->pImpl) return DevelopmentalStage::Initial;
    return brainRef->pImpl->developmentalStage;
}

std::string BrainStatistics::getStatisticsString(class Brain* brain) const {
    std::stringstream ss;
    ss << "=== NLM Brain Statistics ===" << std::endl;
    ss << "Neural Statistics:" << std::endl;
    ss << "  Total spikes: " << getTotalSpikeCount() << std::endl;
    ss << "  Pending spike events: " << getPendingSpikeEventCount() << std::endl;
    ss << "  Total neurons: " << getTotalNeuronCount() << std::endl;
    ss << "  Total synapses: " << getTotalSynapseCount() << std::endl;
    ss << "  Active neurons: " << getActiveNeuronCount() << std::endl;
    ss << "  Firing neurons: " << getFiringNeuronCount() << std::endl;
    ss << "  Average firing rate: " << getAverageFiringRate() << std::endl;
    ss << "  E/I ratio: " << getExcitationInhibitionRatio() << std::endl;
    
    ss << "Memory Statistics:" << std::endl;
    ss << "  Working memory traces: " << getWorkingMemoryActiveTraces() << std::endl;
    ss << "  Episodic memory episodes: " << getEpisodicMemoryEpisodeCount() << std::endl;
    
    ss << "Neuromodulation Statistics:" << std::endl;
    ss << "  Dopamine level: " << getDopamineLevel() << std::endl;
    ss << "  Curiosity level: " << getCuriosityLevel() << std::endl;
    ss << "  Novelty level: " << getNoveltyLevel() << std::endl;
    ss << "  Prediction error level: " << getPredictionErrorLevel() << std::endl;
    
    ss << "Development Statistics:" << std::endl;
    ss << "  Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
    
    return ss.str();
}

void BrainStatistics::logStatistics(class Brain* brain) const {
    if (!brainRef) return;
    
    std::string stats = getStatisticsString(brain);
    // Log statistics - simplified
    // In a real system, this would use the logger
}

} // namespace nlm