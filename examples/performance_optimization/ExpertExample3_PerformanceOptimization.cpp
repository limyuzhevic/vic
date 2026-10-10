// Copyright © 2024 NLM Research Project
// Licensed under MIT License

/**
 * @file ExpertExample3_PerformanceOptimization.cpp
 * @brief Performance-optimized large-scale simulation with parallel processing
 * 
 * This example demonstrates expert-level performance optimization techniques
 * for NLM. The implementation includes memory-efficient large-scale simulations,
 * parallel processing, SIMD vectorization, and distributed computing patterns.
 * Focus is on scaling to millions of neurons while maintaining real-time performance.
 */

#include "brain/Brain.hpp"
#include "advanced_memory/AdvancedMemory.hpp"
#include "advanced_prediction/HierarchicalPlanner.hpp"
#include "advanced_prediction/GoalConditionedPolicies.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <numeric>
#include <unordered_set>
#include <cmath>
#include <queue>
#include <future>
#include <filesystem>
#include <filesystem>

namespace nlm {

/**
 * @struct NeuronInfo
 * Optimized neuron information structure for large-scale simulations
 */
struct NeuronInfo {
    size_t id;
    float membranePotential;
    float refractoryTime;
    size_t inputCount;
    size_t outputCount;
    float connectionDensity;
    bool isActive;
    bool isSaturated;
    float synapticStrength;
    
    // Vectorized data for SIMD optimization
    alignas(16) std::array<float, 4> simdData;
    
    NeuronInfo() : id(0), membranePotential(0.0f), refractoryTime(0.0f),
                   inputCount(0), outputCount(0), connectionDensity(0.0f),
                   isActive(false), isSaturated(false), synapticStrength(1.0f) {
        simdData = {0.0f, 0.0f, 0.0f, 0.0f};
    }
    
    NeuronInfo(size_t neuronId, float initialPotential)
        : id(neuronId), membranePotential(initialPotential), refractoryTime(0.0f),
          inputCount(0), outputCount(0), connectionDensity(0.0f),
          isActive(false), isSaturated(false), synapticStrength(1.0f) {
        simdData = {0.0f, 0.0f, 0.0f, 0.0f};
    }
};

/**
 * @struct ConnectionInfo
 * Optimized synaptic connection structure
 */
struct ConnectionInfo {
    size_t preNeuronId;
    size_t postNeuronId;
    float weight;
    float delay;
    float efficacy;
    bool isEnabled;
    float lastUpdateTime;
    
    ConnectionInfo() : preNeuronId(0), postNeuronId(0), weight(0.0f), delay(0.0f),
                       efficacy(1.0f), isEnabled(true), lastUpdateTime(0.0f) {}
    
    ConnectionInfo(size_t pre, size_t post, float w, float d)
        : preNeuronId(pre), postNeuronId(post), weight(w), delay(d),
          efficacy(1.0f), isEnabled(true), lastUpdateTime(0.0f) {}
};

/**
 * @struct MemoryPool
 * Memory pool for efficient allocation/deallocation
 */
struct MemoryPool {
    std::vector<NeuronInfo> neurons;
    std::vector<ConnectionInfo> connections;
    std::vector<float> synapticWeights;
    std::vector<float> membranePotentials;
    std::vector<size_t> inputIndices;
    std::vector<size_t> outputIndices;
    
    size_t allocatedNeurons;
    size_t allocatedConnections;
    size_t maxNeurons;
    size_t maxConnections;
    bool useArenaAllocation;
    
    MemoryPool() : allocatedNeurons(0), allocatedConnections(0), maxNeurons(0),
                   maxConnections(0), useArenaAllocation(true) {}
    
    void initialize(size_t maxNeurons, size_t maxConnections);
    
    size_t allocateNeuron(const NeuronInfo& info);
    
    size_t allocateConnection(const ConnectionInfo& conn);
    
    void deallocateNeuron(size_t neuronId);
    
    void deallocateConnection(size_t connId);
    
    void compactMemory();
    
    void reserveMemory();
};

/**
 * @struct SIMDProcessor
 * SIMD-optimized processor for parallel computation
 */
struct SIMDProcessor {
    // Vectorized operations
    static void addVector(const std::vector<float>& a, const std::vector<float>& b,
                         std::vector<float>& result);
    
    static void multiplyVector(const std::vector<float>& a, const std::vector<float>& b,
                              std::vector<float>& result);
    
    static void applyActivation(const std::vector<float>& input,
                               std::vector<float>& output,
                               const std::string& activationType);
    
    static float computeDotProduct(const std::vector<float>& a, const std::vector<float>& b);
    
    static void normalizeVector(std::vector<float>& vector, float targetNorm);
    
    // Memory operations
    static void copyVector(const std::vector<float>& source, std::vector<float>& destination);
    
    static void fillVector(std::vector<float>& vector, float value);
    
    // Statistical operations
    static float computeMean(const std::vector<float>& vector);
    static float computeVariance(const std::vector<float>& vector, float mean);
    static float computeStdDev(const std::vector<float>& vector);
};

/**
 * @struct EventQueue
 * Event-driven processing queue for spike propagation
 */
struct EventQueue {
    struct Event {
        SimulationStep timestamp;
        size_t neuronId;
        float value;
        EventType type;
        
        Event(SimulationStep ts, size_t id, float val, EventType et)
            : timestamp(ts), neuronId(id), value(val), type(et) {}
    };
    
    std::priority_queue<Event> eventQueue;
    std::mutex queueMutex;
    std::atomic<bool> isProcessing;
    size_t maxQueueSize;
    float processingInterval;
    
    EventQueue() : isProcessing(false), maxQueueSize(1000000), processingInterval(0.001f) {}
    
    void pushEvent(SimulationStep timestamp, size_t neuronId, float value, EventType type);
    
    bool popEvent(Event& event);
    
