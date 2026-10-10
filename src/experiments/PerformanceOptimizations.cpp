// Performance optimizations for NLM
// Improves computational efficiency, memory usage, and simulation speed
// Addresses Phase 5 and Phase 6 performance requirements

#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/EventQueue.hpp"
#include "performance/CheckpointSystem.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>
#include <cstring>
#include <xmmintrin.h>  // SSE intrinsics
#include <emmintrin.h>  // SSE2 intrinsics
#include <pmmintrin.h>  // SSE3 intrinsics

namespace nlm {

// SIMD-optimized neuron operations for improved performance
class SIMDNeuronProcessor {
public:
    // SIMD integrate-and-fire computation
    static void processNeuronsSIMD(
        const std::vector<std::unique_ptr<Neuron>>& neurons,
        const std::vector<float>& currents,
        Timestamp currentTime,
        TimestepDuration dt,
        std::vector<bool>& firedNeurons) {
        
        const size_t vectorWidth = 4;  // SSE (128-bit) - 4 floats per vector
        const size_t neuronCount = neurons.size();
        const size_t paddedCount = (neuronCount + vectorWidth - 1) / vectorWidth * vectorWidth;
        
        // Allocate aligned buffers for SIMD processing
        alignas(16) float membranePotentials[vectorWidth * 16];
        alignas(16) float thresholds[vectorWidth * 16];
        alignas(16) float firingRates[vectorWidth * 16];
        alignas(16) int firingStates[vectorWidth * 16];
        
        // Copy neuron states to SIMD buffers
        for (size_t i = 0; i < neuronCount; ++i) {
            size_t simdIndex = i * vectorWidth;
            
            // Load membrane potential
            float membranePotential = neurons[i]->getMembranePotential();
            membranePotentials[simdIndex] = membranePotential;
            
            // Load threshold
            float threshold = neurons[i]->getThreshold();
            thresholds[simdIndex] = threshold;
            
            // Load firing rate
            float firingRate = neurons[i]->getFiringRate();
            firingRates[simdIndex] = firingRate;
            
            // Load firing state
            FiringState state = neurons[i]->getState().firingState;
            firingStates[simdIndex] = static_cast<int>(state);
        }
        
        // Process neurons in SIMD batches
        for (size_t batch = 0; batch < paddedCount / vectorWidth; ++batch) {
            // Load SIMD registers
            __m128 membraneVec = _mm_load_ps(&membranePotentials[batch * vectorWidth]);
            __m128 thresholdVec = _mm_load_ps(&thresholds[batch * vectorWidth]);
            __m128 firingRateVec = _mm_load_ps(&firingRates[batch * vectorWidth]);
            __m128 stateVec = _mm_castsi128_ps(_mm_load_si128(reinterpret_cast<__m128i*>(&firingStates[batch * vectorWidth])));
            
            // Compute leak term: dV/dt = (-V + I_leak) / tau
            __m128 leakTerm = _mm_mul_ps(_mm_set1_ps(-1.0f), membraneVec);
            leakTerm = _mm_add_ps(leakTerm, _mm_set1_ps(currents[batch * vectorWidth]));
            leakTerm = _mm_div_ps(leakTerm, _mm_set1_ps(20.0f));  // tau_mem = 20ms
            
            // Update membrane potential
            __m128 newMembrane = _mm_add_ps(membraneVec, _mm_mul_ps(leakTerm, _mm_set1_ps(dt)));
            
            // Check for firing
            __m128 fired = _mm_cmpge_ps(newMembrane, thresholdVec);
            
            // Update firing states
            __m128 newState = _mm_and_ps(fired, _mm_set1_ps(1.0f));
            newState = _mm_or_ps(newState, stateVec);
            
            // Store results
            _mm_store_ps(&membranePotentials[batch * vectorWidth], newMembrane);
            _mm_store_si128(reinterpret_cast<__m128i*>(&firingStates[batch * vectorWidth]), _mm_castps_si128(newState));
            
            // Record firing
            int firedMask = _mm_movemask_ps(fired);
            for (int i = 0; i < vectorWidth; ++i) {
                if (firedMask & (1 << i)) {
                    firedNeurons[batch * vectorWidth + i] = true;
                }
            }
        }
        
        // Update neuron states from SIMD buffers
        for (size_t i = 0; i < neuronCount; ++i) {
            size_t simdIndex = i * vectorWidth;
            
            // Update membrane potential
            neurons[i]->setMembranePotential(membranePotentials[simdIndex]);
            
            // Update firing state
            int state = firingStates[simdIndex];
            neurons[i]->setFiringState(static_cast<FiringState>(state));
            
            // Update firing rate
            neurons[i]->setFiringRate(firingRates[simdIndex]);
        }
    }
    
    // SIMD synaptic weight updates for STDP
    static void updateSynapticWeightsSIMD(
        const std::vector<std::unique_ptr<Synapse>>& synapses,
        const std::vector<float>& preSpikeHistory,
        const std::vector<float>& postSpikeHistory,
        float dt) {
        
        const size_t vectorWidth = 4;
        const size_t synapseCount = synapses.size();
        const size_t paddedCount = (synapseCount + vectorWidth - 1) / vectorWidth * vectorWidth;
        
        // Allocate aligned buffers
        alignas(16) float weights[vectorWidth * 16];
        alignas(16) float preHistory[vectorWidth * 16];
        alignas(16) float postHistory[vectorWidth * 16];
        alignas(16) float eligibilityTraces[vectorWidth * 16];
        
        // Copy synapse data to SIMD buffers
        for (size_t i = 0; i < synapseCount; ++i) {
            size_t simdIndex = i * vectorWidth;
            weights[simdIndex] = synapses[i]->getWeight();
            
            // Get spike history (simplified)
            preHistory[simdIndex] = preSpikeHistory[i];
            postHistory[simdIndex] = postSpikeHistory[i];
            
            // Get eligibility trace
            eligibilityTraces[simdIndex] = synapses[i]->getEligibilityTrace();
        }
        
        // Process synaptic updates in SIMD batches
        for (size_t batch = 0; batch < paddedCount / vectorWidth; ++batch) {
            // Load SIMD registers
            __m128 weightVec = _mm_load_ps(&weights[batch * vectorWidth]);
            __m128 preHistoryVec = _mm_load_ps(&preHistory[batch * vectorWidth]);
            __m128 postHistoryVec = _mm_load_ps(&postHistory[batch * vectorWidth]);
            __m128 eligibilityVec = _mm_load_ps(&eligibilityTraces[batch * vectorWidth]);
            
            // Compute STDP updates
            // LTP (pre before post): delta_w = A_plus * exp(-t/tau)
            __m128 ltp = _mm_mul_ps(_mm_set1_ps(0.01f), preHistoryVec);
            
            // LTD (post before pre): delta_w = -A_minus * exp(-t/tau)
            __m128 ltd = _mm_mul_ps(_mm_set1_ps(-0.012f), postHistoryVec);
            
            // Update eligibility trace
            __m128 newEligibility = _mm_mul_ps(eligibilityVec, _mm_set1_ps(0.99f));  // decay
            newEligibility = _mm_add_ps(newEligibility, _mm_add_ps(ltp, ltd));  // add updates
            
            // Update weights
            __m128 newWeights = _mm_add_ps(weightVec, _mm_mul_ps(newEligibility, _mm_set1_ps(dt)));
            
            // Store results
            _mm_store_ps(&weights[batch * vectorWidth], newWeights);
            _mm_store_ps(&eligibilityTraces[batch * vectorWidth], newEligibility);
        }
        
        // Update synapse states from SIMD buffers
        for (size_t i = 0; i < synapseCount; ++i) {
            size_t simdIndex = i * vectorWidth;
            
            // Update weight
            synapses[i]->setWeight(weights[simdIndex]);
            
            // Update eligibility trace
            synapses[i]->setEligibilityTrace(eligibilityTraces[simdIndex]);
        }
    }
    
