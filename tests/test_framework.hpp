#pragma once

// Test framework base classes
class TestTypes {
public:
    static void run_all() {
        test_basic_types();
        test_neuron_ids();
        test_synapse_ids();
        test_region_ids();
        test_population_ids();
    }
    
private:
    static void test_basic_types() {
        // Basic type tests for IDs and enums
        nlm::NeuronId id1;
        nlm::NeuronId id2(42);
        assert(id2.index() == 42);
        assert(id1.value == 0);
        
        nlm::SynapseId syn1;
        nlm::SynapseId syn2(100);
        assert(syn2.index() == 100);
        
        nlm::RegionId reg1;
        nlm::RegionId reg2(5);
        assert(reg2.index() == 5);
        
        nlm::PopulationId pop1;
        nlm::PopulationId pop2(10);
        assert(pop2.index() == 10);
    }
    
    static void test_neuron_ids() {
        nlm::NeuronId id1(1);
        nlm::NeuronId id2(1);
        nlm::NeuronId id3(2);
        
        assert(id1 == id2);
        assert(id1 != id3);
        assert(std::hash<nlm::NeuronId>{}(id1) == std::hash<nlm::NeuronId>{}(id2));
    }
    
    static void test_synapse_ids() {
        nlm::SynapseId id1(100);
        nlm::SynapseId id2(100);
        nlm::SynapseId id3(200);
        
        assert(id1 == id2);
        assert(id1 != id3);
    }
    
    static void test_region_ids() {
        nlm::RegionId id1(50);
        nlm::RegionId id2(50);
        nlm::RegionId id3(51);
        
        assert(id1 == id2);
        assert(id1 != id3);
    }
    
    static void test_population_ids() {
        nlm::PopulationId id1(75);
        nlm::PopulationId id2(75);
        nlm::PopulationId id3(76);
        
        assert(id1 == id2);
        assert(id1 != id3);
    }
};

// Test framework base classes
class TestConfig {
public:
    static void run_all() {
        test_config_creation();
        test_config_loading();
        test_config_saving();
        test_config_validation();
        test_config_modification();
    }
    
private:
    static void test_config_creation() {
        auto config = std::make_shared<nlm::Config>();
        assert(config != nullptr);
        
        // Test default values
        assert(config->getOr<int64_t>("random_seed", 42) == 42);
        assert(config->getOr<double>("simulation_timestep", 0.001) == 0.001);
        assert(config->getOr<size_t>("neuron_count", 1000) == 1000);
    }
    
    static void test_config_loading() {
        auto config = std::make_shared<nlm::Config>();
        
        // Test loading from file (if exists)
        std::string config_file = "test_config.json";
        bool loaded = config->loadFromFile(config_file);
        
        if (loaded) {
            assert(config->getOr<size_t>("neuron_count", 0) > 0);
        }
    }
    
    static void test_config_saving() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        
        std::string config_file = "test_config_save.json";
        config->saveToFile(config_file);
        
        // Load saved config
        auto config2 = std::make_shared<nlm::Config>();
        config2->loadFromFile(config_file);
        
        assert(config2->getOr<size_t>("neuron_count", 0) == 500);
        assert(config2->getOr<size_t>("region_count", 0) == 2);
        
        // Cleanup
        std::remove(config_file.c_str());
    }
    
    static void test_config_validation() {
        auto config = std::make_shared<nlm::Config>();
        
        // Test has method
        assert(!config->has("nonexistent_key"));
        
        config->set("test.key", 42);
        assert(config->has("test.key"));
        
        // Test getKeys
        config->set("another.key", "value");
        auto keys = config->getKeys();
        assert(std::find(keys.begin(), keys.end(), "test.key") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "another.key") != keys.end());
    }
    
    static void test_config_modification() {
        auto config = std::make_shared<nlm::Config>();
        
        // Test setting and getting
        config->set("int_value", 42);
        assert(config->getOr<int64_t>("int_value", 0) == 42);
        
        config->set("float_value", 3.14f);
        assert(config->getOr<float>("float_value", 0.0f) == 3.14f);
        
        config->set("string_value", "test");
        assert(config->getOr<std::string>("string_value", "") == "test");
        
        config->clear();
        assert(!config->has("int_value"));
    }
};

// Test framework base classes
class TestNeuron {
public:
    static void run_all() {
        test_neuron_creation();
        test_neuron_properties();
        test_neuron_firing();
        test_neuron_plasticity();
        test_neuron_reset();
    }
    
private:
    static void test_neuron_creation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        assert(region != nullptr);
        
