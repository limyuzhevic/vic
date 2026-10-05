#include <iostream>
#include <cassert>
#include "../src/core/Config/Config.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

void testJsonConfig() {
    std::cout << "Testing JSON configuration..." << std::endl;
    
    nlm::Config config;
    
    // Set some test values
    config.set("integer", 42, nlm::ConfigSource::Runtime);
    config.set("double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string", "Hello World", nlm::ConfigSource::Runtime);
    config.set("boolean", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int64_t>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Test saving to JSON
    bool saveSuccess = config.saveToFile("test_config.json");
    assert(saveSuccess && "Failed to save config to JSON");
    
    // Test loading from JSON
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("test_config.json");
    assert(loadSuccess && "Failed to load config from JSON");
    
    // Verify values
    auto intVal = config2.get<int64_t>("integer");
    assert(intVal.has_value() && intVal.value() == 42 && "JSON integer not preserved");
    
    auto doubleVal = config2.get<double>("double");
    assert(doubleVal.has_value() && std::abs(doubleVal.value() - 3.14159) < 0.0001 && "JSON double not preserved");
    
    auto stringVal = config2.get<std::string>("string");
    assert(stringVal.has_value() && stringVal.value() == "Hello World" && "JSON string not preserved");
    
    auto boolVal = config2.get<bool>("boolean");
    assert(boolVal.has_value() && boolVal.value() == true && "JSON boolean not preserved");
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "✓ JSON configuration test passed" << std::endl;
}

void testNeuromodulators() {
    std::cout << "Testing neuromodulators..." << std::endl;
    
    // Create neuromodulators
    nlm::Dopamine dopamine;
    nlm::Acetylcholine acetylcholine;
    nlm::Norepinephrine norepinephrine;
    nlm::Serotonin serotonin;
    
    // Test basic properties
    assert(std::string(dopamine.getName()) == "DA" && "Dopamine name incorrect");
    assert(std::string(acetylcholine.getName()) == "ACh" && "Acetylcholine name incorrect");
    assert(std::string(norepinephrine.getName()) == "NE" && "Norepinephrine name incorrect");
    assert(std::string(serotonin.getName()) == "5-HT" && "Serotonin name incorrect");
    
    // Test level setting
    dopamine.setLevel(0.5f);
    assert(dopamine.getLevel() >= 0.0f && dopamine.getLevel() <= 1.0f && "Dopamine level out of bounds");
    
    acetylcholine.setLevel(0.3f);
    assert(acetylcholine.getLevel() >= 0.0f && acetylcholine.getLevel() <= 1.0f && "Acetylcholine level out of bounds");
    
    norepinephrine.setLevel(0.7f);
    assert(norepinephrine.getLevel() >= 0.0f && norepinephrine.getLevel() <= 1.0f && "Norepinephrine level out of bounds");
    
    serotonin.setLevel(0.9f);
    assert(serotonin.getLevel() >= 0.0f && serotonin.getLevel() <= 1.0f && "Serotonin level out of bounds");
    
    // Test neuromodulation effects
    float dopaminePlasticity = dopamine.getPlasticityFactor();
    float acetylcholinePlasticity = acetylcholine.getPlasticityFactor();
    float norepinephrinePlasticity = norepinephrine.getPlasticityFactor();
    float serotoninPlasticity = serotonin.getPlasticityFactor();
    
    assert(dopaminePlasticity >= 0.1f && dopaminePlasticity <= 3.0f && "Dopamine plasticity factor out of bounds");
    assert(acetylcholinePlasticity >= 1.0f && acetylcholinePlasticity <= 3.0f && "Acetylcholine plasticity factor out of bounds");
    assert(norepinephrinePlasticity >= 0.1f && norepinephrinePlasticity <= 1.8f && "Norepinephrine plasticity factor out of bounds");
    assert(serotoninPlasticity >= 0.3f && serotoninPlasticity <= 2.0f && "Serotonin plasticity factor out of bounds");
    
    // Test update
    dopamine.update(0.001f);
    acetylcholine.update(0.001f);
    norepinephrine.update(0.001f);
    serotonin.update(0.001f);
    
    std::cout << "✓ Neuromodulator test passed" << std::endl;
}

int main() {
    testJsonConfig();
    testNeuromodulators();
    
    std::cout << std::endl << "All tests passed!" << std::endl;
    return 0;
}