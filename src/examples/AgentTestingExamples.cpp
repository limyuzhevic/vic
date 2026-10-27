// NLM Agent Testing Framework Example
// This file demonstrates how to use the enhanced Agent testing framework
// Includes comprehensive examples of different testing scenarios

#include "TestingFramework.hpp"
#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

// Example 1: Basic Test Suite Usage
void example1_basic_usage() {
    std::cout << "=== Example 1: Basic Test Suite Usage ===" << std::endl;
    
    // Initialize NLM logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create simple world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->reset();
    agent->initialize(*world);
    
    // Configure test suite
    nlm::testing::TestConfig testConfig;
    testConfig.verbose = true;
    testConfig.outputFile = "basic_test_results.json";
    testConfig.continueOnFailure = false;
    
    // Run tests
    nlm::testing::runAgentTests(brain, agent, testConfig);
    
    std::cout << std::endl;
}

// Example 2: Custom Test Configuration
void example2_custom_configuration() {
    std::cout << "=== Example 2: Custom Test Configuration ===" << std::endl;
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Setup world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=30, height=30, visionWidth=16, visionHeight=16);
    world->reset();
    agent->initialize(*world);
    
    // Create custom test configuration
    nlm::testing::TestConfig testConfig;
    testConfig.verbose = true;
    testConfig.outputFile = "custom_test_results.json";
    testConfig.continueOnFailure = true;
    
    // Configure agent parameters
    testConfig.agentParams.numSteps = 500;
    testConfig.agentParams.plasticityStrength = 2.0f;
    testConfig.agentParams.noveltyThreshold = 0.05f;
    testConfig.agentParams.enableCuriosity = true;
    testConfig.agentParams.enableDevelopment = true;
    testConfig.agentParams.neuronCount = 1000;
    testConfig.agentParams.connectionProbability = 0.12f;
    
    // Add specific tests to run
    testConfig.enabledTests = {
        "SensoryProcessingTest",
        "MotorDecodingTest",
        "NeuromodulationTest"
    };
    
    // Run tests
    nlm::testing::runAgentTests(brain, agent, testConfig);
    
    std::cout << std::endl;
}

// Example 3: Individual Test Execution
void example3_individual_tests() {
    std::cout << "=== Example 3: Individual Test Execution ===" << std::endl;
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Setup world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=15, height=15, visionWidth=8, visionHeight=8);
    world->reset();
    agent->initialize(*world);
    
    // Create test suite
    nlm::testing::TestConfig testConfig;
    testConfig.verbose = true;
    
    nlm::testing::AgentTestSuite testSuite(testConfig);
    
    // Add specific tests individually
    testSuite.addTest(std::make_shared<nlm::testing::SensoryProcessingTest>());
    testSuite.addTest(std::make_shared<nlm::testing::MotorDecodingTest>());
    testSuite.addTest(std::make_shared<nlm::testing::DevelopmentProgressionTest>());
    
    std::cout << "Running selected tests individually..." << std::endl;
    testSuite.runAll();
    
    std::cout << std::endl;
}

// Example 4: Performance Benchmarking
void example4_performance_benchmarking() {
    std::cout << "=== Example 4: Performance Benchmarking ===" << std::endl;
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Setup world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=25, height=25, visionWidth=12, visionHeight=12);
    world->reset();
    agent->initialize(*world);
    
    // Configure for benchmarking
    nlm::testing::TestConfig testConfig;
    testConfig.verbose = true;
    testConfig.outputFile = "benchmark_results.json";
    testConfig.continueOnFailure = true;
    
    testConfig.agentParams.numSteps = 2000;
    testConfig.agentParams.plasticityStrength = 1.0f;
    testConfig.agentParams.neuronCount = 1500;
    
    std::cout << "Running performance benchmark with high load..." << std::endl;
    nlm::testing::runAgentTests(brain, agent, testConfig);
    
    std::cout << std::endl;
}

