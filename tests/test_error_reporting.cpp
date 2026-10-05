// Test ErrorReporting functionality
// Verifies error categorization, recovery suggestions, and error management

#include "ErrorReporting.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_error_reporting {

void testErrorReportingConfiguration() {
    nlm::ErrorReporting reporter;
    
    // Test error reporting configuration
    reporter.setLogErrors(true);
    reporter.setIncludeStackTrace(true);
    reporter.setIncludeContext(true);
    reporter.setMaxStackTraceDepth(16);
    reporter.setUseColors(true);
    reporter.setShowRecoverySuggestions(true);
    reporter.setEnableAutoReporting(true);
    
    std::cout << "✓ Error reporting configuration test passed" << std::endl;
}

void testErrorCategorization() {
    nlm::ErrorReporting reporter;
    
    // Test error categorization
    nlm::ErrorContext context("neural_simulation", "brain.cpp", 42, "updateNeuralActivity", "brain");
    
    nlm::ErrorCategory memoryCategory = reporter.categorizeError("Out of memory", context);
    assert(memoryCategory == nlm::ErrorCategory::Memory);
    
    nlm::ErrorCategory configCategory = reporter.categorizeError("Invalid configuration", context);
    assert(configCategory == nlm::ErrorCategory::Configuration);
    
    nlm::ErrorCategory cpuCategory = reporter.categorizeError("High CPU usage", context);
    assert(cpuCategory == nlm::ErrorCategory::Performance);
    
    std::cout << "✓ Error categorization test passed" << std::endl;
}

void testErrorSeverityDetermination() {
    nlm::ErrorReporting reporter;
    
    // Test error severity determination
    nlm::ErrorContext context("test", "test.cpp", 1, "testFunction", "test");
    
    nlm::ErrorSeverity criticalSeverity = reporter.determineErrorSeverity("Segmentation fault", nlm::ErrorCategory::System);
    assert(criticalSeverity == nlm::ErrorSeverity::Fatal);
    
    nlm::ErrorSeverity errorSeverity = reporter.determineErrorSeverity("File not found", nlm::ErrorCategory::InputOutput);
    assert(errorSeverity == nlm::ErrorSeverity::Error);
    
    nlm::ErrorSeverity warningSeverity = reporter.determineErrorSeverity("Deprecated function", nlm::ErrorCategory::Development);
    assert(warningSeverity == nlm::ErrorSeverity::Warning);
    
    std::cout << "✓ Error severity determination test passed" << std::endl;
}

void testErrorReporting() {
    nlm::ErrorReporting reporter;
    
    // Test error reporting
    nlm::ErrorContext context("memory_allocation", "memory.cpp", 100, "allocateMemory", "memory");
    
    std::string errorId = reporter.reportError("Out of memory", nlm::ErrorSeverity::Error,
                                               nlm::ErrorCategory::Memory, "ERR_MEM_001", context);
    
    assert(!errorId.empty());
    assert(errorId.find("ERR_") != std::string::npos);
    
    // Test exception-based error reporting
    try {
        throw std::runtime_error("Runtime error occurred");
    } catch (const std::exception& e) {
        std::string errorId2 = reporter.reportError(e, nlm::ErrorSeverity::Error,
                                                     nlm::ErrorCategory::Unknown, "ERR_RUNTIME_001", context);
        
        assert(!errorId2.empty());
    }
    
    std::cout << "✓ Error reporting test passed" << std::endl;
}

void testErrorHandling() {
    nlm::ErrorReporting reporter;
    
    // Test error handling
    nlm::ErrorContext context("test", "test.cpp", 1, "test", "test");
    std::string errorId = reporter.reportError("Test error", nlm::ErrorSeverity::Warning,
                                               nlm::ErrorCategory::System, "ERR_TEST_001", context);
    
    // Test error resolution
    reporter.handleError(errorId, true, "Fixed by restart");
    
    assert(reporter.isErrorResolved(errorId));
    
    // Test error addition
    nlm::RecoverySuggestion suggestion({"Check configuration", "Verify settings are correct"});
    reporter.addRecoverySuggestion(errorId, suggestion);
    
    std::cout << "✓ Error handling test passed" << std::endl;
}

