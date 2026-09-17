// Test file for NLM Brain.cpp critical bug fixes
// This test verifies that the random generator is properly initialized
// and that null pointer access is prevented in brain initialization

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain_initialization {

void testRandomGeneratorInitialization() {
    std::cout << "Testing random generator initialization..." << std::endl;
    
    // Create a default config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Default);
    
    // Create brain - this should properly initialize the random generator
    nlm::Brain brain(config);
    
    // Brain should be initialized
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get the random generator - this should not be null
    auto* rng = brain.getRandomGenerator();
    assert(rng != nullptr);  // CRITICAL FIX: Ensure rng is not null
    
    // Test that random generator produces values
    float rand1 = rng->uniformReal(0.0f, 1.0f);
    float rand2 = rng->uniformReal(0.0f, 1.0f);
    
    assert(rand1 >= 0.0f && rand1 < 1.0f);
    assert(rand2 >= 0.0f && rand2 < 1.0f);
    
    // Ensure reproducibility with same seed
    nlm::Brain brain2(config);
    bool initialized2 = brain2.initialize();
    assert(initialized2);
    
    auto* rng2 = brain2.getRandomGenerator();
    assert(rng2 != nullptr);
    
    float rand1_2 = rng2->uniformReal(0.0f, 1.0f);
    assert(rand1_2 == rand1);  // Should be identical for same seed
    
    std::cout << "✓ Random generator initialization test passed" << std::endl;
}

void testDevelopmentSystemAccess() {
    std::cout << "Testing development system access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get development system - should not be null
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);  // Should be properly initialized
    
    std::cout << "✓ Development system access test passed" << std::endl;
}

void testNeuromodulationSystemsAccess() {
    std::cout << "Testing neuromodulation systems access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get all neuromodulation systems - should not be null
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    std::cout << "✓ Neuromodulation systems access test passed" << std::endl;
}

void testMemorySystemsAccess() {
    std::cout << "Testing memory systems access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get memory systems - should not be null
    auto* workingMem = brain.getWorkingMemory();
    auto* episodicMem = brain.getEpisodicMemory();
    auto* associativeMem = brain.getAssociativeMemory();
    
    assert(workingMem != nullptr);
    assert(episodicMem != nullptr);
    assert(associativeMem != nullptr);
    
    std::cout << "✓ Memory systems access test passed" << std::endl;
}

void testCognitionSystemsAccess() {
    std::cout << "Testing cognition systems access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get cognition systems - should not be null
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    std::cout << "✓ Cognition systems access test passed" << std::endl;
}

void testStructurePlasticityAccess() {
    std::cout << "Testing structural plasticity access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get structural plasticity - should not be null
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    std::cout << "✓ Structural plasticity access test passed" << std::endl;
}

void testSTDPAndHebbianAccess() {
    std::cout << "Testing STDP and Hebbian access..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Get plasticity systems - should not be null
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    
    std::cout << "✓ STDP and Hebbian access test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== NLM Brain Initialization Critical Bug Fix Tests ===" << std::endl;
    std::cout << "Testing fixes for Brain.cpp critical bugs" << std::endl;
    std::cout << "" << std::endl;
    
    testRandomGeneratorInitialization();
    testDevelopmentSystemAccess();
    testNeuromodulationSystemsAccess();
    testMemorySystemsAccess();
    testCognitionSystemsAccess();
    testStructurePlasticityAccess();
    testSTDPAndHebbianAccess();
    
    std::cout << "" << std::endl;
    std::cout << "=== All Tests Passed ===" << std::endl;
    std::cout << "Critical bugs in Brain.cpp have been fixed:" << std::endl;
    std::cout << "✓ Random generator properly initialized" << std::endl;
    std::cout << "✓ All system pointers initialized (not null)" << std::endl;
    std::cout << "✓ Development system accessible" << std::endl;
    std::cout << "✓ Neuromodulation systems accessible" << std::endl;
    std::cout << "✓ Memory systems accessible" << std::endl;
    std::cout << "✓ Cognition systems accessible" << std::endl;
    std::cout << "✓ Structural plasticity accessible" << std::endl;
    std::cout << "✓ STDP and Hebbian plasticity accessible" << std::endl;
}

} // namespace test_brain_initialization

int main() {
    test_brain_initialization::runAllTests();
    return 0;
}
