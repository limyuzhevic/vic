// NLM Test Suite Main
// Phase 2: Real Neural Computation Tests
// Integrates all test modules with the new test framework

#include "tests/framework/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

using namespace nlm;
using namespace nlm::tests;

// Test implementations are declared in their respective header files

// Test runner that manages the entire test suite
class NLMTestRunner {
public:
    struct SuiteResult {
        std::string name;
        bool passed = false;
        double duration_ms = 0.0;
        std::vector<TestFramework::TestResult> testResults;
        std::string summary;
    };
    
    NLMTestRunner() {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║              NLM Phase 2: Real Neural Computation              ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                     Test Suite Framework                      ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    SuiteResult runAllTests() {
        SuiteResult suiteResult;
        suiteResult.name = "NLM Phase 2 Test Suite";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "=== Test Configuration ===" << std::endl;
        
        // Create test configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration
        std::string configFile = "configs/default.cfg";
        // Note: In real implementation, parse command line args for --config
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration." << std::endl;
        }
        
        // Set test-specific defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters for plasticity tests
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
        std::cout << std::endl;
        
        // Initialize logger for tests
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation Tests ===");
        NLM_LOG_INFO("Testing real spiking neural computation:");
        NLM_LOG_INFO("  - LIF Neuron Dynamics");
        NLM_LOG_INFO("  - Synaptic Transmission");
        NLM_LOG_INFO("  - STDP Plasticity");
        NLM_LOG_INFO("  - Hebbian Plasticity");
        NLM_LOG_INFO("  - Structural Plasticity");
        NLM_LOG_INFO("");
        
        // Initialize framework
        TestFramework framework;
        
        // Create brain for testing
        std::cout << "=== Initializing Test Brain ===" << std::endl;
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            suiteResult.passed = false;
            return suiteResult;
        }
        
        brain->logStatus();
        std::cout << std::endl;
        
        // Create and run tests
        std::vector<TestFramework::Test*> tests;
        tests.push_back(new BasicConnectivityTest());
        tests.push_back(new PlasticityExperimentTest());
        tests.push_back(new StdpVerificationTest());
        
        std::cout << "=== Running Test Suite ===" << std::endl;
        std::cout << std::endl;
        
        bool allTestsPassed = true;
        
        for (size_t i = 0; i < tests.size(); ++i) {
            std::cout << "Test " << (i + 1) << "/" << tests.size() << ": " 
                     << tests[i]->getName() << std::endl;
            
            // Set config for the test
            tests[i]->setConfig(config);
            
            // Reset and initialize brain for each test
            brain->reset();
            brain->initialize();
            
            // Run the test
            TestFramework::TestResult result = framework.runTest(tests[i], brain, config);
            suiteResult.testResults.push_back(result);
            
            if (!result.passed) {
                allTestsPassed = false;
            }
            
            std::cout << std::endl;
        }
        
        // Cleanup
        for (auto test : tests) {
            delete test;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        suiteResult.duration_ms = static_cast<double>(duration.count());
        suiteResult.passed = allTestsPassed;
        
        // Generate summary
        suiteResult.summary = generateSummary(suiteResult);
        
        // Display final results
        std::cout << "=== Test Suite Summary ===" << std::endl;
        std::cout << "Total tests: " << tests.size() << std::endl;
        std::cout << "Passed: " << countPassed(suiteResult.testResults) << std::endl;
        std::cout << "Failed: " << countFailed(suiteResult.testResults) << std::endl;
        std::cout << "Duration: " << std::fixed << std::setprecision(2) 
                 << suiteResult.duration_ms << "ms" << std::endl;
        
        if (allTestsPassed) {
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        } else {
            std::cout << "=== SOME TESTS FAILED ===" << std::endl;
            std::cout << std::endl;
            for (const auto& result : suiteResult.testResults) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << ": " << result.message << std::endl;
                }
            }
        }
        
        return suiteResult;
    }
    
private:
    std::string generateSummary(const SuiteResult& result) {
        std::stringstream ss;
        ss << "NLM Phase 2 Test Suite Summary (Duration: " << std::fixed << std::setprecision(2) 
           << result.duration_ms << "ms):\n";
        ss << "Tests run: " << result.testResults.size() << "\n";
        ss << "Passed: " << countPassed(result.testResults) << "\n";
        ss << "Failed: " << countFailed(result.testResults) << "\n";
        
        if (!result.passed) {
            ss << "FAILED\n";
        } else {
            ss << "PASSED\n";
        }
        
        return ss.str();
    }
    
    size_t countPassed(const std::vector<TestFramework::TestResult>& results) const {
        size_t count = 0;
        for (const auto& result : results) {
            if (result.passed) count++;
        }
        return count;
    }
    
    size_t countFailed(const std::vector<TestFramework::TestResult>& results) const {
        return results.size() - countPassed(results);
    }
};

int main(int argc, char** argv) {
    // Parse command line arguments for test configuration
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Override config file if specified via args
    std::cout << "Using config file: " << configFile << std::endl;
    std::cout << std::endl;
    
    // Run test suite
    NLMTestRunner runner;
    NLMTestRunner::SuiteResult result = runner.runAllTests();
    
    // Return appropriate exit code
    return result.passed ? 0 : 1;
}
