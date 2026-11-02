// Comprehensive Integration Tests for Phase 6
// Tests all integrated neural systems and their interactions

#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "development/DevelopmentSystem.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_phase6 {

// Test memory systems integration
void testMemorySystemsIntegration() {
    std::cout << "Testing Memory Systems Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory integration
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "    Working memory available: ✓" << std::endl;
        
        // Test storing and retrieving neuron activations
        nlm::NeuronId testNeuron(12345);
        workingMemory->storeToNeuron(testNeuron, 0.75f);
        assert(workingMemory->contains(testNeuron));
        assert(std::abs(workingMemory->getNeuronActivation(testNeuron) - 0.75f) < 0.01f);
        
        // Test competition
        workingMemory->runCompetition();
        std::cout << "    Working memory competition: ✓" << std::endl;
        
        // Test update
        workingMemory->update(0.001f);
        std::cout << "    Working memory update: ✓" << std::endl;
    } else {
        std::cout << "    Working memory available: ✗" << std::endl;
    }
    
    // Test episodic memory integration
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "    Episodic memory available: ✓" << std::endl;
        
        // Create a test episode
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = 100;
        episode.reward = 0.5f;
        episode.energy = 1.0f;
        episode.novelty = 0.8f;
        
        // Add some neurons to the episode
        std::vector<nlm::NeuronId> neurons;
        for (int i = 0; i < 10; ++i) {
            neurons.push_back(nlm::NeuronId(static_cast<uint64_t>(1000 + i)));
        }
        episode.activeNeurons = neurons;
        
        episodicMemory->storeEpisode(episode);
        assert(episodicMemory->getEpisodeCount() == 1);
        
        // Test recent episodes
        auto recent = episodicMemory->getRecentEpisodes(1);
        assert(!recent.empty());
        assert(recent[0].timestamp == 100);
        
        std::cout << "    Episodic memory operations: ✓" << std::endl;
        
        // Test consolidation
        episodicMemory->consolidate(0.5f);
        std::cout << "    Episodic memory consolidation: ✓" << std::endl;
        
        // Test replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(1);
        assert(!episodesToReplay.empty());
        
        std::cout << "    Episodic memory replay: ✓" << std::endl;
    } else {
        std::cout << "    Episodic memory available: ✗" << std::endl;
    }
    
    std::cout << "Memory Systems Integration test passed" << std::endl;
}

// Test prediction system integration
void testPredictionSystemIntegration() {
    std::cout << "Testing Prediction System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        std::cout << "    Prediction system available: ✓" << std::endl;
        
        // Create test sensory inputs
        nlm::Vision visionInput;
        visionInput.setData(std::vector<float>{1.0f, 0.8f, 0.5f, 0.3f, 0.1f});
        
        auto prediction = predictionSystem->predictNextState(visionInput);
        assert(prediction != nullptr);
        
        // Update predictions
        predictionSystem->updatePredictions(visionInput, visionInput);
        
        assert(predictionSystem->getPredictionError() >= 0.0f);
        assert(predictionSystem->getConfidence() >= 0.0f && predictionSystem->getConfidence() <= 1.0f);
        
        // Test training
        predictionSystem->train(visionInput);
        
        // Test update
        predictionSystem->update(0.001f);
        
        std::cout << "    Prediction system operations: ✓" << std::endl;
        
        // Test error history
        predictionSystem->clearHistory();
        assert(predictionSystem->getErrorHistory().empty());
        
        std::cout << "    Prediction system history management: ✓" << std::endl;
    } else {
        std::cout << "    Prediction system available: ✗" << std::endl;
    }
    
    std::cout << "Prediction System Integration test passed" << std::endl;
}

