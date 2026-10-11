// Neuromodulation Tests
// Phase 2 implementation tests for NLM neuromodulation systems

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_neuromodulation {

void testNeuromodulatorBase() {
    std::cout << "    Testing Neuromodulator base class..." << std::endl;
    
    // Create a simple concrete neuromodulator for testing
    class TestNeuromodulator : public nlm::Neuromodulator {
    public:
        TestNeuromodulator() : level_(0.0f) {}
        
        const char* getName() const override { return "Test"; }
        float getLevel() const override { return level_; }
        void setLevel(float level) override { level_ = level; }
        float getPlasticityFactor() const override { return 1.5f; }
        void update(nlm::TimestepDuration dt) override { level_ += dt * 0.1f; }
        
    private:
        float level_;
    };
    
    TestNeuromodulator nm;
    
    assert(std::string(nm.getName()) == "Test");
    assert(nm.getLevel() == 0.0f);
    assert(nm.getPlasticityFactor() == 1.5f);
    
    nm.setLevel(0.5f);
    assert(nm.getLevel() == 0.5f);
    
    nm.update(1.0f);
    assert(nm.getLevel() == 0.6f); // 0.5 + 1.0 * 0.1
    
    std::cout << "      Neuromodulator base class tests passed" << std::endl;
}

void testDopamine() {
    std::cout << "    Testing Dopamine neuromodulator..." << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test base neuromodulator interface
    assert(std::string(dopamine.getName()) == "Dopamine");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f); // Default implementation
    
    // Test reward signaling
    dopamine.signalReward(1.0f);
    dopamine.signalRewardPredictionError(0.2f);
    
    // Check level increased due to reward
    assert(dopamine.getLevel() > 0.0f);
    
    // Test setting level
    dopamine.setLevel(0.8f);
    assert(dopamine.getLevel() == 0.8f);
    
    // Test update
    dopamine.update(1.0f);
    assert(dopamine.getLevel() >= 0.8f);
    
    // Test plasticity factor (implementation dependent)
    float factor = dopamine.getPlasticityFactor();
    assert(factor >= 0.0f);
    
    std::cout << "      Dopamine tests passed" << std::endl;
}

void testNovelty() {
    std::cout << "    Testing Novelty detection..." << std::endl;
    
    nlm::Novelty novelty;
    
    // Test base neuromodulator interface
    assert(std::string(novelty.getName()) == "Novelty");
    assert(novelty.getLevel() == 0.0f);
    
    // Test novelty detection with different inputs
    // Should increase novelty level for novel stimuli
    novelty.update(0.1f, true);   // Novel stimulus
    novelty.update(0.1f, false); // Familiar stimulus
    
    // Novelty level should be higher due to novel stimulus
    assert(novelty.getLevel() > 0.0f);
    
    // Test level setting
    novelty.setLevel(0.3f);
    assert(novelty.getLevel() == 0.3f);
    
    // Test update with novelty detection
    novelty.update(0.2f, true); // Another novel stimulus
    assert(novelty.getLevel() > 0.3f);
    
    // Test plasticity factor
    float factor = novelty.getPlasticityFactor();
    assert(factor >= 0.0f);
    
    std::cout << "      Novelty tests passed" << std::endl;
}

void testCuriosity() {
    std::cout << "    Testing Curiosity..." << std::endl;
    
    nlm::Curiosity curiosity;
    
    // Test base neuromodulator interface
    assert(std::string(curiosity.getName()) == "Curiosity");
    assert(curiosity.getLevel() == 0.0f);
    
    // Test curiosity level based on unexpected outcomes
    curiosity.update(true, 2.0f);  // High curiosity for unexpected outcome
    curiosity.update(false, 1.0f); // Low curiosity for expected outcome
    
    // Curiosity level should reflect the experience
    assert(curiosity.getLevel() >= 0.0f);
    
    // Test level setting
    curiosity.setLevel(0.5f);
    assert(curiosity.getLevel() == 0.5f);
    
    // Test update with curiosity calculation
    curiosity.update(true, 5.0f); // Very high curiosity
    assert(curiosity.getLevel() > 0.5f);
    
    // Test plasticity factor
    float factor = curiosity.getPlasticityFactor();
    assert(factor >= 0.0f);
    
    std::cout << "      Curiosity tests passed" << std::endl;
}

void testPredictionError() {
    std::cout << "    Testing Prediction Error..." << std::endl;
    
    nlm::PredictionError pe;
    
    // Test base neuromodulator interface
    assert(std::string(pe.getName()) == "PredictionError");
    assert(pe.getLevel() == 0.0f);
    
    // Test prediction error signaling
    pe.signalPredictionError(0.1f);  // Positive error
    assert(pe.getLevel() > 0.0f);
    
    pe.signalPredictionError(-0.2f); // Negative error
    // Level should adjust based on error
    
    // Test level setting
    pe.setLevel(0.0f);
    assert(pe.getLevel() == 0.0f);
    
    // Test update with prediction error
    pe.signalPredictionError(0.5f);
    pe.update(1.0f);
    assert(pe.getLevel() > 0.0f);
    
    // Test plasticity factor
    float factor = pe.getPlasticityFactor();
    assert(factor >= 0.0f);
    
    std::cout << "      Prediction Error tests passed" << std::endl;
}

void testNeuromodulatorIntegration() {
    std::cout << "    Testing neuromodulator integration..." << std::endl;
    
    // Create multiple neuromodulators
    nlm::Dopamine dopamine;
    nlm::Novelty novelty;
    nlm::Curiosity curiosity;
    
    // Set different levels
    dopamine.setLevel(0.8f);
    novelty.setLevel(0.3f);
    curiosity.setLevel(0.5f);
    
    // Check individual levels
    assert(dopamine.getLevel() == 0.8f);
    assert(novelty.getLevel() == 0.3f);
    assert(curiosity.getLevel() == 0.5f);
    
    // Test updates
    dopamine.update(1.0f);
    novelty.update(0.5f, true);
    curiosity.update(true, 2.0f);
    
    // All should have been updated
    assert(dopamine.getLevel() > 0.8f);
    assert(novelty.getLevel() > 0.3f);
    assert(curiosity.getLevel() > 0.5f);
    
    // Check plasticity factors
    float dopamineFactor = dopamine.getPlasticityFactor();
    float noveltyFactor = novelty.getPlasticityFactor();
    float curiosityFactor = curiosity.getPlasticityFactor();
    
    assert(dopamineFactor >= 0.0f);
    assert(noveltyFactor >= 0.0f);
    assert(curiosityFactor >= 0.0f);
    
    std::cout << "      Neuromodulator integration tests passed" << std::endl;
}

void runAll() {
    testNeuromodulatorBase();
    testDopamine();
    testNovelty();
    testCuriosity();
    testPredictionError();
    testNeuromodulatorIntegration();
}

} // namespace test_neuromodulation