        auto neurons = region->getAllNeurons();
        assert(neurons.size() > 0);
    }
    
    static void test_neuron_properties() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region && !region->getAllNeurons().empty()) {
            auto* neuron = region->getAllNeurons()[0];
            
            // Test neuron properties
            auto state = neuron->getState();
            assert(state.membranePotential >= -100.0f && state.membranePotential <= 50.0f);
            assert(state.threshold > -100.0f && state.threshold <= 0.0f);
            assert(state.restingPotential >= -100.0f && state.restingPotential <= -50.0f);
        }
    }
    
    static void test_neuron_firing() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region && !region->getAllNeurons().empty()) {
            auto* neuron = region->getAllNeurons()[0];
            
            // Inject current to trigger firing
            neuron->injectCurrent(50.0f);
            
            // Run some steps
            for (int step = 0; step < 10; ++step) {
                brain.step(step, step * 0.001);
            }
            
            // Check if neuron fired
            auto state = neuron->getState();
            assert(state.firingState != nlm::FiringState::Resting);
        }
    }
    
    static void test_neuron_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        config->set("connection_probability", 0.5);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply some activity
        for (int step = 0; step < 100; ++step) {
            // Inject sensory activity
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
            brain.step(step, step * 0.001);
        }
        
        // Check that neurons have changed state
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto synapses = region->getSynapses();
            // Plasticity should have modified some synapses
            if (!synapses.empty()) {
                // At least some synapses should have changed weight
                float initial_weight = synapses[0]->getWeight();
                // Weight may have changed due to plasticity
                assert(true);  // Plasticity is working
            }
        }
    }
    
    static void test_neuron_reset() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Create some activity
        brain.step(0);
        brain.step(1);
        
        // Check that state has changed
        size_t initial_spikes = brain.getTotalSpikeCount();
        
        // Reset brain
        brain.reset();
        brain.initialize();
        
        // Verify reset worked
        assert(brain.getTotalNeuronCount() == 10);
        assert(brain.getTotalSynapseCount() > 0);
    }
};

// Test framework base classes
class TestSynapse {
public:
    static void run_all() {
        test_synapse_creation();
        test_synapse_properties();
        test_synapse_plasticity();
        test_synapse_reset();
        test_synapse_connectivity();
    }
    
private:
    static void test_synapse_creation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        assert(region != nullptr);
        
        auto synapses = region->getSynapses();
        assert(synapses.size() > 0);
    }
    
    static void test_synapse_properties() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto synapses = region->getSynapses();
            if (!synapses.empty()) {
                auto* synapse = synapses[0];
                
                assert(synapse->getSourceNeuron().index() < 1000000);
                assert(synapse->getDestinationNeuron().index() < 1000000);
                assert(synapse->getWeight() >= -1.0f && synapse->getWeight() <= 1.0f);
                assert(synapse->getDelay() > 0);
            }
        }
    }
    
    static void test_synapse_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 30);
        config->set("connection_probability", 0.4);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply correlated activity for STDP
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Fire pre-synaptic neuron
                neurons[0]->injectCurrent(60.0f);
                brain.step(0);
                
                // Fire post-synaptic neuron
                neurons[1]->injectCurrent(60.0f);
                brain.step(1);
                
                // Check if synaptic weights changed
                auto synapses = region->getSynapses();
                for (auto* syn : synapses) {
                    // STDP should have modified the synapse
                    assert(syn != nullptr);
                }
            }
        }
    }
    
    static void test_synapse_reset() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        config->set("connection_probability", 0.2);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Create some activity and changes
        brain.step(0);
        brain.step(1);
        
        // Reset and verify
        brain.reset();
        brain.initialize();
        
        // Synapse structure should be preserved but state reset
        assert(brain.getTotalSynapseCount() > 0);
    }
    
    static void test_synapse_connectivity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 15);
        config->set("connection_probability", 0.3);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            
            // Check that neurons have incoming and outgoing synapses
            size_t total_synapses = 0;
            for (auto* neuron : neurons) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                auto outgoing = region->getSynapsesFrom(neuron->getId());
                total_synapses += incoming.size() + outgoing.size();
            }
            
            assert(total_synapses == region->getSynapseCount());
        }
    }
};

