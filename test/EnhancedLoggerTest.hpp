#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cassert>
#include <vector>
#include <string>
#include "Logger.hpp"
#include "Enhanced/EnhancedLoggingSystem.hpp"
#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/LogFilter.hpp"

namespace nlm {

// Backward compatibility test class
class BackwardCompatibilityTest {
public:
    static void testOriginalLoggerCompatibility() {
        std::cout << "Testing original Logger class compatibility..." << std::endl;
        
        // Test 1: Basic logging functionality
        std::cout << "1. Testing basic logging..." << std::endl;
        Logger::getGlobal().info("Test message 1");
        Logger::getGlobal().warning("Test message 2");
        Logger::getGlobal().error("Test message 3");
        
        // Test 2: Convenience methods
        std::cout << "2. Testing convenience methods..." << std::endl;
        Logger::getGlobal().debug("Debug message");
        Logger::getGlobal().info("Info message");
        Logger::getGlobal().warning("Warning message");
        Logger::getGlobal().error("Error message");
        Logger::getGlobal().critical("Critical message");
        
        // Test 3: Macros
        std::cout << "3. Testing macros..." << std::endl;
        NLM_LOG_INFO("Macro info message");
        NLM_LOG_WARNING("Macro warning message");
        NLM_LOG_ERROR("Macro error message");
        
        // Test 4: LogStream
        std::cout << "4. Testing LogStream..." << std::endl;
        {
            NLM_LOG_STREAM(LogLevel::Info) << "Stream message with " << 42 << " and " << "string";
        }
        
        // Test 5: Level checking
        std::cout << "5. Testing level checking..." << std::endl;
        bool debugEnabled = Logger::getGlobal().isEnabled(LogLevel::Debug);
        bool infoEnabled = Logger::getGlobal().isEnabled(LogLevel::Info);
        std::cout << "Debug enabled: " << (debugEnabled ? "Yes" : "No") << std::endl;
        std::cout << "Info enabled: " << (infoEnabled ? "Yes" : "No") << std::endl;
        
        // Test 6: Logger management
        std::cout << "6. Testing logger management..." << std::endl;
        auto consoleLogger = std::make_shared<ConsoleLogger>();
        consoleLogger->setLevel(LogLevel::Warning);
        Logger::getGlobal().addLogger(consoleLogger);
        
        Logger::getGlobal().info("This should not appear (below warning level)");
        Logger::getGlobal().warning("This should appear (warning level)");
        
        Logger::getGlobal().clearLoggers();
        
        std::cout << "Original Logger compatibility test passed!" << std::endl;
    }
    
    static void testEnhancedFeatures() {
        std::cout << "\nTesting enhanced logging features..." << std::endl;
        
        // Initialize enhanced logging system
        getEnhancedLoggingSystem().initialize();
        
        // Test 1: Enhanced logging
        std::cout << "1. Testing enhanced logging..." << std::endl;
        getEnhancedLoggingSystem().log("Enhanced log message 1");
        getEnhancedLoggingSystem().log("Enhanced log message 2", LogLevel::Warning);
        getEnhancedLoggingSystem().info("Enhanced info message");
        
        // Test 2: Enhanced formatters
        std::cout << "2. Testing enhanced formatters..." << std::endl;
        auto jsonFormatter = LogFormatterFactory::createFormatter("JSON");
        auto csvFormatter = LogFormatterFactory::createFormatter("CSV");
        
        EnhancedLogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = "Test for formatting";
        entry.sequenceNumber = 1;
        
        std::cout << "JSON: " << jsonFormatter->format(entry) << std::endl;
        std::cout << "CSV: " << csvFormatter->format(entry) << std::endl;
        
        // Test 3: Enhanced filters
        std::cout << "3. Testing enhanced filters..." << stdendl;
        auto levelFilter = std::make_unique<LevelFilter>(LogLevel::Info);
        auto tagFilter = std::make_unique<TagFilter>(std::set<std::string>{"component", "core"});
        
        EnhancedLogEntry entry1;
        entry1.level = LogLevel::Info;
        entry1.tags["component"] = "core";
        entry1.tags["version"] = "1.0.0";
        
        EnhancedLogEntry entry2;
        entry2.level = LogLevel::Error;
        entry2.tags["component"] = "database";
        
        std::cout << "Entry1 with level and tag filter: " 
                  << (levelFilter->shouldLog(entry1) ? "Pass" : "Fail") << std::endl;
        std::cout << "Entry2 with level and tag filter: " 
                  << (levelFilter->shouldLog(entry2) ? "Pass" : "Fail") << std::endl;
        
        // Test 4: Async processing
        std::cout << "4. Testing async processing..." << std::endl;
        getEnhancedLoggingSystem().enableAsyncProcessing(true);
        
        for (int i = 0; i < 5; ++i) {
            getEnhancedLoggingSystem().log("Async log message " + std::to_string(i));
        }
        
        std::cout << "Async processing enabled and logs sent" << std::endl;
        
        // Test 5: Log analysis
        std::cout << "5. Testing log analysis..." << std::endl;
        LogStatistics stats = getEnhancedLoggingSystem().getStatistics();
        std::cout << "Total logs processed: " << stats.totalCount << std::endl;
        std::cout << "Error count: " << stats.errorCount << std::endl;
        std::cout << "Warning count: " << stats.warningCount << std::endl;
        
        // Test 6: Performance monitoring
        std::cout << "6. Testing performance monitoring..." << std::endl;
        std::string perfReport = getEnhancedLoggingSystem().getPerformanceReport();
        std::cout << "Performance report (first 200 chars): " << perfReport.substr(0, 200) << "..." << std::endl;
        
        // Test 7: Export/import
        std::cout << "7. Testing export/import..." << std::endl;
        bool exportResult = getEnhancedLoggingSystem().exportLogs("test_export.json", "json");
        std::cout << "Export result: " << (exportResult ? "Success" : "Failed") << std::endl;
        
        bool importResult = getEnhancedLoggingSystem().importLogs("test_export.json", "json");
        std::cout << "Import result: " << (importResult ? "Success" : "Failed") << std::endl;
        
        // Test 8: Log search
        std::cout << "8. Testing log search..." << std::endl;
        std::vector<EnhancedLogEntry> searchResults = getEnhancedLoggingSystem().searchLogs("Enhanced");
        std::cout << "Search results for 'Enhanced': " << searchResults.size() << " entries found" << std::endl;
        
        // Test 9: Configuration
        std::cout << "9. Testing configuration..." << std::endl;
        getEnhancedLoggingSystem().setLogLevel(LogLevel::Warning);
        std::cout << "Log level set to Warning" << std::endl;
        
        // Test 10: Log rotation
        std::cout << "10. Testing log rotation..." << std::endl;
        getEnhancedLoggingSystem().enableLogRotation(true, 1024); // 1KB
        std::cout << "Log rotation enabled with 1KB max file size" << std::endl;
        
        std::cout << "Enhanced features test completed!" << std::endl;
    }
    
