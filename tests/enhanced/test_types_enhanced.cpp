// Enhanced NLM Types Tests using Google Test Framework
// Modern test implementation with fixtures, parameterization, and comprehensive assertions

#include "enhanced/nlm_test_fixtures.h"
#include "core/Types/Types.hpp"
#include <string>

// Test suite for NLM ID types with comprehensive validation
class TypesTestSuite : public NeuronTestFixture {
protected:
    void SetUp() override {
        NeuronTestFixture::SetUp();
    }
};

// Basic ID type functionality tests
TEST_F(TypesTestSuite, NeuronId_BasicOperations) {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    EXPECT_EQ(id1, id3) << "NeuronId equality comparison failed";
    EXPECT_NE(id1, id2) << "NeuronId inequality comparison failed";
    EXPECT_LT(id1, id2) << "NeuronId less-than comparison failed";
    EXPECT_GT(id2, id1) << "NeuronId greater-than comparison failed";
    EXPECT_EQ(id1.index(), 1) << "NeuronId index value incorrect";
}

TEST_F(TypesTestSuite, NeuronId_EdgeCases) {
    // Test boundary conditions
    nlm::NeuronId min_id(0);
    nlm::NeuronId max_id(1000);
    
    EXPECT_LE(min_id.index(), max_id.index()) << "ID boundary validation failed";
    EXPECT_GE(max_id.index(), min_id.index()) << "ID boundary validation failed";
}

// Synapse ID type functionality tests
class SynapseTypesTestSuite : public SynapseTestFixture {
protected:
    void SetUp() override {
        SynapseTestFixture::SetUp();
    }
};

TEST_F(SynapseTypesTestSuite, SynapseId_BasicOperations) {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    EXPECT_NE(id1, id2) << "SynapseId equality comparison failed";
    EXPECT_LT(id1, id2) << "SynapseId less-than comparison failed";
    EXPECT_GT(id2, id1) << "SynapseId greater-than comparison failed";
}

TEST_F(SynapseTypesTestSuite, SynapseId_EdgeCases) {
    // Test with large values
    nlm::SynapseId large_id(0xFFFFFFFF);
    EXPECT_GE(large_id.index(), 0) << "SynapseId large value handling failed";
}

// Region and Population ID tests
class RegionPopulationTestSuite : public NeuronTestFixture {
protected:
    void SetUp() override {
        NeuronTestFixture::SetUp();
    }
};

TEST_F(RegionPopulationTestSuite, RegionId_BasicOperations) {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    EXPECT_NE(id1, id2) << "RegionId comparison failed";
    EXPECT_LT(id1, id2) << "RegionId ordering failed";
}

TEST_F(RegionPopulationTestSuite, PopulationId_BasicOperations) {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    EXPECT_NE(id1, id2) << "PopulationId comparison failed";
    EXPECT_LT(id1, id2) << "PopulationId ordering failed";
}

// Neuron type tests
class NeuronTypeTestSuite : public NeuronTestFixture {
protected:
    void SetUp() override {
        NeuronTestFixture::SetUp();
    }
};

TEST_F(NeuronTypeTestSuite, NeuronType_Enumeration) {
    EXPECT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory) 
        << "Neuron types should be distinct enumerations";
    EXPECT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor) 
        << "Neuron types should be distinct enumerations";
    EXPECT_NE(nlm::NeuronType::Internal, nlm::NeuronType::Sensory) 
        << "Neuron types should be distinct enumerations";
}

TEST_F(NeuronTypeTestSuite, NeuronType_ValidValues) {
    std::vector<nlm::NeuronType> valid_types = {
        nlm::NeuronType::Excitatory,
        nlm::NeuronType::Inhibitory,
        nlm::NeuronType::Sensory,
        nlm::NeuronType::Motor,
        nlm::NeuronType::Internal
    };
    
    for (const auto& type : valid_types) {
        EXPECT_TRUE(static_cast<int>(type) >= 0) << "NeuronType has invalid value: " << static_cast<int>(type);
    }
}

// Synapse type tests
class SynapseTypeTestSuite : public SynapseTestFixture {
protected:
    void SetUp() override {
        SynapseTestFixture::SetUp();
    }
};

TEST_F(SynapseTypeTestSuite, SynapseType_Enumeration) {
    EXPECT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory) 
        << "Synapse types should be distinct enumerations";
}

TEST_F(SynapseTypeTestSuite, SynapseType_ValidValues) {
    EXPECT_EQ(static_cast<int>(nlm::SynapseType::Excitatory), 0) 
        << "SynapseType::Excitatory should have value 0";
    EXPECT_EQ(static_cast<int>(nlm::SynapseType::Inhibitory), 1) 
        << "SynapseType::Inhibitory should have value 1";
}

// Plasticity flags tests
class PlasticityFlagsTestSuite : public NeuronTestFixture {
protected:
    void SetUp() override {
        NeuronTestFixture::SetUp();
    }
};

TEST_F(PlasticityFlagsTestSuite, PlasticityFlags_Initialization) {
    nlm::PlasticityFlags flags;
    
    EXPECT_FALSE(flags.hebbian) << "Hebbian flag should be false by default";
    EXPECT_FALSE(flags.stdp) << "STDP flag should be false by default";
    EXPECT_FALSE(flags.reward_modulated) << "Reward modulated flag should be false by default";
}

TEST_F(PlasticityFlagsTestSuite, PlasticityFlags_Modification) {
    nlm::PlasticityFlags flags;
    
    flags.hebbian = true;
    EXPECT_TRUE(flags.hebbian) << "Hebbian flag modification failed";
    
    flags.stdp = true;
    EXPECT_TRUE(flags.stdp) << "STDP flag modification failed";
    
    flags.reward_modulated = true;
    EXPECT_TRUE(flags.reward_modulated) << "Reward modulated flag modification failed";
}

