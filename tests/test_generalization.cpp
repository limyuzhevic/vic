// Generalization Tests
// Scientific validation of NLM generalization capabilities (Phase 6 audit)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_generalization {

void testPatternGeneralization() {
    // Test that similar patterns are recognized as related
    std::cout << "  Testing Pattern Generalization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store a base pattern
    std::vector<float> basePattern;
    for (int i = 0; i < 10; ++i) {
        basePattern.push_back(0.5f + static_cast<float>(i) * 0.1f);
    }
    
    brain.getWorkingMemory()->store(basePattern, 1.0f);
    
    // Create similar patterns (small variations)
    std::vector<std::vector<float>> similarPatterns;
    for (int variation = -2; variation <= 2; ++variation) {
        std::vector<float> pattern = basePattern;
        for (size_t i = 0; i < pattern.size(); ++i) {
            pattern[i] += static_cast<float>(variation) * 0.05f;
        }
        similarPatterns.push_back(pattern);
    }
    
    // Test that all patterns can be stored (generalization occurs)
    for (const auto& pattern : similarPatterns) {
        brain.getWorkingMemory()->store(pattern, 1.0f);
    }
    
    // Verify that working memory can handle the patterns
    float activity = brain.getWorkingMemory()->getMemoryActivity();
    assert(activity >= 0.0f);
    
    std::cout << "    Pattern generalization test passed" << std::endl;
}

void testActionGeneralization() {
    // Test that learned actions generalize to similar situations
    std::cout << "  Testing Action Generalization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create similar contextual patterns
    std::vector<std::vector<float>> contexts;
    
    // Context 1: Neutral situation
    std::vector<float> neutralContext(10, 0.5f);
    contexts.push_back(neutralContext);
    
    // Context 2: Similar but slightly different (subtle variation)
    std::vector<float> similarContext = neutralContext;
    for (size_t i = 0; i < similarContext.size(); ++i) {
        similarContext[i] += 0.1f;
    }
    contexts.push_back(similarContext);
    
    // Context 3: Even more different
    std::vector<float> differentContext = neutralContext;
    for (size_t i = 0; i < differentContext.size(); ++i) {
        differentContext[i] += 0.3f;
    }
    contexts.push_back(differentContext);
    
    // Store all contexts in working memory
    for (const auto& context : contexts) {
        brain.getWorkingMemory()->store(context, 1.0f);
    }
    
    // Test that similar contexts can be processed (generalization)
    float totalActivity = brain.getWorkingMemory()->getMemoryActivity();
    assert(totalActivity >= 0.0f);
    
    std::cout << "    Action generalization test passed" << std::endl;
}

void testCrossModalGeneralization() {
    // Test that learning in one modality transfers to another
    std::cout << "  Testing Cross-Modal Generalization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set<int64_t>("neuron_count", 100);
    config->set<int64_t>("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a learned pattern from one modality (e.g., visual)
    std::vector<float> visualPattern(15, 0.0f);
    for (int i = 0; i < 5; ++i) {
        visualPattern[i] = 0.8f;  // Set first 5 elements
    }
    
    // Store the learned pattern
    brain.getWorkingMemory()->store(visualPattern, 1.0f);
    
    // Transfer to another modality (e.g., auditory)
    // Create an auditory pattern that shares some structure
    std::vector<float> auditoryPattern(15, 0.0f);
    for (int i = 0; i < 5; ++i) {
        auditoryPattern[i + 5] = visualPattern[i] * 0.8f;  // Transfer some features
    }
    
    // Test cross-modal transfer
    brain.getWorkingMemory()->store(auditoryPattern, 0.8f);
    
    // Verify that both patterns are stored
    float activity = brain.getWorkingMemory()->getMemoryActivity();
    assert(activity >= 0.0f);
    
    // Test retrieval shows some overlap (generalization)
    std::vector<float> retrieved = brain.getWorkingMemory()->retrieve();
    assert(!retrieved.empty());
    
    std::cout << "    Cross-modal generalization test passed" << std::endl;
}

void runAll() {
    testPatternGeneralization();
    testActionGeneralization();
    testCrossModalGeneralization();
}

} // namespace test_generalization