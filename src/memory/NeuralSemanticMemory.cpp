#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Concept storage
    std::vector<std::string> concepts;
    std::vector<std::vector<float>> conceptPatterns;
    std::vector<float> conceptActivations;
    std::vector<float> conceptStrength;
    std::vector<size_t> conceptUsageCount;
    std::vector<double> conceptLastUsed;
    
    // Semantic associations between concepts
    std::vector<std::tuple<size_t, size_t, float>> associations;
    
    // Semantic network connections
    std::vector<std::pair<size_t, size_t>> semanticConnections;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    pImpl = std::make_unique<Impl>();
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeConcept(const std::string& concept, const std::vector<float>& pattern) {
    if (!brain_) return;
    
    // Check if concept already exists
    auto it = std::find(pImpl->concepts.begin(), pImpl->concepts.end(), concept);
    if (it != pImpl->concepts.end()) {
        size_t idx = std::distance(pImpl->concepts.begin(), it);
        pImpl->conceptPatterns[idx] = pattern;
        pImpl->conceptStrength[idx] = 1.0f;
        pImpl->conceptActivations[idx] = 0.0f;
        pImpl->conceptUsageCount[idx] = 0;
        return;
    }
    
    // Add new concept
    pImpl->concepts.push_back(concept);
    pImpl->conceptPatterns.push_back(pattern);
    pImpl->conceptActivations.push_back(0.0f);
    pImpl->conceptStrength.push_back(1.0f);
    pImpl->conceptUsageCount.push_back(0);
    pImpl->conceptLastUsed.push_back(0.0);
    
    // Create semantic connections to existing concepts
    for (size_t i = 0; i < pImpl->concepts.size() - 1; ++i) {
        // Create bidirectional connections
        pImpl->semanticConnections.emplace_back(i, pImpl->concepts.size() - 1);
        pImpl->semanticConnections.emplace_back(pImpl->concepts.size() - 1, i);
    }
    
    // Encode concept as neural activation pattern
    std::vector<float> neuralPattern = pattern;
    if (!neuralPattern.empty()) {
        float maxVal = *std::max_element(neuralPattern.begin(), neuralPattern.end());
        if (maxVal > 0.0f) {
            for (float& val : neuralPattern) {
                val /= maxVal;
            }
        }
    } else {
        // Generate random pattern if none provided
        size_t patternSize = 100;
        neuralPattern.resize(patternSize);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        for (float& val : neuralPattern) {
            val = dis(gen);
        }
    }
    
    // Store neural representation
    pImpl->conceptPatterns.back() = neuralPattern;
    
    // Create recurrent connections for maintenance
    createSemanticConnections(pImpl->concepts.size() - 1);
    
    NLM_LOG_INFO("Stored semantic concept: " + concept);
}

std::vector<float> NeuralSemanticMemory::retrieveConcept(const std::string& concept) const {
    auto it = std::find(pImpl->concepts.begin(), pImpl->concepts.end(), concept);
    if (it != pImpl->concepts.end()) {
        size_t idx = std::distance(pImpl->concepts.begin(), it);
        return pImpl->conceptPatterns[idx];
    }
    return std::vector<float>();
}

bool NeuralSemanticMemory::hasConcept(const std::string& concept) const {
    return std::find(pImpl->concepts.begin(), pImpl->concepts.end(), concept) != pImpl->concepts.end();
}

std::vector<std::string> NeuralSemanticMemory::getAllConcepts() const {
    return pImpl->concepts;
}

void NeuralSemanticMemory::reinforceConcept(const std::string& concept, float strength) {
    auto it = std::find(pImpl->concepts.begin(), pImpl->concepts.end(), concept);
    if (it != pImpl->concepts.end()) {
        size_t idx = std::distance(pImpl->concepts.begin(), it);
        pImpl->conceptStrength[idx] = std::min(1.0f, pImpl->conceptStrength[idx] + strength);
        pImpl->conceptActivations[idx] = std::min(1.0f, pImpl->conceptActivations[idx] + strength);
        pImpl->conceptUsageCount[idx]++;
        pImpl->conceptLastUsed[idx] = pImpl->brain ? pImpl->brain->getCurrentStep() : 0.0;
    }
}

float NeuralSemanticMemory::getConceptActivation(const std::string& concept) const {
    auto it = std::find(pImpl->concepts.begin(), pImpl->concepts.end(), concept);
    if (it != pImpl->concepts.end()) {
        size_t idx = std::distance(pImpl->concepts.begin(), it);
        return pImpl->conceptActivations[idx];
    }
    return 0.0f;
}

