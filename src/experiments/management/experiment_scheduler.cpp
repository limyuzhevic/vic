#include "experiment_scheduler.hpp"
#include "experiment_factory.hpp"
#include "enhanced_experiments.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>
#include <algorithm>

using json = nlohmann::json;

namespace nlm {

// ExperimentScheduler implementation
void ExperimentScheduler::scheduleExperiment(std::shared_ptr<BaseExperiment> experiment) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    ExperimentQueueEntry entry;
    entry.experiment = experiment;
    entry.scheduledTime = std::chrono::high_resolution_clock::now();
    entry.priority = false; // Default priority
    
    experimentQueue_.push_back(entry);
    queueCondition_.notify_one();
    
    Logger::getInstance().log("Scheduled experiment: " + experiment->getName(), LogLevel::Info);
}

void ExperimentScheduler::scheduleBatch(const std::vector<std::shared_ptr<BaseExperiment>>& experiments) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    for (const auto& experiment : experiments) {
        ExperimentQueueEntry entry;
        entry.experiment = experiment;
        entry.scheduledTime = std::chrono::high_resolution_clock::now();
        entry.priority = false;
        
        experimentQueue_.push_back(entry);
    }
    
    queueCondition_.notify_all();
    Logger::getInstance().log("Scheduled batch of " + std::to_string(experiments.size()) + " experiments", LogLevel::Info);
}

bool ExperimentScheduler::unscheduleExperiment(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    for (auto it = experimentQueue_.begin(); it != experimentQueue_.end(); ++it) {
        if (it->experiment->getName() == experimentName) {
            experimentQueue_.erase(it);
            Logger::getInstance().log("Unscheduled experiment: " + experimentName, LogLevel::Info);
            return true;
        }
    }
    
    return false;
}

ExperimentScheduler::QueueStatus ExperimentScheduler::getQueueStatus() const {
    QueueStatus status;
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    status.pendingExperiments = experimentQueue_.size();
    status.runningExperiments = (currentExperiment_ != nullptr) ? 1 : 0;
    status.completedExperiments = completedExperiments_.size();
    status.failedExperiments = failedExperiments_.size();
    
    // Calculate average progress
    if (!experimentQueue_.empty()) {
        double totalProgress = 0.0;
        for (const auto& entry : experimentQueue_) {
            totalProgress += entry.experiment->getProgress();
        }
        status.averageProgress = totalProgress / experimentQueue_.size();
    } else {
        status.averageProgress = 0.0;
    }
    
    // Estimate time remaining (simplified)
    if (!experimentQueue_.empty()) {
        auto totalTime = (std::chrono::high_resolution_clock::now() - experimentQueue_.front().scheduledTime);
        status.estimatedTimeRemaining = totalTime * 2.0; // Assume 2x current time for remaining work
    } else {
        status.estimatedTimeRemaining = std::chrono::duration<double>(0);
    }
    
    return status;
}

void ExperimentScheduler::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    paused_ = false;
    
    queueCondition_.notify_all();
    
    Logger::getInstance().log("Experiment scheduler started", LogLevel::Info);
}

void ExperimentScheduler::pause() {
    paused_ = true;
    Logger::getInstance().log("Experiment scheduler paused", LogLevel::Info);
}

void ExperimentScheduler::stop() {
    running_ = false;
    paused_ = false;
    
    queueCondition_.notify_all();
    
    Logger::getInstance().log("Experiment scheduler stopped", LogLevel::Info);
}

std::shared_ptr<BaseExperiment> ExperimentScheduler::getCurrentExperiment() const {
    std::lock_guard<std::mutex> lock(currentExperimentMutex_);
    return currentExperiment_;
}

std::vector<std::shared_ptr<BaseExperiment>> ExperimentScheduler::getCompletedExperiments() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return completedExperiments_;
}

bool ExperimentScheduler::saveState(const std::string& filepath) const {
    try {
        json j;
        j["running"] = running_;
        j["paused"] = paused_;
        j["next_experiment_id"] = nextExperimentId_;
        
        // Save queue
        j["queue"] = json::array();
        for (const auto& entry : experimentQueue_) {
            json entryJson;
            entryJson["name"] = entry.experiment->getName();
            entryJson["type"] = entry.experiment->getExperimentType();
            entryJson["scheduled_time"] = std::chrono::duration<double>(
                entry.scheduledTime.time_since_epoch()).count();
            entryJson["priority"] = entry.priority;
            
            // Save experiment state
            entryJson["state_file"] = "experiment_state_" + entry.experiment->getName() + ".json";
            entry.experiment->saveState(entryJson["state_file"].get<std::string>());
            
            j["queue"].push_back(entryJson);
        }
        
        // Save current experiment
        if (currentExperiment_) {
            json currentJson;
            currentJson["name"] = currentExperiment_->getName();
            currentJson["type"] = currentExperiment_->getExperimentType();
            currentJson["progress"] = currentExperiment_->getProgress();
            j["current_experiment"] = currentJson;
        }
        
        // Save completed experiments
        j["completed_experiments"] = json::array();
        for (const auto& experiment : completedExperiments_) {
            json expJson;
            expJson["name"] = experiment->getName();
            expJson["type"] = experiment->getExperimentType();
            expJson["completion_time"] = std::chrono::duration<double>(
                experiment->getExecutionTime().count()).count();
            j["completed_experiments"].push_back(expJson);
        }
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving scheduler state: " << e.what() << std::endl;
    }
    
    return false;
}

