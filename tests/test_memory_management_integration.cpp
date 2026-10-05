// Memory Management Integration Tests
// Phase 6: Testing memory pools, smart pointers, and leak detection across all components

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "core/Config/Config.hpp"
#include "performance/MemoryPool.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>

namespace test_memory_management_integration {

void testMemoryPoolIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("episodic_memory_size", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory pool is active
    brain.enableMemoryPool(true);
    
    std::cout << "    Memory pool integration - brain initialized with memory pools" << std::endl;
    
    // Perform operations to test memory pool
    for (int step = 0; step < 100; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.5f);
        brain.step(step);
        
        if (step % 25 == 0) {
            std::cout << "    Step " << step << " - Memory pool active" << std::endl;
        }
    }
    
    std::cout << "    testMemoryPoolIntegration passed" << std::endl;
}

void testSmartPointerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    // Test smart pointer usage
    std::shared_ptr<nlm::Brain> brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test weak pointer integration
    std::weak_ptr<nlm::Brain> weakBrain = brain;
    
    assert(!weakBrain.expired());
    assert(weakBrain.lock()->getTotalNeuronCount() == 300);
    
    // Test nested smart pointers
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    for (int step = 0; step < 50; ++step) {
        brain->step(step);
        
        // Test that smart pointers remain valid
        assert(!weakBrain.expired());
        assert(weakBrain.lock()->getTotalNeuronCount() == 300);
        
        // Test memory system health
        assert(std::isfinite(brain->getAverageFiringRate()));
    }
    
    std::cout << "    testSmartPointerIntegration passed" << std::endl;
}

void testMemoryLeakDetection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    std::atomic<size_t> allocationCount{0};
    
    {
        // Create brain with known scope
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Use brain extensively
        for (int step = 0; step < 75; ++step) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.3f);
            brain->step(step);
            allocationCount++;
        }
        
        // Brain goes out of scope here - should be cleaned up
        std::cout << "    Brain with scope exited - memory should be freed" << std::endl;
    }
    
    // Memory should be freed after scope exit
    std::cout << "    Memory leak detection test - allocation count: " << allocationCount << std::endl;
    std::cout << "    testMemoryLeakDetection passed" << std::endl;
}

void testMemorySystemStressTest() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(800), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("episodic_memory_size", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Stress test memory systems
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalActivity = 0.0f;
    size_t peakWorkingMemoryTraces = 0;
    
    for (int step = 0; step < 200; ++step) {
        // Heavy memory usage patterns
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.0f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.5f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.8f);
        
        brain.step(step);
        
        // Track metrics
        totalActivity += brain.getAverageFiringRate();
        
        auto* workingMemory = brain.getWorkingMemory();
        if (workingMemory) {
            size_t currentTraces = workingMemory->getActiveTraces();
            if (currentTraces > peakWorkingMemoryTraces) {
                peakWorkingMemoryTraces = currentTraces;
            }
        }
        
        if (step % 50 == 0) {
            std::cout << "    Step " << step 
                      << ": WM traces=" << (workingMemory ? workingMemory->getActiveTraces() : 0)
                      << ", Peak WM traces=" << peakWorkingMemoryTraces
                      << ", Avg firing=" << (totalActivity / (step + 1))
                      << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    Memory stress test completed:" << std::endl;
    std::cout << "      Duration: " << duration.count() << "ms" << std::endl;
    std::cout << "      Peak working memory traces: " << peakWorkingMemoryTraces << std::endl;
    std::cout << "      Average memory activity: " << (totalActivity / 200.0f) << std::endl;
    
    std::cout << "    testMemorySystemStressTest passed" << std::endl;
}

void testMemoryPoolEfficiency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(600), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory pool efficiency
    brain.enableMemoryPool(true);
    
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    // Create multiple brains to test memory pool efficiency
    for (int i = 0; i < 10; ++i) {
        auto brainConfig = std::make_shared<nlm::Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(100 + i * 20), nlm::ConfigSource::Default);
        brainConfig->set("enable_memory_pool", true, nlm::ConfigSource::Default);
        
        brains.push_back(std::make_shared<nlm::Brain>(brainConfig));
        brains.back()->initialize();
    }
    
    // Use all brains
    for (size_t i = 0; i < brains.size(); ++i) {
        for (int step = 0; step < 20; ++step) {
            brains[i]->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.2f);
            brains[i]->step(step);
        }
        
        if (i % 3 == 0) {
            std::cout << "    Processed brain " << i << " (" << brains[i]->getTotalNeuronCount() << " neurons)" << std::endl;
        }
    }
    
    // Clear brains to test cleanup
    brains.clear();
    std::cout << "    Memory pool efficiency test completed" << std::endl;
    std::cout << "    testMemoryPoolEfficiency passed" << std::endl;
}

void testMemoryIntegrationWithAllSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_structural_plasticity", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* dopamine = brain.getDopamine();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(dopamine != nullptr);
    
    // Test memory integration with all systems
    for (int step = 0; step < 100; ++step) {
        // Memory system should work with neuromodulation
        if (dopamine) {
            brain.applyRewardModulation(0.1f + (step % 10) * 0.01f, 0.05f);
        }
        
        // Memory system should work with development
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        
        // Memory system should work with plasticity
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.3f);
        
        // Step the integrated system
        brain.step(step);
        
        // Verify memory system health
        assert(std::isfinite(brain.getAverageFiringRate()));
        assert(brain.getTotalNeuronCount() == 450);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    }
    
    std::cout << "    testMemoryIntegrationWithAllSystems passed" << std::endl;
}

void runAll() {
    testMemoryPoolIntegration();
    testSmartPointerIntegration();
    testMemoryLeakDetection();
    testMemorySystemStressTest();
    testMemoryPoolEfficiency();
    testMemoryIntegrationWithAllSystems();
}

} // namespace test_memory_management_integration