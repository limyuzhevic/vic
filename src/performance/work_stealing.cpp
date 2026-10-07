// WorkStealingThreadPool implementation
template<typename Func, typename... Args>
auto WorkStealingThreadPool::submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, Args...>::type> {
    using ReturnType = typename std::invoke_result<Func, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        [func, args...]() -> ReturnType {
            return func(args...);
        }
    );
    
    std::future<ReturnType> future = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    workAvailable_.notify_one();
    totalTasksSubmitted_.fetch_add(1, std::memory_order_relaxed);
    
    return future;
}

std::future<void> WorkStealingThreadPool::submitVoid(std::function<void()>&& func) {
    auto task = std::make_shared<std::packaged_task<void()>>(std::move(func));
    
    std::future<void> future = task->get_future();
    
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.emplace([task]() { (*task)(); });
    }
    
    workAvailable_.notify_one();
    totalTasksSubmitted_.fetch_add(1, std::memory_order_relaxed);
    
    return future;
}

void WorkStealingThreadPool::submitBatch(const std::vector<std::function<void()>>& tasks) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    for (const auto& task : tasks) {
        taskQueue_.emplace(task);
    }
    workAvailable_.notify_all();
    totalTasksSubmitted_.fetch_add(tasks.size(), std::memory_order_relaxed);
}

void WorkStealingThreadPool::wait() {
    std::unique_lock<std::mutex> lock(queueMutex_);
    workAvailable_.wait(lock, [this] { return taskQueue_.empty(); });
}

size_t WorkStealingThreadPool::getThreadCount() const {
    std::lock_guard<std::mutex> lock(workersMutex_);
    return workers_.size();
}

size_t WorkStealingThreadPool::getIdleThreadCount() const {
    size_t idleCount = 0;
    std::lock_guard<std::mutex> lock(workersMutex_);
    for (const auto& worker : workers_) {
        if (worker.isIdle()) {
            idleCount++;
        }
    }
    return idleCount;
}

size_t WorkStealingThreadPool::getQueueSize() const {
    std::lock_guard<std::mutex> lock(queueMutex_);
    return taskQueue_.size();
}

WorkStealingThreadPool::PoolStats WorkStealingThreadPool::getPoolStats() const {
    PoolStats stats;
    
    stats.threadCount = getThreadCount();
    stats.idleThreads = getIdleThreadCount();
    stats.queueSize = getQueueSize();
    stats.totalTasksSubmitted = totalTasksSubmitted_.load(std::memory_order_relaxed);
    stats.totalTasksCompleted = totalTasksCompleted_.load(std::memory_order_relaxed);
    stats.totalExecutionTime = totalExecutionTime_.load(std::memory_order_relaxed);
    
    if (stats.totalTasksCompleted > 0) {
        stats.averageTaskTimeNs = (double)stats.totalExecutionTime / (double)stats.totalTasksCompleted;
    }
    
    if (stats.threadCount > 0) {
        stats.throughputPerThread = (double)stats.totalTasksCompleted / (double)stats.threadCount;
    }
    
    return stats;
}

void WorkStealingThreadPool::healthCheck() {
    if (!config_.enableHealthMonitoring) return;
    
    PoolStats stats = getPoolStats();
    
    // Check if threads are idle for too long
    if (stats.idleThreads >= stats.threadCount * 0.8) {
        // Scale down threads
        scaleThreads(0.3);  // Target 30% utilization
    } else if (stats.throughputPerThread < 100.0) {
        // Scale up threads if throughput is low
        scaleThreads(0.8);  // Target 80% utilization
    }
}

void WorkStealingThreadPool::scaleThreads(double targetUtilization) {
    std::lock_guard<std::mutex> lock(workersMutex_);
    
    size_t currentThreads = workers_.size();
    size_t optimalThreads = static_cast<size_t>(currentThreads * targetUtilization);
    
    if (optimalThreads > config_.maxThreads) {
        optimalThreads = config_.maxThreads;
    } else if (optimalThreads < 1) {
        optimalThreads = 1;
    }
    
    if (optimalThreads > currentThreads) {
        // Add more threads
        for (size_t i = currentThreads; i < optimalThreads; ++i) {
            WorkerThread worker;
            worker.start(i);
            workers_.push_back(std::move(worker));
        }
    } else if (optimalThreads < currentThreads) {
        // Remove idle threads
        std::vector<WorkerThread> activeWorkers;
        for (auto& worker : workers_) {
            if (!worker.isIdle() || activeWorkers.size() < optimalThreads) {
                activeWorkers.push_back(std::move(worker));
            }
        }
        workers_ = std::move(activeWorkers);
        
        // Shutdown excess workers
        for (auto& worker : workers_) {
            if (worker.threadId >= optimalThreads) {
                worker.shutdown();
            }
        }
    }
}

