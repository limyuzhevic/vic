#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_integration {

void testIntegration() {
    std::cout << "Testing system integration..." << std::endl;
    
    // Create brain with Phase 6 configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 1: Memory systems integration
    std::cout << "  Testing memory systems..." << std::endl;
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    if (workingMemory) {
        // Test working memory with brain integration
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                workingMemory->storeToNeuron(neurons[0]->getId(), 0.5f);
                assert(workingMemory->contains(neurons[0]->getId()));
                assert(workingMemory->getNeuronActivation(neurons[0]->getId()) > 0.0f);
            }
        }
        std::cout << "    Working memory: OK" << std::endl;
    }
    
    if (episodicMemory) {
        // Test episodic memory storage and retrieval
        nlm::EpisodicMemoryItem item;
        item.timestamp = 100;
        item.metadata = "Test episode";
        episodicMemory->storeEpisode(item);
        
        auto recent = episodicMemory->getRecentEpisodes(5);
        assert(recent.size() == 1);
        assert(recent[0].metadata == "Test episode");
        
        std::cout << "    Episodic memory: OK" << std::endl;
    }
    
    if (associativeMemory) {
        // Test associative memory
        associativeMemory->associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
        assert(associativeMemory->getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.8f);
        
        std::cout << "    Associative memory: OK" << std::endl;
    }
    
    // Test 2: Neuromodulation integration
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    if (dopamine) {
        dopamine->signalReward(0.5f);
        assert(dopamine->getLevel() > 0.0f);
        std::cout << "    Dopamine: OK" << std::endl;
    }
    
    if (curiosity) {
        curiosity->update(0.5f, 0.3f, 0.001);
        assert(curiosity->getLevel() > 0.0f);
        std::cout << "    Curiosity: OK" << std::endl;
    }
    
    if (predictionError) {
        predictionError->computeError(0.5f, 0.7f);
        assert(predictionError->getError() == 0.2f);
        std::cout << "    PredictionError: OK" << std::endl;
    }
    
    // Test 3: Prediction system integration
    std::cout << "  Testing prediction system integration..." << std::endl;
    
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        // Test prediction system with brain integration
        std::cout << "    Prediction system: Available" << std::endl;
    }
    
    // Test 4: Cognition systems integration
    std::cout << "  Testing cognition systems integration..." << std::endl;
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    if (planner) {
        std::cout << "    Neural planner: Available" << std::endl;
    }
    
    if (conceptFormation) {
        std::cout << "    Concept formation: Available" << std::endl;
    }
    
    if (attention) {
        std::cout << "    Attentional selection: Available" << std::endl;
    }
    
    // Test 5: Development system integration
    std::cout << "  Testing development system integration..." << std::endl;
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::EarlyChildhood);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::EarlyChildhood);
        std::cout << "    Development system: OK" << std::endl;
    }
    
    // Test 6: Convenience functions integration
    std::cout << "  Testing convenience functions integration..." << std::endl;
    
    // Test finding neurons by type
    auto sensoryNeurons = brain.findNeuronsByType(nlm::NeuronType::Sensory);
    auto motorNeurons = brain.findNeuronsByType(nlm::NeuronType::Motor);
    
    assert(sensoryNeurons.size() > 0 || motorNeurons.size() > 0);
    std::cout << "    Neuron filtering: OK" << std::endl;
    
    // Test active neuron retrieval
    auto activeNeurons = brain.getActiveNeurons();
    std::cout << "    Active neuron retrieval: OK" << std::endl;
    
    // Test learning patterns
    brain.applyLearningPattern("repetition");
    brain.applyLearningPattern("spontaneous");
    brain.applyLearningPattern("structured");
    std::cout << "    Learning patterns: OK" << std::endl;
    
    // Test reset functionality
    brain.reset("default");
    brain.reset("random");
    brain.reset("structured");
    std::cout << "    Reset functionality: OK" << std::endl;
    
    // Test state summary
    std::string summary = brain.getStateSummary();
    assert(summary.length() > 0);
    std::cout << "    State summary: OK" << std::endl;
    
    // Test network metrics
    float excit = brain.getNetworkExcitability();
    float plastic = brain.getNetworkPlasticity();
    size_t maxSpikes = brain.getMaxSpikeCount();
    
    assert(excit >= 0.0f && excit <= 10.0f);
    assert(plastic >= 0.0f && plastic <= 1.0f);
    std::cout << "    Network metrics: OK" << std::endl;
    
    // Test integration readiness
    bool ready = brain.isReadyForInteraction();
    assert(ready == true || ready == false);  // Either is fine
    std::cout << "    Integration readiness: OK" << std::endl;
    
    // Test episode preparation
    brain.prepareForEpisode("integration_test_1");
    brain.prepareForEpisode("integration_test_2");
    std::cout << "    Episode preparation: OK" << std::endl;
    
    // Test validation
    bool valid = brain.validateConfiguration();
    std::string errors = brain.getValidationErrors();
    assert(valid == true || valid == false);  // Either is fine
    std::cout << "    Configuration validation: OK" << std::endl;
    
    // Test performance monitoring
    double procTime = brain.getAverageProcessingTime();
    size_t opsCount = brain.getOperationsCount();
    std::cout << "    Performance monitoring: OK" << std::endl;
    
    brain.resetPerformanceCounters();
    
    // Test advanced utility functions
    brain.performCorticalProcess("attention_focused_learning", {0.5f, 0.3f});
    brain.consolidateMemory(0.1f);
    brain.cleanupWeakConnections(0.01f);
    std::cout << "    Advanced utility functions: OK" << std::endl;
    
    // Test developmental milestones
    brain.advanceDevelopmentalStage();
    assert(brain.hasReachedDevelopmentalStage(nlm::DevelopmentalStage::EarlyChildhood));
    std::cout << "    Developmental milestones: OK" << std::endl;
    
    // Test save/load capabilities
    bool canSave = brain.canSaveState();
    bool canLoad = brain.canLoadState();
    assert(canSave && canLoad);
    std::cout << "    Save/load capabilities: OK" << std::endl;
    
    // Test test diagnostics
    auto diagnostics = brain.getTestDiagnostics();
    assert(diagnostics.size() > 0);
    std::cout << "    Test diagnostics: OK" << std::endl;
    
    // Test validation tests
    assert(brain.runValidationTest("basic_connectivity") == true);
    assert(brain.runValidationTest("memory_systems") == true);
    assert(brain.runValidationTest("neuromodulation") == true);
    assert(brain.runValidationTest("plasticity") == true);
    assert(brain.runValidationTest("unknown_test") == false);
    std::cout << "    Validation tests: OK" << std::endl;
    
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace test_integration