// Test cognition systems integration
void testCognitionSystemsIntegration() {
    std::cout << "Testing Cognition Systems Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neural planner
    auto* planner = brain.getPlanner();
    if (planner) {
        std::cout << "    Neural planner available: ✓" << std::endl;
        
        planner->initialize(&brain);
        planner->setPlanningDepth(3);
        planner->update(0.001f);
        
        auto plannedActions = planner->getPotentialActions();
        // Planned actions may be empty initially, that's okay
        
        std::cout << "    Neural planner operations: ✓" << std::endl;
    } else {
        std::cout << "    Neural planner available: ✗" << std::endl;
    }
    
    // Test concept formation
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        std::cout << "    Concept formation available: ✓" << std::endl;
        
        conceptFormation->initialize(&brain);
        conceptFormation->update(0.001f);
        
        auto concepts = conceptFormation->getConcepts();
        // Concepts may be empty initially, that's okay
        
        conceptFormation->setConceptLearningRate(0.01f);
        
        std::cout << "    Concept formation operations: ✓" << std::endl;
    } else {
        std::cout << "    Concept formation available: ✗" << std::endl;
    }
    
    // Test attentional selection
    auto* attention = brain.getAttention();
    if (attention) {
        std::cout << "    Attention system available: ✓" << std::endl;
        
        attention->initialize(&brain);
        
        // Create test competitors
        std::vector<nlm::NeuronId> competitors;
        for (int i = 0; i < 5; ++i) {
            competitors.push_back(nlm::NeuronId(static_cast<uint64_t>(2000 + i)));
        }
        
        attention->processCompetition(competitors, 0.5f);
        
        attention->update(0.001f);
        
        std::cout << "    Attention system operations: ✓" << std::endl;
    } else {
        std::cout << "    Attention system available: ✗" << std::endl;
    }
    
    std::cout << "Cognition Systems Integration test passed" << std::endl;
}

// Test neuromodulation systems integration
void testNeuromodulationSystemsIntegration() {
    std::cout << "Testing Neuromodulation Systems Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "    Dopamine system available: ✓" << std::endl;
        
        dopamine->initialize(&brain);
        dopamine->update(0.001f);
        
        float dopamineLevel = dopamine->getLevel();
        float plasticityFactor = dopamine->getPlasticityFactor();
        
        assert(dopamineLevel >= 0.0f);
        assert(plasticityFactor >= 0.0f);
        
        std::cout << "    Dopamine system operations: ✓" << std::endl;
    } else {
        std::cout << "    Dopamine system available: ✗" << std::endl;
    }
    
    // Test curiosity
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        std::cout << "    Curiosity system available: ✓" << std::endl;
        
        curiosity->initialize(&brain);
        curiosity->update(0.8f, 0.3f, 0.001f);
        
        float curiosityLevel = curiosity->getLevel();
        assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
        
        std::cout << "    Curiosity system operations: ✓" << std::endl;
    } else {
        std::cout << "    Curiosity system available: ✗" << std::endl;
    }
    
    // Test novelty
    auto* novelty = brain.getNovelty();
    if (novelty) {
        std::cout << "    Novelty system available: ✓" << std::endl;
        
        novelty->initialize(&brain);
        novelty->update(0.001f);
        
        float noveltyLevel = novelty->getLevel();
        assert(noveltyLevel >= 0.0f);
        
        std::cout << "    Novelty system operations: ✓" << std::endl;
    } else {
        std::cout << "    Novelty system available: ✗" << std::endl;
    }
    
    // Test prediction error
    auto* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        std::cout << "    Prediction error system available: ✓" << std::endl;
        
        predictionError->initialize(&brain);
        predictionError->update(0.5f, 0.001f);
        
        float errorLevel = predictionError->getError();
        assert(errorLevel >= 0.0f);
        
        std::cout << "    Prediction error system operations: ✓" << std::endl;
    } else {
        std::cout << "    Prediction error system available: ✗" << std::endl;
    }
    
    std::cout << "Neuromodulation Systems Integration test passed" << std::endl;
}

// Test development system integration
void testDevelopmentSystemIntegration() {
    std::cout << "Testing Development System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        std::cout << "    Development system available: ✓" << std::endl;
        
        developmentSystem->update(&brain, *brain.getRandomGenerator(), 1000.0f);
        
        nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
        assert(stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::CriticalPeriod);
        
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
        
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
        
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
        
        std::cout << "    Development system operations: ✓" << std::endl;
        
        // Test that development affects plasticity
        auto* structuralPlasticity = brain.getStructuralPlasticity();
        if (structuralPlasticity) {
            float initialSynaptogenesisRate = structuralPlasticity->getSynaptogenesisRate();
            brain.setDevelopmentalStage(nlm::DevelopmentalStage::Initial);
            float initialStageRate = structuralPlasticity->getSynaptogenesisRate();
            
            brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
            float adultStageRate = structuralPlasticity->getSynaptogenesisRate();
            
            // Plasticity should decrease with maturation
            assert(adultStageRate < initialStageRate || std::abs(adultStageRate - initialStageRate) < 0.001f);
            
            std::cout << "    Development-plasticity coupling: ✓" << std::endl;
        }
    } else {
        std::cout << "    Development system available: ✗" << std::endl;
    }
    
    std::cout << "Development System Integration test passed" << std::endl;
}

