#include "Synaptogenesis.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float probability;
    float maxConnections;
    std::vector<SynapseId> newSynapses;
    std::mt19937 rng;
    
    Impl() : formationRate(0.0001f), probability(0.15f), maxConnections(1000) {
        std::random_device rd;
        rng.seed(rd());
    }
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

void Synaptogenesis::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Synaptogenesis system initialized");
}

void Synaptogenesis::generateNewConnections(class Brain* brain) {
    if (!brain) return;
    
    pImpl->newSynapses.clear();
    
    // Generate new synaptic connections
    // Simple model: random connections between neurons in same region
    for (auto* region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) continue;
        
        // Limit connections per neuron to avoid explosion
        std::vector<bool> targetNeurons(neurons.size(), false);
        
        // For each neuron, decide whether to form new connections
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (static_cast<float>(region->getNeuronCount(i)) / neurons.size() > pImpl->probability) {
                continue; // Skip if already has many connections
            }
            
            // Form new connections to random target neurons
            size_t maxNew = static_cast<size_t>(neurons.size() * pImpl->formationRate);
            maxNew = std::min(maxNew, static_cast<size_t>(neurons.size() * pImpl->maxConnections / neurons.size()));
            
            std::vector<size_t> targets(neurons.size());
            std::iota(targets.begin(), targets.end(), 0);
            std::shuffle(targets.begin(), targets.end(), pImpl->rng);
            targets.resize(maxNew);
            
            for (size_t targetIdx : targets) {
                if (targetIdx == i) continue; // Don't form self-connections
                
                // Create new synapse (simplified - in real implementation would create actual Synapse objects)
                SynapseId synId(static_cast<uint64_t>(pImpl->newSynapses.size()));
                pImpl->newSynapses.push_back(synId);
                
                NLM_LOG_DEBUG("New synapse formed between neurons " + 
                              std::to_string(i) + " and " + std::to_string(targetIdx));
            }
        }
    }
    
    NLM_LOG_INFO("Generated " + std::to_string(pImpl->newSynapses.size()) + 
                 " new synaptic connections");
}

const std::vector<SynapseId>& Synaptogenesis::getNewSynapses() const {
    return pImpl->newSynapses;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::max(0.0f, std::min(rate, 0.01f));
}

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setProbability(float prob) {
    pImpl->probability = std::max(0.0f, std::min(prob, 1.0f));
}

float Synaptogenesis::getProbability() const {
    return pImpl->probability;
}

void Synaptogenesis::setMaxConnections(size_t max) {
    pImpl->maxConnections = std::max(10.0f, static_cast<float>(max));
}

size_t Synaptogenesis::getMaxConnections() const {
    return static_cast<size_t>(pImpl->maxConnections);
}

void Synaptogenesis::reset() {
    pImpl->newSynapses.clear();
}

} // namespace nlm
