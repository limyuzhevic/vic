// Plasticity Tests
// Tests for Hebbian, STDP, and other plasticity mechanisms

#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_plasticity {

void testHebbianPlasticity() {
    std::cout << "    Testing Hebbian plasticity..." << std::endl;
    
    nlm::HebbianRule hebbian;
    
    // Test basic properties
    assert(std::string(hebbian.getName()) == "Hebbian");
    assert(hebbian.isEnabled());
    
    // Test parameter setters/getters
    hebbian.setLearningRate(0.05f);
    assert(hebbian.getLearningRate() == 0.05f);
    
    hebbian.setMaxWeight(2.0f);
    assert(hebbian.getMaxWeight() == 2.0f);
    
    hebbian.setMinWeight(-2.0f);
    assert(hebbian.getMinWeight() == -2.0f);
    
    hebbian.setCovarianceThreshold(0.1f);
    assert(hebbian.getCovarianceThreshold() == 0.1f);
    
    // Test configuration methods
    hebbian.configureForSupervisedLearning();
    hebbian.configureForUnsupervisedLearning();
    hebbian.configureForOnlineLearning();
    
    // Test statistics
    auto stats = hebbian.getStatistics();
    assert(stats.totalUpdates == 0);
    assert(stats.strongConnections == 0);
    assert(stats.weakConnections == 0);
    assert(stats.balancedConnections == 0);
    
    hebbian.resetStatistics();
    stats = hebbian.getStatistics();
    assert(stats.totalUpdates == 0);
    
    std::cout << "      Hebbian plasticity test PASSED" << std::endl;
}

void testSTDP() {
    std::cout << "    Testing STDP..." << std::endl;
    
    nlm::STDP stdp;
    
    // Test basic properties
    assert(std::string(stdp.getName()) == "STDP");
    assert(stdp.isEnabled());
    
    // Test parameter setters/getters
    stdp.setLTPWeight(0.02f);
    assert(stdp.getLTPWeight() == 0.02f);
    
    stdp.setLTDWeight(0.015f);
    assert(stdp.getLTDWeight() == 0.015f);
    
    stdp.setTimeConstant(20.0f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    stdp.setMinWeight(-1.0f);
    assert(stdp.getMinWeight() == -1.0f);
    
    stdp.setMaxWeight(1.0f);
    assert(stdp.getMaxWeight() == 1.0f);
    
    stdp.setLearningRate(0.01f);
    assert(stdp.getLearningRate() == 0.01f);
    
    // Test configuration
    stdp.configure(0.02f, 0.015f, 20.0f, -1.0f, 1.0f, 0.01f);
    assert(stdp.getLTPWeight() == 0.02f);
    assert(stdp.getLTDWeight() == 0.015f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    // Test statistics
    auto stats = stdp.getStatistics();
    assert(stats.totalLTPEvents == 0);
    assert(stats.totalLTDEvents == 0);
    assert(stats.totalWeightChange == 0.0f);
    
    stdp.resetStatistics();
    stats = stdp.getStatistics();
    assert(stats.totalLTPEvents == 0);
    
    // Test biological plausibility
    assert(stdp.isBiologicallyPlausible());
    stdp.normalizeToBiologicalRanges();
    
    std::cout << "      STDP test PASSED" << std::endl;
}

void testBCMPlasticity() {
    std::cout << "    Testing BCM plasticity..." << std::endl;
    
    nlm::PlasticityRuleFactory factory;
    auto bcm = factory.createBCM();
    
    assert(std::string(bcm->getName()) == "BCM");
    assert(bcm->isEnabled());
    
    // Test BCM-specific parameters
    assert(bcm->isBiologicallyPlausible());
    
    std::cout << "      BCM plasticity test PASSED" << std::endl;
}

void testAntiHebbianPlasticity() {
    std::cout << "    Testing Anti-Hebbian plasticity..." << std::endl;
    
    nlm::PlasticityRuleFactory factory;
    auto antiHebbian = factory.createAntiHebbian();
    
    assert(std::string(antiHebbian->getName()) == "AntiHebbian");
    assert(antiHebbian->isEnabled());
    
    std::cout << "      Anti-Hebbian plasticity test PASSED" << std::endl;
}

void testOjaPlasticity() {
    std::cout << "    Testing Oja plasticity..." << std::endl;
    
    nlm::PlasticityRuleFactory factory;
    auto oja = factory.createOja();
    
    assert(std::string(oja->getName()) == "Oja");
    assert(oja->isEnabled());
    
    std::cout << "      Oja plasticity test PASSED" << std::endl;
}

void runAll() {
    testHebbianPlasticity();
    testSTDP();
    testBCMPlasticity();
    testAntiHebbianPlasticity();
    testOjaPlasticity();
    
    std::cout << "    All plasticity tests PASSED" << std::endl;
}

} // namespace test_plasticity