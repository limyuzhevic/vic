// PerformanceImplementation.cpp - Global performance functions implementation
#include "Performance.hpp"
#include "SIMDEngine.hpp"
#include "AdaptiveProcessor.hpp"
#include "PerformanceBudget.hpp"
#include "PerformanceProfiler.hpp"
#include <iostream>

namespace nlm {

// Global configuration and metrics storage
RuntimePerformanceConfig g_runtimeConfig;
PerformanceMetrics g_currentMetrics;
bool g_performanceMonitoringActive = false;

void setRuntimePerformanceConfig(const RuntimePerformanceConfig& config) {
    g_runtimeConfig = config;
    
    // Update runtime instances with new configuration
    SIMDEngine::getInstance().setPreferAVX512(config.preferAVX512);
    SIMDEngine::getInstance().setPreferAVX2(config.preferAVX2);
    SIMDEngine::getInstance().setPreferSSE(config.preferSSE);
    
    if (PerformanceProfiler::getInstance().isEnabled()) {
        std::cout << "[PERFORMANCE] Runtime configuration updated:" << std::endl;
        std::cout << "[PERFORMANCE]   Memory pool: " << (config.useMemoryPool ? "Enabled" : "Disabled") << std::endl;
        std::cout << "[PERFORMANCE]   Multithreading: " << (config.useMultithreading ? "Enabled" : "Disabled") << std::endl;
        std::cout << "[PERFORMANCE]   SIMD: " << (config.useSIMD ? "Enabled" : "Disabled") << std::endl;
        std::cout << "[PERFORMANCE]   Thread count: " << config.threadCount << std::endl;
    }
}

RuntimePerformanceConfig getRuntimePerformanceConfig() {
    return g_runtimeConfig;
}

void setCurrentPerformanceMetrics(const PerformanceMetrics& metrics) {
    g_currentMetrics = metrics;
}

PerformanceMetrics getCurrentPerformanceMetrics() {
    return g_currentMetrics;
}

void startPerformanceMonitoring() {
    if (!g_performanceMonitoringActive) {
        g_performanceMonitoringActive = true;
        
        // Initialize performance engine
        SIMDEngine::getInstance().detectSIMDCapabilities();
        
        // Start profiler if enabled
        PerformanceProfiler::getInstance().enable(true);
        
        if (PerformanceProfiler::getInstance().isEnabled()) {
            std::cout << "[PERFORMANCE] Performance monitoring started" << std::endl;
        }
    }
}

void stopPerformanceMonitoring() {
    if (g_performanceMonitoringActive) {
        g_performanceMonitoringActive = false;
        
        // Stop profiler
        PerformanceProfiler::getInstance().enable(false);
        
        if (PerformanceProfiler::getInstance().isEnabled()) {
            std::cout << "[PERFORMANCE] Performance monitoring stopped" << std::endl;
        }
    }
}

bool isPerformanceMonitoringActive() {
    return g_performanceMonitoringActive;
}

} // namespace nlm