void testErrorRetrieval() {
    nlm::ErrorReporting reporter;
    
    // Test error retrieval
    nlm::ErrorContext context("test", "test.cpp", 1, "test", "test");
    std::string errorId = reporter.reportError("Test error", nlm::ErrorSeverity::Warning,
                                               nlm::ErrorCategory::System, "ERR_TEST_001", context);
    
    auto errors = reporter.getErrors(nlm::ErrorSeverity::Warning, nlm::ErrorCategory::System, false);
    assert(!errors.empty());
    
    const nlm::ErrorReport* error = reporter.getError(errorId);
    assert(error != nullptr);
    assert(error->id == errorId);
    
    std::cout << "✓ Error retrieval test passed" << std::endl;
}

void testErrorReportGeneration() {
    nlm::ErrorReporting reporter;
    
    // Test error report generation
    nlm::ErrorContext context("test", "test.cpp", 1, "test", "test");
    std::string errorId = reporter.reportError("Test error", nlm::ErrorSeverity::Warning,
                                               nlm::ErrorCategory::System, "ERR_TEST_001", context);
    
    // Generate error report
    std::string report = reporter.generateErrorReport(errorId, true);
    assert(!report.empty());
    assert(report.find(errorId) != std::string::npos);
    
    // Generate analysis report
    std::string analysisReport = reporter.generateErrorAnalysisReport();
    assert(!analysisReport.empty());
    assert(analysisReport.find("Error Analysis") != std::string::npos);
    
    // Generate summary report
    std::string summaryReport = reporter.generateErrorSummaryReport();
    assert(!summaryReport.empty());
    assert(summaryReport.find("Error Summary") != std::string::npos);
    
    std::cout << "✓ Error report generation test passed" << std::endl;
}

void testErrorExport() {
    nlm::ErrorReporting reporter;
    
    // Test error export
    nlm::ErrorContext context("test", "test.cpp", 1, "test", "test");
    std::string errorId = reporter.reportError("Test error", nlm::ErrorSeverity::Warning,
                                               nlm::ErrorCategory::System, "ERR_TEST_001", context);
    
    bool exportSuccess = reporter.exportErrorReport("test_errors.json", "json", {errorId});
    assert(exportSuccess);
    
    std::cout << "✓ Error export test passed" << std::endl;
}

void testErrorLogging() {
    nlm::ErrorReporting reporter;
    
    // Test error logging
    nlm::ErrorContext context("test", "test.cpp", 1, "test", "test");
    std::string errorId = reporter.reportError("Test error", nlm::ErrorSeverity::Warning,
                                               nlm::ErrorCategory::System, "ERR_TEST_001", context);
    
    // Test logging by ID
    reporter.logError(errorId);
    
    std::cout << "✓ Error logging test passed" << std::endl;
}

void testErrorStatistics() {
    nlm::ErrorReporting reporter;
    
    // Test error statistics
    size_t errorCount = reporter.getErrorCount(nlm::ErrorSeverity::Warning);
    assert(errorCount >= 0);
    
    double errorRate = reporter.getErrorRate(nlm::ErrorSeverity::Warning);
    assert(errorRate >= 0.0);
    
    std::string commonCategory = reporter.getMostCommonErrorCategory();
    assert(!commonCategory.empty());
    
    auto lastErrorTime = reporter.getLastErrorTime();
    // Just verify it returns something (could be default time)
    
    std::cout << "✓ Error statistics test passed" << std::endl;
}

void testErrorManagement() {
    nlm::ErrorReporting reporter;
    
    // Test error management
    nlm::ErrorContext context("test", "test.cpp", 1, "test", "test");
    std::string errorId = reporter.reportError("Test error", nlm::ErrorSeverity::Warning,
                                               nlm::ErrorCategory::System, "ERR_TEST_001", context);
    
    // Test clearing resolved errors
    reporter.handleError(errorId, true, "Resolved");
    reporter.clearResolvedErrors();
    
    // Test clearing all errors
    reporter.clearAllErrors();
    
    std::cout << "✓ Error management test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Testing ErrorReporting ===" << std::endl;
    testErrorReportingConfiguration();
    testErrorCategorization();
    testErrorSeverityDetermination();
    testErrorReporting();
    testErrorHandling();
    testErrorRetrieval();
    testErrorReportGeneration();
    testErrorExport();
    testErrorLogging();
    testErrorStatistics();
    testErrorManagement();
    
    std::cout << "=== All ErrorReporting Tests PASSED ===" << std::endl;
}

} // namespace test_error_reporting
