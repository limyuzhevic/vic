#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_phase6_integration {

void testPhase6SystemConnectivity() {
    std::cout << "    Testing Phase 6 system connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test that core systems are initialized
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);  // Working memory is integrated
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);  // Episodic memory is integrated
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);  // Prediction system is integrated
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);  // Planning system is integrated
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);  // Development system is integrated
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);  // Neuromodulation is integrated
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);  // Neuromodulation is integrated
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);  // Neuromodulation is integrated
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);  // Neuromodulation is integrated
    
    std::cout << "    testPhase6SystemConnectivity passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "    Testing memory systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory is accessible
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Test episodic memory is accessible
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test memory retrieval
    auto* retrievedMem = brain.getWorkingMemory();
    assert(retrievedMem != nullptr);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulator components
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test dopamine system
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Test neuromodulation update via brain step
    brain.step(0);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "    Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system update via brain step
    brain.step(0);
    
    std::cout << "    testPredictionSystemIntegration passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development system update via brain step
    brain.step(0);
    
    // Test development stage
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::Maturation);
    
    std::cout << "    testDevelopmentSystemIntegration passed" << std::endl;
}

void testMemoryConnectivity() {
    std::cout << "    Testing memory system connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory connectivity
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Test episodic memory connectivity
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test episodic memory update via brain step
    brain.step(0);
    
    std::cout << "    testMemoryConnectivity passed" << std::endl;
}

void testNeuromodulationConnectivity() {
    std::cout << "    Testing neuromodulation system connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test all neuromodulator components connectivity
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test neuromodulation update via brain step
    brain.step(0);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    std::cout << "    testNeuromodulationConnectivity passed" << std::endl;
}

void testCheckpointingIntegration() {
    std::cout << "    Testing checkpointing system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test checkpoint system via brain save/load
    bool saved = brain.save("/tmp/test_checkpoint.bin");
    assert(saved);
    
    // Reset brain and load
    brain.reset();
    bool loaded = brain.load("/tmp/test_checkpoint.bin");
    assert(loaded);
    
    std::cout << "    testCheckpointingIntegration passed" << std::endl;
}

void testReplayIntegration() {
    std::cout << "    Testing replay system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test replay system (implicit in step method)
    brain.step(0);
    
    std::cout << "    testReplayIntegration passed" << std::endl;
}

void runAll() {
    testPhase6SystemConnectivity();
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testPredictionSystemIntegration();
    testDevelopmentSystemIntegration();
    testMemoryConnectivity();
    testNeuromodulationConnectivity();
    testCheckpointingIntegration();
    testReplayIntegration();
}

} // namespace test_phase6_integration
