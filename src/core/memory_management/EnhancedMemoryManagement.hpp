#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <atomic>
#include <functional>
#include <algorithm>
#include <stdexcept>

namespace nlm
{
// =============================================================================
// Smart Pointer Utilities for Safe Resource Management
// =============================================================================

// Custom deleter for automatic cleanup of arrays
template<typename T>
class ArrayDeleter {
public:
    explicit ArrayDeleter(size_t size = 1) : size_(size) {}
    void operator()(T* ptr) const {
        if (ptr) {
            std::allocator<T>().deallocate(ptr, size_);
        }
    }
private:
    size_t size_;
};

// Smart pointer for neural arrays with bounds checking
template<typename T>
class NeuralArray {
public:
    using ElementType = T;
    using PointerType = std::unique_ptr<T, ArrayDeleter<T>>;
    using ReferenceType = T&;
    using ConstReferenceType = const T&;

    explicit NeuralArray(size_t size = 0) : size_(size), data_(nullptr) {
        if (size_ > 0) {
            std::allocator<T> alloc;
            data_ = PointerType(alloc.allocate(size_), ArrayDeleter<T>(size_));
        }
    }

    NeuralArray(T* ptr, size_t size) : size_(size), data_(ptr) {
        static_cast<void>(data_); // Suppress unused warning
    }

    // Copy constructor with proper allocation
    NeuralArray(const NeuralArray& other) : size_(other.size_) {
        if (size_ > 0) {
            std::allocator<T> alloc;
            T* new_ptr = alloc.allocate(size_);
            std::uninitialized_copy(other.data_.get(), other.data_.get() + size_, new_ptr);
            data_ = PointerType(new_ptr, ArrayDeleter<T>(size_));
        }
    }

    // Move constructor
    NeuralArray(NeuralArray&& other) noexcept : size_(other.size_), data_(std::move(other.data_)) {
        other.size_ = 0;
    }

    // Copy assignment
    NeuralArray& operator=(const NeuralArray& other) {
        if (this != &other) {
            NeuralArray temp(other);
            std::swap(size_, temp.size_);
            std::swap(data_, temp.data_);
        }
        return *this;
    }

    // Move assignment
    NeuralArray& operator=(NeuralArray&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            size_ = other.size_;
            other.size_ = 0;
        }
        return *this;
    }

    ~NeuralArray() = default;

    // Element access with bounds checking
    T& operator[](size_t index) {
        #ifndef NDEBUG
        if (index >= size_) {
            throw std::out_of_range("NeuralArray index out of bounds");
        }
        #endif
        return data_[index];
    }

    const T& operator[](size_t index) const {
        #ifndef NDEBUG
        if (index >= size_) {
            throw std::out_of_range("NeuralArray index out of bounds");
        }
        #endif
        return data_[index];
    }

    // Iterator support
    T* begin() { return data_.get(); }
    T* end() { return data_.get() + size_; }
    const T* begin() const { return data_.get(); }
    const T* end() const { return data_.get() + size_; }

    // Capacity
    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    void resize(size_t newSize) {
        if (newSize == size_) return;
        if (newSize == 0) {
            data_.reset();
            size_ = 0;
            return;
        }

        std::allocator<T> alloc;
        T* new_ptr = alloc.allocate(newSize);
        size_t copyCount = std::min(size_, newSize);
        std::uninitialized_copy(data_.get(), data_.get() + copyCount, new_ptr);
        if (newSize > copyCount) {
            std::uninitialized_value_construct(new_ptr + copyCount, new_ptr + newSize);
        }
        data_ = PointerType(new_ptr, ArrayDeleter<T>(newSize));
        size_ = newSize;
    }

    // Memory pool for arrays (static)
    static void* allocateArray(size_t size) {
        if (size == 0) return nullptr;
        std::allocator<T> alloc;
        return alloc.allocate(size);
    }

    static void deallocateArray(void* ptr, size_t size) {
        if (!ptr) return;
        std::allocator<T> alloc;
        alloc.deallocate(static_cast<T*>(ptr), size);
    }

private:
    size_t size_;
    PointerType data_;
};

// =============================================================================
// Memory Pool for Neural Resources
// =============================================================================

template<typename T, size_t PoolSize = 1024>
class NeuralResourcePool {
public:
    using ResourceType = T;
    using PtrType = std::unique_ptr<ResourceType>;

    NeuralResourcePool() : freeList_(), poolMutex_(), allocatedCount_(0) {
        // Pre-allocate initial pool
        for (size_t i = 0; i < PoolSize; ++i) {
            freeList_.push_back(std::make_unique<ResourceType>());
        }
    }

    ~NeuralResourcePool() {
        clear();
    }

    // Acquire resource from pool or create new if pool exhausted
    PtrType acquire() {
        std::lock_guard<std::mutex> lock(poolMutex_);
        PtrType resource;
        
        if (!freeList_.empty()) {
            resource = std::move(freeList_.back());
            freeList_.pop_back();
        } else {
            resource = std::make_unique<ResourceType>();
            allocatedCount_++;
        }
        
        return resource;
    }

    // Release resource back to pool
    void release(PtrType& resource) {
        if (!resource) return;
        
        std::lock_guard<std::mutex> lock(poolMutex_);
        
        if (freeList_.size() < PoolSize) {
            freeList_.push_back(std::move(resource));
        } else {
            // Pool at capacity, just delete
            resource.reset();
        }
    }

