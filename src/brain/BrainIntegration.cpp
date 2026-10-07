#include "BrainIntegration.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include "../core/Logger/EnhancedLogger.hpp"
#include "../core/Debugging/RealTimeDebugSystem.hpp"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <chrono>

namespace nlm {

struct BrainIntegration::Impl {
    std::shared_ptr<Brain> brain;
    IntegrationConfig config;
    
    std::unique_ptr<VisualizationInterface> visualization;
    std::unique_ptr<DebugSystem> debugSystem;
    std::unique_ptr<EnhancedLogger> logger;
    
    std::mutex mutex;
    bool active;
    
    std::chrono::high_resolution_clock::time_point performanceStart;
    std::string currentPerformanceOperation;
    
    std::vector<VisualizationCallback> visualizationCallbacks;
    std::vector<DebugCallback> debugCallbacks;
    std::vector<PerformanceCallback> performanceCallbacks;
    
    Impl() : active(false) {}
};

BrainIntegration::BrainIntegration() : pImpl(std::make_unique<Impl>()) {}

BrainIntegration::~BrainIntegration() = default;

bool BrainIntegration::initialize(std::shared_ptr<Brain> brain, const IntegrationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!brain) {
        std::cerr << "Invalid brain pointer" << std::endl;
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->config = config;
    pImpl->active = true;
    
    // Initialize visualization system
    if (config.enableVisualization) {
        pImpl->visualization = std::make_unique<VisualizationInterface>();
        if (!pImpl->visualization->initialize(false, true, 
                                              config.visualizationWidth, 
                                              config.visualizationHeight)) {
            std::cerr << "Failed to initialize visualization" << std::endl;
            return false;
        }
        pImpl->visualization->setUpdateRate(config.visualizationUpdateRate);
        NLM_LOG_INFO("Visualization system initialized");
    }
    
    // Initialize debug system
    if (config.enableRealTimeDebugging) {
        pImpl->debugSystem = std::make_unique<DebugSystem>();
        
        // Create and set up enhanced logger
        pImpl->logger = std::make_unique<EnhancedLogger>();
        
        // Configure logging based on debug level
        LogLevel level;
        switch (config.debugLevel) {
            case 0: level = LogLevel::Debug; break;
            case 1: level = LogLevel::Info; break;
            case 2: level = LogLevel::Warning; break;
            case 3: level = LogLevel::Error; break;
            default: level = LogLevel::Info; break;
        }
        
        pImpl->logger->setLevel(level);
        
        // Start real-time monitoring if enabled
        if (config.enableRealTimeDebugging) {
            pImpl->debugSystem->startRealTimeMonitoring();
        }
        
        NLM_LOG_INFO("Debug system initialized");
    }
    
    // Enable specific tracking based on configuration
    if (config.enablePerformanceProfiling) {
        pImpl->debugSystem->startProfiling();
    }
    
    if (config.enableMemoryTracking) {
        pImpl->debugSystem->startMemoryTracking();
    }
    
    if (config.enableNeuralActivityTracking) {
        pImpl->debugSystem->startNeuralActivityTracking();
    }
    
    if (config.enableSimulationStateTracking) {
        pImpl->debugSystem->startSimulationStateTracking();
    }
    
    NLM_LOG_INFO("Brain integration initialized successfully");
    return true;
}

void BrainIntegration::update() {
    if (!pImpl->active || !pImpl->brain) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Record simulation state
    if (pImpl->config.enableSimulationStateTracking && pImpl->debugSystem) {
        SimulationState state;
        state.step = pImpl->brain->getTotalSpikeCount();
        state.time = pImpl->brain->getTotalSpikeCount() * pImpl->brain->getAverageFiringRate();
        state.neuronCount = pImpl->brain->getTotalNeuronCount();
        state.activeNeuronCount = pImpl->brain->getTotalSpikeCount() > 0 ? 
                                 pImpl->brain->getTotalNeuronCount() * 0.1 : 0; // Estimate
        state.totalSpikeCount = pImpl->brain->getTotalSpikeCount();
        state.averageFiringRate = pImpl->brain->getAverageFiringRate();
        state.timestamp = std::chrono::system_clock::now();
        state.stateDescription = "Step " + std::to_string(state.step) + 
                               ", " + std::to_string(state.activeNeuronCount) + 
                               " active neurons";
        
        pImpl->debugSystem->getSimulationMonitor().recordState(state);
    }
    
    // Record neural activity
    if (pImpl->config.enableNeuralActivityTracking && pImpl->debugSystem) {
        NeuralActivity activity;
        activity.neuronId = 0; // Would iterate over all neurons
        activity.firingRate = pImpl->brain->getAverageFiringRate();
        activity.membranePotential = 0.0f; // Would get from brain
        activity.timestamp = std::chrono::system_clock::now();
        activity.spikeCount = pImpl->brain->getTotalSpikeCount();
        activity.averageWeight = 0.0f; // Would calculate from brain
        
        pImpl->debugSystem->getNeuralMonitor().recordActivity(activity);
    }
    
    // Record performance
    if (pImpl->config.enablePerformanceProfiling) {
        pImpl->debugSystem->getProfiler().endFrame();
    }
    
    // Update visualization
    if (pImpl->config.enableVisualization && pImpl->visualization && pImpl->brain) {
        pImpl->visualization->update();
        
        // Visualize network if visualization is active
        if (pImpl->visualization->isActive() && pImpl->debugSystem) {
            pImpl->visualization->visualizeNetwork(*pImpl->brain);
            
            // Visualize activity
            pImpl->visualization->visualizeActivity(*pImpl->brain);
            
            // Visualize spikes
            // Get spikes from brain's spike system
            auto* spikeSystem = pImpl->brain->getSpikeSystem();
            if (spikeSystem) {
                std::vector<SpikeEvent> spikes;
                // Would collect spikes from spike system
                pImpl->visualization->visualizeSpikes(spikes);
            }
            
            // Visualize weights
            pImpl->visualization->visualizeWeights(*pImpl->brain);
        }
    }
    
    // Update debug system
    if (pImpl->debugSystem) {
        pImpl->debugSystem->update();
    }
    
    // Trigger callbacks
    if (pImpl->config.enableVisualization && pImpl->visualization) {
        for (const auto& callback : pImpl->visualizationCallbacks) {
            callback(*pImpl->visualization);
        }
    }
    
    if (pImpl->config.enableRealTimeDebugging && pImpl->debugSystem) {
        for (const auto& callback : pImpl->debugCallbacks) {
            callback(*pImpl->debugSystem);
        }
    }
}

VisualizationInterface& BrainIntegration::getVisualization() const {
    return *pImpl->visualization;
}

DebugSystem& BrainIntegration::getDebugSystem() const {
    return *pImpl->debugSystem;
}

EnhancedLogger& BrainIntegration::getLogger() const {
    return *pImpl->logger;
}

bool BrainIntegration::isActive() const {
    return pImpl->active;
}

void BrainIntegration::setVisualizationEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.enableVisualization = enabled;
    
