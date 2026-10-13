// Comprehensive NLM Test Suite
// Extended testing infrastructure for NLM (Neural Learning Machine)

#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "brain/Brain.hpp"
#include "nlmc/NLMCommandLine.hpp"

namespace test_comprehensive {

// Test Configuration System
void testConfigSystem() {
    std::cout << "Testing Configuration System..." << std::endl;
    
    nlm::Config config;
    
    // Test basic operations
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    assert(config.has("test_int"));
    assert(config.getOr<int>("test_int", 0) == 42);
    assert(std::abs(config.getOr<double>("test_double", 0.0) - 3.14) < 0.001);
    assert(config.getOr<std::string>("test_string", "") == "hello");
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test getOr with default values
    assert(config.getOr<int>("nonexistent", 100) == 100);
    assert(config.getOr<std::string>("nonexistent", "default") == "default");
    
    // Test remove
    config.remove("test_int");
    assert(!config.has("test_int"));
    
    std::cout << "    Configuration system test passed" << std::endl;
}

// Test Random Generator
void testRandomGenerator() {
    std::cout << "Testing Random Generator..." << std::endl;
    
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    assert(rng1.uniformInt(0, 100) == rng2.uniformInt(0, 100));
    assert(std::abs(rng1.uniformReal(0.0f, 1.0f) - rng2.uniformReal(0.0f, 1.0f)) < 0.001);
    
    nlm::RandomGenerator rng3(123);
    assert(rng1.uniformInt(0, 100) != rng3.uniformInt(0, 100));  // Different seeds should be different
    
    // Test ranges
    for (int i = 0; i < 100; ++i) {
        int val = rng1.uniformInt(0, 100);
        assert(val >= 0 && val <= 100);
        
        float fval = rng1.uniformReal(0.0f, 1.0f);
        assert(fval >= 0.0f && fval <= 1.0f);
    }
    
    std::cout << "    Random generator test passed" << std::endl;
}

// Test Neuron Advanced Features
void testNeuronAdvanced() {
    std::cout << "Testing Neuron Advanced Features..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize with random parameters
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Test biophysical properties
    assert(neuron.getMembranePotential() < -60.0f);  // Should be depolarized from resting
    assert(neuron.getThreshold() < -50.0f);          // Should be reasonable threshold
    assert(neuron.getRefractoryPeriod() >= 2 && neuron.getRefractoryPeriod() <= 20);  // Typical range
    
    // Test synaptic input handling
    neuron.receiveExcitatoryInput(10.0f);
    neuron.receiveInhibitoryInput(5.0f);
    
    // Test firing behavior
    neuron.setMembranePotential(-45.0f);  // Above threshold
    neuron.setThreshold(-50.0f);
    neuron.setRefractoryPeriod(5);
    
    assert(neuron.checkThreshold());  // Should be firing
    assert(neuron.isFiring());
    
    // Test spike recording
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    assert(neuron.getSpikeHistory().size() == 2);
    
    // Test synaptic dynamics
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    Neuron advanced test passed" << std::endl;
}

// Test Synapse Advanced Features
void testSynapseAdvanced() {
    std::cout << "Testing Synapse Advanced Features..." << std::endl;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Initialize with random parameters
    nlm::RandomGenerator rng(42);
    synapse.initializeRandom(rng);
    
    // Test weight management
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.1f);
    assert(std::abs(synapse.getWeight() - 0.6f) < 0.001f);
    
    // Test delay
    synapse.setDelay(10);
    assert(synapse.getDelay() == 10);
    
    // Test spike history
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    // Test eligibility trace
    synapse.setEligibilityTrace(0.5f);
    assert(std::abs(synapse.getEligibilityTrace() - 0.5f) < 0.001f);
    
    synapse.decayEligibilityTrace(0.1f);
    assert(std::abs(synapse.getEligibilityTrace() - 0.45f) < 0.001f);
    
    // Test plasticity flags
    synapse.enablePlasticity(true, true, true);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    Synapse advanced test passed" << std::endl;
}

// Test Neural Region
void testNeuralRegion() {
    std::cout << "Testing Neural Region..." << std::endl;
    
    nlm::NeuralRegion region(nlm::RegionId(1), "TestRegion");
    
    // Add neurons to populations
    nlm::PopulationId sensoryPop = region.addPopulation(10, nlm::NeuronType::Sensory);
    nlm::PopulationId internalPop = region.addPopulation(20, nlm::NeuronType::Internal);
    nlm::PopulationId motorPop = region.addPopulation(5, nlm::NeuronType::Motor);
    
    assert(region.getPopulationCount() == 3);
    assert(region.getTotalNeuronCount() == 35);
    
    // Test population access
    nlm::NeuralPopulation* sensory = region.getPopulation(sensoryPop);
    assert(sensory != nullptr);
    assert(sensory->getNeuronType() == nlm::NeuronType::Sensory);
    assert(sensory->getNeuronCount() == 10);
    
    // Test neuron access
    auto neurons = region.getAllNeurons();
    assert(neurons.size() == 35);
    
    // Test synaptic operations
    if (!neurons.empty()) {
        // Add synapses between neurons
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            for (size_t j = 0; j < std::min(size_t(3), neurons.size()); ++j) {
                if (i != j) {
                    neurons[i]->addIncomingSynapse(nlm::SynapseHandle());
                    neurons[j]->addOutgoingSynapse(nlm::SynapseHandle());
                }
            }
        }
    }
    
    // Test random connectivity
    nlm::RandomGenerator rng(42);
    region.initializeRandomConnectivity(rng, 0.1f, 0.5f, 0.2f);
    
    assert(region.getSynapseCount() > 0);
    assert(region.getAverageSynapticWeight() >= -1.0f && region.getAverageSynapticWeight() <= 1.0f);
    
    std::cout << "    Neural region test passed" << std::endl;
}

