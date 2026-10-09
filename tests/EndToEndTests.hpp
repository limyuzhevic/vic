// End-to-End Simulation Tests
// 
// These tests verify complete neural network simulations from initialization to completion,
// including the full agent lifecycle and interaction with the environment.

#ifndef NLM_END_TO_END_TESTS_HPP
#define NLM_END_TO_END_TESTS_HPP

#include "IntegrationTestFramework.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <vector>
#include <iostream>

namespace nlm {
namespace end_to_end_tests {

// Test complete neural network simulation from initialization to completion
class CompleteSimulationTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 1000);
        config_->set("region_count", 3);
        config_->set("connection_probability", 0.08f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 12345u);
        config_->set("stdp_ltp_weight", 0.015f);
        config_->set("stdp_ltd_weight", 0.012f);
        config_->set("synaptogenesis_rate", 0.00005f);
        config_->set("pruning_rate", 0.000005f);
    }
    
    void setupSystem() override {
        // Initialize brain
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
        
        // Create environment
        SimpleWorld world;
        world.configure(50.0f, 50.0f, 20, 20);
        world.setRandomSeed(999);
        
        // Create agent brain interface
        agentBrain_ = std::make_shared<AgentBrain>(brain_);
        agentBrain_->initialize(world);
        
        // Enable key features
        agentBrain_->enableRewardModulation(true);
        agentBrain_->enableStructuralPlasticity(true);
        agentBrain_->enableDevelopment(true);
        agentBrain_->enableCuriosity(true);
    }
    
    void runTest() override {
        // Capture initial state
        SystemStateSnapshot initialState;
        initialState.capture(*brain_);
        
        // Run simulation for multiple episodes
        const int episodes = 3;
        const int stepsPerEpisode = 100;
        
        for (int episode = 0; episode < episodes; ++episode) {
            // Reset for new episode
            brain_->reset();
            brain_->initialize();
            SimpleWorld world;
            world.configure(50.0f, 50.0f, 20, 20);
            world.reset();
            
            // Create fresh agent brain
            AgentBrain episodeAgentBrain(brain_);
            episodeAgentBrain.initialize(world);
            
            float totalReward = 0.0f;
            size_t totalSpikes = 0;
            size_t totalActions = 0;
            
            // Simulate episode
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Get sensory input
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // Process sensory input
                episodeAgentBrain.processSensoryInput(percept);
                
                // Run neural computation
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain_->step(neuralStep);
                }
                
                // Generate and apply action
                MotorCommand cmd = episodeAgentBrain.decodeMotorCommand();
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // Apply neuromodulation
                episodeAgentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // Update system
                world.update(0.01);
                episodeAgentBrain.updateDevelopment(0.01);
                
                totalReward += result.reward;
                totalSpikes += brain_->getFiringNeuronCount();
                totalActions++;
                
                // Verify system state
                assertCondition(brain_->getTotalNeuronCount() == 1000, 
                               "Neuron count should remain constant");
                assertCondition(brain_->getTotalSynapseCount() > 0, 
                               "Should have synapses after initialization");
                assertCondition(episodeAgentBrain.getNeuromodulationLevel() >= 0.0f, 
                               "Neuromodulation level should be non-negative");
            }
            
            recordMetric("episode_" + std::to_string(episode) + ".total_reward", totalReward);
            recordMetric("episode_" + std::to_string(episode) + ".total_spikes", totalSpikes);
            recordMetric("episode_" + std::to_string(episode) + ".total_actions", totalActions);
            
            // Verify post-episode state
            assertCondition(brain_->getTotalNeuronCount() == 1000, 
                           "Neuron count should persist across episodes");
            assertCondition(brain_->getTotalSynapseCount() >= initialState.totalSynapseCount, 
                           "Should have at least initial synapses");
            
            std::cout << "    Episode " << episode << " complete: reward=" << totalReward 
                      << ", spikes=" << totalSpikes << ", actions=" << totalActions << std::endl;
        }
        
        // Verify final system state
        assertCondition(brain_->getTotalNeuronCount() == 1000, 
                       "Final neuron count should match initial");
        assertCondition(brain_->getTotalSynapseCount() >= 500, 
                       "Should have accumulated synapses through learning");
        assertCondition(brain_->getFiringNeuronCount() > 0, 
                       "System should show neural activity");
        
        std::cout << "    Complete simulation test PASSED" << std::endl;
    }
};

