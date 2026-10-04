#include "ThreadSafeSimulationManager.hpp"
#include <thread>
#include <queue>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

// ThreadPool implementation
ThreadPool::ThreadPool(size_t numThreads) : stop_(false) {
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
    }
    
    for (size_t i = 0; i < numThreads; ++i) {
        threads_.emplace_back([this] { workerThread(); });
    }
    
    NLM_LOG_INFO("ThreadPool created with " + std::to_string(numThreads) + " threads");
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::workerThread() {
    while (true) {
        std::unique_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] { 
                return stop_ || !tasks_.empty(); 
            });
            
            if (stop_ && tasks_.empty()) {
                return;
            }
            
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        
        try {
            (*task)();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("ThreadPool task exception: " + std::string(e.what()));
        }
    }
}

// ThreadSafeAllocator implementation
ThreadSafeAllocator::ThreadSafeAllocator(size_t initialPoolSize) 
    : totalAllocated_(0), peakUsage_(0), trackingEnabled_(true), alignment_(64) {
    
    memoryPool_.reserve(initialPoolSize);
    NLM_LOG_INFO("ThreadSafeAllocator created with " + std::to_string(initialPoolSize) + " initial blocks");
}

ThreadSafeAllocator::~ThreadSafeAllocator() {
    // Deallocate all remaining memory blocks
    for (auto& block : memoryPool_) {
        if (block.ptr) {
            std::free(block.ptr);
        }
    }
}

void* ThreadSafeAllocator::allocate(size_t size) {
    return allocateAligned(size, alignment_);
}

void* ThreadSafeAllocator::deallocate(void* ptr, size_t size) {
    return deallocateAligned(ptr, size, alignment_);
}

void* ThreadSafeAllocator::allocateAligned(size_t size, size_t alignment) {
    if (size == 0) {
        return nullptr;
    }
    
    if (!ValidationUtils::validateMemoryAllocation(size, "ThreadSafeAllocator::allocate")) {
        return nullptr;
    }
    
    void* ptr = std::aligned_alloc(alignment, size);
    if (!ptr) {
        NLM_LOG_ERROR("Memory allocation failed for size: " + std::to_string(size));
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(allocatorMutex_);
    
    memoryPool_.emplace_back(ptr, size);
    totalAllocated_ += size;
    
    if (totalAllocated_ > peakUsage_) {
        peakUsage_ = totalAllocated_;
    }
    
    NLM_LOG_DEBUG("Allocated " + std::to_string(size) + " bytes at " + std::to_string(reinterpret_cast<uintptr_t>(ptr)));
    
    return ptr;
}

void ThreadSafeAllocator::deallocateAligned(void* ptr, size_t size, size_t alignment) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(allocatorMutex_);
    
    // Find the block
    auto it = std::find_if(memoryPool_.begin(), memoryPool_.end(),
                          [ptr](const MemoryBlock& block) { return block.ptr == ptr; });n
    
    if (it != memoryPool_.end()) {
        // Deallocate memory
        std::free(ptr);
        
        // Remove from pool
        totalAllocated_ -= it->size;
        memoryPool_.erase(it);
        
        NLM_LOG_DEBUG("Deallocated " + std::to_string(size) + " bytes at " + std::to_string(reinterpret_cast<uintptr_t>(ptr)));
    } else {
        NLM_LOG_WARNING("Attempting to deallocate unknown pointer: " + std::to_string(reinterpret_cast<uintptr_t>(ptr)));
        std::free(ptr); // Still free it to avoid leaks
    }
}

size_t ThreadSafeAllocator::calculateFragmentationRatio() const {
    if (memoryPool_.empty()) {
        return 0;
    }
    
    // Simple fragmentation calculation based on pool size vs allocated size
    size_t totalAllocated = 0;
    for (const auto& block : memoryPool_) {
        totalAllocated += block.size;
    }
    
    // If we have fragmentation, pool size > allocated size
    return (memoryPool_.size() > (totalAllocated / 1024)) ? 1 : 0; // Simplified
}

// ThreadSafeSimulationManager implementation
ThreadSafeSimulationManager::ThreadSafeSimulationManager(std::shared_ptr<ThreadPool> threadPool,
                                                         std::shared_ptr<ThreadSafeAllocator> allocator)
    : running_(false), threadPool_(threadPool), allocator_(allocator) {
    
    if (!threadPool_) {
        threadPool_ = std::make_shared<ThreadPool>();
    }
    
    if (!allocator_) {
        allocator_ = std::make_shared<ThreadSafeAllocator>();
    }
    
    NLM_LOG_INFO("ThreadSafeSimulationManager initialized");
}