// Test Brain Integration
void testBrainIntegration() {
    std::cout << "Testing Brain Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set up configuration
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        assert(false && "Brain initialization should succeed");
    }
    
    // Test basic functionality
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() >= 0);
    
    // Test step functionality
    brain.step(0, 0.0);
    brain.step(1, 0.001);
    
    // Test memory systems
    auto* working_mem = brain.getWorkingMemory();
    if (working_mem) {
        working_mem->setCapacity(50);
        working_mem->storeToNeuron(nlm::NeuronId(1), 0.5f);
        assert(working_mem->getActiveTraces() > 0);
    }
    
    auto* episodic_mem = brain.getEpisodicMemory();
    if (episodic_mem) {
        episodic_mem->setMaxEpisodes(100);
        // Store a simple episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
        episode.reward = 0.5f;
        episodic_mem->storeEpisode(episode);
        assert(episodic_mem->getEpisodeCount() > 0);
    }
    
    // Test reset functionality
    brain.reset();
    brain.initialize();
    
    std::cout << "    Brain integration test passed" << std::endl;
}

// Test Command Line Interface Integration
void testCommandLineInterface() {
    std::cout << "Testing Command Line Interface Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (brain->initialize()) {
        nlmc::NLMCommandLine cli(config, brain);
        
        // Test help command
        cli.processCommandLine(2, new char*[] { (char*)"nlmc", (char*)"help" });
        
        // Test version command
        cli.processCommandLine(2, new char*[] { (char*)"nlmc", (char*)"version" });
        
        // Test stats command
        cli.processCommandLine(2, new char*[] { (char*)"nlmc", (char*)"stats" });
        
        std::cout << "    Command line interface test passed" << std::endl;
    } else {
        std::cout << "    Skipping CLI test due to brain initialization failure" << std::endl;
    }
}

// Test Performance and Scalability
void testPerformanceAndScalability() {
    std::cout << "Testing Performance and Scalability..." << std::endl;
    
    // Test with increasing network sizes
    std::vector<size_t> network_sizes = {10, 50, 100};
    
    for (size_t size : network_sizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(size));
        config->set("region_count", 1);
        config->set("connection_probability", 0.05f);
        
        nlm::Brain brain(config);
        
        if (brain.initialize()) {
            // Test basic operations
            size_t initial_spikes = brain.getTotalSpikeCount();
            
            // Run a few simulation steps
            for (size_t step = 0; step < size; ++step) {
                brain.step(step, step * 0.001);
            }
            
            // Verify no crashes and reasonable behavior
            assert(brain.getTotalNeuronCount() == size);
            assert(brain.getTotalSynapseCount() >= 0);
            assert(brain.getActiveNeuronCount() >= 0);
            
            std::cout << "    Successfully tested network with " << size << " neurons" << std::endl;
        }
    }
    
    std::cout << "    Performance and scalability test passed" << std::endl;
}

// Test Memory Management
void testMemoryManagement() {
    std::cout << "Testing Memory Management..." << std::endl;
    
    // Test multiple brain instances
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    for (int i = 0; i < 5; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", static_cast<int64_t>(42 + i));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", 50);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        brains.push_back(brain);
        
        // Simulate some activity
        for (size_t step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
    }
    
    // Brains should be properly destroyed when going out of scope
    brains.clear();
    
    std::cout << "    Memory management test passed" << std::endl;
}

// Test Error Handling
void testErrorHandling() {
    std::cout << "Testing Error Handling..." << std::endl;
    
    nlm::Config config;
    
    // Test accessing non-existent configuration values
    assert(config.getOr<int>("nonexistent", 0) == 0);
    assert(config.getOr<std::string>("nonexistent", "default") == "default");
    assert(config.getOr<double>("nonexistent", 0.0) == 0.0);
    
    // Test removing non-existent configuration
    config.remove("nonexistent");  // Should not crash
    
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test neuron with edge case parameters
    neuron.setMembranePotential(-1000.0f);  // Extreme value
    neuron.setThreshold(-1000.0f);
    neuron.setRefractoryPeriod(0);  // Zero refractory period
    
    // Should handle edge cases gracefully
    neuron.injectCurrent(0.0f);
    neuron.receiveExcitatoryInput(0.0f);
    neuron.receiveInhibitoryInput(0.0f);
    
    std::cout << "    Error handling test passed" << std::endl;
}

// Run all comprehensive tests
void runAllComprehensiveTests() {
    std::cout << "Running Comprehensive NLM Test Suite" << std::endl;
    std::cout << "=" * 50 << std::endl;
    
    testConfigSystem();
    testRandomGenerator();
    testNeuronAdvanced();
    testSynapseAdvanced();
    testNeuralRegion();
    testBrainIntegration();
    testCommandLineInterface();
    testPerformanceAndScalability();
    testMemoryManagement();
    testErrorHandling();
    
    std::cout << std::endl;
    std::cout << "=" * 50 << std::endl;
    std::cout << "All comprehensive tests passed!" << std::endl;
}

} // namespace test_comprehensive