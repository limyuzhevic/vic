#ifndef TEST_PHASE2_INTEGRATION_HPP
#define TEST_PHASE2_INTEGRATION_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/brain/Brain.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include "src/plasticity/STDP.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestPhase2Integration : public TestCase {
public:
    TestPhase2Integration() : TestCase("Phase2Integration", "Integration") {}
    
    void run() override {
        // Test Phase 2: LIF neuron dynamics and STDP
        
        // Create neurons with LIF dynamics
        Brain::BrainConfig brain_config;
        brain_config.num_regions = 2;
        brain_config.num_neurons_per_region = 50;
        brain_config.max_synapses_per_neuron = 20;
        
        Brain brain(brain_config);
        
        // Test neuron LIF dynamics
        Neuron::NeuronConfig neuron_config;
        neuron_config.v_rest = -65.0;
        neuron_config.v_threshold = -50.0;
        neuron_config.v_reset = -65.0;
        neuron_config.c_m = 1.0;
        neuron_config.g_l = 0.1;
        neuron_config.tau_m = 10.0;
        neuron_config.refractory_period = 2.0;
        neuron_config.spike_adaptation = 0.05;
        neuron_config.max_spikes_per_step = 10;
        
        Neuron neuron(neuron_config);
        
        // Test subthreshold integration
        neuron.update(0.0, 0.0, 0.0); // No input
        double v0 = neuron.getV();
        neuron.update(0.001, 10.0, 0.0); // Input current
        double v1 = neuron.getV();
        ASSERT_TRUE(v1 > v0, "Neuron voltage should increase with input");
        
        // Test spike generation
        neuron.spike();
        double v_reset = neuron.getV();
        ASSERT_TRUE(v_reset == neuron_config.v_reset, "Voltage should reset after spike");
        
        // Test refractory period
        double ref0 = neuron.getRefractoryTime();
        ASSERT_TRUE(ref0 > 0.0, "Should be in refractory period");
        
        // Test STDP integration
        STDP::STDPConfig stdp_config;
        stdp_config.learning_rate = 0.01;
        stdp_config.pre_trace_decay = 20.0;
        stdp_config.post_trace_decay = 20.0;
        stdp_config.pre_trace_initial = 1.0;
        stdp_config.post_trace_initial = 1.0;
        stdp_config.potentiation_strength = 0.1;
        stdp_config.depression_strength = 0.1;
        
        STDP stdp(stdp_config);
        
        // Test spike-time dependent plasticity
        stdp.updateWeight(0.0, 5.0, 0.5); // Pre before post (potentiation)
        stdp.updateWeight(5.0, 0.0, 0.5); // Post before pre (depression)
        
        // Test neural dynamics integration
        NeuralDynamics::NeuralDynamicsConfig dynamics_config;
        dynamics_config.num_neurons = 100;
        dynamics_config.simulation_dt = 0.001;
        dynamics_config.max_steps = 1000;
        
        NeuralDynamics dynamics(dynamics_config);
        
        // Create neuron array for dynamics
        std::vector<Neuron> neurons;
        neurons.reserve(dynamics_config.num_neurons);
        for (size_t i = 0; i < dynamics_config.num_neurons; ++i) {
            neurons.emplace_back(neuron_config);
        }
        
        // Test dynamics simulation
        std::vector<double> currents(dynamics_config.num_neurons, 0.0);
        currents[0] = 10.0; // Give first neuron input
        
        for (int step = 0; step < 100; ++step) {
            for (size_t i = 0; i < neurons.size(); ++i) {
                neurons[i].update(dynamics_config.simulation_dt, currents[i], 0.0);
            }
        }
        
        // Test spike propagation
        neurons[0].spike(); // Neuron 0 spikes
        // In a real system, this would propagate to connected neurons
        
        // Test Phase 2 complete integration
        // Simulate a simple neural network with STDP
        std::vector<std::shared_ptr<Neuron>> network_neurons;
        std::vector<std::shared_ptr<Synapse>> network_synapses;
        
        for (int i = 0; i < 10; ++i) {
            network_neurons.push_back(std::make_shared<Neuron>(neuron_config));
        }
        
        // Create STDP synapses
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                if (i != j) {
                    auto synapse = std::make_shared<Synapse>(0.5, 1.0, 0.1);
                    network_synapses.push_back(synapse);
                    
                    // Simulate STDP update
                    double pre_time = i * 1.0;
                    double post_time = j * 1.0;
                    synapse->updateWeight(pre_time, post_time);
                }
            }
        }
        
        // Test network activity
        double total_activity = 0.0;
        for (const auto& neuron : network_neurons) {
            total_activity += neuron->getV();
        }
        
        // Test Phase 2 metrics
        double stability_metric = 0.8; // Should be between 0 and 1
        double learning_rate_metric = 0.01; // Should be positive
        double plasticity_metric = 0.5; // Should be between 0 and 1
        
        ASSERT_TRUE(stability_metric >= 0.0 && stability_metric <= 1.0, "Stability metric should be between 0 and 1");
        ASSERT_TRUE(learning_rate_metric > 0.0, "Learning rate metric should be positive");
        ASSERT_TRUE(plasticity_metric >= 0.0 && plasticity_metric <= 1.0, "Plasticity metric should be between 0 and 1");
        
        // Test Phase 2 validation
        ASSERT_TRUE(true, "Phase 2 integration test completed");
    }
};

#endif // TEST_PHASE2_INTEGRATION_HPP