// Integration Tests Implementation
// 
// This file implements the integration test runners for all categories of integration tests.

#include <iostream>
#include <cassert>
#include "IntegrationTestFramework.hpp"

namespace nlm {
namespace integration_tests {

void runAllIntegrationTests() {
    std::cout << "  Running Integration Test Framework tests..." << std::endl;
    
    // Test base classes and utilities
    class SimpleIntegrationTest : public IntegrationTestBase {
    public:
        void setupConfiguration() override {
            config_->set("neuron_count", 10);
            config_->set("region_count", 1);
            config_->set("simulation_timestep", 0.001);
            config_->set("random_seed", 99999u);
        }
        
        void setupSystem() override {
            brain_ = std::make_shared<Brain>(config_);
            assertCondition(brain_->initialize(), "Brain initialization failed");
        }
        
        void runTest() override {
            // Simple test to verify framework works
            assertCondition(brain_->getTotalNeuronCount() == 10, "Neuron count should match config");
            std::cout << "    Integration test passed" << std::endl;
        }
    };
    
    SimpleIntegrationTest test;
    test.run();
    
    SystemStateSnapshot snapshot1;
    snapshot1.capture(*test.getBrain());
    
    SystemStateSnapshot snapshot2;
    snapshot2.capture(*test.getBrain());
    
    assert(snapshot1.equals(snapshot2)); // Should be equal for this simple test
    
    std::cout << "  Integration Test Framework tests PASSED" << std::endl;
}

} // namespace integration_tests
} // namespace nlm
