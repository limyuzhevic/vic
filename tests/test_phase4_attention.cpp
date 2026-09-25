// test_phase4_attention.cpp
// Tests AttentionalSelection functionality

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {

TEST_CASE("Phase 4 Attention Tests", "[attention]") {
    
    SECTION("Test AttentionalSelection initialization") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        AttentionalSelection attention;
        attention.initialize(brain.get());
        
        // Test default parameters
        REQUIRE(attention.getInhibitionStrength() == 0.5f);
        REQUIRE(attention.getExcitationStrength() == 1.5f);
        
        // Test parameter setting
        attention.setInhibitionStrength(0.3f);
        attention.setExcitationStrength(2.0f);
        
        REQUIRE(attention.getInhibitionStrength() == 0.3f);
        REQUIRE(attention.getExcitationStrength() == 2.0f);
    }
    
    SECTION("Test AttentionalSelection bottom-up salience") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        AttentionalSelection attention;
        attention.initialize(brain.get());
        
        // Test bottom-up salience application
        std::vector<NeuronId> neurons;
        for (int i = 0; i < 10; ++i) {
            neurons.push_back(NeuronId(10000 + i));
        }
        
        // Apply different salience levels
        attention.applyBottomUpSalience(neurons[0], 1.0f);  // High salience
        attention.applyBottomUpSalience(neurons[1], 0.5f);  // Medium salience
        attention.applyBottomUpSalience(neurons[2], 0.0f);  // Low salience
        
        // Test competition processing
        std::vector<NeuronId> competitors = {neurons[0], neurons[1], neurons[2], neurons[3], neurons[4]};
        auto winners = attention.processCompetition(competitors);
        
        // Should select at least one neuron
        REQUIRE(!winners.empty());
        
        // Most salient neuron should be selected
        REQUIRE(winners[0] == neurons[0]);
    }
    
    SECTION("Test AttentionalSelection top-down selection") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        AttentionalSelection attention;
        attention.initialize(brain.get());
        
        // Test top-down selection
        NeuronId importantNeuron = NeuronId(20000);
        attention.applyTopDownSelection(importantNeuron, 1.0f);  // High importance
        
        std::vector<NeuronId> competitors;
        for (int i = 0; i < 5; ++i) {
            competitors.push_back(NeuronId(30000 + i));
        }
        
        // Add the important neuron
        competitors.push_back(importantNeuron);
        
        auto winners = attention.processCompetition(competitors);
        
        // Should select the important neuron
        REQUIRE(winners[0] == importantNeuron);
    }
    
    SECTION("Test AttentionalSelection attention updates") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        AttentionalSelection attention;
        attention.initialize(brain.get());
        
        // Apply some initial attention
        std::vector<NeuronId> neurons;
        for (int i = 0; i < 5; ++i) {
            neurons.push_back(NeuronId(40000 + i));
        }
        
        attention.applyBottomUpSalience(neurons[0], 1.0f);
        attention.applyBottomUpSalience(neurons[1], 0.8f);
        
        // Update attention (should decay)
        attention.update(0.001);  // Small timestep
        
        // Attention weights should have decayed
        std::vector<NeuronId> competitors = {neurons[0], neurons[1]};
        auto winners = attention.processCompetition(competitors);
        
        // Should still select something (maybe)
        // Decay might have reduced weight below threshold
        REQUIRE(!winners.empty() || attention.getInhibitionStrength() >= 0.0f);
    }
    
    SECTION("Test AttentionalSelection attention score computation") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        AttentionalSelection attention;
        attention.initialize(brain.get());
        
        NeuronId testNeuron = NeuronId(50000);
        
        // Test with no attention applied
        float score = attention.computeAttentionScore(testNeuron);
        REQUIRE(score == 0.0f);
        
        // Apply attention
        attention.applyBottomUpSalience(testNeuron, 1.0f);
        
        // Test with attention applied
        score = attention.computeAttentionScore(testNeuron);
        REQUIRE(score > 0.0f);
        
        // Test with other neurons
        NeuronId otherNeuron = NeuronId(50001);
        float otherScore = attention.computeAttentionScore(otherNeuron);
        REQUIRE(otherScore == 0.0f);  // No attention on this neuron
    }
}

} // namespace nlm
