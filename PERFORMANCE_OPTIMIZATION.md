# NLM Performance Optimization Guide

## Overview

This document provides comprehensive guidance for optimizing NLM (Neural Learning Machine) performance, including memory management, computation efficiency, and debugging tools.

## Performance Optimization Strategies

### 1. Memory Management Optimizations

#### Brain Memory Efficiency

**Problem**: The brain creates many unique_ptr objects that can be optimized for better memory locality.

**Solution**: Use custom memory pools for frequently allocated objects.

```cpp
// Before: Manual allocation with unique_ptr
struct Brain::Impl {
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<std::unique_ptr<Neuron>> neurons;
    std::vector<std::unique_ptr<Synapse>> synapses;
};

// After: Memory pools for better locality
struct Brain::Impl {
    // Memory pools for frequently allocated objects
    std::vector<Neuron> neuronPool;
    std::vector<Synapse> synapsePool;
    std::vector<NeuralRegion> regionPool;
    
    // Smart pointers for ownership management
    std::vector<std::shared_ptr<Neuron>> neurons;
    std::vector<std::shared_ptr<Synapse>> synapses;
    std::vector<std::shared_ptr<NeuralRegion>> regions;
};
```

#### AgentBrain Memory Efficiency

**Problem**: Many small allocations in AgentBrain.cpp.

**Solution**: Use custom allocators and reduce temporary object creation.

```cpp
// Before: Multiple small allocations
std::vector<Neuron*> motorForward_;
std::vector<Neuron*> motorBackward_;
std::vector<Neuron*> motorTurnLeft_;
// ... and many more

// After: Single allocation with indexing
struct MotorGroup {
    Neuron* neurons[64];  // Fixed size for better cache locality
    size_t count = 0;
};

MotorGroup motorGroups[6];  // forward, backward, turnLeft, turnRight, interact, wait
```

### 2. Computation Optimizations

#### Spike System Optimization

**Problem**: Event-driven spike propagation with high overhead.

**Solution**: Use SIMD optimizations and batch processing.

```cpp
// Optimized spike processing with SIMD
void SpikeSystem::processSpikes() {
    // Use SIMD for threshold comparisons
    for (size_t i = 0; i + 4 <= neuronCount_; i += 4) {
        auto potentials = _mm_loadu_ps(neuronPotentials_ + i);
        auto thresholds = _mm_loadu_ps(neuronThresholds_ + i);
        auto crossed = _mm_cmple_ps(potentials, thresholds);
        
        // Process spikes in batches
        if (_mm_movemask_ps(crossed) != 0) {
            for (int j = 0; j < 4; ++j) {
                if (_mm_extract_ps(crossed, j)) {
                    handleSpike(i + j);
                }
            }
        }
    }
}
```

#### Plasticity Computation

**Problem**: STDP and Hebbian plasticity calculations are O(n²).

**Solution**: Use spatial partitioning and neighbor lists.

```cpp
// Before: Full connectivity matrix
void STDP::applyPlasticity() {
    for (size_t i = 0; i < sourceNeurons_.size(); ++i) {
        for (size_t j = 0; j < targetNeurons_.size(); ++j) {
            float delta = computeSTDP(i, j);
            if (std::abs(delta) > epsilon_) {
                applyWeightChange(i, j, delta);
            }
        }
    }
}

// After: Spatial partitioning
class SpatialSTDP {
    std::vector<std::vector<size_t>> spatialHash_[HASH_SIZE];
    
    void applyPlasticity() {
        for (auto& bucket : spatialHash_) {
            for (size_t source : bucket) {
                for (size_t target : findNearbyNeurons(source)) {
                    float delta = computeSTDP(source, target);
                    if (std::abs(delta) > epsilon_) {
                        applyWeightChange(source, target, delta);
                    }
                }
            }
        }
    }
};
```

### 3. Development System Optimization

#### Developmental Stage Transitions

**Problem**: Development transitions are checked every timestep.

**Solution**: Use threshold-based checking and lazy updates.

