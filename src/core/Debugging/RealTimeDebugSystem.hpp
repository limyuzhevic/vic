#pragma once

#include <memory>
#include <unordered_map>
#include <deque>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Real-time debugging system state
struct DebugState {
    std::atomic<bool> isMonitoring{false};
    std::atomic<bool> isProfiling{false};
    std::atomic<bool> isMemoryTracking{false};
    std::atomic<bool> isNeuralTracking{false};
    std::atomic<bool> isSimulationTracking{false};
    
    uint64_t totalDebugEvents{0};
    double currentFPS{0.0};
    size_t currentMemoryUsage{0};
    size_t peakMemoryUsage{0};
    double currentCPULoad{0.0};
    double currentGPULoad{0.0};
    
    std::chrono::system_clock::time_point lastUpdateTime;
    std::chrono::system_clock::time_point startTime;
    
    // Event buffers
    std::deque<std::pair<std::chrono::system_clock::time_point, std::string>> statusEvents;
    std::deque<std::pair<std::chrono::system_clock::time_point, double>> performanceEvents;
    std::deque<std::pair<std::chrono::system_clock::time_point, size_t>> memoryEvents;
    
    DebugState() {
        startTime = std::chrono::system_clock::now();
        lastUpdateTime = startTime;
    }
};

// Thread-safe event queue for real-time debugging
class DebugEventQueue {
public:
    DebugEventQueue() : shouldStop(false), eventCount(0) {}
    
    void pushStatus(const std::string& event) {
        std::lock_guard<std::mutex> lock(eventMutex);
        events.push_back({std::chrono::system_clock::now(), "STATUS: " + event});
        eventCount++;
        eventCV.notify_one();
    }
    
    void pushPerformance(double value) {
        std::lock_guard<std::mutex> lock(eventMutex);
        perfEvents.push_back({std::chrono::system_clock::now(), value});
        eventCount++;
        eventCV.notify_one();
    }
    
    void pushMemory(size_t value) {
        std::lock_guard<std::mutex> lock(eventMutex);
        memEvents.push_back({std::chrono::system_clock::now(), value});
        eventCount++;
        eventCV.notify_one();
    }
    
    struct DebugEvent {
        std::chrono::system_clock::time_point timestamp;
        std::string type;
        std::string data;
        double value;
    };
    
    std::deque<DebugEvent> getEvents(size_t maxCount = 1000) {
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
    
    void clear() {
        std::lock_guard<std::mutex> lock(eventMutex);
        events.clear();
        perfEvents.clear();
        memEvents.clear();
        eventCount = 0;
    }
    
    size_t getEventCount() const {
        return eventCount;
    }
    
    void start() {
        shouldStop = false;
        workerThread = std::thread(&DebugEventQueue::eventProcessor, this);
    }
    
    void stop() {
        shouldStop = true;
        eventCV.notify_one();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    
    ~DebugEventQueue() {
        stop();
    }
    
private:
    void eventProcessor() {
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
    
    std::mutex eventMutex;
    std::condition_variable eventCV;
    bool shouldStop;
    std::thread workerThread;
    size_t eventCount;
    
    std::deque<std::pair<std::chrono::system_clock::time_point, std::string>> events;
    std::deque<std::pair<std::chrono::system_clock::time_point, double>> perfEvents;
    std::deque<std::pair<std::chrono::system_clock::time_point, double>> memEvents;
};

// Real-time performance profiler
class RealTimeProfiler {
public:
    RealTimeProfiler() : isRunning(false), totalTime(0.0), callCount(0), currentFPS(0.0), 
                         avgFrameTime(0.0), maxFrameTime(0.0), minFrameTime(0.0) {}
    
    void startFrame() {
        frameStartTime = std::chrono::high_resolution_clock::now();
    }
    
    void endFrame() {
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
    
    double getCurrentFPS() const {
        return currentFPS;
    }
    
    double getAvgFrameTime() const {
        return avgFrameTime;
    }
    
    double getMaxFrameTime() const {
        return maxFrameTime;
    }
    
    double getMinFrameTime() const {
        return minFrameTime;
    }
    
    std::vector<double> getFrameTimes() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return std::vector<double>(frameTimes.begin(), frameTimes.end());
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(dataMutex);
        frameTimes.clear();
        totalTime = 0.0;
        callCount = 0;
        currentFPS = 0.0;
        avgFrameTime = 0.0;
        maxFrameTime = 0.0;
        minFrameTime = 0.0;
    }
    
private:
    mutable std::mutex dataMutex;
    std::chrono::high_resolution_clock::time_point frameStartTime;
    
    std::deque<double> frameTimes;
    double totalTime;
    size_t callCount;
    
    double currentFPS;
    double avgFrameTime;
    double maxFrameTime;
    double minFrameTime;
};

// System resource monitor
class SystemResourceMonitor {
public:
    SystemResourceMonitor() : cpuUsage(0.0), memoryUsage(0), cpuSamples(0), 
                              isMonitoring(false), monitorThread(), monitorRunning(false) {}
    
    void startMonitoring() {
        isMonitoring = true;
        monitorRunning = true;
        monitorThread = std::thread(&SystemResourceMonitor::monitorWorker, this);
    }
    
    void stopMonitoring() {
        isMonitoring = false;
        monitorRunning = false;
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }
    
    ~SystemResourceMonitor() {
        stopMonitoring();
    }
    
    double getCPUUsage() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return cpuUsage;
    }
    
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return memoryUsage;
    }
    
    size_t getPeakMemoryUsage() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return peakMemoryUsage;
    }
    
