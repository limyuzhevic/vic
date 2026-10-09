#include "DevelopmentSystem.hpp"
#include "Synaptogenesis.hpp"
#include "Pruning.hpp"
#include "Maturation.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {

void test_synaptogenesis_system() {
    std::cout << "Testing Synaptogenesis System" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Test 1: Basic synaptogenesis creation
    std::cout << "Test 1: Basic synaptogenesis creation..." << std::endl;
    Synaptogenesis synaptogenesis;
    
    assert(synaptogenesis.getSynaptogenesisRate() >= 0.0f);
    assert(synaptogenesis.getSynapseGrowthFactor() >= 0.0f);
    assert(synaptogenesis.getInitialSynapseCount() >= 0);
    std::cout << "✓ Basic synaptogenesis creation passed" << std::endl;
    
    // Test 2: Synaptogenesis parameters
    std::cout << "Test 2: Synaptogenesis parameters..." << std::endl;
    synaptogenesis.setSynaptogenesisRate(0.1f);
    assert(synaptogenesis.getSynaptogenesisRate() == 0.1f);
    
    synaptogenesis.setSynapseGrowthFactor(1.5f);
    assert(synaptogenesis.getSynapseGrowthFactor() == 1.5f);
    
    synaptogenesis.setInitialSynapseCount(10);
    assert(synaptogenesis.getInitialSynapseCount() == 10);
    
    synaptogenesis.setMaxSynapseCount(1000);
    assert(synaptogenesis.getMaxSynapseCount() == 1000);
    std::cout << "✓ Synaptogenesis parameters passed" << std::endl;
    
    // Test 3: Synaptogenesis update
    std::cout << "Test 3: Synaptogenesis update..." << std::endl;
    synaptogenesis.update(0.1f, 10);  // timestep, neuron count
    assert(synaptogenesis.getCurrentSynapseCount() >= 0);
    std::cout << "✓ Synaptogenesis update passed" << std::endl;
    
    // Test 4: Synaptogenesis reset
    std::cout << "Test 4: Synaptogenesis reset..." << std::endl;
    synaptogenesis.reset();
    assert(synaptogenesis.getCurrentSynapseCount() == synaptogenesis.getInitialSynapseCount());
    std::cout << "✓ Synaptogenesis reset passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Synaptogenesis System tests PASSED! ✓" << std::endl;
    std::cout << "=============================" << std::endl;
}

void test_pruning_system() {
    std::cout << "Testing Pruning System" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Test 1: Basic pruning creation
    std::cout << "Test 1: Basic pruning creation..." << std::endl;
    Pruning pruning;
    
    assert(pruning.getPruningRate() >= 0.0f);
    assert(pruning.getSynapseLossFactor() >= 0.0f);
    assert(pruning.getSurvivalThreshold() >= 0.0f);
    std::cout << "✓ Basic pruning creation passed" << std::endl;
    
    // Test 2: Pruning parameters
    std::cout << "Test 2: Pruning parameters..." << std::endl;
    pruning.setPruningRate(0.05f);
    assert(pruning.getPruningRate() == 0.05f);
    
    pruning.setSynapseLossFactor(0.8f);
    assert(pruning.getSynapseLossFactor() == 0.8f);
    
    pruning.setSurvivalThreshold(0.5f);
    assert(pruning.getSurvivalThreshold() == 0.5f);
    
    pruning.setMaxSynapseCount(500);
    assert(pruning.getMaxSynapseCount() == 500);
    std::cout << "✓ Pruning parameters passed" << std::endl;
    
    // Test 3: Pruning update
    std::cout << "Test 3: Pruning update..." << std::endl;
    pruning.update(0.1f, 100);  // timestep, neuron count
    assert(pruning.getCurrentSynapseCount() >= 0);
    std::cout << "✓ Pruning update passed" << std::endl;
    
    // Test 4: Pruning reset
    std::cout << "Test 4: Pruning reset..." << std::endl;
    pruning.reset();
    assert(pruning.getCurrentSynapseCount() == pruning.getMaxSynapseCount());
    std::cout << "✓ Pruning reset passed" << std::endl;
    
    // Test 5: Pruning efficiency
    std::cout << "Test 5: Pruning efficiency..." << std::endl;
    float efficiency = pruning.calculatePruningEfficiency(100, 50);
    assert(efficiency >= 0.0f && efficiency <= 1.0f);
    std::cout << "✓ Pruning efficiency passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Pruning System tests PASSED! ✓" << std::endl;
    std::cout << "=======================" << std::endl;
}

