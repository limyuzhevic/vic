// Test classes for NLM Phase 2 functionality
// Extracted from main.cpp for better organization

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class ConnectivityTester {
public:
    ConnectivityTester(std::shared_ptr<Brain> brain);
    
    void runBasicConnectivityTest();
    
private:
    std::shared_ptr<Brain> brain_;
};

class PlasticityTester {
public:
    PlasticityTester(std::shared_ptr<Brain> brain);
    
    void runPlasticityExperiment();
    
private:
    std::shared_ptr<Brain> brain_;
};

class STDPTester {
public:
    STDPTester(std::shared_ptr<Brain> brain);
    
    void runStdpVerification();
    
private:
    std::shared_ptr<Brain> brain_;
};

class LearningExperiment {
public:
    struct Results {
        size_t initialSynapseCount = 0;
        float initialMeanWeight = 0.0f;
        float finalMeanWeight = 0.0f;
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        size_t totalSpikes = 0;
        size_t mostActiveNeurons = 0;
        bool learningOccurred = false;
    };
    
    LearningExperiment(std::shared_ptr<Brain> brain);
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    
    const Results& getResults() const { return results_; }
    
private:
    std::shared_ptr<Brain> brain_;
    Results results_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
};

} // namespace nlm