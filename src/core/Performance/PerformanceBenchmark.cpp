#include "PerformanceBenchmark.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <chrono>
#include <random>
#include <unordered_map>

namespace nlm {

// BenchmarkSuite implementation
class BenchmarkSuite::Impl {
private:
    struct Benchmark {
        std::string name;
        std::string description;
        std::function<void()> benchmarkFunc;
        size_t iterations;
    };
    
    std::vector<Benchmark> benchmarks;
    std::mutex mutex;
    
public:
    void addBenchmark(const std::string& name, const std::string& description,
                     std::function<void()> benchmark,
                     size_t iterations) {
        std::lock_guard<std::mutex> lock(mutex);
        benchmarks.push_back({name, description, std::move(benchmark), iterations});
    }
    
    std::vector<BenchmarkResult> runAll() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<BenchmarkResult> results;
        
        for (const auto& benchmark : benchmarks) {
            results.push_back(runBenchmark(benchmark.name));
        }
        
        return results;
    }
    
    BenchmarkResult runBenchmark(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = std::find_if(benchmarks.begin(), benchmarks.end(),
                              [&name](const Benchmark& b) { return b.name == name; });n        
        if (it == benchmarks.end()) {
            return BenchmarkResult(name, "Benchmark not found");
        }
        
        return runBenchmarkInternal(*it);
    }
    
    std::vector<std::string> getBenchmarkNames() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::string> names;
        for (const auto& benchmark : benchmarks) {
            names.push_back(benchmark.name);
        }
        return names;
    }
    
    size_t getBenchmarkCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return benchmarks.size();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        benchmarks.clear();
    }
    
    void setIterations(size_t iterations) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& benchmark : benchmarks) {
            benchmark.iterations = iterations;
        }
    }
    
    void setIterations(const std::string& name, size_t iterations) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& benchmark : benchmarks) {
            if (benchmark.name == name) {
                benchmark.iterations = iterations;
                break;
            }
        }
    }
    
    std::string generateReport(const std::vector<BenchmarkResult>& results) const {
        std::ostringstream oss;
        oss << "Benchmark Suite Report\n";
        oss << "======================\n\n";
        
        oss << "Total benchmarks: " << results.size() << "\n\n";
        
        for (const auto& result : results) {
            oss << result.toString() << "\n\n";
        }
        
        return oss.str();
    }
    
    bool saveResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Save as CSV
        for (const auto& result : results) {
            file << result.toCSV() << "\n";
        }
        
        return true;
    }
    
    bool loadResults(std::vector<BenchmarkResult>& results, const std::string& filepath) const {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // CSV parsing (simplified)
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                BenchmarkResult result(tokens[0], tokens[1]);
                result.iterations = std::stoul(tokens[2]);
                result.timeSeconds = std::stod(tokens[3]);
                result.operationsPerSecond = std::stod(tokens[7]);
                result.memoryUsageBytes = std::stoull(tokens[8]);
                results.push_back(result);
            }
        }
        
        return true;
    }
    
    ComparisonResult compareWithBaseline(const BenchmarkResult& baseline, const BenchmarkResult& current) const {
        ComparisonResult result;
        result.benchmarkName = current.name;
        
        if (current.timeSeconds == 0.0) {
            result.improvementFactor = 0.0;
            result.timeDifference = 0.0;
            result.isFaster = false;
            result.status = "Invalid (zero time)";
            return result;
        }
        
        double baselineTime = baseline.timeSeconds;
        result.timeDifference = current.timeSeconds - baselineTime;
        
        if (baselineTime == 0.0) {
            result.improvementFactor = current.timeSeconds > 0.0 ? std::numeric_limits<double>::infinity() : 0.0;
            result.isFaster = false;
        } else {
            result.improvementFactor = baselineTime / current.timeSeconds;
            result.isFaster = current.timeSeconds < baselineTime;
        }
        
        if (result.isFaster) {
            result.status = "Faster by " + std::to_string(result.improvementFactor) + "x";
        } else {
            result.status = "Slower by " + std::to_string(std::abs(result.improvementFactor)) + "x";
        }
        
        return result;
    }
    
    size_t getMemoryUsage() {
        // Simplified memory usage - in real implementation would use platform-specific APIs
        return 0;
    }
    
    std::string getSystemInfo() {
        std::ostringstream oss;
        oss << "System: NLM Performance Benchmark";
        return oss.str();
    }
    
    BenchmarkResult runBenchmarkInternal(const Benchmark& benchmark) {
        BenchmarkResult result(benchmark.name, benchmark.description);
        
        PerformanceTimer timer;
        
        timer.start();
        for (size_t i = 0; i < benchmark.iterations; ++i) {
            benchmark.benchmarkFunc();
        }
        timer.stop();
        
        result.iterations = benchmark.iterations;
        result.timeSeconds = timer.elapsed();
        result.timeMilliseconds = timer.elapsedMs();
        result.timeMicroseconds = timer.elapsedUs();
        
        if (result.timeSeconds > 0.0) {
            result.operationsPerSecond = static_cast<double>(benchmark.iterations) / result.timeSeconds;
        }
        
        return result;
    }
};

BenchmarkSuite::BenchmarkSuite() : pImpl(std::make_unique<Impl>()) {}
BenchmarkSuite::~BenchmarkSuite() = default;

void BenchmarkSuite::addBenchmark(const std::string& name, const std::string& description,
                                 std::function<void()> benchmark,
                                 size_t iterations) {
    pImpl->addBenchmark(name, description, std::move(benchmark), iterations);
}

std::vector<BenchmarkResult> BenchmarkSuite::runAll() {
    return pImpl->runAll();
}

BenchmarkResult BenchmarkSuite::runBenchmark(const std::string& name) {
    return pImpl->runBenchmark(name);
}