```cpp
// Before: Check every timestep
void DevelopmentSystem::update(double timestep) {
    developmentalAge_ += timestep;
    
    // Check stage transitions every timestep
    if (developmentalAge_ < criticalPeriodEnd_) {
        currentStage_ = DevelopmentalStage::Initial;
    } else if (developmentalAge_ < maturationEnd_) {
        currentStage_ = DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ < adultEnd_) {
        currentStage_ = DevelopmentalStage::Maturation;
    } else {
        currentStage_ = DevelopmentalStage::Adult;
    }
}

// After: Check based on thresholds
void DevelopmentSystem::update(double timestep) {
    developmentalAge_ += timestep;
    
    // Only update stage when thresholds are crossed
    if (developmentalAge_ >= criticalPeriodEnd_ && 
        currentStage_ != DevelopmentalStage::CriticalPeriod) {
        currentStage_ = DevelopmentalStage::CriticalPeriod;
    } else if (developmentalAge_ >= maturationEnd_ &&
               currentStage_ != DevelopmentalStage::Maturation) {
        currentStage_ = DevelopmentalStage::Maturation;
    } else if (developmentalAge_ >= adultEnd_ &&
               currentStage_ != DevelopmentalStage::Adult) {
        currentStage_ = DevelopmentalStage::Adult;
    }
}
```

### 4. Neuromodulation Optimization

#### Dopamine Computation

**Problem**: Neuromodulators update every synapse every timestep.

**Solution**: Use eligibility traces and sparse updates.

```cpp
// Before: Update all synapses every timestep
void Neuromodulator::applyModulation() {
    for (auto& syn : brain_->getAllSynapses()) {
        float eligibility = syn->getEligibilityTrace();
        if (std::abs(eligibility) > threshold_) {
            float change = eligibility * signal_ * learningRate_;
            syn->addToWeight(change);
        }
    }
}

// After: Sparse updates based on activity
void Neuromodulator::applyModulation() {
    // Only update synapses with significant activity
    for (auto& syn : brain_->getActiveSynapses()) {
        float eligibility = syn->getEligibilityTrace();
        if (std::abs(eligibility) > threshold_) {
            float change = eligibility * signal_ * learningRate_;
            syn->addToWeight(change);
        }
    }
}
```

## Debugging and Monitoring Tools

### 1. Performance Monitoring

#### Simulation Profiler

```cpp
class SimulationProfiler {
public:
    struct Metrics {
        double totalTime;
        size_t totalSpikes;
        size_t activeNeurons;
        double averageFiringRate;
        double memoryUsage;
    };
    
    void startProfiling() {
        startTime_ = std::chrono::high_resolution_clock::now();
        spikeCount_ = 0;
        memorySample_ = MemoryInfo::getCurrentUsage();
    }
    
    void stopProfiling() {
        endTime_ = std::chrono::high_resolution_clock::now();
    }
    
    Metrics getMetrics() const {
        Metrics m;
        m.totalTime = duration<double>(endTime_ - startTime_).count();
        m.totalSpikes = spikeCount_;
        m.activeNeurons = brain_->getFiringNeuronCount();
        m.averageFiringRate = brain_->getAverageFiringRate();
        m.memoryUsage = MemoryInfo::getCurrentUsage();
        return m;
    }
    
    void incrementSpikeCount() { spikeCount_++; }
    
    static std::string formatMetrics(const Metrics& m) {
        std::stringstream ss;
        ss << "Performance Metrics:" << std::endl;
        ss << "  Total time: " << m.totalTime << "s" << std::endl;
        ss << "  Total spikes: " << m.totalSpikes << std::endl;
        ss << "  Active neurons: " << m.activeNeurons << std::endl;
        ss << "  Average firing rate: " << m.averageFiringRate << " Hz" << std::endl;
        ss << "  Memory usage: " << m.memoryUsage / 1024 / 1024 << " MB" << std::endl;
        return ss.str();
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    size_t spikeCount_ = 0;
    size_t memorySample_ = 0;
};
```

#### Memory Leak Detection

```cpp
class MemoryTracker {
public:
    struct AllocationInfo {
        void* address;
        size_t size;
        const char* file;
        int line;
        std::string stackTrace;
    };
    
    static void trackAllocation(void* ptr, size_t size, 
                               const char* file, int line) {
        AllocationInfo info{ptr, size, file, line, getStackTrace()};
        allocations_.insert({ptr, info});
    }
    
    static void trackDeallocation(void* ptr) {
        auto it = allocations_.find(ptr);
        if (it != allocations_.end()) {
            allocations_.erase(it);
        } else {
            // Potential double-free or free of unallocated memory
            reportError("Memory deallocation error", ptr);
        }
    }
    
    static void reportMemoryLeaks() {
        if (!allocations_.empty()) {
            std::cout << "Memory leaks detected:" << std::endl;
            for (const auto& pair : allocations_) {
                const auto& info = pair.second;
                std::cout << "  Leaked " << info.size << " bytes at " 
                         << info.file << ":" << info.line << std::endl;
                std::cout << "    Stack trace: " << info.stackTrace << std::endl;
            }
        }
    }
    
    static std::string getStackTrace() {
        // Implementation would capture stack trace
        return "Stack trace not implemented in this build";
    }
    
private:
    static std::unordered_map<void*, AllocationInfo> allocations_;
    static std::mutex mutex_;
};
```

