#pragma once

#include <chrono>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <atomic>

namespace nlm {

// Performance monitoring and profiling
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        uint64_t totalSimulationSteps = 0;
        float averageStepTime = 0.0f;
        float simdEfficiency = 0.0f;
        float memoryEfficiency = 0.0f;
        uint64_t neuronUpdates = 0;
        uint64_t synapseUpdates = 0;
        uint64_t spikePropagations = 0;
        float cpuUtilization = 0.0f;
        uint64_t memoryUsage = 0;
        uint64_t peakMemoryUsage = 0;
        
        void reset() {
            *this = PerformanceMetrics{};
        }
    };
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor monitor;
        return monitor;
    }
    
    void recordStepTime(float time) {
        std::lock_guard<std::mutex> lock(mutex);
        totalStepTime += time;
        totalSteps++;
        averageStepTime = totalStepTime / totalSteps;
    }
    
    void recordSIMDEfficiency(float efficiency) {
        std::lock_guard<std::mutex> lock(mutex);
        simdEfficiency = (simdEfficiency * recordedSIMDEfficiencies.size() + efficiency) / 
                        (recordedSIMDEfficiencies.size() + 1);
        recordedSIMDEfficiencies.push_back(efficiency);
        if (recordedSIMDEfficiencies.size() > 100) {
            recordedSIMDEfficiencies.erase(recordedSIMDEfficiencies.begin());
        }
    }
    
    void recordMemoryUsage(uint64_t usage) {
        std::lock_guard<std::mutex> lock(mutex);
        memoryUsage = usage;
        if (usage > peakMemoryUsage) peakMemoryUsage = usage;
    }
    
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex);
        PerformanceMetrics metrics = currentMetrics;
        metrics.averageStepTime = totalSteps > 0 ? totalStepTime / totalSteps : 0.0f;
        metrics.simdEfficiency = recordedSIMDEfficiencies.empty() ? 0.0f : simdEfficiency;
        return metrics;
    }
    
    void updateMetrics() {
        std::lock_guard<std::mutex> lock(mutex);
        currentMetrics.totalSimulationSteps = totalSteps;
        currentMetrics.simdEfficiency = simdEfficiency;
        currentMetrics.memoryEfficiency = static_cast<float>(peakMemoryUsage) / 
                                         (totalMemoryAllocated > 0 ? totalMemoryAllocated : 1);
        currentMetrics.neuronUpdates = neuronUpdates.load();
        currentMetrics.synapseUpdates = synapseUpdates.load();
        currentMetrics.spikePropagations = spikePropagations.load();
    }
    
    void printMetrics() const {
        PerformanceMetrics metrics = getMetrics();
        std::cout << "=== Performance Metrics ===" << std::endl;
        std::cout << "Total Simulation Steps: " << metrics.totalSimulationSteps << std::endl;
        std::cout << "Average Step Time: " << metrics.averageStepTime * 1000 << " ms" << std::endl;
        std::cout << "SIMD Efficiency: " << metrics.simdEfficiency * 100 << "%" << std::endl;
        std::cout << "Memory Efficiency: " << metrics.memoryEfficiency * 100 << "%" << std::endl;
        std::cout << "Neuron Updates: " << metrics.neuronUpdates << std::endl;
        std::cout << "Synapse Updates: " << metrics.synapseUpdates << std::endl;
        std::cout << "Spike Propagations: " << metrics.spikePropagations << std::endl;
        std::cout << "Current Memory Usage: " << metrics.memoryUsage / 1024 / 1024 << " MB" << std::endl;
        std::cout << "Peak Memory Usage: " << metrics.peakMemoryUsage / 1024 / 1024 << " MB" << std::endl;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        totalSteps = 0;
        totalStepTime = 0.0f;
        simdEfficiency = 0.0f;
        recordedSIMDEfficiencies.clear();
        neuronUpdates = 0;
        synapseUpdates = 0;
        spikePropagations = 0;
        memoryUsage = 0;
        peakMemoryUsage = 0;
    }
    
    // Increment counters for different operations
    void incrementNeuronUpdates(uint64_t count = 1) { neuronUpdates += count; }
    void incrementSynapseUpdates(uint64_t count = 1) { synapseUpdates += count; }
    void incrementSpikePropagations(uint64_t count = 1) { spikePropagations += count; }
    
