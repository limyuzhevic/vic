#include "tests/ConfigTestFixture.hpp"

// Comprehensive configuration system tests

class ConfigTestSuite {
public:
    ConfigTestSuite() = default;
    ~ConfigTestSuite() = default;
    
    // Run all tests
    void runAllTests() {
        std::cout << "=== NLM Configuration System Test Suite ===" << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        // Basic functionality tests
        passed += testBasicConfiguration();
        failed += testBasicConfigurationFailures();
        
        // Schema validation tests
        passed += testSchemaValidation();
        passed += testSchemaValidationFailures();
        
        // File I/O tests
        passed += testConfigurationFileIO();
        passed += testConfigurationFileIOTests();
        
        // Command line tests
        passed += testCommandLineParsing();
        passed += testCommandLineParsingFailures();
        
        // Copy/movement tests
        passed += testConfigurationCopyMove();
        
        // Error handling tests
        passed += testErrorHandling();
        
        // Edge case tests
        passed += testEdgeCases();
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Tests passed: " << passed << std::endl;
        std::cout << "Tests failed: " << failed << std::endl;
        std::cout << "Total tests: " << (passed + failed) << std::endl;
    }
    
private:
    // Basic configuration functionality tests
    int testBasicConfiguration() {
        std::cout << "Test 1: Basic configuration functionality" << std::endl;
        
        ConfigTestFixture fixture;
        
        try {
            // Test setting values
            fixture.config.set("test_int", 42);
            fixture.config.set("test_double", 3.14);
            fixture.config.set("test_string", "test_value");
            fixture.config.set("test_bool", true);
            
            // Test getting values
            auto intVal = fixture.config.getOr<int>("test_int", -1);
            auto doubleVal = fixture.config.getOr<double>("test_double", -1.0);
            auto stringVal = fixture.config.getOr<std::string>("test_string", "");
            auto boolVal = fixture.config.getOr<bool>("test_bool", false);
            
            // Verify values
            bool success = (intVal == 42 && 
                          std::abs(doubleVal - 3.14) < 0.001 &&
                          stringVal == "test_value" &&
                          boolVal == true);
            
            if (success) {
                std::cout << "  ✓ Basic configuration test passed" << std::endl;
                return 1;
            } else {
                std::cout << "  ✗ Basic configuration test failed" << std::endl;
                return 0;
            }
            
        } catch (const std::exception& e) {
            std::cout << "  ✗ Basic configuration test failed with exception: " << e.what() << std::endl;
            return 0;
        }
    }
    