    if (enabled && !pImpl->visualization) {
        // Initialize visualization
        pImpl->visualization = std::make_unique<VisualizationInterface>();
        pImpl->visualization->initialize(false, true, 
                                        pImpl->config.visualizationWidth, 
                                        pImpl->config.visualizationHeight);
    } else if (!enabled && pImpl->visualization) {
        pImpl->visualization->close();
        pImpl->visualization.reset();
    }
}

void BrainIntegration::setRealTimeDebuggingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.enableRealTimeDebugging = enabled;
    
    if (enabled && !pImpl->debugSystem) {
        // Initialize debug system
        pImpl->debugSystem = std::make_unique<DebugSystem>();
        pImpl->logger = std::make_unique<EnhancedLogger>();
        
        if (pImpl->config.enableRealTimeDebugging) {
            pImpl->debugSystem->startRealTimeMonitoring();
        }
    } else if (!enabled && pImpl->debugSystem) {
        pImpl->debugSystem->stopAllMonitoring();
        pImpl->debugSystem.reset();
        pImpl->logger.reset();
    }
}

void BrainIntegration::setPerformanceProfilingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.enablePerformanceProfiling = enabled;
    
    if (pImpl->debugSystem) {
        if (enabled) {
            pImpl->debugSystem->startProfiling();
        } else {
            pImpl->debugSystem->stopProfiling();
        }
    }
}

