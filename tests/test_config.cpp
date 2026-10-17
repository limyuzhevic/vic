#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

class ConfigTest {
public:
    ConfigTest() {
        std::cout << "Running NLM Configuration System Tests..." << std::endl;
        runAllTests();
        std::cout << "All tests passed! ✓" << std::endl;
    }
    
    void runAllTests() {
        testBasicConfig();
        testTypeConversion();
        testValidation();
        testPresets();
        testFileIO();
        testMerging();
        testSearchAndFilter();
        testStatistics();
        testAdvancedFeatures();
        std::cout << "All " << getTestCount() << " tests passed!" << std::endl;
    }
    
private:
    void testBasicConfig() {
        std::cout << "Test 1: Basic configuration..." << std::endl;
        Config config;
        
        // Set values
        config.set("int_value", 42);
        config.set("double_value", 3.14);
        config.set("string_value", "test");
        config.set("bool_value", true);
        
        // Get values
        assert(config.get<int>("int_value").value() == 42);
        assert(config.get<double>("double_value").value() == 3.14);
        assert(config.get<std::string>("string_value").value() == "test");
        assert(config.get<bool>("bool_value").value() == true);
        
        // Test getOr with defaults
        assert(config.getOr<int>("missing_key", 100) == 100);
        assert(config.getOr<double>("missing_key", 2.71) == 2.71);
        assert(config.getOr<std::string>("missing_key", "default") == "default");
        assert(config.getOr<bool>("missing_key", false) == false);
        
        // Test has
        assert(config.has("int_value"));
        assert(config.has("double_value"));
        assert(config.has("string_value"));
        assert(config.has("bool_value"));
        assert(!config.has("missing_key"));
        
        // Test remove
        config.remove("int_value");
        assert(!config.has("int_value"));
        assert(config.get<int>("int_value").has_value() == false);
        
        incrementTestCount();
    }
    
    void testTypeConversion() {
        std::cout << "Test 2: Type conversion..." << std::endl;
        Config config;
        
        // Test all types
        config.set("int", 123);
        config.set("int64", static_cast<int64_t>(1234567890));
        config.set("double", 3.14159);
        config.set("bool_true", true);
        config.set("bool_false", false);
        config.set("string", "hello world");
        
        // Verify types
        assert(config.get<int>("int").value() == 123);
        assert(config.get<int64_t>("int64").value() == 1234567890);
        assert(config.get<double>("double").value() > 3.14 && config.get<double>("double").value() < 3.15);
        assert(config.get<bool>("bool_true").value() == true);
        assert(config.get<bool>("bool_false").value() == false);
        assert(config.get<std::string>("string").value() == "hello world");
        
        incrementTestCount();
    }
    
    void testValidation() {
        std::cout << "Test 3: Configuration validation..." << std::endl;
        Config config;
        
        // Valid configuration
        config.set("neuron_count", 1000, "Number of neurons", ConfigSource::Default, "range:1,10000", "100");
        config.set("region_count", 1, "Number of regions", ConfigSource::Default, "range:1,10", "1");
        config.set("connection_probability", 0.1f, "Connection probability", ConfigSource::Default, "range:0,1", "0.1");
        
        // Test validation
        auto validationResult = config.validate();
        assert(validationResult.isValid);
        
        incrementTestCount();
    }
    
    void testPresets() {
        std::cout << "Test 4: Configuration presets..." << std::endl;
        
        // Test creating and loading presets
        Config::createDefaultPreset();
        Config::createPerformancePreset();
        Config::createAccuracyPreset();
        Config::createMinimalPreset();
        
        std::vector<std::string> presets = Config::getAvailablePresets();
        assert(std::find(presets.begin(), presets.end(), "default") != presets.end());
        assert(std::find(presets.begin(), presets.end(), "performance") != presets.end());
        assert(std::find(presets.begin(), presets.end(), "accuracy") != presets.end());
        assert(std::find(presets.begin(), presets.end(), "minimal") != presets.end());
        
        incrementTestCount();
    }
    
