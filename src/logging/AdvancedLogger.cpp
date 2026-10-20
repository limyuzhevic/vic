#include "ProgressReporter.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>

using namespace std::literals;

namespace nlm {

struct ProgressReporter::Impl {
    std::shared_ptr<Logger> logger;
    
    std::map<std::string, ProgressReport> activeProgress;
    size_t displayInterval;
    bool enableColors;
    bool enableProgressBar;
    bool enableTimeEstimates;
    double updateFrequency;
    std::chrono::milliseconds lastDisplayTime;
    
    std::function<void(const ProgressReport&)> progressCallback;
    std::mutex progressMutex;
    
    std::vector<LogContext> operationContexts;
    mutable std::mutex contextsMutex;
    
    std::stringstream logBuffer;
    std::vector<std::string> logEntries;
    
    Impl(std::shared_ptr<Logger> logg)
        : logger(logg), displayInterval(100), enableColors(true), 
          enableProgressBar(true), enableTimeEstimates(true), updateFrequency(1.0),
          lastDisplayTime(0) {
        logBuffer.str("");
        logBuffer.clear();
    }
};

ProgressReporter::ProgressReporter(std::shared_ptr<Logger> logger) 
    : pImpl(std::make_unique<Impl>(logger)) {
}

ProgressReporter::~ProgressReporter() = default;

void ProgressReporter::startProgress(const std::string& testName, size_t totalSteps) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    
    ProgressReport report(testName, 0, totalSteps);
    pImpl->activeProgress[testName] = report;
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(report);
    }
    
    // Initial display
    if (pImpl->enableProgressBar) {
        displayProgress(report);
    }
}

void ProgressReporter::updateProgress(const std::string& testName, size_t currentStep,
                                     const std::string& status) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    
    updateProgressInternal(testName, currentStep, status);
}

void ProgressReporter::updateProgressInternal(const std::string& testName, size_t currentStep, const std::string& status) {
    auto it = pImpl->activeProgress.find(testName);
    if (it == pImpl->activeProgress.end()) return;
    
    ProgressReport& report = it->second;
    report.currentStep = currentStep;
    report.statusMessage = status;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - report.startTime);
    
    if (elapsed.count() > 0) {
        report.progressSpeed = (static_cast<double>(currentStep) / elapsed.count()) * 1000.0; // steps per second
        if (report.currentStep > 0 && report.totalSteps > 0) {
            double completionRatio = static_cast<double>(report.currentStep) / report.totalSteps;
            auto estimatedTotal = (elapsed.count() / completionRatio);
            report.estimatedTimeRemaining = std::chrono::milliseconds(
                static_cast<long>(estimatedTotal - elapsed.count()));
        }
    }
    
    // Display progress if needed
    if (pImpl->enableProgressBar && 
        (report.currentStep % pImpl->displayInterval == 0 || 
         report.currentStep == report.totalSteps)) {
        
        auto currentTime = std::chrono::steady_clock::now();
        auto timeSinceLastDisplay = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - pImpl->lastDisplayTime);
        
        if (timeSinceLastDisplay.count() >= 1000 / static_cast<int>(pImpl->updateFrequency)) {
            displayProgress(report);
            pImpl->lastDisplayTime = currentTime;
        }
    }
    
    if (pImpl->progressCallback) {
        pImpl->progressCallback(report);
    }
}

void ProgressReporter::completeProgress(const std::string& testName, bool success,
                                       const std::string& finalMessage) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    
    completeProgressInternal(testName, success, finalMessage);
}

void ProgressReporter::completeProgressInternal(const std::string& testName, bool success,
                                               const std::string& finalMessage) {
    auto it = pImpl->activeProgress.find(testName);
    if (it == pImpl->activeProgress.end()) return;
    
    ProgressReport& report = it->second;
    report.statusMessage = success ? "Success" : "Failed";
    report.currentStep = report.totalSteps;
    
    if (pImpl->enableProgressBar) {
        displayProgress(report);
        std::cout << std::endl; // New line after progress bar
    }
    
    // Log completion
    if (pImpl->logger) {
        std::string level = success ? "INFO" : "ERROR";
        std::string message = testName + " " + (success ? "completed successfully" : "failed") + ": " + finalMessage;
        pImpl->logger->info(message);
    }
    
    // Remove from active progress
    pImpl->activeProgress.erase(it);
}

