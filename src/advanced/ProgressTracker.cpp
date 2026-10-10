// NLM Advanced Features - Progress Tracker Implementation
// Advanced progress tracking system with ETA, checkpoints, and callbacks

#include "ProgressTracker.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <filesystem>

namespace nlm {
namespace advanced {

ProgressTracker::ProgressTracker() 
    : currentProgress{0, 0, 0.0, 0.0, 0.0, "", "", {}}
    , updateInterval(0.1)
    , autoSave(false)
    , autoSaveInterval(60.0)
    , maxCheckpoints(100)
    , enableVisualizations(false)
    , visualizationUpdateInterval(1.0)
    , active(false)
    , paused(false)
    , cancelled(false)
    , completed(false)
    , updateCount(0)
    , totalTime(0.0)
    , totalElapsed(0.0)
    , lastUpdateTime(std::chrono::steady_clock::now()) {}

ProgressTracker::~ProgressTracker() {
    // Cleanup resources
}

void ProgressTracker::startProgress(int totalSteps, const std::string& stageName) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (active) {
        NLM_LOG_WARNING("ProgressTracker: Progress already active. Call cancelProgress() first.");
        return;
    }
    
    currentProgress.totalSteps = totalSteps;
    currentProgress.currentStep = 0;
    currentProgress.percentage = 0.0;
    currentProgress.elapsedTime = 0.0;
    currentProgress.estimatedRemainingTime = 0.0;
    currentProgress.currentStatus = "Starting";
    currentProgress.stageName = stageName;
    currentProgress.metrics.clear();
    
    startTime = std::chrono::steady_clock::now();
    active = true;
    paused = false;
    cancelled = false;
    completed = false;
    updateCount = 0;
    totalElapsed = 0.0;
    
    NLM_LOG_INFO("ProgressTracker: Started progress for " + stageName + " (" + 
                std::to_string(totalSteps) + " steps)");
    
    notifyCallbacks();
}

void ProgressTracker::updateProgress(int currentStep, const std::string& status) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!active || cancelled || paused) {
        return;
    }
    
    currentProgress.currentStep = currentStep;
    currentProgress.currentStatus = status;
    
    // Update metrics if we have previous values
    if (currentProgress.currentStep > 0) {
        // Calculate basic metrics
        currentProgress.metrics["steps_per_second"] = currentStep / (currentProgress.elapsedTime + 0.001);
        currentProgress.metrics["percentage"] = (static_cast<double>(currentStep) / currentProgress.totalSteps) * 100.0;
    }
    
    updateETA();
    updateProgressHistory();
    notifyCallbacks();
    
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - startTime).count();
    totalElapsed = elapsed;
    currentProgress.elapsedTime = elapsed;
    
    updateCount++;
}

void ProgressTracker::completeProgress() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!active || completed) {
        return;
    }
    
    currentProgress.currentStep = currentProgress.totalSteps;
    currentProgress.percentage = 100.0;
    currentProgress.currentStatus = "Completed";
    updateETA();
    completed = true;
    active = false;
    
    NLM_LOG_INFO("ProgressTracker: Progress completed (100%)");
    
    notifyCallbacks();
    
    // Notify completion callback
    if (completionCallback) {
        completionCallback();
    }
    
    completionEvent.notify_all();
}

void ProgressTracker::pauseProgress() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!active || paused) {
        return;
    }
    
    paused = true;
    NLM_LOG_INFO("ProgressTracker: Progress paused");
}

void ProgressTracker::resumeProgress() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!paused || !active) {
        return;
    }
    
    paused = false;
    startTime = std::chrono::steady_clock::now(); // Adjust start time
    NLM_LOG_INFO("ProgressTracker: Progress resumed");
}

void ProgressTracker::cancelProgress() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!active || cancelled) {
        return;
    }
    
    cancelled = true;
    active = false;
    NLM_LOG_INFO("ProgressTracker: Progress cancelled");
    
    notifyCallbacks();
}

bool ProgressTracker::isActive() const {
    std::lock_guard<std::mutex> lock(mutex);
    return active && !cancelled && !completed;
}

bool ProgressTracker::isPaused() const {
    std::lock_guard<std::mutex> lock(mutex);
    return paused && active;
}

int ProgressTracker::getCurrentStep() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.currentStep;
}

int ProgressTracker::getTotalSteps() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.totalSteps;
}