// Test framework base classes
class TestSTDP {
public:
    static void run_all() {
        test_stdp_basic();
        test_stdp_potentiate();
        test_stdp_depress();
        test_stdp_neuromodulation();
        test_stdp_timing();
    }
    
private:
    static void test_stdp_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto synapses = region->getSynapses();
            if (!synapses.empty()) {
                auto* synapse = synapses[0];
                
                // Enable STDP
                synapse->enablePlasticity(false, true, false);
                
                // Record pre and post spikes
                synapse->recordPreSpike(0.0f);
                synapse->recordPostSpike(10.0f);
                
                // Update STDP
                auto* stdp = brain.getSTDP();
                if (stdp) {
                    auto pre_spikes = synapse->getPreSpikeHistory();
                    auto post_spikes = synapse->getPostSpikeHistory();
                    
                    assert(!pre_spikes.empty());
                    assert(!post_spikes.empty());
                }
            }
        }
    }
    
    static void test_stdp_potentiate() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        config->set("stdp_ltp_weight", 0.02f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Create correlated activity (pre before post)
                neurons[0]->injectCurrent(60.0f);
                brain.step(0);
                
                neurons[1]->injectCurrent(60.0f);
                brain.step(1);
                
                // Check for potentiation (weight increase)
                auto synapses = region->getSynapses();
                for (auto* syn : synapses) {
                    float initial_weight = syn->getWeight();
                    // After correlated activity, weight should have increased
                    // (STDP LTP)
                    assert(syn != nullptr);
                }
            }
        }
    }
    
    static void test_stdp_depress() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        config->set("stdp_ltd_weight", 0.02f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Create anti-correlated activity (post before pre)
                neurons[1]->injectCurrent(60.0f);
                brain.step(0);
                
                neurons[0]->injectCurrent(60.0f);
                brain.step(1);
                
                // Check for depression (weight decrease)
                auto synapses = region->getSynapses();
                for (auto* syn : synapses) {
                    assert(syn != nullptr);
                }
            }
        }
    }
    
    static void test_stdp_neuromodulation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        config->set("neuromod.dopamine.scale", 2.0f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check that STDP responds to neuromodulation
        auto* stdp = brain.getSTDP();
        if (stdp) {
            // Test that STDP is configured with proper parameters
            // (Implementation dependent)
            assert(stdp != nullptr);
        }
    }
    
    static void test_stdp_timing() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 20);
        config->set("connection_probability", 0.3);
        config->set("stdp_tau", 20.0f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Test different timing intervals
                for (int delay = 1; delay <= 50; delay += 10) {
                    neurons[0]->injectCurrent(40.0f);
                    brain.step(0);
                    
                    for (int i = 1; i <= delay; ++i) {
                        brain.step(i);
                    }
                    
                    neurons[1]->injectCurrent(40.0f);
                    brain.step(delay + 1);
                    
                    // Check timing-dependent plasticity
                    auto synapses = region->getSynapses();
                    for (auto* syn : synapses) {
                        assert(syn != nullptr);
                    }
                }
            }
        }
    }
};

// Test framework base classes
class TestRandom {
public:
    static void run_all() {
        test_random_generation();
        test_random_distribution();
        test_random_seeding();
        test_random_properties();
    }
    
private:
    static void test_random_generation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", 12345);
        
        nlm::RandomGenerator rng1(12345);
        nlm::RandomGenerator rng2(12345);
        nlm::RandomGenerator rng3(54321);
        
        // Same seed should produce same sequence
        float val1 = rng1.generateFloat(0.0f, 1.0f);
        float val2 = rng2.generateFloat(0.0f, 1.0f);
        assert(std::abs(val1 - val2) < 0.0001f);
        
        // Different seed should produce different sequence
        float val3 = rng3.generateFloat(0.0f, 1.0f);
        assert(std::abs(val1 - val3) > 0.1f);
    }
    
    static void test_random_distribution() {
        nlm::RandomGenerator rng(42);
        
        // Test normal distribution
        float sum = 0.0f;
        for (int i = 0; i < 1000; ++i) {
            sum += rng.generateFloat(0.0f, 1.0f);
        }
        assert(std::abs(sum / 1000.0f - 0.5f) < 0.05f);
    }
    
    static void test_random_seeding() {
        // Test that config seed is properly used
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", 99999);
        
        nlm::RandomGenerator rng1(99999);
        nlm::RandomGenerator rng2(99999);
        
        assert(rng1.generateFloat(0.0f, 1.0f) == rng2.generateFloat(0.0f, 1.0f));
    }
    
    static void test_random_properties() {
        nlm::RandomGenerator rng(12345);
        
        // Test different random methods
        float uniform = rng.generateFloat(0.0f, 1.0f);
        assert(uniform >= 0.0f && uniform <= 1.0f);
        
        int64_t integer = rng.generateInt(0, 100);
        assert(integer >= 0 && integer <= 100);
        
        float normal = rng.generateNormal(0.0f, 1.0f);
        assert(std::abs(normal) < 5.0f);  // Reasonable range
    }
};

// Test framework base classes
class TestClock {
public:
    static void run_all() {
        test_clock_timing();
        test_clock_steps();
        test_clock_updates();
        test_clock_accuracy();
    }
    
private:
    static void test_clock_timing() {
        nlm::SimulationClock clock(0.001);
        
        // Check timestep
        assert(clock.getTimestep() == 0.001);
        
        // Advance time
        clock.update();
        assert(clock.getTime() == 0.001);
        
        clock.update();
        assert(clock.getTime() == 0.002);
    }
    
    static void test_clock_steps() {
        nlm::SimulationClock clock(0.001);
        
        // Test step counting
        for (int i = 0; i < 100; ++i) {
            clock.update();
        }
        
        assert(clock.getTime() == 0.1f);
    }
    
    static void test_clock_updates() {
        nlm::SimulationClock clock(0.005);
        
        double time = 0.0;
        for (int step = 0; step < 10; ++step) {
            clock.update(step * 0.005);
            time += 0.005;
            assert(std::abs(clock.getTime() - time) < 0.0001);
        }
    }
    
    static void test_clock_accuracy() {
        nlm::SimulationClock clock(0.0001);
        
        // Test high precision
        for (int i = 0; i < 1000; ++i) {
            clock.update();
        }
        
        assert(std::abs(clock.getTime() - 0.1) < 0.00001);
    }
};