    double getGPULoad() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return gpuLoad;
    }
    
    struct ResourceStats {
        double cpuUsage;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        double gpuLoad;
        double averageCPUUsage;
        size_t averageMemoryUsage;
    };
    
    ResourceStats getStats() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        ResourceStats stats;
        stats.cpuUsage = cpuUsage;
        stats.memoryUsage = memoryUsage;
        stats.peakMemoryUsage = peakMemoryUsage;
        stats.gpuLoad = gpuLoad;
        stats.averageCPUUsage = totalCPUUsage / std::max(1.0, cpuSamples);
        stats.averageMemoryUsage = totalMemoryUsage / std::max(1, cpuSamples);
        
        return stats;
    }
    
private:
    void monitorWorker() {
        while (monitorRunning) {
            updateResourceStats();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void updateResourceStats() {
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
        
        // GPU load would be obtained from GPU monitoring API
        gpuLoad = 0.0; // Placeholder
    }
    
    mutable std::mutex dataMutex;
    std::thread monitorThread;
    std::atomic<bool> isMonitoring;
    std::atomic<bool> monitorRunning;
    
    double cpuUsage;
    size_t memoryUsage;
    size_t peakMemoryUsage;
    double gpuLoad;
    
    double totalCPUUsage;
    size_t totalMemoryUsage;
    size_t cpuSamples;
};

// Real-time neural activity monitor
class RealTimeNeuralMonitor {
public:
    RealTimeNeuralMonitor() : neuronCount(0), activeNeuronCount(0), 
                             totalSpikes(0), currentFiringRate(0.0), 
                             maxFiringRate(0.0), minFiringRate(0.0),
                             isMonitoring(false), monitorThread(), monitorRunning(false) {}
    
    void startMonitoring() {
        isMonitoring = true;
        monitorRunning = true;
        monitorThread = std::thread(&RealTimeNeuralMonitor::monitorWorker, this);
    }
    
    void stopMonitoring() {
        isMonitoring = false;
        monitorRunning = false;
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }
    
    ~RealTimeNeuralMonitor() {
        stopMonitoring();
    }
    
    void setNeuronCount(size_t count) {
        std::lock_guard<std::mutex> lock(dataMutex);
        neuronCount = count;
    }
    
    void setActiveNeuronCount(size_t count) {
        std::lock_guard<std::mutex> lock(dataMutex);
        activeNeuronCount = count;
    }
    
    void incrementSpikeCount(size_t count) {
        std::lock_guard<std::mutex> lock(dataMutex);
        totalSpikes += count;
        if (count > 0) {
            currentFiringRate = static_cast<double>(totalSpikes) / 
                               static_cast<double>(neuronCount > 0 ? neuronCount : 1);
            if (currentFiringRate > maxFiringRate) maxFiringRate = currentFiringRate;
            if (currentFiringRate < minFiringRate) minFiringRate = currentFiringRate;
        }
    }
    
    void setFiringRate(double rate) {
        std::lock_guard<std::mutex> lock(dataMutex);
        currentFiringRate = rate;
        if (rate > maxFiringRate) maxFiringRate = rate;
        if (rate < minFiringRate) minFiringRate = rate;
    }
    
    size_t getNeuronCount() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return neuronCount;
    }
    
    size_t getActiveNeuronCount() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return activeNeuronCount;
    }
    
    size_t getTotalSpikes() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return totalSpikes;
    }
    
    double getCurrentFiringRate() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return currentFiringRate;
    }
    
    double getMaxFiringRate() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return maxFiringRate;
    }
    
    double getMinFiringRate() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return minFiringRate;
    }
    
    struct NeuralStats {
        size_t neuronCount;
        size_t activeNeuronCount;
        double activeNeuronRatio;
        size_t totalSpikes;
        double averageFiringRate;
        double maxFiringRate;
        double minFiringRate;
        double spikesPerSecond;
    };
    
    NeuralStats getStats() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        NeuralStats stats;
        stats.neuronCount = neuronCount;
        stats.activeNeuronCount = activeNeuronCount;
        stats.activeNeuronRatio = neuronCount > 0 ? static_cast<double>(activeNeuronCount) / neuronCount : 0.0;
        stats.totalSpikes = totalSpikes;
        stats.averageFiringRate = currentFiringRate;
        stats.maxFiringRate = maxFiringRate;
        stats.minFiringRate = minFiringRate;
        stats.spikesPerSecond = currentFiringRate; // Simplified
        
        return stats;
    }
    
    std::string getNeuralReport() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        NeuralStats stats = getStats();
        
        std::ostringstream oss;
        oss << "=== Real-time Neural Activity Report ===\n";
        oss << "Total Neurons: " << stats.neuronCount << "\n";
        oss << "Active Neurons: " << stats.activeNeuronCount << " ("
            << std::fixed << std::setprecision(2) << (stats.activeNeuronRatio * 100.0) << "%)\n";
        oss << "Total Spikes: " << stats.totalSpikes << "\n";
        oss << "Current Firing Rate: " << std::fixed << std::setprecision(2)
            << stats.averageFiringRate << " Hz\n";
        oss << "Max Firing Rate: " << std::fixed << std::setprecision(2)
            << stats.maxFiringRate << " Hz\n";
        oss << "Min Firing Rate: " << std::fixed << std::setprecision(2)
            << stats.minFiringRate << " Hz\n";
        
        return oss.str();
    }
    