void WorkStealingThreadPool::shutdown() {
    shutdown_ = true;
    workAvailable_.notify_all();
    
    std::lock_guard<std::mutex> lock(workersMutex_);
    for (auto& worker : workers_) {
        worker.shutdown();
    }
}

std::vector<size_t> WorkStealingThreadPool::getOptimalThreadAffinities(size_t numThreads, bool enableNUMA) {
    std::vector<size_t> affinities;
    
    if (enableNUMA && numa_available() >= 0) {
        // NUMA-aware thread placement
        int numNodes = numa_num_task_nodes();
        if (numNodes > 1) {
            for (size_t i = 0; i < numThreads; ++i) {
                int node = i % numNodes;
                affinities.push_back(1 << node);  // CPU mask
            }
        } else {
            // No NUMA support, use all CPUs
            for (size_t i = 0; i < numThreads; ++i) {
                affinities.push_back(~0ULL);
            }
        }
    } else {
        // Simple affinity: assign each thread to a specific CPU
        for (size_t i = 0; i < numThreads; ++i) {
            affinities.push_back(1ULL << i);
        }
    }
    
    return affinities;
}

// WorkStealingNeuralProcessor implementation
WorkStealingNeuralProcessor::WorkStealingNeuralProcessor(const ProcessingConfig& config)
    : config_(config), threadPool_(WorkStealingThreadPool::PoolConfig()) {
    threadLocalStates_.resize(config.numThreads);
    threadPool_.startWorkers(config.numThreads);
}

WorkStealingNeuralProcessor::~WorkStealingNeuralProcessor() {
    threadPool_.shutdown();
}

