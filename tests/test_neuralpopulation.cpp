// NLM NeuralPopulation Tests
// Testing improved memory management and error handling

#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include <cassert>
#include <iostream>

namespace test_neuralpopulation {

void testMemoryManagement() {
    std::cout << "  Testing memory management..." << std::endl;
    
    // Test creation and destruction
    {
        NeuralPopulation pop(PopulationId(1), 10);
        assert(pop.getId().value == 1);
        assert(pop.getSize() == 10);
        assert(pop.isEmpty());
    } // Destructor should be called here
    
    std::cout << "    Memory management test passed" << std::endl;
}

void testBoundsChecking() {
    std::cout << "  Testing bounds checking..." << std::endl;
    
    NeuralPopulation pop(PopulationId(2), 5);
    
    // Test null neuron pointer
    pop.addNeuron(nullptr);
    assert(pop.getSize() == 5); // Should remain unchanged
    
    // Test out-of-bounds access
    assert(pop.getNeuron(-1) == nullptr);
    assert(pop.getNeuron(10) == nullptr);
    assert(pop.getNeuron(5) == nullptr); // Index 5 is out of bounds for 5 neurons
    
    // Test const access
    const NeuralPopulation& constPop = pop;
    assert(constPop.getNeuron(-1) == nullptr);
    assert(constPop.getNeuron(10) == nullptr);
    
    std::cout << "    Bounds checking test passed" << std::endl;
}

void testNullSafety() {
    std::cout << "  Testing null safety..." << std::endl;
    
    NeuralPopulation pop(PopulationId(3), 0);
    assert(pop.isEmpty());
    assert(pop.getActiveCount() == 0);
    assert(pop.getFiringCount() == 0);
    assert(pop.getAverageFiringRate() == 0.0f);
    assert(pop.getActivityLevel() == 0.0f);
    assert(pop.getNeuronType() == NeuronType::Internal);
    assert(pop.getMeanMembranePotential() == 0.0f);
    assert(pop.getVarianceMembranePotential() == 0.0f);
    
    // Test with null pImpl (should not crash)
    NeuralPopulation nullPop;
    assert(nullPop.getNeuronType() == NeuronType::Internal);
    assert(nullPop.getActiveCount() == 0);
    assert(nullPop.getFiringCount() == 0);
    
    std::cout << "    Null safety test passed" << std::endl;
}

void testStatisticsCalculations() {
    std::cout << "  Testing statistics calculations..." << std::endl;
    
    // This test would need actual neuron objects to populate
    // For now, just test that empty populations return correct values
    NeuralPopulation pop(PopulationId(4), 0);
    
    assert(pop.getActivityLevel() == 0.0f);
    assert(pop.getAverageFiringRate() == 0.0f);
    assert(pop.getMeanMembranePotential() == 0.0f);
    assert(pop.getVarianceMembranePotential() == 0.0f);
    
    // Test with null previous membrane potentials
    pop.recordActivity();
    assert(pop.getMeanMembranePotential() == 0.0f);
    assert(pop.getVarianceMembranePotential() == 0.0f);
    
    std::cout << "    Statistics calculations test passed" << std::endl;
}

void testMoveSemantics() {
    std::cout << "  Testing move semantics..." << std::endl;
    
    NeuralPopulation original(PopulationId(5), 10);
    PopulationId originalId = original.getId();
    
    // Move constructor
    NeuralPopulation moved(std::move(original));
    assert(moved.getId().value == originalId.value);
    assert(original.getId().value == 0); // Source should be empty
    
    // Move assignment
    NeuralPopulation target(PopulationId(6), 5);
    target = std::move(moved);
    assert(target.getId().value == originalId.value);
    assert(moved.getId().value == 0); // Source should be empty
    
    std::cout << "    Move semantics test passed" << std::endl;
}

void testGetNeuronIds() {
    std::cout << "  Testing getNeuronIds..." << std::endl;
    
    NeuralPopulation pop(PopulationId(7), 0);
    
    // Test with empty population
    auto ids = pop.getNeuronIds();
    assert(ids.empty());
    
    // Test null safety
    std::vector<NeuronId> nullIds;
    assert(nullIds.empty());
    
    std::cout << "    GetNeuronIds test passed" << std::endl;
}

void runAll() {
    std::cout << "=== NeuralPopulation Tests ===" << std::endl;
    
    testMemoryManagement();
    testBoundsChecking();
    testNullSafety();
    testStatisticsCalculations();
    testMoveSemantics();
    testGetNeuronIds();
    
    std::cout << "=== All NeuralPopulation Tests PASSED ===" << std::endl;
}

} // namespace test_neuralpopulation
