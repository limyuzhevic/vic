// Regression Testing Suite
// Tests historical behavior, API compatibility, feature regression, and performance regression

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <map>
#include <cmath>

namespace test_regression {

class RegressionTester {
public:
    struct RegressionTestResult {
        std::string test_name;
        bool passed;
        std::string details;
        double performance_baseline_ms;
        double performance_current_ms;
        double performance_regression_percent;
    };

    std::vector<RegressionTestResult> results;

    void log_result(const std::string& test_name, bool passed, const std::string& details,
                   double baseline_ms = 0.0, double current_ms = 0.0) {
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << ": " << details << std::endl;
        
        double regression_percent = 0.0;
        if (baseline_ms > 0.0 && current_ms > 0.0) {
            regression_percent = ((current_ms - baseline_ms) / baseline_ms) * 100.0;
        }
        
        results.push_back({test_name, passed, details, baseline_ms, current_ms, regression_percent});
    }

    void test_historical_behavior() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Historical Behavior" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Neuron behavior consistency
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Create neurons with historical parameters
        std::vector<nlm::Neuron> neurons;
        for (int i = 0; i < 100; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            
            // Historical default values
            neuron.setMembranePotential(-70.0f); // Historical default
            neuron.setThreshold(-55.0f); // Historical default
            neuron.setRefractoryPeriod(5); // Historical default
            
            neurons.push_back(neuron);
        }
        
        // Apply historical operations
        for (int step = 0; step < 50; ++step) {
            for (auto& neuron : neurons) {
                neuron.injectCurrent(1.0f);
                neuron.recordSpike(step * 0.1f);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Verify expected behavior
        bool behavior_consistent = true;
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (neurons[i].getMembranePotential() < -100.0f || neurons[i].getMembranePotential() > 0.0f) {
                behavior_consistent = false;
                break;
            }
        }
        
        log_result("Neuron historical behavior", behavior_consistent, 
                 std::to_string(duration.count()) + "ms, " + std::to_string(neurons.size()) + " neurons processed",
                 duration.count(), duration.count());

        // Test 2: Synapse behavior consistency
        start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<nlm::Synapse> synapses;
        for (int i = 0; i < 50; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
            
            // Historical defaults
            synapse.setWeight(0.5f); // Historical default
            synapse.setDelay(1); // Historical default
            synapse.setEfficacy(1.0f); // Historical default
            
            synapses.push_back(synapse);
        }
        
        // Apply historical synaptic operations
        for (auto& synapse : synapses) {
            synapse.recordPreSpike(0.0f);
            synapse.recordPostSpike(0.1f);
            synapse.setEligibilityTrace(0.3f);
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Verify expected synaptic behavior
        bool synaptic_behavior_consistent = true;
        for (size_t i = 0; i < synapses.size(); ++i) {
            if (std::abs(synapses[i].getWeight() - 0.5f) > 0.001f ||
                synapses[i].getDelay() != 1 ||
                std::abs(synapses[i].getEfficacy() - 1.0f) > 0.001f) {
                synaptic_behavior_consistent = false;
                break;
            }
        }
        
        log_result("Synapse historical behavior", synaptic_behavior_consistent,
                 std::to_string(duration.count()) + "ms, " + std::to_string(synapses.size()) + " synapses processed",
                 duration.count(), duration.count());
    }

    void test_api_compatibility() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing API Compatibility" << std::endl;
        cout << std::string(70, '=') << std::endl;

        // Test 1: Core API methods still available
        bool api_methods_present = true;
        
        try {
            nlm::Brain brain;
            
            // Test all core methods exist and work
            brain.initialize();
            brain.step();
            brain.getTotalNeuronCount();
            brain.getTotalErrorCount();
            brain.saveState();
            brain.loadState("");
            
        } catch (...) {
            api_methods_present = false;
        }
        
        log_result("Core API methods present", api_methods_present, "All essential methods functional",
                 0.0, 0.0);

        // Test 2: Configuration API compatibility
        bool config_api_compatible = true;
        
        try {
            nlm::Config config;
            
            // Historical configuration methods
            config.set("brain.neuron_count", 100);
            config.get("brain.neuron_count");
            config.loadFromFile("");
            config.saveToFile("");
            config.validate_key("test");
            config.validate_value_type("test", "int");
            
        } catch (...) {
            config_api_compatible = false;
        }
        
        log_result("Configuration API compatibility", config_api_compatible,
                 "Historical configuration methods preserved",
                 0.0, 0.0);

        // Test 3: Agent API compatibility
        bool agent_api_compatible = true;
        
        try {
            nlm::AgentBrain agent_brain("TestAgent");
            nlm::AgentBody agent_body("TestBody");
            
            // Core agent methods
            agent_brain.initialize();
            agent_brain.step();
            agent_brain.receiveSensoryInput(nlm::SensoryPercept());
            agent_brain.sendAction(nlm::Action());
            
            agent_body.initialize();
            agent_body.step();
            agent_body.receiveAction(nlm::Action());
            agent_body.setBrain(&agent_brain);
            
        } catch (...) {
            agent_api_compatible = false;
        }
        
        log_result("Agent API compatibility", agent_api_compatible,
                 "Historical agent APIs maintained",
                 0.0, 0.0);
    }

    void test_feature_regression() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Feature Regression" << std::endl;
        std::string(70, '=') << std::endl;

        // Test 1: Plasticity rules regression
        bool plasticity_regression_detected = false;
        
