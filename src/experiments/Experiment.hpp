#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include <string>

namespace nlm {
namespace experiments {

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        std::cout << "Initial state recorded:" << std::endl;
        std::cout << "  Synapses: " << initialSynapseCount << std::endl;
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            std::cout << "  Mean weight: " << std::fixed << std::setprecision(6) << mean << std::endl;
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        std::cout << "Final state recorded:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            std::cout << "  Mean weight: " << std::fixed << std::setprecision(6) << mean << std::endl;
        }
    }
    
    void computeStatistics() {
        std::cout << std::endl;
        std::cout << "=== Learning Experiment Results ===" << std::endl;
        std::cout << std::endl;
        
        if (initialWeights.empty() || finalWeights.empty()) {
            std::cout << "ERROR: No weights recorded" << std::endl;
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        std::cout << "Weight Statistics:" << std::endl;
        std::cout << "  Initial mean weight: " << std::fixed << std::setprecision(6) << initialMean << std::endl;
        std::cout << "  Final mean weight: " << std::fixed << std::setprecision(6) << finalMean << std::endl;
        std::cout << "  Change: " << std::fixed << std::setprecision(6) << (finalMean - initialMean) << std::endl;
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        std::cout << std::endl;
        std::cout << "Synaptic Changes:" << std::endl;
        std::cout << "  Strengthened: " << strengthened << std::endl;
        std::cout << "  Weakened: " << weakened << std::endl;
        std::cout << "  Unchanged: " << unchanged << std::endl;
        
        std::cout << std::endl;
        std::cout << "Spike Activity:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Most active neurons recorded: " << mostActiveNeurons.size() << std::endl;
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        std::cout << std::endl;
        if (learningOccurred) {
            std::cout << "✓ LEARNING DETECTED: Synaptic weights changed through experience" << std::endl;
        } else {
            std::cout << "✗ NO LEARNING: Weights did not change significantly" << std::endl;
        }
    }
};

} // namespace experiments
} // namespace nlm
