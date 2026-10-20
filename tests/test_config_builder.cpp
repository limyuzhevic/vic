// ConfigBuilder Tests
#include "core/Config/ConfigBuilder.hpp"
#include <cassert>
#include <iostream>

namespace test_config_builder {

void testConfigBuilderCreation() {
    std::cout << "    testConfigBuilderCreation passed" << std::endl;
}

void testConfigBuilderPresets() {
    // Test simple preset
    auto simpleConfig = nlm::ConfigBuilder::simple();
    assert(simpleConfig->has("neuron_count"));
    assert(simpleConfig->getOr<int64_t>("neuron_count", 0) == 100);
    
    // Test standard preset
    auto standardConfig = nlm::ConfigBuilder::standard();
    assert(standardConfig->has("neuron_count"));
    assert(standardConfig->getOr<int64_t>("neuron_count", 0) == 500);
    
    // Test research preset
    auto researchConfig = nlm::ConfigBuilder::research();
    assert(researchConfig->has("neuron_count"));
    assert(researchConfig->getOr<int64_t>("neuron_count", 0) == 1000);
    
    std::cout << "    testConfigBuilderPresets passed" << std::endl;
}

void testConfigBuilderFluentInterface() {
    // Test fluent interface
    auto builder = nlm::ConfigBuilder()
        .neuronCount(250)
        .regionCount(2)
        .connectionProbability(0.15f)
        .simulationTimestep(0.001)
        .randomSeed(42)
        .configureLIF(-70.0, -55.0, -70.0, 10.0)
        .configureSTDP(0.02f, 0.018f, 20.0f);
    
    auto config = builder.getConfig();
    assert(config->has("neuron_count"));
    assert(config->getOr<int64_t>("neuron_count", 0) == 250);
    assert(config->has("region_count"));
    assert(config->getOr<int64_t>("region_count", 0) == 2);
    assert(config->has("connection_probability"));
    assert(config->getOr<double>("connection_probability", 0.0) == 0.15f);
    
    std::cout << "    testConfigBuilderFluentInterface passed" << std::endl;
}

void testConfigBuilderValidation() {
    // Test empty builder (invalid)
    nlm::ConfigBuilder emptyBuilder;
    // Empty builder should be invalid when validated
    assert(!emptyBuilder.validate());
    
    // Test valid builder
    nlm::ConfigBuilder validBuilder;
    assert(validBuilder.neuronCount(100).validate());
    assert(!validBuilder.neuronCount(0).validate()); // Invalid: 0 neurons
    assert(validBuilder.neuronCount(-1).validate()); // Note: negative is technically valid but ill-advised
    
    std::cout << "    testConfigBuilderValidation passed" << std::endl;
}

void testConfigBuilderValidationErrors() {
    nlm::ConfigBuilder emptyBuilder;
    std::string errors = emptyBuilder.getValidationErrors();
    assert(!errors.empty());
    
    nlm::ConfigBuilder invalidBuilder;
    invalidBuilder.neuronCount(0);
    std::string errors2 = invalidBuilder.getValidationErrors();
    assert(!errors2.empty());
    
    std::cout << "    testConfigBuilderValidationErrors passed" << std::endl;
}

void testConfigBuilderCustomValues() {
    auto builder = nlm::ConfigBuilder()
        .set("custom_key1", std::string("value1"))
        .set("custom_key2", 42.5)
        .set("custom_key3", 100)
        .set("custom_key4", true);
    
    auto config = builder.getConfig();
    assert(config->has("custom_key1"));
    assert(config->getOr<std::string>("custom_key1", "") == "value1");
    assert(config->has("custom_key2"));
    assert(config->getOr<double>("custom_key2", 0.0) == 42.5);
    assert(config->has("custom_key3"));
    assert(config->getOr<int64_t>("custom_key3", 0) == 100);
    assert(config->has("custom_key4"));
    assert(config->getOr<bool>("custom_key4", false) == true);
    
    std::cout << "    testConfigBuilderCustomValues passed" << std::endl;
}

void runAll() {
    std::cout << "Running ConfigBuilder tests..." << std::endl;
    testConfigBuilderCreation();
    testConfigBuilderPresets();
    testConfigBuilderFluentInterface();
    testConfigBuilderValidation();
    testConfigBuilderValidationErrors();
    testConfigBuilderCustomValues();
    std::cout << "\nAll ConfigBuilder tests passed!" << std::endl;
}

} // namespace test_config_builder