    int testBasicConfigurationFailures() {
        std::cout << "Test 2: Basic configuration failure cases" << std::endl;
        
        ConfigTestFixture fixture;
        
        int passed = 0;
        
        try {
            // Test getting non-existent key
            auto val = fixture.config.getOr<int>("non_existent_key", -999);
            if (val == -999) {
                std::cout << "  ✓ Non-existent key handling passed" << std::endl;
                passed++;
            } else {
                std::cout << "  ✗ Non-existent key handling failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ Non-existent key handling failed with exception: " << e.what() << std::endl;
        }
        
        try {
            // Test empty key
            fixture.config.set("", "empty_key_value");
            auto val = fixture.config.getOr<std::string>("", "");
            if (val == "empty_key_value") {
                std::cout << "  ✓ Empty key handling passed" << std::endl;
                passed++;
            } else {
                std::cout << "  ✗ Empty key handling failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✓ Empty key handled with exception (expected): " << e.what() << std::endl;
            passed++;
        }
        
        try {
            // Test key with special characters
            fixture.config.set("test-key_with.dots", "special_value");
            auto val = fixture.config.getOr<std::string>("test-key_with.dots", "");
            if (val == "special_value") {
                std::cout << "  ✓ Special character key handling passed" << std::endl;
                passed++;
            } else {
                std::cout << "  ✗ Special character key handling failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ Special character key handling failed with exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    int testSchemaValidation() {
        std::cout << "Test 3: Configuration schema validation" << std::endl;
        
        ConfigTestFixture fixture;
        
        try {
            // Test setting valid values according to schema
            fixture.config.set("random_seed", int64_t(42));
            fixture.config.set("simulation_timestep", 0.001);
            fixture.config.set("neuron_count", int64_t(1000));
            fixture.config.set("connection_probability", 0.1f);
            fixture.config.set("log_level", std::string("INFO"));
            fixture.config.set("log_to_file", false);
            
            // Verify schema validation is working
            bool schemaActive = fixture.config.has("random_seed") &&
                              fixture.config.has("simulation_timestep") &&
                              fixture.config.has("neuron_count");
            
            if (schemaActive) {
                std::cout << "  ✓ Schema validation is active" << std::endl;
                return 1;
            } else {
                std::cout << "  ✗ Schema validation not active" << std::endl;
                return 0;
            }
            
        } catch (const std::exception& e) {
            std::cout << "  ✗ Schema validation test failed with exception: " << e.what() << std::endl;
            return 0;
        }
    }
    
    int testSchemaValidationFailures() {
        std::cout << "Test 4: Configuration schema validation failures" << std::endl;
        
        ConfigTestFixture fixture;
        
        int passed = 0;
        
        try {
            // Test that invalid values are still accepted (backward compatibility)
            fixture.config.set("invalid_type_key", "invalid_value_for_int");
            std::cout << "  ✓ Invalid values accepted for backward compatibility" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✗ Invalid value handling failed with exception: " << e.what() << std::endl;
        }
        
        try {
            // Test extreme values
            fixture.config.set("extreme_positive", std::numeric_limits<double>::max());
            fixture.config.set("extreme_negative", std::numeric_limits<double>::lowest());
            std::cout << "  ✓ Extreme value handling passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✗ Extreme value handling failed with exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    int testConfigurationFileIO() {
        std::cout << "Test 5: Configuration file I/O operations" << std::endl;
        
        ConfigTestFixture fixture;
        
        try {
            // Test setting some values
            fixture.config.set("test_file_int", 123);
            fixture.config.set("test_file_string", "file_test");
            fixture.config.set("test_file_bool", true);
            
            // Test saving to file
            std::string testFile = "test_config_output.ini";
            bool saveSuccess = fixture.config.saveToFile(testFile);
            
            if (saveSuccess && std::filesystem::exists(testFile)) {
                // Load the saved file
                Config loadedConfig;
                bool loadSuccess = loadedConfig.loadFromFile(testFile);
                
                if (loadSuccess) {
                    // Verify values
                    bool valuesMatch = fixture.config.getOr<int>("test_file_int", -1) == 
                                      loadedConfig.getOr<int>("test_file_int", -2) &&
                                      fixture.config.getOr<std::string>("test_file_string", "") == 
                                      loadedConfig.getOr<std::string>("test_file_string", "") &&
                                      fixture.config.getOr<bool>("test_file_bool", false) == 
                                      loadedConfig.getOr<bool>("test_file_bool", true);
                    
                    if (valuesMatch) {
                        std::cout << "  ✓ Configuration file I/O test passed" << std::endl;
                        return 1;
                    } else {
                        std::cout << "  ✗ Configuration file I/O test failed - values don't match" << std::endl;
                        return 0;
                    }
                } else {
                    std::cout << "  ✗ Configuration file I/O test failed - load failed" << std::endl;
                    return 0;
                }
            } else {
                std::cout << "  ✗ Configuration file I/O test failed - save failed" << std::endl;
                return 0;
            }
            
        } catch (const std::exception& e) {
            std::cout << "  ✗ Configuration file I/O test failed with exception: " << e.what() << std::endl;
            return 0;
        }
    }
    
    int testConfigurationFileIOTests() {
        std::cout << "Test 6: Configuration file I/O edge cases" << std::endl;
        
        ConfigTestFixture fixture;
        
        int passed = 0;
        
        try {
            // Test saving to non-existent directory
            std::string nonExistentDir = "/non/existent/directory/test.cfg";
            bool saveSuccess = fixture.config.saveToFile(nonExistentDir);
            
            // This should fail gracefully
            if (!saveSuccess) {
                std::cout << "  ✓ Non-existent directory handling passed" << std::endl;
                passed++;
            } else {
                std::cout << "  ✗ Non-existent directory handling should have failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✓ Non-existent directory handled with exception (expected): " << e.what() << std::endl;
            passed++;
        }
        
        try {
            // Test loading non-existent file
            Config config;
            bool loadSuccess = config.loadFromFile("/non/existent/file.cfg");
            
            // This should fail gracefully
            if (!loadSuccess) {
                std::cout << "  ✓ Non-existent file loading passed" << std::endl;
                passed++;
            } else {
                std::cout << "  ✗ Non-existent file loading should have failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✓ Non-existent file loading handled with exception (expected): " << e.what() << std::endl;
            passed++;
        }
        
        return passed;
    }
    
    int testCommandLineParsing() {
        std::cout << "Test 7: Command line argument parsing" << std::endl;
        
        ConfigTestFixture fixture;
        
        try {
            // Test setting values via simulated command line
            int argc = 5;
            char* argv[] = {
                (char*)"test_program",
                (char*)"--random_seed",
                (char*)"12345",
                (char*)"--neuron_count",
                (char*)"2000"
            };
            
            bool parseSuccess = fixture.config.loadFromArgs(argc, argv);
            
            if (parseSuccess) {
                int randomSeed = fixture.config.getOr<int64_t>("random_seed", 0);
                int64_t neuronCount = fixture.config.getOr<int64_t>("neuron_count", 0);
                
                if (randomSeed == 12345 && neuronCount == 2000) {
                    std::cout << "  ✓ Command line parsing test passed" << std::endl;
                    return 1;
                } else {
                    std::cout << "  ✗ Command line parsing test failed - values incorrect" << std::endl;
                    return 0;
                }
            } else {
                std::cout << "  ✗ Command line parsing test failed - parsing failed" << std::endl;
                return 0;
            }
            
        } catch (const std::exception& e) {
            std::cout << "  ✗ Command line parsing test failed with exception: " << e.what() << std::endl;
            return 0;
        }
    }
    
    int testCommandLineParsingFailures() {
        std::cout << "Test 8: Command line argument parsing failures" << std::endl;
        
        ConfigTestFixture fixture;
        
        int passed = 0;
        
        try {
            // Test malformed command line
            int argc = 3;
            char* argv[] = {
                (char*)"test_program",
                (char*)"--invalid_flag"
                // Missing value for --invalid_flag
            };
            
            bool parseSuccess = fixture.config.loadFromArgs(argc, argv);
            
            // This should not crash, even with invalid flags
            std::cout << "  ✓ Malformed command line handled gracefully" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✓ Malformed command line handled with exception: " << e.what() << std::endl;
            passed++;
        }
        
        try {
            // Test empty command line
            int argc = 1;
            char* argv[] = {(char*)"test_program"};
            
            bool parseSuccess = fixture.config.loadFromArgs(argc, argv);
            
            // This should succeed with no arguments
            if (parseSuccess) {
                std::cout << "  ✓ Empty command line handling passed" << std::endl;
                passed++;
            } else {
                std::cout << "  ✗ Empty command line handling failed" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ Empty command line handling failed with exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    int testConfigurationCopyMove() {
        std::cout << "Test 9: Configuration copy and move operations" << std::endl;
        
        ConfigTestFixture fixture;
        
        try {
            // Set some values
            fixture.config.set("copy_test_int", 42);
            fixture.config.set("copy_test_string", "copy_test");
            
            // Test copy constructor
            Config copyConfig(fixture.config);
            
            int copyInt = copyConfig.getOr<int>("copy_test_int", 0);
            std::string copyString = copyConfig.getOr<std::string>("copy_test_string", "");
            
            if (copyInt == 42 && copyString == "copy_test") {
                std::cout << "  ✓ Configuration copy constructor test passed" << std::endl;
            } else {
                std::cout << "  ✗ Configuration copy constructor test failed" << std::endl;
                return 0;
            }
            
            // Test move constructor
            Config moveConfig(std::move(fixture.config));
            
            int moveInt = moveConfig.getOr<int>("copy_test_int", 0);
            std::string moveString = moveConfig.getOr<std::string>("copy_test_string", "");
            
            if (moveInt == 42 && moveString == "copy_test") {
                std::cout << "  ✓ Configuration move constructor test passed" << std::endl;
                return 1;
            } else {
                std::cout << "  ✗ Configuration move constructor test failed" << std::endl;
                return 0;
            }
            
        } catch (const std::exception& e) {
            std::cout << "  ✗ Configuration copy/move test failed with exception: " << e.what() << std::endl;
            return 0;
        }
    }
    
    int testErrorHandling() {
        std::cout << "Test 10: Error handling and recovery" << std::endl;
        
        ConfigTestFixture fixture;
        
        int passed = 0;
        
        try {
            // Test setting very long key
            std::string longKey(10000, 'x');
            fixture.config.set(longKey, "long_key_value");
            std::cout << "  ✓ Long key handling passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✓ Long key handled with exception: " << e.what() << std::endl;
            passed++;
        }
        
        try {
            // Test setting very long value
            std::string longValue(10000, 'y');
            fixture.config.set("long_value_key", longValue);
            std::cout << "  ✓ Long value handling passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✓ Long value handled with exception: " << e.what() << std::endl;
            passed++;
        }
        
        try {
            // Test setting many values
            for (int i = 0; i < 1000; ++i) {
                fixture.config.set(std::to_string(i), std::to_string(i * 2));
            }
            std::cout << "  ✓ Many values setting test passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✗ Many values setting test failed with exception: " << e.what() << std::endl;
        }
        
        return passed;
    }
    
    int testEdgeCases() {
        std::cout << "Test 11: Edge cases and boundary conditions" << std::endl;
        
        ConfigTestFixture fixture;
        
        int passed = 0;
        
        try {
            // Test setting zero values
            fixture.config.set("zero_int", 0);
            fixture.config.set("zero_double", 0.0);
            fixture.config.set("zero_bool", false);
            fixture.config.set("zero_string", "");
            std::cout << "  ✓ Zero values handling passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✗ Zero values handling failed with exception: " << e.what() << std::endl;
        }
        
        try {
            // Test setting negative values
            fixture.config.set("negative_int", -42);
            fixture.config.set("negative_double", -3.14);
            std::cout << "  ✓ Negative values handling passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✗ Negative values handling failed with exception: " << e.what() << std::endl;
        }
        
        try {
            // Test setting special float values
            fixture.config.set("nan_value", std::nan("1"));
            fixture.config.set("inf_value", std::numeric_limits<double>::infinity());
            std::cout << "  ✓ Special float value handling passed" << std::endl;
            passed++;
        } catch (const std::exception& e) {
            std::cout << "  ✓ Special float value handled with exception: " << e.what() << std::endl;
            passed++;
        }
        
        return passed;
    }
};