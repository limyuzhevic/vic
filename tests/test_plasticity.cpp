// Plasticity System Tests
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_plasticity {

void testPlasticityRule() {
    // Test base class cannot be instantiated (abstract)
    // This test would be void for PlasticityRule as it's abstract
    // but we can test the enabled flag functionality
    
    // Create a concrete implementation to test base class methods
    class TestPlasticityRule : public nlm::PlasticityRule {
    public:
        void update(nlm::Synapse*, const std::vector<nlm::Timestamp>&,
                   const std::vector<nlm::Timestamp>&, nlm::TimestepDuration) override {}
        void applyWeightChange(nlm::Synapse*, nlm::SynapticWeight) override {}
        const char* getName() const override { return "TestRule"; }
    };
    
    TestPlasticityRule rule;
    
    // Test initial enabled state
    assert(rule.isEnabled() == true);
    
    // Test disabling
    rule.setEnabled(false);
    assert(rule.isEnabled() == false);
    
    // Test enabling
    rule.setEnabled(true);
    assert(rule.isEnabled() == true);
    
    std::cout << "    testPlasticityRule passed" << std::endl;
}

void testHebbian() {
    nlm::Hebbian hebbian;
    
    // Test initialization
    assert(hebbian.getName() == std::string("Hebbian"));
    
    // Test learning rate defaults
    float defaultRate = hebbian.getLearningRate();
    assert(defaultRate == 0.01f); // Default should be 0.01
    
    // Test learning rate setting
    hebbian.setLearningRate(0.1f);
    assert(hebbian.getLearningRate() == 0.1f);
    
    // Test max weight defaults
    float defaultMaxWeight = hebbian.getMaxWeight();
    assert(defaultMaxWeight == 1.0f); // Default should be 1.0
    
    // Test max weight setting
    hebbian.setMaxWeight(2.0f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    // Test mathematical formulation
    std::string formulation = hebbian.getMathematicalFormulation();
    assert(!formulation.empty());
    assert(formulation.find("Hebbian") != std::string::npos);
    
    // Test movement constructor
    nlm::Hebbian moved(std::move(hebbian));
    // After move, moved should have default values
    assert(moved.getLearningRate() == 0.01f);
    
    // Test move assignment
    nlm::Hebbian assigned = std::move(moved);
    assert(assigned.getLearningRate() == 0.01f);
    
    std::cout << "    testHebbian passed" << std::endl;
}

void testSTDP() {
    nlm::STDP stdp;
    
    // Test initialization
    assert(stdp.getName() == std::string("STDP"));
    
    // Test learning rate defaults
    float defaultRate = stdp.getLearningRate();
    assert(defaultRate == 0.01f); // Default should be 0.01
    
    // Test learning rate setting
    stdp.setLearningRate(0.05f);
    assert(stdp.getLearningRate() == 0.05f);
    
    // Test time window defaults
    float defaultTimeWindow = stdp.getTimeWindow();
    assert(defaultTimeWindow > 0.0f);
    
    // Test time window setting
    stdp.setTimeWindow(20.0f);
    assert(stdp.getTimeWindow() == 20.0f);
    
    // Test asymmetric weights
    float defaultLTP = stdp.getLTPWeight();
    float defaultLTD = stdp.getLTDWeight();
    
    assert(defaultLTP != defaultLTD); // Should be different
    assert(defaultLTP > 0.0f);
    assert(defaultLTD < 0.0f);
    
    // Test LTP/LTD setting
    stdp.setLTPWeight(0.1f);
    stdp.setLTDWeight(-0.1f);
    
    assert(stdp.getLTPWeight() == 0.1f);
    assert(stdp.getLTDWeight() == -0.1f);
    
    // Test max weight
    stdp.setMaxWeight(2.0f);
    assert(stdp.getMaxWeight() == 2.0f);
    
    // Test max weight clamping by testing with values
    // (This would require a synapse object to test fully)
    
    // Test mathematical formulation
    std::string formulation = stdp.getMathematicalFormulation();
    assert(!formulation.empty());
    assert(formulation.find("STDP") != std::string::npos);
    
    std::cout << "    testSTDP passed" << std::endl;
}

void testStructuralPlasticity() {
    nlm::StructuralPlasticity structuralPlasticity;
    
    // Test initialization
    assert(structuralPlasticity.getName() == std::string("StructuralPlasticity"));
    
    // Test formation probability
    float formationProb = structuralPlasticity.getFormationProbability();
    assert(formationProb >= 0.0f && formationProb <= 1.0f);
    
    // Test formation probability setting
    structuralPlasticity.setFormationProbability(0.5f);
    assert(structuralPlasticity.getFormationProbability() == 0.5f);
    
    // Test pruning probability
    float pruningProb = structuralPlasticity.getPruningProbability();
    assert(pruningProb >= 0.0f && pruningProb <= 1.0f);
    
    // Test pruning probability setting
    structuralPlasticity.setPruningProbability(0.3f);
    assert(structuralPlasticity.getPruningProbability() == 0.3f);
    
    // Test probability bounds
    structuralPlasticity.setFormationProbability(0.0f);
    structuralPlasticity.setPruningProbability(0.0f);
    
    assert(structuralPlasticity.getFormationProbability() == 0.0f);
    assert(structuralPlasticity.getPruningProbability() == 0.0f);
    
    // Test max connections
    assert(structuralPlasticity.getMaxConnections() > 0);
    
    // Test max connections setting
    structuralPlasticity.setMaxConnections(1000);
    assert(structuralPlasticity.getMaxConnections() == 1000);
    
    // Test connection lifetime
    float lifetime = structuralPlasticity.getConnectionLifetime();
    assert(lifetime > 0.0f);
    
    // Test connection lifetime setting
    structuralPlasticity.setConnectionLifetime(1000.0f);
    assert(structuralPlasticity.getConnectionLifetime() == 1000.0f);
    
    // Test mathematical formulation
    std::string formulation = structuralPlasticity.getMathematicalFormulation();
    assert(!formulation.empty());
    assert(formulation.find("StructuralPlasticity") != std::string::npos);
    
    std::cout << "    testStructuralPlasticity passed" << std::endl;
}

void runAll() {
    testPlasticityRule();
    testHebbian();
    testSTDP();
    testStructuralPlasticity();
}

} // namespace test_plasticity