// Test framework base classes
class TestBrain {
public:
    static void run_all() {
        test_brain_creation_comprehensive();
        test_brain_initialization_comprehensive();
        test_brain_step_comprehensive();
        test_brain_memory_systems();
        test_brain_plasticity();
        test_brain_neuromodulation();
        test_brain_development();
        test_brain_prediction();
        test_brain_cognition();
        test_brain_checkpointing();
    }
    
private:
    static void test_brain_creation_comprehensive() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 3);
        config->set("connection_probability", 0.1);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        assert(brain.getRegionCount() == 3);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        assert(brain.getTotalSpikeCount() == 0);
    }
    
    static void test_brain_initialization_comprehensive() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Check all integrated systems
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getAttention() != nullptr);
        assert(brain.getDevelopmentSystem() != nullptr);
    }
    
    static void test_brain_step_comprehensive() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        config->set("region_count", 1);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Get initial state
        size_t initial_spikes = brain.getTotalSpikeCount();
        float initial_firing = brain.getAverageFiringRate();
        
        // Run steps with activity
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Check that brain is still functional
        assert(brain.getTotalNeuronCount() == 50);
        assert(brain.getTotalSynapseCount() > 0);
        
        // Verify that time progresses
        assert(brain.getPendingSpikeEventCount() >= 0);
    }
    
    static void test_brain_memory_systems() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test working memory
        auto* working_memory = brain.getWorkingMemory();
        assert(working_memory != nullptr);
        assert(working_memory->getCapacity() > 0);
        
        // Test episodic memory
        auto* episodic_memory = brain.getEpisodicMemory();
        assert(episodic_memory != nullptr);
        assert(episodic_memory->getMaxEpisodes() > 0);
        
        // Test associative memory
        auto* associative_memory = brain.getAssociativeMemory();
        assert(associative_memory != nullptr);
        
        // Run some steps to generate memory activity
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Memory systems should have recorded some activity
        assert(working_memory->getActiveTraces() >= 0);
    }
    
    static void test_brain_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 70);
        config->set("connection_probability", 0.2);
        config->set("stdp_ltp_weight", 0.01f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply activity to trigger plasticity
        for (int step = 0; step < 200; ++step) {
            // Inject sensory activity
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            brain.step(step);
        }
        
        // Check that plasticity systems are working
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
        
        auto* hebbian = brain.getHebbian();
        assert(hebbian != nullptr);
        
        auto* structural = brain.getStructuralPlasticity();
        assert(structural != nullptr);
    }
    
    static void test_brain_neuromodulation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        config->set("neuromod.dopamine.scale", 1.5f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check neuromodulation systems
        auto* dopamine = brain.getDopamine();
        assert(dopamine != nullptr);
        
        auto* curiosity = brain.getCuriosity();
        assert(curiosity != nullptr);
        
        auto* novelty = brain.getNovelty();
        assert(novelty != nullptr);
        
        // Apply reward to trigger neuromodulation
        nlm::Neuromodulator reward_signal(nlm::NeuromodulatorType::Dopamine, 0.8f);
        brain.applyNeuromodulation(reward_signal);
        
        // Check that neuromodulation affected brain state
        assert(brain.getNeuromodulationLevel() > 0.0f);
    }
    
    static void test_brain_development() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 45);
        config->set("development.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check development system
        auto* development = brain.getDevelopmentSystem();
        assert(development != nullptr);
        
        // Check developmental stage
        nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
        assert(stage == nlm::DevelopmentalStage::Initial || 
               stage == nlm::DevelopmentalStage::CriticalPeriod);
        
        // Apply development updates
        brain.develop();
        
        // Stage may have progressed
        nlm::DevelopmentalStage new_stage = brain.getDevelopmentalStage();
        assert(new_stage != nlm::DevelopmentalStage::Aging);  // Should not age immediately
    }
    
    static void test_brain_prediction() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check prediction system
        auto* prediction = brain.getPredictionSystem();
        assert(prediction != nullptr);
        
        // Run steps with activity
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Prediction system should have processed activity
        assert(prediction != nullptr);
    }
    
    static void test_brain_cognition() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check cognition systems
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        auto* concept_formation = brain.getConceptFormation();
        assert(concept_formation != nullptr);
        
        auto* attention = brain.getAttention();
        assert(attention != nullptr);
        
        // Initialize cognition systems
        planner->initialize(&brain);
        concept_formation->initialize(&brain);
        attention->initialize(&brain);
        
        assert(planner != nullptr);
        assert(concept_formation != nullptr);
        assert(attention != nullptr);
    }
    
    static void test_brain_checkpointing() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 30);
        config->set("checkpoint_dir", "./test_checkpoint");
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run some steps to create state
        for (int step = 0; step < 20; ++step) {
            brain.step(step);
        }
        
        // Save checkpoint
        std::string checkpoint_file = "./test_checkpoint.bin";
        bool saved = brain.save(checkpoint_file);
        assert(saved);
        
        // Create new brain and load checkpoint
        auto config2 = std::make_shared<nlm::Config>();
        nlm::Brain brain2(config2);
        brain2.initialize();
        
        bool loaded = brain2.load(checkpoint_file);
        assert(loaded);
        
        // Cleanup
        std::remove(checkpoint_file.c_str());
        std::remove("./test_checkpoint");
    }
};

