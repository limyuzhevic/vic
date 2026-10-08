// Fixed AdvancedBrain.cpp with proper includes and namespace
// This file now uses the correct structure definitions from AdvancedBrain.hpp

#include "AdvancedBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Implementation helper function for logging
namespace {
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
}

struct AdvancedBrain::Impl {
    std::chrono::steady_clock::time_point simulationStartTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    std::atomic<uint64_t> memoryAllocations;
    std::atomic<uint64_t> memoryDeallocations;
    std::atomic<size_t> currentMemoryUsage;
    size_t peakMemoryUsage;
    
    struct PerformanceSample {
        SimulationStep step;
        double simulationTime;
        double memoryUsage;
        float firingRate;
        float cpuUsage;
        double elapsedSeconds;
    };
    
    std::vector<PerformanceSample> performanceSamples;
    size_t maxSamples;
    
    Impl() 
        : memoryAllocations(0), memoryDeallocations(0), currentMemoryUsage(0), 
          peakMemoryUsage(0), maxSamples(100), 
          simulationStartTime(std::chrono::steady_clock::now()),
          lastUpdateTime(std::chrono::steady_clock::now()) {}
};

AdvancedBrain::AdvancedBrain(std::shared_ptr<Config> config,
                            bool enableMonitoring,
                            bool enableThreading,
                            bool enableProfiling)
    : monitoringEnabled(enableMonitoring)
    , threadingEnabled(enableThreading)
    , profilingEnabled(enableProfiling)
    , currentStep(0)
    , simulationSpeed(0.0)
    , startTime(std::chrono::steady_clock::now())
    , totalMemoryAllocations(0)
    , totalMemoryDeallocations(0)
    , maxMemoryUsage(0)
    , nextCallbackId(0)
    , isPausedState(false)
{
    pImpl = std::make_unique<Impl>();
    pBrain = std::make_shared<Brain>(config);
    
    setupDefaultAdvancedConfig();
    
    NLM_LOG_INFO("AdvancedBrain initialized with monitoring=" << monitoringEnabled 
                 << ", threading=" << threadingEnabled 
                 << ", profiling=" << profilingEnabled);
}

AdvancedBrain::~AdvancedBrain() = default;

AdvancedBrain::AdvancedBrain(AdvancedBrain&& other) noexcept
    : pImpl(std::move(other.pImpl))
    , pBrain(std::move(other.pBrain))
    , advancedConfig(std::move(other.advancedConfig))
    , monitoringEnabled(other.monitoringEnabled)
    , threadingEnabled(other.threadingEnabled)
    , profilingEnabled(other.profilingEnabled)
    , currentStep(other.currentStep)
    , simulationSpeed(other.simulationSpeed)
    , startTime(other.startTime)
    , totalMemoryAllocations(other.totalMemoryAllocations)
    , totalMemoryDeallocations(other.totalMemoryDeallocations)
    , maxMemoryUsage(other.maxMemoryUsage)
    , eventCallbacks(std::move(other.eventCallbacks))
    , nextCallbackId(other.nextCallbackId)
    , isPausedState(other.isPausedState)
    , pauseReason(std::move(other.pauseReason)) {
    
    // Reset other to avoid double destruction
    other.currentStep = 0;
    other.simulationSpeed = 0.0;
    other.isPausedState = false;
    other.nextCallbackId = 0;
}

AdvancedBrain& AdvancedBrain::operator=(AdvancedBrain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        pBrain = std::move(other.pBrain);
        advancedConfig = std::move(other.advancedConfig);
        monitoringEnabled = other.monitoringEnabled;
        threadingEnabled = other.threadingEnabled;
        profilingEnabled = other.profilingEnabled;
        currentStep = other.currentStep;
        simulationSpeed = other.simulationSpeed;
        startTime = other.startTime;
        totalMemoryAllocations = other.totalMemoryAllocations;
        totalMemoryDeallocations = other.totalMemoryDeallocations;
        maxMemoryUsage = other.maxMemoryUsage;
        eventCallbacks = std::move(other.eventCallbacks);
        nextCallbackId = other.nextCallbackId;
        isPausedState = other.isPausedState;
        pauseReason = std::move(other.pauseReason);
        
        other.currentStep = 0;
        other.simulationSpeed = 0.0;
        other.isPausedState = false;
        other.nextCallbackId = 0;
    }
    return *this;
}

