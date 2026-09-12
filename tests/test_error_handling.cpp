#include <gtest/gtest.h>
#include "../src/core/Error/Errors.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/plasticity/STDP.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

using namespace nlm;

// Test basic error categories
TEST(ErrorHandlingTest, ErrorCategoryNames) {
    NLMException initError(NLMErrorCategory::Initialization, "Test init error");
    EXPECT_EQ(initError.getCategoryName(), std::string("Initialization"));
    EXPECT_EQ(std->getMessage(), std::string("Test init error"));
}

// Test specific error types
TEST(ErrorHandlingTest, NLMInitializationError) {
    EXPECT_THROW(NLMInitializationError("Null brain pointer"), NLMInitializationError);
}

TEST(ErrorHandlingTest, NLMValidationError) {
    EXPECT_THROW(NLMValidationError("Invalid input"), NLMValidationError);
}

TEST(ErrorHandlingTest, NLMMemoryError) {
    EXPECT_THROW(NLMMemoryError("Capacity exceeded"), NLMMemoryError);
}

TEST(ErrorHandlingTest, NLMNeuralDynamicsError) {
    EXPECT_THROW(NLMNeuralDynamicsError("Synapse not found"), NLMNeuralDynamicsError);
}

TEST(ErrorHandlingTest, NLMPlasticityError) {
    EXPECT_THROW(NLMPlasticityError("Plasticity disabled"), NLMPlasticityError);
}

TEST(ErrorHandlingTest, NLMConfigurationError) {
    EXPECT_THROW(NLMConfigurationError("Invalid config key"), NLMConfigurationError);
}

// Test validation utilities
TEST(ValidationUtilsTest, ValidatePointerNotNull) {
    int value = 42;
    
    EXPECT_NO_THROW(ValidationUtils::validatePointerNotNull(&value, "test operation"));
    
    int* nullPtr = nullptr;
    EXPECT_THROW(ValidationUtils::validatePointerNotNull(nullPtr, "test operation"),
                 NLMValidationError);
}

TEST(ValidationUtilsTest, ValidateRange) {
    EXPECT_NO_THROW(ValidationUtils::validateRange(5, 1, 10, "range test"));
    
    EXPECT_THROW(ValidationUtils::validateRange(15, 1, 10, "range test"),
                 NLMValidationError);
    
    EXPECT_THROW(ValidationUtils::validateRange(0, 1, 10, "range test"),
                 NLMValidationError);
}

TEST(ValidationUtilsTest, ValidateNonNegative) {
    EXPECT_NO_THROW(ValidationUtils::validateNonNegative(0.0f, "non-negative test"));
    EXPECT_NO_THROW(ValidationUtils::validateNonNegative(5.0f, "non-negative test"));
    
    EXPECT_THROW(ValidationUtils::validateNonNegative(-1.0f, "non-negative test"),
                 NLMValidationError);
}

TEST(ValidationUtilsTest, ValidateNotEmpty) {
    std::vector<float> data = {1.0f, 2.0f, 3.0f};
    EXPECT_NO_THROW(ValidationUtils::validateNotEmpty(data, "not empty test"));
    
    std::vector<float> empty;
    EXPECT_THROW(ValidationUtils::validateNotEmpty(empty, "not empty test"),
                 NLMValidationError);
}

TEST(ValidationUtilsTest, ValidateMemoryCapacity) {
    EXPECT_NO_THROW(ValidationUtils::validateMemoryCapacity(50, 100, "memory test"));
    
    EXPECT_THROW(ValidationUtils::validateMemoryCapacity(150, 100, "memory test"),
                 NLMMemoryError);
}

