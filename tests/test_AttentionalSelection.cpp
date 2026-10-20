// AttentionalSelection Tests
#include "memory/NeuralWorkingMemory.hpp"  // Using this header for AttentionalSelection
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_attention_selection {

void testAttentionCreation() {
    nlm::AttentionalSelection attention;
    
    assert(attention.getAttendedRegions().empty());
    assert(attention.getWinners().empty());
    assert(attention.getInhibitionStrength() == 0.5f);  // Default
    assert(attention.getExcitationStrength() == 1.5f);   // Default
    assert(attention.getCompetitionThreshold() == 0.3f); // Default
    
    std::cout << "    testAttentionCreation passed" << std::endl;
}

void testAttentionInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    attention.initialize(&brain);
    
    assert(attention.getAttendedRegions().empty());
    assert(attention.getWinners().empty());
    
    std::cout << "    testAttentionInitialization passed" << std::endl;
}

void testAttentionParameters() {
    nlm::AttentionalSelection attention;
    
    // Test default parameters
    assert(attention.getInhibitionStrength() == 0.5f);
    assert(attention.getExcitationStrength() == 1.5f);
    assert(attention.getCompetitionThreshold() == 0.3f);
    
    // Set new parameters
    attention.setInhibitionStrength(0.7f);
    attention.setExcitationStrength(2.0f);
    attention.setCompetitionThreshold(0.5f);
    
    assert(attention.getInhibitionStrength() == 0.7f);
    assert(attention.getExcitationStrength() == 2.0f);
    assert(attention.getCompetitionThreshold() == 0.5f);
    
    std::cout << "    testAttentionParameters passed" << std::endl;
}

void testAttentionFocus() {
    nlm::AttentionalSelection attention;
    
    assert(attention.getAttendedRegions().empty());
    
    // Focus on a region
    attention.focusOnRegion(nlm::RegionId(1));
    assert(attention.getAttendedRegions().size() == 1);
    assert(attention.getAttendedRegions()[0] == nlm::RegionId(1));
    
    // Focus on another region
    attention.focusOnRegion(nlm::RegionId(2));
    assert(attention.getAttendedRegions().size() == 2);
    assert(attention.getAttendedRegions()[1] == nlm::RegionId(2));
    
    // Release attention
    attention.releaseAttention();
    assert(attention.getAttendedRegions().empty());
    
    std::cout << "    testAttentionFocus passed" << std::endl;
}

void testAttentionCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    attention.initialize(&brain);
    
    // Create competitor neurons
    std::vector<nlm::NeuronId> competitors = {
        nlm::NeuronId(1001),
        nlm::NeuronId(1002),
        nlm::NeuronId(1003),
        nlm::NeuronId(1004),
        nlm::NeuronId(1005)
    };
    
    // Apply some top-down and bottom-up biases
    attention.applyTopDownBias(competitors[0], 0.8f);
    attention.applyTopDownBias(competitors[2], 0.6f);
    attention.applyBottomUpSalience(competitors[1], 0.9f);
    attention.applyBottomUpSalience(competitors[4], 0.7f);
    
    // Process competition
    float globalInhibition = 0.5f;
    std::vector<nlm::NeuronId> winners = attention.processCompetition(competitors, globalInhibition);
    
    // Should select some winners
    assert(!winners.empty());
    assert(winners.size() <= competitors.size());
    
    // Check that winners have higher activation (implicitly)
    for (const auto& winner : winners) {
        assert(attention.isAttended(winner));
    }
    
    std::cout << "    testAttentionCompetition passed" << std::endl;
}

void testAttentionInhibition() {
    nlm::AttentionalSelection attention;
    
    // Test inhibition for a neuron that's not a winner
    nlm::NeuronId neuron = nlm::NeuronId(2001);
    float inhibition = attention.getInhibitionFor(neuron);
    assert(inhibition == attention.getInhibitionStrength());
    
    // Create a winner set
    std::vector<nlm::NeuronId> winners = {neuron};
    
    // Note: This is tricky to test without accessing private members
    // We'll test the API that we can access
    
    std::cout << "    testAttentionInhibition passed" << std::endl;
}

void testAttentionExcitation() {
    nlm::AttentionalSelection attention;
    
    // Test excitation for a neuron that's not a winner
    nlm::NeuronId neuron = nlm::NeuronId(3001);
    float excitation = attention.getExcitationFor(neuron);
    assert(excitation == 0.0f);  // Not attended, no excitation
    
    // The logic is that only winners get excitation
    
    std::cout << "    testAttentionExcitation passed" << std::endl;
}

void testAttentionUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    attention.initialize(&brain);
    
    // Apply some biases
    attention.applyTopDownBias(nlm::NeuronId(4001), 0.8f);
    attention.applyBottomUpSalience(nlm::NeuronId(4002), 0.9f);
    
    // Update attention (should decay biases)
    attention.update(1.0f);
    
    // Biases should have decayed (this is internal, so we just ensure no crash)
    
    std::cout << "    testAttentionUpdate passed" << std::endl;
}

void testAttentionIsAttended() {
    nlm::AttentionalSelection attention;
    
    assert(!attention.isAttended(nlm::NeuronId(5001)));
    
    // Note: We can't directly add winners without accessing private members
    // But we can test the API
    
    std::cout << "    testAttentionIsAttended passed" << std::endl;
}

void testAttentionReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    attention.initialize(&brain);
    
    // Apply some biases
    attention.applyTopDownBias(nlm::NeuronId(6001), 0.8f);
    attention.applyBottomUpSalience(nlm::NeuronId(6002), 0.9f);
    attention.focusOnRegion(nlm::RegionId(1));
    
    // Reset
    attention.reset();
    
    // Should be back to initial state
    assert(attention.getAttendedRegions().empty());
    // Note: We can't easily verify biases were cleared without accessing private members
    
    std::cout << "    testAttentionReset passed" << std::endl;
}

void testAttentionGlobalInhibition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AttentionalSelection attention;
    attention.initialize(&brain);
    
    // Create competitors
    std::vector<nlm::NeuronId> competitors = {
        nlm::NeuronId(7001), nlm::NeuronId(7002),
        nlm::NeuronId(7003), nlm::NeuronId(7004)
    };
    
    // Apply different biases
    attention.applyTopDownBias(competitors[0], 1.0f);
    attention.applyTopDownBias(competitors[1], 0.5f);
    attention.applyTopDownBias(competitors[2], 0.2f);
    attention.applyTopDownBias(competitors[3], 0.8f);
    
    // Test with different global inhibition levels
    float strongInhibition = 0.9f;
    auto winnersStrong = attention.processCompetition(competitors, strongInhibition);
    
    float weakInhibition = 0.1f;
    auto winnersWeak = attention.processCompetition(competitors, weakInhibition);
    
    // With strong inhibition, fewer neurons should win
    assert(winnersStrong.size() <= winnersWeak.size());
    
    std::cout << "    testAttentionGlobalInhibition passed" << std::endl;
}

void runAll() {
    testAttentionCreation();
    testAttentionInitialization();
    testAttentionParameters();
    testAttentionFocus();
    testAttentionCompetition();
    testAttentionInhibition();
    testAttentionExcitation();
    testAttentionUpdate();
    testAttentionIsAttended();
    testAttentionReset();
    testAttentionGlobalInhibition();
}

} // namespace test_attention_selection
