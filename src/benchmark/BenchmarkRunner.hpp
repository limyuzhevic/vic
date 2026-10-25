// Benchmark classes for NLM testing
// Phase 2: Real Neural Computation
//
// This file contains declarations for benchmark and test classes extracted from main.cpp
// to improve code organization and reusability

#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class BenchmarkRunner;
class LearningExperiment;
class Phase2Tests;

// Benchmark test results structure
struct BenchmarkResults {
    std::string testName;
    double executionTime;
    bool success;
    std::string details;
    size_t neuronsTested;
    size_t synapsesTested;
    double spikeRate;
    
    BenchmarkResults();
};

// Connectivity benchmark - tests basic neural communication
class ConnectivityBenchmark {
public:
    ConnectivityBenchmark(std::shared_ptr<Brain> brain);
    BenchmarkResults run();
    
private:
    std::shared_ptr<Brain> brain_;
};

// Plasticity benchmark - tests learning and synaptic changes
class PlasticityBenchmark {
public:
    PlasticityBenchmark(std::shared_ptr<Brain> brain);
    BenchmarkResults run();
    
private:
    std::shared_ptr<Brain> brain_;
};

// STDP benchmark - tests spike-timing dependent plasticity
class StdpBenchmark {
public:
    StdpBenchmark(std::shared_ptr<Brain> brain);
    BenchmarkResults run();
    
private:
    std::shared_ptr<Brain> brain_;
};

// Comprehensive benchmark runner that manages all tests
class BenchmarkRunner {
public:
    BenchmarkRunner(std::shared_ptr<Brain> brain);
    
    // Run individual benchmarks
    BenchmarkResults runConnectivityBenchmark();
    BenchmarkResults runPlasticityBenchmark();
    BenchmarkResults runStdpBenchmark();
    
    // Run all benchmarks and return aggregated results
    std::vector<BenchmarkResults> runAllBenchmarks();
    
    // Print benchmark results to log
    void printResults(const std::vector<BenchmarkResults>& results);
    
private:
    std::shared_ptr<Brain> brain_;
};

// Learning experiment results
struct LearningExperimentResults {
    std::string experimentName;
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    size_t strengthenedSynapses;
    size_t weakenedSynapses;
    size_t unchangedSynapses;
    size_t totalSpikes;
    size_t mostActiveNeuronsRecorded;
    bool learningOccurred;
    std::string status;
    
    LearningExperimentResults();
};

// Learning experiment - measures synaptic changes through experience
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed = 42);
    LearningExperimentResults run();
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    
    // Learning experiment data
    size_t initialSynapseCount_ = 0;
    std::vector<float> initialWeights_;
    float initialMeanWeight_ = 0.0f;
    
    std::vector<float> finalWeights_;
    float finalMeanWeight_ = 0.0f;
    std::vector<NeuronId> mostActiveNeurons_;
    
    // Helper methods
    void recordInitialState();
    void recordFinalState();
    void computeStatistics(LearningExperimentResults& results);
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    void runLearningExperience();
};

// Phase 2 comprehensive test suite
class Phase2Tests {
public:
    Phase2Tests(std::shared_ptr<Brain> brain);
    void runAllTests();
    
private:
    std::shared_ptr<Brain> brain_;
    
    void runBasicConnectivityTest();
    void runPlasticityExperiment();
    void runStdpVerification();
};

} // namespace nlm