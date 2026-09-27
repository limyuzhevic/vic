#include "PerformanceIntegration.hpp"
#include "Performance.hpp"
#include "MemoryPool.hpp"
#include "SIMDVectorization.hpp"
#include "Matrix4x4.hpp"
#include "ParallelProcessing.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace nlm {

// Initialize static members
std::unique_ptr<PerformanceProfiler> PerformanceProfiler::instance_ = nullptr;
PerformanceStats PerformanceProfiler::globalStats_ = PerformanceStats();
PerformanceConfig PerformanceProfiler::globalConfig_ = PerformanceConfig();

PerformanceIntegration::Profiler::Profiler(const std::string& name)
    : name_(name) {
    start();
}

PerformanceIntegration::Profiler::~Profiler() {
    stop();
    if (PerformanceProfiler::instance_) {
        PerformanceProfiler::instance_->addProfile(name_, getDuration());
    }
}

void PerformanceIntegration::Profiler::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceIntegration::Profiler::stop() {
    endTime_ = std::chrono::high_resolution_clock::now();
}

double PerformanceIntegration::Profiler::getDuration() const {
    if (endTime_.time_since_epoch().count() == 0) {
        return 0.0;
    }
    auto duration = std::chrono::duration<double, std::milli>(endTime_ - startTime_);
    return duration.count() / 1000.0;
}

void PerformanceIntegration::Profiler::reportAll() {
    if (PerformanceProfiler::instance_) {
        PerformanceProfiler::instance_->reportAll();
    }
}

PerformanceStats PerformanceIntegration::Profiler::getAggregateStats() {
    return PerformanceProfiler::getGlobalStats();
}

void PerformanceIntegration::initialize(const PerformanceConfig& config) {
    // Store configuration
    config_ = config;
    
    // Setup hardware detection
    setupHardwareDetection();
    
    // Setup optimizations based on configuration and hardware
    if (config.enableMemoryPools && simdAvailable_) {
        setupMemoryPools();
    }
    
    if (config.enableSIMD && simdAvailable_) {
        setupSIMD();
    }
    
    if (config.enableMatrixAccel && simdAvailable_) {
        setupMatrixLib();
    }
    
    if (config.enableParallel && parallelAvailable_) {
        setupParallelProcessing();
    }
    
    // Enable all optimizations by default
    useMemoryPools_ = config.enableMemoryPools;
    useSIMD_ = config.enableSIMD;
    useParallel_ = config.enableParallel;
    
    // Initialize performance profiler if enabled
    if (config.enableProfiling || config.enableBenchmarking) {
        PerformanceProfiler::initialize(config);
    }
}

void PerformanceIntegration::cleanup() {
    if (PerformanceProfiler::instance_) {
        PerformanceProfiler::instance_.reset();
    }
    
    neuronPool_.reset();
    synapsePool_.reset();
    simdVectorization_.reset();
    matrixLib_.reset();
    threadPool_.reset();
}

void PerformanceIntegration::setupHardwareDetection() {
    // Detect CPU capabilities
    simdAvailable_ = SIMDVectorization::detectSIMDSupport();
    avxAvailable_ = SIMDVectorization::hasAVX();
    avx2Available_ = SIMDVectorization::hasAVX2();
    avx512Available_ = SIMDVectorization::hasAVX512();
    neonAvailable_ = SIMDVectorization::hasNEON();
    
    // Get core count
    coreCount_ = std::thread::hardware_concurrency();
    parallelAvailable_ = (coreCount_ > 1);
    
    // Log hardware detection
    std::cout << "Hardware detection: SIMD=" << (simdAvailable_ ? "Yes" : "No") << std::endl;
    std::cout << "  AVX=" << (avxAvailable_ ? "Yes" : "No") << std::endl;
    std::cout << "  AVX2=" << (avx2Available_ ? "Yes" : "No") << std::endl;
    std::cout << "  AVX512=" << (avx512Available_ ? "Yes" : "No") << std::endl;
    std::cout << "  NEON=" << (neonAvailable_ ? "Yes" : "No") << std::endl;
    std::cout << "  Cores=" << coreCount_ << std::endl;
}

void PerformanceIntegration::setupMemoryPools() {
    try {
        neuronPool_ = std::make_unique<MemoryPool<Neuron>>(1024, 4);
        synapsePool_ = std::make_unique<MemoryPool<Synapse>>(1024, 4);
        
        std::cout << "Memory pools initialized: " << config_.neuronPoolSize 
                  << " neurons, " << config_.synapsePoolSize << " synapses" << std::endl;
    } catch (const std::bad_alloc&) {
        std::cerr << "Warning: Failed to initialize memory pools" << std::endl;
    }
}

