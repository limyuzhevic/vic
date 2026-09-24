#include "RunCommand.hpp"

namespace nlm {

// Explicitly instantiate the LearningExperiment struct for backward compatibility
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
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    }
    
    void computeStatistics() {
        std::cout << "\n=== Learning Experiment Results ===\n\n";
        
        if (initialWeights.empty() || finalWeights.empty()) {
            std::cout << "ERROR: No weights recorded\n";
            return;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        std::cout << "Weight Statistics:\n";
        std::cout << "  Initial mean weight: " << initialMean << "\n";
        std::cout << "  Final mean weight: " << finalMean << "\n";
        std::cout << "  Change: " << finalMean - initialMean << "\n";
        
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
        
        std::cout << "\nSynaptic Changes:\n";
        std::cout << "  Strengthened: " << strengthened << "\n";
        std::cout << "  Weakened: " << weakened << "\n";
        std::cout << "  Unchanged: " << unchanged << "\n";
        
        std::cout << "\nSpike Activity:\n";
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "  Most active neurons recorded: " << mostActiveNeurons.size() << "\n";
        
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        std::cout << "\n";
        if (learningOccurred) {
            std::cout << "✓ LEARNING DETECTED: Synaptic weights changed through experience\n";
        } else {
            std::cout << "✗ NO LEARNING: Weights did not change significantly\n";
        }
    }
};

} // namespace nlm