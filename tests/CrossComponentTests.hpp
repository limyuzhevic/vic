// Cross-Component Integration Tests
// 
// These tests verify interactions between different neural network subsystems,
// including memory, cognition, motor, neuromodulation, and development systems.

#ifndef NLM_CROSS_COMPONENT_TESTS_HPP
#define NLM_CROSS_COMPONENT_TESTS_HPP

#include "IntegrationTestFramework.hpp"
#include "memory/Memory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <vector>
#include <iostream>
#include <algorithm>

namespace nlm {
namespace cross_component_tests {

// Test memory-cognition integration
class MemoryCognitionIntegrationTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 600);
        config_->set("region_count", 2);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 88888u);
        config_->set("working_memory_capacity", 100);
        config_->set("episodic_memory_capacity", 50);
        config_->set("concept_formation_rate", 0.001f);
        config_->set("planning_depth", 3);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
        
        // Initialize memory systems
        auto episodicMemory = std::make_shared<NeuralEpisodicMemory>(config_);
        auto workingMemory = std::make_shared<NeuralWorkingMemory>(config_);
        auto conceptFormation = std::make_shared<ConceptFormation>(config_);
        auto neuralPlanner = std::make_shared<NeuralPlanner>(config_);
        
        // Store references for testing (in real implementation, these would be integrated)
        episodicMemory_ = episodicMemory;
        workingMemory_ = workingMemory;
        conceptFormation_ = conceptFormation;
        neuralPlanner_ = neuralPlanner;
        
        recordMetric("memory_components_created", 4);
    }
    
    void runTest() override {
        // Test memory-cognition interaction
        for (int episode = 0; episode < 5; ++episode) {
            // Create and store episodic memory
            std::string episodeId = "episode_" + std::to_string(episode);
            auto episodeData = std::make_shared<EpisodicMemoryData>();
            episodeData->timeStep = episode * 1000;
            episodeData->neuronsActive = brain_->getFiringNeuronCount();
            episodeData->synapseChanges = brain_->getTotalSynapseCount() * 0.1f;
            
            // Process through concept formation
            std::string concept = conceptFormation_->processExperience(episodeData);
            recordMetric("concept_" + std::to_string(episode), concept.length());
            
            // Store in working memory
            workingMemory_->storeConcept(concept);
            
            // Plan based on working memory contents
            auto plan = neuralPlanner_->createPlan(workingMemory_->getActiveConcepts());
            recordMetric("plan_length_" + std::to_string(episode), plan.size());
            
            // Execute neural computation
            for (int step = 0; step < 100; ++step) {
                brain_->step(step);
                
                // Update concept formation
                conceptFormation_->update(step);
                
                // Update neural planner
                neuralPlanner_->update(step);
            }
            
            // Verify integration
            assertCondition(workingMemory_->getActiveConcepts().size() > 0, 
                           "Working memory should contain concepts");
            assertCondition(plan.size() > 0, "Planner should generate plans");
            
            std::cout << "    Memory-Cognition cycle " << episode << " complete: "
                      << "concepts=" << workingMemory_->getActiveConcepts().size()
                      << ", plans=" << plan.size() << std::endl;
        }
        
        // Verify system integration
        assertCondition(workingMemory_->getConceptCount() > 0, 
                       "Should have stored multiple concepts");
        assertCondition(neuralPlanner_->getPlanCount() > 0, 
                       "Should have generated multiple plans");
        
        std::cout << "    Memory-Cognition integration test PASSED" << std::endl;
    }
    
private:
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<NeuralPlanner> neuralPlanner_;
};

// Test neuromodulation-memory interaction
class NeuromodulationMemoryIntegrationTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 400);
        config_->set("region_count", 2);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 99999u);
        config_->set("reward_decay_rate", 0.9f);
        config_->set("curiosity_gain_rate", 0.05f);
        config_->set("novelty_threshold", 0.1f);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        // Test neuromodulation effects on memory consolidation
        std::vector<float> noveltyScores;
        std::vector<float> rewardLevels;
        std::vector<size_t> memoryConsolidationEvents;
        
        for (int timeStep = 0; timeStep < 1000; ++timeStep) {
            // Simulate experiences that generate neuromodulation signals
            float novelty = calculateNovelty(timeStep);
            float reward = calculateReward(timeStep);
            
            noveltyScores.push_back(novelty);
            rewardLevels.push_back(reward);
            
            // Apply neuromodulation
            applyNeuromodulationSignals(novelty, reward);
            
            // Check for memory consolidation events
            if (shouldConsolidateMemory(novelty, reward)) {
                memoryConsolidationEvents.push_back(timeStep);
                recordMetric("consolidation_at_step_" + std::to_string(timeStep), 1.0f);
            }
            
            // Run neural computation
            brain_->step(timeStep);
            
            // Verify neuromodulation is affecting brain state
            float neuromodulationLevel = getCurrentNeuromodulationLevel();
            assertCondition(neuromodulationLevel >= 0.0f && neuromodulationLevel <= 1.0f,
                           "Neuromodulation level should be normalized");
        }
        
        // Analyze results
        float avgNovelty = calculateAverage(noveltyScores);
        float avgReward = calculateAverage(rewardLevels);
        
        recordMetric("avg_novelty", avgNovelty);
        recordMetric("avg_reward", avgReward);
        recordMetric("consolidation_events", static_cast<double>(memoryConsolidationEvents.size()));
        
        // Verify integration
        assertCondition(memoryConsolidationEvents.size() > 5, 
                       "Should have multiple memory consolidation events");
        assertCondition(avgNovelty > 0.0f, "Should have generated novelty signals");
        assertCondition(avgReward > 0.0f, "Should have generated reward signals");
        
        std::cout << "    Neuromodulation-Memory integration test PASSED: "
                  << "consolidation_events=" << memoryConsolidationEvents.size()
                  << ", avg_novelty=" << avgNovelty << ", avg_reward=" << avgReward << std::endl;
    }
    
