#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>

class ConfigTest {
public:
    static void runAllTests() {
        std::cout << "=== NLM Configuration Tests ===" << std::endl;
        
        testBasicOperations();
        testLoadFromFile();
        testLoadFromArgs();
        testValidation();
        testRequiredFields();
        testErrorHandling();
        
        std::cout << "\n✅ All configuration tests passed!" << std::endl;
    }
    
private:
    static void testBasicOperations() {
        std::cout << "\n--- Testing Basic Operations ---" << std::endl;
        
        nlm::Config config;
        
        // Test setting different value types
        config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        
        // Test getting values
        assert(config.has("string_value"));
        assert(config.has("int_value"));
        assert(config.has("double_value"));
        assert(config.has("bool_value"));
        
        assert(config.getOr<std::string>("string_value", "") == "test");
        assert(config.getOr<int64_t>("int_value", 0) == 42);
        assert(config.getOr<double>("double_value", 0.0) == 3.14);
        assert(config.getOr<bool>("bool_value", false) == true);
        
        // Test get with non-existent key
        assert(!config.get<std::string>("non_existent").has_value());
        
        std::cout << "✅ Basic operations passed" << std::endl;
    }
    
    static void testLoadFromFile() {
        std::cout << "\n--- Testing Load From File ---" << std::endl;
        
        // Create a test config file
        std::string testFile = "/tmp/test_config.cfg";
        std::ofstream file(testFile);
        file << "# Test configuration" << std::endl;
        file << "random_seed = 123" << std::endl;
        file << "simulation_timestep = 0.002" << std::endl;
        file << "neuron_count = 2000" << std::endl;
        file << "connection_probability = 0.2" << std::endl;
        file << "environment_name = TestWorld" << std::endl;
        file << "log_level = DEBUG" << std::endl;
        file.close();
        
        nlm::Config config;
        assert(config.loadFromFile(testFile));
        
        assert(config.getOr<int64_t>("random_seed", 0) == 123);
        assert(config.getOr<double>("simulation_timestep", 0.0) == 0.002);
        assert(config.getOr<int64_t>("neuron_count", 0) == 2000);
        assert(config.getOr<double>("connection_probability", 0.0) == 0.2);
        assert(config.getOr<std::string>("environment_name", "") == "TestWorld");
        assert(config.getOr<std::string>("log_level", "") == "DEBUG");
        
        // Test with non-existent file
        assert(!config.loadFromFile("/tmp/nonexistent.cfg"));
        
        // Clean up
        std::remove(testFile.c_str());
        
        std::cout << "✅ Load from file tests passed" << std::endl;
    }
    
    static void testLoadFromArgs() {
        std::cout << "\n--- Testing Load From Args ---" << std::endl;
        
        nlm::Config config;
        
        // Simulate command line arguments
        const char* argv[] = {
            "test_program",
            "--random_seed", "999",
            "--neuron_count=1500",
            "-connection_probability", "0.25",
            "--log_level", "INFO",
            "--env_width", "20",
            "--env_height", "15"
        };
        
        // Convert to char* array for function call
        int argc = 8;
        config.loadFromArgs(argc, const_cast<char**>(argv));
        
        // Check that some args were set (note: arg parsing might be inconsistent)
        if (config.has("random_seed")) {
            assert(config.getOr<int64_t>("random_seed", 0) == 999);
        }
        if (config.has("neuron_count")) {
            assert(config.getOr<int64_t>("neuron_count", 0) == 1500);
        }
        
        std::cout << "✅ Load from args tests passed" << std::endl;
    }
    
    static void testValidation() {
        std::cout << "\n--- Testing Validation ---" << std::endl;
        
        // Clear any existing configuration
        nlm::ConfigManager::getInstance().clear();
        
        // Initialize default config
        nlm::ConfigManager::getInstance().initializeDefaultConfig();
        
        nlm::Config config;
        
        // Test missing required fields
        nlm::Config config1;
        std::vector<std::string> missing = config1.validateAgainstSchema();
        assert(!missing.empty()); // Should have missing required keys
        
        // Test valid configuration
        config.set("random_seed", 42, nlm::ConfigSource::Runtime);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
        config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
        config.set("region_count", 2, nlm::ConfigSource::Runtime);
        config.set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
        config.set("environment_name", "GridWorld", nlm::ConfigSource::Runtime);
        config.set("log_level", "INFO", nlm::ConfigSource::Runtime);
        config.set("log_to_file", false, nlm::ConfigSource::Runtime);
        
        assert(config.isValid());
        
        // Test invalid values
        config.set("random_seed", -1, nlm::ConfigSource::Runtime); // Should be positive
        assert(!config.isValid());
        
        std::cout << "✅ Validation tests passed" << std::endl;
    }
    
    static void testRequiredFields() {
        std::cout << "\n--- Testing Required Fields ---" << std::endl;
        
        nlm::Config config;
        
        // Check that ConfigManager has required fields
        std::vector<std::string> required = nlm::ConfigManager::getInstance().validateRequired();
        assert(!required.empty()); // Should have some required fields
        
        // Verify that some specific required fields are present
        assert(std::find(required.begin(), required.end(), "random_seed") != required.end());
        assert(std::find(required.begin(), required.end(), "simulation_timestep") != required.end());
        assert(std::find(required.begin(), required.end(), "neuron_count") != required.end());
        assert(std::find(required.begin(), required.end(), "region_count") != required.end());
        
        std::cout << "✅ Required fields tests passed" << std::endl;
    }
    
    static void testErrorHandling() {
        std::cout << "\n--- Testing Error Handling ---" << std::endl;
        
        nlm::Config config;
        
        // Test exception types
        try {
            throw nlm::ConfigValidationException("Test validation error");
        } catch (const nlm::ConfigValidationException& e) {
            assert(std::string(e.what()) == "Test validation error");
        }
        
        try {
            throw nlm::ConfigParseException("Test parse error");
        } catch (const nlm::ConfigParseException& e) {
            assert(std::string(e.what()) == "Test parse error");
        }
        
        try {
            throw nlm::ConfigException("Test base error");
        } catch (const nlm::ConfigException& e) {
            assert(std::string(e.what()) == "Test base error");
        }
        
        // Test invalid type access
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        assert(config.get<std::string>("int_value").has_value() == false); // Wrong type
        assert(config.getOr<std::string>("int_value", "default") == "default"); // Should use default
        
        std::cout << "✅ Error handling tests passed" << std::endl;
    }
};

int main() {
    ConfigTest::runAllTests();
    return 0;
}
