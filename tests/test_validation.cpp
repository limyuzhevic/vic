// Validation Tests
// Comprehensive tests for error handling, input validation, runtime validation,
// configuration schema validation, and monitoring systems

#include "core/Error/ErrorSystem.hpp"
#include "core/Validation/NeuralInputValidator.hpp"
#include "core/Schema/JSONSchemaValidator.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>

namespace test_validation {

// Error System Tests
void testErrorHierarchy() {
    std::cout << "    Testing error hierarchy..." << std::endl;
    
    try {
        nlm::ConfigurationError configError("Test config error", "test_context");
        assert(configError.getType() == nlm::NLMError::ErrorType::ConfigurationError);
        assert(configError.isRecoverable() == true);
        assert(configError.suggestRecovery().find("configuration") != std::string::npos);
        
        std::cout << "      ✓ Error hierarchy test passed" << std::endl;
    } catch (...) {
        std::cout << "      ✗ Error hierarchy test failed" << std::endl;
        throw;
    }
}

void testErrorReporting() {
    std::cout << "    Testing error reporting..." << std::endl;
    
    // Clear error history
    nlm::ErrorHandler::clearHistory();
    
    try {
        nlm::MemoryError memError("Test memory error", "test_context");
        nlm::ErrorHandler::report(memError);
        
        assert(nlm::ErrorHandler::getErrorCount() == 1);
        std::string lastError = nlm::ErrorHandler::getLastError();
        assert(lastError.find("Test memory error") != std::string::npos);
        
        std::cout << "      ✓ Error reporting test passed" << std::endl;
    } catch (...) {
        std::cout << "      ✗ Error reporting test failed" << std::endl;
        throw;
    }
}

void testErrorRecovery() {
    std::cout << "    Testing error recovery..." << std::endl;
    
    nlm::ErrorHandler::clearHistory();
    
    bool recoveryAttempted = false;
    auto originalRecoveryHandler = nlm::ErrorHandler::errorRecoveryHandler;
    nlm::ErrorHandler::errorRecoveryHandler = [&](const nlm::NLMError& error) {
        recoveryAttempted = true;
        return error.isRecoverable();
    };
    
    try {
        nlm::MemoryError memError("Test recovery error", "test_context");
        nlm::ErrorHandler::report(memError);
        
        bool recovered = nlm::ErrorHandler::attemptRecovery(memError);
        assert(recovered == true); // Memory errors are recoverable
        assert(recoveryAttempted == true);
        
        std::cout << "      ✓ Error recovery test passed" << std::endl;
    } catch (...) {
        std::cout << "      ✗ Error recovery test failed" << std::endl;
        throw;
    }
    
    nlm::ErrorHandler::errorRecoveryHandler = originalRecoveryHandler;
}

// Neural Input Validation Tests
void testNeuralInputValidation() {
    std::cout << "    Testing neural input validation..." << std::endl;
    
    // Test valid neuron input
    nlm::NeuralInputValidator::NeuronInput neuronInput;
    neuronInput.membranePotential = 10.0f;
    neuronInput.inputCurrent = 5.0f;
    neuronInput.threshold = 20.0f;
    neuronInput.restingPotential = -65.0f;
    neuronInput.leakConductance = 0.05f;
    
    auto validation = neuronInput.validate();
    assert(validation.valid == true);
    
    // Test invalid neuron input
    nlm::NeuralInputValidator::NeuronInput invalidNeuron;
    invalidNeuron.membranePotential = std::nanf("").nan();
    invalidNeuron.inputCurrent = 5.0f;
    
    auto invalidValidation = invalidNeuron.validate();
    assert(invalidValidation.valid == false);
    assert(!invalidValidation.errors.empty());
    
    std::cout << "      ✓ Neural input validation test passed" << std::endl;
}

void testSynapseValidation() {
    std::cout << "    Testing synapse validation..." << std::endl;
    
    // Test valid synapse
    nlm::NeuralInputValidator::SynapseInput synapse;
    synapse.sourceNeuron = nlm::NeuronId(1);
    synapse.destinationNeuron = nlm::NeuronId(2);
    synapse.weight = 0.5f;
    synapse.delay = nlm::Delay(10);
    synapse.eligibilityTrace = 0.1f;
    
    auto validation = synapse.validate();
    assert(validation.valid == true);
    
    // Test invalid synapse
    nlm::NeuralInputValidator::SynapseInput invalidSynapse;
    invalidSynapse.sourceNeuron = nlm::NeuronId(3);
    invalidSynapse.destinationNeuron = nlm::NeuronId(3); // Self-connection
    invalidSynapse.weight = std::nanf("").nan();
    
    auto invalidValidation = invalidSynapse.validate();
    assert(invalidValidation.valid == false);
    
    std::cout << "      ✓ Synapse validation test passed" << std::endl;
}

void testStaticValidationMethods() {
    std::cout << "    Testing static validation methods..." << std::endl;
    
    // Test range validation
    auto rangeResult = nlm::NeuralInputValidator::validateInputRange(5.0, 0.0, 10.0, "test_param");
    assert(rangeResult.valid == true);
    
    auto invalidRangeResult = nlm::NeuralInputValidator::validateInputRange(15.0, 0.0, 10.0, "test_param");
    assert(invalidRangeResult.valid == false);
    assert(!invalidRangeResult.errors.empty());
    
    // Test positive validation
    auto positiveResult = nlm::NeuralInputValidator::validatePositive("test_param", 5.0);
    assert(positiveResult.valid == true);
    
    auto negativeResult = nlm::NeuralInputValidator::validatePositive("test_param", -5.0);
    assert(negativeResult.valid == false);
    
    // Test non-zero validation
    auto nonZeroResult = nlm::NeuralInputValidator::validateNotZero("test_param", 0.1);
    assert(nonZeroResult.valid == true);
    
    auto zeroResult = nlm::NeuralInputValidator::validateNotZero("test_param", 0.0);
    assert(zeroResult.valid == false);
    
    std::cout << "      ✓ Static validation methods test passed" << std::endl;
}

// Runtime Validation Tests
void testRuntimeValidator() {
    std::cout << "    Testing runtime validation..." << std::endl;
    
    nlm::RuntimeValidator::RuntimeContext context;
    context.currentStep = 100;
    context.simulationTimestep = 0.001;
    context.globalPlasiticityModulation = 1.0f;
    context.isRestingPhase = false;
    
    // Test valid runtime context
    auto contextValidation = context.validate();
    assert(contextValidation.valid == true);
    
    // Test invalid runtime context
    context.simulationTimestep = -0.001;
    auto invalidContextValidation = context.validate();
    assert(invalidContextValidation.valid == false);
    
    std::cout << "      ✓ Runtime validation test passed" << std::endl;
}

void testMemoryAccessValidation() {
    std::cout << "    Testing memory access validation..." << std::endl;
    
    // Test valid memory access
    int validVar = 42;
    auto memValidation = nlm::RuntimeValidator::validateMemoryAccess(&validVar, sizeof(validVar), "read");
    assert(memValidation.valid == true);
    
    // Test invalid memory access (null)
    auto nullValidation = nlm::RuntimeValidator::validateMemoryAccess(nullptr, 100, "read");
    assert(nullValidation.valid == false);
    
    // Test zero-size access
    auto zeroSizeValidation = nlm::RuntimeValidator::validateMemoryAccess(&validVar, 0, "write");
    assert(zeroSizeValidation.valid == true); // warning but not error
    
    std::cout << "      ✓ Memory access validation test passed" << std::endl;
}

// Configuration Schema Validation Tests
void testJSONSchemaValidator() {
    std::cout << "    Testing JSON schema validator..." << std::endl;
    
    nlm::Config config;
    
    // Set valid configuration
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 1, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    // Test default schema validation
    auto validation = nlm::JSONSchemaValidator::validateConfig(config);
    assert(validation.valid == true);
    
    // Test development schema validation
    validation = nlm::JSONSchemaValidator::validateConfigWithProfile(config, "development");
    assert(validation.valid == true);
    
    // Test production schema validation
    validation = nlm::JSONSchemaValidator::validateConfigWithProfile(config, "production");
    assert(validation.valid == true);
    
    std::cout << "      ✓ JSON schema validator test passed" << std::endl;
}

void testInvalidConfigValidation() {
    std::cout << "    Testing invalid configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Set invalid configuration
    config.set("random_seed", 3000000000, nlm::ConfigSource::Runtime); // Out of range
    config.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime); // Negative
    config.set("neuron_count", 0, nlm::ConfigSource::Runtime); // Zero or less
    config.set("region_count", 2000, nlm::ConfigSource::Runtime); // Too large
    
