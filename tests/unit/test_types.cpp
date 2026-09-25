// Types Unit Tests
// Comprehensive test suite for NLM type system using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>

namespace nlm {
namespace test {

// Test fixture for types tests
class TypesTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Test various type instantiations
        neuronId1 = NeuronId(1);
        neuronId2 = NeuronId(2);
        synapseId1 = SynapseId(1);
        synapseId2 = SynapseId(2);
        regionId1 = RegionId(1);
        regionId2 = RegionId(2);
        populationId1 = PopulationId(1);
        populationId2 = PopulationId(2);
    }
    
    void TearDown() override {
        TestBase::TearDown();
    }
    
protected:
    nlm::NeuronId neuronId1;
    nlm::NeuronId neuronId2;
    nlm::SynapseId synapseId1;
    nlm::SynapseId synapseId2;
    nlm::RegionId regionId1;
    nlm::RegionId regionId2;
    nlm::PopulationId populationId1;
    nlm::PopulationId populationId2;
};

// Test NeuronId operations
TEST_F(TypesTest, NeuronIdBasic) {
    EXPECT_TRUE(neuronId1.isValid());
    EXPECT_TRUE(neuronId2.isValid());
    
    // Equality comparison
    EXPECT_EQ(neuronId1, neuronId1);
    EXPECT_NE(neuronId1, neuronId2);
    
    // Less than comparison for ordering
    EXPECT_LT(neuronId1, neuronId2);
    EXPECT_FALSE(neuronId2 < neuronId1);
}

// Test SynapseId operations
TEST_F(TypesTest, SynapseIdBasic) {
    EXPECT_TRUE(synapseId1.isValid());
    EXPECT_TRUE(synapseId2.isValid());
    
    // Equality comparison
    EXPECT_EQ(synapseId1, synapseId1);
    EXPECT_NE(synapseId1, synapseId2);
    
    // Less than comparison for ordering
    EXPECT_LT(synapseId1, synapseId2);
    EXPECT_FALSE(synapseId2 < synapseId1);
}

// Test RegionId operations
TEST_F(TypesTest, RegionIdBasic) {
    EXPECT_TRUE(regionId1.isValid());
    EXPECT_TRUE(regionId2.isValid());
    
    // Equality comparison
    EXPECT_EQ(regionId1, regionId1);
    EXPECT_NE(regionId1, regionId2);
    
    // Less than comparison for ordering
    EXPECT_LT(regionId1, regionId2);
    EXPECT_FALSE(regionId2 < regionId1);
}

// Test PopulationId operations
TEST_F(TypesTest, PopulationIdBasic) {
    EXPECT_TRUE(populationId1.isValid());
    EXPECT_TRUE(populationId2.isValid());
    
    // Equality comparison
    EXPECT_EQ(populationId1, populationId1);
    EXPECT_NE(populationId1, populationId2);
    
    // Less than comparison for ordering
    EXPECT_LT(populationId1, populationId2);
    EXPECT_FALSE(populationId2 < populationId1);
}

// Test ID invalid state
TEST_F(TypesTest, InvalidId) {
    nlm::NeuronId invalidNeuron;
    nlm::SynapseId invalidSynapse;
    nlm::RegionId invalidRegion;
    nlm::PopulationId invalidPopulation;
    
    // Invalid IDs should not be valid
    EXPECT_FALSE(invalidNeuron.isValid());
    EXPECT_FALSE(invalidSynapse.isValid());
    EXPECT_FALSE(invalidRegion.isValid());
    EXPECT_FALSE(invalidPopulation.isValid());
}

// Test NeuronType enum
TEST_F(TypesTest, NeuronType) {
    EXPECT_EQ(static_cast<int>(nlm::NeuronType::Internal), 0);
    EXPECT_EQ(static_cast<int>(nlm::NeuronType::Excitatory), 1);
    EXPECT_EQ(static_cast<int>(nlm::NeuronType::Inhibitory), 2);
    
    // Test all enum values
    std::vector<nlm::NeuronType> types = {
        nlm::NeuronType::Internal,
        nlm::NeuronType::Excitatory,
        nlm::NeuronType::Inhibitory
    };
    
    for (auto type : types) {
        // Should be able to create neuron with each type
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setType(type);
        EXPECT_EQ(neuron.getType(), type);
    }
}

// Test SynapseType enum
TEST_F(TypesTest, SynapseType) {
    EXPECT_EQ(static_cast<int>(nlm::SynapseType::Excitatory), 0);
    EXPECT_EQ(static_cast<int>(nlm::SynapseType::Inhibitory), 1);
    
    // Test all enum values
    std::vector<nlm::SynapseType> types = {
        nlm::SynapseType::Excitatory,
        nlm::SynapseType::Inhibitory
    };
    
    for (auto type : types) {
        // Should be able to create synapse with each type
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(type);
        EXPECT_EQ(synapse.getType(), type);
    }
}