bool ExperimentScheduler::loadState(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Restore scheduler state
        running_ = j["running"].get<bool>();
        paused_ = j["paused"].get<bool>();
        nextExperimentId_ = j["next_experiment_id"].get<size_t>();
        
        // Clear existing queue
        experimentQueue_.clear();
        
        // Load queue from saved state
        for (const auto& entryJson : j["queue"]) {
            ExperimentQueueEntry entry;
            entry.experiment = nullptr;
            entry.experiment->setName(entryJson["name"].get<std::string>());
            
            // Load experiment state (would need to reconstruct experiment from file)
            std::string stateFile = entryJson["state_file"].get<std::string>();
            // TODO: Load experiment from state file
            
            auto scheduledTime = entryJson["scheduled_time"].get<double>();
            entry.scheduledTime = std::chrono::high_resolution_clock::now() - 
                                 std::chrono::duration<double>(scheduledTime);
            entry.priority = entryJson["priority"].get<bool>();
            
            experimentQueue_.push_back(entry);
        }
        
        // Restore current experiment
        if (j.find("current_experiment") != j.end()) {
            auto currentJson = j["current_experiment"];
            currentExperiment_ = nullptr; // TODO: Load from state file
        }
        
        // Restore completed experiments
        completedExperiments_.clear();
        for (const auto& expJson : j["completed_experiments"]) {
            std::shared_ptr<BaseExperiment> experiment = nullptr; // TODO: Load from state file
            if (experiment) {
                completedExperiments_.push_back(experiment);
            }
        }
        
        // Start worker threads if running
        if (running_) {
            queueCondition_.notify_all();
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading scheduler state: " << e.what() << std::endl;
    }
    
    return false;
}

void ExperimentScheduler::cleanupCompletedExperiments() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    // Remove old completed experiments
    auto it = std::remove_if(completedExperiments_.begin(), completedExperiments_.end(),
                            [](const std::shared_ptr<BaseExperiment>& exp) {
                                // Keep experiments for a certain time
                                auto elapsed = std::chrono::high_resolution_clock::now() - exp->getExecutionTime();
                                return elapsed > std::chrono::hours(24); // Keep for 24 hours
                            });
    
    completedExperiments_.erase(it, completedExperiments_.end());
}

void ExperimentScheduler::workerThread(size_t workerId) {
    Logger::getInstance().log("Worker thread " + std::to_string(workerId) + " started", LogLevel::Info);
    
    while (running_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        
        // Wait for work or shutdown
        queueCondition_.wait_for(lock, std::chrono::seconds(1), 
                                [this]() { return !running_ || !paused_ || !experimentQueue_.empty(); });
        
        if (!running_ || paused_ || experimentQueue_.empty()) {
            continue;
        }
        
        // Get next experiment
        ExperimentQueueEntry entry = experimentQueue_.front();
        experimentQueue_.erase(experimentQueue_.begin());
        
        lock.unlock();
        
        // Set as current experiment
        {
            std::lock_guard<std::mutex> currentLock(currentExperimentMutex_);
            currentExperiment_ = entry.experiment;
        }
        
        // Run experiment
        try {
            if (entry.experiment->initialize()) {
                if (entry.experiment->run()) {
                    entry.experiment->finalize();
                    
                    std::lock_guard<std::mutex> completedLock(queueMutex_);
                    completedExperiments_.push_back(entry.experiment);
                    
                    Logger::getInstance().log("Experiment completed: " + entry.experiment->getName(), LogLevel::Info);
                } else {
                    std::lock_guard<std::mutex> failedLock(queueMutex_);
                    failedExperiments_.push_back(entry.experiment);
                    
                    Logger::getInstance().log("Experiment failed: " + entry.experiment->getName(), LogLevel::Error);
                }
            } else {
                std::lock_guard<std::mutex> failedLock(queueMutex_);
                failedExperiments_.push_back(entry.experiment);
                
                Logger::getInstance().log("Experiment initialization failed: " + entry.experiment->getName(), LogLevel::Error);
            }
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> failedLock(queueMutex_);
            failedExperiments_.push_back(entry.experiment);
            
            Logger::getInstance().log("Experiment exception: " + std::string(e.what()), LogLevel::Error);
        }
        
        // Clear current experiment
        {
            std::lock_guard<std::mutex> currentLock(currentExperimentMutex_);
            currentExperiment_ = nullptr;
        }
        
        // Small delay between experiments
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    Logger::getInstance().log("Worker thread " + std::to_string(workerId) + " stopped", LogLevel::Info);
}

} // namespace nlm