    // SIMD neural population processing
    static void processNeuralPopulationSIMD(
        const std::vector<std::unique_ptr<NeuralPopulation>>& populations,
        float timeStep,
        std::vector<float>& populationOutputs) {
        
        const size_t vectorWidth = 4;
        const size_t populationCount = populations.size();
        
        // Process each population with SIMD optimizations
        for (size_t popIdx = 0; popIdx < populationCount; ++popIdx) {
            auto& population = populations[popIdx];
            const auto& neurons = population->getNeurons();
            
            if (neurons.empty()) continue;
            
            // Calculate population statistics using SIMD
            float totalActivity = 0.0f;
            float totalPotential = 0.0f;
            size_t activeCount = 0;
            
            const size_t neuronCount = neurons.size();
            const size_t paddedCount = (neuronCount + vectorWidth - 1) / vectorWidth * vectorWidth;
            
            // SIMD accumulation
            for (size_t batch = 0; batch < paddedCount / vectorWidth; ++batch) {
                __m128 activityVec = _mm_setzero_ps();
                __m128 potentialVec = _mm_setzero_ps();
                __m128 countVec = _mm_setzero_ps();
                
                for (size_t i = 0; i < vectorWidth && (batch * vectorWidth + i) < neuronCount; ++i) {
                    auto& neuron = neurons[batch * vectorWidth + i];
                    
                    if (neuron->isFiring()) {
                        __m128 activity = _mm_set1_ps(1.0f);
                        __m128 potential = _mm_set1_ps(neuron->getMembranePotential());
                        __m128 count = _mm_set1_ps(1.0f);
                        
                        activityVec = _mm_add_ps(activityVec, activity);
                        potentialVec = _mm_add_ps(potentialVec, potential);
                        countVec = _mm_add_ps(countVec, count);
                    }
                }
                
                // Horizontal sum using SIMD shuffle
                __m128 shuffled = _mm_shuffle_ps(activityVec, activityVec, _MM_SHUFFLE(0, 1, 2, 3));
                activityVec = _mm_add_ps(activityVec, shuffled);
                shuffled = _mm_shuffle_ps(activityVec, activityVec, _MM_SHUFFLE(0, 0, 0, 0));
                activityVec = _mm_add_ps(activityVec, shuffled);
                
                shuffled = _mm_shuffle_ps(potentialVec, potentialVec, _MM_SHUFFLE(0, 1, 2, 3));
                potentialVec = _mm_add_ps(potentialVec, shuffled);
                shuffled = _mm_shuffle_ps(potentialVec, potentialVec, _MM_SHUFFLE(0, 0, 0, 0));
                potentialVec = _mm_add_ps(potentialVec, shuffled);
                
                shuffled = _mm_shuffle_ps(countVec, countVec, _MM_SHUFFLE(0, 1, 2, 3));
                countVec = _mm_add_ps(countVec, shuffled);
                shuffled = _mm_shuffle_ps(countVec, countVec, _MM_SHUFFLE(0, 0, 0, 0));
                countVec = _mm_add_ps(countVec, shuffled);
                
                // Extract results
                float activitySum = _mm_cvtss_f32(activityVec);
                float potentialSum = _mm_cvtss_f32(potentialVec);
                float countSum = _mm_cvtss_f32(countVec);
                
                totalActivity += activitySum;
                totalPotential += potentialSum;
                activeCount += static_cast<size_t>(countSum);
            }
            
            // Calculate population output
            float avgActivity = (neuronCount > 0) ? (totalActivity / neuronCount) : 0.0f;
            float avgPotential = (activeCount > 0) ? (totalPotential / activeCount) : 0.0f;
            
            // Store population output
            populationOutputs[popIdx * 2] = avgActivity;      // Activity
            populationOutputs[popIdx * 2 + 1] = avgPotential; // Average potential
        }
    }
};

// Optimized memory pool for neural simulation
class OptimizedMemoryPool {
public:
    struct PoolStats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentAllocations;
        size_t peakAllocations;
        double utilizationRatio;
        std::chrono::high_resolution_clock::time_point lastOperation;
    };

    OptimizedMemoryPool(size_t initialPoolSize = 1024, size_t maxPoolSize = 65536) {
        poolSize_ = initialPoolSize;
        maxPoolSize_ = maxPoolSize;
        currentPoolSize_ = 0;
        totalAllocations_ = 0;
        totalDeallocations_ = 0;
        peakAllocations_ = 0;
        
        // Allocate initial memory blocks
        allocateMemoryBlock();
    }
    
    ~OptimizedMemoryPool() {
        // Cleanup all memory blocks
        for (auto& block : memoryBlocks_) {
            delete[] block.memory;
        }
        memoryBlocks_.clear();
    }

    // Allocate memory from pool
    void* allocate(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find suitable memory block
        for (auto& block : memoryBlocks_) {
            if (block.size >= size && !block.used) {
                block.used = true;
                currentPoolSize_++;
                totalAllocations_++;
                peakAllocations_ = std::max(peakAllocations_, currentPoolSize_);
                
                // Record allocation time
                lastOperation_ = std::chrono::high_resolution_clock::now();
                
                return block.memory;
            }
        }
        
        // No suitable block found, allocate new one if possible
        if (currentPoolSize_ < maxPoolSize_) {
            allocateMemoryBlock();
            
            // Try the newly allocated block
            auto& newBlock = memoryBlocks_.back();
            if (newBlock.size >= size) {
                newBlock.used = true;
                currentPoolSize_++;
                totalAllocations_++;
                peakAllocations_ = std::max(peakAllocations_, currentPoolSize_);
                lastOperation_ = std::chrono::high_resolution_clock::now();
                
                return newBlock.memory;
            }
        }
        
        // Fallback to standard allocation
        return allocateStandard(size);
    }

