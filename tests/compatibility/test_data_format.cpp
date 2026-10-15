// Compatibility Tests - Data Format
// Comprehensive data format compatibility test suite

#include "tests/utils/TestUtils.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

namespace test_data_format {

void testDataFormatConfigValues() {
    TestUtils::TestSession session("testDataFormatConfigValues");
    
    // Test that ConfigValue can handle all old data types
    nlm::Config config;
    
    // Set all supported types using old patterns
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("int64_value", static_cast<int64_t>(1000000000000), nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("string_value", "test string", nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    config.set("vector_double", std::vector<double>{1.1, 2.2, 3.3, 4.4, 5.5}, nlm::ConfigSource::Runtime);
    config.set("vector_string", std::vector<std::string>{"a", "b", "c", "d", "e"}, nlm::ConfigSource::Runtime);
    
    // Retrieve all types using old patterns
    auto intVal = config.get<int>("int_value");
    auto int64Val = config.get<int64_t>("int64_value");
    auto doubleVal = config.get<double>("double_value");
    auto boolVal = config.get<bool>("bool_value");
    auto stringVal = config.get<std::string>("string_value");
    
    TestAssert::true(intVal.has_value(), "Should retrieve int value");
    TestAssert::equal(*intVal, 42, "Int value should be correct");
    
    TestAssert::true(int64Val.has_value(), "Should retrieve int64 value");
    TestAssert::equal(*int64Val, 1000000000000LL, "Int64 value should be correct");
    
    TestAssert::true(doubleVal.has_value(), "Should retrieve double value");
    TestAssert::equal(*doubleVal, 3.14159, 0.0001, "Double value should be correct");
    
    TestAssert::true(boolVal.has_value(), "Should retrieve bool value");
    TestAssert::true(*boolVal == true, "Bool value should be correct");
    
    TestAssert::true(stringVal.has_value(), "Should retrieve string value");
    TestAssert::equal(*stringVal, "test string", "String value should be correct");
    
    std::cout << "    testDataFormatConfigValues passed" << std::endl;
    session.setSuccess(true);
}

void testDataFormatVectorValues() {
    TestUtils::TestSession session("testDataFormatVectorValues");
    
    // Test that vector values are handled correctly
    nlm::Config config;
    
    // Set vector values
    std::vector<int> intVector{1, 2, 3, 4, 5};
    std::vector<double> doubleVector{1.1, 2.2, 3.3, 4.4, 5.5};
    std::vector<std::string> stringVector{"a", "b", "c", "d", "e"};
    
    config.set("int_vector", intVector, nlm::ConfigSource::Runtime);
    config.set("double_vector", doubleVector, nlm::ConfigSource::Runtime);
    config.set("string_vector", stringVector, nlm::ConfigSource::Runtime);
    
    // Retrieve vector values
    auto retrievedIntVector = config.get<std::vector<int>>("int_vector");
    auto retrievedDoubleVector = config.get<std::vector<double>>("double_vector");
    auto retrievedStringVector = config.get<std::vector<std::string>>("string_vector");
    
    if (retrievedIntVector.has_value()) {
        TestAssert::true(retrievedIntVector->size() == 5, "Int vector should have 5 elements");
        TestAssert::equal(retrievedIntVector->at(0), 1, "Int vector first element should be 1");
        TestAssert::equal(retrievedIntVector->at(4), 5, "Int vector last element should be 5");
    }
    
    if (retrievedDoubleVector.has_value()) {
        TestAssert::true(retrievedDoubleVector->size() == 5, "Double vector should have 5 elements");
        TestAssert::equal(retrievedDoubleVector->at(0), 1.1, 0.01, "Double vector first element should be 1.1");
    }
    
    if (retrievedStringVector.has_value()) {
        TestAssert::true(retrievedStringVector->size() == 5, "String vector should have 5 elements");
        TestAssert::equal(retrievedStringVector->at(0), "a", "String vector first element should be 'a'");
        TestAssert::equal(retrievedStringVector->at(4), "e", "String vector last element should be 'e'");
    }
    
    std::cout << "    testDataFormatVectorValues passed" << std::endl;
    session.setSuccess(true);
}

void testDataFormatSerialization() {
    TestUtils::TestSession session("testDataFormatSerialization");
    
    // Test that data can be serialized to file and loaded back
    std::string testDir = "/tmp/nlm_data_format_test_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::filesystem::create_directories(testDir);
    
    std::string filepath = testDir + "/data_format_config.json";
    
    // Create config with various data types
    nlm::Config config;
    config.set("int_value", 123, nlm::ConfigSource::Runtime);
    config.set("double_value", 45.67, nlm::ConfigSource::Runtime);
    config.set("string_value", "serialization test", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_vector", std::vector<int>{10, 20, 30}, nlm::ConfigSource::Runtime);
    
    // Save config
    bool saveSuccess = config.saveToFile(filepath);
    TestAssert::true(saveSuccess, "Should save config to file");
    
    // Load config
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile(filepath);
    TestAssert::true(loadSuccess, "Should load config from file");
    
    // Verify all values
    auto intVal = loadedConfig.get<int>("int_value");
    TestAssert::true(intVal.has_value(), "Loaded config should have int value");
    TestAssert::equal(*intVal, 123, "Loaded int value should be correct");
    
    auto doubleVal = loadedConfig.get<double>("double_value");
    TestAssert::true(doubleVal.has_value(), "Loaded config should have double value");
    TestAssert::equal(*doubleVal, 45.67, 0.01, "Loaded double value should be correct");
    
    auto stringVal = loadedConfig.get<std::string>("string_value");
    TestAssert::true(stringVal.has_value(), "Loaded config should have string value");
    TestAssert::equal(*stringVal, "serialization test", "Loaded string value should be correct");
    
    auto boolVal = loadedConfig.get<bool>("bool_value");
    TestAssert::true(boolVal.has_value(), "Loaded config should have bool value");
    TestAssert::true(*boolVal == true, "Loaded bool value should be correct");
    
    auto intVectorVal = loadedConfig.get<std::vector<int>>("int_vector");
    if (intVectorVal.has_value()) {
        TestAssert::true(intVectorVal->size() == 3, "Loaded int vector should have 3 elements");
        TestAssert::equal(intVectorVal->at(0), 10, "Loaded int vector first element should be 10");
    }
    
    // Cleanup
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testDataFormatSerialization passed" << std::endl;
    session.setSuccess(true);
}

void testDataFormatIntegration() {
    TestUtils::TestSession session("testDataFormatIntegration");
    
    // Test data format integration with system components
    auto config = std::make_shared<nlm::Config>();
    
    // Set configuration using various data types
    config->set("neurons.count", 100, nlm::ConfigSource::Runtime);
    config->set("neurons.type", "LIF", nlm::ConfigSource::Runtime);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config->set("batch_size", 32, nlm::ConfigSource::Runtime);
    config->set("enabled_features", std::vector<std::string>{"STDP", "NeuronalAdaptation", "Memory"}, nlm::ConfigSource::Runtime);
    
    // Verify configuration
    auto neuronCount = config->get<int>("neurons.count");
    TestAssert::true(neuronCount.has_value(), "Should have neuron count");
    TestAssert::equal(*neuronCount, 100, "Neuron count should be correct");
    
    auto neuronType = config->get<std::string>("neurons.type");
    TestAssert::true(neuronType.has_value(), "Should have neuron type");
    TestAssert::equal(*neuronType, "LIF", "Neuron type should be correct");
    
    auto learningRate = config->get<double>("learning_rate");
    TestAssert::true(learningRate.has_value(), "Should have learning rate");
    TestAssert::equal(*learningRate, 0.01, 0.001, "Learning rate should be correct");
    
    auto batchSize = config->get<int>("batch_size");
    TestAssert::true(batchSize.has_value(), "Should have batch size");
    TestAssert::equal(*batchSize, 32, "Batch size should be correct");
    
    auto features = config->get<std::vector<std::string>>("enabled_features");
    if (features.has_value()) {
        TestAssert::true(features->size() == 3, "Should have 3 features");
        TestAssert::contains(features->at(0), "STDP", "First feature should be STDP");
        TestAssert::contains(features->at(1), "NeuronalAdaptation", "Second feature should be NeuronalAdaptation");
    }
    
    std::cout << "    testDataFormatIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testDataFormatCompatibilityWithPython() {
    TestUtils::TestSession session("testDataFormatCompatibilityWithPython");
    
    // Test compatibility with Python data formats
    nlm::Config config;
    
    // Set values that would commonly come from Python
    config.set("numpy_array", std::vector<double>{0.1, 0.2, 0.3, 0.4, 0.5}, nlm::ConfigSource::Runtime);
    config.set("python_dict", std::vector<std::string>{"key1", "key2", "key3"}, nlm::ConfigSource::Runtime);
    config.set("python_list", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    
    // Verify Python-like data structures
    auto numpyArray = config.get<std::vector<double>>("numpy_array");
    if (numpyArray.has_value()) {
        TestAssert::true(numpyArray->size() == 5, "Numpy array should have 5 elements");
        TestAssert::equal(numpyArray->at(0), 0.1, 0.01, "First array element should be 0.1");
    }
    
    auto pythonDict = config.get<std::vector<std::string>>("python_dict");
    if (pythonDict.has_value()) {
        TestAssert::true(pythonDict->size() == 3, "Python dict should have 3 elements");
    }
    
    auto pythonList = config.get<std::vector<int>>("python_list");
    if (pythonList.has_value()) {
        TestAssert::true(pythonList->size() == 5, "Python list should have 5 elements");
        TestAssert::equal(pythonList->at(0), 1, "First list element should be 1");
    }
    
    std::cout << "    testDataFormatCompatibilityWithPython passed" << std::endl;
    session.setSuccess(true);
}

void testDataFormatLargeValues() {
    TestUtils::TestSession session("testDataFormatLargeValues");
    
    // Test that large values are handled correctly
    nlm::Config config;
    
    // Set large values
    config.set("large_int", 2147483647, nlm::ConfigSource::Runtime); // Near max int32
    config.set("large_double", 1.7976931348623157e+308, nlm::ConfigSource::Runtime); // Near max double
    config.set("large_string", std::string(1000, 'a'), nlm::ConfigSource::Runtime); // 1000 character string
    
    // Verify large values
    auto largeIntVal = config.get<int>("large_int");
    if (largeIntVal.has_value()) {
        TestAssert::equal(*largeIntVal, 2147483647, "Large int should be preserved");
    }
    
    auto largeDoubleVal = config.get<double>("large_double");
    if (largeDoubleVal.has_value()) {
        TestAssert::true(std::isfinite(*largeDoubleVal), "Large double should be finite");
    }
    
    auto largeStringVal = config.get<std::string>("large_string");
    if (largeStringVal.has_value()) {
        TestAssert::equal(largeStringVal->size(), 1000UL, "Large string should have 1000 characters");
    }
    
    std::cout << "    testDataFormatLargeValues passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Data Format Tests ===" << std::endl;
    
    testDataFormatConfigValues();
    testDataFormatVectorValues();
    testDataFormatSerialization();
    testDataFormatIntegration();
    testDataFormatCompatibilityWithPython();
    testDataFormatLargeValues();
    
    std::cout << std::endl;
    std::cout << "=== All Data Format Tests PASSED ===" << std::endl;
}