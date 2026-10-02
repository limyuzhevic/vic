// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
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
    }
    
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
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Initial developmental stage should be Initial
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Update development
    brain.updateDevelopment(10.0);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    brain.updateDevelopment(60.0);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    brain.updateDevelopment(240.0);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    brain.updateDevelopment(660.0);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testBrainDevelopment passed" << std::endl;
}

void testBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction error calculation
    float error1 = brain.applyRewardModulation(1.0f, 0.5f);
    assert(error1 == 0.5f);  // reward - predicted_reward
    
    float error2 = brain.applyRewardModulation(0.5f, 0.8f);
    assert(error2 == -0.3f);  // reward - predicted_reward
    
    // Test expected reward tracking
    float expected1 = brain.getExpectedReward();
    assert(expected1 > 0.0f);
    
    std::cout << "    testBrainRewardModulation passed" << std::endl;
}

void testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    assert(brain.getNeuromodulationLevel() == 0.0f);
    assert(brain.getCuriosityLevel() == 0.0f);
    assert(brain.getNoveltyLevel() == 0.0f);
    assert(brain.getPredictionError() == 0.0f);
    
    // Test neuromodulation after reward
    brain.applyRewardModulation(0.8f, 0.3f);
    assert(brain.getPredictionError() == 0.5f);
    assert(brain.getNeuromodulationLevel() > 0.0f);  // Should be dopamine signal
    
    std::cout << "    testBrainNeuromodulation passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test action production with no input
    auto action1 = brain.produceAction();
    assert(action1 != nullptr);
    
    // Test action production after step
    brain.step(1);
    auto action2 = brain.produceAction();
    assert(action2 != nullptr);
    
    // Actions should have valid properties
    assert(action2->getType() != nlm::ActionType::Unknown);
    assert(action2->getValue() >= 0.0f);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify brain has expected properties after initialization
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test region accessibility
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        assert(region->getRegionId() == rid);
    }
    
    // Test brain status
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainIntegration passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainDevelopment();
    testBrainRewardModulation();
    testBrainNeuromodulation();
    testBrainIntegration();
}

} // namespace test_brain
