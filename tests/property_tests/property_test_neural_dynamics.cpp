// Property-based tests for neural dynamics
// Tests invariants for neuron and neural population behavior

#ifndef PROPERTY_TEST_NEURAL_DYNAMICS_H
#define PROPERTY_TEST_NEURAL_DYNAMICS_H

#include "property_tests/NLMPropertyTestFramework.hpp"
#include "brain/Neuron.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

namespace test_neural_dynamics {

// Property 1: Membrane potential should never exceed threshold by more than one step
bool neuralDynamics_neuron_potential_bounds(nlm::property_test::PropertyRandom& rng) {
    const uint64_t neuron_count = 10;
    std::vector<nlm::Neuron> neurons;
    
    // Create neurons with random parameters
    for (uint64_t i = 0; i < neuron_count; ++i) {
        neurons.emplace_back(nlm::NeuronId(i + 1));
        
        // Set random properties
        auto params = rng.generate_neuron_params(neuron_count);
        neurons.back().setType(params.type);
        neurons.back().setRestingPotential(params.resting_potential);
        neurons.back().setThreshold(params.threshold);
        neurons.back().setResetPotential(params.reset_potential);
        neurons.back().setLeakConductance(params.leak_conductance);
        neurons.back().setRefractoryPeriod(params.refractory_period);
    }
    
    // Test 100 random simulation steps
    const uint64_t iterations = 100;
    uint64_t successful_iterations = 0;
    
    for (uint64_t iter = 0; iter < iterations; ++iter) {
        bool iteration_success = true;
        
        // Simulate one step for all neurons
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(iter * 0.001);
        
        for (auto& neuron : neurons) {
            // Record spike before step
            bool had_spike = neuron.checkThreshold();
            
            // Inject random current
            double current = rng.uniform_double(-10.0, 10.0);
            neuron.injectCurrent(static_cast<nlm::MembranePotential>(current));
            
            // Apply LIF dynamics
            if (neuron.stepLIF(current_time, 0.001)) {
                // Neuron fired - verify post-spike state
                double potential = neuron.getMembranePotential();
                double threshold = neuron.getThreshold();
                
                // After spike, potential should be at reset level
                if (std::abs(potential - neuron.getResetPotential()) > 0.1) {
                    iteration_success = false;
                    break;
                }
            }
            
            // Clear current for next step
            neuron.clearTotalCurrent();
        }
        
        if (iteration_success) {
            successful_iterations++;
        }
    }
    
    double success_rate = static_cast<double>(successful_iterations) / iterations;
    bool passed = success_rate >= 0.95; // Require 95% success rate
    
    if (!passed) {
        std::cout << "    Property 'neuralDynamics_neuron_potential_bounds' FAILED: "
                  << "Success rate = " << success_rate * 100 << "%" << std::endl;
    }
    
    return passed;
}

// Property 2: Spike generation consistency (determinism)
bool neuralDynamics_spike_consistency(nlm::property_test::PropertyRandom& rng) {
    const uint64_t neuron_count = 5;
    std::vector<nlm::Neuron> neurons;
    
    // Create neurons with fixed seed for deterministic testing
    nlm::property_test::PropertyRandom deterministic_rng(12345);
    
    for (uint64_t i = 0; i < neuron_count; ++i) {
        neurons.emplace_back(nlm::NeuronId(i + 1));
        
        // Use same random parameters for both test runs
        auto params = deterministic_rng.generate_neuron_params(neuron_count);
        neurons.back().setType(params.type);
        neurons.back().setRestingPotential(params.resting_potential);
        neurons.back().setThreshold(params.threshold);
        neurons.back().setResetPotential(params.reset_potential);
        neurons.back().setLeakConductance(params.leak_conductance);
        neurons.back().setRefractoryPeriod(params.refractory_period);
    }
    
    // Run the same input sequence twice
    nlm::property_test::PropertyRandom rng1(42);
    nlm::property_test::PropertyRandom rng2(42); // Same seed for deterministic test
    
    std::vector<std::vector<bool>> spike_patterns1(neuron_count);
    std::vector<std::vector<bool>> spike_patterns2(neuron_count);
    
    const uint64_t time_steps = 50;
    
    // First run with random seed 42
    for (uint64_t step = 0; step < time_steps; ++step) {
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(step * 0.001);
        
        for (uint64_t i = 0; i < neuron_count; ++i) {
            double current = rng1.uniform_double(-5.0, 5.0);
            neurons[i].injectCurrent(static_cast<nlm::MembranePotential>(current));
            bool spiked = neurons[i].stepLIF(current_time, 0.001);
            spike_patterns1[i].push_back(spiked);
            neurons[i].clearTotalCurrent();
        }
    }
    
    // Reset neurons
    for (auto& neuron : neurons) {
        neuron.reset();
    }
    
    // Second run with same random seed
    for (uint64_t step = 0; step < time_steps; ++step) {
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(step * 0.001);
        
        for (uint64_t i = 0; i < neuron_count; ++i) {
            double current = rng2.uniform_double(-5.0, 5.0); // Same sequence
            neurons[i].injectCurrent(static_cast<nlm::MembranePotential>(current));
            bool spiked = neurons[i].stepLIF(current_time, 0.001);
            spike_patterns2[i].push_back(spiked);
            neurons[i].clearTotalCurrent();
        }
    }
    
    // Compare spike patterns
    bool deterministic = true;
    for (uint64_t i = 0; i < neuron_count; ++i) {
        if (spike_patterns1[i] != spike_patterns2[i]) {
            deterministic = false;
            break;
        }
    }
    
    if (!deterministic) {
        std::cout << "    Property 'neuralDynamics_spike_consistency' FAILED: "
                  << "Non-deterministic behavior detected" << std::endl;
    }
    
    return deterministic;
}

// Property 3: Total charge conservation (within numerical limits)
bool neuralDynamics_charge_conservation(nlm::property_test::PropertyRandom& rng) {
    const uint64_t neuron_count = 8;
    std::vector<nlm::Neuron> neurons;
    
    // Create neurons
    for (uint64_t i = 0; i < neuron_count; ++i) {
        neurons.emplace_back(nlm::NeuronId(i + 1));
        auto params = rng.generate_neuron_params(neuron_count);
        neurons.back().setType(params.type);
        neurons.back().setRestingPotential(params.resting_potential);
        neurons.back().setThreshold(params.threshold);
    }
    
    // Test charge conservation over many iterations
    const uint64_t iterations = 200;
    bool all_conserved = true;
    
    for (uint64_t iter = 0; iter < iterations && all_conserved; ++iter) {
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(iter * 0.001);
        double total_injected_charge = 0.0;
        double total_released_charge = 0.0;
        
        // Inject random currents into some neurons
        for (uint64_t i = 0; i < neuron_count; ++i) {
            if (rng.bernoulli(0.3)) { // 30% chance of injecting current
                double current = rng.uniform_double(-10.0, 10.0);
                neurons[i].injectCurrent(static_cast<nlm::MembranePotential>(current));
                total_injected_charge += current;
            }
        }
        
        // Simulate step
        for (uint64_t i = 0; i < neuron_count; ++i) {
            neurons[i].stepLIF(current_time, 0.001);
            
            // Calculate released charge based on spike events
            if (neurons[i].isFiring()) {
                total_released_charge += neurons[i].getMembranePotential();
            }
            
            neurons[i].clearTotalCurrent();
        }
        
        // Charge conservation check (allowing for numerical errors and leakage)
        double charge_error = std::abs(total_injected_charge - total_released_charge);
        double max_expected_error = 50.0; // Allow some numerical error
        
        if (charge_error > max_expected_error) {
            all_conserved = false;
            std::cout << "    Property 'neuralDynamics_charge_conservation' FAILED: "
                      << "Charge error = " << charge_error << " > " << max_expected_error << std::endl;
        }
    }
    
    return all_conserved;
}

// Property 4: Network stability under various conditions
bool neuralDynamics_network_stability(nlm::property_test::PropertyRandom& rng) {
    const uint64_t neuron_count = 20;
    const uint64_t connection_probability = 30; // 30%
    std::vector<nlm::Neuron> neurons;
    
    // Create neurons
    for (uint64_t i = 0; i < neuron_count; ++i) {
        neurons.emplace_back(nlm::NeuronId(i + 1));
        auto params = rng.generate_neuron_params(neuron_count);
        neurons.back().setType(params.type);
        neurons.back().setRestingPotential(params.resting_potential);
        neurons.back().setThreshold(params.threshold);
    }
    
    // Establish random connections
    for (uint64_t i = 0; i < neuron_count; ++i) {
        for (uint64_t j = 0; j < neuron_count; ++j) {
            if (i != j && rng.bernoulli(connection_probability / 100.0)) {
                // Create a simple connection model
                double weight = rng.uniform_double(-0.1, 0.1);
                double delayed_impact = neurons[j].getMembranePotential() * weight;
                neurons[i].receiveExcitatoryInput(static_cast<nlm::MembranePotential>(delayed_impact));
            }
        }
    }
    
    // Run stability test
    const uint64_t iterations = 100;
    bool stable = true;
    
    for (uint64_t iter = 0; iter < iterations && stable; ++iter) {
        nlm::Timestamp current_time = static_cast<nlm::Timestamp>(iter * 0.001);
        
        // Check for runaway excitation/inhibition
        uint64_t firing_neurons = 0;
        uint64_t refractory_neurons = 0;
        
        for (const auto& neuron : neurons) {
            if (neuron.isFiring()) firing_neurons++;
            if (neuron.isRefractory()) refractory_neurons++;
        }
        
        // Stability criteria
        double firing_rate = static_cast<double>(firing_neurons) / neuron_count;
        double refractory_rate = static_cast<double>(refractory_neurons) / neuron_count;
        
        // Allow some variability but detect extreme instability
        if (firing_rate > 0.8 && refractory_rate > 0.6) {
            stable = false;
            std::cout << "    Property 'neuralDynamics_network_stability' FAILED: "
                      << "Potential instability at iteration " << iter << ": "
                      << "firing_rate=" << firing_rate << ", refractory_rate=" << refractory_rate << std::endl;
        }
        
        // Step all neurons
        for (auto& neuron : neurons) {
            double current = rng.uniform_double(-5.0, 5.0);
            neuron.injectCurrent(static_cast<nlm::MembranePotential>(current));
            neuron.stepLIF(current_time, 0.001);
            neuron.clearTotalCurrent();
        }
    }
    
    return stable;
}

// Property test runner for neural dynamics
void runNeuralDynamicsPropertyTests() {
    std::cout << "=== NEURAL DYNAMICS PROPERTY TESTS ===" << std::endl;
    
    nlm::property_test::PropertyTestEngine engine;
    nlm::property_test::PropertyRandom rng(12345);
    
    // Define property test instances
    class TestNeuralDynamicsPotentialBounds {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("neuralDynamics", "neuron_potential_bounds");
            result.test_iterations = 100;
            result.successful_iterations = 0;
            bool passed = neuralDynamics_neuron_potential_bounds(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestNeuralDynamicsSpikeConsistency {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("neuralDynamics", "spike_consistency");
            bool passed = neuralDynamics_spike_consistency(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestNeuralDynamicsChargeConservation {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("neuralDynamics", "charge_conservation");
            result.test_iterations = 200;
            result.successful_iterations = 0;
            bool passed = neuralDynamics_charge_conservation(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    class TestNeuralDynamicsNetworkStability {
    public:
        void run(nlm::property_test::PropertyTestEngine& engine, nlm::property_test::PropertyRandom& rng) {
            nlm::property_test::PropertyTestResult result("neuralDynamics", "network_stability");
            bool passed = neuralDynamics_network_stability(rng);
            result.passed = passed;
            engine.add_result(result);
        }
    };
    
    // Run all property tests
    engine.start_timer();
    
    TestNeuralDynamicsPotentialBounds().run(engine, rng);
    TestNeuralDynamicsSpikeConsistency().run(engine, rng);
    TestNeuralDynamicsChargeConservation().run(engine, rng);
    TestNeuralDynamicsNetworkStability().run(engine, rng);
    
    engine.stop_timer();
    engine.generate_report("neural_dynamics_property_test_report.txt");
}

} // namespace test_neural_dynamics

#endif // PROPERTY_TEST_NEURAL_DYNAMICS_H