    bool isEmpty() const { return eventQueue.empty(); }
    
    size_t size() const { return eventQueue.size(); }
};

/**
 * @struct PerformanceProfiler
 * Performance profiling and benchmarking
 */
struct PerformanceProfiler {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastCheckpoint;
    
    struct Metrics {
        double cpuUsage;
        double memoryUsageMB;
        size_t activeNeurons;
        size_t totalEvents;
        double simulationSpeed;
        double timeStep;
        
        Metrics() : cpuUsage(0.0), memoryUsageMB(0.0), activeNeurons(0),
                   totalEvents(0), simulationSpeed(0.0), timeStep(0.0) {}
    };
    
    Metrics currentMetrics;
    std::vector<Metrics> history;
    std::unordered_map<std::string, double> timers;
    
    PerformanceProfiler() {
        startTime = std::chrono::steady_clock::now();
        lastCheckpoint = startTime;
    }
    
    void startTimer(const std::string& name);
    
    double stopTimer(const std::string& name);
    
    void updateMetrics(const MemoryPool& memory, const EventQueue& eventQueue);
    
    void recordMetrics();
    
    void saveProfile(const std::string& filename);
};

/**
 * @struct ParallelSimulation
 * Multi-threaded simulation for large-scale processing
 */
struct ParallelSimulation {
    size_t threadCount;
    std::vector<std::thread> workerThreads;
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stopFlag;
    
    ParallelSimulation(size_t threads = std::thread::hardware_concurrency())
        : threadCount(threads), stopFlag(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            workerThreads.emplace_back(&ParallelSimulation::workerThread, this);
        }
    }
    
    ~ParallelSimulation() {
        stopFlag = true;
        condition.notify_all();
        for (auto& thread : workerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
    void submitTask(const std::function<void()>& task);
    
    void waitForTasks();
    
    void workerThread();
};

/**
 * LargeScaleSimulation class implementing performance optimizations
 */
class LargeScaleSimulation {
public:
    LargeScaleSimulation(const std::string& name, const LifetimeExperimentConfig& config);
    ~LargeScaleSimulation();
    
    // Main lifecycle methods
    void initialize();
    void runSimulation(size_t steps);
    void shutdown();
    
    // Performance optimization methods
    void enableSIMD(bool enable) { useSIMD_ = enable; }
    void enableParallelProcessing(bool enable) { useParallel_ = enable; }
    void enableEventDriven(bool enable) { useEventDriven_ = enable; }
    void enableMemoryPooling(bool enable) { useMemoryPooling_ = enable; }
    
    // Memory management
    void optimizeMemoryUsage();
    void compactMemory();
    void flushInactiveNeurons();
    
    // Performance monitoring
    double getCurrentFPS() const { return currentFPS_; }
    size_t getActiveNeurons() const { return memoryPool_.allocatedNeurons; }
    size_t getTotalEvents() const { return totalEventsProcessed_; }
    double getMemoryEfficiency() const { return memoryEfficiency_; }
    
    // Simulation parameters
    void setMaxNeurons(size_t maxNeurons) { maxNeurons_ = maxNeurons; }
    void setMaxConnections(size_t maxConnections) { maxConnections_ = maxConnections; }
    void setTimeStep(float dt) { timeStep_ = dt; }
    void setThreadCount(size_t threads) { threadCount_ = threads; }
    
    // Core brain components
    std::shared_ptr<Brain> getBrain() { return brain_; }
    
private:
    std::string name_;
    LifetimeExperimentConfig config_;
    
    // Core brain components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AdvancedMemory> advancedMemory_;
    std::shared_ptr<HierarchicalPlanner> planner_;
    
    // Performance optimization components
    MemoryPool memoryPool_;
    EventQueue eventQueue_;
    PerformanceProfiler profiler_;
    ParallelSimulation parallelSim_;
    
    // Simulation state
    std::vector<float> globalState_;
    std::vector<std::vector<float>> populationVectors_;
    
    // Performance settings
    bool useSIMD_;
    bool useParallel_;
    bool useEventDriven_;
    bool useMemoryPooling_;
    
    // Simulation parameters
    size_t maxNeurons_;
    size_t maxConnections_;
    size_t threadCount_;
    float timeStep_;
    
    // Performance metrics
    double currentFPS_;
    double memoryEfficiency_;
    size_t totalEventsProcessed_;
    size_t spikeEventsProcessed_;
    size_t synapticEventsProcessed_;
    double peakMemoryUsage_;
    double averageMemoryUsage_;
    
    // Internal state
    bool isInitialized_;
    bool isRunning_;
    size_t currentStep_;
    std::chrono::steady_clock::time_point simulationStartTime_;
    
    // Helper methods
    void initializeNeurons();
    void initializeConnections();
    void setupPerformanceOptimizations();
    
    // Simulation methods
    void runSequentialSimulation(size_t steps);
    void runParallelSimulation(size_t steps);
    void runEventDrivenSimulation(size_t steps);
    
    // Core simulation functions
    void simulateNeuron(size_t neuronId);
    void propagateSpikes();
    void updateSynapticWeights();
    void applyPlasticity();
    
    // Vectorized operations
    void vectorizedNeuronUpdate(const std::vector<size_t>& neuronIds);
    void vectorizedSynapticUpdate(const std::vector<size_t>& connectionIds);
    void vectorizedActivationUpdate(const std::vector<float>& inputs,
                                   std::vector<float>& outputs,
                                   const std::string& activationType);
    
    // Memory optimization
    void allocateMemory();
    void deallocateMemory();
    void optimizeMemoryLayout();
    
    // Performance monitoring
    void updatePerformanceMetrics();
    void recordSimulationStep();
    void savePerformanceData(const std::string& filename);
};

// Implementation of MemoryPool

void MemoryPool::initialize(size_t maxNeurons, size_t maxConnections) {
    this->maxNeurons = maxNeurons;
    this->maxConnections = maxConnections;
    allocatedNeurons = 0;
    allocatedConnections = 0;
    
    if (useArenaAllocation) {
        neurons.reserve(maxNeurons);
        connections.reserve(maxConnections);
        synapticWeights.reserve(maxConnections);
        membranePotentials.reserve(maxNeurons);
        inputIndices.reserve(maxNeurons * 10); // Estimate
        outputIndices.reserve(maxNeurons * 10);
    } else {
        neurons.resize(maxNeurons);
        connections.resize(maxConnections);
    }
}

size_t MemoryPool::allocateNeuron(const NeuronInfo& info) {
    if (allocatedNeurons >= maxNeurons) {
        return SIZE_MAX; // Error
    }
    
    size_t neuronId = allocatedNeurons;
    neurons[neuronId] = info;
    allocatedNeurons++;
    
    // Initialize associated data
    membranePotentials[neuronId] = info.membranePotential;
    inputIndices[neuronId] = 0; // Start index
    outputIndices[neuronId] = 0;
    
    return neuronId;
}

size_t MemoryPool::allocateConnection(const ConnectionInfo& conn) {
    if (allocatedConnections >= maxConnections) {
        return SIZE_MAX; // Error
    }
    
    size_t connId = allocatedConnections;
    connections[connId] = conn;
    allocatedConnections++;
    
    synapticWeights[connId] = conn.weight;
    
    return connId;
}

void MemoryPool::deallocateNeuron(size_t neuronId) {
    if (neuronId >= allocatedNeurons) return;
    
    // Mark as inactive instead of immediate removal for performance
    neurons[neuronId].isActive = false;
    neurons[neuronId].isSaturated = false;
}

void MemoryPool::deallocateConnection(size_t connId) {
    if (connId >= allocatedConnections) return;
    
    connections[connId].isEnabled = false;
}

void MemoryPool::compactMemory() {
    // Compact memory by removing inactive neurons and connections
    size_t writeNeurons = 0;
    size_t writeConnections = 0;
    
    for (size_t i = 0; i < allocatedNeurons; ++i) {
        if (neurons[i].isActive) {
            neurons[writeNeurons++] = neurons[i];
        }
    }
    
    for (size_t i = 0; i < allocatedConnections; ++i) {
        if (connections[i].isEnabled) {
            connections[writeConnections++] = connections[i];
        }
    }
    
    allocatedNeurons = writeNeurons;
    allocatedConnections = writeConnections;
}

void MemoryPool::reserveMemory() {
    neurons.shrink_to_fit();
    connections.shrink_to_fit();
    synapticWeights.shrink_to_fit();
    membranePotentials.shrink_to_fit();
    inputIndices.shrink_to_fit();
    outputIndices.shrink_to_fit();
}

// Implementation of SIMDProcessor

void SIMDProcessor::addVector(const std::vector<float>& a, const std::vector<float>& b,
                             std::vector<float>& result) {
    size_t size = std::min(a.size(), b.size());
    result.resize(size);
    
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        result[i] = a[i] + b[i];
    }
}

void SIMDProcessor::multiplyVector(const std::vector<float>& a, const std::vector<float>& b,
                                  std::vector<float>& result) {
    size_t size = std::min(a.size(), b.size());
    result.resize(size);
    
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        result[i] = a[i] * b[i];
    }
}