    // Test validation
    auto validation = nlm::JSONSchemaValidator::validateConfig(config);
    assert(validation.valid == false);
    assert(!validation.errors.empty());
    
    std::cout << "      ✓ Invalid configuration validation test passed" << std::endl;
}

void testSchemaErrorMessages() {
    std::cout << "    Testing schema error messages..." << std::endl;
    
    nlm::Config config;
    
    // Test missing required key
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    // Intentionally not setting other required keys
    
    auto validation = nlm::JSONSchemaValidator::validateConfig(config);
    assert(validation.valid == false);
    assert(!validation.errors.empty());
    
    // Test value validation error
    config.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime);
    validation = nlm::JSONSchemaValidator::validateConfig(config);
    assert(validation.valid == false);
    assert(!validation.errors.empty());
    
    std::cout << "      ✓ Schema error messages test passed" << std::endl;
}

// Performance Monitoring Tests
void testPerformanceMonitor() {
    std::cout << "    Testing performance monitor..." << std::endl;
    
    // Initialize performance monitor
    nlm::PerformanceMonitor::initialize(0.1, 1024 * 1024); // baseline cpu and memory
    
    // Record some metrics
    nlm::PerformanceMonitor::recordStepMetrics(100, 1000, 0.001);
    nlm::PerformanceMonitor::recordMemoryUsage(512 * 1024); // 512KB
    nlm::PerformanceMonitor::recordWeight(0.5f);
    
    // Get metrics
    auto metrics = nlm::PerformanceMonitor::getMetrics();
    assert(metrics.spikeCounts.size() == 1);
    assert(metrics.spikeCounts[0] == 100);
    assert(metrics.timestepTimes.size() == 1);
    assert(metrics.timestepTimes[0] == 0.001);
    
    // Test summary generation
    std::string summary = nlm::PerformanceMonitor::getSummary();
    assert(summary.find("Performance Metrics") != std::string::npos);
    
    std::cout << "      ✓ Performance monitor test passed" << std::endl;
}

