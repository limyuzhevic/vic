#pragma once

#include <string>
#include <vector>
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    ~LearningExperiment() = default;
    
    // State recording
    void recordInitialState();
    void recordFinalState();
    
    // Statistics computation
    void computeStatistics();
    
    // Accessors
    const std::vector<float>& getInitialWeights() const { return initialWeights; }
    const std::vector<float>& getFinalWeights() const { return finalWeights; }
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons; }
    uint64_t getSeed() const { return seed; }
    size_t getInitialSynapseCount() const { return initialSynapseCount; }

private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
};

} // namespace nlm