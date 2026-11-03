// Test program to verify fixes
// Tests: Config.saveToFile() fix and Logger thread safety

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>

void testConfigSaveToFile() {
    std::cout << "Testing Config.saveToFile() fix..." << std::endl;
    
    nlm::Config config;
    
    // Set various types of values
    config.set("string_key", "Hello World");
    config.set("int_key", 42);
    config.set("double_key", 3.14159);
    config.set("bool_key", true);
    config.set("int64_key", static_cast<int64_t>(9876543210));
    
    std::vector<std::string> string_list = {"apple", "banana", "cherry"};
    config.set("string_list", string_list);
    
    std::vector<int> int_list = {1, 2, 3, 4, 5};
    config.set("int_list", int_list);
    
    // Save to file
    if (config.saveToFile("test_config.cfg")) {
        std::cout << "✓ Config saved to test_config.cfg" << std::endl;
        
        // Read back and verify
        std::ifstream file("test_config.cfg");
        std::string line;
        bool foundStringKey = false;
        bool foundIntKey = false;
        
        while (std::getline(file, line)) {
            if (line.find("string_key") != std::string::npos && 
                line.find("Hello World") != std::string::npos) {
                foundStringKey = true;
            }
            if (line.find("int_key") != std::string::npos && 
                line.find("42") != std::string::npos) {
                foundIntKey = true;
            }
        }
        
        if (foundStringKey && foundIntKey) {
            std::cout << "✓ Config file contains actual values (not PLACEHOLDER_VALUE)" << std::endl;
        } else {
            std::cout << "✗ Config file verification failed" << std::endl;
        }
        
        // Clean up
        std::remove("test_config.cfg");
    } else {
        std::cout << "✗ Failed to save config to file" << std::endl;
    }
}

void testLoggerThreadSafety() {
    std::cout << "\nTesting Logger thread safety..." << std::endl;
    
    // Initialize logger
    auto& logger = nlm::Logger::getGlobal();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    const int numThreads = 10;
    const int logsPerThread = 100;
    
    std::cout << "Logging " << numThreads * logsPerThread << " messages from " << numThreads << " threads..." << std::endl;
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, threadId = i]() {
            for (int j = 0; j < logsPerThread; ++j) {
                logger.info("Thread " + std::to_string(threadId) + 
                           " message " + std::to_string(j));
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "✓ Logger thread safety test completed" << std::endl;
}

int main() {
    std::cout << "=== Testing NLM Fixes ===" << std::endl;
    
    testConfigSaveToFile();
    testLoggerThreadSafety();
    
    std::cout << "\n=== All tests completed ===" << std::endl;
    return 0;
}