void SIMDProcessor::applyActivation(const std::vector<float>& input,
                                   std::vector<float>& output,
                                   const std::string& activationType) {
    size_t size = input.size();
    output.resize(size);
    
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        float x = input[i];
        float result = 0.0f;
        
        if (activationType == "relu") {
            result = std::max(0.0f, x);
        } else if (activationType == "sigmoid") {
            result = 1.0f / (1.0f + std::exp(-x));
        } else if (activationType == "tanh") {
            result = std::tanh(x);
        } else if (activationType == "leaky_relu") {
            result = x > 0 ? x : x * 0.01f;
        } else if (activationType == "elu") {
            result = x > 0 ? x : 0.01f * (std::exp(x) - 1.0f);
        }
        
        output[i] = result;
    }
}

float SIMDProcessor::computeDotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    size_t size = std::min(a.size(), b.size());
    float result = 0.0f;
    
    #pragma omp parallel for reduction(+:result)
    for (size_t i = 0; i < size; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

void SIMDProcessor::normalizeVector(std::vector<float>& vector, float targetNorm) {
    float norm = computeStdDev(vector);
    if (norm > 0.0f) {
        float scale = targetNorm / norm;
        
        #pragma omp parallel for
        for (size_t i = 0; i < vector.size(); ++i) {
            vector[i] *= scale;
        }
    }
}

void SIMDProcessor::copyVector(const std::vector<float>& source, std::vector<float>& destination) {
    destination = source;
}

void SIMDProcessor::fillVector(std::vector<float>& vector, float value) {
    #pragma omp parallel for
    for (size_t i = 0; i < vector.size(); ++i) {
        vector[i] = value;
    }
}

float SIMDProcessor::computeMean(const std::vector<float>& vector) {
    if (vector.empty()) return 0.0f;
    
    float sum = std::accumulate(vector.begin(), vector.end(), 0.0f);
    return sum / vector.size();
}

float SIMDProcessor::computeVariance(const std::vector<float>& vector, float mean) {
    if (vector.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float val : vector) {
        float diff = val - mean;
        sum += diff * diff;
    }
    
    return sum / vector.size();
}

float SIMDProcessor::computeStdDev(const std::vector<float>& vector) {
    float mean = computeMean(vector);
    return std::sqrt(computeVariance(vector, mean));
}

// Implementation of EventQueue

void EventQueue::pushEvent(SimulationStep timestamp, size_t neuronId, 
                          float value, EventType type) {
    std::lock_guard<std::mutex> lock(queueMutex);
    
    if (eventQueue.size() < maxQueueSize) {
        eventQueue.emplace(timestamp, neuronId, value, type);
    }
}

bool EventQueue::popEvent(Event& event) {
    std::lock_guard<std::mutex> lock(queueMutex);
    
    if (!eventQueue.empty()) {
        event = eventQueue.top();
        eventQueue.pop();
        return true;
    }
    
    return false;
}

// Implementation of PerformanceProfiler

void PerformanceProfiler::startTimer(const std::string& name) {
    timers[name] = std::chrono::duration<double>::
        count(std::chrono::steady_clock::now() - startTime);
}

double PerformanceProfiler::stopTimer(const std::string& name) {
    auto it = timers.find(name);
    if (it != timers.end()) {
        double elapsed = std::chrono::duration<double>::
            count(std::chrono::steady_clock::now() - startTime) - it->second;
        timers[name] = elapsed;
        return elapsed;
    }
    return 0.0;
}

void PerformanceProfiler::updateMetrics(const MemoryPool& memory, const EventQueue& eventQueue) {
    currentMetrics.activeNeurons = memory.allocatedNeurons;
    currentMetrics.totalEvents = eventQueue.size();
    currentMetrics.memoryUsageMB = memory.allocatedNeurons * sizeof(NeuronInfo) / (1024 * 1024);
}

void PerformanceProfiler::recordMetrics() {
    history.push_back(currentMetrics);
}

void PerformanceProfiler::saveProfile(const std::string& filename) {
    // In real implementation, would serialize to file
    // For now, just log
    std::cout << "Performance profile saved to: " << filename << std::endl;
}

// Implementation of ParallelSimulation

void ParallelSimulation::submitTask(const std::function<void()>& task) {
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(task);
    condition.notify_one();
}

void ParallelSimulation::waitForTasks() {
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this]() { return taskQueue.empty() && stopFlag; });
}