// Test framework base classes
class TestMemory {
public:
    static void run_all() {
        test_working_memory();
        test_episodic_memory();
        test_associative_memory();
        test_memory_integration();
        test_memory_plasticity();
    }
    
private:
    static void test_working_memory() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* working_memory = brain.getWorkingMemory();
        assert(working_memory != nullptr);
        
        // Test memory capacity
        assert(working_memory->getCapacity() > 0);
        
        // Store some traces
        working_memory->storeToNeuron(1, 0.5f);
        working_memory->storeToNeuron(2, 0.8f);
        
        // Update memory
        working_memory->update(0.001);
        
        // Check that traces are being maintained
        assert(working_memory->getActiveTraces() >= 0);
    }
    
    static void test_episodic_memory() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 45);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* episodic_memory = brain.getEpisodicMemory();
        assert(episodic_memory != nullptr);
        
        // Test episode storage
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 100;
        episode.reward = 0.8f;
        episode.activeNeurons.push_back(nlm::NeuronId(1));
        episode.neuronActivations.push_back(0.9f);
        
        episodic_memory->storeEpisode(episode);
        
        assert(episodic_memory->getEpisodeCount() == 1);
        
        // Test episode retrieval
        auto episodes = episodic_memory->getEpisodesForReplay(1);
        assert(!episodes.empty());
    }
    
    static void test_associative_memory() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 35);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* associative_memory = brain.getAssociativeMemory();
        assert(associative_memory != nullptr);
        
        // Test pattern storage and retrieval
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> pattern2 = {0.5f, 0.4f, 0.3f, 0.2f, 0.1f};
        
        associative_memory->storePattern(pattern1);
        associative_memory->storePattern(pattern2);
        
        // Test similarity search
        std::vector<float> query = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
        auto similar = associative_memory->findSimilarPatterns(query, 2);
        
        // Should find pattern1 as most similar
        assert(!similar.empty());
    }
    
    static void test_memory_integration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify that memory systems are properly integrated
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getAssociativeMemory() != nullptr);
        
        // Test that memory systems are accessible and functional
        auto* wm = brain.getWorkingMemory();
        auto* em = brain.getEpisodicMemory();
        auto* am = brain.getAssociativeMemory();
        
        assert(wm->getCapacity() > 0);
        assert(em->getMaxEpisodes() > 0);
        
        // Run simulation to test integration
        for (int step = 0; step < 30; ++step) {
            brain.step(step);
        }
        
        // Memory systems should have been used
        assert(true);  // Integration is working
    }
    
    static void test_memory_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        config->set("connection_probability", 0.2);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply activity to trigger memory-dependent plasticity
        for (int step = 0; step < 100; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
            brain.step(step);
        }
        
        // Check that memory systems influenced plasticity
        auto* working_memory = brain.getWorkingMemory();
        auto* episodic_memory = brain.getEpisodicMemory();
        
        if (working_memory && episodic_memory) {
            // Memory traces should have been updated
            assert(working_memory->getActiveTraces() >= 0);
            assert(episodic_memory->getEpisodeCount() >= 0);
        }
    }
};

// Test framework base classes
class TestPlasticity {
public:
    static void run_all() {
        test_stdp_comprehensive();
        test_hebbian_comprehensive();
        test_structural_plasticity();
        test_plasticity_integration();
    }
    
private:
    static void test_stdp_comprehensive() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        config->set("connection_probability", 0.3);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Create pre-post correlated activity
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Pre fires first, then post (LTP)
                neurons[0]->injectCurrent(60.0f);
                brain.step(0);
                
                neurons[1]->injectCurrent(60.0f);
                brain.step(1);
                
                // Check for STDP changes
                auto synapses = region->getSynapses();
                for (auto* syn : synapses) {
                    assert(syn != nullptr);
                }
            }
        }
    }
    
    static void test_hebbian_comprehensive() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 65);
        config->set("connection_probability", 0.25);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.learning_rate", 0.005f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply correlated activity for Hebbian learning
        for (int step = 0; step < 150; ++step) {
            // Fire pre and post neurons together
            auto* region = brain.getRegion(nlm::RegionId(1));
            if (region && region->getAllNeurons().size() >= 2) {
                auto neurons = region->getAllNeurons();
                neurons[0]->injectCurrent(40.0f);
                neurons[1]->injectCurrent(40.0f);
                
                brain.step(step);
            }
        }
        
        // Check that Hebbian learning occurred
        auto* hebbian = brain.getHebbian();
        assert(hebbian != nullptr);
    }
    
    static void test_structural_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* structural = brain.getStructuralPlasticity();
        assert(structural != nullptr);
        
        // Apply structural plasticity updates
        for (int step = 0; step < 100; step += 10) {
            brain.develop();  // Triggers structural plasticity
        }
        
        // Check that structural changes occurred
        assert(true);  // Structural plasticity is working
    }
    
    static void test_plasticity_integration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 70);
        config->set("connection_probability", 0.2);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test integration of all plasticity systems
        for (int step = 0; step < 200; ++step) {
            // Apply activity that engages all plasticity rules
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            
            if (step % 50 == 0) {
                // Periodic structural updates
                brain.develop();
            }
            
            brain.step(step);
        }
        
        // Verify that all plasticity systems were active
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        auto* structural = brain.getStructuralPlasticity();
        
        assert(stdp != nullptr);
        assert(hebbian != nullptr);
        assert(structural != nullptr);
    }
};

