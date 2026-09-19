#include "SleepSystem.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../memory/EpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct SleepSystem::Impl {
    class Brain* brain;
    float sleepDepth;               // 0.0 = awake, 1.0 = deep sleep
    float consolidationThreshold;   // Memory relevance threshold for consolidation
    SimulationStep sleepStartTime;   // When sleep began
    SimulationStep nextReplayTime;   // Next memory replay scheduled
    SimulationStep nextConsolidationTime; // Next consolidation scheduled
    SimulationStep nextDevelopmentTime;    // Next development update scheduled
    
    // Sleep state tracking
    bool isSleeping;                // Current sleep state
    float arousalLevel;            // Wake-up propensity
    float memoryReplayStrength;     // How strongly memories are replayed
    float synapticDownscaling;      // Global synaptic scaling during sleep
    
    // Memory replay tracking
    std::vector<NeuronId> replayedNeurons; // Neurons activated during replay
    std::vector<float> replayStrengths;    // Strength of replayed activations
    
    // Synaptic changes during sleep
    std::vector<std::tuple<NeuronId, NeuronId, float>> synapticChanges; // delta weight changes
    
    Impl() : brain(nullptr), sleepDepth(0.0f), consolidationThreshold(0.3f),
             sleepStartTime(0), nextReplayTime(100), nextConsolidationTime(2000),
             nextDevelopmentTime(1000), isSleeping(false), arousalLevel(0.5f),
             memoryReplayStrength(0.8f), synapticDownscaling(0.95f) {}
};

SleepSystem::SleepSystem() : pImpl(new Impl) {}

SleepSystem::~SleepSystem() = default;

void SleepSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Sleep system initialized");
    
    // Schedule initial sleep/wake cycle
    pImpl->sleepStartTime = 0;
    pImpl->nextReplayTime = 100;
    pImpl->nextConsolidationTime = 2000;
    pImpl->nextDevelopmentTime = 1000;
}

void SleepSystem::enterSleep(float sleepDepth) {
    // Enter sleep state
    pImpl->sleepDepth = sleepDepth;
    pImpl->isSleeping = true;
    pImpl->sleepStartTime = 0; // Current time would go here
    pImpl->arousalLevel = 0.5f - sleepDepth * 0.3f; // Deeper sleep = harder to wake
    
    NLM_LOG_INFO("Sleep system entered with depth: " + std::to_string(sleepDepth));
    
    // Schedule sleep-dependent processes
    pImpl->nextReplayTime = 100;
    pImpl->nextConsolidationTime = 2000;
    pImpl->nextDevelopmentTime = 1000;
}

void SleepSystem::exitSleep() {
    // Exit sleep state
    if (!pImpl->isSleeping) return;
    
    float sleepDuration = 0.0f; // Would be calculated from current time
    NLM_LOG_INFO("Sleep system exited after: " + std::to_string(sleepDuration) + "s");
    
    pImpl->isSleeping = false;
    pImpl->sleepDepth = 0.0f;
    pImpl->arousalLevel = 0.5f;
    pImpl->replayedNeurons.clear();
    pImpl->replayStrengths.clear();
    pImpl->synapticChanges.clear();
}

bool SleepSystem::isSleeping() const {
    return pImpl->isSleeping;
}

float SleepSystem::getSleepDepth() const {
    return pImpl->sleepDepth;
}

void SleepSystem::scheduleConsolidation() {
    // Schedule memory consolidation
    if (!pImpl->brain || !pImpl->brain->getEpisodicMemory()) {
        NLM_LOG_WARNING("Sleep system: No episodic memory available for consolidation");
        return;
    }
    
    // Check for memories worth consolidating
    auto* episodicMem = pImpl->brain->getEpisodicMemory();
    size_t episodeCount = episodicMem->getEpisodeCount();
    
    if (episodeCount < 10) {
        NLM_LOG_INFO("Sleep system: Not enough episodes for consolidation");
        return;
    }
    
    // Get recent episodes
    auto recentEpisodes = episodicMem->getRecentEpisodes(std::min<size_t>(50, episodeCount));
    
    // Consolidate based on relevance
    float totalRelevance = 0.0f;
    for (const auto& episode : recentEpisodes) {
        // Simple relevance calculation based on reward and activity
        float relevance = std::abs(episode.reward) + episode.neuronActivations.size() * 0.1f;
        totalRelevance += relevance;
    }
    
    if (totalRelevance > pImpl->consolidationThreshold * 100.0f) {
        NLM_LOG_INFO("Sleep system: Initiating memory consolidation");
        
        // Perform consolidation
        pImpl->brain->getEpisodicMemory()->consolidate(pImpl->consolidationThreshold);
        
        // Schedule next consolidation
        pImpl->nextConsolidationTime = static_cast<SimulationStep>(episodeCount * 10);
    } else {
        NLM_LOG_INFO("Sleep system: Skipping consolidation - insufficient relevance");
    }
}

void SleepSystem::performMemoryReplay() {
    // Perform memory replay during sleep
    if (!pImpl->isSleeping || !pImpl->brain) return;
    
    NLM_LOG_INFO("Sleep system: Performing memory replay");
    
    // Clear previous replay
    pImpl->replayedNeurons.clear();
    pImpl->replayStrengths.clear();
    pImpl->synapticChanges.clear();
    
    auto* episodicMem = pImpl->brain->getEpisodicMemory();
    if (!episodicMem) return;
    
    // Get episodes for replay
    auto episodesToReplay = episodicMem->getEpisodesForReplay(3);
    
    for (const auto* episode : episodesToReplay) {
        // Replay the episode
        replayEpisode(episode);
    }
    
    // Schedule next replay
    pImpl->nextReplayTime = pImpl->nextReplayTime + 100;
    
    // Apply synaptic downscaling (proteome)
    applySynapticDownscaling();
}

