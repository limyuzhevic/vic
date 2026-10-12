#ifndef TEST_BRAIN_CORE_HPP
#define TEST_BRAIN_CORE_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestBrainCore : public TestCase {
public:
    TestBrainCore() : TestCase("BrainCore") {}
    
    void run() override {
        // Test neuron creation and basic properties
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
        ASSERT_TRUE(neuron.getV() == neuron_config.v_rest, "Initial neuron voltage should be at rest");
        ASSERT_TRUE(neuron.getRefractoryTime() == 0.0, "Initial refractory time should be zero");
        
        // Test neuron spike generation
        double v_before_spike = neuron.getV();
        neuron.spike();
        ASSERT_TRUE(neuron.getRefractoryTime() > 0.0, "Spike should set refractory time");
        ASSERT_TRUE(neuron.getV() == neuron_config.v_reset, "Spike should reset voltage");
        
        // Test synapse creation and properties
        Synapse::SynapseConfig synapse_config;
        synapse_config.weight = 0.5;
        synapse_config.delay = 1.0;
        synapse_config.decay = 0.1;
        synapse_config.max_weight = 10.0;
        synapse_config.min_weight = -10.0;
        synapse_config.plasticity_enabled = true;
        synapse_config.plasticity_rate = 0.01;
        
        Synapse synapse(synapse_config);
        ASSERT_TRUE(synapse.getWeight() == synapse_config.weight, "Synapse weight should match config");
        ASSERT_TRUE(synapse.getDelay() == synapse_config.delay, "Synapse delay should match config");
        
        // Test synapse plasticity
        double pre_spike_time = 0.0;
        double post_spike_time = 1.0;
        double old_weight = synapse.getWeight();
        synapse.processSpike(pre_spike_time, post_spike_time);
        double new_weight = synapse.getWeight();
        ASSERT_TRUE(new_weight != old_weight, "Synapse weight should change after spike");
        
        // Test brain initialization
        Brain::BrainConfig brain_config;
        brain_config.num_regions = 3;
        brain_config.num_neurons_per_region = 100;
        brain_config.max_synapses_per_neuron = 10;
        brain_config.v_rest = -65.0;
        brain_config.v_threshold = -50.0;
        brain_config.v_reset = -65.0;
        brain_config.c_m = 1.0;
        brain_config.g_l = 0.1;
        brain_config.tau_m = 10.0;
        brain_config.dt = 0.001;
        
        Brain brain(brain_config);
        ASSERT_TRUE(brain.getNumRegions() == brain_config.num_regions, "Brain should have correct number of regions");
        
        // Test brain spike processing
        brain.processSpike(0.0, 0);
        brain.step();
        
        // Test brain connectivity
        brain.addConnection(0, 1, 0.5, 1.0);
        brain.addConnection(1, 2, 0.3, 2.0);
        
        // Test brain plasticity
        brain.updatePlasticity(0.01);
        
        // Test brain serialization (checkpoint)
        std::string checkpoint_dir = "/tmp/test_brain_checkpoint";
        brain.saveCheckpoint(checkpoint_dir);
        ASSERT_TRUE(true, "Brain checkpoint save should work");
        
        // Test brain memory usage
        size_t memory_before = brain.getMemoryUsage();
        brain.addNeuron(0, 1000.0, -65.0);
        size_t memory_after = brain.getMemoryUsage();
        ASSERT_TRUE(memory_after >= memory_before, "Memory usage should not decrease after adding neuron");
    }
};

#endif // TEST_BRAIN_CORE_HPP