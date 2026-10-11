// NLM Learning Experiment Test
// Phase 2: Real Neural Computation Tests

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <numeric>

// Forward declarations
namespace nlm {
    class Brain;
    struct RegionId;
    struct NeuronId;
    class Neuron;
    class Synapse;
    namespace logger {
        class Logger;
        class ConsoleLogger;
        class LogLevel;
    }
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<nlm::Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<nlm::NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<nlm::Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "Initial state recorded:");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "Final state recorded:");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "=== Learning Experiment Results ===");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "Weight Statistics:");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Initial mean weight: " + std::to_string(initialMean));
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Final mean weight: " + std::to_string(finalMean));
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Change: " + std::to_string(finalMean - initialMean));
        
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
        
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "Synaptic Changes:");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Strengthened: " + std::to_string(strengthened));
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Weakened: " + std::to_string(weakened));
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Unchanged: " + std::to_string(unchanged));
        
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "Spike Activity:");
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "");
        if (learningOccurred) {
            nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            nlm::logger::Logger::getGlobal()->log(nlm::logger::LogLevel::Info, "✗ NO LEARNING: Weights did not change significantly");
        }
    }
};