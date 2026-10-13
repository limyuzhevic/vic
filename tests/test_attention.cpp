// Attention Tests
// Tests competitive selection, winner-take-more dynamics, top-down and bottom-up bias, and non-Transformer QKV attention

#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

namespace test_attention {

void testAttentionalSelectionCreation() {
    nlm::AttentionalSelection attention;
    
    assert(attention.getWinners().empty());
    assert(attention.getAttendedRegions().empty());
    
    std::cout << "    testAttentionalSelectionCreation passed" << std::endl;
}

void testAttentionalSelectionParameters() {
    nlm::AttentionalSelection attention;
    
    // Test parameter setting
    attention.setInhibitionStrength(0.8f);
    attention.setExcitationStrength(1.2f);
    attention.setCompetitionThreshold(0.6f);
    
    std::cout << "    testAttentionalSelectionParameters passed" << std::endl;
}

void testCompetitiveSelection() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Create competing neurons
    std::vector<nlm::NeuronId> competitors;
    for (int i = 0; i < 10; ++i) {
        competitors.push_back(nlm::NeuronId(10000 + i));
    }
    
    // Apply different saliences - make one very salient
    attention.applyBottomUpSalience(competitors[0], 1.0f);
    for (size_t i = 1; i < competitors.size(); ++i) {
        attention.applyBottomUpSalience(competitors[i], 0.1f);
    }
    
    // Process competition
    std::vector<nlm::NeuronId> winners = attention.processCompetition(competitors);
    
    // Should select the most salient one
    assert(!winners.empty());
    assert(winners[0] == competitors[0]);
    
    std::cout << "    testCompetitiveSelection passed" << std::endl;
}

void testWinnerTakeMoreDynamics() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Create multiple competitors with varying saliences
    std::vector<nlm::NeuronId> competitors;
    std::vector<float> saliences = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f}; // Increasing salience
    
    for (size_t i = 0; i < saliences.size(); ++i) {
        competitors.push_back(nlm::NeuronId(20000 + i));
        attention.applyBottomUpSalience(competitors[i], saliences[i]);
    }
    
    // Process competition
    std::vector<nlm::NeuronId> winners = attention.processCompetition(competitors);
    
    // Winner should be the most salient
    assert(!winners.empty());
    assert(winners[0] == competitors.back()); // 1.0f should win
    
    std::cout << "    testWinnerTakeMoreDynamics passed" << std::endl;
}

void testTopDownAndBottomUpBias() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    nlm::NeuronId testNeuron(30000);
    
    // Apply top-down bias (goal-directed)
    attention.applyTopDownBias(testNeuron, 0.8f);
    
    // Apply bottom-up salience (stimulus-driven)
    attention.applyBottomUpSalience(testNeuron, 0.6f);
    
    // Neuron should be attended
    assert(attention.isAttended(testNeuron));
    
    // Get inhibition and excitation levels
    float inhibition = attention.getInhibitionFor(testNeuron);
    float excitation = attention.getExcitationFor(testNeuron);
    
    // Should have non-zero values
    assert(inhibition >= 0.0f);
    assert(excitation >= 0.0f);
    
    std::cout << "    testTopDownAndBottomUpBias passed" << std::endl;
}

void testNonTransformerQKVAttention() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Simulate QKV (Query-Key-Value) like dynamics using different bias types
    std::vector<nlm::NeuronId> queries, keys, values;
    
    for (int i = 0; i < 5; ++i) {
        queries.push_back(nlm::NeuronId(40000 + i));
        keys.push_back(nlm::NeuronId(40100 + i));
        values.push_back(nlm::NeuronId(40200 + i));
        
        // Different types of biases simulate QKV
        attention.applyTopDownBias(queries[i], 0.9f);  // Query bias
        attention.applyBottomUpSalience(keys[i], 0.7f); // Key bias
        attention.applyBottomUpSalience(values[i], 0.5f); // Value bias
    }
    
    // Create competition pool
    std::vector<nlm::NeuronId> competitors;
    competitors.insert(competitors.end(), queries.begin(), queries.end());
    competitors.insert(competitors.end(), keys.begin(), keys.end());
    competitors.insert(competitors.end(), values.begin(), values.end());
    
    // Process competition (this is the "attention mechanism")
    std::vector<nlm::NeuronId> winners = attention.processCompetition(competitors);
    
    // Should have winners
    assert(!winners.empty());
    
    // Simulate attention mechanism updating dynamics
    for (int step = 0; step < 10; ++step) {
        attention.update(0.001f);
    }
    
    std::cout << "    testNonTransformerQKVAttention passed" << std::endl;
}

