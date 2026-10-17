// LearningExperiment.cpp
// Implementation of LearningExperiment utilities

#include "LearningExperiment.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

// Include necessary type headers for the implementation
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {
namespace experiment_utils {

// Calculate correlation between two weight vectors
float calculateWeightCorrelation(const std::vector<float>& v1, const std::vector<float>& v2) {
    if (v1.empty() || v2.empty() || v1.size() != v2.size()) {
        return 0.0f;
    }
    
    float sum1 = std::accumulate(v1.begin(), v1.end(), 0.0f);
    float sum2 = std::accumulate(v2.begin(), v2.end(), 0.0f);
    
    float mean1 = sum1 / v1.size();
    float mean2 = sum2 / v2.size();
    
    float numerator = 0.0f;
    float denom1 = 0.0f;
    float denom2 = 0.0f;
    
    for (size_t i = 0; i < v1.size(); ++i) {
        float diff1 = v1[i] - mean1;
        float diff2 = v2[i] - mean2;
        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }
    
    if (denom1 == 0.0f || denom2 == 0.0f) {
        return 0.0f;
    }
    
    return numerator / std::sqrt(denom1 * denom2);
}

// Calculate statistical measures
WeightStats calculateWeightStats(const std::vector<float>& weights) {
    WeightStats stats;
    
    if (weights.empty()) {
        stats.mean = 0.0f;
        stats.variance = 0.0f;
        stats.min = 0.0f;
        stats.max = 0.0f;
        stats.median = 0.0f;
        return stats;
    }
    
    // Calculate mean
    stats.mean = std::accumulate(weights.begin(), weights.end(), 0.0f) / weights.size();
    
    // Calculate variance
    float sumSqDiff = 0.0f;
    for (float w : weights) {
        float diff = w - stats.mean;
        sumSqDiff += diff * diff;
    }
    stats.variance = sumSqDiff / weights.size();
    
    // Calculate min and max
    stats.min = *std::min_element(weights.begin(), weights.end());
    stats.max = *std::max_element(weights.begin(), weights.end());
    
    // Calculate median
    std::vector<float> sortedWeights(weights);
    std::sort(sortedWeights.begin(), sortedWeights.end());
    size_t n = sortedWeights.size();
    if (n % 2 == 0) {
        stats.median = (sortedWeights[n/2 - 1] + sortedWeights[n/2]) / 2.0f;
    } else {
        stats.median = sortedWeights[n/2];
    }
    
    return stats;
}

// Check if experiment results are statistically significant
bool isLearningSignificant(const std::vector<float>& initial, const std::vector<float>& final, float threshold) {
    if (initial.empty() || final.empty() || initial.size() != final.size()) {
        return false;
    }
    
    float initialMean = std::accumulate(initial.begin(), initial.end(), 0.0f) / initial.size();
    float finalMean = std::accumulate(final.begin(), final.end(), 0.0f) / final.size();
    
    float meanDiff = std::abs(finalMean - initialMean);
    
    // Check mean difference
    if (meanDiff > threshold) {
        return true;
    }
    
    // Check if any individual synapses changed significantly
    for (size_t i = 0; i < initial.size(); ++i) {
        if (std::abs(final[i] - initial[i]) > threshold * 10.0f) {
            return true;
        }
    }
    
    return false;
}

// Format experiment results for output
std::string formatExperimentResults(const LearningExperiment& experiment) {
    std::stringstream ss;
    
    ss << "=== Learning Experiment Results ===\n";
    ss << "Seed: " << experiment.seed << "\n";
    ss << "Initial synapses: " << experiment.initialSynapseCount << "\n";
    ss << "Final synapses: " << experiment.finalWeights.size() << "\n";
    ss << "Total spikes: " << experiment.brain->getTotalSpikeCount() << "\n";
    ss << "Most active neurons: " << experiment.mostActiveNeurons.size() << "\n";
    
    if (!experiment.initialWeights.empty() && !experiment.finalWeights.empty()) {
        float initialSum = std::accumulate(experiment.initialWeights.begin(), 
                                         experiment.initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(experiment.finalWeights.begin(), 
                                       experiment.finalWeights.end(), 0.0f);
        
        ss << "Initial mean weight: " << (initialSum / experiment.initialWeights.size()) << "\n";
        ss << "Final mean weight: " << (finalSum / experiment.finalWeights.size()) << "\n";
        ss << "Weight change: " << ((finalSum / experiment.finalWeights.size()) - 
                                  (initialSum / experiment.initialWeights.size())) << "\n";
    }
    
    return ss.str();
}

} // namespace experiment_utils

} // namespace nlm
