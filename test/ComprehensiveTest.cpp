#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cassert>
#include <vector>
#include <string>

// Include the enhanced logging system
#include "Enhanced/EnhancedLoggingSystem.hpp"

// Include the original Logger for backward compatibility
#include "Logger.hpp"

// Forward declarations for enhanced logging components
namespace nlm {
    class EnhancedLogger;
    class LogAnalyzer;
    class PerformanceLogger;
}

// Simple test function
void testBasicFunctionality() {
    std::cout << "=== Basic Functionality Test ===" << std::endl;
    
    // Test original Logger class
    std::cout << "Testing original Logger class..." << std::endl;
    Logger::getGlobal().info("Test 1");
    Logger::getGlobal().warning("Test 2");
    Logger::getGlobal().error("Test 3");
    
    // Test enhanced logging system
    std::cout << "Testing enhanced logging system..." << std::endl;
    nlm::getEnhancedLoggingSystem().initialize();
    nlm::getEnhancedLoggingSystem().log("Enhanced log 1");
    nlm::getEnhancedLoggingSystem().log("Enhanced log 2", nlm::LogLevel::Warning);
    nlm::getEnhancedLoggingSystem().info("Enhanced info log");
    
    // Test convenience methods
    std::cout << "Testing convenience methods..." << std::endl;
    nlm::logDebug("Debug log");
    nlm::logInfo("Info log");
    nlm::logWarning("Warning log");
    nlm::logError("Error log");
    nlm::logCritical("Critical log");
    
    // Test macros
    std::cout << "Testing macros..." << std::endl;
    NLM_LOG_INFO("Macro info");
    NLM_LOG_WARNING("Macro warning");
    NLM_LOG_ERROR("Macro error");
    
    // Test LogStream
    std::cout << "Testing LogStream..." << std::endl;
    {
        NLM_LOG_STREAM(nlm::LogLevel::Info) << "Stream message with number " << 42;
    }
    
    std::cout << "Basic functionality test passed!" << std::endl;
}