std::vector<std::string> BenchmarkSuite::getBenchmarkNames() const {
    return pImpl->getBenchmarkNames();
}

size_t BenchmarkSuite::getBenchmarkCount() const {
    return pImpl->getBenchmarkCount();
}

void BenchmarkSuite::clear() {
    pImpl->clear();
}

void BenchmarkSuite::setIterations(size_t iterations) {
    pImpl->setIterations(iterations);
}

void BenchmarkSuite::setIterations(const std::string& name, size_t iterations) {
    pImpl->setIterations(name, iterations);
}

std::string BenchmarkSuite::generateReport(const std::vector<BenchmarkResult>& results) const {
    return pImpl->generateReport(results);
}

bool BenchmarkSuite::saveResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) const {
    return pImpl->saveResults(results, filepath);
}

bool BenchmarkSuite::loadResults(std::vector<BenchmarkResult>& results, const std::string& filepath) const {
    return pImpl->loadResults(results, filepath);
}

BenchmarkSuite::ComparisonResult BenchmarkSuite::compareWithBaseline(const BenchmarkResult& baseline, const BenchmarkResult& current) const {
    return pImpl->compareWithBaseline(baseline, current);
}

size_t BenchmarkSuite::getMemoryUsage() {
    return Impl::getMemoryUsage();
}

std::string BenchmarkSuite::getSystemInfo() {
    return Impl::getSystemInfo();
}

// OptimizationAnalyzer implementation
class OptimizationAnalyzer::Impl {
private:
    std::mutex mutex;
    
public:
    std::string analyzePerformance(const std::vector<BenchmarkResult>& results) const {
        std::ostringstream oss;
        oss << "Performance Analysis Report\n";
        oss << "==========================\n\n";
        
        if (results.empty()) {
            oss << "No benchmarks to analyze.\n";
            return oss.str();
        }
        
        oss << "Summary Statistics:\n";
        double totalTime = 0.0;
        double totalOPS = 0.0;
        
        for (const auto& result : results) {
            totalTime += result.timeSeconds;
            totalOPS += result.operationsPerSecond;
        }
        
        oss << "  Average time per benchmark: " << totalTime / results.size() << "s\n";
        oss << "  Average throughput: " << totalOPS / results.size() << " ops/s\n\n";
        
        return oss.str();
    }
    
    std::vector<std::string> generateRecommendations(const std::vector<BenchmarkResult>& results) const {
        std::vector<std::string> recommendations;
        
        if (results.empty()) {
            return recommendations;
        }
        
        // Find slowest benchmark
        double maxTime = 0.0;
        std::string slowestName;
        
        for (const auto& result : results) {
            if (result.timeSeconds > maxTime) {
                maxTime = result.timeSeconds;
                slowestName = result.name;
            }
        }
        
        if (!slowestName.empty()) {
            recommendations.push_back("Consider optimizing '" + slowestName + "' - it's the slowest benchmark");
        }
        
        return recommendations;
    }
    
    std::vector<std::string> identifyBottlenecks(const std::vector<BenchmarkResult>& results) const {
        std::vector<std::string> bottlenecks;
        
        for (const auto& result : results) {
            if (result.getStandardDeviation() > result.getMeanTime() * 0.5) {
                bottlenecks.push_back(result.name + " - high variance indicates potential instability");
            }
        }
        
        return bottlenecks;
    }
    
    double calculateSpeedup(const BenchmarkResult& baseline, const BenchmarkResult& optimized) const {
        if (baseline.timeSeconds == 0.0) return 0.0;
        return baseline.timeSeconds / optimized.timeSeconds;
    }
    
    std::string analyzeMemoryUsage(const std::vector<BenchmarkResult>& results) const {
        std::ostringstream oss;
        oss << "Memory Usage Analysis:\n";
        
        double totalMemory = 0.0;
        for (const auto& result : results) {
            totalMemory += result.memoryUsageBytes;
        }
        
        if (!results.empty()) {
            oss << "  Average memory per benchmark: " << totalMemory / results.size() << " bytes\n";
        }
        
        return oss.str();
    }
    
    CacheMetrics profileCachePerformance() const {
        CacheMetrics metrics;
        // Simplified implementation
        metrics.cacheEfficiency = 0.85; // Placeholder
        return metrics;
    }
    
    CPUMetrics analyzeCPUUtilization() const {
        CPUMetrics metrics;
        // Simplified implementation
        metrics.utilization = 0.75; // Placeholder
        return metrics;
    }
    
    std::string generateOptimizationReport(const std::vector<BenchmarkResult>& results) const {
        std::ostringstream oss;
        oss << "Optimization Report\n";
        oss << "==================\n\n";
        
        oss << analyzePerformance(results);
        
        auto recommendations = generateRecommendations(results);
        if (!recommendations.empty()) {
            oss << "Recommendations:\n";
            for (const auto& rec : recommendations) {
                oss << "  - " << rec << "\n";
            }
            oss << "\n";
        }
        
        auto bottlenecks = identifyBottlenecks(results);
        if (!bottlenecks.empty()) {
            oss << "Potential Bottlenecks:\n";
            for (const auto& bottleneck : bottlenecks) {
                oss << "  - " << bottleneck << "\n";
            }
            oss << "\n";
        }
        
        oss << analyzeMemoryUsage(results);
        
        return oss.str();
    }
};

OptimizationAnalyzer::OptimizationAnalyzer() : pImpl(std::make_unique<Impl>()) {}
OptimizationAnalyzer::~OptimizationAnalyzer() = default;

std::string OptimizationAnalyzer::analyzePerformance(const std::vector<BenchmarkResult>& results) const {
    return pImpl->analyzePerformance(results);
}

