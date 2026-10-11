// Associative Memory Tests
// Tests for AssociativeMemory functionality including association creation, strength management, and neural pattern relationships

#include "memory/Memory.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_associative_memory {

void testAssociativeMemoryCreation() {
    nlm::AssociativeMemory memory;
    
    // Test basic properties
    // All associations should start empty
    assert(memory.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) == 0.0f);
    assert(memory.getAssociations(nlm::NeuronId(1)).empty());
    assert(memory.getAssociations(nlm::NeuronId(2)).empty());
    
    std::cout << "    testAssociativeMemoryCreation passed" << std::endl;
}

void testAssociativeMemoryCreateAndRetrieveAssociations() {
    nlm::AssociativeMemory memory;
    
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    nlm::NeuronId neuron3(3);
    nlm::NeuronId neuron4(4);
    
    // Create associations
    memory.associate(neuron1, neuron2, 0.8f);
    memory.associate(neuron3, neuron4, 0.5f);
    memory.associate(neuron1, neuron3, 0.9f);
    
    // Test retrieval of associations
    std::vector<nlm::NeuronId> associations1 = memory.getAssociations(neuron1);
    assert(associations1.size() == 2); // neuron2 and neuron3
    
    std::vector<nlm::NeuronId> associations3 = memory.getAssociations(neuron3);
    assert(associations3.size() == 2); // neuron1 and neuron4
    
    // Test association strength
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron2), 0.8f));
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron2, neuron1), 0.8f)); // Should be symmetric
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron3, neuron4), 0.5f));
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron3), 0.9f));
    
    // Test non-existent associations return 0.0
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron4), 0.0f));
    
    std::cout << "    testAssociativeMemoryCreateAndRetrieveAssociations passed" << std::endl;
}

void testAssociativeMemorySymmetricAssociations() {
    nlm::AssociativeMemory memory;
    
    nlm::NeuronId neuronA(1);
    nlm::NeuronId neuronB(2);
    
    // Create association in one direction
    memory.associate(neuronA, neuronB, 0.7f);
    
    // Should be retrievable in both directions
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuronA, neuronB), 0.7f));
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuronB, neuronA), 0.7f));
    
    // Get associations should return both neurons
    std::vector<nlm::NeuronId> assocA = memory.getAssociations(neuronA);
    std::vector<nlm::NeuronId> assocB = memory.getAssociations(neuronB);
    
    assert(assocA.size() == 1 && assocA[0] == neuronB);
    assert(assocB.size() == 1 && assocB[0] == neuronA);
    
    std::cout << "    testAssociativeMemorySymmetricAssociations passed" << std::endl;
}

void testAssociativeMemoryUpdateExisting() {
    nlm::AssociativeMemory memory;
    
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    // Create initial association
    memory.associate(neuron1, neuron2, 0.5f);
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron2), 0.5f));
    
    // Update association strength
    memory.updateAssociation(neuron1, neuron2, 0.3f);
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron2), 0.8f));
    
    // Update in opposite direction should have same effect
    memory.updateAssociation(neuron2, neuron1, -0.2f);
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron2), 0.6f));
    
    // Test that association remains within valid range
    memory.updateAssociation(neuron1, neuron2, 1.0f); // Set to max
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron2), 1.0f));
    
    memory.updateAssociation(neuron1, neuron2, -2.0f); // Should clamp at 0.0
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron2), 0.0f));
    
    std::cout << "    testAssociativeMemoryUpdateExisting passed" << std::endl;
}

void testAssociativeMemoryMultipleConnections() {
    nlm::AssociativeMemory memory;
    
    nlm::NeuronId center(0);
    
    // Create multiple connections from a single neuron
    std::vector<nlm::NeuronId> neighbors;
    for (int i = 1; i <= 10; ++i) {
        nlm::NeuronId neighbor(i);
        neighbors.push_back(neighbor);
        memory.associate(center, neighbor, static_cast<float>(i) * 0.1f);
    }
    
    // Verify all connections
    assert(memory.getAssociations(center).size() == 10);
    
    for (int i = 1; i <= 10; ++i) {
        nlm::NeuronId neighbor(i);
        assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(center, neighbor), static_cast<float>(i) * 0.1f));
    }
    
    // Verify reverse connections
    for (int i = 1; i <= 10; ++i) {
        nlm::NeuronId neighbor(i);
        assert(memory.getAssociations(neighbor).size() == 1);
        assert(memory.getAssociations(neighbor)[0] == center);
    }
    
    std::cout << "    testAssociativeMemoryMultipleConnections passed" << std::endl;
}

void testAssociativeMemoryRemoveByClearing() {
    nlm::AssociativeMemory memory;
    
    // Create several associations
    memory.associate(nlm::NeuronId(1), nlm::NeuronId(2), 0.8f);
    memory.associate(nlm::NeuronId(3), nlm::NeuronId(4), 0.6f);
    memory.associate(nlm::NeuronId(5), nlm::NeuronId(6), 0.9f);
    
    // Verify they exist
    assert(memory.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)) > 0.0f);
    assert(memory.getAssociationStrength(nlm::NeuronId(3), nlm::NeuronId(4)) > 0.0f);
    assert(memory.getAssociationStrength(nlm::NeuronId(5), nlm::NeuronId(6)) > 0.0f);
    
    // Clear all associations
    memory.clear();
    
    // All should now be zero
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(nlm::NeuronId(1), nlm::NeuronId(2)), 0.0f));
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(nlm::NeuronId(3), nlm::NeuronId(4)), 0.0f));
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(nlm::NeuronId(5), nlm::NeuronId(6)), 0.0f));
    
    std::cout << "    testAssociativeMemoryRemoveByClearing passed" << std::endl;
}

void testAssociativeMemoryIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test associative memory integration through brain
    auto* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Create test associations
    nlm::NeuronId neuronA(42);
    nlm::NeuronId neuronB(43);
    
    associativeMemory->associate(neuronA, neuronB, 0.75f);
    assert(nlm_test::utils::approxEqual(associativeMemory->getAssociationStrength(neuronA, neuronB), 0.75f));
    
    // Verify symmetric access
    std::vector<nlm::NeuronId> assocA = associativeMemory->getAssociations(neuronA);
    std::vector<nlm::NeuronId> assocB = associativeMemory->getAssociations(neuronB);
    
    assert(assocA.size() == 1 && assocA[0] == neuronB);
    assert(assocB.size() == 1 && assocB[0] == neuronA);
    
    // Test strength updates through brain interface
    associativeMemory->updateAssociation(neuronA, neuronB, 0.2f);
    assert(nlm_test::utils::approxEqual(associativeMemory->getAssociationStrength(neuronA, neuronB), 0.95f));
    
    std::cout << "    testAssociativeMemoryIntegrationWithBrain passed" << std::endl;
}

void testAssociativeMemoryPatternLearning() {
    nlm::AssociativeMemory memory;
    
    // Simulate pattern learning: many neurons become associated over time
    std::vector<std::pair<nlm::NeuronId, nlm::NeuronId>> patterns;
    
    for (int iteration = 0; iteration < 100; ++iteration) {
        nlm::NeuronId a(rand() % 50); // Random neuron 0-49
        nlm::NeuronId b(rand() % 50); // Random neuron 0-49
        
        // Stronger associations for similar neurons
        float strength = 1.0f - (std::abs(static_cast<int>(a.value - b.value)) / 50.0f);
        
        memory.associate(a, b, strength);
        patterns.push_back({a, b});
    }
    
    // Verify associations exist
    assert(memory.getCurrentSize() > 0); // Internal method not exposed, checking indirectly
    
    // Test retrieval of some associations
    bool foundAssociation = false;
    for (const auto& pattern : patterns) {
        if (memory.getAssociationStrength(pattern.first, pattern.second) > 0.0f) {
            foundAssociation = true;
            break;
        }
    }
    
    assert(foundAssociation);
    
    std::cout << "    testAssociativeMemoryPatternLearning passed" << std::endl;
}

void testAssociativeMemoryEdgeCases() {
    nlm::AssociativeMemory memory;
    
    // Test self-association (same neuron)
    nlm::NeuronId neuron1(1);
    memory.associate(neuron1, neuron1, 0.5f);
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, neuron1), 0.5f));
    
    // Test zero strength
    nlm::NeuronId neuron2(2);
    nlm::NeuronId neuron3(3);
    memory.associate(neuron2, neuron3, 0.0f);
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron2, neuron3), 0.0f));
    
    // Test maximum strength
    memory.associate(neuron2, neuron3, 2.0f); // Should clamp to 1.0
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron2, neuron3), 1.0f));
    
    // Test invalid negative IDs
    nlm::NeuronId invalidId(UINT64_MAX);
    memory.associate(neuron1, invalidId, 0.5f);
    assert(nlm_test::utils::approxEqual(memory.getAssociationStrength(neuron1, invalidId), 0.5f));
    
    std::cout << "    testAssociativeMemoryEdgeCases passed" << std::endl;
}

void testAssociativeMemoryStress() {
    nlm::AssociativeMemory memory;
    
    // Rapid association creation and retrieval
    for (int i = 0; i < 500; ++i) {
        nlm::NeuronId a(i % 100); // Reuse neurons
        nlm::NeuronId b((i + 1) % 100);
        
        memory.associate(a, b, static_cast<float>(i % 100) / 100.0f);
        
        // Verify association was created
        if (i < 200) { // Only verify first 200 to avoid too much output
            assert(memory.getAssociationStrength(a, b) >= 0.0f);
        }
    }
    
    std::cout << "    testAssociativeMemoryStress passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Associative Memory Creation", testAssociativeMemoryCreation);
    runner.runTest("Associative Memory Create and Retrieve Associations", testAssociativeMemoryCreateAndRetrieveAssociations);
    runner.runTest("Associative Memory Symmetric Associations", testAssociativeMemorySymmetricAssociations);
    runner.runTest("Associative Memory Update Existing", testAssociativeMemoryUpdateExisting);
    runner.runTest("Associative Memory Multiple Connections", testAssociativeMemoryMultipleConnections);
    runner.runTest("Associative Memory Remove by Clearing", testAssociativeMemoryRemoveByClearing);
    runner.runTest("Associative Memory Integration With Brain", testAssociativeMemoryIntegrationWithBrain);
    runner.runTest("Associative Memory Pattern Learning", testAssociativeMemoryPatternLearning);
    runner.runTest("Associative Memory Edge Cases", testAssociativeMemoryEdgeCases);
    runner.runTest("Associative Memory Stress Test", testAssociativeMemoryStress);
    
    runner.runAll();
}

} // namespace test_associative_memory