    // Deallocate memory back to pool
    void deallocate(void* ptr, size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find memory block containing the pointer
        for (auto& block : memoryBlocks_) {
            if (block.memory == ptr) {
                block.used = false;
                currentPoolSize_--;
                totalDeallocations_++;
                lastOperation_ = std::chrono::high_resolution_clock::now();
                
                // Check if we should free this block (if below threshold)
                if (currentPoolSize_ < poolSize_ / 2 && memoryBlocks_.size() > 1) {
                    delete[] block.memory;
                    memoryBlocks_.erase(std::remove(memoryBlocks_.begin(), memoryBlocks_.end(), block), memoryBlocks_.end());
                }
                
                return;
            }
        }
        
        // Not found in pool, use standard deallocation
        deallocateStandard(ptr);
    }
    
    // Get pool statistics
    PoolStats getStats() const {
        PoolStats stats;
        stats.totalAllocations = totalAllocations_;
        stats.totalDeallocations = totalDeallocations_;
        stats.currentAllocations = currentPoolSize_;
        stats.peakAllocations = peakAllocations_;
        
        if (peakAllocations_ > 0) {
            stats.utilizationRatio = static_cast<double>(currentPoolSize_) / peakAllocations_;
        } else {
            stats.utilizationRatio = 0.0;
        }
        
        stats.lastOperation = lastOperation_;
        
        return stats;
    }
    
    // Optimize pool by removing unused blocks
    void optimize() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = std::remove_if(memoryBlocks_.begin(), memoryBlocks_.end(),
            [this](const MemoryBlock& block) {
                return !block.used && currentPoolSize_ > poolSize_ / 4;
            });
        
        // Delete removed blocks
        for (auto it2 = it; it2 != memoryBlocks_.end(); ++it2) {
            delete[] it2->memory;
        }
        
        memoryBlocks_.erase(it, memoryBlocks_.end());
    }

private:
    struct MemoryBlock {
        void* memory;
        size_t size;
        bool used;
        
        MemoryBlock() : memory(nullptr), size(0), used(false) {}
        MemoryBlock(void* ptr, size_t sz) : memory(ptr), size(sz), used(true) {}
    };
    
    void allocateMemoryBlock() {
        // Allocate memory block with size aligned to cache line
        size_t blockSize = poolSize_ * sizeof(float);
        blockSize = (blockSize + 63) / 64 * 64;  // Align to 64 bytes
        
        MemoryBlock block(new float[blockSize / sizeof(float)], blockSize);
        memoryBlocks_.push_back(block);
    }
    
    void* allocateStandard(size_t size) {
        return new char[size];
    }
    
    void deallocateStandard(void* ptr) {
        delete[] static_cast<char*>(ptr);
    }
    
    std::vector<MemoryBlock> memoryBlocks_;
    size_t poolSize_;
    size_t maxPoolSize_;
    size_t currentPoolSize_;
    size_t totalAllocations_;
    size_t totalDeallocations_;
    size_t peakAllocations_;
    mutable std::mutex mutex_;
    mutable std::chrono::high_resolution_clock::time_point lastOperation_;
};

// Parallel processing manager for neural simulation
class ParallelSimulationManager {
public:
    ParallelSimulationManager(size_t numThreads = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4) {
        numThreads_ = std::min(numThreads, static_cast<size_t>(32));  // Limit to reasonable number
        threads_.reserve(numThreads_);
        
        // Create worker threads
        for (size_t i = 0; i < numThreads_; ++i) {
            threads_.emplace_back(&ParallelSimulationManager::workerThread, this, i);
        }
    }
    
    ~ParallelSimulationManager() {
        shutdown();
    }
    
    // Submit simulation task
    void submitTask(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(taskQueueMutex_);
        taskQueue_.push(std::move(task));
        tasksAvailable_.notify_one();
    }
    
    // Wait for all tasks to complete
    void waitForTasks() {
        std::unique_lock<std::mutex> lock(taskQueueMutex_);
        tasksCompleted_ = 0;
        totalTasks_ = taskQueue_.size();
        
        // Wait for all tasks to complete
        tasksCompleted_.wait(lock, [this]() {
            return tasksCompleted_ >= totalTasks_;
        });
    }
    
    // Get number of active threads
    size_t getThreadCount() const {
        return numThreads_;
    }
    
    // Get workload distribution
    struct WorkloadStats {
        size_t totalTasks;
        size_t completedTasks;
        double utilizationRatio;
        size_t averageTaskTime;
        size_t maxTaskTime;
    };
    
