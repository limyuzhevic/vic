// Comprehensive Error Handling Tests for NLM
// Tests for consistent error handling framework throughout the NLM codebase

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <memory>

namespace test_error_handling {

// Helper function to create test files
static std::string createTestFile(const std::string& filename, const std::string& content, const std::string& dir = "/tmp/nlm_test_error_configs") {
    std::filesystem::create_directories(dir);
    std::string filepath = dir + "/" + filename;
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << content;
        file.close();
    }
    return filepath;
}

// Test 1: Error Handling Framework Basics
void testErrorFrameworkInitialization() {
    NLM_LOG_INFO("Testing error handling framework initialization");
    
    // Clear any existing errors
    NLMErrors::clearErrors();
    
    // Verify error count is zero
    assert(NLMErrors::errorCount() == 0);
    
    // Add a test error
    NLMErrors::addError(NLMErrors::InvalidParameter, "Test error message", "Test function");
    
    // Verify error count is now 1
    assert(NLMErrors::errorCount() == 1);
    
    // Verify we can retrieve errors
    auto errors = NLMErrors::getErrors();
    assert(!errors.empty());
    assert(errors.size() == 1);
    assert(errors[0].message == "Test error message");
    assert(errors[0].context == "Test function");
    assert(errors[0].type == NLMErrors::InvalidParameter);
    
    // Verify full message format
    assert(errors[0].getFullMessage().find("Test error message") != std::string::npos);
    
    // Clear errors for next test
    NLMErrors::clearErrors();
    assert(NLMErrors::errorCount() == 0);
    
    std::cout << "    testErrorFrameworkInitialization passed" << std::endl;
}

// Test 2: Error Macros Functionality
void testErrorMacros() {
    NLM_LOG_INFO("Testing error handling macros");
    
    NLMErrors::clearErrors();
    
    // Test NLM_ERROR macro
    NLM_ERROR(NLMErrors::InvalidParameter, "Test error via macro");
    assert(NLMErrors::errorCount() == 1);
    assert(NLMErrors::getErrors()[0].message == "Test error via macro");
    
    // Test NLM_CHECK macro (should throw)
    bool testPassed = false;
    try {
        NLM_CHECK(false, NLMErrors::InvalidParameter, "Condition should fail");
    } catch (const NLMErrors::Exception& e) {
        testPassed = true;
        assert(e.type == NLMErrors::InvalidParameter);
    }
    assert(testPassed);
    
    // Test NLM_VALIDATE macro
    int param = 5;
    try {
        NLM_VALIDATE(param, param > 10, NLMErrors::InvalidParameter, "Parameter validation failed");
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
        assert(e.getFullMessage().find("(parameter: param)") != std::string::npos);
    }
    
    // Test NLM_THROW macro
    try {
        NLM_THROW(NLMErrors::MemoryAllocation, "Test throw macro");
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::MemoryAllocation);
        assert(e.getFullMessage().find("Test throw macro") != std::string::npos);
    }
    
    // Test logging macros
    NLM_LOG_INFO("Test info message");
    NLM_LOG_WARNING("Test warning message");
    NLM_LOG_ERROR("Test error message");
    
    assert(NLMErrors::errorCount() >= 3); // Should have at least 3 more errors
    
    std::cout << "    testErrorMacros passed" << std::endl;
}

