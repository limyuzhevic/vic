// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        
        std::cout << "    testBrainCreation passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainCreation FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testBrainCreationWithNullConfig() {
    try {
        // Test with null config - should handle gracefully
        nlm::Brain brain(nullptr);
        std::cerr << "    testBrainCreationWithNullConfig FAILED: Should have thrown exception" << std::endl;
        assert(false);
    } catch (const std::exception& e) {
        std::cout << "    testBrainCreationWithNullConfig passed (threw as expected)" << std::endl;
    }
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        
        bool success = brain.initialize();
        assert(success);
        
        // Verify brain initialized with expected properties
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        std::cout << "    testBrainInitialization passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainInitialization FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testBrainInitializationValidation() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test invalid neuron_count
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(!success); // Should fail with invalid neuron_count
    
    std::cout << "    testBrainInitializationValidation passed (properly rejected invalid config)" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run a few steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Just verify it doesn't crash
        assert(brain.getTotalNeuronCount() == 100);
        
        std::cout << "    testBrainStep passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainStep FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        brain.initialize();
        
        brain.step(0);
        brain.reset();
        
        // Verify reset worked
        assert(brain.getTotalNeuronCount() == 50);
        
        std::cout << "    testBrainReset passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainReset FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify regions
        assert(brain.getRegionCount() == 3);
        
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 3);
        
        std::cout << "    testBrainRegions passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainRegions FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        brain.initialize();
        
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        std::cout << "    testBrainActionProduction passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainActionProduction FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    try {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify statistics are accessible
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        
        std::cout << "    testBrainStatus passed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "    testBrainStatus FAILED: " << e.what() << std::endl;
        throw;
    }
}

void runAll() {
    testBrainCreation();
    testBrainCreationWithNullConfig();
    testBrainInitialization();
    testBrainInitializationValidation();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
}

} // namespace test_brain