void BrainIntegration::setMemoryTrackingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.enableMemoryTracking = enabled;
    
    if (pImpl->debugSystem) {
        if (enabled) {
            pImpl->debugSystem->startMemoryTracking();
        } else {
            pImpl->debugSystem->stopMemoryTracking();
        }
    }
}

void BrainIntegration::setNeuralActivityTrackingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.enableNeuralActivityTracking = enabled;
    
    if (pImpl->debugSystem) {
        if (enabled) {
            pImpl->debugSystem->startNeuralActivityTracking();
        } else {
            pImpl->debugSystem->stopNeuralActivityTracking();
        }
    }
}

void BrainIntegration::setSimulationStateTrackingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.enableSimulationStateTracking = enabled;
    
    if (pImpl->debugSystem) {
        if (enabled) {
            pImpl->debugSystem->startSimulationStateTracking();
        } else {
            pImpl->debugSystem->stopSimulationStateTracking();
        }
    }
}

void BrainIntegration::startMonitoring() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->startRealTimeMonitoring();
    }
}

void BrainIntegration::stopMonitoring() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->stopRealTimeMonitoring();
    }
}

void BrainIntegration::pauseMonitoring() {
    // Implementation would pause monitoring
}

void BrainIntegration::resumeMonitoring() {
    // Implementation would resume monitoring
}

void BrainIntegration::startProfiling() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->startProfiling();
    }
}

void BrainIntegration::stopProfiling() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->stopProfiling();
    }
}

void BrainIntegration::startMemoryTracking() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->startMemoryTracking();
    }
}

void BrainIntegration::stopMemoryTracking() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->stopMemoryTracking();
    }
}

void BrainIntegration::startNeuralActivityTracking() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->startNeuralActivityTracking();
    }
}

void BrainIntegration::stopNeuralActivityTracking() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->stopNeuralActivityTracking();
    }
}

void BrainIntegration::startSimulationStateTracking() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->startSimulationStateTracking();
    }
}

void BrainIntegration::stopSimulationStateTracking() {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->stopSimulationStateTracking();
    }
}

std::string BrainIntegration::generateDebugReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== Brain Integration Debug Report ===\n\n";
    
    if (pImpl->debugSystem) {
        auto report = pImpl->debugSystem->generateDebugReport();
        oss << report.generateFullReport();
    }
    
    // Add integration-specific information
    if (pImpl->brain) {
        oss << "\n=== Brain Information ===\n";
        oss << "Total Neurons: " << pImpl->brain->getTotalNeuronCount() << "\n";
        oss << "Total Synapses: " << pImpl->brain->getTotalSynapseCount() << "\n";
        oss << "Regions: " << pImpl->brain->getRegionCount() << "\n";
        oss << "Current Step: " << pImpl->brain->getTotalSpikeCount() << "\n";
        oss << "Current Time: " << pImpl->brain->getTotalSpikeCount() * 0.001 << " seconds\n";
        oss << "Average Firing Rate: " << pImpl->brain->getAverageFiringRate() << " Hz\n";
    }
    
    return oss.str();
}

bool BrainIntegration::exportVisualization(const std::string& filename, 
                                          OutputFormat format, int quality) {
    if (!pImpl->visualization) return false;
    
    return pImpl->visualization->exportVisualization(filename, format, quality);
}

bool BrainIntegration::exportDebugData(const std::string& filename) {
    // Export debug data to file
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    // In a real implementation, serialize debug data
    // For now, create a placeholder file
    file << "Debug data export placeholder\n";
    
    return true;
}

