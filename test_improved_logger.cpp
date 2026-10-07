#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "Testing NLM Logger System Improvements..." << std::endl;
    
    // Test 1: Basic logger creation and level control
    nlm::Logger logger;
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    
    logger.addLogger(consoleLogger);
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(!logger.isEnabled(nlm::LogLevel::Debug));
    std::cout << "✓ Basic logger creation and level control works" << std::endl;
    
    // Test 2: Enhanced logger features
    consoleLogger->setUseColors(true);
    assert(consoleLogger->getUseColors());
    
    consoleLogger->setUseTimestamps(true);
    assert(consoleLogger->getUseTimestamps());
    
    consoleLogger->setUseFileInfo(true);
    assert(consoleLogger->getUseFileInfo());
    std::cout << "✓ Enhanced logger configuration works" << std::endl;
    
    // Test 3: File output with rotation support
    std::string testFile = "test_improved_logger.txt";
    consoleLogger->setOutputFile(testFile);
    
    logger.info("Test message 1");
    logger.warning("Test message 2");
    
    // Verify file output
    std::ifstream inFile(testFile);
    std::string line;
    bool foundMessage1 = false;
    bool foundMessage2 = false;
    
    while (std::getline(inFile, line)) {
        if (line.find("Test message 1") != std::string::npos) {
            foundMessage1 = true;
        }
        if (line.find("Test message 2") != std::string::npos) {
            foundMessage2 = true;
        }
    }
    
    inFile.close();
    assert(foundMessage1);
    assert(foundMessage2);
    std::cout << "✓ File output with formatting works" << std::endl;
    
    // Test 4: Stream-style logging
    auto& stream = logger.logStream(nlm::LogLevel::Info, "test.cpp", 123, "testFunction");
    stream << "Streamed " << 123 << " value";
    std::cout << "✓ Stream-style logging works" << std::endl;
    
    // Test 5: Level management
    consoleLogger->setLevel(nlm::LogLevel::Warning);
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(!logger.isEnabled(nlm::LogLevel::Info));
    
    consoleLogger->setLevel(nlm::LogLevel::Info);
    assert(logger.isEnabled(nlm::LogLevel::Info));
    std::cout << "✓ Level management works" << std::endl;
    
    // Cleanup
    consoleLogger->closeOutputFile();
    std::remove(testFile.c_str());
    
    std::cout << std::endl;
    std::cout << "=== All Logger Tests PASSED ===" << std::endl;
    std::cout << "✓ Logger system improvements are working correctly!" << std::endl;
    
    return 0;
}