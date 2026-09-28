// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

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
    
    // Test memory system access
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    size_t initialNeurons = brain.getTotalNeuronCount();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify brain is still functional
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getActiveNeuronCount() >= 0);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Generate some activity
    brain.step(0);
    size_t spikesBefore = brain.getTotalSpikeCount();
    
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);  // Should still have synapses after reset
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
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
        
        // Test region properties
        assert(region->getId() == rid);
        assert(region->getName() != "");
        assert(region->getTotalNeuronCount() > 0);
    }
    
    // Test adding a new region
    nlm::RegionId newRegionId = brain.addRegion("NewRegion");
    assert(newRegionId.index() > 0);
    assert(brain.getRegionCount() == 4);
    
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
    
    // Verify action has valid type
    assert(static_cast<int>(action->getType()) >= 0);
    
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
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    
    // Test developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainSensoryMotor() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get sensory neurons
    auto neurons = brain.getRegion(nlm::RegionId(1))->getAllNeurons();
    
    // Find sensory neuron and inject current
    for (auto* neuron : neurons) {
        if (neuron->getType() == nlm::NeuronType::Sensory) {
            neuron->injectCurrent(10.0f);  // Strong input
            break;
        }
    }
    
    // Step the brain
    brain.step(0);
    
    // Verify action was produced
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainSensoryMotor passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Test working memory
    assert(workingMemory != nullptr);
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test episodic memory
    assert(episodicMemory != nullptr);
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 1.0f;
    episode.activeNeurons.push_back(nlm::NeuronId(1));
    episode.neuronActivations.push_back(0.8f);
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Test associative memory
    assert(associativeMemory != nullptr);
    associativeMemory->storePattern({nlm::NeuronId(1), nlm::NeuronId(2)}, 0.9f);
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Test neuromodulation updates
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify neuromodulators are accessible
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    std::cout << "    testBrainNeuromodulation passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainSensoryMotor();
    testBrainMemorySystems();
    testBrainNeuromodulation();
}

} // namespace test_brain