        try {
            nlm::STDP stdp;
            nlm::Hebbian hebbian;
            nlm::StructuralPlasticity struct_plasticity;
            
            nlm::Neuron pre_neuron(nlm::NeuronId(1));
            nlm::Neuron post_neuron(nlm::NeuronId(2));
            
            // Apply all plasticity rules
            stdp.applyLearningRule(&pre_neuron, &post_neuron, 0.1f);
            hebbian.applyLearningRule(&pre_neuron, &post_neuron, 0.1f);
            struct_plasticity.applyLearningRule(&pre_neuron, &post_neuron, 0.1f);
            
        } catch (...) {
            plasticity_regression_detected = true;
        }
        
        log_result("Plasticity rules functionality", !plasticity_regression_detected,
                 "All plasticity rule systems operational",
                 0.0, 0.0);

        // Test 2: Neuromodulation regression
        bool neuromodulation_regression_detected = false;
        
        try {
            nlm::Reward reward_mod;
            nlm::Curiosity curiosity_mod;
            nlm::Novelty novelty_mod;
            
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            // Apply all neuromodulators
            reward_mod.applyModulation(&neuron, 1.0f);
            curiosity_mod.applyModulation(&neuron, 1.0f);
            novelty_mod.applyModulation(&neuron, 1.0f);
            
        } catch (...) {
            neuromodulation_regression_detected = true;
        }
        
        log_result("Neuromodulation functionality", !neuromodulation_regression_detected,
                 "All neuromodulation systems operational",
                 0.0, 0.0);

        // Test 3: Memory systems regression
        bool memory_regression_detected = false;
        
        try {
            nlm::NeuralWorkingMemory working_memory;
            nlm::NeuralEpisodicMemory episodic_memory;
            nlm::Memory associative_memory;
            
            // Test memory operations
            working_memory.storeExperience(1.0f, 1.0f);
            episodic_memory.encodeExperience(std::vector<float>(10, 1.0f));
            associative_memory.storeAssociation(
                std::vector<float>(10, 0.5f),
                std::vector<float>(10, 0.5f)
            );
            
        } catch (...) {
            memory_regression_detected = true;
        }
        
        log_result("Memory systems functionality", !memory_regression_detected,
                 "All memory system operations functional",
                 0.0, 0.0);
    }

    void test_performance_regression() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Performance Regression" << std::endl;
        cout << std::string(70, '=') << std::endl;

        // Test 1: Brain performance regression
        auto start_time = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain;
        for (int i = 0; i < 100; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            brain.addNeuron(neuron);
        }
        
        brain.initialize();
        
        for (int step = 0; step < 50; ++step) {
            brain.step();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto current_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        // Baseline performance (would normally be stored from previous runs)
        double baseline_duration = 100.0; // 100ms baseline
        double performance_regression = ((current_duration - baseline_duration) / baseline_duration) * 100.0;
        
        bool performance_acceptable = (performance_regression < 50.0); // Less than 50% regression
        
        log_result("Brain performance", performance_acceptable,
                 std::to_string(current_duration) + "ms (" + std::to_string(performance_regression) + "% regression)",
                 baseline_duration, current_duration);

        // Test 2: Agent performance regression
        start_time = std::chrono::high_resolution_clock::now();
        
        nlm::AgentBrain agent_brain("PerformanceTest");
        nlm::AgentBody agent_body("BodyTest");
        
        agent_brain.initialize();
        agent_body.initialize();
        
        for (int step = 0; step < 50; ++step) {
            agent_brain.step();
            agent_body.step();
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        current_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        baseline_duration = 150.0; // 150ms baseline
        performance_regression = ((current_duration - baseline_duration) / baseline_duration) * 100.0;
        
        performance_acceptable = (performance_regression < 50.0);
        
        log_result("Agent performance", performance_acceptable,
                 std::to_string(current_duration) + "ms (" + std::to_string(performance_regression) + "% regression)",
                 baseline_duration, current_duration);

        // Test 3: Memory performance regression
        start_time = std::chrono::high_resolution_clock::now();
        
        nlm::NeuralWorkingMemory working_memory;
        for (int i = 0; i < 1000; ++i) {
            working_memory.storeExperience((float)i, (float)i * 0.1f);
        }
        
        for (int i = 0; i < 100; ++i) {
            working_memory.retrieveExperience();
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        current_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        baseline_duration = 50.0; // 50ms baseline
        performance_regression = ((current_duration - baseline_duration) / baseline_duration) * 100.0;
        
        performance_acceptable = (performance_regression < 100.0); // Less than 100% regression
        
        log_result("Memory performance", performance_acceptable,
                 std::to_string(current_duration) + "ms (" + std::to_string(performance_regression) + "% regression)",
                 baseline_duration, current_duration);
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "REGRESSION TESTING SUITE" << std::endl;
        std::string(80, '=') << std::endl;

        test_historical_behavior();
        test_api_compatibility();
        test_feature_regression();
        test_performance_regression();

        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::string(80, '=') << std::endl;
        
        int passed = 0, failed = 0;
        double total_regression = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            total_regression += result.performance_regression_percent;
        }
        
        std::cout << "Total Tests: " << (passed + failed) << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Overall Performance Regression: " << total_regression << "%" << std::endl;

        std::cout << std::endl << "Detailed Results:" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << (result.passed ? "PASS" : "FAIL") << " " << result.test_name << ": " << result.details << std::endl;
            if (result.performance_regression_percent > 0) {
                std::cout << "    Performance regression: " << std::fixed << std::setprecision(1)
                          << result.performance_regression_percent << "%" << std::endl;
            }
        }
    }
};

int main() {
    RegressionTester tester;
    tester.run_all_tests();
    
    return 0;
}
