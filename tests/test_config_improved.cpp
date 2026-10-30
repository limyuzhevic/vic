// Config Test
// Test the improved Config system with JSON/YAML parsing and robust functionality

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testBasicFunctionality() {
    std::cout << "  Testing basic Config functionality..." << std::endl;
    nlm::Config config;
    
    // Test creation
    assert(!config.has("test"));
    
    // Test setting different types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test getting values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    Basic functionality test passed" << std::endl;
}

void testJSONParsing() {
    std::cout << "  Testing JSON parsing..." << std::endl;
    
    // Create a temporary JSON file
    std::string testContent = R"({
        "name": "test brain",
        "neuron_count": 1000,
        "region_count": 5,
        "connection_probability": 0.1,
        "enable_plasticity": true,
        "learning_rate": 0.01,
        "parameters": [1.0, 2.0, 3.0, 4.0, 5.0],
        "description": "JSON test configuration"
    })";
    
    std::ofstream jsonFile("test_config.json");
    if (jsonFile.is_open()) {
        jsonFile << testContent;
        jsonFile.close();
        
        nlm::Config config;
        bool success = config.loadFromFile("test_config.json");
        assert(success);
        
        // Verify JSON parsing worked
        assert(config.has("name"));
        auto name = config.get<std::string>("name");
        assert(name.has_value() && *name == "test brain");
        
        assert(config.has("neuron_count"));
        auto neuronCount = config.get<int64_t>("neuron_count");
        assert(neuronCount.has_value() && *neuronCount == 1000);
        
        assert(config.has("region_count"));
        auto regionCount = config.get<int64_t>("region_count");
        assert(regionCount.has_value() && *regionCount == 5);
        
        assert(config.has("connection_probability"));
        auto connProb = config.get<double>("connection_probability");
        assert(connProb.has_value() && *connProb == 0.1);
        
        assert(config.has("enable_plasticity"));
        auto plasticity = config.get<bool>("enable_plasticity");
        assert(plasticity.has_value() && *plasticity == true);
        
        assert(config.has("learning_rate"));
        auto learningRate = config.get<double>("learning_rate");
        assert(learningRate.has_value() && *learningRate == 0.01);
        
        // Test array parsing
        assert(config.has("parameters"));
        // Note: Our JSON parser converts arrays to std::vector<double>
        
        // Test description
        assert(config.has("description"));
        auto description = config.get<std::string>("description");
        assert(description.has_value() && *description == "JSON test configuration");
        
        std::cout << "    JSON parsing test passed" << std::endl;
        
        // Clean up
        std::filesystem::remove("test_config.json");
    }
}

void testSaveToFile() {
    std::cout << "  Testing save to file..." << std::endl;
    
    nlm::Config config;
    
    // Set up test data
    config.set("brain_name", std::string("TestBrain"), nlm::ConfigSource::Runtime);
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("enable_plasticity", true, nlm::ConfigSource::Runtime);
    config.set("parameters", std::vector<double>({1.0, 2.0, 3.0, 4.0, 5.0}), nlm::ConfigSource::Runtime);
    config.set("description", std::string("Test configuration for save functionality"), nlm::ConfigSource::Runtime);
    
    // Save to file
    bool success = config.saveToFile("test_save_config.json");
    assert(success);
    
    // Verify the file was created and contains expected data
    std::ifstream file("test_save_config.json");
    assert(file.is_open());
    
    std::string line;
    bool foundBrainName = false;
    bool foundNeuronCount = false;
    bool foundLearningRate = false;
    bool foundPlasticity = false;
    bool foundParameters = false;
    
    while (std::getline(file, line)) {
        if (line.find("brain_name") != std::string::npos) {
            foundBrainName = true;
        }
        if (line.find("neuron_count") != std::string::npos) {
            foundNeuronCount = true;
        }
        if (line.find("learning_rate") != std::string::npos) {
            foundLearningRate = true;
        }
        if (line.find("enable_plasticity") != std::string::npos) {
            foundPlasticity = true;
        }
        if (line.find("parameters") != std::string::npos) {
            foundParameters = true;
        }
    }
    
    assert(foundBrainName);
    assert(foundNeuronCount);
    assert(foundLearningRate);
    assert(foundPlasticity);
    assert(foundParameters);
    
    file.close();
    
    std::cout << "    Save to file test passed" << std::endl;
    
    // Clean up
    std::filesystem::remove("test_save_config.json");
}

void testCommandLineParsing() {
    std::cout << "  Testing command line argument parsing..." << std::endl;
    
    // Simulate command line arguments
    const char* argv[] = {
        "test_program",
        "--name", "Alice",
        "--age", "30",
        "--learning_rate", "0.05",
        "-enabled", "true"
    };
    
    nlm::Config config;
    bool success = config.loadFromArgs(5, const_cast<char**>(argv));
    assert(success);
    
    // Verify parsing
    assert(config.has("name"));
    auto name = config.get<std::string>("name");
    assert(name.has_value() && *name == "Alice");
    
    assert(config.has("age"));
    auto age = config.get<int>("age");
    assert(age.has_value() && *age == 30);
    
    assert(config.has("learning_rate"));
    auto lr = config.get<double>("learning_rate");
    assert(lr.has_value() && *lr == 0.05);
    
    assert(config.has("enabled"));
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value() && *enabled == true);
    
    std::cout << "    Command line parsing test passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "  Testing configuration summary..." << std::endl;
    
    nlm::Config config;
    
    // Add test data
    config.set("key1", 42, nlm::ConfigSource::Runtime);
    config.set("key2", std::string("test string"), nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    config.set("key4", false, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    
    // Verify summary contains expected information
    assert(summary.find("Configuration (4 entries):") != std::string::npos);
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    assert(summary.find("key4") != std::string::npos);
    
    std::cout << "    Configuration summary test passed" << std::endl;
}

void testConfigGetOr() {
    std::cout << "  Testing getOr functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test with missing key
    int val1 = config.getOr<int>("missing", 100);
    assert(val1 == 100);
    
    // Test with existing key
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    int val2 = config.getOr<int>("existing", 100);
    assert(val2 == 50);
    
    // Test with different types
    config.set("name", std::string("Alice"), nlm::ConfigSource::Runtime);
    std::string val3 = config.getOr<std::string>("name", "Bob");
    assert(val3 == "Alice");
    
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    bool val4 = config.getOr<bool>("enabled", false);
    assert(val4 == true);
    
    double val5 = config.getOr<double>("speed", 1.5);
    assert(val5 == 1.5);
    
    std::cout << "    getOr functionality test passed" << std::endl;
}

void runAll() {
    std::cout << "Running improved Config system tests..." << std::endl;
    std::cout << "=" * 50 << std::endl;
    
    testBasicFunctionality();
    testJSONParsing();
    testSaveToFile();
    testCommandLineParsing();
    testConfigSummary();
    testConfigGetOr();
    
    std::cout << "=" * 50 << std::endl;
    std::cout << "All Config system tests passed!" << std::endl;
}

} // namespace test_config

int main() {
    test_config::runAll();
    return 0;
}