BrainIntegration::IntegrationStats BrainIntegration::getStats() const {
    IntegrationStats stats;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->brain) {
        stats.totalNeurons = pImpl->brain->getTotalNeuronCount();
        stats.activeNeurons = pImpl->brain->getTotalSpikeCount() > 0 ? 
                             static_cast<uint64_t>(pImpl->brain->getAverageFiringRate() * stats.totalNeurons) : 0;
        stats.totalSpikes = pImpl->brain->getTotalSpikeCount();
        stats.currentFiringRate = pImpl->brain->getAverageFiringRate();
        stats.currentStep = pImpl->brain->getTotalSpikeCount();
        stats.currentTime = pImpl->brain->getTotalSpikeCount() * 0.001;
    }
    
    if (pImpl->debugSystem) {
        auto debugStats = pImpl->debugSystem->getProfiler().getFrameTimes();
        if (!debugStats.empty()) {
            double total = std::accumulate(debugStats.begin(), debugStats.end(), 0.0);
            stats.visualizationFPS = 1.0 / (total / debugStats.size());
        }
        
        auto resourceStats = pImpl->debugSystem->getResourceMonitor().getStats();
        stats.memoryUsage = resourceStats.memoryUsage;
        stats.peakMemoryUsage = resourceStats.peakMemoryUsage;
        stats.cpuLoad = resourceStats.cpuUsage;
        stats.gpuLoad = resourceStats.gpuLoad;
        
        stats.debugEventsCount = pImpl->debugSystem->getEventQueue().getEventCount();
        stats.isMonitoring = pImpl->debugSystem->getDebugState().isMonitoring;
    }
    
    stats.isProfiling = pImpl->config.enablePerformanceProfiling;
    stats.isVisualizationActive = pImpl->visualization && pImpl->visualization->isActive();
    
    return stats;
}

void BrainIntegration::registerVisualizationCallback(const VisualizationCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->visualizationCallbacks.push_back(callback);
}

void BrainIntegration::unregisterVisualizationCallback() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->visualizationCallbacks.clear();
}

void BrainIntegration::registerDebugCallback(const DebugCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugCallbacks.push_back(callback);
}

void BrainIntegration::unregisterDebugCallback() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugCallbacks.clear();
}

void BrainIntegration::registerPerformanceCallback(const PerformanceCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceCallbacks.push_back(callback);
}

void BrainIntegration::unregisterPerformanceCallback() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceCallbacks.clear();
}

void BrainIntegration::startPerformanceMeasurement(const std::string& operationName,
                                                 const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->currentPerformanceOperation = operationName;
    pImpl->performanceStart = std::chrono::high_resolution_clock::now();
}

void BrainIntegration::stopPerformanceMeasurement() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->currentPerformanceOperation.empty()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->performanceStart);
        
        PerformanceMeasurement measurement;
        measurement.operationName = pImpl->currentPerformanceOperation;
        measurement.duration = duration.count();
        measurement.timestamp = endTime;
        measurement.context = pImpl->config.enablePerformanceProfiling ? 
                             "profile" : "manual";
        
        // Record measurement
        if (pImpl->debugSystem) {
            pImpl->debugSystem->getProfiler().recordMetric(
                measurement.operationName, measurement.duration, "ms", measurement.context);
        }
        
        pImpl->currentPerformanceOperation.clear();
    }
}

double BrainIntegration::getCurrentPerformanceMeasurement() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->currentPerformanceOperation.empty()) {
        return 0.0;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->performanceStart);
    return duration.count();
}

void BrainIntegration::recordCustomPerformance(const std::string& name, double value,
                                              const std::string& unit,
                                              const std::string& context) {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->getProfiler().recordMetric(name, value, unit, context);
    }
}

std::vector<BrainIntegration::NeuralActivityAnalysis> BrainIntegration::analyzeNeuralActivity(
    uint64_t startStep, uint64_t endStep) const {
    std::vector<NeuralActivityAnalysis> analyses;
    
    if (!pImpl->debugSystem || !pImpl->brain) return analyses;
    
    auto neuralStats = pImpl->debugSystem->getNeuralMonitor().getStats();
    
    NeuralActivityAnalysis analysis;
    analysis.neuronId = 0; // Would be expanded to analyze all neurons
    analysis.averageFiringRate = neuralStats.averageFiringRate;
    analysis.maxFiringRate = neuralStats.maxFiringRate;
    analysis.minFiringRate = neuralStats.minFiringRate;
    analysis.spikeCount = neuralStats.totalSpikes;
    analysis.averageMembranePotential = 0.0f; // Would be calculated
    analysis.timeStepsAnalyzed = endStep - startStep;
    
    analyses.push_back(analysis);
    
    return analyses;
}