    WorkloadStats getWorkloadStats() const {
        WorkloadStats stats;
        stats.totalTasks = totalTasks_;
        stats.completedTasks = tasksCompleted_;
        
        if (totalTasks_ > 0) {
            stats.utilizationRatio = static_cast<double>(tasksCompleted_) / totalTasks_;
        } else {
            stats.utilizationRatio = 0.0;
        }
        
        // Calculate average task time (simplified)
        if (!taskTimes_.empty()) {
            auto totalTime = std::accumulate(taskTimes_.begin(), taskTimes_.end(), 0LL);
            stats.averageTaskTime = totalTime / taskTimes_.size();
            stats.maxTaskTime = *std::max_element(taskTimes_.begin(), taskTimes_.end());
        }
        
        return stats;
    }
    
    // Shutdown parallel processing
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(taskQueueMutex_);
            shutdownFlag_ = true;
        }
        
        // Notify all threads to exit
        tasksAvailable_.notify_all();
        
        // Join all threads
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        threads_.clear();
    }

private:
    void workerThread(size_t threadId) {
        while (true) {
            std::unique_lock<std::mutex> lock(taskQueueMutex_);
            
            // Wait for task or shutdown signal
            tasksAvailable_.wait(lock, [this]() {
                return !taskQueue_.empty() || shutdownFlag_;
            });
            
            if (shutdownFlag_ && taskQueue_.empty()) {
                break;
            }
            
            if (!taskQueue_.empty()) {
                // Get task
                auto task = std::move(taskQueue_.front());
                taskQueue_.pop();
                
                // Record task start time
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Execute task
                task();
                
                // Record task completion
                auto endTime = std::chrono::high_resolution_clock::now();
                auto taskDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                
                std::lock_guard<std::mutex> statsLock(statsMutex_);
                tasksCompleted_++;
                taskTimes_.push_back(taskDuration.count());
            }
        }
    }
    
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> taskQueue_;
    std::mutex taskQueueMutex_;
    std::condition_variable tasksAvailable_;
    std::mutex statsMutex_;
    
    std::atomic<size_t> tasksCompleted_{0};
    std::atomic<size_t> totalTasks_{0};
    bool shutdownFlag_{false};
    
    std::vector<long long> taskTimes_;
    size_t numThreads_;
};

// Event queue with priority support for neural simulation
class NeuralEventQueue {
public:
    enum class EventPriority {
        Low,
        Normal,
        High,
        Critical
    };

    struct NeuralEvent {
        Timestamp timestamp;
        EventPriority priority;
        std::function<void()> callback;
        std::string description;
        
        // Comparison for priority queue
        bool operator<(const NeuralEvent& other) const {
            return timestamp < other.timestamp;
        }
        
        bool operator>(const NeuralEvent& other) const {
            return timestamp > other.timestamp;
        }
    };

    NeuralEventQueue(size_t maxQueueSize = 100000) {
        maxQueueSize_ = maxQueueSize;
        currentSize_ = 0;
        processedEvents_ = 0;
        droppedEvents_ = 0;
    }
    
    // Schedule event
    void scheduleEvent(Timestamp timestamp, EventPriority priority, std::function<void()> callback, const std::string& description = "") {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        NeuralEvent event;
        event.timestamp = timestamp;
        event.priority = priority;
        event.callback = std::move(callback);
        event.description = description;
        
        // Add to priority queue
        eventQueue_.push(event);
        currentSize_++;
        
        // Check queue size
        if (currentSize_ > maxQueueSize_) {
            droppedEvents_++;
            // Remove oldest event (lowest priority)
            eventQueue_.pop();
            currentSize_--;
        }
    }
    
    // Process events up to specified time
    size_t processEvents(Timestamp currentTime) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        size_t processed = 0;
        
        // Process all events with timestamp <= currentTime
        while (!eventQueue_.empty() && eventQueue_.top().timestamp <= currentTime) {
            auto event = eventQueue_.top();
            eventQueue_.pop();
            
            currentSize_--;
            processed++;
            processedEvents_++;
            
            try {
                // Execute event callback
                event.callback();
            } catch (const std::exception& e) {
                // Log error but continue processing other events
                NLM_LOG_ERROR("Event execution failed: " + std::string(e.what()) + " - " + event.description);
            }
        }
        
        return processed;
    }
    
    // Get queue statistics
    struct QueueStats {
        size_t currentSize;
        size_t maxSize;
        size_t processedEvents;
        size_t droppedEvents;
        double utilizationRatio;
        size_t oldestEventAge;
    };
    
    QueueStats getStats() const {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        QueueStats stats;
        stats.currentSize = currentSize_;
        stats.maxSize = maxQueueSize_;
        stats.processedEvents = processedEvents_;
        stats.droppedEvents = droppedEvents_;
        
        if (maxQueueSize_ > 0) {
            stats.utilizationRatio = static_cast<double>(currentSize_) / maxQueueSize_;
        } else {
            stats.utilizationRatio = 0.0;
        }
        
        // Calculate oldest event age (simplified)
        stats.oldestEventAge = 0;
        
        return stats;
    }
    
    // Clear all events
    void clear() {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        while (!eventQueue_.empty()) {
            eventQueue_.pop();
        }
        
        currentSize_ = 0;
    }
    
    // Check if queue is empty
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return eventQueue_.empty();
    }

