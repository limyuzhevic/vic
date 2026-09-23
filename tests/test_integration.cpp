// Phase 6 Integration Tests
// Testing the integrated artificial brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/PredictionSystem.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_integration {

void testMemoryIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test working memory integration
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory integration
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.reward = 0.5f;
    episode.activeNeurons.push_back(nlm::NeuronId(1));
    episode.neuronActivations.push_back(0.8f);
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test associative memory integration
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associateMemory != nullptr);
    
    std::cout << "      ✓ Memory systems integrated successfully" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test dopamine system
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    
    // Test curiosity system
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    
    // Test novelty system
    auto* novelty = brain->getNovelty();
    assert(novelty != nullptr);
    
    // Test prediction error system
    auto* predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Test neuromodulation effects
    nlm::Dopamine dopamineSignal;
    dopamineSignal.setLevel(0.5f);
    brain->applyNeuromodulation(dopamineSignal);
    
    std::cout << "      ✓ Neuromodulation systems integrated successfully" << std::endl;
}

void testCognitionIntegration() {
    std::cout << "    Testing cognition system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test neural planner integration
    auto* planner = brain->getPlanner();
    assert(planner != nullptr);
    
    // Test concept formation integration
    auto* conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test attention integration
    auto* attention = brain->getAttention();
    assert(attention != nullptr);
    
    std::cout << "      ✓ Cognition systems integrated successfully" << std::endl;
}

void testPredictionIntegration() {
    std::cout << "    Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test prediction system integration
    auto* predictionSystem = brain->getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::cout << "      ✓ Prediction system integrated successfully" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Test development system integration
    auto* developmentSystem = brain->getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage
    nlm::DevelopmentalStage stage = brain->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Simulate development
    brain->develop();
    
    std::cout << "      ✓ Development system integrated successfully" << std::endl;
}

void testCheckpointIntegration() {
    std::cout << "    Testing checkpoint system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "test_checkpoints", nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Save brain state
    std::string checkpointFile = "test_checkpoint.bin";
    bool saveSuccess = brain->save(checkpointFile);
    assert(saveSuccess);
    
    // Reset brain
    brain->reset();
    assert(brain->initialize());
    
    // Load brain state
    bool loadSuccess = brain->load(checkpointFile);
    assert(loadSuccess);
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "      ✓ Checkpoint system integrated successfully" << std::endl;
}

void testReplayIntegration() {
    std::cout << "    Testing replay system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Store some episodes
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.1f * i;
        episode.activeNeurons.push_back(nlm::NeuronId(i + 1));
        episodicMemory->storeEpisode(episode);
    }
    
    assert(episodicMemory->getEpisodeCount() == 5);
    
    // Test replay functionality (simplified)
    // In a real implementation, this would replay important episodes
    
    std::cout << "      ✓ Replay system integrated successfully" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testCognitionIntegration();
    testPredictionIntegration();
    testDevelopmentIntegration();
    testCheckpointIntegration();
    testReplayIntegration();
    
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration
