// Advanced Feature Integration Tests
// Comprehensive test suite for advanced feature integration

#include "AdvancedFeatureIntegrationTestSuite.h"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/ActionConsequencePredictor.hpp"
#include "../src/prediction/PredictionErrorSignal.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace test_suites {

class AdvancedFeatureIntegrationTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Advanced Feature Integration Tests ===" << std::endl;
        
        test_end_to_end_system_integration();
        test_performance_and_scalability();
        test_cross_component_interaction();
        test_backward_compatibility();
        test_real_time_integration();
        test_hybrid_system_integration();
        test_adaptive_system_integration();
        test_monitoring_and_alerting();
        test_error_recovery();
        test_system_optimization();
        
        std::cout << "=== Advanced Feature Integration Tests Complete ===" << std::endl;
    }

private:
    static void test_end_to_end_system_integration() {
        std::cout << "Testing end-to-end system integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 600);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Create agent and world
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        auto world = std::make_unique<nlm::SimpleWorld>();
        
        // Configure world
        world->configure(20, 20, 10, 10);
        world->setAgentStart(10, 10);
        
        // Integrate agent with world
        agent->initialize(world.get());
        
        // End-to-end simulation
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // World update
            world->update(step);
            
            // Get sensor percept
            auto percept = world->getSensoryPercept();
            
            // Agent processing
            agent->processSensoryInput(percept);
            
            // Get action
            auto action = brain.produceAction();
            if (action) {
                world->applyMotorCommand(action->getActionType(), step);
            }
            
            // Reward application
            float reward = 0.1f;
            if (step % 20 == 0) reward = 1.0f;
            agent->applyRewardModulation(reward, 0.0f);
            
            // Development update
            agent->updateDevelopment(step);
        }
        
        // Verify end-to-end integration
        assert(agent->getBrain().get() == &brain);
        assert(world->getWidth() == 20);
        assert(world->getHeight() == 20);
        
        std::cout << "    End-to-end system integration: PASSED" << std::endl;
    }
    
    static void test_performance_and_scalability() {
        std::cout << "Testing performance and scalability..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Performance testing
        std::vector<double> performanceMetrics;
        std::vector<double> scalabilityMetrics;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Measure performance
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Complex operations
            brain.updatePlasticity();
            brain.develop();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            performanceMetrics.push_back(duration);
            
            // Scalability test (increase complexity)
            if (step % 20 == 0) {
                config->set("neuron_count", 500 + step * 50);
                brain.reset();
                config->set("neuron_count", 500 + step * 50);
                brain.initialize();
                
                double scalabilityScore = brain.getPerformanceScore();
                scalabilityMetrics.push_back(scalabilityScore);
            }
        }
        
        // Verify performance and scalability
        assert(performanceMetrics.size() == 80);
        assert(scalabilityMetrics.size() == 4);  // 4 scalability tests
        
        // Performance should be reasonable
        double avgPerformance = std::accumulate(performanceMetrics.begin(), performanceMetrics.end(), 0.0) / performanceMetrics.size();
        assert(avgPerformance < 100.0);  // Less than 100ms per operation
        
        // Scalability should improve
        double initialScalability = scalabilityMetrics[0];
        double finalScalability = scalabilityMetrics.back();
        assert(finalScalability >= initialScalability * 0.9f);  // At least maintain performance
        
        std::cout << "    Performance and scalability: PASSED" << std::endl;
    }
    
    static void test_cross_component_interaction() {
        std::cout << "Testing cross-component interaction..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Cross-component interaction testing
        std::vector<std::string> interactionTypes;
        std::vector<double> interactionScores;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            // Test cross-component interactions
            double memoryInteraction = brain.getMemorySystemInteraction();
            double predictionInteraction = brain.getPredictionSystemInteraction();
            double neuromodulationInteraction = brain.getNeuromodulationInteraction();
            double cognitiveInteraction = brain.getCognitiveSystemInteraction();
            
            double totalInteraction = memoryInteraction + predictionInteraction + 
                                    neuromodulationInteraction + cognitiveInteraction;
            
            interactionTypes.push_back("interaction_" + std::to_string(step % 5));
            interactionScores.push_back(totalInteraction);
            
            // Verify cross-component coordination
            assert(memoryInteraction >= 0.0f && memoryInteraction <= 1.0f);
            assert(predictionInteraction >= 0.0f && predictionInteraction <= 1.0f);
            assert(neuromodulationInteraction >= 0.0f && neuromodulationInteraction <= 1.0f);
            assert(cognitiveInteraction >= 0.0f && cognitiveInteraction <= 1.0f);
        }
        
        // Verify cross-component interaction
        assert(interactionTypes.size() == 70);
        assert(interactionScores.size() == 70);
        
        // Cross-component interaction should be effective
        double avgInteractionScore = std::accumulate(interactionScores.begin(), interactionScores.end(), 0.0) / interactionScores.size();
        assert(avgInteractionScore >= 0.5f);
        
        std::cout << "    Cross-component interaction: PASSED" << std::endl;
    }
    
    static void test_backward_compatibility() {
        std::cout << "Testing backward compatibility..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Backward compatibility testing
        std::vector<std::string> compatibilityTests;
        std::vector<bool> compatibilityResults;
        
        for (int step = 0; step < 60; ++step) {
            std::string test = "compat_test_" + std::to_string(step % 6);
            compatibilityTests.push_back(test);
            
            // Test backward compatibility with legacy configurations
            if (step % 10 == 0) {
                // Old-style configuration
                auto legacyConfig = std::make_shared<nlm::Config>();
                legacyConfig->set("neuron_count", 100, nlm::ConfigSource::Default);
                legacyConfig->set("learning_rate", 0.1f, nlm::ConfigSource::Default);
                
                // Convert to new format
                bool conversionSuccess = brain.convertFromLegacyConfig(legacyConfig);
                compatibilityResults.push_back(conversionSuccess);
            }
            
            // Test API compatibility
            bool apiCompatible = brain.checkAPIBackwardCompatibility();
            compatibilityResults.push_back(apiCompatible);
            
            if (step % 12 == 0) {
                // Test data format compatibility
                bool dataCompatible = brain.checkDataFormatCompatibility();
                compatibilityResults.push_back(dataCompatible);
            }
        }
        
        // Verify backward compatibility
        assert(compatibilityTests.size() == 60);
        assert(compatibilityResults.size() == 60);
        
        // Backward compatibility should be maintained
        float compatibilitySuccessRate = 0.0f;
        for (bool result : compatibilityResults) {
            if (result) compatibilitySuccessRate += 100.0f;
        }
        compatibilitySuccessRate /= compatibilityResults.size();
        assert(compatibilitySuccessRate >= 80.0f);  // At least 80% backward compatibility
        
        std::cout << "    Backward compatibility: PASSED" << std::endl;
    }
    
    static void test_real_time_integration() {
        std::cout << "Testing real-time integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 350);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Real-time integration testing
        std::vector<double> latencyMetrics;
        std::vector<double> throughputMetrics;
        
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            // Measure real-time performance
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Real-time operations
            brain.updatePlasticity();
            brain.develop();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            latencyMetrics.push_back(duration);
            
            // Calculate throughput
            double throughput = 1000.0 / duration;  // Operations per second
            throughputMetrics.push_back(throughput);
            
            // Real-time constraints
            assert(duration < 50.0);  // Less than 50ms latency
        }
        
        // Verify real-time integration
        assert(latencyMetrics.size() == 50);
        assert(throughputMetrics.size() == 50);
        
        // Latency should be low
        double avgLatency = std::accumulate(latencyMetrics.begin(), latencyMetrics.end(), 0.0) / latencyMetrics.size();
        assert(avgLatency < 25.0);  // Less than 25ms average latency
        
        // Throughput should be high
        double avgThroughput = std::accumulate(throughputMetrics.begin(), throughputMetrics.end(), 0.0) / throughputMetrics.size();
        assert(avgThroughput > 20.0);  // More than 20 operations per second
        
        std::cout << "    Real-time integration: PASSED" << std::endl;
    }
    
    static void test_hybrid_system_integration() {
        std::cout << "Testing hybrid system integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Hybrid system integration testing
        std::vector<std::string> integrationScenarios;
        std::vector<double> integrationScores;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            // Hybrid integration scenarios
            std::string scenario = "hybrid_" + std::to_string(step % 4);
            integrationScenarios.push_back(scenario);
            
            // Neural-hybrid integration
            double neuralHybridScore = brain.getNeuralHybridIntegrationScore();
            integrationScores.push_back(neuralHybridScore);
            
            // Check hybrid system health
            bool hybridSystemHealthy = brain.checkHybridSystemHealth();
            assert(hybridSystemHealthy);
            
            if (step % 14 == 0) {
                // Hybrid system optimization
                brain.optimizeHybridSystem();
            }
        }
        
        // Verify hybrid system integration
        assert(integrationScenarios.size() == 70);
        assert(integrationScores.size() == 70);
        
        // Hybrid integration should be effective
        double avgIntegrationScore = std::accumulate(integrationScores.begin(), integrationScores.end(), 0.0) / integrationScores.size();
        assert(avgIntegrationScore >= 0.7f);
        
        std::cout << "    Hybrid system integration: PASSED" << std::endl;
    }
    
    static void test_adaptive_system_integration() {
        std::cout << "Testing adaptive system integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Adaptive system integration testing
        std::vector<double> adaptationMetrics;
        std::vector<double> learningRates;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Adaptive system metrics
            double adaptationScore = brain.getAdaptiveSystemScore();
            double currentLearningRate = brain.getCurrentLearningRate();
            
            adaptationMetrics.push_back(adaptationScore);
            learningRates.push_back(currentLearningRate);
            
            // Adaptive system adjustment
            if (step % 16 == 0) {
                brain.adjustAdaptiveSystem(adaptationScore);
            }
            
            // Learning rate adaptation
            if (step % 20 == 0) {
                double optimalLearningRate = brain.getOptimalLearningRate();
                brain.setLearningRate(optimalLearningRate);
            }
        }
        
        // Verify adaptive system integration
        assert(adaptationMetrics.size() == 80);
        assert(learningRates.size() == 80);
        
        // Adaptation should be effective
        double avgAdaptationScore = std::accumulate(adaptationMetrics.begin(), adaptationMetrics.end(), 0.0) / adaptationMetrics.size();
        assert(avgAdaptationScore >= 0.6f);
        
        // Learning rate should be reasonable
        double avgLearningRate = std::accumulate(learningRates.begin(), learningRates.end(), 0.0) / learningRates.size();
        assert(avgLearningRate >= 0.01f && avgLearningRate <= 0.5f);
        
        std::cout << "    Adaptive system integration: PASSED" << std::endl;
    }
    
    static void test_monitoring_and_alerting() {
        std::cout << "Testing monitoring and alerting..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 350);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Monitoring and alerting testing
        std::vector<std::string> alertTypes;
        std::vector<bool> alertResults;
        
        for (int step = 0; step < 60; ++step) {
            brain.step(step);
            
            // Monitoring system operations
            if (step % 12 == 0) {
                // Health check
                bool healthy = brain.performHealthCheck();
                assert(healthy);
            }
            
            if (step % 15 == 0) {
                // Performance monitoring
                auto metrics = brain.getPerformanceMetrics();
                assert(!metrics.empty());
            }
            
            if (step % 18 == 0) {
                // Resource monitoring
                bool resourcesAvailable = brain.checkResourceAvailability();
                assert(resourcesAvailable);
            }
            
            // Alerting system
            std::string alertType = "alert_" + std::to_string(step % 5);
            alertTypes.push_back(alertType);
            
            bool alertTriggered = brain.triggerAlert(alertType);
            alertResults.push_back(alertTriggered);
            
            if (alertTriggered) {
                // Alert handling
                brain.handleAlert(alertType);
            }
        }
        
        // Verify monitoring and alerting
        assert(alertTypes.size() == 60);
        assert(alertResults.size() == 60);
        
        // Alerting should be effective
        float alertSuccessRate = 0.0f;
        for (bool result : alertResults) {
            if (result) alertSuccessRate += 100.0f;
        }
        alertSuccessRate /= alertResults.size();
        assert(alertSuccessRate >= 70.0f);  // At least 70% alert success
        
        std::cout << "    Monitoring and alerting: PASSED" << std::endl;
    }
    
    static void test_error_recovery() {
        std::cout << "Testing error recovery..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Error recovery testing
        std::vector<std::string> errorTypes;
        std::vector<bool> recoveryResults;
        
        for (int step = 0; step < 70; ++step) {
            brain.step(step);
            
            // Error injection and recovery
            std::string errorType = "error_" + std::to_string(step % 6);
            errorTypes.push_back(errorType);
            
            // Simulate error
            bool errorInjected = brain.injectError(errorType);
            if (errorInjected) {
                // Attempt recovery
                bool recoverySuccess = brain.recoverFromError(errorType);
                recoveryResults.push_back(recoverySuccess);
                
                if (recoverySuccess) {
                    // Verify recovery
                    assert(brain.getErrorCount() == 0);
                }
            }
            
            if (step % 14 == 0) {
                // Error cleanup
                brain.cleanupErrors();
            }
        }
        
        // Verify error recovery
        assert(errorTypes.size() == 70);
        assert(recoveryResults.size() == 70);
        
        // Error recovery should be effective
        float recoverySuccessRate = 0.0f;
        for (bool result : recoveryResults) {
            if (result) recoverySuccessRate += 100.0f;
        }
        recoverySuccessRate /= recoveryResults.size();
        assert(recoverySuccessRate >= 75.0f);  // At least 75% recovery success
        
        std::cout << "    Error recovery: PASSED" << std::endl;
    }
    
    static void test_system_optimization() {
        std::cout << "Testing system optimization..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // System optimization testing
        std::vector<double> optimizationMetrics;
        std::vector<double> efficiencyMetrics;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // System optimization
            double optimizationScore = brain.getOptimizationScore();
            double efficiencyScore = brain.getSystemEfficiency();
            
            optimizationMetrics.push_back(optimizationScore);
            efficiencyMetrics.push_back(efficiencyScore);
            
            // Apply optimizations
            if (step % 20 == 0) {
                brain.optimizePlasticity();
                brain.optimizeMemory();
                brain.optimizeLearning();
            }
        }
        
        // Verify system optimization
        assert(optimizationMetrics.size() == 80);
        assert(efficiencyMetrics.size() == 80);
        
        // Optimization should be effective
        double avgOptimizationScore = std::accumulate(optimizationMetrics.begin(), optimizationMetrics.end(), 0.0) / optimizationMetrics.size();
        assert(avgOptimizationScore >= 0.7f);
        
        // Efficiency should improve
        double avgEfficiencyScore = std::accumulate(efficiencyMetrics.begin(), efficiencyMetrics.end(), 0.0) / efficiencyMetrics.size();
        assert(avgEfficiencyScore >= 0.6f);
        
        std::cout << "    System optimization: PASSED" << std::endl;
    }
};

} // namespace test_suites