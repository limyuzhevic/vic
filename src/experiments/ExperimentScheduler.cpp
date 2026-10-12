// ExperimentScheduler.cpp implementation
#include "ExperimentScheduler.hpp"
#include <algorithm>

namespace nlm {

ExperimentScheduler::ExperimentScheduler(ScheduleMode mode) : mode(mode), running(false),
    progress(0.0), completedCount(0), totalCount(0), paused(false) {}

ExperimentScheduler::~ExperimentScheduler() {
    stop();
}

void ExperimentScheduler::addExperiment(const std::string& name, const ExperimentSchedule& schedule) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Store schedule
    schedules[name] = schedule;
    totalCount++;
}

void ExperimentScheduler::removeExperiment(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (schedules.find(name) != schedules.end()) {
        if (schedules[name].currentlyRunning) {
            // Should stop running experiment
        }
        schedules.erase(name);
        totalCount--;
    }
}

void ExperimentScheduler::clearSchedule() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Stop all running experiments
    for (auto& pair : schedules) {
        if (pair.second.currentlyRunning) {
            // Stop running experiment
        }
    }
    
    schedules.clear();
    completedCount = 0;
    totalCount = 0;
    progress = 0.0;
}

std::vector<ExperimentResult> ExperimentScheduler::executeSchedule(std::shared_ptr<Brain> brain,
                                                                     std::shared_ptr<Environment> environment) {
    std::lock_guard<std::mutex> lock(mutex);
    
    running = true;
    progress = 0.0;
    completedCount = 0;
    results.clear();
    
    // Get execution order
    std::vector<std::string> executionOrder = getExecutionOrder();
    
    // Execute experiments in order
    for (const auto& name : executionOrder) {
        if (!running) break;
        
        auto it = schedules.find(name);
        if (it == schedules.end()) continue;
        
        ExperimentSchedule& schedule = it->second;
        
        // Check if experiment is ready to run
        if (schedule.shouldRunNow()) {
            schedule.currentlyRunning = true;
            
            // Execute experiment
            ExperimentResult result = executeSingle(name, brain, environment, schedule);
            results.push_back(result);
            
            schedule.currentlyRunning = false;
            completedCount++;
            
            // Update progress
            progress = static_cast<double>(completedCount) / static_cast<double>(totalCount);
            
            // Notify completion
            if (completionCallback) {
                completionCallback(result);
            }
        }
        
        // Wait if paused
        if (paused) {
            std::unique_lock<std::mutex> lk(mutex);
            cv.wait(lk, [this]() { return !paused; });
        }
    }
    
    running = false;
    return results;
}

ExperimentResult ExperimentScheduler::executeSingle(const std::string& name,
                                                     std::shared_ptr<Brain> brain,
                                                     std::shared_ptr<Environment> environment,
                                                     const ExperimentSchedule& schedule) {
    // Simplified execution - in real implementation would create and run experiment
    ExperimentResult result;
    result.experimentName = name;
    result.success = true;
    result.totalReward = 100.0;  // Placeholder
    result.averageFiringRate = 0.5;  // Placeholder
    
    // Simulate some work
    for (size_t i = 0; i < 100; ++i) {
        if (paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    return result;
}

std::vector<ExperimentResult> ExperimentScheduler::executeBatch(const std::vector<std::string>& names,
                                                                 std::shared_ptr<Brain> brain,
                                                                 std::shared_ptr<Environment> environment,
                                                                 bool parallel) {
    if (parallel) {
        // Execute in parallel
        std::vector<std::future<ExperimentResult>> futures;
        
        for (const auto& name : names) {
            futures.push_back(std::async(std::launch::async, [this, name, brain, environment]() {
                ExperimentSchedule schedule(name);
                return executeSingle(name, brain, environment, schedule);
            }));
        }
        
        std::vector<ExperimentResult> results;
        for (auto& future : futures) {
            results.push_back(future.get());
        }
        
        return results;
    } else {
        // Execute sequentially
        std::vector<ExperimentResult> results;
        
        for (const auto& name : names) {
            ExperimentSchedule schedule(name);
            results.push_back(executeSingle(name, brain, environment, schedule));
        }
        
        return results;
    }
}

std::vector<std::string> ExperimentScheduler::getExecutionOrder() const {
    std::vector<std::string> order;
    
    switch (mode) {
        case ScheduleMode::Sequential:
            for (const auto& pair : schedules) {
                order.push_back(pair.first);
            }
            break;
            
        case ScheduleMode::Priority:
            {
                std::vector<std::pair<int, std::string>> priorityList;
                for (const auto& pair : schedules) {
                    priorityList.push_back(std::make_pair(pair.second.priority, pair.first));
                }
                std::sort(priorityList.begin(), priorityList.end(),
                         [](const auto& a, const auto& b) { return a.first > b.first; });
                for (const auto& pair : priorityList) {
                    order.push_back(pair.second);
                }
            }
            break;
            
        case ScheduleMode::RoundRobin:
            // Simplified round-robin
            for (size_t i = 0; i < schedules.size(); ++i) {
                auto it = schedules.begin();
                std::advance(it, i % schedules.size());
                order.push_back(it->first);
            }
            break;
            
        case ScheduleMode::Deadline:
            {
                std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> deadlineList;
                for (const auto& pair : schedules) {
                    deadlineList.push_back(std::make_pair(pair.second.deadline, pair.first));
                }
                std::sort(deadlineList.begin(), deadlineList.end(),
                         [](const auto& a, const auto& b) { return a.first < b.first; });
                for (const auto& pair : deadlineList) {
                    order.push_back(pair.second);
                }
            }
            break;
    }
    
    return order;
}

std::map<std::string, std::chrono::system_clock::time_point> ExperimentScheduler::calculateTimeline() const {
    std::map<std::string, std::chrono::system_clock::time_point> timeline;
    
    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    
    for (const auto& pair : schedules) {
        timeline[pair.first] = currentTime;
        // Add some time for execution
        currentTime += std::chrono::seconds(30);
    }
    
    return timeline;
}

void ExperimentScheduler::pause() {
    std::lock_guard<std::mutex> lock(mutex);
    paused = true;
}

void ExperimentScheduler::resume() {
    std::lock_guard<std::mutex> lock(mutex);
    paused = false;
    cv.notify_all();
}

void ExperimentScheduler::stop() {
    std::lock_guard<std::mutex> lock(mutex);
    running = false;
    paused = false;
    cv.notify_all();
}

} // namespace nlm
