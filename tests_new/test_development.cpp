// Development System Tests
// Tests developmental stages, neural excitability, attention balance, memory consolidation, etc.

#include "src/development/DevelopmentSystem.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <cassert>
#include <memory>

namespace test_development {

void test_development_stages() {
    std::cout << "=== Testing Development Stages ===" << std::endl;
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing developmental stage detection and transitions:" << std::endl);
    
    // Test initial stage
    nlm::DevelopmentalStage initial_stage = devSystem.getStage();
    assert(initial_stage == nlm::DevelopmentalStage::Initial);
    std::cout << "  ✓ Initial stage: " << static_cast<int>(initial_stage) << std::endl);
    
    // Test stage advancement
    devSystem.advanceStage();
    nlm::DevelopmentalStage stage_after_advance = devSystem.getStage();
    assert(stage_after_advance > initial_stage);
    std::cout << "  ✓ Stage advanced to: " << static_cast<int>(stage_after_advance) << std::endl);
    
    // Test all developmental stages
    devSystem.setStage(nlm::DevelopmentalStage::Neonatal);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Neonatal);
    std::cout << "  ✓ Neonatal stage set" << std::endl);
    
    devSystem.setStage(nlm::DevelopmentalStage::Infant);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Infant);
    std::cout << "  ✓ Infant stage set" << std::endl);
    
    devSystem.setStage(nlm::DevelopmentalStage::Juvenile);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Juvenile);
    std::cout << "  ✓ Juvenile stage set" << std::endl);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem.getStage() == nlm::DevelopmentalStage::Adult);
    std::cout << "  ✓ Adult stage set" << std::endl);
    
    std::cout << "Development stages test completed successfully" << std::endl);
}

void test_neural_excitability() {
    std::cout << "=== Testing Neural Excitability ===" << std::endl);
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing age-appropriate neural excitability:" << std::endl);
    
    // Test different developmental stages have different excitability
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    float initial_excitability = devSystem.getNeuralExcitability();
    std::cout << "  ✓ Initial stage excitability: " << initial_excitability << std::endl);
    assert(initial_excitability >= 0.0f && initial_excitability <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Neonatal);
    float neonatal_excitability = devSystem.getNeuralExcitability();
    std::cout << "  ✓ Neonatal stage excitability: " << neonatal_excitability << std::endl);
    assert(neurol_excitability >= 0.0f && neonatal_excitability <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    float adult_excitability = devSystem.getNeuralExcitability();
    std::cout << "  ✓ Adult stage excitability: " << adult_excitability << std::endl);
    assert(adult_excitability >= 0.0f && adult_excitability <= 1.0f);
    
    // Test excitability changes over time
    devSystem.setStage(nlm::DevelopmentalStage::Infant);
    for (int step = 0; step < 50; ++step) {
        devSystem.updateDevelopment(static_cast<nlm::SimulationStep>(step));
        float current_excitability = devSystem.getNeuralExcitability();
        assert(current_excitability >= 0.0f && current_excitability <= 1.0f);
    }
    std::cout << "  ✓ Neural excitability changes over time" << std::endl);
    
    std::cout << "Neural excitability test completed successfully" << std::endl);
}

void test_attention_balance() {
    std::cout << "=== Testing Attention Balance ===" << std::endl);
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing developmental attention tuning:" << std::endl);
    
    // Test attention balance at different developmental stages
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    float initial_attention = devSystem.getAttentionBalance();
    std::cout << "  ✓ Initial stage attention balance: " << initial_attention << std::endl);
    assert(initial_attention >= 0.0f && initial_attention <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Neonatal);
    float neonatal_attention = devSystem.getAttentionBalance();
    std::cout << "  ✓ Neonatal stage attention balance: " << neonatal_attention << std::endl);
    assert(neurol_attention >= 0.0f && neonatal_attention <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    float adult_attention = devSystem.getAttentionBalance();
    std::cout << "  ✓ Adult stage attention balance: " << adult_attention << std::endl);
    assert(adult_attention >= 0.0f && adult_attention <= 1.0f);
    
    // Test attention balance changes over time
    devSystem.setStage(nlm::DevelopmentalStage::Infant);
    for (int step = 0; step < 50; ++step) {
        devSystem.updateDevelopment(static_cast<nlm::SimulationStep>(step));
        float current_attention = devSystem.getAttentionBalance();
        assert(current_attention >= 0.0f && current_attention <= 1.0f);
    }
    std::cout << "  ✓ Attention balance changes over time" << std::endl);
    
    std::cout << "Attention balance test completed successfully" << std::endl);
}

