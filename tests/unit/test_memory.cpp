// Memory Unit Tests
// Comprehensive test suite for memory systems using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>

namespace nlm {
namespace test {

// Test fixture for memory tests
class MemoryTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test neurons for memory testing
        neuron1 = createTestNeuron(NeuronId(1), NeuronType::Excitatory);
        neuron2 = createTestNeuron(NeuronId(2), NeuronType::Inhibitory);
        neuron3 = createTestNeuron(NeuronId(3), NeuronType::Internal);
    }
    
    void TearDown() override {
        TestBase::TearDown();
    }
    
protected:
    nlm::Neuron neuron1;
    nlm::Neuron neuron2;
    nlm::Neuron neuron3;
};

// Test working memory basic operations
TEST_F(MemoryTest, WorkingMemoryBasic) {
    // Note: WorkingMemory is a placeholder in the current codebase
    // This test validates the interface and basic operations
    
    // Test capacity
    size_t capacity = nlm::Memory::getWorkingMemoryCapacity();
    EXPECT_GT(capacity, 0);
    
    // Test storage
    nlm::NeuronId neuronId = NeuronId(1);
    float value = 0.75f;
    
    nlm::Memory::storeWorkingMemory(neuronId, value);
    float retrieved = nlm::Memory::retrieveWorkingMemory(neuronId);
    
    EXPECT_FLOAT_EQ(retrieved, value);
    
    // Test contains
    EXPECT_TRUE(nlm::Memory::containsWorkingMemory(neuronId));
    
    // Test clear
    nlm::Memory::clearWorkingMemory();
    EXPECT_FALSE(nlm::Memory::containsWorkingMemory(neuronId));
}