ThreadSafeSimulationManager::~ThreadSafeSimulationManager() {
    stopSimulation();
}

void ThreadSafeSimulationManager::startSimulation(std::function<void()> simulationStep) {
    if (running_) {
        NLM_LOG_WARNING("Simulation already running");
        return;
    }
    
    running_ = true;
    
    // Submit simulation step as an operation
    addNeuralOperation([this, simulationStep]() {
        if (!running_) return;
        
        try {
            simulationStep();
            updateStats();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Simulation step exception: " + std::string(e.what()));
        }
    });
    
    NLM_LOG_INFO("ThreadSafeSimulationManager simulation started");
}

void ThreadSafeSimulationManager::stopSimulation() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Wait for thread pool to complete tasks
    if (threadPool_) {
        threadPool_->waitForCompletion();
    }
    
    clearOperations();
    NLM_LOG_INFO("ThreadSafeSimulationManager simulation stopped");
}

void ThreadSafeSimulationManager::executeOperations() {
    if (!running_) {
        return;
    }
    
    std::vector<std::function<void()>> operations;
    {
        std::lock_guard<std::mutex> lock(operationMutex_);
        operations = std::move(operations_);
    }
    
    for (auto& operation : operations) {
        if (running_) {
            if (threadPool_) {
                threadPool_->execute(operation);
            } else {
                // Execute synchronously if no thread pool
                try {
                    operation();
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Operation execution exception: " + std::string(e.what()));
                }
            }
        }
    }
}

void ThreadSafeSimulationManager::updateStats() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stats_.lastUpdate).count();
    
    if (elapsed > 1000) { // Update stats every second
        // Calculate simulation speed (approximate)
        stats_.simulationSpeed = operations_.size() / (elapsed / 1000.0);
        
        // Update last update time
        stats_.lastUpdate = now;
    }
}

double ThreadSafeSimulationManager::calculateCPULoad() const {
    // Simplified CPU load calculation
    return 0.0; // TODO: Implement actual CPU monitoring
}

// ThreadSafeNeuralMemory implementation
ThreadSafeNeuralMemory::ThreadSafeNeuralMemory(size_t maxMemoryMB) 
    : memoryLimitMB_(maxMemoryMB), autoManagementEnabled_(true), 
      peakMemoryUsageMB_(0), currentMemoryUsageMB_(0) {
    
    NLM_LOG_INFO("ThreadSafeNeuralMemory initialized with " + std::to_string(maxMemoryMB) + " MB limit");
}

ThreadSafeNeuralMemory::~ThreadSafeNeuralMemory() {
    // Cleanup will be handled by destructor
}

bool ThreadSafeNeuralMemory::allocateNeuralMemory(size_t neuronCount, size_t synapseCount) {
    // Calculate required memory (rough estimate)
    size_t memoryNeededMB = (neuronCount * 64 + synapseCount * 32) / (1024 * 1024);
    
    if (!isMemoryAllocationSafe(neuronCount, synapseCount)) {
        NLM_LOG_ERROR("Memory allocation unsafe: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(synapseCount) + " synapses");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    memoryBlocks_.emplace_back(neuronCount, synapseCount);
    currentMemoryUsageMB_ += memoryNeededMB;
    
    if (currentMemoryUsageMB_ > peakMemoryUsageMB_) {
        peakMemoryUsageMB_ = currentMemoryUsageMB_;
    }
    
    NLM_LOG_INFO("Allocated neural memory: " + std::to_string(neuronCount) + " neurons, " +
                std::to_string(synapseCount) + " synapses (" + std::to_string(currentMemoryUsageMB_) + " MB)");
    
    return true;
}

void ThreadSafeNeuralMemory::freeNeuralMemory(size_t neuronCount, size_t synapseCount) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    // Find and remove memory blocks
    for (auto it = memoryBlocks_.begin(); it != memoryBlocks_.end(); ++it) {
        if (it->neuronCount == neuronCount && it->synapseCount == synapseCount) {
            currentMemoryUsageMB_ -= (neuronCount * 64 + synapseCount * 32) / (1024 * 1024);
            memoryBlocks_.erase(it);
            NLM_LOG_INFO("Freed neural memory: " + std::to_string(neuronCount) + " neurons, " +
                        std::to_string(synapseCount) + " synapses");
            return;
        }
    }
}

ThreadSafeNeuralMemory::NeuralMemoryStats ThreadSafeNeuralMemory::getMemoryStats() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    size_t availableNeuronSlots = (memoryLimitMB_ * 1024 * 1024) / 64 - getTotalNeuronCount();
    size_t availableSynapseSlots = (memoryLimitMB_ * 1024 * 1024) / 32 - getTotalSynapseCount();
    
    return {
        availableNeuronSlots,
        availableSynapseSlots,
        currentMemoryUsageMB_,
        peakMemoryUsageMB_,
        checkMemoryPressure()
    };
}