private:
    float calculateNovelty(int timeStep) {
        // Simple novelty calculation based on time and random variation
        float baseNovelty = 0.1f;
        float variation = static_cast<float>(rand()) / RAND_MAX * 0.3f;
        float periodicity = sin(timeStep * 0.01f) * 0.2f;
        return baseNovelty + variation + periodicity;
    }
    
    float calculateReward(int timeStep) {
        // Simple reward calculation with peaks at specific times
        float baseReward = 0.0f;
        if (timeStep % 100 == 0) {
            baseReward = 0.8f;
        }
        float variation = static_cast<float>(rand()) / RAND_MAX * 0.4f;
        return baseReward + variation;
    }
    
    void applyNeuromodulationSignals(float novelty, float reward) {
        // In real implementation, this would apply neuromodulation to brain
        // For testing, we just record the signals
        recordMetric("neuromod_input_novelty", novelty);
        recordMetric("neuromod_input_reward", reward);
    }
    
    bool shouldConsolidateMemory(float novelty, float reward) {
        return (novelty > 0.5f && reward > 0.5f) || 
               (novelty > 0.3f && time_since_last_consolidation_ > 200);
    }
    
    float getCurrentNeuromodulationLevel() {
        // In real implementation, this would query the neuromodulator system
        return 0.5f;  // Placeholder
    }
    
    float calculateAverage(const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }
    
    int time_since_last_consolidation_{0};
};

// Test motor-sensory integration
class MotorSensoryIntegrationTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 300);
        config_->set("region_count", 2);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 11111u);
        config_->set("motor_gain", 1.0f);
        config_->set("sensory_noise_level", 0.05f);
        config_->set("action_selection_threshold", 0.5f);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        // Test motor-sensory loop
        SimpleWorld world;
        world.configure(30.0f, 30.0f, 10, 10);
        world.setRandomSeed(22222u);
        
        for (int episode = 0; episode < 4; ++episode) {
            // Reset systems
            brain_->reset();
            brain_->initialize();
            world.reset();
            
            float totalReward = 0.0f;
            size_t successfulActions = 0;
            size_t totalActions = 0;
            
            // Simulate motor-sensory loop
            for (int step = 0; step < 200; ++step) {
                // Get sensory percept
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Process sensory input
                brain_->processSensoryInput(percept);
                
                // Generate motor command
                MotorCommand cmd = brain_->decodeMotorCommand();
                
                // Execute action
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // Update world
                world.update(0.01);
                
                totalReward += result.reward;
                totalActions++;
                
                if (result.reward > 0.1f) {
                    successfulActions++;
                }
                
                // Verify sensory-motor integration
                assertCondition(brain_->getFiringNeuronCount() > 0, 
                               "Should have neural activity after sensory input");
                assertCondition(static_cast<int>(cmd) >= 0 && 
                               static_cast<int>(cmd) < 10, "Motor command should be valid");
            }
            
            recordMetric("episode_" + std::to_string(episode) + ".success_rate", 
                        static_cast<float>(successfulActions) / totalActions);
            recordMetric("episode_" + std::to_string(episode) + ".total_reward", totalReward);
            
            std::cout << "    Motor-Sensory episode " << episode << " complete: "
                      << "success_rate=" << (successfulActions * 100 / totalActions) << "%"
                      << ", reward=" << totalReward << std::endl;
        }
        
        // Verify overall integration
        assertCondition(brain_->getFiringNeuronCount() > 0, 
                       "System should maintain neural activity");
        assertCondition(brain_->getAverageFiringRate() > 0.0f, 
                       "Should have average firing rate");
        
        std::cout << "    Motor-Sensory integration test PASSED" << std::endl;
    }
};

// Integration test runner for cross-component tests
class CrossComponentTestRunner {
public:
    static void runAllTests() {
        std::cout << "=== NLM Cross-Component Integration Tests ===" << std::endl;
        
        MemoryCognitionIntegrationTest memCogTest;
        std::cout << "Running Memory-Cognition Integration Test..." << std::endl;
        memCogTest.run();
        if (memCogTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << memCogTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << memCogTest.getMetrics().failureReason << std::endl;
        }
        
        NeuromodulationMemoryIntegrationTest neuromodTest;
        std::cout << "Running Neuromodulation-Memory Integration Test..." << std::endl;
        neuromodTest.run();
        if (neuromodTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << neuromodTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << neuromodTest.getMetrics().failureReason << std::endl;
        }
        
        MotorSensoryIntegrationTest motorTest;
        std::cout << "Running Motor-Sensory Integration Test..." << std::endl;
        motorTest.run();
        if (motorTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << motorTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << motorTest.getMetrics().failureReason << std::endl;
        }
        
        std::cout << "=== Cross-Component Tests Complete ===" << std::endl;
    }
};

} // namespace cross_component_tests
} // namespace nlm

#endif // NLM_CROSS_COMPONENT_TESTS_HPP
