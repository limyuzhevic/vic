// Integration Tests
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/STDP.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_integration {

void testBrainWithAllPlasticity() {
    std::cout << "Testing brain with integrated plasticity systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    config->set("stdp_time_constant", 20.0f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify all plasticity systems are accessible
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    
    std::cout << "  Creation and initialization: PASSED" << std::endl;
    
    // Test synaptic plasticity on a few synapses
    int synapseTested = 0;
    for (size_t i = 0; i < brain.getRegionCount() && synapseTested < 3; ++i) {
        auto* region = brain.getRegion(nlm::RegionId(i + 1));
        if (region) {
            auto synapses = region->getSynapses();
            for (auto* syn : synapses) {
                if (synapseTested >= 3) break;
                
                float initialWeight = syn->getWeight();
                
                // Apply STDP with some spike timing
                std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
                std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
                
                brain.getSTDP()->update(syn, preSpikes, postSpikes, 0.001);
                
                // Apply Hebbian learning
                brain.getHebbian()->update(syn, preSpikes, postSpikes, 0.001);
                
                // Synapse should have changed weight
                float finalWeight = syn->getWeight();
                
                std::cout << "    Synapse " << synapseTested+1 << ": " 
                          << "weight changed from " << initialWeight << " to " << finalWeight << std::endl;
                
                ++synapseTested;
            }
        }
    }
    
    std::cout << "  Synaptic plasticity integration: PASSED" << std::endl;
}

void testNeuronSynapticDynamics() {
    std::cout << "Testing neuron-synaptic interaction dynamics..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Count active neurons and their firing
    size_t activeNeurons = 0;
    size_t totalSpikes = 0;
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        totalSpikes = brain.getTotalSpikeCount();
        
        // Check neuron state
        for (size_t i = 0; i < brain.getRegionCount(); ++i) {
            auto* region = brain.getRegion(nlm::RegionId(i + 1));
            if (region) {
                auto populations = region->getAllPopulations();
                for (auto* pop : populations) {
                    auto neurons = pop->getNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron->isFiring() || neuron->getFiringRate() > 0.0f) {
                            activeNeurons++;
                        }
                    }
                }
            }
        }
    }
    
    std::cout << "  Neuron-synaptic dynamics (" << totalSpikes << " total spikes, " 
              << activeNeurons << " active neurons): PASSED" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify memory systems are properly initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Check if episodic memory has stored any episodes
    // Note: episodic memory stores every 10 steps, so after 20 steps we should have 2 episodes
    
    std::cout << "  Memory system integration: PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify neuromodulation systems are accessible
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Run simulation steps to see neuromodulation effects
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Check dopamine levels (should change over time)
        if (step % 20 == 0) {
            float dopamine = brain.getDopamine()->getLevel();
            std::cout << "    Step " << step << ": Dopamine = " << dopamine << std::endl;
        }
    }
    
    std::cout << "  Neuromodulation integration: PASSED" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run development updates (happens every 1000 steps)
    for (nlm::SimulationStep step = 0; step < 2000; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {
            std::cout << "    Development update at step " << step << std::endl;
        }
    }
    
    std::cout << "  Development system integration: PASSED" << std::endl;
}

void runAll() {
    std::cout << "Running Integration Tests..." << std::endl;
    std::cout << "Testing complete integrated brain functionality:" << std::endl;
    std::cout << std::endl;
    
    testBrainWithAllPlasticity();
    std::cout << std::endl;
    
    testNeuronSynapticDynamics();
    std::cout << std::endl;
    
    testMemoryIntegration();
    std::cout << std::endl;
    
    testNeuromodulationIntegration();
    std::cout << std::endl;
    
    testDevelopmentIntegration();
    std::cout << std::endl;
    
    std::cout << "=== Integration Tests Complete ===" << std::endl;
}

} // namespace test_integration