    // Clear all resources
    void clear() {
        std::lock_guard<std::mutex> lock(poolMutex_);
        freeList_.clear();
    }

    // Get current pool size
    size_t size() const {
        std::lock_guard<std::mutex> lock(poolMutex_);
        return freeList_.size();
    }

    // Get total allocated count
    size_t getAllocatedCount() const {
        std::lock_guard<std::mutex> lock(poolMutex_);
        return allocatedCount_;
    }

    // Thread-safe allocation
    template<typename... Args>
    PtrType create(Args&&... args) {
        std::lock_guard<std::mutex> lock(poolMutex_);
        PtrType resource;
        
        if (!freeList_.empty()) {
            resource = std::move(freeList_.back());
            freeList_.pop_back();
            // Reconstruct with new arguments
            resource->~ResourceType();
            new(resource.get()) ResourceType(std::forward<Args>(args)...);
        } else {
            resource = std::make_unique<ResourceType>(std::forward<Args>(args)...);
            allocatedCount_++;
        }
        
        return resource;
    }

private:
    std::vector<PtrType> freeList_;
    mutable std::mutex poolMutex_;
    size_t allocatedCount_;
};

// =============================================================================
// RAII Resource Acquisition for Exception Safety
// =============================================================================

template<typename T>
class RAIIWrapper {
public:
    using ResourceType = T;
    using PtrType = std::unique_ptr<ResourceType>;

    explicit RAIIWrapper(PtrType resource) : resource_(std::move(resource)) {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper initialized with null resource");
        }
    }

    ~RAIIWrapper() {
        release();
    }

    // Delete copy operations
    RAIIWrapper(const RAIIWrapper&) = delete;
    RAIIWrapper& operator=(const RAIIWrapper&) = delete;

    // Move operations
    RAIIWrapper(RAIIWrapper&& other) noexcept : resource_(std::move(other.resource_)) {}
    RAIIWrapper& operator=(RAIIWrapper&& other) noexcept {
        if (this != &other) {
            release();
            resource_ = std::move(other.resource_);
        }
        return *this;
    }

    ResourceType& get() {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper: Resource not available");
        }
        return *resource_;
    }

    const ResourceType& get() const {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper: Resource not available");
        }
        return *resource_;
    }

    ResourceType* operator->() {
        return resource_.get();
    }

    const ResourceType* operator->() const {
        return resource_.get();
    }

    bool isValid() const noexcept {
        return static_cast<bool>(resource_);
    }

    void reset() {
        release();
    }

    void release() {
        resource_.reset();
    }

    // Execute a function with the resource, ensuring cleanup
    template<typename F, typename... Args>
    auto executeWith(F&& func, Args&&... args) -> decltype(func(std::declval<ResourceType&>(), std::forward<Args>(args)...)) {
        if (!resource_) {
            throw std::runtime_error("RAIIWrapper: Cannot execute with null resource");
        }
        return func(get(), std::forward<Args>(args)...);
    }

private:
    PtrType resource_;
};

// =============================================================================
// Enhanced Memory Management Utilities
// =============================================================================

class MemoryTracker {
public:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
        std::source_location location;
    };

    static MemoryTracker& instance() {
        static MemoryTracker tracker;
        return tracker;
    }

    void trackAllocation(void* ptr, size_t size, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = AllocationInfo{
            ptr, size, std::chrono::system_clock::now(), context, std::source_location::current()
        };
    }

    void trackDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allocations_.find(ptr);
        if (it != allocations_.end()) {
            allocations_.erase(it);
        }
    }

    std::vector<AllocationInfo> getAllocations() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AllocationInfo> result;
        result.reserve(allocations_.size());
        for (const auto& pair : allocations_) {
            result.push_back(pair.second);
        }
        return result;
    }

    size_t getAllocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
    }

    // Check for potential leaks
    std::vector<void*> findLeaks() const {
        std::vector<void*> leaks;
        std::lock_guard<std::mutex> lock(mutex_);
        leaks.reserve(allocations_.size());
        for (const auto& pair : allocations_) {
            leaks.push_back(pair.first);
        }
        return leaks;
    }

private:
    MemoryTracker() = default;
    ~MemoryTracker() = default;
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<void*, AllocationInfo> allocations_;
};

// =============================================================================
// Leak Detection and Resource Cleanup
// =============================================================================

class LeakDetector {
public:
    static bool checkForLeaks() {
        auto leaks = MemoryTracker::instance().findLeaks();
        return !leaks.empty();
    }

    static void reportLeaks() {
        auto leaks = MemoryTracker::instance().findLeaks();
        if (leaks.empty()) {
            return; // No leaks found
        }
        
        std::cerr << "MEMORY LEAKS DETECTED:" << std::endl;
        for (void* ptr : leaks) {
            std::cerr << "  Leaked pointer: " << ptr << std::endl;
        }
        std::cerr << "Total leaked objects: " << leaks.size() << std::endl;
    }

    static void forceCleanup() {
        MemoryTracker::instance().clear();
    }
};

// =============================================================================
// Enhanced Pimpl for Brain Class with Better Resource Management
// =============================================================================