double ProgressTracker::getPercentage() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.percentage;
}

double ProgressTracker::getElapsedTime() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.elapsedTime;
}

double ProgressTracker::getEstimatedRemainingTime() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.estimatedRemainingTime;
}

const ProgressInfo& ProgressTracker::getProgressInfo() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress;
}

std::string ProgressTracker::getStageName() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.stageName;
}

std::map<std::string, double> ProgressTracker::getMetrics() const {
    std::lock_guard<std::mutex> lock(mutex);
    return currentProgress.metrics;
}

void ProgressTracker::addCallback(const ProgressCallback& callback) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (callback.enabled) {
        callbacks.push_back(callback);
        NLM_LOG_INFO("ProgressTracker: Added callback: " + callback.name);
    }
}

void ProgressTracker::removeCallback(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::remove_if(callbacks.begin(), callbacks.end(),
                             [&name](const ProgressCallback& cb) { return cb.name == name; });
    if (it != callbacks.end()) {
        callbacks.erase(it, callbacks.end());
        NLM_LOG_INFO("ProgressTracker: Removed callback: " + name);
    }
}

void ProgressTracker::clearCallbacks() {
    std::lock_guard<std::mutex> lock(mutex);
    callbacks.clear();
    NLM_LOG_INFO("ProgressTracker: Cleared all callbacks");
}

void ProgressTracker::setProgressCallback(const std::function<void(const ProgressInfo&)>& callback, const std::string& name) {
    progressCallback = callback;
    NLM_LOG_INFO("ProgressTracker: Set progress callback: " + name);
}

void ProgressTracker::updateETA() {
    currentProgress.estimatedRemainingTime = calculateETA(currentProgress.currentStep, currentProgress.totalSteps, currentProgress.elapsedTime);
}

double ProgressTracker::calculateETA(int current, int total, double elapsedTime) const {
    if (current <= 0 || total <= 0) return 0.0;
    return (total - current) / static_cast<double>(current) * elapsedTime;
}

std::string ProgressTracker::formatTime(double seconds) const {
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;
    int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    std::ostringstream oss;
    if (hours > 0) oss << hours << "h ";
    oss << minutes << "m " << secs << "." << std::setw(3) << std::setfill('0') << millisecs << "s";
    return oss.str();
}

bool ProgressTracker::createCheckpoint(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Check if checkpoint already exists
    for (const auto& checkpoint : checkpoints) {
        if (checkpoint.name == name) {
            NLM_LOG_WARNING("ProgressTracker: Checkpoint already exists: " + name);
            return false;
        }
    }
    
    Checkpoint checkpoint;
    checkpoint.name = name;
    checkpoint.timestamp = std::chrono::system_clock::now();
    checkpoint.description = description;
    checkpoint.metadata["current_step"] = std::to_string(currentProgress.currentStep);
    checkpoint.metadata["total_steps"] = std::to_string(currentProgress.totalSteps);
    checkpoint.metadata["percentage"] = std::to_string(currentProgress.percentage);
    checkpoint.metadata["elapsed_time"] = std::to_string(currentProgress.elapsedTime);
    checkpoint.metadata["status"] = currentProgress.currentStatus;
    checkpoint.metadata["stage_name"] = currentProgress.stageName;
    checkpoint.checkpointSize = sizeof(ProgressInfo); // Simplified size calculation
    
    checkpoints.push_back(checkpoint);
    
    NLM_LOG_INFO("ProgressTracker: Created checkpoint: " + name);
    
    // Notify checkpoint callback
    if (checkpointCallback) {
        checkpointCallback(checkpoint);
    }
    
    return true;
}

bool ProgressTracker::restoreCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& checkpoint : checkpoints) {
        if (checkpoint.name == name) {
            // Restore progress info from checkpoint metadata
            currentProgress.currentStep = std::stoi(checkpoint.metadata.at("current_step"));
            currentProgress.totalSteps = std::stoi(checkpoint.metadata.at("total_steps"));
            currentProgress.percentage = std::stod(checkpoint.metadata.at("percentage"));
            currentProgress.elapsedTime = std::stod(checkpoint.metadata.at("elapsed_time"));
            currentProgress.currentStatus = checkpoint.metadata.at("status");
            currentProgress.stageName = checkpoint.metadata.at("stage_name");
            
            active = true;
            completed = false;
            cancelled = false;
            paused = false;
            
            NLM_LOG_INFO("ProgressTracker: Restored checkpoint: " + name);
            return true;
        }
    }
    
    NLM_LOG_ERROR("ProgressTracker: Checkpoint not found: " + name);
    return false;
}