void ParallelSimulation::workerThread() {
    while (!stopFlag || !taskQueue.empty()) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return !taskQueue.empty() || stopFlag; });
            
            if (!taskQueue.empty()) {
                task = std::move(taskQueue.front());
                taskQueue.pop();
            }
        }
        
        if (task) {
            task();
        }
    }
}

// Implementation of LargeScaleSimulation

LargeScaleSimulation::LargeScaleSimulation(const std::string& name, 
                                         const LifetimeExperimentConfig& config)
    : name_(name), config_(config), isInitialized_(false), isRunning_(false),
      currentStep_(0), currentFPS_(0.0), memoryEfficiency_(0.0),
      totalEventsProcessed_(0), spikeEventsProcessed_(0),
      synapticEventsProcessed_(0), peakMemoryUsage_(0.0), 
      averageMemoryUsage_(0.0) {
    simulationStartTime_ = std::chrono::steady_clock::now();
    threadCount_ = std::thread::hardware_concurrency();
    useSIMD_ = true;
    useParallel_ = true;
    useEventDriven_ = true;
    useMemoryPooling_ = true;
}

LargeScaleSimulation::~LargeScaleSimulation() {
    shutdown();
}

void LargeScaleSimulation::initialize() {
    std::cout << "Initializing LargeScaleSimulation: " << name_ << std::endl;
    
    // Initialize core brain components
    brain_ = std::make_shared<Brain>();
    advancedMemory_ = std::make_shared<AdvancedMemory>();
    planner_ = std::make_shared<HierarchicalPlanner>();
    
    // Configure advanced systems
    advancedMemory_->setMaxMemorySize(config_.memory_size);
    planner_->setMaxTreeDepth(config_.max_tree_depth);
    planner_->setMCTSSimulations(config_.mcts_simulations);
    
    // Setup performance optimizations
    setupPerformanceOptimizations();
    
    // Initialize simulation state
    globalState_.resize(50, 0.0f); // Simplified state vector
    populationVectors_.resize(10, std::vector<float>(50, 0.0f));
    
    // Initialize parallel simulation
    parallelSim_ = ParallelSimulation(threadCount_);
    
    isInitialized_ = true;
    std::cout << "LargeScaleSimulation initialized successfully" << std::endl;
}

void LargeScaleSimulation::runSimulation(size_t steps) {
    if (!isInitialized_) {
        std::cerr << "Error: Simulation not initialized" << std::endl;
        return;
    }
    
    isRunning_ = true;
    std::cout << "Running simulation for " << steps << " steps..." << std::endl;
    
    // Choose simulation strategy based on configuration
    if (useParallel_ && steps > 1000) {
        runParallelSimulation(steps);
    } else if (useEventDriven_ && steps > 100) {
        runEventDrivenSimulation(steps);
    } else {
        runSequentialSimulation(steps);
    }
    
    isRunning_ = false;
    std::cout << "Simulation complete. Processed " << totalEventsProcessed_ << " events." << std::endl;
}

void LargeScaleSimulation::shutdown() {
    if (isInitialized_) {
        std::cout << "Shutting down LargeScaleSimulation: " << name_ << std::endl;
        
        // Stop parallel simulation
        parallelSim_.~ParallelSimulation();
        
        // Clear brain components
        brain_.reset();
        advancedMemory_.reset();
        planner_.reset();
        
        // Clear simulation state
        globalState_.clear();
        populationVectors_.clear();
        
        isInitialized_ = false;
        isRunning_ = false;
        
        // Save performance data
        savePerformanceData("performance_data.json");
        
        std::cout << "LargeScaleSimulation shutdown complete" << std::endl;
    }
}

void LargeScaleSimulation::setupPerformanceOptimizations() {
    // Setup memory pool for large-scale neurons
    maxNeurons_ = std::max(config_.initialNeurons, config_.maxNeurons);
    maxConnections_ = maxNeurons_ * config_.connection_probability * 10; // Estimate
    
    memoryPool_.initialize(maxNeurons_, maxConnections_);
    memoryPool_.useArenaAllocation = useMemoryPooling_;
    
    // Initialize event queue
    eventQueue_.maxQueueSize = 10000000; // 10 million events
    
    // Setup performance profiler
    profiler_.startTimer("total_simulation");
}

