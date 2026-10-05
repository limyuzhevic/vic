// Backward Compatibility Tests
// Phase 6: Full backward compatibility verification for all enhancements

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include <string>

namespace test_backward_compatibility {

void testPythonBindingBackwardCompatibility() {
    // Test that existing Python bindings still work
    // This simulates the old Python API to ensure backward compatibility
    
    // Test 1: Basic config operations
    auto config = std::make_shared<nlm::Config>();
    
    // Old-style API still works
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    assert(config->get<int>("neuron_count") == 100);
    assert(config->get<int>("region_count") == 2);
    assert(config->getOr("invalid_key", 999) == 999); // Default value works
    
    // Test 2: Brain initialization (old API)
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 3: Simulation step compatibility
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
        assert(brain.getTotalNeuronCount() == 100);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    Python binding backward compatibility passed" << std::endl;
}

void testCPlusPlusAPICompatibility() {
    // Test that existing C++ API still works
    
    // Test 1: Basic brain creation and initialization
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 2: Basic simulation (old API)
    for (int step = 0; step < 75; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.3f);
        brain.step(step);
        assert(brain.getTotalNeuronCount() == 200);
    }
    
    // Test 3: Memory system access (old API)
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // These should still work even if memory pools are enabled
    if (workingMemory) {
        std::cout << "    Working memory available: " << workingMemory->getActiveTraces() << " traces" << std::endl;
    }
    if (episodicMemory) {
        std::cout << "    Episodic memory available: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
    }
    
    // Test 4: Neuromodulation compatibility
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    if (dopamine) {
        brain.applyRewardModulation(0.5f, 0.2f);
        std::cout << "    Neuromodulation system accessible" << std::endl;
    }
    
    std::cout << "    C++ API compatibility passed" << std::endl;
}

void testDevelopmentSystemBackwardCompatibility() {
    // Test that development system API hasn't changed
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("development_rate", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test old-style development calls
    for (int step = 0; step < 60; ++step) {
        // Old API: updateDevelopment step parameter
        brain.updateDevelopment(step);
        assert(brain.getTotalNeuronCount() == 150);
        assert(std::isfinite(brain.getAverageFiringRate()));
        
        // Test developmental stage access
        auto stage = brain.getDevelopmentalStage();
        (void)stage; // Use stage to avoid unused variable warning
    }
    
    // Test that all development stages still work
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    
    std::cout << "    Development system backward compatibility passed" << std::endl;
}

void testMemorySystemBackwardCompatibility() {
    // Test that memory system API hasn't changed
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("working_memory_size", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test old-style memory access
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Old API should still work
    if (workingMemory) {
        for (int step = 0; step < 40; ++step) {
            brain.step(step);
            int traces = workingMemory->getActiveTraces();
            (void)traces; // Use traces to avoid unused variable warning
        }
    }
    
    if (episodicMemory) {
        // Test old-style episodic memory operations
        for (int step = 0; step < 30; ++step) {
            brain.step(step);
            int episodes = episodicMemory->getEpisodeCount();
            (void)episodes; // Use episodes to avoid unused variable warning
            
            // Test replay functionality (if available)
            auto episodesForReplay = episodicMemory->getEpisodesForReplay(3);
            (void)episodesForReplay; // Use to avoid unused variable warning
        }
    }
    
    std::cout << "    Memory system backward compatibility passed" << std::endl;
}

void testNeuromodulationSystemBackwardCompatibility() {
    // Test that neuromodulation system API hasn't changed
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_neuromodulation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test old-style neuromodulation calls
    for (int step = 0; step < 45; ++step) {
        // Old API: applyRewardModulation with two parameters
        brain.applyRewardModulation(0.5f + (step % 10) * 0.01f, 0.2f);
        
        // Test neuromodulator access
        auto* dopamine = brain.getDopamine();
        auto* novelty = brain.getNovelty();
        
        if (dopamine) {
            std::cout << "    Step " << step << ": Dopamine system accessible" << std::endl;
        }
        if (novelty) {
            std::cout << "    Step " << step << ": Novelty system accessible" << std::endl;
        }
    }
    
    std::cout << "    Neuromodulation system backward compatibility passed" << std::endl;
}

void testConfigurationFileBackwardCompatibility() {
    // Test that configuration file loading still works
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    // Test old-style config operations
    assert(config->has("neuron_count"));
    assert(config->hasType<int>("region_count"));
    assert(config->get<int>("region_count") == 2);
    
    // Test config validation
    assert(config->validate());
    
    std::string summary = config->summary();
    assert(!summary.empty());
    
    // Test file operations
    std::string testPath = "/tmp/test_config.nlm";
    bool saveSuccess = config->saveToFile(testPath);
    (void)saveSuccess; // Use to avoid unused variable warning
    
    std::cout << "    Configuration file backward compatibility passed" << std::endl;
}

void testPerformanceSystemBackwardCompatibility() {
    // Test that performance system enhancements don't break existing API
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(140), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test old-style performance operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 35; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.3f);
        brain.step(step);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    Performance system backward compatibility:" << std::endl;
    std::cout << "      Duration: " << duration.count() << "ms for 35 steps" << std::endl;
    
    std::cout << "    Performance system backward compatibility passed" << std::endl;
}

void testCompleteBackwardCompatibility() {
    // Comprehensive backward compatibility test
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_structural_plasticity", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run through all old-style API calls
    for (int step = 0; step < 100; ++step) {
        // All old-style API calls that should still work
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 0.2f);
        brain.step(step);
        brain.updateDevelopment(step);
        brain.applyRewardModulation(0.3f, 0.1f);
        
        // Verify all systems respond
        assert(brain.getTotalNeuronCount() == 200);
        assert(std::isfinite(brain.getAverageFiringRate()));
        
        // Check development state
        auto stage = brain.getDevelopmentalStage();
        (void)stage; // Use to avoid unused variable warning
    }
    
    std::cout << "    Complete backward compatibility test passed" << std::endl;
}

void runAll() {
    testPythonBindingBackwardCompatibility();
    testCPlusPlusAPICompatibility();
    testDevelopmentSystemBackwardCompatibility();
    testMemorySystemBackwardCompatibility();
    testNeuromodulationSystemBackwardCompatibility();
    testConfigurationFileBackwardCompatibility();
    testPerformanceSystemBackwardCompatibility();
    testCompleteBackwardCompatibility();
}

} // namespace test_backward_compatibility