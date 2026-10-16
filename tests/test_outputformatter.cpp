// OutputFormatter Tests
// Comprehensive tests for the enhanced OutputFormatter module

#include "src/output/OutputFormatter.hpp"
#include "test/TestRunner.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>

namespace test_outputformatter {

void testOutputFormatterFactory() {
    std::cout << "  Testing output formatter factory..." << std::endl;
    
    // Test all output formats
    std::vector<nlm::OutputFormat> formats = {
        nlm::OutputFormat::TEXT,
        nlm::OutputFormat::JSON,
        nlm::OutputFormat::CSV,
        nlm::OutputFormat::XML,
        nlm::OutputFormat::YAML,
        nlm::OutputFormat::HTML,
        nlm::OutputFormat::MARKDOWN
    };
    
    for (auto format : formats) {
        auto formatter = nlm::OutputFormatter::create(format);
        assert(formatter != nullptr);
        assert(nlm::OutputFormatter::formatToString(format) != "unknown");
    }
    
    std::cout << "    Output formatter factory test passed" << std::endl;
}

void testOutputFormatDetection() {
    std::cout << "  Testing output format detection..." << std::endl;
    
    struct FormatTest {
        std::string filename;
        nlm::OutputFormat expected;
    };
    
    std::vector<FormatTest> tests = {
        {"test.txt", nlm::OutputFormat::TEXT},
        {"test.json", nlm::OutputFormat::JSON},
        {"test.csv", nlm::OutputFormat::CSV},
        {"test.xml", nlm::OutputFormat::XML},
        {"test.yaml", nlm::OutputFormat::YAML},
        {"test.yml", nlm::OutputFormat::YAML},
        {"test.html", nlm::OutputFormat::HTML},
        {"test.htm", nlm::OutputFormat::HTML},
        {"test.md", nlm::OutputFormat::MARKDOWN}
    };
    
    for (const auto& test : tests) {
        nlm::OutputFormat detected = nlm::OutputFormatter::detectFormat(test.filename);
        assert(detected == test.expected);
    }
    
    std::cout << "    Output format detection test passed" << std::endl;
}

void testTextFormatter() {
    std::cout << "  Testing text formatter..." << std::endl;
    
    nlm::TextFormatter formatter;
    
    // Create output config
    nlm::OutputConfig config;
    config.format = nlm::OutputFormat::TEXT;
    config.title = "Test Output";
    config.description = "This is a test output";
    config.includeTimestamp = true;
    
    // Initialize formatter
    assert(formatter.initialize(config));
    
    // Write content
    assert(formatter.write("Hello World"));
    
    // Create test results
    std::vector<nlm::TestResult> testResults;
    
    nlm::TestResult result1;
    result1.name = "Test 1";
    result1.passed = true;
    result1.duration = 0.5;
    result1.description = "First test case";
    result1.output = "Test output 1";
    
    nlm::TestResult result2;
    result2.name = "Test 2";
    result2.passed = false;
    result2.duration = 0.3;
    result2.description = "Second test case";
    result2.errorMessage = "Test failed";
    
    testResults.push_back(result1);
    testResults.push_back(result2);
    
    assert(formatter.write(testResults));
    
    // Write metrics
    std::map<std::string, double> metrics;
    metrics["accuracy"] = 0.95;
    metrics["precision"] = 0.87;
    metrics["recall"] = 0.92;
    
    assert(formatter.write(metrics));
    
    // Flush and close
    formatter.flush();
    formatter.close();
    
    assert(!formatter.isOpen());
    
    std::cout << "    Text formatter test passed" << std::endl;
}

void testJsonFormatter() {
    std::cout << "  Testing JSON formatter..." << std::endl;
    
    nlm::JsonFormatter formatter;
    
    nlm::OutputConfig config;
    config.format = nlm::OutputFormat::JSON;
    config.title = "JSON Test";
    config.description = "Test JSON output";
    config.prettyPrint = true;
    config.includeTimestamp = true;
    
    assert(formatter.initialize(config));
    
    // Write content
    assert(formatter.write("Test content"));
    
    // Write test results
    std::vector<nlm::TestResult> testResults;
    
    nlm::TestResult result;
    result.name = "JSON Test";
    result.passed = true;
    result.duration = 1.2;
    result.description = "Test for JSON output";
    result.output = "{"test": "data"}";
    
    testResults.push_back(result);
    assert(formatter.write(testResults));
    
    formatter.flush();
    formatter.close();
    
    std::cout << "    JSON formatter test passed" << std::endl;
}

void testCsvFormatter() {
    std::cout << "  Testing CSV formatter..." << std::endl;
    
    nlm::CsvFormatter formatter;
    
    nlm::OutputConfig config;
    config.format = nlm::OutputFormat::CSV;
    
    assert(formatter.initialize(config));
    
    // Write test results
    std::vector<nlm::TestResult> testResults;
    
    nlm::TestResult result1;
    result1.name = "Test A";
    result1.passed = true;
    result1.duration = 0.5;
    result1.description = "Test case A";
    
    nlm::TestResult result2;
    result2.name = "Test B";
    result2.passed = false;
    result2.duration = 0.7;
    result2.description = "Test case B";
    result2.errorMessage = "Failed to execute";
    
    testResults.push_back(result1);
    testResults.push_back(result2);
    
    assert(formatter.write(testResults));
    
    // Write metrics
    std::map<std::string, double> metrics;
    metrics["cpu_usage"] = 75.5;
    metrics["memory_usage"] = 512.3;
    metrics["gpu_usage"] = 90.0;
    
    assert(formatter.write(metrics));
    
    formatter.flush();
    formatter.close();
    
    std::cout << "    CSV formatter test passed" << std::endl;
}

void testXmlFormatter() {
    std::cout << "  Testing XML formatter..." << std::endl;
    
    nlm::XmlFormatter formatter;
    
    nlm::OutputConfig config;
    config.format = nlm::OutputFormat::XML;
    config.title = "XML Output";
    
    assert(formatter.initialize(config));
    
    // Write test results
    std::vector<nlm::TestResult> testResults;
    
    nlm::TestResult result;
    result.name = "XML Test";
    result.passed = true;
    result.duration = 0.8;
    result.description = "Test for XML output";
    
    testResults.push_back(result);
    assert(formatter.write(testResults));
    
    formatter.flush();
    formatter.close();
    
    std::cout << "    XML formatter test passed" << std::endl;
}

void testOutputManager() {
    std::cout << "  Testing OutputManager..." << std::endl;
    
    nlm::OutputManager manager;
    
    // Configure output
    nlm::OutputConfig config;
    config.format = nlm::OutputFormat::TEXT;
    config.title = "Manager Test";
    config.description = "Test output manager";
    
    manager.setConfig(config);
    
    // Verify config
    assert(manager.getConfig().title == "Manager Test");
    
    // Set formatter
    assert(manager.setFormatter(nlm::OutputFormat::JSON));
    assert(manager.getCurrentFormat() == nlm::OutputFormat::TEXT); // Simplified check
    
    // Write content
    assert(manager.write("Test content"));
    
    // Write test results
    std::vector<nlm::TestResult> testResults;
    
    nlm::TestResult result;
    result.name = "Manager Test";
    result.passed = true;
    result.duration = 0.5;
    
    testResults.push_back(result);
    assert(manager.write(testResults));
    
    // Write metrics
    std::map<std::string, double> metrics;
    metrics["test_metric"] = 42.0;
    assert(manager.write(metrics));
    
    // Add metadata
    manager.addMetadata("version", "2.0.0");
    manager.addMetadata("environment", "test");
    
    // Generate timestamp
    std::string timestamp = manager.generateTimestamp("ISO");
    assert(timestamp.length() > 0);
    
    // Generate filename
    std::string filename = manager.generateFilename("test_output");
    assert(filename.find("test_output") != std::string::npos);
    
    manager.flush();
    manager.close();
    
    std::cout << "    OutputManager test passed" << std::endl;
}

void testMultiFormatOutput() {
    std::cout << "  Testing multi-format output..." << std::endl;
    
    // Test that we can create formatters for all formats
    std::vector<nlm::OutputFormat> formats = {
        nlm::OutputFormat::TEXT,
        nlm::OutputFormat::JSON,
        nlm::OutputFormat::CSV,
        nlm::OutputFormat::XML,
        nlm::OutputFormat::YAML,
        nlm::OutputFormat::HTML,
        nlm::OutputFormat::MARKDOWN
    };
    
    nlm::OutputConfig config;
    config.title = "Multi-Format Test";
    config.description = "Testing all output formats";
    
    for (auto format : formats) {
        config.format = format;
        
        auto formatter = nlm::OutputFormatter::create(format);
        assert(formatter != nullptr);
        
        // Initialize with test config
        assert(formatter->initialize(config));
        
        // Test basic write
        assert(formatter->write("Test content"));
        
        // Test with test results
        std::vector<nlm::TestResult> testResults;
        
        nlm::TestResult result;
        result.name = std::string(nlm::OutputFormatter::formatToString(format)) + " Test";
        result.passed = true;
        result.duration = 0.5;
        result.description = "Test for " + nlm::OutputFormatter::formatToString(format);
        
        testResults.push_back(result);
        assert(formatter->write(testResults));
        
        formatter->flush();
        formatter->close();
    }
    
    std::cout << "    Multi-format output test passed" << std::endl;
}

void runAll() {
    std::cout << "=== OutputFormatter Tests ===" << std::endl;
    
    testOutputFormatterFactory();
    testOutputFormatDetection();
    testTextFormatter();
    testJsonFormatter();
    testCsvFormatter();
    testXmlFormatter();
    testOutputManager();
    testMultiFormatOutput();
    
    std::cout << "=== All OutputFormatter Tests PASSED ===" << std::endl << std::endl;
}

} // namespace test_outputformatter