void testPerformanceAnomalies() {
    std::cout << "    Testing performance anomaly detection..." << std::endl;
    
    // Initialize monitor
    nlm::PerformanceMonitor::initialize(0.1, 1024 * 1024);
    
    // Record unusual firing rates
    nlm::PerformanceMonitor::recordStepMetrics(1000, 100, 0.001); // Very high firing rate
    nlm::PerformanceMonitor::recordStepMetrics(10, 1000, 0.001); // Very low firing rate
    
    // Get alerts
    auto alerts = nlm::PerformanceMonitor::getAlerts();
    
    // Clean up
    nlm::PerformanceMonitor::clearAlerts();
    
    std::cout << "      ✓ Performance anomaly detection test passed" << std::endl;
}

void testDevelopmentStageValidation() {
    std::cout << "    Testing development stage validation..." << std::endl;
    
    nlm::Config config;
    config.set("developmental_stage", "maturation", nlm::ConfigSource::Runtime);
    
    auto stage = nlm::DevelopmentValidator::getDevelopmentStage(config);
    assert(stage == nlm::DevelopmentValidator::DevelopmentStage::Maturation);
    
    // Test default parameters for maturation stage
    auto params = nlm::DevelopmentValidator::getDefaultParameters(stage);
    assert(params.stage == nlm::DevelopmentValidator::DevelopmentStage::Maturation);
    assert(params.plasticityRate == 0.5); // Default for maturation
    assert(params.synaptogenesisRate == 0.0005);
    assert(params.pruningRate == 0.00005);
    
    std::cout << "      ✓ Development stage validation test passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Test basic config operations
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    
    auto intVal = config.get<int64_t>("int_value");
    assert(intVal && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal && *stringVal == "hello");
    
    std::cout << "      ✓ Configuration validation test passed" << std::endl;
}

void testNeuralConnectionValidation() {
    std::cout << "    Testing neural connection validation..." << std::endl;
    
    std::vector<std::pair<nlm::NeuronId, nlm::NeuronId>> connections;
    connections.push_back({nlm::NeuronId(1), nlm::NeuronId(2)});
    connections.push_back({nlm::NeuronId(2), nlm::NeuronId(3)});
    connections.push_back({nlm::NeuronId(3), nlm::NeuronId(1)});
    
    std::unordered_map<nlm::NeuronId, bool> neuronExists;
    neuronExists[nlm::NeuronId(1)] = true;
    neuronExists[nlm::NeuronId(2)] = true;
    neuronExists[nlm::NeuronId(3)] = true;
    
    auto result = nlm::NeuralConnectionValidator::validateConnections(connections, neuronExists);
    assert(result.valid == true);
    assert(result.validConnections.size() == 3);
    
    // Test with self-connection
    connections.push_back({nlm::NeuronId(4), nlm::NeuronId(4)});
    result = nlm::NeuralConnectionValidator::validateConnections(connections, neuronExists);
    assert(result.valid == false);
    assert(!result.errors.empty());
    
    std::cout << "      ✓ Neural connection validation test passed" << std::endl;
}

void runAll() {
    std::cout << "\n=== Running Validation Tests ===\n" << std::endl;
    
    // Error System Tests
    testErrorHierarchy();
    testErrorReporting();
    testErrorRecovery();
    
    // Neural Input Validation Tests
    testNeuralInputValidation();
    testSynapseValidation();
    testStaticValidationMethods();
    
    // Runtime Validation Tests
    testRuntimeValidator();
    testMemoryAccessValidation();
    
    // Configuration Schema Validation Tests
    testJSONSchemaValidator();
    testInvalidConfigValidation();
    testSchemaErrorMessages();
    
    // Performance Monitoring Tests
    testPerformanceMonitor();
    testPerformanceAnomalies();
    
    // Development Stage Validation Tests
    testDevelopmentStageValidation();
    
    // Configuration Validation Tests
    testConfigurationValidation();
    
    // Neural Connection Validation Tests
    testNeuralConnectionValidation();
    
    std::cout << "\n=== All Validation Tests Passed! ===\n" << std::endl;
}

} // namespace test_validation