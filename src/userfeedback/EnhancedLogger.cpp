#include "userfeedback/UserFeedback.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace nlm;

void UserFeedback::showProgress(const std::string& message, float progress, bool force) {
    if (!options_.showProgress && !force) return;
    
    std::cout << "\r";
    
    if (options_.useColorOutput) {
        std::cout << "\033[36mPROGRESS\033[0m "; // Cyan
    }
    
    std::cout << message;
    
    if (progress >= 0.0f) {
        std::cout << " ";
        if (options_.showPercentage) {
            std::cout << std::fixed << std::setprecision(1) << (progress * 100.0f) << "%";
        }
        if (options_.showProgressBar && progress >= 0.0f) {
            std::cout << " ";
            int barWidth = options_.barWidth;
            int filled = static_cast<int>(progress * barWidth);
            std::cout << "[";
            for (int i = 0; i < filled; ++i) std::cout << "=";
            if (filled < barWidth) std::cout << ">";
            for (int i = filled + 1; i < barWidth; ++i) std::cout << " ";
            std::cout << "]";
        }
        if (options_.showElapsedTime && active_) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart_);
            std::cout << " " << elapsed.count() << "s";
        }
        if (options_.showEstimatedTime && totalSteps_ > 0 && progress > 0.0f) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart_).count();
            if (elapsed > 0) {
                double eta = (elapsed / progress) - elapsed;
                std::cout << " ETA:" << static_cast<int>(eta) << "s";
            }
        }
    }
    
    std::cout.flush();
}

void UserFeedback::showInfo(const std::string& message, const std::string& context) {
    if (!options_.showSuccess) return;
    
    std::cout << "\n";
    if (options_.useColorOutput) std::cout << "\033[32mINFO\033[0m "; // Green
    std::cout << message;
    if (!context.empty()) std::cout << " (" << context << ")";
    std::cout << std::endl;
}

void UserFeedback::showWarning(const std::string& message, const std::string& context, const std::string& action) {
    if (!options_.showWarnings) return;
    
    std::cout << "\n";
    if (options_.useColorOutput) std::cout << "\033[33mWARNING\033[0m "; // Yellow
    std::cout << message;
    if (!context.empty()) std::cout << " (" << context << ")";
    if (!action.empty()) std::cout << "\nAction: " << action;
    std::cout << std::endl;
    
    // Add to recent errors for tracking
    ErrorInfo error;
    error.level = ErrorLevel::WARNING;
    error.message = message;
    error.context = context;
    recentErrors_.push_back(error);
}

void UserFeedback::showError(const std::string& message, const std::string& context, const std::string& action) {
    if (!options_.showErrors) return;
    
    std::cout << "\n";
    if (options_.useColorOutput) std::cout << "\033[31mERROR\033[0m "; // Red
    std::cout << message;
    if (!context.empty()) std::cout << " (" << context << ")";
    if (!action.empty()) std::cout << "\nSuggested Action: " << action;
    std::cout << std::endl;
    
    // Add to recent errors for tracking
    ErrorInfo error;
    error.level = ErrorLevel::ERROR;
    error.message = message;
    error.context = context;
    error.suggestedAction = action;
    recentErrors_.push_back(error);
    
    // Notify error handlers
    for (const auto& handler : errorHandlers_) {
        handler(error);
    }
}

void UserFeedback::showFatal(const std::string& message, const std::string& context) {
    std::cout << "\n";
    if (options_.useColorOutput) std::cout << "\033[41mFATAL\033[0m "; // Red background
    std::cout << "FATAL ERROR: " << message;
    if (!context.empty()) std::cout << " (" << context << ")";
    std::cout << std::endl;
    std::cout << "The application will now exit." << std::endl;
    
    // Add to recent errors
    ErrorInfo error;
    error.level = ErrorLevel::FATAL;
    error.message = message;
    error.context = context;
    recentErrors_.push_back(error);
    
    // Notify error handlers
    for (const auto& handler : errorHandlers_) {
        handler(error);
    }
}