bool AdvancedBrain::initialize(bool enableMemoryTracking,
                               bool enableEventLogging,
                               bool enablePerformanceCounters) {
    NLM_LOG_INFO("Initializing AdvancedBrain with memoryTracking=" << enableMemoryTracking 
                 << ", eventLogging=" << enableEventLogging 
                 << ", performanceCounters=" << enablePerformanceCounters);
    
    if (!pBrain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize base Brain");
        return false;
    }
    
    // Update advanced configuration
    advancedConfig.enableMemoryTracking = enableMemoryTracking;
    advancedConfig.enableEventLogging = enableEventLogging;
    advancedConfig.enablePerformanceCounters = enablePerformanceCounters;
    
    // Reset performance counters
    resetPerformanceCounters();
    
    NLM_LOG_INFO("AdvancedBrain initialization complete");
    return true;
}

SimulationResult AdvancedBrain::runWithMonitoring(SimulationStep startStep,
                                                 SimulationStep numSteps,
                                                 std::function<void(float)> callback,
                                                 SimulationStep monitorInterval) {
    if (isPausedState) {
        NLM_LOG_WARNING("Cannot run simulation while paused");
        return SimulationResult{};
    }
    
    NLM_LOG_INFO("Starting simulation with monitoring: startStep=" << startStep 
                 << ", numSteps=" << numSteps 
                 << ", monitorInterval=" << monitorInterval);
    
    // Update current step
    currentStep = startStep;
    pImpl->simulationStartTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->simulationStartTime;
    
    // Calculate target end step
    SimulationStep endStep = startStep + numSteps;
    
    // Run simulation with monitoring
    for (SimulationStep step = startStep; step < endStep && !g_shutdown; ++step) {
        if (isPausedState) {
            NLM_LOG_INFO("Simulation paused at step " << step);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Record pre-step metrics
        auto stepStartTime = std::chrono::steady_clock::now();
        
        // Run brain step
        pBrain->step(step);
        
        // Calculate elapsed time and update performance metrics
        auto stepEndTime = std::chrono::steady_clock::now();
        double stepTime = std::chrono::duration<double>(stepEndTime - stepStartTime).count();
        
        // Update performance stats
        updatePerformanceStats();
        
        // Collect data sample
        if (advancedConfig.enablePerformanceCounters && monitoringEnabled) {
            PerformanceSample sample;
            sample.step = step;
            sample.simulationTime = stepTime;
            sample.memoryUsage = static_cast<double>(getMemoryUsage());
            sample.firingRate = pBrain->getAverageFiringRate();
            sample.cpuUsage = calculateCPUUsage();
            sample.elapsedSeconds = std::chrono::duration<double>(stepEndTime - pImpl->simulationStartTime).count();
            
            std::lock_guard<std::mutex> lock(pImpl->statsMutex);
            pImpl->performanceSamples.push_back(sample);
            
            // Keep only last N samples
            if (pImpl->performanceSamples.size() > pImpl->maxSamples) {
                pImpl->performanceSamples.erase(pImpl->performanceSamples.begin());
            }
        }
        
        // Update simulation speed
        updateSimulationSpeed(stepTime);
        
        // Invoke progress callback
        if (callback) {
            float progress = static_cast<float>(step - startStep) / static_cast<float>(numSteps);
            callback(progress);
        }
        
        // Log progress at intervals
        if (advancedConfig.enableEventLogging && monitorInterval > 0 && step % monitorInterval == 0) {
            NLM_LOG_INFO("Step " << step << "/" << numSteps 
                         << " | Firing rate: " << pBrain->getAverageFiringRate()
                         << " | Memory usage: " << getMemoryUsage()
                         << " bytes | Speed: " << getSimulationSpeed() << " steps/sec");
        }
    }
    
    NLM_LOG_INFO("Simulation completed: " << (endStep - startStep) << " steps executed");
    
    return getSimulationResult();
}

PerformanceStats AdvancedBrain::getPerformanceStats() const {
    PerformanceStats stats;
    
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    stats.totalSteps = currentStep;
    stats.totalTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
    stats.averageStepTime = stats.totalTime / (currentStep > 0 ? currentStep : 1);
    stats.currentMemoryUsage = pImpl->currentMemoryUsage;
    stats.peakMemoryUsage = pImpl->peakMemoryUsage;
    stats.memoryAllocations = totalMemoryAllocations;
    stats.memoryDeallocations = totalMemoryDeallocations;
    stats.currentFiringRate = pBrain->getAverageFiringRate();
    stats.totalSpikes = pBrain->getTotalSpikeCount();
    stats.currentSimulationSpeed = simulationSpeed;
    stats.isPaused = isPausedState;
    
    return stats;
}

uint64_t AdvancedBrain::getMemoryUsage() const {
    return pImpl->currentMemoryUsage;
}

SimulationStats AdvancedBrain::getCurrentStats() const {
    SimulationStats stats;
    
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    stats.currentStep = currentStep;
    stats.currentTime = currentStep * pImpl->timestep;
    stats.currentFiringRate = pBrain->getAverageFiringRate();
    stats.currentMemoryUsage = pImpl->currentMemoryUsage;
    stats.totalSynapses = pBrain->getTotalSynapseCount();
    stats.totalNeurons = pBrain->getTotalNeuronCount();
    stats.activeNeurons = pBrain->getActiveNeuronCount();
    
    return stats;
}

void AdvancedBrain::configureFeature(const std::string& feature, const ConfigValue& value) {
    advancedConfig.customFeatures[feature] = value;
    NLM_LOG_INFO("Configured advanced feature: " << feature << " = " << value);
}

std::optional<ConfigValue> AdvancedBrain::getConfiguration(const std::string& feature) const {
    auto it = advancedConfig.customFeatures.find(feature);
    if (it != advancedConfig.customFeatures.end()) {
        return it->second;
    }
    return std::nullopt;
}

void AdvancedBrain::enableFeature(const std::string& feature, bool enabled) {
    advancedConfig.enabledFeatures[feature] = enabled;
    NLM_LOG_INFO("Advanced feature " << feature << " " << (enabled ? "enabled" : "disabled"));
}

bool AdvancedBrain::isFeatureEnabled(const std::string& feature) const {
    auto it = advancedConfig.enabledFeatures.find(feature);
    return it != advancedConfig.enabledFeatures.end() ? it->second : false;
}

bool AdvancedBrain::exportState(const std::string& filepath, bool includeMetadata) {
    try {
        NLM_LOG_INFO("Exporting brain state to: " << filepath);
        
        // Ensure directory exists
        std::filesystem::path dir(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Export brain state
        bool saveSuccess = pBrain->save(filepath + ".brain");
        
        if (saveSuccess && includeMetadata) {
            // Add metadata
            nlohmann::json jsonData;
            jsonData["version"] = "advanced_1.0";
            jsonData["timestamp"] = getCurrentTimestamp();
            jsonData["memoryUsage"] = getMemoryUsage();
            jsonData["totalSteps"] = currentStep;
            jsonData["enabledFeatures"] = advancedConfig.enabledFeatures;
            
            std::ofstream metadataFile(filepath + ".metadata.json");
            if (metadataFile.is_open()) {
                metadataFile << std::setw(2) << jsonData;
            }
        }
        
        NLM_LOG_INFO("Brain state export completed");
        return saveSuccess;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain state export: " << e.what());
        return false;
    }
}

bool AdvancedBrain::importState(const std::string& filepath, bool validateMetadata) {
    try {
        NLM_LOG_INFO("Importing brain state from: " << filepath);
        
        // Check if files exist
        std::string brainFile = filepath + ".brain";
        std::string metadataFile = filepath + ".metadata.json";
        
        if (!std::filesystem::exists(brainFile)) {
            NLM_LOG_ERROR("Brain file not found: " << brainFile);
            return false;
        }
        
        // Load brain state
        bool loadSuccess = pBrain->load(brainFile);
        if (!loadSuccess) {
            NLM_LOG_ERROR("Failed to load brain state");
            return false;
        }
        
        if (validateMetadata && std::filesystem::exists(metadataFile)) {
            // Validate metadata
            std::ifstream metaFile(metadataFile);
            nlohmann::json metadata;
            metaFile >> metadata;
            
            // Validate metadata consistency
            if (metadata.contains("memoryUsage")) {
                uint64_t storedMemory = metadata["memoryUsage"].get<uint64_t>();
                if (storedMemory != getMemoryUsage()) {
                    NLM_LOG_WARNING("Memory usage mismatch in metadata: " 
                                   << storedMemory << " vs " << getMemoryUsage());
                }
            }
        }
        
        NLM_LOG_INFO("Brain state import completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain state import: " << e.what());
        return false;
    }
}

uint32_t AdvancedBrain::addEventCallback(BrainEvent eventType,
                                         std::function<void(const BrainEventData&)> callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    
    uint32_t callbackId = nextCallbackId++;
    eventCallbacks.push_back(callback);
    
    NLM_LOG_INFO("Added event callback with ID: " << callbackId << " for event type: " 
                 << static_cast<int>(eventType));
    
    return callbackId;
}

double AdvancedBrain::getUptime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return static_cast<double>(duration.count());
}

double AdvancedBrain::getSimulationSpeed() const {
    return simulationSpeed;
}

void AdvancedBrain::pause(const std::string& reason) {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    if (!isPausedState) {
        isPausedState = true;
        pauseReason = reason;
        NLM_LOG_INFO("Simulation paused: " << reason);
    } else {
        NLM_LOG_INFO("Simulation already paused");
    }
}

void AdvancedBrain::resume() {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    if (isPausedState) {
        isPausedState = false;
        pauseReason.clear();
        NLM_LOG_INFO("Simulation resumed");
    }
}

bool AdvancedBrain::isPaused() const {
    return isPausedState;
}

std::string AdvancedBrain::getPauseReason() const {
    return pauseReason;
}

bool AdvancedBrain::exportPerformanceData(const std::string& filepath,
                                           const std::string& format) {
    try {
        NLM_LOG_INFO("Exporting performance data to: " << filepath 
                     << " in format: " << format);
        
        if (format == "csv") {
            std::ofstream csvFile(filepath);
            if (!csvFile.is_open()) {
                NLM_LOG_ERROR("Failed to open CSV file: " << filepath);
                return false;
            }
            
            csvFile << "Step,Time,MemoryUsage,FiringRate,CPULoad,ElapsedSeconds" << std::endl;
            
            std::lock_guard<std::mutex> lock(pImpl->statsMutex);
            for (const auto& sample : pImpl->performanceSamples) {
                csvFile << sample.step << ","
                        << sample.simulationTime << ","
                        << sample.memoryUsage << ","
                        << sample.firingRate << ","
                        << sample.cpuUsage << ","
                        << sample.elapsedSeconds << std::endl;
            }
        } else if (format == "json") {
            nlohmann::json jsonData;
            
            std::lock_guard<std::mutex> lock(pImpl->statsMutex);
            jsonData["performanceSamples"] = pImpl->performanceSamples;
            jsonData["totalSamples"] = pImpl->performanceSamples.size();
            jsonData["peakMemoryUsage"] = pImpl->peakMemoryUsage;
            jsonData["totalMemoryAllocations"] = totalMemoryAllocations;
            jsonData["totalMemoryDeallocations"] = totalMemoryDeallocations;
            
            std::ofstream jsonFile(filepath);
            if (jsonFile.is_open()) {
                jsonFile << std::setw(2) << jsonData;
            }
        } else {
            NLM_LOG_ERROR("Unsupported export format: " << format);
            return false;
        }
        
        NLM_LOG_INFO("Performance data export completed");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during performance data export: " << e.what());
        return false;
    }
}

bool AdvancedBrain::loadAdvancedConfig(const std::string& filepath,
                                       ConfigSource source) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        NLM_LOG_ERROR("Failed to load advanced configuration from: " << filepath);
        return false;
    }
    
    // Apply advanced configuration
    if (auto monitoringEnabled = config->get<bool>("advanced.monitoring_enabled")) {
        this->monitoringEnabled = *monitoringEnabled;
    }
    
    if (auto threadingEnabled = config->get<bool>("advanced.threading_enabled")) {
        this->threadingEnabled = *threadingEnabled;
    }
    
    if (auto profilingEnabled = config->get<bool>("advanced.profiling_enabled")) {
        this->profilingEnabled = *profilingEnabled;
    }
    
    // Load custom features (simplified)
    if (config->has("advanced.custom_features")) {
        if (auto customFeatures = config->get<std::string>("advanced.custom_features")) {
            // In a full implementation, parse JSON from customFeatures
            NLM_LOG_INFO("Custom features configuration loaded (parsing not implemented)");
        }
    }
    
    NLM_LOG_INFO("Advanced configuration loaded from: " << filepath);
    return true;
}

