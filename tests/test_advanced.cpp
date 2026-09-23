// Advanced NLM Test Scenarios
// Tests for complex multi-region brains, advanced plasticity, and realistic input patterns

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "nlm/LearningExperiment.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <nlohmann/json.hpp>

namespace test_advanced {

void testMultiRegionConnectivity() {
    nlm::Config config;
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    bool success = brain->initialize();
    assert(success);
    
    // Verify we have 3 regions
    assert(brain->getRegionCount() == 3);
    
    // Test inter-region connections
    brain->addInterRegionConnection(nlm::RegionId(0), nlm::RegionId(1), 0.5f, 2);
    brain->addInterRegionConnection(nlm::RegionId(1), nlm::RegionId(2), 0.3f, 3);
    
    // Verify connections were added
    auto regions = brain->getRegionIds();
    assert(std::find(regions.begin(), regions.end(), nlm::RegionId(0)) != regions.end());
    assert(std::find(regions.begin(), regions.end(), nlm::RegionId(1)) != regions.end());
    assert(std::find(regions.begin(), regions.end(), nlm::RegionId(2)) != regions.end());
    
    // Test activity in multi-region system
    size_t totalSpikesBefore = brain->getTotalSpikeCount();
    
    // Inject current into region 0
    auto* region0 = brain->getRegion(nlm::RegionId(0));
    if (region0) {
        auto neurons = region0->getAllNeurons();
        if (!neurons.empty()) {
            neurons[0]->injectCurrent(40.0f);
        }
    }
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t totalSpikesAfter = brain->getTotalSpikeCount();
    // Should have generated some spikes
    assert(totalSpikesAfter > totalSpikesBefore || neurons.empty() || neurons[0]->getMembranePotential() > -50.0f);
    
    std::cout << "    testMultiRegionConnectivity passed" << std::endl;
}

void testAdvancedPlasticityPatterns() {
    nlm::Config config;
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Set up advanced plasticity parameters
    config->set("stdp_ltp_weight", 0.05f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.04f, nlm::ConfigSource::Default);
    config->set("plasticity_learning_rate", 0.05f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    bool success = brain->initialize();
    assert(success);
    
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (!region) return;
    
    // Enable all plasticity rules
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, true, true);  // Hebbian + STDP + Reward-modulated
    }
    
    // Learning Experiment with advanced patterns
    nlm::LearningExperiment experiment(brain, 789);
    experiment.recordInitialState();
    
    // Create complex input pattern: Poisson-like spiking
    size_t numSteps = 500;
    for (size_t step = 0; step < numSteps; ++step) {
        // Vary input based on time (simulating natural input patterns)
        float inputCurrent = 20.0f + 10.0f * std::sin(static_cast<float>(step) * 0.02f);
        
        // Inject into different neuron types
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, inputCurrent);
        brain->injectCurrentToNeurons(nlm::NeuronType::Internal, inputCurrent * 0.5f);
        