class Brain::ImplEnhanced {
public:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Memory pools for frequently allocated objects
    static thread_local NeuralResourcePool<Neuron, 256> neuronPool;
    static thread_local NeuralResourcePool<Synapse, 512> synapsePool;
    static thread_local NeuralResourcePool<NeuralPopulation, 128> populationPool;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Neural arrays using smart pointers for better memory management
    NeuralArray<Neuron*> sensoryNeurons;
    NeuralArray<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Resource tracking
    std::atomic<size_t> allocationCount{0};
    std::atomic<size_t> deallocationCount{0};
    std::chrono::steady_clock::time_point creationTime;
    
    ImplEnhanced(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
        , creationTime(std::chrono::steady_clock::now())
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Track allocation
        allocationCount.fetch_add(1, std::memory_order_relaxed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    ~ImplEnhanced() {
        allocationCount.fetch_add(1, std::memory_order_relaxed);
        // All resources are cleaned up automatically by smart pointers
    }
    
    // Exception-safe resource acquisition
    template<typename T, typename... Args>
    std::unique_ptr<T> acquireResource(Args&&... args) {
        try {
            return std::make_unique<T>(std::forward<Args>(args)...);
        } catch (const std::exception& e) {
            // Log the error but don't throw - allow partial recovery
            return nullptr;
        }
    }
    
    // Enhanced move operations for neurons
    void moveNeuronsToArray(std::vector<Neuron*>& source, NeuralArray<Neuron*>& target) {
        if (source.empty()) return;
        
        // Use temporary vector for safe move
        std::vector<Neuron*> temp(std::make_move_iterator(source.begin()),
                                 std::make_move_iterator(source.end()));
        
        // Allocate and initialize array
        target.resize(temp.size());
        for (size_t i = 0; i < temp.size(); ++i) {
            target[i] = temp[i];
        }
        
        source.clear();
    }
    
    // Safe cleanup with exception guarantees
    void safeCleanup() noexcept {
        try {
            // Reset all systems
            spikeSystem.reset();
            stdp.reset();
            hebbian.reset();
            structuralPlasticity.reset();
            
            workingMemory.reset();
            episodicMemory.reset();
            associativeMemory.reset();
            predictionSystem.reset();
            planner.reset();
            conceptFormation.reset();
            attention.reset();
            developmentSystem.reset();
            dopamine.reset();
            curiosity.reset();
            predictionError.reset();
            novelty.reset();
            checkpointManager.reset();
            
            // Clear neural arrays
            sensoryNeurons.resize(0);
            motorNeurons.resize(0);
            
            // Clear regions
            regions.clear();
            interRegionConnections.clear();
            
            // Cleanup memory pools
            neuronPool.clear();
            synapsePool.clear();
            populationPool.clear();
            
        } catch (...) {
            // Ensure cleanup continues even if individual cleanup fails
        }
    }
    
    // Get statistics
    size_t getAllocationCount() const noexcept { return allocationCount.load(); }
    size_t getDeallocationCount() const noexcept { return deallocationCount.load(); }
    std::chrono::steady_clock::time_point getCreationTime() const { return creationTime; }
    
private:
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

// =============================================================================
// Enhanced Config Class with Optimized Memory Management
// =============================================================================

class Config::ImplEnhanced {
public:
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex;
    
    // Memory optimization
    static thread_local std::unordered_map<std::string, ConfigValue> keyValueCache;
    size_t maxEntries;
    size_t maxEntrySize;
    
    ImplEnhanced() : maxEntries(10000), maxEntrySize(1024 * 1024) {} // 10k entries, 1MB max
    
    ~ImplEnhanced() {
        clear();
    }
    
    // Thread-safe get with caching
    template<typename T>
    std::optional<T> getWithCache(const std::string& key) const {
        // Check cache first
        auto it = keyValueCache.find(key);
        if (it != keyValueCache.end()) {
            try {
                return std::get<T>(it->second);
            } catch (const std::bad_variant_access&) {
                keyValueCache.erase(it);
            }
        }
        
        // Cache miss - compute normally
        std::lock_guard<std::mutex> lock(mutex);
        auto entryIt = std::find_if(entries.begin(), entries.end(),
                                   [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entryIt == entries.end()) {
            return std::nullopt;
        }
        
        try {
            T result = std::get<T>(entryIt->value);
            
            // Update cache if within limits
            if (keyValueCache.size() < maxEntries) {
                keyValueCache[key] = entryIt->value;
            }
            
            return result;
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Enhanced set with validation and leak prevention
    void setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        validateKeyWithSize(key);
        validateValueWithSize(value);
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = std::find_if(entries.begin(), entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries.end()) {
            it->value = value;
            it->source = source;
            it->lastModified = std::chrono::system_clock::now();
            
            // Update cache
            keyValueCache[key] = value;
        } else {
            entries.emplace_back(key, value, source);
            
            // Update cache
            if (keyValueCache.size() < maxEntries) {
                keyValueCache[key] = value;
            }
        }
    }
    
    // Enhanced clear with thread safety
    void clearWithThreadSafety() {
        std::lock_guard<std::mutex> lock(mutex);
        entries.clear();
        keyValueCache.clear();
    }
    
    // Thread-safe operations
    bool tryLock() {
        return mutex.try_lock();
    }
    
    void unlock() {
        mutex.unlock();
    }
    
    // Memory statistics
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& entry : entries) {
            total += entry.key.size() + sizeof(ConfigValue) + entry.description.size();
        }
        return total;
    }
    
    size_t getEntryCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return entries.size();
    }
    
private:
    static void validateKeyWithSize(const std::string& key) {
        if (key.empty() || key.length() > 256) {
            throw ConfigException("Invalid key: " + key, "validateKeyWithSize");
        }
    }
    
    static void validateValueWithSize(const ConfigValue& value) {
        std::visit([](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                if (val.length() > 1024 * 1024) {
                    throw ConfigException("Value too long", "validateValueWithSize");
                }
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (s.length() > 1024 * 1024) {
                        throw ConfigException("Value too long", "validateValueWithSize");
                    }
                }
            }
        }, value);
    }
};

