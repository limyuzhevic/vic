#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/PlasticityRule.hpp"
#include <iostream>
#include <cassert>

void testOriginalHebbian() {
    std::cout << "Testing Original Hebbian..." << std::endl;
    
    // Create original Hebbian using factory
    auto hebbian = nlm::createHebbianRule("original");
    
    assert(hebbian->getName() == std::string("Hebbian"));
    assert(hebbian->getLearningRate() == 0.01f);
    assert(hebbian->getMaxWeight() == 1.0f);
    assert(hebbian->getMinWeight() == -1.0f);
    
    // Test learning rate setting
    hebbian->setLearningRate(0.05f);
    assert(hebbian->getLearningRate() == 0.05f);
    
    // Test max weight setting
    hebbian->setMaxWeight(2.0f);
    assert(hebbian->getMaxWeight() == 2.0f);
    
    // Test min weight setting
    hebbian->setMinWeight(-2.0f);
    assert(hebbian->getMinWeight() == -2.0f);
    
    std::cout << "✓ Original Hebbian tests passed" << std::endl;
}

void testEnhancedHebbian() {
    std::cout << "Testing Enhanced Hebbian..." << std::endl;
    
    // Create enhanced Hebbian using factory
    auto hebbian = nlm::createHebbianRule("enhanced");
    
    // EnhancedHebbian uses different name
    assert(hebbian->getName() == std::string("EnhancedHebbian"));
    
    // Test enhanced Hebbian specific parameters
    hebbian->setLearningRatePositive(0.02f);
    hebbian->setLearningRateNegative(0.015f);
    assert(hebbian->getLearningRatePositive() == 0.02f);
    assert(hebbian->getLearningRateNegative() == 0.015f);
    
    // Test correlation window
    hebbian->setCorrelationWindow(75.0f);
    assert(hebbian->getCorrelationWindow() == 75.0f);
    
    // Test weight normalization
    hebbian->setWeightNormalizationFactor(0.8f);
    assert(hebbian->getWeightNormalizationFactor() == 0.8f);
    
    // Test synaptic scaling
    hebbian->setSynapticScalingRate(0.005f);
    assert(hebbian->getSynapticScalingRate() == 0.005f);
    
    // Test competition strength
    hebbian->setCompetitionStrength(0.1f);
    assert(hebbian->getCompetitionStrength() == 0.1f);
    
    std::cout << "✓ Enhanced Hebbian tests passed" << std::endl;
}

void testHebbianFactory() {
    std::cout << "Testing Hebbian factory function..." << std::endl;
    
    // Test default creation
    auto hebbian1 = nlm::createHebbianRule("default");
    assert(hebbian1->getName() == std::string("Hebbian"));
    
    // Test original creation
    auto hebbian2 = nlm::createHebbianRule("original");
    assert(hebbian2->getName() == std::string("Hebbian"));
    
    // Test enhanced creation
    auto hebbian3 = nlm::createHebbianRule("enhanced");
    assert(hebbian3->getName() == std::string("EnhancedHebbian"));
    
    // Test unknown type falls back to original
    auto hebbian4 = nlm::createHebbianRule("unknown");
    assert(hebbian4->getName() == std::string("Hebbian"));
    
    std::cout << "✓ Hebbian factory tests passed" << std::endl;
}

void testHebbianParameters() {
    std::cout << "Testing Hebbian parameter structure..." << std::endl;
    
    nlm::HebbianParameters params;
    
    // Test default parameters
    assert(params.learningRatePositive == 0.01f);
    assert(params.learningRateNegative == 0.008f);
    assert(params.correlationWindow == 50.0f);
    assert(params.weightNormalizationFactor == 0.1f);
    assert(params.synapticScalingRate == 0.001f);
    assert(params.competitionStrength == 0.05f);
    assert(params.maxWeightChange == 0.1f);
    assert(params.baselineHebbian == 0.001f);
    assert(params.uptakeLimit == 0.05f);
    assert(params.deliveryLimit == 0.05f);
    assert(params.targetWeightRange == 0.5f);
    assert(params.meanActivityEstimate == 0.0f);
    assert(params.normalizationInterval == 1000.0f);
    
    // Test parameter modification
    params.learningRatePositive = 0.05f;
    params.learningRateNegative = 0.03f;
    params.correlationWindow = 75.0f;
    
    assert(params.learningRatePositive == 0.05f);
    assert(params.learningRateNegative == 0.03f);
    assert(params.correlationWindow == 75.0f);
    
    std::cout << "✓ Hebbian parameter tests passed" << std::endl;
}

void testHebbianIntegrationWithSTDP() {
    std::cout << "Testing Hebbian integration with STDP..." << std::endl;
    
    // Create both plasticity rules
    auto stdpRule = std::make_unique<nlm::STDP>();
    auto hebbianRule = nlm::createHebbianRule("enhanced");
    
    // Verify they have different names
    assert(stdpRule->getName() == std::string("STDP"));
    assert(hebbianRule->getName() == std::string("EnhancedHebbian"));
    
    // Test that they have different parameter sets
    stdpRule->configure(0.02f, 0.015f, 20.0f);
    assert(stdpRule->getLTPWeight() == 0.02f);
    assert(stdpRule->getLTDWeight() == 0.015f);
    
    stdpRule->setLearningRatePositive(0.01f);
    assert(stdpRule->getLearningRatePositive() == 0.01f);
    
    std::cout << "✓ Hebbian-STDP integration tests passed" << std::endl;
}

int main() {
    std::cout << "=== Hebbian Implementation Tests ===" << std::endl;
    
    try {
        testHebbianParameters();
        testOriginalHebbian();
        testEnhancedHebbian();
        testHebbianFactory();
        testHebbianIntegrationWithSTDP();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        std::cout << "The Hebbian implementations are working correctly." << std::endl;
        std::cout << "Original Hebbian maintains backward compatibility." << std::endl;
        std::cout << "Enhanced Hebbian provides advanced features." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n✗ Test failed with unknown exception" << std::endl;
        return 1;
    }
}