### 2. Real-time Monitoring

#### Monitoring Decorators

```cpp
cpp11/14 feature - lambda function implementation

class MonitoringDecorator {
public:
    template<typename Func>
    auto wrapWithMonitoring(Func&& func, const std::string& operationName) {
        return [this, func = std::move(func), operationName](auto&&... args) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Call the original function
            auto result = func(std::forward<decltype(args)>(args)...);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            // Log performance metrics
            logOperation(operationName, duration, args...);
            
            // Check for performance issues
            if (duration > performanceThresholds_[operationName]) {
                reportPerformanceIssue(operationName, duration, args...);
            }
            
            return result;
        };
    }
    
    void setPerformanceThreshold(const std::string& operationName, 
                                 double threshold) {
        performanceThresholds_[operationName] = threshold;
    }
    
private:
    std::unordered_map<std::string, double> performanceThresholds_;
    
    void logOperation(const std::string& operationName, double duration, 
                     auto&&... args) {
        // Log operation details
        std::cout << "[PERF] " << operationName << ": " << duration << "s" << std::endl;
    }
    
    void reportPerformanceIssue(const std::string& operationName, 
                               double duration, auto&&... args) {
        std::cout << "[WARNING] Slow operation detected: " 
                 << operationName << " took " << duration << "s" << std::endl;
    }
};
```

### 3. Visualization Tools

#### Brain Activity Visualizer

```cpp
class BrainActivityVisualizer {
public:
    void createNeuronActivityPlot(const Brain& brain, 
                                 const std::string& filename) {
        std::vector<size_t> neuronCounts;
        std::vector<double> firingRates;
        
        // Collect neuron activity data
        for (size_t step = 0; step < simulationSteps_; ++step) {
            neuronCounts.push_back(brain.getFiringNeuronCount());
            firingRates.push_back(brain.getAverageFiringRate());
        }
        
        // Create visualization using matplotlib (if available)
        // or generate simple text-based visualization
        generateTextPlot(neuronCounts, firingRates, filename);
    }
    
    void createSynapseWeightEvolution(const Brain& brain,
                                     const std::string& filename) {
        std::vector<double> weightChanges;
        
        // Track synapse weight changes over time
        for (size_t step = 0; step < simulationSteps_; ++step) {
            double totalWeightChange = calculateTotalWeightChange(brain);
            weightChanges.push_back(totalWeightChange);
        }
        
        generateWeightEvolutionPlot(weightChanges, filename);
    }
    
private:
    void generateTextPlot(const std::vector<size_t>& neuronCounts,
                         const std::vector<double>& firingRates,
                         const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return;
        }
        
        file << "Neuron Activity Over Time" << std::endl;
        file << "="*50 << std::endl;
        
        // Generate ASCII bar chart
        size_t maxCount = *std::max_element(neuronCounts.begin(), neuronCounts.end());
        
        for (size_t i = 0; i < neuronCounts.size(); i += 10) {  // Sample every 10 steps
            file << "Step " << i << ": ";
            
            // Neuron count bar
            size_t neuronBar = (neuronCounts[i] * 50) / maxCount;
            file << "Neurons: [";
            for (size_t j = 0; j < neuronBar; ++j) file << "#";
            for (size_t j = neuronBar; j < 50; ++j) file << " ";
            file << "] " << neuronCounts[i] << std::endl;
            
            // Firing rate bar
            double firingBar = (firingRates[i] * 50) / 20.0;  // Assume max 20 Hz
            file << "Firing:  [";
            for (size_t j = 0; j < firingBar; ++j) file << "#";
            for (size_t j = firingBar; j < 50; ++j) file << " ";
            file << "] " << std::fixed << std::setprecision(2) << firingRates[i] << " Hz" << std::endl;
        }
        
        file << "="*50 << std::endl;
    }
    
    double calculateTotalWeightChange(const Brain& brain) {
        double total = 0.0;
        for (auto& region : brain.getRegions()) {
            for (auto& syn : region->getSynapses()) {
                total += std::abs(syn->getWeightChange());
            }
        }
        return total;
    }
};
```

