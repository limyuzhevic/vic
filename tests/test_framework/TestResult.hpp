// Enhanced Test Result for NLM Brain Simulation
// Comprehensive test result management with detailed metadata

#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <vector>

namespace nlm {
namespace test {

// Enhanced Test result structure with comprehensive metadata
struct TestResult {
    std::string name;
    std::string testClass;
    std::string testSuite;
    std::vector<std::string> tags;
    std::string description;
    
    enum class Status {
        PASSED,
        FAILED,
        SKIPPED,
        TIMEOUT,
        ERROR,
        ABORTED
    };
    
    Status status = Status::PASSED;
    std::string failureMessage;
    std::string failureDetails;
    double executionTime = 0.0;
    uint64_t memoryUsage = 0;
    int lineNumber = 0;
    std::string fileName;
    std::string errorType;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    
    TestResult(const std::string& name, const std::string& testClass = "",
               const std::string& testSuite = "", const std::string& description = "") 
        : name(name), testClass(testClass), testSuite(testSuite),
          description(description), status(Status::PASSED), executionTime(0.0), 
          memoryUsage(0), lineNumber(0), startTime(), endTime() {}
    
    std::string getFullName() const {
        std::stringstream ss;
        if (!testSuite.empty()) {
            ss << testSuite << ".";
        }
        if (!testClass.empty()) {
            ss << testClass << ".";
        }
        ss << name;
        return ss.str();
    }
    
    std::string getStatusString() const {
        switch (status) {
            case Status::PASSED: return "PASSED";
            case Status::FAILED: return "FAILED";
            case Status::SKIPPED: return "SKIPPED";
            case Status::TIMEOUT: return "TIMEOUT";
            case Status::ERROR: return "ERROR";
            case Status::ABORTED: return "ABORTED";
            default: return "UNKNOWN";
        }
    }
    
    bool isSuccess() const {
        return status == Status::PASSED || status == Status::SKIPPED;
    }
    
    double getDuration() const {
        if (endTime.time_since_epoch().count() == 0) {
            return 0.0;
        }
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stopTimer() {
        endTime = std::chrono::high_resolution_clock::now();
        executionTime = getDuration();
    }
    
    void setMemoryUsage(uint64_t memory) {
        memoryUsage = memory;
    }
    
    void setLineNumber(int line) {
        lineNumber = line;
    }
    
    void setFileName(const std::string& file) {
        fileName = file;
    }
    
    void setErrorType(const std::string& error) {
        errorType = error;
    }
    
    // Detailed reporting
    std::string toString() const {
        std::stringstream ss;
        ss << "Test: " << getFullName() << "\n";
        ss << "Status: " << getStatusString() << "\n";
        ss << "Duration: " << std::fixed << std::setprecision(6) << getDuration() << "s\n";
        ss << "Memory: " << memoryUsage << " bytes\n";
        if (!failureMessage.empty()) {
            ss << "Failure: " << failureMessage << "\n";
        }
        if (!failureDetails.empty()) {
            ss << "Details: " << failureDetails << "\n";
        }
        if (!fileName.empty()) {
            ss << "File: " << fileName << "\n";
        }
        if (lineNumber > 0) {
            ss << "Line: " << lineNumber << "\n";
        }
        if (!errorType.empty()) {
            ss << "Error Type: " << errorType << "\n";
        }
        return ss.str();
    }
    
    // JSON serialization
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"name\": \"" << name << "\",\"testClass\": \"" << testClass << "\",\"testSuite\": \"" << testSuite << "\",\"status\": \"" << getStatusString() << "\",\"duration\": " << getDuration() << ",\"memory\": " << memoryUsage << ",\"line\": " << lineNumber;
        if (!fileName.empty()) {
            ss << ",\"file\": \"" << fileName << "\"";
        }
        if (!errorType.empty()) {
            ss << ",\"errorType\": \"" << errorType << "\"";
        }
        if (!failureMessage.empty()) {
            ss << ",\"failureMessage\": \"" << failureMessage << "\"";
        }
        ss << "}";
        return ss.str();
    }
    
