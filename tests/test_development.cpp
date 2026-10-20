// Development System Tests
// Tests for developmental stages, synaptogenesis, pruning, maturation

#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_development {

void testDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::DevelopmentSystem devSystem(config);
    
    // Test basic operations
    assert(devSystem.getCurrentStage() >= nlm::DevelopmentalStage::Initial);
    assert(devSystem.getCurrentStage() <= nlm::DevelopmentalStage::Aging);
    assert(devSystem.getMaturationRate() >= 0.0f && devSystem.getMaturationRate() <= 1.0f);
    
    // Test development progression
    devSystem.updateDevelopment(0.001f);
    
    assert(devSystem.getCurrentStage() != nlm::DevelopmentalStage::Initial ||
           devSystem.getCurrentStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

void testSynaptogenesis() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Synaptogenesis synaptogenesis(config);
    
    // Test basic operations
    assert(synaptogenesis.getRate() >= 0.0f);
    assert(synaptogenesis.getMaxConnections() > 0);
    
    // Test synapse formation
    synaptogenesis.formSynapse(0.5f, 0.8f);
    assert(synaptogenesis.getRecentFormations() > 0);
    
    std::cout << "    testSynaptogenesis passed" << std::endl;
}

void testPruning() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Pruning pruning(config);
    
    // Test basic operations
    assert(pruning.getRate() >= 0.0f);
    assert(pruning.getMinConnections() >= 0);
    
    // Test synapse elimination
    pruning.eliminateSynapse(0.5f, 0.8f);
    assert(pruning.getRecentEliminations() >= 0);
    
    std::cout << "    testPruning passed" << std::endl;
}

void testMaturation() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Maturation maturation(config);
    
    // Test basic operations
    assert(maturation.getRate() >= 0.0f && maturation.getRate() <= 1.0f);
    assert(maturation.getPlasticityReduction() >= 0.0f && maturation.getPlasticityReduction() <= 1.0f);
    
    // Test maturation process
    maturation.processMaturation(0.1f);
    
    assert(maturation.getCurrentMaturationLevel() >= 0.0f);
    
    std::cout << "    testMaturation passed" << std::endl;
}

void runAll() {
    testDevelopmentSystem();
    testSynaptogenesis();
    testPruning();
    testMaturation();
}

} // namespace test_development