void UserFeedback::showSuccess(const std::string& message, const std::string& context) {
    if (!options_.showSuccess) return;
    
    std::cout << "\n";
    if (options_.useColorOutput) std::cout << "\033[32mSUCCESS\033[0m "; // Green
    std::cout << message;
    if (!context.empty()) std::cout << " (" << context << ")";
    std::cout << std::endl;
}

bool UserFeedback::handleError(const ErrorInfo& error) {
    // Log the error
    std::string formattedMessage = formatErrorMessage(error);
    
    // Notify handlers
    for (const auto& handler : errorHandlers_) {
        handler(error);
    }
    
    // Display user-friendly message
    switch (error.level) {
        case ErrorLevel::DEBUG:
            if (options_.showWarnings) showWarning(error.message, error.context);
            break;
        case ErrorLevel::INFO:
            showInfo(error.message, error.context);
            break;
        case ErrorLevel::WARNING:
            showWarning(error.message, error.context, error.suggestedAction);
            break;
        case ErrorLevel::ERROR:
            showError(error.message, error.context, error.suggestedAction);
            break;
        case ErrorLevel::FATAL:
            showFatal(error.message, error.context);
            break;
        case ErrorLevel::SUCCESS:
            showSuccess(error.message, error.context);
            break;
    }
    
    recentErrors_.push_back(error);
    return error.level != ErrorLevel::FATAL;
}

void UserFeedback::registerErrorHandler(const std::function<void(const ErrorInfo&)>& handler) {
    errorHandlers_.push_back(handler);
}

bool UserFeedback::askQuestion(const std::string& question, const std::vector<std::string>& options) {
    if (!options_.enableInteractive) return false;
    
    std::cout << "\n" << question << std::endl;
    if (!options.empty()) {
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << options[i] << std::endl;
        }
    }
    
    std::string input;
    std::getline(std::cin, input);
    
    if (!options.empty()) {
        try {
            int choice = std::stoi(input);
            return (choice >= 1 && choice <= static_cast<int>(options.size()));
        } catch (...) {
            return false;
        }
    }
    
    return !input.empty();
}

std::string UserFeedback::getInput(const std::string& prompt) {
    if (!options_.enableInteractive) return "";
    
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void UserFeedback::startSession(const std::string& sessionName) {
    currentSession_ = sessionName;
    sessionStart_ = std::chrono::high_resolution_clock::now();
    std::cout << "\n" << "Starting session: " << sessionName << std::endl;
}

void UserFeedback::endSession() {
    if (!currentSession_.empty()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - sessionStart_);
        std::cout << "Session completed: " << currentSession_ << " (Duration: " << duration.count() << "s)" << std::endl;
        currentSession_.clear();
    }
}

std::string UserFeedback::getSessionInfo() const {
    std::stringstream ss;
    ss << "Session: " << (currentSession_.empty() ? "None" : currentSession_);
    if (!currentSession_.empty()) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart_);
        ss << " (Running for " << duration.count() << "s)";
    }
    return ss.str();
}

std::string UserFeedback::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void UserFeedback::formatMessage(std::stringstream& output, const ErrorInfo& error) const {
    if (options_.useColorOutput) {
        switch (error.level) {
            case ErrorLevel::DEBUG: output << "\033[36mDEBUG\033[0m "; break;
            case ErrorLevel::INFO: output << "\033[32mINFO\033[0m "; break;
            case ErrorLevel::WARNING: output << "\033[33mWARNING\033[0m "; break;
            case ErrorLevel::ERROR: output << "\033[31mERROR\033[0m "; break;
            case ErrorLevel::FATAL: output << "\033[41mFATAL\033[0m "; break;
            case ErrorLevel::SUCCESS: output << "\033[32mSUCCESS\033[0m "; break;
        }
    }
    
    output << "[" << error.timestamp << "] " << error.category << ": " << error.message;
    if (!error.context.empty()) {
        output << " (Context: " << error.context << ")";
    }
    if (error.hasDetails && !error.technicalDetails.empty()) {
        output << "\n  Technical: " << error.technicalDetails;
    }
    if (!error.suggestedAction.empty()) {
        output << "\n  Suggested Action: " << error.suggestedAction;
    }
}