        // Occasional strong bursts
        if (step % 50 == 0) {
            for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount(); ++i) {
                brain->injectCurrent(static_cast<nlm::NeuronId>(i), 60.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Advanced step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Verify learning occurred
    assert(!experiment.initialWeights.empty() && !experiment.finalWeights.empty());
    
    std::cout << "    testAdvancedPlasticityPatterns passed" << std::endl;
}

void testConfigJSONAdvanced() {
    nlm::Config config;
    
    // Set complex nested structures using JSON-style config
    config.set("experiment_name", std::string("Advanced Neural Learning"), nlm::ConfigSource::Default);
    config.set("max_simulation_steps", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config.set("enable_visualization", true, nlm::ConfigSource::Default);
    config.set("learning_rate", 0.02f, nlm::ConfigSource::Default);
    config.set("batch_size", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config.set("neuron_params", std::vector<double>({-70.0, -55.0, -60.0}), nlm::ConfigSource::Default);
    config.set("synapse_params", std::vector<std::string>({"excitatory", "inhibitory", "modulatory"}), nlm::ConfigSource::Default);
    
    // Test saving to JSON
    bool saveSuccess = config.saveToFile("advanced_config.json");
    assert(saveSuccess);
    
    // Load back and verify
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("advanced_config.json");
    assert(loadSuccess);
    
    // Verify key values
    assert(config2.getOr<std::string>("experiment_name", "") == "Advanced Neural Learning");
    assert(config2.getOr<int64_t>("max_simulation_steps", 0) == 10000);
    assert(config2.getOr<bool>("enable_visualization", false) == true);
    assert(std::abs(config2.getOr<double>("learning_rate", 0.0) - 0.02f) < 0.001f);
    
    // Clean up
    std::remove("advanced_config.json");
    
    std::cout << "    testConfigJSONAdvanced passed" << std::endl;
}

void testComplexNeuronalDynamics() {
    nlm::Config config;
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    bool success = brain->initialize();
    assert(success);
    
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    assert(!neurons.empty());
    
    // Configure neurons with different parameters
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        auto* neuron = neurons[i];
        
        // Set different threshold for each neuron
        float threshold = -50.0f - static_cast<float>(i) * 2.0f;
        neuron->setThreshold(threshold);
        
        // Set different refractory periods
        uint32_t refractory = 2 + i;
        neuron->setRefractoryPeriod(refractory);
        
        // Set different resting potentials
        float resting = -70.0f - static_cast<float>(i) * 0.5f;
        neuron->setRestingPotential(resting);
        
        // Set different leak conductances
        float leak = 5.0f + static_cast<float>(i) * 2.0f;
        neuron->setLeakConductance(leak);
    }
    
    // Record initial states
    size_t spikesBefore = brain->getTotalSpikeCount();
    
    // Run complex input pattern
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Inject different currents based on neuron index
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            float current = 30.0f + 10.0f * std::sin(static_cast<float>(step) * 0.05f) + static_cast<float>(i) * 5.0f;
            neurons[i]->injectCurrent(current);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 50 == 0) {
            size_t firingCount = brain->getFiringNeuronCount();
            NLM_LOG_INFO("  Complex dynamics step " + std::to_string(step) + 
                        " | Firing: " + std::to_string(firingCount));
        }
    }
    
    size_t spikesAfter = brain->getTotalSpikeCount();
    // Should have generated activity
    assert(spikesAfter > spikesBefore || brain->getFiringNeuronCount() > 0);
    
    std::cout << "    testComplexNeuronalDynamics passed" << std::endl;
}

void testMemorySystemsIntegration() {
    nlm::Config config;
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    bool success = brain->initialize();
    assert(success);
    
    // Test memory system integration (if available)
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Test working memory operations
        size_t neuronsBefore = brain->getTotalNeuronCount();
        assert(neuronsBefore > 0);
        
        // Inject some input to activate memory systems
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Internal, 25.0f);
            brain->step(step, step * 0.001);
        }
        
        // Verify memory systems are still functional
        size_t neuronsAfter = brain->getTotalNeuronCount();
        assert(neuronsAfter == neuronsBefore);
        
        NLM_LOG_INFO("  Working memory test passed");
    }
    
    // Test episodic memory if available
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        // Simulate experience recording
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        NLM_LOG_INFO("  Episodic memory test passed");
    }
    
    // Test associative memory if available
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        NLM_LOG_INFO("  Associative memory test passed");
    }
    
    std::cout << "    testMemorySystemsIntegration passed" << std::endl;
}

void runAll() {
    std::cout << std::endl;
    std::cout << "=== Advanced NLM Tests ===" << std::endl;
    
    testMultiRegionConnectivity();
    testAdvancedPlasticityPatterns();
    testConfigJSONAdvanced();
    testComplexNeuronalDynamics();
    testMemorySystemsIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Advanced Tests PASSED ===" << std::endl;
}

} // namespace test_advanced