// =============================================================================
// Enhanced Logger Class with RAII for Log Resources
// =============================================================================

class Logger::ImplEnhanced {
public:
    std::string name;
    std::vector<std::shared_ptr<LogAppender>> appenders;
    std::shared_ptr<LogFormatter> formatter;
    std::shared_ptr<ILogFilter> filter;
    LogLevel level = LogLevel::Info;
    bool flushOnEachLog = false;
    LogMetrics metrics;
    std::mutex mutex;
    std::atomic<uint64_t> sequenceCounter = 0;
    bool asyncEnabled = false;
    std::shared_ptr<AsyncLogger> asyncLogger;
    
    // Resource pools for memory optimization
    std::unique_ptr<LogEntryPool> entryPool;
    std::unique_ptr<StringPool> stringPool;
    
    // Thread-local storage for performance
    static thread_local std::unique_ptr<LogEntryPool> threadLocalEntryPool;
    static thread_local std::unique_ptr<StringPool> threadLocalStringPool;
    
    // Configuration and control
    std::chrono::steady_clock::time_point lastCleanupTime;
    size_t cleanupThreshold;
    
    ImplEnhanced(const std::string& loggerName) 
        : name(loggerName)
        , formatter(std::make_shared<PatternFormatter>()) 
        , lastCleanupTime(std::chrono::steady_clock::now())
        , cleanupThreshold(10000)  // Cleanup every 10k logs
    {
        // Set up default console appender
        auto consoleAppender = std::make_shared<ConsoleAppender>();
        consoleAppender->setLevel(level);
        consoleAppender->setUseColors(true);
        appenders.push_back(consoleAppender);
        
        // Initialize resource pools
        entryPool = std::make_unique<LogEntryPool>(1024);
        stringPool = std::make_unique<StringPool>(2048);
        
        // Initialize thread-local pools
        if (!threadLocalEntryPool) {
            threadLocalEntryPool = std::make_unique<LogEntryPool>(512);
        }
        if (!threadLocalStringPool) {
            threadLocalStringPool = std::make_unique<StringPool>(1024);
        }
    }
    
    ~ImplEnhanced() {
        // Ensure all resources are cleaned up
        asyncLogger.reset();
        entryPool.reset();
        stringPool.reset();
        threadLocalEntryPool.reset();
        threadLocalStringPool.reset();
    }
    
    // RAII for log entry acquisition
    LogEntry* acquireEntry() {
        // Try thread-local pool first
        if (threadLocalEntryPool) {
            LogEntry* entry = threadLocalEntryPool->acquire();
            if (entry) {
                return entry;
            }
        }
        
        // Try global pool
        if (entryPool) {
            LogEntry* entry = entryPool->acquire();
            if (entry) {
                return entry;
            }
        }
        
        // Fallback to heap allocation
        return new LogEntry();
    }
    
    // RAII for log entry release
    void releaseEntry(LogEntry* entry) {
        if (!entry) return;
        
        // Try thread-local pool first
        if (threadLocalEntryPool) {
            threadLocalEntryPool->release(entry);
            return;
        }
        
        // Try global pool
        if (entryPool) {
            entryPool->release(entry);
            return;
        }
        
        // Fallback to heap deallocation
        delete entry;
    }
    
    // RAII for string acquisition
    std::string* acquireString() {
        // Try thread-local pool first
        if (threadLocalStringPool) {
            std::string* str = threadLocalStringPool->acquire();
            if (str) {
                return str;
            }
        }
        
        // Try global pool
        if (stringPool) {
            std::string* str = stringPool->acquire();
            if (str) {
                return str;
            }
        }
        
        // Fallback to heap allocation
        return new std::string();
    }
    
    // RAII for string release
    void releaseString(std::string* str) {
        if (!str) return;
        
        // Try thread-local pool first
        if (threadLocalStringPool) {
            threadLocalStringPool->release(str);
            return;
        }
        
        // Try global pool
        if (stringPool) {
            stringPool->release(str);
            return;
        }
        
        // Fallback to heap deallocation
        delete str;
    }
    
