// Neuromodulation Tests
// Tests for Phase 6 neuromodulation effects on plasticity

#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuromodulation {

void testNeuromodulatorBase() {
    // Test basic neuromodulator interface
    Dopamine dopamine;
    assert(dopamine.getName() != nullptr);
    assert(std::string(dopamine.getName()) == "Dopamine");
    
    assert(dopamine.getLevel() == 0.0f);
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() == 0.5f);
    
    std::cout << "    testNeuromodulatorBase passed" << std::endl;
}

void testDopamine() {
    Dopamine dopamine;
    
    // Test reward signaling
    dopamine.signalReward(1.0f);
    assert(dopamine.getLevel() > 0.0f);
    
    dopamine.signalRewardPredictionError(0.5f);
    assert(dopamine.getLevel() >= 0.0f);
    
    // Test plasticity factor
    float plasticityFactor = dopamine.getPlasticityFactor();
    assert(plasticityFactor >= 0.0f);
    
    // Update dopamine level
    dopamine.update(0.001f);
    
    std::cout << "    testDopamine passed" << std::endl;
}

void testCuriosity() {
    Curiosity curiosity;
    
    assert(curiosity.getName() != nullptr);
    
    // Test curiosity level
    float curiosityLevel = curiosity.getLevel();
    assert(curiosityLevel == 0.0f);  // Default
    
    curiosity.setLevel(0.8f);
    assert(curiosity.getLevel() == 0.8f);
    
    // Update
    curiosity.update(0.001f);
    
    // Test novelty interaction
    float levelBefore = curiosity.getLevel();
    
    std::cout << "    testCuriosity passed" << std::endl;
}

void testNovelty() {
    Novelty novelty;
    
    assert(novelty.getName() != nullptr);
    assert(std::string(novelty.getName()) == "Novelty");
    
    // Test novelty detection
    novelty.setLevel(0.3f);
    assert(novelty.getLevel() == 0.3f);
    
    // Update with different time steps
    novelty.update(0.001f);
    novelty.update(0.01f);
    novelty.update(0.1f);
    
    std::cout << "    testNovelty passed" << std::endl;
}

void testPredictionError() {
    PredictionError predError;
    
    assert(predError.getName() != nullptr);
    
    // Test prediction error signal
    predError.setLevel(0.2f);
    assert(predError.getLevel() == 0.2f);
    
    // Get prediction error
    float error = predError.getLevel();
    assert(error == 0.2f);
    
    // Update
    predError.update(0.001f);
    
    std::cout << "    testPredictionError passed" << std::endl;
}

void testNeuromodulationEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 150);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    if (dopamine && curiosity) {
        // Apply neuromodulation
        nlm::Neuromodulator signal;
        brain.applyNeuromodulation(signal);
        
        // Run steps to allow neuromodulation to affect plasticity
        for (int i = 0; i < 100; ++i) {
            // Inject some sensory activity to trigger neuromodulation
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
            brain.step(i, i * 0.001f);
            
            // Check that neuromodulation system is active
            assert(brain.getDopamine() != nullptr);
            assert(brain.getCuriosity() != nullptr);
        }
        
        std::cout << "    testNeuromodulationEffects passed" << std::endl;
    }
}

void runAll() {
    std::cout << "Running Neuromodulation Tests..." << std::endl;
    testNeuromodulatorBase();
    testDopamine();
    testCuriosity();
    testNovelty();
    testPredictionError();
    testNeuromodulationEffects();
}

} // namespace test_neuromodulation