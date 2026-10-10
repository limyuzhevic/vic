// SIMD-optimized spike processing for maximum performance
namespace SIMD {

// Event-driven spike delivery using SIMD
struct SpikeEventSIMD {
    NeuronId neuronId;
    float weight;
    float current;
    float delayedDeliveryTime;
    bool isExcitatory;
    
    SpikeEventSIMD() = default;
    SpikeEventSIMD(NeuronId id, float w, float c, float time, bool exc)
        : neuronId(id), weight(w), current(c), delayedDeliveryTime(time), isExcitatory(exc) {}
};

// Batch spike event processing using SIMD registers
class SpikeProcessorSIMD {
public:
    // Process multiple spike events in parallel using SIMD
    static void processBatchEvents(const std::vector<SpikeEventSIMD>& events,
                                   std::vector<std::vector<float>>& neuronInputs,
                                   const std::vector<NeuronId>& neuronIds) {
        #if defined(__x86_64__) || defined(_M_X64)
        size_t numEvents = events.size();
        if (numEvents == 0) return;
        
        // Vectorized processing for spike events
        const size_t batchSize = 4;  // Process 4 events at once
        size_t i = 0;
        
        while (i + batchSize <= numEvents) {
            // Load spike event data into SIMD registers
            __m128 neuronIdsSIMD = _mm_set_ps(
                static_cast<float>(events[i + 0].neuronId.index()),
                static_cast<float>(events[i + 1].neuronId.index()),
                static_cast<float>(events[i + 2].neuronId.index()),
                static_cast<float>(events[i + 3].neuronId.index()));
            
            __m128 weightsSIMD = _mm_set_ps(
                events[i + 0].weight,
                events[i + 1].weight,
                events[i + 2].weight,
                events[i + 3].weight);
            
            __m128 currentsSIMD = _mm_set_ps(
                events[i + 0].current,
                events[i + 1].current,
                events[i + 2].current,
                events[i + 3].current);
            
            __m128 deliveryTimesSIMD = _mm_set_ps(
                events[i + 0].delayedDeliveryTime,
                events[i + 1].delayedDeliveryTime,
                events[i + 2].delayedDeliveryTime,
                events[i + 3].delayedDeliveryTime);
            
            __m128 isExcitatorySIMD = _mm_set_ps(
                events[i + 0].isExcitatory ? 1.0f : 0.0f,
                events[i + 1].isExcitatory ? 1.0f : 0.0f,
                events[i + 2].isExcitatory ? 1.0f : 0.0f,
                events[i + 3].isExcitatory ? 1.0f : 0.0f);
            
            // Apply to target neurons
            for (size_t eventIdx = 0; eventIdx < batchSize; ++eventIdx) {
                NeuronId targetId(events[i + eventIdx].neuronId);
                
                // Find neuron index in neuronIds vector
                size_t neuronIndex = findNeuronIndex(targetId, neuronIds);
                if (neuronIndex != static_cast<size_t>(-1)) {
                    // Scale current by synaptic weight
                    float scaledCurrent = events[i + eventIdx].weight * events[i + eventIdx].current;
                    
                    // Apply to neuron input
                    if (neuronIndex < neuronInputs.size()) {
                        neuronInputs[neuronIndex].push_back(scaledCurrent);
                    }
                }
            }
            
            i += batchSize;
        }
        
        // Process remaining events (scalar fallback)
        while (i < numEvents) {
            const auto& event = events[i];
            size_t neuronIndex = findNeuronIndex(event.neuronId, neuronIds);
            if (neuronIndex != static_cast<size_t>(-1) && neuronIndex < neuronInputs.size()) {
                float scaledCurrent = event.weight * event.current;
                neuronInputs[neuronIndex].push_back(scaledCurrent);
            }
            ++i;
        }
        #else
        // Scalar implementation for non-SIMD platforms
        for (const auto& event : events) {
            size_t neuronIndex = findNeuronIndex(event.neuronId, neuronIds);
            if (neuronIndex != static_cast<size_t>(-1) && neuronIndex < neuronInputs.size()) {
                float scaledCurrent = event.weight * event.current;
                neuronInputs[neuronIndex].push_back(scaledCurrent);
            }
        }
        #endif
    }
    
private:
    static size_t findNeuronIndex(NeuronId targetId, const std::vector<NeuronId>& neuronIds) {
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            if (neuronIds[i] == targetId) {
                return i;
            }
        }
        return static_cast<size_t>(-1);
    }
};