### 4. Debug Output

#### Debug Logger

```cpp
class DebugLogger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    struct LogEntry {
        Level level;
        std::string message;
        std::string file;
        int line;
        std::chrono::system_clock::time_point timestamp;
    };
    
    static void log(Level level, const std::string& message,
                   const char* file = __builtin_FILE(),
                   int line = __builtin_LINE()) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        LogEntry entry{
            level,
            message,
            std::string(file),
            line,
            std::chrono::system_clock::now()
        };
        
        entries_.push_back(entry);
        
        // Output to console with color coding
        printColored(entry);
        
        // Write to file if debugging
        if (debugEnabled_) {
            writeToFile(entry);
        }
    }
    
    static void enableDebugOutput(const std::string& filename) {
        debugEnabled_ = true;
        debugFile_.open(filename);
    }
    
    static void disableDebugOutput() {
        debugEnabled_ = false;
        if (debugFile_.is_open()) {
            debugFile_.close();
        }
    }
    
    static void printLogEntries() {
        for (const auto& entry : entries_) {
            printColored(entry);
        }
    }
    
    static void clearLogEntries() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_.clear();
    }
    
private:
    static std::vector<LogEntry> entries_;
    static std::mutex mutex_;
    static bool debugEnabled_;
    static std::ofstream debugFile_;
    
    static void printColored(const LogEntry& entry) {
        const char* colorCodes[] = {
            "\033[36m",  // DEBUG - Cyan
            "\033[32m",  // INFO - Green
            "\033[33m",  // WARNING - Yellow
            "\033[31m",  // ERROR - Red
            "\033[41m"   // CRITICAL - Red background
        };
        
        const char* resetCode = "\033[0m";
        
        std::cout << colorCodes[static_cast<int>(entry.level)]
                 << "[" << levelToString(entry.level) << "] "
                 << entry.message << " ("
                 << entry.file << ":" << entry.line << ")"
                 << resetCode << std::endl;
    }
    
    static std::string levelToString(Level level) {
        switch (level) {
            case Level::DEBUG: return "DEBUG";
            case Level::INFO: return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR: return "ERROR";
            case Level::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    static void writeToFile(const LogEntry& entry) {
        if (debugFile_.is_open()) {
            debugFile_ << entry.timestamp.time_since_epoch().count() << " "
                     << levelToString(entry.level) << " "
                     << entry.message << " "
                     << entry.file << ":" << entry.line << std::endl;
        }
    }
};
```

## Performance Testing Framework

### 1. Benchmark Suite