// Test error propagation through components
class TestEnvironment : public ::testing::Test {
protected:
    void SetUp() override {
        // Create simple test environment
        config_ = std::make_shared<Config>();
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

TEST_F(TestEnvironment, AgentBrainInitializationError) {
    EXPECT_THROW(AgentBrain(nullptr), NLMInitializationError);
}

TEST_F(TestEnvironment, AgentBrainValidationErrors) {
    auto agentBrain = std::make_shared<AgentBrain>(brain_);
    
    // Create a mock world object (for testing purposes)
    class MockWorld {
    public:
        size_t getVisionWidth() const { return 10; }
        size_t getVisionHeight() const { return 10; }
    } mockWorld;
    
    // Test with invalid world pointer
    EXPECT_THROW(agentBrain->initialize(nullptr), NLMValidationError);
    
    // Test initialization (should pass with valid world)
    EXPECT_NO_THROW(agentBrain->initialize(mockWorld));
}

TEST_F(TestEnvironment, NeuralWorkingMemoryValidation) {
    auto memory = std::make_shared<NeuralWorkingMemory>();
    
    // Test initialization with null pointer
    EXPECT_THROW(memory->initialize(nullptr), NLMValidationError);
    
    // Test initialization with valid brain pointer
    EXPECT_NO_THROW(memory->initialize(brain_.get()));
}

// Test exception safety
TEST(ExceptionSafetyTest, SafeResourceCleanup) {
    int* resource = new int(42);
    
    {
        ExceptionSafety::SafeResource<int> safeResource(resource, true, "test operation");
        EXPECT_EQ(safeResource.get(), resource);
    }
    
    // Resource should be cleaned up
    EXPECT_TRUE(resource == nullptr);
}

TEST(ExceptionSafetyTest, ExceptionSafeState) {
    ExceptionSafety::ExceptionSafeState<int> safeState(10, "state test");
    
    EXPECT_EQ(safeState.getState(), 10);
    EXPECT_FALSE(safeState.isDirty());
    
    safeState.setState(20);
    EXPECT_EQ(safeState.getState(), 20);
    EXPECT_TRUE(safeState.isDirty());
    
    safeState.markClean();
    EXPECT_FALSE(safeState.isDirty());
}

// Test error handling in plasticity
TEST(PlasticityErrorHandlingTest, STDPValidation) {
    STDP stdp;
    Synapse synapse(SynapseId(1), NeuronId(2), NeuronId(3));
    
    // Test with null synapse
    EXPECT_THROW(stdp.update(nullptr, std::vector<Timestamp>(), std::vector<Timestamp>(), 0.0f),
                 NLMValidationError);
    
    std::vector<Timestamp> preSpikes = {0, 100};
    std::vector<Timestamp> postSpikes = {50, 150};
    
    // Test with empty spike histories
    EXPECT_THROW(stdp.update(&synapse, {}, postSpikes, 0.0f),
                 NLMValidationError);
    
    EXPECT_THROW(stdp.update(&synapse, preSpikes, {}, 0.0f),
                 NLMValidationError);
    
    // Test valid update
    EXPECT_NO_THROW(stdp.update(&synapse, preSpikes, postSpikes, 0.0f));
}

// Test error handling in neuron operations
TEST(NeuronErrorHandlingTest, NeuronValidation) {
    Neuron neuron(NeuronId(1));
    
    // Test null pointer validation in synaptic operations
    // (These would be called internally during operations)
    
    neuron.initializeRandom(*brain_->getRandomGenerator());
    neuron.reset();
}

// Test error handling in memory systems
TEST(MemoryErrorHandlingTest, MemoryCapacityValidation) {
    auto memory = std::make_shared<NeuralWorkingMemory>();
    memory->initialize(brain_.get());
    
    std::vector<float> pattern = {1.0f, 2.0f, 3.0f};
    
    // Should work within capacity
    EXPECT_NO_THROW(memory->store(pattern, 1.0f));
    
    // Test with null brain pointer (after initialization removal)
    auto memory2 = std::make_shared<NeuralWorkingMemory>();
    memory2->brain_ = nullptr;
    EXPECT_NO_THROW(memory2->store(pattern, 1.0f)); // Should silently fail
}

// Test error message formatting
TEST(ErrorFormattingTest, ErrorMessageFormatting) {
    std::string formatted = ValidationUtils::formatError("operation", "details");
    EXPECT_EQ(formatted, "operation: details");
    
    formatted = ValidationUtils::formatError("operation", "");
    EXPECT_EQ(formatted, "operation");
}

// Test that exceptions propagate correctly
TEST(ExceptionPropagationTest, ExceptionPropagation) {
    try {
        throw NLMValidationError("Test error");
    } catch (const NLMValidationError& e) {
        EXPECT_EQ(e.getCategory(), NLMErrorCategory::Validation);
        EXPECT_EQ(std::string(e.what()), "Test error");
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
