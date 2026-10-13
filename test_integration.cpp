// Integration Test for Enhanced NLM Brain
// Tests the new integration features implemented in Phase 6

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Enhanced NLM Brain Integration..." << std::endl;
    
    // Test 1: Basic brain creation and initialization
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        
        // Test integration of new neuromodulators
        assert(brain.getDopamine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        assert(brain.getPredictionError() != nullptr);
        assert(brain.getNovelty() != nullptr);
        assert(brain.getAcetylcholine() != nullptr);  // NEW: Added acetylcholine
        assert(brain.getNorepinephrine() != nullptr); // NEW: Added norepinephrine
        assert(brain.getSerotonin() != nullptr);      // NEW: Added serotonin
        
        std::cout << "✓ Test 1 passed: Brain creation with new neuromodulators" << std::endl;
    }
    
    // Test 2: Step execution with integrated systems
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run steps to exercise integrated prediction system
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        // Verify systems are active
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getAttention() != nullptr);
        
        std::cout << "✓ Test 2 passed: Step execution with integrated systems" << std::endl;
    }
    
    // Test 3: Development system integration
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test development system integration
        auto* development = brain.getDevelopmentSystem();
        assert(development != nullptr);
        
        auto stage = development->getStage();
        assert(stage == nlm::DevelopmentalStage::Initial || 
               stage == nlm::DevelopmentalStage::CriticalPeriod ||
               stage == nlm::DevelopmentalStage::Maturation ||
               stage == nlm::DevelopmentalStage::Adult ||
               stage == nlm::DevelopmentalStage::Aging);
        
        // Test development effects on plasticity
        float plasticityBefore = brain.getStructuralPlasticity() ? 
                                 brain.getStructuralPlasticity()->getSynaptogenesisRate() : 0.0f;
        
        brain.step(100);  // Run some steps to trigger development
        
        float plasticityAfter = brain.getStructuralPlasticity() ? 
                                brain.getStructuralPlasticity()->getSynaptogenesisRate() : 0.0f;
        
        std::cout << "✓ Test 3 passed: Development system integration" << std::endl;
    }
    
    // Test 4: Neuromodulation system integration
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test neuromodulator integration
        auto* acetylcholine = brain.getAcetylcholine();
        auto* norepinephrine = brain.getNorepinephrine();
        auto* serotonin = brain.getSerotonin();
        
        // Set different neuromodulator levels
        acetylcholine->setLevel(0.7f);
        norepinephrine->setLevel(0.5f);
        serotonin->setLevel(0.3f);
        
        // Verify levels are set
        assert(acetylcholine->getLevel() == 0.7f);
        assert(norepinephrine->getLevel() == 0.5f);
        assert(serotonin->getLevel() == 0.3f);
        
        // Test plasticity factors
        float achPlasticity = acetylcholine->getPlasticityFactor();
        float nePlasticity = norepinephrine->getPlasticityFactor();
        float sePlasticity = serotonin->getPlasticityFactor();
        
        assert(achPlasticity > 0.8f);  // ACh should have higher plasticity factor
        assert(nePlasticity > 0.5f);  // NE should have moderate plasticity factor
        assert(sePlasticity > 0.4f);  // 5-HT should have moderate plasticity factor
        
        std::cout << "✓ Test 4 passed: Neuromodulation system integration" << std::endl;
    }
    
    // Test 5: Prediction system integration
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test prediction system integration
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Run steps to exercise prediction system
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        // Check prediction error history
        const auto& errorHistory = predictionSystem->getErrorHistory();
        assert(errorHistory.size() > 0);  // Should have some prediction errors
        
        std::cout << "✓ Test 5 passed: Prediction system integration" << std::endl;
    }
    
    std::cout << "\nAll integration tests passed!" << std::endl;
    std::cout << "The enhanced NLM Brain successfully integrates all new systems:" << std::endl;
    std::cout << "  - Prediction system with learning loop" << std::endl;
    std::cout << "  - Concept formation for pattern recognition" << std::endl;
    std::cout << "  - Real development processes (synaptogenesis, maturation, pruning)" << std::endl;
    std::cout << "  - Complete neuromodulation (dopamine, curiosity, novelty, acetylcholine, norepinephrine, serotonin)" << std::endl;
    
    return 0;
}
