#include "ParallelProcessing.hpp"
#include <thread>
#include <future>
#include <mutex>

namespace nlm {

ParallelNeuralProcessor::ParallelNeuralProcessor(size_t threadCount)
    : threadCount_(threadCount)
    , maxBatchSize_(1024)
    , shutdownRequested_(false)
    , taskCount_(0)
    , completedTasks_(0)
{
    for (size_t i = 0; i < threadCount_; ++i) {
        threads_.emplace_back(&ParallelNeuralProcessor::workerThread, this);
    }
}

ParallelNeuralProcessor::~ParallelNeuralProcessor() {
    shutdownRequested_ = true;
    condition_.notify_all();
    
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ParallelNeuralProcessor::processNeurons(std::vector<Neuron*>& neurons, 
                                           const std::vector<float>& inputs,
                                           const std::vector<float>& weights,
                                           std::vector<float>& outputs,
                                           double timestep) {
    if (neurons.empty()) return;
    
    size_t numTasks = (neurons.size() + maxBatchSize_ - 1) / maxBatchSize_;
    taskCount_ = numTasks;
    completedTasks_ = 0;
    
    // Distribute work among threads
    for (size_t taskId = 0; taskId < numTasks; ++taskId) {
        size_t begin = taskId * maxBatchSize_;
        size_t end = std::min(begin + maxBatchSize_, neurons.size());
        
        taskQueue_.push({neurons, inputs, weights, outputs, timestep, begin, end});
    }
    
    // Wait for all tasks to complete
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this]() { return completedTasks_ == taskCount_; });
}

void ParallelNeuralProcessor::workerThread() {
    while (!shutdownRequested_) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (taskQueue_.empty()) {
                condition_.wait_for(lock, std::chrono::milliseconds(10), 
                                   [this]() { return shutdownRequested_ || !taskQueue_.empty(); });
                continue;
            }
            task = taskQueue_.front();
            taskQueue_.pop();
        }
        
        // Process the task
        for (size_t i = task.begin; i < task.end; ++i) {
            // Simple neuron processing (placeholder)
            float& output = task.outputs[task.begin + (i - task.begin)];
            output = task.inputs[i] * task.weights[i];
        }
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            completedTasks_++;
        }
        
        condition_.notify_one();
    }
}

void ParallelNeuralProcessor::setBatchSize(size_t size) {
    maxBatchSize_ = size;
}

void ParallelNeuralProcessor::setThreadCount(size_t count) {
    if (count == threadCount_) return;
    
    // Shutdown current threads
    shutdownRequested_ = true;
    condition_.notify_all();
    
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Start new threads with updated count
    threadCount_ = count;
    shutdownRequested_ = false;
    
    threads_.clear();
    for (size_t i = 0; i < threadCount_; ++i) {
        threads_.emplace_back(&ParallelNeuralProcessor::workerThread, this);
    }
}

} // namespace nlm