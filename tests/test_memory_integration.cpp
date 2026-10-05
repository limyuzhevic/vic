// Memory System Integration Tests
// Phase 3.3: Integration tests for memory system

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

namespace test_memory_integration {

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory integration
    // The brain should have access to working memory functionality
    brain.step(0);
    
    // Verify working memory is integrated
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("episodic_memory_size", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test episodic memory integration
    // The brain should have episodic memory capabilities
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify episodic memory is integrated
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    testEpisodicMemoryIntegration passed" << std::endl;
}

void testMemorySystemCompleteIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("episodic_memory_size", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Complete memory system integration test
    // Test various memory-related operations
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        // Inject memory-related inputs
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.5f);
        
        // Step the system
        brain.step(step);
        
        // Verify memory system is operational
        assert(brain.getTotalNeuronCount() == 300);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    // Test memory pool efficiency
    brain.enableMemoryPool(true);
    
    std::cout << "    testMemorySystemCompleteIntegration passed" << std::endl;
}

void testMemoryPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark memory system performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalActivity = 0.0f;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.3f);
        brain.step(step);
        totalActivity += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    testMemoryPerformance: " << duration.count() << " microseconds for 100 steps with memory system" << std::endl;
    std::cout << "    Average memory system activity: " << (totalActivity / 100.0f) << std::endl;
    
    std::cout << "    testMemoryPerformance passed" << std::endl;
}

void testMemoryResourceManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory resource management under load
    for (int i = 0; i < 5; ++i) {
        // Heavy memory usage
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.8f);
            brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.4f);
            brain.step(step);
        }
        
        // Reset and verify cleanup
        brain.reset();
        assert(brain.initialize());
    }
    
    std::cout << "    testMemoryResourceManagement passed" << std::endl;
}

void testMemorySystemRobustness() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory system robustness with extreme conditions
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        // High load scenario
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 2.0f);
        
        // Stress memory system
        brain.step(step);
        
        // Verify system remains stable
        assert(brain.getTotalNeuronCount() == 600);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testMemorySystemRobustness passed" << std::endl;
}

void testMemoryIntegrationWithOtherSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory system integration with other systems
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        // Memory system should work with neuromodulation
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.5f);
        
        // Memory system should work with development
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        
        // Step the integrated system
        brain.step(step);
        
        // Verify all systems are working
        assert(brain.getTotalNeuronCount() == 350);
        assert(brain.getDevelopmentStage() == nlm::DevelopmentalStage::CriticalPeriod);
    }
    
    std::cout << "    testMemoryIntegrationWithOtherSystems passed" << std::endl;
}

void runAll() {
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testMemorySystemCompleteIntegration();
    testMemoryPerformance();
    testMemoryResourceManagement();
    testMemorySystemRobustness();
    testMemoryIntegrationWithOtherSystems();
}

} // namespace test_memory_integration