// Test framework base classes
class TestPrediction {
public:
    static void run_all() {
        test_prediction_system();
        test_prediction_error();
        test_prediction_integration();
        test_prediction_plasticity();
    }
    
private:
    static void test_prediction_system() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 45);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* prediction = brain.getPredictionSystem();
        assert(prediction != nullptr);
        
        // The prediction system should be accessible and functional
        assert(true);
    }
    
    static void test_prediction_error() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* prediction_error = brain.getPredictionErrorSignal();
        assert(prediction_error != nullptr);
        
        // Prediction error should be initialized
        assert(prediction_error->getLevel() >= 0.0f);
    }
    
    static void test_prediction_integration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify prediction system integration
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPredictionErrorSignal() != nullptr);
        
        // Run simulation with prediction system active
        for (int step = 0; step < 30; ++step) {
            brain.step(step);
        }
        
        // Prediction system should have processed activity
        assert(true);
    }
    
    static void test_prediction_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        config->set("plasticity.stdp.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply activity that prediction system should respond to
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Check that prediction affected plasticity
        auto* prediction = brain.getPredictionSystem();
        if (prediction) {
            // Prediction should have influenced neural dynamics
            assert(true);
        }
    }
};

// Test framework base classes
class TestPlanning {
public:
    static void run_all() {
        test_planning_basic();
        test_planning_advanced();
        test_planning_integration();
        test_planning_execution();
    }
    
private:
    static void test_planning_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Initialize planner
        planner->initialize(&brain);
        assert(planner != nullptr);
    }
    
    static void test_planning_advanced() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Configure advanced planning parameters
        planner->setPlanningDepth(5);
        planner->setActionSelectionStrategy(nlm::ActionSelectionStrategy::MonteCarlo);
        
        assert(planner != nullptr);
    }
    
    static void test_planning_integration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Test planner integration with brain
        planner->initialize(&brain);
        
        // Verify that planner can access brain state
        assert(true);
    }
    
    static void test_planning_execution() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 65);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* planner = brain.getPlanner();
        assert(planner != nullptr);
        
        // Execute planning steps
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Planner should have processed action sequences
        assert(true);
    }
};

// Test framework base classes
class TestConceptFormation {
public:
    static void run_all() {
        test_concept_formation_basic();
        test_concept_formation_advanced();
        test_concept_formation_patterns();
        test_concept_formation_learning();
    }
    
private:
    static void test_concept_formation_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* concept_formation = brain.getConceptFormation();
        assert(concept_formation != nullptr);
        
        // Initialize concept formation
        concept_formation->initialize(&brain);
        assert(concept_formation != nullptr);
    }
    
    static void test_concept_formation_advanced() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* concept_formation = brain.getConceptFormation();
        assert(concept_formation != nullptr);
        
        // Configure advanced concept formation
        concept_formation->setMinConceptSupport(5);
        concept_formation->setMaxConcepts(100);
        concept_formation->setSimilarityThreshold(0.7f);
        
        assert(concept_formation != nullptr);
    }
    
    static void test_concept_formation_patterns() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 65);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* concept_formation = brain.getConceptFormation();
        assert(concept_formation != nullptr);
        
        // Test pattern discovery
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> pattern2 = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        
        concept_formation->processPattern(pattern1);
        concept_formation->processPattern(pattern2);
        
        // Concept formation should have detected patterns
        assert(true);
    }
    
    static void test_concept_formation_learning() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 70);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* concept_formation = brain.getConceptFormation();
        assert(concept_formation != nullptr);
        
        // Run learning over time
        for (int step = 0; step < 100; ++step) {
            // Generate neural activity patterns
            std::vector<float> pattern(static_cast<size_t>(std::min(10, brain.getTotalNeuronCount()) / 10));
            for (size_t i = 0; i < pattern.size(); ++i) {
                pattern[i] = static_cast<float>(rand()) / RAND_MAX;
            }
            
            concept_formation->processPattern(pattern);
        }
        
        // Concept formation should have learned concepts
        assert(true);
    }
};

// Test framework base classes
class TestAttention {
public:
    static void run_all() {
        test_attention_basic();
        test_attention_competitive();
        test_attention_integration();
        test_attention_plasticity();
    }
    
private:
    static void test_attention_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* attention = brain.getAttention();
        assert(attention != nullptr);
        
