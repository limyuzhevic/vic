// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(config.getSectionNames().empty());
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    // Test nested configuration
    auto& memorySection = config.addSection("memory", "Memory system configuration");
    memorySection.set<size_t>("capacity", 1000, "Maximum number of memory traces");
    memorySection.set<float>("decay_rate", 0.01f, "Decay rate for memory traces");
    
    auto& neuromodulationSection = config.addSection("neuromodulation", "Neuromodulation system configuration");
    neuromodulationSection.set<float>("dopamine_base", 0.1f, "Base dopamine level");
    neuromodulationSection.set<bool>("reward_modulation_enabled", true, "Enable reward-based neuromodulation");
    
    // Test getting from sections
    auto capacity = memorySection.get<size_t>("capacity");
    assert(capacity == 1000);
    
    auto decayRate = memorySection.get<float>("decay_rate");
    assert(decayRate == 0.01f);
    
    auto dopamine = neuromodulationSection.get<float>("dopamine_base");
    assert(dopamine == 0.1f);
    
    auto rewardModEnabled = neuromodulationSection.get<bool>("reward_modulation_enabled");
    assert(rewardModEnabled == true);
    
    // Test backward compatibility - flat set/get
    config.set("int_value", 42);
    auto intVal = config.get<int>("int_value");
    assert(intVal == 42);
    
    config.set("double_value", 3.14);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal > 3.13 && doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal == "hello");
    
    config.set("bool_value", true);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    // Test getOr from sections
    auto& memorySection = config.addSection("memory");
    memorySection.set<size_t>("capacity", 500);
    
    auto capacity = memorySection.get<size_t>("capacity");
    assert(capacity == 500);
    
    // Test missing key
    auto missingCapacity = memorySection.getOr<size_t>("missing_capacity", 1000);
    assert(missingCapacity == 1000);
    
    // Test backward compatibility - flat getOr
    auto val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    auto& memorySection = config.addSection("memory");
    memorySection.set<size_t>("capacity", 1000);
    memorySection.set<float>("decay_rate", 0.01f);
    
    assert(memorySection.has("capacity"));
    assert(memorySection.has("decay_rate"));
    
    memorySection.set("capacity", 500);  // Update existing
    assert(memorySection.get<size_t>("capacity") == 500);
    
    // Remove from section
    memorySection.remove("decay_rate");
    assert(!memorySection.has("decay_rate"));
    
    // Test backward compatibility - flat remove
    config.set("test", 123);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    // Add sections with different keys
    auto& memorySection = config.addSection("memory");
    memorySection.set<size_t>("capacity", 1000);
    memorySection.set<float>("decay_rate", 0.01f);
    
    auto& neuromodulationSection = config.addSection("neuromodulation");
    neuromodulationSection.set<float>("dopamine_base", 0.1f);
    neuromodulationSection.set<bool>("reward_modulation_enabled", true);
    
    auto& systemSection = config.addSection("system");
    systemSection.set<size_t>("random_seed", 42);
    
    // Test getKeys from flat config
    auto keys = config.getKeys();
    // Should have keys from all sections
    assert(!keys.empty());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    auto& memorySection = config.addSection("memory");
    
    // Test with integer validator (min=0, max=1000)
    auto validator = std::make_shared<nlm::IntegerValidator>(0, 1000);
    memorySection.set<size_t>("capacity", 500, "Capacity", validator, true);
    
    assert(memorySection.get<size_t>("capacity") == 500);
    
    // Test validation failure
    bool caught = false;
    try {
        auto invalidValidator = std::make_shared<nlm::IntegerValidator>(-100, 100);
        memorySection.set<size_t>("invalid", -50, "Invalid capacity", invalidValidator, true);
    } catch (const std::runtime_error& e) {
        caught = true;
    }
    assert(caught);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testDefaultConfig() {
    nlm::Config config = nlm::Config::createDefaultConfig();
    
    // Check that default config has all sections
    auto sectionNames = config.getSectionNames();
    assert(sectionNames.size() >= 4);  // memory, neuromodulation, prediction, plasticity at least
    
    // Check that system section has random seed
    assert(config.has<size_t>("random_seed"));
    assert(config.get<size_t>("random_seed") == 42);
    
    // Check that system section has simulation timestep
    assert(config.has<double>("simulation_timestep"));
    assert(config.get<double>("simulation_timestep") == 0.001);
    
    std::cout << "    testDefaultConfig passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    auto& memorySection = config.addSection("memory");
    memorySection.set<size_t>("capacity", 1000);
    memorySection.set<float>("decay_rate", 0.01f);
    
    auto& neuromodulationSection = config.addSection("neuromodulation");
    neuromodulationSection.set<float>("dopamine_base", 0.1f);
    neuromodulationSection.set<bool>("reward_modulation_enabled", true);
    
    // Generate summary
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("memory") != std::string::npos);
    assert(summary.find("neuromodulation") != std::string::npos);
    assert(summary.find("1000") != std::string::npos);
    assert(summary.find("0.1f") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set up some test configuration
    config.set("int_value", 42);
    config.set("string_value", "test");
    config.set("bool_value", true);
    
    // Save to file
    std::string testFile = "test_config.json";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    
    // Verify values are loaded
    assert(config2.get<int>("int_value") == 42);
    assert(config2.get<std::string>("string_value") == "test");
    assert(config2.get<bool>("bool_value") == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testDefaultConfig();
    testConfigSummary();
    testConfigFileIO();
}

} // namespace test_config