bool ThreadSafeNeuralMemory::isMemoryAllocationSafe(size_t neuronCount, size_t synapseCount) const {
    size_t memoryNeededMB = (neuronCount * 64 + synapseCount * 32) / (1024 * 1024);
    
    // Check memory limit
    if (currentMemoryUsageMB_ + memoryNeededMB > memoryLimitMB_ * 1024 * 1024) {
        NLM_LOG_WARNING("Memory allocation would exceed limit: " + std::to_string(currentMemoryUsageMB_ + memoryNeededMB) +
                       " MB > " + std::to_string(memoryLimitMB_ * 1024 * 1024) + " MB");
        return false;
    }
    
    // Check pressure
    if (checkMemoryPressure()) {
        NLM_LOG_WARNING("Memory pressure detected, allocation may cause performance degradation");
    }
    
    return true;
}

void ThreadSafeNeuralMemory::compactMemory() {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    // Simple compaction: remove unused blocks
    auto it = std::remove_if(memoryBlocks_.begin(), memoryBlocks_.end(),
                             [](const NeuralMemoryBlock& block) { return !block.inUse; });
    memoryBlocks_.erase(it, memoryBlocks_.end());
    
    NLM_LOG_INFO("Memory compaction completed");
}

void ThreadSafeNeuralMemory::monitorMemoryUsage() {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    size_t oldUsage = currentMemoryUsageMB_;
    
    // Recalculate usage based on current blocks
    currentMemoryUsageMB_ = 0;
    for (const auto& block : memoryBlocks_) {
        if (block.inUse) {
            currentMemoryUsageMB_ += (block.neuronCount * 64 + block.synapseCount * 32) / (1024 * 1024);
        }
    }
    
    if (currentMemoryUsageMB_ > peakMemoryUsageMB_) {
        peakMemoryUsageMB_ = currentMemoryUsageMB_;
    }
    
    if (checkMemoryPressure()) {
        NLM_LOG_WARNING("Memory pressure detected: " + std::to_string(currentMemoryUsageMB_) + " MB used");
        triggerGarbageCollection();
    }
}

size_t ThreadSafeNeuralMemory::calculateMemoryUsage() const {
    size_t total = 0;
    for (const auto& block : memoryBlocks_) {
        if (block.inUse) {
            total += (block.neuronCount * 64 + block.synapseCount * 32) / (1024 * 1024);
        }
    }
    return total;
}

void ThreadSafeNeuralMemory::triggerGarbageCollection() {
    // Remove oldest unused blocks
    auto now = std::chrono::system_clock::now();
    auto it = std::remove_if(memoryBlocks_.begin(), memoryBlocks_.end(),
                             [this, now](const NeuralMemoryBlock& block) {
                                 return !block.inUse && 
                                        std::chrono::duration_cast<std::chrono::minutes>(now - block.allocatedTime).count() > 5;
                             });
    memoryBlocks_.erase(it, memoryBlocks_.end());
    
    NLM_LOG_INFO("Garbage collection completed");
}

bool ThreadSafeNeuralMemory::checkMemoryPressure() const {
    if (currentMemoryUsageMB_ > memoryLimitMB_ * 0.9) {
        return true; // 90% or more memory usage indicates pressure
    }
    return false;
}

size_t ThreadSafeNeuralMemory::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& block : memoryBlocks_) {
        if (block.inUse) {
            total += block.neuronCount;
        }
    }
    return total;
}

size_t ThreadSafeNeuralMemory::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& block : memoryBlocks_) {
        if (block.inUse) {
            total += block.synapseCount;
        }
    }
    return total;
}

} // namespace nlm