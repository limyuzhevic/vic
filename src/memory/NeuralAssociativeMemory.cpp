#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

for (const auto& assoc : associations_) {
            if (std::get<0>(assoc) == queryNeuron || std::get<1>(assoc) == queryNeuron) {
                // Retrieve the other neuron's pattern
                NeuronId otherNeuron = std::get<0>(assoc) == queryNeuron ? 
                    std::get<1>(assoc) : std::get<0>(assoc);
                
                auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                    [otherNeuron](const auto& pair) {
                                        return pair.first == otherNeuron;
                                    });
                if (it != patternNeurons_.end()) {
                    results.push_back(it->second);
                    if (results.size() >= maxResults) break;
                }
            }
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (!neuronA.value || !neuronB.value) return 0.0f;
    
    for (const auto& assoc : associations_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (!neuronA.value || !neuronB.value) return;
    
    for (auto& assoc : associations_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            break;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    std::vector<std::vector<float>> results;
    
    // Find direct associations
    auto direct = retrieve(cuePattern, 10);
    results.insert(results.end(), direct.begin(), direct.end());
    
    // In a full implementation, this would use spreading activation algorithm
    // where activated neurons activate their neighbors, and so on
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(
    const std::vector<float>& pattern) {
    // Find existing neuron with similar pattern
    float bestSim = 0.0f;
    NeuronId bestNeuron;
    
    for (const auto& pair : patternNeurons_) {
        float sim = computeSimilarity(pair.second, pattern);
        if (sim > bestSim) {
            bestSim = sim;
            bestNeuron = pair.first;
        }
    }
    
    if (bestSim >= 0.8f) {
        return bestNeuron;
    }
    
    // Create new neuron for pattern
    NeuronId newNeuron(patternNeurons_.size() + 10000);
    patternNeurons_.emplace_back(newNeuron, pattern);
    return newNeuron;
}

float NeuralAssociativeMemory::computeSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm

