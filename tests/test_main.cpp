#pragma once

// Unit Tests for NLM (熙然) Neural Learning Machine
// Phase 6: Final Integration - Comprehensive Unit Testing Framework

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cmath>
#include "core/Config/Config.hpp"
#include "core/ErrorHandling.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrainFactory.hpp"
#include "agent/AdvancedAgentConfiguration.hpp"
#include "performance/PerformanceOptimization.hpp"

namespace nlm {
namespace tests {

// ===== BRAINCORE UNIT TESTS =====
class BrainCoreTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
        brainCore_ = std::make_unique<BrainCore>(config_);
    }
    
    std::shared_ptr<Config> config_;
    std::unique_ptr<BrainCore> brainCore_;
};

TEST_F(BrainCoreTest, ConstructorInitializesComponents) {
    EXPECT_NE(brainCore_, nullptr);
    EXPECT_NE(brainCore_->getRandomGenerator(), nullptr);
    EXPECT_NE(brainCore_->getSpikeSystem(), nullptr);
    EXPECT_NE(brainCore_->getSTDP(), nullptr);
    EXPECT_NE(brainCore_->getHebbian(), nullptr);
    EXPECT_NE(brainCore_->getStructuralPlasticity(), nullptr);
    EXPECT_NE(brainCore_->getWorkingMemory(), nullptr);
    EXPECT_NE(brainCore_->getEpisodicMemory(), nullptr);
    EXPECT_NE(brainCore_->getAssociativeMemory(), nullptr);
    EXPECT_NE(brainCore_->getPredictionSystem(), nullptr);
    EXPECT_NE(brainCore_->getPlanner(), nullptr);
    EXPECT_NE(brainCore_->getConceptFormation(), nullptr);
    EXPECT_NE(brainCore_->getAttention(), nullptr);
    EXPECT_NE(brainCore_->getDevelopmentSystem(), nullptr);
    EXPECT_NE(brainCore_->getDopamine(), nullptr);
    EXPECT_NE(brainCore_->getCuriosity(), nullptr);
    EXPECT_NE(brainCore_->getNovelty(), nullptr);
    EXPECT_NE(brainCore_->getPredictionErrorSignal(), nullptr);
    EXPECT_NE(brainCore_->getCheckpointManager(), nullptr);
}

TEST_F(BrainCoreTest, ConfigValidation) {
    // Add test for configuration validation
    config_->set("simulation_timestep", 0.001);
    config_->set("neuron_count", 100);
    config_->set("region_count", 1);
    config_->set("connection_probability", 0.1f);
    
    // This would test the ConfigValidator
    ValidationResult result = ConfigValidator::validate(config_);
    EXPECT_TRUE(result.isValid());
}

TEST_F(BrainCoreTest, RandomGeneratorInitialization) {
    uint64_t seed = config_->getOr<int64_t>("random_seed", 42);
    EXPECT_EQ(brainCore_->getRandomGenerator()->getSeed(), seed);
}

// ===== AGENT BRAIN FACTORY UNIT TESTS =====
class AgentBrainFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
        brain_ = std::make_shared<Brain>(config_);
        
        // Initialize brain with minimal configuration
        config_->set("simulation_timestep", 0.001);
        config_->set("neuron_count", 50);
        config_->set("region_count", 1);
        config_->set("connection_probability", 0.1f);
        
        brain_->initialize();
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

TEST_F(AgentBrainFactoryTest, DefaultAgentCreation) {
    SimpleWorld world(32, 32);
    auto agent = AgentBrainFactory::createAgent(brain_, world, AgentProfile::Default);
    
    EXPECT_NE(agent, nullptr);
    EXPECT_EQ(agent->getSensoryInputSize(), 256 + 8 + 4 + 6);  // Vision + touch + internal + proprioception
    EXPECT_EQ(agent->getMotorOutputSize(), 6);  // 6 action types
}

TEST_F(AgentBrainFactoryTest, CustomAgentCreation) {
    SimpleWorld world(32, 32);
    
    // Create custom configuration
    AgentBrainFactory::AgentConfig config;
    config.neuromodulation.enableRewardModulation = false;
    config.neuromodulation.enableCuriosity = true;
    config.behavioral.actionSelection.explorationRate = 0.3f;
    
    auto agent = AgentBrainFactory::createCustomAgent(brain_, world, config);
    
    EXPECT_NE(agent, nullptr);
    EXPECT_FALSE(agent->isRewardModulationEnabled());
    EXPECT_TRUE(agent->isCuriosityEnabled());
}

TEST_F(AgentBrainFactoryTest, ProfileConfigs) {
    auto configs = AgentBrainFactory::getAvailableProfiles();
    EXPECT_FALSE(configs.empty());
    
    for (const auto& profile : configs) {
        auto config = AgentBrainFactory::getProfileConfig(AgentProfileFromString(profile));
        EXPECT_TRUE(AgentBrainFactory::validateConfig(config, std::string()));
    }
}

// ===== ADVANCED AGENT CONFIGURATION UNIT TESTS =====
class AdvancedAgentConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = AdvancedAgentConfig();
    }
    
    AdvancedAgentConfig config_;
};

TEST_F(AdvancedAgentConfigTest, DefaultConfiguration) {
    EXPECT_EQ(config_.neural.architecture.inputNeurons, 256);
    EXPECT_EQ(config_.neural.architecture.hiddenNeurons, 512);
    EXPECT_EQ(config_.neural.architecture.outputNeurons, 6);
    EXPECT_EQ(config_.neural.architecture.numLayers, 3);
}

TEST_F(AdvancedAgentConfigTest, ConfigurationValidation) {
    // Test with invalid configuration
    AdvancedAgentConfig invalidConfig;
    invalidConfig.neural.architecture.inputNeurons = 0;  // Invalid
    
    std::string errors = invalidConfig.getValidationErrors();
    EXPECT_FALSE(errors.empty());
}

TEST_F(AdvancedAgentConfigTest, ConfigurationCopy) {
    AdvancedAgentConfig config2 = config_;
    
    EXPECT_EQ(config2.neural.architecture.inputNeurons, config_.neural.architecture.inputNeurons);
    EXPECT_EQ(config2.behavioral.actionSelection.explorationRate, config_.behavioral.actionSelection.explorationRate);
}

TEST_F(AdvancedAgentConfigTest, SpecializationConfigs) {
    auto specializations = AdvancedAgentFactory::getBuiltInSpecializations();
    EXPECT_FALSE(specializations.empty());
    
    for (const auto& specialization : specializations) {
        EXPECT_FALSE(specialization.second.getValidationErrors().empty());
    }
}

// ===== ERROR HANDLING UNIT TESTS =====
class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for error handling tests
    }
};

TEST(ErrorHandlingTest, NLMErrorCreation) {
    NLMError error(NLMError::ErrorCode::INVALID_CONFIG, "Test error message", "test_context");
    EXPECT_EQ(error.getErrorCode(), NLMError::ErrorCode::INVALID_CONFIG);
    EXPECT_NE(std::string(error.what()).find("INVALID_CONFIG"), std::string::npos);
    EXPECT_NE(std::string(error.what()).find("test_context"), std::string::npos);
}

TEST(ErrorHandlingTest, ErrorCodeToString) {
    EXPECT_EQ(NLMError::errorCodeToString(NLMError::ErrorCode::SUCCESS), "SUCCESS");
    EXPECT_EQ(NLMError::errorCodeToString(NLMError::ErrorCode::INVALID_CONFIG), "INVALID_CONFIG");
    EXPECT_EQ(NLMError::errorCodeToString(NLMError::ErrorCode::MEMORY_ALLOCATION_FAILED), "MEMORY_ALLOCATION_FAILED");
}

// ===== PERFORMANCE OPTIMIZATION UNIT TESTS =====
class PerformanceOptimizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for performance optimization tests
    }
};

TEST(PerformanceOptimizationTest, SIMDUtilsDotProduct) {
    std::vector<float> a = {1.0f, 2.0f, 3.0f, 4.0f};
    std::vector<float> b = {2.0f, 3.0f, 4.0f, 5.0f};
    
    float result = SIMDUtils::dotProduct(a, b);
    float expected = 1.0f*2.0f + 2.0f*3.0f + 3.0f*4.0f + 4.0f*5.0f;
    EXPECT_NEAR(result, expected, 0.001f);
}

TEST(PerformanceOptimizationTest, SIMDUtilsMatrixVectorMultiply) {
    std::vector<std::vector<float>> matrix = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f}
    };
    std::vector<float> vector = {1.0f, 2.0f, 3.0f};
    
    auto result = SIMDUtils::matrixVectorMultiply(matrix, vector);
    
    EXPECT_EQ(result.size(), 2);
    EXPECT_NEAR(result[0], 1.0f*1.0f + 2.0f*2.0f + 3.0f*3.0f, 0.001f);
    EXPECT_NEAR(result[1], 4.0f*1.0f + 5.0f*2.0f + 6.0f*3.0f, 0.001f);
}

TEST(PerformanceOptimizationTest, SIMDUtilsReLU) {
    std::vector<float> input = {-1.0f, 0.0f, 2.0f, -3.0f};
    auto result = SIMDUtils::relu(input);
    
    EXPECT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], 0.0f);
    EXPECT_EQ(result[1], 0.0f);
    EXPECT_EQ(result[2], 2.0f);
    EXPECT_EQ(result[3], 0.0f);
}

TEST(PerformanceOptimizationTest, MemoryPoolBasic) {
    MemoryPool pool(1024, 100);
    
    // Allocate some memory
    void* ptr1 = pool.allocate();
    void* ptr2 = pool.allocate();
    
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    
    EXPECT_EQ(pool.allocatedBlocks(), 2);
    EXPECT_GT(pool.memoryUsage(), 0);
}

TEST(PerformanceOptimizationTest, CacheOptimizerTranspose) {
    std::vector<std::vector<float>> matrix = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    };
    
    auto transposed = CacheOptimizer::transposeMatrix(matrix);
    
    EXPECT_EQ(transposed.size(), 3);  // 3 columns
    EXPECT_EQ(transposed[0].size(), 3);  // 3 rows
    EXPECT_EQ(transposed[0][0], 1.0f);
    EXPECT_EQ(transposed[0][1], 4.0f);
    EXPECT_EQ(transposed[0][2], 7.0f);
}

TEST(PerformanceOptimizationTest, ParallelProcessorSimple) {
    std::vector<std::vector<float>> matrix = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    };
    std::vector<float> vector = {1.0f, 1.0f, 1.0f};
    
    auto result = ParallelProcessor::parallelMatrixVectorMultiply(matrix, vector, 2);
    
    EXPECT_EQ(result.size(), 3);
    EXPECT_NEAR(result[0], 6.0f, 0.001f);
    EXPECT_NEAR(result[1], 15.0f, 0.001f);
    EXPECT_NEAR(result[2], 24.0f, 0.001f);
}

// ===== INTEGRATION TESTS =====
class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
        brain_ = std::make_shared<Brain>(config_);
        
        // Configure brain
        config_->set("simulation_timestep", 0.001);
        config_->set("neuron_count", 100);
        config_->set("region_count", 1);
        config_->set("connection_probability", 0.1f);
        
        brain_->initialize();
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

TEST_F(IntegrationTest, BrainInitialization) {
    EXPECT_TRUE(brain_->initialize());
    EXPECT_GT(brain_->getTotalNeuronCount(), 0);
    EXPECT_GE(brain_->getTotalSynapseCount(), 0);
}

TEST_F(IntegrationTest, BrainStep) {
    size_t initialSpikes = brain_->getTotalSpikeCount();
    
    brain_->step(100, 0.1);
    
    EXPECT_LE(brain_->getTotalSpikeCount(), initialSpikes + 100);  // At most 100 new spikes
}

TEST_F(IntegrationTest, AgentIntegration) {
    SimpleWorld world(32, 32);
    auto agent = AgentBrainFactory::createAgent(brain_, world, AgentProfile::Default);
    
    EXPECT_NE(agent, nullptr);
    EXPECT_NE(agent->getBrain(), nullptr);
    
    // Test sensory processing
    SensoryPercept percept;
    for (size_t i = 0; i < 256; ++i) percept.addVision(i * 0.01f);
    
    agent->processSensoryInput(percept);
    
    // Test action decoding
    MotorCommand action = agent->decodeMotorCommand();
    EXPECT_TRUE(static_cast<int>(action) >= 0 && static_cast<int>(action) < 8);
}

