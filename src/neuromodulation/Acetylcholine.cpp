#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float attentionGain;
    float memoryConsolidation;
    float decayRate;
    float learningRate;
    SimulationStep lastAttentionUpdate;
    std::vector<NeuronId> attendedNeurons;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , attentionGain(1.0f)
        , memoryConsolidation(0.0f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , lastAttentionUpdate(0) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates plasticity: enhances LTP when attention is high
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Memory consolidation effect
    if (pImpl->memoryConsolidation > 0.5f) {
        // Strong memory consolidation - reactivate memory traces
        for (const auto& neuronId : pImpl->attendedNeurons) {
            // Find neuron in brain
            for (auto& region : pImpl->brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == neuronId) {
                            // Enhance activity for consolidation
                            neuron->injectCurrent(pImpl->level * 2.0f);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Update attention system
    if (pImpl->brain->getAttention()) {
        pImpl->brain->getAttention()->update(pImpl->timestep);
    }
}

void Acetylcholine::enhanceAttention(const std::vector<NeuronId>& neurons) {
    // Select neurons to attend to based on novelty, relevance, etc.
    if (neurons.empty()) return;
    
    // Find most active/neuronId
    float maxActivity = 0.0f;
    NeuronId bestNeuron;
    
    for (const auto& neuronId : neurons) {
        float activity = 0.0f;
        // Get current activity from working memory
        if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
            activity = pImpl->brain->getWorkingMemory()->getNeuronActivation(neuronId);
        }
        
        if (activity > maxActivity) {
            maxActivity = activity;
            bestNeuron = neuronId;
        }
    }
    
    if (maxActivity > 0.1f) {
        pImpl->attendedNeurons.clear();
        pImpl->attendedNeurons.push_back(bestNeuron);
        
        // Signal attention to attended neuron
        if (pImpl->brain) {
            pImpl->brain->injectCurrent(bestNeuron, pImpl->level * 5.0f);
        }
        
        // Log attention selection
        NLM_LOG_INFO("Acetylcholine: Attending to neuron " + 
                    std::to_string(bestNeuron.getId()) + 
                    " with activity " + std::to_string(maxActivity));
    }
}

void Acetylcholine::consolidateMemory(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Store pattern in attended neurons for consolidation
    pImpl->attendedNeurons.clear();
    
    // Create neural representation of pattern
    for (size_t i = 0; i < pattern.size() && i < 10; ++i) {
        if (pattern[i] > 0.3f) {
            NeuronId neuron(i + 10000);  // Create synthetic neuron for pattern
            pImpl->attendedNeurons.push_back(neuron);
        }
    }
    
    // Increase memory consolidation strength
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.2f * pImpl->level);
    
    // Schedule consolidation with the attended neurons
    if (pImpl->brain && !pImpl->attendedNeurons.empty()) {
        for (const auto& neuronId : pImpl->attendedNeurons) {
            pImpl->brain->injectCurrent(neuronId, pImpl->level * 3.0f);
        }
    }
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->level;
}

bool Acetylcholine::isAttendingTo(NeuronId neuron) const {
    return std::find(pImpl->attendedNeurons.begin(), 
                    pImpl->attendedNeurons.end(), neuron) != pImpl->attendedNeurons.end();
}

} // namespace nlm
