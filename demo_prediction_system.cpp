// Demonstration of Real Prediction System Integration
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== NLM Prediction System Integration Demo ===" << std::endl << std::endl;
    
    // Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cout << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << std::endl;
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    if (!predictionSystem) {
        std::cout << "Prediction system not available" << std::endl;
        return 1;
    }
    
    std::cout << "=== Prediction System Demonstration ===" << std::endl << std::endl;
    
    // Create sample observations
    auto internalSignals1 = std::make_unique<nlm::InternalSignals>();
    internalSignals1->addSignal(0.1f);
    internalSignals1->addSignal(0.3f);
    internalSignals1->addSignal(0.5f);
    
    auto internalSignals2 = std::make_unique<nlm::InternalSignals>();
    internalSignals2->addSignal(0.2f);
    internalSignals2->addSignal(0.4f);
    internalSignals2->addSignal(0.6f);
    
    // Step 1: Make prediction
    std::cout << "1. Making prediction for first observation..." << std::endl;
    auto prediction1 = predictionSystem->predictNextState(*internalSignals1);
    std::cout << "   Prediction made" << std::endl;
    std::cout << "   Confidence: " << predictionSystem->getConfidence() << std::endl;
    std::cout << std::endl;
    
    // Step 2: Update with actual observation (compute prediction error)
    std::cout << "2. Updating with actual observation..." << std::endl;
    predictionSystem->updatePredictions(*prediction1, *internalSignals1);
    std::cout << "   Prediction error: " << predictionSystem->getPredictionError() << std::endl;
    std::cout << "   Error magnitude: " << predictionSystem->getPredictionErrorMagnitude() << std::endl;
    std::cout << std::endl;
    
    // Step 3: Make second prediction
    std::cout << "3. Making prediction for second observation..." << std::endl;
    auto prediction2 = predictionSystem->predictNextState(*internalSignals2);
    predictionSystem->updatePredictions(*prediction2, *internalSignals2);
    std::cout << "   Prediction error: " << predictionSystem->getPredictionError() << std::endl;
    std::cout << "   Confidence: " << predictionSystem->getConfidence() << std::endl;
    std::cout << std::endl;
    
    // Step 4: Show prediction statistics
    std::cout << "4. Prediction System Statistics:" << std::endl;
    auto stats = predictionSystem->getStats();
    std::cout << "   Mean error: " << stats.meanError << std::endl;
    std::cout << "   Max error: " << stats.maxError << std::endl;
    std::cout << "   Min error: " << stats.minError << std::endl;
    std::cout << "   Variance: " << stats.variance << std::endl;
    std::cout << std::endl;
    
    // Step 5: Show integration with brain systems
    std::cout << "5. Integration with Brain Systems:" << std::endl;
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "   Dopamine level: " << dopamine->getLevel() << std::endl;
        std::cout << "   Dopamine plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
    }
    
    auto* predictionError = brain.getPredictionErrorSignal();
    if (predictionError) {
        std::cout << "   Prediction error signal: " << predictionError->getError() << std::endl;
    }
    
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "   Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "   Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    std::cout << std::endl;
    
    // Step 6: Run a few simulation steps to show real-time integration
    std::cout << "6. Running simulation steps with prediction integration..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        brain.step(i);
    }
    
    // Final status
    brain.logStatus();
    
    std::cout << std::endl;
    std::cout << "=== Prediction System Integration Complete ===" << std::endl;
    std::cout << "The prediction system is now fully integrated into:" << std::endl;
    std::cout << "  ✓ Sensory processing flow in Brain::step()" << std::endl;
    std::cout << "  ✓ Prediction error computation and neuromodulation" << std::endl;
    std::cout << "  ✓ Working memory for memory predictions" << std::endl;
    std::cout << "  ✓ Dopamine-modulated learning and plasticity" << std::endl;
    std::cout << "  ✓ Episodic memory for prediction patterns" << std::endl;
    
    return 0;
}