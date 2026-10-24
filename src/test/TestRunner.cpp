#include "TestRunner.hpp"
#include "TestFramework.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm::test {

// Test runner implementation

// Global test runner instance
static std::unique_ptr<TestRunner> globalTestRunner;

// Initialize test framework
void initializeTestFramework() {
    globalTestRunner = std::make_unique<TestRunner>();
    
    // Add all available tests
    globalTestRunner->addTest(std::make_unique<BasicConnectivityTest>());
    globalTestRunner->addTest(std::make_unique<PlasticityLearningTest>());
    globalTestRunner->addTest(std::make_unique<StdpVerificationTest>());
    globalTestRunner->addTest(std::make_unique<CuriositySystemTest>());
}

// Run all tests with configuration
std::vector<TestResult> runAllTests(const TestConfig& config) {
    if (!globalTestRunner) {
        initializeTestFramework();
    }
    
    return globalTestRunner->runAllTests(config);
}

// Get default test configuration for Phase 6
TestConfig getPhase6TestConfig() {
    TestConfig config;
    config.name = "NLM Phase 6 Complete Test Suite";
    config.description = "Comprehensive validation of all integrated systems for Phase 6";
    config.seed = 42;
    config.neuronCount = 1000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.timestep = 0.001;
    
    // Additional configuration for advanced features
    config.additionalConfig["stdp_ltp_weight"] = "0.02";
    config.additionalConfig["stdp_ltd_weight"] = "0.015";
    config.additionalConfig["stdp_tau"] = "20.0";
    config.additionalConfig["synaptogenesis_rate"] = "0.0001";
    config.additionalConfig["pruning_rate"] = "0.00001";
    
    return config;
}

// Get the global test runner instance
TestRunner& getTestRunner() {
    if (!globalTestRunner) {
        initializeTestFramework();
    }
    return *globalTestRunner;
}

} // namespace nlm::test
