// BrainMemoryIntegration.cpp - Working memory and episodic memory integration implementation
// Contains memory system integration functionality

#include "BrainMemoryIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Implementation of BrainMemoryIntegration

BrainMemoryIntegration::BrainMemoryIntegration(BrainCore* core) : pImpl(new Impl(core)) {}

BrainMemoryIntegration::~BrainMemoryIntegration() = default;

BrainMemoryIntegration::BrainMemoryIntegration(BrainMemoryIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainMemoryIntegration& BrainMemoryIntegration::operator=(BrainMemoryIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainMemoryIntegration::initialize() {
    NLM_LOG_INFO("Initializing Brain Memory Integration...");
    
    if (!pImpl->brainCore) {
        NLM_LOG_ERROR("Cannot initialize memory integration - no brain core available");
        return false;
    }
    
    pImpl->brainCore->getWorkingMemory();
    pImpl->brainCore->getEpisodicMemory();
    pImpl->brainCore->getAssociativeMemory();
    
    NLM_LOG_INFO("Brain Memory Integration initialized successfully");
    return true;
}

void BrainMemoryIntegration::update(TimestepDuration dt, SimulationStep currentStep, Timestamp currentTime) {
    updateWorkingMemory(dt);
    updateEpisodicMemory(currentStep, currentTime);
    updateAssociativeMemory();
    processMemoryReplay(currentStep);
    processMemoryConsolidation(currentStep);
}

void BrainMemoryIntegration::storeToNeuron(NeuronId neuronId, float activationLevel) {
    pImpl->recentActiveNeurons.push_back(neuronId);
    pImpl->recentActivationLevels.push_back(activationLevel);
    
    // Limit history size
    if (pImpl->recentActiveNeurons.size() > 100) {
        pImpl->recentActiveNeurons.erase(pImpl->recentActiveNeurons.begin());
        pImpl->recentActivationLevels.erase(pImpl->recentActivationLevels.begin());
    }
}

void BrainMemoryIntegration::captureCurrentStateAsEpisode(SimulationStep currentStep, Timestamp currentTime) {
    // Create new episode from current brain state
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.activeNeurons.clear();
    episode.neuronActivations.clear();
    
    // Capture active neurons from core brain
    if (pImpl->brainCore) {
        auto* workingMem = pImpl->brainCore->getWorkingMemory();
        auto* episodicMem = pImpl->brainCore->getEpisodicMemory();
        auto* associatMem = pImpl->brainCore->getAssociativeMemory();
        
        // Capture active working memory traces
        if (workingMem) {
            auto activeTraces = workingMem->getActiveTraces();
            for (const auto& trace : activeTraces) {
                episode.activeNeurons.push_back(trace.neuronId);
                episode.neuronActivations.push_back(trace.activation);
            }
        }
        
        // Capture reward from dopamine system if available
        auto* dopamine = pImpl->brainCore->getDopamine();
        if (dopamine) {
            episode.reward = dopamine->getLevel();
        }
        
        // Store episode
        if (episodicMem) {
            episodicMem->storeEpisode(episode);
        }
    }
}

void BrainMemoryIntegration::replayEpisode(const class EpisodicMemoryItem* episode) {
    if (!pImpl->brainCore || !episode) return;
    
    auto* workingMem = pImpl->brainCore->getWorkingMemory();
    if (workingMem) {
        for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
            workingMem->storeToNeuron(episode->activeNeurons[i], 
                                    episode->neuronActivations[i]);
        }
    }
}

void BrainMemoryIntegration::consolidateMemory(float threshold) {
    if (pImpl->brainCore) {
        auto* episodicMem = pImpl->brainCore->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->consolidate(threshold);
        }
    }
}

std::vector<const EpisodicMemoryItem*> BrainMemoryIntegration::getEpisodesForReplay(size_t count) {
    std::vector<const EpisodicMemoryItem*> episodes;
    if (pImpl->brainCore) {
        auto* episodicMem = pImpl->brainCore->getEpisodicMemory();
        if (episodicMem) {
            episodes = episodicMem->getEpisodesForReplay(count);
        }
    }
    return episodes;
}

void BrainMemoryIntegration::clear() {
    pImpl->recentActiveNeurons.clear();
    pImpl->recentActivationLevels.clear();
    
    if (pImpl->brainCore) {
        auto* workingMem = pImpl->brainCore->getWorkingMemory();
        auto* episodicMem = pImpl->brainCore->getEpisodicMemory();
        auto* associatMem = pImpl->brainCore->getAssociativeMemory();
        
        if (workingMem) workingMem->clear();
        if (episodicMem) episodicMem->clear();
        if (associatMem) associatMem->clear();
    }
}

size_t BrainMemoryIntegration::getActiveWorkingMemoryTraces() const {
    if (pImpl->brainCore) {
        auto* workingMem = pImpl->brainCore->getWorkingMemory();
        if (workingMem) {
            return workingMem->getActiveTraces();
        }
    }
    return 0;
}

size_t BrainMemoryIntegration::getEpisodeCount() const {
    if (pImpl->brainCore) {
        auto* episodicMem = pImpl->brainCore->getEpisodicMemory();
        if (episodicMem) {
            return episodicMem->getEpisodeCount();
        }
    }
    return 0;
}

void BrainMemoryIntegration::Impl::Impl(BrainCore* core) : brainCore(core) {}

void BrainMemoryIntegration::updateWorkingMemory(TimestepDuration dt) {
    if (pImpl->brainCore) {
        auto* workingMem = pImpl->brainCore->getWorkingMemory();
        if (workingMem) {
            workingMem->update(dt);
        }
    }
}

void BrainMemoryIntegration::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->brainCore) {
        auto* episodicMem = pImpl->brainCore->getEpisodicMemory();
        if (episodicMem) {
            // Update episodic memory with recent activity
            if (!pImpl->recentActiveNeurons.empty()) {
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.activeNeurons = pImpl->recentActiveNeurons;
                episode.neuronActivations = pImpl->recentActivationLevels;
                episodicMem->storeEpisode(episode);
            }
        }
    }
}

void BrainMemoryIntegration::updateAssociativeMemory() {
    if (pImpl->brainCore) {
        auto* associatMem = pImpl->brainCore->getAssociativeMemory();
        if (associatMem && !pImpl->recentActiveNeurons.empty()) {
            // Update associations based on recent neural activity patterns
            associatMem->update(pImpl->recentActiveNeurons);
        }
    }
}

void BrainMemoryIntegration::processMemoryReplay(SimulationStep currentStep) {
    if (pImpl->brainCore) {
        // Check if it's time for replay based on replay interval
        // This will be handled by the core brain's step function
        // Memory integration provides replay episodes when requested
    }
}

void BrainMemoryIntegration::processMemoryConsolidation(SimulationStep currentStep) {
    if (pImpl->brainCore) {
        // Check if it's time for consolidation based on consolidation interval
        // This will be handled by the core brain's step function
        // Memory integration provides consolidation when requested
    }
}

void BrainMemoryIntegration::calculateEpisodicStats() const {}

} // namespace nlm

