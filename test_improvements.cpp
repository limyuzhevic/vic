// Test program to verify the improved NLM functionality
// This tests the new cloning, batch processing, and configuration validation features

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>

void test_configuration_validation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Set some configuration values
    config.set("random_seed", static_cast<int64_t>(42));
    config.set("neuron_count", static_cast<int64_t>(100));
    config.set("region_count", static_cast<int64_t>(2));
    config.set("connection_probability", 0.1f);
    config.set("dopamine_baseline", 0.1f);
    
    // Test basic validation
    bool valid = config.validate("");
    std::cout << "  Configuration validation result: " << (valid ? "PASS" : "FAIL") << std::endl;
    
    // Test validation errors
    auto errors = config.getValidationErrors();
    if (!errors.empty()) {
        std::cout << "  Validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "    " << error << std::endl;
        }
    }
    
    std::cout << "  Configuration validation test completed." << std::endl;
}

void test_brain_cloning() {
    std::cout << "Testing brain cloning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(10)); // Small for testing
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.2f);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cout << "  ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    // Test cloning
    auto clonedBrain = brain->clone();
    if (!clonedBrain) {
        std::cout << "  ERROR: Failed to clone brain" << std::endl;
        return;
    }
    
    if (!clonedBrain->initialize()) {
        std::cout << "  ERROR: Failed to initialize cloned brain" << std::endl;
        return;
    }
    
    std::cout << "  Brain cloning test completed successfully." << std::endl;
}

void test_environment_cloning() {
    std::cout << "Testing environment cloning..." << std::endl;
    
    // Create a GridWorldEnvironment
    auto env = std::make_shared<nlm::GridWorldEnvironment>(5, 5);
    
    // Get initial state
    auto observation = env->reset();
    std::cout << "  Environment initial observation received" << std::endl;
    
    // Test cloning
    auto clonedEnv = env->clone();
    if (!clonedEnv) {
        std::cout << "  ERROR: Failed to clone environment" << std::endl;
        return;
    }
    
    // Get cloned environment state
    auto clonedObservation = clonedEnv->reset();
    std::cout << "  Cloned environment reset successfully" << std::endl;
    
    std::cout << "  Environment cloning test completed successfully." << std::endl;
}

void test_experiment_runner_batch() {
    std::cout << "Testing experiment runner batch processing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(5)); // Very small for testing
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.3f);
    
    auto brainTemplate = std::make_shared<nlm::Brain>(config);
    brainTemplate->initialize();
    
    auto environmentTemplate = std::make_shared<nlm::GridWorldEnvironment>(3, 3);
    
    auto runner = std::make_shared<nlm::ExperimentRunner>();
    
    // Create test experiments
    std::vector<std::shared_ptr<nlm::Experiment>> experiments;
    for (int i = 0; i < 3; ++i) {
        auto exp = runner->createExperiment("TestExperiment_" + std::to_string(i));
        exp->recordMetric("test_metric", 1.0f + i * 0.1f);
        experiments.push_back(exp);
    }
    
    // Progress callback for testing
    auto progressCallback = [](float progress) {
        std::cout << "  Batch progress: " << (progress * 100.0f) << "%" << std::endl;
    };
    
    // Run batch experiment
    std::vector<std::shared_ptr<nlm::Experiment>> results;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    results = runner->runBatch(experiments, brainTemplate, environmentTemplate, 100, progressCallback);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "  Batch experiment completed: " << results.size() << "/" << experiments.size()
              << " experiments successful in " << duration.count() << " ms" << std::endl;
    
    // Test saving results
    bool saveSuccess = runner->saveResults("test_results.txt");
    std::cout << "  Results save: " << (saveSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Clean up test file
    std::remove("test_results.txt");
    
    std::cout << "  Experiment runner batch test completed." << std::endl;
}

void test_configuration_file() {
    std::cout << "Testing configuration file handling..." << std::endl;
    
    nlm::Config config;
    
    // Test loading from default.cfg
    bool loadSuccess = config.loadFromFile("configs/default.cfg");
    std::cout << "  Configuration file loading: " << (loadSuccess ? "SUCCESS" << std::endl : "FAILED" << std::endl);
    
    if (loadSuccess) {
        std::cout << "  Configuration summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test specific values
        auto randomSeed = config.get<int64_t>("random_seed");
        if (randomSeed.has_value()) {
            std::cout << "  random_seed: " << randomSeed.value() << std::endl;
        }
        
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount.has_value()) {
            std::cout << "  neuron_count: " << neuronCount.value() << std::endl;
        }
    }
    
    std::cout << "  Configuration file test completed." << std::endl;
}

int main() {
    std::cout << "=== NLM Functionality Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    test_configuration_validation();
    std::cout << std::endl;
    
    test_brain_cloning();
    std::cout << std::endl;
    
    test_environment_cloning();
    std::cout << std::endl;
    
    test_configuration_file();
    std::cout << std::endl;
    
    test_experiment_runner_batch();
    std::cout << std::endl;
    
    std::cout << "=== All tests completed ===" << std::endl;
    return 0;
}