    // Exception-safe logging with proper resource management
    void safeLog(const LogEntry& originalEntry) {
        // Acquire resources with exception safety
        RAIIWrapper<LogEntry> entryWrapper(acquireEntry());
        LogEntry& entry = entryWrapper.get();
        
        // Copy data
        entry.level = originalEntry.level;
        entry.message = originalEntry.message;
        entry.file = originalEntry.file;
        entry.line = originalEntry.line;
        entry.function = originalEntry.function;
        entry.timestamp = originalEntry.timestamp;
        entry.fields = originalEntry.fields;
        entry.tags = originalEntry.tags;
        entry.module = originalEntry.module;
        entry.sequenceNumber = ++sequenceCounter;
        
        // Create string for message (if needed)
        RAIIWrapper<std::string> messageWrapper(acquireString());
        messageWrapper.get() = originalEntry.message;
        
        try {
            // Check if logging is enabled
            if (entry.level < level) {
                return;
            }
            
            // Apply filter if set
            if (filter && !filter->isEnabled()) {
                return;
            }
            if (filter && !filter->shouldLog(entry)) {
                return;
            }
            
            // Format the log message
            std::string formattedMessage;
            if (formatter) {
                formattedMessage = formatter->format(entry);
            } else {
                formattedMessage = entry.message;
            }
            
            // Record metrics
            metrics.recordLog(std::chrono::steady_clock::now() - startTime);
            
            // Use async logger if enabled
            if (asyncEnabled && asyncLogger) {
                asyncLogger->log(entry);
            } else {
                // Synchronous logging
                for (auto& appender : appenders) {
                    if (appender) {
                        appender->append(formattedMessage);
                    }
                }
                
                if (flushOnEachLog) {
                    for (auto& appender : appenders) {
                        if (appender) {
                            appender->flush();
                        }
                    }
                }
            }
            
        } catch (const std::exception& e) {
            // Ensure resources are cleaned up on exception
            throw;
        }
        
        // Release resources
        releaseString(&messageWrapper.get());
        releaseEntry(&entryWrapper.get());
    }
    
    // Periodic cleanup to prevent memory leaks
    void periodicCleanup() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastCleanupTime);
        
        if (elapsed.count() >= 60) {  // Every minute
            lastCleanupTime = now;
            
            // Cleanup thread-local pools
            if (threadLocalEntryPool) {
                threadLocalEntryPool->clear();
            }
            if (threadLocalStringPool) {
                threadLocalStringPool->clear();
            }
            
            // Cleanup global pools if memory usage is high
            if (entryPool && entryPool->size() > 512) {
                entryPool->clear();
            }
            if (stringPool && stringPool->size() > 1024) {
                stringPool->clear();
            }
        }
    }
    
    // Get performance statistics
    double getAverageLatency() const {
        return metrics.getAverageLatency();
    }
    
    uint64_t getLogCount() const {
        return metrics.logCount;
    }
    
    size_t getEntryPoolSize() const {
        return entryPool ? entryPool->size() : 0;
    }
    
    size_t getStringPoolSize() const {
        return stringPool ? stringPool->size() : 0;
    }
    
private:
    std::chrono::steady_clock::time_point startTime;
};

// =============================================================================
// Enhanced AgentBrain Class with Improved Memory Management
// =============================================================================

class AgentBrain::ImplEnhanced {
public:
    std::shared_ptr<Brain> brain_;
    
    // Improved memory management for neural arrays
    using NeuralArray = std::vector<Neuron*>;
    NeuralArray motorForward_;
    NeuralArray motorBackward_;
    NeuralArray motorTurnLeft_;
    NeuralArray motorTurnRight_;
    NeuralArray motorInteract_;
    NeuralArray motorWait_;
    
    NeuralArray sensoryVision_;
    NeuralArray sensoryTouch_;
    NeuralArray sensoryInternal_;
    NeuralArray sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Memory management improvements
    size_t allocationCount;
    size_t deallocationCount;
    std::chrono::steady_clock::time_point creationTime;
    
    ImplEnhanced(std::shared_ptr<Brain> brain)
        : brain_(std::move(brain))
        , dopamineLevel_(0.0f)
        , noveltyLevel_(0.0f)
        , curiosityLevel_(0.0f)
        , predictionError_(0.0f)
        , expectedReward_(0.0f)
        , developmentalAge_(0.0)
        , plasticityModifier_(1.0f)
        , rewardModulationEnabled_(true)
        , structuralPlasticityEnabled_(true)
        , developmentEnabled_(true)
        , curiosityEnabled_(true)
        , sensoryNoveltyDecay_(0.99f)
        , allocationCount(0)
        , deallocationCount(0)
        , creationTime(std::chrono::steady_clock::now())
    {
        if (!brain_) {
            throw std::invalid_argument("AgentBrain initialized with null brain pointer");
        }
    }
    
    ~ImplEnhanced() {
        deallocationCount++;
    }
    
    // Enhanced neuron distribution with RAII
    void distributeMotorNeurons(const NeuralArray& neurons) {
        for (Neuron* neuron : neurons) {
            if (!neuron) continue;
            
            // Use neural activity as basis for distribution
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            
            // Distribute based on activity thresholds
            if (activity > 0.8f) motorForward_.push_back(neuron);
            else if (activity > 0.6f) motorBackward_.push_back(neuron);
            else if (activity > 0.4f) motorTurnLeft_.push_back(neuron);
            else if (activity > 0.2f) motorTurnRight_.push_back(neuron);
            else if (activity > 0.1f) motorInteract_.push_back(neuron);
            else motorWait_.push_back(neuron);
        }
    }
    
    void distributeSensoryNeurons(const NeuralArray& neurons) {
        for (Neuron* neuron : neurons) {
            if (!neuron) continue;
            
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            
            // Simplified distribution - could be enhanced based on neuron type
            if (activity > 0.5f) {
                // Assign to vision, touch, internal, or proprioception based on some criteria
                // For now, just add to vision pool
                sensoryVision_.push_back(neuron);
            } else {
                sensoryTouch_.push_back(neuron);
            }
        }
    }
    
    // Enhanced reward modulation with RAII
    void applyRewardModulation(float reward, float predictedReward) {
        if (!brain_ || !rewardModulationEnabled_) return;
        
        // Compute prediction error
        predictionError_ = reward - predictedReward;
        
        // Update expected reward (exponential moving average)
        expectedReward_ = 0.95f * expectedReward_ + (1.0f - 0.95f) * reward;
        
        // Dopamine-like signal (based on prediction error)
        dopamineLevel_ = predictionError_;
        
        // Clamp to reasonable range
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
        
        // Apply to all synapses with eligibility traces
        const auto& regions = brain_->getRegions();
        if (regions.empty()) return;
        
        for (const auto& region : regions) {
            if (!region) continue;
            
            const auto& synapses = region->getSynapses();
            if (synapses.empty()) continue;
            
            for (auto* syn : synapses) {
                if (!syn) continue;
                
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    // Apply reward-modulated weight change
                    float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                    syn->addToWeight(delta);
                    
                    // Decay eligibility trace
                    syn->decayEligibilityTrace(0.1f);
                }
            }
        }
        
        // Modulate plasticity based on dopamine
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply to STDP
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
    
    // Safe array access with bounds checking
    Neuron* safeGetNeuron(const NeuralArray& neurons, size_t index) const {
        if (index < neurons.size()) {
            return neurons[index];
        }
        return nullptr;
    }
    
    // Memory statistics
    size_t getAllocationCount() const { return allocationCount; }
    size_t getDeallocationCount() const { return deallocationCount; }
    std::chrono::steady_clock::time_point getCreationTime() const { return creationTime; }
    
    // Enhanced reset with exception safety
    void safeReset() {
        try {
            // Reset neuromodulation state
            dopamineLevel_ = 0.0f;
            noveltyLevel_ = 0.0f;
            curiosityLevel_ = 0.0f;
            predictionError_ = 0.0f;
            expectedReward_ = 0.0f;
            
            // Reset development state
            developmentalAge_ = 0.0;
            plasticityModifier_ = 1.0f;
            
            // Clear neural arrays
            motorForward_.clear();
            motorBackward_.clear();
            motorTurnLeft_.clear();
            motorTurnRight_.clear();
            motorInteract_.clear();
            motorWait_.clear();
            
            sensoryVision_.clear();
            sensoryTouch_.clear();
            sensoryInternal_.clear();
            sensoryProprioception_.clear();
            
            // Clear previous sensory state
            previousVision_.clear();
            
        } catch (...) {
            // Ensure cleanup continues even if individual reset fails
        }
    }
};

// =============================================================================
// Custom RAII Classes for Specific Resource Types
// =============================================================================

class NeuronPoolRAII {
public:
    NeuronPoolRAII(size_t initialSize = 256) {
        pool_.reserve(initialSize);
    }
    
    ~NeuronPoolRAII() {
        clear();
    }
    
    // Delete copy operations
    NeuronPoolRAII(const NeuronPoolRAII&) = delete;
    NeuronPoolRAII& operator=(const NeuronPoolRAII&) = delete;
    
    // Move operations
    NeuronPoolRAII(NeuronPoolRAII&& other) noexcept : pool_(std::move(other.pool_)) {}
    NeuronPoolRAII& operator=(NeuronPoolRAII&& other) noexcept {
        if (this != &other) {
            clear();
            pool_ = std::move(other.pool_);
        }
        return *this;
    }
    
    // Acquire neuron from pool or create new
    Neuron* acquire() {
        Neuron* neuron = nullptr;
        if (!pool_.empty()) {
            neuron = pool_.back();
            pool_.pop_back();
        } else {
            neuron = new Neuron();
        }
        return neuron;
    }
    
    // Release neuron back to pool
    void release(Neuron* neuron) {
        if (neuron) {
            pool_.push_back(neuron);
        }
    }
    
    // Clear all neurons
    void clear() {
        for (Neuron* neuron : pool_) {
            delete neuron;
        }
        pool_.clear();
    }
    
    // Get pool statistics
    size_t size() const { return pool_.size(); }
    
private:
    std::vector<Neuron*> pool_;
};

class SynapsePoolRAII {
public:
    SynapsePoolRAII(size_t initialSize = 512) {
        pool_.reserve(initialSize);
    }
    
    ~SynapsePoolRAII() {
        clear();
    }
    
    // Delete copy operations
    SynapsePoolRAII(const SynapsePoolRAII&) = delete;
    SynapsePoolRAII& operator=(const SynapsePoolRAII&) = delete;
    
    // Move operations
    SynapsePoolRAII(SynapsePoolRAII&& other) noexcept : pool_(std::move(other.pool_)) {}
    SynapsePoolRAII& operator=(SynapsePoolRAII&& other) noexcept {
        if (this != &other) {
            clear();
            pool_ = std::move(other.pool_);
        }
        return *this;
    }
    
    // Acquire synapse from pool or create new
    Synapse* acquire() {
        Synapse* synapse = nullptr;
        if (!pool_.empty()) {
            synapse = pool_.back();
            pool_.pop_back();
        } else {
            synapse = new Synapse();
        }
        return synapse;
    }
    
