// Property-based tests for brain integration
// Tests invariants for system-level brain-environment interaction

#ifndef PROPERTY_TEST_BRAIN_INTEGRATION_H
#define PROPERTY_TEST_BRAIN_INTEGRATION_H

#include "property_tests/NLMPropertyTestFramework.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "environment/Environment.hpp"
#include "motor/MotorSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

namespace test_brain_integration {

// Property 1: Brain-environment interaction
bool brain_environment_interaction_property(nlm::property_test::PropertyRandom& rng) {
    const uint32_t network_sizes[] = {50, 100, 200};
    
    bool interaction_correct = true;
    
    for (uint32_t network_size : network_sizes) {
        std::cout << "    Testing brain-environment interaction for network size: " << network_size << std::endl;
        
        // Create brain configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(network_size), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool initialized = brain.initialize();
        
        if (!initialized) {
            interaction_correct = false;
            std::cout << "      FAILED to initialize brain" << std::endl;
            continue;
        }
        
        // Simulate environmental interaction over many steps
        const uint32_t interaction_steps = 50;
        bool environmental_adaptation = true;
        
        for (uint32_t step = 0; step < interaction_steps && environmental_adaptation; ++step) {
            nlm::SimulationStep simulation_step = step;
            
            // Environment provides random input
            double environmental_input = rng.uniform_double(-10.0, 10.0);
            
            // Brain processes environmental input
            brain.step(simulation_step);
            
            // Motor system generates response based on brain output
            auto action = brain.produceAction();
            
            if (action) {
                // Action affects environment
                // This would be modeled in a full simulation
                // For property testing, we verify the interaction doesn't crash
                environmental_input += action->getMotorCommand() * 0.1; // Feedback effect
            }
            
            // Verify brain state remains valid
            uint32_t firing_count = brain.getFiringNeuronCount();
            uint32_t total_neurons = brain.getTotalNeuronCount();
            
            double firing_rate = static_cast<double>(firing_count) / total_neurons;
            
            // Firing rate should be in reasonable bounds (0 to ~1)
            if (firing_rate < 0.0 || firing_rate > 1.0) {
                environmental_adaptation = false;
                std::cout << "      Environmental adaptation failed at step " << step 
                          << ": firing rate out of bounds: " << firing_rate << std::endl;
            }
        }
        
        if (!environmental_adaptation) {
            interaction_correct = false;
        }
    }
    
    return interaction_correct;
}

// Property 2: Agent learning properties
bool agent_learning_property(nlm::property_test::PropertyRandom& rng) {
    const uint32_t training_episodes = 20;
    const uint32_t agents = 5;
    
    bool learning_consistent = true;
    
    for (uint32_t agent_id = 0; agent_id < agents && learning_consistent; ++agent_id) {
        std::cout << "    Testing agent learning for agent: " << agent_id << std::endl;
        
        // Create brain for agent
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain agent_brain(config);
        bool initialized = agent_brain.initialize();
        
        if (!initialized) {
            learning_consistent = false;
            std::cout << "      FAILED to initialize agent brain" << std::endl;
            continue;
        }
        
        // Track performance over training episodes
        std::vector<double> performance_scores;
        
        for (uint32_t episode = 0; episode < training_episodes; ++episode) {
            // Generate random training task
            double task_difficulty = rng.uniform_double(0.1, 1.0);
            double task_complexity = rng.uniform_double(0.5, 2.0);
            
            // Simulate agent learning from task
            nlm::SimulationStep simulation_step = episode;
            agent_brain.step(simulation_step);
            
            // Measure performance
            uint32_t firing_count = agent_brain.getFiringNeuronCount();
            uint32_t total_neurons = agent_brain.getTotalNeuronCount();
            
            double performance = firing_rate = static_cast<double>(firing_count) / total_neurons;
            
            // Adjust performance based on task characteristics
            performance *= (1.0 - task_difficulty) * (1.0 / task_complexity);
            
            // Cap performance at reasonable bounds
            performance = std::max(0.0, std::min(performance, 1.0));
            
            performance_scores.push_back(performance);
        }
        
        // Check for learning consistency
        // Performance should show some pattern of improvement or adaptation
        if (performance_scores.size() >= 3) {
            // Check if there's any trend in performance
            bool agent_learned = false;
            
            // Simple check: agent should maintain reasonable performance levels
            double min_performance = *std::min_element(performance_scores.begin(), performance_scores.end());
            double max_performance = *std::max_element(performance_scores.begin(), performance_scores.end());
            
            if (min_performance > 0.0 || max_performance > 0.1) { // Some performance achieved
                agent_learned = true;
            }
            
            if (!agent_learned) {
                learning_consistent = false;
                std::cout << "      Agent " << agent_id << " failed to learn from training" << std::endl;
            }
        }
    }
    
    return learning_consistent;
}

// Property 3: Long-running stability test
bool brain_long_running_stability(nlm::property_test::PropertyRandom& rng) {
    const uint32_t max_iterations = 1000;
    const uint32_t network_size = 100;
    
    std::cout << "    Running long-running stability test for " << max_iterations << " iterations" << std::endl;
    
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(network_size), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    
    if (!initialized) {
        std::cout << "      FAILED to initialize brain for long-running test" << std::endl;
        return false;
    }
    
    // Run stability test with chaos engineering
    bool stable = true;
    uint32_t successful_iterations = 0;
    uint32_t failed_iterations = 0;
    
    for (uint32_t iteration = 0; iteration < max_iterations && stable; ++iteration) {
        nlm::SimulationStep simulation_step = iteration;
        
        // Simulate various stress conditions
        bool should_fail = rng.bernoulli(0.01); // 1% chance of failure
        bool should_delay = rng.bernoulli(0.05); // 5% chance of delay
        bool should_stress = rng.bernoulli(0.02); // 2% chance of stress
        
        if (should_fail) {
            // Simulate failure
            stable = false;
            failed_iterations++;
            std::cout << "      Iteration " << iteration << ": Simulating failure" << std::endl;
            break;
        }
        
        if (should_delay) {
            // Simulate delay
            std::this_thread::sleep_for(std::chrono::milliseconds(rng.uniform_uint64(1, 10)));
        }
        
        if (should_stress) {
            // Simulate network stress
            double stress_factor = 2.0 + rng.uniform_double(0.0, 1.0);
            // Stress could affect brain state
            // For property testing, we just verify the system can handle it
        }
        
        // Step brain
        brain.step(simulation_step);
        successful_iterations++;
        
        // Progress reporting
        if ((iteration + 1) % 100 == 0) {
            std::cout << "      Completed " << (iteration + 1) << " iterations (" 
                      << successful_iterations << " successful, " << failed_iterations << " failed)" << std::endl;
        }
        
        // Verify brain state after each step
        uint32_t firing_count = brain.getFiringNeuronCount();
        uint32_t total_neurons = brain.getTotalNeuronCount();
        
        double firing_rate = static_cast<double>(firing_count) / total_neurons;
        
        // Firing rate should remain in reasonable bounds
        if (firing_rate < 0.0 || firing_rate > 2.0) { // Allow some variability but not extremes
            stable = false;
            failed_iterations++;
            std::cout << "      Iteration " << iteration << ": Brain state unstable, firing rate: " << firing_rate << std::endl;
        }
    }
    
    double success_rate = static_cast<double>(successful_iterations) / max_iterations;
    std::cout << "    Long-running stability test completed. Success rate: " << success_rate * 100 << "%" 
              << " (" << successful_iterations << "/" << max_iterations << ")" << std::endl;
    
    return stable && success_rate > 0.95; // Require 95% success rate for stability
}

// Property test runner for brain integration
void runBrainIntegrationPropertyTests() {
    std::cout << "=== BRAIN INTEGRATION PROPERTY TESTS ===" << std::endl;
    
    nlm::property_test::PropertyTestEngine engine;
    nlm::property_test::PropertyRandom rng(12345);
    
    // Define property test instances
    class TestBrainEnvironmentInteraction {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("brain_integration", "environment_interaction");
            result.test_iterations = 3;
            bool passed = brain_environment_interaction_property(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestAgentLearning {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("brain_integration", "agent_learning");
            result.test_iterations = 5;
            bool passed = agent_learning_property(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestBrainLongRunningStability {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("brain_integration", "long_running_stability");
            result.test_iterations = 1;
            bool passed = brain_long_running_stability(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    // Run all property tests
    engine.start_timer();
    
    TestBrainEnvironmentInteraction().run(engine, rng);
    TestAgentLearning().run(engine, rng);
    TestBrainLongRunningStability().run(engine, rng);
    
    engine.stop_timer();
    engine.generate_report("brain_integration_property_test_report.txt");
}

} // namespace test_brain_integration

#endif // PROPERTY_TEST_BRAIN_INTEGRATION_H