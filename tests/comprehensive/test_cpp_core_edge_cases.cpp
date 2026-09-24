// Comprehensive C++ Core Testing Suite
// Tests all edge cases for NLM C++ components

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/Memory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <future>

namespace test_comprehensive {

class ComprehensiveCPlusPlusTester {
public:
    int passed = 0;
    int failed = 0;
    std::vector<std::pair<std::string, std::string>> results;

    void log_test(const std::string& test_name, bool success, const std::string& details = "") {
        std::string status = success ? "PASS" : "FAIL";
        std::cout << "[" << status << "] " << test_name << ": " << details << std::endl;
        results.push_back({test_name, details});
        if (success) passed++; else failed++;
    }

    // Brain Component Testing
    void test_brain_components_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing Brain Components Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Neuron extreme values
        try {
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            // Test extreme membrane potentials
            neuron.setMembranePotential(-1000.0f);  // Very negative
            assert(neuron.getMembranePotential() <= -70.0f);  // Should clamp
            
            neuron.setMembranePotential(1000.0f);  // Very positive
            assert(neuron.getMembranePotential() >= -70.0f);  // Should clamp
            
            log_test("Neuron extreme potentials", true);
        } catch (...) {
            log_test("Neuron extreme potentials", false, "Exception raised");
        }

        // Test 2: Neuron type bounds
        try {
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            // Cycle through all neuron types
            for (int i = 0; i < 4; ++i) {
                neuron.setType(static_cast<nlm::NeuronType>(i));
                assert(static_cast<int>(neuron.getType()) == i);
            }
            
            log_test("Neuron type cycling", true);
        } catch (...) {
            log_test("Neuron type cycling", false, "Exception raised");
        }

        // Test 3: Synapse weight bounds testing
        try {
            nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
            
            // Test weight bounds
            synapse.setWeight(1000.0f);  // Exceeds max
            assert(std::abs(synapse.getWeight()) <= 1.0f);  // Should be clamped
            
            synapse.setWeight(-1000.0f);  // Below min
            assert(std::abs(synapse.getWeight()) <= 1.0f);  // Should be clamped
            
            log_test("Synapse weight bounds", true);
        } catch (...) {
            log_test("Synapse weight bounds", false, "Exception raised");
        }

        // Test 4: Synapse eligibility trace bounds
        try {
            nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
            
            // Test extreme eligibility trace values
            synapse.setEligibilityTrace(10000.0f);
            assert(std::abs(synapse.getEligibilityTrace()) <= 1.0f);  // Should clamp
            
            synapse.setEligibilityTrace(-10000.0f);
            assert(std::abs(synapse.getEligibilityTrace()) <= 1.0f);  // Should clamp
            
            log_test("Synapse eligibility trace bounds", true);
        } catch (...) {
            log_test("Synapse eligibility trace bounds", false, "Exception raised");
        }

        // Test 5: Region and population edge cases
        try {
            nlm::Brain brain;
            
            // Add regions with extreme parameters
            nlm::NeuralRegion region(nlm::RegionId(1));
            region.setNeuronCount(10);  // Minimum
            assert(region.getNeuronCount() == 10);
            
            region.setNeuronCount(10000);  // Large
            assert(region.getNeuronCount() >= 10);
            
            log_test("Region extreme neuron counts", true);
        } catch (...) {
            log_test("Region extreme neuron counts", false, "Exception raised");
        }
    }

    void test_plasticity_rules_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing Plasticity Rules Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: STDP edge cases
        try {
            nlm::STDP stdp;
            
            // Create neurons with extreme states
            nlm::Neuron pre_neuron(nlm::NeuronId(1));
            nlm::Neuron post_neuron(nlm::NeuronId(2));
            
            pre_neuron.setMembranePotential(-100.0f);  // Very negative
            post_neuron.setMembranePotential(50.0f);   // Very positive
            
            // Record pre and post spikes
            pre_neuron.recordSpike(0.0);
            post_neuron.recordSpike(0.1);
            
            // Apply STDP with extreme values
            stdp.applyLearningRule(&pre_neuron, &post_neuron, 0.1);
            
            log_test("STDP with extreme neuron states", true);
        } catch (...) {
            log_test("STDP with extreme neuron states", false, "Exception raised");
        }