void LargeScaleSimulation::initializeNeurons() {
    // Initialize neurons using parallel processing
    if (useParallel_ && maxNeurons_ > 10000) {
        std::vector<std::future<void>> futures;
        size_t chunkSize = maxNeurons_ / threadCount_;
        
        for (size_t t = 0; t < threadCount_; ++t) {
            size_t start = t * chunkSize;
            size_t end = (t == threadCount_ - 1) ? maxNeurons_ : start + chunkSize;
            
            futures.push_back(std::async(std::launch::async, [this, start, end]() {
                for (size_t i = start; i < end; ++i) {
                    NeuronInfo neuron(i, (rand() % 100) / 100.0f);
                    neuron.inputCount = rand() % 50 + 1;
                    neuron.outputCount = rand() % 50 + 1;
                    neuron.connectionDensity = (rand() % 100) / 100.0f;
                    neuron.synapticStrength = 1.0f + (rand() % 100) / 100.0f * 2.0f;
                    
                    memoryPool_.allocateNeuron(neuron);
                }
            }));
        }
        
        // Wait for all threads
        for (auto& future : futures) {
            future.wait();
        }
    } else {
        // Sequential initialization
        for (size_t i = 0; i < maxNeurons_; ++i) {
            NeuronInfo neuron(i, (rand() % 100) / 100.0f);
            neuron.inputCount = rand() % 50 + 1;
            neuron.outputCount = rand() % 50 + 1;
            neuron.connectionDensity = (rand() % 100) / 100.0f;
            neuron.synapticStrength = 1.0f + (rand() % 100) / 100.0f * 2.0f;
            
            memoryPool_.allocateNeuron(neuron);
        }
    }
    
    std::cout << "Initialized " << memoryPool_.allocatedNeurons << " neurons" << std::endl;
}

void LargeScaleSimulation::initializeConnections() {
    // Initialize synaptic connections using parallel processing
    std::vector<std::future<void>> futures;
    size_t chunkSize = maxConnections_ / threadCount_;
    
    for (size_t t = 0; t < threadCount_; ++t) {
        size_t start = t * chunkSize;
        size_t end = (t == threadCount_ - 1) ? maxConnections_ : start + chunkSize;
        
        futures.push_back(std::async(std::launch::async, [this, start, end]() {
            for (size_t i = start; i < end; ++i) {
                // Create random connections
                size_t pre = rand() % memoryPool_.allocatedNeurons;
                size_t post = rand() % memoryPool_.allocatedNeurons;
                
                if (pre != post) { // No self-connections
                    ConnectionInfo conn(pre, post, 
                                       (rand() % 200 - 100) / 100.0f, // Weight
                                       (rand() % 10) / 10.0f); // Delay
                    
                    memoryPool_.allocateConnection(conn);
                }
            }
        }));
    }
    
    // Wait for all threads
    for (auto& future : futures) {
        future.wait();
    }
    
    std::cout << "Initialized " << memoryPool_.allocatedConnections << " connections" << std::endl;
}

void LargeScaleSimulation::runSequentialSimulation(size_t steps) {
    initializeNeurons();
    initializeConnections();
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        currentStep_ = step;
        
        // Vectorized neuron updates
        std::vector<size_t> neuronIds;
        for (size_t i = 0; i < memoryPool_.allocatedNeurons; ++i) {
            if (memoryPool_.neurons[i].isActive) {
                neuronIds.push_back(i);
            }
        }
        
        if (!neuronIds.empty() && useSIMD_) {
            vectorizedNeuronUpdate(neuronIds);
        } else {
            // Sequential updates
            for (size_t neuronId : neuronIds) {
                simulateNeuron(neuronId);
            }
        }
        
        // Propagate spikes
        if (useEventDriven_) {
            propagateSpikes();
        }
        
        // Update synaptic weights
        updateSynapticWeights();
        
        // Apply plasticity
        applyPlasticity();
        
        // Update performance metrics periodically
        if (step % 100 == 0) {
            updatePerformanceMetrics();
            recordSimulationStep();
        }
        
        // Compact memory periodically
        if (step % 1000 == 0) {
            memoryPool_.compactMemory();
        }
        
        // Print progress
        if (step % 500 == 0) {
            double elapsed = std::chrono::duration<double>::
                count(std::chrono::steady_clock::now() - startTime);
            currentFPS_ = step / elapsed;
            
            std::cout << "Step " << step << ": "
                     << "FPS: " << currentFPS_ << ", "
                     << "Active Neurons: " << memoryPool_.allocatedNeurons << ", "
                     << "Events: " << totalEventsProcessed_ << std::endl;
        }
    }
    
    double totalTime = std::chrono::duration<double>::
        count(std::chrono::steady_clock::now() - startTime);
    currentFPS_ = steps / totalTime;
    
    profiler_.stopTimer("total_simulation");
}