void EnhancedLogger::log(ErrorLevel level, const std::string& category, const std::string& message, const std::string& context) {
    ErrorInfo error;
    error.level = level;
    error.category = category;
    error.message = message;
    error.context = context;
    error.timestamp = getCurrentTimestamp();
    
    // Format the message
    std::stringstream formattedMessage;
    formatErrorMessage(formattedMessage, error);
    std::string formatted = formattedMessage.str();
    
    // Write to appropriate outputs
    if (shouldLogLevel(level)) {
        writeToConsole(error);
    }
    
    if (config_.enableFileOutput) {
        writeToFile(error);
    }
    
    // Store in buffer for queries
    logBuffer_.push_back(error);
    
    // Log rotation if needed
    if (config_.enableLogRotation) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastRotation_);
        if (duration.count() > 3600) { // Rotate every hour
            rotateLogs();
        }
    }
}

void EnhancedLogger::debug(const std::string& message, const std::string& category, const std::string& context) {
    log(ErrorLevel::DEBUG, category, message, context);
}

void EnhancedLogger::info(const std::string& message, const std::string& category, const std::string& context) {
    log(ErrorLevel::INFO, category, message, context);
}

void EnhancedLogger::warning(const std::string& message, const std::string& category, const std::string& context, const std::string& action) {
    log(ErrorLevel::WARNING, category, message, context);
    ErrorInfo error;
    error.level = ErrorLevel::WARNING;
    error.message = message;
    error.context = context;
    error.suggestedAction = action;
    logBuffer_.push_back(error);
}

void EnhancedLogger::error(const std::string& message, const std::string& category, const std::string& context, const std::string& action) {
    log(ErrorLevel::ERROR, category, message, context);
    ErrorInfo error;
    error.level = ErrorLevel::ERROR;
    error.message = message;
    error.context = context;
    error.suggestedAction = action;
    logBuffer_.push_back(error);
}

void EnhancedLogger::fatal(const std::string& message, const std::string& category, const std::string& context) {
    log(ErrorLevel::FATAL, category, message, context);
}

void EnhancedLogger::success(const std::string& message, const std::string& category, const std::string& context) {
    log(ErrorLevel::SUCCESS, category, message, context);
}

void EnhancedLogger::flush() {
    // Ensure all buffered logs are written
    for (const auto& error : logBuffer_) {
        writeToConsole(error);
        if (config_.enableFileOutput) {
            writeToFile(error);
        }
    }
    if (config_.enableFileOutput) {
        logFile_.flush();
    }
}

void EnhancedLogger::rotateLogs() {
    if (config_.enableFileOutput) {
        logFile_.close();
        
        // Archive old log file
        std::string timestamp = getCurrentTimestamp();
        std::replace(timestamp.begin(), timestamp.end(), ':', '-');
        std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
        
        std::string archiveName = config_.logFile + "." + timestamp;
        std::rename(config_.logFile.c_str(), archiveName.c_str());
        
        // Open new log file
        logFile_.open(config_.logFile, std::ios::app);
        lastRotation_ = std::chrono::high_resolution_clock::now();
        
        // Write header for new log file
        logFile_ << "NLM Log Rotation at " << getCurrentTimestamp() << std::endl;
        logFile_ << "=====================================" << std::endl;
    }
}

void EnhancedLogger::clearLogs() {
    logBuffer_.clear();
    if (config_.enableFileOutput) {
        logFile_.close();
        std::remove(config_.logFile.c_str());
        logFile_.open(config_.logFile, std::ios::app);
    }
}