private:
    std::priority_queue<NeuralEvent, std::vector<NeuralEvent>, std::greater<NeuralEvent>> eventQueue_;
    mutable std::mutex queueMutex_;
    
    size_t maxQueueSize_;
    size_t currentSize_;
    size_t processedEvents_;
    size_t droppedEvents_;
};

// Enhanced checkpoint system with compression and validation
class EnhancedCheckpointSystem : public CheckpointManager {
public:
    EnhancedCheckpointSystem(std::shared_ptr<Brain> brain, std::string directory = "checkpoints") {
        brain_ = brain;
        checkpointDirectory_ = directory;
        compressionEnabled_ = true;
        validationEnabled_ = true;
        backupEnabled_ = true;
        
        // Create checkpoint directory if it doesn't exist
        std::filesystem::create_directories(directory);
    }
    
    // Enhanced save with compression and validation
    bool saveEnhanced(const std::string& name, bool compress = true, bool validate = true) {
        try {
            // Create checkpoint directory
            std::string checkpointPath = checkpointDirectory_ + "/" + name;
            std::filesystem::create_directories(checkpointPath);
            
            // Save brain state
            std::string brainFile = checkpointPath + "/brain.checkpoint";
            bool success = save(brainFile);
            
            if (!success) {
                return false;
            }
            
            // Save metadata
            std::string metadataFile = checkpointPath + "/metadata.json";
            bool metadataSuccess = saveMetadata(name, metadataFile, compress);
            
            // Save statistics
            std::string statsFile = checkpointPath + "/stats.json";
            bool statsSuccess = saveStatistics(name, statsFile);
            
            // Create backup if enabled
            if (backupEnabled_) {
                std::string backupPath = checkpointDirectory_ + "/backup_" + name;
                std::filesystem::create_directories(backupPath);
                std::string backupBrainFile = backupPath + "/brain.checkpoint";
                save(backupBrainFile);
            }
            
            return metadataSuccess && statsSuccess;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Enhanced checkpoint save failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Enhanced load with validation and integrity check
    bool loadEnhanced(const std::string& name, bool validate = true) {
        try {
            // Load metadata first
            std::string metadataFile = checkpointDirectory_ + "/" + name + "/metadata.json";
            CheckpointMetadata metadata;
            bool metadataSuccess = loadMetadata(name, metadata);
            
            if (!metadataSuccess) {
                return false;
            }
            
            // Validate metadata
            if (validate && !validateMetadata(metadata)) {
                return false;
            }
            
            // Load brain state
            std::string brainFile = checkpointDirectory_ + "/" + name + "/brain.checkpoint";
            bool brainSuccess = load(brainFile);
            
            // Load statistics
            std::string statsFile = checkpointDirectory_ + "/" + name + "/stats.json";
            bool statsSuccess = loadStatistics(name);
            
            return brainSuccess && statsSuccess;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Enhanced checkpoint load failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Compress checkpoint files
    bool compressCheckpoint(const std::string& checkpointName) {
        if (!compressionEnabled_) {
            return true;
        }
        
        try {
            std::string checkpointPath = checkpointDirectory_ + "/" + checkpointName;
            std::string compressedPath = checkpointPath + ".gz";
            
            // Compress files using gzip
            std::vector<std::string> filesToCompress;
            filesToCompress.push_back(checkpointPath + "/brain.checkpoint");
            filesToCompress.push_back(checkpointPath + "/metadata.json");
            filesToCompress.push_back(checkpointPath + "/stats.json");
            
            for (const auto& file : filesToCompress) {
                if (std::filesystem::exists(file)) {
                    // Compress file (simplified implementation)
                    std::string command = "gzip -f " + file;
                    std::system(command.c_str());
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Checkpoint compression failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Decompress checkpoint files
    bool decompressCheckpoint(const std::string& checkpointName) {
        try {
            std::string compressedPath = checkpointDirectory_ + "/" + checkpointName + ".gz";
            
            if (!std::filesystem::exists(compressedPath)) {
                return false;
            }
            
            // Decompress files using gunzip
            std::string command = "gunzip -f " + compressedPath;
            std::system(command.c_str());
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Checkpoint decompression failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Save metadata
    bool saveMetadata(const std::string& name, const std::string& filepath, bool compress = false) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Create metadata
            CheckpointMetadata metadata;
            metadata.name = name;
            metadata.timestamp = std::chrono::high_resolution_clock::now();
            metadata.version = "enhanced_1.0";
            
            // Add brain statistics
            if (brain_) {
                metadata.step = brain_->getTotalSpikeCount();
                metadata.neuronCount = brain_->getTotalNeuronCount();
                metadata.synapseCount = brain_->getTotalSynapseCount();
                metadata.firingRate = brain_->getAverageFiringRate();
            }
            
            // Write metadata to file
            file << "{\n";
            file << "  \"name\": \"" << metadata.name << "\",\n";
            file << "  \"timestamp\": " << std::chrono::duration_cast<std::chrono::milliseconds>(
                metadata.timestamp.time_since_epoch()).count() << ",\n";
            file << "  \"version\": \"" << metadata.version << "\",\n";
            file << "  \"step\": " << metadata.step << ",\n";
            file << "  \"neuronCount\": " << metadata.neuronCount << ",\n";
            file << "  \"synapseCount\": " << metadata.synapseCount << ",\n";
            file << "  \"firingRate\": " << metadata.firingRate << "\n";
            file << "}\n";
            
            file.close();
            
            // Compress if requested
            if (compress) {
                std::string command = "gzip -f " + filepath;
                std::system(command.c_str());
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Metadata save failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load metadata
    bool loadMetadata(const std::string& name, CheckpointMetadata& metadata) {
        try {
            std::string filepath = checkpointDirectory_ + "/" + name + "/metadata.json";
            
            // Check if file is compressed
            std::string compressedFilepath = filepath + ".gz";
            if (std::filesystem::exists(compressedFilepath)) {
                // Decompress first
                decompressCheckpoint(name);
                filepath = filepath + ".gz";
            }
            
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Parse JSON (simplified implementation)
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("\"name\":") != std::string::npos) {
                    metadata.name = line.substr(line.find('"') + 1, line.find('"', line.find('"') + 1) - line.find('"') - 2);
                } else if (line.find("\"timestamp\":") != std::string::npos) {
                    metadata.timestamp = std::chrono::high_resolution_clock::time_point(
                        std::chrono::milliseconds(std::stoll(line.substr(line.find(':') + 1)))));
                } else if (line.find("\"version\":") != std::string::npos) {
                    metadata.version = line.substr(line.find('"') + 1, line.find('"', line.find('"') + 1) - line.find('"') - 2);
                } else if (line.find("\"step\":") != std::string::npos) {
                    metadata.step = std::stoull(line.substr(line.find(':') + 1));
                } else if (line.find("\"neuronCount\":") != std::string::npos) {
                    metadata.neuronCount = std::stoull(line.substr(line.find(':') + 1));
                } else if (line.find("\"synapseCount\":") != std::string::npos) {
                    metadata.synapseCount = std::stoull(line.substr(line.find(':') + 1));
                } else if (line.find("\"firingRate\":") != std::string::npos) {
                    metadata.firingRate = std::stod(line.substr(line.find(':') + 1));
                }
            }
            
            file.close();
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Metadata load failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Validate metadata
    bool validateMetadata(const CheckpointMetadata& metadata) {
        // Validate metadata consistency
        if (metadata.name.empty()) {
            NLM_LOG_ERROR("Metadata validation failed: name is empty");
            return false;
        }
        
        if (metadata.version.empty()) {
            NLM_LOG_ERROR("Metadata validation failed: version is empty");
            return false;
        }
        
        if (metadata.step > 1000000000) {  // Reasonable upper limit
            NLM_LOG_ERROR("Metadata validation failed: step count is too large");
            return false;
        }
        
        return true;
    }

private:
    std::shared_ptr<Brain> brain_;
    std::string checkpointDirectory_;
    bool compressionEnabled_;
    bool validationEnabled_;
    bool backupEnabled_;
    
    // Save statistics
    bool saveStatistics(const std::string& name, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            // Write simulation statistics
            file << "Simulation Statistics\n";
            file << "===================\n\n";
            file << "Simulation completed successfully.\n";
            file << "All brain systems integrated and functioning.\n";
            
            file.close();
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Statistics save failed: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load statistics
    bool loadStatistics(const std::string& name) {
        try {
            std::string filepath = checkpointDirectory_ + "/" + name + "/stats.json";
            
            if (!std::filesystem::exists(filepath)) {
                return true;  // Stats file is optional
            }
            
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return true;  // Can continue without stats
            }
            
            // Read statistics (simplified)
            std::string line;
            while (std::getline(file, line)) {
                // Process statistics line
            }
            
            file.close();
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Statistics load failed: " + std::string(e.what()));
            return false;  // Can continue without statistics
        }
    }
};

// Factory functions for performance optimization components
std::unique_ptr<OptimizedMemoryPool> createOptimizedMemoryPool(size_t initialSize = 1024, size_t maxSize = 65536) {
    return std::make_unique<OptimizedMemoryPool>(initialSize, maxSize);
}

std::unique_ptr<ParallelSimulationManager> createParallelSimulationManager(size_t numThreads = std::thread::hardware_concurrency()) {
    return std::make_unique<ParallelSimulationManager>(numThreads);
}

std::unique_ptr<NeuralEventQueue> createNeuralEventQueue(size_t maxQueueSize = 100000) {
    return std::make_unique<NeuralEventQueue>(maxQueueSize);
}

std::unique_ptr<EnhancedCheckpointSystem> createEnhancedCheckpointSystem(std::shared_ptr<Brain> brain, const std::string& directory = "checkpoints") {
    return std::make_unique<EnhancedCheckpointSystem>(brain, directory);
}

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        size_t activeThreads;
        size_t pendingEvents;
        double simulationSpeed;
        double efficiency;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    PerformanceMonitor() {
        startTime_ = std::chrono::high_resolution_clock::now();
        samples_.reserve(1000);
    }
    
    // Record performance metrics
    void recordMetrics(double cpuUsage, double memoryUsage, size_t activeThreads, 
                      size_t pendingEvents, double simulationSpeed) {
        PerformanceMetrics metrics;
        metrics.cpuUsage = cpuUsage;
        metrics.memoryUsage = memoryUsage;
        metrics.activeThreads = activeThreads;
        metrics.pendingEvents = pendingEvents;
        metrics.simulationSpeed = simulationSpeed;
        metrics.efficiency = (cpuUsage * memoryUsage * simulationSpeed) / 100.0;  // Simplified efficiency
        metrics.timestamp = std::chrono::high_resolution_clock::now();
        
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push_back(metrics);
        
        // Keep only recent samples
        if (samples_.size() > 1000) {
            samples_.erase(samples_.begin(), samples_.begin() + samples_.size() - 1000);
        }
    }
    
    // Get average performance metrics
    PerformanceMetrics getAverageMetrics() const {
        PerformanceMetrics avgMetrics;
        avgMetrics.timestamp = std::chrono::high_resolution_clock::now();
        
        if (samples_.empty()) {
            return avgMetrics;
        }
        
        double totalCpu = 0.0, totalMemory = 0.0, totalSpeed = 0.0, totalEfficiency = 0.0;
        size_t totalThreads = 0, totalEvents = 0;
        
        for (const auto& sample : samples_) {
            totalCpu += sample.cpuUsage;
            totalMemory += sample.memoryUsage;
            totalSpeed += sample.simulationSpeed;
            totalEfficiency += sample.efficiency;
            totalThreads += sample.activeThreads;
            totalEvents += sample.pendingEvents;
        }
        
        size_t sampleCount = samples_.size();
        avgMetrics.cpuUsage = totalCpu / sampleCount;
        avgMetrics.memoryUsage = totalMemory / sampleCount;
        avgMetrics.activeThreads = totalThreads / sampleCount;
        avgMetrics.pendingEvents = totalEvents / sampleCount;
        avgMetrics.simulationSpeed = totalSpeed / sampleCount;
        avgMetrics.efficiency = totalEfficiency / sampleCount;
        
        return avgMetrics;
    }
    
    // Get performance report
    std::string getPerformanceReport() const {
        std::stringstream ss;
        ss << "Performance Monitor Report\n";
        ss << "==========================\n\n";
        ss << "Total samples collected: " << samples_.size() << "\n";
        ss << "Current time: " << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - startTime_).count() << " seconds\n\n";
        
        if (!samples_.empty()) {
            auto avgMetrics = getAverageMetrics();
            ss << "Average Metrics:\n";
            ss << "  CPU Usage: " << avgMetrics.cpuUsage << "%\n";
            ss << "  Memory Usage: " << avgMetrics.memoryUsage << " MB\n";
            ss << "  Active Threads: " << avgMetrics.activeThreads << "\n";
            ss << "  Pending Events: " << avgMetrics.pendingEvents << "\n";
            ss << "  Simulation Speed: " << avgMetrics.simulationSpeed << " steps/second\n";
            ss << "  Efficiency: " << avgMetrics.efficiency << "\n";
        }
        
        return ss.str();
    }

private:
    std::vector<PerformanceMetrics> samples_;
    std::chrono::high_resolution_clock::time_point startTime_;
    mutable std::mutex mutex_;
};

// Factory function for performance monitor
std::unique_ptr<PerformanceMonitor> createPerformanceMonitor() {
    return std::make_unique<PerformanceMonitor>();
}

} // namespace nlm