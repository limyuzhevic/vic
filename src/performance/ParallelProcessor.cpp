#include "ParallelProcessor.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>

namespace nlm {

ParallelProcessor::ParallelProcessor(size_t threadCount)
    : numThreads_(threadCount)
    , maxThreadCount_(threadCount)
    , completedTasks_(0)
    , stopFlag_(false)
    , totalTaskTime_(0.0)
    , taskTimeCount_(0)
    , workStealingEnabled_(true)
    , taskAffinitySet_(false)
    , cacheLineSize_(64)
{
    // Create worker threads
    workers_.reserve(numThreads_);
    for (size_t i = 0; i < numThreads_; ++i) {
        workers_.emplace_back([this, i]() {
            threadFunction(i);
        });
    }
    
    // Setup thread affinity if supported
    setupThreadAffinity();
}

ParallelProcessor::~ParallelProcessor() {
    // Signal threads to stop
    stopFlag_.store(true, std::memory_order_release);
    
    // Notify all threads
    {
        std::lock_guard<std::mutex> lock(taskMutex_);
        taskReady_.notify_all();
    }
    
    // Wait for threads to finish
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ParallelProcessor::threadFunction(size_t threadId) {
    std::vector<Task> localTasks;
    localTasks.reserve(TASK_BATCH_SIZE);
    
    while (!stopFlag_.load(std::memory_order_acquire)) {
        Task task;
        
        // Try to steal work from other threads' queues
        if (workStealingEnabled_) {
            size_t sourceThread = (threadId + 1) % numThreads_;
            {
                std::lock_guard<std::mutex> lock(taskMutex_);
                if (!threadTaskQueues_[sourceThread].empty()) {
                    task = threadTaskQueues_[sourceThread].front();
                    threadTaskQueues_[sourceThread].pop();
                }
            }
        }
        
        if (!task.function) {
            // No work to steal, get own work
            {
                std::unique_lock<std::mutex> lock(taskMutex_);
                taskReady_.wait(lock, [this] { 
                    return stopFlag_.load(std::memory_order_acquire) || !taskQueue_.empty(); 
                });
                
                if (stopFlag_.load(std::memory_order_acquire) && taskQueue_.empty()) {
                    break;
                }
                
                if (!taskQueue_.empty()) {
                    task = taskQueue_.front();
                    taskQueue_.pop();
                }
            }
        }
        
        if (task.function) {
            // Execute task
            auto startTime = std::chrono::high_resolution_clock::now();
            task.function();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            totalTaskTime_ += duration;
            taskTimeCount_++;
            completedTasks_.fetch_add(1, std::memory_order_relaxed);
            
            // Signal completion
            {
                std::lock_guard<std::mutex> lock(completionMutex_);
                completedTasks_.fetch_add(1, std::memory_order_relaxed);
                taskCompleted_.notify_one();
            }
        }
    }
}

void ParallelProcessor::setupThreadAffinity() {
    if (!taskAffinitySet_) {
        // Setup thread affinity for better cache performance
        // This is platform-specific code
        taskAffinitySet_ = true;
    }
}

void ParallelProcessor::setWorkStealing(bool enable) {
    workStealingEnabled_ = enable;
    if (!enable) {
        // Merge thread-local queues into main queue
        std::lock_guard<std::mutex> lock(taskMutex_);
        for (size_t i = 0; i < numThreads_; ++i) {
            while (!threadTaskQueues_[i].empty()) {
                taskQueue_.push(threadTaskQueues_[i].front());
                threadTaskQueues_[i].pop();
            }
        }
    }
}

void ParallelProcessor::setThreadAffinity(bool enable) {
    taskAffinitySet_ = enable;
    if (enable) {
        setupThreadAffinity();
    }
}

void ParallelProcessor::setTaskBatchSize(size_t batchSize) {
    TASK_BATCH_SIZE = batchSize;
}

void ParallelProcessor::setTaskPriority(bool highPriority) {
    highPriorityTasks_ = highPriority;
}

template<typename Function>
void ParallelProcessor::submitTask(Function&& task, size_t neuronIndex) {
    std::lock_guard<std::mutex> lock(taskMutex_);
    
    ParallelTask parallelTask;
    parallelTask.function = std::forward<Function>(task);
    parallelTask.neuronIndex = neuronIndex;
    parallelTask.timestamp = std::chrono::high_resolution_clock::now();
    parallelTask.priority = highPriorityTasks_ ? TaskPriority::High : TaskPriority::Normal;
    
    taskQueue_.push(parallelTask);
    taskReady_.notify_one();
}

void ParallelProcessor::processNeuronBatch(std::vector<Neuron*>& neurons, 
                                          SimulationStep step, 
                                          Timestamp time) {
    if (neurons.empty()) return;
    
    // Batch processing for neurons
    size_t batchSize = neurons.size();
    if (batchSize <= BATCH_SIZE_THRESHOLD) {
        // Small batch - process directly
        for (auto* neuron : neurons) {
            neuron->stepLIF(time, timestep_);
        }
    } else {
        // Large batch - use parallel processing
        submitTask([neurons, step, time]() {
            for (auto* neuron : neurons) {
                neuron->stepLIF(time, timestep_);
            }
        }, 0); // Use index 0 for batch tasks
    }
}

void ParallelProcessor::processSpikeBatch(std::vector<Neuron*>& spikes,
                                         SimulationStep step, 
                                         Timestamp time) {
    if (spikes.empty()) return;
    
    // Batch processing for spike handling
    size_t batchSize = spikes.size();
    if (batchSize <= SPIKE_BATCH_THRESHOLD) {
        // Small batch - process directly
        for (auto* neuron : spikes) {
            // Process spike
            auto incoming = neuron->getIncomingSynapses();
            for (auto& syn : incoming) {
                // Deliver synaptic input
                if (syn->getType() == SynapseType::Excitatory) {
                    neuron->receiveExcitatoryInput(syn->getWeight() * 10.0f);
                } else {
                    neuron->receiveInhibitoryInput(syn->getWeight() * 10.0f);
                }
            }
        }
    } else {
        // Large batch - use parallel processing
        submitTask([spikes, step, time]() {
            for (auto* neuron : spikes) {
                auto incoming = neuron->getIncomingSynapses();
                for (auto& syn : incoming) {
                    if (syn->getType() == SynapseType::Excitatory) {
                        neuron->receiveExcitatoryInput(syn->getWeight() * 10.0f);
                    } else {
                        neuron->receiveInhibitoryInput(syn->getWeight() * 10.0f);
                    }
                }
            }
        }, 1); // Use index 1 for spike batch tasks
    }
}

void ParallelProcessor::processPlasticityBatch(std::vector<Synapse*>& synapses,
                                              SimulationStep step, 
                                              Timestamp time) {
    if (synapses.empty()) return;
    
    // Batch processing for plasticity
    size_t batchSize = synapses.size();
    if (batchSize <= PLASTICITY_BATCH_THRESHOLD) {
        // Small batch - process directly
        for (auto* synapse : synapses) {
            // Apply plasticity
            synapse->step(time);
        }
    } else {
        // Large batch - use parallel processing
        submitTask([synapses, step, time]() {
            for (auto* synapse : synapses) {
                synapse->step(time);
            }
        }, 2); // Use index 2 for plasticity batch tasks
    }
}

void ParallelProcessor::waitForCompletion() {
    std::unique_lock<std::mutex> lock(completionMutex_);
    taskCompleted_.wait(lock, [this] { 
        return completedTasks_.load(std::memory_order_acquire) >= lastCompletedCount_; 
    });
}

void ParallelProcessor::barrier() {
    std::lock_guard<std::mutex> lock(completionMutex_);
    barrierCount_++;
    
    if (barrierCount_ >= numThreads_) {
        barrierCount_ = 0;
        taskCompleted_.notify_all();
    } else {
        taskCompleted_.wait(lock, [this] { return barrierCount_ == 0; });
    }
}

void ParallelProcessor::setTimestep(TimestepDuration timestep) {
    timestep_ = timestep;
}

void ParallelProcessor::setBatchSizeThresholds(size_t neuronThreshold, 
                                               size_t spikeThreshold, 
                                               size_t plasticityThreshold) {
    BATCH_SIZE_THRESHOLD = neuronThreshold;
    SPIKE_BATCH_THRESHOLD = spikeThreshold;
    PLASTICITY_BATCH_THRESHOLD = plasticityThreshold;
}

void ParallelProcessor::enableDynamicScheduling(bool enable) {
    highPriorityTasks_ = enable;
}

size_t ParallelProcessor::getCompletedTasks() const {
    return completedTasks_.load(std::memory_order_acquire);
}

double ParallelProcessor::getAverageTaskTime() const {
    if (taskTimeCount_ == 0) return 0.0;
    return totalTaskTime_ / taskTimeCount_;
}

size_t ParallelProcessor::getActiveThreads() const {
    size_t active = 0;
    for (size_t i = 0; i < numThreads_; ++i) {
        if (!threadTaskQueues_[i].empty()) {
            active++;
        }
    }
    return active + (taskQueue_.size() > 0 ? 1 : 0);
}

void ParallelProcessor::reportPerformance() {
    double avgTaskTime = getAverageTaskTime();
    size_t activeThreads = getActiveThreads();
    
    std::string report = "ParallelProcessor Performance:\n";
    report += "Active threads: " + std::to_string(activeThreads) + "\n";
    report += "Completed tasks: " + std::to_string(getCompletedTasks()) + "\n";
    report += "Average task time: " + std::to_string(avgTaskTime) + " ms\n";
    report += "Queue size: " + std::to_string(taskQueue_.size()) + "\n";
    report += "Work stealing enabled: " + std::to_string(workStealingEnabled_) + "\n";
    
    NLM_LOG_INFO(report);
}

} // namespace nlm