    static void testNeuralSimulationLogging() {
        std::cout << "\nTesting neural simulation logging..." << std::endl;
        
        // Create performance logger
        PerformanceLogger perfLogger;
        
        // Simulate neural simulation logs
        for (int i = 0; i < 3; ++i) {
            perfLogger.logPerformance("neuron_update", 0.01 * i, 1024, i);
            perfLogger.logPerformance("synaptic_plasticity", 0.02 * i, 2048, i);
            perfLogger.logPerformance("learning_algorithm", 0.005 * i, 512, i);
        }
        
        std::cout << "Performance report:" << std::endl;
        std::string perfReport = perfLogger.getPerformanceReport();
        std::cout << perfReport << std::endl;
        
        std::cout << "Neural simulation logging test completed!" << std::endl;
    }
    
    static void testMultiFormatExport() {
        std::cout << "\nTesting multi-format export..." << std::endl;
        
        // Create sample logs
        std::vector<EnhancedLogEntry> sampleLogs;
        for (int i = 0; i < 10; ++i) {
            EnhancedLogEntry entry;
            entry.level = static_cast<LogLevel>(i % 3);
            entry.message = "Sample log entry " + std::to_string(i);
            entry.sequenceNumber = i;
            sampleLogs.push_back(entry);
        }
        
        // Export in different formats
        std::cout << "Exporting to JSON..." << std::endl;
        bool jsonResult = getEnhancedLoggingSystem().exportLogs("sample_logs.json", "json");
        std::cout << "JSON export: " << (jsonResult ? "Success" : "Failed") << std::endl;
        
        std::cout << "Exporting to CSV..." << std::endl;
        bool csvResult = getEnhancedLoggingSystem().exportLogs("sample_logs.csv", "csv");
        std::cout << "CSV export: " << (csvResult ? "Success" : "Failed") << std::endl;
        
        std::cout << "Exporting to XML..." << std::endl;
        bool xmlResult = getEnhancedLoggingSystem().exportLogs("sample_logs.xml", "xml");
        std::cout << "XML export: " << (xmlResult ? "Success" : "Failed") << std::endl;
        
        std::cout << "Multi-format export test completed!" << std::endl;
    }
    
    static void testRealTimeMonitoring() {
        std::cout << "\nTesting real-time monitoring..." << std::endl;
        
        // Enable monitoring
        getEnhancedLoggingSystem().enableAsyncProcessing(true);
        
        // Generate some logs
        std::cout << "Generating test logs for monitoring..." << std::endl;
        for (int i = 0; i < 20; ++i) {
            getEnhancedLoggingSystem().log("Real-time log " + std::to_string(i));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        // Generate system report
        std::cout << "\nGenerating system report..." << std::endl;
        std::string report = getEnhancedLoggingSystem().generateSystemReport();
        std::cout << report << std::endl;
        
        // Check statistics
        LogStatistics stats = getEnhancedLoggingSystem().getStatistics();
        std::cout << "\nFinal Statistics:" << std::endl;
        std::cout << "  Total logs: " << stats.totalCount << std::endl;
        std::cout << "  Error rate: " << (stats.errorCount * 100.0 / stats.totalCount) << "%" << std::endl;
        std::cout << "  Warning rate: " << (stats.warningCount * 100.0 / stats.totalCount) << "%" << std::endl;
        
        std::cout << "Real-time monitoring test completed!" << std::endl;
    }
    
    static void runAllTests() {
        std::cout << "=========================================" << std::endl;
        std::cout << "Enhanced Logging System - Backward Compatibility Test" << std::endl;
        std::cout << "=========================================" << std::endl;
        
        try {
            testOriginalLoggerCompatibility();
            testEnhancedFeatures();
            testNeuralSimulationLogging();
            testMultiFormatExport();
            testRealTimeMonitoring();
            
            std::cout << "\n=========================================" << std::endl;
            std::cout << "All tests completed successfully!" << std::endl;
            std::cout << "=========================================" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "\nTest failed with exception: " << e.what() << std::endl;
            throw;
        }
    }
};

} // namespace nlm