void PerformanceIntegration::setupSIMD() {
    try {
        simdVectorization_ = std::make_unique<SIMDVectorization>();
        std::cout << "SIMD vectorization initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to initialize SIMD: " << e.what() << std::endl;
    }
}

void PerformanceIntegration::setupMatrixLib() {
    try {
        matrixLib_ = std::make_unique<Matrix4x4>();
        std::cout << "Matrix library initialized" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to initialize matrix library: " << e.what() << std::endl;
    }
}

void PerformanceIntegration::setupParallelProcessing() {
    try {
        threadPool_ = std::make_unique<ThreadPool>(getOptimalThreadCount());
        std::cout << "Parallel processing initialized with " << getOptimalThreadCount() 
                  << " threads" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to initialize parallel processing: " << e.what() << std::endl;
    }
}

bool PerformanceIntegration::isEnabled(const std::string& name) const {
    if (name == "memory_pools") return useMemoryPools_;
    if (name == "simd") return useSIMD_;
    if (name == "parallel") return useParallel_;
    if (name == "matrix") return (matrixLib_ != nullptr);
    return false;
}

PerformanceStats PerformanceIntegration::getStats() const {
    PerformanceStats stats;
    
    // Memory statistics
    if (neuronPool_) {
        stats.totalMemoryAllocated = neuronPool_->memoryUsage() + 
                                     (synapsePool_ ? synapsePool_->memoryUsage() : 0);
        stats.peakMemoryUsage = stats.totalMemoryAllocated;
        stats.allocationCount = neuronPool_->allocatedCount() + 
                                (synapsePool_ ? synapsePool_->allocatedCount() : 0);
        stats.deallocationCount = 0; // Would need to track this
    }
    
    // SIMD statistics
    stats.simdActive = (simdVectorization_ != nullptr);
    if (simdVectorization_) {
        // Would get actual SIMD instructions count from profiler
    }
    
    // Parallel statistics
    stats.parallelActive = (threadPool_ != nullptr);
    if (threadPool_) {
        stats.activeThreads = threadPool_->getThreadCount();
    }
    
    // Component status
    stats.componentStatus["memory_pools"] = (neuronPool_ != nullptr);
    stats.componentStatus["simd"] = (simdVectorization_ != nullptr);
    stats.componentStatus["parallel"] = (threadPool_ != nullptr);
    stats.componentStatus["matrix"] = (matrixLib_ != nullptr);
    
    return stats;
}

void PerformanceIntegration::configure(const PerformanceConfig& config) {
    config_ = config;
    
    // Re-enable optimizations if they were previously enabled
    if (config.enableMemoryPools) enableMemoryPools(true);
    if (config.enableSIMD) enableSIMD(true);
    if (config.enableParallel) enableParallel(true);
}

PerformanceIntegration::~PerformanceIntegration() {
    cleanup();
}

PerformanceProfiler::PerformanceProfiler(const std::string& component)
    : component_(component) {
}

PerformanceProfiler::~PerformanceProfiler() {
    if (instance_) {
        instance_->addProfile(component_, getDuration());
    }
}

void PerformanceProfiler::initialize(const PerformanceConfig& config) {
    if (!instance_) {
        instance_ = std::make_unique<PerformanceProfiler>("__system__");
    }
    globalConfig_ = config;
}

PerformanceStats PerformanceProfiler::getGlobalStats() {
    return globalStats_;
}

void PerformanceProfiler::reportPerformance(const std::string& filename) {
    std::cout << "Performance Report" << std::endl;
    std::cout << "=================" << std::endl;
    std::cout << "Total Memory Allocated: " << globalStats_.totalMemoryAllocated << " bytes" << std::endl;
    std::cout << "Peak Memory Usage: " << globalStats_.peakMemoryUsage << " bytes" << std::endl;
    std::cout << "SIMD Active: " << (globalStats_.simdActive ? "Yes" : "No") << std::endl;
    std::cout << "Parallel Active: " << (globalStats_.parallelActive ? "Yes" : "No") << std::endl;
    std::cout << "Average FPS: " << globalStats_.averageFPS << std::endl;
    std::cout << "Average Memory Per Frame: " << globalStats_.averageMemoryPerFrame << " bytes" << std::endl;
}

} // namespace nlm
