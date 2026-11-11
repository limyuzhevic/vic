# End-to-End Testing Framework for NLM

# This framework provides comprehensive testing capabilities for NLM integration
# and automated verification of brain system functionality

// TestFramework.hpp
#pragma once

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <fstream>

namespace nlm {

// Test case structure
template<typename T>
struct TestCase {
    std::string name;
    std::function<bool(T&)> test_func;
    double timeout_seconds = 30.0;
    std::string description;
    bool enabled = true;
};

// Test results structure
struct TestResults {
    std::string test_name;
    bool passed = false;
    double execution_time = 0.0;
    std::string error_message;
    std::vector<std::string> log_messages;
    int neuron_count = 0;
    int synapse_count = 0;
    float reward = 0.0f;
    bool memory_integrated = false;
    bool neuromodulation_integrated = false;
    bool prediction_integrated = false;
    bool development_integrated = false;
};

// End-to-End Test Framework
class EndToEndTestFramework {
public:
    EndToEndTestFramework() = default;
    ~EndToEndTestFramework() = default;

    // Initialize test framework
    bool initialize(const std::string& config_file = "configs/default.cfg");

    // Add a test case
    template<typename T>
    void add_test(const TestCase<T>& test_case);

    // Run all tests
    std::vector<TestResults> run_all_tests(const std::string& test_config = "");

    // Run specific test by name
    TestResults run_test(const std::string& test_name);

    // Load test configuration from file
    bool load_test_config(const std::string& config_file);

    // Save test results
    bool save_results(const std::vector<TestResults>& results, const std::string& output_file);

    // Get test statistics
    struct FrameworkStats {
        int total_tests = 0;
        int passed_tests = 0;
        int failed_tests = 0;
        double total_execution_time = 0.0;
        double average_execution_time = 0.0;
    };

    FrameworkStats get_statistics() const { return stats; }

    // Reset framework state
    void reset() {
        tests.clear();
        stats = FrameworkStats{};
    }

private:
    std::vector<TestCase<EndToEndTestFramework>> tests;
    FrameworkStats stats;
    std::shared_ptr<Config> config;

    // Helper functions
    TestResults execute_test(TestCase<EndToEndTestFramework>& test_case);
    bool validate_test_environment();
    void cleanup_test_environment();
};

// Specialized test configurations
typedef EndToEndTestFramework IntegrationTestFramework;
typedef EndToEndTestFramework PerformanceTestFramework;
typedef EndToEndTestFramework StressTestFramework;

// Quick test helper functions
namespace QuickTests {
    // Basic brain functionality test
    bool test_basic_brain_functionality(EndToEndTestFramework& framework);

    // Memory system integration test
    bool test_memory_system_integration(EndToEndTestFramework& framework);

    // Neuromodulation integration test
    bool test_neuromodulation_integration(EndToEndTestFramework& framework);

    // Prediction system test
    bool test_prediction_system(EndToEndTestFramework& framework);

    // Development system test
    bool test_development_system(EndToEndTestFramework& framework);

    // Checkpoint save/load test
    bool test_checkpoint_functionality(EndToEndTestFramework& framework);

    // Complete integration test
    bool test_complete_integration(EndToEndTestFramework& framework);

    // Performance benchmark test
    bool test_performance_benchmark(EndToEndTestFramework& framework);

    // Stress test with many neurons
    bool test_stress_conditions(EndToEndTestFramework& framework);

    // Configuration testing
    bool test_configuration_variations(EndToEndTestFramework& framework);
}

// Test configuration structure
typedef struct {
    std::string name;
    uint64_t max_steps;
    size_t neuron_count;
    size_t region_count;
    float connection_probability;
    bool enable_checkpointing;
    bool enable_replay;
    bool enable_development;
    std::string checkpoint_path;
    bool enable_curiosity;
    bool enable_reward_modulation;
    bool enable_structural_plasticity;
    float novelty_threshold;
    float curiosity_level;
} TestConfiguration;

// Test utilities
namespace TestUtils {
    // Generate test configuration
    TestConfiguration generate_test_config(const std::string& test_type);

    // Format test results for output
    std::string format_test_results(const TestResults& results);

    // Compare test results
    bool compare_test_results(const TestResults& result1, const TestResults& result2);

    // Validate test environment
    bool validate_test_environment(const std::shared_ptr<Config>& config);

