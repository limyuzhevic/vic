#pragma once

#include "PerformanceOptimizer.hpp"
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <random>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>

namespace nlm {

class PerformanceOptimizer {
public:
    struct OptimizationConfig {
        bool enableSIMD{true};
        bool enableParallelization{true};
        bool enableCaching{true};
        bool enableVectorization{true};
        int maxThreads{std::thread::hardware_concurrency()};
        size_t cacheSize{1024 * 1024}; // 1MB default
        float optimizationThreshold{0.8f}; // Optimize if > 80% of potential
        bool adaptiveOptimization{true};
    };

    PerformanceOptimizer();
    ~PerformanceOptimizer();

    // Disable copying, enable moving
    PerformanceOptimizer(const PerformanceOptimizer&) = delete;
    PerformanceOptimizer& operator=(const PerformanceOptimizer&) = delete;
    PerformanceOptimizer(PerformanceOptimizer&&) noexcept;
    PerformanceOptimizer& operator=(PerformanceOptimizer&&) noexcept;

    // Configuration
    void configure(const OptimizationConfig& config);
    const OptimizationConfig& getConfig() const;
    
    // Algorithm optimization
    template<typename Iterator, typename Predicate>
    void optimizeForParallel(Iterator begin, Iterator end, Predicate pred);
    
    template<typename Iterator, typename Operation>
    void optimizeForVectorization(Iterator begin, Iterator end, Operation op);
    
    // Memory optimization
    template<typename T>
    std::vector<T> optimizeMemoryAccess(const std::vector<T>& data);
    
    // Cache optimization
    template<typename Key, typename Value>
    class Cache {
    public:
        Cache(size_t maxSize) : maxSize(maxSize) {}
        void put(const Key& key, const Value& value);
        bool get(const Key& key, Value& value) const;
        void clear();
        size_t size() const;
    
    private:
        std::unordered_map<Key, Value> cache;
        size_t maxSize;
        mutable std::mutex mutex;
    };

    // Performance analysis
    template<typename Container>
    double analyzeComplexity(const Container& data) const;
    
    template<typename Iterator>
    double calculateSpatialLocality(Iterator begin, Iterator end) const;
    
    // Adaptive optimization
    void monitorAndOptimize();
    void enableAdaptiveMode(bool enable);
    
    // SIMD operations
    static float vectorizedSum(const std::vector<float>& data);
    static void vectorizedCopy(const std::vector<float>& src, std::vector<float>& dst);
    static void vectorizedScale(std::vector<float>& data, float scalar);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
