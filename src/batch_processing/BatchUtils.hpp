#pragma once

#include "../performance/ParallelProcessing.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <future>
#include <condition_variable>

namespace nlm {

// Batch progress tracking
struct BatchProgress {
    std::atomic<size_t> currentTask{0};
    std::atomic<size_t> totalTasks{0};
    std::atomic<size_t> completedTasks{0};
    std::atomic<size_t> failedTasks{0};
    std::mutex progressMutex;
    std::condition_variable progressCV;
    
    BatchProgress() = default;
    BatchProgress(size_t total) : totalTasks(total) {}
    
    float getProgress() const {
        if (totalTasks == 0) return 1.0f;
        return static_cast<float>(completedTasks) / totalTasks;
    }
    
    std::string getStatusString() const {
        std::lock_guard<std::mutex> lock(progressMutex);
        return "Progress: " + std::to_string(getProgress() * 100) + "% (" +
               std::to_string(completedTasks) + "/" + std::to_string(totalTasks) +
               ") Current: " + std::to_string(currentTask);
    }
};

// Batch simulation result
struct BatchSimulationResult {
    size_t batchIndex;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Environment> environment;
    std::string configName;
    double executionTime;
    std::string status;
    std::map<std::string, double> metrics;
    std::string errorMessage;
    
    BatchSimulationResult(size_t index, std::shared_ptr<Brain> b, std::shared_ptr<Environment> e,
                         const std::string& name, double time, const std::string& status = "completed")
        : batchIndex(index), brain(std::move(b)), environment(std::move(e)),
          configName(name), executionTime(time), status(status) {}
};nlm