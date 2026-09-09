// Memory tracking for debugging
#ifdef DEBUG_MEMORY
struct MemoryTracker {
    static std::atomic<size_t> allocations;
    static std::atomic<size_t> deallocations;
    static std::mutex mutex;
    
    static void allocate(const char* file, int line, size_t size);
    static void deallocate(const char* file, int line, size_t size);
    static void printReport();
};

#define NLM_ALLOC(size) MemoryTracker::allocate(__FILE__, __LINE__, size)
#define NLM_FREE(size) MemoryTracker::deallocate(__FILE__, __LINE__, size)
#endif

// Smart pointer helpers for better memory management
namespace nlm {
    // Custom deleter for neuron pointers
    struct NeuronDeleter {
        std::vector<Neuron*>* neuronsList;
        NeuronDeleter(std::vector<Neuron*>* list) : neuronsList(list) {}
        void operator()(Neuron* ptr) {
            if (ptr && neuronsList) {
                auto it = std::find(neuronsList->begin(), neuronsList->end(), ptr);
                if (it != neuronsList->end()) {
                    neuronsList->erase(it);
                }
            }
            delete ptr;
        }
    };
    
    // Custom deleter for synapse pointers
    struct SynapseDeleter {
        std::vector<Synapse*>* synapsesList;
        SynapseDeleter(std::vector<Synapse*>* list) : synapsesList(list) {}
        void operator()(Synapse* ptr) {
            if (ptr && synapsesList) {
                auto it = std::find(synapsesList->begin(), synapsesList->end(), ptr);
                if (it != synapsesList->end()) {
                    synapsesList->erase(it);
                }
            }
            delete ptr;
        }
    };
    
    // Memory-safe vector that automatically cleans up pointers
    template<typename T>
    class SafeVector {
    public:
        SafeVector() = default;
        ~SafeVector() {
            for (auto* ptr : pointers) {
                delete ptr;
            }
        }
        
        void add(T* ptr) {
            if (ptr) {
                pointers.push_back(ptr);
            }
        }
        
        void remove(T* ptr) {
            auto it = std::find(pointers.begin(), pointers.end(), ptr);
            if (it != pointers.end()) {
                pointers.erase(it);
            }
        }
        
        T* get(size_t index) {
            return (index < pointers.size()) ? pointers[index] : nullptr;
        }
        
        const T* get(size_t index) const {
            return (index < pointers.size()) ? pointers[index] : nullptr;
        }
        
        size_t size() const { return pointers.size(); }
        bool empty() const { return pointers.empty(); }
        
        // Iteration support
        typename std::vector<T*>::iterator begin() { return pointers.begin(); }
        typename std::vector<T*>::iterator end() { return pointers.end(); }
        typename std::vector<T*>::const_iterator begin() const { return pointers.begin(); }
        typename std::vector<T*>::const_iterator end() const { return pointers.end(); }
        
        // Clear without deleting
        void clearNoDelete() {
            pointers.clear();
        }
        
    private:
        std::vector<T*> pointers;
    };
    
    // Memory pool for frequently allocated objects
    template<typename T>
    class MemoryPool {
    public:
        MemoryPool(size_t poolSize = 1000) : poolSize(poolSize) {}
        
        T* allocate() {
            if (freeList.empty()) {
                return new T[chunkSize];
            }
            T* ptr = freeList.back();
            freeList.pop_back();
            return ptr;
        }
        
        void deallocate(T* ptr) {
            freeList.push_back(ptr);
        }
        
        ~MemoryPool() {
            // Deallocate all chunks
            for (auto ptr : chunks) {
                delete[] ptr;
            }
        }
        
    private:
        std::vector<T*> chunks;
        std::vector<T*> freeList;
        size_t poolSize;
        const size_t chunkSize = 100;
    };
    
    // Memory-safe state for objects with validation
    template<typename T>
    class MemorySafeState {
    public:
        MemorySafeState() : initialized(false), deleted(false) {}
        