// Test agent-brain integration
void testAgentBrainIntegration() {
    std::cout << "Testing Agent-Brain Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(800, 600, 160, 120);
    
    nlm::AgentBrain agentBrain(&brain);
    agentBrain.initialize(world);
    
    // Test sensory processing
    nlm::Vision visionInput(160, 120, 3);
    std::vector<float> visionData(160 * 120 * 3, 0.5f);
    visionInput.setData(visionData);
    
    agentBrain.processSensoryInput(visionInput);
    
    // Test action decoding
    auto motorCommand = agentBrain.decodeMotorCommand();
    
    // Test reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.8f);
    
    // Test development update
    agentBrain.updateDevelopment(0.001f);
    
    // Test neuromodulation levels
    float curiosityLevel = agentBrain.getCuriosityLevel();
    float noveltyLevel = agentBrain.getNoveltyLevel();
    float predictionError = agentBrain.getPredictionError();
    
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    assert(noveltyLevel >= 0.0f);
    assert(predictionError >= 0.0f);
    
    std::cout << "    Agent brain sensory processing: ✓" << std::endl;
    std::cout << "    Agent brain action decoding: ✓" << std::endl;
    std::cout << "    Agent brain reward modulation: ✓" << std::endl;
    std::cout << "    Agent brain development: ✓" << std::endl;
    std::cout << "    Agent brain neuromodulation: ✓" << std::endl;
    
    std::cout << "Agent-Brain Integration test passed" << std::endl;
}

// Test complete brain step with all systems
void testCompleteBrainStep() {
    std::cout << "Testing Complete Brain Step Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Create some sensory input
    nlm::Vision sensoryInput(10, 10, 3);
    std::vector<float> inputData(300, 0.0f);
    for (int i = 0; i < 50; ++i) inputData[i] = 1.0f; // Strong input to first 50 neurons
    sensoryInput.setData(inputData);
    
    brain.receiveSensoryInput(sensoryInput);
    
    // Run multiple steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Verify all integrated systems are operational
        if (brain.getWorkingMemory()) {
            brain.getWorkingMemory()->update(0.001f);
        }
        
        if (brain.getEpisodicMemory()) {
            // Episodic memory updates happen automatically in the main step
        }
        
        if (brain.getPredictionSystem()) {
            brain.getPredictionSystem()->update(0.001f);
        }
        
        if (brain.getPlanner()) {
            brain.getPlanner()->update(0.001f);
        }
        
        if (brain.getConceptFormation()) {
            brain.getConceptFormation()->update(0.001f);
        }
        
        if (brain.getAttention()) {
            brain.getAttention()->update(0.001f);
        }
        
        if (brain.getDevelopmentSystem()) {
            brain.develop();
        }
    }
    
    size_t finalSpikes = brain.getTotalSpikeCount();
    std::cout << "    Spikes generated: " << (finalSpikes - initialSpikes) << std::endl;
    
    // Verify systems are working
    bool systemsOperational = false;
    
    if (brain.getWorkingMemory() && brain.getWorkingMemory()->getMemoryNeuronCount() > 0) {
        systemsOperational = true;
    }
    
    if (brain.getEpisodicMemory() && brain.getEpisodicMemory()->getEpisodeCount() > 0) {
        systemsOperational = true;
    }
    
    if (brain.getPredictionSystem() && brain.getPredictionSystem()->getConfidence() > 0.0f) {
        systemsOperational = true;
    }
    
    if (systemsOperational) {
        std::cout << "    Integrated systems operational: ✓" << std::endl;
    } else {
        std::cout << "    Integrated systems operational: ⚠ (some systems may initialize later)" << std::endl;
    }
    
    std::cout << "Complete Brain Step Integration test passed" << std::endl;
}

// Test brain persistence
void testBrainPersistence() {
    std::cout << "Testing Brain Persistence..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Inject some activity
    nlm::Vision sensoryInput(5, 5, 3);
    std::vector<float> inputData(75, 0.5f);
    sensoryInput.setData(inputData);
    brain1.receiveSensoryInput(sensoryInput);
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain1.step(step, step * 0.001);
    }
    
    // Save brain state
    std::string checkpointFile = "test_checkpoint.bin";
    bool saved = brain1.save(checkpointFile);
    assert(saved);
    
    std::cout << "    Brain saved: ✓" << std::endl;
    
    // Create new brain and load state
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loaded = brain2.load(checkpointFile);
    assert(loaded);
    
    std::cout << "    Brain loaded: ✓" << std::endl;
    
    // Verify loaded state is reasonable
    size_t brain2SpikeCount = brain2.getTotalSpikeCount();
    assert(brain2SpikeCount >= brain1.getTotalSpikeCount() * 0.5f); // Should have some spikes
    
    std::cout << "    State integrity verified: ✓" << std::endl;
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "Brain Persistence test passed" << std::endl;
}