std::vector<std::string> OptimizationAnalyzer::generateRecommendations(const std::vector<BenchmarkResult>& results) const {
    return pImpl->generateRecommendations(results);
}

std::vector<std::string> OptimizationAnalyzer::identifyBottlenecks(const std::vector<BenchmarkResult>& results) const {
    return pImpl->identifyBottlenecks(results);
}

 double OptimizationAnalyzer::calculateSpeedup(const BenchmarkResult& baseline, const BenchmarkResult& optimized) const {
    return pImpl->calculateSpeedup(baseline, optimized);
}

std::string OptimizationAnalyzer::analyzeMemoryUsage(const std::vector<BenchmarkResult>& results) const {
    return pImpl->analyzeMemoryUsage(results);
}

OptimizationAnalyzer::CacheMetrics OptimizationAnalyzer::profileCachePerformance() const {
    return pImpl->profileCachePerformance();
}

OptimizationAnalyzer::CPUMetrics OptimizationAnalyzer::analyzeCPUUtilization() const {
    return pImpl->analyzeCPUUtilization();
}

std::string OptimizationAnalyzer::generateOptimizationReport(const std::vector<BenchmarkResult>& results) const {
    return pImpl->generateOptimizationReport(results);
}

// ThreadPool implementation
class ThreadPool::Impl {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stopFlag;
    size_t threadCount;
    
public:
    Impl(size_t threadCount) : stopFlag(false), threadCount(threadCount) {
        for (size_t i = 0; i < threadCount; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]() {
                            return this->stopFlag || !this->tasks.empty();
                        });
                        
                        if (this->stopFlag && this->tasks.empty()) {
                            return;
                        }
                        
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    ~Impl() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        
        condition.notify_all();
        
        for (auto& worker : workers) {
            worker.join();
        }
    }
    
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<decltype(f(args...))> result = task->get_future();
        
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (stopFlag) {
                throw std::runtime_error("Cannot submit task to stopped thread pool");
            }
            
            tasks.emplace([task]() { (*task)(); });
        }
        
        condition.notify_one();
        
        return result;
    }
    
    void wait() const {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this]() { return tasks.empty(); });
    }
    
    void stop() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        
        condition.notify_all();
    }
    
    size_t getThreadCount() const {
        return threadCount;
    }
    
    bool isRunning() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return !stopFlag || !tasks.empty();
    }
};

ThreadPool::ThreadPool(size_t threadCount) : pImpl(std::make_unique<Impl>(threadCount)) {}
ThreadPool::~ThreadPool() = default;

template<typename F, typename... Args>
auto ThreadPool::submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    return pImpl->submit(std::forward<F>(f), std::forward<Args>(args)...);
}

void ThreadPool::wait() const {
    pImpl->wait();
}

void ThreadPool::stop() {
    pImpl->stop();
}

size_t ThreadPool::getThreadCount() const {
    return pImpl->getThreadCount();
}

bool ThreadPool::isRunning() const {
    return pImpl->isRunning();
}

// SIMDUtils implementation
bool SIMDUtils::hasSSE2() {
#ifdef __x86_64__
    return true;
#else
    return false;
#endif
}

bool SIMDUtils::hasSSE3() {
    // Implementation would check CPU features
    return hasSSE2();
}

bool SIMDUtils::hasSSSE3() {
    return hasSSE3();
}

bool SIMDUtils::hasSSE4() {
    return hasSSSE3();
}

bool SIMDUtils::hasAVX() {
    return hasSSE4();
}

bool SIMDUtils::hasAVX2() {
    return hasAVX();
}

bool SIMDUtils::hasAVX512() {
    return hasAVX2();
}

bool SIMDUtils::hasFMA() {
    return hasAVX();
}

void SIMDUtils::vectorAddSSE2(const float* a, const float* b, float* c, size_t n) {
    // SSE2 implementation
    for (size_t i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vc = _mm_add_ps(va, vb);
        _mm_storeu_ps(c + i, vc);
    }
}

void SIMDUtils::vectorAddAVX(const float* a, const float* b, float* c, size_t n) {
    // AVX implementation
    for (size_t i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }
}

void SIMDUtils::vectorMultiplySSE2(const float* a, const float* b, float* c, size_t n) {
    // SSE2 multiply implementation
    for (size_t i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vc = _mm_mul_ps(va, vb);
        _mm_storeu_ps(c + i, vc);
    }
}

void SIMDUtils::vectorMultiplyAVX(const float* a, const float* b, float* c, size_t n) {
    // AVX multiply implementation
    for (size_t i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }
}

std::string SIMDUtils::getBestSIMDLevel() {
    if (hasAVX512()) return "AVX-512";
    if (hasAVX2()) return "AVX-256";
    if (hasAVX()) return "AVX";
    if (hasSSE4()) return "SSE4.2";
    if (hasSSE2()) return "SSE2";
    return "None";
}

BenchmarkResult SIMDUtils::benchmarkSIMDAdd(size_t size) {
    BenchmarkResult result("SIMD Add", "Vector addition benchmark");
    
    // Allocate aligned memory
    size_t alignedSize = (size + 31) / 32 * 32;
    std::vector<float> a(alignedSize);
    std::vector<float> b(alignedSize);
    std::vector<float> c(alignedSize);
    
    // Initialize with random values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < alignedSize; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    vectorAddAVX(a.data(), b.data(), c.data(), alignedSize);
    
    timer.stop();
    
    result.iterations = 1;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(alignedSize) / result.timeSeconds;
    }
    
    return result;
}

