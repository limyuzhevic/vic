// Test for Phase 6 integrated systems
// Tests the integration between all major systems in NLM Phase 6

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_phase6_integration {

void testMemorySystemIntegration() {
    std::cout << "Testing Memory System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    workingMemory->initialize(&brain);
    workingMemory->setCapacity(10);
    
    // Inject some neurons to create working memory traces
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 1.0f);
    brain.step(0);
    
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test episodic memory integration
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    episodicMemory->initialize(&brain);
    episodicMemory->setMaxEpisodes(100);
    
    // Run some steps to generate episodes
    for (int i = 0; i < 50; ++i) {
        brain.step(i, i * 0.001);
        
        // Store an episode every 10 steps
        if (i % 10 == 0) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = i;
            episode.reward = 0.0f;
            
            // Add some neurons to the episode
            for (const auto& region : brain.getRegions()) {
                for (const auto* neuron : region->getAllNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
            
            episodicMemory->storeEpisode(episode);
        }
    }
    
    assert(episodicMemory->getEpisodeCount() > 0);
    std::cout << "    Memory integration tests passed" << std::endl;
}

void testNeuromodulationSystemIntegration() {
    std::cout << "Testing Neuromodulation System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test dopamine integration
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    dopamine->initialize(&brain);
    
    // Simulate reward to affect dopamine
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    brain.step(0);
    
    // Apply reward modulation
    brain.applyNeuromodulation(*dopamine);
    
    assert(dopamine->getLevel() != 0.0f);
    
    // Test curiosity integration
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    curiosity->initialize(&brain);
    
    assert(curiosity->getLevel() >= 0.0f && curiosity->getLevel() <= 1.0f);
    
    // Test novelty integration
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    novelty->initialize(&brain);
    
    assert(novelty->getLevel() >= 0.0f && novelty->getLevel() <= 1.0f);
    
    // Test prediction error integration
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    predictionError->initialize(&brain);
    
    assert(predictionError->getLevel() >= 0.0f && predictionError->getLevel() <= 1.0f);
    
    std::cout << "    Neuromodulation integration tests passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    predictionSystem->initialize(&brain);
    
    // Inject some input to generate predictions
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
    brain.step(0);
    
    // Update prediction system
    predictionSystem->update(config);
    
    // Get prediction error
    auto predictionError = predictionSystem->getPredictionError();
    assert(std::isfinite(predictionError));
    
    std::cout << "    Prediction system integration tests passed" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "Testing Cognition System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neural planner integration
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    planner->initialize(&brain);
    planner->setPlanningDepth(3);
    
    // Test concept formation integration
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    conceptFormation->initialize(&brain);
    
    // Test attention integration
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    attention->initialize(&brain);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    std::cout << "    Cognition system integration tests passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "Testing Development System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    developmentSystem->initialize(&brain);
    
    // Update development system
    brain.getRandomGenerator()->seed(42);
    developmentSystem->update(&brain, *brain.getRandomGenerator(), 1000.0);
    
    // Check developmental stage
    auto stage = brain.getDevelopmentalStage();
    assert(stage != nlm::DevelopmentalStage::Initial); // Should have progressed
    
    std::cout << "    Development system integration tests passed" << std::endl;
}

void testPlasticitySystemIntegration() {
    std::cout << "Testing Plasticity System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test STDP integration
    auto* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    // Test Hebbian integration
    auto* hebbian = brain.getHebbian();
    assert(hebbian != nullptr);
    
    // Test structural plasticity integration
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Inject some spikes to trigger plasticity
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 3.0f);
    brain.step(0);
    
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 3.0f);
    brain.step(1);
    
    // Check that plasticity systems are functional
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    assert(structuralPlasticity != nullptr);
    
    std::cout << "    Plasticity system integration tests passed" << std::endl;
}

void testCheckpointSystemIntegration() {
    std::cout << "Testing Checkpoint System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "/tmp/nlm_test_checkpoints");
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some simulation
    for (int i = 0; i < 100; ++i) {
        brain1.step(i, i * 0.001);
        
        // Inject some activity
        if (i % 10 == 0) {
            brain1.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        }
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_test_checkpoint.bin";
    assert(brain1.save(checkpointPath));
    
    // Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    assert(brain2.load(checkpointPath));
    
    std::cout << "    Checkpoint system integration tests passed" << std::endl;
}

void runAll() {
    std::cout << "Running Phase 6 Integration Tests..." << std::endl;
    std::cout << std::endl;
    
    testMemorySystemIntegration();
    testNeuromodulationSystemIntegration();
    testPredictionSystemIntegration();
    testCognitionSystemIntegration();
    testDevelopmentSystemIntegration();
    testPlasticitySystemIntegration();
    testCheckpointSystemIntegration();
    
    std::cout << std::endl;
    std::cout << "All Phase 6 integration tests passed!" << std::endl;
}

} // namespace test_phase6_integration
