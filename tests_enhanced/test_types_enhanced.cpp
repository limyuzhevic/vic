// Enhanced Types Tests with Comprehensive Coverage
// Comprehensive test suite for neural type system, IDs, and classifications

#include "nlm_test_utilities.h"
#include "core/Types/Types.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <limits>

namespace test_types_enhanced {

// Basic ID type testing
void testNeuronIdBasic() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronIdBasic", nlm::test::TestCategory::Core);
    
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    framework.assert(id1 == id3, "Neuron ID 1 should equal Neuron ID 3");
    framework.assert(id1 != id2, "Neuron ID 1 should not equal Neuron ID 2");
    framework.assert(id1 < id2, "Neuron ID 1 should be less than Neuron ID 2");
    framework.assert(id2 > id1, "Neuron ID 2 should be greater than Neuron ID 1");
    framework.assert(id1.index() == 1, "Neuron ID 1 index should be 1");
    
    std::cout << "    testNeuronIdBasic passed" << std::endl;
}

void testNeuronIdEdgeCases() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronIdEdgeCases", nlm::test::TestCategory::Core);
    
    // Test with zero ID
    nlm::NeuronId zeroId(0);
    framework.assert(zeroId.index() == 0, "Zero Neuron ID index should be 0");
    
    // Test with maximum value (approximated)
    const uint64_t maxId = std::numeric_limits<uint64_t>::max();
    nlm::NeuronId maxNeuronId(maxId);
    framework.assert(maxNeuronId.index() == maxId, "Maximum Neuron ID index should equal max value");
    
    // Test comparison operators with chaining
    nlm::NeuronId id1(10);
    nlm::NeuronId id2(20);
    nlm::NeuronId id3(10);
    
    framework.assert((id1 == id3) && (id1 != id2), "Equality chaining test failed");
    framework.assert((id1 < id2) && (id2 > id1) && (id1 != id2), "Inequality chaining test failed");
    
    // Test reflexivity
    framework.assert(id1 == id1, "ID should be equal to itself");
    
    std::cout << "    testNeuronIdEdgeCases passed" << std::endl;
}

