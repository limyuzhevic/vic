#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {
namespace framework {

// Test mode enumeration
enum class TestMode {
    Connectivity,
    Plasticity,
    STDP,
    All
};

// Test status enumeration
enum class TestStatus {
    NotStarted,
    Running,
    Completed,
    Failed,
    Skipped
};

// Test intensity levels
enum class TestIntensity {
    Light,
    Medium,
    Heavy
};

// Test result structure
struct TestResult {
    TestMode mode;
    TestStatus status;
    std::chrono::duration<double> duration;
    std::string description;
    std::string message;
    bool success;
    
    TestResult() : mode(TestMode::All), status(TestStatus::NotStarted), 
                  duration(0), success(false) {}
};

// Test framework class
class TestFramework {
public:
    TestFramework();
    ~TestFramework();
    
    // Configuration loading
    bool loadConfiguration(const std::string& configFile = "configs/default.cfg");
    bool loadFromCommandLine(int argc, char** argv);
    
    // Brain setup
    bool setupBrain();
    bool resetBrain();
    
    // Test execution
    bool runTest(TestMode mode);
    bool runAllTests();
    
    // Individual test methods
    bool runConnectivityTest();
    bool runPlasticityTest();
    bool runStdpTest();
    
    // Test configuration
    void setTestMode(TestMode mode);
    void setTestIntensity(TestIntensity intensity);
    void setTestDuration(int duration); // in steps
    void setRandomSeed(uint64_t seed);
    
    // Result collection and reporting
    std::vector<TestResult> getResults() const;
    void reportResults() const;
    
    // Status and logging
    void logStatus() const;
    bool isValid() const;
    
    // Access to core components (for backward compatibility)
    std::shared_ptr<const Config> getConfig() const;
    std::shared_ptr<Brain> getBrain();
    
private:
    // Core components
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    
    // Test state
    TestMode testMode_;
    TestIntensity testIntensity_;
    int testDuration_;
    uint64_t randomSeed_;
    std::vector<TestResult> results_;
    
    // Configuration constants
    const double defaultTimestep_ = 0.001;
    const int defaultNeuronCount_ = 500;
    const int defaultRegionCount_ = 1;
    const float defaultConnectionProbability_ = 0.15f;
    
    // Configuration keys
    const std::string keyRandomSeed_ = "random_seed";
    const std::string keyTimestep_ = "simulation_timestep";
    const std::string keyNeuronCount_ = "neuron_count";
    const std::string keyRegionCount_ = "region_count";
    const std::string keyConnectionProbability_ = "connection_probability";
    
    // Test constants
    const int connectivityTestSteps_ = 50;
    const int plasticityTestSteps_ = 1000;
    const int stdpTestTrials_ = 50;
    
    // Helper methods
    void setupLogger();
    void setupDefaultConfiguration();
    void printBanner() const;
    bool parseCommandLineArgs(int argc, char** argv);
    std::string testModeToString(TestMode mode) const;
    TestStatus getTestStatusFromBrain(const std::string& testName) const;
};

} // namespace framework
} // namespace nlm
