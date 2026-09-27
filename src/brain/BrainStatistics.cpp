// BrainStatistics.cpp - Statistics and logging integration implementation
#include "BrainStatistics.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

BrainStatistics::BrainStatistics() : brain_(nullptr) {}

BrainStatistics::~BrainStatistics() = default;

float BrainStatistics::getExcitationInhibitionRatio() const {
    if (!brain_) return 0.0f;
    return brain_->getExcitationInhibitionRatio();
}

size_t BrainStatistics::getTotalSpikeCount() const {
    if (!brain_) return 0;
    return brain_->getTotalSpikeCount();
}

size_t BrainStatistics::getPendingSpikeEventCount() const {
    if (!brain_) return 0;
    return brain_->getPendingSpikeEventCount();
}

size_t BrainStatistics::getTotalNeuronCount() const {
    if (!brain_) return 0;
    return brain_->getTotalNeuronCount();
}

size_t BrainStatistics::getTotalSynapseCount() const {
    if (!brain_) return 0;
    return brain_->getTotalSynapseCount();
}

size_t BrainStatistics::getActiveNeuronCount() const {
    if (!brain_) return 0;
    return brain_->getActiveNeuronCount();
}

size_t BrainStatistics::getFiringNeuronCount() const {
    if (!brain_) return 0;
    return brain_->getFiringNeuronCount();
}

float BrainStatistics::getAverageFiringRate() const {
    if (!brain_) return 0.0f;
    return brain_->getAverageFiringRate();
}

void BrainStatistics::logStatus() const {
    if (!brain_) {
        NLM_LOG_ERROR("BrainStatistics: Cannot log - no brain reference!");
        return;
    }
    
    brain_->logStatus();
}

std::string BrainStatistics::getStatusReport() const {
    if (!brain_) return "No brain reference";
    
    std::stringstream ss;
    ss << "=== NLM Brain Statistics (Phase 6 - Integrated) ===" << std::endl;
    ss << "Regions: " << brain_->getRegionCount() << std::endl;
    ss << "Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
    ss << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
    ss << "Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
    ss << "Firing neurons (this step): " << brain_->getFiringNeuronCount() << std::endl;
    ss << "Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    ss << "Pending spike events: " << brain_->getPendingSpikeEventCount() << std::endl;
    ss << "Average firing rate: " << std::fixed << std::setprecision(3) 
       << brain_->getAverageFiringRate() << std::endl;
    ss << "E/I ratio: " << std::fixed << std::setprecision(3) 
       << brain_->getExcitationInhibitionRatio() << std::endl;
    
    // Memory system status
    auto* workingMem = brain_->getWorkingMemory();
    if (workingMem) {
        ss << "Working memory traces: " << workingMem->getActiveTraces() << std::endl;
    }
    
    auto* episodicMem = brain_->getEpisodicMemory();
    if (episodicMem) {
        ss << "Episodic memory episodes: " << episodicMem->getEpisodeCount() << std::endl;
    }
    
    // Neuromodulation status
    auto* dopamine = brain_->getDopamine();
    if (dopamine) {
        ss << "Dopamine level: " << std::fixed << std::setprecision(3) 
           << dopamine->getLevel() << std::endl;
    }
    
    // Development status
    ss << "Developmental stage: " << static_cast<int>(brain_->getDevelopmentalStage()) 
       << std::endl;
    
    return ss.str();
}

} // namespace nlm