        void initialize(const T& state) {
            if (deleted) {
                throw std::runtime_error("Cannot initialize deleted object");
            }
            state_ = state;
            initialized = true;
        }
        
        T& getState() {
            if (!initialized) {
                throw std::runtime_error("Object not initialized");
            }
            if (deleted) {
                throw std::runtime_error("Object has been deleted");
            }
            return state_;
        }
        
        const T& getState() const {
            if (!initialized) {
                throw std::runtime_error("Object not initialized");
            }
            if (deleted) {
                throw std::runtime_error("Object has been deleted");
            }
            return state_;
        }
        
        void reset() {
            state_ = T();
            initialized = false;
        }
        
        void cleanup() {
            if (initialized) {
                state_.~T();
            }
            initialized = false;
            deleted = true;
        }
        
        bool isValid() const {
            return initialized && !deleted;
        }
        
    private:
        T state_;
        bool initialized;
        bool deleted;
    };
    
    // Scoped memory guard for RAII
    class MemoryGuard {
    public:
        MemoryGuard(std::function<void()> cleanup) : cleanupFn(cleanup) {}
        ~MemoryGuard() {
            if (cleanupFn) {
                cleanupFn();
            }
        }
        
        void release() {
            cleanupFn = nullptr;
        }
        
    private:
        std::function<void()> cleanupFn;
    };
}

// Enhanced neuron with memory management
class Neuron {
public:
    Neuron(NeuronId id) : id(id), stateInitialized(false), deleted(false) {}
    
    ~Neuron() {
        if (stateInitialized && !deleted) {
            state.~NeuronState();
        }
    }
    
    void initialize() {
        if (deleted) {
            throw std::runtime_error("Cannot initialize deleted neuron");
        }
        new (&state) NeuronState();
        stateInitialized = true;
    }
    
    NeuronState& getState() {
        if (!stateInitialized || deleted) {
            throw std::runtime_error("Neuron in invalid state");
        }
        return state;
    }
    
    const NeuronState& getState() const {
        if (!stateInitialized || deleted) {
            throw std::runtime_error("Neuron in invalid state");
        }
        return state;
    }
    
    void reset() {
        if (stateInitialized) {
            state.~NeuronState();
        }
        stateInitialized = false;
        deleted = false;
    }
    
    void cleanup() {
        if (stateInitialized) {
            state.~NeuronState();
        }
        stateInitialized = false;
        deleted = true;
    }
    
    bool isValid() const { return stateInitialized && !deleted; }
    
private:
    NeuronId id;
    NeuronState state;
    bool stateInitialized;
    bool deleted;
};

// Memory-safe brain with automatic cleanup
class Brain {
public:
    ~Brain() {
        // Cleanup all components
        cleanupAllRegions();
        cleanupMemorySystems();
        cleanupPlasticitySystems();
    }
    
    void initialize() {
        if (!initialized) {
            // Initialize all systems
            initializeNeurons();
            initializeSynapses();
            initializeMemorySystems();
            initializePlasticitySystems();
            initialized = true;
        }
    }
    
    void reset() {
        if (initialized) {
            // Reset all systems
            resetNeurons();
            resetSynapses();
            resetMemorySystems();
            resetPlasticitySystems();
            currentStep = 0;
            currentTime = 0.0;
        }
    }
    
    void cleanup() {
        if (initialized) {
            reset();
            cleanupAllRegions();
            cleanupMemorySystems();
            cleanupPlasticitySystems();
            initialized = false;
        }
    }
    
    bool isValid() const { return initialized; }
    
    // Safe access methods
    Neuron* getNeuron(NeuronId id) {
        auto* region = getRegionForNeuron(id);
        return region ? region->getNeuron(id) : nullptr;
    }
    
    Synapse* getSynapse(SynapseId id) {
        auto* region = getRegionForSynapse(id);
        return region ? region->getSynapse(id) : nullptr;
    }
    
private:
    bool initialized = false;
    SimulationStep currentStep = 0;
    Timestamp currentTime = 0.0;
    
