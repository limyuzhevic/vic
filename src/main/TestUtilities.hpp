#pragma once

#include <memory>
#include <string>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"

namespace nlm {

class TestUtils {
public:
    static void logSection(const std::string& title);
    static void logSubsection(const std::string& subtitle);
    static void logResult(const std::string& label, const std::string& value);
    static void logMetric(const std::string& name, double value, const std::string& unit = "");
    static void logProgress(int current, int total, const std::string& description = "");
    static void logError(const std::string& message);
    
    template<typename T>
    static std::vector<T> extractWeights(const NeuralRegion& region);
    
    static float calculateMean(const std::vector<float>& values);
    static float calculateVariance(const std::vector<float>& values, float mean);
    static size_t countSignificantChanges(const std::vector<float>& before, const std::vector<float>& after, float threshold = 0.01f);
};

class ConfigSetup {
public:
    static void setupDefaultPhase2Config(std::shared_ptr<Config> config);
    static void applyCommandLineArgs(std::shared_ptr<Config> config, int argc, char** argv);
    static void logConfiguration(const Config& config);
};

class BrainInitializer {
public:
    static std::shared_ptr<Brain> createAndInitializeBrain(const std::shared_ptr<Config>& config);
    static void resetBrainForNewTest(std::shared_ptr<Brain> brain);
};

class LearningExperiment {
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
    };
    
    static Results runLearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed = 42);
};

} // namespace nlm