float NeuralSemanticMemory::getSemanticActivity() const {
    if (pImpl->conceptActivations.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (float activation : pImpl->conceptActivations) {
        totalActivity += activation;
    }
    return totalActivity / pImpl->conceptActivations.size();
}

void NeuralSemanticMemory::createSemanticConnections(size_t conceptIdx) {
    if (!brain_ || conceptIdx >= pImpl->conceptPatterns.size()) return;
    
    // Get all neurons from brain
    std::vector<Neuron*> allNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                allNeurons.push_back(neuron);
            }
        }
    }
    
    if (allNeurons.empty()) return;
    
    // Create semantic connections for this concept
    // Each concept activates a subset of neurons based on its pattern
    const std::vector<float>& pattern = pImpl->conceptPatterns[conceptIdx];
    size_t neuronsToConnect = std::min<size_t>(pattern.size(), allNeurons.size());
    
    for (size_t i = 0; i < neuronsToConnect; ++i) {
        if (pattern[i] > 0.5f) {  // Strong activation threshold
            NeuronId from = allNeurons[i]->getId();
            NeuronId to = allNeurons[conceptIdx % allNeurons.size()]->getId();
            
            // Create recurrent connection for semantic maintenance
            if (from != to) {
                createRecurrentConnection(from, to, pattern[i] * 0.5f);
            }
        }
    }
}

void NeuralSemanticMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    if (!brain_) return;
    
    // Check if connection already exists
    for (const auto& conn : pImpl->semanticConnections) {
        if ((conn.first == from.index() && conn.second == to.index()) ||
            (conn.first == to.index() && conn.second == from.index())) {
            return;
        }
    }
    
    pImpl->semanticConnections.emplace_back(from.index(), to.index());
    
    // Apply connection strength to actual neural connections
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                if (neuron->getId() == from) {
                    for (Neuron* target : pop->getNeurons()) {
                        if (target->getId() == to) {
                            // Apply semantic connection as synaptic input
                            target->injectCurrent(strength * 10.0f);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void NeuralSemanticMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Decay concept activations
    for (float& activation : pImpl->conceptActivations) {
        activation *= (1.0f - 0.01f);  // Decay rate
    }
    
    // Update semantic connections
    updateSemanticConnections(dt);
    
    // Propagate activation through semantic network
    propagateSemanticActivation(dt);
}

void NeuralSemanticMemory::updateSemanticConnections(TimestepDuration dt) {
    // Gradually strengthen weak associations
    for (auto& assoc : pImpl->associations) {
        size_t idx1 = std::get<0>(assoc);
        size_t idx2 = std::get<1>(assoc);
        float& strength = std::get<2>(assoc);
        
        if (strength < 1.0f) {
            strength = std::min(1.0f, strength + 0.001f * static_cast<float>(dt));
        }
    }
}

void NeuralSemanticMemory::propagateSemanticActivation(TimestepDuration dt) {
    if (pImpl->semanticConnections.empty()) return;
    
    // Create activation map
    std::vector<float> nodeActivations(pImpl->concepts.size(), 0.0f);
    for (size_t i = 0; i < pImpl->concepts.size(); ++i) {
        nodeActivations[i] = pImpl->conceptActivations[i] * pImpl->conceptStrength[i];
    }
    
    // Propagate through connections
    for (const auto& conn : pImpl->semanticConnections) {
        size_t from = conn.first;
        size_t to = conn.second;
        
        if (from < nodeActivations.size() && to < nodeActivations.size()) {
            float activation = nodeActivations[from] * 0.5f;  // Connection weight
            nodeActivations[to] += activation;
            
            // Apply to neurons
            if (brain_) {
                brain_->injectCurrentToNeurons(NeuronType::Internal, activation * 2.0f);
            }
        }
    }
    
    // Update concept activations
    for (size_t i = 0; i < pImpl->conceptActivations.size(); ++i) {
        pImpl->conceptActivations[i] = std::min(1.0f, nodeActivations[i]);
    }
}

void NeuralSemanticMemory::clear() {
    pImpl->concepts.clear();
    pImpl->conceptPatterns.clear();
    pImpl->conceptActivations.clear();
    pImpl->conceptStrength.clear();
    pImpl->conceptUsageCount.clear();
    pImpl->conceptLastUsed.clear();
    pImpl->associations.clear();
    pImpl->semanticConnections.clear();
}

} // namespace nlm