private:
    PerformanceMonitor() : totalSteps(0), totalStepTime(0.0f), simdEfficiency(0.0f),
                           neuronUpdates(0), synapseUpdates(0), spikePropagations(0),
                           memoryUsage(0), peakMemoryUsage(0), totalMemoryAllocated(0) {}
    
    mutable std::mutex mutex;
    uint64_t totalSteps = 0;
    float totalStepTime = 0.0f;
    float simdEfficiency = 0.0f;
    std::vector<float> recordedSIMDEfficiencies;
    std::atomic<uint64_t> neuronUpdates{0};
    std::atomic<uint64_t> synapseUpdates{0};
    std::atomic<uint64_t> spikePropagations{0};
    uint64_t memoryUsage = 0;
    uint64_t peakMemoryUsage = 0;
    uint64_t totalMemoryAllocated = 0;
    
    PerformanceMetrics currentMetrics;
};

// Modern C++20 features implementation
namespace cpp20 {
    // Concepts for compile-time safety
    template<typename T>
    concept NeuronStateConcept = requires(T t) {
        t.membranePotential;
        t.restingPotential;
        t.threshold;
        t.leakConductance;
        t.firingState;
        t.refractoryRemaining;
    };
    
    // Example concept usage for type safety
    template<typename T>
    concept SynapseConcept = requires(T t) {
        t.weight;
        t.delay;
        t.type;
        t.recordPreSpike;
        t.recordPostSpike;
    };
    
    // Structured bindings for better code readability
    struct NeuronDynamicsResult {
        float membranePotential;
        bool shouldFire;
        float firingRate;
        float adaptationVariable;
        
        // Helper methods
        void print() const {
            std::cout << "Membrane Potential: " << membranePotential << " mV" << std::endl;
            std::cout << "Should Fire: " << (shouldFire ? "Yes" : "No") << std::endl;
            std::cout << "Firing Rate: " << firingRate << " Hz" << std::endl;
            std::cout << "Adaptation Variable: " << adaptationVariable << std::endl;
        }
    };
    
    // C++20 ranges utilities
    class RangeUtilities {
    public:
        // Create a filtered view of neurons based on firing state
        template<typename Container>
        static auto neuronsByFiringState(const Container& states, FiringState state) {
            return std::views::filter([&state](const auto& s) { return s.firingState == state; });
        }
        
        // Create a filtered view of neurons based on firing rate
        template<typename Container>
        static auto neuronsByFiringRate(const Container& states, float minRate) {
            return std::views::filter([&minRate](const auto& s) { return s.firingRate >= minRate; });
        }
        
        // Create a sorted view by membrane potential
        template<typename Container>
        static auto neuronsByMembranePotential(const Container& states, bool ascending = true) {
            auto sorted = std::views::all(states);
            if (ascending) {
                return sorted | std::views::transform(&NeuronState::membranePotential) | 
                       std::views::common;
            } else {
                return sorted | std::views::transform(&NeuronState::membranePotential) | 
                       std::views::reverse | std::views::common;
            }
        }
        
        // Create a chunked view for parallel processing
        template<typename Container>
        static auto chunkView(const Container& container, size_t chunkSize) {
            return std::views::chunk(container, chunkSize);
        }
    };
    
    // constexpr functions for compile-time computations
    class MathConstants {
    public:
        static constexpr float PI = 3.14159265358979323846f;
        static constexpr float E = 2.71828182845904523536f;
        static constexpr float GOLDEN_RATIO = 1.61803398874989484820f;
        
        // Compute LIF time constant at compile time
        static constexpr float computeLIFTimeConstant(float capacitance, float conductance) {
            return capacitance / conductance;
        }
        
        // Compute membrane potential update at compile time
        static constexpr float computeMembranePotential(float previousPotential, float current,
                                                        float leakConductance, float dt) {
            return previousPotential + (current - previousPotential) * (dt / 20.0f) -
                   leakConductance * previousPotential * dt;
        }
        
        // Compute exponential decay at compile time
        static constexpr float computeExponentialDecay(float value, float timeConstant, float dt) {
            return value * std::exp(-dt / timeConstant);
        }
    };
    
    // Example coroutine for async neural simulation
    class AsyncNeuralSimulation {
    public:
        struct SimulationStep {
            SimulationStep nlm::SimulationStep step;
            Timestamp time;
            std::vector<NeuronId> activeNeurons;
            std::vector<NeuronId> firingNeurons;
            std::vector<std::pair<NeuronId, NeuronId>> synapticEvents;
        };
        
        // Coroutine that yields simulation steps
        co_generator<SimulationStep> runSimulation() {
            for (nlm::SimulationStep step = 0; step < maxSteps; ++step) {
                co_yield SimulationStep{
                    step,
                    static_cast<Timestamp>(step),
                    getActiveNeurons(),
                    getFiringNeurons(),
                    getSynapticEvents()
                };
            }
        }
        
        // Coroutine for memory management
        co_generator<std::pair<Neuron*, Synapse*>> allocateResources(size_t count) {
            for (size_t i = 0; i < count; ++i) {
                Neuron* neuron = MemoryManager::getInstance().allocateNeuron(
                    NeuronId(i), NeuronType::Internal);
                Synapse* synapse = MemoryManager::getInstance().allocateSynapse(
                    SynapseId(i), NeuronId(i), NeuronId(i + 1));
                co_yield std::make_pair(neuron, synapse);
            }
        }
        
