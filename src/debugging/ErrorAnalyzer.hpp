// src/debugging/ErrorAnalyzer.hpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Error analyzer for systematic error analysis
class ErrorAnalyzer {
public:
    ErrorAnalyzer();
    ~ErrorAnalyzer();
    
    // Record an error occurrence
    void recordError(const std::string& errorType, const std::string& errorMessage,
                     const std::string& context = "", const std::string& stackTrace = "");
    
    // Get error count by type
    size_t getErrorCount(const std::string& errorType) const;
    
    // Get all error types
    std::vector<std::string> getErrorTypes() const;
    
    // Analyze error patterns and return recommendations
    std::vector<std::string> analyzePatterns() const;
    
    // Get recent errors (last N errors)
    std::vector<std::string> getRecentErrors(size_t limit = 10) const;
    
    // Clear all recorded errors
    void clear();
    
    // Get error analysis report
    std::string getReport() const;
    
    // Enable/disable automatic error reporting
    void setAutoReport(bool enable) { autoReport_ = enable; }
    
    // Set error threshold for reporting
    void setErrorThreshold(size_t threshold) { errorThreshold_ = threshold; }
    
    // Check if an error should be reported
    bool shouldReportError(const std::string& errorType) const;
    
    // Export errors to file
    bool exportErrors(const std::string& filepath) const;
    
    // Import errors from file
    bool importErrors(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool autoReport_;
    size_t errorThreshold_;
};

} // namespace nlm