template<typename Func>
void SIMDUtils::autoDispatch(Func&& func, size_t size) {
    if (hasAVX512()) {
        func("AVX-512", size);
    } else if (hasAVX2()) {
        func("AVX-256", size);
    } else if (hasAVX()) {
        func("AVX", size);
    } else if (hasSSE4()) {
        func("SSE4.2", size);
    } else if (hasSSE2()) {
        func("SSE2", size);
    } else {
        func("Scalar", size);
    }
}

// MemoryOptimizer implementation
class MemoryOptimizer::Impl {
private:
    std::mutex mutex;
    
public:
    void* allocateAligned(size_t size, size_t alignment) {
        // Platform-specific aligned allocation
        void* ptr;
#ifdef _WIN32
        ptr = _aligned_malloc(size, alignment);
#else
        ptr = std::aligned_alloc(alignment, size);
#endif
        return ptr;
    }
    
    void freeAligned(void* ptr) {
        if (ptr) {
#ifdef _WIN32
            _aligned_free(ptr);
#else
            std::free(ptr);
#endif
        }
    }
    
    void* allocatePool(size_t poolSize, size_t elementSize) {
        size_t alignedSize = (elementSize + 63) & ~63;
        size_t totalSize = poolSize * alignedSize;
        return allocateAligned(totalSize, 64);
    }
    
    void freePool(void* pool) {
        freeAligned(pool);
    }
    
    size_t getCacheLineSize() {
        return 64; // Most modern systems use 64-byte cache lines
    }
    
    size_t getOptimalAlignment() {
        return 64;
    }
    
    void prefetch(const void* ptr, size_t offset) {
        // Simple prefetch
        _mm_prefetch(static_cast<const char*>(ptr) + offset, _MM_HINT_T0);
    }
    
    BenchmarkResult benchmarkMemoryBandwidth(size_t size) {
        BenchmarkResult result("Memory Bandwidth", "Memory bandwidth benchmark");
        
        // Allocate aligned memory
        size_t alignedSize = (size + 63) & ~63;
        std::vector<uint8_t> a(alignedSize);
        std::vector<uint8_t> b(alignedSize);
        std::vector<uint8_t> c(alignedSize);
        
        // Initialize with pattern
        for (size_t i = 0; i < alignedSize; ++i) {
            a[i] = i % 256;
            b[i] = 255 - a[i];
        }
        
        PerformanceTimer timer;
        timer.start();
        
        // Perform memory operations
        for (size_t i = 0; i < alignedSize; i += 64) {
            c[i] = a[i] + b[i];
        }
        
        timer.stop();
        
        result.iterations = 1;
        result.timeSeconds = timer.elapsed();
        result.timeMilliseconds = timer.elapsedMs();
        result.timeMicroseconds = timer.elapsedUs();
        
        if (result.timeSeconds > 0.0) {
            result.operationsPerSecond = static_cast<double>(alignedSize) / result.timeSeconds;
        }
        
        return result;
    }
    
    BenchmarkResult benchmarkCachePerformance() {
        BenchmarkResult result("Cache Performance", "Cache performance benchmark");
        
        // Allocate three arrays to test cache behavior
        size_t arraySize = 1024 * 1024; // 1MB
        
        std::vector<uint8_t> a(arraySize);
        std::vector<uint8_t> b(arraySize);
        std::vector<uint8_t> c(arraySize);
        
        // Initialize with random values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        
        for (size_t i = 0; i < arraySize; ++i) {
            a[i] = dist(gen);
            b[i] = dist(gen);
        }
        
        PerformanceTimer timer;
        timer.start();
        
        // Sequential access (good cache locality)
        for (size_t i = 0; i < arraySize; ++i) {
            c[i] = a[i] + b[i];
        }
        
        timer.stop();
        
        result.iterations = 1;
        result.timeSeconds = timer.elapsed();
        result.timeMilliseconds = timer.elapsedMs();
        result.timeMicroseconds = timer.elapsedUs();
        
        if (result.timeSeconds > 0.0) {
            result.operationsPerSecond = static_cast<double>(arraySize) / result.timeSeconds;
        }
        
        return result;
    }
};

MemoryOptimizer::MemoryOptimizer() : pImpl(std::make_unique<Impl>()) {}
MemoryOptimizer::~MemoryOptimizer() = default;

void* MemoryOptimizer::allocateAligned(size_t size, size_t alignment) {
    return pImpl->allocateAligned(size, alignment);
}

void MemoryOptimizer::freeAligned(void* ptr) {
    pImpl->freeAligned(ptr);
}

void* MemoryOptimizer::allocatePool(size_t poolSize, size_t elementSize) {
    return pImpl->allocatePool(poolSize, elementSize);
}

void MemoryOptimizer::freePool(void* pool) {
    pImpl->freePool(pool);
}

size_t MemoryOptimizer::getCacheLineSize() {
    return Impl::getCacheLineSize();
}

size_t MemoryOptimizer::getOptimalAlignment() {
    return Impl::getOptimalAlignment();
}

void MemoryOptimizer::prefetch(const void* ptr, size_t offset) {
    Impl::prefetch(ptr, offset);
}

BenchmarkResult MemoryOptimizer::benchmarkMemoryBandwidth(size_t size) {
    return Impl::benchmarkMemoryBandwidth(size);
}

BenchmarkResult MemoryOptimizer::benchmarkCachePerformance() {
    return Impl::benchmarkCachePerformance();
}