```cpp
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string name;
        double timeSeconds;
        size_t memoryBytes;
        double spikesPerSecond;
        double efficiencyScore;
    };
    
    BenchmarkResult runBenchmark(const std::string& name,
                                 std::function<void()> benchmarkFunction,
                                 size_t iterations = 100) {
        BenchmarkResult result;
        result.name = name;
        
        // Warm up
        for (size_t i = 0; i < 10; ++i) {
            benchmarkFunction();
        }
        
        // Actual benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            benchmarkFunction();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        
        result.timeSeconds = std::chrono::duration<double>(endTime - startTime).count() / iterations;
        result.memoryBytes = measureMemoryUsage();
        result.spikesPerSecond = measureSpikesPerSecond();
        result.efficiencyScore = calculateEfficiencyScore(result);
        
        return result;
    }
    
    void runAllBenchmarks() {
        std::vector<BenchmarkResult> results;
        
        // Brain benchmarks
        results.push_back(runBenchmark("Brain Initialization",
            [this]() { createTestBrain(); }));
        
        results.push_back(runBenchmark("Simulation Step",
            [this]() { runSimulationStep(); }));
        
        // Agent benchmarks
        results.push_back(runBenchmark("Agent Processing",
            [this]() { processAgent(); }));
        
        // Plasticity benchmarks
        results.push_back(runBenchmark("STDP Application",
            [this]() { applySTDP(); }));
        
        // Development benchmarks
        results.push_back(runBenchmark("Development Update",
            [this]() { updateDevelopment(); }));
        
        printResults(results);
        saveResults(results);
    }
    
private:
    double measureMemoryUsage() {
        // Implementation would use platform-specific memory measurement
        return 0.0;
    }
    
    double measureSpikesPerSecond() {
        // Implementation would track spikes during benchmark
        return 0.0;
    }
    
    double calculateEfficiencyScore(const BenchmarkResult& result) {
        // Calculate efficiency based on time, memory, and spikes
        return 1.0 / (result.timeSeconds * (result.memoryBytes / 1024.0));
    }
    
    void printResults(const std::vector<BenchmarkResult>& results) {
        std::cout << "Performance Benchmark Results" << std::endl;
        std::cout << "=" * 60 << std::endl;
        
        for (const auto& result : results) {
            std::cout << result.name << ":" << std::endl;
            std::cout << "  Time: " << result.timeSeconds << " s" << std::endl;
            std::cout << "  Memory: " << result.memoryBytes / 1024 << " KB" << std::endl;
            std::cout << "  Spikes/sec: " << result.spikesPerSecond << std::endl;
            std::cout << "  Efficiency: " << result.efficiencyScore << std::endl;
            std::cout << std::endl;
        }
    }
    
    void saveResults(const std::vector<BenchmarkResult>& results) {
        std::ofstream file("benchmark_results.txt");
        if (!file.is_open()) {
            return;
        }
        
        for (const auto& result : results) {
            file << result.name << "," << result.timeSeconds << ","
                 << result.memoryBytes << "," << result.spikesPerSecond << ","
                 << result.efficiencyScore << std::endl;
        }
        
        file.close();
    }
    
    // Helper functions for benchmarks
    void createTestBrain() { /* Implementation */ }
    void runSimulationStep() { /* Implementation */ }
    void processAgent() { /* Implementation */ }
    void applySTDP() { /* Implementation */ }
    void updateDevelopment() { /* Implementation */ }
};
```

### 2. Automated Performance Testing

```cpp
// Integration with existing test framework
class PerformanceTests {
public:
    static void registerPerformanceTests() {
        // Register performance tests with test framework
        // This would integrate with existing unit test framework
        
        // Example for Google Test or similar framework
        // TEST_F(PerformanceTests, BrainInitialization) {
        //     BENCHMARK(createTestBrain);
        //     EXPECT_GT(benchmark_result.time, 0.0);
        // }
    }
    
    static void runPerformanceRegressionTests() {
        // Compare current performance with baseline
        std::vector<BenchmarkResult> currentResults = runAllBenchmarks();
        std::vector<BenchmarkResult> baselineResults = loadBaselineResults();
        
        if (baselineResults.empty()) {
            std::cout << "No baseline results available. "
                     << "Run benchmarks first to establish baseline." << std::endl;
            return;
        }
        
        double maxAcceptableRegression = 0.1;  // 10% regression threshold
        
        for (size_t i = 0; i < currentResults.size(); ++i) {
            if (i < baselineResults.size()) {
                double regression = (currentResults[i].timeSeconds - 
                                   baselineResults[i].timeSeconds) /
                                  baselineResults[i].timeSeconds;
                
                if (regression > maxAcceptableRegression) {
                    std::cout << "PERFORMANCE REGRESSION DETECTED:" << std::endl;
                    std::cout << "  Test: " << currentResults[i].name << std::endl;
                    std::cout << "  Baseline: " << baselineResults[i].timeSeconds << " s" << std::endl;
                    std::cout << "  Current: " << currentResults[i].timeSeconds << " s" << std::endl;
                    std::cout << "  Regression: " << (regression * 100) << "%" << std::endl;
                    
                    // Fail test if regression exceeds threshold
                    if (regression > 0.2) {  // 20% threshold for failure
                        throw std::runtime_error("Performance regression exceeds threshold");
                    }
                }
            }
        }
    }
};
```

## Configuration Optimization

### 1. Automated Configuration Tuning