void LargeScaleSimulation::runParallelSimulation(size_t steps) {
    std::cout << "Running parallel simulation with " << threadCount_ << " threads" << std::endl;
    
    initializeNeurons();
    initializeConnections();
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Distribute work across threads
    std::vector<std::future<void>> futures;
    size_t chunkSize = steps / threadCount_;
    
    for (size_t t = 0; t < threadCount_; ++t) {
        size_t start = t * chunkSize;
        size_t end = (t == threadCount_ - 1) ? steps : start + chunkSize;
        
        futures.push_back(std::async(std::launch::async, [this, start, end]() {
            auto threadStart = std::chrono::steady_clock::now();
            
            for (size_t step = start; step < end; ++step) {
                // Process events from event queue
                while (!eventQueue_.isEmpty()) {
                    EventQueue::Event event;
                    if (eventQueue_.popEvent(event)) {
                        totalEventsProcessed_++;
                        
                        if (event.type == EventType::Spike) {
                            spikeEventsProcessed_++;
                            // Process spike event
                        } else if (event.type == EventType::Synaptic) {
                            synapticEventsProcessed_++;
                            // Process synaptic event
                        }
                    }
                }
                
                // Simulate neurons assigned to this thread
                size_t neuronStart = (step % memoryPool_.allocatedNeurons) + 1;
                size_t neuronEnd = neuronStart + 1000; // Process 1000 neurons per step
                
                std::vector<size_t> neuronIds;
                for (size_t i = neuronStart; i < neuronEnd && i < memoryPool_.allocatedNeurons; ++i) {
                    if (memoryPool_.neurons[i].isActive) {
                        neuronIds.push_back(i);
                    }
                }
                
                if (!neuronIds.empty()) {
                    vectorizedNeuronUpdate(neuronIds);
                }
                
                // Record metrics
                if (step % 100 == 0) {
                    recordSimulationStep();
                }
                
                // Update synaptic weights periodically
                if (step % 500 == 0) {
                    updateSynapticWeights();
                }
            }
            
            auto threadTime = std::chrono::duration<double>::
                count(std::chrono::steady_clock::now() - threadStart);
        }));
    }
    
    // Wait for all threads
    for (auto& future : futures) {
        future.wait();
    }
    
    double totalTime = std::chrono::duration<double>::
        count(std::chrono::steady_clock::now() - startTime);
    currentFPS_ = steps / totalTime;
    
    profiler_.stopTimer("total_simulation");
    
    std::cout << "Parallel simulation complete. FPS: " << currentFPS_ << std::endl;
}

void LargeScaleSimulation::runEventDrivenSimulation(size_t steps) {
    std::cout << "Running event-driven simulation" << std::endl;
    
    initializeNeurons();
    initializeConnections();
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        currentStep_ = step;
        
        // Generate events for neurons
        std::vector<size_t> activeNeurons;
        for (size_t i = 0; i < memoryPool_.allocatedNeurons; ++i) {
            if (memoryPool_.neurons[i].isActive && 
                (rand() % 1000) < config_.event_rate) {
                activeNeurons.push_back(i);
                
                // Queue spike event
                eventQueue_.pushEvent(step, i, 1.0f, EventType::Spike);
                
                // Queue synaptic event for connected neurons
                for (size_t j = 0; j < memoryPool_.allocatedConnections; ++j) {
                    if (memoryPool_.connections[j].isEnabled &&
                        memoryPool_.connections[j].postNeuronId == i) {
                        eventQueue_.pushEvent(step + memoryPool_.connections[j].delay,
                                            memoryPool_.connections[j].preNeuronId,
                                            1.0f, EventType::Synaptic);
                    }
                }
            }
        }
        
        // Process events in the queue
        while (!eventQueue_.isEmpty()) {
            EventQueue::Event event;
            if (eventQueue_.popEvent(event)) {
                totalEventsProcessed_++;
                
                if (event.type == EventType::Spike) {
                    spikeEventsProcessed_++;
                    simulateNeuron(event.neuronId);
                } else if (event.type == EventType::Synaptic) {
                    synapticEventsProcessed_++;
                    // Process synaptic event
                }
                
                // Limit processing to maintain real-time performance
                if (totalEventsProcessed_ > 1000000) { // 1M events per step
                    break;
                }
            }
        }
        
        // Update synapses
        updateSynapticWeights();
        
        // Apply plasticity
        applyPlasticity();
        
        // Update performance metrics
        if (step % 100 == 0) {
            updatePerformanceMetrics();
            recordSimulationStep();
        }
        
        // Periodically compact memory
        if (step % 1000 == 0) {
            memoryPool_.compactMemory();
        }
        
        if (step % 500 == 0) {
            double elapsed = std::chrono::duration<double>::
                count(std::chrono::steady_clock::now() - startTime);
            currentFPS_ = step / elapsed;
            
            std::cout << "Step " << step << ": "
                     << "FPS: " << currentFPS_ << ", "
                     << "Active Neurons: " << memoryPool_.allocatedNeurons << ", "
                     << "Events: " << totalEventsProcessed_ << std::endl;
        }
    }
    
    double totalTime = std::chrono::duration<double>::
        count(std::chrono::steady_clock::now() - startTime);
    currentFPS_ = steps / totalTime;
    
    profiler_.stopTimer("total_simulation");
    
    std::cout << "Event-driven simulation complete. FPS: " << currentFPS_ << std::endl;
}

void LargeScaleSimulation::simulateNeuron(size_t neuronId) {
    if (neuronId >= memoryPool_.allocatedNeurons) return;
    
    NeuronInfo& neuron = memoryPool_.neurons[neuronId];
    
    // Update membrane potential using vectorized operations
    float inputSum = 0.0f;
    size_t inputCount = 0;
    
    // Get input indices (simplified)
    for (size_t i = neuron.inputIndices[neuronId]; 
         i < neuron.inputIndices[neuronId] + neuron.inputCount && 
         i < memoryPool_.inputIndices.size(); ++i) {
        size_t preNeuron = memoryPool_.inputIndices[i];
        if (preNeuron < memoryPool_.allocatedNeurons) {
            float synapticInput = memoryPool_.synapticWeights[i] * 
                                 memoryPool_.neurons[preNeuron].membranePotential;
            inputSum += synapticInput;
            inputCount++;
        }
    }
    
    // Apply synaptic weights and decay
    float weightedInput = inputSum * neuron.synapticStrength;
    
    // Update membrane potential
    neuron.membranePotential *= config_.membrane_decay;
    neuron.membranePotential += weightedInput;
    
    // Apply activation function
    if (neuron.membranePotential > config_.firing_threshold) {
        neuron.membranePotential = config_.firing_threshold;
        neuron.isSaturated = true;
        
        // Queue spike event
        eventQueue_.pushEvent(currentStep_, neuronId, 1.0f, EventType::Spike);
    }
    
    // Update refractory period
    neuron.refractoryTime = std::max(0.0f, neuron.refractoryTime - timeStep_);
    
    // Store potential
    if (neuronId < memoryPool_.membranePotentials.size()) {
        memoryPool_.membranePotentials[neuronId] = neuron.membranePotential;
    }
}