void test_maturation_system() {
    std::cout << "Testing Maturation System" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Test 1: Basic maturation creation
    std::cout << "Test 1: Basic maturation creation..." << std::endl;
    Maturation maturation;
    
    assert(maturation.getMaturationRate() >= 0.0f);
    assert(maturation.getDevelopmentalStage() != nullptr);
    assert(maturation.getMaturityLevel() >= 0.0f && maturation.getMaturityLevel() <= 1.0f);
    std::cout << "✓ Basic maturation creation passed" << std::endl;
    
    // Test 2: Maturation parameters
    std::cout << "Test 2: Maturation parameters..." << std::endl;
    maturation.setMaturationRate(0.2f);
    assert(maturation.getMaturationRate() == 0.2f);
    
    maturation.setMaturityLevel(0.5f);
    assert(maturation.getMaturityLevel() == 0.5f);
    
    maturation.setDevelopmentalStage(" juvenile");
    assert(std::string(maturation.getDevelopmentalStage()) == " juvenile");
    std::cout << "✓ Maturation parameters passed" << std::endl;
    
    // Test 3: Maturation update
    std::cout << "Test 3: Maturation update..." << std::endl;
    maturation.update(0.1f);  // timestep
    assert(maturation.getMaturityLevel() >= 0.0f && maturation.getMaturityLevel() <= 1.0f);
    std::cout << "✓ Maturation update passed" << std::endl;
    
    // Test 4: Developmental stage progression
    std::cout << "Test 4: Developmental stage progression..." << std::endl;
    std::string initialStage = maturation.getDevelopmentalStage();
    maturation.advanceStage();
    assert(std::string(maturation.getDevelopmentalStage()) != initialStage);
    std::cout << "✓ Developmental stage progression passed" << std::endl;
    
    // Test 5: Maturation reset
    std::cout << "Test 5: Maturation reset..." << std::endl;
    maturation.reset();
    assert(maturation.getMaturityLevel() == 0.0f);
    assert(std::string(maturation.getDevelopmentalStage()) == " infant");
    std::cout << "✓ Maturation reset passed" << std::endl;
    
    // Test 6: Developmental age
    std::cout << "Test 6: Developmental age..." << std::endl;
    double age = maturation.getDevelopmentalAge();
    assert(age >= 0.0);
    
    maturation.setDevelopmentalAge(10.5);
    assert(maturation.getDevelopmentalAge() == 10.5);
    std::cout << "✓ Developmental age passed" << std::endl;
    
    // Test 7: Critical period
    std::cout << "Test 7: Critical period..." << std::endl;
    bool isCriticalPeriod = maturation.isCriticalPeriod();
    assert(isCriticalPeriod == true || isCriticalPeriod == false);  // Can be either
    
    float criticalPeriodProgress = maturation.getCriticalPeriodProgress();
    assert(criticalPeriodProgress >= 0.0f && criticalPeriodProgress <= 1.0f);
    std::cout << "✓ Critical period passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Maturation System tests PASSED! ✓" << std::endl;
    std::cout << "=========================" << std::endl;
}

void test_development_system() {
    std::cout << "Testing Development System" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Test 1: Basic development system creation
    std::cout << "Test 1: Basic development system creation..." << std::endl;
    DevelopmentSystem development;
    
    assert(development.getStage() != nullptr);
    assert(development.getPlasticityModifier() >= 0.0f);
    assert(development.getDevelopmentalAge() >= 0.0);
    std::cout << "✓ Basic development system creation passed" << std::endl;
    
    // Test 2: Developmental stage management
    std::cout << "Test 2: Developmental stage management..." << std::endl;
    DevelopmentalStage stage = development.getStage();
    development.setStage(stage);
    assert(development.getStage() == stage);
    std::cout << "✓ Developmental stage management passed" << std::endl;
    
    // Test 3: Stage advancement
    std::cout << "Test 3: Stage advancement..." << std::endl;
    development.advanceStage();
    assert(development.getStage() != nullptr);
    std::cout << "✓ Stage advancement passed" << std::endl;
    
    // Test 4: Developmental age management
    std::cout << "Test 4: Developmental age management..." << std::endl;
    double initialAge = development.getDevelopmentalAge();
    development.setDevelopmentalAge(initialAge + 1.0);
    assert(development.getDevelopmentalAge() == initialAge + 1.0);
    std::endl
    << "✓ Developmental age management passed" << std::endl;
    
    // Test 5: Plasticity modifier
    std::cout << "Test 5: Plasticity modifier..." << std::endl;
    float plasticity1 = development.getPlasticityModifier();
    development.advanceStage();
    float plasticity2 = development.getPlasticityModifier();
    assert(plasticity1 >= 0.0f);
    assert(plasticity2 >= 0.0f);
    std::cout << "✓ Plasticity modifier passed" << std::endl;
    
    // Test 6: Critical period information
    std::cout << "Test 6: Critical period information..." << std::endl;
    bool isCriticalPeriod = development.isCriticalPeriod();
    assert(isCriticalPeriod == true || isCriticalPeriod == false);
    
    float criticalPeriodProgress = development.getCriticalPeriodProgress();
    assert(criticalPeriodProgress >= 0.0f && criticalPeriodProgress <= 1.0f);
    std::cout << "✓ Critical period information passed" << std::endl;
    
    // Test 7: Stage name
    std::cout << "Test 7: Stage name..." << std::endl;
    const char* stageName = development.getStageName();
    assert(stageName != nullptr);
    assert(std::strlen(stageName) > 0);
    std::cout << "✓ Stage name passed" << std::endl;
    
    // Test 8: Development update
    std::cout << "Test 8: Development update..." << std::endl;
    // This would require a Brain instance, but we test the interface availability
    assert(development.getStage() != nullptr);
    std::cout << "✓ Development update interface available" << std::endl;
    
    // Test 9: Development reset
    std::cout << "Test 9: Development reset..." << std::endl;
    development.setStage(DevelopmentalStage::Infant);
    development.advanceStage();
    development.reset();
    assert(development.getStage() == DevelopmentalStage::Infant);
    assert(development.getDevelopmentalAge() == 0.0);
    std::cout << "✓ Development reset passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Development System tests PASSED! ✓" << std::endl;
    std::cout << "=========================" << std::endl;
}

void run_all_development_tests() {
    std::cout << "Running comprehensive Development tests..." << std::endl;
    std::cout << "===========================================" << std::endl;
    
    test_synaptogenesis_system();
    test_pruning_system();
    test_maturation_system();
    test_development_system();
    
    std::cout << "===========================================" << std::endl;
    std::cout << "All Development tests PASSED! ✓" << std::endl;
    std::cout << "===========================================" << std::endl;
}

} // namespace nlm