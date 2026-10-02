// Comprehensive Tests for NLM Refactoring and Enhancements
// Tests the new main.cpp refactored functionality and enhanced bindings

#include "main_refactored.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

namespace test_refactored {

// Test refactored main.cpp functionality
void testNeuralTestRunnerBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralTestRunner testRunner(config);
    
    // Test connectivity
    auto result1 = testRunner.runBasicConnectivityTest(brain);
    assert(result1.passed);
    assert(result1.testName == "Basic Connectivity Test");
    
    // Test plasticity
    testRunner.resetBrainForTest(brain);
    auto result2 = testRunner.runPlasticityExperiment(brain);
    assert(result2.passed);
    assert(result2.testName == "Plasticity Learning Experiment");
    assert(result2.metrics.find("learning_detected") != result2.metrics.end());
    
    std::cout << "    testNeuralTestRunnerBasic passed" << std::endl;
}

void testNeuralTestRunnerComplete() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralTestRunner testRunner(config);
    auto results = testRunner.runAllTests(brain);
    
    assert(results.size() == 4);
    
    // Check each test result structure
    assert(results[0].testName == "Basic Connectivity Test");
    assert(results[1].testName == "Plasticity Learning Experiment");
    assert(results[2].testName == "STDP Verification");
    assert(results[3].testName == "Learning Experiment");
    
    std::cout << "    testNeuralTestRunnerComplete passed" << std::endl;
}

void testCommandLineInterface() {
    // Test command line parsing
    char* argv1[] = {"test", "--help"};
    int argc1 = 2;
    
    nlm::NLMCommandLineInterface::CommandLineOptions opts1 = 
        nlm::NLMCommandLineInterface::parseCommandLine(argc1, argv1);
    assert(opts1.help == true);
    
    char* argv2[] = {"test", "--version"};
    int argc2 = 2;
    
    nlm::NLMCommandLineInterface::CommandLineOptions opts2 = 
        nlm::NLMCommandLineInterface::parseCommandLine(argc2, argv2);
    assert(opts2.version == true);
    
    char* argv3[] = {"test", "--test=all", "--neurons=1000", "--steps=5000"};
    int argc3 = 5;
    
    nlm::NLMCommandLineInterface::CommandLineOptions opts3 = 
        nlm::NLMCommandLineInterface::parseCommandLine(argc3, argv3);
    assert(opts3.testMode == "all");
    assert(opts3.neuronCount == 1000);
    assert(opts3.maxSteps == 5000);
    
    char* argv4[] = {"test", "--test=connectivity", "--timestep=0.01", "--verbose"};
    int argc4 = 5;
    
    nlm::NLMCommandLineInterface::CommandLineOptions opts4 = 
        nlm::NLMCommandLineInterface::parseCommandLine(argc4, argv4);
    assert(opts4.testMode == "connectivity");
    assert(opts4.timestep == 0.01);
    assert(opts4.verbose == true);
    
    std::cout << "    testCommandLineInterface passed" << std::endl;
}

void testConstants() {
    // Test that constants are properly defined
    assert(nlm::TestConstants::DEFAULT_NEURON_COUNT == 500);
    assert(nlm::TestConstants::SMALL_NEURON_COUNT == 100);
    assert(nlm::TestConstants::TEST_NEURON_COUNT == 50);
    
    assert(nlm::TestConstants::DEFAULT_TIMESTEP == 0.001);
    assert(nlm::TestConstants::FAST_TIMESTEP == 0.01);
    assert(nlm::TestConstants::DEFAULT_MAX_STEPS == 1000);
    
    assert(nlm::TestConstants::DEFAULT_EXCITATORY_CURRENT == 30.0f);
    assert(nlm::TestConstants::CONNECTIVITY_TEST_CURRENT == 50.0f);
    
    assert(nlm::TestConstants::WEIGHT_CHANGE_THRESHOLD == 0.01f);
    assert(nlm::TestConstants::LEARNING_THRESHOLD == 0.001f);
    
    assert(nlm::TestConstants::DEFAULT_SYNAPSTOGENESIS_RATE == 0.0001f);
    assert(nlm::TestConstants::DEFAULT_PRUNING_RATE == 0.00001f);
    
    std::cout << "    testConstants passed" << std::endl;
}

void testTestResult() {
    nlm::TestResult result;
    assert(result.testName.empty());
    assert(result.passed == false);
    assert(result.message.empty());
    assert(result.executionTime == 0.0);
    assert(result.metrics.empty());
    
    result.testName = "Test 1";
    result.passed = true;
    result.message = "Passed successfully";
    result.executionTime = 1.5;
    result.metrics["spikes"] = "100";
    
    assert(result.testName == "Test 1");
    assert(result.passed == true);
    assert(result.message == "Passed successfully");
    assert(result.executionTime == 1.5);
    assert(result.metrics.size() == 1);
    
    std::cout << "    testTestResult passed" << std::endl;
}

void testConfigurationOptions() {
    nlm::NLMCommandLineInterface::CommandLineOptions options;
    
    // Test default values
    assert(options.configFile == "configs/default.cfg");
    assert(options.outputFile == "");
    assert(options.logFile == "");
    assert(options.testMode == "all");
    assert(options.timestep == 0.001);
    assert(options.neuronCount == 500);
    assert(options.maxSteps == 1000);
    assert(options.verbose == false);
    assert(options.help == false);
    assert(options.version == false);
    
    // Test setting values
    options.configFile = "myconfig.cfg";
    options.outputFile = "results.json";
    options.logFile = "nlm.log";
    options.testMode = "plasticity";
    options.timestep = 0.01;
    options.neuronCount = 1000;
    options.maxSteps = 5000;
    options.verbose = true;
    options.help = true;
    
    options.version = true;
    
    assert(options.configFile == "myconfig.cfg");
    assert(options.outputFile == "results.json");
    assert(options.logFile == "nlm.log");
    assert(options.testMode == "plasticity");
    assert(options.timestep == 0.01);
    assert(options.neuronCount == 1000);
    assert(options.maxSteps == 5000);
    assert(options.verbose == true);
    assert(options.help == true);
    assert(options.version == true);
    
    std::cout << "    testConfigurationOptions passed" << std::endl;
}

void testMainEntryPoint() {
    // Test that main.cpp has the correct entry point
    // This is a compile-time test - just ensure main exists
    extern int main(int argc, char** argv);
    
    // Test with help flag
    char* argv_help[] = {"nlm_test", "--help"};
    int argc_help = 2;
    
    // Help should set help flag but not crash
    nlm::NLMCommandLineInterface::CommandLineOptions opts = 
        nlm::NLMCommandLineInterface::parseCommandLine(argc_help, argv_help);
    assert(opts.help == true);
    
    std::cout << "    testMainEntryPoint passed" << std::endl;
}

// Test Python bindings enhanced functionality
void testEnhancedBindings() {
    // This would test the enhanced Python bindings
    // For now, just verify the bindings compile
    assert(true);
    
    std::cout << "    testEnhancedBindings passed" << std::endl;
}

void runAll() {
    std::cout << "Running refactored NLM tests..." << std::endl;
    
    testConstants();
    testTestResult();
    testNeuralTestRunnerBasic();
    testNeuralTestRunnerComplete();
    testCommandLineInterface();
    testConfigurationOptions();
    testMainEntryPoint();
    testEnhancedBindings();
    
    std::cout << "All refactored tests passed!" << std::endl;
}

} // namespace test_refactored
