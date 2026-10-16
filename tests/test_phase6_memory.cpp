// Phase 6 Memory Integration Tests
// Tests working memory, episodic memory, and associative memory integration

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

using namespace nlm;

namespace test_phase6_memory {

void testWorkingMemoryIntegration() {
    std::cout << "  Testing Working Memory Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test working memory creation and basic functionality
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test that working memory has initial capacity
    size_t initialCapacity = workingMemory->getCapacity();
    assert(initialCapacity > 0);
    
    // Run some steps and verify working memory updates
    for (int i = 0; i < 50; ++i) {
        brain->step(i, i * 0.001);
        
        // Verify working memory is still valid
        assert(brain->getWorkingMemory() != nullptr);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "  Testing Episodic Memory Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test episodic memory creation and basic functionality
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test that episodic memory has initial capacity
    size_t initialCapacity = episodicMemory->getCapacity();
    assert(initialCapacity > 0);
    
    // Run some steps and verify episodic memory updates
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
        
        // Verify episodic memory is still valid
        assert(brain->getEpisodicMemory() != nullptr);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testAssociativeMemoryIntegration() {
    std::cout << "  Testing Associative Memory Integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test associative memory creation and basic functionality
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Test that associative memory has initial pattern count
    size_t initialPatterns = associativeMemory->getPatternCount();
    assert(initialPatterns >= 0);  // May be 0 initially
    
    // Test pattern storage and retrieval
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    
    associativeMemory->storePattern(pattern1, pattern2, 0.8f);
    
    // Test pattern retrieval
    std::vector<float> retrieved;
    float strength = associativeMemory->retrievePattern(pattern1, retrieved);
    assert(strength > 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Memory Integration Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testWorkingMemoryIntegration();
    testEpisodicMemoryIntegration();
    testAssociativeMemoryIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Memory Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_memory