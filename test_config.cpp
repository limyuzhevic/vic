#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::cout << "=== NLM Configuration System Test ===" << std::endl;
    std::cout << "Testing new JSON-based configuration system..." << std::endl << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Test 1: Create a config and test basic operations
    std::cout << "--- Test 1: Basic Config Operations ---" << std::endl;
    auto config = std::make_shared<Config>();
    
    // Set various types of values
    config->set("integer_key", 42);
    config->set("float_key", 3.14159);
    config->set("string_key", "Hello World");
    config->set("bool_key", true);
    config->set("int64_key", static_cast<int64_t>(9876543210));
    config->set("vector_key", std::vector<int>({1, 2, 3, 4, 5}));
    
    // Get values
    std::cout << "integer_key: " << config->getOr<int>("integer_key", -1) << std::endl;
    std::cout << "float_key: " << config->getOr<double>("float_key", -1.0) << std::endl;
    std::cout << "string_key: " << config->getOr<std::string>("string_key", "") << std::endl;
    std::cout << "bool_key: " << config->getOr<bool>("bool_key", false) << std::endl;
    std::cout << "int64_key: " << config->getOr<int64_t>("int64_key", -1) << std::endl;
    
    // Test vector retrieval
    auto vecOpt = config->get<std::vector<int>>("vector_key");
    if (vecOpt.has_value()) {
        std::cout << "vector_key: ";
        for (size_t i = 0; i < vecOpt.value().size(); ++i) {
            std::cout << vecOpt.value()[i];
            if (i < vecOpt.value().size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Has integer_key: " << config->has("integer_key") << std::endl;
    std::cout << "Has nonexistent_key: " << config->has("nonexistent_key") << std::endl;
    
    // Test 2: Save and load JSON config
    std::cout << "\n--- Test 2: JSON Config Save/Load ---" << std::endl;
    std::string testFile = "/tmp/nlm_test_config.json";
    
    if (config->saveToFile(testFile)) {
        std::cout << "✓ Config saved to: " << testFile << std::endl;
        
        // Load back
        auto loadedConfig = Config::fromJson(nlohmann::json::parse(std::ifstream(testFile)));
        
        std::cout << "✓ Config loaded from JSON" << std::endl;
        std::cout << "Loaded integer_key: " << loadedConfig->getOr<int>("integer_key", -1) << std::endl;
        std::cout << "Loaded float_key: " << loadedConfig->getOr<double>("float_key", -1.0) << std::endl;
        std::cout << "Loaded string_key: " << loadedConfig->getOr<std::string>("string_key", "") << std::endl;
        
    } else {
        std::cout << "✗ Failed to save config" << std::endl;
    }
    
    // Test 3: Test command-line loading
    std::cout << "\n--- Test 3: Command-Line Config Loading ---" << std::endl;
    auto cmdConfig = std::make_shared<Config>();
    
    // Simulate command-line args
    const char* testArgs[] = {"test", "--int_key", "123", "--string_key", "test_value", "--bool_key", "true"};
    int testArgc = 6;
    
    if (cmdConfig->loadFromArgs(testArgc, const_cast<char**>(testArgs))) {
        std::cout << "✓ Command-line config loaded" << std::endl;
        std::cout << "int_key: " << cmdConfig->getOr<int>("int_key", -1) << std::endl;
        std::cout << "string_key: " << cmdConfig->getOr<std::string>("string_key", "") << std::endl;
        std::cout << "bool_key: " << cmdConfig->getOr<bool>("bool_key", false) << std::endl;
    } else {
        std::cout << "✗ Failed to load command-line config" << std::endl;
    }
    
    // Test 4: Test config summary
    std::cout << "\n--- Test 4: Config Summary ---" << std::endl;
    std::cout << config->summary() << std::endl;
    
    std::cout << "\n=== Configuration System Test Complete ===" << std::endl;
    return 0;
}