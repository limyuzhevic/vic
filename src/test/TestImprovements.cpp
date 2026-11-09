// Compile and execution test for improved NLM neural network simulation
// This test verifies that the improvements are working correctly

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "test/TestRunner.hpp"

#include <iostream>
#include <memory>

int main_test() {
    // Initialize logger for testing
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Testing improved NLM neural network simulation...\n" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Configure test runner with minimal settings for quick test
    TestConfig testConfig;
    testConfig.verboseLogging = false;
    testConfig.profilePerformance = false;
    testConfig.enableResultValidation = true;
    testConfig.connectivityTestCurrent = 10.0f;
    testConfig.plasticityTestCurrent = 15.0f;
    testConfig.plasticityTestSteps = 100;
    testConfig.stdpTestCurrent = 20.0f;
    testConfig.stdpTestTrials = 10;
    
    TestRunner testRunner(testConfig);
    
    // Run connectivity test
    std::cout << "Running connectivity test..." << std::endl;
    TestStats connectivityResult = testRunner.runTest(brain, "connectivity");
    
    if (!connectivityResult.passed) {
        std::cout << "FAIL: Connectivity test failed: " << connectivityResult.details << std::endl;
        return 1;
    }
    
    std::cout << "PASS: Connectivity test completed" << std::endl;
    
    // Reset brain for plasticity test
    brain->reset();
    brain->initialize();
    
    // Run plasticity test
    std::cout << "Running plasticity test..." << std::endl;
    TestStats plasticityResult = testRunner.runTest(brain, "plasticity");
    
    if (!plasticityResult.passed) {
        std::cout << "FAIL: Plasticity test failed: " << plasticityResult.details << std::endl;
        return 1;
    }
    
    std::cout << "PASS: Plasticity test completed" << std::endl;
    
    // Reset brain for STDP test
    brain->reset();
    brain->initialize();
    
    // Run STDP test
    std::cout << "Running STDP test..." << std::endl;
    TestStats stdpResult = testRunner.runTest(brain, "stdp");
    
    if (!stdpResult.passed) {
        std::cout << "FAIL: STDP test failed: " << stdpResult.details << std::endl;
        return 1;
    }
    
    std::cout << "PASS: STDP test completed" << std::endl;
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=== Test Suite Summary ===" << std::endl;
    std::cout << "Connectivity: " << (connectivityResult.passed ? "PASS" : "FAIL") << std::endl;
    std::cout << "Plasticity: " << (plasticityResult.passed ? "PASS" : "FAIL") << std::endl;
    std::cout << "STDP: " << (stdpResult.passed ? "PASS" : "FAIL") << std::endl;
    std::cout << std::endl;
    
    if (connectivityResult.passed && plasticityResult.passed && stdpResult.passed) {
        std::cout << "All tests PASSED! The improvements are working correctly." << std::endl;
        return 0;
    } else {
        std::cout << "Some tests FAILED. Please review the implementation." << std::endl;
        return 1;
    }
}

int main_benchmark() {
    // Quick benchmark of the improvements
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running benchmark of improved NLM...\n" << std::endl;
    
    // Create config
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run multiple steps
    for (int step = 0; step < 500; ++step) {
        brain->step(step, step * 0.001);
    }
    
    std::cout << "Benchmark completed successfully." << std::endl;
    std::cout << "Final spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}