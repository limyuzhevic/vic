// PlasticityRule Tests
// Base plasticity rule management and configuration

#include "plasticity/PlasticityRule.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_plasticity_rule {

void testPlasticityRuleCreation() {
    nlm::PlasticityRule rule;
    assert(rule.getLearningRate() == 0.01f);
    assert(rule.getMaxWeightChange() == 1.0f);
    assert(rule.isEnabled() == true);
    std::cout << "    testPlasticityRuleCreation passed" << std::endl;
}

void testPlasticityRuleLearningRate() {
    nlm::PlasticityRule rule;
    
    rule.setLearningRate(0.05f);
    assert(rule.getLearningRate() == 0.05f);
    
    rule.setLearningRate(0.1f);
    assert(rule.getLearningRate() == 0.1f);
    
    std::cout << "    testPlasticityRuleLearningRate passed" << std::endl;
}

void testPlasticityRuleMaxWeightChange() {
    nlm::PlasticityRule rule;
    
    rule.setMaxWeightChange(2.0f);
    assert(rule.getMaxWeightChange() == 2.0f);
    
    rule.setMaxWeightChange(5.0f);
    assert(rule.getMaxWeightChange() == 5.0f);
    
    std::cout << "    testPlasticityRuleMaxWeightChange passed" << std::endl;
}

void testPlasticityRuleEnabled() {
    nlm::PlasticityRule rule;
    
    rule.setEnabled(false);
    assert(rule.isEnabled() == false);
    
    rule.setEnabled(true);
    assert(rule.isEnabled() == true);
    
    std::cout << "    testPlasticityRuleEnabled passed" << std::endl;
}

void testPlasticityRuleType() {
    nlm::PlasticityRule rule;
    assert(rule.getType() == nlm::PlasticityRuleType::Hebbian);
    
    std::cout << "    testPlasticityRuleType passed" << std::endl;
}

void testPlasticityRuleName() {
    nlm::PlasticityRule rule;
    assert(rule.getName() != nullptr);
    assert(std::string(rule.getName()) == "PlasticityRule");
    
    std::cout << "    testPlasticityRuleName passed" << std::endl;
}

void testPlasticityRuleClone() {
    nlm::PlasticityRule original;
    original.setLearningRate(0.05f);
    original.setEnabled(false);
    
    auto cloned = original.clone();
    assert(cloned != nullptr);
    assert(cloned->getLearningRate() == 0.05f);
    assert(cloned->isEnabled() == false);
    assert(cloned->getType() == nlm::PlasticityRuleType::Hebbian);
    
    std::cout << "    testPlasticityRuleClone passed" << std::endl;
}

void testPlasticityRuleReset() {
    nlm::PlasticityRule rule;
    
    rule.setLearningRate(0.05f);
    rule.setMaxWeightChange(2.0f);
    rule.setEnabled(false);
    
    rule.reset();
    
    assert(rule.getLearningRate() == 0.01f);  // Reset to default
    assert(rule.getMaxWeightChange() == 1.0f);  // Reset to default
    assert(rule.isEnabled() == true);  // Reset to default
    
    std::cout << "    testPlasticityRuleReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running PlasticityRule tests..." << std::endl;
    testPlasticityRuleCreation();
    testPlasticityRuleLearningRate();
    testPlasticityRuleMaxWeightChange();
    testPlasticityRuleEnabled();
    testPlasticityRuleType();
    testPlasticityRuleName();
    testPlasticityRuleClone();
    testPlasticityRuleReset();
}

} // namespace test_plasticity_rule