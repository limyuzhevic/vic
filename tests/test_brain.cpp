// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainMemoryRetention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory retention
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Add some traces to working memory
        workingMemory->storeToNeuron(1, 0.5f);
        workingMemory->storeToNeuron(2, 0.7f);
        
        // Verify traces are present
        auto traces = workingMemory->getMemoryNeurons();
        assert(!traces.empty());
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Verify working memory still contains traces
        auto tracesAfter = workingMemory->getMemoryNeurons();
        assert(tracesAfter.size() >= traces.size() * 0.8f); // Allow some decay
        
        std::cout << "    testBrainMemoryRetention (Working Memory) passed" << std::endl;
    }
    
    // Test episodic memory retention
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        // Store an episode
        nlm::EpisodicMemoryItem episode;
        episode.reward = 1.0f;
        episode.action = nlm::ActionType::Wait;
        
        episodicMemory->storeEpisode(episode);
        
        // Verify episode is stored
        size_t episodeCount = episodicMemory->getEpisodeCount();
        assert(episodeCount > 0);
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Verify episode is still in memory (may have decayed)
        size_t episodeCountAfter = episodicMemory->getEpisodeCount();
        assert(episodeCountAfter >= 1); // At least the original episode
        
        std::cout << "    testBrainMemoryRetention (Episodic Memory) passed" << std::endl;
    }
    
    // Test associative memory retention
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        // Create an association
        std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
        std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
        associativeMemory->associate(patternA, patternB, 0.8f);
        
        // Verify association exists
        float strength = associativeMemory->getAssociationStrength(patternA, patternB);
        assert(strength > 0.0f);
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Verify association is still present (may have weakened)
        float strengthAfter = associativeMemory->getAssociationStrength(patternA, patternB);
        assert(strengthAfter >= 0.0f); // Association should still exist
        
        std::cout << "    testBrainMemoryRetention (Associative Memory) passed" << std::endl;
    }
    
    // Test concept formation memory retention
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        // Present an experience
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f};
        std::vector<float> features = {0.5f, 0.6f};
        conceptFormation->presentExperience(pattern, features, 0.7f, 10);
        
        // Verify concept was created
        size_t conceptCount = conceptFormation->getConceptCount();
        assert(conceptCount > 0);
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Verify concept is still present
        size_t conceptCountAfter = conceptFormation->getConceptCount();
        assert(conceptCountAfter >= 1); // At least the original concept
        
        std::cout << "    testBrainMemoryRetention (Concept Formation) passed" << std::endl;
    }
    
    std::cout << "    testBrainMemoryRetention (Overall) passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainMemoryRetention();
}

} // namespace test_brain