```cpp
class ConfigurationTuner {
public:
    struct TuningParameters {
        size_t neuronCount;
        float connectionProbability;
        float stdpLearningRate;
        float curiosityLevel;
        double developmentSpeed;
    };
    
    TuningParameters optimizeForPerformance(const BenchmarkResult& baseline,
                                             const PerformanceGoals& goals) {
        TuningParameters bestParams = initialParameters_;
        double bestScore = std::numeric_limits<double>::max();
        
        // Random search with local optimization
        for (int iteration = 0; iteration < maxIterations_; ++iteration) {
            TuningParameters candidate = generateCandidateParameters();
            
            // Evaluate candidate
            double score = evaluateParameters(candidate, goals);
            
            if (score < bestScore) {
                bestScore = score;
                bestParams = candidate;
            }
            
            // Local optimization around best candidate
            if (iteration % 100 == 0) {
                bestParams = localOptimization(bestParams, goals);
            }
        }
        
        return bestParams;
    }
    
private:
    double evaluateParameters(const TuningParameters& params,
                              const PerformanceGoals& goals) {
        // Create brain with candidate parameters
        auto config = std::make_shared<Config>();
        config->set("neuron_count", params.neuronCount);
        config->set("connection_probability", params.connectionProbability);
        config->set("stdp_ltp_weight", params.stdpLearningRate);
        // ... set other parameters
        
        // Run benchmarks
        auto results = runBenchmarksWithParameters(params);
        
        // Calculate score based on goals
        double score = 0.0;
        
        // Minimize time
        if (goals.maxTime > 0) {
            score += std::pow(results.time - goals.maxTime, 2);
        }
        
        // Minimize memory
        if (goals.maxMemory > 0) {
            score += std::pow(results.memory - goals.maxMemory, 2);
        }
        
        // Maximize spikes
        if (goals.minSpikes > 0) {
            score += std::pow(goals.minSpikes - results.spikesPerSecond, 2);
        }
        
        return score;
    }
    
    TuningParameters generateCandidateParameters() {
        TuningParameters params;
        
        // Random search around current parameters
        params.neuronCount = currentParameters_.neuronCount +
                           (rand() % 201 - 100);  // ±100 neurons
        params.neuronCount = std::max(100, std::min(params.neuronCount, 10000));
        
        params.connectionProbability = currentParameters_.connectionProbability +
                                      ((rand() % 200 - 100) / 1000.0f);  // ±0.1
        params.connectionProbability = std::max(0.01f, std::min(params.connectionProbability, 0.5f));
        
        // ... generate other parameters
        
        return params;
    }
    
    TuningParameters localOptimization(const TuningParameters& params,
                                        const PerformanceGoals& goals) {
        TuningParameters optimized = params;
        
        // Gradient descent-like optimization
        for (int step = 0; step < 10; ++step) {
            double gradient = evaluateParameters(optimized, goals);
            
            // Small perturbations
            TuningParameters candidate = optimized;
            candidate.neuronCount += (rand() % 21 - 10);  // ±10
            candidate.connectionProbability += (rand() % 21 - 10) / 1000.0f;
            
            double candidateScore = evaluateParameters(candidate, goals);
            
            if (candidateScore < gradient) {
                optimized = candidate;
                gradient = candidateScore;
            }
        }
        
        return optimized;
    }
    
    std::vector<BenchmarkResult> runBenchmarksWithParameters(
        const TuningParameters& params) {
        std::vector<BenchmarkResult> results;
        
        // Run benchmarks with current parameters
        results.push_back(runBenchmark("Initialization", [this, params]() {
            createBrainWithParams(params);
        }));
        
        results.push_back(runBenchmark("Simulation", [this, params]() {
            runSimulationWithParams(params);
        }));
        
        // ... run other benchmarks
        
        return results;
    }
    
    TuningParameters currentParameters_;
    TuningParameters initialParameters_;
    int maxIterations_ = 1000;
};
```

## Integration with Build System

### 1. CMake Performance Configuration

```cmake
# CMake options for performance optimization
option(NLM_ENABLE_PERFORMANCE_PROFILING "Enable performance profiling" ON)
option(NLM_ENABLE_MEMORY_TRACKING "Enable memory tracking" ON)
option(NLM_ENABLE_DEBUG_LOGGING "Enable debug logging" OFF)
option(NLM_OPTIMIZE_FOR_PERFORMANCE "Optimize for performance" ON)

# Performance compiler flags
if (NLM_OPTIMIZE_FOR_PERFORMANCE)
    add_compile_options(-O3 -march=native -flto)
    add_link_options(-flto)
endif()

# Profiling support
if (NLM_ENABLE_PERFORMANCE_PROFILING)
    add_definitions(-DNLM_PERFORMANCE_PROFILING)
    find_package(GPerf OPTIONAL_COMPONENTS)
endif()

# Memory tracking
if (NLM_ENABLE_MEMORY_TRACKING)
    add_definitions(-DNLM_MEMORY_TRACKING)
endif()

# Debug logging
if (NLM_ENABLE_DEBUG_LOGGING)
    add_definitions(-DNLM_DEBUG_LOGGING)
endif()
```