        // Coroutine for performance benchmarking
        co_generator<float> benchmarkOperations() {
            for (int i = 0; i < benchmarkIterations; ++i) {
                float time = runBenchmarkTest();
                co_yield time;
            }
        }
        
    private:
        const nlm::SimulationStep maxSteps = 1000;
        const size_t benchmarkIterations = 100;
        
        std::vector<NeuronId> getActiveNeurons() const {
            std::vector<NeuronId> active;
            // Implementation would get active neurons from the brain
            return active;
        }
        
        std::vector<NeuronId> getFiringNeurons() const {
            std::vector<NeuronId> firing;
            // Implementation would get firing neurons from the brain
            return firing;
        }
        
        std::vector<std::pair<NeuronId, NeuronId>> getSynapticEvents() const {
            std::vector<std::pair<NeuronId, NeuronId>> events;
            // Implementation would get synaptic events from the brain
            return events;
        }
        
        float runBenchmarkTest() {
            // Implementation would run a benchmark test
            return 0.0f;
        }
    };
    
    // Usage examples and demonstrations
    void demonstrateC++20Features() {
        std::cout << "=== C++20 Features Demonstration ===" << std::endl;
        
        // Demonstrate concepts
        std::cout << "1. Concepts for type safety:" << std::endl;
        std::cout << "   - NeuronStateConcept ensures type has required members" << std::endl;
        std::cout << "   - SynapseConcept ensures type has required methods" << std::endl;
        
        // Demonstrate structured bindings
        std::cout << std::endl << "2. Structured bindings for readability:" << std::endl;
        NeuronDynamicsResult result{
            membranePotential: -65.0f,
            shouldFire: true,
            firingRate: 50.0f,
            adaptationVariable: 0.5f
        };
        result.print();
        
        // Demonstrate ranges
        std::cout << std::endl << "3. C++20 ranges for neural computation:" << std::endl;
        std::cout << "   - neuronsByFiringState: Filter neurons by state" << std::endl;
        std::cout << "   - neuronsByFiringRate: Filter neurons by rate threshold" << std::endl;
        std::cout << "   - neuronsByMembranePotential: Sort by membrane potential" << std::endl;
        
        // Demonstrate constexpr functions
        std::cout << std::endl << "4. Compile-time computations:" << std::endl;
        std::cout << "   - MathConstants::PI = " << MathConstants::PI << std::endl;
        std::cout << "   - MathConstants::GOLDEN_RATIO = " << MathConstants::GOLDEN_RATIO << std::endl;
        std::cout << "   - Compute LIF time constant at compile time: " 
                  << MathConstants::computeLIFTimeConstant(1.0f, 0.05f) << " ms" << std::endl;
        
        // Demonstrate coroutines
        std::cout << std::endl << "5. Asynchronous neural simulation:" << std::endl;
        std::cout << "   - runSimulation(): Yields simulation steps" << std::endl;
        std::cout << "   - allocateResources(): Coroutine for resource management" << std::endl;
        std::cout << "   - benchmarkOperations(): Coroutine for performance testing" << std::endl;
    }
}

// SIMD runtime detection and dispatch
namespace simd {
    struct SIMDInfo {
        bool sse2 = false;
        bool sse3 = false;
        bool ssse3 = false;
        bool sse4_1 = false;
        bool sse4_2 = false;
        bool avx = false;
        bool avx2 = false;
        bool avx512f = false;
        bool neon = false;
    };
    
    SIMDInfo detectSIMD();
    
    // Optimization flags
    enum SIMDFlags {
        SSE2 = 0x01,
        SSE3 = 0x02,
        SSSE3 = 0x04,
        SSE4_1 = 0x08,
        SSE4_2 = 0x10,
        AVX = 0x20,
        AVX2 = 0x40,
        AVX512 = 0x80
    };
    
    uint32_t currentSIMDFlags;
    
    void initializeSIMD();
}

// Memory management optimizations
class MemoryOptimizer {
public:
    // Memory pool statistics
    struct MemoryPoolStats {
        uint64_t allocations = 0;
        uint64_t deallocations = 0;
        uint64_t hits = 0;
        uint64_t misses = 0;
        uint64_t memoryAllocated = 0;
        uint64_t memoryUsed = 0;
    };
    
    // Cache line optimization
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t ALIGNMENT = 64;
    
    // Memory alignment helpers
    static bool isAligned(const void* ptr, size_t alignment = ALIGNMENT);
    static void* alignedAllocate(size_t size, size_t alignment = ALIGNMENT);
    static void alignedDeallocate(void* ptr, size_t size);
    
