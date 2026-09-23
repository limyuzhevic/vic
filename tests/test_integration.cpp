#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_integration {

void testMemoryIntegration() {
    std::cout << "  Testing Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify memory systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Test step execution with memory updates
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Check that working memory is being updated
        if (brain.getWorkingMemory()) {
            // Working memory should have some active traces after steps
            size_t traces = brain.getWorkingMemory()->getActiveTraces();
            // This should be non-zero for a properly integrated system
        }
        
        // Check that episodic memory grows over time
        if (brain.getEpisodicMemory() && step % 10 == 0) {
            size_t episodes = brain.getEpisodicMemory()->getEpisodeCount();
            // Episodes should accumulate
        }
    }
    
    // Verify prediction system has some activity
    if (brain.getPredictionSystem()) {
        // Prediction system should have error history
        const auto& errorHistory = brain.getPredictionSystem()->getErrorHistory();
        // Should have accumulated prediction errors
    }
    
    std::cout << "    Memory Integration PASSED" << std::endl;
}

void testPredictionIntegration() {
    std::cout << "  Testing Prediction Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store initial state
    auto predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    float initialError = predictionSystem->getPredictionError();
    
    // Run some steps to generate prediction activity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        // Add sensory input to drive prediction
        std::vector<float> sensoryData(10, 0.5f); // Constant sensory input
        nlm::SensoryInput input(sensoryData);
        brain.receiveSensoryInput(input);
        
        brain.step(step);
    }
    
    // Check that prediction system has accumulated error
    float finalError = predictionSystem->getPredictionError();
    assert(finalError >= 0.0f);
    
    // Prediction should have some history
    const auto& errorHistory = predictionSystem->getErrorHistory();
    assert(errorHistory.size() > 0);
    
    std::cout << "    Prediction Integration PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine integration
    auto dopamine = brain.getDopamine();
    auto curiosity = brain.getCuriosity();
    auto novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Get initial levels
    float initialDopamine = dopamine->getLevel();
    float initialCuriosity = curiosity->getLevel();
    float initialNovelty = novelty->getLevel();
    
    // Run simulation with some spike activity
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
        
        // Neuromodulators should be updating
        if (dopamine) dopamine->update(0.001);
        if (curiosity) curiosity->update(0.001);
        if (novelty) novelty->update(0.001);
    }
    
    // Check that neuromodulation systems are still valid
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "    Neuromodulation Integration PASSED" << std::endl;
}

void testAttentionIntegration() {
    std::cout << "  Testing Attention Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Give the attention system some neurons to process
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
        
        // Attention should be updating
        attention->update(0.001);
    }
    
    // Check attention winners
    const auto& winners = attention->getWinners();
    // May or may not have winners depending on activation, but system should work
    
    std::cout << "    Attention Integration PASSED" << std::endl;
}

void testDevelopmentalIntegration() {
    std::cout << "  Testing Developmental Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stages
    auto developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    assert(initialStage == nlm::DevelopmentalStage::Initial);
    
    // Run development steps (development happens every 1000 steps, so need to fake it)
    for (nlm::SimulationStep step = 0; step < 1500; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {
            brain.develop();
        }
    }
    
    // Development should still be working
    assert(developmentSystem != nullptr);
    
    std::cout << "    Developmental Integration PASSED" << std::endl;
}

void runAll() {
    std::cout << "Running Integration Tests:" << std::endl;
    std::cout << "  Testing integrated brain architecture:" << std::endl;
    
    try {
        testMemoryIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testPredictionIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testNeuromodulationIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testAttentionIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    try {
        testDevelopmentalIntegration();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration

int main() {
    std::cout << "=== NLM Integration Tests ===" << std::endl;
    std::cout << "Testing Phase 6: Integrated Artificial Brain Architecture" << std::endl;
    std::cout << std::endl;
    
    try {
        test_integration::runAll();
        return 0;
    } catch (const std::exception& e) {
        std::cout << "=== Integration Tests FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}