    // Regions and systems
    std::vector<NeuralRegion*> regions;
    std::vector<Neuron*> allNeurons;
    std::vector<Synapse*> allSynapses;
    
    void initializeNeurons() {
        for (auto* region : regions) {
            for (auto* neuron : region->getAllNeurons()) {
                neuron->initialize();
                allNeurons.push_back(neuron);
            }
        }
    }
    
    void initializeSynapses() {
        for (auto* region : regions) {
            for (auto* synapse : region->getAllSynapses()) {
                synapse->initialize();
                allSynapses.push_back(synapse);
            }
        }
    }
    
    void cleanupAllRegions() {
        for (auto* region : regions) {
            if (region) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron) {
                        neuron->cleanup();
                    }
                }
                for (auto* synapse : region->getAllSynapses()) {
                    if (synapse) {
                        synapse->reset();
                    }
                }
            }
        }
    }
    
    // Placeholder methods for other systems
    void initializeMemorySystems() {}
    void initializePlasticitySystems() {}
    void resetNeurons() {}
    void resetSynapses() {}
    void resetMemorySystems() {}
    void resetPlasticitySystems() {}
    void cleanupMemorySystems() {}
    void cleanupPlasticitySystems() {}
    NeuralRegion* getRegionForNeuron(NeuronId id) { return nullptr; }
    NeuralRegion* getRegionForSynapse(SynapseId id) { return nullptr; }
};

// Thread-safe memory allocator for concurrent access
class ThreadSafeAllocator {
public:
    static ThreadSafeAllocator& getInstance() {
        static ThreadSafeAllocator instance;
        return instance;
    }
    
    template<typename T>
    T* allocate(size_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex);
        return new T[count];
    }
    
    template<typename T>
    void deallocate(T* ptr, size_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex);
        delete[] ptr;
    }
    
    // For single objects
    template<typename T>
    T* allocate() {
        std::lock_guard<std::mutex> lock(mutex);
        return new T;
    }
    
    template<typename T>
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        delete ptr;
    }
    
private:
    ThreadSafeAllocator() = default;
    ~ThreadSafeAllocator() = default;
    ThreadSafeAllocator(const ThreadSafeAllocator&) = delete;
    ThreadSafeAllocator& operator=(const ThreadSafeAllocator&) = delete;
    
    std::mutex mutex;
};

// Memory error handling
namespace memory {
    enum class ErrorCode {
        ALLOCATION_FAILED,
        DOUBLE_FREE,
        NULL_POINTER_DEREFERENCE,
        BUFFER_OVERFLOW,
        INVALID_ALIGNMENT,
        OUT_OF_MEMORY,
        CORRUPTION_DETECTED
    };
    
    class MemoryError : public std::runtime_error {
    public:
        MemoryError(ErrorCode code, const std::string& message)
            : std::runtime_error(message), code(code) {}
        
        ErrorCode getCode() const { return code; }
        
    private:
        ErrorCode code;
    };
    
    // Memory validation
    class MemoryValidator {
    public:
        static bool validatePointer(void* ptr);
        static bool validateBuffer(void* buffer, size_t size);
        static bool validateAlignment(void* ptr, size_t alignment);
        
        static void enableBoundsChecking(bool enable) {
            boundsCheckingEnabled = enable;
        }
        
        static bool isBoundsCheckingEnabled() {
            return boundsCheckingEnabled;
        }
        
    private:
        static bool boundsCheckingEnabled;
    };
    
    // Memory leak detector
    class LeakDetector {
    public:
        static void startDetection();
        static void stopDetection();
        static void reportLeaks();
        static size_t getAllocationCount();
        static size_t getDeallocationCount();
    };
}

// Global memory management functions
namespace memory {
    void initializeMemorySystem();
    void shutdownMemorySystem();
    void setMemoryLimits(size_t maxAllocations, size_t maxMemory);
    size_t getCurrentMemoryUsage();
    size_t getPeakMemoryUsage();
    void resetMemoryStats();
}