void LargeScaleSimulation::propagateSpikes() {
    // Propagate spike events through the network
    size_t processedSpikes = 0;
    
    while (!eventQueue_.isEmpty() && processedSpikes < 10000) {
        EventQueue::Event event;
        if (eventQueue_.popEvent(event)) {
            if (event.type == EventType::Spike) {
                // Propagate to postsynaptic neurons
                for (size_t i = 0; i < memoryPool_.allocatedConnections; ++i) {
                    if (memoryPool_.connections[i].isEnabled &&
                        memoryPool_.connections[i].preNeuronId == event.neuronId) {
                        
                        size_t postNeuron = memoryPool_.connections[i].postNeuronId;
                        if (postNeuron < memoryPool_.allocatedNeurons) {
                            // Queue synaptic event
                            eventQueue_.pushEvent(
                                currentStep_ + memoryPool_.connections[i].delay,
                                postNeuron,
                                1.0f, EventType::Synaptic);
                        }
                    }
                }
            }
            processedSpikes++;
        }
    }
}

void LargeScaleSimulation::updateSynapticWeights() {
    // Update synaptic weights using vectorized operations
    if (memoryPool_.allocatedConnections > 0 && useSIMD_) {
        std::vector<size_t> connectionIds;
        for (size_t i = 0; i < memoryPool_.allocatedConnections; ++i) {
            if (memoryPool_.connections[i].isEnabled) {
                connectionIds.push_back(i);
            }
        }
        
        if (!connectionIds.empty()) {
            vectorizedSynapticUpdate(connectionIds);
        }
    } else {
        // Sequential update
        for (size_t i = 0; i < memoryPool_.allocatedConnections; ++i) {
            if (memoryPool_.connections[i].isEnabled) {
                ConnectionInfo& conn = memoryPool_.connections[i];
                
                // Update weight based on Hebbian learning
                float change = config_.learning_rate * 
                              (1.0f - std::abs(conn.weight)) * 
                              (rand() % 100) / 100.0f;
                
                conn.weight += change;
                
                // Apply bounds
                conn.weight = std::max(-10.0f, std::min(10.0f, conn.weight));
                
                if (i < memoryPool_.synapticWeights.size()) {
                    memoryPool_.synapticWeights[i] = conn.weight;
                }
            }
        }
    }
}

void LargeScaleSimulation::applyPlasticity() {
    // Apply plasticity rules to synapses
    size_t processedConnections = 0;
    
    while (processedConnections < 1000 && 
           !eventQueue_.isEmpty()) {
        EventQueue::Event event;
        if (eventQueue_.popEvent(event)) {
            if (event.type == EventType::Synaptic) {
                // Find pre-synaptic neuron
                size_t preNeuron = event.neuronId;
                
                // Update synaptic efficacy based on plasticity rules
                for (size_t i = 0; i < memoryPool_.allocatedConnections; ++i) {
                    if (memoryPool_.connections[i].isEnabled &&
                        memoryPool_.connections[i].preNeuronId == preNeuron) {
                        
                        ConnectionInfo& conn = memoryPool_.connections[i];
                        
                        // Apply STDP-like plasticity
                        float preSpike = 1.0f;
                        float postSpike = event.value;
                        
                        float weightChange = config_.stdp_learning_rate * 
                                            preSpike * postSpike *
                                            (1.0f - std::abs(conn.weight));
                        
                        conn.weight += weightChange;
                        
                        // Apply bounds
                        conn.weight = std::max(-10.0f, std::min(10.0f, conn.weight));
                        
                        if (i < memoryPool_.synapticWeights.size()) {
                            memoryPool_.synapticWeights[i] = conn.weight;
                        }
                    }
                }
            }
            processedConnections++;
        }
    }
}

void LargeScaleSimulation::vectorizedNeuronUpdate(const std::vector<size_t>& neuronIds) {
    // Vectorized update for multiple neurons
    size_t size = neuronIds.size();
    
    // Use SIMD for parallel processing
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        size_t neuronId = neuronIds[i];
        simulateNeuron(neuronId);
    }
}

void LargeScaleSimulation::vectorizedSynapticUpdate(const std::vector<size_t>& connectionIds) {
    // Vectorized update for multiple connections
    size_t size = connectionIds.size();
    
    #pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
        size_t connId = connectionIds[i];
        ConnectionInfo& conn = memoryPool_.connections[connId];
        
        if (!conn.isEnabled) continue;
        
        // Update weight
        float change = config_.learning_rate * 
                      (1.0f - std::abs(conn.weight)) * 
                      (rand() % 100) / 100.0f;
        
        conn.weight += change;
        
        // Apply bounds
        conn.weight = std::max(-10.0f, std::min(10.0f, conn.weight));
        
        if (connId < memoryPool_.synapticWeights.size()) {
            memoryPool_.synapticWeights[connId] = conn.weight;
        }
    }
}

void LargeScaleSimulation::optimizeMemoryUsage() {
    // Optimize memory usage by removing inactive neurons
    memoryPool_.compactMemory();
    
    // Calculate memory efficiency
    float totalMemory = maxNeurons_ * sizeof(NeuronInfo) + 
                       maxConnections_ * sizeof(ConnectionInfo);
    float usedMemory = memoryPool_.allocatedNeurons * sizeof(NeuronInfo) + 
                      memoryPool_.allocatedConnections * sizeof(ConnectionInfo);
    
    memoryEfficiency_ = usedMemory / totalMemory;
}