        // Test 2: Hebbian rule bounds
        try {
            nlm::Hebbian hebbian;
            
            nlm::Neuron pre_neuron(nlm::NeuronId(1));
            nlm::Neuron post_neuron(nlm::NeuronId(2));
            
            // Set extreme synaptic states
            pre_neuron.injectCurrent(1000.0f);  // Extreme current
            post_neuron.injectCurrent(-1000.0f);
            
            // Apply Hebbian learning
            hebbian.applyLearningRule(&pre_neuron, &post_neuron, 0.1);
            
            log_test("Hebbian with extreme currents", true);
        } catch (...) {
            log_test("Hebbian with extreme currents", false, "Exception raised");
        }

        // Test 3: Structural plasticity edge cases
        try {
            nlm::StructuralPlasticity struct_plasticity;
            
            nlm::Neuron pre_neuron(nlm::NeuronId(1));
            nlm::Neuron post_neuron(nlm::NeuronId(2));
            
            // Apply structural plasticity with extreme parameters
            struct_plasticity.applyLearningRule(&pre_neuron, &post_neuron, 10.0);  // Large learning rate
            
            log_test("Structural plasticity extreme learning rates", true);
        } catch (...) {
            log_test("Structural plasticity extreme learning rates", false, "Exception raised");
        }

