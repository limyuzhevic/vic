#pragma once

#include "ExecutionEngine.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>

namespace nlm {

class ExecutionEngine::Impl {
public:
    struct Execution {
        ExecutionContext context;
        std::atomic<ExecutionStatus> status;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        std::string threadId;
    };
    
    ExecutionEngine() : nextExecutionId_(0), running_(false) {
        startBackgroundThread();
    }
    
    ~ExecutionEngine() {
        stopBackgroundThread();
    }
    
    bool execute(const ExecutionContext& context) {
        // Execute synchronously
        Execution execution;
        execution.context = context;
        execution.status = ExecutionStatus::Running;
        execution.startTime = std::chrono::steady_clock::now();
        execution.endTime = execution.startTime;
        
        executions_[context.pluginName] = execution;
        
        // In a real implementation, this would actually execute the plugin
        // For now, just simulate execution
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        execution.status = ExecutionStatus::Completed;
        execution.endTime = std::chrono::steady_clock::now();
        
        NLM_LOG_INFO(std::string("Execution completed: ") + context.pluginName + 
                    " operation: " + context.operation);
        
        return true;
    }
    
    bool executeAsync(const ExecutionContext& context) {
        // In a real implementation, this would execute asynchronously
        // For now, just queue for background execution
        std::lock_guard<std::mutex> lock(queueMutex_);
        executionQueue_.push(context);
        
        NLM_LOG_INFO(std::string("Execution queued: ") + context.pluginName + 
                    " operation: " + context.operation);
        
        return true;
    }
    
    ExecutionStatus getExecutionStatus(const std::string& executionId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (executions_.count(executionId) == 0) {
            return ExecutionStatus::Cancelled;
        }
        
        return executions_.at(executionId).status.load();
    }
    
    bool cancelExecution(const std::string& executionId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (executions_.count(executionId) == 0) {
            return false;
        }
        
        executions_[executionId].status = ExecutionStatus::Cancelled;
        return true;
    }
    
    std::vector<ExecutionContext> getExecutionHistory(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ExecutionContext> history;
        
        for (const auto& pair : executions_) {
            if (pair.first == pluginName) {
                history.push_back(pair.second.context);
            }
        }
        
        return history;
    }
    
private:
    void startBackgroundThread() {
        running_ = true;
        backgroundThread_ = std::thread(&ExecutionEngine::Impl::backgroundWorker, this);
    }
    
    void stopBackgroundThread() {
        running_ = false;
        
        // Wake up the background thread
        std::lock_guard<std::mutex> lock(queueMutex_);
        queueCondition_.notify_all();
        
        if (backgroundThread_.joinable()) {
            backgroundThread_.join();
        }
    }
    
    void backgroundWorker() {
        while (running_) {
            ExecutionContext context;
            
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                queueCondition_.wait_for(lock, std::chrono::milliseconds(100), 
                    [this]() { return !executionQueue_.empty(); });
                
                if (!executionQueue_.empty()) {
                    context = executionQueue_.front();
                    executionQueue_.pop();
                }
            }
            
            if (!context.pluginName.empty()) {
                execute(context);
            }
        }
    }
    
    // Member variables
    mutable std::mutex mutex_;
    mutable std::mutex queueMutex_;
    std::atomic<int> nextExecutionId_;
    bool running_;
    std::thread backgroundThread_;
    std::condition_variable queueCondition_;
    std::queue<ExecutionContext> executionQueue_;
    std::unordered_map<std::string, Execution> executions_;
};

} // namespace nlm
