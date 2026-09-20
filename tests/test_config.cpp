// Config Tests
#include "core/Config/Config.hpp"
#include "core/Config/ConfigParser.hpp"
#include <cassert>
#include <iostream>
#include <nlohmann/json.hpp>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigLoadFromFile() {
    nlm::Config config;
    
    // Test loading from JSON file
    // Create a temporary JSON file
    std::string jsonContent = R"({
        "learning_rate": 0.01,
        "plasticity": true,
        "synapses": [10, 20, 30],
        "neuron": {
            "count": 500,
            "region": {
                "count": 1,
                "type": "cortex"
            }
        }
    })";
    
    // Write to a temporary file
    const std::string testFile = "test_config.json";
    std::ofstream file(testFile);
    file << jsonContent;
    file.close();
    
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    // Check that values were loaded
    auto lr = config.get<double>("learning_rate");
    assert(lr.has_value());
    assert(abs(*lr - 0.01) < 0.001);
    
    auto plasticity = config.get<bool>("plasticity");
    assert(plasticity.has_value());
    assert(*plasticity == true);
    
    auto synapses = config.get<std::vector<int>>("synapses");
    assert(synapses.has_value());
    assert(synapses->size() == 3);
    assert((*synapses)[0] == 10);
    
    auto neuronCount = config.get<int>("neuron.count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 500);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigLoadFromArgs() {
    nlm::Config config;
    
    // Test command line argument parsing
    const char* argv[] = {
        "program_name",
        "--learning_rate", "0.05",
        "--plasticity", "true",
        "-synapses", "1,2,3"
    };
    
    int argc = 6;
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    auto lr = config.get<double>("learning_rate");
    assert(lr.has_value());
    assert(abs(*lr - 0.05) < 0.001);
    
    auto plasticity = config.get<bool>("plasticity");
    assert(plasticity.has_value());
    assert(*plasticity == true);
    
    auto synapses = config.get<std::string>("synapses");
    assert(synapses.has_value());
    assert(*synapses == "1,2,3");
    
    std::cout << "    testConfigLoadFromArgs passed" << std::endl;
}

void testConfigFileFormatDetection() {
    std::string jsonContent = "{\"key\": \"value\"}";
    std::string yamlContent = "key: value\n  nested: true";
    std::string simpleContent = "key = value\nnumber = 42";
    
    nlm::FileFormatDetector::Format jsonFormat = nlm::FileFormatDetector::detectFromContent(jsonContent);
    assert(jsonFormat == nlm::FileFormatDetector::Format::JSON);
    
    nlm::FileFormatDetector::Format yamlFormat = nlm::FileFormatDetector::detectFromContent(yamlContent);
    assert(yamlFormat == nlm::FileFormatDetector::Format::YAML);
    
    nlm::FileFormatDetector::Format simpleFormat = nlm::FileFormatDetector::detectFromContent(simpleContent);
    assert(simpleFormat == nlm::FileFormatDetector::Format::SimpleKeyValue);
    
    std::cout << "    testConfigFileFormatDetection passed" << std::endl;
}

void testConfigParserManager() {
    nlm::ConfigParserManager manager;
    
    // Get parser names
    auto parserNames = manager.getParserNames();
    assert(!parserNames.empty());
    
    // Test JSON parsing
    std::string jsonContent = "{\"test\": \"value\"}";
    std::vector<std::pair<std::string, nlm::ConfigValue>> entries;
    
    bool parsed = manager.parseFromContent(jsonContent, entries);
    assert(parsed);
    assert(entries.size() > 0);
    
    // Test backward compatibility with simple key=value
    std::string simpleContent = "test = value\nnumber = 42";
    entries.clear();
    
    parsed = manager.parseFromContent(simpleContent, entries);
    assert(parsed);
    assert(entries.size() >= 2);
    
    std::cout << "    testConfigParserManager passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromFile();
    testConfigLoadFromArgs();
    testConfigFileFormatDetection();
    testConfigParserManager();
}

} // namespace test_config