std::vector<Checkpoint> ProgressTracker::getAvailableCheckpoints() const {
    std::lock_guard<std::mutex> lock(mutex);
    return checkpoints;
}

bool ProgressTracker::deleteCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::remove_if(checkpoints.begin(), checkpoints.end(),
                             [&name](const Checkpoint& cp) { return cp.name == name; });
    if (it != checkpoints.end()) {
        checkpoints.erase(it, checkpoints.end());
        NLM_LOG_INFO("ProgressTracker: Deleted checkpoint: " + name);
        return true;
    }
    
    NLM_LOG_WARNING("ProgressTracker: Checkpoint not found for deletion: " + name);
    return false;
}

bool ProgressTracker::cleanupOldCheckpoints(int keepCount) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (checkpoints.size() <= static_cast<size_t>(keepCount)) {
        return true;
    }
    
    // Sort checkpoints by timestamp (oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const Checkpoint& a, const Checkpoint& b) { return a.timestamp < b.timestamp; });
    
    // Remove oldest checkpoints
    int toRemove = checkpoints.size() - keepCount;
    for (int i = 0; i < toRemove; ++i) {
        NLM_LOG_INFO("ProgressTracker: Cleaning up old checkpoint: " + checkpoints[i].name);
    }
    
    checkpoints.erase(checkpoints.begin(), checkpoints.begin() + toRemove);
    
    cleanupOldCheckpointsInternal();
    return true;
}

void ProgressTracker::displayProgressBar(const ProgressInfo& info, bool showPercentage, bool showETA) {
    std::cout << "\r";
    
    // Progress bar
    int barWidth = 50;
    int progressWidth = static_cast<int>((info.percentage / 100.0) * barWidth);
    
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < progressWidth) std::cout << "=";
        else std::cout << " ";
    }
    std::cout << "]";
    
    if (showPercentage) {
        std::cout << " " << std::setw(3) << static_cast<int>(info.percentage) << "%";
    }
    
    if (showETA && info.estimatedRemainingTime > 0) {
        std::cout << " ETA: " << formatTime(info.estimatedRemainingTime);
    }
    
    std::cout << " " << info.currentStatus;
    
    if (info.currentStep > 0 && info.totalSteps > 0) {
        std::cout << " (" << info.currentStep << "/" << info.totalSteps << ")";
    }
    
    std::cout << std::flush;
}

void ProgressTracker::displayProgressText(const ProgressInfo& info) {
    std::cout << "Progress Update:" << std::endl;
    std::cout << "  Stage: " << info.stageName << std::endl;
    std::cout << "  Progress: " << formatPercentage(info.percentage) << std::endl;
    std::cout << "  Current Step: " << info.currentStep << " of " << info.totalSteps << std::endl;
    std::cout << "  Status: " << info.currentStatus << std::endl;
    std::cout << "  Elapsed Time: " << formatTime(info.elapsedTime) << std::endl;
    
    if (info.estimatedRemainingTime > 0) {
        std::cout << "  Estimated Remaining: " << formatTime(info.estimatedRemainingTime) << std::endl;
    }
    
    std::cout << "  Metrics:" << std::endl;
    for (const auto& metric : info.metrics) {
        std::cout << "    " << metric.first << ": " << metric.second << std::endl;
    }
    std::cout << std::endl;
}

void ProgressTracker::displayProgressJSON(const ProgressInfo& info, std::ostream& output) {
    output << "{" << std::endl;
    output << "  \"current_step\": " << info.currentStep << "," << std::endl;
    output << "  \"total_steps\": " << info.totalSteps << "," << std::endl;
    output << "  \"percentage\": " << std::fixed << std::setprecision(2) << info.percentage << "," << std::endl;
    output << "  \"elapsed_time\": " << std::fixed << std::setprecision(3) << info.elapsedTime << "," << std::endl;
    output << "  \"estimated_remaining_time\": " << std::fixed << std::setprecision(3) << info.estimatedRemainingTime << "," << std::endl;
    output << "  \"current_status\": \"" << info.currentStatus << "\"," << std::endl;
    output << "  \"stage_name\": \"" << info.stageName << "\"";
    
    if (!info.metrics.empty()) {
        output << "," << std::endl;
        output << "  \"metrics\": {" << std::endl;
        bool first = true;
        for (const auto& metric : info.metrics) {
            if (!first) output << "," << std::endl;
            output << "    \"" << metric.first << "\": " << std::fixed << std::setprecision(6) << metric.second;
            first = false;
        }
        output << std::endl << "  }";
    }
    
    output << std::endl << "}" << std::endl;
}

