#include "Logger.hpp"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

struct FileLogger::Impl {
    std::ofstream fileOut;
    std::string currentFile;
    std::mutex fileMutex;
    
    // Rotation configuration
    RotationConfig rotationConfig;
    uint64_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotation;
    
    // Buffering
    bool enableBuffering = true;
    size_t bufferSize = 8192;
    std::string buffer;
    bool flushOnEachLog = false;
    
    // Category filtering
    std::unordered_map<std::string, bool> categoryEnabled;
    LogLevel level = LogLevel::Info;
    std::shared_ptr<LogFormatter> formatter;
};

FileLogger::FileLogger()
    : pImpl(std::make_unique<Impl>()) {
    // Initialize category filters
    pImpl->categoryEnabled[LogCategory::Default] = true;
    pImpl->categoryEnabled[LogCategory::Core] = true;
    pImpl->categoryEnabled[LogCategory::Memory] = true;
    pImpl->categoryEnabled[LogCategory::Brain] = true;
    pImpl->categoryEnabled[LogCategory::Motor] = true;
    pImpl->categoryEnabled[LogCategory::Sensory] = true;
    pImpl->categoryEnabled[LogCategory::Cognition] = true;
    pImpl->categoryEnabled[LogCategory::Development] = true;
    pImpl->categoryEnabled[LogCategory::Plasticity] = true;
    pImpl->categoryEnabled[LogCategory::Neuromodulation] = true;
    pImpl->categoryEnabled[LogCategory::Prediction] = true;
    pImpl->categoryEnabled[LogCategory::ErrorHandling] = true;
    pImpl->categoryEnabled[LogCategory::Config] = true;
    pImpl->categoryEnabled[LogCategory::Plugin] = true;
    pImpl->categoryEnabled[LogCategory::Performance] = true;
    pImpl->categoryEnabled[LogCategory::MemoryAllocation] = true;
    pImpl->categoryEnabled[LogCategory::Threading] = true;
    pImpl->categoryEnabled[LogCategory::Backtrace] = true;
}

FileLogger::FileLogger(LogLevel level)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    
    // Initialize category filters
    pImpl->categoryEnabled[LogCategory::Default] = true;
    pImpl->categoryEnabled[LogCategory::Core] = true;
    pImpl->categoryEnabled[LogCategory::Memory] = true;
    pImpl->categoryEnabled[LogCategory::Brain] = true;
    pImpl->categoryEnabled[LogCategory::Motor] = true;
    pImpl->categoryEnabled[LogCategory::Sensory] = true;
    pImpl->categoryEnabled[LogCategory::Cognition] = true;
    pImpl->categoryEnabled[LogCategory::Development] = true;
    pImpl->categoryEnabled[LogCategory::Plasticity] = true;
    pImpl->categoryEnabled[LogCategory::Neuromodulation] = true;
    pImpl->categoryEnabled[LogCategory::Prediction] = true;
    pImpl->categoryEnabled[LogCategory::ErrorHandling] = true;
    pImpl->categoryEnabled[LogCategory::Config] = true;
    pImpl->categoryEnabled[LogCategory::Plugin] = true;
    pImpl->categoryEnabled[LogCategory::Performance] = true;
    pImpl->categoryEnabled[LogCategory::MemoryAllocation] = true;
    pImpl->categoryEnabled[LogCategory::Threading] = true;
    pImpl->categoryEnabled[LogCategory::Backtrace] = true;
}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!isCategoryEnabled(entry.category)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    
    // Check if rotation is needed
    if (pImpl->rotationConfig.strategy != RotationStrategy::None) {
        performRotationIfNeeded();
    }
    
    // Format the log entry
    std::string formattedMessage;
    if (pImpl->formatter) {
        formattedMessage = pImpl->formatter->format(entry);
    } else {
        formattedMessage = SimpleFormatter().format(entry);
    }
    
    // Add newline
    formattedMessage += "\n";
    
    // Write to file
    if (pImpl->enableBuffering) {
        pImpl->buffer += formattedMessage;
        if (pImpl->buffer.size() >= pImpl->bufferSize || pImpl->flushOnEachLog) {
            pImpl->fileOut.write(pImpl->buffer.c_str(), pImpl->buffer.size());
            pImpl->fileOut.flush();
            pImpl->currentFileSize += pImpl->buffer.size();
            pImpl->buffer.clear();
        }
    } else {
        pImpl->fileOut.write(formattedMessage.c_str(), formattedMessage.size());
        pImpl->currentFileSize += formattedMessage.size();
        if (pImpl->flushOnEachLog) {
            pImpl->fileOut.flush();
        }
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setCategoryFilter(const std::string& category) {
    // Simple implementation: enable/disable specific category
    for (auto& pair : pImpl->categoryEnabled) {
        pair.second = (pair.first == category);
    }
}

void FileLogger::addCategory(const std::string& category) {
    pImpl->categoryEnabled[category] = true;
}

void FileLogger::removeCategory(const std::string& category) {
    pImpl->categoryEnabled.erase(category);
}

bool FileLogger::isCategoryEnabled(const std::string& category) const {
    auto it = pImpl->categoryEnabled.find(category);
    return (it != pImpl->categoryEnabled.end()) && it->second;
}

void FileLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<LogFormatter> FileLogger::getFormatter() const {
    return pImpl->formatter;
}

bool FileLogger::open(const std::string& filepath, const RotationConfig& rotation) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Set rotation config
    pImpl->rotationConfig = rotation;
    pImpl->lastRotation = std::chrono::system_clock::now();
    
    // Open file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        return false;
    }
    
    pImpl->currentFile = filepath;
    
    // Get current file size
    pImpl->currentFileSize = std::filesystem::file_size(filepath);
    
    return true;
}

