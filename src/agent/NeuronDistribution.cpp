#include "NeuronDistribution.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

NeuronDistribution::NeuronDistribution(std::shared_ptr<Random> randomGenerator)
    : randomGenerator_(randomGenerator) {
    if (!randomGenerator_) {
        NLM_LOG_ERROR("Random generator is null in NeuronDistribution constructor");
        throw std::runtime_error("Random generator cannot be null");
    }
}

void NeuronDistribution::distribute(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups,
                                   const std::string& algorithm) {
    setAlgorithm(algorithm);
    
    if (groups.empty()) {
        NLM_LOG_ERROR("Cannot distribute neurons - no groups provided");
        return;
    }
    
    if (neurons.empty()) {
        NLM_LOG_DEBUG("No neurons to distribute");
        return;
    }
    
    NLM_LOG_INFO("Distributing " + std::to_string(neurons.size()) + " neurons among " + 
                 std::to_string(groups.size()) + " groups using " + algorithm + " algorithm");
    
    // Clear existing neurons from groups
    for (auto* group : groups) {
        group->clear();
    }
    
    // Choose distribution algorithm
    switch (getAlgorithmType()) {
        case AlgorithmType::MODULO:
            distributeModulo(neurons, groups);
            break;
        case AlgorithmType::RANDOM:
            distributeRandom(neurons, groups);
            break;
        case AlgorithmType::BALANCED:
            distributeBalanced(neurons, groups);
            break;
        case AlgorithmType::WEIGHTED:
            distributeWeighted(neurons, groups);
            break;
        case AlgorithmType::HIERARCHICAL:
            distributeHierarchical(neurons, groups);
            break;
        default:
            NLM_LOG_ERROR("Unknown distribution algorithm");
            distributeModulo(neurons, groups); // Fallback
            break;
    }
}

void NeuronDistribution::distributeModulo(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups) {
    for (size_t i = 0; i < neurons.size(); ++i) {
        size_t groupIndex = i % groups.size();
        groups[groupIndex]->addNeuron(neurons[i]);
    }
}

void NeuronDistribution::distributeRandom(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups) {
    std::vector<size_t> groupIndices(groups.size());
    std::iota(groupIndices.begin(), groupIndices.end(), 0);
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        size_t randomIndex = randomGenerator_->uniformInt(0, static_cast<uint32_t>(groups.size() - 1));
        groups[randomIndex]->addNeuron(neurons[i]);
    }
}

void NeuronDistribution::distributeBalanced(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups) {
    // Calculate how many neurons each group should get (as evenly as possible)
    size_t neuronsPerGroup = neurons.size() / groups.size();
    size_t remainder = neurons.size() % groups.size();
    
    // Sort groups by current neuron count (ascending) for balanced distribution
    std::vector<NeuronGroup*> sortedGroups = groups;
    std::sort(sortedGroups.begin(), sortedGroups.end(),
              [](const NeuronGroup* a, const NeuronGroup* b) {
                  return a->getNeuronCount() < b->getNeuronCount();
              });
    
    // Distribute neurons
    size_t neuronIndex = 0;
    for (size_t i = 0; i < groups.size(); ++i) {
        size_t neuronsToAdd = neuronsPerGroup + (i < remainder ? 1 : 0);
        
        for (size_t j = 0; j < neuronsToAdd && neuronIndex < neurons.size(); ++j) {
            sortedGroups[i]->addNeuron(neurons[neuronIndex]);
            ++neuronIndex;
        }
    }
}

