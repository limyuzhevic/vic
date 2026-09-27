// Test file for the NLM Training Module
// This file demonstrates the basic functionality of the training module

#include "training/TrainingCore.hpp"
#include "training/TrainingComponents.hpp"
#include "training/TrainingModule.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing NLM Training Module..." << std::endl;
    
    // Test 1: Create training core
    auto training_core = std::make_shared<nlm::TrainingCore>();
    assert(training_core != nullptr);
    std::cout << "✓ TrainingCore created successfully" << std::endl;
    
    // Test 2: Create training config
    auto training_config = std::make_shared<nlm::TrainingConfig>();
    assert(training_config != nullptr);
    std::cout << "✓ TrainingConfig created successfully" << std::endl;
    
    // Test 3: Create training callback
    auto training_callback = std::make_shared<nlm::SimpleTrainingCallback>();
    assert(training_callback != nullptr);
    std::cout << "✓ SimpleTrainingCallback created successfully" << std::endl;
    
    // Test 4: Create training module
    auto training_module = std::make_shared<nlm::TrainingModule>();
    assert(training_module != nullptr);
    std::cout << "✓ TrainingModule created successfully" << std::endl;
    
    // Test 5: Create training patterns
    auto training_patterns = std::make_shared<nlm::CommonTrainingPatterns>();
    assert(training_patterns != nullptr);
    std::cout << "✓ CommonTrainingPatterns created successfully" << std::endl;
    
    // Test 6: Create training persistence
    auto training_persistence = std::make_shared<nlm::TrainingPersistence>();
    assert(training_persistence != nullptr);
    std::cout << "✓ TrainingPersistence created successfully" << std::endl;
    
    // Test 7: Create simple training config
    auto simple_config = std::make_shared<nlm::SimpleTrainingConfig>();
    assert(simple_config != nullptr);
    std::cout << "✓ SimpleTrainingConfig created successfully" << std::endl;
    
    // Test 8: Create advanced training features
    auto advanced_features = std::make_shared<nlm::AdvancedTrainingFeatures>();
    assert(advanced_features != nullptr);
    std::cout << "✓ AdvancedTrainingFeatures created successfully" << std::endl;
    
    // Test 9: Test hyperparameter optimizer
    nlm::HyperparameterOptimizer optimizer;
    std::vector<nlm::Hyperparameter> param_space = {
        {"learning_rate", 0.001f, 0.0001f, 0.01f},
        {"discount_factor", 0.99f, 0.9f, 0.999f},
    };
    
    auto results = optimizer.optimize(
        param_space,
        [](const std::vector<nlm::Hyperparameter>&) { return 0.0f; },  // Dummy evaluation function
        nlm::OptimizationStrategy::RANDOM_SEARCH,
        10  // max iterations
    );
    
    assert(results.size() == 10);
    std::cout << "✓ HyperparameterOptimizer test passed" << std::endl;
    
    // Test 10: Test reward configuration
    nlm::RewardConfig reward_config;
    reward_config.type = nlm::RewardConfig::Type::SIMPLE;
    reward_config.simpleReward = 1.0f;
    
    assert(reward_config.type == nlm::RewardConfig::Type::SIMPLE);
    assert(reward_config.simpleReward == 1.0f);
    std::cout << "✓ RewardConfig test passed" << std::endl;
    
    // Test 11: Test environment configuration
    nlm::EnvironmentConfig env_config;
    env_config.enableDebugOutput = true;
    env_config.timeStep = 0.1f;
    env_config.maxTimePerEpisode = 100.0f;
    
    assert(env_config.enableDebugOutput == true);
    assert(env_config.timeStep == 0.1f);
    assert(env_config.maxTimePerEpisode == 100.0f);
    std::cout << "✓ EnvironmentConfig test passed" << std::endl;
    
    // Test 12: Test validation configuration
    nlm::ValidationConfig validation_config;
    validation_config.enableValidation = true;
    validation_config.validationFrequency = 10.0f;
    validation_config.earlyStopping = true;
    validation_config.patience = 20;
    
    assert(validation_config.enableValidation == true);
    assert(validation_config.validationFrequency == 10.0f);
    assert(validation_config.earlyStopping == true);
    assert(validation_config.patience == 20);
    std::cout << "✓ ValidationConfig test passed" << std::endl;
    
    // Test 13: Test callback methods
    auto callback = std::make_shared<nlm::SimpleTrainingCallback>();
    callback->onTrainingStart();
    callback->afterEpisode(1, 10.5f, true);
    callback->afterStep(1, 1, 1.0f, 0.5f);
    callback->onTrainingEnd();
    callback->onError("Test error");
    callback->onCheckpointSaved("test_checkpoint.nlm");
    std::cout << "✓ SimpleTrainingCallback methods test passed" << std::endl;
    
    // Test 14: Test performance monitoring
    nlm::TrainingMonitor monitor;
    monitor.recordEpisodePerformance(1, 10.5f, 0.5f, 1.0f);
    monitor.recordEpisodePerformance(2, 12.3f, 0.3f, 1.2f);
    
    assert(monitor.getEpisodesCompleted() == 2);
    assert(monitor.getAverageReward() == 11.4f);  // (10.5 + 12.3) / 2
    assert(monitor.getAverageLoss() == 0.4f);    // (0.5 + 0.3) / 2
    std::cout << "✓ TrainingMonitor test passed" << std::endl;
    
    std::cout << "\nAll training module tests passed!" << std::endl;
    
    return 0;
}
