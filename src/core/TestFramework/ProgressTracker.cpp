#include "ProgressTracker.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

ProgressTracker::ProgressTracker() 
    : updateInterval_(100), showDetails_(true), colorOutput_(true) {
}

void ProgressTracker::startTest(const std::string& testName, const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProgressEntry entry;
    entry.testName = testName;
    entry.category = category;
    entry.startTime = std::chrono::steady_clock::now();
    entry.endTime = std::chrono::steady_clock::time_point();
    entry.progress = 0.0f;
    entry.completed = false;
    entry.success = false;
    entry.statusMessage = "Starting...";
    entry.outputLines = 0;
    
    progress_.push_back(entry);
}

void ProgressTracker::updateProgress(const std::string& testName, float progress, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : progress_) {
        if (entry.testName == testName && !entry.completed) {
            entry.progress = std::max(0.0f, std::min(1.0f, progress));
            if (!message.empty()) {
                entry.statusMessage = message;
            }
            break;
        }
    }
    
    lastUpdate_ = std::chrono::steady_clock::now();
}

void ProgressTracker::endTest(const std::string& testName, bool success, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : progress_) {
        if (entry.testName == testName && !entry.completed) {
            entry.endTime = std::chrono::steady_clock::now();
            entry.progress = 1.0f;
            entry.completed = true;
            entry.success = success;
            entry.statusMessage = message;
            break;
        }
    }
}

void ProgressTracker::endTest(const std::string& testName, bool success, const std::string& message, 
                              const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& entry : progress_) {
        if (entry.testName == testName && !entry.completed) {
            entry.endTime = std::chrono::steady_clock::now();
            entry.progress = 1.0f;
            entry.completed = true;
            entry.success = success;
            entry.statusMessage = message;
            entry.errorMessage = error;
            break;
        }
    }
}

const ProgressTracker::ProgressEntry* ProgressTracker::getCurrentTest() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& entry : progress_) {
        if (!entry.completed) {
            return &entry;
        }
    }
    
    return nullptr;
}

const std::vector<ProgressTracker::ProgressEntry>& ProgressTracker::getAllProgress() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return progress_;
}

size_t ProgressTracker::getTotalTests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return progress_.size();
}

size_t ProgressTracker::getCompletedTests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t completed = 0;
    for (const auto& entry : progress_) {
        if (entry.completed) {
            completed++;
        }
    }
    return completed;
}

size_t ProgressTracker::getFailedTests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t failed = 0;
    for (const auto& entry : progress_) {
        if (entry.completed && !entry.success) {
            failed++;
        }
    }
    return failed;
}

size_t ProgressTracker::getSkippedTests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t skipped = 0;
    for (const auto& entry : progress_) {
        if (!entry.completed && entry.progress == 0.0f) {
            skipped++;
        }
    }
    return skipped;
}

float ProgressTracker::getOverallProgress() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (progress_.empty()) {
        return 0.0f;
    }
    
    float completed = 0.0f;
    for (const auto& entry : progress_) {
        if (entry.completed) {
            completed += 1.0f;
        } else if (entry.progress > 0.0f) {
            completed += entry.progress;
        }
    }
    
    return completed / static_cast<float>(progress_.size());
}

void ProgressTracker::displayProgress(bool forceUpdate) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto timeSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate_);
    
    if (!forceUpdate && timeSinceUpdate.count() < updateInterval_) {
        return;
    }
    
    if (!showDetails_) {
        return;
    }
    
    displayProgressInternal();
    lastUpdate_ = now;
}

void ProgressTracker::displayFinalSummary() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::cout << std::endl;
    std::cout << "=== Test Execution Summary ===" << std::endl;
    std::cout << std::endl;
    
    size_t total = progress_.size();
    size_t completed = getCompletedTests();
    size_t failed = getFailedTests();
    size_t skipped = getSkippedTests();
    
    std::cout << "Total Tests: " << total << std::endl;
    std::cout << "Completed: " << completed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Skipped: " << skipped << std::endl;
    
    if (total > 0) {
        float successRate = (static_cast<float>(completed - failed) / static_cast<float>(total)) * 100.0f;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
    }
    
    if (failed > 0) {
        std::cout << std::endl;
        std::cout << "Failed Tests:" << std::endl;
        for (const auto& entry : progress_) {
            if (entry.completed && !entry.success) {
                std::cout << "  - " << entry.testName;
                if (!entry.errorMessage.empty()) {
                    std::cout << ": " << entry.errorMessage;
                }
                std::cout << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
}

std::string ProgressTracker::getProgressSummary() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Progress: " << std::fixed << std::setprecision(1) << (getOverallProgress() * 100.0f) << "% ("
       << getCompletedTests() << "/" << getTotalTests() << " tests)";
    
    return ss.str();
}

void ProgressTracker::setUpdateInterval(int interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    updateInterval_ = interval;
}

void ProgressTracker::setShowDetails(bool show) {
    std::lock_guard<std::mutex> lock(mutex_);
    showDetails_ = show;
}

void ProgressTracker::setColorOutput(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    colorOutput_ = enable;
}

void ProgressTracker::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    progress_.clear();
}

void ProgressTracker::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    progress_.clear();
    lastUpdate_ = std::chrono::steady_clock::now();
}

void ProgressTracker::displayProgressInternal() const {
    if (progress_.empty()) {
        return;
    }
    
    // Clear line for progress update
    std::cout << "\r";
    
    // Display progress bar
    float overallProgress = getOverallProgress();
    int barWidth = 50;
    int filledWidth = static_cast<int>(barWidth * overallProgress);
    
    std::cout << "["; 
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << (overallProgress * 100.0f) << "%";
    
    // Display current test
    const ProgressEntry* currentTest = getCurrentTest();
    if (currentTest) {
        std::cout << " - " << currentTest->testName << " (" << currentTest->statusMessage << ")";
    }
    
    std::cout.flush();
}

std::string ProgressTracker::formatTestName(const std::string& name, bool active) const {
    if (colorOutput_) {
        return active ? "\033[1;32m" + name + "\033[0m" : name;
    }
    return name;
}

std::string ProgressTracker::formatProgress(float progress) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << (progress * 100.0f) << "%";
    return ss.str();
}

std::string ProgressTracker::getStatusColor(bool success, bool active) const {
    if (!colorOutput_) {
        return "";
    }
    
    if (active) {
        return "\033[1;32m"; // Green for active
    } else if (success) {
        return "\033[32m"; // Green for success
    } else {
        return "\033[31m"; // Red for failure
    }
}

} // namespace nlm