void ProgressTracker::displayProgressCSV(const ProgressInfo& info, std::ostream& output) {
    output << "current_step,total_steps,percentage,elapsed_time,estimated_remaining_time,current_status,stage_name" << std::endl;
    output << info.currentStep << ","
           << info.totalSteps << ","
           << std::fixed << std::setprecision(2) << info.percentage << ","
           << std::fixed << std::setprecision(3) << info.elapsedTime << ","
           << std::fixed << std::setprecision(3) << info.estimatedRemainingTime << ","
           << "\"" << info.currentStatus << "\","
           << "\"" << info.stageName << "\"" << std::endl;
}

void ProgressTracker::displayProgressXML(const ProgressInfo& info, std::ostream& output) {
    output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    output << "<progress>" << std::endl;
    output << "  <current_step>" << info.currentStep << "</current_step>" << std::endl;
    output << "  <total_steps>" << info.totalSteps << "</total_steps>" << std::endl;
    output << "  <percentage>" << std::fixed << std::setprecision(2) << info.percentage << "</percentage>" << std::endl;
    output << "  <elapsed_time>" << std::fixed << std::setprecision(3) << info.elapsedTime << "</elapsed_time>" << std::endl;
    output << "  <estimated_remaining_time>" << std::fixed << std::setprecision(3) << info.estimatedRemainingTime << "</estimated_remaining_time>" << std::endl;
    output << "  <current_status>" << info.currentStatus << "</current_status>" << std::endl;
    output << "  <stage_name>" << info.stageName << "</stage_name>" << std::endl;
    output << "  <metrics>" << std::endl;
    
    for (const auto& metric : info.metrics) {
        output << "    <metric name=\"" << metric.first << "\">" << std::fixed << std::setprecision(6) << metric.second << "</metric>" << std::endl;
    }
    
    output << "  </metrics>" << std::endl;
    output << "</progress>" << std::endl;
}

void ProgressTracker::setUpdateInterval(double interval) {
    std::lock_guard<std::mutex> lock(mutex);
    updateInterval = interval;
}

void ProgressTracker::setAutoSave(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    autoSave = enabled;
}

void ProgressTracker::setAutoSaveInterval(double interval) {
    std::lock_guard<std::mutex> lock(mutex);
    autoSaveInterval = interval;
}

void ProgressTracker::setMaxCheckpoints(int maxCheckpoints) {
    std::lock_guard<std::mutex> lock(mutex);
    this->maxCheckpoints = maxCheckpoints;
    cleanupOldCheckpointsInternal();
}

void ProgressTracker::setEnableVisualizations(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    enableVisualizations = enabled;
}

void ProgressTracker::setVisualizationUpdateInterval(double interval) {
    std::lock_guard<std::mutex> lock(mutex);
    visualizationUpdateInterval = interval;
}

int ProgressTracker::getUpdateCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return updateCount;
}

double ProgressTracker::getTotalTime() const {
    std::lock_guard<std::mutex> lock(mutex);
    return totalTime;
}

double ProgressTracker::getAverageUpdateInterval() const {
    std::lock_guard<std::mutex> lock(mutex);
    return updateCount > 0 ? totalElapsed / updateCount : 0.0;
}

void ProgressTracker::exportProgressHistory(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex);
    saveProgressToFile(filename);
}

void ProgressTracker::importProgressHistory(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex);
    loadProgressFromFile(filename);
}

std::vector<ProgressInfo> ProgressTracker::getProgressHistory() const {
    std::lock_guard<std::mutex> lock(mutex);
    return progressHistory;
}

void ProgressTracker::lock() {
    mutex.lock();
}

void ProgressTracker::unlock() {
    mutex.unlock();
}

bool ProgressTracker::tryLock() {
    return mutex.try_lock();
}

void ProgressTracker::waitForCompletion() {
    std::unique_lock<std::mutex> lock(mutex);
    completionEvent.wait(lock, [this]() { return completed || cancelled; });
}