TEST_F(IntegrationTest, ErrorHandlingIntegration) {
    // Test that error handling works with brain operations
    ValidationResult result = ValidationCoordinator::validateBrain(brain_.get(), "integration_test");
    EXPECT_TRUE(result.isValid());
}

// ===== PERFORMANCE INTEGRATION TESTS =====
class PerformanceIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
        brain_ = std::make_shared<Brain>(config_);
        
        config_->set("simulation_timestep", 0.001);
        config_->set("neuron_count", 200);
        config_->set("region_count", 1);
        config_->set("connection_probability", 0.1f);
        
        brain_->initialize();
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

TEST_F(PerformanceIntegrationTest, PerformanceMetricsCollection) {
    PerformanceMonitor monitor;
    
    monitor.startMeasurement("test_operation");
    brain_->step(10, 0.01);
    monitor.endMeasurement("test_operation");
    
    PerformanceMetrics metrics = monitor.getMetrics();
    EXPECT_GT(metrics.wallTime, 0.0);
}

TEST_F(PerformanceIntegrationTest, SIMDOperations) {
    std::vector<float> a = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::vector<float> b = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    
    float dotProduct = SIMDUtils::dotProduct(a, b);
    float expected = 1.0f*2.0f + 2.0f*3.0f + 3.0f*4.0f + 4.0f*5.0f + 5.0f*6.0f;
    EXPECT_NEAR(dotProduct, expected, 0.001f);
}

TEST_F(PerformanceIntegrationTest, MemoryPoolPerformance) {
    MemoryPool pool(1024, 1000);
    
    // Allocate many blocks
    std::vector<void*> blocks;
    for (size_t i = 0; i < 500; ++i) {
        blocks.push_back(pool.allocate());
    }
    
    EXPECT_EQ(pool.allocatedBlocks(), 500);
    
    // Free half of them
    for (size_t i = 0; i < 250; ++i) {
        pool.free(blocks[i]);
    }
    
    EXPECT_EQ(pool.allocatedBlocks(), 250);
}

// ===== TEST SUITE =====
class NLMFullTestSuite : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize global components
    }
    
    void TearDown() override {
        // Clean up global components
    }
};

TEST_F(NLMFullTestSuite, FullIntegrationTest) {
    // Run a comprehensive integration test
    config_ = std::make_shared<Config>();
    brain_ = std::make_shared<Brain>(config_);
    
    config_->set("simulation_timestep", 0.001);
    config_->set("neuron_count", 150);
    config_->set("region_count", 1);
    config_->set("connection_probability", 0.1f);
    
    brain_->initialize();
    
    // Test brain operations
    EXPECT_TRUE(brain_->initialize());
    EXPECT_GT(brain_->getTotalNeuronCount(), 0);
    
    // Test agent creation
    SimpleWorld world(32, 32);
    auto agent = AgentBrainFactory::createAgent(brain_, world, AgentProfile::Default);
    EXPECT_NE(agent, nullptr);
    
    // Test error handling
    ValidationResult validationResult = ValidationCoordinator::validateBrain(brain_.get(), "full_test");
    EXPECT_TRUE(validationResult.isValid());
    
    // Test performance
    PerformanceMonitor monitor;
    monitor.startMeasurement("full_integration_test");
    brain_->step(50, 0.05);
    monitor.endMeasurement("full_integration_test");
    
    PerformanceMetrics metrics = monitor.getMetrics();
    EXPECT_GT(metrics.wallTime, 0.0);
    
    // All tests passed
    SUCCEED << "Full integration test passed successfully";
}

} // namespace tests
} // namespace nlm

// Entry point for unit tests
int main(int argc, char** argv) {
    std::cout << "NLM Unit Test Suite - Phase 6 Final Integration" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    
    int result = RUN_ALL_TESTS();
    
    if (result == 0) {
        std::cout << std::endl;
        std::cout << "✓ All unit tests passed successfully!" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Performance optimizations and new functionality verified." << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "✗ Some unit tests failed." << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "Please review the failed tests above." << std::endl;
    }
    
    return result;
}

(End of file - total 1200 lines)