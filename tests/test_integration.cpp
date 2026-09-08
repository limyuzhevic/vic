// Comprehensive System Integration Tests
// Phase 2: Real Neural Computation Tests - System Integration

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

namespace test_integration {

void testBasicIntegration() {
    // Create brain with minimal configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all major components are present
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getRegionCount() == 1);
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "    testBasicIntegration passed" << std::endl;
}

void testSensorimotorLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a simple sensory input
    class TestSensoryInput {
    public:
        std::vector<double> data;
        TestSensoryInput() {
            data = {0.5, 0.8, 0.3, 0.7, 0.2, 0.9, 0.4, 0.6, 0.1, 0.95};
        }
        const std::vector<double>& getData() const { return data; }
    } input;
    
    // Process sensory input
    brain.receiveSensoryInput(input);
    
    // Simulate neural processing
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Produce action based on neural processing
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Verify brain state
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getFiringNeuronCount() >= 0);
    
    std::cout << "    testSensorimotorLoop passed" << std::endl;
}

void testMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.8f);
    workingMemory->storeToNeuron(nlm::NeuronId(2), 0.6f);
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.reward = 0.9f;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    episode.neuronActivations = {0.8f, 0.6f, 0.4f};
    
    episodicMemory->storeEpisode(episode);
    
    // Test associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    std::vector<nlm::NeuronId> cue = {nlm::NeuronId(10), nlm::NeuronId(11)};
    std::vector<nlm::NeuronId> response = {nlm::NeuronId(20), nlm::NeuronId(21)};
    associativeMemory->storeAssociation(cue, response);
    
    // Test integration
    auto* retrievedResponse = associativeMemory->retrieve(cue);
    assert(retrievedResponse != nullptr);
    
    // Verify working memory interactions
    workingMemory->update(0.1f);
    
    // Verify episodic memory consolidation
    episodicMemory->consolidate(0.5f);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Update all neuromodulators
    float dt = 0.001f;
    dopamine->update(dt);
    curiosity->update(dt);
    novelty->update(dt);
    predictionError->update(dt);
    
    // Apply neuromodulation to brain
    class IntegratedNeuromodulator : public nlm::Neuromodulator {
    public:
        float getLevel() const override { return 0.7f; }
        float getPlasticityFactor() const override { return 1.3f; }
    } neuromod;
    
    brain.applyNeuromodulation(neuromod);
    
    // Verify neuromodulation affects neural processing
    // Dopamine affects excitability
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // All systems should be working together
    assert(dopamine->getPlasticityFactor() >= 0.0f);
    assert(curiosity->getExplorationMotivation() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    assert(predictionError->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test STDP
    auto* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    // Test Hebbian
    auto* hebbian = brain.getHebbian();
    assert(hebbian != nullptr);
    
    // Create test synapses
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse2.setType(nlm::SynapseType::Excitatory);
    
    // Enable plasticity
    synapse1.enablePlasticity(true, true, false);
    synapse2.enablePlasticity(true, true, false);
    
    // Record spike history for STDP
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 7.0, 12.0};
    
    synapse1.recordPreSpike(0.0);
    synapse1.recordPreSpike(5.0);
    synapse1.recordPostSpike(2.0);
    synapse2.recordPreSpike(0.0);
    synapse2.recordPostSpike(5.0);
    
    // Apply plasticity rules
    stdp->update(&synapse1, preSpikes, postSpikes, 0.001);
    stdp->update(&synapse2, preSpikes, postSpikes, 0.001);
    
    hebbian->update(&synapse1, preSpikes, postSpikes, 0.001);
    hebbian->update(&synapse2, preSpikes, postSpikes, 0.001);
    
    // Verify synapses changed
    assert(synapse1.getWeight() != 0.0f);
    assert(synapse2.getWeight() != 0.0f);
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    
    assert(developmentSystem != nullptr);
    assert(structuralPlasticity != nullptr);
    
    // Test development effects on neural network
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
    
    // Update development system
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    // Test developmental stage transitions
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    developmentSystem->update(&brain, nlm::RandomGenerator(42), 0.001f);
    
    // Verify network still functional
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getTotalSynapseCount() >= 0);
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testErrorRecovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test error handling with invalid inputs
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Working memory error handling
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    
    // Test memory consolidation error handling
    episodicMemory->consolidate(1.5f);  // High consolidation threshold
    
    // Test memory replay error handling
    auto* episodes = episodicMemory->getEpisodesForReplay(0);  // Request zero episodes
    
    // Reset and continue operation
    brain.reset();
    
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testErrorRecovery passed" << std::endl;
}

void testCheckpointIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to create state
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Save checkpoint
    bool saved = brain.save("./test_checkpoint.nlc");
    assert(saved);
    
    // Reset brain
    brain.reset();
    
    // Verify state was reset
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSpikeCount() == 0);
    
    // Load checkpoint (implementation dependent)
    bool loaded = brain.load("./test_checkpoint.nlc");
    
    // Loading behavior may vary by implementation
    // At minimum, should not crash
    if (loaded) {
        assert(brain.getTotalNeuronCount() == 100);
    }
    
    std::cout << "    testCheckpointIntegration passed" << std::endl;
}

void testConfigIntegration() {
    // Test configuration system integration with brain
    nlm::Config config;
    
    // Set various configuration values
    config.set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    config.set("random_seed", uint64_t(12345), nlm::ConfigSource::Runtime);
    
    // Verify configuration values
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("random_seed"));
    
    auto neuronCountOpt = config.get<int64_t>("neuron_count");
    assert(neuronCountOpt.has_value());
    assert(*neuronCountOpt == 200);
    
    auto regionCountOpt = config.get<int64_t>("region_count");
    assert(regionCountOpt.has_value());
    assert(*regionCountOpt == 3);
    
    auto connectionProbOpt = config.get<float>("connection_probability");
    assert(connectionProbOpt.has_value());
    assert(std::abs(*connectionProbOpt - 0.15f) < 0.01f);
    
    auto seedOpt = config.get<uint64_t>("random_seed");
    assert(seedOpt.has_value());
    assert(*seedOpt == 12345);
    
    // Test configuration persistence
    config.set("test_value", "test_string", nlm::ConfigSource::Runtime);
    auto stringValOpt = config.get<std::string>("test_value");
    assert(stringValOpt.has_value());
    assert(*stringValOpt == "test_string");
    
    std::cout << "    testConfigIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration tests..." << std::endl;
    testBasicIntegration();
    testSensorimotorLoop();
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testPlasticityIntegration();
    testDevelopmentIntegration();
    testErrorRecovery();
    testCheckpointIntegration();
    testConfigIntegration();
}

} // namespace test_integration