bool ProgressTracker::waitForCompletion(double timeout) {
    std::unique_lock<std::mutex> lock(mutex);
    return completionEvent.wait_for(lock, std::chrono::duration<double>(timeout),
                                    [this]() { return completed || cancelled; });
}

void ProgressTracker::setCompletionEvent(const std::string& eventName) {
    std::lock_guard<std::mutex> lock(mutex);
    completionEventName = eventName;
}

void ProgressTracker::notifyCallbacks() {
    for (const auto& callback : callbacks) {
        if (callback.enabled && callback.callback) {
            callback.callback(currentProgress);
        }
    }
}

void ProgressTracker::updateProgressHistory() {
    progressHistory.push_back(currentProgress);
    
    // Limit history size
    if (progressHistory.size() > 1000) {
        progressHistory.erase(progressHistory.begin());
    }
}

bool ProgressTracker::saveProgressToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("ProgressTracker: Could not save progress to file: " + filename);
        return false;
    }
    
    // Save progress history in JSON format
    file << "[" << std::endl;
    bool first = true;
    for (const auto& progress : progressHistory) {
        if (!first) file << "," << std::endl;
        // Simplified JSON serialization
        file << "{\"step\": " << progress.currentStep << ", \"total\": " << progress.totalSteps 
             << ", \"percentage\": " << std::fixed << std::setprecision(2) << progress.percentage;
        first = false;
    }
    file << std::endl << "]" << std::endl;
    
    file.close();
    NLM_LOG_INFO("ProgressTracker: Progress history saved to: " + filename);
    return true;
}

bool ProgressTracker::loadProgressFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("ProgressTracker: Could not load progress from file: " + filename);
        return false;
    }
    
    // Simplified JSON parsing
    progressHistory.clear();
    std::string line;
    while (std::getline(file, line)) {
        // Parse line (simplified)
        if (line.find("{\"step\": ") != std::string::npos) {
            ProgressInfo progress;
            // Parse step
            size_t stepPos = line.find("{\"step\": ") + 10;
            size_t commaPos = line.find(",", stepPos);
            if (stepPos != std::string::npos && commaPos != std::string::npos) {
                progress.currentStep = std::stoi(line.substr(stepPos, commaPos - stepPos));
            }
            
            // Parse total
            size_t totalPos = line.find("\"total\": ") + 9;
            if (totalPos != std::string::npos) {
                progress.totalSteps = std::stoi(line.substr(totalPos));
            }
            
            progressHistory.push_back(progress);
        }
    }
    
    file.close();
    NLM_LOG_INFO("ProgressTracker: Progress history loaded from: " + filename);
    return true;
}

void ProgressTracker::cleanupOldCheckpointsInternal() {
    if (checkpoints.size() <= static_cast<size_t>(maxCheckpoints)) {
        return;
    }
    
    // Sort by timestamp
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const Checkpoint& a, const Checkpoint& b) { return a.timestamp < b.timestamp; });
    
    // Remove oldest checkpoints
    int toRemove = checkpoints.size() - maxCheckpoints;
    for (int i = 0; i < toRemove; ++i) {
        checkpoints.erase(checkpoints.begin());
    }
}

std::string ProgressTracker::generateCheckpointPath(const std::string& name) const {
    std::filesystem::path basePath = std::filesystem::current_path();
    std::filesystem::path checkpointDir = basePath / "checkpoints";
    std::filesystem::create_directories(checkpointDir);
    
    std::ostringstream oss;
    oss << checkpointDir.string() << "/" << name << "_" << getCurrentTimestamp() << ".cp";
    return oss.str();
}

bool ProgressTracker::validateCheckpoint(const Checkpoint& checkpoint) const {
    // Check if checkpoint has required metadata
    const std::vector<std::string> requiredFields = {"current_step", "total_steps", "percentage", "elapsed_time"};
    
    for (const auto& field : requiredFields) {
        if (checkpoint.metadata.find(field) == checkpoint.metadata.end()) {
            NLM_LOG_WARNING("ProgressTracker: Invalid checkpoint - missing field: " + field);
            return false;
        }
    }
    
    return true;
}

void ProgressTracker::addProgressTracker(const std::shared_ptr<ProgressTracker>& tracker) {
    std::lock_guard<std::mutex> lock(mutex);
    childTrackers.push_back(tracker);
}

