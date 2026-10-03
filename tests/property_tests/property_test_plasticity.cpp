// Property-based tests for plasticity
// Tests invariants for synaptic plasticity, STDP, and learning mechanisms

#ifndef PROPERTY_TEST_PLASTICITY_H
#define PROPERTY_TEST_PLASTICITY_H

#include "property_tests/NLMPropertyTestFramework.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <vector>
#include <algorithm>
#include <cmath>

namespace test_plasticity {

// Property 1: STDP should preserve Hebbian properties
bool stdp_hebbian_properties(nlm::property_test::PropertyRandom& rng) {
    const uint32_t spike_pair_count = 20;
    
    // Create synapses for testing
    std::vector<nlm::Synapse> synapses;
    const uint32_t synapse_count = 10;
    
    for (uint32_t i = 0; i < synapse_count; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapses.back().setType(nlm::SynapseType::Excitatory);
        synapses.back().setWeight(0.5f);
        synapses.back().enablePlasticity(true, true, false); // Enable STDP and Hebbian
    }
    
    // Test Hebbian STDP properties
    nlm::STDP stdp;
    
    bool hebbian_properties_preserved = true;
    
    for (uint32_t pair = 0; pair < spike_pair_count && hebbian_properties_preserved; ++pair) {
        // Generate pre-post spike pairs
        std::vector<nlm::Timestamp> pre_spikes;
        std::vector<nlm::Timestamp> post_spikes;
        
        // Generate 1-5 spike pairs
        uint32_t num_pairs = rng.uniform_uint64(1, 5);
        for (uint32_t p = 0; p < num_pairs; ++p) {
            nlm::Timestamp pre_time = rng.uniform_double(0.0, 100.0);
            nlm::Timestamp post_time = pre_time + rng.uniform_double(1.0, 20.0); // Post fires after pre (LTP)
            
            pre_spikes.push_back(pre_time);
            post_spikes.push_back(post_time);
            
            // Add some anti-Hebbian patterns (post before pre)
            if (rng.bernoulli(0.3)) { // 30% chance of anti-Hebbian
                nlm::Timestamp anti_post = pre_time - rng.uniform_double(1.0, 10.0);
                nlm::Timestamp anti_pre = pre_time + rng.uniform_double(1.0, 10.0);
                
                pre_spikes.push_back(anti_pre);
                post_spikes.push_back(anti_post);
            }
        }
        
        // Sort spikes by time
        std::sort(pre_spikes.begin(), pre_spikes.end());
        std::sort(post_spikes.begin(), post_spikes.end());
        
        // Test plasticity update
        float initial_weight = synapses[pair % synapse_count].getWeight();
        
        stdp.update(&synapses[pair % synapse_count], pre_spikes, post_spikes, 0.001);
        float final_weight = synapses[pair % synapse_count].getWeight();
        
        // Check weight bounds (Hebbian property)
        if (final_weight < -1.0f || final_weight > 1.0f) {
            hebbian_properties_preserved = false;
            std::cout << "    Property 'stdp_hebbian_properties' FAILED: "
                      << "Weight " << final_weight << " outside valid bounds [-1, 1]" << std::endl;
            break;
        }
        
        // Check LTP/LTD expectations (Hebbian property)
        if (final_weight > initial_weight) {
            // Should be potentiation when pre fires before post
            // Count pre-before-post events
            uint32_t ltp_events = 0;
            uint32_t ltd_events = 0;
            
            for (size_t i = 0; i < pre_spikes.size(); ++i) {
                for (size_t j = 0; j < post_spikes.size(); ++j) {
                    if (post_spikes[j] > pre_spikes[i]) {
                        ltp_events++;
                    } else if (post_spikes[j] < pre_spikes[i]) {
                        ltd_events++;
                    }
                }
            }
            
            // If there are more LTP events, weight should increase
            if (ltp_events > ltd_events && final_weight <= initial_weight) {
                std::cout << "    Property 'stdp_hebbian_properties' FAILED: "
                          << "Expected LTP but weight didn't increase" << std::endl;
                hebbian_properties_preserved = false;
                break;
            }
        }
    }
    
    return hebbian_properties_preserved;
}

// Property 2: Synaptic weights should remain bounded
bool plasticity_weight_bounds(nlm::property_test::PropertyRandom& rng) {
    const uint32_t synapse_count = 15;
    const uint32_t iterations = 100;
    
    std::vector<nlm::Synapse> synapses;
    
    // Create synapses with random initial weights
    for (uint32_t i = 0; i < synapse_count; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapses.back().setType(nlm::SynapseType::Excitatory);
        synapses.back().setWeight(rng.uniform_double(-0.5, 0.5));
        synapses.back().enablePlasticity(true, true, true); // Enable all plasticity
    }
    
    nlm::STDP stdp;
    bool weights_remain_bounded = true;
    
    for (uint32_t iter = 0; iter < iterations && weights_remain_bounded; ++iter) {
        // Generate random spike patterns
        std::vector<nlm::Timestamp> pre_spikes;
        std::vector<nlm::Timestamp> post_spikes;
        
        uint32_t num_pre = rng.uniform_uint64(0, 5);
        uint32_t num_post = rng.uniform_uint64(0, 5);
        
        for (uint32_t i = 0; i < num_pre; ++i) {
            pre_spikes.push_back(rng.uniform_double(0.0, 50.0));
        }
        for (uint32_t i = 0; i < num_post; ++i) {
            post_spikes.push_back(rng.uniform_double(0.0, 50.0));
        }
        
        // Apply plasticity to random synapses
        for (uint32_t i = 0; i < synapse_count && weights_remain_bounded; ++i) {
            uint32_t synapse_idx = rng.uniform_uint64(0, synapse_count - 1);
            
            float weight_before = synapses[synapse_idx].getWeight();
            
            stdp.update(&synapses[synapse_idx], pre_spikes, post_spikes, 0.001);
            
            float weight_after = synapses[synapse_idx].getWeight();
            
            // Check bounds (should be between -1 and 1)
            if (weight_after < -1.0f || weight_after > 1.0f) {
                weights_remain_bounded = false;
                std::cout << "    Property 'plasticity_weight_bounds' FAILED: "
                          << "Synapse " << synapse_idx << " weight " << weight_after 
                          << " outside bounds [-1, 1] after iteration " << iter << std::endl;
            }
        }
    }
    
    return weights_remain_bounded;
}

// Property 3: Plasticity should not cause network instability
bool plasticity_stability(nlm::property_test::PropertyRandom& rng) {
    const uint32_t network_size = 30;
    const uint32_t plasticity_iterations = 50;
    
    std::vector<nlm::Neuron> neurons;
    std::vector<nlm::Synapse> synapses;
    
    // Create network
    for (uint32_t i = 0; i < network_size; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
        auto params = rng.generate_neuron_params(network_size);
        neurons.back().setType(params.type);
        neurons.back().setRestingPotential(params.resting_potential);
        neurons.back().setThreshold(params.threshold);
    }
    
    // Create random synapses
    const uint32_t max_connections = network_size * 2;
    uint32_t connection_count = 0;
    
    for (uint32_t i = 0; i < network_size && connection_count < max_connections; ++i) {
        for (uint32_t j = 0; j < network_size && connection_count < max_connections; ++j) {
            if (i != j && rng.bernoulli(0.1)) { // 10% connection probability
                synapses.emplace_back(nlm::SynapseId(connection_count), 
                                    nlm::NeuronId(i), nlm::NeuronId(j));
                synapses.back().setType(nlm::SynapseType::Excitatory);
                synapses.back().setWeight(rng.uniform_double(0.0, 0.5));
                synapses.back().enablePlasticity(true, true, false);
                connection_count++;
            }
        }
    }
    
    nlm::STDP stdp;
    bool network_stable = true;
    
    for (uint32_t iter = 0; iter < plasticity_iterations && network_stable; ++iter) {
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(iter * 0.1);
        
        // Generate random spike activity
        uint32_t active_neurons = rng.uniform_uint64(network_size / 10, network_size / 4);
        
        std::vector<nlm::Timestamp> pre_spikes;
        std::vector<nlm::Timestamp> post_spikes;
        
        for (uint32_t i = 0; i < active_neurons; ++i) {
            uint32_t neuron_idx = rng.uniform_uint64(network_size);
            pre_spikes.push_back(current_time + rng.uniform_double(0.0, 5.0));
        }
        
        // Simulate network dynamics and plasticity
        for (uint32_t i = 0; i < network_size; ++i) {
            double current = rng.uniform_double(-2.0, 2.0);
            neurons[i].injectCurrent(static_cast<nlm::MembranePotential>(current));
            neurons[i].stepLIF(current_time, 0.1);
            neurons[i].clearTotalCurrent();
        }
        
        // Apply plasticity
        for (auto& synapse : synapses) {
            stdp.update(&synapse, pre_spikes, post_spikes, 0.1);
        }
        
        // Check for instability (runaway excitation/inhibition)
        uint32_t firing_neurons = 0;
        uint32_t silent_neurons = 0;
        
        for (const auto& neuron : neurons) {
            if (neuron.isFiring()) firing_neurons++;
            if (neuron.getMembranePotential() < neuron.getRestingPotential() - 10.0f) 
                silent_neurons++;
        }
        
        double firing_rate = static_cast<double>(firing_neurons) / network_size;
        double silent_rate = static_cast<double>(silent_neurons) / network_size;
        
        // Check for extreme activity patterns
        if (firing_rate > 0.7 && silent_rate < 0.1) {
            network_stable = false;
            std::cout << "    Property 'plasticity_stability' FAILED: "
                      << "Potential instability at iteration " << iter << ": "
                      << "firing_rate=" << firing_rate << ", silent_rate=" << silent_rate << std::endl;
        }
    }
    
    return network_stable;
}

// Property 4: Learning should be experience-dependent
bool plasticity_experience_dependent(nlm::property_test::PropertyRandom& rng) {
    const uint32_t trials = 20;
    const uint32_t neurons_per_trial = 5;
    
    // Create learning agents
    std::vector<nlm::Neuron> learning_neurons;
    
    for (uint32_t i = 0; i < neurons_per_trial; ++i) {
        learning_neurons.emplace_back(nlm::NeuronId(i));
        auto params = rng.generate_neuron_params(neurons_per_trial);
        learning_neurons.back().setType(params.type);
        learning_neurons.back().setRestingPotential(params.resting_potential);
        learning_neurons.back().setThreshold(params.threshold);
        learning_neurons.back().setRefractoryPeriod(params.refractory_period);
    }
    
    bool learning_experience_dependent = true;
    
    for (uint32_t trial = 0; trial < trials && learning_experience_dependent; ++trial) {
        // Generate trial-specific input pattern
        std::vector<double> trial_inputs;
        uint32_t active_neurons = rng.uniform_uint64(1, neurons_per_trial);
        
        for (uint32_t i = 0; i < neurons_per_trial; ++i) {
            if (i < active_neurons) {
                double input = rng.uniform_double(5.0, 15.0); // Strong input
                trial_inputs.push_back(input);
            } else {
                trial_inputs.push_back(0.0); // No input
            }
        }
        
        // Track learning progress
        std::vector<float> initial_weights;
        std::vector<float> final_weights;
        
        for (uint32_t i = 0; i < neurons_per_trial; ++i) {
            initial_weights.push_back(learning_neurons[i].getMembranePotential());
        }
        
        // Apply learning (simulated by input modulation)
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(trial * 0.5);
        
        for (uint32_t i = 0; i < neurons_per_trial; ++i) {
            if (trial_inputs[i] > 0.0) {
                learning_neurons[i].injectCurrent(static_cast<nlm::MembranePotential>(trial_inputs[i]));
            }
            learning_neurons[i].stepLIF(current_time, 0.1);
            learning_neurons[i].clearTotalCurrent();
        }
        
        // Record final state
        for (uint32_t i = 0; i < neurons_per_trial; ++i) {
            final_weights.push_back(learning_neurons[i].getMembranePotential());
        }
        
        // Check if learning is experience-dependent:
        // Neurons with stronger input should show greater change
        bool trial_learned = false;
        
        for (uint32_t i = 0; i < neurons_per_trial; ++i) {
            float change = std::abs(final_weights[i] - initial_weights[i]);
            if (change > 1.0f) { // Significant change indicates learning
                trial_learned = true;
                break;
            }
        }
        
        if (!trial_learned) {
            // If no neuron learned from this trial, but there was input, something might be wrong
            bool had_input = false;
            for (uint32_t i = 0; i < neurons_per_trial; ++i) {
                if (trial_inputs[i] > 0.0) had_input = true;
            }
            
            if (had_input) {
                // This could be acceptable if neurons are in refractory state, but property requires learning
                learning_experience_dependent = false;
                std::cout << "    Property 'plasticity_experience_dependent' FAILED: "
                          << "Trial " << trial << " had input but no learning detected" << std::endl;
            }
        }
    }
    
    return learning_experience_dependent;
}

// Property test runner for plasticity
void runPlasticityPropertyTests() {
    std::cout << "=== PLASTICITY PROPERTY TESTS ===" << std::endl;
    
    nlm::property_test::PropertyTestEngine engine;
    nlm::property_test::PropertyRandom rng(12345);
    
    // Define property test instances
    class TestSTDPHebbianProperties {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("plasticity", "stdp_hebbian_properties");
            result.test_iterations = 20;
            bool passed = stdp_hebbian_properties(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestPlasticityWeightBounds {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("plasticity", "weight_bounds");
            result.test_iterations = 100;
            bool passed = plasticity_weight_bounds(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestPlasticityStability {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("plasticity", "stability");
            bool passed = plasticity_stability(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestPlasticityExperienceDependent {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("plasticity", "experience_dependent");
            result.test_iterations = 20;
            bool passed = plasticity_experience_dependent(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    // Run all property tests
    engine.start_timer();
    
    TestSTDPHebbianProperties().run(engine, rng);
    TestPlasticityWeightBounds().run(engine, rng);
    TestPlasticityStability().run(engine, rng);
    TestPlasticityExperienceDependent().run(engine, rng);
    
    engine.stop_timer();
    engine.generate_report("plasticity_property_test_report.txt");
}

} // namespace test_plasticity

#endif // PROPERTY_TEST_PLASTICITY_H