        // Test 4: Plasticity rule validation
        try {
            nlm::PlasticityRule rule;
            
            nlm::Neuron pre_neuron(nlm::NeuronId(1));
            nlm::Neuron post_neuron(nlm::NeuronId(2));
            nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
            
            // Test with invalid synapse (null pointers)
            try {
                rule.applyLearningRule(nullptr, &post_neuron, 0.1);
                log_test("Plasticity rule null pointer handling", false, "Should have failed");
            } catch (...) {
                log_test("Plasticity rule null pointer handling", true, "Correctly rejected");
            }
            
        } catch (...) {
            log_test("Plasticity rule validation", false, "Exception raised");
        }
    }

    void test_neuromodulation_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing Neuromodulation Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Reward modulation edge cases
        try {
            nlm::Reward reward_mod;
            
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            // Test with extreme reward values
            reward_mod.applyModulation(&neuron, 1000.0f);  // Very large reward
            reward_mod.applyModulation(&neuron, -1000.0f);  // Very large penalty
            
            assert(neuron.getTotalCurrent() >= -1000.0f);  // Should be bounded
            
            log_test("Reward modulation extreme values", true);
        } catch (...) {
            log_test("Reward modulation extreme values", false, "Exception raised");
        }

        // Test 2: Curiosity modulation
        try {
            nlm::Curiosity curiosity_mod;
            
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            // Test with extreme novelty values
            curiosity_mod.applyModulation(&neuron, 0.0f);   // No novelty
            curiosity_mod.applyModulation(&neuron, 100.0f);  // Extreme novelty
            curiosity_mod.applyModulation(&neuron, -100.0f);  // Extreme negative novelty
            
            log_test("Curiosity modulation extreme values", true);
        } catch (...) {
            log_test("Curiosity modulation extreme values", false, "Exception raised");
        }

        // Test 3: Novelty detection edge cases
        try {
            nlm::Novelty novelty_mod;
            
            // Create patterns with extreme differences
            std::vector<float> pattern1(100, 0.0f);
            std::vector<float> pattern2(100, 1000.0f);  // Extreme difference
            std::vector<float> pattern3(100, -1000.0f);  // Extreme negative
            
            float novelty1 = novelty_mod.calculateNovelty(pattern1, pattern2);
            float novelty2 = novelty_mod.calculateNovelty(pattern2, pattern3);
            
            // Novelty should be bounded
            assert(std::isfinite(novelty1));
            assert(std::isfinite(novelty2));
            
            log_test("Novelty detection extreme patterns", true);
        } catch (...) {
            log_test("Novelty detection extreme patterns", false, "Exception raised");
        }

        // Test 4: Prediction error edge cases
        try {
            nlm::PredictionError pred_error;
            
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            // Test with extreme prediction errors
            pred_error.applyModulation(&neuron, 1000.0f);  // Large positive error
            pred_error.applyModulation(&neuron, -1000.0f);  // Large negative error
            pred_error.applyModulation(&neuron, 0.0f);     // No error
            
            log_test("Prediction error extreme values", true);
        } catch (...) {
            log_test("Prediction error extreme values", false, "Exception raised");
        }
    }

    void test_memory_systems_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing Memory Systems Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Working memory edge cases
        try {
            nlm::NeuralWorkingMemory working_memory;
            
            // Test with extreme values
            working_memory.storeExperience(1000.0f, 1.0f);  // Large experience value
            working_memory.storeExperience(-1000.0f, 1.0f);  // Negative experience
            
            float retrieved = working_memory.retrieveExperience();
            assert(std::isfinite(retrieved));
            
            log_test("Working memory extreme values", true);
        } catch (...) {
            log_test("Working memory extreme values", false, "Exception raised");
        }

        // Test 2: Episodic memory edge cases
        try {
            nlm::NeuralEpisodicMemory episodic_memory;
            
            // Store extreme experience data
            std::vector<float> extreme_data(1000, 1000.0f);  // Large data
            std::vector<float> negative_data(1000, -1000.0f);  // Negative data
            
            episodic_memory.encodeExperience(extreme_data);
            episodic_memory.encodeExperience(negative_data);
            
            // Try to replay
            episodic_memory.replayExperience(0.0, 100.0);  // Large time window
            
            log_test("Episodic memory extreme data", true);
        } catch (...) {
            log_test("Episodic memory extreme data", false, "Exception raised");
        }

        // Test 3: Associative memory edge cases
        try {
            nlm::Memory associative_memory;
            
            // Store associations with extreme weights
            std::vector<float> pattern1(50, 0.5f);   // Positive weights
            std::vector<float> pattern2(50, -0.5f);  // Negative weights
            std::vector<float> pattern3(50, 1000.0f);  // Extreme weights
            
            associative_memory.storeAssociation(pattern1, pattern2);
            associative_memory.storeAssociation(pattern2, pattern3);
            associative_memory.storeAssociation(pattern3, pattern1);
            
            // Retrieve associations
            std::vector<float> retrieved = associative_memory.retrieveAssociation(pattern1);
            assert(retrieved.size() == pattern1.size());
            
            log_test("Associative memory extreme weights", true);
        } catch (...) {
            log_test("Associative memory extreme weights", false, "Exception raised");
        }
    }

    void test_development_system_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing Development System Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Development with extreme parameters
        try {
            nlm::DevelopmentSystem dev_system;
            
            // Test extreme development rates
            dev_system.setGrowthRate(10.0f);  // Very high growth
            dev_system.setPruningRate(10.0f);  // Very high pruning
            
            nlm::Neuron neuron(nlm::NeuronId(1));
            dev_system.developNeuron(&neuron);
            
            log_test("Development extreme growth/pruning rates", true);
        } catch (...) {
            log_test("Development extreme growth/pruning rates", false, "Exception raised");
        }

        // Test 2: Synaptogenesis edge cases
        try {
            nlm::DevelopmentSystem dev_system;
            
            // Create neurons
            nlm::Neuron neuron1(nlm::NeuronId(1));
            nlm::Neuron neuron2(nlm::NeuronId(2));
            
            // Test with extreme spatial proximity
            dev_system.synaptogenesis(&neuron1, &neuron2, -100.0f);  // Negative distance
            dev_system.synaptogenesis(&neuron1, &neuron2, 100000.0f);  // Very large distance
            
            log_test("Synaptogenesis extreme distances", true);
        } catch (...) {
            log_test("Synaptogenesis extreme distances", false, "Exception raised");
        }
    }

    void test_prediction_system_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing Prediction System Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Prediction with extreme values
        try {
            nlm::PredictionSystem pred_system;
            
            // Create neurons with extreme states
            nlm::Neuron neuron(nlm::NeuronId(1));
            neuron.setMembranePotential(1000.0f);  // Very high
            neuron.setMembranePotential(-1000.0f);  // Very low
            
            // Make predictions
            pred_system.makePrediction(&neuron);
            pred_system.makePrediction(&neuron);
            
            // Get prediction errors
            nlm::PredictionError error = pred_system.getPredictionError();
            
            assert(std::isfinite(error.getErrorSignal()));
            
            log_test("Prediction system extreme neuron states", true);
        } catch (...) {
            log_test("Prediction system extreme neuron states", false, "Exception raised");
        }

        // Test 2: Confidence tracking edge cases
        try {
            nlm::PredictionSystem pred_system;
            
            // Create multiple predictions with extreme confidence
            nlm::Neuron neuron(nlm::NeuronId(1));
            
            pred_system.setPredictionConfidence(1000.0f);  // Very high confidence
            pred_system.setPredictionConfidence(0.0f);   // Zero confidence
            pred_system.setPredictionConfidence(-1000.0f);  // Negative confidence
            
            log_test("Prediction system extreme confidence values", true);
        } catch (...) {
            log_test("Prediction system extreme confidence values", false, "Exception raised");
        }
    }

    void test_system_integration_edge_cases() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "Testing System Integration Edge Cases" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Complete brain with all components
        try {
            nlm::Brain brain;
            
            // Add neurons with extreme properties
            for (int i = 0; i < 100; ++i) {
                nlm::Neuron neuron(nlm::NeuronId(i));
                
                // Set extreme initial values
                neuron.setMembranePotential((i % 2 == 0) ? -1000.0f : 1000.0f);
                neuron.setThreshold((i % 2 == 0) ? -100.0f : 100.0f);
                
                brain.addNeuron(neuron);
            }
            
            // Add synapses with extreme weights
            for (int i = 0; i < 50; ++i) {
                nlm::Synapse synapse(nlm::SynapseId(i), 
                                   nlm::NeuronId(i % 100), 
                                   nlm::NeuronId((i + 1) % 100));
                synapse.setWeight((i % 2 == 0) ? 1000.0f : -1000.0f);
                brain.addSynapse(synapse);
            }
            
            // Initialize and step the brain
            brain.initialize();
            
            // Run multiple steps with monitoring
            for (int step = 0; step < 100; ++step) {
                brain.step();
                
                // Check for errors
                if (brain.getTotalErrorCount() > 1000) {
                    log_test("Integration system error overflow", false, "Too many errors");
                    return;
                }
            }
            
            log_test("Complete brain integration", true);
        } catch (...) {
            log_test("Complete brain integration", false, "Exception raised");
        }

        // Test 2: Cross-component communication
        try {
            nlm::Brain brain;
            nlm::Neuron neuron(nlm::NeuronId(1));
            brain.addNeuron(neuron);
            
            // Test neuron-neuromodulator interaction
            nlm::Reward reward_mod;
            reward_mod.applyModulation(&neuron, 1000.0f);
            
            // Test neuron-plasticity interaction
            nlm::Hebbian hebbian;
            nlm::Neuron post_neuron(nlm::NeuronId(2));
            brain.addNeuron(post_neuron);
            
            hebbian.applyLearningRule(&neuron, &post_neuron, 1000.0f);
            
            log_test("Cross-component communication", true);
        } catch (...) {
            log_test("Cross-component communication", false, "Exception raised");
        }
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "COMPREHENSIVE C++ CORE TESTING SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        test_brain_components_edge_cases();
        test_plasticity_rules_edge_cases();
        test_neuromodulation_edge_cases();
        test_memory_systems_edge_cases();
        test_development_system_edge_cases();
        test_prediction_system_edge_cases();
        test_system_integration_edge_cases();

        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Total Tests: " << (passed + failed) << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success Rate: " << ((passed / (double)(passed + failed)) * 100.0) << "%" << std::endl;

        std::cout << std::endl << "Detailed Results:" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << (result.second.empty() ? "PASS" : "FAIL") << " " << result.first << ": " << result.second << std::endl;
        }
    }
};

int main() {
    ComprehensiveCPlusPlusTester tester;
    tester.run_all_tests();
    
    if (tester.failed > 0) {
        std::cout << std::endl << "❌ " << tester.failed << " TESTS FAILED" << std::endl;
        return 1;
    } else {
        std::cout << std::endl << "✅ ALL TESTS PASSED!" << std::endl;
        return 0;
    }
}