// Test episodic memory basic operations
TEST_F(MemoryTest, EpisodicMemoryBasic) {
    // Note: EpisodicMemory is a placeholder in the current codebase
    // This test validates the interface and basic operations
    
    nlm::SimulationStep step = 100;
    nlm::NeuronId neuronId = NeuronId(1);
    float value = 0.85f;
    
    // Store episodic memory
    nlm::Memory::storeEpisodicMemory(step, neuronId, value, "Test event");
    
    // Retrieve episodic memory
    auto episodicData = nlm::Memory::getEpisodicMemory(step);
    EXPECT_FALSE(episodicData.empty());
    
    // Verify content
    bool found = false;
    for (const auto& item : episodicData) {
        if (item.neurons[0] == neuronId && item.values[0] == value && item.metadata == "Test event") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
    
    // Test capacity
    size_t capacity = nlm::Memory::getEpisodicMemoryCapacity();
    EXPECT_GT(capacity, 0);
}

// Test working memory decay
TEST_F(MemoryTest, WorkingMemoryDecay) {
    nlm::NeuronId neuronId = NeuronId(2);
    float initialValue = 1.0f;
    
    nlm::Memory::storeWorkingMemory(neuronId, initialValue);
    
    // Apply decay
    float decayRate = 0.1f; // 10% decay per step
    nlm::Memory::decayWorkingMemory(decayRate);
    
    float decayedValue = nlm::Memory::retrieveWorkingMemory(neuronId);
    EXPECT_LT(decayedValue, initialValue);
    EXPECT_GT(decayedValue, 0.0f);
    
    // Verify decay formula
    float expected = initialValue * std::pow(1.0f - decayRate, 1.0f);
    EXPECT_FLOAT_EQ(decayedValue, expected);
}

// Test working memory capacity
TEST_F(MemoryTest, WorkingMemoryCapacity) {
    // Test with different neuron IDs
    for (int i = 0; i < 10; ++i) {
        nlm::NeuronId neuronId = NeuronId(i);
        float value = static_cast<float>(i) / 10.0f;
        nlm::Memory::storeWorkingMemory(neuronId, value);
        EXPECT_TRUE(nlm::Memory::containsWorkingMemory(neuronId));
    }
    
    // Verify capacity is not exceeded
    size_t currentSize = nlm::Memory::getWorkingMemorySize();
    size_t capacity = nlm::Memory::getWorkingMemoryCapacity();
    EXPECT_LE(currentSize, capacity);
    
    nlm::Memory::clearWorkingMemory();
}

// Test episodic memory storage capacity
TEST_F(MemoryTest, EpisodicMemoryCapacity) {
    nlm::SimulationStep startStep = 1000;
    
    // Store multiple episodic memories
    for (int i = 0; i < 5; ++i) {
        nlm::SimulationStep step = startStep + i;
        nlm::NeuronId neuronId = NeuronId(i);
        float value = static_cast<float>(i) / 5.0f;
        
        nlm::Memory::storeEpisodicMemory(step, neuronId, value, "Test event");
    }
    
    // Retrieve all memories
    for (int i = 0; i < 5; ++i) {
        nlm::SimulationStep step = startStep + i;
        auto episodicData = nlm::Memory::getEpisodicMemory(step);
        EXPECT_FALSE(episodicData.empty());
        EXPECT_EQ(episodicData.size(), 1);
        
        EXPECT_EQ(episodicData[0].neurons[0], NeuronId(i));
        EXPECT_FLOAT_EQ(episodicData[0].values[0], static_cast<float>(i) / 5.0f);
        EXPECT_EQ(episodicData[0].metadata, "Test event");
    }
}

// Test memory consolidation
TEST_F(MemoryTest, MemoryConsolidation) {
    // Store some data
    nlm::NeuronId neuronId = NeuronId(3);
    float initialValue = 0.9f;
    nlm::SimulationStep step = 500;
    
    nlm::Memory::storeWorkingMemory(neuronId, initialValue);
    nlm::Memory::storeEpisodicMemory(step, neuronId, initialValue, "Consolidation test");
    
    // Apply consolidation
    nlm::Memory::consolidateMemory(step);
    
    // Verify that working memory is cleared after consolidation
    EXPECT_FALSE(nlm::Memory::containsWorkingMemory(neuronId));
    
    // Verify that episodic memory still exists
    auto episodicData = nlm::Memory::getEpisodicMemory(step);
    EXPECT_FALSE(episodicData.empty());
}

// Test memory replay
TEST_F(MemoryTest, MemoryReplay) {
    // Store some episodic memory
    nlm::SimulationStep step = 2000;
    nlm::NeuronId neuronId = NeuronId(4);
    float value = 0.7f;
    
    nlm::Memory::storeEpisodicMemory(step, neuronId, value, "Replay test");
    
    // Request replay
    nlm::Memory::replayMemory(step);
    
    // Verify that replay doesn't crash and that original memory persists
    auto episodicData = nlm::Memory::getEpisodicMemory(step);
    EXPECT_FALSE(episodicData.empty());
    EXPECT_EQ(episodicData[0].neurons[0], neuronId);
    EXPECT_FLOAT_EQ(episodicData[0].values[0], value);
}

// Test memory statistics
TEST_F(MemoryTest, MemoryStatistics) {
    nlm::Memory::clearWorkingMemory();
    nlm::Memory::clearEpisodicMemory();
    
    // Store some test data
    nlm::NeuronId neuronId = NeuronId(5);
    float value = 0.6f;
    nlm::SimulationStep step = 3000;
    
    nlm::Memory::storeWorkingMemory(neuronId, value);
    nlm::Memory::storeEpisodicMemory(step, neuronId, value, "Stats test");
    
    // Get statistics
    auto stats = nlm::Memory::getMemoryStats();
    
    EXPECT_GT(stats.workingMemorySize, 0);
    EXPECT_GT(stats.episodicMemorySize, 0);
    EXPECT_GT(stats.totalMemoryUsage, 0);
    EXPECT_LE(stats.workingMemorySize + stats.episodicMemorySize, stats.totalMemoryUsage);
}

// Test memory with neural data
TEST_F(MemoryTest, MemoryWithNeuralData) {
    nlm::NeuronId neuronId = NeuronId(6);
    float neuralActivity = 0.8f;
    
    // Store neural activity in working memory
    nlm::Memory::storeWorkingMemory(neuronId, neuralActivity);
    
    // Retrieve and verify
    float retrievedActivity = nlm::Memory::retrieveWorkingMemory(neuronId);
    EXPECT_FLOAT_EQ(retrievedActivity, neuralActivity);
    
    // Store in episodic memory with timestamp
    nlm::SimulationStep step = 4000;
    nlm::Memory::storeEpisodicMemory(step, neuronId, neuralActivity, "Neural activity");
    
    // Retrieve episodic data
    auto episodicData = nlm::Memory::getEpisodicMemory(step);
    EXPECT_FALSE(episodicData.empty());
    
    // Verify neural data integrity
    EXPECT_EQ(episodicData[0].neurons[0], neuronId);
    EXPECT_FLOAT_EQ(episodicData[0].values[0], neuralActivity);
}

// Parameterized test for memory with different data sizes
class ParameterizedMemoryTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(100, 0.1f, 0.01f, "Large working memory"),
            TestParameters(500, 0.05f, 0.001f, "Very large working memory"),
            TestParameters(1000, 0.02f, 0.0001f, "Massive working memory")
        };
    }
};

TEST_P(ParameterizedMemoryTest, MemoryWithDifferentSizes) {
    TestParameters params = GetParam();
    
    // Store multiple neural memories
    for (int i = 0; i < params.neuronCount; ++i) {
        nlm::NeuronId neuronId = NeuronId(i);
        float value = static_cast<float>(i) / static_cast<float>(params.neuronCount);
        
        nlm::Memory::storeWorkingMemory(neuronId, value);
        
        // Immediately retrieve and verify
        float retrieved = nlm::Memory::retrieveWorkingMemory(neuronId);
        EXPECT_FLOAT_EQ(retrieved, value);
        
        // Store in episodic memory
        nlm::SimulationStep step = 10000 + i;
        nlm::Memory::storeEpisodicMemory(step, neuronId, value, "Parameterized test");
    }
    
    // Verify all stored
    EXPECT_EQ(nlm::Memory::getWorkingMemorySize(), static_cast<size_t>(params.neuronCount));
    
    nlm::Memory::clearWorkingMemory();
}