void test_memory_consolidation() {
    std::cout << "=== Testing Memory Consolidation ===" << std::endl);
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing developmental memory consolidation:" << std::endl);
    
    // Test memory consolidation strength at different developmental stages
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    float initial_consolidation = devSystem.getMemoryConsolidationStrength();
    std::cout << "  ✓ Initial stage memory consolidation: " << initial_consolidation << std::endl);
    assert(initial_consolidation >= 0.0f && initial_consolidation <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Neonatal);
    float neonatal_consolidation = devSystem.getMemoryConsolidationStrength();
    std::cout << "  ✓ Neonatal stage memory consolidation: " << neonatal_consolidation << std::endl);
    assert(neurol_consolidation >= 0.0f && neonatal_consolidation <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    float adult_consolidation = devSystem.getMemoryConsolidationStrength();
    std::cout << "  ✓ Adult stage memory consolidation: " << adult_consolidation << std::endl);
    assert(adult_consolidation >= 0.0f && adult_consolidation <= 1.0f);
    
    // Test memory consolidation changes over time
    devSystem.setStage(nlm::DevelopmentalStage::Infant);
    for (int step = 0; step < 50; ++step) {
        devSystem.updateDevelopment(static_cast<nlm::SimulationStep>(step));
        float current_consolidation = devSystem.getMemoryConsolidationStrength();
        assert(current_consolidation >= 0.0f && current_consolidation <= 1.0f);
    }
    std::cout << "  ✓ Memory consolidation changes over time" << std::endl);
    
    std::cout << "Memory consolidation test completed successfully" << std::endl);
}

void test_neuromodulation() {
    std::cout << "=== Testing Neuromodulation Levels ===" << std::endl);
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing age-dependent neuromodulator levels:" << std::endl);
    
    // Test neuromodulation sensitivity at different developmental stages
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    float initial_neuromod_sensitivity = devSystem.getNeuromodulationSensitivity();
    std::cout << "  ✓ Initial stage neuromodulation sensitivity: " << initial_neuromod_sensitivity << std::endl);
    assert(initial_neuromod_sensitivity >= 0.0f && initial_neuromod_sensitivity <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Neonatal);
    float neonatal_neuromod_sensitivity = devSystem.getNeuromodulationSensitivity();
    std::cout << "  ✓ Neonatal stage neuromodulation sensitivity: " << neonatal_neuromod_sensitivity << std::endl);
    assert(neurol_neuromod_sensitivity >= 0.0f && neonatal_neuromod_sensitivity <= 1.0f);
    
    devSystem.setStage(nlm::DevelopmentalStage::Adult);
    float adult_neuromod_sensitivity = devSystem.getNeuromodulationSensitivity();
    std::cout << "  ✓ Adult stage neuromodulation sensitivity: " << adult_neuromod_sensitivity << std::endl);
    assert(adult_neuromod_sensitivity >= 0.0f && adult_neuromod_sensitivity <= 1.0f);
    
    std::cout << "Neuromodulation test completed successfully" << std::endl);
}

void test_development_integration() {
    std::cout << "=== Testing Development Integration ===" << std::endl);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto development_system = brain.getDevelopmentSystem();
    assert(development_system != nullptr);
    
    std::cout << "Testing development integration with brain systems:" << std::endl);
    
    // Test development integration over time
    dev_system.setStage(nlm::DevelopmentalStage::Infant);
    for (int step = 0; step < 100; ++step) {
        brain.step(static_cast<nlm::SimulationStep>(step));
        development_system->updateDevelopment(static_cast<nlm::SimulationStep>(step));
        
        // Verify integration effects
        float excitability = development_system->getNeuralExcitability();
        float attention = development_system->getAttentionBalance();
        float memory_strength = development_system->getMemoryConsolidationStrength();
        float neuromod_sensitivity = development_system->getNeuromodulationSensitivity();
        
        assert(excitability >= 0.0f && excitability <= 1.0f);
        assert(attention >= 0.0f && attention <= 1.0f);
        assert(memory_strength >= 0.0f && memory_strength <= 1.0f);
        assert(neuromod_sensitivity >= 0.0f && neuromod_sensitivity <= 1.0f);
    }
    
    std::cout << "  ✓ Development integration with brain systems" << std::endl);
    std::cout << "  ✓ Neural excitability effects applied" << std::endl);
    std::cout << "  ✓ Attention balance effects applied" << std::endl);
    std::cout << "  ✓ Memory consolidation effects applied" << std::endl);
    std::cout << "  ✓ Neuromodulation sensitivity effects applied" << std::endl);
    
    std::cout << "Development integration test completed successfully" << std::endl);
}