// Test FiringState enum
TEST_F(TypesTest, FiringState) {
    EXPECT_EQ(static_cast<int>(nlm::FiringState::Resting), 0);
    EXPECT_EQ(static_cast<int>(nlm::FiringState::Active), 1);
    EXPECT_EQ(static_cast<int>(nlm::FiringState::Refractory), 2);
    
    // Test all enum values
    std::vector<nlm::FiringState> states = {
        nlm::FiringState::Resting,
        nlm::FiringState::Active,
        nlm::FiringState::Refractory
    };
    
    for (auto state : states) {
        // Should be able to set neuron to each firing state
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setFiringState(state);
        EXPECT_EQ(neuron.getState().firingState, state);
    }
}

// Test NeuronType helper functions
TEST_F(TypesTest, NeuronTypeHelpers) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    EXPECT_TRUE(neuron.isExcitatory());
    EXPECT_FALSE(neuron.isInhibitory());
    EXPECT_FALSE(neuron.isInternal());
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    EXPECT_FALSE(neuron.isExcitatory());
    EXPECT_TRUE(neuron.isInhibitory());
    EXPECT_FALSE(neuron.isInternal());
    
    neuron.setType(nlm::NeuronType::Internal);
    EXPECT_FALSE(neuron.isExcitatory());
    EXPECT_FALSE(neuron.isInhibitory());
    EXPECT_TRUE(neuron.isInternal());
}

// Test SynapseType helper functions
TEST_F(TypesTest, SynapseTypeHelpers) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    EXPECT_TRUE(synapse.isExcitatory());
    EXPECT_FALSE(synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    EXPECT_FALSE(synapse.isExcitatory());
    EXPECT_TRUE(synapse.isInhibitory());
}

// Test numeric type compatibility
TEST_F(TypesTest, NumericTypes) {
    // Test float types
    nlm::MembranePotential potential = -70.5f;
    nlm::FiringRate rate = 10.5f;
    float floatVal = 3.14159f;
    
    EXPECT_FLOAT_EQ(potential, -70.5f);
    EXPECT_FLOAT_EQ(rate, 10.5f);
    EXPECT_FLOAT_EQ(floatVal, 3.14159f);
    
    // Test int types
    nlm::NeuronId id1 = NeuronId(1);
    nlm::NeuronId id2 = NeuronId(2);
    nlm::SynapseId synId1 = SynapseId(1);
    nlm::SynapseId synId2 = SynapseId(2);
    
    EXPECT_EQ(id1, id1);
    EXPECT_NE(id1, id2);
    EXPECT_LT(id1, id2);
    
    EXPECT_EQ(synId1, synId1);
    EXPECT_NE(synId1, synId2);
    EXPECT_LT(synId1, synId2);
}

// Test type comparisons across different ID types
TEST_F(TypesTest, CrossTypeComparisons) {
    // IDs of different types should not be directly comparable (compile-time check)
    // This test validates that each ID type is properly encapsulated
    
    // Each ID type should maintain its own equality and ordering
    EXPECT_TRUE(neuronId1 == neuronId1);
    EXPECT_FALSE(neuronId1 == neuronId2);
    
    EXPECT_TRUE(synapseId1 == synapseId1);
    EXPECT_FALSE(synapseId1 == synapseId2);
    
    EXPECT_TRUE(regionId1 == regionId1);
    EXPECT_FALSE(regionId1 == regionId2);
    
    EXPECT_TRUE(populationId1 == populationId1);
    EXPECT_FALSE(populationId1 == populationId2);
}

// Test ID construction and default values
TEST_F(TypesTest, IDConstruction) {
    // Default constructors should create invalid IDs
    nlm::NeuronId defaultNeuronId;
    nlm::SynapseId defaultSynapseId;
    nlm::RegionId defaultRegionId;
    nlm::PopulationId defaultPopulationId;
    
    EXPECT_FALSE(defaultNeuronId.isValid());
    EXPECT_FALSE(defaultSynapseId.isValid());
    EXPECT_FALSE(defaultRegionId.isValid());
    EXPECT_FALSE(defaultPopulationId.isValid());
    
    // Parameterized constructors should create valid IDs
    nlm::NeuronId paramNeuronId(42);
    nlm::SynapseId paramSynapseId(42);
    nlm::RegionId paramRegionId(42);
    nlm::PopulationId paramPopulationId(42);
    
    EXPECT_TRUE(paramNeuronId.isValid());
    EXPECT_TRUE(paramSynapseId.isValid());
    EXPECT_TRUE(paramRegionId.isValid());
    EXPECT_TRUE(paramPopulationId.isValid());
}

// Test ID casting and conversion
TEST_F(TypesTest, IDCasting) {
    // IDs should support implicit conversion to integers for compatibility
    nlm::NeuronId neuronId(42);
    nlm::SynapseId synapseId(42);
    nlm::RegionId regionId(42);
    nlm::PopulationId populationId(42);
    
    // Should be able to compare with integers
    int neuronInt = static_cast<int>(neuronId);
    int synapseInt = static_cast<int>(synapseId);
    int regionInt = static_cast<int>(regionId);
    int populationInt = static_cast<int>(populationId);
    
    EXPECT_EQ(neuronInt, 42);
    EXPECT_EQ(synapseInt, 42);
    EXPECT_EQ(regionInt, 42);
    EXPECT_EQ(populationInt, 42);
}

