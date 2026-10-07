#include "RealTimeDebugSystem.hpp"
#include <cmath>
#include <fstream>
#include <algorithm>

namespace nlm {

// DebugEventQueue implementation
DebugEventQueue::DebugEventQueue() : shouldStop(false), eventCount(0) {}

void DebugEventQueue::pushStatus(const std::string& event) {
    std::lock_guard<std::mutex> lock(eventMutex);
    events.push_back({std::chrono::system_clock::now(), "STATUS: " + event});
    eventCount++;
    eventCV.notify_one();
}

void DebugEventQueue::pushPerformance(double value) {
    std::lock_guard<std::mutex> lock(eventMutex);
    perfEvents.push_back({std::chrono::system_clock::now(), value});
    eventCount++;
    eventCV.notify_one();
}

void DebugEventQueue::pushMemory(size_t value) {
    std::lock_guard<std::mutex> lock(eventMutex);
    memEvents.push_back({std::chrono::system_clock::now(), value});
    eventCount++;
    eventCV.notify_one();
}

std::deque<DebugEventQueue::DebugEvent> DebugEventQueue::getEvents(size_t maxCount) {
    std::lock_guard<std::mutex> lock(eventMutex);
    std::deque<DebugEvent> result;
    
    auto it = events.end();
    std::advance(it, -std::min(maxCount, static_cast<size_t>(events.size())));
    
    for (auto iter = it; iter != events.end(); ++iter) {
        result.push_back({iter->first, "status", iter->second, 0.0});
    }
    
    it = perfEvents.end();
    std::advance(it, -std::min(maxCount, static_cast<size_t>(perfEvents.size())));
    
    for (auto iter = it; iter != perfEvents.end(); ++iter) {
        result.push_back({iter->first, "performance", "", iter->second});
    }
    
    it = memEvents.end();
    std::advance(it, -std::min(maxCount, static_cast<size_t>(memEvents.size())));
    
    for (auto iter = it; iter != memEvents.end(); ++iter) {
        result.push_back({iter->first, "memory", "", static_cast<double>(iter->second)});
    }
    
    return result;
}

void DebugEventQueue::clear() {
    std::lock_guard<std::mutex> lock(eventMutex);
    events.clear();
    perfEvents.clear();
    memEvents.clear();
    eventCount = 0;
}

size_t DebugEventQueue::getEventCount() const {
    return eventCount;
}

void DebugEventQueue::start() {
    shouldStop = false;
    workerThread = std::thread(&DebugEventQueue::eventProcessor, this);
}

void DebugEventQueue::stop() {
    shouldStop = true;
    eventCV.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void DebugEventQueue::eventProcessor() {
    while (!shouldStop) {
        std::unique_lock<std::mutex> lock(eventMutex);
        if (eventCV.wait_for(lock, std::chrono::milliseconds(100), 
                           [this] { return !events.empty() || shouldStop; })) {
            
            if (shouldStop) break;
            
            // Process events periodically
            auto now = std::chrono::system_clock::now();
            
            // Clean old events (older than 10 minutes)
            auto cutoff = now - std::chrono::minutes(10);
            
            auto removeOldEvents = [cutoff](std::deque<std::pair<std::chrono::system_clock::time_point, std::string>>& events) {
                while (!events.empty() && events.front().first < cutoff) {
                    events.pop_front();
                }
            };
            
            removeOldEvents(events);
            removeOldEvents(perfEvents);
            removeOldEvents(memEvents);
        }
    }
}

// RealTimeProfiler implementation
void RealTimeProfiler::startFrame() {
    frameStartTime = std::chrono::high_resolution_clock::now();
}

void RealTimeProfiler::endFrame() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - frameStartTime);
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    double frameTime = duration.count() / 1000.0; // Convert to seconds
    frameTimes.push_back(frameTime);
    
    if (frameTimes.size() > 1000) {
        frameTimes.pop_front();
    }
    
    totalTime += frameTime;
    callCount++;
    
    if (callCount > 0) {
        currentFPS = 1.0 / (totalTime / callCount);
    }
    
    if (frameTimes.size() > 0) {
        avgFrameTime = 0.0;
        maxFrameTime = 0.0;
        minFrameTime = std::numeric_limits<double>::max();
        
        for (double time : frameTimes) {
            avgFrameTime += time;
            if (time > maxFrameTime) maxFrameTime = time;
            if (time < minFrameTime) minFrameTime = time;
        }
        
        avgFrameTime /= frameTimes.size();
    }
}

// SystemResourceMonitor implementation
void SystemResourceMonitor::monitorWorker() {
    while (monitorRunning) {
        updateResourceStats();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SystemResourceMonitor::updateResourceStats() {
    // Update CPU usage (simplified calculation)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    double currentCPU = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1000.0 +
                       (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1000.0;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (cpuSamples == 0) {
        cpuUsage = currentCPU;
        totalCPUUsage = currentCPU;
    } else {
        cpuUsage = (cpuUsage + currentCPU) / 2.0;
        totalCPUUsage += currentCPU;
    }
    
    cpuSamples++;
    
    // Update memory usage
    memoryUsage = usage.ru_maxrss * 1024; // Convert KB to bytes
    totalMemoryUsage += memoryUsage;
    
    if (memoryUsage > peakMemoryUsage) {
        peakMemoryUsage = memoryUsage;
    }
}

// RealTimeNeuralMonitor implementation
void RealTimeNeuralMonitor::monitorWorker() {
    while (monitorRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// RealTimeSimulationMonitor implementation
void RealTimeSimulationMonitor::monitorWorker() {
    while (monitorRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// DebugSystem implementation
DebugSystem::DebugSystem() : debugState(), eventQueue(), profiler(), resourceMonitor(), 
                   neuralMonitor(), simulationMonitor(), logger(nullptr) {
    debugState.startTime = std::chrono::system_clock::now();
    debugState.lastUpdateTime = debugState.startTime;
    
    // Start monitoring systems
    resourceMonitor.startMonitoring();
    neuralMonitor.startMonitoring();
    simulationMonitor.startMonitoring();
    eventQueue.start();
}

DebugSystem::~DebugSystem() {
    stopAllMonitoring();
}

void DebugSystem::update() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration<double>(now - debugState.lastUpdateTime);
    
    if (elapsed.count() >= 1.0) { // Update every second
        debugState.lastUpdateTime = now;
        
        // Update statistics
        debugState.currentFPS = profiler.getCurrentFPS();
        debugState.currentMemoryUsage = static_cast<size_t>(resourceMonitor.getMemoryUsage());
        debugState.peakMemoryUsage = static_cast<size_t>(resourceMonitor.getPeakMemoryUsage());
        debugState.currentCPULoad = resourceMonitor.getCPUUsage();
        debugState.currentGPULoad = resourceMonitor.getGPULoad();
        
        debugState.totalDebugEvents = eventQueue.getEventCount();
        
        // Update time series
        debugState.performanceEvents.push_back({now, debugState.currentFPS});
        debugState.memoryEvents.push_back({now, debugState.currentMemoryUsage});
        
        // Clean old events
        auto cutoff = now - std::chrono::minutes(5);
        
        auto removeOldEvents = [cutoff](std::deque<std::pair<std::chrono::system_clock::time_point, double>>& events) {
            while (!events.empty() && events.front().first < cutoff) {
                events.pop_front();
            }
        };
        
        removeOldEvents(debugState.performanceEvents);
        removeOldEvents(debugState.memoryEvents);
    }
}

} // namespace nlm