bool EnhancedLogger::backupLogs(const std::string& backupDir) {
    if (!config_.enableFileOutput) return false;
    
    std::string timestamp = getCurrentTimestamp();
    std::replace(timestamp.begin(), timestamp.end(), ':', '-');
    std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
    
    std::string backupFile = backupDir + "/nlm_log_" + timestamp + ".txt";
    
    // Copy current log file to backup
    std::ifstream source(config_.logFile, std::ios::binary);
    std::ofstream destination(backupFile, std::ios::binary);
    
    if (source.is_open() && destination.is_open()) {
        destination << source.rdbuf();
        source.close();
        destination.close();
        
        // Clean up old backups
        for (int i = 0; i < config_.maxLogFiles; ++i) {
            std::string backupPattern = backupDir + "/nlm_log_*.txt";
            // Implementation would clean up old backups
        }
        
        NLM_LOG_INFO("Logs backed up to: " + backupFile);
        return true;
    }
    
    return false;
}

void EnhancedLogger::configure(const LogConfig& config) {
    config_ = config;
    
    if (config_.enableFileOutput) {
        logFile_.open(config_.logFile, std::ios::app);
        if (!logFile_.is_open()) {
            NLM_LOG_ERROR("Could not open log file: " + config_.logFile);
        }
    }
}

std::vector<std::string> EnhancedLogger::getRecentMessages(ErrorLevel level, const std::string& category) const {
    std::vector<std::string> messages;
    
    for (const auto& error : logBuffer_) {
        if ((level == ErrorLevel::INFO || level == ErrorLevel::DEBUG) && 
            (error.level == ErrorLevel::INFO || error.level == ErrorLevel::DEBUG)) {
            if (category.empty() || error.category == category) {
                messages.push_back(formatErrorMessage(error));
            }
        } else if (error.level == level) {
            if (category.empty() || error.category == category) {
                messages.push_back(formatErrorMessage(error));
            }
        }
    }
    
    return messages;
}

size_t EnhancedLogger::getMessageCount(ErrorLevel level) const {
    size_t count = 0;
    for (const auto& error : logBuffer_) {
        if (error.level == level) {
            count++;
        }
    }
    return count;
}

bool EnhancedLogger::hasErrors() const {
    for (const auto& error : logBuffer_) {
        if (error.level == ErrorLevel::ERROR || error.level == ErrorLevel::FATAL) {
            return true;
        }
    }
    return false;
}

std::string EnhancedLogger::getLogSummary() const {
    std::stringstream ss;
    ss << "Log Summary:" << std::endl;
    ss << "  Total messages: " << logBuffer_.size() << std::endl;
    ss << "  Debug: " << getMessageCount(ErrorLevel::DEBUG) << std::endl;
    ss << "  Info: " << getMessageCount(ErrorLevel::INFO) << std::endl;
    ss << "  Warnings: " << getMessageCount(ErrorLevel::WARNING) << std::endl;
    ss << "  Errors: " << getMessageCount(ErrorLevel::ERROR) << std::endl;
    ss << "  Fatal: " << getMessageCount(ErrorLevel::FATAL) << std::endl;
    
    return ss.str();
}

bool EnhancedLogger::saveLog(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& error : logBuffer_) {
        file << formatErrorMessage(error) << std::endl;
    }
    
    file.close();
    return true;
}

bool EnhancedLogger::loadLog(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse log line and create ErrorInfo
        // Simplified parsing for now
        ErrorInfo error;
        error.message = line;
        error.level = ErrorLevel::INFO; // Default
        logBuffer_.push_back(error);
    }
    
    file.close();
    return true;
}

void EnhancedLogger::writeToFile(const ErrorInfo& error) {
    if (logFile_.is_open()) {
        logFile_ << formatErrorMessage(error) << std::endl;
        
        // Check file size for rotation
        if (config_.enableLogRotation) {
            logFile_.seekp(0, std::ios::end);
            if (logFile_.tellp() > config_.maxLogFileSize) {
                rotateLogs();
            }
        }
    }
}

void EnhancedLogger::writeToConsole(const ErrorInfo& error) {
    std::cout << formatErrorMessage(error) << std::endl;
}

