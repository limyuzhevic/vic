#include "common/CommonUtils.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

void testCLIUtils() {
    std::cout << "Testing CLIUtils..." << std::endl;
    
    // Test trim
    std::string test1 = "  hello  ";
    std::string trimmed = nlm::CLIUtils::trim(test1);
    assert(trimmed == "hello");
    
    // Test toLower
    std::string test2 = "Hello World";
    std::string lower = nlm::CLIUtils::toLower(test2);
    assert(lower == "hello world");
    
    // Test startsWith
    assert(nlm::CLIUtils::startsWith("hello world", "hello"));
    assert(!nlm::CLIUtils::startsWith("hello world", "world"));
    
    // Test parseOutputFormat
    assert(nlm::CLIUtils::parseOutputFormat("JSON") == nlm::OutputFormat::JSON);
    assert(nlm::CLIUtils::parseOutputFormat("json") == nlm::OutputFormat::JSON);
    assert(nlm::CLIUtils::parseOutputFormat("CONSOLE") == nlm::OutputFormat::CONSOLE);
    assert(nlm::CLIUtils::parseOutputFormat("unknown") == nlm::OutputFormat::CONSOLE);
    
    // Test getCurrentTimestamp
    std::string timestamp = nlm::CLIUtils::getCurrentTimestamp();
    assert(timestamp.length() >= 15); // YYYYMMDD_HHMMSS format
    
    std::cout << "  ✓ CLIUtils tests passed" << std::endl;
}

void testStringUtils() {
    std::cout << "Testing StringUtils..." << std::endl;
    
    // Test leftAlign
    std::string left = nlm::StringUtils::leftAlign("hello", 10);
    assert(left == "hello     ");
    
    // Test rightAlign
    std::string right = nlm::StringUtils::rightAlign("hello", 10);
    assert(right == "     hello");
    
    // Test centerAlign
    std::string center = nlm::StringUtils::centerAlign("hello", 11);
    assert(center == "   hello   ");
    
    // Test toString
    assert(nlm::StringUtils::toString(3.14159) == "3.142");
    assert(nlm::StringUtils::toString(42) == "42");
    assert(nlm::StringUtils::toString(true) == "true");
    
    // Test split
    std::vector<std::string> parts = nlm::StringUtils::split("a,b,c", ',');
    assert(parts.size() == 3);
    assert(parts[0] == "a");
    assert(parts[1] == "b");
    assert(parts[2] == "c");
    
    // Test join
    std::string joined = nlm::StringUtils::join(parts, "-");
    assert(joined == "a-b-c");
    
    // Test contains
    assert(nlm::StringUtils::contains("hello world", "world"));
    assert(!nlm::StringUtils::contains("hello world", "earth"));
    
    // Test replaceAll
    std::string replaced = nlm::StringUtils::replaceAll("hello world", "world", "NLM");
    assert(replaced == "hello NLM");
    
    // Test escapeJSON
    std::string escaped = nlm::StringUtils::escapeJSON("Hello \"World\"");
    assert(escaped == "Hello \\"World\\"");
    
    std::cout << "  ✓ StringUtils tests passed" << std::endl;
}

void testTimer() {
    std::cout << "Testing Timer..." << std::endl;
    
    nlm::Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double elapsed = timer.elapsedSeconds();
    
    assert(elapsed >= 0.09 && elapsed <= 1.0); // Allow some tolerance
    
    double ms = timer.elapsedMilliseconds();
    assert(ms >= 90.0 && ms <= 1000.0);
    
    timer.reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    elapsed = timer.elapsedSeconds();
    assert(elapsed >= 0.04 && elapsed <= 0.6);
    
    std::cout << "  ✓ Timer tests passed" << std::endl;
}

void testOutputManager() {
    std::cout << "Testing OutputManager..." << std::endl;
    
    // Test format changes
    nlm::OutputManager::setOutputFormat(nlm::OutputFormat::CONSOLE);
    assert(nlm::OutputManager::getOutputFormat() == nlm::OutputFormat::CONSOLE);
    
    nlm::OutputManager::setOutputFormat(nlm::OutputFormat::JSON);
    assert(nlm::OutputManager::getOutputFormat() == nlm::OutputFormat::JSON);
    
    // Test output methods (just check they don't crash)
    OutputManager::printHeader("Test Header");
    OutputManager::printSection("Test Section");
    OutputManager::printResult("Test Integer", 42);
    OutputManager::printResult("Test Float", 3.14159);
    OutputManager::printResult("Test String", "Hello World");
    OutputManager::printProgress("Test Progress", 5, 10);
    OutputManager::printSuccess("Test Success");
    OutputManager::printWarning("Test Warning");
    OutputManager::printError("Test Error");
    OutputManager::printInfo("Test Info");
    OutputManager::printDebug("Test Debug");
    
    std::cout << "  ✓ OutputManager tests passed" << std::endl;
}

void testExperimentResult() {
    std::cout << "Testing ExperimentResult..." << std::endl;
    
    nlm::ExperimentResult result;
    result.name = "Test Experiment";
    result.startTime = 100.0;
    result.endTime = 200.0;
    result.duration = 100.0;
    result.totalReward = 150.5f;
    result.averageReward = 15.05f;
    result.successRate = 75.0f;
    result.metadata["param1"] = "value1";
    result.metadata["param2"] = "value2";
    
    // Test summary
    std::string summary = result.summary();
    assert(summary.find("Test Experiment") != std::string::npos);
    assert(summary.find("Duration: 100.0") != std::string::npos);
    
    // Test JSON (basic structure check)
    std::string json = result.toJSON();
    assert(json.find("\"name\": \"Test Experiment\"") != std::string::npos);
    assert(json.find("\"totalReward\": 150.5") != std::string::npos);
    
    std::cout << "  ✓ ExperimentResult tests passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Common Utils Test Suite ===\n" << std::endl;
    
    try {
        testCLIUtils();
        testStringUtils();
        testTimer();
        testOutputManager();
        testExperimentResult();
        
        std::cout << "\n=== All Tests Passed! ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nTest failed with unknown exception" << std::endl;
        return 1;
    }
}