void FileLogger::close() {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    
    if (pImpl->enableBuffering && !pImpl->buffer.empty()) {
        pImpl->fileOut.write(pImpl->buffer.c_str(), pImpl->buffer.size());
        pImpl->fileOut.flush();
        pImpl->currentFileSize += pImpl->buffer.size();
        pImpl->buffer.clear();
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->currentFile.clear();
}

std::string FileLogger::getCurrentFile() const {
    return pImpl->currentFile;
}

void FileLogger::enableBuffering(bool enabled, size_t bufferSize) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->enableBuffering = enabled;
    pImpl->bufferSize = bufferSize;
    
    // Flush buffer if enabled is false
    if (!enabled && !pImpl->buffer.empty()) {
        pImpl->fileOut.write(pImpl->buffer.c_str(), pImpl->buffer.size());
        pImpl->fileOut.flush();
        pImpl->currentFileSize += pImpl->buffer.size();
        pImpl->buffer.clear();
    }
}

void FileLogger::setFlushOnEachLog(bool flush) {
    pImpl->flushOnEachLog = flush;
}

void FileLogger::performRotationIfNeeded() {
    if (pImpl->rotationConfig.strategy == RotationStrategy::None) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    
    switch (pImpl->rotationConfig.strategy) {
        case RotationStrategy::SizeBased:
            if (pImpl->currentFileSize >= pImpl->rotationConfig.maxFileSize) {
                rotateFiles();
            }
            break;
        
        case RotationStrategy::TimeBasedHourly:
            if (std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation).count() >= 1) {
                rotateFiles();
            }
            break;
        
        case RotationStrategy::TimeBasedDaily:
            if (std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation).count() >= 24) {
                rotateFiles();
            }
            break;
        
        case RotationStrategy::TimeBasedWeekly:
            if (std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation).count() >= 168) {
                rotateFiles();
            }
            break;
        
        default:
            break;
    }
}

void FileLogger::rotateFiles() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Generate new filename
    std::string newFile = pImpl->currentFile;
    if (pImpl->rotationConfig.strategy != RotationStrategy::SizeBased) {
        // Time-based rotation
        auto time = std::chrono::system_clock::to_time_t(pImpl->lastRotation);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), pImpl->rotationConfig.pattern.c_str());
        newFile = pImpl->currentFile + "." + ss.str();
    } else {
        // Size-based rotation - add .1, .2, etc.
        int rotationNum = 1;
        std::string rotatedFile = pImpl->currentFile;
        
        // Remove old rotated files if exceeds max
        while (rotationNum < pImpl->rotationConfig.maxFiles) {
            std::string testFile = pImpl->currentFile + "." + std::to_string(rotationNum);
            if (std::filesystem::exists(testFile)) {
                rotatedFile = testFile;
                rotationNum++;
            } else {
                break;
            }
        }
        
        // Rename current file to rotated version
        if (rotationNum < pImpl->rotationConfig.maxFiles) {
            std::filesystem::rename(pImpl->currentFile, rotatedFile);
        }
        
        newFile = pImpl->currentFile;
    }
    
    // Reopen new file
    pImpl->fileOut.open(newFile, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFile = newFile;
        pImpl->currentFileSize = 0;
        pImpl->lastRotation = std::chrono::system_clock::now();
        
        // Flush buffer if any
        if (!pImpl->buffer.empty()) {
            pImpl->fileOut.write(pImpl->buffer.c_str(), pImpl->buffer.size());
            pImpl->fileOut.flush();
            pImpl->buffer.clear();
        }
    }
}