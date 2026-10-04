// Neuromodulation Tests
// Tests Dopamine, Curiosity, Novelty, and PredictionError systems

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

void testNeuromodulatorBase() {
    // Test base Neuromodulator
    nlm::Dopamine dopamine;
    
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f);
    
    std::cout << "    testNeuromodulatorBase passed" << std::endl;
}

void testDopamine() {
    nlm::Dopamine dopamine;
    
    // Test level
    assert(dopamine.getLevel() == 0.0f);
    
    // In a real test, we would modify level through simulation
    // For now, just test that it exists and has basic functionality
    
    std::cout << "    testDopamine passed" << std::endl;
}

void testCuriosity() {
    nlm::Curiosity curiosity;
    
    // Test level
    assert(curiosity.getLevel() == 0.0f);
    
    // Test activity
    curiosity.update(0.001f);
    
    std::cout << "    testCuriosity passed" << std::endl;
}

void testNovelty() {
    nlm::Novelty novelty;
    
    // Test novelty level
    assert(novelty.getNoveltyLevel() == 0.0f);
    
    // Test activity
    novelty.update(0.001f);
    
    // Test is active
    assert(novelty.isActive() == true);  // Always active for this test
    
    std::cout << "    testNovelty passed" << std::endl;
}

void testPredictionError() {
    nlm::PredictionError predError;
    
    // Test prediction error level
    assert(predError.getLevel() == 0.0f);
    
    // Test update
    predError.update(0.001f);
    
    std::cout << "    testPredictionError passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    bool success = brain->initialize();
    assert(success);
    
    // Verify neuromodulation systems are present
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getPredictionErrorSignal() != nullptr);
    
    // Run some simulation steps
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Inject sensory input to stimulate neuromodulation
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
    }
    
    // Check neuromodulation levels
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        // Should be a valid level
        assert(dopamineLevel >= 0.0f);
    }
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation tests..." << std::endl;
    testNeuromodulatorBase();
    testDopamine();
    testCuriosity();
    testNovelty();
    testPredictionError();
    testNeuromodulationIntegration();
}

} // namespace test_neuromodulation