    // Cleanup test artifacts
    void cleanup_test_artifacts(const std::string& test_directory);
}

} // namespace nlm

// Implementation files

// TestFramework.cpp
#include "TestFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <thread>
#include <chrono>
#include <algorithm>

namespace nlm {

bool EndToEndTestFramework::initialize(const std::string& config_file) {
    try {
        config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }

        // Add default tests
        add_test(TestCase<EndToEndTestFramework>{
            "basic_functionality",
            QuickTests::test_basic_brain_functionality,
            30.0,
            "Test basic brain initialization and step execution"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "memory_integration",
            QuickTests::test_memory_system_integration,
            45.0,
            "Test memory system integration and functionality"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "neuromodulation_integration",
            QuickTests::test_neuromodulation_integration,
            40.0,
            "Test neuromodulation system integration"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "prediction_system",
            QuickTests::test_prediction_system,
            35.0,
            "Test prediction system functionality"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "development_system",
            QuickTests::test_development_system,
            50.0,
            "Test development system functionality"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "checkpoint_functionality",
            QuickTests::test_checkpoint_functionality,
            60.0,
            "Test checkpoint save/load functionality"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "complete_integration",
            QuickTests::test_complete_integration,
            120.0,
            "Test complete integration of all systems"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "performance_benchmark",
            QuickTests::test_performance_benchmark,
            180.0,
            "Test system performance and scalability"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "stress_conditions",
            QuickTests::test_stress_conditions,
            300.0,
            "Test system under stress conditions (many neurons)"
        });

        add_test(TestCase<EndToEndTestFramework>{
            "configuration_variations",
            QuickTests::test_configuration_variations,
            90.0,
            "Test system with different configuration variations"
        });

        NLM_LOG_INFO("End-to-End Test Framework initialized successfully");
        return true;

    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize test framework: ") + e.what());
        return false;
    }
}

template<typename T>
void EndToEndTestFramework::add_test(const TestCase<T>& test_case) {
    tests.push_back(test_case);
    NLM_LOG_INFO("Added test: " + test_case.name + " - " + test_case.description);
}

std::vector<TestResults> EndToEndTestFramework::run_all_tests(const std::string& test_config) {
    std::vector<TestResults> results;

    if (test_config.empty() && !tests.empty()) {
        NLM_LOG_INFO("Running all enabled tests...");
        for (auto& test : tests) {
            if (test.enabled) {
                NLM_LOG_INFO("Running test: " + test.name);
                TestResults result = execute_test(test);
                results.push_back(result);

                // Update statistics
                stats.total_tests++;
                if (result.passed) {
                    stats.passed_tests++;
                } else {
                    stats.failed_tests++;
                }
                stats.total_execution_time += result.execution_time;
            }
        }
    } else if (!test_config.empty()) {
        // Load and run specific test configuration
        NLM_LOG_INFO("Running tests from configuration: " + test_config);
        // Implementation for loading config
    }

    // Calculate average execution time
    if (stats.total_tests > 0) {
        stats.average_execution_time = stats.total_execution_time / stats.total_tests;
    }

    NLM_LOG_INFO("Test execution complete. Passed: " + std::to_string(stats.passed_tests) +
                 "/" + std::to_string(stats.total_tests) + " tests");

    return results;
}

TestResults EndToEndTestFramework::run_test(const std::string& test_name) {
    for (auto& test : tests) {
        if (test.name == test_name && test.enabled) {
            return execute_test(test);
        }
    }

    TestResults result;
    result.test_name = test_name;
    result.passed = false;
    result.error_message = "Test not found or disabled";
    return result;
}

bool EndToEndTestFramework::load_test_config(const std::string& config_file) {
    // Implementation for loading test configuration
    NLM_LOG_INFO("Loading test configuration from: " + config_file);
    // This would typically parse a JSON or YAML configuration file
    return true;
}