// Parameterized test for ID generation
class ParameterizedIdTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(10, 0.1f, 0.01f, "Small IDs"),
            TestParameters(100, 0.2f, 0.05f, "Medium IDs"),
            TestParameters(1000, 0.15f, 0.1f, "Large IDs")
        };
    }
};

TEST_P(ParameterizedIdTest, IDGenerationWithDifferentSizes) {
    TestParameters params = GetParam();
    
    std::vector<nlm::NeuronId> neuronIds;
    std::vector<nlm::SynapseId> synapseIds;
    std::vector<nlm::RegionId> regionIds;
    std::vector<nlm::PopulationId> populationIds;
    
    for (int i = 0; i < params.neuronCount; ++i) {
        neuronIds.push_back(NeuronId(i));
        synapseIds.push_back(SynapseId(i));
        regionIds.push_back(RegionId(i));
        populationIds.push_back(PopulationId(i));
    }
    
    // All generated IDs should be valid
    for (const auto& id : neuronIds) {
        EXPECT_TRUE(id.isValid());
    }
    
    for (const auto& id : synapseIds) {
        EXPECT_TRUE(id.isValid());
    }
    
    for (const auto& id : regionIds) {
        EXPECT_TRUE(id.isValid());
    }
    
    for (const auto& id : populationIds) {
        EXPECT_TRUE(id.isValid());
    }
}

INSTANTIATE_TEST_SUITE_P(
    IdParameterizedTests,
    ParameterizedIdTest,
    ::testing::ValuesIn(ParameterizedIdTest::GetTestParameters())
);

// Performance benchmark for ID operations
TEST(IdPerformanceTest, IDCreationPerformance) {
    TestBase testBase;
    
    auto func = []() {
        nlm::NeuronId neuronId(1);
        nlm::SynapseId synapseId(1);
        nlm::RegionId regionId(1);
        nlm::PopulationId populationId(1);
    };
    
    auto result = testBase.runBenchmark(func, 100000);
    
    std::cout << "ID creation performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to create at least 100,000 IDs per second
    EXPECT_GT(result.opsPerMs, 100000.0);
}

// Performance benchmark for ID comparisons
TEST(IdPerformanceTest, IDComparisonPerformance) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    
    TestBase testBase;
    auto func = [&]() {
        bool equal = (id1 == id1);
        bool notEqual = (id1 != id2);
        bool less = (id1 < id2);
    };
    
    auto result = testBase.runBenchmark(func, 1000000);
    
    std::cout << "ID comparison performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests for types
TEST_F(TypesTest, TypeEdgeCases) {
    // Test with ID 0 (zero)
    nlm::NeuronId zeroNeuronId(0);
    EXPECT_TRUE(zeroNeuronId.isValid());
    EXPECT_EQ(zeroNeuronId, NeuronId(0));
    
    // Test with large IDs
    nlm::NeuronId largeNeuronId(std::numeric_limits<uint64_t>::max() / 2);
    EXPECT_TRUE(largeNeuronId.isValid());
    
    // Test ID 0 for other types
    nlm::SynapseId zeroSynapseId(0);
    nlm::RegionId zeroRegionId(0);
    nlm::PopulationId zeroPopulationId(0);
    
    EXPECT_TRUE(zeroSynapseId.isValid());
    EXPECT_TRUE(zeroRegionId.isValid());
    EXPECT_TRUE(zeroPopulationId.isValid());
}

// Test type equality and ordering consistency
TEST_F(TypesTest, TypeConsistency) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(3);
    
    // Transitivity of less than
    EXPECT_TRUE(id1 < id3);
    EXPECT_TRUE(id2 < id3);
    EXPECT_TRUE(id1 < id2);
    
    // Antisymmetry
    EXPECT_FALSE(id2 < id1);
    EXPECT_FALSE(id3 < id2);
    
    // Totality (for same type)
    EXPECT_TRUE(id1 <= id2 || id2 <= id1);
    EXPECT_TRUE(id2 <= id3 || id3 <= id2);
    EXPECT_TRUE(id1 <= id3 || id3 <= id1);
}

// Test type hash consistency
TEST_F(TypesTest, TypeHashConsistency) {
    // IDs should be hashable (compile-time check)
    // This validates that ID types can be used in hash maps
    
    std::unordered_map<nlm::NeuronId, int> neuronMap;
    neuronMap[neuronId1] = 1;
    neuronMap[neuronId2] = 2;
    
    EXPECT_EQ(neuronMap[neuronId1], 1);
    EXPECT_EQ(neuronMap[neuronId2], 2);
    
    std::unordered_map<nlm::SynapseId, int> synapseMap;
    synapseMap[synapseId1] = 1;
    synapseMap[synapseId2] = 2;
    
    EXPECT_EQ(synapseMap[synapseId1], 1);
    EXPECT_EQ(synapseMap[synapseId2], 2);
}

} // namespace test
} // namespace nlm
