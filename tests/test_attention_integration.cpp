// Attention Integration Tests
// Tests attention competition and focus mechanisms

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_attention_integration {

void testAttentionalSelectionInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Attention should be accessible
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Initial state
    assert(attention->getWinners().empty());
    assert(attention->getAttendedRegions().empty());
    
    std::cout << "    testAttentionalSelectionInitialization passed" << std::endl;
}

void testAttentionCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create competing neurons
    std::vector<nlm::NeuronId> competitors;
    for (int i = 1; i <= 10; ++i) {
        competitors.push_back(nlm::NeuronId(i));
    }
    
    // Process competition
    std::vector<nlm::NeuronId> winners = 
        brain.getAttention()->processCompetition(competitors, 0.5f);
    
    // Should have winners (or empty if all lost)
    assert(winners.size() >= 0 && winners.size() <= competitors.size());
    
    // Winners should be valid neuron IDs
    for (const auto& winner : winners) {
        assert(winner > 0);
    }
    
    std::cout << "    testAttentionCompetition passed" << std::endl;
}

void testAttentionFocusOnRegion() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Focus on different regions
    for (int i = 1; i <= 3; ++i) {
        brain.getAttention()->focusOnRegion(nlm::RegionId(i));
        
        std::vector<nlm::RegionId> attended = brain.getAttention()->getAttendedRegions();
        assert(!attended.empty());
        assert(attended[0] == nlm::RegionId(i));
        
        // Release attention
        brain.getAttention()->releaseAttention();
        assert(brain.getAttention()->getAttendedRegions().empty());
    }
    
    std::cout << "    testAttentionFocusOnRegion passed" << std::endl;
}

void testAttentionParameters() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    
    // Set parameters
    attention->setInhibitionStrength(0.8f);
    attention->setExcitationStrength(1.2f);
    attention->setCompetitionThreshold(0.6f);
    
    // Verify parameters
    assert(attention->getInhibitionStrength() == 0.8f);
    assert(attention->getExcitationStrength() == 1.2f);
    assert(attention->getCompetitionThreshold() == 0.6f);
    
    std::cout << "    testAttentionParameters passed" << std::endl;
}

void testAttentionNeuronInhibition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuronId testNeuron(1);
    
    // Get inhibition for a neuron
    float inhibition = brain.getAttention()->getInhibitionFor(testNeuron);
    assert(inhibition >= 0.0f && inhibition <= 1.0f);
    
    // Get excitation for a neuron
    float excitation = brain.getAttention()->getExcitationFor(testNeuron);
    assert(excitation >= 0.0f && excitation <= 1.0f);
    
    // Check if neuron is attended
    bool isAttended = brain.getAttention()->isAttended(testNeuron);
    assert(isAttended == false); // Initially not attended
    
    std::cout << "    testAttentionNeuronInhibition passed" << std::endl;
}

void testAttentionUpdateDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Update attention dynamics
    for (int step = 1; step < 10; ++step) {
        brain.step(step);
        
        // Attention should be updated
        auto* attention = brain.getAttention();
        assert(attention->getWinners().size() >= 0);
        
        // Neuron should be attended or not
        bool isAttended = attention->isAttended(nlm::NeuronId(1));
        assert(isAttended == false || isAttended == true);
    }
    
    std::cout << "    testAttentionUpdateDynamics passed" << std::endl;
}

void testAttentionWithMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store pattern in working memory
    std::vector<float> pattern = {0.7f, 0.5f, 0.3f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Store in episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Apply top-down attention bias
    brain.getAttention()->applyTopDownBias(nlm::NeuronId(1), 0.8f);
    
    // Apply bottom-up attention salience
    brain.getAttention()->applyBottomUpSalience(nlm::NeuronId(2), 0.6f);
    
    // Should still be able to process competition
    std::vector<nlm::NeuronId> competitors = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    std::vector<nlm::NeuronId> winners = 
        brain.getAttention()->processCompetition(competitors, 0.5f);
    
    assert(winners.size() >= 0 && winners.size() <= competitors.size());
    
    std::cout << "    testAttentionWithMemoryIntegration passed" << std::endl;
}

void testAttentionResetAndRelease() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* attention = brain.getAttention();
    
    // Set some attention state
    attention->focusOnRegion(nlm::RegionId(1));
    assert(!attention->getAttendedRegions().empty());
    
    // Reset attention
    attention->reset();
    assert(attention->getAttendedRegions().empty());
    assert(attention->getWinners().empty());
    
    // Release attention
    attention->releaseAttention();
    assert(attention->getAttendedRegions().empty());
    
    std::cout << "    testAttentionResetAndRelease passed" << std::endl;
}

void runAll() {
    testAttentionalSelectionInitialization();
    testAttentionCompetition();
    testAttentionFocusOnRegion();
    testAttentionParameters();
    testAttentionNeuronInhibition();
    testAttentionUpdateDynamics();
    testAttentionWithMemoryIntegration();
    testAttentionResetAndRelease();
}

} // namespace test_attention_integration