void NeuronDistribution::distributeWeighted(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups) {
    // Calculate weights based on group priority
    std::vector<float> weights;
    float totalWeight = 0.0f;
    
    for (const auto* group : groups) {
        float weight = calculateGroupWeight(group);
        weights.push_back(weight);
        totalWeight += weight;
    }
    
    // Normalize weights
    if (totalWeight > 0.0f) {
        for (auto& weight : weights) {
            weight /= totalWeight;
        }
    }
    
    // Distribute neurons based on weights
    std::vector<size_t> groupIndices;
    for (size_t i = 0; i < groups.size(); ++i) {
        // Add the group to the selection list weighted by its priority
        size_t count = static_cast<size_t>(weights[i] * neurons.size());
        for (size_t j = 0; j < count; ++j) {
            groupIndices.push_back(i);
        }
    }
    
    // Ensure all groups get at least one neuron
    std::unordered_map<size_t, size_t> groupCounts;
    for (size_t i = 0; i < groups.size(); ++i) {
        groupCounts[i] = 0;
    }
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        size_t groupIndex = groupIndices[i % groupIndices.size()];
        groups[groupIndex]->addNeuron(neurons[i]);
        groupCounts[groupIndex]++;
    }
    
    // Log distribution results
    for (size_t i = 0; i < groups.size(); ++i) {
        if (groupCounts[i] > 0) {
            NLM_LOG_DEBUG("Group " + std::to_string(i) + " got " + std::to_string(groupCounts[i]) + " neurons");
        }
    }
}

void NeuronDistribution::distributeHierarchical(std::vector<Neuron*>& neurons, const std::vector<NeuronGroup*>& groups) {
    // Sort groups by priority (descending)
    std::vector<NeuronGroup*> sortedGroups = groups;
    sortGroupsByPriority(sortedGroups);
    
    // Distribute neurons hierarchically (higher priority groups get more neurons)
    size_t neuronsDistributed = 0;
    for (const auto* group : sortedGroups) {
        // Calculate how many neurons this group should get based on priority
        float priority = static_cast<float>(group->getPriority());
        size_t neuronsForGroup = static_cast<size_t>((neurons.size() * priority) / 100.0f);
        
        if (neuronsForGroup > 0) {
            for (size_t i = 0; i < neuronsForGroup && neuronsDistributed < neurons.size(); ++i) {
                group->addNeuron(neurons[neuronsDistributed]);
                ++neuronsDistributed;
            }
        }
    }
    
    // Distribute any remaining neurons evenly
    if (neuronsDistributed < neurons.size()) {
        size_t remaining = neurons.size() - neuronsDistributed;
        size_t groupIndex = 0;
        
        for (size_t i = 0; i < remaining; ++i) {
            sortedGroups[groupIndex]->addNeuron(neurons[neuronsDistributed]);
            ++neuronsDistributed;
            groupIndex = (groupIndex + 1) % sortedGroups.size();
        }
    }
}

void NeuronDistribution::sortGroupsByPriority(std::vector<NeuronGroup*>& groups) {
    std::sort(groups.begin(), groups.end(),
              [](const NeuronGroup* a, const NeuronGroup* b) {
                  return a->getPriority() > b->getPriority(); // Higher priority first
              });
}

float NeuronDistribution::calculateGroupWeight(const NeuronGroup* group) const {
    // Simple weight calculation based on priority
    return static_cast<float>(group->getPriority() + 1); // +1 to avoid zero weight
}

NeuronDistribution::AlgorithmType NeuronDistribution::getAlgorithmType() const {
    if (algorithmName_ == "MODULO") return AlgorithmType::MODULO;
    if (algorithmName_ == "RANDOM") return AlgorithmType::RANDOM;
    if (algorithmName_ == "BALANCED") return AlgorithmType::BALANCED;
    if (algorithmName_ == "WEIGHTED") return AlgorithmType::WEIGHTED;
    if (algorithmName_ == "HIERARCHICAL") return AlgorithmType::HIERARCHICAL;
    
    NLM_LOG_WARNING("Unknown distribution algorithm: " + algorithmName_);
    return AlgorithmType::MODULO;
}

std::string NeuronDistribution::getStats() const {
    std::stringstream ss;
    ss << "NeuronDistribution Stats:" << std::endl;
    ss << "  Algorithm: " << algorithmName_ << std::endl;
    ss << "  Algorithm Type: " << static_cast<int>(getAlgorithmType()) << std::endl;
    return ss.str();
}

} // namespace nlm