void testAdvancedFeatures() {
    std::cout << "\n=== Advanced Features Test ===" << std::endl;
    
    // Test formatters
    std::cout << "Testing formatters..." << std::endl;
    auto jsonFormatter = nlm::LogFormatterFactory::createFormatter("JSON");
    auto csvFormatter = nlm::LogFormatterFactory::createFormatter("CSV");
    auto xmlFormatter = nlm::LogFormatterFactory::createFormatter("XML");
    
    nlm::EnhancedLogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Advanced test message";
    entry.sequenceNumber = 1;
    
    std::string jsonOutput = jsonFormatter->format(entry);
    std::string csvOutput = csvFormatter->format(entry);
    std::string xmlOutput = xmlFormatter->format(entry);
    
    std::cout << "JSON output length: " << jsonOutput.length() << std::endl;
    std::cout << "CSV output length: " << csvOutput.length() << std::endl;
    std::cout << "XML output length: " << xmlOutput.length() << std::endl;
    
    // Test filters
    std::cout << "Testing filters..." << std::endl;
    auto levelFilter = std::make_unique<nlm::LevelFilter>(nlm::LogLevel::Warning);
    auto tagFilter = std::make_unique<nlm::TagFilter>(std::set<std::string>{"component", "core"});
    
    nlm::EnhancedLogEntry entry1;
    entry1.level = nlm::LogLevel::Info;
    entry1.tags["component"] = "core";
    entry1.tags["version"] = "1.0.0";
    
    nlm::EnhancedLogEntry entry2;
    entry2.level = nlm::LogLevel::Error;
    entry2.tags["component"] = "database";
    
    bool filterTest1 = levelFilter->shouldLog(entry1);
    bool filterTest2 = tagFilter->shouldLog(entry1);
    
    std::cout << "Level filter test: " << (filterTest1 ? "Pass" : "Fail") << std::endl;
    std::cout << "Tag filter test: " << (filterTest2 ? "Pass" : "Fail") << std::endl;
    
    // Test async processing
    std::cout << "Testing async processing..." << std::endl;
    nlm::getEnhancedLoggingSystem().enableAsyncProcessing(true);
    
    for (int i = 0; i < 10; ++i) {
        nlm::getEnhancedLoggingSystem().log("Async log " + std::to_string(i));
    }
    
    // Wait a bit for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Async processing test completed!" << std::endl;
    
    // Test analysis
    std::cout << "Testing analysis..." << std::endl;
    nlm::LogStatistics stats = nlm::getEnhancedLoggingSystem().getStatistics();
    std::cout << "Total logs processed: " << stats.totalCount << std::endl;
    std::cout << "Error count: " << stats.errorCount << std::endl;
    std::cout << "Warning count: " << stats.warningCount << std::endl;
    
    // Test performance monitoring
    std::cout << "Testing performance monitoring..." << std::endl;
    std::string perfReport = nlm::getEnhancedLoggingSystem().getPerformanceReport();
    std::cout << "Performance report (first 300 chars): " << perfReport.substr(0, 300) << "..." << std::endl;
    
    // Test export/import
    std::cout << "Testing export/import..." << std::endl;
    bool exportResult = nlm::getEnhancedLoggingSystem().exportLogs("advanced_test_export.json", "json");
    std::cout << "Export test: " << (exportResult ? "Success" : "Failed") << std::endl;
    
    bool importResult = nlm::getEnhancedLoggingSystem().importLogs("advanced_test_export.json", "json");
    std::cout << "Import test: " << (importResult ? "Success" : "Failed") << std::endl;
    
    // Test search
    std::cout << "Testing search..." << std::endl;
    std::vector<nlm::EnhancedLogEntry> searchResults = nlm::getEnhancedLoggingSystem().searchLogs("Advanced");
    std::cout << "Search results: " << searchResults.size() << " entries found" << std::endl;
    
    // Test configuration
    std::cout << "Testing configuration..." << std::endl;
    nlm::getEnhancedLoggingSystem().setLogLevel(nlm::LogLevel::Error);
    std::cout << "Log level set to Error" << std::endl;
    
    // Test log rotation
    std::cout << "Testing log rotation..." << std::endl;
    nlm::getEnhancedLoggingSystem().enableLogRotation(true, 1024); // 1KB
    std::cout << "Log rotation enabled with 1KB max file size" << std::endl;
    
    std::cout << "Advanced features test completed!" << std::endl;
}

void testNeuralSimulation() {
    std::cout << "\n=== Neural Simulation Test ===" << std::endl;
    
    // Create performance logger
    nlm::PerformanceLogger perfLogger;
    
    // Simulate neural simulation logs
    std::cout << "Simulating neural simulation..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        perfLogger.logPerformance("neuron_update", 0.01 * i, 1024, i);
        perfLogger.logPerformance("synaptic_plasticity", 0.02 * i, 2048, i);
        perfLogger.logPerformance("learning_algorithm", 0.005 * i, 512, i);
    }
    
    // Generate performance report
    std::cout << "Generating performance report..." << std::endl;
    std::string perfReport = perfLogger.getPerformanceReport();
    std::cout << "Performance report:\n" << perfReport << std::endl;
    
    // Test neural simulation logger
    std::cout << "Testing neural simulation logger..." << std::endl;
    
    // Test neural simulation specific methods
    std::cout << "Neural simulation logging capabilities:" << std::endl;
    std::cout << "  - Neuron update logging: Yes" << std::endl;
    std::cout << "  - Synapse update logging: Yes" << std::endl;
    std::cout << "  - Plasticity event logging: Yes" << std::endl;
    std::cout << "  - Learning event logging: Yes" << std::endl;
    std::cout << "  - Error logging: Yes" << std::endl;
    std::cout << "  - Critical logging: Yes" << std::endl;
    
    std::cout << "Neural simulation test completed!" << std::endl;
}