// Example 5: Extended Test Suite with Custom Tests
void example5_extended_test_suite() {
    std::cout << "=== Example 5: Extended Test Suite with Custom Tests ===" << std::endl;
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Setup world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=40, height=40, visionWidth=16, visionHeight=16);
    world->reset();
    agent->initialize(*world);
    
    // Create extended test suite
    nlm::testing::TestConfig testConfig;
    testConfig.verbose = true;
    testConfig.outputFile = "extended_test_results.json";
    testConfig.continueOnFailure = true;
    
    // Create test suite
    nlm::testing::AgentTestSuite testSuite(testConfig);
    
    // Add all default tests
    testSuite.addDefaultTests();
    
    // Add custom test
    class CustomTest : public nlm::testing::AgentTest {
    public:
        TestResult run() override {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                // Custom test logic: Check if agent can process multiple sensory inputs
                bool testPassed = true;
                std::string message = "Custom test passed";
                
                // Create multiple sensory inputs
                for (int i = 0; i < 5; ++i) {
                    SensoryPercept percept;
                    std::vector<float> vision(64, 0.0f);
                    
                    // Different pattern each iteration
                    for (int j = 0; j < 10; ++j) {
                        vision[j] = static_cast<float>(i + 1) / 5.0f;
                    }
                    
                    percept.setVision(vision);
                    agent_->processSensoryInput(percept);
                }
                
                // Verify agent state changed
                float finalCuriosity = agent_->getCuriosityLevel();
                if (finalCuriosity < 0.01f) {
                    testPassed = false;
                    message = "Custom test failed - curiosity did not increase with multiple inputs";
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                
                TestResult result(getTestName(), testPassed, message);
                result.metrics["final_curiosity"] = std::to_string(finalCuriosity);
                result.metrics["inputs_processed"] = "5";
                
                return result;
                
            } catch (const std::exception& e) {
                auto endTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double>(endTime - startTime).count();
                
                return TestResult(getTestName(), false, 
                                std::string("Exception: ") + e.what());
            }
        }
        
        std::string getTestName() const override { return "CustomMultiInputTest"; }
    };
    
    testSuite.addTest(std::make_shared<CustomTest>());
    
    std::cout << "Running extended test suite with custom test..." << std::endl;
    testSuite.runAll();
    
    std::cout << std::endl;
}

// Example 6: Test Results Analysis
void example6_test_results_analysis() {
    std::cout << "=== Example 6: Test Results Analysis ===" << std::endl;
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Setup world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->reset();
    agent->initialize(*world);
    
    // Run tests
    nlm::testing::TestConfig testConfig;
    testConfig.verbose = false; // Suppress output for analysis
    testConfig.outputFile = "analysis_test_results.json";
    
    nlm::testing::runAgentTests(brain, agent, testConfig);
    
    // Load and analyze results (simplified analysis)
    std::cout << "Test Results Analysis:" << std::endl;
    std::cout << "  - Load results file: analysis_test_results.json" << std::endl;
    std::cout << "  - Generate summary report..." << std::endl;
    
    // This would normally load the JSON and perform statistical analysis
    // For now, we'll just show the summary
    std::cout << "  - Total tests executed: Check analysis_test_results.json" << std::endl;
    std::cout << "  - Success rate: Check analysis_test_results.json" << std::endl;
    std::cout << "  - Average execution time: Check analysis_test_results.json" << std::endl;
    std::cout << std::endl;
    std::cout << "Analysis complete. See JSON file for detailed results." << std::endl;
    
    std::cout << std::endl;
}

// Main function to run all examples
int main() {
    std::cout << "NLM Agent Testing Framework Examples" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    try {
        example1_basic_usage();
        example2_custom_configuration();
        example3_individual_tests();
        example4_performance_benchmarking();
        example5_extended_test_suite();
        example6_test_results_analysis();
        
        std::cout << "=== All Examples Completed ===" << std::endl;
        std::cout << "Check output files in the current directory for detailed results." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error running examples: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}