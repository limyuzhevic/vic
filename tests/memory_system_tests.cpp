// Memory System Integration Test Suite
// This file contains comprehensive tests for the NLM neural simulation memory systems.

#include "test_memory_system.h"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <random>
#include <limits>
#include <algorithm>

namespace test_memory_system {
    
    // Forward declarations
    bool testNeuralWorkingMemoryBasic()
    bool testNeuralWorkingMemoryPersistence()
    bool testNeuralWorkingMemoryIntegration()
    bool testNeuralEpisodicMemoryBasic()
    bool testNeuralEpisodicMemoryRetrieval()
    bool testNeuralEpisodicMemoryIntegration()
    bool testNeuralAssociativeMemoryBasic()
    bool testNeuralAssociativeMemorySpreading()
    bool testNeuralAssociativeMemoryIntegration()
    bool testMemorySystemIntegration()
    bool testMemoryPerformance()
    bool testMemoryErrorHandling()
    bool testMemoryConsistency()
    
    class MemorySystemTests : public MemorySystemTestFramework {
    public:
        MemorySystemTests() : MemorySystemTestFramework() {
            registerAllTests();
        }
        
    protected:
        void runNeuralWorkingMemoryTests() override {
            std::cout << "Running NeuralWorkingMemory tests..." << std::endl;
            registerTest("NeuralWorkingMemory_Basic", testNeuralWorkingMemoryBasic);
            registerTest("NeuralWorkingMemory_Persistence", testNeuralWorkingMemoryPersistence);
            registerTest("NeuralWorkingMemory_Integration", testNeuralWorkingMemoryIntegration);
        }
        
        void runNeuralEpisodicMemoryTests() override {
            std::cout << "Running NeuralEpisodicMemory tests..." << std::endl;
            registerTest("NeuralEpisodicMemory_Basic", testNeuralEpisodicMemoryBasic);
            registerTest("NeuralEpisodicMemory_Retrieval", testNeuralEpisodicMemoryRetrieval);
            registerTest("NeuralEpisodicMemory_Integration", testNeuralEpisodicMemoryIntegration);
        }
        
        void runNeuralAssociativeMemoryTests() override {
            std::cout << "Running NeuralAssociativeMemory tests..." << std::endl;
            registerTest("NeuralAssociativeMemory_Basic", testNeuralAssociativeMemoryBasic);
            registerTest("NeuralAssociativeMemory_Spreading", testNeuralAssociativeMemorySpreading);
            registerTest("NeuralAssociativeMemory_Integration", testNeuralAssociativeMemoryIntegration);
        }
        
        void runIntegrationTests() override {
            std::cout << "Running Integration tests..." << std::endl;
            registerTest("MemorySystem_Integration", testMemorySystemIntegration);
        }
        
        void runPerformanceTests() override {
            std::cout << "Running Performance tests..." << std::endl;
            registerTest("Memory_Performance", testMemoryPerformance);
        }
        
        void runErrorHandlingTests() override {
            std::cout << "Running Error Handling tests..." << std::endl;
            registerTest("Memory_ErrorHandling", testMemoryErrorHandling);
        }
        
        void runMemoryConsistencyTests() override {
            std::cout << "Running Memory Consistency tests..." << std::endl;
            registerTest("Memory_Consistency", testMemoryConsistency);
        }
        
    private:
        void registerAllTests() {
            runNeuralWorkingMemoryTests();
            runNeuralEpisodicMemoryTests();
            runNeuralAssociativeMemoryTests();
            runIntegrationTests();
            runPerformanceTests();
            runErrorHandlingTests();
            runMemoryConsistencyTests();
        }
    };
    
