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
    if (!success) {
        std::cout << "    ERROR: Brain initialization failed" << std::endl;
        return;
    }
    
    // Verify brain initialized with expected properties
    if (brain.getRegionCount() != 2) {
        std::cout << "    ERROR: Expected 2 regions, got " << brain.getRegionCount() << std::endl;
        return;
    }
    if (brain.getTotalNeuronCount() != 100) {
        std::cout << "    ERROR: Expected 100 neurons, got " << brain.getTotalNeuronCount() << std::endl;
        return;
    }
    if (brain.getTotalSynapseCount() <= 0) {
        std::cout << "    ERROR: Expected > 0 synapses, got " << brain.getTotalSynapseCount() << std::endl;
        return;
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
        std::cout << "    ERROR: Expected 100 neurons, got " << brain.getTotalNeuronCount() << std::endl;
        return;
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
        std::cout << "    ERROR: Expected 50 neurons after reset, got " << brain.getTotalNeuronCount() << std::endl;
        return;
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
        std::cout << "    ERROR: Expected 3 regions, got " << regionIds.size() << std::endl;
        return;
    }
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        if (region == nullptr) {
            std::cout << "    ERROR: Region not found" << std::endl;
            return;
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
        std::cout << "    ERROR: Action is nullptr" << std::endl;
        return;
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
        std::cout << "    ERROR: Expected > 0 neurons, got " << brain.getTotalNeuronCount() << std::endl;
        return;
    }
    if (brain.getTotalSynapseCount() < 0) {
        std::cout << "    ERROR: Expected >= 0 synapses, got " << brain.getTotalSynapseCount() << std::endl;
        return;
    }
    if (brain.getFiringNeuronCount() < 0) {
        std::cout << "    ERROR: Expected >= 0 firing neurons, got " << brain.getFiringNeuronCount() << std::endl;
        return;
    }
    if (brain.getAverageFiringRate() < 0.0f) {
        std::cout << "    ERROR: Expected >= 0.0 average firing rate, got " << brain.getAverageFiringRate() << std::endl;
        return;
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
