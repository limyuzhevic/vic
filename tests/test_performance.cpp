// Performance Tests
// Tests for performance improvements and validation enhancements

#include "core/Config/Config.hpp"
#include "core/ConfigManager/ConfigManager.hpp"
#include "cmd/CommandLineParser.hpp"
#include "output/OutputFormatter.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <filesystem>

namespace test_performance {

void testMemoryManagementEnhancements() {
    std::cout << "  Testing memory management enhancements..." << std::endl;
    
    // Test ConfigManager memory efficiency
    {
        nlm::ConfigManager manager1;
        
        // Set up configuration
        for (int i = 0; i < 1000; ++i) {
            manager1.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
        }
        
        // Get configuration summary (tests memory access)
        std::string summary = manager1.summary();
        assert(summary.find("1000") != std::string::npos);
        
        // Test cleanup
        manager1.clearTests(); // This would clear internal test state
    }
    
    // Test memory operations
    auto config = std::make_shared<nlm::Config>();
    
    // Test various memory operations
    config->set("large_int", int64_t(9223372036854775807), nlm::ConfigSource::Runtime);
    config->set("large_double", 1.7976931348623157e+308, nlm::ConfigSource::Runtime); // max double
    
    assert(config->get<int64_t>("large_int") == 9223372036854775807LL);
    assert(config->get<double>("large_double") == 1.7976931348623157e+308);
    
    std::cout << "    Memory management enhancements test passed" << std::endl;
}

void testConfigurationValidationPerformance() {
    std::cout << "  Testing configuration validation performance..." << std::endl;
    
    nlm::ConfigManager configManager;
    nlm::ConfigSchema schema;
    
    // Create a complex schema
    for (int i = 0; i < 100; ++i) {
        std::string paramName = "param_" + std::to_string(i);
        schema.addParameter(paramName, nlm::ConfigType::INT, 
                           "Parameter " + std::to_string(i), false, "0", "1000");
    }
    
    configManager.setSchema(schema);
    
    // Create test configuration
    nlm::Config config;
    for (int i = 0; i < 100; ++i) {
        config.set("param_" + std::to_string(i), i % 100, nlm::ConfigSource::Runtime);
    }
    
    // Measure validation performance
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::string> errors;
    bool valid = configManager.validate(config, errors);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(valid);
    assert(errors.empty());
    
    // Should validate 100 parameters in reasonable time (less than 10ms for simple validation)
    assert(duration.count() < 10000);
    
    std::cout << "    Configuration validation performance test passed (duration: " 
              << duration.count() << " us)" << std::endl;
}

void testCommandLineParsingPerformance() {
    std::cout << "  Testing command-line parsing performance..." << std::endl;
    
    nlm::CommandLineParser parser;
    
    // Add many options (simulating real-world usage)
    for (int i = 0; i < 50; ++i) {
        parser.addOption(std::string("option_" + std::to_string(i)), 
                        std::string(1, 'a' + (i % 26)), 
                        "Option " + std::to_string(i), "string", 
                        "default_" + std::to_string(i), 
                        "ENV_OPTION_" + std::to_string(i),
                        [](const std::string& v) { return !v.empty(); },
                        false, "general");
    }
    
    // Create test command line arguments
    int argc = 51; // Program name + 50 arguments
    char** argv = new char*[argc];
    argv[0] = const_cast<char*>("nlm_test");
    
    for (int i = 1; i < argc; ++i) {
        argv[i] = const_cast<char*>(("--option_" + std::to_string(i-1)).c_str());
    }
    
    // Measure parsing performance
    auto start = std::chrono::high_resolution_clock::now();
    
    nlm::ParsedArgs args;
    bool parseResult = parser.parse(argc, argv, args);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    delete[] argv;
    
    assert(parseResult);
    
    // Should parse 50 arguments in reasonable time (less than 100ms)
    assert(duration.count() < 100);
    
    std::cout << "    Command-line parsing performance test passed (duration: " 
              << duration.count() << " ms)" << std::endl;
}

void testOutputFormattingPerformance() {
    std::cout << "  Testing output formatting performance..." << std::endl;
    
    // Test performance with different output formats
    std::vector<nlm::OutputFormat> formats = {
        nlm::OutputFormat::TEXT,
        nlm::OutputFormat::JSON,
        nlm::OutputFormat::CSV,
        nlm::OutputFormat::XML
    };
    
    for (auto format : formats) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto formatter = nlm::OutputFormatter::create(format);
        assert(formatter != nullptr);
        
        nlm::OutputConfig config;
        config.format = format;
        config.title = "Performance Test";
        config.description = "Testing performance of " + nlm::OutputFormatter::formatToString(format);
        
        assert(formatter->initialize(config));
        
        // Write test results
        std::vector<nlm::TestResult> testResults;
        for (int i = 0; i < 20; ++i) {
            nlm::TestResult result;
            result.name = "Test " + std::to_string(i);
            result.passed = i % 2 == 0; // Alternate pass/fail
            result.duration = 0.1 + (i * 0.01);
            result.description = "Performance test case " + std::to_string(i);
            testResults.push_back(result);
        }
        
        assert(formatter->write(testResults));
        
        formatter->flush();
        formatter->close();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should process 20 test results in reasonable time (less than 50ms per format)
        assert(duration.count() < 50);
        
        std::cout << "      " << nlm::OutputFormatter::formatToString(format) 
                  << " format: " << duration.count() << " ms" << std::endl;
    }
    
