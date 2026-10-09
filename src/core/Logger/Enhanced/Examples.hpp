#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/LogFilter.hpp"
#include "Enhanced/AsyncProcessor.hpp"
#include "Enhanced/Analysis.hpp"
#include "Enhanced/Management.hpp"
#include "Enhanced/Performance.hpp"

namespace nlm {

// Integration Example 1: Basic Enhanced Logging
class BasicEnhancedLoggingExample {
public:
    static void demonstrate() {
        std::cout << "=== Basic Enhanced Logging Example ===\n";
        
        // Create enhanced log formatters
        auto jsonFormatter = LogFormatterFactory::createFormatter("JSON");
        auto csvFormatter = LogFormatterFactory::createFormatter("CSV");
        auto templateFormatter = LogFormatterFactory::createFormatter("Template", "{timestamp} [{level}] {message}");
        
        // Create an enhanced log entry
        EnhancedLogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = "Application started successfully";
        entry.file = "main.cpp";
        entry.line = 42;
        entry.function = "main";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main_thread";
        entry.tags["component"] = "core";
        entry.tags["version"] = "1.0.0";
        entry.metadata["severity"] = "medium";
        entry.sequenceNumber = 1;
        
        // Demonstrate different formatters
        std::cout << "JSON Format:" << std::endl;
        std::cout << jsonFormatter->format(entry) << std::endl;
        
        std::cout << "CSV Format:" << std::endl;
        std::cout << csvFormatter->format(entry) << std::endl;
        
        std::cout << "Template Format:" << std::endl;
        std::cout << templateFormatter->format(entry) << std::endl;
        
        std::cout << "\n";
    }
};

// Integration Example 2: Advanced Filtering
class AdvancedFilteringExample {
public:
    static void demonstrate() {
        std::cout << "=== Advanced Filtering Example ===\n";
        
        // Create composite filter with multiple criteria
        auto levelFilter = LogFilterFactory::createFilter("Level", "Info");
        auto tagFilter = LogFilterFactory::createFilter("Tag", "component,core");
        
        auto compositeFilter = std::make_unique<CompositeFilter>(CompositeFilter::Operation::AND);
        compositeFilter->addFilter(std::move(levelFilter));
        compositeFilter->addFilter(std::move(tagFilter));
        
        std::cout << "Composite filter: " << compositeFilter->getName() << std::endl;
        
        // Test with log entries
        EnhancedLogEntry entry1;
        entry1.level = LogLevel::Info;
        entry1.message = "This should pass";
        entry1.tags["component"] = "core";
        entry1.tags["version"] = "1.0.0";
        
        EnhancedLogEntry entry2;
        entry2.level = LogLevel::Warning;
        entry2.message = "This should fail";
        entry2.tags["component"] = "database";
        
        EnhancedLogEntry entry3;
        entry3.level = LogLevel::Info;
        entry3.message = "Another entry";
        entry3.tags["component"] = "database";
        
        std::cout << "Entry1 passes: " << (compositeFilter->shouldLog(entry1) ? "Yes" : "No") << std::endl;
        std::cout << "Entry2 passes: " << (compositeFilter->shouldLog(entry2) ? "Yes" : "No") << std::endl;
        std::cout << "Entry3 passes: " << (compositeFilter->shouldLog(entry3) ? "Yes" : "No") << std::endl;
        
        std::cout << "\n";
    }
};

// Integration Example 3: Async Log Processing
class AsyncLogProcessingExample {
public:
    static void demonstrate() {
        std::cout << "=== Async Log Processing Example ===\n";
        
        // Create async processor
        auto asyncProcessor = std::make_unique<AsyncLogProcessor>(4);
        
        // Add formatters
        asyncProcessor->addFormatter(LogFormatterFactory::createFormatter("JSON"));
        asyncProcessor->addFormatter(LogFormatterFactory::createFormatter("CSV"));
        
        // Create log entries
        std::vector<EnhancedLogEntry> logs;
        for (int i = 0; i < 5; ++i) {
            EnhancedLogEntry entry;
            entry.level = static_cast<LogLevel>(i % 3);
            entry.message = "Async log message " + std::to_string(i);
            entry.file = "async_example.cpp";
            entry.line = i + 1;
            entry.function = "asyncFunction";
            entry.timestamp = std::chrono::system_clock::now();
            entry.threadId = "async_thread";
            entry.sequenceNumber = i;
            logs.push_back(entry);
            
            asyncProcessor->addLog(std::make_unique<EnhancedLogEntry>(entry));
        }
        
        // Wait for completion
        asyncProcessor->waitForCompletion();
        
        std::cout << "Processed " << asyncProcessor->getProcessedCount() << " log entries" << std::endl;
        std::cout << "Errors: " << asyncProcessor->getErrorCount() << std::endl;
        
        std::cout << "\n";
    }
};

// Integration Example 4: Log Analysis and Pattern Detection
class LogAnalysisExample {
public:
    static void demonstrate() {
        std::cout << "=== Log Analysis Example ===\n";
        
        // Create log analyzer
        LogAnalyzer analyzer(1000);
        
        // Add sample logs
        for (int i = 0; i < 100; ++i) {
            EnhancedLogEntry entry;
            entry.level = (i % 10 < 7) ? LogLevel::Info : LogLevel::Error;
            entry.message = "Sample log entry number " + std::to_string(i);
            entry.file = "sample.cpp";
            entry.line = 42;
            entry.function = "sampleFunction";
            entry.timestamp = std::chrono::system_clock::now();
            entry.threadId = "thread_" + std::to_string(i % 4);
            entry.tags["batch"] = std::to_string(i / 20);
            entry.sequenceNumber = i;
            
            analyzer.addLog(entry);
        }
        
        // Get statistics
        LogStatistics stats = analyzer.getStatistics();
        std::cout << "Total logs: " << stats.totalCount << std::endl;
        std::cout << "Error rate: " << (stats.errorCount * 100.0 / stats.totalCount) << "%" << std::endl;
        
        // Get pattern matches
        std::vector<PatternMatch> patterns = analyzer.getPatternMatches();
        std::cout << "Pattern matches: " << patterns.size() << std::endl;
        
        // Generate report
        std::string report = analyzer.generateReport("text");
        std::cout << "\nReport:\n" << report << std::endl;
        
        std::cout << "\n";
    }
};

// Integration Example 5: Log Management and Rotation
class LogManagementExample {
public:
    static void demonstrate() {
        std::cout << "=== Log Management Example ===\n";
        
        // Create log manager
        LogManager logManager("logs/example");
        logManager.addLogFilePattern("*.log");
        
        // Create rotation policy
        SizeRotationPolicy sizePolicy(1024); // Rotate when log reaches 1KB
        
        // Simulate log files
        std::vector<LogFileInfo> logFiles;
        for (int i = 0; i < 3; ++i) {
            LogFileInfo fileInfo;
            fileInfo.path = "logs/example/app_" + std::to_string(i) + ".log";
            fileInfo.name = "app_" + std::to_string(i) + ".log";
            fileInfo.size = i * 500; // Varying sizes
            fileInfo.creationTime = std::chrono::system_clock::now();
            fileInfo.modificationTime = std::chrono::system_clock::now();
            fileInfo.lineCount = 100 + i * 50;
            fileInfo.format = "text";
            
            logFiles.push_back(fileInfo);
        }
        
        // Demonstrate search
        std::cout << "Searching for logs with 'important' keyword..." << std::endl;
        std::vector<EnhancedLogEntry> searchResults = logManager.searchLogs("important");
        std::cout << "Found " << searchResults.size() << " matching logs" << std::endl;
        
        // Demonstrate export
        std::cout << "Exporting logs to JSON..." << std::endl;
        std::vector<EnhancedLogEntry> sampleLogs;
        EnhancedLogEntry sampleEntry;
        sampleEntry.level = LogLevel::Info;
        sampleEntry.message = "Sample log for export";
        sampleEntry.sequenceNumber = 1;
        sampleLogs.push_back(sampleEntry);
        
        if (logManager.exportLogs("exported_logs.json", sampleLogs, "json")) {
            std::cout << "Export successful!" << std::endl;
        } else {
            std::cout << "Export failed!" << std::endl;
        }
        
        // Demonstrate visualization
        std::cout << "Generating log visualization..." << std::endl;
        std::string visualization = logManager.generateLogVisualization(sampleLogs, "text");
        std::cout << "Visualization (first 500 chars):" << std::endl;
        std::cout << visualization.substr(0, std::min(500, static_cast<int>(visualization.size()))) << "..." << std::endl;
        
        std::cout << "\n";
    }
};

// Integration Example 6: Performance Optimization
class PerformanceOptimizationExample {
public:
    static void demonstrate() {
        std::cout << "=== Performance Optimization Example ===\n";
        
        // Create ultra-fast logger
        UltraFastLogger ultraLogger;
        ultraLogger.setLogThreshold(LogLevel::Warning);
        
        // Demonstrate fast logging
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 10000; ++i) {
            ultraLogger.log(LogLevel::Info, "Performance test log message " + std::to_string(i));
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Logged 10,000 messages in " << duration.count() << " ms" << std::endl;
        std::cout << "Average time per log: " << (duration.count() * 1000.0 / 10000) << " microseconds" << std::endl;
        
        // Create neural simulation logger
        NeuralSimulationLogger neuroLogger;
        neuroLogger.setEnabled(true);
        
        std::cout << "Logging neural simulation events..." << std::endl;
        neuroLogger.logNeuronUpdate("neuron_1", 0.5, 0.8, 100);
        neuroLogger.logSynapseUpdate("synapse_1", "synapse_2", -0.1, 0.4);
        neuroLogger.logPlasticityEvent("long_term_potentiation", "synaptic strengthening");
        neuroLogger.logLearningEvent("backpropagation", 0.95);
        
        std::cout << "Neural simulation logging complete" << std::endl;
        
        std::cout << "\n";
    }
};

// Integration Example 7: Complete System Integration
class CompleteSystemIntegrationExample {
public:
    static void demonstrate() {
        std::cout << "=== Complete System Integration Example ===\n";
        
        // Create log analyzer
        LogAnalyzer analyzer(10000);
        
        // Create log manager
        LogManager logManager("logs/integrated");
        logManager.addLogFilePattern("*.log");
        
        // Create async processor
        auto asyncProcessor = std::make_unique<AsyncLogProcessor>(2);
        asyncProcessor->addFormatter(LogFormatterFactory::createFormatter("JSON"));
        asyncProcessor->setOutputCallback([&](const std::string& formatted) {
            // Store for analysis
            std::cout << "Processed: " << formatted.substr(0, 100) << "..." << std::endl;
        });
        
        // Create performance logger
        PerformanceLogger perfLogger;
        
        // Simulate application logs
        for (int i = 0; i < 20; ++i) {
            EnhancedLogEntry entry;
            entry.level = (i % 5 == 0) ? LogLevel::Error : 
                         (i % 3 == 0) ? LogLevel::Warning : LogLevel::Info;
            entry.message = "Integrated application log " + std::to_string(i);
            entry.file = "integrated_app.cpp";
            entry.line = i + 1;
            entry.function = "integratedFunction";
            entry.timestamp = std::chrono::system_clock::now();
            entry.threadId = "worker_thread";
            entry.tags["module"] = (i % 3 == 0) ? "database" : (i % 3 == 1) ? "network" : "cache";
            entry.metadata["performance"] = std::to_string(i * 10.0);
            entry.sequenceNumber = i;
            
            // Add to analyzer
            analyzer.addLog(entry);
            
            // Add to async processor
            asyncProcessor->addLog(std::make_unique<EnhancedLogEntry>(entry));
            
            // Log performance metrics
            perfLogger.logPerformance("log_processing", i * 0.001, 1024, i);
        }
        
        // Wait for async processing
        asyncProcessor->waitForCompletion();
        
        // Generate comprehensive report
        std::cout << "\n=== Comprehensive Analysis Report ===" << std::endl;
        std::cout << "Application Statistics:" << std::endl;
        LogStatistics stats = analyzer.getStatistics();
        std::cout << "  Total logs: " << stats.totalCount << std::endl;
        std::cout << "  Error rate: " << (stats.errorCount * 100.0 / stats.totalCount) << "%" << std::endl;
        
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::string perfReport = perfLogger.getPerformanceReport();
        std::cout << "  " << perfReport << std::endl;
        
        std::cout << "\nSystem Integration Complete!" << std::endl;
    }
};

// Integration Example 8: Backward Compatibility Bridge
class BackwardCompatibilityExample {
public:
    static void demonstrate() {
        std::cout << "=== Backward Compatibility Example ===\n";
        
        std::cout << "This example shows how to use the enhanced logging system" << std::endl;
        std::cout << "while maintaining compatibility with the original Logger class." << std::endl;
        std::cout << std::endl;
        
        // The original Logger class can still be used as before
        std::cout << "Original usage still works:" << std::endl;
        std::cout << "  Logger::getGlobal().info(\"Message\");" << std::endl;
        std::cout << "  Logger::getGlobal().addLogger(std::make_shared<ConsoleLogger>());" << std::endl;
        std::cout << std::endl;
        
        // Enhanced features can be added alongside existing functionality
        std::cout << "Enhanced features can be added:
        - Multiple formatters
        - Advanced filtering
        - Async processing
        - Log analysis
        - Management tools
        - Performance optimization
        - Integration capabilities" << std::endl;
        
        std::cout << std::endl;
        std::cout << "The enhanced system extends the original Logger class while maintaining full backward compatibility." << std::endl;
    }
};

} // namespace nlm