void ProgressReporter::stopProgress(const std::string& testName) {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    
    pImpl->activeProgress.erase(testName);
}

void ProgressReporter::displayProgress(const ProgressReport& report) {
    if (!pImpl->enableProgressBar) return;
    
    std::stringstream ss;
    
    // Test name
    ss << "[" << report.testName << "] ";
    
    // Progress bar
    int barWidth = 40;
    int progressPos = static_cast<int>((static_cast<double>(report.currentStep) / report.totalSteps) * barWidth);
    
    ss << "["; 
    for (int i = 0; i < barWidth; ++i) {
        if (i < progressPos) {
            ss << "=";
        } else if (i == progressPos) {
            ss << ">";
        } else {
            ss << " ";
        }
    }
    ss << "] ";
    
    // Percentage
    ss << std::fixed << std::setprecision(1) << report.percentage << "% ";
    
    // Current/Total
    ss << report.currentStep << "/" << report.totalSteps << " ";
    
    // Status
    ss << "- " << report.statusMessage << " ";
    
    // Speed and ETA
    if (pImpl->enableTimeEstimates) {
        if (report.progressSpeed > 0) {
            ss << "[" << std::fixed << std::setprecision(1) << report.progressSpeed << " steps/s" << "; ";
            if (report.estimatedTimeRemaining.count() > 0) {
                auto eta = report.estimatedTimeRemaining;
                ss << "ETA: " << (eta.count() / 1000) << "s";
            } else {
                ss << "ETA: unknown";
            }
            ss << "]";
        }
    }
    
    std::cout << ss.str() << std::flush;
}

void ProgressReporter::setDisplayInterval(size_t interval) {
    pImpl->displayInterval = interval;
}

void ProgressReporter::setEnableColors(bool enable) {
    pImpl->enableColors = enable;
}

void ProgressReporter::setOutputStream(std::ostream& stream) {
    // In real implementation, would store reference to output stream
    // For now, just using std::cout
}

void ProgressReporter::setEnableProgressBar(bool enable) {
    pImpl->enableProgressBar = enable;
}

void ProgressReporter::setEnableTimeEstimates(bool enable) {
    pImpl->enableTimeEstimates = enable;
}

void ProgressReporter::setUpdateFrequency(double frequency) {
    pImpl->updateFrequency = frequency;
}

void ProgressReporter::setProgressCallback(std::function<void(const ProgressReport&)> callback) {
    pImpl->progressCallback = callback;
}

std::vector<ProgressReport> ProgressReporter::getActiveProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    std::vector<ProgressReport> reports;
    reports.reserve(pImpl->activeProgress.size());
    
    for (const auto& pair : pImpl->activeProgress) {
        reports.push_back(pair.second);
    }
    
    return reports;
}

bool ProgressReporter::isProgressActive(const std::string& testName) const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    return pImpl->activeProgress.find(testName) != pImpl->activeProgress.end();
}

std::chrono::milliseconds ProgressReporter::getTotalProgressUpdates() const {
    // Implementation would count total updates
    return std::chrono::milliseconds(0);
}

std::chrono::milliseconds ProgressReporter::getTotalElapsedTime() const {
    std::lock_guard<std::mutex> lock(pImpl->progressMutex);
    
    if (pImpl->activeProgress.empty()) {
        return std::chrono::milliseconds(0);
    }
    
    auto maxElapsed = std::chrono::milliseconds(0);
    auto now = std::chrono::steady_clock::now();
    
    for (const auto& pair : pImpl->activeProgress) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pair.second.startTime);
        if (elapsed > maxElapsed) {
            maxElapsed = elapsed;
        }
    }
    
    return maxElapsed;
}

AdvancedLogger::AdvancedLogger(std::shared_ptr<Logger> baseLogger) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = baseLogger;
}

AdvancedLogger::~AdvancedLogger() = default;