INSTANTIATE_TEST_SUITE_P(
    MemoryParameterizedTests,
    ParameterizedMemoryTest,
    ::testing::ValuesIn(ParameterizedMemoryTest::GetTestParameters())
);

// Performance benchmark for working memory operations
TEST(MemoryPerformanceTest, WorkingMemoryPerformance) {
    TestBase testBase;
    
    auto func = []() {
        nlm::NeuronId neuronId = NeuronId(1);
        float value = 0.5f;
        nlm::Memory::storeWorkingMemory(neuronId, value);
        float retrieved = nlm::Memory::retrieveWorkingMemory(neuronId);
        EXPECT_FLOAT_EQ(retrieved, value);
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "Working memory performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to perform at least 50,000 memory operations per second
    EXPECT_GT(result.opsPerMs, 50000.0);
}

// Performance benchmark for episodic memory operations
TEST(MemoryPerformanceTest, EpisodicMemoryPerformance) {
    TestBase testBase;
    
    auto func = []() {
        nlm::SimulationStep step = 1000;
        nlm::NeuronId neuronId = NeuronId(2);
        float value = 0.5f;
        nlm::Memory::storeEpisodicMemory(step, neuronId, value, "Benchmark");
    };
    
    auto result = testBase.runBenchmark(func, 5000);
    
    std::cout << "Episodic memory performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests for memory
TEST_F(MemoryTest, MemoryEdgeCases) {
    nlm::NeuronId neuronId = NeuronId(7);
    
    // Test with extreme values
    nlm::Memory::storeWorkingMemory(neuronId, 1.0f);
    EXPECT_FLOAT_EQ(nlm::Memory::retrieveWorkingMemory(neuronId), 1.0f);
    
    nlm::Memory::storeWorkingMemory(neuronId, 0.0f);
    EXPECT_FLOAT_EQ(nlm::Memory::retrieveWorkingMemory(neuronId), 0.0f);
    
    nlm::Memory::storeWorkingMemory(neuronId, -1.0f);
    EXPECT_FLOAT_EQ(nlm::Memory::retrieveWorkingMemory(neuronId), -1.0f);
    
    // Test with extreme timestamps
    nlm::SimulationStep extremeStep = 0;
    nlm::Memory::storeEpisodicMemory(extremeStep, neuronId, 0.5f, "Zero timestamp");
    
    extremeStep = 999999999;
    nlm::Memory::storeEpisodicMemory(extremeStep, neuronId, 0.5f, "Max timestamp");
    
    nlm::Memory::clearWorkingMemory();
}

// Test memory validation
TEST_F(MemoryTest, MemoryValidation) {
    nlm::NeuronId neuronId = NeuronId(8);
    float value = 0.3f;
    
    nlm::Memory::storeWorkingMemory(neuronId, value);
    
    // Get memory statistics
    auto stats = nlm::Memory::getMemoryStats();
    
    // Verify statistics are valid
    EXPECT_GE(stats.workingMemorySize, 0);
    EXPECT_GE(stats.episodicMemorySize, 0);
    EXPECT_GE(stats.totalMemoryUsage, 0);
    EXPECT_LE(stats.workingMemorySize + stats.episodicMemorySize, stats.totalMemoryUsage);
    
    nlm::Memory::clearWorkingMemory();
}

// Test memory with multiple neurons
TEST_F(MemoryTest, MemoryWithMultipleNeurons) {
    std::vector<nlm::NeuronId> neuronIds;
    std::vector<float> values;
    
    // Store data for multiple neurons
    for (int i = 0; i < 20; ++i) {
        nlm::NeuronId neuronId = NeuronId(i);
        float value = static_cast<float>(i) / 20.0f;
        
        neuronIds.push_back(neuronId);
        values.push_back(value);
        
        nlm::Memory::storeWorkingMemory(neuronId, value);
    }
    
    // Retrieve and verify all data
    for (size_t i = 0; i < neuronIds.size(); ++i) {
        float retrieved = nlm::Memory::retrieveWorkingMemory(neuronIds[i]);
        EXPECT_FLOAT_EQ(retrieved, values[i]);
    }
    
    // Store episodic memories
    nlm::SimulationStep step = 5000;
    for (size_t i = 0; i < neuronIds.size(); ++i) {
        nlm::Memory::storeEpisodicMemory(step + i, neuronIds[i], values[i], "Multi-neuron test");
    }
    
    // Verify episodic memories
    for (size_t i = 0; i < neuronIds.size(); ++i) {
        auto episodicData = nlm::Memory::getEpisodicMemory(step + i);
        EXPECT_FALSE(episodicData.empty());
        EXPECT_EQ(episodicData[0].neurons[0], neuronIds[i]);
        EXPECT_FLOAT_EQ(episodicData[0].values[0], values[i]);
    }
    
    nlm::Memory::clearWorkingMemory();
    nlm::Memory::clearEpisodicMemory();
}

} // namespace test
} // namespace nlm