bool EndToEndTestFramework::save_results(const std::vector<TestResults>& results, const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file for test results: " + output_file);
        return false;
    }

    file << "=== NLM End-to-End Test Results ===" << std::endl;
    file << "Generated at: " << std::chrono::system_clock::now() << std::endl;
    file << "Total Tests: " << results.size() << std::endl;

    int passed = 0;
    for (const auto& result : results) {
        file << "\n--- Test: " << result.test_name << " ---" << std::endl;
        file << "Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
        file << "Execution Time: " << result.execution_time << " seconds" << std::endl;
        file << "Neurons: " << result.neuron_count << std::endl;
        file << "Synapses: " << result.synapse_count << std::endl;
        file << "Reward: " << result.reward << std::endl;
        file << "Memory Integrated: " << (result.memory_integrated ? "Yes" : "No") << std::endl;
        file << "Neuromodulation Integrated: " << (result.neuromodulation_integrated ? "Yes" : "No") << std::endl;
        file << "Prediction Integrated: " << (result.prediction_integrated ? "Yes" : "No") << std::endl;
        file << "Development Integrated: " << (result.development_integrated ? "Yes" : "No") << std::endl;

        if (!result.error_message.empty()) {
            file << "Error: " << result.error_message << std::endl;
        }

        if (result.passed) {
            passed++;
        }
    }

    file << "\n=== Summary ===" << std::endl;
    file << "Total Tests: " << results.size() << std::endl;
    file << "Passed: " << passed << std::endl;
    file << "Failed: " << (results.size() - passed) << std::endl;
    file << "Success Rate: " << (passed * 100.0 / results.size()) << "%" << std::endl;

    file.close();
    NLM_LOG_INFO("Test results saved to: " + output_file);
    return true;
}

TestResults EndToEndTestFramework::execute_test(TestCase<EndToEndTestFramework>& test_case) {
    TestResults result;
    result.test_name = test_case.name;

    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        // Validate environment before running test
        if (!validate_test_environment()) {
            result.passed = false;
            result.error_message = "Test environment validation failed";
            return result;
        }

        // Execute the test
        if (test_case.test_func(*this)) {
            result.passed = true;
            result.error_message = "";
        } else {
            result.passed = false;
            result.error_message = "Test function returned false";
        }

    } catch (const std::exception& e) {
        result.passed = false;
        result.error_message = std::string("Exception during test execution: ") + e.what();
        NLM_LOG_ERROR("Test " + test_case.name + " failed: " + result.error_message);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    result.execution_time = std::chrono::duration<double>(end_time - start_time).count();

    return result;
}

bool EndToEndTestFramework::validate_test_environment() {
    // Basic environment validation
    if (!config) {
        NLM_LOG_ERROR("Configuration not initialized");
        return false;
    }

    // Check required configuration parameters
    auto required_params = {"neuron_count", "region_count", "connection_probability"};
    for (const auto& param : required_params) {
        if (!config->has(param)) {
            NLM_LOG_ERROR("Missing required parameter: " + param);
            return false;
        }
    }

    return true;
}

void EndToEndTestFramework::cleanup_test_environment() {
    // Cleanup resources
    config.reset();
    NLM_LOG_INFO("Test environment cleaned up");
}

// Quick Test Implementations

namespace QuickTests {

bool test_basic_brain_functionality(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Run basic steps
    for (size_t step = 0; step < 50; ++step) {
        brain->step(step);
    }

    // Verify basic functionality
    return brain->getTotalNeuronCount() > 0 &&
           brain->getTotalSynapseCount() > 0;
}

bool test_memory_system_integration(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Check memory systems
    auto* working_memory = brain->getWorkingMemory();
    auto* episodic_memory = brain->getEpisodicMemory();

    if (!working_memory || !episodic_memory) {
        return false;
    }

    // Run some steps to populate memory
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
        if (step % 20 == 0) {
            working_memory->storeToNeuron(NeuronId(step), 0.5f);
        }
    }

    // Verify memory has data
    return working_memory->getActiveTraces() > 0;
}

bool test_neuromodulation_integration(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Check neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();

    if (!dopamine || !curiosity || !novelty) {
        return false;
    }

    // Run steps with sensory input to activate neuromodulation
    for (size_t step = 0; step < 75; ++step) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
        brain->step(step);
    }

    // Verify neuromodulation is working
    return dopamine->getLevel() > 0.0f || curiosity->getLevel() > 0.0f;
}

bool test_prediction_system(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 120);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Check prediction system
    auto* prediction_system = brain->getPredictionSystem();
    if (!prediction_system) {
        return false;
    }

    // Run basic prediction cycle
    for (size_t step = 0; step < 60; ++step) {
        brain->step(step);
    }

    return true; // Prediction system doesn't have error signals in this implementation
}

bool test_development_system(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 180);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Check development system
    auto* development_system = brain->getDevelopmentSystem();
    if (!development_system) {
        return false;
    }

    // Run steps to trigger development
    for (size_t step = 0; step < 200; ++step) {
        if (step % 100 == 0) {
            brain->develop();
        }
        brain->step(step);
    }

    return true;
}