void AdvancedLogger::logOperation(const std::string& operation, const std::string& component,
                                 const std::string& testName, bool success,
                                 const std::string& errorMessage) {
    LogContext context(operation, component, testName);
    
    std::string level = success ? "INFO" : "ERROR";
    std::string message = operation + " " + component;
    if (!errorMessage.empty()) {
        message += " - Error: " + errorMessage;
    }
    
    logWithContext(context, message, level);
    
    // Store context for potential export
    {
        std::lock_guard<std::mutex> lock(pImpl->contextsMutex);
        pImpl->operationContexts.push_back(context);
    }
}

void AdvancedLogger::logWithContext(const LogContext& context, const std::string& message,
                                   const std::string& level) {
    // Format with timestamp
    std::stringstream ss;
    formatLogEntry(ss, level, message, context);
    
    std::string formattedMessage = ss.str();
    
    // Output to log
    if (pImpl->logger) {
        if (level == "ERROR" || level == "CRITICAL") {
            pImpl->logger->error(formattedMessage);
        } else if (level == "WARNING") {
            pImpl->logger->warning(formattedMessage);
        } else if (level == "DEBUG") {
            pImpl->logger->debug(formattedMessage);
        } else {
            pImpl->logger->info(formattedMessage);
        }
    }
    
    // Store in buffer
    pImpl->logBuffer << formattedMessage << std::endl;
    pImpl->logEntries.push_back(formattedMessage);
}

void AdvancedLogger::logProgress(const std::string& testName, size_t step, size_t total,
                                const std::string& message) {
    ProgressReport report(testName, step, total);
    report.statusMessage = message;
    
    // Output to logger
    if (pImpl->logger) {
        pImpl->logger->info("Progress " + report.testName + ": " + 
                          std::to_string(report.percentage) + "% - " + report.statusMessage);
    }
}

void AdvancedLogger::logPerformanceMetric(const std::string& metricName, double value,
                                         const std::string& unit, 
                                         const std::string& testName) {
    std::stringstream ss;
    ss << "Performance Metric: " << metricName << " = " << std::fixed << std::setprecision(4)
       << value << " " << unit;
    if (!testName.empty()) {
        ss << " [Test: " << testName << "]";
    }
    
    logWithContext(LogContext("metric", "performance", testName), ss.str(), "INFO");
}

std::string AdvancedLogger::generateLogJSON(const std::vector<LogContext>& contexts) const {
    std::stringstream json;
    json << "[";
    
    for (size_t i = 0; i < contexts.size(); ++i) {
        if (i > 0) json << ",";
        
        json << "{\"operation\": \"" << contexts[i].operation << "\", \"component\": \"" 
             << contexts[i].component << "\", \"test\": \"" << contexts[i].testName 
             << "\", \"metadata\": ";
        
        // Would serialize metadata here
        json << "{}" << "}";
    }
    
    json << "]";
    return json.str();
}

void AdvancedLogger::exportLogs(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        if (pImpl->logger) {
            pImpl->logger->error("Failed to export logs to: " + filepath);
        }
        return;
    }
    
    file << pImpl->logBuffer.str();
    file.close();
}

void AdvancedLogger::logErrorWithContext(const std::string& error, const std::string& operation,
                                       const std::string& component, const std::string& testName) {
    logOperation(operation, component, testName, false, error);
}

void AdvancedLogger::logBatch(const std::vector<std::string>& messages, 
                             const std::string& level) {
    for (const auto& message : messages) {
        logWithContext(LogContext("batch", "console", ""), message, level);
    }
}

void AdvancedLogger::formatLogEntry(std::stringstream& stream, const std::string& level,
                                   const std::string& message, const LogContext& context) const {
    // Timestamp
    stream << "[" << generateTimestamp() << "] ";
    
    // Level
    stream << "[" << level << "] ";
    
    // Context if available
    if (!context.testName.empty()) {
        stream << "[Test: " << context.testName << "] ";
    }
    if (!context.component.empty()) {
        stream << "[" << context.component << "] ";
    }
    
    // Message
    stream << message;
}

std::string AdvancedLogger::generateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace nlm