private:
    void monitorWorker() {
        while (monitorRunning) {
            // Update neural statistics periodically
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    mutable std::mutex dataMutex;
    std::thread monitorThread;
    std::atomic<bool> isMonitoring;
    std::atomic<bool> monitorRunning;
    
    size_t neuronCount;
    size_t activeNeuronCount;
    size_t totalSpikes;
    double currentFiringRate;
    double maxFiringRate;
    double minFiringRate;
};

// Real-time simulation monitor
class RealTimeSimulationMonitor {
public:
    RealTimeSimulationMonitor() : currentStep(0), currentTime(0.0), 
                                 totalSteps(0), simulationSpeed(0.0),
                                 isMonitoring(false), monitorThread(), monitorRunning(false),
                                 fps(0.0), frameTime(0.0) {}
    
    void startMonitoring() {
        isMonitoring = true;
        monitorRunning = true;
        startTime = std::chrono::system_clock::now();
        monitorThread = std::thread(&RealTimeSimulationMonitor::monitorWorker, this);
    }
    
    void stopMonitoring() {
        isMonitoring = false;
        monitorRunning = false;
        if (monitorThread.joinable()) {
            monitorThread.join();
        }
    }
    
    ~RealTimeSimulationMonitor() {
        stopMonitoring();
    }
    
    void setStepInfo(uint64_t step, double time) {
        std::lock_guard<std::mutex> lock(dataMutex);
        currentStep = step;
        currentTime = time;
        
        if (!lastStepTime.empty()) {
            auto duration = std::chrono::duration<double>(time - lastStepTime);
            frameTime = duration.count();
            if (frameTime > 0.0) {
                fps = 1.0 / frameTime;
            }
        }
        
        lastStepTime = time;
    }
    
    void incrementStep() {
        std::lock_guard<std::mutex> lock(dataMutex);
        currentStep++;
        totalSteps++;
        
        if (!lastTime.empty()) {
            auto duration = std::chrono::duration<double>(currentTime - lastTime);
            if (duration.count() > 0.0) {
                simulationSpeed = 1.0 / duration.count();
            }
        }
        
        lastTime = currentTime;
    }
    
    void recordError(const std::string& error) {
        std::lock_guard<std::mutex> lock(dataMutex);
        errors.push_back(error);
        errorCount++;
        
        if (errors.size() > 1000) {
            errors.erase(errors.begin());
        }
    }
    
    uint64_t getCurrentStep() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return currentStep;
    }
    
    double getCurrentTime() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return currentTime;
    }
    
    uint64_t getTotalSteps() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return totalSteps;
    }
    
    double getSimulationSpeed() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return simulationSpeed;
    }
    
    double getFPS() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return fps;
    }
    
    double getFrameTime() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return frameTime;
    }
    
    size_t getErrorCount() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return errorCount;
    }
    
    struct SimulationStats {
        uint64_t currentStep;
        double currentTime;
        uint64_t totalSteps;
        double simulationSpeed;
        double fps;
        double frameTime;
        size_t errorCount;
        std::vector<std::string> recentErrors;
    };
    
    SimulationStats getStats() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        SimulationStats stats;
        stats.currentStep = currentStep;
        stats.currentTime = currentTime;
        stats.totalSteps = totalSteps;
        stats.simulationSpeed = simulationSpeed;
        stats.fps = fps;
        stats.frameTime = frameTime;
        stats.errorCount = errorCount;
        
        if (errors.size() > 100) {
            stats.recentErrors.assign(errors.end() - 100, errors.end());
        } else {
            stats.recentErrors = errors;
        }
        
        return stats;
    }
    
    std::string getSimulationReport() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        SimulationStats stats = getStats();
        
        std::ostringstream oss;
        oss << "=== Real-time Simulation Report ===\n";
        oss << "Current Step: " << stats.currentStep << " (Total: " << stats.totalSteps << ")\n";
        oss << "Current Time: " << std::fixed << std::setprecision(4) << stats.currentTime << " seconds\n";
        oss << "Simulation Speed: " << std::fixed << std::setprecision(2) << stats.simulationSpeed << "x\n";
        oss << "FPS: " << std::fixed << std::setprecision(2) << stats.fps << "\n";
        oss << "Frame Time: " << std::fixed << std::setprecision(4) << stats.frameTime * 1000.0 << " ms\n";
        oss << "Error Count: " << stats.errorCount << "\n";
        
        if (!stats.recentErrors.empty()) {
            oss << "\nRecent Errors:\n";
            for (const auto& error : stats.recentErrors) {
                oss << "  " << error << "\n";
            }
        }
        
        return oss.str();
    }
    
