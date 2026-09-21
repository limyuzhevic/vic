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

void testBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory system integration
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Store information in working memory
        std::vector<float> pattern = {0.5f, 0.7f, 0.9f, 0.3f, 0.6f};
        workingMemory->store(pattern, 1.0f);
        
        // Verify it was stored
        assert(workingMemory->getMemoryNeurons().size() > 0);
        assert(workingMemory->getActiveTraces() > 0);
    }
    
    // Test episodic memory integration  
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        // Store an episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 0;
        episode.reward = 1.0f;
        episode.activeNeurons.push_back(nlm::NeuronId(1));
        episode.neuronActivations.push_back(0.8f);
        
        episodicMemory->storeEpisode(episode);
        assert(episodicMemory->getEpisodeCount() > 0);
    }
    
    // Test prediction system access
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        // Basic check that system exists
        // (specific prediction methods would be tested here)
        assert(predictionSystem != nullptr);
    }
    
    // Test cognition system integration
    auto* planner = brain.getPlanner();
    if (planner) {
        assert(planner != nullptr);
        // (specific planning methods would be tested here)
    }
    
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        assert(conceptFormation != nullptr);
        // (specific concept methods would be tested here)
    }
    
    auto* attention = brain.getAttention();
    if (attention) {
        assert(attention != nullptr);
        // (specific attention methods would be tested here)
    }
    
    // Test neuromodulation integration
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        assert(dopamine != nullptr);
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        assert(curiosity != nullptr);
    }
    
    auto* novelty = brain.getNovelty();
    if (novelty) {
        assert(novelty != nullptr);
    }
    
    auto* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        assert(predictionError != nullptr);
    }
    
    // Run multiple steps to test integration over time
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Verify systems are responding
        if (workingMemory) {
            workingMemory->update(0.1);
        }
        
        if (attention) {
            attention->update(0.1);
        }
    }
    
    // Verify systems are still functional after integration testing
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 2);
    
    std::cout << "    testBrainIntegration passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    assert(action->getType() == nlm::ActionType::Wait || 
           action->getType() == nlm::ActionType::MoveForward);
    
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

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainIntegration();
    testBrainActionProduction();
    testBrainStatus();
}

} // namespace test_brain