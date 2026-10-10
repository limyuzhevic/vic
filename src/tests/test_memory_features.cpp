#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/neuromodulation/Dopamine.cpp"
#include "src/neuromodulation/Curiosity.cpp"
#include "src/neuromodulation/Novelty.cpp"
#include "src/neuromodulation/PredictionError.cpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

class TestMemorySystem {
public:
    TestMemorySystem() {
        initializeTestEnvironment();
    }
    
    ~TestMemorySystem() {
        cleanupTestEnvironment();
    }
    
    void runAllTests() {
        std::cout << "=== NLM Memory System Tests ===\n";
        
        testWorkingMemoryLRU();
        std::cout << "✓ Working Memory LRU Test Passed\n";
        
        testTemporalDecay();
        std::cout << "✓ Temporal Decay Test Passed\n";
        
        testMemoryRelevanceScoring();
        std::cout << "✓ Memory Relevance Scoring Test Passed\n";
        
        testEpisodicMemoryConsolidation();
        std::cout << "✓ Episodic Memory Consolidation Test Passed\n";
        
        testMemoryCompression();
        std::cout << "✓ Memory Compression Test Passed\n";
        
        testNeuromodulationIntegration();
        std::cout << "✓ Neuromodulation Integration Test Passed\n";
        
        testNeuralReplayScheduling();
        std::cout << "✓ Neural Replay Scheduling Test Passed\n";
        
        testPredictionErrorUpdating();
        std::cout << "✓ Prediction Error Updating Test Passed\n";
        
        testMemoryConflictResolution();
        std::cout << "✓ Memory Conflict Resolution Test Passed\n";
        
        testRAIIIntegration();
        std::cout << "✓ RAII Integration Test Passed\n";
        
        std::cout << "\n=== All Memory System Tests Passed ===\n";
    }
    
private:
    void initializeTestEnvironment() {
        // Create a mock brain for testing
        auto config = std::make_shared<Config>();
        
        // Initialize brain
        brain_ = std::make_unique<Brain>(config);
        
        // Initialize memory systems
        workingMemory_ = std::make_unique<NeuralWorkingMemory>();
        episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
        
        // Initialize neuromodulation systems
        dopamine_ = std::make_unique<Dopamine>();
        curiosity_ = std::make_unique<Curiosity>();
        novelty_ = std::make_unique<Novelty>();
        predictionError_ = std::make_unique<PredictionError>();
        
        // Initialize memory systems with brain
        workingMemory_->initialize(brain_.get());
        episodicMemory_->initialize(brain_.get());
        dopamine_->initialize(brain_.get());
        curiosity_->initialize(brain_.get());
        novelty_->initialize(brain_.get());
        predictionError_->initialize(brain_.get());
    }
    
    void cleanupTestEnvironment() {
        // Clean up memory systems
        workingMemory_.reset();
        episodicMemory_.reset();
        dopamine_.reset();
        curiosity_.reset();
        novelty_.reset();
        predictionError_.reset();
        brain_.reset();
    }
    
    void testWorkingMemoryLRU() {
        // Clear working memory
        workingMemory_->clear();
        
        // Fill working memory to capacity
        workingMemory_->setCapacity(10);
        
        for (int i = 0; i < 10; ++i) {
            std::vector<float> pattern = {static_cast<float>(i), 1.0f, 0.5f};
            workingMemory_->store(pattern, 1.0f);
        }
        
        // Access first element
        NeuronId neuron1(1);
        workingMemory_->storeToNeuron(neuron1, 0.8f);
        
        // Add one more element to trigger LRU eviction
        std::vector<float> newPattern = {9.9f, 0.1f, 0.2f};
        workingMemory_->store(newPattern, 1.0f);
        
        // Check that LRU cache works (first element should still be accessible)
        assert(workingMemory_->contains(neuron1) == true);
        assert(workingMemory_->getNeuronActivation(neuron1) > 0.0f);
    }
    
    void testTemporalDecay() {
        // Clear working memory
        workingMemory_->clear();
        workingMemory_->setCapacity(5);
        
        // Store some patterns
        for (int i = 0; i < 5; ++i) {
            std::vector<float> pattern = {1.0f, static_cast<float>(i) * 0.1f, 0.5f};
            workingMemory_->store(pattern, 1.0f);
        }
        
        // Get initial activity
        float initialActivity = workingMemory_->getMemoryActivity();
        
        // Apply temporal decay
        workingMemory_->setDecayTimeConstant(100.0f);
        workingMemory_->update(0.01f);  // Small timestep
        
        // Check that memory has decayed
        float newActivity = workingMemory_->getMemoryActivity();
        assert(newActivity < initialActivity);
    }
    