void SleepSystem::replayEpisode(const EpisodicMemoryItem* episode) {
    // Replay a single episode
    if (!episode) return;
    
    float replayStrength = std::min(pImpl->memoryReplayStrength, 
                                   static_cast<float>(episode->neuronActivations.size()) / 100.0f);
    
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuronId = episode->activeNeurons[i];
        float activation = episode->neuronActivations[i] * replayStrength;
        
        // Find neuron in brain and activate it
        replayNeuronActivation(neuronId, activation);
        
        // Store for statistics
        pImpl->replayedNeurons.push_back(neuronId);
        pImpl->replayStrengths.push_back(activation);
    }
    
    // Hebbian-like strengthening of replayed connections
    strengthenReplayConnections(episode);
}

void SleepSystem::replayNeuronActivation(NeuronId neuronId, float strength) {
    // Activate a neuron during memory replay
    if (!pImpl->brain) return;
    
    // Find the neuron in the brain
    for (auto& region : pImpl->brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                // Inject current proportional to replay strength
                float replayCurrent = strength * 5.0f; // Scale factor
                neuron->injectCurrent(replayCurrent);
                break;
            }
        }
    }
}

void SleepSystem::strengthenReplayConnections(const EpisodicMemoryItem* episode) {
    // Strengthen connections between replayed neurons
    if (!episode || episode->activeNeurons.size() < 2) return;
    
    // Simple rule: co-activated neurons during replay should strengthen
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron1 = episode->activeNeurons[i];
        float strength1 = episode->neuronActivations[i];
        
        for (size_t j = i + 1; j < episode->activeNeurons.size(); ++j) {
            NeuronId neuron2 = episode->activeNeurons[j];
            float strength2 = episode->neuronActivations[j];
            
            // Calculate connection strength based on co-activation
            float connectionStrength = strength1 * strength2 * 0.1f;
            
            // Record synaptic change
            pImpl->synapticChanges.emplace_back(neuron1, neuron2, connectionStrength);
        }
    }
}

void SleepSystem::applySynapticDownscaling() {
    // Apply global synaptic downscaling (proteome) during sleep
    if (!pImpl->brain) return;
    
    NLM_LOG_INFO("Sleep system: Applying synaptic downscaling");
    
    // Scale down all synaptic weights slightly
    // This is a simple implementation - in a real system this would be more sophisticated
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Reduce synaptic weight by a small factor
            float currentWeight = syn->getWeight();
            float scaledWeight = currentWeight * pImpl->synapticDownscaling;
            
            // Record change
            pImpl->synapticChanges.emplace_back(
                syn->getSourceNeuron(), 
                syn->getDestinationNeuron(), 
                scaledWeight - currentWeight
            );
        }
    }
}

void SleepSystem::updateDevelopmentSchedule() {
    // Update developmental schedule during sleep
    if (!pImpl->brain || !pImpl->brain->getDevelopmentSystem()) {
        NLM_LOG_WARNING("Sleep system: No development system available");
        return;
    }
    
    NLM_LOG_INFO("Sleep system: Updating development schedule");
    
    // Check if development update is needed
    if (pImpl->nextDevelopmentTime < pImpl->sleepStartTime) {
        // Schedule development update
        // In a real implementation, this would call the development system
        // to apply age-dependent changes
        
        pImpl->nextDevelopmentTime = pImpl->sleepStartTime + 1000;
        
        NLM_LOG_INFO("Sleep system: Development update scheduled");
    }
}

void SleepSystem::reset() {
    // Reset sleep system to initial state
    pImpl->sleepDepth = 0.0f;
    pImpl->isSleeping = false;
    pImpl->arousalLevel = 0.5f;
    pImpl->sleepStartTime = 0;
    pImpl->nextReplayTime = 100;
    pImpl->nextConsolidationTime = 2000;
    pImpl->nextDevelopmentTime = 1000;
    
    pImpl->replayedNeurons.clear();
    pImpl->replayStrengths.clear();
    pImpl->synapticChanges.clear();
    
    NLM_LOG_INFO("Sleep system reset");
}

float SleepSystem::getConsolidationThreshold() const {
    return pImpl->consolidationThreshold;
}

void SleepSystem::setConsolidationThreshold(float threshold) {
    pImpl->consolidationThreshold = threshold;
}

float SleepSystem::getMemoryReplayStrength() const {
    return pImpl->memoryReplayStrength;
}

void SleepSystem::setMemoryReplayStrength(float strength) {
    pImpl->memoryReplayStrength = strength;
}

const std::vector<SleepSystem::SleepEvent> SleepSystem::getRecentEvents(SimulationStep fromStep) const {
    std::vector<SleepEvent> result;
    
    // Add replay events
    for (size_t i = 0; i < pImpl->replayedNeurons.size(); ++i) {
        SleepEvent event;
        event.timestamp = fromStep + static_cast<SimulationStep>(i);
        event.type = "replay";
        event.neuronId = pImpl->replayedNeurons[i];
        event.strength = pImpl->replayStrengths[i];
        result.push_back(event);
    }
    
    // Add synaptic change events
    for (const auto& change : pImpl->synapticChanges) {
        SleepEvent event;
        event.timestamp = std::get<0>(change);
        event.type = "synaptic_change";
        event.neuronId = std::get<1>(change);
        event.strength = std::get<2>(change);
        result.push_back(event);
    }
    
    return result;
}

int SleepSystem::getReplayCount() const {
    return pImpl->replayedNeurons.size();
}

int SleepSystem::getSynapticChangesCount() const {
    return pImpl->synapticChanges.size();
}

} // namespace nlm
