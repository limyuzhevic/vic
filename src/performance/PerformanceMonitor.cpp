#pragma once

#include "PerformanceMonitor.hpp"
#include "core/Logger/Logger.hpp"
#include <unordered_map>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

// Performance monitoring implementation
struct PerformanceMonitor::Impl {
    std::chrono::system_clock::time_point lastFrameTime;
    double lastFrameDuration = 0.0;
    double fps = 0.0;
    
    std::unordered_map<std::string, PerformanceMetric> metrics;
    std::vector<PerformanceEvent> eventHistory;
    
    size_t maxHistorySize = 1000;
    bool recordHistory = true;
    
    // Frame timing
    size_t frameCount = 0;
    double totalFrameTime = 0.0;
    
    // Performance thresholds for warnings
    double slowFrameThreshold = 0.033;  // 33ms = 30 FPS
    double verySlowFrameThreshold = 0.1;  // 100ms = 10 FPS
    
    // Memory tracking
    size_t peakMemoryUsage = 0;
    size_t currentMemoryUsage = 0;
    
    Impl() {
        lastFrameTime = std::chrono::system_clock::now();
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startFrame() {
    pImpl->lastFrameTime = std::chrono::system_clock::now();
}

void PerformanceMonitor::endFrame() {
    auto currentTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->lastFrameTime).count();
    
    pImpl->lastFrameDuration = duration;
    pImpl->frameCount++;
    pImpl->totalFrameTime += duration;
    
    // Calculate FPS
    if (pImpl->totalFrameTime > 0.0) {
        pImpl->fps = pImpl->frameCount / pImpl->totalFrameTime;
    }
    
    // Record frame time in metrics
    recordMetric("frame_time", duration);
    
    // Check for slow frames
    if (duration > pImpl->slowFrameThreshold) {
        auto logger = Logger::getGlobal();
        logger->warning("Slow frame detected: " + std::to_string(duration * 1000) + "ms");
    }
    
    if (duration > pImpl->verySlowFrameThreshold) {
        auto logger = Logger::getGlobal();
        logger->critical("Very slow frame detected: " + std::to_string(duration * 1000) + "ms");
    }
}

double PerformanceMonitor::getFPS() const {
    return pImpl->fps;
}

double PerformanceMonitor::getLastFrameTime() const {
    return pImpl->lastFrameDuration;
}

double PerformanceMonitor::getAverageFrameTime() const {
    if (pImpl->frameCount == 0) return 0.0;
    return pImpl->totalFrameTime / pImpl->frameCount;
}

void PerformanceMonitor::recordMetric(const std::string& name, double value) {
    auto& metric = pImpl->metrics[name];
    metric.value = value;
    metric.lastUpdate = std::chrono::system_clock::now();
    
    if (pImpl->recordHistory) {
        metric.history.push_back({value, std::chrono::system_clock::now()});
        if (metric.history.size() > pImpl->maxHistorySize) {
            metric.history.erase(metric.history.begin());
        }
    }
}

const PerformanceMetric& PerformanceMonitor::getMetric(const std::string& name) const {
    static const PerformanceMetric emptyMetric{0.0, std::chrono::system_clock::now()};
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return it->second;
    }
    return emptyMetric;
}

std::vector<PerformanceMetric> PerformanceMonitor::getAllMetrics() const {
    std::vector<PerformanceMetric> result;
    result.reserve(pImpl->metrics.size());
    
    for (const auto& pair : pImpl->metrics) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<PerformanceEvent> PerformanceMonitor::getRecentEvents(size_t limit) const {
    std::vector<PerformanceEvent> result;
    result.reserve(std::min(pImpl->eventHistory.size(), limit));
    
    if (limit > pImpl->eventHistory.size()) {
        limit = pImpl->eventHistory.size();
    }
    
    auto startIt = pImpl->eventHistory.end() - limit;
    for (auto it = startIt; it != pImpl->eventHistory.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void PerformanceMonitor::addEvent(const std::string& name, const std::string& description) {
    PerformanceEvent event;
    event.name = name;
    event.description = description;
    event.timestamp = std::chrono::system_clock::now();
    
    if (pImpl->recordHistory) {
        pImpl->eventHistory.push_back(event);
        if (pImpl->eventHistory.size() > pImpl->maxHistorySize) {
            pImpl->eventHistory.erase(pImpl->eventHistory.begin());
        }
    }
}

void PerformanceMonitor::setHistorySize(size_t size) {
    pImpl->maxHistorySize = size;
}

void PerformanceMonitor::enableHistory(bool enable) {
    pImpl->recordHistory = enable;
}

void PerformanceMonitor::setSlowFrameThreshold(double threshold) {
    pImpl->slowFrameThreshold = threshold;
}

void PerformanceMonitor::setVerySlowFrameThreshold(double threshold) {
    pImpl->verySlowFrameThreshold = threshold;
}

void PerformanceMonitor::updateMemoryUsage(size_t memoryBytes) {
    pImpl->currentMemoryUsage = memoryBytes;
    if (memoryBytes > pImpl->peakMemoryUsage) {
        pImpl->peakMemoryUsage = memoryBytes;
    }
}

size_t PerformanceMonitor::getPeakMemoryUsage() const {
    return pImpl->peakMemoryUsage;
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    return pImpl->currentMemoryUsage;
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::ostringstream oss;
    oss << "=== NLM Performance Report ===" << std::endl;
    oss << std::endl;
    
    // Frame statistics
    oss << "Frame Statistics:" << std::endl;
    oss << "  FPS: " << pImpl->fps << std::endl;
    oss << "  Last frame: " << pImpl->lastFrameDuration * 1000 << "ms" << std::endl;
    oss << "  Average frame: " << getAverageFrameTime() * 1000 << "ms" << std::endl;
    oss << "  Total frames: " << pImpl->frameCount << std::endl;
    oss << std::endl;
    
    // Memory statistics
    oss << "Memory Statistics:" << std::endl;
    oss << "  Current usage: " << (pImpl->currentMemoryUsage / 1024) << " KB" << std::endl;
    oss << "  Peak usage: " << (pImpl->peakMemoryUsage / 1024) << " KB" << std::endl;
    oss << std::endl;
    
    // Metrics
    oss << "Key Metrics:" << std::endl;
    for (const auto& pair : pImpl->metrics) {
        if (pair.first == "frame_time") continue;  // Already shown
        oss << "  " << pair.first << ": " << pair.second.value << std::endl;
    }
    oss << std::endl;
    
    return oss.str();
}

} // namespace nlm
