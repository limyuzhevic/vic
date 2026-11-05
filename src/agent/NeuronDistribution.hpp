// NeuronDistribution.hpp
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <random>
#include "../core/Types/Types.hpp"
#include "NeuronGroup.hpp"

namespace nlm {

class NeuronDistribution {
public:
    NeuronDistribution(std::shared_ptr<Random> randomGenerator);
    ~NeuronDistribution() = default;
    
    // Distribute neurons among groups
    void distribute(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups,
                   const std::string& algorithm = "MODULO");
    
    // Get distribution algorithm name
    std::string getAlgorithmName() const { return algorithmName_; }
    
    // Set distribution algorithm
    void setAlgorithm(const std::string& algorithm) { algorithmName_ = algorithm; }
    
    // Get algorithm type
    enum AlgorithmType {
        MODULO,
        RANDOM,
        BALANCED,
        WEIGHTED,
        HIERARCHICAL
    };
    
    AlgorithmType getAlgorithmType() const;
    
    // Get distribution statistics
    std::string getStats() const;
    
private:
    std::shared_ptr<Random> randomGenerator_;
    std::string algorithmName_ = "MODULO";
    
    // Algorithm implementations
    void distributeModulo(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups);
    void distributeRandom(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups);
    void distributeBalanced(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups);
    void distributeWeighted(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups);
    void distributeHierarchical(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups);
    
    // Helper methods
    void sortGroupsByPriority(std::vector<NeuronGroup*>& groups);
    float calculateGroupWeight(const NeuronGroup* group) const;
};

} // namespace nlm
