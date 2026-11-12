// Example experiment runner with better organization
// Refactored from monolithic main.cpp functionality

#pragma once

#include "examples/MainRunner.hpp"
#include <vector>
#include <string>
#include <iostream>

namespace nlm {
namespace examples {

// Result structure for experiment runs
struct ExperimentResult {
    std::string experimentName;
    size_t timesteps;
    size_t totalSpikes;
    size_t neuronCount;
    size_t synapseCount;
    double meanInitialWeight;
    double meanFinalWeight;
    float weightChange;
    bool learningOccurred;
    std::vector<NeuronId> mostActiveNeurons;
    std::string timestamp;
    
    // Calculate learning based on weight changes
    void calculateLearning() {
        if (meanInitialWeight > 0) {
            weightChange = meanFinalWeight - meanInitialWeight;
            learningOccurred = (std::abs(weightChange) > 0.001f);
        } else {
            weightChange = 0.0f;
            learningOccurred = false;
        }
    }
};

class ExperimentRunner {
public:
    // Run Phase 2 demonstration
    static ExperimentResult runPhase2Demo(std::shared_ptr<Brain> brain, size_t timesteps = 1000);
    
    // Run Phase 3 demonstration
    static ExperimentResult runPhase3Demo(std::shared_ptr<Brain> brain, size_t timesteps = 1000);
    
    // Run Phase 4 demonstration
    static ExperimentResult runPhase4Demo(std::shared_ptr<Brain> brain, size_t timesteps = 1000);
    
    // Run Phase 5 demonstration
    static ExperimentResult runPhase5Demo(std::shared_ptr<Brain> brain, size_t timesteps = 1000);
    
    // Run Phase 6 demonstration
    static ExperimentResult runPhase6Demo(std::shared_ptr<Brain> brain, size_t timesteps = 1000);
    
    // Run learning experiment
    static ExperimentResult runLearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed, size_t timesteps = 5000);
    
    // Run connectivity test
    static ExperimentResult runConnectivityTest(std::shared_ptr<Brain> brain, size_t timesteps = 1000);
    
    // Run STDP verification
    static ExperimentResult runStdpVerification(std::shared_ptr<Brain> brain, size_t trials = 50);
    
    // Run plasticity study
    static ExperimentResult runPlasticityStudy(std::shared_ptr<Brain> brain, size_t timesteps = 2000);
    
    // Print experiment results
    static void printExperimentResults(const ExperimentResult& result);
    
    // Export experiment results to JSON
    static std::string exportResultsToJson(const ExperimentResult& result);
    
private:
    // Helper function to get initial weights
    static std::vector<float> getInitialWeights(std::shared_ptr<Brain> brain);
    
    // Helper function to get final weights
    static std::vector<float> getFinalWeights(std::shared_ptr<Brain> brain);
    
    // Helper function to calculate weight statistics
    static std::pair<float, float> calculateWeightStats(const std::vector<float>& weights);
};

}} // namespace nlm::examples