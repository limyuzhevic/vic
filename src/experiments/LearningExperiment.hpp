// Learning Experiment Test Framework
// Extracted from main.cpp for better modularity and testing

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <numeric>
#include "brain/Brain.hpp"

namespace nlm {

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
        
        // Using Logger helper instead of NLM_LOG_INFO macro
        if (auto logger = Logger::getGlobal()) {
            logger->info("Initial state recorded:");
            logger->info("  Synapses: " + std::to_string(initialSynapseCount));
            if (!initialWeights.empty()) {
                float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float mean = sum / initialWeights.size();
                logger->info("  Mean weight: " + std::to_string(mean));
            }
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
        
        if (auto logger = Logger::getGlobal()) {
            logger->info("Final state recorded:");
            logger->info("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            if (!finalWeights.empty()) {
                float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float mean = sum / finalWeights.size();
                logger->info("  Mean weight: " + std::to_string(mean));
            }
        }
    }
    
    void computeStatistics() {
        if (auto logger = Logger::getGlobal()) {
            logger->info("");
            logger->info("=== Learning Experiment Results ===");
            logger->info("");
            
            if (initialWeights.empty() || finalWeights.empty()) {
                logger->info("ERROR: No weights recorded");
                return;
            }
            
            // Compute weight changes
            float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float initialMean = initialSum / initialWeights.size();
            float finalMean = finalSum / finalWeights.size();
            
            logger->info("Weight Statistics:");
            logger->info("  Initial mean weight: " + std::to_string(initialMean));
            logger->info("  Final mean weight: " + std::to_string(finalMean));
            logger->info("  Change: " + std::to_string(finalMean - initialMean));
            
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
            
            logger->info("");
            logger->info("Synaptic Changes:");
            logger->info("  Strengthened: " + std::to_string(strengthened));
            logger->info("  Weakened: " + std::to_string(weakened));
            logger->info("  Unchanged: " + std::to_string(unchanged));
            
            logger->info("");
            logger->info("Spike Activity:");
            logger->info("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            logger->info("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
            
            // Determine if learning occurred
            bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                  (strengthened > 0 || weakened > 0);
            
            logger->info("");
            if (learningOccurred) {
                logger->info("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            } else {
                logger->info("✗ NO LEARNING: Weights did not change significantly");
            }
        }
    }
};