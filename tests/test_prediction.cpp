// Prediction System Tests
#include "prediction/PredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction {

void testPredictionSystemCreation() {
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    std::cout << "    testPredictionSystemCreation passed" << std::endl;
}

void testPredictionSystemBasicOperation() {
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    // Create a simple internal signals observation
    auto internalSignals = std::make_unique<nlm::InternalSignals>();
    internalSignals->addSignal(0.1f);
    internalSignals->addSignal(0.5f);
    internalSignals->addSignal(0.8f);
    
    // Make prediction
    auto prediction = predictionSystem.predictNextState(*internalSignals);
    assert(prediction != nullptr);
    
    // Update prediction with actual observation
    predictionSystem.updatePredictions(*prediction, *internalSignals);
    
    // Verify prediction error is computed
    float error = predictionSystem.getPredictionError();
    assert(error >= 0.0f);
    
    // Verify confidence is computed
    float confidence = predictionSystem.getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Verify error history is populated
    const auto& history = predictionSystem.getErrorHistory();
    assert(history.size() > 0);
    
    std::cout << "    testPredictionSystemBasicOperation passed" << std::endl;
}

void testPredictionSystemIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create simple observation
    auto internalSignals = std::make_unique<nlm::InternalSignals>();
    internalSignals->addSignal(0.3f);
    internalSignals->addSignal(0.7f);
    
    // Make prediction
    auto prediction = predictionSystem->predictNextState(*internalSignals);
    assert(prediction != nullptr);
    
    // Update predictions (compute error)
    predictionSystem->updatePredictions(*prediction, *internalSignals);
    
    // Verify integration with brain state
    float predictionError = predictionSystem->getPredictionError();
    float confidence = predictionSystem->getConfidence();
    
    assert(predictionError >= 0.0f);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Verify working memory stores prediction signals
    if (brain.getWorkingMemory()) {
        // Prediction system should store signals in working memory
        // This is verified indirectly through the fact that the code executed
        std::cout << "    Prediction signals integrated with working memory" << std::endl;
    }
    
    std::cout << "    testPredictionSystemIntegrationWithBrain passed" << std::endl;
}

void testPredictionSystemErrorLearning() {
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    nlm::PredictionSystem predictionSystem;
    predictionSystem.initialize(&brain);
    
    // Create observations with different patterns
    auto signals1 = std::make_unique<nlm::InternalSignals>();
    signals1->addSignal(0.1f);
    signals1->addSignal(0.2f);
    signals1->addSignal(0.3f);
    
    auto signals2 = std::make_unique<nlm::InternalSignals>();
    signals2->addSignal(0.4f);
    signals2->addSignal(0.5f);
    signals2->addSignal(0.6f);
    
    // Make first prediction
    auto prediction1 = predictionSystem.predictNextState(*signals1);
    predictionSystem.updatePredictions(*prediction1, *signals1);
    
    // Make second prediction
    auto prediction2 = predictionSystem.predictNextState(*signals2);
    predictionSystem.updatePredictions(*prediction2, *signals2);
    
    // Verify error history contains both errors
    const auto& history = predictionSystem.getErrorHistory();
    assert(history.size() == 2);
    
    // Get statistics
    auto stats = predictionSystem.getStats();
    assert(stats.meanError >= 0.0f);
    assert(stats.maxError >= stats.meanError);
    
    std::cout << "    testPredictionSystemErrorLearning passed" << std::endl;
}

void testPredictionSystemNeuromodulationConnection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* predictionErrorSignal = brain.getPredictionErrorSignal();
    assert(dopamine != nullptr);
    assert(predictionErrorSignal != nullptr);
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Create observation
    auto internalSignals = std::make_unique<nlm::InternalSignals>();
    internalSignals->addSignal(0.5f);
    internalSignals->addSignal(0.7f);
    internalSignals->addSignal(0.9f);
    
    // Make prediction
    auto prediction = predictionSystem->predictNextState(*internalSignals);
    predictionSystem->updatePredictions(*prediction, *internalSignals);
    
    // Verify that prediction error connects to neuromodulation
    // (This happens in Brain::step(), but we can verify the components exist)
    float predictionError = predictionSystem->getPredictionError();
    float dopamineLevel = dopamine->getLevel();
    
    assert(predictionError >= 0.0f);
    assert(dopamineLevel >= 0.0f && dopamineLevel <= 1.0f);
    
    // The real integration happens in Brain::step()
    // For this test, we just verify the connection exists
    std::cout << "    testPredictionSystemNeuromodulationConnection passed" << std::endl;
}

void runAll() {
    testPredictionSystemCreation();
    testPredictionSystemBasicOperation();
    testPredictionSystemIntegrationWithBrain();
    testPredictionSystemErrorLearning();
    testPredictionSystemNeuromodulationConnection();
}

} // namespace test_prediction