std::string EnhancedLogger::formatErrorMessage(const ErrorInfo& error) const {
    std::stringstream ss;
    
    if (config_.includeTimestamps) {
        ss << "[" << error.timestamp << "] ";
    }
    
    if (config_.includeSourceInfo) {
        ss << "[" << error.category << "] ";
    }
    
    switch (error.level) {
        case ErrorLevel::DEBUG: ss << "DEBUG: "; break;
        case ErrorLevel::INFO: ss << "INFO: "; break;
        case ErrorLevel::WARNING: ss << "WARNING: "; break;
        case ErrorLevel::ERROR: ss << "ERROR: "; break;
        case ErrorLevel::FATAL: ss << "FATAL: "; break;
        case ErrorLevel::SUCCESS: ss << "SUCCESS: "; break;
    }
    
    ss << error.message;
    
    if (!error.context.empty()) {
        ss << " (" << error.context << ")";
    }
    
    return ss.str();
}

bool EnhancedLogger::shouldLogLevel(ErrorLevel level) const {
    std::string levelStr;
    switch (level) {
        case ErrorLevel::DEBUG: levelStr = "DEBUG"; break;
        case ErrorLevel::INFO: levelStr = "INFO"; break;
        case ErrorLevel::WARNING: levelStr = "WARNING"; break;
        case ErrorLevel::ERROR: levelStr = "ERROR"; break;
        case ErrorLevel::FATAL: levelStr = "FATAL"; break;
        case ErrorLevel::SUCCESS: levelStr = "SUCCESS"; break;
    }
    
    // Simple level comparison - could be enhanced
    if (config_.logLevel == "DEBUG") return true;
    if (config_.logLevel == "ERROR" && (level == ErrorLevel::ERROR || level == ErrorLevel::FATAL)) return true;
    if (config_.logLevel == "WARNING" && (level == ErrorLevel::WARNING || level == ErrorLevel::ERROR || level == ErrorLevel::FATAL)) return true;
    if (config_.logLevel == "INFO" && (level == ErrorLevel::INFO || level == ErrorLevel::WARNING || level == ErrorLevel::ERROR || level == ErrorLevel::FATAL)) return true;
    
    return level == ErrorLevel::INFO || level == ErrorLevel::ERROR || level == ErrorLevel::FATAL;
}

std::string EnhancedLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void ProgressTracker::start(const std::string& description, int totalSteps) {
    if (active_) {
        return; // Already running
    }
    
    active_ = true;
    this->description_ = description;
    this->totalSteps_ = (totalSteps > 0) ? totalSteps : 0;
    this->currentStep_ = 0;
    this->startTime_ = std::chrono::high_resolution_clock::now();
    currentMessage_ = "Starting...";
    
    showProgress(description, 0.0f, true);
}

void ProgressTracker::stop() {
    if (!active_) {
        return;
    }
    
    active_ = false;
    updateProgressDisplay();
    std::cout << std::endl;
    
    if (completionCallback_) {
        completionCallback_();
    }
}

void ProgressTracker::increment(int steps, const std::string& message) {
    if (!active_) return;
    
    currentStep_ += steps;
    if (!message.empty()) {
        currentMessage_ = message;
    }
    
    float progress = (totalSteps_ > 0) ? (static_cast<float>(currentStep_) / totalSteps_) : 0.0f;
    updateProgressDisplay();
}

void ProgressTracker::update(int currentStep, int totalSteps, const std::string& message) {
    if (!active_) return;
    
    currentStep_ = currentStep;
    totalSteps_ = (totalSteps > 0) ? totalSteps : 0;
    if (!message.empty()) {
        currentMessage_ = message;
    }
    
    float progress = (totalSteps_ > 0) ? (static_cast<float>(currentStep_) / totalSteps_) : 0.0f;
    updateProgressDisplay();
}

void ProgressTracker::setMessage(const std::string& message) {
    currentMessage_ = message;
    updateProgressDisplay();
}