// BranchPredictor implementation
BenchmarkResult BranchPredictor::benchmarkBranchPrediction(size_t iterations) {
    BenchmarkResult result("Branch Prediction", "Branch prediction benchmark");
    
    std::vector<bool> conditions(iterations);
    std::mt19937 gen(42);
    
    // Generate random conditions (50% true, 50% false)
    for (size_t i = 0; i < iterations; ++i) {
        conditions[i] = std::bernoulli_distribution(0.5)(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    // Branch prediction benchmark
    int count = 0;
    for (size_t i = 0; i < iterations; ++i) {
        if (conditions[i]) {
            count++;
        }
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult BranchPredictor::benchmarkConditionalBranch(size_t iterations) {
    BenchmarkResult result("Conditional Branch", "Conditional branch benchmark");
    
    std::vector<bool> conditions(iterations);
    std::mt19937 gen(42);
    
    for (size_t i = 0; i < iterations; ++i) {
        conditions[i] = std::bernoulli_distribution(0.5)(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    int count = 0;
    for (size_t i = 0; i < iterations; ++i) {
        if (conditions[i]) {
            count += 2;
        } else {
            count += 3;
        }
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult BranchPredictor::benchmarkLoopBranch(size_t iterations) {
    BenchmarkResult result("Loop Branch", "Loop branch benchmark");
    
    std::vector<int> data(iterations);
    std::mt19937 gen(42);
    
    for (size_t i = 0; i < iterations; ++i) {
        data[i] = gen() % 100;
    }
    
    PerformanceTimer timer;
    timer.start();
    
    int count = 0;
    for (size_t i = 0; i < iterations; ++i) {
        if (data[i] % 2 == 0) {
            count++;
        }
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult BranchPredictor::benchmarkBranchPredictability(size_t iterations) {
    BenchmarkResult result("Branch Predictability", "Branch predictability benchmark");
    
    std::vector<bool> conditions(iterations);
    std::mt19937 gen(42);
    
    // Generate predictable sequence (alternating)
    for (size_t i = 0; i < iterations; ++i) {
        conditions[i] = (i % 2) == 0;
    }
    
    PerformanceTimer timer;
    timer.start();
    
    int count = 0;
    for (size_t i = 0; i < iterations; ++i) {
        if (conditions[i]) {
            count++;
        }
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

// FPOptimizer implementation
BenchmarkResult FPOptimizer::benchmarkFPAddition(size_t iterations) {
    BenchmarkResult result("FP Addition", "Floating-point addition benchmark");
    
    std::vector<float> a(iterations);
    std::vector<float> b(iterations);
    std::vector<float> c(iterations);
    
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < iterations; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    for (size_t i = 0; i < iterations; ++i) {
        c[i] = a[i] + b[i];
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult FPOptimizer::benchmarkFPMultiplication(size_t iterations) {
    BenchmarkResult result("FP Multiplication", "Floating-point multiplication benchmark");
    
    std::vector<float> a(iterations);
    std::vector<float> b(iterations);
    std::vector<float> c(iterations);
    
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < iterations; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    for (size_t i = 0; i < iterations; ++i) {
        c[i] = a[i] * b[i];
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult FPOptimizer::benchmarkFPDivision(size_t iterations) {
    BenchmarkResult result("FP Division", "Floating-point division benchmark");
    
    std::vector<float> a(iterations);
    std::vector<float> b(iterations);
    std::vector<float> c(iterations);
    
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < iterations; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    for (size_t i = 0; i < iterations; ++i) {
        c[i] = a[i] / b[i];
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult FPOptimizer::benchmarkTrigonometric(size_t iterations) {
    BenchmarkResult result("Trigonometric", "Trigonometric functions benchmark");
    
    std::vector<float> a(iterations);
    std::vector<float> b(iterations);
    std::vector<float> c(iterations);
    
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(-3.14159f, 3.14159f);
    
    for (size_t i = 0; i < iterations; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    for (size_t i = 0; i < iterations; ++i) {
        c[i] = std::sin(a[i]) * std::cos(b[i]);
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult FPOptimizer::benchmarkSquareRoot(size_t iterations) {
    BenchmarkResult result("Square Root", "Square root benchmark");
    
    std::vector<float> a(iterations);
    std::vector<float> b(iterations);
    
    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    
    for (size_t i = 0; i < iterations; ++i) {
        a[i] = dist(gen);
        b[i] = dist(gen);
    }
    
    PerformanceTimer timer;
    timer.start();
    
    for (size_t i = 0; i < iterations; ++i) {
        b[i] = std::sqrt(a[i]);
    }
    
    timer.stop();
    
    result.iterations = iterations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(iterations) / result.timeSeconds;
    }
    
    return result;
}

// ConcurrentUtils implementation
template<typename Iterator, typename T>
BenchmarkResult ConcurrentUtils::benchmarkParallelReduce(Iterator begin, Iterator end, T init) {
    BenchmarkResult result("Parallel Reduce", "Parallel reduction benchmark");
    
    size_t size = std::distance(begin, end);
    std::vector<typename std::iterator_traits<Iterator>::value_type> data(begin, end);
    
    PerformanceTimer timer;
    timer.start();
    
    // Use thread pool if available
    ThreadPool pool(std::thread::hardware_concurrency());
    
    auto chunksize = std::max<size_t>(1, size / pool.getThreadCount());
    std::vector<T> partialSums(pool.getThreadCount(), init);
    
    // Submit tasks
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < pool.getThreadCount(); ++i) {
        size_t start = i * chunksize;
        size_t end = (i == pool.getThreadCount() - 1) ? size : start + chunksize;
        
        futures.push_back(pool.submit([&, start, end]() {
            for (size_t j = start; j < end; ++j) {
                partialSums[i] += data[j];
            }
        }));
    }
    
    // Wait for completion
    for (auto& future : futures) {
        future.wait();
    }
    
    // Combine results
    T finalResult = init;
    for (const auto& partial : partialSums) {
        finalResult += partial;
    }
    
    timer.stop();
    
    result.iterations = size;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(size) / result.timeSeconds;
    }
    
    return result;
}

template<typename Iterator>
BenchmarkResult ConcurrentUtils::benchmarkParallelSort(Iterator begin, Iterator end) {
    BenchmarkResult result("Parallel Sort", "Parallel sort benchmark");
    
    size_t size = std::distance(begin, end);
    std::vector<typename std::iterator_traits<Iterator>::value_type> data(begin, end);
    
    PerformanceTimer timer;
    timer.start();
    
    // Use thread pool if available
    ThreadPool pool(std::thread::hardware_concurrency());
    
    // Simple parallel sort (simplified implementation)
    std::sort(data.begin(), data.end());
    
    timer.stop();
    
    result.iterations = size;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(size) / result.timeSeconds;
    }
    
    return result;
}

template<typename Iterator, typename Function>
BenchmarkResult ConcurrentUtils::benchmarkParallelMap(Iterator begin, Iterator end, Function func) {
    BenchmarkResult result("Parallel Map", "Parallel map benchmark");
    
    size_t size = std::distance(begin, end);
    std::vector<typename std::iterator_traits<Iterator>::value_type> data(begin, end);
    
    PerformanceTimer timer;
    timer.start();
    
    // Use thread pool if available
    ThreadPool pool(std::thread::hardware_concurrency());
    
    std::vector<typename std::iterator_traits<Iterator>::value_type> output;
    
    auto chunksize = std::max<size_t>(1, size / pool.getThreadCount());
    std::vector<std::future<void>> futures;
    
    for (size_t i = 0; i < pool.getThreadCount(); ++i) {
        size_t start = i * chunksize;
        size_t end = (i == pool.getThreadCount() - 1) ? size : start + chunksize;
        
        futures.push_back(pool.submit([&, start, end]() {
            for (size_t j = start; j < end; ++j) {
                output.push_back(func(data[j]));
            }
        }));
    }
    
    // Wait for completion
    for (auto& future : futures) {
        future.wait();
    }
    
    timer.stop();
    
    result.iterations = size;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(size) / result.timeSeconds;
    }
    
    return result;
}

BenchmarkResult ConcurrentUtils::benchmarkConcurrentContainer(size_t threadCount, size_t operations) {
    BenchmarkResult result("Concurrent Container", "Concurrent container benchmark");
    
    std::vector<int> container;
    container.reserve(operations);
    
    PerformanceTimer timer;
    timer.start();
    
    ThreadPool pool(threadCount);
    
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < operations; ++i) {
        futures.push_back(pool.submit([&]() {
            // Simulate concurrent operations
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }));
    }
    
    // Wait for completion
    for (auto& future : futures) {
        future.wait();
    }
    
    timer.stop();
    
    result.iterations = operations;
    result.timeSeconds = timer.elapsed();
    result.timeMilliseconds = timer.elapsedMs();
    result.timeMicroseconds = timer.elapsedUs();
    
    if (result.timeSeconds > 0.0) {
        result.operationsPerSecond = static_cast<double>(operations) / result.timeSeconds;
    }
    
    return result;
}

// BenchmarkManager implementation
class BenchmarkManager::Impl {
private:
    std::map<std::string, BenchmarkSuite> suites;
    std::mutex mutex;
    bool initialized;
    
public:
    Impl() : initialized(false) {}
    
    void initialize() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!initialized) {
            initialized = true;
            // Initialize default suites
            addDefaultSuites();
        }
    }
    
    void addSuite(const std::string& name, const BenchmarkSuite& suite) {
        std::lock_guard<std::mutex> lock(mutex);
        suites[name] = suite;
    }
    
    std::vector<std::string> getSuiteNames() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::string> names;
        for (const auto& pair : suites) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<BenchmarkResult> runAllBenchmarks() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<BenchmarkResult> allResults;
        
        for (const auto& pair : suites) {
            auto results = pair.second.runAll();
            allResults.insert(allResults.end(), results.begin(), results.end());
        }
        
        return allResults;
    }
    
    std::vector<BenchmarkResult> runSuite(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = suites.find(name);
        if (it == suites.end()) {
            return std::vector<BenchmarkResult>();
        }
        
        return it->second.runAll();
    }
    
    std::string generateComprehensiveReport(const std::vector<BenchmarkResult>& allResults) const {
        std::ostringstream oss;
        oss << "NLM Comprehensive Performance Report\n";
        oss << "==================================\n\n";
        
        oss << "Total benchmarks: " << allResults.size() << "\n\n";
        
        // Group by category
        std::map<std::string, std::vector<BenchmarkResult>> groupedResults;
        for (const auto& result : allResults) {
            std::string category;
            if (result.name.find("SIMD") != std::string::npos) {
                category = "SIMD Performance";
            } else if (result.name.find("Memory") != std::string::npos) {
                category = "Memory Performance";
            } else if (result.name.find("Branch") != std::string::npos) {
                category = "Branch Prediction";
            } else if (result.name.find("FP") != std::string::npos) {
                category = "Floating-Point";
            } else if (result.name.find("Reduce") != std::string::npos) {
                category = "Parallel Operations";
            } else {
                category = "General";
            }
            
            groupedResults[category].push_back(result);
        }
        
        for (const auto& category : groupedResults) {
            oss << category.first << ":\n";
            for (const auto& result : category.second) {
                oss << "  " << result.toString() << "\n\n";
            }
        }
        
        return oss.str();
    }
    
    bool saveResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Save as CSV
        for (const auto& result : results) {
            file << result.toCSV() << "\n";
        }
        
        return true;
    }
    
    bool loadResults(std::vector<BenchmarkResult>& results, const std::string& filepath) const {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // CSV parsing
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                BenchmarkResult result(tokens[0], tokens[1]);
                result.iterations = std::stoul(tokens[2]);
                result.timeSeconds = std::stod(tokens[3]);
                result.operationsPerSecond = std::stod(tokens[7]);
                result.memoryUsageBytes = std::stoull(tokens[8]);
                results.push_back(result);
            }
        }
        
        return true;
    }
    
    std::string compareWithHistory(const std::vector<BenchmarkResult>& currentResults,
                                  const std::string& historyFile) const {
        std::ostringstream oss;
        oss << "Performance Comparison\n";
        oss << "======================\n\n";
        
        // Load historical results
        std::vector<BenchmarkResult> historicalResults;
        if (loadResults(historicalResults, historyFile)) {
            oss << "Historical data loaded from " << historyFile << "\n\n";
            
            // Compare results
            for (const auto& current : currentResults) {
                auto historicalIt = std::find_if(historicalResults.begin(), historicalResults.end(),
                                                 [&current](const BenchmarkResult& r) { return r.name == current.name; });
                
                if (historicalIt != historicalResults.end()) {
                    auto comparison = BenchmarkSuite::Impl::compareWithBaseline(*historicalIt, current);
                    oss << current.name << ": " << comparison.status << "\n";
                }
            }
        } else {
            oss << "Could not load historical data from " << historyFile << "\n";
        }
        
        return oss.str();
    }
    
    PerformanceStats calculatePerformanceStats(const std::vector<BenchmarkResult>& results) const {
        PerformanceStats stats;
        
        if (results.empty()) return stats;
        
        std::vector<double> times;
        std::vector<double> opsPerSec;
        
        for (const auto& result : results) {
            times.push_back(result.timeSeconds);
            opsPerSec.push_back(result.operationsPerSecond);
        }
        
        // Calculate statistics
        stats.minTime = *std::min_element(times.begin(), times.end());
        stats.maxTime = *std::max_element(times.begin(), times.end());
        
        double sumTimes = std::accumulate(times.begin(), times.end(), 0.0);
        stats.meanTime = sumTimes / times.size();
        
        double sumSquared = 0.0;
        for (double time : times) {
            double diff = time - stats.meanTime;
            sumSquared += diff * diff;
        }
        stats.stddevTime = std::sqrt(sumSquared / (times.size() - 1));
        
        // Calculate median
        std::vector<double> sortedTimes = times;
        std::sort(sortedTimes.begin(), sortedTimes.end());
        size_t mid = sortedTimes.size() / 2;
        stats.medianTime = (sortedTimes.size() % 2 == 0) ? 
            (sortedTimes[mid - 1] + sortedTimes[mid]) / 2 : sortedTimes[mid];
        
        double sumOPS = std::accumulate(opsPerSec.begin(), opsPerSec.end(), 0.0);
        stats.totalIterations = std::accumulate(results.begin(), results.end(), 0, 
                                              [](size_t acc, const BenchmarkResult& r) { return acc + r.iterations; });
        stats.throughput = stats.totalIterations / stats.meanTime;
        stats.efficiency = stats.throughput / stats.totalIterations; // Simplified
        
        return stats;
    }
    
    std::vector<std::string> generateOptimizationRecommendations(const std::vector<BenchmarkResult>& results) const {
        std::vector<std::string> recommendations;
        
        if (results.empty()) return recommendations;
        
        // Find slowest benchmark
        double maxTime = 0.0;
        std::string slowestName;
        
        for (const auto& result : results) {
            if (result.timeSeconds > maxTime) {
                maxTime = result.timeSeconds;
                slowestName = result.name;
            }
        }
        
        if (!slowestName.empty()) {
            recommendations.push_back("Focus optimization efforts on '" + slowestName + "' - it's the performance bottleneck");
        }
        
        // Check for high variance
        for (const auto& result : results) {
            double variance = result.getStandardDeviation();
            double mean = result.getMeanTime();
            if (mean > 0.0 && variance / mean > 0.5) {
                recommendations.push_back("Reduce variance in '" + result.name + "' for more consistent performance");
            }
        }
        
        return recommendations;
    }
    
    std::string getSystemPerformanceProfile() const {
        std::ostringstream oss;
        oss << "NLM System Performance Profile\n";
        oss << "==============================\n\n";
        
        oss << "Benchmark suites available: " << suites.size() << "\n";
        oss << "Total benchmarks: ";
        
        size_t totalBenchmarks = 0;
        for (const auto& pair : suites) {
            totalBenchmarks += pair.second.getBenchmarkCount();
        }
        
        oss << totalBenchmarks << "\n\n";
        
        oss << "Recommended optimization areas:\n";
        oss << "  - SIMD vectorization for numerical operations\n";
        oss << "  - Memory access patterns and prefetching\n";
        oss << "  - Branch prediction for conditional logic\n";
        oss << "  - Concurrent execution for parallel workloads\n";
        
        return oss.str();
    }
    
    void addDefaultSuites() {
        // Add SIMD benchmark suite
        BenchmarkSuite simdSuite;
        simdSuite.addBenchmark("Vector Add", "SIMD vector addition benchmark", 
                              []() { SIMDUtils::vectorAddAVX(nullptr, nullptr, nullptr, 1000); }, 
                              1000);
        addSuite("SIMD Performance", simdSuite);
        
        // Add memory benchmark suite
        BenchmarkSuite memorySuite;
        memorySuite.addBenchmark("Memory Bandwidth", "Memory bandwidth benchmark",
                                []() { MemoryOptimizer::benchmarkMemoryBandwidth(1024 * 1024); },
                                1);
        addSuite("Memory Performance", memorySuite);
        
        // Add branch prediction benchmark suite
        BenchmarkSuite branchSuite;
        branchSuite.addBenchmark("Branch Prediction", "Branch prediction benchmark",
                                []() { BranchPredictor::benchmarkBranchPrediction(10000); },
                                10000);
        addSuite("Branch Prediction", branchSuite);
    }
};

BenchmarkManager::BenchmarkManager() : pImpl(std::make_unique<Impl>()) {}
BenchmarkManager::~BenchmarkManager() = default;

void BenchmarkManager::initialize() {
    pImpl->initialize();
}

void BenchmarkManager::addSuite(const std::string& name, const BenchmarkSuite& suite) {
    pImpl->addSuite(name, suite);
}

std::vector<std::string> BenchmarkManager::getSuiteNames() const {
    return pImpl->getSuiteNames();
}

std::vector<BenchmarkResult> BenchmarkManager::runAllBenchmarks() {
    return pImpl->runAllBenchmarks();
}

std::vector<BenchmarkResult> BenchmarkManager::runSuite(const std::string& name) {
    return pImpl->runSuite(name);
}

std::string BenchmarkManager::generateComprehensiveReport(const std::vector<BenchmarkResult>& allResults) const {
    return pImpl->generateComprehensiveReport(allResults);
}

bool BenchmarkManager::saveResults(const std::vector<BenchmarkResult>& results, const std::string& filepath) const {
    return pImpl->saveResults(results, filepath);
}

bool BenchmarkManager::loadResults(std::vector<BenchmarkResult>& results, const std::string& filepath) const {
    return pImpl->loadResults(results, filepath);
}

std::string BenchmarkManager::compareWithHistory(const std::vector<BenchmarkResult>& currentResults,
                                                const std::string& historyFile) const {
    return pImpl->compareWithHistory(currentResults, historyFile);
}

BenchmarkManager::PerformanceStats BenchmarkManager::calculatePerformanceStats(const std::vector<BenchmarkResult>& results) const {
    return pImpl->calculatePerformanceStats(results);
}

std::vector<std::string> BenchmarkManager::generateOptimizationRecommendations(const std::vector<BenchmarkResult>& results) const {
    return pImpl->generateOptimizationRecommendations(results);
}

std::string BenchmarkManager::getSystemPerformanceProfile() const {
    return pImpl->getSystemPerformanceProfile();
}

// Performance namespace functions
namespace Performance {
    BenchmarkResult benchmarkOperation(const std::string& name, std::function<void()> operation,
                                      size_t iterations) {
        BenchmarkSuite suite;
        suite.addBenchmark(name, "", std::move(operation), iterations);
        auto results = suite.runAll();
        return !results.empty() ? results[0] : BenchmarkResult(name, "");
    }
    
    bool assertPerformance(const std::string& testName, double maxTimeSeconds,
                          std::function<void()> testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        testFunc();
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (elapsed > maxTimeSeconds) {
            NLM_LOG_WARNING("Performance assertion failed: '" + testName + "' took " + 
                           std::to_string(elapsed) + "s (max: " + std::to_string(maxTimeSeconds) + "s)");
            return false;
        }
        
        return true;
    }
    
    BenchmarkResult benchmarkMemory(size_t size) {
        BenchmarkSuite suite;
        suite.addBenchmark("Memory Test", "Memory benchmark",
                          [size]() { MemoryOptimizer::benchmarkMemoryBandwidth(size); },
                          1);
        auto results = suite.runAll();
        return !results.empty() ? results[0] : BenchmarkResult("Memory Test", "");
    }
    
    BenchmarkResult benchmarkCPU(size_t iterations) {
        BenchmarkSuite suite;
        suite.addBenchmark("CPU Test", "CPU benchmark",
                          [iterations]() { BranchPredictor::benchmarkBranchPrediction(iterations); },
                          iterations);
        auto results = suite.runAll();
        return !results.empty() ? results[0] : BenchmarkResult("CPU Test", "");
    }
    
    std::map<std::string, BenchmarkResult> benchmarkSystem() {
        std::map<std::string, BenchmarkResult> results;
        
        BenchmarkManager manager;
        manager.initialize();
        
        auto allResults = manager.runAllBenchmarks();
        
        for (const auto& result : allResults) {
            results[result.name] = result;
        }
        
        return results;
    }
    
    std::string generatePerformanceReport() {
        BenchmarkManager manager;
        manager.initialize();
        
        auto results = manager.runAllBenchmarks();
        return manager.generateComprehensiveReport(results);
    }
    
    std::string getPerformanceStats() {
        BenchmarkManager manager;
        manager.initialize();
        
        auto results = manager.runAllBenchmarks();
        auto stats = manager.calculatePerformanceStats(results);
        
        std::ostringstream oss;
        oss << "Performance Statistics:\n";
        oss << "  Min time: " << stats.minTime << "s\n";
        oss << "  Max time: " << stats.maxTime << "s\n";
        oss << "  Mean time: " << stats.meanTime << "s\n";
        oss << "  Median time: " << stats.medianTime << "s\n";
        oss << "  Std dev: " << stats.stddevTime << "s\n";
        oss << "  Total iterations: " << stats.totalIterations << "\n";
        oss << "  Throughput: " << stats.throughput << " ops/s\n";
        oss << "  Efficiency: " << stats.efficiency << "\n";
        
        return oss.str();
    }
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
    template BenchmarkResult ConcurrentUtils::benchmarkParallelReduce<std::vector<int>::iterator, int>(
        std::vector<int>::iterator, std::vector<int>::iterator, int);
    template BenchmarkResult ConcurrentUtils::benchmarkParallelSort<std::vector<int>::iterator>(
        std::vector<int>::iterator, std::vector<int>::iterator);
    template BenchmarkResult ConcurrentUtils::benchmarkParallelMap<std::vector<int>::iterator, int>(
        std::vector<int>::iterator, std::vector<int>::iterator, std::function<int(int)>);
}