private:
    void monitorWorker() {
        while (monitorRunning) {
            // Update simulation statistics periodically
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    mutable std::mutex dataMutex;
    std::thread monitorThread;
    std::atomic<bool> isMonitoring;
    std::atomic<bool> monitorRunning;
    
    uint64_t currentStep;
    double currentTime;
    uint64_t totalSteps;
    double simulationSpeed;
    double fps;
    double frameTime;
    
    std::chrono::system_clock::time_point startTime;
    double lastStepTime;
    double lastTime;
    
    size_t errorCount;
    std::vector<std::string> errors;
};

// Comprehensive debugging system
class DebugSystem {
public:
    DebugSystem() : debugState(), eventQueue(), profiler(), resourceMonitor(), 
                   neuralMonitor(), simulationMonitor(), logger(nullptr) {
        debugState.startTime = std::chrono::system_clock::now();
        debugState.lastUpdateTime = debugState.startTime;
        
        // Start monitoring systems
        resourceMonitor.startMonitoring();
        neuralMonitor.startMonitoring();
        simulationMonitor.startMonitoring();
        eventQueue.start();
    }
    
    ~DebugSystem() {
        stopAllMonitoring();
    }
    
    void setLogger(std::shared_ptr<ConsoleDebugLogger> log) {
        logger = log;
    }
    
    void startRealTimeMonitoring() {
        debugState.isMonitoring = true;
        debugState.isRealTimeMonitoring = true;
        
        if (logger) {
            logger->startRealTimeMonitoring();
        }
    }
    
    void stopRealTimeMonitoring() {
        debugState.isMonitoring = false;
        debugState.isRealTimeMonitoring = false;
        
        if (logger) {
            logger->stopRealTimeMonitoring();
        }
    }
    
    void startProfiling() {
        debugState.isProfiling = true;
    }
    
    void stopProfiling() {
        debugState.isProfiling = false;
    }
    
    void startMemoryTracking() {
        debugState.isMemoryTracking = true;
    }
    
    void stopMemoryTracking() {
        debugState.isMemoryTracking = false;
    }
    
    void startNeuralTracking() {
        debugState.isNeuralTracking = true;
    }
    
    void stopNeuralTracking() {
        debugState.isNeuralTracking = false;
    }
    
    void startSimulationTracking() {
        debugState.isSimulationTracking = true;
    }
    
    void stopSimulationTracking() {
        debugState.isSimulationTracking = false;
    }
    
    void update() {
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
        
        // Log debug state periodically
        if (debugState.totalDebugEvents % 1000 == 0) {
            if (logger) {
                std::ostringstream oss;
                oss << "Debug State - Step " << simulationMonitor.getCurrentStep() 
                    << ", Time: " << std::fixed << std::setprecision(2) << simulationMonitor.getCurrentTime()
                    << ", FPS: " << std::fixed << std::setprecision(2) << debugState.currentFPS
                    << ", Memory: " << debugState.currentMemoryUsage / 1024 << " KB"
                    << ", Events: " << debugState.totalDebugEvents;
                
                logger->pushStatus(oss.str());
            }
        }
    }
    
    void recordEvent(const std::string& event) {
        if (logger) {
            logger->pushStatus(event);
        }
    }
    
    void recordPerformance(double value) {
        if (logger) {
            logger->pushPerformance(value);
        }
    }
    
    void recordMemory(size_t value) {
        if (logger) {
            logger->pushMemory(value);
        }
    }
    
    struct DebugReport {
        std::string debugState;
        std::string performanceReport;
        std::string memoryReport;
        std::string neuralActivityReport;
        std::string simulationReport;
        std::string systemResourcesReport;
        
        std::string generateFullReport() const {
            std::ostringstream oss;
            oss << "=== Comprehensive Debug Report ===\n\n";
            oss << debugState << "\n";
            oss << performanceReport << "\n";
            oss << memoryReport << "\n";
            oss << neuralActivityReport << "\n";
            oss << simulationReport << "\n";
            oss << systemResourcesReport << "\n";
            return oss.str();
        }
    };
    
    DebugReport generateDebugReport() {
        DebugReport report;
        
        // Generate debug state report
        std::ostringstream debugStateOss;
        debugStateOss << "=== Debug System State ===\n";
        debugStateOss << "Monitoring: " << (debugState.isMonitoring ? "ON" : "OFF") << "\n";
        debugStateOss << "Profiling: " << (debugState.isProfiling ? "ON" : "OFF") << "\n";
        debugStateOss << "Memory Tracking: " << (debugState.isMemoryTracking ? "ON" : "OFF") << "\n";
        debugStateOss << "Neural Tracking: " << (debugState.isNeuralTracking ? "ON" : "OFF") << "\n";
        debugStateOss << "Simulation Tracking: " << (debugState.isSimulationTracking ? "ON" : "OFF") << "\n";
        debugStateOss << "Total Debug Events: " << debugState.totalDebugEvents << "\n";
        debugStateOss << "Current FPS: " << std::fixed << std::setprecision(2) << debugState.currentFPS << "\n";
        debugStateOss << "Current Memory Usage: " << debugState.currentMemoryUsage << " bytes\n";
        debugStateOss << "Peak Memory Usage: " << debugState.peakMemoryUsage << " bytes\n";
        debugStateOss << "Current CPU Load: " << std::fixed << std::setprecision(2) << debugState.currentCPULoad << "%\n";
        debugStateOss << "Current GPU Load: " << std::fixed << std::setprecision(2) << debugState.currentGPULoad << "%\n";
        
        report.debugState = debugStateOss.str();
        
        // Generate other reports using the respective monitors
        // (In a real implementation, these would be obtained from the respective monitor objects)
        
        return report;
    }
    
    void stopAllMonitoring() {
        stopRealTimeMonitoring();
        resourceMonitor.stopMonitoring();
        neuralMonitor.stopMonitoring();
        simulationMonitor.stopMonitoring();
        eventQueue.stop();
    }
    
    RealTimeProfiler& getProfiler() {
        return profiler;
    }
    
    SystemResourceMonitor& getResourceMonitor() {
        return resourceMonitor;
    }
    
    RealTimeNeuralMonitor& getNeuralMonitor() {
        return neuralMonitor;
    }
    
    RealTimeSimulationMonitor& getSimulationMonitor() {
        return simulationMonitor;
    }
    
    DebugEventQueue& getEventQueue() {
        return eventQueue;
    }
    
private:
    DebugState debugState;
    DebugEventQueue eventQueue;
    RealTimeProfiler profiler;
    SystemResourceMonitor resourceMonitor;
    RealTimeNeuralMonitor neuralMonitor;
    RealTimeSimulationMonitor simulationMonitor;
    std::shared_ptr<ConsoleDebugLogger> logger;
};

} // namespace nlm