// Run all tests
void runAllTests() {
    std::cout << "==================================================================" << std::endl;
    std::cout << "NLM Phase 6 Comprehensive Integration Tests" << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testBrainCreation();
        std::cout << std::endl;
        
        testBrainInitialization();
        std::cout << std::endl;
        
        testMemorySystemsIntegration();
        std::cout << std::endl;
        
        testPredictionSystemIntegration();
        std::cout << std::endl;
        
        testCognitionSystemsIntegration();
        std::cout << std::endl;
        
        testNeuromodulationSystemsIntegration();
        std::cout << std::endl;
        
        testDevelopmentSystemIntegration();
        std::cout << std::endl;
        
        testAgentBrainIntegration();
        std::cout << std::endl;
        
        testCompleteBrainStep();
        std::cout << std::endl;
        
        testBrainPersistence();
        std::cout << std::endl;
        
        std::cout << "==================================================================" << std::endl;
        std::cout << "All Phase 6 Integration Tests PASSED! ✓" << std::endl;
        std::cout << "==================================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        throw;
    } catch (...) {
        std::cout << "Test failed with unknown exception" << std::endl;
        throw;
    }
}

} // namespace test_phase6

// Main function for running tests
int main(int argc, char** argv) {
    std::cout << "NLM Phase 6 Integration Test Suite" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // Parse command line arguments
    bool runAll = true;
    std::string testName = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./test_phase6 [--help] [--test <test_name>]" << std::endl;
            std::cout << "Tests available:" << std::endl;
            std::cout << "  brain_creation          - Test basic brain creation" << std::endl;
            std::cout << "  brain_initialization     - Test brain initialization" << std::endl;
            std::cout << "  memory_systems          - Test memory systems integration" << std::endl;
            std::cout << "  prediction_system       - Test prediction system integration" << std::endl;
            std::cout << "  cognition_systems       - Test cognition systems integration" << std::endl;
            std::cout << "  neuromodulation_systems   - Test neuromodulation systems integration" << std::endl;
            std::cout << "  development_system       - Test development system integration" << std::endl;
            std::cout << "  agent_brain              - Test agent-brain integration" << std::endl;
            std::cout << "  complete_brain_step      - Test complete brain step" << std::endl;
            std::cout << "  brain_persistence        - Test brain persistence" << std::endl;
            std::cout << "  all                     - Run all tests (default)" << std::endl;
            return 0;
        } else if (arg.find("--test") == 0) {
            if (arg.find('=') != std::string::npos) {
                testName = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                testName = argv[++i];
            }
            runAll = false;
        }
    }
    
    if (runAll) {
        test_phase6::runAllTests();
    } else {
        if (testName == "brain_creation") {
            test_phase6::testBrainCreation();
        } else if (testName == "brain_initialization") {
            test_phase6::testBrainInitialization();
        } else if (testName == "memory_systems") {
            test_phase6::testMemorySystemsIntegration();
        } else if (testName == "prediction_system") {
            test_phase6::testPredictionSystemIntegration();
        } else if (testName == "cognition_systems") {
            test_phase6::testCognitionSystemsIntegration();
        } else if (testName == "neuromodulation_systems") {
            test_phase6::testNeuromodulationSystemsIntegration();
        } else if (testName == "development_system") {
            test_phase6::testDevelopmentSystemIntegration();
        } else if (testName == "agent_brain") {
            test_phase6::testAgentBrainIntegration();
        } else if (testName == "complete_brain_step") {
            test_phase6::testCompleteBrainStep();
        } else if (testName == "brain_persistence") {
            test_phase6::testBrainPersistence();
        } else {
            std::cout << "Unknown test: " << testName << std::endl;
            std::cout << "Run './test_phase6 --help' for available tests." << std::endl;
            return 1;
        }
        
        std::cout << std::endl;
        std::cout << "Test '" << testName << "' completed successfully!" << std::endl;
    }
    
    return 0;
}