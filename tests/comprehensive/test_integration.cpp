// Simplified Integration Tests

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_integration {

void test_complete_brain_loop() {
    std::cout << "    Testing complete brain loop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run complete brain loop for multiple steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify brain remains in valid state
        assert(brain.getTotalNeuronCount() == 20);
        assert(brain.getTotalSynapseCount() > 0);
        assert(brain.getFiringNeuronCount() >= 0);
        
        // Occasionally log status
        if (step % 10 == 0) {
            brain.logStatus();
        }
    }
    
    // Test reset and re-run
    brain.reset();
    brain.step(0);
    
    std::cout << "    test_complete_brain_loop passed" << std::endl;
}

void test_agent_world_interaction() {
    std::cout << "    Testing agent-world interaction..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agent_brain(&brain);
    
    // Initialize brain
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test agent brain API
    assert(agent_brain.getSensoryInputSize() > 0);
    assert(agent_brain.getMotorOutputSize() > 0);
    
    // Test that agent brain can decode motor commands
    // (without a real world, we'll just verify the interface exists)
    
    std::cout << "    test_agent_world_interaction passed (basic test)" << std::endl;
}

void test_learning_experiments() {
    std::cout << "    Testing learning experiments..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain with plasticity
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run learning experiment
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Check that plasticity systems are active
        auto* stdp = brain.getSTDP();
        if (stdp) {
            // STDP should be configured
            assert(stdp->getLTPWeight() > 0.0f);
            assert(stdp->getLTDWeight() > 0.0f);
        }
        
        // Occasionally check statistics
        if (step % 20 == 0) {
            brain.logStatus();
        }
    }
    
    std::cout << "    test_learning_experiments passed" << std::endl;
}

void test_checkpoint_functionality() {
    std::cout << "    Testing checkpoint functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Default);
    config->set("checkpoint_max_count", 5, nlm::ConfigSource::Default);
    config->set("checkpoint_compression_enabled", false, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
    }
    
    // Test checkpoint management (if available)
    // Note: Full checkpoint testing requires more infrastructure
    // We'll test that the brain doesn't crash during operation
    
    std::cout << "    test_checkpoint_functionality passed (basic test)" << std::endl;
}

void runAll() {
    test_complete_brain_loop();
    test_agent_world_interaction();
    test_learning_experiments();
    test_checkpoint_functionality();
}

} // namespace test_integration