### 2. Python Integration

```python
# Performance monitoring decorator for Python bindings
import functools
import time

class PerformanceMonitor:
    def __init__(self, enabled=True):
        self.enabled = enabled
        self.timers = {}
    
    def time_operation(self, operation_name):
        def decorator(func):
            @functools.wraps(func)
            def wrapper(*args, **kwargs):
                if self.enabled:
                    start_time = time.perf_counter()
                    result = func(*args, **kwargs)
                    end_time = time.perf_counter()
                    
                    duration = end_time - start_time
                    self.record_operation(operation_name, duration, args, kwargs)
                    
                    # Warn about slow operations
                    if duration > self.get_threshold(operation_name):
                        print(f"[PERFORMANCE WARNING] {operation_name} took {duration:.3f}s")
                else:
                    result = func(*args, **kwargs)
                
                return result
            
            return wrapper
        
        return decorator
    
    def record_operation(self, operation_name, duration, args, kwargs):
        # Store performance data
        if operation_name not in self.timers:
            self.timers[operation_name] = []
        
        self.timers[operation_name].append({
            'duration': duration,
            'timestamp': time.time(),
            'args_size': len(str(args)),
            'kwargs_size': len(str(kwargs))
        })
    
    def get_threshold(self, operation_name):
        # Default thresholds
        thresholds = {
            'createBrain': 0.1,  # 100ms
            'initialize': 0.05,  # 50ms
            'step': 0.01,  # 10ms
            'processSensoryInput': 0.005,  # 5ms
            'decodeMotorCommand': 0.002,  # 2ms
            'applyRewardModulation': 0.001,  # 1ms
            'updateDevelopment': 0.002,  # 2ms
            'run_episode': 1.0,  # 1s
        }
        
        return thresholds.get(operation_name, 1.0)
    
    def get_statistics(self, operation_name):
        if operation_name not in self.timers:
            return None
        
        durations = [t['duration'] for t in self.timers[operation_name]]
        
        stats = {
            'mean': sum(durations) / len(durations),
            'median': sorted(durations)[len(durations) // 2],
            'min': min(durations),
            'max': max(durations),
            'stddev': (sum((x - sum(durations) / len(durations)) ** 2 for x in durations) / len(durations)) ** 0.5,
            'count': len(durations)
        }
        
        return stats
    
    def print_report(self):
        print("\nPerformance Monitoring Report")
        print("=" * 50)
        
        for operation_name, data in self.timers.items():
            if data:
                durations = [t['duration'] for t in data]
                stats = {
                    'mean': sum(durations) / len(durations),
                    'min': min(durations),
                    'max': max(durations),
                    'count': len(durations)
                }
                
                print(f"{operation_name}:")
                print(f"  Mean: {stats['mean']:.4f}s")
                print(f"  Min: {stats['min']:.4f}s")
                print(f"  Max: {stats['max']:.4f}s")
                print(f"  Count: {stats['count']}")
                print()

# Global performance monitor
performance_monitor = PerformanceMonitor(enabled=True)

# Decorator for slow operations
@performance_monitor.time_operation('python_operation')
def slow_python_operation():
    # Simulate slow operation
    time.sleep(0.05)
    return "done"

# Usage example
if __name__ == "__main__":
    # Run some operations
    for i in range(10):
        slow_python_operation()
    
    # Print report
    performance_monitor.print_report()
```

## Summary

Performance optimization in NLM involves multiple strategies:

1. **Memory Optimization**: Use memory pools, reduce allocations, improve locality
2. **Computation Optimization**: Use SIMD, spatial partitioning, batch processing
3. **Development Optimization**: Lazy updates, threshold-based checking
4. **Neuromodulation Optimization**: Sparse updates, eligibility traces
5. **Monitoring and Debugging**: Real-time profiling, memory tracking, performance metrics
6. **Configuration Tuning**: Automated parameter optimization
7. **Integration**: Build system integration, Python monitoring

The NLM project now provides comprehensive tools for performance optimization, debugging, and monitoring, making it easier to develop efficient neural simulations while maintaining high code quality and user experience.