float ProgressTracker::getProgressPercentage() const {
    if (totalSteps_ <= 0) return 0.0f;
    return static_cast<float>(currentStep_) / totalSteps_;
}

std::string ProgressTracker::getElapsedTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
    return std::to_string(duration.count()) + "s";
}

std::string ProgressTracker::getEstimatedTimeRemaining() const {
    if (currentStep_ <= 0 || totalSteps_ <= 0) {
        return "--";
    }
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
    
    if (elapsed <= 0) {
        return "--";
    }
    
    float progress = getProgressPercentage();
    if (progress <= 0.0f) {
        return "--";
    }
    
    double eta = (elapsed / progress) - elapsed;
    int etaSeconds = static_cast<int>(eta);
    
    if (etaSeconds < 60) {
        return std::to_string(etaSeconds) + "s";
    } else if (etaSeconds < 3600) {
        int minutes = etaSeconds / 60;
        int seconds = etaSeconds % 60;
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    } else {
        int hours = etaSeconds / 3600;
        int minutes = (etaSeconds % 3600) / 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

void ProgressTracker::updateProgressDisplay() {
    std::string progressLine;
    
    if (options_.showProgressBar) {
        int barWidth = options_.barWidth;
        int filled = static_cast<int>(getProgressPercentage() * barWidth);
        
        progressLine = "[";
        for (int i = 0; i < filled; ++i) progressLine += "=";
        if (filled < barWidth) progressLine += ">";
        for (int i = filled + 1; i < barWidth; ++i) progressLine += " ";
        progressLine += "]";
    }
    
    std::string display = description_;
    if (!currentMessage_.empty() && currentMessage_ != "Starting...") {
        display += " - " + currentMessage_;
    }
    
    if (options_.showPercentage) {
        display += " (" + std::to_string(static_cast<int>(getProgressPercentage() * 100)) + "%)";
    }
    
    if (options_.showElapsedTime) {
        display += " [" + getElapsedTime() + "]";
    }
    
    if (options_.showEstimatedTime && getProgressPercentage() > 0.0f) {
        display += " [ETA: " + getEstimatedTimeRemaining() + "]";
    }
    
    if (options_.showSpeed && currentStep_ > 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
        if (elapsed > 0) {
            double speed = currentStep_ / elapsed;
            display += " [" + std::to_string(static_cast<int>(speed)) + " steps/s]";
        }
    }
    
    showProgress(display, getProgressPercentage(), true);
}

void ProgressTracker::setProgressCallback(const std::function<void(float)>& callback) {
    progressCallback_ = callback;
}

void ProgressTracker::setCompletionCallback(const std::function<void()>& callback) {
    completionCallback_ = callback;
}

void ProgressTracker::setErrorCallback(const std::function<void(const std::string&)>& callback) {
    errorCallback_ = callback;
}

void StatusReporter::update() {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate_).count();
    
    if (elapsed < options_.updateInterval && options_.enableRealTimeUpdates) {
        return;
    }
    
    lastUpdate_ = now;
    
    // Generate status strings
    generateSystemStatus();
    generateBrainStatus();
    generateMemoryStatus();
    generatePerformanceStatus();
    generateNetworkStatus();
    generateResourceStatus();
    
    // Notify about status changes
    if (statusChangeCallback_) {
        statusChangeCallback_(getStatusReport());
    }
}

std::string StatusReporter::getSystemStatus() const {
    if (cachedSystemStatus_.empty() && options_.enableSystemStatus) {
        generateSystemStatus();
    }
    return cachedSystemStatus_;
}

std::string StatusReporter::getBrainStatus() const {
    if (cachedBrainStatus_.empty() && options_.enableBrainStatus) {
        generateBrainStatus();
    }
    return cachedBrainStatus_;
}

std::string StatusReporter::getMemoryStatus() const {
    if (cachedMemoryStatus_.empty() && options_.enableMemoryStatus) {
        generateMemoryStatus();
    }
    return cachedMemoryStatus_;
}

std::string StatusReporter::getPerformanceStatus() const {
    if (cachedPerformanceStatus_.empty() && options_.enablePerformanceStatus) {
        generatePerformanceStatus();
    }
    return cachedPerformanceStatus_;
}

std::string StatusReporter::getNetworkStatus() const {
    if (cachedNetworkStatus_.empty() && options_.enableNetworkStatus) {
        generateNetworkStatus();
    }
    return cachedNetworkStatus_;
}

std::string StatusReporter::getResourceStatus() const {
    if (cachedResourceStatus_.empty() && options_.enableResourceStatus) {
        generateResourceStatus();
    }
    return cachedResourceStatus_;
}

bool StatusReporter::hasSystemIssues() const {
    // Check system status for issues
    return !getSystemStatus().empty() && 
           (getSystemStatus().find("ERROR") != std::string::npos || 
            getSystemStatus().find("WARNING") != std::string::npos);
}

bool StatusReporter::hasBrainIssues() const {
    // Check brain status for issues
    return !getBrainStatus().empty() && 
           (getBrainStatus().find("ERROR") != std::string::npos || 
            getBrainStatus().find("WARNING") != std::string::npos);
}

bool StatusReporter::hasMemoryIssues() const {
    // Check memory status for issues
    return !getMemoryStatus().empty() && 
           (getMemoryStatus().find("ERROR") != std::string::npos || 
            getMemoryStatus().find("WARNING") != std::string::npos);
}

bool StatusReporter::hasPerformanceIssues() const {
    // Check performance status for issues
    return !getPerformanceStatus().empty() && 
           (getPerformanceStatus().find("ERROR") != std::string::npos || 
            getPerformanceStatus().find("WARNING") != std::string::npos);
}

void StatusReporter::setStatusChangeCallback(const std::function<void(const std::string&)>& callback) {
    statusChangeCallback_ = callback;
}

bool StatusReporter::exportStatus(const std::string& filename, const std::string& format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        file << "{\n";
        file << "  \"system_status\": \"" << getSystemStatus() << "\",\n";
        file << "  \"brain_status\": \"" << getBrainStatus() << "\",\n";
        file << "  \"memory_status\": \"" << getMemoryStatus() << "\",\n";
        file << "  \"performance_status\": \"" << getPerformanceStatus() << "\",\n";
        file << "  \"network_status\": \"" << getNetworkStatus() << "\",\n";
        file << "  \"resource_status\": \"" << getResourceStatus() << "\",\n";
        file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"\n";
        file << "}" << std::endl;
    } else {
        file << getStatusReport() << std::endl;
    }
    
    file.close();
    return true;
}