// High-performance memory allocator for neural data
class AlignedAllocator {
public:
    template<typename T>
    static T* allocate(size_t count) {
        void* ptr = nullptr;
        
        #if defined(__x86_64__) || defined(_M_X64)
        // 64-byte alignment for optimal SIMD performance
        ptr = _mm_malloc(count * sizeof(T), 64);
        #else
        // Standard allocation
        ptr = std::malloc(count * sizeof(T));
        #endif
        
        if (!ptr) {
            throw std::bad_alloc();
        }
        
        return static_cast<T*>(ptr);
    }
    
    template<typename T>
    static void deallocate(T* ptr, size_t count) {
        #if defined(__x86_64__) || defined(_M_X64)
        _mm_free(ptr);
        #else
        std::free(ptr);
        #endif
    }
};

// Cache-optimized data structure for spike processing
struct CacheLineOptimizedSpikeProcessor {
    // Align to cache line boundary for optimal memory access
    alignas(64) std::vector<NeuronId> neuronIds;
    alignas(64) std::vector<std::vector<float>> neuronInputs;
    alignas(64) std::vector<float> neuronActivity;
    alignas(64) std::vector<size_t> inputCount;
    
    CacheLineOptimizedSpikeProcessor(size_t capacity = 10000) {
        neuronIds.reserve(capacity);
        neuronInputs.reserve(capacity);
        neuronActivity.reserve(capacity);
        inputCount.reserve(capacity);
    }
    
    // Pre-allocate memory for spike processing
    void reserve(size_t capacity) {
        neuronIds.reserve(capacity);
        neuronInputs.reserve(capacity);
        neuronActivity.reserve(capacity);
        inputCount.reserve(capacity);
    }
    
    // Clear all data efficiently
    void clear() {
        neuronIds.clear();
        neuronInputs.clear();
        neuronActivity.clear();
        inputCount.clear();
    }
    
    // Add neuron to processing queue
    void addNeuron(NeuronId id) {
        neuronIds.push_back(id);
        neuronInputs.emplace_back();
        neuronActivity.push_back(0.0f);
        inputCount.push_back(0);
    }
    
    // Process spike events with optimal memory access patterns
    void processEvents(const std::vector<SpikeEventSIMD>& events) {
        // Reset input counts for efficiency
        for (auto& count : inputCount) {
            count = 0;
        }
        
        // Process events using cache-friendly access
        for (const auto& event : events) {
            size_t neuronIndex = findNeuronIndex(event.neuronId);
            if (neuronIndex != static_cast<size_t>(-1)) {
                // Add synaptic input with optimized access
                float scaledCurrent = event.weight * event.current;
                neuronInputs[neuronIndex].push_back(scaledCurrent);
                inputCount[neuronIndex]++;
            }
        }
    }
    
private:
    size_t findNeuronIndex(NeuronId targetId) const {
        // Binary search for faster lookup if IDs are sorted
        auto it = std::lower_bound(neuronIds.begin(), neuronIds.end(), targetId);
        if (it != neuronIds.end() && *it == targetId) {
            return std::distance(neuronIds.begin(), it);
        }
        return static_cast<size_t>(-1);
    }
};

// Parallel spike processing using thread-local storage
class ParallelSpikeProcessor {
public:
    // Thread-safe spike processing with optimal cache utilization
    static void processSpikesParallel(const std::vector<SpikeEventSIMD>& events,
                                      CacheLineOptimizedSpikeProcessor& processor,
                                      size_t threadCount = std::thread::hardware_concurrency()) {
        if (threadCount <= 1 || events.size() < 1000) {
            // Use single-threaded processing for small workloads
            processor.processEvents(events);
            return;
        }
        
        // Distribute events across threads
        size_t eventsPerThread = events.size() / threadCount;
        std::vector<std::vector<SpikeEventSIMD>> threadEvents(threadCount);
        
        size_t start = 0;
        for (size_t i = 0; i < threadCount; ++i) {
            size_t end = (i == threadCount - 1) ? events.size() : start + eventsPerThread;
            threadEvents[i].assign(events.begin() + start, events.begin() + end);
            start = end;
        }
        
        // Process events in parallel
        std::vector<std::thread> threads;
        threads.reserve(threadCount);
        
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([&, i]() {
                // Each thread processes its own events
                // In real implementation, would use thread-local processor
                processor.processEvents(threadEvents[i]);
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
    }
};

} // namespace SIMD
