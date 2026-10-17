#include "ModernCpp20Features.hpp"
#include <cmath>
#include <random>

namespace nlm {

// NeuronLike concept helper functions
NeuronId getNeuronId(const NeuronLike& neuron) {
    return neuron.getId();
}

MembranePotential getMembranePotential(const NeuronLike& neuron) {
    return neuron.getMembranePotential();
}

// SynapseLike concept helper functions  
SynapseId getSynapseId(const SynapseLike& synapse) {
    return synapse.getId();
}

SynapticWeight getSynapseWeight(const SynapseLike& synapse) {
    return synapse.getWeight();
}

// NeuralActivityRangeProcessor implementation
double NeuralActivityRangeProcessor::calculateSpikeRate(size_t iteration) const {
    if (activities_.empty()) return 0.0;
    
    double mean = calculateMean();
    double stddev = calculateStandardDeviation();
    
    if (stddev < 0.001) {
        return mean;
    }
    
    // Normalize and scale spike rate
    return mean / stddev;
}

bool NeuralActivityRangeProcessor::shouldStop(size_t iteration) const {
    // Example stopping condition: if spike rate stabilizes
    if (iteration < 10) return false;
    
    double currentRate = calculateSpikeRate(iteration);
    double previousRate = calculateSpikeRate(iteration - 1);
    
    return std::abs(currentRate - previousRate) < 0.001;
}

// NeuralStateManager implementation
double NeuralStateManager::getCurrentValue() const {
    return currentValue_;
}

std::string NeuralStateManager::getCurrentLabel() const {
    return currentLabel_;
}

// NeuralCheckpointManager implementation
// Implementation would be in header for this simple class

// NeuralOperationExecutor implementation
double NeuralOperationExecutor::calculateSynapticWeight(
    const Synapse& synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) {
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return std::unexpected("Insufficient spike data");
    }
    
    double weightChange = 0.0;
    
    for (const auto& preTime : preSpikes) {
        for (const auto& postTime : postSpikes) {
            double dt = static_cast<double>(postTime - preTime);
            weightChange += std::exp(-std::abs(dt) / 20.0);
        }
    }
    
    return weightChange * synapse.getWeight() * 0.01;
}

std::expected<std::vector<NeuronId>, std::string> NeuralOperationExecutor::findConnectedNeurons(
    NeuronId sourceNeuron,
    const std::unordered_map<NeuronId, std::vector<NeuronId>>& connectivity) {
    
    auto it = connectivity.find(sourceNeuron);
    if (it == connectivity.end()) {
        return std::unexpected("Source neuron not found in connectivity map");
    }
    
    return it->second;
}

// NeuralTaskProcessor implementation
double NeuralTaskProcessor::processNeuronActivity(NeuronId neuronId) {
    // Process neuron activity using modern C++20 features
    
    // Use std::ranges for data processing
    std::vector<double> spikes = getSpikeHistory(neuronId);
    
    if (spikes.empty()) {
        return 0.0;
    }
    
    // Use std::views for lazy evaluation
    auto spikeRates = spikes | std::views::transform([](double spike) { return 1.0 / (1.0 + std::abs(spike)); });
    
    // Use std::reduce for parallel summation
    double totalRate = std::reduce(spikeRates.begin(), spikeRates.end(), 0.0);
    
    return totalRate / spikes.size();
}

// NeuralOutputUtils implementation
// Implementation is in header for these utility functions

// NLMVersionUtils implementation
// Implementation is in header for these version utilities

} // namespace nlm