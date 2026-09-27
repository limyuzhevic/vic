// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
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
    if (!success) {
        throw nlm::NLMTestException("Brain initialization test failed: initialize() returned false");
    }
    
    // Verify brain initialized with expected properties
    if (brain.getRegionCount() != 2) {
        throw nlm::NLMTestException("Brain initialization test failed: expected 2 regions, got " + 
                                   std::to_string(brain.getRegionCount()));
    }
    if (brain.getTotalNeuronCount() != 100) {
        throw nlm::NLMTestException("Brain initialization test failed: expected 100 neurons, got " + 
                                   std::to_string(brain.getTotalNeuronCount()));
    }
    if (brain.getTotalSynapseCount() <= 0) {
        throw nlm::NLMTestException("Brain initialization test failed: expected > 0 synapses, got " + 
                                   std::to_string(brain.getTotalSynapseCount()));
    }
    
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
    if (brain.getTotalNeuronCount() != 100) {
        throw nlm::NLMTestException("Brain step test failed: expected 100 neurons, got " + 
                                   std::to_string(brain.getTotalNeuronCount()));
    }
    
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
    if (brain.getTotalNeuronCount() != 50) {
        throw nlm::NLMTestException("Brain reset test failed: expected 50 neurons after reset, got " + 
                                   std::to_string(brain.getTotalNeuronCount()));
    }
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    if (regionIds.size() != 3) {
        throw nlm::NLMTestException("Brain regions test failed: expected 3 regions, got " + 
                                   std::to_string(regionIds.size()));
    }
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        if (region == nullptr) {
            throw nlm::NLMTestException("Brain regions test failed: getRegion returned nullptr for region ID");
        }
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
    if (action == nullptr) {
        throw nlm::NLMTestException("Brain action production test failed: produceAction returned nullptr");
    }
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    if (brain.getTotalNeuronCount() <= 0) {
        throw nlm::NLMTestException("Brain status test failed: total neuron count should be > 0, got " + 
                                   std::to_string(brain.getTotalNeuronCount()));
    }
    if (brain.getTotalSynapseCount() < 0) {
        throw nlm::NLMTestException("Brain status test failed: total synapse count should be >= 0, got " + 
                                   std::to_string(brain.getTotalSynapseCount()));
    }
    if (brain.getFiringNeuronCount() < 0) {
        throw nlm::NLMTestException("Brain status test failed: firing neuron count should be >= 0, got " + 
                                   std::to_string(brain.getFiringNeuronCount()));
    }
    if (brain.getAverageFiringRate() < 0.0f) {
        throw nlm::NLMTestException("Brain status test failed: average firing rate should be >= 0.0, got " + 
                                   std::to_string(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
}

} // namespace test_brain
