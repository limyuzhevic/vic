// Test framework for NLM Advanced Features
// Tests for logging, configuration, agent system, world simulation, and CLI

#include "../core/Logger/AdvancedLogger.hpp"
#include "../core/Config/ConfigManager.hpp"
#include "../agent/EnhancedAgentBrain.hpp"
#include "../world/EnhancedSimpleWorld.hpp"
#include "../NLMShell.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

namespace nlm {
namespace test_advanced {

// Test logging functionality
class LoggingTest {
public:
    static void testBasicLogging() {
        nlm::getLogger().info("TestLogger", "Basic logging test");
        nlm::getLogger().debug("TestLogger", "Debug message");
        nlm::getLogger().error("TestLogger", "Error message");
        
        // Test level filtering
        nlm::getLogger().setLevel(LogLevel::Warning);
        nlm::getLogger().info("TestLogger", "This should not appear");
        nlm::getLogger().setLevel(LogLevel::Info);
        
        std::cout << "    LoggingTest::testBasicLogging passed" << std::endl;
    }
    
    static void testLogHandlers() {
        // Create a file log handler
        std::string testFile = "test_log.txt";
        auto fileHandler = std::make_shared<FileLogHandler>(testFile);
        nlm::getLogger().addHandler(fileHandler);
        
        nlm::getLogger().info("TestLogger", "Test message for file logging");
        
        // Verify file was written
        std::ifstream file(testFile);
        assert(file.good());
        
        std::string line;
        std::getline(file, line);
        assert(!line.empty());
        assert(line.find("Test message") != std::string::npos);
        
        file.close();
        std::remove(testFile.c_str());
        
        // Remove handler
        nlm::getLogger().removeHandler(fileHandler.get());
        
        std::cout << "    LoggingTest::testLogHandlers passed" << std::endl;
    }
    
    static void testLogBuffer() {
        // Clear any existing handlers
        nlm::getLogger().setLevel(LogLevel::Debug);
        
        // Add buffer handler
        auto bufferHandler = std::make_shared<LogBuffer>(100, 0.5);
        nlm::getLogger().addHandler(bufferHandler);
        
        // Send messages to buffer
        for (int i = 0; i < 50; ++i) {
            nlm::getLogger().info("BufferTest", "Message " + std::to_string(i));
        }
        
        // Wait for buffer flush
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        
        // Check message count
        assert(bufferHandler->getMessageCount() == 0);
        
        // Remove handler
        nlm::getLogger().removeHandler(bufferHandler.get());
        
        std::cout << "    LoggingTest::testLogBuffer passed" << std::endl;
    }
};

// Test configuration manager functionality
class ConfigManagerTest {
public:
    static void testBasicConfig() {
        ConfigManager config("TestConfig");
        
        // Test basic operations
        config.set("int_value", 42);
        config.set("float_value", 3.14f);
        config.set("string_value", std::string("test"));
        config.set("bool_value", true);
        config.set("vector_int", std::vector<int>{1, 2, 3});
        config.set("vector_float", std::vector<float>{1.0f, 2.0f, 3.0f});
        
        assert(config.hasKey("int_value"));
        assert(config.hasKey("float_value"));
        assert(config.hasKey("string_value"));
        assert(config.hasKey("bool_value"));
        assert(config.hasKey("vector_int"));
        assert(config.hasKey("vector_float"));
        
        // Test get operations
        assert(config.getInt("int_value") == 42);
        assert(config.getFloat("float_value") > 3.13 && config.getFloat("float_value") < 3.15);
        assert(config.getString("string_value") == "test");
        assert(config.getBool("bool_value") == true);
        
        auto vecInt = config.getVectorInt("vector_int");
        assert(vecInt.size() == 3);
        assert(vecInt[0] == 1 && vecInt[1] == 2 && vecInt[2] == 3);
        
        auto vecFloat = config.getVectorFloat("vector_float");
        assert(vecFloat.size() == 3);
        assert(vecFloat[0] > 0.9 && vecFloat[0] < 1.1);
        
        // Test validation
        assert(config.addIntEntry("validated_int", 100, "Test int", true, 0, 1000));
        assert(!config.addIntEntry("invalid_int", 2000, "Test int", true, 0, 1000));
        
        // Test groups
        assert(config.createGroup("TestGroup"));
        assert(config.addToGroup("int_value", "TestGroup"));
        assert(config.addToGroup("float_value", "TestGroup"));
        
        auto groupKeys = config.getKeysInGroup("TestGroup");
        assert(groupKeys.size() == 2);
        
        std::cout << "    ConfigManagerTest::testBasicConfig passed" << std::endl;
    }
    
    static void testFileOperations() {
        ConfigManager config("TestFileConfig");
        
        // Set some test values
        config.set("test_int", 123);
        config.set("test_string", "hello");
        config.set("test_float", 45.67f);
        
        // Save to file
        std::string filename = "test_config_save.txt";
        bool saveResult = config.saveToFile(filename);
        assert(saveResult);
        
        // Create new config and load
        ConfigManager config2("TestFileConfig2");
        bool loadResult = config2.loadFromFile(filename);
        assert(loadResult);
        
        // Verify loaded values
        assert(config2.getInt("test_int") == 123);
        assert(config2.getString("test_string") == "hello");
        assert(config2.getFloat("test_float") > 45.66 && config2.getFloat("test_float") < 45.68);
        
        // Clean up
        std::remove(filename.c_str());
        
        std::cout << "    ConfigManagerTest::testFileOperations passed" << std::endl;
    }
    