    std::cout << "    Output formatting performance test passed" << std::endl;
}

void testEnhancedErrorHandlingPerformance() {
    std::cout << "  Testing enhanced error handling performance..." << std::endl;
    
    nlm::ConfigManager configManager;
    
    // Create schema with many validation rules
    nlm::ConfigSchema schema;
    for (int i = 0; i < 100; ++i) {
        schema.addParameter("param_" + std::to_string(i), nlm::ConfigType::INT,
                           "Parameter " + std::to_string(i), true, "0", "100");
    }
    
    configManager.setSchema(schema);
    
    // Create invalid configuration to test error handling
    nlm::Config invalidConfig;
    for (int i = 0; i < 100; ++i) {
        invalidConfig.set("param_" + std::to_string(i), i - 101, nlm::ConfigSource::Runtime); // All invalid
    }
    
    // Measure error detection performance
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::string> errors;
    bool valid = configManager.validate(invalidConfig, errors);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    assert(!valid);
    assert(errors.size() == 100); // Should have 100 errors
    
    // Should detect 100 validation errors in reasonable time (less than 10ms)
    assert(duration.count() < 10000);
    
    std::cout << "    Enhanced error handling performance test passed (duration: " 
              << duration.count() << " us, errors: " << errors.size() << ")" << std::endl;
}

void testMemorySystemOptionsPerformance() {
    std::cout << "  Testing memory system options performance..." << std::endl;
    
    // Test memory system configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set various memory-related configuration options
    config->set("working_memory_size", int64_t(1000000), nlm::ConfigSource::Runtime);
    config->set("episodic_memory_size", int64_t(100000), nlm::ConfigSource::Runtime);
    config->set("semantic_memory_size", int64_t(500000), nlm::ConfigSource::Runtime);
    config->set("procedural_memory_size", int64_t(200000), nlm::ConfigSource::Runtime);
    config->set("associative_memory_size", int64_t(300000), nlm::ConfigSource::Runtime);
    config->set("memory_cleanup_interval", int64_t(1000), nlm::ConfigSource::Runtime);
    config->set("memory_compress_threshold", 0.8f, nlm::ConfigSource::Runtime);
    config->set("memory_enable_compression", true, nlm::ConfigSource::Runtime);
    config->set("memory_tracking_enabled", true, nlm::ConfigSource::Runtime);
    
    // Measure memory configuration access performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        // Access various memory parameters
        config->getOr<int64_t>("working_memory_size", 0);
        config->getOr<int64_t>("episodic_memory_size", 0);
        config->getOr<int64_t>("semantic_memory_size", 0);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should perform 3000 memory accesses in reasonable time (less than 5ms)
    assert(duration.count() < 5000);
    
    std::cout << "    Memory system options performance test passed (duration: " 
              << duration.count() << " us)" << std::endl;
}

void testDebugAndProfilingOptionsPerformance() {
    std::cout << "  Testing debug and profiling options performance..." << std::endl;
    
    // Test debug and profiling configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set various debug and profiling options
    config->set("debug_mode", true, nlm::ConfigSource::CommandLine);
    config->set("log_level", std::string("DEBUG"), nlm::ConfigSource::CommandLine);
    config->set("enable_metrics", true, nlm::ConfigSource::CommandLine);
    config->set("profile_steps", int64_t(100), nlm::ConfigSource::CommandLine);
    config->set("benchmark", true, nlm::ConfigSource::CommandLine);
    config->set("validation", true, nlm::ConfigSource::CommandLine);
    config->set("show_timing", true, nlm::ConfigSource::CommandLine);
    config->set("enable_profiling", true, nlm::ConfigSource::CommandLine);
    config->set("memory_profiling", true, nlm::ConfigSource::CommandLine);
    config->set("cpu_profiling", true, nlm::ConfigSource::CommandLine);
    
    // Measure debug/profiling configuration access performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        // Access various debug/profiling parameters
        config->getOr<bool>("debug_mode", false);
        config->getOr<std::string>("log_level", "INFO");
        config->getOr<bool>("enable_metrics", false);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should perform 3000 debug/profiling accesses in reasonable time (less than 5ms)
    assert(duration.count() < 5000);
    
    std::cout << "    Debug and profiling options performance test passed (duration: " 
              << duration.count() << " us)" << std::endl;
}

void runAll() {
    std::cout << "=== Performance Tests ===" << std::endl;
    
    testMemoryManagementEnhancements();
    testConfigurationValidationPerformance();
    testCommandLineParsingPerformance();
    testOutputFormattingPerformance();
    testEnhancedErrorHandlingPerformance();
    testMemorySystemOptionsPerformance();
    testDebugAndProfilingOptionsPerformance();
    
    std::cout << "=== All Performance Tests PASSED ===" << std::endl << std::endl;
}

} // namespace test_performance