void ProgressTracker::removeProgressTracker(const std::shared_ptr<ProgressTracker>& tracker) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::remove_if(childTrackers.begin(), childTrackers.end(),
                             [&tracker](const std::shared_ptr<ProgressTracker>& t) { return t.get() == tracker.get(); });
    childTrackers.erase(it, childTrackers.end());
}

void ProgressTracker::aggregateProgress() {
    // Aggregate progress from child trackers
    int totalSteps = 0;
    int currentSteps = 0;
    
    for (const auto& tracker : childTrackers) {
        totalSteps += tracker->getTotalSteps();
        currentSteps += tracker->getCurrentStep();
    }
    
    if (totalSteps > 0) {
        currentProgress.percentage = (static_cast<double>(currentSteps) / totalSteps) * 100.0;
    }
}

std::string ProgressTracker::formatPercentage(double percentage) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percentage << "%";
    return oss.str();
}

std::string ProgressTracker::formatTime(double seconds, bool includeMilliseconds) {
    if (includeMilliseconds) {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        int millisecs = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        std::ostringstream oss;
        if (hours > 0) oss << hours << "h ";
        oss << minutes << "m " << secs << "." << std::setw(3) << std::setfill('0') << millisecs << "s";
        return oss.str();
    } else {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        
        std::ostringstream oss;
        if (hours > 0) oss << hours << "h ";
        oss << minutes << "m " << secs << "s";
        return oss.str();
    }
}

std::string ProgressTracker::formatSpeed(double value, const std::string& unit) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value << " " << unit;
    return oss.str();
}

std::string ProgressTracker::generateProgressID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    
    std::time_t now = std::time(nullptr);
    char timeBuf[20];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y%m%d%H%M%S", std::localtime(&now));
    
    return std::string(timeBuf) + "_" + std::to_string(dis(gen));
}

std::string ProgressTracker::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char timeBuf[20];
    std::strftime(timeBuf, sizeof(timeBuf), "%Y%m%d_%H%M%S", std::localtime(&now));
    return std::string(timeBuf);
}

ProgressTracker& ProgressTracker::getInstance() {
    static ProgressTracker instance;
    return instance;
}

void ProgressTracker::resetInstance() {
    // Reset singleton instance (use with caution)
    // This is primarily for testing purposes
}

// Callback function implementations (inline in header would be better, but keeping here for compatibility)
void ProgressTracker::setProgressCallback(ProgressCallbackFunc callback) {
    progressCallback = callback;
}

void ProgressTracker::setCheckpointCallback(CheckpointCallbackFunc callback) {
    checkpointCallback = callback;
}

void ProgressTracker::setCompletionCallback(CompletionCallbackFunc callback) {
    completionCallback = callback;
}

void ProgressTracker::startNeuralSimulationProgress(int neuronCount, int steps) {
    std::string stageName = "Neural Simulation (" + std::to_string(neuronCount) + " neurons)";
    startProgress(steps, stageName);
    
    // Add neural-specific metrics
    currentProgress.metrics["neuron_count"] = neuronCount;
    currentProgress.metrics["max_fire_rate"] = 100.0; // Hz
    currentProgress.metrics["synapse_density"] = 0.1; // connections per neuron
}

void ProgressTracker::startBenchmarkProgress(int benchmarkCount, const std::vector<std::string>& benchmarkNames) {
    std::string stageName = "Benchmarking (" + std::to_string(benchmarkCount) + " tests)";
    startProgress(benchmarkCount, stageName);
    
    currentProgress.metrics["benchmark_names"] = "";
    for (size_t i = 0; i < benchmarkNames.size(); ++i) {
        if (i > 0) currentProgress.metrics["benchmark_names"] += ", ";
        currentProgress.metrics["benchmark_names"] += benchmarkNames[i];
    }
}

void ProgressTracker::startBatchProcessingProgress(int fileCount, const std::string& operation) {
    std::string stageName = "Batch Processing: " + operation;
    startProgress(fileCount, stageName);
    currentProgress.metrics["operation"] = operation;
}

void ProgressTracker::startCheckpointingProgress(int checkpointCount) {
    std::string stageName = "Checkpointing (" + std::to_string(checkpointCount) + " checkpoints)";
    startProgress(checkpointCount, stageName);
}

} // namespace advanced
} // namespace nlm