    static void testCommandLineArgs() {
        ConfigManager config("TestCLIConfig");
        
        // Simulate command line arguments
        const int argc = 5;
        char* argv[] = {
            (char*)"program",
            (char*)"--int_value=42",
            (char*)"--float_value=3.14",
            (char*)"--string_value=hello",
            (char*)"--bool_value=true"
        };
        
        config.loadFromCommandLine(argc, argv);
        
        assert(config.getInt("int_value") == 42);
        assert(config.getFloat("float_value") > 3.13 && config.getFloat("float_value") < 3.15);
        assert(config.getString("string_value") == "hello");
        assert(config.getBool("bool_value") == true);
        
        std::cout << "    ConfigManagerTest::testCommandLineArgs passed" << std::endl;
    }
    
    static void testValidation() {
        ConfigManager config("TestValidation");
        
        // Add valid entries
        assert(config.addIntEntry("valid1", 50, "Test", true, 0, 100));
        assert(config.addFloatEntry("valid2", 3.14f, "Test", true, 0.0f, 10.0f));
        assert(config.addBoolEntry("valid3", true, "Test", true));
        
        // Add invalid entries
        assert(!config.addIntEntry("invalid1", -50, "Test", true, 0, 100)); // Too small
        assert(!config.addIntEntry("invalid2", 200, "Test", true, 0, 100)); // Too large
        assert(!config.addFloatEntry("invalid3", -1.5f, "Test", true, 0.0f, 1.0f)); // Too small
        
        // Test validation errors
        std::vector<std::string> errors = config.getValidationErrors();
        assert(!errors.empty());
        
        std::cout << "    ConfigManagerTest::testValidation passed" << std::endl;
    }
};

// Test agent brain enhancements
class EnhancedAgentBrainTest {
public:
    static void testBrainCreation() {
        // This test would require a brain object, which is complex
        // For now, just verify the test framework works
        std::cout << "    EnhancedAgentBrainTest::testBrainCreation skipped (requires brain)" << std::endl;
    }
    
    static void testFeatureSystem() {
        // Test feature flagging system
        // This would require integration with the actual enhanced brain
        std::cout << "    EnhancedAgentBrainTest::testFeatureSystem skipped (requires brain)" << std::endl;
    }
};

// Test world simulation enhancements
class EnhancedWorldTest {
public:
    static void testWorldConfiguration() {
        // This test would require integration with the actual enhanced world
        std::cout << "    EnhancedWorldTest::testWorldConfiguration skipped (requires world)" << std::endl;
    }
    
    static void testPatternSystem() {
        // Test pattern-based object placement
        // This would require integration with the actual enhanced world
        std::cout << "    EnhancedWorldTest::testPatternSystem skipped (requires world)" << std::endl;
    }
};

// Test command line interface
class ShellTest {
public:
    static void testBasicCommands() {
        // Test basic CLI functionality
        // This would require full integration testing
        std::cout << "    ShellTest::testBasicCommands skipped (requires full CLI integration)" << std::endl;
    }
    
    static void testCommandParsing() {
        // Test command parsing and execution
        // This would require full integration testing
        std::cout << "    ShellTest::testCommandParsing skipped (requires full CLI integration)" << std::endl;
    }
};

// Main test runner
void runAllAdvancedTests() {
    std::cout << "Running NLM Advanced Feature Tests" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // Initialize logger
    nlm::Logger::getInstance().initialize();
    
    // Run logging tests
    std::cout << "Testing Logging System..." << std::endl;
    LoggingTest::testBasicLogging();
    LoggingTest::testLogHandlers();
    LoggingTest::testLogBuffer();
    std::cout << std::endl;
    
    // Run configuration manager tests
    std::cout << "Testing Configuration Manager..." << std::endl;
    ConfigManagerTest::testBasicConfig();
    ConfigManagerTest::testFileOperations();
    ConfigManagerTest::testCommandLineArgs();
    ConfigManagerTest::testValidation();
    std::cout << std::endl;
    
    // Run enhanced agent brain tests
    std::cout << "Testing Enhanced Agent Brain..." << std::endl;
    EnhancedAgentBrainTest::testBrainCreation();
    EnhancedAgentBrainTest::testFeatureSystem();
    std::cout << std::endl;
    
    // Run enhanced world tests
    std::cout << "Testing Enhanced World Simulation..." << std::endl;
    EnhancedWorldTest::testWorldConfiguration();
    EnhancedWorldTest::testPatternSystem();
    std::cout << std::endl;
    
    // Run shell tests
    std::cout << "Testing NLM Shell..." << std::endl;
    ShellTest::testBasicCommands();
    ShellTest::testCommandParsing();
    std::cout << std::endl;
    
    // Shutdown logger
    nlm::Logger::getInstance().shutdown();
    
    std::cout << "All Advanced Tests Completed" << std::endl;
}

} // namespace test_advanced
} // namespace nlm