// src/debugging/ErrorAnalyzer.cpp
#include "ErrorAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ctime>

namespace nlm {

struct ErrorAnalyzer::Impl {
    struct ErrorRecord {
        std::string type;
        std::string message;
        std::string context;
        std::string stackTrace;
        std::chrono::system_clock::time_point timestamp;
        size_t occurrenceCount;
        
        ErrorRecord(const std::string& t, const std::string& m, const std::string& c = "", const std::string& s = "")
            : type(t), message(m), context(c), stackTrace(s), occurrenceCount(1) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<ErrorRecord> errors;
    std::unordered_map<std::string, size_t> errorCounts;
    std::chrono::system_clock::time_point sessionStart;
    
    Impl() {
        sessionStart = std::chrono::system_clock::now();
    }
};

ErrorAnalyzer::ErrorAnalyzer() {
    pImpl = std::make_unique<Impl>();
    autoReport_ = true;
    errorThreshold_ = 10;
}

ErrorAnalyzer::~ErrorAnalyzer() = default;

void ErrorAnalyzer::recordError(const std::string& errorType, const std::string& errorMessage,
                               const std::string& context, const std::string& stackTrace) {
    // Check if similar error was recently recorded
    bool found = false;
    for (auto& error : pImpl->errors) {
        if (error.type == errorType && error.message == errorMessage && error.context == context) {
            error.occurrenceCount++;
            found = true;
            break;
        }
    }
    
    if (!found) {
        pImpl->errors.emplace_back(errorType, errorMessage, context, stackTrace);
    }
    
    pImpl->errorCounts[errorType]++;
    
    // Auto-report if enabled and threshold exceeded
    if (autoReport_ && shouldReportError(errorType)) {
        Logger::getGlobal().error("Error analyzed: " + errorType + " - " + errorMessage + 
                                " (Context: " + context + ")");
    }
}

size_t ErrorAnalyzer::getErrorCount(const std::string& errorType) const {
    auto it = pImpl->errorCounts.find(errorType);
    return (it != pImpl->errorCounts.end()) ? it->second : 0;
}

std::vector<std::string> ErrorAnalyzer::getErrorTypes() const {
    std::vector<std::string> types;
    types.reserve(pImpl->errorCounts.size());
    for (const auto& pair : pImpl->errorCounts) {
        types.push_back(pair.first);
    }
    return types;
}

std::vector<std::string> ErrorAnalyzer::analyzePatterns() const {
    std::vector<std::string> patterns;
    
    if (pImpl->errors.empty()) {
        return patterns;
    }
    
    // Analyze most frequent error type
    std::string mostFrequentType;
    size_t maxCount = 0;
    for (const auto& pair : pImpl->errorCounts) {
        if (pair.second > maxCount) {
            mostFrequentType = pair.first;
            maxCount = pair.second;
        }
    }
    
    if (!mostFrequentType.empty() && maxCount > 1) {
        patterns.push_back("Most frequent error type: " + mostFrequentType + " (occurred " + 
                          std::to_string(maxCount) + " times)");
    }
    
    // Analyze error timing patterns
    if (pImpl->errors.size() > 3) {
        auto timePoints = std::vector<std::chrono::system_clock::time_point>();
        for (const auto& error : pImpl->errors) {
            timePoints.push_back(error.timestamp);
        }
        std::sort(timePoints.begin(), timePoints.end());
        
        // Calculate average time between errors
        if (timePoints.size() > 1) {
            auto first = timePoints.front();
            auto last = timePoints.back();
            auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(last - first).count();
            size_t errorCount = pImpl->errors.size();
            
            if (totalDuration > 0) {
                double avgInterval = static_cast<double>(totalDuration) / errorCount;
                patterns.push_back("Average error interval: " + std::to_string(avgInterval) + " seconds");
            }
        }
    }
    
    // Check for context-specific errors
    std::unordered_map<std::string, size_t> contextCounts;
    for (const auto& error : pImpl->errors) {
        if (!error.context.empty()) {
            contextCounts[error.context]++;
        }
    }
    
    for (const auto& pair : contextCounts) {
        if (pair.second >= 2) {
            patterns.push_back("Context '" + pair.first + "' has " + std::to_string(pair.second) + 
                              " associated errors");
        }
    }
    
    return patterns;
}

std::vector<std::string> ErrorAnalyzer::getRecentErrors(size_t limit) const {
    std::vector<std::string> recentErrors;
    
    // Create copies and sort by timestamp (newest first)
    auto sortedErrors = pImpl->errors;
    std::sort(sortedErrors.begin(), sortedErrors.end(),
              [](const Impl::ErrorRecord& a, const Impl::ErrorRecord& b) {
                  return a.timestamp > b.timestamp;
              });
    
    for (size_t i = 0; i < std::min(limit, sortedErrors.size()); ++i) {
        const auto& error = sortedErrors[i];
        std::time_t time = std::chrono::system_clock::to_time_t(error.timestamp);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        
        recentErrors.push_back(std::string(timeBuf) + " - " + error.type + ": " + error.message +
                              " (Count: " + std::to_string(error.occurrenceCount) + ")");
    }
    
    return recentErrors;
}

void ErrorAnalyzer::clear() {
    pImpl->errors.clear();
    pImpl->errorCounts.clear();
}

std::string ErrorAnalyzer::getReport() const {
    std::stringstream ss;
    ss << "Error Analyzer Report\n";
    ss << "======================\n";
    ss << "Total errors recorded: " << pImpl->errors.size() << "\n";
    ss << "Session duration: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - pImpl->sessionStart).count() << " seconds\n\n";
    
    if (!pImpl->errorCounts.empty()) {
        ss << "Error counts by type:\n";
        for (const auto& pair : pImpl->errorCounts) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
        ss << "\n";
    }
    
    auto patterns = analyzePatterns();
    if (!patterns.empty()) {
        ss << "Pattern analysis:\n";
        for (const auto& pattern : patterns) {
            ss << "  - " << pattern << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool ErrorAnalyzer::shouldReportError(const std::string& errorType) const {
    return autoReport_ && pImpl->errorCounts[errorType] >= errorThreshold_;
}

bool ErrorAnalyzer::exportErrors(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Error Analysis Export\n";
    file << "======================\n\n";
    
    for (const auto& error : pImpl->errors) {
        std::time_t time = std::chrono::system_clock::to_time_t(error.timestamp);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        
        file << "Time: " << timeBuf << "\n";
        file << "Type: " << error.type << "\n";
        file << "Message: " << error.message << "\n";
        file << "Context: " << error.context << "\n";
        file << "Stack Trace: " << error.stackTrace << "\n";
        file << "Occurrence Count: " << error.occurrenceCount << "\n\n";
    }
    
    return true;
}

bool ErrorAnalyzer::importErrors(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentType, currentMessage, currentContext, currentStackTrace;
    size_t currentCount = 0;
    bool inRecord = false;
    
    while (std::getline(file, line)) {
        if (line.find("Type: ") == 0) {
            currentType = line.substr(6);
        } else if (line.find("Message: ") == 0) {
            currentMessage = line.substr(8);
        } else if (line.find("Context: ") == 0) {
            currentContext = line.substr(8);
        } else if (line.find("Stack Trace: ") == 0) {
            currentStackTrace = line.substr(12);
        } else if (line.find("Occurrence Count: ") == 0) {
            currentCount = std::stoul(line.substr(18));
            if (!currentType.empty()) {
                recordError(currentType, currentMessage, currentContext, currentStackTrace);
                // Adjust count if imported record has more occurrences
                if (currentCount > 1) {
                    for (auto& error : pImpl->errors) {
                        if (error.type == currentType && error.message == currentMessage) {
                            error.occurrenceCount = currentCount;
                            break;
                        }
                    }
                }
            }
            inRecord = false;
        }
    }
    
    return true;
}

} // namespace nlm