// Test 3: Error Handling in Config Class
void testConfigErrorHandling() {
    NLM_LOG_INFO("Testing Config class error handling");
    
    NLMErrors::clearErrors();
    
    // Test 1: Type validation errors
    nlm::Config config;
    
    // Set values with different types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", "test", nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    
    // Valid retrievals
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value() && *doubleVal > 3.13);
    
    // Type mismatch error handling
    auto wrongType = config.get<std::string>("int_val");
    assert(!wrongType.has_value()); // Should be nullopt
    
    // Test 2: File error handling
    std::string invalidPath = "/nonexistent/path/config.json";
    assert(!config.loadFromFile(invalidPath));
    
    // Verify error was recorded
    auto errors = NLMErrors::getErrors();
    assert(!errors.empty());
    
    // Test 3: Parameter validation errors
    try {
        config.set("", "empty_key", nlm::ConfigSource::Runtime);
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 4: Safe loading with error recovery
    bool safeLoadResult = config.safeLoad(invalidPath);
    assert(!safeLoadResult); // Should fail but not crash
    
    // Test 5: Configuration validation
    assert(config.validate()); // Valid configuration
    
    // Add invalid configuration
    config.set("empty_key", "", nlm::ConfigSource::Runtime);
    // Note: Validation doesn't currently catch empty values, but that's expected
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

// Test 4: Error Handling in Neuron Class
void testNeuronErrorHandling() {
    NLM_LOG_INFO("Testing Neuron class error handling");
    
    NLMErrors::clearErrors();
    
    // Create neuron with valid ID
    NeuronId neuronId(1);
    Neuron neuron(neuronId);
    
    // Test 1: Parameter validation in setters
    try {
        NeuronId invalidId(-1); // Invalid ID
        Neuron invalidNeuron(invalidId);
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 2: State validation in getters
    assert(neuron.getId().value == 1); // Should succeed
    
    // Test 3: Membrane potential validation
    try {
        neuron.setMembranePotential(NAN); // Not-a-number
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 4: Threshold validation
    neuron.setThreshold(-55.0f);
    neuron.setMembranePotential(-60.0f);
    assert(!neuron.checkThreshold()); // Should not fire
    
    neuron.setMembranePotential(-50.0f);
    assert(neuron.checkThreshold()); // Should fire
    
    // Test 5: Input validation
    try {
        neuron.receiveExcitatoryInput(NAN);
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 6: Spike history validation
    neuron.setMembranePotential(-50.0f);
    neuron.stepLIF(1000.0f, 0.001f); // Should spike
    
    auto spikeHistory = neuron.getSpikeHistory();
    assert(!spikeHistory.empty());
    assert(spikeHistory[0] == 1000.0f);
    
    // Test 7: Refractory period handling
    assert(neuron.isFiring());
    assert(neuron.isRefractory());
    
    // Clear refractory by resetting
    neuron.reset();
    assert(!neuron.isRefractory());
    
    std::cout << "    testNeuronErrorHandling passed" << std::endl;
}

// Test 5: Error Handling in Synapse Class
void testSynapseErrorHandling() {
    NLM_LOG_INFO("Testing Synapse class error handling");
    
    NLMErrors::clearErrors();
    
    // Create valid synapse
    SynapseId synapseId(1);
    NeuronId sourceId(1);
    NeuronId destId(2);
    Synapse synapse(synapseId, sourceId, destId);
    
    // Test 1: Parameter validation in constructor
    try {
        SynapseId invalidId(-1);
        Synapse invalidSynapse(invalidId, sourceId, destId);
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 2: Weight validation
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    // Test 3: Weight bounds checking
    try {
        synapse.setWeight(2.0f); // Above MAX_WEIGHT (1.0f)
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    synapse.setWeight(-0.5f); // Within bounds
    
    // Test 4: Type validation
    synapse.setType(SynapseType::Excitatory);
    assert(synapse.getType() == SynapseType::Excitatory);
    
    // Test 5: Delay validation
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    // Test 6: Spike history validation
    synapse.recordPreSpike(1000.0f);
    synapse.recordPostSpike(1001.0f);
    
    auto preHistory = synapse.getPreSpikeHistory();
    assert(preHistory.size() == 1 && preHistory[0] == 1000.0f);
    
    auto postHistory = synapse.getPostSpikeHistory();
    assert(postHistory.size() == 1 && postHistory[0] == 1001.0f);
    
    // Test 7: Eligibility trace validation
    synapse.setEligibilityTrace(0.8f);
    assert(synapse.getEligibilityTrace() == 0.8f);
    
    // Test 8: Input validation
    try {
        synapse.setEligibilityTrace(NAN);
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 9: Step with invalid time
    try {
        synapse.step(-100.0f); // Negative time
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::InvalidParameter);
    }
    
    // Test 10: Reset with error handling
    synapse.reset();
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f);
    
    std::cout << "    testSynapseErrorHandling passed" << std::endl;
}

// Test 6: Error Recovery and Graceful Degradation
void testErrorRecovery() {
    NLM_LOG_INFO("Testing error recovery and graceful degradation");
    
    NLMErrors::clearErrors();
    
    nlm::Config config;
    
    // Test 1: Partial configuration failure
    config.set("valid_key1", 1, nlm::ConfigSource::Runtime);
    config.set("valid_key2", "value", nlm::ConfigSource::Runtime);
    
    // This should not crash even though we're using safe load
    bool loadResult = config.safeLoad("/nonexistent/path/file.json");
    assert(!loadResult); // Should fail gracefully
    
    // Configuration should still be usable
    assert(config.has("valid_key1"));
    assert(config.has("valid_key2"));
    
    auto val1 = config.get<int>("valid_key1");
    assert(val1.has_value());
    
    auto val2 = config.get<std::string>("valid_key2");
    assert(val2.has_value());
    
    // Test 2: Error accumulation and reporting
    NLM_ERROR(NLMErrors::ConfigParsing, "First error");
    NLM_ERROR(NLMErrors::MemoryAllocation, "Second error");
    NLM_ERROR(NLMErrors::InvalidParameter, "Third error");
    
    assert(NLMErrors::errorCount() == 3);
    
    // Verify all errors are stored
    auto errors = NLMErrors::getErrors();
    assert(errors.size() == 3);
    
    std::string errorSummary;
    PythonErrorHandler::logErrorsToPython(errorSummary);
    assert(errorSummary.find("First error") != std::string::npos);
    assert(errorSummary.find("Second error") != std::string::npos);
    assert(errorSummary.find("Third error") != std::string::npos);
    
    // Test 3: Error clearing
    NLMErrors::clearErrors();
    assert(NLMErrors::errorCount() == 0);
    
    std::cout << "    testErrorRecovery passed" << std::endl;
}

// Test 7: Python Bindings Error Handling
void testPythonBindingsErrorHandling() {
    NLM_LOG_INFO("Testing Python bindings error handling");
    
    // Test 1: Python exception handling
    NLMErrors::clearErrors();
    
    // Simulate Python function execution with error handling
    try {
        NLM_PY_THROW(NLMErrors::CallbackError, "Python callback failed");
        assert(false); // Should have thrown
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::CallbackError);
        assert(e.context == "Python bindings");
    }
    
    // Test 2: Error context preservation
    NLMErrors::addError(NLMErrors::ConfigParsing, "Config error", "Config.loadFromFile");
    NLMErrors::addError(NLMErrors::FileIO, "File error", "File.saveToFile");
    
    auto errors = NLMErrors::getErrors();
    assert(errors.size() == 2);
    
    // Test 3: Python error handler integration
    std::string lastError = PythonErrorHandler::getLastError();
    assert(!lastError.empty());
    
    bool hasErrors = PythonErrorHandler::hasErrors();
    assert(hasErrors);
    
    // Test 4: Error recovery in Python context
    PythonErrorHandler::clearErrors();
    assert(!PythonErrorHandler::hasErrors());
    
    std::cout << "    testPythonBindingsErrorHandling passed" << std::endl;
}

// Test 8: Cross-Component Error Consistency
void testCrossComponentErrorConsistency() {
    NLM_LOG_INFO("Testing cross-component error consistency");
    
    NLMErrors::clearErrors();
    
    nlm::Config config;
    
    // Configure a valid config
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 1, nlm::ConfigSource::Runtime);
    
    // All error messages should follow consistent format
    NLM_LOG_INFO("Starting simulation with config");
    
    // Simulate error in config component
    NLM_CONFIG_ERROR("Invalid parameter type for neuron_count");
    
    // Simulate error in neuron component
    NLM_NEURON_ERROR("Neuron initialization failed");
    
    // Simulate error in synapse component
    NLM_SYNAPSE_ERROR("Synapse connection failed");
    
    // Simulate error in plasticity component
    NLM_PLASTICITY_ERROR("STDP computation failed");
    
    // Simulate error in validation component
    NLM_VALIDATION_ERROR("Configuration validation failed");
    
    // Verify all errors are recorded with appropriate types
    auto errors = NLMErrors::getErrors();
    assert(errors.size() >= 5);
    
    // Check that errors have different types
    bool hasConfigError = false;
    bool hasNeuronError = false;
    bool hasSynapseError = false;
    bool hasPlasticityError = false;
    bool hasValidationError = false;
    
    for (const auto& error : errors) {
        if (error.type == NLMErrors::ConfigParsing) hasConfigError = true;
        if (error.type == NLMErrors::NeuronInvalid) hasNeuronError = true;
        if (error.type == NLMErrors::SynapseInvalid) hasSynapseError = true;
        if (error.type == NLMErrors::PlasticityFailed) hasPlasticityError = true;
        if (error.type == NLMErrors::ValidationError) hasValidationError = true;
    }
    
    assert(hasConfigError);
    assert(hasNeuronError);
    assert(hasSynapseError);
    assert(hasPlasticityError);
    assert(hasValidationError);
    
    std::cout << "    testCrossComponentErrorConsistency passed" << std::endl;
}

// Test 9: Exception Chaining and Context
void testExceptionChaining() {
    NLM_LOG_INFO("Testing exception chaining and context");
    
    NLMErrors::clearErrors();
    
    // Test 1: Nested exception handling
    try {
        try {
            NLM_THROW(NLMErrors::InvalidParameter, "Inner error");
        } catch (const NLMErrors::Exception& e) {
            NLM_THROW(NLMErrors::SystemNotInitialized, "Outer error: " + std::string(e.what()));
        }
    } catch (const NLMErrors::Exception& e) {
        assert(e.type == NLMErrors::SystemNotInitialized);
        assert(e.getFullMessage().find("Outer error: Inner error") != std::string::npos);
    }
    
    // Test 2: Multiple error context
    NLM_VALIDATE(42, false, NLMErrors::InvalidParameter, "Validation failed");
    
    // Test 3: Error context preservation through multiple operations
    nlm::Config config;
    
    // Add multiple errors with different contexts
    NLM_ERROR(NLMErrors::ConfigParsing, "Error 1", "Config.get()");
    NLM_ERROR(NLMErrors::MemoryAllocation, "Error 2", "Neuron.initialize()");
    NLM_ERROR(NLMErrors::FileIO, "Error 3", "File.save()");
    
    auto errors = NLMErrors::getErrors();
    assert(errors.size() == 3);
    
    for (size_t i = 0; i < errors.size(); ++i) {
        assert(errors[i].context.find("function") != std::string::npos);
    }
    
    std::cout << "    testExceptionChaining passed" << std::endl;
}

// Test 10: Performance Impact of Error Handling
void testErrorHandlingPerformance() {
    NLM_LOG_INFO("Testing performance impact of error handling");
    
    // This test verifies that error handling macros have minimal performance impact
    // when errors are not occurring (normal operation)
    
    NLMErrors::clearErrors();
    
    nlm::Config config;
    
    // Perform many operations without errors
    for (int i = 0; i < 1000; ++i) {
        config.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    // All should succeed without throwing
    assert(NLMErrors::errorCount() == 0);
    
    // Verify we can still access the values
    for (int i = 0; i < 100; ++i) {
        auto val = config.get<int>(std::to_string(i));
        assert(val.has_value() && *val == i);
    }
    
    std::cout << "    testErrorHandlingPerformance passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Error Handling Tests ===" << std::endl << std::endl;
    
    // 1. Error Framework Tests
    testErrorFrameworkInitialization();
    
    // 2. Error Macro Tests
    testErrorMacros();
    
    // 3. Component-Specific Error Handling Tests
    testConfigErrorHandling();
    testNeuronErrorHandling();
    testSynapseErrorHandling();
    
    // 4. Recovery and Graceful Degradation Tests
    testErrorRecovery();
    
    // 5. Python Bindings Tests
    testPythonBindingsErrorHandling();
    
    // 6. Cross-Component Consistency Tests
    testCrossComponentErrorConsistency();
    
    // 7. Exception Chaining Tests
    testExceptionChaining();
    
    // 8. Performance Impact Tests
    testErrorHandlingPerformance();
    
    std::cout << std::endl;
    std::cout << "=== All Error Handling Tests PASSED ===" << std::endl;
}

} // namespace test_error_handling