void WorkStealingNeuralProcessor::processNeurons(size_t numNeurons,
                                                std::function<void(size_t, ProcessingUnit&)> processFunc,
                                                std::function<void()>* syncFunc) {
    if (numNeurons == 0) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Distribute work across threads
    size_t workPerThread = numNeurons / threadPool_.getThreadCount();
    size_t remaining = numNeurons % threadPool_.getThreadCount();
    
    std::vector<std::future<void>> futures;
    
    for (size_t i = 0; i < threadPool_.getThreadCount(); ++i) {
        size_t threadStart = i * workPerThread + std::min(i, remaining);
        size_t threadEnd = threadStart + workPerThread + (i < remaining ? 1 : 0);
        
        if (threadStart < threadEnd) {
            futures.push_back(threadPool_.submit([&, threadStart, threadEnd, &processFunc]() {
                ProcessingUnit& localState = threadLocalStates_[std::hash<std::thread::id>{}(std::this_thread::get_id()) % threadLocalStates_.size()];
                
                for (size_t idx = threadStart; idx < threadEnd; ++idx) {
                    processFunc(idx, localState);
                }
            }));
        }
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    // Call synchronization function if provided
    if (syncFunc) {
        (*syncFunc)();
    }
    
    // Update statistics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    
    totalNeuronsProcessed_ += numNeurons;
    totalProcessingTime_ += duration.count();
}

void WorkStealingNeuralProcessor::processNeuronsSIMD(size_t numNeurons,
                                                   std::function<void(size_t, size_t, ProcessingUnit&)> processFunc,
                                                   std::function<void()>* syncFunc) {
    // SIMD-optimized processing with batch operations
    if (numNeurons == 0) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Use SIMD for vectorized processing
    size_t batchSize = config_.enableSIMD ? 16 : 1;  // SIMD width
    size_t numBatches = (numNeurons + batchSize - 1) / batchSize;
    
    std::vector<std::future<void>> futures;
    
    for (size_t i = 0; i < numBatches; ++i) {
        size_t batchStart = i * batchSize;
        size_t batchEnd = std::min(batchStart + batchSize, numNeurons);
        
        futures.push_back(threadPool_.submit([&, batchStart, batchEnd, &processFunc]() {
            ProcessingUnit& localState = threadLocalStates_[std::hash<std::thread::id>{}(std::this_thread::get_id()) % threadLocalStates_.size()];
            
            for (size_t idx = batchStart; idx < batchEnd; idx += batchSize) {
                processFunc(idx, batchEnd - idx, localState);
            }
        }));
    }
    
    // Wait for all SIMD tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    if (syncFunc) {
        (*syncFunc)();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    
    totalNeuronsProcessed_ += numNeurons;
    totalProcessingTime_ += duration.count();
}

void WorkStealingNeuralProcessor::processSynapses(size_t numSynapses,
                                                std::function<void(size_t, ProcessingUnit&)> processFunc) {
    if (numSynapses == 0) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Distribute synapse processing across threads
    size_t workPerThread = numSynapses / threadPool_.getThreadCount();
    size_t remaining = numSynapses % threadPool_.getThreadCount();
    
    std::vector<std::future<void>> futures;
    
    for (size_t i = 0; i < threadPool_.getThreadCount(); ++i) {
        size_t threadStart = i * workPerThread + std::min(i, remaining);
        size_t threadEnd = threadStart + workPerThread + (i < remaining ? 1 : 0);
        
        if (threadStart < threadEnd) {
            futures.push_back(threadPool_.submit([&, threadStart, threadEnd, &processFunc]() {
                ProcessingUnit& localState = threadLocalStates_[std::hash<std::thread::id>{}(std::this_thread::get_id()) % threadLocalStates_.size()];
                
                for (size_t idx = threadStart; idx < threadEnd; ++idx) {
                    processFunc(idx, localState);
                }
            }));
        }
    }
    
    // Wait for completion
    for (auto& future : futures) {
        future.wait();
    }
    
    totalSynapsesProcessed_ += numSynapses;
}

WorkStealingNeuralProcessor::ProcessingStats WorkStealingNeuralProcessor::getProcessingStats() const {
    ProcessingStats stats;
    
    stats.totalNeuronsProcessed = totalNeuronsProcessed_.load(std::memory_order_relaxed);
    stats.totalSynapsesProcessed = totalSynapsesProcessed_.load(std::memory_order_relaxed);
    
    // Estimate spikes from processing activity (simplified)
    stats.totalSpikesGenerated = stats.totalNeuronsProcessed * 0.05;  // 5% spike probability
    
    if (totalProcessingTime_ > 0) {
        stats.averageProcessingTimeNs = (double)totalProcessingTime_ / (double)(stats.totalNeuronsProcessed + 1);
    }
    
    if (totalNeuronsProcessed_ > 0) {
        stats.throughputNeuronsPerSecond = (double)stats.totalNeuronsProcessed * 1e9 / (double)totalProcessingTime_;
    }
    
    stats.activeThreads = activeThreads_.load(std::memory_order_relaxed);
    stats.idleThreads = idleThreads_.load(std::memory_order_relaxed);
    
    return stats;
}

WorkStealingThreadPool::PoolStats WorkStealingNeuralProcessor::getThreadPoolStats() const {
    return threadPool_.getPoolStats();
}

void WorkStealingNeuralProcessor::performLoadBalancing() {
    // Simple load balancing based on thread performance
    threadPool_.healthCheck();
    adjustThreadsBasedOnPerformance();
}

void WorkStealingNeuralProcessor::adjustThreadsBasedOnPerformance() {
    WorkStealingThreadPool::PoolStats poolStats = threadPool_.getPoolStats();
    
    // Adjust thread count based on CPU utilization and queue size
    if (poolStats.queueSize > poolStats.threadCount * 100) {
        // High queue size, add more threads
        threadPool_.scaleThreads(0.9);
    } else if (poolStats.throughputPerThread < 10.0) {
        // Low throughput, add more threads
        threadPool_.scaleThreads(0.8);
    } else if (poolStats.idleThreads > poolStats.threadCount * 0.5) {
        // Many idle threads, reduce thread count
        threadPool_.scaleThreads(0.3);
    }
}

} // namespace nlm