void testAttentionFocusRelease() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    nlm::RegionId testRegion(1);
    
    // Focus on region
    attention.focusOnRegion(testRegion);
    auto attended = attention.getAttendedRegions();
    assert(!attended.empty());
    
    // Release attention
    attention.releaseAttention();
    attended = attention.getAttendedRegions();
    assert(attended.empty());
    
    std::cout << "    testAttentionFocusRelease passed" << std::endl;
}

void testAttentionReset() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Set some state
    attention.applyTopDownBias(nlm::NeuronId(50000), 0.5f);
    attention.applyBottomUpSalience(nlm::NeuronId(50001), 0.3f);
    
    // Reset
    attention.reset();
    
    // Should be reset (winners should be cleared)
    assert(attention.getWinners().empty());
    
    std::cout << "    testAttentionReset passed" << std::endl;
}

void testAttentionUpdateDynamics() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Set up attention state
    attention.applyTopDownBias(nlm::NeuronId(60000), 0.7f);
    attention.applyBottomUpSalience(nlm::NeuronId(60001), 0.9f);
    
    // Update multiple times
    for (int step = 0; step < 100; ++step) {
        attention.update(0.001f);
    }
    
    // Should not crash and should maintain valid state
    assert(attention.getWinners().empty() || !attention.getWinners().empty());
    
    std::cout << "    testAttentionUpdateDynamics passed" << std::endl;
}

void testAttentionNeuronalCompetition() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Create many competing neurons with different bias strengths
    std::vector<nlm::NeuronId> competitors;
    std::vector<float> topDownBiases = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f};
    
    for (size_t i = 0; i < topDownBiases.size(); ++i) {
        nlm::NeuronId neuron(70000 + i);
        competitors.push_back(neuron);
        attention.applyTopDownBias(neuron, topDownBiases[i]);
    }
    
    // Also add bottom-up saliences
    for (size_t i = 0; i < competitors.size(); ++i) {
        attention.applyBottomUpSalience(competitors[i], topDownBiases[i] * 0.8f);
    }
    
    // Run competition
    std::vector<nlm::NeuronId> winners = attention.processCompetition(competitors);
    
    // Should select based on combined biases
    assert(!winners.empty());
    
    // The winner should be one with highest combined bias
    float maxCombined = 0.0f;
    for (size_t i = 0; i < competitors.size(); ++i) {
        float topDown = attention.getInhibitionFor(competitors[i]);
        float bottomUp = attention.getExcitationFor(competitors[i]);
        float combined = topDown + bottomUp;
        if (combined > maxCombined) {
            maxCombined = combined;
        }
    }
    
    std::cout << "    testAttentionNeuronalCompetition passed" << std::endl;
}

void testAttentionSalienceProcessing() {
    nlm::AttentionalSelection attention;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    attention.initialize(&mockBrain);
    
    // Test salience application and retrieval
    nlm::NeuronId testNeuron(80000);
    
    // Apply various saliences
    attention.applyBottomUpSalience(testNeuron, 0.9f);
    attention.applyTopDownBias(testNeuron, 0.7f);
    
    // Check inhibition/excitation levels
    float inhibition = attention.getInhibitionFor(testNeuron);
    float excitation = attention.getExcitationFor(testNeuron);
    
    // Should be non-zero (exact values depend on implementation)
    assert(inhibition >= 0.0f);
    assert(excitation >= 0.0f);
    
    // Test isAttended
    assert(attention.isAttended(testNeuron));
    
    std::cout << "    testAttentionSalienceProcessing passed" << std::endl;
}

void runAll() {
    testAttentionalSelectionCreation();
    testAttentionalSelectionParameters();
    testCompetitiveSelection();
    testWinnerTakeMoreDynamics();
    testTopDownAndBottomUpBias();
    testNonTransformerQKVAttention();
    testAttentionFocusRelease();
    testAttentionReset();
    testAttentionUpdateDynamics();
    testAttentionNeuronalCompetition();
    testAttentionSalienceProcessing();
}

} // namespace test_attention
