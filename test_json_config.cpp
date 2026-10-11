// JSON Configuration Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_json_config {

void testJSONFileParsing() {
    nlm::Config config;
    
    // Load from JSON file
    std::string jsonPath = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_9233f00d-377c-4e48-b26b-d98a2f247e8d/config_enhanced.json";
    bool loadResult = config.loadFromFile(jsonPath);
    assert(loadResult);
    
    // Check that values were loaded
    assert(config.has("neuronCount"));
    assert(config.has("learningRate"));
    assert(config.has("timeSteps"));
    assert(config.has("stdpLTPWeight"));
    assert(config.has("stdpLTDWeight"));
    assert(config.has("stdpTimeConstant"));
    assert(config.has("plasticityLearningRate"));
    assert(config.has("threadCount"));
    assert(config.has("simdEnabled"));
    assert(config.has("memoryPoolSize"));
    assert(config.has("debugLevel"));
    assert(config.has("profilingEnabled"));
    assert(config.has("validationEnabled"));
    assert(config.has("synapticDensity"));
    assert(config.has("populationSize"));
    assert(config.has("regionSize"));
    assert(config.has("neuronTypes"));
    
    // Verify specific values
    auto neuronCount = config.get<int>("neuronCount");
    assert(neuronCount.has_value());
    assert(*neuronCount == 5000);
    
    auto learningRate = config.get<double>("learningRate");
    assert(learningRate.has_value());
    assert(*learningRate == 0.05);
    
    auto timeSteps = config.get<int>("timeSteps");
    assert(timeSteps.has_value());
    assert(*timeSteps == 200);
    
    auto stdpLTPWeight = config.get<double>("stdpLTPWeight");
    assert(stdpLTPWeight.has_value());
    assert(*stdpLTPWeight == 0.1);
    
    auto stdpLTDWeight = config.get<double>("stdpLTDWeight");
    assert(stdpLTDWeight.has_value());
    assert(*stdpLTDWeight == 0.12);
    
    auto stdpTimeConstant = config.get<double>("stdpTimeConstant");
    assert(stdpTimeConstant.has_value());
    assert(*stdpTimeConstant == 20.0);
    
    auto plasticityLearningRate = config.get<double>("plasticityLearningRate");
    assert(plasticityLearningRate.has_value());
    assert(*plasticityLearningRate == 0.05);
    
    auto threadCount = config.get<int>("threadCount");
    assert(threadCount.has_value());
    assert(*threadCount == 8);
    
    auto simdEnabled = config.get<bool>("simdEnabled");
    assert(simdEnabled.has_value());
    assert(*simdEnabled == true);
    
    auto memoryPoolSize = config.get<int>("memoryPoolSize");
    assert(memoryPoolSize.has_value());
    assert(*memoryPoolSize == 131072);
    
    auto debugLevel = config.get<int>("debugLevel");
    assert(debugLevel.has_value());
    assert(*debugLevel == 2);
    
    auto profilingEnabled = config.get<bool>("profilingEnabled");
    assert(profilingEnabled.has_value());
    assert(*profilingEnabled == false);
    
    auto validationEnabled = config.get<bool>("validationEnabled");
    assert(validationEnabled.has_value());
    assert(*validationEnabled == true);
    
    auto synapticDensity = config.get<double>("synapticDensity");
    assert(synapticDensity.has_value());
    assert(*synapticDensity == 0.15);
    
    auto populationSize = config.get<int>("populationSize");
    assert(populationSize.has_value());
    assert(*populationSize == 200);
    
    auto regionSize = config.get<double>("regionSize");
    assert(regionSize.has_value());
    assert(*regionSize == 2.0);
    
    auto neuronTypes = config.get<std::string>("neuronTypes");
    assert(neuronTypes.has_value());
    assert(*neuronTypes == "excitatory,inhibitory,modulatory");
    
    // Test summary includes loaded values
    std::string summary = config.summary();
    assert(summary.find("5000") != std::string::npos);
    assert(summary.find("0.05") != std::string::npos);
    assert(summary.find("8") != std::string::npos);
    
    std::cout << "    testJSONFileParsing passed" << std::endl;
}

void testJSONFileFormat() {
    nlm::Config config;
    
    // Test malformed JSON (should fail gracefully)
    std::string malformedPath = "/tmp/malformed.json";
    std::ofstream malformedFile(malformedPath);
    malformedFile << "{ invalid json }";
    malformedFile.close();
    
    bool loadMalformed = config.loadFromFile(malformedPath);
    // Should handle gracefully and return false
    assert(!loadMalformed);
    
    // Clean up
    std::filesystem::remove(malformedPath);
    
    std::cout << "    testJSONFileFormat passed" << std::endl;
}

void testJSONSaveAndLoad() {
    nlm::Config config;
    
    // Set some values
    config.set("neuronCount", 2000, nlm::ConfigSource::Runtime);
    config.set("learningRate", 0.02, nlm::ConfigSource::Runtime);
    config.set("debugLevel", 1, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string jsonPath = "/tmp/saved_config.json";
    bool saveResult = config.saveToFile(jsonPath);
    assert(saveResult);
    
    // Load from JSON file
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(jsonPath);
    assert(loadResult);
    
    // Verify values
    assert(config2.has("neuronCount"));
    assert(config2.has("learningRate"));
    assert(config2.has("debugLevel"));
    
    auto neuronCount = config2.get<int>("neuronCount");
    assert(neuronCount.has_value() && *neuronCount == 2000);
    
    auto learningRate = config2.get<double>("learningRate");
    assert(learningRate.has_value() && *learningRate == 0.02);
    
    auto debugLevel = config2.get<int>("debugLevel");
    assert(debugLevel.has_value() && *debugLevel == 1);
    
    // Clean up
    std::filesystem::remove(jsonPath);
    
    std::cout << "    testJSONSaveAndLoad passed" << std::endl;
}

void testConfigCategoriesInJSON() {
    nlm::Config config;
    
    // Set values with different categories
    config.set("stdpLTPWeight", 0.1, nlm::ConfigSource::Runtime);    // Advanced
    config.set("threadCount", 4, nlm::ConfigSource::Runtime);       // Performance  
    config.set("debugLevel", 2, nlm::ConfigSource::Runtime);        // Development
    config.set("neuronCount", 1000, nlm::ConfigSource::Runtime);    // Neural
    config.set("timeSteps", 100, nlm::ConfigSource::Runtime);       // Basic
    
    // Save to JSON
    std::string jsonPath = "/tmp/categorized_config.json";
    bool saveResult = config.saveToFile(jsonPath);
    assert(saveResult);
    
    // Load from JSON
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(jsonPath);
    assert(loadResult);
    
    // Verify all values are present
    assert(config2.has("stdpLTPWeight"));
    assert(config2.has("threadCount"));
    assert(config2.has("debugLevel"));
    assert(config2.has("neuronCount"));
    assert(config2.has("timeSteps"));
    
    // Check summary includes categories
    std::string summary = config2.summary();
    assert(summary.find("Advanced") != std::string::npos || summary.find("0.1") != std::string::npos);
    
    // Clean up
    std::filesystem::remove(jsonPath);
    
    std::cout << "    testConfigCategoriesInJSON passed" << std::endl;
}

void runAll() {
    testJSONFileParsing();
    testJSONFileFormat();
    testJSONSaveAndLoad();
    testConfigCategoriesInJSON();
    
    std::cout << "\nAll JSON config tests passed!" << std::endl;
}

} // namespace test_json_config

int main() {
    std::cout << "Running JSON Configuration Tests..." << std::endl;
    test_json_config::runAll();
    return 0;
}