    // Release synapse back to pool
    void release(Synapse* synapse) {
        if (synapse) {
            pool_.push_back(synapse);
        }
    }
    
    // Clear all synapses
    void clear() {
        for (Synapse* synapse : pool_) {
            delete synapse;
        }
        pool_.clear();
    }
    
    // Get pool statistics
    size_t size() const { return pool_.size(); }
    
private:
    std::vector<Synapse*> pool_;
};

// =============================================================================
// Enhanced Global Memory Management
// =============================================================================

class GlobalMemoryManager {
public:
    static GlobalMemoryManager& instance() {
        static GlobalMemoryManager manager;
        return manager;
    }
    
    // Global resource pools
    static thread_local NeuronPoolRAII neuronPool;
    static thread_local SynapsePoolRAII synapsePool;
    static std::mutex managerMutex;
    
    // Allocate neuron with pool
    static Neuron* allocateNeuron() {
        return neuronPool.acquire();
    }
    
    // Deallocate neuron with pool
    static void deallocateNeuron(Neuron* neuron) {
        neuronPool.release(neuron);
    }
    
    // Allocate synapse with pool
    static Synapse* allocateSynapse() {
        return synapsePool.acquire();
    }
    
    // Deallocate synapse with pool
    static void deallocateSynapse(Synapse* synapse) {
        synapsePool.release(synapse);
    }
    
    // Cleanup all pools
    static void cleanupAll() {
        std::lock_guard<std::mutex> lock(managerMutex);
        neuronPool.clear();
        synapsePool.clear();
    }
    
    // Get memory statistics
    static size_t getNeuronPoolSize() {
        return neuronPool.size();
    }
    
    static size_t getSynapsePoolSize() {
        return synapsePool.size();
    }
    
private:
    GlobalMemoryManager() = default;
    ~GlobalMemoryManager() = default;
    GlobalMemoryManager(const GlobalMemoryManager&) = delete;
    GlobalMemoryManager& operator=(const GlobalMemoryManager&) = delete;
};

// =============================================================================
// Implementation of Enhanced Brain Class
// =============================================================================

// Static member definitions
thread_local NeuralResourcePool<Neuron, 256> Brain::ImplEnhanced::neuronPool;
thread_local NeuralResourcePool<Synapse, 512> Brain::ImplEnhanced::synapsePool;
thread_local NeuralResourcePool<NeuralPopulation, 128> Brain::ImplEnhanced::populationPool;

// =============================================================================
// Constructor and Destructor
// =============================================================================

Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<ImplEnhanced>(config)) {}

Brain::~Brain() {
    if (pImpl) {
        pImpl->safeCleanup();
        pImpl.reset();
    }
}

// =============================================================================
// Move Operations
// =============================================================================

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        if (pImpl) {
            pImpl->safeCleanup();
            pImpl.reset();
        }
        pImpl = std::move(other.pImpl);
        other.pImpl = nullptr;
    }
    return *this;
}

// =============================================================================
// Initialization and Configuration
// =============================================================================

bool Brain::initialize(std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Configuration cannot be null");
    }
    
    if (pImpl) {
        throw std::runtime_error("Brain is already initialized");
    }
    
    pImpl = std::make_unique<ImplEnhanced>(config);
    
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Initialize all integrated systems
    initializeIntegratedSystems();
    
    return true;
}

void Brain::initializeIntegratedSystems() {
    if (!pImpl) return;
    
    try {
        // Initialize memory systems
        if (pImpl->workingMemory) {
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(1000);
        }
        
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
        }
        
        if (pImpl->associativeMemory) {
            pImpl->associativeMemory->initialize(this);
        }
        
        // Initialize cognition systems
        if (pImpl->planner) {
            pImpl->planner->initialize(this);
            pImpl->planner->setPlanningDepth(5);
        }
        
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->initialize(this);
        }
        
        if (pImpl->attention) {
            pImpl->attention->initialize(this);
            pImpl->attention->setInhibitionStrength(0.5f);
            pImpl->attention->setExcitationStrength(1.5f);
        }
        
        // Initialize neuromodulation
        if (pImpl->novelty) {
            pImpl->novelty->initialize(this);
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->initialize(this);
        }
        
        // Register spike handlers
        registerSpikeHandlers();
        
        // Configure checkpoint manager
        if (pImpl->checkpointManager) {
            std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
        
    } catch (const std::exception& e) {
        // Log error and ensure cleanup
        NLM_LOG_ERROR(std::string("Error initializing integrated systems: ") + e.what());
        pImpl->safeCleanup();
        throw;
    }
}

void Brain::registerSpikeHandlers() {
    if (!pImpl) return;
    
    // Spike count handler
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Synaptic input delivery handler
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        deliverSynapticInput(event);
    });
}

void Brain::deliverSynapticInput(const DelayedSpikeEvent& event) {
    if (!pImpl) return;
    
    // Find destination neuron and deliver synaptic input
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == event.destination_neuron) {
                // Apply synaptic weight as current
                MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                if (event.is_excitatory) {
                    neuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    neuron->receiveInhibitoryInput(-synapticCurrent);
                }
                return;
            }
        }
    }
}

