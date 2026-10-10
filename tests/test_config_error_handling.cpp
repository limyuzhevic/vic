// Config Error Handling Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_config_error_handling {

void createTestConfigFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void cleanupTestConfigFile(const std::string& filename) {
    std::remove(filename.c_str());
}

void testLoadFromFileLenientMode() {
    nlm::Config config;
    createTestConfigFile("test_config.txt", 
        "# Test config file\n"
        "valid_key = 42\n"
        "another_key = hello world\n"
        "# Comment line\n"
        "vector_int = 1,2,3,4,5\n"
        "invalid_line_without_equals\n"
        "# Another comment\n"
        "valid_key2 = test_value\n");
    
    bool result = config.loadFromFile("test_config.txt");
    assert(result == true);
    
    assert(config.has("valid_key"));
    assert(*config.get<int>("valid_key") == 42);
    
    assert(config.has("another_key"));
    assert(*config.get<std::string>("another_key") == "hello world");
    
    assert(config.has("vector_int"));
    auto vec = *config.get<std::vector<int>>("vector_int");
    assert(vec.size() == 5);
    assert(vec[0] == 1 && vec[1] == 2 && vec[2] == 3 && vec[3] == 4 && vec[4] == 5);
    
    assert(config.has("valid_key2"));
    assert(*config.get<std::string>("valid_key2") == "test_value");
    
    // Check error tracking
    assert(config.getSuccessfulParses() == 4);
    assert(config.getErrorCount() == 1);
    assert(config.getErrorRecoveryMode() == nlm::ErrorRecoveryMode::Lenient);
    
    const auto& errors = config.getErrors();
    assert(errors.size() == 1);
    assert(errors[0].lineNumber == 6);
    assert(errors[0].errorMessage == "Malformed line - expected format: key=value");
    
    std::cout << "    testLoadFromFileLenientMode passed" << std::endl;
    
    cleanupTestConfigFile("test_config.txt");
}

void testLoadFromFileStrictMode() {
    nlm::Config config;
    createTestConfigFile("test_config.txt", 
        "# Test config file\n"
        "valid_key = 42\n"
        "invalid_line_without_equals\n"
        "another_key = hello");
    
    bool result = config.loadFromFile("test_config.txt");
    assert(result == false);
    
    // Should only have parsed the first line
    assert(config.has("valid_key"));
    assert(*config.get<int>("valid_key") == 42);
    
    // Check error tracking
    assert(config.getSuccessfulParses() == 1);
    assert(config.getErrorCount() == 1);
    assert(config.getErrorRecoveryMode() == nlm::ErrorRecoveryMode::Strict);
    
    const auto& errors = config.getErrors();
    assert(errors.size() == 1);
    assert(errors[0].lineNumber == 3);
    
    std::cout << "    testLoadFromFileStrictMode passed" << std::endl;
    
    cleanupTestConfigFile("test_config.txt");
}

void testLoadFromFileEmptyKey() {
    nlm::Config config;
    createTestConfigFile("test_config.txt", 
        "# Test config file\n"
        "= value_without_key\n"
        "valid_key = 123");
    
    bool result = config.loadFromFile("test_config.txt");
    assert(result == true);
    
    // Should only have parsed the second line
    assert(config.has("valid_key"));
    assert(*config.get<int>("valid_key") == 123);
    
    // Check error tracking
    assert(config.getSuccessfulParses() == 1);
    assert(config.getErrorCount() == 1);
    
    const auto& errors = config.getErrors();
    assert(errors.size() == 1);
    assert(errors[0].lineNumber == 2);
    assert(errors[0].errorMessage == "Empty key found");
    
    std::cout << "    testLoadFromFileEmptyKey passed" << std::endl;
    
    cleanupTestConfigFile("test_config.txt");
}

void testLoadFromFileParseError() {
    nlm::Config config;
    createTestConfigFile("test_config.txt", 
        "# Test config file\n"
        "valid_key = 42\n"
        "bad_int = not_a_number\n"
        "another_key = 99");
    
    bool result = config.loadFromFile("test_config.txt");
    assert(result == true);
    
    // Should only have parsed the first line
    assert(config.has("valid_key"));
    assert(*config.get<int>("valid_key") == 42);
    
    // Check error tracking
    assert(config.getSuccessfulParses() == 1);
    assert(config.getErrorCount() == 1);
    
    const auto& errors = config.getErrors();
    assert(errors.size() == 1);
    assert(errors[0].lineNumber == 3);
    assert(errors[0].errorMessage.find("Failed to parse value for key: bad_int") != std::string::npos);
    
    std::cout << "    testLoadFromFileParseError passed" << std::endl;
    
    cleanupTestConfigFile("test_config.txt");
}

void testErrorRecoveryModeSetter() {
    nlm::Config config;
    
    // Test setting lenient mode (default)
    assert(config.setErrorRecoveryMode(nlm::ErrorRecoveryMode::Lenient));
    assert(config.getErrorRecoveryMode() == nlm::ErrorRecoveryMode::Lenient);
    
    // Test setting strict mode
    assert(config.setErrorRecoveryMode(nlm::ErrorRecoveryMode::Strict));
    assert(config.getErrorRecoveryMode() == nlm::ErrorRecoveryMode::Strict);
    
    // Test setting back to lenient
    assert(config.setErrorRecoveryMode(nlm::ErrorRecoveryMode::Lenient));
    assert(config.getErrorRecoveryMode() == nlm::ErrorRecoveryMode::Lenient);
    
    std::cout << "    testErrorRecoveryModeSetter passed" << std::endl;
}

void testClearResetsErrorTracking() {
    nlm::Config config;
    createTestConfigFile("test_config.txt", 
        "valid_key = 42\n"
        "invalid_line\n"
        "another_key = 99");
    
    config.loadFromFile("test_config.txt");
    
    assert(config.getSuccessfulParses() == 1);
    assert(config.getErrorCount() == 1);
    assert(config.getErrors().size() == 1);
    
    config.clear();
    
    // Error tracking should be reset
    assert(config.getSuccessfulParses() == 0);
    assert(config.getErrorCount() == 0);
    assert(config.getErrors().size() == 0);
    
    // Config values should be cleared
    assert(!config.has("valid_key"));
    assert(!config.has("another_key"));
    
    std::cout << "    testClearResetsErrorTracking passed" << std::endl;
    
    cleanupTestConfigFile("test_config.txt");
}

void runAll() {
    testErrorRecoveryModeSetter();
    testLoadFromFileLenientMode();
    testLoadFromFileStrictMode();
    testLoadFromFileEmptyKey();
    testLoadFromFileParseError();
    testClearResetsErrorTracking();
}

} // namespace test_config_error_handling
