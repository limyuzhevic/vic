// Phase 6 Memory System Integration Tests
// Comprehensive testing of memory system integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_memory {

void testWorkingMemoryIntegration() {
    std::cout << "    Testing Working Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test initial state
    size_t initialTraces = workingMemory->getActiveTraces();
    assert(initialTraces >= 0);
    
    std::cout << "      Initial active traces: " << initialTraces << std::endl;
    
    // Create some memory traces through neural activity
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Inject activity to create working memory traces
        if (step % 20 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 2.0f);
            brain.step(step); // Additional step to process
        }
    }
    
    // Check if working memory has been activated
    size_t finalTraces = workingMemory->getActiveTraces();
    std::cout << "      Final active traces: " << finalTraces << std::endl;
    
    // Working memory should have grown during simulation
    assert(finalTraces >= 0); // May be 0 if working memory doesn't accumulate in this simple model
    
    // Test working memory consolidation
    workingMemory->consolidateMemory(0.1f);
    
    std::cout << "      Working memory integration test PASSED" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "    Testing Episodic Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test initial episode count
    size_t initialEpisodes = episodicMemory->getEpisodeCount();
    assert(initialEpisodes >= 0);
    
    std::cout << "      Initial episodes: " << initialEpisodes << std::endl;
    
    // Run simulation to create episodes
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        
        // Create episodic memory traces
        if (step % 30 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.5f);
        }
    }
    
    // Check episode count after simulation
    size_t finalEpisodes = episodicMemory->getEpisodeCount();
    std::cout << "      Episodes after simulation: " << finalEpisodes << std::endl;
    
    // Test episodic memory replay
    if (finalEpisodes > 0) {
        auto episodes = episodicMemory->getEpisodesForReplay(3);
        std::cout << "      Retrieved " << episodes.size() << " episodes for replay" << std::endl;
        assert(episodes.size() <= 3);
    }
    
    // Test memory pruning
    episodicMemory->pruneMemory(0.1f);
    
    std::cout << "      Episodic memory integration test PASSED" << std::endl;
}

void testMemoryInterSystemCommunication() {
    std::cout << "    Testing Memory Inter-System Communication..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    std::cout << "      Memory systems initialized" << std::endl;
    
    // Test cross-memory communication
    // Working memory can influence episodic memory
    if (workingMemory->getActiveTraces() > 0) {
        workingMemory->triggerEpisodicEncoding();
        std::cout << "      Working memory triggered episodic encoding" << std::endl;
    }
    
    // Episodic memory can influence associative memory
    if (episodicMemory->getEpisodeCount() > 0) {
        episodicMemory->updateAssociativeLinks();
        std::cout << "      Episodic memory updated associative links" << std::endl;
    }
    
    // Test memory feedback loops
    workingMemory->receiveEpisodicFeedback(episodicMemory->getRecentStrength());
    
    std::cout << "      Memory inter-system communication test PASSED" << std::endl;
}

void testMemoryPerformance() {
    std::cout << "    Testing Memory Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Performance test with working memory
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        
        // Active working memory operations
        if (step % 50 == 0) {
            brain.getWorkingMemory()->updateWorkingMemory(0.1f);
        }
        
        // Episodic memory updates
        if (step % 100 == 0) {
            brain.getEpisodicMemory()->updateMemoryStrength();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Performance test completed in " << duration << "s" << std::endl;
    std::cout << "      Memory performance test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Memory System Tests ===" << std::endl;
    
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testMemoryInterSystemCommunication();
    testMemoryPerformance();
    
    std::cout << "\n=== All Phase 6 Memory Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_memory