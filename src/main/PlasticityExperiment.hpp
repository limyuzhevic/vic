#pragma once

#include <memory>
#include "brain/Brain.hpp"
#include "main/TestUtilities.hpp"

namespace nlm {

namespace plasticity_experiment {

class PlasticityExperiment {
public:
    struct Results {
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<NeuronId> mostActiveNeurons;
        float initialMean = 0.0f;
        float finalMean = 0.0f;
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        size_t totalSpikes = 0;
        bool learningOccurred = false;
        float weightChange = 0.0f;
    };
    
    static Results run(std::shared_ptr<Brain> brain, uint64_t seed = 42);
    static void logResults(const Results& results);
    
private:
    static std::vector<NeuronId> selectPatternNeurons(std::shared_ptr<Brain> brain, size_t count);
    static void applyRepeatedInputPattern(std::shared_ptr<Brain> brain, const std::vector<NeuronId>& patternNeurons, size_t steps);
};

} // namespace plasticity_experiment

} // namespace nlm