bool AdvancedBrain::isThreadSafe() const {
    return threadingEnabled;
}

ProfilingInfo AdvancedBrain::getProfilingInfo(size_t maxSamples) const {
    ProfilingInfo info;
    
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    info.totalSamples = pImpl->performanceSamples.size();
    info.maxSamples = maxSamples;
    info.currentMemoryUsage = pImpl->currentMemoryUsage;
    info.peakMemoryUsage = pImpl->peakMemoryUsage;
    info.totalMemoryAllocations = totalMemoryAllocations;
    info.totalMemoryDeallocations = totalMemoryDeallocations;
    
    if (!pImpl->performanceSamples.empty()) {
        double totalTime = 0.0;
        double totalMemory = 0.0;
        double totalFiringRate = 0.0;
        double totalCPULoad = 0.0;
        
        for (const auto& sample : pImpl->performanceSamples) {
            totalTime += sample.simulationTime;
            totalMemory += sample.memoryUsage;
            totalFiringRate += sample.firingRate;
            totalCPULoad += sample.cpuUsage;
        }
        
        info.averageStepTime = totalTime / pImpl->performanceSamples.size();
        info.averageMemoryUsage = totalMemory / pImpl->performanceSamples.size();
        info.averageFiringRate = totalFiringRate / pImpl->performanceSamples.size();
        info.averageCPULoad = totalCPULoad / pImpl->performanceSamples.size();
        
        // Calculate variance
        double timeVariance = 0.0, memoryVariance = 0.0;
        for (const auto& sample : pImpl->performanceSamples) {
            timeVariance += std::pow(sample.simulationTime - info.averageStepTime, 2);
            memoryVariance += std::pow(sample.memoryUsage - info.averageMemoryUsage, 2);
        }
        
        info.stepTimeVariance = timeVariance / pImpl->performanceSamples.size();
        info.memoryUsageVariance = memoryVariance / pImpl->performanceSamples.size();
        
        // Calculate trends
        if (pImpl->performanceSamples.size() >= 5) {
            for (size_t i = 0; i < pImpl->performanceSamples.size() - 4; ++i) {
                size_t endIdx = std::min(i + 5, pImpl->performanceSamples.size());
                double trend = (pImpl->performanceSamples[endIdx - 1].simulationTime - 
                               pImpl->performanceSamples[i].simulationTime) / 
                              static_cast<double>(endIdx - i - 1);
                info.stepTimeTrend.push_back(trend);
                
                trend = (pImpl->performanceSamples[endIdx - 1].memoryUsage - 
                        pImpl->performanceSamples[i].memoryUsage) / 
                       static_cast<double>(endIdx - i - 1);
                info.memoryTrend.push_back(trend);
            }
        }
    }
    
    return info;
}

