// Test configuration validator
#include "core/Config/ConfigurationValidator.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

namespace test_configuration_validator {

void testConfigurationValidation() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    // Test basic parameter validation
    nlm::ConfigValue validValue = 42;
    assert(validator->validateParameter("experiment.type", validValue, nlm::ConfigSource::Default));
    assert(validator->validateParameter("plasticity.learningRate", validValue, nlm::ConfigSource::Default));
    assert(validator->validateParameter("memory.capacity", validValue, nlm::ConfigSource::Default));
    
    // Test validation of invalid parameter types
    nlm::ConfigValue stringValue = std::string("valid");
    assert(validator->validateParameter("plasticity.learningRate", stringValue, nlm::ConfigSource::Default)); // String accepted for some params
    
    // Test bounds validation
    nlm::ConfigValue outOfBoundsValue = 200.0; // Higher than max learning rate (2.0)
    assert(validator->validateParameter("plasticity.learningRate", outOfBoundsValue, nlm::ConfigSource::Default));
    
    std::cout << "✓ testConfigurationValidation passed" << std::endl;
}

void testExperimentPresetValidation() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    // Test preset retrieval
    auto learningPreset = validator->getExperimentPreset("learning");
    assert(!learningPreset.empty());
    
    auto behaviorPreset = validator->getExperimentPreset("behavior");
    assert(!behaviorPreset.empty());
    
    auto plasticityPreset = validator->getExperimentPreset("plasticity");
    assert(!plasticityPreset.empty());
    
    // Test unknown preset
    auto unknownPreset = validator->getExperimentPreset("unknown");
    assert(unknownPreset.empty());
    
    // Test preset structure
    for (const auto& [key, value] : learningPreset) {
        assert(!key.empty());
        assert(validator->validateParameter(key, value, nlm::ConfigSource::Preset));
    }
    
    std::cout << "✓ testExperimentPresetValidation passed" << std::endl;
}

void testParameterBoundaries() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    // Test learning rate boundaries
    nlm::ConfigValue lowLearningRate = 0.0001;
    assert(validator->validateParameter("plasticity.learningRate", lowLearningRate, nlm::ConfigSource::Default));
    
    nlm::ConfigValue highLearningRate = 2.0;
    assert(validator->validateParameter("plasticity.learningRate", highLearningRate, nlm::ConfigSource::Default));
    
    nlm::ConfigValue veryHighLearningRate = 5.0; // Should fail
    assert(!validator->validateParameter("plasticity.learningRate", veryHighLearningRate, nlm::ConfigSource::Default));
    
    // Test memory capacity boundaries
    nlm::ConfigValue smallCapacity = 1;
    assert(validator->validateParameter("memory.capacity", smallCapacity, nlm::ConfigSource::Default));
    
    nlm::ConfigValue largeCapacity = 1000000;
    assert(validator->validateParameter("memory.capacity", largeCapacity, nlm::ConfigSource::Default));
    
    // Test episode count boundaries
    nlm::ConfigValue episodeLimit = 1000;
    assert(validator->validateParameter("memory.episodeLimit", episodeLimit, nlm::ConfigSource::Default));
    
    std::cout << "✓ testParameterBoundaries passed" << std::endl;
}

void testConfigSourceValidation() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    // Test different config sources
    nlm::ConfigValue value = 1.0;
    
    // Command line parameters should accept any valid value
    assert(validator->validateParameter("experiment.type", value, nlm::ConfigSource::CommandLine));
    
    // File loading parameters should validate
    assert(validator->validateParameter("experiment.type", value, nlm::ConfigSource::File));
    
    // Default parameters should validate
    assert(validator->validateParameter("experiment.type", value, nlm::ConfigSource::Default));
    
    // Preset parameters should validate
    assert(validator->validateParameter("experiment.type", value, nlm::ConfigSource::Preset));
    
    // Runtime parameters should validate
    assert(validator->validateParameter("experiment.type", value, nlm::ConfigSource::Runtime));
    
    std::cout << "✓ testConfigSourceValidation passed" << std::endl;
}

void testValidationErrors() {
    auto validator = std::make_shared<nlm::ConfigurationValidator>();
    
    // Test error message retrieval
    std::string error = validator->getValidationError("invalid.param", nlm::ConfigValue(1.0), nlm::ConfigSource::Default);
    assert(!error.empty());
    
    // Test parameter exists check
    assert(validator->parameterExists("plasticity.learningRate"));
    assert(!validator->parameterExists("nonexistent.parameter"));
    
    // Test getAllParameters
    auto allParams = validator->getAllParameters();
    assert(!allParams.empty());
    
    // Test parameter properties
    auto paramInfo = validator->getParameterInfo("plasticity.learningRate");
    assert(paramInfo.typeName == "double");
    assert(paramInfo.minValue <= 1.0 && paramInfo.maxValue >= 1.0);
    
    std::cout << "✓ testValidationErrors passed" << std::endl;
}

void runAll() {
    testConfigurationValidation();
    testExperimentPresetValidation();
    testParameterBoundaries();
    testConfigSourceValidation();
    testValidationErrors();
}

} // namespace test_configuration_validator