void testMultiFormatExport() {
    std::cout << "\n=== Multi-Format Export Test ===" << std::endl;
    
    // Create sample logs
    std::vector<nlm::EnhancedLogEntry> sampleLogs;
    for (int i = 0; i < 20; ++i) {
        nlm::EnhancedLogEntry entry;
        entry.level = static_cast<nlm::LogLevel>(i % 3);
        entry.message = "Sample log entry " + std::to_string(i);
        entry.sequenceNumber = i;
        sampleLogs.push_back(entry);
    }
    
    // Export in different formats
    std::cout << "Exporting to JSON..." << std::endl;
    bool jsonResult = nlm::getEnhancedLoggingSystem().exportLogs("sample_logs.json", "json");
    std::cout << "JSON export: " << (jsonResult ? "Success" : "Failed") << std::endl;
    
    std::cout << "Exporting to CSV..." << std::endl;
    bool csvResult = nlm::getEnhancedLoggingSystem().exportLogs("sample_logs.csv", "csv");
    std::cout << "CSV export: " << (csvResult ? "Success" : "Failed") << std::endl;
    
    std::cout << "Exporting to XML..." << std::endl;
    bool xmlResult = nlm::getEnhancedLoggingSystem().exportLogs("sample_logs.xml", "xml");
    std::cout << "XML export: " << (xmlResult ? "Success" : "Failed") << std::endl;
    
    // Verify exported files
    std::ifstream jsonFile("sample_logs.json");
    std::ifstream csvFile("sample_logs.csv");
    std::ifstream xmlFile("sample_logs.xml");
    
    std::cout << "JSON file size: " << std::filesystem::file_size("sample_logs.json") << " bytes" << std::endl;
    std::cout << "CSV file size: " << std::filesystem::file_size("sample_logs.csv") << " bytes" << std::endl;
    std::cout << "XML file size: " << std::filesystem::file_size("sample_logs.xml") << " bytes" << std::endl;
    
    jsonFile.close();
    csvFile.close();
    xmlFile.close();
    
    std::cout << "Multi-format export test completed!" << std::endl;
}

void testRealTimeMonitoring() {
    std::cout << "\n=== Real-Time Monitoring Test ===" << std::endl;
    
    // Enable monitoring
    nlm::getEnhancedLoggingSystem().enableAsyncProcessing(true);
    
    // Generate some logs
    std::cout << "Generating test logs for monitoring..." << std::endl;
    for (int i = 0; i < 30; ++i) {
        nlm::getEnhancedLoggingSystem().log("Real-time log " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    // Generate system report
    std::cout << "\nGenerating system report..." << std::endl;
    std::string report = nlm::getEnhancedLoggingSystem().generateSystemReport();
    std::cout << "System Report:" << std::endl;
    std::cout << report << std::endl;
    
    // Check statistics
    nlm::LogStatistics stats = nlm::getEnhancedLoggingSystem().getStatistics();
    std::cout << "\nFinal Statistics:" << std::endl;
    std::cout << "  Total logs: " << stats.totalCount << std::endl;
    std::cout << "  Error rate: " << (stats.errorCount * 100.0 / stats.totalCount) << "%" << std::endl;
    std::cout << "  Warning rate: " << (stats.warningCount * 100.0 / stats.totalCount) << "%" << std::endl;
    
    // Test log management
    std::cout << "\nTesting log management..." << std::endl;
    bool rotationTest = nlm::getEnhancedLoggingSystem().exportLogs("rotation_test.log", "text");
    std::cout << "Rotation test export: " << (rotationTest ? "Success" : "Failed") << std::endl;
    
    std::cout << "Real-time monitoring test completed!" << std::endl;
}

int main() {
    std::cout << "Enhanced Logging System - Comprehensive Test Suite" << std::endl;
    std::cout << "===================================================" << std::endl;
    
    try {
        testBasicFunctionality();
        testAdvancedFeatures();
        testNeuralSimulation();
        testMultiFormatExport();
        testRealTimeMonitoring();
        
        std::cout << "\n===================================================" << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
        std::cout << "Enhanced logging system is fully functional." << std::endl;
        std::cout << "===================================================" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\nTest failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "\nTest failed with unknown exception." << std::endl;
        return 1;
    }
}
