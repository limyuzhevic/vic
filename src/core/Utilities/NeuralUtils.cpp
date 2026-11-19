#include "MathUtils.hpp"
#include <random>

namespace nlm {

// NeuralUtils implementation
float NeuralUtils::initializeWeight(std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(rng);
}

void NeuralUtils::initializeWeights(std::vector<float>& weights, std::mt19937& rng) {
    weights.resize(weights.size());
    for (auto& weight : weights) {
        weight = initializeWeight(rng);
    }
}

void NeuralUtils::initializeWeights(std::vector<std::vector<float>>& weights, std::mt19937& rng) {
    for (auto& row : weights) {
        initializeWeights(row, rng);
    }
}

bool NeuralUtils::isSpikeTimeClose(float time1, float time2, float tolerance) {
    return std::abs(time1 - time2) <= tolerance;
}

float NeuralUtils::calculateSpikeFrequency(const std::vector<float>& spikeTimes) {
    if (spikeTimes.size() < 2) return 0.0f;
    
    float totalDuration = spikeTimes.back() - spikeTimes.front();
    if (totalDuration <= 0.0f) return 0.0f;
    
    return static_cast<float>(spikeTimes.size() - 1) / totalDuration;
}

std::vector<float> NeuralUtils::filterSpikeTimes(const std::vector<float>& spikeTimes, float minInterval) {
    std::vector<float> filtered;
    if (spikeTimes.empty()) return filtered;
    
    filtered.push_back(spikeTimes.front());
    for (size_t i = 1; i < spikeTimes.size(); ++i) {
        if (spikeTimes[i] - spikeTimes[i-1] >= minInterval) {
            filtered.push_back(spikeTimes[i]);
        }
    }
    return filtered;
}

float NeuralUtils::calculateSynapticStrength(float weight, float efficacy, float eligibilityTrace) {
    return weight * efficacy * eligibilityTrace;
}

bool NeuralUtils::isValidSynapticConnection(float weight, float minWeight, float maxWeight) {
    return weight >= minWeight && weight <= maxWeight;
}

bool NeuralUtils::isNeuronFiring(float membranePotential, float threshold) {
    return membranePotential >= threshold;
}

float NeuralUtils::calculateFiringRate(const std::vector<float>& spikeTimes, double duration) {
    if (spikeTimes.empty() || duration <= 0.0) return 0.0f;
    return static_cast<float>(spikeTimes.size()) / duration;
}

size_t NeuralUtils::calculateTotalConnections(const std::vector<size_t>& layerSizes) {
    size_t total = 0;
    for (size_t i = 0; i < layerSizes.size() - 1; ++i) {
        total += layerSizes[i] * layerSizes[i + 1];
    }
    return total;
}

std::vector<size_t> NeuralUtils::calculateLayerOutputSizes(const std::vector<size_t>& layerSizes) {
    std::vector<size_t> outputSizes;
    if (layerSizes.empty()) return outputSizes;
    
    for (size_t i = 1; i < layerSizes.size(); ++i) {
        outputSizes.push_back(layerSizes[i]);
    }
    return outputSizes;
}

} // namespace nlm
