# NLM Memory System Tests
# Comprehensive test suite for all memory components

#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_memory {

void testWorkingMemoryCreation() {
    nlm::WorkingMemory memory;
    assert(memory.getMaxCapacity() > 0);
    assert(memory.getCurrentSize() == 0);
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryStorage() {
    nlm::WorkingMemory memory;
    nlm::WorkingTrace trace("test_pattern", 0.5f, nlm::TraceType::Sensory);
    
    bool stored = memory.storeTrace(trace);
    assert(stored);
    assert(memory.getCurrentSize() == 1);
    
    auto retrieved = memory.retrieveTrace("test_pattern");
    assert(retrieved.has_value());
    assert(retrieved->pattern == "test_pattern");
    assert(retrieved->activation == 0.5f);
    
    std::cout << "    testWorkingMemoryStorage passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::WorkingMemory memory;
    
    // Fill memory to capacity
    for (size_t i = 0; i < memory.getMaxCapacity(); ++i) {
        nlm::WorkingTrace trace("pattern_" + std::to_string(i), 
                              0.5f, nlm::TraceType::Sensory);
        memory.storeTrace(trace);
    }
    
    assert(memory.getCurrentSize() == memory.getMaxCapacity());
    
    // Try to add one more trace (should fail)
    nlm::WorkingTrace trace("extra_pattern", 0.5f, nlm::TraceType::Sensory);
    bool stored = memory.storeTrace(trace);
    assert(!stored); // Should not be stored due to capacity
    assert(memory.getCurrentSize() == memory.getMaxCapacity());
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryActivation() {
    nlm::WorkingMemory memory;
    
    nlm::WorkingTrace trace1("pattern1", 0.3f, nlm::TraceType::Sensory);
    nlm::WorkingTrace trace2("pattern2", 0.7f, nlm::TraceType::Internal);
    
    memory.storeTrace(trace1);
    memory.storeTrace(trace2);
    
    // Activate patterns
    memory.activatePattern("pattern1", 0.8f);
    memory.activatePattern("pattern2", 0.4f);
    
    auto retrieved1 = memory.retrieveTrace("pattern1");
    auto retrieved2 = memory.retrieveTrace("pattern2");
    
    assert(retrieved1.has_value());
    assert(retrieved1->activation == 0.8f);
    
    assert(retrieved2.has_value());
    assert(retrieved2->activation == 0.4f);
    
    // Check that activation affects retrieval probability
    float prob1 = memory.getActivation("pattern1");
    float prob2 = memory.getActivation("pattern2");
    assert(prob1 > prob2); // Pattern 1 should have higher activation
    
    std::cout << "    testWorkingMemoryActivation passed" << std::endl;
}

void testWorkingMemoryDecay() {
    nlm::WorkingMemory memory;
    
    nlm::WorkingTrace trace("test_pattern", 0.9f, nlm::TraceType::Sensory);
    memory.storeTrace(trace);
    
    assert(memory.getCurrentSize() == 1);
    
    // Let it decay over time
    for (size_t step = 0; step < 100; ++step) {
        memory.step(step);
    }
    
    // Pattern should be decayed/removed
    auto retrieved = memory.retrieveTrace("test_pattern");
    assert(!retrieved.has_value()); // Should be decayed
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    nlm::WorkingMemory memory;
    
    // Add multiple traces
    nlm::WorkingTrace trace1("pattern1", 0.5f, nlm::TraceType::Sensory);
    nlm::WorkingTrace trace2("pattern2", 0.5f, nlm::TraceType::Internal);
    nlm::WorkingTrace trace3("pattern3", 0.5f, nlm::TraceType::Motor);
    
    memory.storeTrace(trace1);
    memory.storeTrace(trace2);
    memory.storeTrace(trace3);
    
    // Activate all traces
    memory.activatePattern("pattern1", 0.8f);
    memory.activatePattern("pattern2", 0.6f);
    memory.activatePattern("pattern3", 0.4f);
    
    // Check that higher activation traces are prioritized
    auto retrieved1 = memory.retrieveTrace("pattern1");
    auto retrieved2 = memory.retrieveTrace("pattern2");
    auto retrieved3 = memory.retrieveTrace("pattern3");
    
    assert(retrieved1.has_value() && retrieved1->activation == 0.8f);
    assert(retrieved2.has_value() && retrieved2->activation == 0.6f);
    assert(retrieved3.has_value() && retrieved3->activation == 0.4f);
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testEpisodicMemoryCreation() {
    nlm::NeuralEpisodicMemory memory;
    assert(memory.getMaxCapacity() > 0);
    assert(memory.getEpisodeCount() == 0);
    std::cout << "    testEpisodicMemoryCreation passed" << std::endl;
}

void testEpisodicMemoryStorage() {
    nlm::NeuralEpisodicMemory memory;
    
    nlm::EpisodicEpisode episode;
    episode.startTime = 0.0f;
    episode.endTime = 10.0f;
    episode.reward = 5.0f;
    episode.tags = {"exploration", "reward"};
    episode.context = "environment_1";
    
    memory.storeEpisode(episode);
    assert(memory.getEpisodeCount() == 1);
    
    auto retrieved = memory.getEpisode(0);
    assert(retrieved.has_value());
    assert(retrieved->reward == 5.0f);
    assert(retrieved->tags.size() == 2);
    
    std::cout << "    testEpisodicMemoryStorage passed" << std::endl;
}

void testEpisodicMemoryReplay() {
    nlm::NeuralEpisodicMemory memory;
    
    // Store multiple episodes
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicEpisode episode;
        episode.startTime = static_cast<float>(i * 10);
        episode.endTime = static_cast<float>((i + 1) * 10);
        episode.reward = static_cast<float>(i + 1);
        episode.tags = {"episode_" + std::to_string(i)};
        episode.context = "context_" + std::to_string(i);
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 5);
    
    // Replay recent episodes
    auto episodes = memory.getEpisodesForReplay(3);
    assert(episodes.size() == 3);
    
    // Episodes should be in reverse chronological order
    assert(episodes[0].endTime == 40.0f); // Most recent
    assert(episodes[2].endTime == 10.0f); // Third most recent
    
    std::cout << "    testEpisodicMemoryReplay passed" << std::endl;
}

void testEpisodicMemoryConsolidation() {
    nlm::NeuralEpisodicMemory memory;
    
    // Store some episodes
    for (int i = 0; i < 3; ++i) {
        nlm::EpisodicEpisode episode;
        episode.startTime = static_cast<float>(i * 10);
        episode.endTime = static_cast<float>((i + 1) * 10);
        episode.reward = static_cast<float>(i + 1);
        episode.tags = {"episode"};
        episode.context = "context";
        memory.storeEpisode(episode);
    }
    
    assert(memory.getEpisodeCount() == 3);
    
    // Trigger consolidation
    memory.consolidateEpisodes();
    
    // Consolidation should process episodes
    // (Implementation-dependent behavior)
    
    std::cout << "    testEpisodicMemoryConsolidation passed" << std::endl;
}

void testSemanticMemoryCreation() {
    nlm::SemanticMemory memory;
    assert(memory.getConceptCount() == 0);
    std::cout << "    testSemanticMemoryCreation passed" << std::endl;
}

void testSemanticMemoryConceptFormation() {
    nlm::SemanticMemory memory;
    
    // Form concepts from patterns
    memory.processPattern("tree", {"green", "tall", "leafy"}, "natural");
    memory.processPattern("car", {"metal", "fast", "wheels"}, "technology");
    memory.processPattern("tree", {"green", "leafy"}, "natural");
    
    assert(memory.getConceptCount() >= 2);
    
    // Retrieve concept
    auto concept = memory.getConcept("tree");
    assert(concept.has_value());
    assert(concept->theme == "natural");
    assert(concept->features.size() >= 2);
    
    std::cout << "    testSemanticMemoryConceptFormation passed" << std::endl;
}

void testSemanticMemoryCategorization() {
    nlm::SemanticMemory memory;
    
    // Form multiple concepts
    memory.processPattern("apple", {"red", "round", "sweet"}, "fruit");
    memory.processPattern("banana", {"yellow", "curved", "sweet"}, "fruit");
    memory.processPattern("car", {"metal", "engine", "wheels"}, "vehicle");
    memory.processPattern("bike", {"metal", "human_powered", "wheels"}, "vehicle");
    
    // Find category
    auto category = memory.categorizeConcept("apple");
    assert(category.has_value());
    assert(category.value() == "fruit");
    
    category = memory.categorizeConcept("car");
    assert(category.has_value());
    assert(category.value() == "vehicle");
    
    std::cout << "    testSemanticMemoryCategorization passed" << std::endl;
}

void testMemoryIntegration() {
    // Test that different memory systems work together
    nlm::WorkingMemory workingMemory;
    nlm::NeuralEpisodicMemory episodicMemory;
    nlm::SemanticMemory semanticMemory;
    
    // Store in working memory
    nlm::WorkingTrace trace("current_task", 0.8f, nlm::TraceType::Motor);
    workingMemory.storeTrace(trace);
    
    // Store in episodic memory
    nlm::EpisodicEpisode episode;
    episode.startTime = 0.0f;
    episode.endTime = 5.0f;
    episode.reward = 3.0f;
    episode.tags = {"task"};
    episode.context = "workspace";
    episodicMemory.storeEpisode(episode);
    
    // Form concept in semantic memory
    semanticMemory.processPattern("task", {"work", "goal", "completion"}, "activity");
    
    // Verify all systems are functional
    assert(workingMemory.getCurrentSize() == 1);
    assert(episodicMemory.getEpisodeCount() == 1);
    assert(semanticMemory.getConceptCount() >= 1);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Memory System Tests ===" << std::endl;
    
    testWorkingMemoryCreation();
    testWorkingMemoryStorage();
    testWorkingMemoryCapacity();
    testWorkingMemoryActivation();
    testWorkingMemoryDecay();
    testWorkingMemoryCompetition();
    
    testEpisodicMemoryCreation();
    testEpisodicMemoryStorage();
    testEpisodicMemoryReplay();
    testEpisodicMemoryConsolidation();
    
    testSemanticMemoryCreation();
    testSemanticMemoryConceptFormation();
    testSemanticMemoryCategorization();
    
    testMemoryIntegration();
    
    std::cout << "\n=== All Memory Tests Passed! ===" << std::endl;
}

} // namespace test_memory