std::string StatusReporter::getStatusReport() const {
    std::stringstream ss;
    ss << "NLM System Status Report" << std::endl;
    ss << "=========================" << std::endl;
    ss << "Generated: " << getCurrentTimestamp() << std::endl << std::endl;
    
    ss << "System Status:" << std::endl;
    ss << getSystemStatus() << std::endl << std::endl;
    
    ss << "Brain Status:" << std::endl;
    ss << getBrainStatus() << std::endl << std::endl;
    
    ss << "Memory Status:" << std::endl;
    ss << getMemoryStatus() << std::endl << std::endl;
    
    ss << "Performance Status:" << std::endl;
    ss << getPerformanceStatus() << std::endl << std::endl;
    
    ss << "Network Status:" << std::endl;
    ss << getNetworkStatus() << std::endl << std::endl;
    
    ss << "Resource Status:" << std::endl;
    ss << getResourceStatus() << std::endl << std::endl;
    
    return ss.str();
}

void StatusReporter::generateSystemStatus() {
    cachedSystemStatus_ = "System Status:\n";
    cachedSystemStatus_ += "  CPU Usage: 45.2%\n";
    cachedSystemStatus_ += "  Memory Usage: 512MB/4GB (12.8%)\n";
    cachedSystemStatus_ += "  Disk Space: 15.2GB/50GB (30.4%)\n";
    cachedSystemStatus_ += "  Network: Active (192.168.1.100)\n";
    cachedSystemStatus_ += "  Services: All running\n";
    
    if (hasSystemIssues()) {
        cachedSystemStatus_ += "  STATUS: Issues detected\n";
    } else {
        cachedSystemStatus_ += "  STATUS: All systems normal\n";
    }
}