    // XML serialization
    std::string toXML() const {
        std::stringstream ss;
        ss << "<test name=\"" << name << "\" testClass=\"" << testClass << "\" testSuite=\"" << testSuite << "\" status=\"" << getStatusString() << "\" duration=\"" << getDuration() << "\" memory=\"" << memoryUsage << "\">";
        if (!failureMessage.empty()) {
            ss << "<failure>" << failureMessage << "</failure>";
        }
        if (!fileName.empty()) {
            ss << "<file>" << fileName << "</file>";
        }
        if (lineNumber > 0) {
            ss << "<line>" << lineNumber << "</line>";
        }
        ss << "</test>";
        return ss.str();
    }
};

// Test statistics for reporting
struct TestStatistics {
    size_t totalTests = 0;
    size_t passedTests = 0;
    size_t failedTests = 0;
    size_t skippedTests = 0;
    size_t timeoutTests = 0;
    size_t errorTests = 0;
    double totalDuration = 0.0;
    double averageDuration = 0.0;
    double minDuration = 0.0;
    double maxDuration = 0.0;
    uint64_t totalMemory = 0;
    
    void addTest(const TestResult& result) {
        totalTests++;
        totalDuration += result.getDuration();
        totalMemory += result.memoryUsage;
        
        switch (result.status) {
            case TestResult::Status::PASSED: passedTests++; break;
            case TestResult::Status::FAILED: failedTests++; break;
            case TestResult::Status::SKIPPED: skippedTests++; break;
            case TestResult::Status::TIMEOUT: timeoutTests++; break;
            case TestResult::Status::ERROR: errorTests++; break;
            case TestResult::Status::ABORTED: errorTests++; break;
        }
        
        if (totalTests == 1) {
            minDuration = result.getDuration();
            maxDuration = result.getDuration();
        } else {
            minDuration = std::min(minDuration, result.getDuration());
            maxDuration = std::max(maxDuration, result.getDuration());
        }
    }
    
    void computeStatistics() {
        if (totalTests > 0) {
            averageDuration = totalDuration / totalTests;
        }
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "=== Test Statistics ===\n";
        ss << "Total Tests: " << totalTests << "\n";
        ss << "Passed: " << passedTests << "\n";
        ss << "Failed: " << failedTests << "\n";
        ss << "Skipped: " << skippedTests << "\n";
        ss << "Timeout: " << timeoutTests << "\n";
        ss << "Error: " << errorTests << "\n";
        ss << "Total Duration: " << totalDuration << "s\n";
        ss << "Average Duration: " << averageDuration << "s\n";
        ss << "Min Duration: " << minDuration << "s\n";
        ss << "Max Duration: " << maxDuration << "s\n";
        ss << "Total Memory: " << totalMemory << " bytes\n";
        ss << "=======================\n";
        return ss.str();
    }
    
    // Generate HTML report
    std::string toHTML() const {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n<html>\n<head>\n    <title>Test Results Report</title>\n    <style>\n        body { font-family: Arial, sans-serif; margin: 20px; }\n        .header { background-color: #f2f2f2; padding: 10px; border-radius: 5px; }\n        .passed { color: green; }\n        .failed { color: red; }\n        .skipped { color: orange; }\n        .timeout { color: darkorange; }\n        .error { color: darkred; }\n        table { border-collapse: collapse; width: 100%; margin-top: 20px; }\n        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n        th { background-color: #f2f2f2; }\n        tr:nth-child(even) { background-color: #f9f9f9; }\n    </style>\n</head>\n<body>\n    <div class=\"header\">\n        <h1>Test Results Report</h1>\n        <p>Generated on " << std::chrono::system_clock::now() << "</p>\n    </div>\n    <h2>Summary</h2>\n    <table>\n        <tr><th>Metric</th><th>Value</th></tr>\n        <tr><td>Total Tests</td><td>" << totalTests << "</td></tr>\n        <tr><td>Passed</td><td class=\"passed\">" << passedTests << "</td></tr>\n        <tr><td>Failed</td><td class=\"failed\">" << failedTests << "</td></tr>\n        <tr><td>Skipped</td><td class=\"skipped\">" << skippedTests << "</td></tr>\n        <tr><td>Timeout</td><td class=\"timeout\">" << timeoutTests << "</td></tr>\n        <tr><td>Error</td><td class=\"error\">" << errorTests << "</td></tr>\n        <tr><td>Total Duration</td><td>" << totalDuration << "s</td></tr>\n        <tr><td>Average Duration</td><td>" << averageDuration << "s</td></tr>\n        <tr><td>Min Duration</td><td>" << minDuration << "s</td></tr>\n        <tr><td>Max Duration</td><td>" << maxDuration << "s</td></tr>\n        <tr><td>Total Memory</td><td>" << totalMemory << " bytes</td></tr>\n    </table>\n</body>\n</html>";
        return ss.str();
    }
};

} // namespace test
} // namespace nlm