void AdvancedBrain::resetPerformanceCounters() {
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    pImpl->performanceSamples.clear();
    pImpl->currentMemoryUsage = 0;
    pImpl->peakMemoryUsage = 0;
    totalMemoryAllocations = 0;
    totalMemoryDeallocations = 0;
    
    NLM_LOG_INFO("Performance counters reset");
}

void AdvancedBrain::updatePerformanceStats() {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - pImpl->lastUpdateTime).count();
    
    if (elapsed >= 1) { // Update every second
        double speed = static_cast<double>(currentStep) / elapsed;
        simulationSpeed = speed;
        pImpl->lastUpdateTime = currentTime;
        
        // Collect memory usage (simplified)
        collectMemoryUsage();
    }
}

void AdvancedBrain::collectMemoryUsage() {
    // Simplified memory usage calculation
    // In a real implementation, you would use platform-specific APIs:
    // - Windows: GetProcessMemoryInfo()
    // - Linux: /proc/self/statm or /proc/self/status  
    // - macOS: mach_task_self_()
    // - Cross-platform: use memory profiling libraries
    
    static size_t testMemory = 1024; // Base memory usage
    size_t newMemory = testMemory + (currentStep * 10); // Simulated growth
    
    if (newMemory > pImpl->currentMemoryUsage) {
        pImpl->currentMemoryUsage = newMemory;
        if (newMemory > pImpl->peakMemoryUsage) {
            pImpl->peakMemoryUsage = newMemory;
        }
    }
    
    totalMemoryAllocations++;
}