    void testFileIO() {
        std::cout << "Test 5: File I/O..." << std::endl;
        Config config;
        
        // Add test data
        config.set("int", 42);
        config.set("double", 3.14);
        config.set("string", "test_string");
        config.set("bool", true);
        
        // Save to file
        std::string testFile = "test_config.cfg";
        bool saved = config.saveToFile(testFile);
        assert(saved);
        
        // Load from file
        Config loadedConfig;
        bool loaded = loadedConfig.loadFromFile(testFile);
        assert(loaded);
        
        // Verify data
        assert(loadedConfig.get<int>("int").value() == 42);
        assert(loadedConfig.get<double>("double").value() > 3.13 && loadedConfig.get<double>("double").value() < 3.15);
        assert(loadedConfig.get<std::string>("string").value() == "test_string");
        assert(loadedConfig.get<bool>("bool").value() == true);
        
        // Clean up
        std::remove(testFile.c_str());
        
        incrementTestCount();
    }
    
    void testMerging() {
        std::cout << "Test 6: Configuration merging..." << std::endl;
        Config config1;
        Config config2;
        
        config1.set("int", 1, ConfigSource::Default);
        config1.set("double", 1.1, ConfigSource::Default);
        config1.set("string", "from_config1", ConfigSource::Default);
        
        config2.set("double", 2.2, ConfigSource::File);
        config2.set("string", "from_config2", ConfigSource::File);
        config2.set("bool", true, ConfigSource::File);
        
        // Merge config2 into config1
        config1.merge(config2);
        
        // Check merged results
        assert(config1.get<int>("int").value() == 1);  // From config1
        assert(config1.get<double>("double").value() == 2.2);  // From config2 (overwrites)
        assert(config1.get<std::string>("string").value() == "from_config2");  // From config2
        assert(config1.get<bool>("bool").value() == true);  // From config2
        
        incrementTestCount();
    }
    
    void testSearchAndFilter() {
        std::cout << "Test 7: Search and filtering..." << std::endl;
        Config config;
        
        // Add test data
        config.set("neuron_count", 100);
        config.set("region_count", 1);
        config.set("connection_probability", 0.1);
        config.set("stdp_ltp_weight", 0.01);
        config.set("stdp_ltd_weight", 0.012);
        config.set("stdp_tau", 20.0);
        config.set("synaptogenesis_rate", 0.0001);
        config.set("pruning_rate", 0.00001);
        
        // Test search for keys containing "neuron"
        auto neuronKeys = config.findKeysContaining("neuron");
        assert(neuronKeys.size() == 1);
        assert(neuronKeys[0] == "neuron_count");
        
        // Test search for keys containing "syn"
        auto synKeys = config.findKeysContaining("syn");
        assert(synKeys.size() >= 3);  // synaptogenesis_rate, pruning_rate, etc.
        
        // Test get keys by source
        auto defaultKeys = config.getKeysBySource(ConfigSource::Default);
        assert(defaultKeys.size() >= 8);  // All keys should be Default
        
        incrementTestCount();
    }
    
    void testStatistics() {
        std::cout << "Test 8: Statistics..." << std::endl;
        Config config;
        
        // Add some test data
        for (int i = 0; i < 10; ++i) {
            config.set("key_" + std::to_string(i), i);
        }
        
        // Get statistics
        auto stats = config.getStatistics();
        assert(stats.totalEntries == 10);
        assert(stats.uniqueKeys == 10);
        assert(stats.entriesBySource[ConfigSource::Default] == 10);
        
        // Test empty config
        Config emptyConfig;
        auto emptyStats = emptyConfig.getStatistics();
        assert(emptyStats.totalEntries == 0);
        assert(emptyStats.uniqueKeys == 0);
        
        incrementTestCount();
    }
    
    void testAdvancedFeatures() {
        std::cout << "Test 9: Advanced features..." << std::endl;
        Config config;
        
        // Test command line loading
        int argc = 3;
        char* argv[] = { (char*)"test", (char*)"--test_key", (char*)"test_value" };
        bool loaded = config.loadFromArgs(argc, argv);
        assert(loaded);
        
        // Test string loading
        std::string configString = "key1 = value1\nkey2 = 42\nkey3 = true";
        bool stringLoaded = config.loadFromString(configString);
        assert(stringLoaded);
        
        // Test validation ranges
        config.set("test_range", 50, "Test range", ConfigSource::Runtime, "range:0,100");
        
        incrementTestCount();
    }
    
    void incrementTestCount() {
        testCount++;
    }
    
    int getTestCount() const {
        return testCount;
    }
    
    int testCount = 0;
};

int main() {
    try {
        ConfigTest tester;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}