// NLM Test Framework
// Provides modular test runners for different phases and features

#ifndef NLM_TEST_FRAMEWORK_HPP
#define NLM_TEST_FRAMEWORK_HPP

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double duration;
};

// Test runner base class
class TestBase {
public:
    virtual ~TestBase() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<TestResult> run() = 0;
};

// Brain setup configuration
struct BrainConfig {
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    double timestep = 0.001;
    int64_t randomSeed = 42;
    
    // Plasticity parameters
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Development parameters
    float developmentDuration = 60.0;  // seconds
    
    // Experiment parameters
    int maxSteps = 1000;
    bool enableCheckpointing = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
};

// Test framework utilities
class TestFramework {
public:
    static std::shared_ptr<Brain> createTestBrain(const BrainConfig& config);
    static void runPhase2Tests(std::shared_ptr<Brain> brain);
    static void runPlasticityTests(std::shared_ptr<Brain> brain);
    static void runSTDPTests(std::shared_ptr<Brain> brain);
    static void runIntegrationTests();
};

} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_HPP