// =============================================================================
// Simulation Control
// =============================================================================

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl) return;
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process pending delayed spikes
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons
    updateNeurons();
    
    // Detect spikes and schedule events
    detectAndScheduleSpikes();
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Update all integrated systems
    updateIntegratedSystems();
    
    // Perform periodic maintenance
    performPeriodicMaintenance(currentStep);
    
    // Update memory pools
    pImpl->neuronPool.clear();
    pImpl->synapsePool.clear();
    pImpl->populationPool.clear();
}

void Brain::updateNeurons() {
    if (!pImpl) return;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(pImpl->currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::detectAndScheduleSpikes() {
    if (!pImpl) return;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(pImpl->currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Create spike event
                    SpikeEvent event(neuron->getId(), pImpl->currentTime, pImpl->currentStep);
                    pImpl->spikeSystem->queueSpike(event);
                    
                    // Process plasticity
                    processPlasticity(neuron, event);
                }
            }
        }
    }
}

void Brain::processPlasticity(Neuron* neuron, const SpikeEvent& event) {
    if (!neuron) return;
    
    // Record post-synaptic spike for incoming synapses (plasticity)
    auto region = getRegion(neuron->getRegionId());
    if (!region) return;
    
    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
    for (Synapse* syn : incomingSynapses) {
        syn->recordPostSpike(pImpl->currentTime);
    }
    
    // Schedule delayed spike events
    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
    for (Synapse* syn : outgoingSynapses) {
        // Create delayed spike event
        Delay delay = syn->getDelay();
        SimulationStep deliveryStep = pImpl->currentStep + delay;
        Timestamp deliveryTime = pImpl->currentTime + delay * pImpl->timestep;

        DelayedSpikeEvent delayedEvent(
            neuron->getId(),
            syn->getDestinationNeuron(),
            syn->getId(),
            syn->getWeight(),
            syn->getType(),
            pImpl->currentTime,
            deliveryTime,
            pImpl->currentStep,
            deliveryStep
        );

        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
        
        // Record pre-synaptic spike for plasticity
        syn->recordPreSpike(pImpl->currentTime);
    }
}

void Brain::updateIntegratedSystems() {
    if (!pImpl) return;
    
    try {
        // Update working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(pImpl->timestep);
        }
        
        // Apply neuromodulation effects
        if (pImpl->novelty) {
            pImpl->novelty->update(pImpl->timestep);
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->timestep);
        }
        
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = pImpl->dopamine->getLevel();
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
        
        // Apply plasticity rules
        applyPlasticityRules();
        
        // Update episodic memory
        updateEpisodicMemory();
        
        // Update cognition systems
        updateCognitionSystems();
        
        // Update structural plasticity
        updateStructuralPlasticity();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error updating integrated systems: ") + e.what());
    }
}

void Brain::applyPlasticityRules() {
    if (!pImpl) return;
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(pImpl->currentTime);
        }
    }
}

void Brain::updateEpisodicMemory() {
    if (!pImpl) return;
    
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = pImpl->currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void Brain::updateCognitionSystems() {
    if (!pImpl) return;
    
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::updateStructuralPlasticity() {
    if (!pImpl) return;
    
    // Apply structural plasticity periodically
    if (pImpl->currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // Update development effects
    if (pImpl->currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::performPeriodicMaintenance(SimulationStep currentStep) {
    if (!pImpl) return;
    
    // Replay important memories
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, pImpl->currentTime);
    }
}

// =============================================================================
// State Management and Persistence
// =============================================================================

void Brain::reset(bool preserveConfig) {
    if (!pImpl) return;
    
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

// =============================================================================
// Neural Array Accessors
// =============================================================================

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl ? pImpl->workingMemory.get() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl ? pImpl->episodicMemory.get() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl ? pImpl->associativeMemory.get() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl ? pImpl->predictionSystem.get() : nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl ? pImpl->planner.get() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl ? pImpl->conceptFormation.get() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return pImpl ? pImpl->attention.get() : nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl ? pImpl->developmentSystem.get() : nullptr;
}

Dopamine* Brain::getDopamine() {
    return pImpl ? pImpl->dopamine.get() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return pImpl ? pImpl->curiosity.get() : nullptr;
}

Novelty* Brain::getNovelty() {
    return pImpl ? pImpl->novelty.get() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl ? pImpl->predictionError.get() : nullptr;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl ? pImpl->rng.get() : nullptr;
}

// =============================================================================
// Memory Statistics and Diagnostics
// =============================================================================

void Brain::logStatus() const {
    if (!pImpl) return;
    
    NLM_LOG_INFO("=== Enhanced NLM Brain Status ===");
    NLM_LOG_INFO("Regions: " + std::to_string(pImpl->regions.size()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

// =============================================================================
// Global Memory Management Functions
// =============================================================================

namespace {
    class MemoryInitializer {
    public:
        MemoryInitializer() {
            // Register cleanup at program exit
            static bool registered = []() {
                std::atexit(GlobalMemoryManager::cleanupAll);
                return true;
            }();
        }
    } memoryInitializer;
}

void cleanupEnhancedResources() {
    GlobalMemoryManager::cleanupAll();
}

void reportMemoryLeaks() {
    LeakDetector::reportLeaks();
}

void forceMemoryCleanup() {
    MemoryTracker::instance().clear();
    LeakDetector::forceCleanup();
}

} // namespace nlm