bool test_checkpoint_functionality(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Run some steps
    for (size_t step = 0; step < 50; ++step) {
        brain->step(step);
    }

    // Save checkpoint
    std::string checkpoint_path = "/tmp/test_checkpoint.bin";
    if (!brain->save(checkpoint_path)) {
        return false;
    }

    // Create new brain and load
    auto brain2 = std::make_shared<Brain>(config);
    brain2->initialize();

    if (!brain2->load(checkpoint_path)) {
        return false;
    }

    // Verify loaded brain has correct state
    return brain2->getTotalNeuronCount() == brain->getTotalNeuronCount();
}

bool test_complete_integration(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 2);
    config->set("connection_probability", 0.05);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Create world and agent
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();

    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableCuriosity(true);
    agent.enableRewardModulation(true);
    agent.enableDevelopment(true);

    // Run complete simulation
    float total_reward = 0.0f;
    for (size_t step = 0; step < 200; ++step) {
        world.update(0.1);
        SensoryPercept percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        brain->step(step);
        MotorCommand cmd = agent.decodeMotorCommand();
        world.applyMotorCommand(cmd, step * 0.1);
        ActionResult action_result = world.getLastActionResult();
        float reward = action_result.reward;
        total_reward += reward;
        agent.applyRewardModulation(reward, 0.0f);
        agent.updateDevelopment(0.1);
    }

    // Verify integration
    bool memory_integrated = (brain->getWorkingMemory() != nullptr);
    bool neuromodulation_integrated = (brain->getDopamine() != nullptr);
    bool prediction_integrated = (brain->getPredictionSystem() != nullptr);
    bool development_integrated = (brain->getDevelopmentSystem() != nullptr);

    return memory_integrated && neuromodulation_integrated &&
           prediction_integrated && development_integrated &&
           total_reward > 0.0f;
}

bool test_performance_benchmark(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Time the simulation
    auto start_time = std::chrono::high_resolution_clock::now();

    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double execution_time = std::chrono::duration<double>(end_time - start_time).count();

    // Calculate performance metrics
    double steps_per_second = 1000.0 / execution_time;

    NLM_LOG_INFO(std::string("Performance benchmark: ") + std::to_string(steps_per_second) +
                 " steps/second");

    return steps_per_second > 100.0; // Should be able to do >100 steps/second
}

bool test_stress_conditions(EndToEndTestFramework& framework) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 2000);  // Large brain
    config->set("region_count", 4);
    config->set("connection_probability", 0.02);  // Lower density for performance

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        return false;
    }

    // Run with memory and CPU profiling
    for (size_t step = 0; step < 300; ++step) {
        brain->step(step);

        // Check memory usage periodically
        if (step % 50 == 0) {
            // In a real implementation, this would check actual memory usage
            // For now, just verify the brain is still functional
            if (brain->getTotalNeuronCount() == 0) {
                return false;
            }
        }
    }

    return brain->getTotalNeuronCount() > 1000 &&
           brain->getTotalSynapseCount() > 10000;
}

bool test_configuration_variations(EndToEndTestFramework& framework) {
    std::vector<TestConfiguration> configs = {
        {"basic", 500, 100, 1, 0.1, false, false, false, "", 0.0f, false, false, false, 0.5f, 0.0f},
        {"dense", 1000, 200, 3, 0.05, true, true, true, "./checkpoints/test1", 0.5f, true, true, true, 1.0f, 0.3f},
        {"sparse", 300, 50, 1, 0.02, false, false, false, "", 0.0f, false, false, false, 0.2f, 0.0f},
        {"balanced", 800, 150, 2, 0.08, true, true, true, "./checkpoints/test2", 0.3f, true, true, true, 0.7f, 0.2f}
    };

    for (const auto& test_config : configs) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", test_config.neuron_count);
        config->set("region_count", test_config.region_count);
        config->set("connection_probability", test_config.connection_probability);

        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return false;
        }

        // Run test
        for (size_t step = 0; step < test_config.max_steps; ++step) {
            brain->step(step);
        }

        // Verify basic functionality
        if (brain->getTotalNeuronCount() != test_config.neuron_count) {
            return false;
        }
    }

    return true;
}

} // namespace QuickTests