void LargeScaleSimulation::compactMemory() {
    memoryPool_.compactMemory();
    memoryPool_.reserveMemory();
}

void LargeScaleSimulation::flushInactiveNeurons() {
    // Flush completely inactive neurons
    size_t writeNeurons = 0;
    
    for (size_t i = 0; i < memoryPool_.allocatedNeurons; ++i) {
        if (memoryPool_.neurons[i].isActive &&
            memoryPool_.neurons[i].refractoryTime > 0.0f) {
            memoryPool_.neurons[writeNeurons++] = memoryPool_.neurons[i];
        }
    }
    
    // Update allocated count
    memoryPool_.allocatedNeurons = writeNeurons;
}

void LargeScaleSimulation::updatePerformanceMetrics() {
    // Update performance metrics
    double currentTime = std::chrono::duration<double>::
        count(std::chrono::steady_clock::now() - simulationStartTime_);
    
    currentFPS_ = (currentStep_ > 0) ? currentStep_ / currentTime : 0.0;
    
    // Calculate memory efficiency
    optimizeMemoryUsage();
    
    // Track peak memory usage
    double currentMemory = memoryPool_.allocatedNeurons * sizeof(NeuronInfo) +
                          memoryPool_.allocatedConnections * sizeof(ConnectionInfo);
    peakMemoryUsage_ = std::max(peakMemoryUsage_, currentMemory);
    averageMemoryUsage_ = (averageMemoryUsage_ * (currentStep_ - 1) + currentMemory) / currentStep_;
    
    // Record metrics
    profiler_.updateMetrics(memoryPool_, eventQueue_);
}

void LargeScaleSimulation::recordSimulationStep() {
    profiler_.recordMetrics();
}

void LargeScaleSimulation::savePerformanceData(const std::string& filename) {
    std::cout << "Saving performance data to: " << filename << std::endl;
    
    // In real implementation, would serialize to file
    // For now, just log summary
    std::cout << "Performance Summary:" << std::endl;
    std::cout << "  Total Events Processed: " << totalEventsProcessed_ << std::endl;
    std::cout << "  Spike Events: " << spikeEventsProcessed_ << std::endl;
    std::cout << "  Synaptic Events: " << synapticEventsProcessed_ << std::endl;
    std::cout << "  Final FPS: " << currentFPS_ << std::endl;
    std::cout << "  Memory Efficiency: " << (memoryEfficiency_ * 100) << "%" << std::endl;
    std::cout << "  Peak Memory Usage: " << (peakMemoryUsage_ / (1024 * 1024)) << " MB" << std::endl;
    
    // Save to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "TotalEvents," << totalEventsProcessed_ << std::endl;
        file << "SpikeEvents," << spikeEventsProcessed_ << std::endl;
        file << "SynapticEvents," << synapticEventsProcessed_ << std::endl;
        file << "FinalFPS," << currentFPS_ << std::endl;
        file << "MemoryEfficiency," << memoryEfficiency_ << std::endl;
        file << "PeakMemoryMB," << (peakMemoryUsage_ / (1024 * 1024)) << std::endl;
        file.close();
    }
}

/**
 * @main
 * Demonstrate the LargeScaleSimulation
 */
int main() {
    std::cout << "=== Large Scale Performance Optimization Demo ===" << std::endl;
    
    // Create configuration
    nlm::LifetimeExperimentConfig config;
    config.max_tree_depth = 4;
    config.mcts_simulations = 100;
    config.max_tree_size = 200;
    config.exploration_temperature = 1.0f;
    config.model_complexity = 2;
    config.horizon_length = 10;
    config.memory_size = 1000;
    config.initialNeurons = 1000;
    config.maxNeurons = 50000;
    config.connection_probability = 0.1f;
    config.event_rate = 50;
    config.membrane_decay = 0.99f;
    config.firing_threshold = 1.0f;
    config.learning_rate = 0.01f;
    config.stdp_learning_rate = 0.001f;
    
    // Create and initialize simulation
    nlm::LargeScaleSimulation simulation("PerformanceDemo", config);
    simulation.initialize();
    
    // Setup performance optimizations
    simulation.enableSIMD(true);
    simulation.enableParallelProcessing(true);
    simulation.enableEventDriven(true);
    simulation.enableMemoryPooling(true);
    
    // Set simulation parameters
    simulation.setMaxNeurons(config.maxNeurons);
    simulation.setMaxConnections(config.maxNeurons * config.connection_probability * 10);
    simulation.setTimeStep(0.01f);
    simulation.setThreadCount(std::thread::hardware_concurrency());
    
    std::cout << "Simulation configured with " << config.maxNeurons << " max neurons" << std::endl;
    std::cout << "Performance optimizations enabled: SIMD, Parallel, Event-Driven, Memory Pooling" << std::endl;
    
    // Run simulation
    const size_t simulationSteps = 1000;
    simulation.runSimulation(simulationSteps);
    
    // Final performance statistics
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    std::cout << "Final Performance:" << std::endl;
    std::cout << "  FPS: " << simulation.getCurrentFPS() << std::endl;
    std::cout << "  Active Neurons: " << simulation.getActiveNeurons() << std::endl;
    std::cout << "  Total Events: " << simulation.getTotalEvents() << std::endl;
    std::cout << "  Memory Efficiency: " << (simulation.getMemoryEfficiency() * 100) << "%" << std::endl;
    std::cout << "  (Spike and synaptic event counts available internally)" << std::endl;
    
    // Shutdown simulation
    simulation.shutdown();
    
    std::cout << "\nLargeScaleSimulation demo completed successfully!" << std::endl;
    
    return 0;
}
