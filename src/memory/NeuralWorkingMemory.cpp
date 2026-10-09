# pragma once

#include "Memory.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

/**
 * @brief Neural working memory system for maintaining active neural traces
 * 
 * Working memory in NLM is a short-term memory system that maintains the most
 * recent neural activity patterns. It operates on a competitive basis where
 * the strongest traces win and weaker traces decay over time.
 */

NeuralWorkingMemory::NeuralWorkingMemory() : Memory(), decayRate(0.95f), 
                                         competitionThreshold(0.6f), 
                                         maxActiveTraces(100), enableReplay(true) {}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    this->brain = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::setDecayRate(float rate) {
    decayRate = std::clamp(rate, 0.5f, 0.999f); // Reasonable decay range
}

void NeuralWorkingMemory::setCompetitionThreshold(float threshold) {
    competitionThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void NeuralWorkingMemory::setMaxActiveTraces(size_t max) {
    setCapacity(max);
    maxActiveTraces = max;
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuronId, float activation) {
    if (activation <= 0.0f) {
        return; // Only store positive activations
    }
    
    // Calculate trace strength based on activation and neuron type
    float strength = activation;
    if (auto* neuron = findNeuron(neuronId)) {
        NeuronType type = neuron->getType();
        if (type == NeuronType::Sensory) {
            strength *= 1.5f; // Sensory inputs get boosted strength
        } else if (type == NeuronType::Motor) {
            strength *= 1.2f; // Motor outputs get moderate boost
        }
    }
    
    // Create or update trace
    MemoryTrace trace;
    trace.neuronId = neuronId;
    trace.strength = strength;
    trace.timeToLive = 10.0f; // Traces live for 10 cleanup cycles
    trace.timestamp = brain ? brain->getCurrentStep() : 0;
    trace.type = MemoryTraceType::Working;
    
    Memory::addTrace(trace);
    
    // Apply competitive inhibition if needed
    if (getActiveTraces() > maxActiveTraces * 0.8f) {
        pruneWeakTraces(competitionThreshold * getMaxTraceValue());
    }
}

void NeuralWorkingMemory::update(float dt) {
    // Apply decay to all traces
    for (auto& trace : traces) {
        trace.strength *= decayRate;
        trace.timeToLive -= dt;
        
        // Update trace based on neural activity if brain is available
        if (brain && trace.timeToLive > 0.0f) {
            if (auto* neuron = findNeuron(trace.neuronId)) {
                float currentActivation = std::abs(neuron->getState().membranePotential - 
                                               neuron->getState().restingPotential) / 100.0f;
                trace.strength = std::max(trace.strength, currentActivation * 2.0f);
            }
        }
    }
    
    Memory::updateTraceMax();
}

void NeuralWorkingMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Convert episode to working memory traces
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        if (i < episode.neuronActivations.size()) {
            float activation = episode.neuronActivations[i];
            storeToNeuron(episode.activeNeurons[i], activation);
        }
    }
}

void NeuralWorkingMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!enableReplay || !episode) {
        return;
    }
    
    // Replay episode by strengthening relevant traces
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        if (i < episode->neuronActivations.size()) {
            float replayActivation = episode->neuronActivations[i] * 2.0f; // Boost during replay
            storeToNeuron(episode->activeNeurons[i], replayActivation);
        }
    }
    
    NLM_LOG_DEBUG("Replayed episode with " + std::to_string(episode->activeNeurons.size()) + 
                 " active neurons");
}

Neuron* NeuralWorkingMemory::findNeuron(NeuronId neuronId) {
    if (!brain) return nullptr;
    
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    return neuron;
                }
            }
        }
    }
    return nullptr;
}

const Neuron* NeuralWorkingMemory::findNeuron(NeuronId neuronId) const {
    return const_cast<NeuralWorkingMemory*>(this)->findNeuron(neuronId);
}

std::vector<NeuronId> NeuralWorkingMemory::getMemoryNeurons() const {
    std::vector<NeuronId> neuronIds;
    neuronIds.reserve(traces.size());
    for (const auto& trace : traces) {
        neuronIds.push_back(trace.neuronId);
    }
    return neuronIds;
}

float NeuralWorkingMemory::getAverageTraceStrength() const {
    if (traces.empty()) return 0.0f;
    
    float totalStrength = 0.0f;
    for (const auto& trace : traces) {
        totalStrength += trace.strength;
    }
    return totalStrength / static_cast<float>(traces.size());
}

void NeuralWorkingMemory::clear() {
    Memory::clear();
    NLM_LOG_DEBUG("NeuralWorkingMemory cleared");
}

void NeuralWorkingMemory::exportToStream(std::ostream& os) const {
    Memory::exportToStream(os);
    os << "  Decay rate: " << decayRate << std::endl;
    os << "  Competition threshold: " << competitionThreshold << std::endl;
    os << "  Max active traces: " << maxActiveTraces << std::endl;
    os << "  Replay enabled: " << (enableReplay ? "Yes" : "No") << std::endl;
}

} // namespace nlm
