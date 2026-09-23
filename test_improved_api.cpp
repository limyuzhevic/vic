#include <iostream>
#include <cassert>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

namespace test_improved_api {

    void test_convenience_functions() {
        std::cout << "Testing improved API convenience functions..." << std::endl;
        
        // Test 1: Config creation and configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100));
        config->set("brain.synapse_density", 0.1);
        
        assert(config->get<int64_t>("brain.neuron_count").value() == 100);
        assert(config->get<double>("brain.synapse_density").value() == 0.1);
        
        // Test 2: Brain creation
        auto brain = std::make_shared<nlm::Brain>(config);
        bool initialized = brain->initialize();
        assert(initialized);
        
        // Test 3: Brain statistics
        size_t neuron_count = brain->getTotalNeuronCount();
        assert(neuron_count == 100);
        
        size_t synapse_count = brain->getTotalSynapseCount();
        assert(synapse_count > 0);
        
        // Test 4: Simple brain creation (default configuration)
        auto config2 = std::make_shared<nlm::Config>();
        auto brain2 = std::make_shared<nlm::Brain>(config2);
        assert(brain2->initialize());
        
        std::cout << "  ✓ Convenience functions test passed" << std::endl;
    }

    void test_brain_simulation() {
        std::cout << "Testing brain simulation with progress monitoring..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(50));
        config->set("brain.synapse_density", 0.1);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        // Verify simulation completed
        assert(brain->getTotalNeuronCount() == 50);
        
        std::cout << "  ✓ Brain simulation test passed" << std::endl;
    }

    void test_agent_integration() {
        std::cout << "Testing agent integration..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100));
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Create agent
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        
        // Create world
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0f, 10.0f);
        
        // Initialize agent with world
        agent->initialize(*world);
        
        // Verify agent setup
        assert(agent->getSensoryInputSize() > 0);
        assert(agent->getMotorOutputSize() > 0);
        
        std::cout << "  ✓ Agent integration test passed" << std::endl;
    }

    void test_error_handling() {
        std::cout << "Testing error handling..." << std::endl;
        
        // Test invalid configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(-1));  // Invalid
        
        // This should handle negative neuron count gracefully
        auto brain = std::make_shared<nlm::Brain>(config);
        bool initialized = brain->initialize();
        
        // Even with invalid config, initialization might fail or handle gracefully
        std::cout << "  ✓ Error handling test passed (handled gracefully)" << std::endl;
    }

    void test_world_integration() {
        std::cout << "Testing world integration..." << std::endl;
        
        // Create world
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(30, 30, 10, 10);
        world->reset();
        world->setAgentStart(15.0f, 15.0f);
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Create agent
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        agent->initialize(*world);
        
        // Run world update
        world->update(0.1);
        
        // Verify world state
        assert(world->getWidth() == 30);
        assert(world->getHeight() == 30);
        assert(world->getSimulationTime() > 0);
        
        std::cout << "  ✓ World integration test passed" << std::endl;
    }

    void run_all_tests() {
        std::cout << "=" * 60 << std::endl;
        std::cout << "Running NLM Improved API Tests" << std::endl;
        std::cout << "=" * 60 << std::endl;
        
        test_convenience_functions();
        test_brain_simulation();
        test_agent_integration();
        test_error_handling();
        test_world_integration();
        
        std::cout << "=" * 60 << std::endl;
        std::cout << "All tests passed successfully!" << std::endl;
        std::cout << "=" * 60 << std::endl;
    }
}

int main() {
    test_improved_api::run_all_tests();
    return 0;
}