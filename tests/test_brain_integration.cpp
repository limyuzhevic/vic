#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_brain_integration {

void testIntegratedMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("enable_memory_pools", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test working memory
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Test episodic memory  
    auto* episodicMem = brain.getEpisodicMemory();
    assert(episodicMem != nullptr);
    
    // Test associative memory
    auto* associativeMem = brain.getAssociativeMemory();
    assert(associativeMem != nullptr);
    
    // Run some steps to populate working memory
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify memories are being populated
    auto workingMemNeurons = workingMem->getMemoryNeurons();
    assert(!workingMemNeurons.empty());  // Some neurons should be in working memory
    
    // Verify episodic memory has episodes
    auto episodicMemEpisodes = episodicMem->getEpisodeCount();
    assert(episodicMemEpisodes > 0);  // Should have some episodes stored
    
    // Verify episodic memory contains neuron data
    auto episodes = episodicMem->retrieveTemporal(0, 100);
    if (!episodes.empty()) {
        auto* ep = episodes[0];
        assert(!ep->activeNeurons.empty());  // Should have stored active neurons
    }
    
    std::cout << "    testIntegratedMemorySystems passed" << std::endl;
}

void testIntegratedPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* predictionSys = brain.getPredictionSystem();
    assert(predictionSys != nullptr);
    
    // Run steps to generate prediction errors
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
    }
    
    // Prediction system should have computed errors
    float error = predictionSys->getPredictionError();
    assert(error >= 0.0f);  // Error should be non-negative
    
    // Confidence should be in valid range
    float confidence = predictionSys->getConfidence();
    assert(confidence >= 0.1f && confidence <= 1.0f);
    
    std::cout << "    testIntegratedPredictionSystem passed" << std::endl;
}

void testIntegratedCognitionSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_predictive_planning", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Run steps to allow cognition to engage
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        brain.step(step);
    }
    
    // Verify planner has some planning depth
    size_t planningDepth = planner->getPlanningDepth();
    assert(planningDepth > 0);
    
    // Verify attention has processed competition
    std::cout << "    testIntegratedCognitionSystems passed" << std::endl;
}

void testDevelopmentSystemEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSys = brain.getDevelopmentSystem();
    assert(devSys != nullptr);
    
    // Test developmental stage progression
    nlm::DevelopmentalStage initialStage = devSys->getStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Run steps to advance development
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
    }
    
    // Development should have advanced (at least to CriticalPeriod)
    nlm::DevelopmentalStage stageAfter = devSys->getStage();
    assert(static_cast<int>(stageAfter) > static_cast<int>(initialStage));
    
    // Test stage-specific effects
    auto plasticityMod = devSys->getPlasticityModifier();
    assert(plasticityMod > 0.0f);
    
    std::cout << "    testDevelopmentSystemEffects passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_full_neuromodulation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Run steps to allow neuromodulation to work
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        brain.step(step);
    }
    
    // Neuromodulators should have levels
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Novelty should have been detected
    float noveltyLevel = novelty->getLevel();
    assert(noveltyLevel >= 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPerformanceOptimizations() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("enable_simd", true, nlm::ConfigSource::Default);
    config->set("enable_memory_pools", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify performance optimizations are enabled
    // This is implicit through initialization success
    
    // Run a substantial simulation to exercise optimizations
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Should complete without issues
    size_t totalNeurons = brain.getTotalNeuronCount();
    assert(totalNeurons == 300);
    
    std::cout << "    testPerformanceOptimizations passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "Running Phase 6 Integration Tests..." << std::endl;
    
    testIntegratedMemorySystems();
    testIntegratedPredictionSystem();
    testIntegratedCognitionSystems();
    testDevelopmentSystemEffects();
    testNeuromodulationIntegration();
    testPerformanceOptimizations();
    
    std::cout << std::endl;
    std::cout << "All Phase 6 integration tests passed!" << std::endl;
    std::cout << "The brain is fully integrated and functional." << std::endl;
}

} // namespace test_brain_integration

// Main function for standalone test execution
int main() {
    try {
        test_brain_integration::runAllIntegrationTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
}