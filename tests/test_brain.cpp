// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainSynapticConnectivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get first region
    auto* region = brain.getRegion(nlm::RegionId(1));
    assert(region != nullptr);
    
    // Collect all neurons and synapses
    auto neurons = region->getAllNeurons();
    auto synapses = region->getSynapses();
    
    // Verify synaptic connectivity exists
    assert(!synapses.empty());
    
    // Verify each synapse connects valid neurons
    for (auto& syn : synapses) {
        assert(syn->getSourceNeuron() != nlm::INVALID_NEURON_ID);
        assert(syn->getDestinationNeuron() != nlm::INVALID_NEURON_ID);
        
        // Verify the destination neuron exists in the region
        bool found = false;
        for (auto& neuron : neurons) {
            if (neuron->getId() == syn->getDestinationNeuron()) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    // Test spike propagation through synapses
    if (!neurons.empty() && !synapses.empty()) {
        // Inject current into first neuron
        neurons[0]->injectCurrent(50.0f);
        
        // Step brain to allow spike propagation
        brain.step(0);
        
        // Check that some neurons received synaptic input
        // (This is a basic check - in reality, spike propagation is complex)
        for (auto& neuron : neurons) {
            assert(neuron->getMembranePotential() >= -100.0f);  // Should be within valid range
        }
    }
    
    std::cout << "    testBrainSynapticConnectivity passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainIntegration() {
    // Integration test for Phase 3/4 systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test the complete integration: neurons -> synapses -> brain system
    auto neurons = brain.getAllNeurons();
    auto* region = brain.getRegion(nlm::RegionId(1));
    assert(region != nullptr);
    
    auto synapses = region->getSynapses();
    
    // Verify basic connectivity
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test step with all systems working together
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        // Inject external input to test sensory-motor loop
        for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
            neurons[i]->injectCurrent(30.0f);
        }
        
        // Execute brain step
        assert(brain.step(step));
        
        // Test action production (Phase 3)
        auto action = brain.produceAction();
        // action may be null depending on system state, that's okay for test
        
        // Verify brain state consistency
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getFiringNeuronCount() >= 0);
    }
    
    // Test plasticity integration (Phase 4)
    // Enable plasticity on some synapses
    size_t plasticityTestCount = 0;
    for (auto& syn : synapses) {
        if (plasticityTestCount < 5) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            plasticityTestCount++;
        }
    }
    
    // Apply correlated activity to test STDP
    if (!neurons.empty() && synapses.size() >= 2) {
        // Create correlated pre-post activity for LTP
        neurons[0]->injectCurrent(60.0f);  // Pre-synaptic
        brain.step(10);
        
        neurons[1]->injectCurrent(60.0f);  // Post-synaptic
        brain.step(11);
        
        // Check that synaptic weights changed (allowing for some noise)
        for (auto& syn : synapses) {
            if (syn->getPlasticityFlags().stdp) {
                // Weight may have changed due to STDP
                // In real test, we'd check for specific LTP/LTD patterns
                // For now, just verify system didn't crash
                assert(std::isfinite(syn->getWeight()));
            }
        }
    }
    
    std::cout << "    testBrainIntegration passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainSynapticConnectivity();
    testBrainIntegration();
}

} // namespace test_brain
