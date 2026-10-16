// BrainNeuromodulationManager Comprehensive Tests
// Tests Dopamine, Curiosity, Novelty, PredictionError

#include "brain/BrainNeuromodulationManager.hpp"
#include "brain/BrainNeuralCore.hpp"
#include "brain/BrainPlasticityManager.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "environment/Observation.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_brain_neuromodulation_manager {

void testBrainNeuromodulationManagerCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    
    assert(neuromod.initialize(&neuralCore));
    
    std::cout << "    testBrainNeuromodulationManagerCreation passed" << std::endl;
}

void testDopamineIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    nlm::BrainPlasticityManager plasticity(config);
    plasticity.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Get Dopamine component
    nlm::Dopamine* dopamine = neuromod.getDopamine();
    assert(dopamine != nullptr);
    
    // Test dopamine signaling
    neuromod.signalReward(0.8f); // Positive reward
    neuromod.signalReward(-0.5f); // Negative reward
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void testCuriosityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Get Curiosity component
    nlm::Curiosity* curiosity = neuromod.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity computation
    neuromod.computeCuriosity();
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testNoveltyIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Get Novelty component
    nlm::Novelty* novelty = neuromod.getNovelty();
    assert(novelty != nullptr);
    
    // Create observation (simplified)
    nlm::Observation observation;
    observation.sensorId = 1;
    observation.values = {0.5f, 0.7f, 0.9f};
    observation.temporalPattern = {0.0, 1.0, 2.0};
    
    // Signal novelty
    neuromod.signalNovelty(observation);
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void testPredictionErrorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Get PredictionError component
    nlm::PredictionError* predictionError = neuromod.getPredictionError();
    assert(predictionError != nullptr);
    
    // Test prediction error signaling
    neuromod.signalPredictionError(0.3f); // Small positive error
    neuromod.signalPredictionError(-0.7f); // Large negative error
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testNeuromodulationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    nlm::BrainPlasticityManager plasticity(config);
    plasticity.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Apply neuromodulation effects to neural core
    nlm::Neuromodulator dopamineSignal;
    dopamineSignal.setType(nlm::ModulatorType::Dopamine);
    dopamineSignal.setValue(1.0f);
    neuromod.applyEffectsToNeuralCore(&neuralCore);
    
    // Apply neuromodulation effects to plasticity
    neuromod.applyEffectsToPlasticity(&plasticity);
    
    std::cout << "    testNeuromodulationEffects passed" << std::endl;
}

void testNeuromodulationUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Create observation (simplified)
    nlm::Observation observation;
    observation.sensorId = 1;
    observation.values = {0.5f, 0.7f, 0.9f};
    
    // Update neuromodulation (with observation)
    neuromod.update(0.1f, &observation);
    
    // Update without observation
    neuromod.update(0.2f);
    
    std::cout << "    testNeuromodulationUpdate passed" << std::endl;
}

void testNeuromodulationReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Apply some neuromodulation
    neuromod.signalReward(0.8f);
    neuromod.signalPredictionError(0.3f);
    
    // Reset neuromodulation
    neuromod.reset();
    
    std::cout << "    testNeuromodulationReset passed" << std::endl;
}

void testNeuromodulationStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::BrainNeuromodulationManager neuromod(config);
    nlm::BrainNeuralCore neuralCore(config);
    neuralCore.initialize();
    assert(neuromod.initialize(&neuralCore));
    
    // Test configuration access
    std::shared_ptr<const nlm::Config> cfg = neuromod.getConfig();
    assert(cfg != nullptr);
    
    // Test status logging
    neuromod.logStatus();
    
    std::cout << "    testNeuromodulationStatus passed" << std::endl;
}

void runAll() {
    std::cout << "Running BrainNeuromodulationManager tests..." << std::endl;
    
    testBrainNeuromodulationManagerCreation();
    testDopamineIntegration();
    testCuriosityIntegration();
    testNoveltyIntegration();
    testPredictionErrorIntegration();
    testNeuromodulationEffects();
    testNeuromodulationUpdate();
    testNeuromodulationReset();
    testNeuromodulationStatus();
    
    std::cout << "\nAll BrainNeuromodulationManager tests passed!" << std::endl;
}

} // namespace test_brain_neuromodulation_manager