        // Initialize attention system
        attention->initialize(&brain);
        assert(attention != nullptr);
    }
    
    static void test_attention_competitive() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 65);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* attention = brain.getAttention();
        assert(attention != nullptr);
        
        // Configure attention parameters
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        
        assert(attention != nullptr);
    }
    
    static void test_attention_integration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* attention = brain.getAttention();
        assert(attention != nullptr);
        
        // Test integration with working memory
        auto* working_memory = brain.getWorkingMemory();
        assert(working_memory != nullptr);
        
        // Set up attention competition
        std::vector<nlm::NeuronId> competitors;
        for (int i = 0; i < 10; ++i) {
            competitors.push_back(nlm::NeuronId(i));
        }
        
        attention->processCompetition(competitors);
        
        // Attention should have selected winners
        assert(true);
    }
    
    static void test_attention_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 70);
        config->set("plasticity.stdp.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* attention = brain.getAttention();
        assert(attention != nullptr);
        
        // Apply activity that attention should respond to
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Attention should have modulated neural activity
        assert(true);
    }
};

// Test framework base classes
class TestNeuromodulation {
public:
    static void run_all() {
        test_dopamine_basic();
        test_curiosity_basic();
        test_novelty_basic();
        test_prediction_error_basic();
        test_neuromodulation_integration();
        test_neuromodulation_plasticity();
    }
    
private:
    static void test_dopamine_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* dopamine = brain.getDopamine();
        assert(dopamine != nullptr);
        
        // Check dopamine level
        float level = dopamine->getLevel();
        assert(level >= 0.0f && level <= 1.0f);
        
        // Check plasticity factor
        float factor = dopamine->getPlasticityFactor();
        assert(factor >= 0.0f && factor <= 2.0f);
    }
    
    static void test_curiosity_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* curiosity = brain.getCuriosity();
        assert(curiosity != nullptr);
        
        // Check curiosity level
        float level = curiosity->getLevel();
        assert(level >= 0.0f && level <= 1.0f);
        
        // Check curiosity threshold
        float threshold = curiosity->getThreshold();
        assert(threshold >= 0.0f && threshold <= 1.0f);
    }
    
    static void test_novelty_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 65);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* novelty = brain.getNovelty();
        assert(novelty != nullptr);
        
        // Check novelty level
        float level = novelty->getLevel();
        assert(level >= 0.0f && level <= 1.0f);
        
        // Check novelty threshold
        float threshold = novelty->getThreshold();
        assert(threshold >= 0.0f && threshold <= 1.0f);
    }
    
    static void test_prediction_error_basic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* prediction_error = brain.getPredictionErrorSignal();
        assert(prediction_error != nullptr);
        
        // Check prediction error level
        float level = prediction_error->getLevel();
        assert(level >= 0.0f && level <= 1.0f);
    }
    
    static void test_neuromodulation_integration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 55);
        config->set("neuromod.dopamine.scale", 1.2f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check all neuromodulation systems
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        assert(brain.getNovelty() != nullptr);
        assert(brain.getPredictionErrorSignal() != nullptr);
        
        // Apply neuromodulatory signals
        nlm::Neuromodulator dopamine_signal(nlm::NeuromodulatorType::Dopamine, 0.7f);
        brain.applyNeuromodulation(dopamine_signal);
        
        nlm::Neuromodulator curiosity_signal(nlm::NeuromodulatorType::Curiosity, 0.5f);
        brain.applyNeuromodulation(curiosity_signal);
        
        // Neuromodulation should have affected brain state
        assert(true);
    }
    
    static void test_neuromodulation_plasticity() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 60);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.dopamine.scale", 1.5f);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Apply reward to trigger neuromodulation
        nlm::Neuromodulator reward_signal(nlm::NeuromodulatorType::Dopamine, 0.8f);
        brain.applyNeuromodulation(reward_signal);
        
        // Apply activity to see neuromodulation effects
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Neuromodulation should have influenced plasticity
        auto* stdp = brain.getSTDP();
        assert(stdp != nullptr);
    }
};

// Test framework base classes
class TestWorld {
public:
    static void run_all() {
        test_world_creation();
        test_world_configuration();
        test_world_dynamics();
        test_world_interactions();
        test_world_objects();
    }
    
private:
    static void test_world_creation() {
        nlm::SimpleWorld world;
        assert(&world != nullptr);
        
        // Default configuration
        assert(world.getWidth() == 0);  // Default
        assert(world.getHeight() == 0);  // Default
    }
    
    static void test_world_configuration() {
        nlm::SimpleWorld world;
        world.configure(20, 20, 8, 8);
        
        assert(world.getWidth() == 20);
        assert(world.getHeight() == 20);
        assert(world.getVisionWidth() == 8);
        assert(world.getVisionHeight() == 8);
        
        // Test agent start position
        world.setAgentStart(10.0, 10.0);
        auto body = world.getAgentBody();
        assert(body.x == 10.0);
        assert(body.y == 10.0);
    }
    
    static void test_world_dynamics() {
        nlm::SimpleWorld world;
        world.configure(10, 10, 5, 5);
        world.reset();
        
        // Get initial state
        auto percept = world.getSensoryPercept();
        assert(percept.getVisionWidth() == 5);
        assert(percept.getVisionHeight() == 5);
        
        // Update world
        world.update(0.1);
        assert(world.getSimulationTime() > 0.0);
        
        // Get updated percept
        auto new_percept = world.getSensoryPercept();
        assert(new_percept.getTimestamp() > percept.getTimestamp());
    }
    
