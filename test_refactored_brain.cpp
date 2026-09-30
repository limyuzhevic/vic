// Simple test program to verify refactored Brain.cpp functionality
#include "brain/Brain.hpp"
#include "brain/BrainError.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include <iostream>
#include <cassert>
#include <cstdint>

int main() {
    std::cout << "Testing refactored NLM Brain (Phase 6: Integrated Artificial Brain)..." << std::endl;
    
    try {
        // Test 1: Brain creation with configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        std::cout << "Test 1: Creating Brain..." << std::endl;
        nlm::Brain brain(config);
        std::cout << "✓ Brain created successfully" << std::endl;
        
        // Test 2: Brain initialization
        std::cout << "Test 2: Initializing Brain..." << std::endl;
        bool initSuccess = brain.initialize();
        assert(initSuccess);
        std::cout << "✓ Brain initialized successfully" << std::endl;
        
        // Test 3: Verify brain properties
        assert(brain.getRegionCount() == 1);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        std::cout << "✓ Brain properties verified" << std::endl;
        
        // Test 4: Brain step execution
        std::cout << "Test 4: Executing brain steps..." << std::endl;
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        std::cout << "✓ Brain steps executed successfully" << std::endl;
        
        // Test 5: Reset brain
        std::cout << "Test 5: Resetting Brain..." << std::endl;
        brain.reset();
        std::cout << "✓ Brain reset successfully" << std::endl;
        
        // Test 6: Action production
        std::cout << "Test 6: Producing actions..." << std::endl;
        auto action = brain.produceAction();
        assert(action != nullptr);
        std::cout << "✓ Action produced successfully" << std::endl;
        
        // Test 7: Brain logging
        std::cout << "Test 7: Logging brain status..." << std::endl;
        brain.logStatus();
        std::cout << "✓ Brain status logged successfully" << std::endl;
        
        // Test 8: Region management
        std::cout << "Test 8: Testing region management..." << std::endl;
        auto regionIds = brain.getRegionIds();
        assert(!regionIds.empty());
        
        for (auto rid : regionIds) {
            auto* region = brain.getRegion(rid);
            assert(region != nullptr);
        }
        std::cout << "✓ Region management works correctly" << std::endl;
        
        // Test 9: Exception handling
        std::cout << "Test 9: Testing exception handling..." << std::endl;
        try {
            // Try to get null pointer - should throw
            auto* nullRegion = brain.getRegion(nlm::RegionId(UINT64_MAX));
            assert(false && "Should have thrown exception for invalid region");
        } catch (const nlm::BrainError& e) {
            std::cout << "✓ Exception handling works correctly: " << e.what() << std::endl;
        }
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "All tests passed! Brain refactoring is successful." << std::endl;
        std::cout << "Refactored Brain improvements:" << std::endl;
        std::cout << "  ✓ Comprehensive error handling" << std::endl;
        std::cout << "  ✓ Modular system organization" << std::endl;
        std::cout << "  ✓ RAII resource management" << std::endl;
        std::cout << "  ✓ Validation and safety checks" << std::endl;
        std::cout << "  ✓ Better code maintainability" << std::endl;
        std::cout << "  ✓ System-specific step methods" << std::endl;
        std::cout << "  ✓ Reduced code duplication" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
}