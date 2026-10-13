#pragma once

#include "BrainInitializer.hpp"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace nlm {

class Brain;

class BrainThreadSafetyManager {
public:
    explicit BrainThreadSafetyManager(std::shared_ptr<Config> config);
    ~BrainThreadSafetyManager() = default;
    
    // Thread safety for brain operations
    void acquireStepLock();
    void releaseStepLock();
    bool tryAcquireStepLock();
    
    // Thread safety for memory operations
    void acquireMemoryLock();
    void releaseMemoryLock();
    bool tryAcquireMemoryLock();
    
    // Thread safety for configuration
    void acquireConfigLock();
    void releaseConfigLock();
    bool tryAcquireConfigLock();
    
    // Thread safety for checkpoint operations
    void acquireCheckpointLock();
    void releaseCheckpointLock();
    bool tryAcquireCheckpointLock();
    
    // Synchronization primitives
    void waitForStepCompletion();
    void notifyStepCompleted();
    void waitForInitialization();
    void notifyInitializationComplete();
    
    // Thread state management
    bool isStepInProgress() const;
    bool isInitialized() const;
    bool isLocked() const;
    
    // Thread safety annotations for compilation
    template<typename T>
    class ThreadSafePtr {
    public:
        explicit ThreadSafePtr(T* ptr = nullptr) : ptr_(ptr), locked_(false) {}
        ~ThreadSafePtr() = default;
        
        T* get() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return ptr_;
        }
        
        void set(T* ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            ptr_ = ptr;
        }
        
        T& operator*() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return *ptr_;
        }
        
        T* operator->() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return ptr_;
        }
        
        bool try_lock() {
            return mutex_.try_lock();
        }
        
    private:
        T* ptr_;
        mutable std::mutex mutex_;
        mutable bool locked_;
    };
    
    // Thread safety for different operations
    class ThreadSafeStepExecutor {
    public:
        ThreadSafeStepExecutor(Brain* brain, BrainThreadSafetyManager* threadSafety);
        ~ThreadSafeStepExecutor();
        
        void executeStep(SimulationStep step, Timestamp time);
        bool tryExecuteStep(SimulationStep step, Timestamp time);
        
    private:
        Brain* brain_;
        BrainThreadSafetyManager* threadSafety_;
    };
    
    class ThreadSafeMemoryAccessor {
    public:
        ThreadSafeMemoryAccessor(Brain* brain, BrainThreadSafetyManager* threadSafety);
        ~ThreadSafeMemoryAccessor();
        
        NeuralWorkingMemory* getWorkingMemory();
        NeuralEpisodicMemory* getEpisodicMemory();
        NeuralAssociativeMemory* getAssociativeMemory();
        
    private:
        Brain* brain_;
        BrainThreadSafetyManager* threadSafety_;
    };
    
    // Configuration and advanced features
    void configureThreadPoolSize(Brain* brain, size_t threadPoolSize);
    void enableParallelProcessing(Brain* brain, bool enabled);
    void enableLockFreeAlgorithms(Brain* brain, bool enabled);
    
    // Performance optimization
    void optimizeForSingleThread(Brain* brain);
    void optimizeForMultiThread(Brain* brain);
    void balanceThreadUsage(Brain* brain);
    
    // Error handling for thread safety
    std::string getThreadSafetyError() const { return threadSafetyError; }
    void clearThreadSafetyError() { threadSafetyError.clear(); }
    
    // Debugging and monitoring
    void logThreadSafetyStatus(Brain* brain) const;
    void monitorDeadlockPotential(Brain* brain);
    void detectResourceContention(Brain* brain);

private:
    // Synchronization primitives
    std::mutex stepMutex_;
    std::mutex memoryMutex_;
    std::mutex configMutex_;
    std::mutex checkpointMutex_;
    
    std::condition_variable stepCV_;
    std::condition_variable initCV_;
    
    // Atomic flags for thread safety
    std::atomic<bool> stepInProgress_;
    std::atomic<bool> initialized_;
    std::atomic<size_t> activeReaders_;
    std::atomic<size_t> activeWriters_;
    
    // Thread configuration
    size_t threadPoolSize_;
    bool parallelProcessingEnabled_;
    bool lockFreeAlgorithmsEnabled_;
    
    // Deadlock detection
    std::chrono::steady_clock::time_point lastLockTime_;
    std::chrono::milliseconds maxWaitTime_;
    
    // Error tracking
    std::string threadSafetyError_;
    
    // Helper methods
    void initializeSynchronizationPrimitives();
    void setupThreadPool(Brain* brain);
    void configureDeadlockDetection(Brain* brain);
    void setupLockFreeDataStructures(Brain* brain);
    
    // Timeout and retry logic
    bool waitWithTimeout(std::mutex& mutex, std::chrono::milliseconds timeout);
    void handleLockContention(Brain* brain);
    void recoverFromDeadlock(Brain* brain);
    
    // Performance monitoring
    void monitorThreadPerformance(Brain* brain);
    void analyzeLockContention(Brain* brain);
    void optimizeLockGranularity(Brain* brain);
};