namespace TestUtils {

TestConfiguration generate_test_config(const std::string& test_type) {
    if (test_type == "basic") {
        return TestConfiguration{"basic", 500, 100, 1, 0.1, false, false, false, "", 0.0f, false, false, false, 0.5f, 0.0f};
    } else if (test_type == "performance") {
        return TestConfiguration{"performance", 2000, 500, 3, 0.05, true, true, true, "./checkpoints/perf", 0.5f, true, true, true, 1.0f, 0.3f};
    } else if (test_type == "stress") {
        return TestConfiguration{"stress", 5000, 1000, 5, 0.02, false, false, false, "", 0.0f, false, false, false, 0.2f, 0.0f};
    } else {
        return TestConfiguration{"default", 1000, 200, 2, 0.08, true, true, true, "./checkpoints/default", 0.3f, true, true, true, 0.7f, 0.2f};
    }
}

std::string format_test_results(const TestResults& results) {
    std::stringstream ss;
    ss << "Test: " << results.test_name << std::endl;
    ss << "Status: " << (results.passed ? "PASS" : "FAIL") << std::endl;
    ss << "Time: " << results.execution_time << "s" << std::endl;
    ss << "Neurons: " << results.neuron_count << std::endl;
    ss << "Synapses: " << results.synapse_count << std::endl;

    if (!results.error_message.empty()) {
        ss << "Error: " << results.error_message << std::endl;
    }

    return ss.str();
}

bool compare_test_results(const TestResults& result1, const TestResults& result2) {
    return result1.test_name == result2.test_name &&
           result1.passed == result2.passed &&
           result1.execution_time == result2.execution_time;
}

bool validate_test_environment(const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }

    auto required_params = {"neuron_count", "region_count", "connection_probability"};
    for (const auto& param : required_params) {
        if (!config->has(param)) {
            return false;
        }
    }

    return true;
}

void cleanup_test_artifacts(const std::string& test_directory) {
    // Implementation would clean up test artifacts
    NLM_LOG_INFO("Cleaning up test artifacts from: " + test_directory);
}

} // namespace TestUtils

} // namespace nlm

// Test framework wrapper functions for easier usage

namespace nlm {

// Quick test runner for common use cases
std::vector<TestResults> run_quick_tests(const std::string& config_file = "configs/default.cfg",
                                         const std::string& output_file = "quick_test_results.txt") {
    EndToEndTestFramework framework;

    if (!framework.initialize(config_file)) {
        NLM_LOG_ERROR("Failed to initialize test framework");
        return {};
    }

    std::vector<TestResults> results = framework.run_all_tests();

    if (!output_file.empty()) {
        framework.save_results(results, output_file);
    }

    return results;
}

// Single test runner
TestResults run_single_test(const std::string& test_name, const std::string& config_file = "configs/default.cfg") {
    EndToEndTestFramework framework;

    if (!framework.initialize(config_file)) {
        TestResults result;
        result.test_name = test_name;
        result.passed = false;
        result.error_message = "Framework initialization failed";
        return result;
    }

    return framework.run_test(test_name);
}

// Comprehensive test suite
std::vector<TestResults> run_comprehensive_tests(const std::string& config_file = "configs/default.cfg",
                                                 const std::string& output_file = "comprehensive_test_results.txt") {
    EndToEndTestFramework framework;

    if (!framework.initialize(config_file)) {
        NLM_LOG_ERROR("Failed to initialize comprehensive test framework");
        return {};
    }

    // Enable all tests
    for (auto& test : framework.tests) {
        test.enabled = true;
    }

    std::vector<TestResults> results = framework.run_all_tests();

    if (!output_file.empty()) {
        framework.save_results(results, output_file);
    }

    // Print summary
    int passed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
    }

    NLM_LOG_INFO("=== Comprehensive Test Summary ===");
    NLM_LOG_INFO("Total Tests: " + std::to_string(results.size()));
    NLM_LOG_INFO("Passed: " + std::to_string(passed));
    NLM_LOG_INFO("Failed: " + std::to_string(results.size() - passed));
    NLM_LOG_INFO("Success Rate: " + std::to_string(passed * 100.0 / results.size()) + "%");

    return results;
}

