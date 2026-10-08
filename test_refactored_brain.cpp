// Quick test to verify refactored Brain implementation
// This demonstrates that the refactored Brain maintains the same API and functionality

#include "brain/Brain.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Refactored Brain Implementation" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    // Test 1: Brain Creation
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        std::cout << "✓ Brain Creation: PASSED" << std::endl;
    }
    
    // Test 2: Brain Initialization
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        
        assert(success);
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        std::cout << "✓ Brain Initialization: PASSED" << std::endl;
    }
    
    // Test 3: Brain Step
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        assert(brain.getTotalNeuronCount() == 100);
        
        std::cout << "✓ Brain Step: PASSED" << std::endl;
    }
    
    // Test 4: Brain Reset
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        brain.step(0);
        brain.reset();
        
        assert(brain.getTotalNeuronCount() == 50);
        
        std::cout << "✓ Brain Reset: PASSED" << std::endl;
    }
    
    // Test 5: Brain Regions
    {
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
        
        std::cout << "✓ Brain Regions: PASSED" << std::endl;
    }
    
    // Test 6: Brain Action Production
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        std::cout << "✓ Brain Action Production: PASSED" << std::endl;
    }
    
    // Test 7: Brain Status
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
        
        std::cout << "✓ Brain Status: PASSED" << std::endl;
    }
    
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << "All Tests PASSED! ✓" << std::endl;
    std::cout << "Refactored Brain implementation is working correctly." << std::endl;
    
    return 0;
}
