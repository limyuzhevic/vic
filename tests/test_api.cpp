#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_api {

void testAPISurface() {
    std::cout << "Testing API surface..." << std::endl;
    
    // Create brain for testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test 1: Core API surface
    std::cout << "  Test 1: Core API surface..." << std::endl;
    
    // Configuration API
    assert(brain.getConfig() != nullptr);
    assert(brain.getRandomGenerator() != nullptr);
    
    // Region API
    auto regions = brain.getRegionIds();
    assert(regions.size() == 2);
    
    auto* region1 = brain.getRegion(nlm::RegionId(1));
    auto* region2 = brain.getRegion(nlm::RegionId(2));
    assert(region1 != nullptr);
    assert(region2 != nullptr);
    
    // Neuron API
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getActiveNeuronCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Statistics API
    float eiRatio = brain.getExcitationInhibitionRatio();
    float avgFiring = brain.getAverageFiringRate();
    assert(eiRatio >= 0.0f);
    assert(avgFiring >= 0.0f);
    
    // Action API
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    Core API: OK" << std::endl;
    
    // Test 2: Neuromodulation API
    std::cout << "  Test 2: Neuromodulation API..." << std::endl;
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    if (dopamine) {
        dopamine->signalReward(0.5f);
        dopamine->signalRewardPredictionError(0.2f);
        dopamine->update(0.001);
        std::cout << "    Dopamine API: OK" << std::endl;
    }
    
    if (curiosity) {
        curiosity->update(0.3f, 0.2f, 0.001);
        std::cout << "    Curiosity API: OK" << std::endl;
    }
    
    if (novelty) {
        novelty->update(0.001);
        std::cout << "    Novelty API: OK" << std::endl;
    }
    
    if (predictionError) {
        predictionError->computeError(0.5f, 0.7f);
        std::cout << "    PredictionError API: OK" << std::endl;
    }
    
    // Test 3: Memory systems API
    std::cout << "  Test 3: Memory systems API..." << std::endl;
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    if (workingMemory) {
        // Test working memory API
        auto* region = brain.getRegion(nlm::RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                workingMemory->storeToNeuron(neurons[0]->getId(), 0.5f);
                assert(workingMemory->contains(neurons[0]->getId()));
            }
        }
        std::cout << "    Working memory API: OK" << std::endl;
    }
    
    if (episodicMemory) {
        // Test episodic memory API
        nlm::EpisodicMemoryItem item;
        item.metadata = "Test episode";
        episodicMemory->storeEpisode(item);
        
        auto recent = episodicMemory->getRecentEpisodes(5);
        assert(recent.size() == 1);
        std::cout << "    Episodic memory API: OK" << std::endl;
    }
    
    if (associativeMemory) {
        // Test associative memory API
        associativeMemory->associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
        assert(associativeMemory->getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.8f);
        std::cout << "    Associative memory API: OK" << std::endl;
    }
    
    // Test 4: Prediction system API
    std::cout << "  Test 4: Prediction system API..." << std::endl;
    
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        std::cout << "    Prediction system API: OK" << std::endl;
    }
    
    // Test 5: Cognition systems API
    std::cout << "  Test 5: Cognition systems API..." << std::endl;
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    if (planner) {
        std::cout << "    Neural planner API: OK" << std::endl;
    }
    
    if (conceptFormation) {
        std::cout << "    Concept formation API: OK" << std::endl;
    }
    
    if (attention) {
        std::cout << "    Attentional selection API: OK" << std::endl;
    }
    
    // Test 6: Development system API
    std::cout << "  Test 6: Development system API..." << std::endl;
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::EarlyChildhood);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::EarlyChildhood);
        std::cout << "    Development system API: OK" << std::endl;
    }
    
    // Test 7: Convenience functions API
    std::cout << "  Test 7: Convenience functions API..." << std::endl;
    
    // All convenience functions tested in previous test, now just verify they exist
    auto sensoryNeurons = brain.findNeuronsByType(nlm::NeuronType::Sensory);
    auto activeNeurons = brain.getActiveNeurons();
    
    std::string summary = brain.getStateSummary();
    float excit = brain.getNetworkExcitability();
    float plastic = brain.getNetworkPlasticity();
    size_t maxSpikes = brain.getMaxSpikeCount();
    
    assert(summary.length() > 0);
    assert(excit >= 0.0f && excit <= 10.0f);
    assert(plastic >= 0.0f && plastic <= 1.0f);
    std::cout << "    Convenience functions API: OK" << std::endl;
    
    // Test 8: Advanced utility functions API
    std::cout << "  Test 8: Advanced utility functions API..." << std::endl;
    
    brain.performCorticalProcess("attention_focused_learning", {0.5f, 0.3f});
    brain.consolidateMemory(0.1f);
    brain.cleanupWeakConnections(0.01f);
    std::cout << "    Advanced utility functions API: OK" << std::endl;
    
    // Test 9: Developmental milestones API
    std::cout << "  Test 9: Developmental milestones API..." << std::endl;
    
    brain.advanceDevelopmentalStage();
    assert(brain.hasReachedDevelopmentalStage(nlm::DevelopmentalStage::EarlyChildhood));
    std::cout << "    Developmental milestones API: OK" << std::endl;
    
    // Test 10: Integration checkpoint API
    std::cout << "  Test 10: Integration checkpoint API..." << std::endl;
    
    bool canSave = brain.canSaveState();
    bool canLoad = brain.canLoadState();
    assert(canSave && canLoad);
    std::cout << "    Integration checkpoint API: OK" << std::endl;
    
    // Test 11: Test diagnostics API
    std::cout << "  Test 11: Test diagnostics API..." << std::endl;
    
    auto diagnostics = brain.getTestDiagnostics();
    assert(diagnostics.size() > 0);
    std::cout << "    Test diagnostics API: OK" << std::endl;
    
    // Test 12: Validation tests API
    std::cout << "  Test 12: Validation tests API..." << std::endl;
    
    assert(brain.runValidationTest("basic_connectivity") == true);
    assert(brain.runValidationTest("memory_systems") == true);
    assert(brain.runValidationTest("neuromodulation") == true);
    assert(brain.runValidationTest("plasticity") == true);
    assert(brain.runValidationTest("unknown_test") == false);
    std::cout << "    Validation tests API: OK" << std::endl;
    
    // Test 13: State management API
    std::cout << "  Test 13: State management API..." << std::endl;
    
    brain.reset("default");
    brain.reset("random");
    brain.reset("structured");
    std::cout << "    State management API: OK" << std::endl;
    
    // Test 14: Performance monitoring API
    std::cout << "  Test 14: Performance monitoring API..." << std::endl;
    
    brain.resetPerformanceCounters();
    double procTime = brain.getAverageProcessingTime();
    size_t opsCount = brain.getOperationsCount();
    std::cout << "    Performance monitoring API: OK" << std::endl;
    
    // Test 15: Logging API
    std::cout << "  Test 15: Logging API..." << std::endl;
    
    brain.logStatus();
    std::cout << "    Logging API: OK" << std::endl;
    
    std::cout << "All API surface tests passed!" << std::endl;
}

} // namespace test_api
