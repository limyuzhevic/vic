// Test configuration for NLM experiments
// Centralized configuration for all test parameters

#pragma once

#include <string>
#include <map>
#include <vector>
#include "core/Constants.hpp"

namespace nlm {

class TestConfig {
public:
    struct TestParameters {
        // Basic simulation parameters
        uint64_t randomSeed = constants::DEFAULT_RANDOM_SEED;
        double timestep = constants::DEFAULT_TIMESTEP;
        int64_t neuronCount = constants::DEFAULT_NEURON_COUNT;
        int64_t regionCount = constants::DEFAULT_REGION_COUNT;
        float connectionProbability = constants::DEFAULT_CONNECTION_PROBABILITY;
        
        // STDP parameters
        float stdpLtpWeight = constants::STP_LTP_WEIGHT_BASE;
        float stdpLtdWeight = constants::STP_LTD_WEIGHT_BASE;
        float stdpTau = constants::STP_TAU_BASE;
        
        // Structural plasticity parameters
        float synaptogenesisRate = constants::INITIAL_SYNAPTOGENESIS_RATE;
        float pruningRate = constants::INITIAL_PRUNING_RATE;
        
        // Learning experiment parameters
        size_t neuronsToInject = 10;  // For connectivity test
        size_t stepsForPlasticity = 1000;  // For plasticity experiment
        size_t stepsForStdp = 50;  // For STDP verification
        size_t sensoryNeuronsToInject = 20;  // Percentage of total for input patterns
        
        // Activity thresholds
        float activityThreshold = constants::ACTIVITY_THRESHOLD;
        float weightChangeThreshold = constants::WEIGHT_CHANGE_THRESHOLD;
        
        // Logging and output
        bool verboseLogging = true;
        bool enableDebugOutput = false;
        
        // Configuration file
        std::string configFile = "configs/default.cfg";
        
        // Command line arguments
        std::vector<std::string> extraArgs;
    };
    
    TestConfig();
    explicit TestConfig(const std::string& configFile);
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Load configuration from command line arguments
    void loadFromArgs(int argc, char** argv);
    
    // Get parameters
    const TestParameters& getParameters() const { return params_; }
    TestParameters& getParameters() { return params_; }
    
    // Get specific parameter
    template<typename T>
    T get(const std::string& key, T defaultValue) const {
        auto it = customParams_.find(key);
        if (it != customParams_.end()) {
            // This is a simplified implementation - in practice you'd need
            // type conversion logic
            return defaultValue;
        }
        return defaultValue;
    }
    
    // Set custom parameter
    template<typename T>
    void set(const std::string& key, T value) {
        customParams_[key] = std::to_string(value);
    }
    
    // Print configuration summary
    void printConfigSummary() const;
    
private:
    TestParameters params_;
    std::map<std::string, std::string> customParams_;
    
    // Parse command line arguments
    void parseArgs(const std::vector<std::string>& args);
    
    // Parse numeric value from string
    template<typename T>
    T parseNumber(const std::string& str) const;
};

// Inline implementations
inline TestConfig::TestConfig() = default;

inline TestConfig::TestConfig(const std::string& configFile) {
    params_.configFile = configFile;
    if (!loadFromFile(configFile)) {
        // Use defaults if file not found
    }
}

// Helper function to create default test configuration
test_config inline TestConfig getDefaultTestConfig() {
    return TestConfig();
}

} // namespace nlm