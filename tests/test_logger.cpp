// Enhanced Logger Tests
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_logger {

void testLoggerCreation() {
    nlm::Logger logger;
    nlm::ConsoleLogger consoleLogger;
    
    // Test basic logger creation
    logger.addLogger(std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(!logger.isEnabled(nlm::LogLevel::Debug));
    
    std::cout << "    testLoggerCreation passed" << std::endl;
}

void testLoggerLevelControl() {
    nlm::Logger logger;
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    logger.addLogger(consoleLogger);
    
    // Test level control
    assert(consoleLogger->getLevel() == nlm::LogLevel::Warning);
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(!logger.isEnabled(nlm::LogLevel::Info));
    assert(!logger.isEnabled(nlm::LogLevel::Debug));
    
    consoleLogger->setLevel(nlm::LogLevel::Info);
    assert(logger.isEnabled(nlm::LogLevel::Info));
    
    std::cout << "    testLoggerLevelControl passed" << std::endl;
}

void testLoggerFileOutput() {
    nlm::Logger logger;
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
    
    std::string testFile = "test_logger_output.txt";
    consoleLogger->setOutputFile(testFile);
    
    logger.addLogger(consoleLogger);
    logger.info("Test message 1");
    logger.warning("Test message 2");
    
    // Check file output
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
    
    // Cleanup
    consoleLogger->closeOutputFile();
    std::remove(testFile.c_str());
    
    std::cout << "    testLoggerFileOutput passed" << std::endl;
}

void testLoggerColors() {
    nlm::Logger logger;
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    
    consoleLogger->setUseColors(true);
    assert(consoleLogger->getUseColors());
    
    consoleLogger->setUseColors(false);
    assert(!consoleLogger->getUseColors());
    
    logger.addLogger(consoleLogger);
    
    std::cout << "    testLoggerColors passed" << std::endl;
}

void testLoggerTimestamps() {
    nlm::Logger logger;
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    
    consoleLogger->setUseTimestamps(true);
    assert(consoleLogger->getUseTimestamps());
    
    consoleLogger->setUseTimestamps(false);
    assert(!consoleLogger->getUseTimestamps());
    
    logger.addLogger(consoleLogger);
    
    std::cout << "    testLoggerTimestamps passed" << std::endl;
}

void testLoggerFileInfo() {
    nlm::Logger logger;
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    
    consoleLogger->setUseFileInfo(true);
    assert(consoleLogger->getUseFileInfo());
    
    consoleLogger->setUseFileInfo(false);
    assert(!consoleLogger->getUseFileInfo());
    
    logger.addLogger(consoleLogger);
    
    std::cout << "    testLoggerFileInfo passed" << std::endl;
}

void testLoggerConvenienceFunctions() {
    // Test global logger functions
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger.addLogger(consoleLogger);
    
    // Test global logging macros (via functions)
    nlm::logGlobal(nlm::LogLevel::Info, "Global info test");
    nlm::logGlobal(nlm::LogLevel::Warning, "Global warning test");
    
    assert(logger.isEnabled(nlm::LogLevel::Info));
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    
    std::cout << "    testLoggerConvenienceFunctions passed" << std::endl;
}

void testLoggerStream() {
    nlm::Logger& logger = nlm::Logger::getGlobal();
    
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger.clearLoggers();
    logger.addLogger(consoleLogger);
    
    // Test stream-style logging
    auto& stream = logger.logStream(nlm::LogLevel::Info, "test.cpp", 123, "testFunction");
    stream << "Streamed " << 123 << " value";
    
    std::cout << "    testLoggerStream passed" << std::endl;
}

void testLoggerClear() {
    nlm::Logger logger;
    
    auto consoleLogger1 = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    auto consoleLogger2 = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    
    logger.addLogger(consoleLogger1);
    logger.addLogger(consoleLogger2);
    
    assert(logger.isEnabled(nlm::LogLevel::Warning));
    assert(logger.isEnabled(nlm::LogLevel::Info));
    
    logger.clearLoggers();
    assert(!logger.isEnabled(nlm::LogLevel::Warning));
    assert(!logger.isEnabled(nlm::LogLevel::Info));
    
    std::cout << "    testLoggerClear passed" << std::endl;
}

void runAll() {
    testLoggerCreation();
    testLoggerLevelControl();
    testLoggerFileOutput();
    testLoggerColors();
    testLoggerTimestamps();
    testLoggerFileInfo();
    testLoggerConvenienceFunctions();
    testLoggerStream();
    testLoggerClear();
}

} // namespace test_logger