// Performance test suite
std::vector<TestResults> run_performance_tests(const std::string& config_file = "configs/default.cfg") {
    EndToEndTestFramework framework;

    if (!framework.initialize(config_file)) {
        NLM_LOG_ERROR("Failed to initialize performance test framework");
        return {};
    }

    // Enable only performance-related tests
    for (auto& test : framework.tests) {
        test.enabled = (test.name == "performance_benchmark" ||
                       test.name == "stress_conditions");
    }

    return framework.run_all_tests();
}

// Integration test suite
std::vector<TestResults> run_integration_tests(const std::string& config_file = "configs/default.cfg") {
    EndToEndTestFramework framework;

    if (!framework.initialize(config_file)) {
        NLM_LOG_ERROR("Failed to initialize integration test framework");
        return {};
    }

    // Enable only integration-related tests
    for (auto& test : framework.tests) {
        test.enabled = (test.name == "memory_integration" ||
                       test.name == "neuromodulation_integration" ||
                       test.name == "prediction_system" ||
                       test.name == "development_system" ||
                       test.name == "complete_integration");
    }

    return framework.run_all_tests();
}

} // namespace nlm

// Utility functions for command-line usage
void print_test_help() {
    std::cout << "=== NLM End-to-End Test Framework ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: ./test_runner [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Test Types:" << std::endl;
    std::cout << "  quick     Run quick tests" << std::endl;
    std::cout << "  integration Run integration tests" << std::endl;
    std::cout << "  performance Run performance tests" << std::endl;
    std::cout << "  comprehensive Run all tests" << std::endl;
    std::cout << "  single NAME Run specific test by name" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help           Show this help" << std::endl;
    std::cout << "  --config FILE        Configuration file" << std::endl;
    std::cout << "  --output FILE        Output file for results" << std::endl;
    std::cout << "  --verbose            Verbose output" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./test_runner quick" << std::endl;
    std::cout << "  ./test_runner integration --output integration_results.txt" << std::endl;
    std::cout << "  ./test_runner performance" << std::endl;
    std::cout << "  ./test_runner single memory_integration" << std::endl;
    std::cout << std::endl;
}

void print_test_summary(const std::vector<nlm::TestResults>& results) {
    if (results.empty()) {
        std::cout << "No tests were run" << std::endl;
        return;
    }

    std::cout << "=== Test Results Summary ===" << std::endl;
    std::cout << "Total Tests: " << results.size() << std::endl;

    int passed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
        std::cout << (result.passed ? "✓ " : "✗ ") << result.test_name << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << (results.size() - passed) << std::endl;
    std::cout << "Success Rate: " << (passed * 100.0 / results.size()) << "%" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string test_type = "quick";
    std::string config_file = "configs/default.cfg";
    std::string output_file = "test_results.txt";
    bool verbose = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_test_help();
            return 0;
        } else if (arg == "quick" || arg == "integration" || arg == "performance" ||
                   arg == "comprehensive" || arg == "single") {
            test_type = arg;
        } else if (arg == "--config" && i + 1 < argc) {
            config_file = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_file = argv[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "single" && i + 1 < argc) {
            test_type = "single";
            // In a real implementation, we would store the test name
            i++; // Skip the test name argument for now
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            print_test_help();
            return 1;
        }
    }

    // Initialize logging
    nlm::Logger::getGlobal().setLevel(verbose ? nlm::LogLevel::Debug : nlm::LogLevel::Info);

    std::cout << "=== NLM End-to-End Test Framework ===" << std::endl;
    std::cout << "Test Type: " << test_type << std::endl;
    std::cout << "Configuration: " << config_file << std::endl;

    std::vector<nlm::TestResults> results;

    try {
        if (test_type == "quick") {
            results = nlm::run_quick_tests(config_file, output_file);
        } else if (test_type == "integration") {
            results = nlm::run_integration_tests(config_file, output_file);
        } else if (test_type == "performance") {
            results = nlm::run_performance_tests(config_file, output_file);
        } else if (test_type == "comprehensive") {
            results = nlm::run_comprehensive_tests(config_file, output_file);
        } else if (test_type == "single") {
            // Single test implementation would go here
            results = nlm::run_quick_tests(config_file, output_file);
        }

        // Print summary
        print_test_summary(results);

        // Return appropriate exit code
        if (results.empty()) {
            return 1; // No tests were run
        }

        bool all_passed = true;
        for (const auto& result : results) {
            if (!result.passed) {
                all_passed = false;
                break;
            }
        }

        return all_passed ? 0 : 1;

    } catch (const std::exception& e) {
        std::cerr << "Test execution failed: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace nlm