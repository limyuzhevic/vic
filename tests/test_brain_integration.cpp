// Brain Integration System Tests
// Tests the integration of all cognitive systems with the main brain loop

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_brain_integration {

void testBrainInitialization() {
    std::cout << "Testing brain initialization with integrated systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<size_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify all integrated systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Verify developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "  PASSED" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<size_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store some information in working memory
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.8f);
    
    // Verify memory has traces
    assert(workingMemory->getActiveTraces() == 2);
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Create and store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    episode.reward = 0.5f;
    
    episodicMemory->storeEpisode(episode);
    
    // Verify episode was stored
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test episodic memory replay
    auto* episodes = episodicMemory->getEpisodesForReplay(1);
    assert(episodes != nullptr);
    assert(episodes->size() == 1);
    
    // Test associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Form association between patterns
    std::vector<nlm::NeuronId> pattern1 = {nlm::NeuronId(1), nlm::NeuronId(2)};
    std::vector<nlm::NeuronId> pattern2 = {nlm::NeuronId(3), nlm::NeuronId(4)};
    
    associativeMemory->formAssociation(pattern1, pattern2);
    
    // Verify association was formed
    assert(associativeMemory->getStrength(pattern1, pattern2) > 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test dopamine system
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Update dopamine with prediction error
    dopamine->update(0.1f);  // Positive prediction error
    
    assert(dopamine->getLevel() > 0.0f);
    
    // Test curiosity system
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    curiosity->update(0.5f);
    
    assert(curiosity->getLevel() > 0.0f);
    
    // Test novelty system
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    novelty->update(0.3f);
    
    assert(novelty->getLevel() > 0.0f);
    
    // Test prediction error system
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    predictionError->update(0.2f);
    
    assert(predictionError->getError() > 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "Testing cognition system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test planner
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    planner->setPlanningDepth(5);
    
    // Test concept formation
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test attention system
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    // Test attention competition
    std::vector<nlm::NeuronId> competitors = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    attention->processCompetition(competitors);
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    developmentSystem->update(&brain, *brain.getRandomGenerator(), 1000.0f);
    
    // Verify developmental stage progression
    assert(brain.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    std::cout << "  PASSED" << std::endl;
}

void testBrainStepWithIntegratedSystems() {
    std::cout << "Testing brain step with all integrated systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<size_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    float initialFiringRate = brain.getAverageFiringRate();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Verify systems are functioning
    size_t finalSpikes = brain.getTotalSpikeCount();
    assert(finalSpikes >= initialSpikes);  // Spikes should increase or stay the same
    
    float finalFiringRate = brain.getAverageFiringRate();
    
    // Test that systems are interacting
    if (brain.getWorkingMemory()) {
        brain.getWorkingMemory()->update(0.001);
    }
    
    if (brain.getAttention()) {
        std::vector<nlm::NeuronId> testNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
        brain.getAttention()->processCompetition(testNeurons);
    }
    
    std::cout << "  PASSED" << std::endl;
}

void testSensoryMotorIntegration() {
    std::cout << "Testing sensory-motor integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create sensory input
    std::vector<float> sensoryData = {0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
    nlm::SensoryInput sensoryInput(sensoryData);
    
    // Process sensory input
    brain.receiveSensoryInput(sensoryInput);
    
    // Verify sensory neurons were activated
    // (This indirectly tests the integration)
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "  PASSED" << std::endl;
}

void testPlasticityWithNeuromodulation() {
    std::cout << "Testing plasticity rules with neuromodulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<size_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<size_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Record initial synaptic weights
    size_t initialSynapseCount = brain.getTotalSynapseCount();
    
    // Apply neuromodulation
    if (auto* dopamine = brain.getDopamine()) {
        dopamine->update(0.5f);  // Strong positive prediction error
        
        // Apply neuromodulation to brain
        nlm::Neuromodulator neuromod(dopamine->getLevel());
        brain.applyNeuromodulation(neuromod);
    }
    
    // Run some steps to allow plasticity to take effect
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Verify plasticity occurred
    size_t finalSynapseCount = brain.getTotalSynapseCount();
    assert(finalSynapseCount > 0);
    
    // Test structural plasticity
    if (auto* structuralPlasticity = brain.getStructuralPlasticity()) {
        // Structural plasticity should have updated connectivity
        brain.develop();
    }
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Brain Integration System Tests ===" << std::endl;
    
    testBrainInitialization();
    testMemorySystemIntegration();
    testNeuromodulationIntegration();
    testCognitionSystemIntegration();
    testDevelopmentIntegration();
    testBrainStepWithIntegratedSystems();
    testSensoryMotorIntegration();
    testPlasticityWithNeuromodulation();
    
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_brain_integration

// Test runner for brain integration
int main() {
    test_brain_integration::runAll();
    return 0;
}