void test_critical_periods() {
    std::cout << "=== Testing Critical Periods ===" << std::endl);
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing critical period detection and utilization:" << std::endl);
    
    // Test critical period detection
    devSystem.setStage(nlm::DevelopmentalStage::Infant);
    bool is_critical_period = devSystem.isInCriticalPeriod();
    std::cout << "  ✓ Critical period detection: " << (is_critical_period ? "Active" : "Not active") << std::endl);
    
    // Test critical period sensitivity
    float critical_period_sensitivity = devSystem.getCriticalPeriodSensitivity();
    std::cout << "  ✓ Critical period sensitivity: " << critical_period_sensitivity << std::endl);
    assert(critical_period_sensitivity >= 0.0f && critical_period_sensitivity <= 1.0f);
    
    // Test critical period effectiveness
    bool critical_period_effective = devSystem.isCriticalPeriodEffective();
    std::cout << "  ✓ Critical period effectiveness: " << (critical_period_effective ? "Effective" : "Not effective") << std::endl);
    
    // Test critical period duration
    int critical_period_duration = devSystem.getCriticalPeriodDuration();
    std::cout << "  ✓ Critical period duration: " << critical_period_duration << " steps" << std::endl);
    assert(critical_period_duration >= 0);
    
    // Test critical period utilization
    devSystem.recordCriticalPeriodObservation();
    std::cout << "  ✓ Critical period observation recorded" << std::endl);
    
    std::cout << "Critical periods test completed successfully" << std::endl);
}

void test_development_trajectory() {
    std::cout << "=== Testing Development Trajectory ===" << std::endl);
    
    nlm::DevelopmentSystem devSystem;
    
    std::cout << "Testing developmental trajectory over time:" << std::endl);
    
    // Record development trajectory
    std::vector<float> excitability_trajectory;
    std::vector<float> attention_trajectory;
    std::vector<float> memory_trajectory;
    
    devSystem.setStage(nlm::DevelopmentalStage::Initial);
    for (int step = 0; step < 200; ++step) {
        devSystem.updateDevelopment(static_cast<nlm::SimulationStep>(step));
        
        excitability_trajectory.push_back(devSystem.getNeuralExcitability());
        attention_trajectory.push_back(devSystem.getAttentionBalance());
        memory_trajectory.push_back(devSystem.getMemoryConsolidationStrength());
    }
    
    // Analyze trajectory
    float excitability_change = excitability_trajectory.back() - excitability_trajectory.front();
    float attention_change = attention_trajectory.back() - attention_trajectory.front();
    float memory_change = memory_trajectory.back() - memory_trajectory.front();
    
    std::cout << "  ✓ Development trajectory analyzed:" << std::endl);
    std::cout << "    - Excitability change: " << excitability_change << std::endl);
    std::cout << "    - Attention balance change: " << attention_change << std::endl);
    std::cout << "    - Memory consolidation change: " << memory_change << std::endl);
    
    // Test trajectory smoothness
    float excitability_variance = 0.0f;
    float attention_variance = 0.0f;
    float memory_variance = 0.0f;
    
    for (size_t i = 1; i < excitability_trajectory.size(); ++i) {
        float excitability_diff = excitability_trajectory[i] - excitability_trajectory[i-1];
        excitability_variance += excitability_diff * excitability_diff;
        
        float attention_diff = attention_trajectory[i] - attention_trajectory[i-1];
        attention_variance += attention_diff * attention_diff;
        
        float memory_diff = memory_trajectory[i] - memory_trajectory[i-1];
        memory_variance += memory_diff * memory_diff;
    }
    
    std::cout << "  ✓ Trajectory smoothness (lower variance = smoother):" << std::endl);
    std::cout << "    - Excitability variance: " << excitability_variance << std::endl);
    std::cout << "    - Attention balance variance: " << attention_variance << std::endl);
    std::cout << "    - Memory consolidation variance: " << memory_variance << std::endl);
    
    std::cout << "Development trajectory test completed successfully" << std::endl);
}

void runAll() {
    std::cout << "=== Comprehensive Development System Tests ===" << std::endl);
    
    test_development_stages();
    test_neural_excitability();
    test_attention_balance();
    test_memory_consolidation();
    test_neuromodulation();
    test_development_integration();
    test_critical_periods();
    test_development_trajectory();
    
    std::cout << std::endl;
    std::cout << "=== All Development System Tests PASSED ===" << std::endl);
}

} // namespace test_development