void AdvancedBrain::invokeEventCallbacks(const BrainEvent& event) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    
    BrainEventData eventData;
    eventData.event = event;
    eventData.timestamp = std::chrono::system_clock::now();
    eventData.brain = this->pBrain.get();
    
    for (auto& callback : eventCallbacks) {
        try {
            callback(eventData);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in event callback: " << e.what());
        }
    }
}

void AdvancedBrain::setupDefaultAdvancedConfig() {
    // Set up default advanced configuration options
    advancedConfig.enableMemoryTracking = monitoringEnabled;
    advancedConfig.enableEventLogging = true;
    advancedConfig.enablePerformanceCounters = monitoringEnabled;
    
    // Set up default enabled features
    advancedConfig.enabledFeatures["advanced_monitoring"] = monitoringEnabled;
    advancedConfig.enabledFeatures["advanced_threading"] = threadingEnabled;
    advancedConfig.enabledFeatures["advanced_profiling"] = profilingEnabled;
    advancedConfig.enabledFeatures["event_callbacks"] = true;
    advancedConfig.enabledFeatures["performance_export"] = monitoringEnabled;
    
    // Set up default timestep
    pImpl->timestep = 0.001;
}

double AdvancedBrain::calculateCPUUsage() {
    // Simplified CPU usage calculation
    // In a real implementation, you would:
    // 1. Sample CPU time at intervals
    // 2. Calculate CPU usage as (CPU time / wall time) * 100%
    // 3. Use platform-specific APIs (GetProcessTimes() on Windows, etc.)
    
    static double lastCPUUsage = 0.0;
    static auto lastTime = std::chrono::steady_clock::now();
    
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();
    
    if (elapsed >= 1) {
        // Simulate CPU usage based on simulation activity
        double cpuUsage = std::min(100.0, (simulationSpeed * 1000.0) / 10.0); // Simplified calculation
        lastCPUUsage = cpuUsage;
        lastTime = currentTime;
    }
    
    return lastCPUUsage;
}

void AdvancedBrain::updateSimulationSpeed(double stepTime) {
    // Update simulation speed (steps per second)
    if (stepTime > 0.0) {
        simulationSpeed = 1.0 / stepTime;
    }
}

SimulationResult AdvancedBrain::getSimulationResult() const {
    SimulationResult result;
    
    std::lock_guard<std::mutex> lock(pImpl->statsMutex);
    
    result.totalSteps = currentStep;
    result.totalTime = getUptime();
    result.averageFiringRate = pBrain->getAverageFiringRate();
    result.totalSpikes = pBrain->getTotalSpikeCount();
    result.currentMemoryUsage = pImpl->currentMemoryUsage;
    result.peakMemoryUsage = pImpl->peakMemoryUsage;
    result.performanceStats = getPerformanceStats();
    result.isSuccessful = !isPausedState() && !g_shutdown;
    
    return result;
}

} // namespace nlm