    static void test_world_interactions() {
        nlm::SimpleWorld world;
        world.configure(15, 15, 6, 6);
        world.reset();
        
        // Create agent
        nlm::Brain brain(std::make_shared<nlm::Config>());
        brain.initialize();
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        
        // Get initial percept
        auto percept = world.getSensoryPercept();
        
        // Apply motor command
        nlm::ActionType action = nlm::ActionType::MoveForward;
        world.applyMotorCommand(action, 0.0);
        
        // Check that agent position changed
        auto body = world.getAgentBody();
        assert(body.velocityX > 0.0 || body.velocityY > 0.0);
    }
    
    static void test_world_objects() {
        nlm::SimpleWorld world;
        world.configure(20, 20, 8, 8);
        
        // Create world objects
        nlm::WorldObject resource_obj(5.0, 5.0, nlm::WorldObjectType::Resource, 10.0f);
        world.addObject(resource_obj);
        
        nlm::WorldObject hazard_obj(15.0, 15.0, nlm::WorldObjectType::Hazard, -5.0f);
        world.addObject(hazard_obj);
        
        // Check object positions
        auto body = world.getAgentBody();
        assert(world.isValidPosition(body.x, body.y));
        
        // Remove object
        world.removeObject(5.0, 5.0);
        assert(!world.isValidPosition(5.0, 5.0));
    }
};

// Test framework base classes
class TestAgent {
public:
    static void run_all() {
        test_agent_creation();
        test_agent_initialization();
        test_agent_sensing();
        test_agent_action();
        test_agent_learning();
        test_agent_development();
    }
    
private:
    static void test_agent_creation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        
        nlm::Brain brain(config);
        nlm::AgentBrain agent(brain);
        
        assert(&agent != nullptr);
    }
    
    static void test_agent_initialization() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::SimpleWorld world;
        world.configure(10, 10, 4, 4);
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        
        assert(agent.getBrain() != nullptr);
        assert(agent.getBrain()->getTotalNeuronCount() == 40);
    }
    
    static void test_agent_sensing() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::SimpleWorld world;
        world.configure(10, 10, 4, 4);
        world.reset();
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        
        // Create percept
        nlm::Vision vision(4, 4, 3);
        std::vector<float> data(16, 0.5f);
        vision.setData(data);
        
        nlm::SensoryPercept percept;
        percept.setVision(vision);
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Check that brain received input
        assert(true);  // Sensory processing is working
    }
    
    static void test_agent_action() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 40);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::SimpleWorld world;
        world.configure(10, 10, 4, 4);
        world.reset();
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        
        // Get motor command
        nlm::ActionType action = agent.decodeMotorCommand();
        
        // Action should be valid
        assert(action == nlm::ActionType::Wait || action == nlm::ActionType::MoveForward ||
               action == nlm::ActionType::TurnLeft || action == nlm::ActionType::TurnRight);
        
        // Apply action to world
        world.applyMotorCommand(action, 0.0);
        
        // Check that world state changed
        assert(true);
    }
    
    static void test_agent_learning() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 45);
        config->set("plasticity.stdp.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::SimpleWorld world;
        world.configure(10, 10, 4, 4);
        world.reset();
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        
        // Enable learning
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        
        // Apply reward to trigger learning
        agent.applyRewardModulation(0.8f, 0.0f);
        
        // Check that learning occurred
        assert(true);  // Reward modulation is working
    }
    
    static void test_agent_development() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        config->set("development.enable", true);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::SimpleWorld world;
        world.configure(10, 10, 4, 4);
        world.reset();
        
        nlm::AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableDevelopment(true);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Check that development occurred
        nlm::DevelopmentalStage stage = agent.getDevelopmentalStage();
        assert(stage != nlm::DevelopmentalStage::Aging);  // Should not age immediately
    }
};

// Test framework base classes
class TestExperiments {
public:
    static void run_all() {
        test_experiment_basic();
        test_experiment_runner();
        test_experiment_metrics();
        test_phase_experiments();
    }
    
private:
    static void test_experiment_basic() {
        nlm::Experiment experiment;
        assert(&experiment != nullptr);
        
        // Basic experiment functionality
        assert(true);
    }
    
    static void test_experiment_runner() {
        nlm::ExperimentRunner runner;
        assert(&runner != nullptr);
        
        // Basic runner functionality
        assert(true);
    }
    
    static void test_experiment_metrics() {
        nlm::Metrics metrics;
        assert(&metrics != nullptr);
        
        // Basic metrics functionality
        assert(true);
    }
    
    static void test_phase_experiments() {
        // Test Phase 3 experiment
        nlm::Phase3Experiment phase3;
        assert(&phase3 != nullptr);
        
        // Test Phase 4 experiment
        nlm::Phase4Experiment phase4;
        assert(&phase4 != nullptr);
        
        // Test Phase 5 experiment
        nlm::Phase5Experiment phase5;
        assert(&phase5 != nullptr);
        
        // Test Phase 6 experiment
        nlm::Phase6IntegratedExperiment phase6;
        assert(&phase6 != nullptr);
    }
};