// Test neural development over time
class NeuralDevelopmentTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 800);
        config_->set("region_count", 2);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 55555u);
        config_->set("synaptogenesis_rate", 0.0001f);
        config_->set("pruning_rate", 0.00001f);
        config_->set("maturation_rate", 0.01f);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
        
        // Setup development system
        brain_->enableDevelopment(true);
    }
    
    void runTest() override {
        // Take initial development snapshot
        SystemStateSnapshot initialState;
        initialState.capture(*brain_);
        
        // Simulate development over time
        for (int day = 0; day < 30; ++day) {
            // Run neural simulation steps
            for (int step = 0; step < 1000; ++step) {
                brain_->step(step);
                brain_->updateDevelopment(0.001);
            }
            
            // Check developmental progress
            if (brain_->getTotalSynapseCount() > initialState.totalSynapseCount) {
                recordMetric("development_day_" + std::to_string(day) + ".new_synapses", 
                           brain_->getTotalSynapseCount() - initialState.totalSynapseCount);
            }
            
            // Verify development
            assertCondition(brain_->getTotalNeuronCount() == 800, 
                           "Neuron count should remain stable");
            assertCondition(brain_->getTotalSynapseCount() > 0, 
                           "Development should create synapses");
            
            std::cout << "    Development day " << day 
                      << ": neurons=" << brain_->getTotalNeuronCount()
                      << ", synapses=" << brain_->getTotalSynapseCount()
                      << ", firing=" << brain_->getFiringNeuronCount() << std::endl;
        }
        
        // Verify significant development occurred
        assertCondition(brain_->getTotalSynapseCount() > initialState.totalSynapseCount * 1.5, 
                       "Development should increase synapses by at least 50%");
        
        std::cout << "    Neural development test PASSED" << std::endl;
    }
};

// Test plasticity and learning capabilities
class PlasticityLearningTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 500);
        config_->set("region_count", 1);
        config_->set("connection_probability", 0.12f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 77777u);
        config_->set("stdp_ltp_weight", 0.02f);
        config_->set("stdp_ltd_weight", 0.018f);
        config_->set("learning_rate", 0.01f);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        // Record initial state
        SystemStateSnapshot initialState;
        initialState.capture(*brain_);
        
        // Run learning experiments
        const int learningSteps = 2000;
        float learningProgress = 0.0f;
        
        for (int step = 0; step < learningSteps; ++step) {
            // Create learning input pattern
            for (size_t i = 0; i < std::min(size_t(20), brain_->getTotalNeuronCount()); ++i) {
                brain_->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            }
            
            // Run neural computation
            brain_->step(step);
            
            // Record learning progress
            if (step % 500 == 0) {
                float currentProgress = static_cast<float>(brain_->getTotalSynapseCount()) / 
                                       static_cast<float>(initialState.totalSynapseCount);
                recordMetric("step_" + std::to_string(step) + ".progress", currentProgress);
            }
            
            // Verify learning conditions
            assertCondition(brain_->getTotalNeuronCount() == 500, 
                           "Neuron count should remain constant during learning");
        }
        
        // Verify learning occurred
        float finalProgress = static_cast<float>(brain_->getTotalSynapseCount()) / 
                             static_cast<float>(initialState.totalSynapseCount);
        assertCondition(finalProgress > 1.1f, 
                       "Learning should increase synapses by at least 10%");
        
        // Verify plasticity rules are working
        assertCondition(brain_->getAverageFiringRate() > 0.0f, 
                       "Neurons should be firing during learning");
        
        std::cout << "    Plasticity and learning test PASSED: " 
                  << "progress=" << (finalProgress - 1.0f) * 100 << "% increase" << std::endl;
    }
};

// Integration test runner for end-to-end tests
class EndToEndTestRunner {
public:
    static void runAllTests() {
        std::cout << "=== NLM End-to-End Integration Tests ===" << std::endl;
        
        CompleteSimulationTest simTest;
        std::cout << "Running Complete Simulation Test..." << std::endl;
        simTest.run();
        if (simTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << simTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << simTest.getMetrics().failureReason << std::endl;
        }
        
        NeuralDevelopmentTest devTest;
        std::cout << "Running Neural Development Test..." << std::endl;
        devTest.run();
        if (devTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << devTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << devTest.getMetrics().failureReason << std::endl;
        }
        
        PlasticityLearningTest plasticTest;
        std::cout << "Running Plasticity and Learning Test..." << std::endl;
        plasticTest.run();
        if (plasticTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << plasticTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << plasticTest.getMetrics().failureReason << std::endl;
        }
        
        std::cout << "=== End-to-End Tests Complete ===" << std::endl;
    }
};

} // namespace end_to_end_tests
} // namespace nlm

#endif // NLM_END_TO_END_TESTS_HPP