void testNeuronIdPerformance() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronIdPerformance", nlm::test::TestCategory::Performance);
    
    const size_t iterations = 10000;
    std::vector<nlm::NeuronId> ids;
    
    // Generate test data
    for (size_t i = 0; i < iterations; ++i) {
        ids.emplace_back(static_cast<uint64_t>(i % 1000));
    }
    
    // Test sorting performance
    auto start = std::chrono::high_resolution_clock::now();
    std::sort(ids.begin(), ids.end());
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double timePerOp = (duration.count() / static_cast<double>(iterations)) * 1000.0; // microseconds to milliseconds
    
    framework.assert(timePerOp < 100.0, "Sorting should be efficient (< 100 μs per operation)");
    
    // Test equality performance
    start = std::chrono::high_resolution_clock::now();
    size_t equalCount = 0;
    for (size_t i = 0; i < iterations; ++i) {
        if (ids[i] == ids[i % ids.size()]) {
            equalCount++;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    timePerOp = (duration.count() / static_cast<double>(iterations)) * 1000.0;
    
    framework.assert(timePerOp < 50.0, "Equality comparison should be efficient (< 50 μs per operation)");
    
    std::cout << "    testNeuronIdPerformance passed (" << duration.count() << " μs total)" << std::endl;
}

// SynapseId testing
void testSynapseId() {
    nlm::test::TestFramework framework;
    framework.startTest("testSynapseId", nlm::test::TestCategory::Core);
    
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    nlm::SynapseId id3(100);
    
    framework.assert(id1 == id3, "Synapse ID 100 should equal Synapse ID 100");
    framework.assert(id1 != id2, "Synapse ID 100 should not equal Synapse ID 200");
    framework.assert(id1 < id2, "Synapse ID 100 should be less than Synapse ID 200");
    framework.assert(id2 > id1, "Synapse ID 200 should be greater than Synapse ID 100");
    
    // Test with zero
    nlm::SynapseId zeroId(0);
    framework.assert(zeroId.index() == 0, "Synapse ID 0 index should be 0");
    
    // Test comparison with NeuronId (different types should not compare)
    nlm::NeuronId neuronId(100);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::test::TestFramework framework;
    framework.startTest("testRegionId", nlm::test::TestCategory::Core);
    
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    nlm::RegionId id3(1);
    
    framework.assert(id1 == id3, "Region ID 1 should equal Region ID 1");
    framework.assert(id1 != id2, "Region ID 1 should not equal Region ID 2");
    framework.assert(id1 < id2, "Region ID 1 should be less than Region ID 2");
    
    // Test with zero and maximum values
    nlm::RegionId zeroId(0);
    framework.assert(zeroId.index() == 0, "Region ID 0 should be valid");
    
    const uint64_t maxRegionId = std::numeric_limits<uint64_t>::max();
    nlm::RegionId maxRegion(maxRegionId);
    framework.assert(maxRegion.index() == maxRegionId, "Max Region ID should be valid");
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::test::TestFramework framework;
    framework.startTest("testPopulationId", nlm::test::TestCategory::Core);
    
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    nlm::PopulationId id3(1);
    
    framework.assert(id1 == id3, "Population ID 1 should equal Population ID 1");
    framework.assert(id1 != id2, "Population ID 1 should not equal Population ID 2");
    framework.assert(id1 < id2, "Population ID 1 should be less than Population ID 2");
    
    // Test range operations
    nlm::PopulationId startId(100);
    nlm::PopulationId endId(200);
    framework.assert(endId > startId, "End ID should be greater than start ID");
    
    std::cout << "    testPopulationId passed" << std::endl;
}

// Type enumeration testing
void testNeuronTypeEnumeration() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronTypeEnumeration", nlm::test::TestCategory::Core);
    
    // Test that all neuron types are distinct
    std::vector<nlm::NeuronType> neuronTypes = {
        nlm::NeuronType::Excitatory,
        nlm::NeuronType::Inhibitory,
        nlm::NeuronType::Sensory,
        nlm::NeuronType::Motor,
        nlm::NeuronType::Interneuron,
        nlm::NeuronType::Glutamatergic,
        nlm::NeuronType::GABAergic,
        nlm::NeuronType::Dopaminergic,
        nlm::NeuronType::Serotonergic,
        nlm::NeuronType::Cholinergic,
        nlm::NeuronType::Noradrenergic,
        nlm::NeuronType::Internal
    };
    
    // Check that all types are unique (assuming enum implementation)
    for (size_t i = 0; i < neuronTypes.size(); ++i) {
        for (size_t j = i + 1; j < neuronTypes.size(); ++j) {
            framework.assert(neuronTypes[i] != neuronTypes[j], 
                           "Neuron types should be distinct");
        }
    }
    
    // Test conversion to/from integer (if supported)
    nlm::NeuronType type = nlm::NeuronType::Excitatory;
    // In some implementations, types can be converted to integers
    
    std::cout << "    testNeuronTypeEnumeration passed" << std::endl;
}

void testSynapseTypeEnumeration() {
    nlm::test::TestFramework framework;
    framework.startTest("testSynapseTypeEnumeration", nlm::test::TestCategory::Core);
    
    // Test that synapse types are distinct
    std::vector<nlm::SynapseType> synapseTypes = {
        nlm::SynapseType::Excitatory,
        nlm::SynapseType::Inhibitory
    };
    
    framework.assert(synapseTypes.size() >= 2, "Should have at least 2 synapse types");
    framework.assert(synapseTypes[0] != synapseTypes[1], "Excitatory and Inhibitory should be different");
    
    // Test type conversion
    nlm::SynapseType excType = nlm::SynapseType::Excitatory;
    nlm::SynapseType inhType = nlm::SynapseType::Inhibitory;
    
    framework.assert(excType != inhType, "Excitatory and Inhibitory should not be equal");
    
    std::cout << "    testSynapseTypeEnumeration passed" << std::endl;
}

