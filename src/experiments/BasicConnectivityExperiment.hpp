#pragma once

/**
 * BasicConnectivityExperiment - Phase 2: Basic Neural Connectivity
 * 
 * This experiment demonstrates fundamental neural connectivity:
 * - LIF neuron dynamics
 * - Spike propagation through networks
 * - Initial synaptic functionality
 * 
 * Learning Experiment: Demonstrates measurable synaptic changes through experience
 */

#include "experiments/Experiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <iomanip>

namespace nlm {

/**
 * Basic connectivity experiment result
 */
struct BasicConnectivityResult {
    // Basic connectivity metrics
    bool spikesPropagate;
    size_t spikesGenerated;
    float meanMembranePotential;
    float meanWeightChange;
    
    // Learning metrics
    bool learningDetected;
    float initialMeanWeight;
    float finalMeanWeight;
    size_t strengthenedConnections;
    size_t weakenedConnections;
    size_t unchangedConnections;
    
    // Neuron statistics
    size_t totalSpikes;
    size_t firingNeuronCount;
    
    BasicConnectivityResult()
        : spikesPropagate(false)
        , spikesGenerated(0)
        , meanMembranePotential(0.0f)
        , meanWeightChange(0.0f)
        , learningDetected(false)
        , initialMeanWeight(0.0f)
        , finalMeanWeight(0.0f)
        , strengthenedConnections(0)
        , weakenedConnections(0)
        , unchangedConnections(0)
        , totalSpikes(0)
        , firingNeuronCount(0) {}
};

/**
 * Basic connectivity experiment configuration
 */
struct BasicConnectivityConfig {
    uint64_t steps;
    float currentInjection;
    bool enablePlasticity;
    bool enableStdp;
    uint64_t seed;
    
    BasicConnectivityConfig()
        : steps(1000)
        , currentInjection(50.0f)
        , enablePlasticity(true)
        , enableStdp(true)
        , seed(42) {}
};

/**
 * Basic connectivity experiment runner
 */
class BasicConnectivityExperiment {
public:
    BasicConnectivityExperiment();
    ~BasicConnectivityExperiment();
    
    /**
     * Run the basic connectivity experiment
     */
    BasicConnectivityResult run(const BasicConnectivityConfig& config, std::shared_ptr<Brain> brain = nullptr);
    
    /**
     * Run connectivity test only
     */
    bool runConnectivityTest(std::shared_ptr<Brain> brain);
    
    /**
     * Run plasticity learning experiment
     */
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain, uint64_t steps = 1000);
    
    /**
     * Run STDP verification experiment
     */
    bool runStdpVerification(std::shared_ptr<Brain> brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Learning experiment (legacy compatibility)
 */
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
};

} // namespace nlm