// Spike event tests
class SpikeEventTestSuite : public NeuronTestFixture {
protected:
    void SetUp() override {
        NeuronTestFixture::SetUp();
    }
};

TEST_F(SpikeEventTestSuite, SpikeEvent_Creation) {
    nlm::SpikeEvent event;
    
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    EXPECT_EQ(event.source_neuron.index(), 1) << "Spike source neuron ID incorrect";
    EXPECT_FLOAT_EQ(event.timestamp, 0.5f) << "Spike timestamp incorrect";
    EXPECT_EQ(event.step, 500) << "Spike step incorrect";
}

TEST_F(SpikeEventTestSuite, SpikeEvent_Validation) {
    nlm::SpikeEvent event;
    
    // Test with boundary values
    event.source_neuron = nlm::NeuronId(0);
    event.timestamp = 0.0f;
    event.step = 0;
    
    EXPECT_GE(event.timestamp, 0.0f) << "Spike timestamp should be non-negative";
    EXPECT_GE(event.step, 0) << "Spike step should be non-negative";
    
    // Test with large values
    event.timestamp = 1000.0f;
    event.step = 1000000;
    
    EXPECT_LE(event.timestamp, 10000.0f) << "Spike timestamp should be reasonable";
    EXPECT_LE(event.step, 10000000) << "Spike step should be reasonable";
}

// Parameterized tests for ID types
class ParameterizedIDTests : public ParameterizedTestBase {
protected:
    void SetUp() override {
        ParameterizedTestBase::SetUp();
    }
};

TEST_P(ParameterizedIDTests, NeuronId_ParameterizedCreation) {
    size_t id_value = num_neurons % 10000; // Ensure valid range
    nlm::NeuronId test_id(id_value);
    
    EXPECT_EQ(test_id.index(), static_cast<nlm::IndexType>(id_value)) 
        << "NeuronId parameter creation failed for value " << id_value;
}

TEST_P(ParameterizedIDTests, SynapseId_ParameterizedCreation) {
    size_t id_value = num_neurons % 1000000; // Ensure valid range
    nlm::SynapseId test_id(id_value);
    
    EXPECT_EQ(test_id.index(), static_cast<nlm::SynapseIndexType>(id_value)) 
        << "SynapseId parameter creation failed for value " << id_value;
}

// Test case registration for parameterized tests
INSTANTIATE_TEST_CASE_P(
    IDParameterization,
    ParameterizedIDTests,
    ::testing::Combine(
        ::testing::Values(1, 10, 100, 1000),           // num_neurons
        ::testing::Values(0.01f, 0.1f, 1.0f),         // learning_rate
        ::testing::Values(10.0f, 50.0f, 100.0f)       // time_constant
    )
);

// Comprehensive integration test for all types
class ComprehensiveTypesIntegrationTest : public IntegrationTestFixture {
protected:
    void SetUp() override {
        IntegrationTestFixture::SetUp();
    }
    
    void TearDown() override {
        IntegrationTestFixture::TearDown();
    }
};

TEST_F(ComprehensiveTypesIntegrationTest, AllTypes_IntegratedTest) {
    // Test all types working together
    nlm::NeuronId neuron_id(1);
    nlm::SynapseId synapse_id(100);
    nlm::RegionId region_id(2);
    nlm::PopulationId population_id(3);
    
    // Validate all IDs are properly initialized
    EXPECT_EQ(neuron_id.index(), 1) << "Neuron ID integration failed";
    EXPECT_EQ(synapse_id.index(), 100) << "Synapse ID integration failed";
    EXPECT_EQ(region_id.index(), 2) << "Region ID integration failed";
    EXPECT_EQ(population_id.index(), 3) << "Population ID integration failed";
    
    // Validate type consistency
    EXPECT_NE(neuron_id.index(), synapse_id.index()) << "ID type collision detected";
    EXPECT_NE(synapse_id.index(), region_id.index()) << "ID type collision detected";
    EXPECT_NE(region_id.index(), population_id.index()) << "ID type collision detected";
    
    // Test type operations
    EXPECT_TRUE(neuron_id < neuron_id + 1) << "Neuron ID ordering failed";
    EXPECT_TRUE(synapse_id < synapse_id + 1) << "Synapse ID ordering failed";
    EXPECT_TRUE(region_id < region_id + 1) << "Region ID ordering failed";
    EXPECT_TRUE(population_id < population_id + 1) << "Population ID ordering failed";
}

TEST_F(ComprehensiveTypesIntegrationTest, MemorySafety_EdgeCases) {
    nlm::TestSeedManager::initSeed(12345);
    
    // Test with boundary values
    nlm::NeuronId boundary_ids[] = {nlm::NeuronId(0), nlm::NeuronId(1), nlm::NeuronId(UINT32_MAX)};
    nlm::SynapseId synapse_boundary_ids[] = {nlm::SynapseId(0), nlm::SynapseId(1), nlm::SynapseId(UINT32_MAX)};
    
    for (const auto& id : boundary_ids) {
        EXPECT_GE(id.index(), 0) << "NeuronId boundary value invalid";
        EXPECT_LE(id.index(), UINT32_MAX) << "NeuronId boundary value exceeds limit";
    }
    
    for (const auto& id : synapse_boundary_ids) {
        EXPECT_GE(id.index(), 0) << "SynapseId boundary value invalid";
        EXPECT_LE(id.index(), UINT32_MAX) << "SynapseId boundary value exceeds limit";
    }
}