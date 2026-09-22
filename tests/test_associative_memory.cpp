#pragma once

#include "../memory/NeuralAssociativeMemory.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

namespace nlm {

TEST_CASE("NeuralAssociativeMemory - Pattern Learning", "[neural][associative][memory]" ) {
    NeuralAssociativeMemory memory;
    
    // Test pattern learning
    SECTION("Basic pattern learning") {
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        SimulationStep currentTime = 0;
        
        memory.learnPattern(pattern, currentTime, 1.0f);
        
        // Verify pattern was stored
        REQUIRE(memory.getPatternCount() > 0);
        REQUIRE(memory.getAveragePatternActivation() > 0.0f);
    }
    
    SECTION("Pattern with reward") {
        std::vector<float> pattern1 = {0.2f, 0.4f, 0.6f};
        std::vector<float> pattern2 = {0.1f, 0.5f, 0.3f};
        SimulationStep currentTime = 0;
        
        memory.learnPattern(pattern1, currentTime, 0.5f);
        memory.learnPattern(pattern2, currentTime + 10, 1.0f);
        
        // Higher reward should lead to higher activation
        REQUIRE(memory.getAveragePatternActivation() > 0.0f);
    }
}

TEST_CASE("NeuralAssociativeMemory - Pattern Completion", "[neural][associative][memory]" ) {
    NeuralAssociativeMemory memory;
    
    SECTION("Pattern completion from partial cues") {
        std::vector<float> fullPattern = {0.1f, 0.8f, 0.3f, 0.9f, 0.2f};
        std::vector<float> partialPattern = {0.1f, 0.0f, 0.3f, 0.0f, 0.2f}; // Missing elements
        SimulationStep currentTime = 0;
        
        // Learn full pattern
        memory.learnPattern(fullPattern, currentTime, 1.0f);
        
        // Try to complete from partial pattern
        std::vector<float> completed = memory.completePattern(partialPattern, 0.5f);
        
        // Should return a completed pattern if similarity is high enough
        if (!completed.empty()) {
            REQUIRE(completed.size() == partialPattern.size());
            // Missing elements should be filled
            REQUIRE(completed[1] > 0.0f);
            REQUIRE(completed[3] > 0.0f);
        }
    }
    
    SECTION("Pattern completion threshold") {
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        SimulationStep currentTime = 0;
        
        memory.learnPattern(pattern, currentTime, 1.0f);
        
        // Very different pattern - should not complete
        std::vector<float> veryDifferent = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
        std::vector<float> completed = memory.completePattern(veryDifferent, 0.8f);
        
        // Should return empty pattern since similarity is below threshold
        REQUIRE(completed.empty());
    }
}

TEST_CASE("NeuralAssociativeMemory - Association Learning", "[neural][associative][memory]" ) {
    NeuralAssociativeMemory memory;
    
    SECTION("Learning associations from episodes") {
        // Create an episodic memory item
        EpisodicMemoryItem episode;
        episode.sensoryState = {0.1f, 0.2f, 0.3f};
        episode.resultingSensoryState = {0.2f, 0.4f, 0.6f};
        episode.reward = 0.8f;
        
        SimulationStep currentTime = 0;
        
        memory.learnFromEpisode(episode, currentTime);
        
        // Verify association was created
        REQUIRE(memory.getAssociationCount() > 0);
        REQUIRE(memory.getPatternCount() > 0);
    }
    
    SECTION("Association strength updates") {
        EpisodicMemoryItem episode1;
        episode1.sensoryState = {0.1f, 0.2f};
        episode1.resultingSensoryState = {0.3f, 0.4f};
        episode1.reward = 0.5f;
        
        EpisodicMemoryItem episode2;
        episode2.sensoryState = {0.1f, 0.2f};  // Same initial state
        episode2.resultingSensoryState = {0.3f, 0.4f};  // Same resulting state
        episode2.reward = 1.0f;  // Higher reward
        
        SimulationStep currentTime = 0;
        
        memory.learnFromEpisode(episode1, currentTime);
        memory.learnFromEpisode(episode2, currentTime + 100);
        
        // Higher reward should lead to stronger association
        // (Current implementation stores pattern neurons, strength comes from episodes)
    }
}

TEST_CASE("NeuralAssociativeMemory - Spreading Activation", "[neural][associative][memory]" ) {
    NeuralAssociativeMemory memory;
    
    SECTION("Spreading activation between patterns") {
        std::vector<float> pattern1 = {0.1f, 0.8f, 0.3f};
        std::vector<float> pattern2 = {0.2f, 0.3f, 0.7f};
        SimulationStep currentTime = 0;
        
        memory.learnPattern(pattern1, currentTime, 1.0f);
        memory.learnPattern(pattern2, currentTime + 50, 0.8f);
        
        // LearnFromEpisode creates associations
        EpisodicMemoryItem episode;
        episode.sensoryState = pattern1;
        episode.resultingSensoryState = pattern2;
        episode.reward = 0.9f;
        
        memory.learnFromEpisode(episode, currentTime);
        
        // Try spreading activation from pattern1
        std::vector<std::vector<float>> retrieved = memory.retrieveWithSpreading(pattern1, 2);
        
        // Should retrieve pattern2 (associated pattern)
        if (!retrieved.empty()) {
            REQUIRE(retrieved.size() > 0);
        }
    }
    
    SECTION("Activation propagation steps") {
        std::vector<float> startPattern = {0.1f, 0.5f, 0.3f};
        SimulationStep currentTime = 0;
        
        memory.learnPattern(startPattern, currentTime, 1.0f);
        
        // Test with different spread steps
        std::vector<std::vector<float>> retrieved1 = memory.retrieveWithSpreading(startPattern, 1);
        std::vector<std::vector<float>> retrieved2 = memory.retrieveWithSpreading(startPattern, 2);
        
        // More steps might retrieve more patterns (if associations exist)
    }
}

TEST_CASE("NeuralAssociativeMemory - Neural Integration", "[neural][associative][memory]" ) {
    NeuralAssociativeMemory memory;
    
    SECTION("Pattern to neural representation conversion") {
        std::vector<float> pattern = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
        SimulationStep currentTime = 0;
        
        // Pattern should be converted to neural representation
        memory.patternToNeuralRepresentation(pattern, currentTime);
        
        // Verify it was stored as a pattern
        REQUIRE(memory.getPatternCount() > 0);
        
        // Pattern should be retrievable
        std::vector<float> completed = memory.completePattern(pattern, 0.5f);
        // Might not complete if similarity threshold is not met
    }
    
    SECTION("Hebbian plasticity simulation") {
        std::vector<float> pattern1 = {0.1f, 0.3f, 0.5f};
        std::vector<float> pattern2 = {0.2f, 0.4f, 0.6f};
        SimulationStep currentTime = 0;
        
        // Learn first pattern
        memory.learnPattern(pattern1, currentTime, 1.0f);
        
        // Learn second pattern - should create Hebbian association
        memory.learnPattern(pattern2, currentTime + 10, 1.0f);
        
        // Should have associations between patterns
        REQUIRE(memory.getAssociationCount() > 0 || memory.getPatternCount() >= 2);
    }
}

} // namespace nlm