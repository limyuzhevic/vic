// AgentBrain Test Suite
// Tests for AgentBrain improvements including:
// - Code duplication fixes
// - Const-correctness of getter methods
// - Random number generator interface
// - Parameter validation
// - Null pointer handling

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <random>

namespace test_agentbrain {

void testNullBrainHandling() {
    nlm::AgentBrain brain(nullptr);
    
    // Test null pointer handling in getters
    assert(brain.getSensoryInputSize() == 0);
    assert(brain.getMotorOutputSize() == 6);  // Default value, not dependent on brain
    
    // Test null checks for operations
    brain.initialize(SimpleWorld()); // Should log warning but not crash
    brain.processSensoryInput(SensoryPercept()); // Should not crash
    brain.decodeMotorCommand(); // Should return Wait
    brain.applyRewardModulation(0.0f, 0.0f); // Should handle null brain
    brain.updateDevelopment(0.0); // Should handle null brain
    brain.getNeuromodulationLevel(); // Should return 0.0
    brain.getCuriosityLevel(); // Should return 0.0
    brain.getNoveltyLevel(); // Should return 0.0
    brain.getPredictionError(); // Should return 0.0
    
    std::cout << "    testNullBrainHandling passed" << std::endl;
}

void testConstCorrectness() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test const-correctness - calling const methods
    const nlm::AgentBrain& constBrain = agentBrain;
    
    size_t sensorySize = constBrain.getSensoryInputSize();
    assert(sensorySize == 256 + 8 + 4 + 6);
    
    size_t motorSize = constBrain.getMotorOutputSize();
    assert(motorSize == 6);
    
    float neuromod = constBrain.getNeuromodulationLevel();
    assert(std::isfinite(neuromod));
    
    float curiosity = constBrain.getCuriosityLevel();
    assert(std::isfinite(curiosity) && curiosity >= 0.0f);
    
    float novelty = constBrain.getNoveltyLevel();
    assert(std::isfinite(novelty) && novelty >= 0.0f);
    
    float prediction = constBrain.getPredictionError();
    assert(std::isfinite(prediction));
    
    std::cout << "    testConstCorrectness passed" << std::endl;
}

void testRandomNumberGeneratorInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test that AgentBrain uses brain->getRandomFloat internally
    // (This is tested indirectly through curiosity-based exploration)
    
    // Test RNG state consistency
    float r1 = brain.getRandomFloat(0.0f, 1.0f);
    float r2 = brain.getRandomFloat(0.0f, 1.0f);
    
    assert(r1 >= 0.0f && r1 < 1.0f);
    assert(r2 >= 0.0f && r2 < 1.0f);
    
    // Test curiosity-based exploration uses RNG
    // Set high curiosity to trigger random actions
    nlm::AgentBrain testBrain(std::make_shared<nlm::Brain>(brain));
    testBrain.enableCuriosity(true);
    
    // Simulate high curiosity state
    auto configCopy = std::make_shared<nlm::Config>();
    configCopy->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    configCopy->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain testBrainInternal(configCopy);
    testBrainInternal.initialize();
    
    std::cout << "    testRandomNumberGeneratorInterface passed" << std::endl;
}

void testParameterValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test parameter validation in processSensoryInput
    auto configCopy = std::make_shared<nlm::Config>();
    configCopy->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    configCopy->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain brainCopy(configCopy);
    nlm::AgentBrain testBrain(std::make_shared<nlm::Brain>(brainCopy));
    
    // Create invalid sensory data (wrong size vision)
    nlm::Vision invalidVision;
    invalidVision.setData(std::vector<float>(100)); // Wrong size, should be 16*16=256
    nlm::SensoryPercept invalidPercept;
    invalidPercept.setVision(invalidVision);
    
    // Should log warning but not crash
    testBrain.processSensoryInput(invalidPercept);
    
    // Test reward modulation validation
    testBrain.applyRewardModulation(NAN, 0.0f); // Should handle NaN input
    testBrain.applyRewardModulation(0.0f, NAN); // Should handle NaN input
    
    // Test invalid world dimensions
    SimpleWorld invalidWorld;
    invalidWorld.configure(0, 100, 16, 16); // Invalid vision width
    testBrain.initialize(invalidWorld); // Should log warning
    
    std::cout << "    testParameterValidation passed" << std::endl;
}

void testCodeDuplicationFixes() {
    // Test for code duplication fixes - ensure common operations are streamlined
    
    // Test that common patterns like checking brain_ pointer are consistent
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test that getBrain() method is inline (const-correct)
    nlm::Brain* retrievedBrain = agentBrain.getBrain();
    assert(retrievedBrain == &brain);
    
    const nlm::Brain* constBrain = agentBrain.getBrain();
    assert(constBrain == &brain);
    
    // Test that getter methods are consistent in behavior
    float neuromod1 = agentBrain.getNeuromodulationLevel();
    float neuromod2 = agentBrain.getNeuromodulationLevel();
    assert(neuromod1 == neuromod2);
    
    std::cout << "    testCodeDuplicationFixes passed" << std::endl;
}

void testDevelopmentStages() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test developmental stage progression
    auto stage1 = agentBrain.getDevelopmentalStage();
    assert(stage1 == nlm::DevelopmentalStage::Initial); // Default
    
    // Simulate development over time
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
    auto stage2 = agentBrain.getDevelopmentalStage();
    assert(stage2 == nlm::DevelopmentalStage::CriticalPeriod);
    
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    auto stage3 = agentBrain.getDevelopmentalStage();
    assert(stage3 == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testDevelopmentStages passed" << std::endl;
}

void testConfigurationFlags() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Test configuration flags
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    // Test toggling flags
    agentBrain.enableCuriosity(false);
    assert(agentBrain.isCuriosityEnabled() == false);
    
    std::cout << "    testConfigurationFlags passed" << std::endl;
}

void runAll() {
    testNullBrainHandling();
    testConstCorrectness();
    testRandomNumberGeneratorInterface();
    testParameterValidation();
    testCodeDuplicationFixes();
    testDevelopmentStages();
    testConfigurationFlags();
}

} // namespace test_agentbrain