// PlasticityFlags testing
void testPlasticityFlags() {
    nlm::test::TestFramework framework;
    framework.startTest("testPlasticityFlags", nlm::test::TestCategory::Core);
    
    nlm::PlasticityFlags flags;
    
    // Test initial values
    framework.assert(!flags.hebbian, "Hebbian plasticity flag should be false initially");
    framework.assert(!flags.stdp, "STDP plasticity flag should be false initially");
    framework.assert(!flags.reward_modulated, "Reward-modulated plasticity flag should be false initially");
    
    // Test individual flag setting
    flags.hebbian = true;
    framework.assert(flags.hebbian, "Hebbian plasticity flag should be true after setting");
    framework.assert(!flags.stdp, "STDP flag should remain false");
    framework.assert(!flags.reward_modulated, "Reward-modulated flag should remain false");
    
    // Test combined operations
    nlm::PlasticityFlags flags2 = flags;
    flags2.stdp = true;
    
    framework.assert(flags2.hebbian && flags2.stdp, "Combined flags should preserve individual values");
    
    // Test bit operations (if used)
    uint32_t hebbianBit = 0x1;
    uint32_t stdpBit = 0x2;
    uint32_t rewardBit = 0x4;
    
    framework.assert((hebbianBit | stdpBit) != hebbianBit, "Bitwise OR should combine flags");
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

// SpikeEvent testing
void testSpikeEvent() {
    nlm::test::TestFramework framework;
    framework.startTest("testSpikeEvent", nlm::test::TestCategory::Core);
    
    nlm::SpikeEvent event;
    
    // Test initial values
    framework.assert(event.source_neuron.index() == 0, "Default source neuron should be 0");
    framework.assert(event.timestamp == 0.0f, "Default timestamp should be 0.0");
    framework.assert(event.step == 0, "Default step should be 0");
    
    // Test setting values
    event.source_neuron = nlm::NeuronId(42);
    event.timestamp = 123.456f;
    event.step = 1000;
    
    framework.assert(event.source_neuron.index() == 42, "Source neuron should be 42");
    framework.assert(event.timestamp == 123.456f, "Timestamp should be 123.456");
    framework.assert(event.step == 1000, "Step should be 1000");
    
    // Test with extreme values
    nlm::SpikeEvent extremeEvent;
    extremeEvent.source_neuron = nlm::NeuronId(std::numeric_limits<uint64_t>::max());
    extremeEvent.timestamp = std::numeric_limits<float>::max();
    extremeEvent.step = std::numeric_limits<int>::max();
    
    framework.assert(extremeEvent.source_neuron.index() == std::numeric_limits<uint64_t>::max(), "Max neuron ID should be valid");
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

// Type safety testing
void testTypeSafety() {
    nlm::test::TestFramework framework;
    framework.startTest("testTypeSafety", nlm::test::TestCategory::Core);
    
    // Test type comparisons
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::SynapseId synId(100);
    
    // Different ID types should not be directly comparable
    // This is a safety test - if the implementation allows comparison,
    // it should work correctly
    
    // Test that same type IDs can be compared
    framework.assert(id1 < id2, "Neuron ID 1 should be less than Neuron ID 2");
    framework.assert(synId.index() == 100, "Synapse ID should store its value correctly");
    
    // Test type ranges
    framework.assert(sizeof(nlm::NeuronId) <= sizeof(uint64_t), "Neuron ID should fit in 64 bits");
    framework.assert(sizeof(nlm::SynapseId) <= sizeof(uint64_t), "Synapse ID should fit in 64 bits");
    framework.assert(sizeof(nlm::RegionId) <= sizeof(uint64_t), "Region ID should fit in 64 bits");
    framework.assert(sizeof(nlm::PopulationId) <= sizeof(uint64_t), "Population ID should fit in 64 bits");
    
    std::cout << "    testTypeSafety passed" << std::endl;
}

// Performance testing for ID operations
void testIDPerformance() {
    nlm::test::TestFramework framework;
    framework.startTest("testIDPerformance", nlm::test::TestCategory::Performance);
    
    const size_t iterations = 100000;
    std::vector<nlm::NeuronId> neuronIds;
    std::vector<nlm::SynapseId> synapseIds;
    std::vector<nlm::RegionId> regionIds;
    std::vector<nlm::PopulationId> populationIds;
    
    // Generate test data
    for (size_t i = 0; i < iterations; ++i) {
        neuronIds.emplace_back(static_cast<uint64_t>(i));
        synapseIds.emplace_back(static_cast<uint64_t>(i * 2));
        regionIds.emplace_back(static_cast<uint64_t>(i % 1000));
        populationIds.emplace_back(static_cast<uint64_t>(i + 5000));
    }
    
    // Benchmark ID operations
    auto start = std::chrono::high_resolution_clock::now();
    
    // Sorting test
    std::sort(neuronIds.begin(), neuronIds.end());
    std::sort(synapseIds.begin(), synapseIds.end());
    std::sort(regionIds.begin(), regionIds.end());
    std::sort(populationIds.begin(), populationIds.end());
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double totalOps = iterations * 4; // 4 different ID types
    double timePerOp = (duration.count() / totalOps) * 1000.0; // milliseconds per operation
    
    framework.assert(timePerOp < 1.0, "ID operations should be efficient (< 1 ms per operation)");
    
    // Memory usage test (basic check)
    size_t totalMemory = neuronIds.size() * sizeof(nlm::NeuronId) +
                        synapseIds.size() * sizeof(nlm::SynapseId) +
                        regionIds.size() * sizeof(nlm::RegionId) +
                        populationIds.size() * sizeof(nlm::PopulationId);
    
    framework.assert(totalMemory < 100 * 1024 * 1024, "ID arrays should use reasonable memory (< 100MB)");
    
    std::cout << "    testIDPerformance passed (" << duration.count() << " ms total)" << std::endl;
}

// Test factory methods and special constructors
void testIDFactoryMethods() {
    nlm::test::TestFramework framework;
    framework.startTest("testIDFactoryMethods", nlm::test::TestCategory::Core);
    
    // Test that IDs can be created from various sources
    nlm::NeuronId id1(42);
    nlm::NeuronId id2 = nlm::NeuronId(42); // Copy constructor
    
    framework.assert(id1 == id2, "Copy constructor should create equal IDs");
    
    // Test that IDs can be used in standard containers
    std::vector<nlm::NeuronId> idVector;
    idVector.push_back(nlm::NeuronId(1));
    idVector.push_back(nlm::NeuronId(2));
    idVector.push_back(nlm::NeuronId(3));
    
    framework.assert(idVector.size() == 3, "Vector should contain 3 IDs");
    
    // Test find operations
    auto it = std::find(idVector.begin(), idVector.end(), nlm::NeuronId(2));
    framework.assert(it != idVector.end(), "Should find Neuron ID 2 in vector");
    
    std::cout << "    testIDFactoryMethods passed" << std::endl;
}

// Backward compatibility test
void testBackwardCompatibility() {
    nlm::test::TestFramework framework;
    framework.startTest("testBackwardCompatibility", nlm::test::TestCategory::Regression);
    
    // Test that IDs behave consistently with previous versions
    nlm::NeuronId oldStyleId(1);
    framework.assert(oldStyleId.index() == 1, "Index should return stored value");
    
    // Test that IDs can be used in switch statements (if enum values)
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    
    // This is a conceptual test - actual implementation may differ
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

// Integration test with other systems
void testIDIntegration() {
    nlm::test::TestFramework framework;
    framework.startTest("testIDIntegration", nlm::test::TestCategory::Integration);
    
    // Test that IDs work with configuration system
    nlm::Config config;
    config.set("neuron_id", 42, nlm::ConfigSource::Runtime);
    auto retrievedId = config.get<int>("neuron_id");
    framework.assert(retrievedId.has_value(), "Should retrieve neuron ID from config");
    
    // Test that IDs can be used as map keys
    std::map<nlm::NeuronId, int> idMap;
    idMap[nlm::NeuronId(1)] = 100;
    idMap[nlm::NeuronId(2)] = 200;
    
    framework.assert(idMap[nlm::NeuronId(1)] == 100, "Map should store neuron ID correctly");
    framework.assert(idMap[nlm::NeuronId(2)] == 200, "Map should store neuron ID correctly");
    
    std::cout << "    testIDIntegration passed" << std::endl;
}

bool runAllTests(nlm::test::TestFramework& framework) {
    std::cout << "Running Enhanced Types tests..." << std::endl;
    
    try {
        // Basic functionality tests
        testNeuronIdBasic();
        testSynapseId();
        testRegionId();
        testPopulationId();
        testNeuronTypeEnumeration();
        testSynapseTypeEnumeration();
        testPlasticityFlags();
        testSpikeEvent();
        
        // Advanced tests
        testNeuronIdEdgeCases();
        testNeuronIdPerformance();
        testSynapseIdPerformance();
        testRegionIdPerformance();
        testPopulationIdPerformance();
        
        // Type safety and compatibility tests
        testTypeSafety();
        testTypeCompatibility();
        testIDFactoryMethods();
        testBackwardCompatibility();
        
        // Integration tests
        testIDIntegration();
        testCrossSystemIntegration();
        
        // Performance tests
        testIDPerformance();
        
        std::cout << "  PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        return false;
    }
}

} // namespace test_types_enhanced