    // Test implementations
    bool testNeuralWorkingMemoryBasic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            return assertFalse(true, "Failed to initialize brain");
        }
        
        auto* workingMemory = brain.getWorkingMemory();
        if (!workingMemory) {
            return assertFalse(true, "Working memory not initialized");
        }
        
        // Test basic storage and retrieval
        std::vector<float> pattern = {0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
        workingMemory->store(pattern, 1.0f);
        
        std::vector<float> retrieved = workingMemory->retrieve();
        return assertGreaterThan(retrieved.size(), 0, "Retrieved pattern should not be empty") &&
               assertTrue(retrieved.size() >= 3, "Should retrieve at least 3 neurons from 5-element pattern");
    }
    
    bool testNeuralWorkingMemoryPersistence() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        
        // Store multiple patterns
        for (int i = 0; i < 10; ++i) {
            std::vector<float> pattern(i + 1, 0.5f);
            workingMemory->store(pattern, 0.5f);
        }
        
        // Verify persistence by checking capacity
        size_t capacity = workingMemory->getCapacity();
        bool hasCapacity = capacity > 0;
        
        // Run some steps to test persistence during dynamics
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Verify memory still exists after simulation steps
        bool memoryStillExists = !workingMemory->getMemoryNeurons().empty() || 
                                !workingMemory->getMemoryActivations().empty();
        
        return assertTrue(hasCapacity, "Working memory should have capacity > 0") &&
               assertTrue(memoryStillExists, "Working memory should persist after simulation steps");
    }
    
    bool testNeuralWorkingMemoryIntegration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        
        // Test integration with sensory input
        std::vector<float> sensoryInput = {1.0f, 0.0f, 0.5f, 0.0f, 1.0f};
        brain.receiveSensoryInput(*reinterpret_cast<const nlm::SensoryInput*>(&sensoryInput));
        
        // Test storage and retrieval integration
        workingMemory->store(sensoryInput, 0.8f);
        
        // Verify integration by checking that working memory has content
        bool hasContent = workingMemory->getMemoryActivity() > 0.0f;
        
        // Update working memory (maintenance)
        workingMemory->update(0.001f);
        
        // Verify memory updates
        bool updated = workingMemory->getMemoryNeurons().size() > 0;
        
        return assertTrue(hasContent, "Working memory should contain sensory input") &&
               assertTrue(updated, "Working memory should update during simulation");
    }
    
    bool testNeuralEpisodicMemoryBasic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        
        // Create and store an episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.action = nlm::ActionType::MoveForward;
        episode.reward = 0.8f;
        episode.positionX = 10.0f;
        episode.positionY = 20.0f;
        episode.orientation = 45.0f;
        episode.sensoryState = {0.5f, 0.7f, 0.3f, 0.9f, 0.2f};
        
        episodicMemory->storeEpisode(episode);
        
        // Verify storage
        bool stored = episodicMemory->getEpisodeCount() > 0;
        
        // Test retrieval
        auto recent = episodicMemory->getRecentEpisodes(1);
        bool retrieved = !recent.empty() && recent[0] != nullptr;
        
        return assertTrue(stored, "Episodic memory should store episodes") &&
               assertTrue(retrieved, "Episodic memory should retrieve episodes");
    }
    
    bool testNeuralEpisodicMemoryRetrieval() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        
        // Store multiple episodes with different characteristics
        for (int i = 0; i < 10; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.action = (i % 3 == 0) ? nlm::ActionType::MoveForward : 
                              (i % 3 == 1) ? nlm::ActionType::TurnLeft : 
                              nlm::ActionType::TurnRight;
            episode.reward = 0.5f + (i * 0.05f);
            episode.positionX = i * 5.0f;
            episode.positionY = i * 3.0f;
            episode.orientation = i * 30.0f;
            episode.sensoryState = std::vector<float>(5, 0.5f + i * 0.02f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Test temporal retrieval
        auto temporal = episodicMemory->retrieveTemporal(2, 7, 5);
        bool temporalRetrieved = !temporal.empty();
        
        // Test location retrieval
        auto location = episodicMemory->retrieveByLocation(10.0f, 15.0f, 5.0f, 3);
        bool locationRetrieved = !location.empty();
        
        // Test action retrieval
        auto action = episodicMemory->retrieveAfterAction(nlm::ActionType::MoveForward, 3);
        bool actionRetrieved = !action.empty();
        
        return assertTrue(temporalRetrieved, "Should retrieve episodes by time") &&
               assertTrue(locationRetrieved, "Should retrieve episodes by location") &&
               assertTrue(actionRetrieved, "Should retrieve episodes by action");
    }
    
    bool testNeuralEpisodicMemoryIntegration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* episodicMemory = brain.getEpisodicMemory();
        
        // Create episode and store
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 10;
        episode.action = nlm::ActionType::Wait;
        episode.reward = 0.3f;
        episode.sensoryState = {0.2f, 0.4f, 0.6f, 0.8f, 0.1f};
        
        episodicMemory->storeEpisode(episode);
        
        // Test integration with working memory
        auto* workingMemory = brain.getWorkingMemory();
        if (!workingMemory) {
            return assertFalse(true, "Working memory should be available");
        }
        
        // Store some working memory content
        workingMemory->store(episode.sensoryState, 0.7f);
        
        // Test replay integration
        episodicMemory->replayEpisode(&episode);
        
        bool integrated = episodicMemory->getEpisodeCount() > 0 && workingMemory->getMemoryActivity() > 0.0f;
        
        return assertTrue(integrated, "Episodic memory should integrate with working memory");
    }
    
    bool testNeuralAssociativeMemoryBasic() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Create two patterns
        std::vector<float> patternA = {1.0f, 0.0f, 0.5f, 0.0f, 1.0f};
        std::vector<float> patternB = {0.0f, 1.0f, 0.5f, 1.0f, 0.0f};
        
        // Create association
        associativeMemory->associate(patternA, patternB, 0.8f);
        
        // Verify association
        float strength = associativeMemory->getAssociationStrength(patternA, patternB);
        bool associated = strength > 0.0f;
        
        // Test retrieval
        std::vector<std::vector<float>> retrieved = associativeMemory->retrieve(patternA, 3);
        bool retrievedItems = !retrieved.empty();
        
        return assertTrue(associated, "Should create associative memory connections") &&
               assertTrue(retrievedItems, "Should retrieve associated patterns");
    }
    
    bool testNeuralAssociativeMemorySpreading() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Create a chain of associations: A -> B -> C -> D
        std::vector<float> patternA = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        std::vector<float> patternB = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
        std::vector<float> patternC = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
        std::vector<float> patternD = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
        
        associativeMemory->associate(patternA, patternB, 0.7f);
        associativeMemory->associate(patternB, patternC, 0.7f);
        associativeMemory->associate(patternC, patternD, 0.7f);
        
        // Test spreading activation from A
        std::vector<std::vector<float>> spread = associativeMemory->spreadActivation(patternA, 3);
        
        bool spreading = spread.size() > 1; // Should include B, C, D
        
        return assertTrue(spreading, "Should spread activation through associative chains");
    }
    
    bool testNeuralAssociativeMemoryIntegration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Create episode with sensory states
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 5;
        episode.action = nlm::ActionType::MoveForward;
        episode.sensoryState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        episode.resultingSensoryState = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        
        // Associate from experience
        associativeMemory->associateFromExperience(episode);
        
        // Verify integration
        bool integrated = associativeMemory->getAssociationCount() > 0;
        
        // Test retrieval based on experience
        std::vector<std::vector<float>> retrieved = associativeMemory->retrieve(episode.sensoryState, 3);
        
        bool canRetrieve = !retrieved.empty();
        
        return assertTrue(integrated, "Associative memory should integrate with episodic memory") &&
               assertTrue(canRetrieve, "Should retrieve based on episodic experience");
    }
    
    bool testMemorySystemIntegration() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify all memory systems are accessible
        bool hasWorkingMemory = brain.getWorkingMemory() != nullptr;
        bool hasEpisodicMemory = brain.getEpisodicMemory() != nullptr;
        bool hasAssociativeMemory = brain.getAssociativeMemory() != nullptr;
        
        // Test coordinated memory operations
        std::vector<float> sensoryInput = {0.7f, 0.3f, 0.9f, 0.1f, 0.5f};
        brain.receiveSensoryInput(*reinterpret_cast<const nlm::SensoryInput*>(&sensoryInput));
        
        // Working memory stores sensory input
        auto* workingMemory = brain.getWorkingMemory();
        workingMemory->store(sensoryInput, 0.8f);
        
        // Create episodic memory from current state
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.action = nlm::ActionType::Wait;
        episode.reward = 0.6f;
        episode.sensoryState = sensoryInput;
        
        auto* episodicMemory = brain.getEpisodicMemory();
        episodicMemory->storeEpisode(episode);
        
        // Associate patterns based on episode
        auto* associativeMemory = brain.getAssociativeMemory();
        associativeMemory->associateFromExperience(episode);
        
        // Test system-wide operations
        brain.step(10);
        
        // Verify all systems are still functional
        bool systemsWorking = workingMemory->getMemoryNeurons().size() > 0 &&
                             episodicMemory->getEpisodeCount() > 0 &&
                             associativeMemory->getAssociationCount() > 0;
        
        return assertTrue(hasWorkingMemory && hasEpisodicMemory && hasAssociativeMemory, 
                         "All memory systems should be initialized") &&
               assertTrue(systemsWorking, "All memory systems should be functional after integration");
    }
    
    bool testMemoryPerformance() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Test performance of working memory operations
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        
        startTimer();
        for (int i = 0; i < 100; ++i) {
            workingMemory->store(pattern, 0.5f + (i * 0.01f));
        }
        double storageTime = stopTimer();
        
        // Test episodic memory operations
        startTimer();
        for (int i = 0; i < 50; ++i) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.sensoryState = std::vector<float>(5, 0.1f * i);
            episode.action = static_cast<nlm::ActionType>(i % 3);
            episode.reward = 0.1f * i;
            episodicMemory->storeEpisode(episode);
        }
        double episodicTime = stopTimer();
        
        // Test associative memory operations
        startTimer();
        for (int i = 0; i < 50; ++i) {
            std::vector<float> patternA(5, 0.1f * i);
            std::vector<float> patternB(5, 0.2f * i);
            associativeMemory->associate(patternA, patternB, 0.5f + (i * 0.01f));
        }
        double associativeTime = stopTimer();
        
        // Performance should be reasonable (less than 1 second per 1000 operations)
        bool performanceOk = storageTime < 1000.0f && episodicTime < 1000.0f && associativeTime < 1000.0f;
        
        return assertTrue(performanceOk, "Memory operations should complete within reasonable time");
    }
    
    bool testMemoryErrorHandling() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Test error handling with empty patterns
        bool emptyPatternHandled = true;
        try {
            std::vector<float> emptyPattern;
            workingMemory->store(emptyPattern, 1.0f);
        } catch (...) {
            emptyPatternHandled = false;
        }
        
        // Test error handling with null episode
        bool nullEpisodeHandled = true;
        try {
            episodicMemory->replayEpisode(nullptr);
        } catch (...) {
            nullEpisodeHandled = false;
        }
        
        // Test memory system resilience after errors
        workingMemory->store({0.5f, 0.5f}, 1.0f);
        bool resilience = workingMemory->getMemoryActivity() > 0.0f;
        
        return assertTrue(emptyPatternHandled, "Should handle empty patterns gracefully") &&
               assertTrue(nullEpisodeHandled, "Should handle null episode gracefully") &&
               assertTrue(resilience, "Memory systems should remain functional after errors");
    }
    
    bool testMemoryConsistency() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        // Store consistent data
        std::vector<float> consistentPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        workingMemory->store(consistentPattern, 0.5f);
        
        nlm::EpisodicMemoryItem consistentEpisode;
        consistentEpisode.timestamp = 1;
        consistentEpisode.sensoryState = consistentPattern;
        consistentEpisode.action = nlm::ActionType::Wait;
        consistentEpisode.reward = 0.7f;
        
        episodicMemory->storeEpisode(consistentEpisode);
        
        // Verify consistency between systems
        std::vector<float> retrievedFromWorking = workingMemory->retrieve();
        
        bool patternConsistent = !retrievedFromWorking.empty() && 
                                std::abs(retrievedFromWorking[0] - consistentPattern[0]) < 0.1f;
        
        // Test memory state consistency after simulation steps
        for (int step = 0; step < 20; ++step) {
            brain.step(step);
        }
        
        bool stateConsistent = workingMemory->getMemoryNeurons().size() > 0 &&
                              episodicMemory->getEpisodeCount() > 0;
        
        return assertTrue(patternConsistent, "Memory patterns should be consistent") &&
               assertTrue(stateConsistent, "Memory state should remain consistent after simulation");
    }
    
    void runAllTests() {
        std::cout << "=== NLM Memory System Integration Tests ===" << std::endl;
        std::cout << "Testing comprehensive memory system functionality:" << std::endl;
        
        MemorySystemTests tester;
        tester.runAllTests();
        
        std::cout << std::endl << "=== Test Summary ===" << std::endl;
        tester.printResults();
    }
    
} // namespace test_memory_system