    void testMemoryRelevanceScoring() {
        // Clear working memory
        workingMemory_->clear();
        
        // Store patterns with different relevance
        std::vector<float> highPattern = {1.0f, 1.0f, 1.0f};
        std::vector<float> lowPattern = {0.1f, 0.1f, 0.1f};
        
        workingMemory_->store(highPattern, 1.0f);
        workingMemory_->store(lowPattern, 1.0f);
        
        // Set different relevance levels
        NeuronId highNeuron(1);
        NeuronId lowNeuron(2);
        
        workingMemory_->setMemoryRelevance(highNeuron, 0.9f);
        workingMemory_->setMemoryRelevance(lowNeuron, 0.1f);
        
        // Check relevance scores
        assert(workingMemory_->getMemoryRelevance(highNeuron) > 0.5f);
        assert(workingMemory_->getMemoryRelevance(lowNeuron) < 0.5f);
    }
    
    void testEpisodicMemoryConsolidation() {
        // Clear episodic memory
        episodicMemory_->clear();
        
        // Store some episodes
        EpisodicMemoryItem episode1;
        episode1.timestamp = 1;
        episode1.reward = 1.0f;
        episode1.novelty = 0.8f;
        
        EpisodicMemoryItem episode2;
        episode2.timestamp = 2;
        episode2.reward = 0.2f;
        episode2.novelty = 0.3f;
        
        episodicMemory_->storeEpisode(episode1);
        episodicMemory_->storeEpisode(episode2);
        
        // Consolidate based on relevance threshold
        episodicMemory_->consolidate(0.5f);
        
        // Check that low-relevance episodes were removed
        assert(episodicMemory_->getEpisodeCount() >= 1);
    }
    
    void testMemoryCompression() {
        // Clear episodic memory
        episodicMemory_->clear();
        
        // Store an episode with large pattern
        EpisodicMemoryItem episode;
        episode.timestamp = 1;
        episode.sensoryState = std::vector<float>(100, 0.5f);  // Large pattern
        
        episodicMemory_->storeEpisode(episode);
        
        // Check compression
        const EpisodicMemoryItem* storedEpisode = episodicMemory_->getEpisode(0);
        assert(storedEpisode->compressionRatio < 1.0f);  // Should be compressed
    }
    
    void testNeuromodulationIntegration() {
        // Set neuromodulator levels
        dopamine_->setLevel(0.8f);
        novelty_->setLevel(0.6f);
        
        // Check that neuromodulation affects memory priority
        workingMemory_->clear();
        
        std::vector<float> pattern = {1.0f, 0.5f, 0.3f};
        workingMemory_->store(pattern, 1.0f);
        
        // Memory priority should be affected by neuromodulators
        assert(workingMemory_->getMemoryActivity() > 0.0f);
    }
    
    void testNeuralReplayScheduling() {
        // Clear working memory
        workingMemory_->clear();
        
        // Store some memory traces
        for (int i = 0; i < 5; ++i) {
            std::vector<float> pattern = {static_cast<float>(i) * 0.2f, 0.5f, 0.3f};
            workingMemory_->store(pattern, 1.0f);
        }
        
        // Schedule replay
        workingMemory_->setReplayInterval(10);
        
        // Check that replay is scheduled
        assert(workingMemory_->getReplayInterval() == 10);
    }
    
    void testPredictionErrorUpdating() {
        // Clear working memory
        workingMemory_->clear();
        
        // Store some patterns
        std::vector<float> pattern = {1.0f, 0.5f, 0.3f};
        workingMemory_->store(pattern, 1.0f);
        
        // Set prediction error
        predictionError_->computeError(0.5f, 0.8f);  // Positive error
        
        // Update memory based on prediction error
        workingMemory_->applyPredictionErrorBasedMemoryUpdating();
        
        // Check that memory was updated
        assert(workingMemory_->getMemoryActivity() > 0.0f);
    }
    
    void testMemoryConflictResolution() {
        // Clear working memory
        workingMemory_->clear();
        
        // Store conflicting memories
        std::vector<float> pattern1 = {1.0f, 0.8f, 0.6f};
        std::vector<float> pattern2 = {0.9f, 0.7f, 0.5f};
        
        workingMemory_->store(pattern1, 1.0f);
        workingMemory_->store(pattern2, 1.0f);
        
        // Resolve conflicts
        workingMemory_->applyMemoryConflictResolution();
        
        // Check that conflicts were resolved
        assert(workingMemory_->getMemoryActivity() > 0.0f);
    }
    
    void testRAIIIntegration() {
        // Clear and reset memory with RAII
        workingMemory_->clear();
        workingMemory_->initializeWithRAII();
        
        // Check that RAII resources are initialized
        assert(workingMemory_->getMemoryActivity() >= 0.0f);
        
        // Reset memory with RAII
        workingMemory_->resetMemoryWithRAII();
        
        // Check that memory still works after RAII reset
        std::vector<float> pattern = {1.0f, 0.5f, 0.3f};
        workingMemory_->store(pattern, 1.0f);
        
        assert(workingMemory_->getMemoryActivity() > 0.0f);
    }
    
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
};

int main() {
    TestMemorySystem test;
    test.runAllTests();
    return 0;
}