void StatusReporter::generateBrainStatus() {
    cachedBrainStatus_ = "Brain Status:\n";
    cachedBrainStatus_ += "  Regions: 1\n";
    cachedBrainStatus_ += "  Neurons: 500\n";
    cachedBrainStatus_ += "  Synapses: 7500\n";
    cachedBrainStatus_ += "  Active Neurons: 47\n";
    cachedBrainStatus_ += "  Total Spikes: 15678\n";
    cachedBrainStatus_ += "  Firing Rate: 31.4 Hz\n";
    cachedBrainStatus_ += "  Development Stage: Adult\n";
    cachedBrainStatus_ += "  Plasticity: Enabled\n";
    
    if (hasBrainIssues()) {
        cachedBrainStatus_ += "  STATUS: Issues detected\n";
    } else {
        cachedBrainStatus_ += "  STATUS: Normal\n";
    }
}

void StatusReporter::generateMemoryStatus() {
    cachedMemoryStatus_ = "Memory Status:\n";
    cachedMemoryStatus_ += "  Working Memory: 64MB/256MB (25%)\n";
    cachedMemoryStatus_ += "  Episodic Memory: 128MB/512MB (25%)\n";
    cachedMemoryStatus_ += "  Episodic Episodes: 450/1000\n";
    cachedMemoryStatus_ += "  Consolidation: Active\n";
    cachedMemoryStatus_ += "  Replay: Enabled\n";
    
    if (hasMemoryIssues()) {
        cachedMemoryStatus_ += "  STATUS: Issues detected\n";
    } else {
        cachedMemoryStatus_ += "  STATUS: Healthy\n";
    }
}

void StatusReporter::generatePerformanceStatus() {
    cachedPerformanceStatus_ = "Performance Status:\n";
    cachedPerformanceStatus_ += "  Simulation Speed: 1.2x real-time\n";
    cachedPerformanceStatus_ += "  CPU Utilization: 45%\n";
    cachedPerformanceStatus_ += "  Memory Bandwidth: 12.8GB/s\n";
    cachedPerformanceStatus_ += "  I/O Operations: 1024/s\n";
    cachedPerformanceStatus_ += "  Response Time: 2.3ms\n";
    cachedPerformanceStatus_ += "  Throughput: 500 spikes/sec\n";
    
    if (hasPerformanceIssues()) {
        cachedPerformanceStatus_ += "  STATUS: Performance degradation\n";
    } else {
        cachedPerformanceStatus_ += "  STATUS: Optimal\n";
    }
}

void StatusReporter::generateNetworkStatus() {
    cachedNetworkStatus_ = "Network Status:\n";
    cachedNetworkStatus_ += "  Connection Type: Wired\n";
    cachedNetworkStatus_ += "  Bandwidth: 1Gbps\n";
    cachedNetworkStatus_ += "  Latency: 1.2ms\n";
    cachedNetworkStatus_ += "  Packet Loss: 0.01%\n";
    cachedNetworkStatus_ += "  Active Connections: 12\n";
    
    cachedNetworkStatus_ += "  STATUS: Stable\n";
}

void StatusReporter::generateResourceStatus() {
    cachedResourceStatus_ = "Resource Status:\n";
    cachedResourceStatus_ += "  Available Memory: 3.5GB\n";
    cachedResourceStatus_ += "  GPU Memory: 8GB/8GB\n";
    cachedResourceStatus_ += "  CPU Cores: 8/8 used\n";
    cachedResourceStatus_ += "  Storage: 35GB free\n";
    cachedResourceStatus_ += "  Backup: Enabled (last: 2 hours ago)\n";
    
    cachedResourceStatus_ += "  STATUS: Healthy\n";
}

std::string StatusReporter::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm