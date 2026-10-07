// TestParameters.hpp
// Centralized test configuration for NLM

#ifndef NLM_TESTPARAMETERS_HPP
#define NLM_TESTPARAMETERS_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Test parameters structure
struct TestParameters {
    // Basic test configuration
    std::string name;                            // Test name
    uint64_t neuronCount = 500;                  // Number of neurons
    uint64_t maxSteps = 1000;                    // Maximum simulation steps
    double timestep = 0.001;                     // Simulation timestep
    uint64_t randomSeed = 42;                    // Random seed
    
    // Plasticity parameters
    bool enableHebbian = true;                   // Enable Hebbian plasticity
    bool enableStdp = true;                      // Enable STDP plasticity
    bool enableStructuralPlasticity = true;       // Enable structural plasticity
    
    // Development parameters
    bool enableDevelopment = false;               // Enable development stages
    float synaptogenesisRate = 0.0001f;          // Synaptogenesis rate
    float pruningRate = 0.00001f;                // Pruning rate
    float maturationRate = 0.00005f;             // Maturation rate
    
    // STDP parameters
    float stdpLtpWeight = 0.02f;                 // STDP LTP weight
    float stdpLtdWeight = 0.015f;                // STDP LTD weight
    float stdpTau = 20.0f;                       // STDP time constant
    
    // Connectivity parameters
    float connectionProbability = 0.15f;         // Connection probability
    float initialWeightMean = 0.5f;              // Initial weight mean
    float initialWeightStd = 0.1f;               // Initial weight std deviation
    
    // Test-specific parameters
    uint64_t testNeuronCount = 20;               // Neurons to activate in tests
    float testInjectionCurrent = 50.0f;          // Current injection for tests
    uint64_t testSteps = 100;                    // Steps for simple tests
    uint64_t plasticityTestSteps = 1000;         // Steps for plasticity tests
    uint64_t stdpTestSteps = 200;                 // Steps for STDP tests
    uint64_t learningTestSteps = 500;            // Steps for learning tests
    
    // Thresholds and tolerances
    float weightChangeThreshold = 0.01f;         // Minimum weight change to consider learning
    float firingRateThreshold = 0.1f;            // Minimum firing rate threshold
    float convergenceThreshold = 0.001f;         // Convergence threshold for tests
    
    // Logging and output
    bool verboseLogging = true;                  // Enable verbose logging
    bool progressLogging = true;                 // Log progress updates
    bool benchmarkMode = false;                  // Enable benchmark mode
    
    // Timing and performance
    bool measureWallTime = true;                 // Measure wall clock time
    bool measureCpuTime = false;                 // Measure CPU time
    
    // Test validation
    bool validate() const;
    
    // Get test configuration summary
    std::string getConfigSummary() const;
    
    // Load parameters from configuration file
    bool loadFromFile(const std::string& filename);
    
    // Load parameters from command line
    void loadFromCommandLine(const CommandLineOptions& cliOptions);
    
    // Create test-specific configuration
    TestParameters forTest(const std::string& testName) const;
    
    // Create default configuration
    static TestParameters defaultConfig();
    
    // Create performance test configuration
    static TestParameters performanceConfig(uint64_t neurons, uint64_t steps);
    
    // Create quick test configuration
    static TestParameters quickConfig();
    
private:
    // Validate parameter ranges
    bool validateRanges() const;
    
    // Validate plasticity parameters
    bool validatePlasticityParams() const;
    
    // Validate development parameters
    bool validateDevelopmentParams() const;
    
    // Validate STDP parameters
    bool validateStdpParams() const;
    
    // Validate connectivity parameters
    bool validateConnectivityParams() const;
}; // namespace nlm

#endif // NLM_TESTPARAMETERS_HPP