    // Prefetch optimization
    static inline void prefetchRead(const void* ptr, int locality = 3) {
        _mm_prefetch(static_cast<const char*>(ptr), _MM_HINT_T0 + locality);
    }
    
    static inline void prefetchWrite(const void* ptr, int locality = 3) {
        _mm_prefetch(static_cast<const char*>(ptr), _MM_HINT_T1 + locality);
    }
    
    // Memory usage tracking
    static MemoryPoolStats getNeuronPoolStats();
    static MemoryPoolStats getSynapsePoolStats();
    
    // Global memory manager access
    static class MemoryManager& getMemoryManager();
};

// Parallel processing optimizations
class ParallelProcessor {
public:
    // Thread pool for parallel neural computation
    struct ThreadPool {
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex mutex;
        std::condition_variable cv;
        bool stop;
        
        ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
        ~ThreadPool();
        
        void submit(std::function<void()> task);
        void waitForCompletion();
        size_t getThreadCount() const { return threads.size(); }
    };
    
    // Thread-local storage for per-thread optimizations
    struct ThreadLocalStorage {
        std::vector<NeuronState> neuronStates;
        std::vector<float> synapticInputs;
        std::vector<float> currents;
        std::vector<NeuronId> activeNeurons;
        
        ThreadLocalStorage() {
            neuronStates.reserve(1024);
            synapticInputs.reserve(1024);
            currents.reserve(1024);
            activeNeurons.reserve(1024);
        }
    };
    
    static thread_local ThreadLocalStorage threadLocalStorage;
    
    // Parallel processing of neural computations
    static void processNeuronsParallel(const std::vector<Neuron*>& neurons, float dt);
    static void processSynapsesParallel(const std::vector<Synapse*>& synapses, float dt);
    static void propagateSpikesParallel(const std::vector<Neuron*>& neurons, float dt);
    
    // Parallel spike detection and propagation
    static void detectSpikesParallel(const std::vector<Neuron*>& neurons, std::vector<bool>& shouldFire);
    static void propagateSpikesParallel(const std::vector<Neuron*>& neurons, 
                                       const std::vector<bool>& shouldFire, float dt);
    
    // Statistics tracking
    struct ParallelStats {
        uint64_t parallelOperations = 0;
        uint64_t threadSwitches = 0;
        float parallelEfficiency = 0.0f;
    };
    
    static ParallelStats& getStats() { static ParallelStats stats; return stats; }
};

// Compilation optimization flags
namespace compilation {
    // Compiler optimization flags
    struct OptimizationFlags {
        bool enableSIMD = true;
        bool enablePGO = false;
        bool enableLTO = false;
        bool enableProfile = false;
        bool optimizeForCache = true;
        bool optimizeForMemory = true;
        bool optimizeForParallel = true;
    };
    
    // Profile-guided optimization support
    class PGOSupport {
    public:
        static void initialize();
        static void finalize();
        static void generateProfileData();
        static void applyProfileData();
        
        // Profile data structures
        struct ProfileData {
            std::vector<uint64_t> branchCounters;
            std::vector<uint64_t> functionCallCounters;
            std::vector<float> executionTimes;
            std::vector<uint64_t> cacheMissCounters;
        };
        
        static ProfileData getProfileData();
        static void setProfileData(const ProfileData& data);
    };
    
    // Link-time optimization support
    class LTOSupport {
    public:
        static void initialize();
        static void finalize();
        static bool isAvailable();
        
        // LTO optimization levels
        enum OptimizationLevel {
            LTO_NONE,
            LTO_BASIC,
            LTO_FULL
        };
        
        static void setOptimizationLevel(OptimizationLevel level);
        static OptimizationLevel getOptimizationLevel();
    };
    
    // Hardware-specific optimizations
    namespace hardware {
        struct CPUInfo {
            std::string brand;
            uint32_t cacheSizeL1 = 0;
            uint32_t cacheSizeL2 = 0;
            uint32_t cacheSizeL3 = 0;
            uint32_t coreCount = 0;
            float frequency = 0.0f;
            bool supportsAVX = false;
            bool supportsAVX2 = false;
            bool supportsAVX512 = false;
            bool supportsNEON = false;
        };
        
        CPUInfo getCPUInfo();
        
        // CPU-specific optimizations
        void optimizeForCPU(const CPUInfo& cpu);
        bool isOptimizationAvailable(const CPUInfo& cpu, SIMDFlags flag);
    }
}

} // namespace nlm

// Include implementation headers
#include "MemoryManager.hpp"
#include "NeuronSIMD.hpp"
#include "SynapseSIMD.hpp"
#include "PerformanceMonitor.hpp"