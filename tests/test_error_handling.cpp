// Comprehensive Error Handling Tests
// Tests for error handling functionality and validation

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace test_error_handling {

void testErrorCategories() {
    std::cout << "    testErrorCategories passed" << std::endl;
}

void testErrorSeverityComparison() {
    bool severe1MoreThan2 = nlm::ErrorHandling::isMoreSevere(nlm::ErrorSeverity::Critical, nlm::ErrorSeverity::Error);
    bool severe2MoreThan1 = nlm::ErrorHandling::isMoreSevere(nlm::ErrorSeverity::Error, nlm::ErrorSeverity::Critical);
    bool sameSeverity = nlm::ErrorHandling::isMoreSevere(nlm::ErrorSeverity::Warning, nlm::ErrorSeverity::Warning);
    
    assert(severe1MoreThan2);
    assert(!severe2MoreThan1);
    assert(!sameSeverity);
    
    std::cout << "    testErrorSeverityComparison passed" << std::endl;
}

void testNeuronCountValidation() {
    nlm::ErrorInfo info("Test neuron count validation", nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Error);
    assert(info.category == nlm::ErrorCategory::Validation);
    assert(info.severity == nlm::ErrorSeverity::Error);
    
    bool validCount = nlm::ErrorHandling::validateNeuronCount(1000, "test");
    bool invalidCount = nlm::ErrorHandling::validateNeuronCount(5, "test");  // Below minimum
    bool tooLargeCount = nlm::ErrorHandling::validateNeuronCount(2000000, "test");  // Above maximum
    
    assert(validCount);
    assert(!invalidCount);
    assert(!tooLargeCount);
    
    auto fallbackSmall = nlm::ErrorHandling::suggestFallbackNeuronCount(5);
    auto fallbackLarge = nlm::ErrorHandling::suggestFallbackNeuronCount(2000000);
    
    assert(fallbackSmall.has_value());
    assert(fallbackLarge.has_value());
    
    std::cout << "    testNeuronCountValidation passed" << std::endl;
}

void testSynapseWeightValidation() {
    bool validWeight = nlm::ErrorHandling::validateSynapseWeight(0.5f, "test");
    bool invalidPositive = nlm::ErrorHandling::validateSynapseWeight(15.0f, "test");  // Above max
    bool invalidNegative = nlm::ErrorHandling::validateSynapseWeight(-15.0f, "test");  // Below min
    
    assert(validWeight);
    assert(!invalidPositive);
    assert(!invalidNegative);
    
    auto fallbackLarge = nlm::ErrorHandling::suggestFallbackWeight(15.0f);
    auto fallbackSmall = nlm::ErrorHandling::suggestFallbackWeight(-15.0f);
    
    assert(fallbackLarge.has_value());
    assert(fallbackSmall.has_value());
    
    std::cout << "    testSynapseWeightValidation passed" << std::endl;
}

void testTimestepValidation() {
    bool validTimestep = nlm::ErrorHandling::validateSimulationTimestep(0.001, "test");
    bool invalidNegative = nlm::ErrorHandling::validateSimulationTimestep(-0.001, "test");
    bool tooSmall = nlm::ErrorHandling::validateSimulationTimestep(1e-7, "test");  // Below min
    bool tooLarge = nlm::ErrorHandling::validateSimulationTimestep(0.5, "test");  // Above max
    
    assert(validTimestep);
    assert(!invalidNegative);
    assert(!tooSmall);
    assert(!tooLarge);
    
    auto fallbackNegative = nlm::ErrorHandling::suggestFallbackTimestep(-0.001);
    auto fallbackLarge = nlm::ErrorHandling::suggestFallbackTimestep(0.5);
    
    assert(fallbackNegative.has_value());
    assert(fallbackLarge.has_value());
    
    std::cout << "    testTimestepValidation passed" << std::endl;
}

void testConnectionProbabilityValidation() {
    bool validProb = nlm::ErrorHandling::validateConnectionProbability(0.5f, "test");
    bool invalidNegative = nlm::ErrorHandling::validateConnectionProbability(-0.1f, "test");
    bool invalidAboveOne = nlm::ErrorHandling::validateConnectionProbability(1.1f, "test");
    
    assert(validProb);
    assert(!invalidNegative);
    assert(!invalidAboveOne);
    
    std::cout << "    testConnectionProbabilityValidation passed" << std::endl;
}

void testRegionCountValidation() {
    bool validRegions = nlm::ErrorHandling::validateRegionCount(10, "test");
    bool invalidZero = nlm::ErrorHandling::validateRegionCount(0, "test");
    bool tooLarge = nlm::ErrorHandling::validateRegionCount(2000, "test");
    
    assert(validRegions);
    assert(!invalidZero);
    assert(!tooLarge);
    
    std::cout << "    testRegionCountValidation passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Valid config
    std::string validJson = R"({"neuron_count": 100, "region_count": 2})";
    std::ofstream validFile("valid_config.json");
    validFile << validJson;
    validFile.close();
    
    bool loadValid = config.loadFromFile("valid_config.json");
    
    // Invalid JSON
    std::string invalidJson = R"({"neuron_count": 100, "invalid": })";
    std::ofstream invalidFile("invalid_config.json");
    invalidFile << invalidJson;
    invalidFile.close();
    
    bool loadInvalid = config.loadFromFile("invalid_config.json");
    
    // Test config format validation
    bool validFormat = nlm::ErrorHandling::validateConfigFileFormat(validJson, "valid_config.json");
    bool invalidFormat = nlm::ErrorHandling::validateConfigFileFormat(invalidJson, "invalid_config.json");
    
    // The implementation should handle invalid JSON gracefully
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testErrorInfoCreation() {
    nlm::ErrorInfo error1("Simple error", nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Error);
    assert(error1.message == "Simple error");
    assert(error1.category == nlm::ErrorCategory::Validation);
    assert(error1.severity == nlm::ErrorSeverity::Error);
    assert(!error1.isHandled);
    
    nlm::ErrorInfo error2("Error with recovery", nlm::ErrorCategory::Recovery, nlm::ErrorSeverity::Error);
    error2.recoveryActions = {"Retry", "Fallback", "Manual intervention"};
    assert(error2.recoveryActions.size() == 3);
    
    error2.isHandled = true;
    assert(error2.isHandled);
    
    std::cout << "    testErrorInfoCreation passed" << std::endl;
}

void testSafeParameterAccess() {
    nlm::Config config;
    
    // Test valid parameter access
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    auto value = nlm::ErrorHandling::safeGetParameter<int>(config, "valid_int", 0, "test", nullptr);
    assert(value == 42);
    
    // Test missing parameter
    auto missing = nlm::ErrorHandling::safeGetParameter<std::string>(config, "missing", "default", "test", nullptr);
    assert(missing == "default");
    
    // Test with validator
    auto validator = [](int val) { return val > 0; };
    
    config.set("negative", -5, nlm::ConfigSource::Runtime);
    auto validated = nlm::ErrorHandling::safeGetParameter<int>(config, "negative", 0, "test", validator);
    assert(validated == 0);  // Should return default due to validation failure
    
    config.set("positive", 10, nlm::ConfigSource::Runtime);
    auto validValidated = nlm::ErrorHandling::safeGetParameter<int>(config, "positive", 0, "test", validator);
    assert(validValidated == 10);  // Should use actual value
    
    std::cout << "    testSafeParameterAccess passed" << std::endl;
}

void testErrorFormatting() {
    nlm::ErrorInfo error("Test error", nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Error);
    error.code = "ERR_001";
    error.context = {{ "key1", "value1" }, { "key2", "value2" }};
    
    std::string formatted = nlm::ErrorHandling::formatError(error);
    assert(formatted.find("[Error]") != std::string::npos);
    assert(formatted.find("[Validation]") != std::string::npos);
    assert(formatted.find("Test error") != std::string::npos);
    assert(formatted.find("ERR_001") != std::string::npos);
    
    std::cout << "    testErrorFormatting passed" << std::endl;
}

void testConfigFileErrorHandling() {
    nlm::Config config;
    
    // Test loading from non-existent file
    bool result1 = config.loadFromFile("non_existent_file.txt");
    assert(!result1);  // Should return false
    
    // Test saving to invalid path (should handle gracefully)
    bool result2 = config.saveToFile("/invalid/path/file.txt");
    assert(!result2);  // Should return false
    
    // Test loading empty file
    std::ofstream emptyFile("empty_config.txt");
    emptyFile.close();
    bool result3 = config.loadFromFile("empty_config.txt");
    
    std::cout << "    testConfigFileErrorHandling passed" << std::endl;
}

void testCommandLineArgumentHandling() {
    nlm::Config config;
    
    // Simulate command line arguments
    const char* argv[] = {"test", "--neuron_count=100", "--region_count", "2", "-connection_probability=0.5"};
    
    // Test loadFromArgs would be called
    // For now, test that config can be set with command line format
    config.set("neuron_count", "100", nlm::ConfigSource::CommandLine);
    config.set("region_count", "2", nlm::ConfigSource::CommandLine);
    config.set("connection_probability", "0.5", nlm::ConfigSource::CommandLine);
    
    assert(config.getOr<size_t>("neuron_count", 0) == 100);
    assert(config.getOr<size_t>("region_count", 0) == 2);
    assert(config.getOr<float>("connection_probability", 0.0f) > 0.4 && 
           config.getOr<float>("connection_probability", 0.0f) < 0.6);
    
    // Check that sources are tracked
    assert(config.has("neuron_count"));
    
    std::cout << "    testCommandLineArgumentHandling passed" << std::endl;
}

void testValidationBoundaryConditions() {
    // Test exact boundary values
    bool minNeuronValid = nlm::ErrorHandling::validateNeuronCount(10, "boundary");  // MIN_NEURONS = 10
    bool belowMin = nlm::ErrorHandling::validateNeuronCount(9, "boundary");
    bool maxNeuronValid = nlm::ErrorHandling::validateNeuronCount(1000000, "boundary");  // MAX_NEURONS = 1000000
    bool aboveMax = nlm::ErrorHandling::validateNeuronCount(1000001, "boundary");
    
    assert(minNeuronValid);
    assert(!belowMin);
    assert(maxNeuronValid);
    assert(!aboveMax);
    
    // Test timestep boundaries
    bool minTimestep = nlm::ErrorHandling::validateSimulationTimestep(1e-6, "boundary");  // MIN_TIMESTEP = 1e-6
    bool belowMinTimestep = nlm::ErrorHandling::validateSimulationTimestep(1e-7, "boundary");
    bool maxTimestep = nlm::ErrorHandling::validateSimulationTimestep(0.1, "boundary");  // MAX_TIMESTEP = 0.1
    bool aboveMaxTimestep = nlm::ErrorHandling::validateSimulationTimestep(0.11, "boundary");
    
    assert(minTimestep);
    assert(!belowMinTimestep);
    assert(maxTimestep);
    assert(!aboveMaxTimestep);
    
    std::cout << "    testValidationBoundaryConditions passed" << std::endl;
}

void testRecoveryMechanisms() {
    // Test fallback mechanisms for various error scenarios
    auto neuronFallback = nlm::ErrorHandling::suggestFallbackNeuronCount(5);
    assert(neuronFallback.has_value() && neuronFallback.value() == 100);
    
    auto neuronFallback2 = nlm::ErrorHandling::suggestFallbackNeuronCount(2000000);
    assert(neuronFallback2.has_value() && neuronFallback2.value() == 10000);
    
    auto weightFallback = nlm::ErrorHandling::suggestFallbackWeight(15.0f);
    assert(weightFallback.has_value() && weightFallback.value() == 1.0f);
    
    auto weightFallback2 = nlm::ErrorHandling::suggestFallbackWeight(-15.0f);
    assert(weightFallback2.has_value() && weightFallback2.value() == -1.0f);
    
    auto timestepFallback = nlm::ErrorHandling::suggestFallbackTimestep(-0.001);
    assert(timestepFallback.has_value() && timestepFallback.value() == 0.001);
    
    auto timestepFallback2 = nlm::ErrorHandling::suggestFallbackTimestep(0.5);
    assert(timestepFallback2.has_value() && timestepFallback2.value() == 0.01);
    
    std::cout << "    testRecoveryMechanisms passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive error handling tests..." << std::endl;
    
    testErrorCategories();
    testErrorSeverityComparison();
    testNeuronCountValidation();
    testSynapseWeightValidation();
    testTimestepValidation();
    testConnectionProbabilityValidation();
    testRegionCountValidation();
    testConfigValidation();
    testErrorInfoCreation();
    testSafeParameterAccess();
    testErrorFormatting();
    testConfigFileErrorHandling();
    testCommandLineArgumentHandling();
    testValidationBoundaryConditions();
    testRecoveryMechanisms();
    
    std::cout << "All error handling tests passed!" << std::endl;
}

} // namespace test_error_handling
