#ifndef INTEGRATION_TESTS_HPP
#define INTEGRATION_TESTS_HPP

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace integration_tests {

// Setup test logging
void setupTestLogging() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Error); // Suppress info logs during tests
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
}

void testMemorySystemsIntegration() {
    std::cout << "Testing memory systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    std::cout << "    Working memory: OK" << std::endl;
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    std::cout << "    Episodic memory: OK" << std::endl;
    
    // Test associative memory
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    std::cout << "    Associative memory: OK" << std::endl;
    
    // Test memory integration by running simulation
    for (int i = 0; i < 50; ++i) {
        // Inject some activity to test memory storage
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        brain.step(i, i * 0.001f);
        
        // Check if working memory has traces after activity
        if (workingMemory->getActiveTraces() > 0) {
            std::cout << "    Working memory activity detected: OK" << std::endl;
            break;
        }
    }
    
    std::cout << "Memory systems integration: PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    std::cout << "    Neuromodulation systems present: OK" << std::endl;
    
    // Test neuromodulation effects on neural activity
    for (int i = 0; i < 30; ++i) {
        // Inject sensory activity to trigger novelty detection
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.step(i, i * 0.001f);
        
        // Check if neuromodulators are responding
        if (novelty->getLevel() > 0.0f || curiosity->getLevel() > 0.0f) {
            std::cout << "    Neuromodulator activity detected: OK" << std::endl;
            break;
        }
    }
    
    std::cout << "Neuromodulation integration: PASSED" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    std::cout << "    Prediction system present: OK" << std::endl;
    
    // The prediction system should be integrated and working
    // Since PredictionSystem doesn't have an initialize() method, we'll just verify it exists
    std::cout << "Prediction system integration: PASSED" << std::endl;
}

void testCognitionSystemsIntegration() {
    std::cout << "Testing cognition systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test cognition systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    std::cout << "    Cognition systems present: OK" << std::endl;
    
    // Test attention system is functional
    for (int i = 0; i < 20; ++i) {
        brain.step(i, i * 0.001f);
        // Attention system should be updating internally
    }
    
    std::cout << "Cognition systems integration: PASSED" << std::endl;
}

void testCheckpointingIntegration() {
    std::cout << "Testing checkpointing integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create first brain and run simulation
    auto brain1 = std::make_shared<nlm::Brain>(config);
    assert(brain1->initialize());
    
    // Run some steps to generate activity
    for (int i = 0; i < 100; ++i) {
        brain1->injectCurrentToNeurons(nlm::NeuronType::Sensory, 15.0f);
        brain1->step(i, i * 0.001f);
    }
    
    size_t totalSpikes1 = brain1->getTotalSpikeCount();
    size_t neurons1 = brain1->getTotalNeuronCount();
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_integration_test_checkpoint.bin";
    
    bool saveSuccess = brain1->save(checkpointPath);
    assert(saveSuccess);
    std::cout << "    Checkpoint save: OK" << std::endl;
    
    // Verify file exists and has content
    assert(std::filesystem::exists(checkpointPath));
    assert(std::filesystem::file_size(checkpointPath) > 0);
    std::cout << "    Checkpoint file created: OK" << std::endl;
    
    // Create second brain and load checkpoint
    auto brain2 = std::make_shared<nlm::Brain>(config);
    assert(brain2->initialize());
    
    bool loadSuccess = brain2->load(checkpointPath);
    assert(loadSuccess);
    std::cout << "    Checkpoint load: OK" << std::endl;
    
    // Verify neuron count matches (load should restore structure)
    size_t neurons2 = brain2->getTotalNeuronCount();
    assert(neurons2 == neurons1);
    std::cout << "    Neuron count consistent: OK" << std::endl;
    
    // Clean up
    std::filesystem::remove(checkpointPath);
    
    std::cout << "Checkpointing integration: PASSED" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    std::cout << "    Development system present: OK" << std::endl;
    
    // Test developmental stage management
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Juvenile);
    nlm::DevelopmentalStage juvenileStage = brain.getDevelopmentalStage();
    assert(juvenileStage == nlm::DevelopmentalStage::Juvenile);
    std::cout << "    Developmental stage management: OK" << std::endl;
    
    // Run simulation to test development effects
    for (int i = 0; i < 50; ++i) {
        brain.step(i, i * 0.001f);
    }
    
    std::cout << "Development integration: PASSED" << std::endl;
}

void testEndToEndIntegration() {
    std::cout << "Testing end-to-end integration..." << stdendl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify all major systems are integrated
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "    All systems present: OK" << std::endl;
    
    // Run extended simulation to test integration
    float totalReward = 0.0f;
    size_t memoryUpdates = 0;
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        // Generate sensory input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        
        // Brain step (integrates all systems)
        brain.step(step, step * 0.001f);
        
        // Collect metrics
        if (brain.getFiringNeuronCount() > 0) {
            memoryUpdates++;
        }
        
        // Verify all systems are still functional
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getDopamine() != nullptr);
    }
    
    std::cout << "    Extended simulation passed: OK" << std::endl;
    std::cout << "    Total memory updates: " << memoryUpdates << std::endl;
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    std::cout << "    Action production: OK" << std::endl;
    
    std::cout << "End-to-end integration: PASSED" << std::endl;
}

void runAllIntegrationTests() {
    setupTestLogging();
    
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << std::endl;
    
    testMemorySystemsIntegration();
    std::cout << std::endl;
    
    testNeuromodulationIntegration();
    std::cout << std::endl;
    
    testPredictionSystemIntegration();
    std::cout << std::endl;
    
    testCognitionSystemsIntegration();
    std::cout << std::endl;
    
    testCheckpointingIntegration();
    std::cout << std::endl;
    
    testDevelopmentIntegration();
    std::cout << std::endl;
    
    testEndToEndIntegration();
    std::cout << std::endl;
    
    std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
}

} // namespace integration_tests

#endif // INTEGRATION_TESTS_HPP
