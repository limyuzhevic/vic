// NLM Phase 2 Enhanced Demo Experiments
// 
// This module provides enhanced, well-structured experiment demonstrations
// that follow the established NLM experiment framework patterns.
// 
// Benefits over the original main.cpp approach:
// 1. Follows established experiment framework patterns
// 2. Better separation of concerns
// 3. Improved testability and modularity
// 4. Enhanced documentation and user experience
// 5. Integration with existing experiment runner
// 6. Support for parameter configuration and result collection

#pragma once

#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

namespace nlm {

/**
 * LearningExperiment - Enhanced version of the original LearningExperiment struct
 * 
 * This experiment demonstrates measurable synaptic changes through experience,
 * following the established Experiment framework patterns.
 * 
 * Improvements over the original:
 * - Uses proper Experiment class inheritance
 * - Better parameter configuration
 * - Enhanced result collection and metrics
 * - Improved logging and user feedback
 * - Better integration with brain systems
 */
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed, const std::string& name = "LearningExperiment");
    ~LearningExperiment();
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Configuration
    void setNeuronCount(size_t count);
    void setPlasticityEnabled(bool hebbian, bool stdp, bool structural);
    
    // Experiment execution
    bool runExperiment();
    
    // Data collection
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    
    // Results
    const std::vector<float>& getInitialWeights() const;
    const std::vector<float>& getFinalWeights() const;
    const std::vector<NeuronId>& getMostActiveNeurons() const;
    bool hasLearningOccurred() const;
    float getWeightChangeMean() const;
    float getTotalSynapticChange() const;
    
    // Status and reporting
    bool isCompleted() const;
    void printResults() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * BasicConnectivityExperiment - Enhanced neural connectivity verification
 * 
 * Demonstrates that neurons can connect and propagate signals through the network.
 * 
 * Improvements over original:
 * - Configurable neuron count and current strength
 * - Better error handling and validation
 * - Enhanced reporting with neuron-specific metrics
 * - Integration with brain's diagnostic systems
 */
class BasicConnectivityExperiment {
public:
    BasicConnectivityExperiment(std::shared_ptr<Brain> brain, const std::string& name = "BasicConnectivityExperiment");
    ~BasicConnectivityExperiment();
    
    // Configuration
    void setNeuronCount(size_t count);
    void setCurrentStrength(float strength);
    void setStepCount(size_t steps);
    
    // Experiment execution
    bool runExperiment();
    
    // Results
    size_t getSpikeCount() const;
    float getAvgMembranePotential() const;
    const std::vector<size_t>& getNeuronSpikeCounts() const;
    bool wasSuccessful() const;
    
    // Status and reporting
    void printResults() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * StdpVerificationExperiment - Enhanced STDP plasticity verification
 * 
 * Demonstrates spike-timing dependent plasticity (STDP) mechanisms.
 * 
 * Improvements over original:
 * - Better experimental control and statistical validation
 * - Configurable timing parameters
 * - Enhanced error reporting and validation
 * - Support for multiple synapse testing
 * - Integration with brain's plasticity monitoring
 */
class StdpVerificationExperiment {
public:
    StdpVerificationExperiment(std::shared_ptr<Brain> brain, const std::string& name = "StdpVerificationExperiment");
    ~StdpVerificationExperiment();
    
    // Configuration
    void setSynapseCount(size_t count);
    void setLtpThreshold(float threshold);
    void setLtdThreshold(float threshold);
    void setPrePostTiming(float preDelay, float postDelay);
    void setTrialCount(size_t trials);
    
    // Experiment execution
    bool runExperiment();
    
    // Results
    const std::vector<float>& getBeforeWeights() const;
    const std::vector<float>& getAfterWeights() const;
    float getTotalWeightChange() const;
    float getLtpFraction() const;
    float getLtdFraction() const;
    float getNoChangeFraction() const;
    bool wasSuccessful() const;
    
    // Status and reporting
    void printResults() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * NlmPhase2Demo - Main demonstration orchestrator
 * 
 * Enhanced main program that runs Phase 2 demonstrations using the new
 * experiment framework. Provides improved user experience, better
 * error handling, and enhanced reporting.
 * 
 * Key improvements:
 * - Uses new experiment classes instead of raw functions
 * - Better configuration management
 * - Enhanced logging and progress reporting
 * - Integration with experiment runner
 * - Improved error handling and recovery
 */
class NlmPhase2Demo {
public:
    NlmPhase2Demo();
    ~NlmPhase2Demo();
    
    // Configuration
    void setRandomSeed(uint64_t seed);
    void setNeuronCount(size_t count);
    void setConfigFile(const std::string& filename);
    
    // Demo execution
    bool runAllDemonstrations();
    
    // Status reporting
    void printDemoHeader() const;
    void printDemoFooter() const;
    std::string getDemoStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