std::string BrainIntegration::getNeuralActivityReport() const {
    if (!pImpl->debugSystem) return "Debug system not initialized";
    
    return pImpl->debugSystem->getNeuralMonitor().getNeuralReport();
}

BrainIntegration::MemoryAnalysis BrainIntegration::analyzeMemory() const {
    MemoryAnalysis analysis;
    
    if (!pImpl->debugSystem) return analysis;
    
    auto memoryInfo = pImpl->debugSystem->getResourceMonitor().getStats();
    analysis.totalAllocated = memoryInfo.totalMemoryUsage;
    analysis.currentUsage = memoryInfo.memoryUsage;
    analysis.peakUsage = memoryInfo.peakMemoryUsage;
    
    if (memoryInfo.cpuSamples > 0) {
        analysis.averageAllocation = memoryInfo.totalMemoryUsage / memoryInfo.cpuSamples;
    }
    
    return analysis;
}

std::string BrainIntegration::getMemoryReport() const {
    if (!pImpl->debugSystem) return "Debug system not initialized";
    
    auto memoryInfo = pImpl->debugSystem->getResourceMonitor().getStats();
    
    std::ostringstream oss;
    oss << "=== Memory Analysis Report ===\n";
    oss << "Current Memory Usage: " << memoryInfo.memoryUsage << " bytes\n";
    oss << "Peak Memory Usage: " << memoryInfo.peakMemoryUsage << " bytes\n";
    oss << "Average Memory Usage: " << memoryInfo.averageMemoryUsage << " bytes\n";
    oss << "CPU Samples: " << memoryInfo.cpuSamples << "\n";
    
    return oss.str();
}

BrainIntegration::SimulationAnalysis BrainIntegration::analyzeSimulation() const {
    SimulationAnalysis analysis;
    
    if (!pImpl->debugSystem) return analysis;
    
    auto simStats = pImpl->debugSystem->getSimulationMonitor().getStats();
    
    analysis.totalSteps = simStats.totalSteps;
    analysis.totalTime = simStats.currentTime;
    analysis.averageStepTime = simStats.frameTime;
    analysis.simulationSpeed = simStats.simulationSpeed;
    analysis.errorCount = simStats.errorCount;
    analysis.errors = simStats.recentErrors;
    
    // Populate step times
    for (uint64_t i = 0; i < analysis.totalSteps && i < 1000; ++i) {
        analysis.stepTimes.push_back(std::make_pair(i, i * analysis.averageStepTime));
    }
    
    return analysis;
}

std::string BrainIntegration::getSimulationReport() const {
    if (!pImpl->debugSystem) return "Debug system not initialized";
    
    return pImpl->debugSystem->getSimulationMonitor().getSimulationReport();
}

void BrainIntegration::recordEvent(const std::string& eventType, const std::string& message,
                                  const std::unordered_map<std::string, std::string>& metadata) {
    if (pImpl->debugSystem) {
        pImpl->debugSystem->getEventQueue().pushStatus("[" + eventType + "] " + message);
    }
}

std::vector<BrainIntegration::DebugEvent> BrainIntegration::getRecentEvents(size_t count) const {
    std::vector<DebugEvent> events;
    
    if (pImpl->debugSystem) {
        auto debugEvents = pImpl->debugSystem->getEventQueue().getEvents(count);
        
        for (const auto& debugEvent : debugEvents) {
            DebugEvent event;
            event.timestamp = debugEvent.timestamp;
            event.eventType = debugEvent.type;
            event.message = debugEvent.data;
            events.push_back(event);
        }
    }
    
    return events;
}

} // namespace nlm
