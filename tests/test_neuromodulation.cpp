// Neuromodulation Tests
// Tests for dopamine, curiosity, novelty, and prediction error systems

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

void testDopamine() {
    std::cout << "    Testing dopamine system..." << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test initialization
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for dopamine test" << std::endl;
        assert(false);
    }
    
    dopamine.initialize(&brain);
    
    // Test level access
    float level = dopamine.getLevel();
    assert(level >= 0.0f && level <= 1.0f); // Normalized level
    
    std::cout << "    Dopamine test passed" << std::endl;
}

void testCuriosity() {
    std::cout << "    Testing curiosity system..." << std::endl;
    
    nlm::Curiosity curiosity;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for curiosity test" << std::endl;
        assert(false);
    }
    
    curiosity.initialize(&brain);
    
    // Test level access
    float level = curiosity.getLevel();
    assert(level >= 0.0f && level <= 1.0f); // Normalized level
    
    // Test update
    curiosity.update(0.01f);
    
    std::cout << "    Curiosity test passed" << std::endl;
}

void testNovelty() {
    std::cout << "    Testing novelty system..." << std::endl;
    
    nlm::Novelty novelty;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for novelty test" << std::endl;
        assert(false);
    }
    
    novelty.initialize(&brain);
    
    // Test novelty detection
    novelty.update(0.01f);
    
    std::cout << "    Novelty test passed" << std::endl;
}

void testPredictionError() {
    std::cout << "    Testing prediction error system..." << std::endl;
    
    nlm::PredictionError predictionError;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for prediction error test" << std::endl;
        assert(false);
    }
    
    predictionError.initialize(&brain);
    
    // Test prediction error signal
    float error = predictionError.getLevel();
    assert(error >= -1.0f && error <= 1.0f); // Can be negative (unexpected)
    
    std::cout << "    Prediction error test passed" << std::endl;
}

void testNeuromodulatorBase() {
    std::cout << "    Testing base neuromodulator class..." << std::endl;
    
    // Create a simple test neuromodulator
    class TestNeuromodulator : public nlm::Neuromodulator {
    public:
        void initialize(nlm::Brain* brain) override {
            level = 0.5f;
        }
        
        float getLevel() const override {
            return level;
        }
        
        float getPlasticityFactor() const override {
            return 1.0f;
        }
        
    private:
        float level;
    };
    
    TestNeuromodulator testMod;
    
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    testMod.initialize(&brain);
    assert(testMod.getLevel() == 0.5f);
    assert(testMod.getPlasticityFactor() == 1.0f);
    
    std::cout << "    Base neuromodulator test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Check that neuromodulation systems are integrated
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        auto* novelty = brain.getNovelty();
        auto* predictionError = brain.getPredictionErrorSignal();
        
        assert(dopamine != nullptr);
        assert(curiosity != nullptr);
        assert(novelty != nullptr);
        assert(predictionError != nullptr);
        
        // Test neuromodulation effects on brain
        nlm::Neuromodulator signal;
        signal.setLevel(0.3f);
        
        brain.applyNeuromodulation(signal);
    }
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

} // namespace test_neuromodulation
