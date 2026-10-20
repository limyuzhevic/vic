#include "DebugInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <thread>
#include <atomic>
#include <sstream>
#include <ctime>

namespace nlm {

struct DebugInterface::Impl {
    bool active;
    DebugMode debugMode;
    MonitoringMode monitoringMode;
    bool realTimeDebuggingEnabled;
    bool memoryLeakDetectionEnabled;
    bool performanceProfilingEnabled;
    bool errorDetectionEnabled;
    bool stepByStepDebuggingEnabled;
    bool commandLineInterfaceEnabled;
    bool debugOutputEnabled;
    int outputLevel;
    
    // Recording
    bool isRecording;
    std::string recordingFilename;
    std::vector<DebugStepInfo> recordedSteps;
    uint64_t recordingStartStep;
    
    // Performance tracking
    PerformanceMetrics currentPerformance;
    SimulationMetrics currentSimulation;
    ResourceUsage currentResourceUsage;
    NeuralMetrics currentNeuralMetrics;
    
    // Memory tracking
    std::vector<MemoryAllocation> activeAllocations;
    std::vector<MemoryAllocation> deallocations;
    std::atomic<size_t> totalAllocations;
    std::atomic<size_t> totalDeallocations;
    
    // Error tracking
    std::vector<ErrorInfo> recentErrors;
    std::vector<ErrorInfo> criticalErrors;
    
    // Debugging state
    uint64_t currentDebugStep;
    uint64_t maxDebugStep;
    std::vector<DebugStepInfo> debugHistory;
    size_t debugHistoryMaxSize;
    
    // Monitoring hooks
    std::vector<MonitoringHook> monitoringHooks;
    
    // Callbacks
    std::function<void(const std::string&, const std::string&)> debugEventCallback;
    std::function<void(const PerformanceMetrics&)> performanceCallback;
    std::function<void(const std::vector<MemoryAllocation>&)> memoryCallback;
    std::function<void(const NeuralMetrics&)> neuralCallback;
    
    // System resources
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::thread monitoringThread;
    std::atomic<bool> monitoringThreadRunning;
    std::chrono::milliseconds monitoringInterval;
    
    Impl() : active(false), debugMode(DebugMode::Detailed), monitoringMode(MonitoringMode::Active),
             realTimeDebuggingEnabled(true), memoryLeakDetectionEnabled(true),
             performanceProfilingEnabled(true), errorDetectionEnabled(true),
             stepByStepDebuggingEnabled(false), commandLineInterfaceEnabled(true),
             debugOutputEnabled(true), outputLevel(1), isRecording(false),
             recordingStartStep(0), totalAllocations(0), totalDeallocations(0),
             currentDebugStep(0), maxDebugStep(0), debugHistoryMaxSize(1000),
             monitoringThreadRunning(false), monitoringInterval(100) {
        initializeSystemMetrics();
    }
    
    void initializeSystemMetrics() {
        currentPerformance.timestamp = std::chrono::steady_clock::now();
        currentSimulation.timestamp = currentPerformance.timestamp;
        currentResourceUsage.timestamp = currentPerformance.timestamp;
        currentNeuralMetrics.timestamp = currentPerformance.timestamp;
    }
    
    void startMonitoringThread() {
        monitoringThreadRunning = true;
        monitoringThread = std::thread(&Impl::monitoringLoop, this);
    }
    
    void stopMonitoringThread() {
        monitoringThreadRunning = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
    
    void monitoringLoop() {
        while (monitoringThreadRunning) {
            updateMetrics();
            executeMonitoringHooks();
            std::this_thread::sleep_for(monitoringInterval);
        }
    }
    
    void updateMetrics() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastUpdateTime
        );
        
        // Update performance metrics
        currentPerformance.timestamp = now;
        currentSimulation.timestamp = now;
        currentResourceUsage.timestamp = now;
        currentNeuralMetrics.timestamp = now;
        
        // Update counters
        if (debugOutputEnabled && outputLevel >= 2) {
            NLM_LOG_INFO("DebugInterface update: Step " + std::to_string(currentSimulation.currentStep) +
                        " | Performance: " + std::to_string(currentPerformance.cpuUsage) + "% CPU");
        }
    }
    
    void executeMonitoringHooks() {
        auto now = std::chrono::steady_clock::now();
        for (auto& hook : monitoringHooks) {
            if (hook.isActive) {
                auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
                    now - hook.lastExecution
                );
                if (duration.count() >= hook.executionInterval) {
                    // Execute hook
                    std::unordered_map<std::string, double> metrics;
                    // Collect metrics for hook execution
                    hook.callback(metrics);
                    hook.lastExecution = now;
                }
            }
        }
    }
    
    std::string generateStackTrace() {
        // Simplified stack trace generation
        return "Stack trace not implemented in this simplified version";
    }
    
    void captureSystemSnapshot() {
        // Capture current system state
        auto now = std::chrono::steady_clock::now();
        DebugStepInfo stepInfo;
        stepInfo.step = currentDebugStep;
        stepInfo.timestamp = now;
        stepInfo.isHealthy = true;
        
        // Add system states
        std::stringstream ss;
        ss << "Step: " << currentDebugStep
           << " | CPU Usage: " << currentPerformance.cpuUsage << "%"
           << " | Memory Usage: " << currentPerformance.memoryUsage << "%"
           << " | Active Neurons: " << currentNeuralMetrics.activeNeuronCount
           << " | Total Spikes: " << currentSimulation.spikeCount;
        stepInfo.systemStates["system"] = ss.str();
        
        debugHistory.push_back(stepInfo);
        if (debugHistory.size() > debugHistoryMaxSize) {
            debugHistory.erase(debugHistory.begin());
        }
    }
};

DebugInterface::DebugInterface() : pImpl(std::make_unique<Impl>()) {}

DebugInterface::~DebugInterface() {
    close();
}

bool DebugInterface::initialize() {
    try {
        NLM_LOG_INFO("Initializing DebugInterface...");
        
        // Initialize backends
        initializeBackends();
        
        // Start monitoring thread
        pImpl->startMonitoringThread();
        
        pImpl->active = true;
        pImpl->lastUpdateTime = std::chrono::steady_clock::now();
        
        NLM_LOG_INFO("DebugInterface initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize DebugInterface: " + std::string(e.what()));
        return false;
    }
}

void DebugInterface::update() {
    if (!pImpl->active) return;
    
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - pImpl->lastUpdateTime
    );
    
    // Update metrics if output is enabled
    if (pImpl->debugOutputEnabled && pImpl->outputLevel >= 2) {
        NLM_LOG_INFO("DebugInterface update: " + std::to_string(duration.count()) + "ms since last update");
    }
    
    pImpl->lastUpdateTime = now;
    
    // Capture debug step if step-by-step debugging is enabled
    if (pImpl->stepByStepDebuggingEnabled) {
        pImpl->captureSystemSnapshot();
    }
}

void DebugInterface::close() {
    if (pImpl->active) {
        NLM_LOG_INFO("Closing DebugInterface...");
        
        pImpl->stopMonitoringThread();
        cleanupBackends();
        
        // Stop recording if active
        if (pImpl->isRecording) {
            stopRecording();
        }
        
        pImpl->active = false;
        NLM_LOG_INFO("DebugInterface closed successfully");
    }
}

bool DebugInterface::isActive() const {
    return pImpl->active;
}

void DebugInterface::setDebugMode(DebugMode mode) {
    pImpl->debugMode = mode;
    std::string modeStr;
    switch (mode) {
        case DebugMode::None: modeStr = "None"; break;
        case DebugMode::Basic: modeStr = "Basic"; break;
        case DebugMode::Detailed: modeStr = "Detailed"; break;
        case DebugMode::Expert: modeStr = "Expert"; break;
    }
    NLM_LOG_INFO("Debug mode set to: " + modeStr);
}

DebugMode DebugInterface::getDebugMode() const {
    return pImpl->debugMode;
}

void DebugInterface::setMonitoringMode(MonitoringMode mode) {
    pImpl->monitoringMode = mode;
    std::string modeStr;
    switch (mode) {
        case MonitoringMode::Disabled: modeStr = "Disabled"; break;
        case MonitoringMode::Passive: modeStr = "Passive"; break;
        case MonitoringMode::Active: modeStr = "Active"; break;
        case MonitoringMode::Alerting: modeStr = "Alerting"; break;
    }
    NLM_LOG_INFO("Monitoring mode set to: " + modeStr);
}

MonitoringMode DebugInterface::getMonitoringMode() const {
    return pImpl->monitoringMode;
}

void DebugInterface::enableRealTimeDebugging(bool enable) {
    pImpl->realTimeDebuggingEnabled = enable;
    NLM_LOG_INFO("Real-time debugging " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isRealTimeDebuggingEnabled() const {
    return pImpl->realTimeDebuggingEnabled;
}

void DebugInterface::enableMemoryLeakDetection(bool enable) {
    pImpl->memoryLeakDetectionEnabled = enable;
    NLM_LOG_INFO("Memory leak detection " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isMemoryLeakDetectionEnabled() const {
    return pImpl->memoryLeakDetectionEnabled;
}

std::vector<MemoryAllocation> DebugInterface::detectMemoryLeaks() const {
    std::vector<MemoryAllocation> leaks;
    
    if (!pImpl->memoryLeakDetectionEnabled) {
        NLM_LOG_WARNING("Memory leak detection is not enabled");
        return leaks;
    }
    
    // Simple leak detection: allocations without corresponding deallocations
    for (const auto& alloc : pImpl->activeAllocations) {
        // Check if this allocation has been deallocated
        bool isDeallocated = false;
        for (const auto& dealloc : pImpl->deallocations) {
            if (alloc.address == dealloc.address) {
                isDeallocated = true;
                break;
            }
        }
        
        if (!isDeallocated && alloc.allocationTime > std::chrono::steady_clock::now() - std::chrono::seconds(300)) {
            // Consider recent allocations as potential leaks
            MemoryAllocation leak = alloc;
            leak.isLeaked = true;
            leaks.push_back(leak);
        }
    }
    
    NLM_LOG_INFO("Memory leak detection found " + std::to_string(leaks.size()) + " potential leaks");
    return leaks;
}

void DebugInterface::trackMemoryAllocation(void* address, size_t size, const std::string& site) {
    if (!pImpl->memoryLeakDetectionEnabled) return;
    
    MemoryAllocation alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.allocationTime = std::chrono::steady_clock::now();
    alloc.allocationSite = site;
    alloc.threadId = std::this_thread::get_id();
    alloc.isLeaked = false;
    
    pImpl->activeAllocations.push_back(alloc);
    pImpl->totalAllocations++;
    
    // Call memory callback if set
    if (pImpl->memoryCallback) {
        pImpl->memoryCallback(pImpl->activeAllocations);
    }
    
    if (pImpl->debugOutputEnabled && pImpl->outputLevel >= 2) {
        NLM_LOG_DEBUG("Memory allocation: " + std::to_string(size) + " bytes at " + 
                     std::to_string(reinterpret_cast<uintptr_t>(address)) + 
                     " from " + site);
    }
}

void DebugInterface::trackMemoryDeallocation(void* address) {
    if (!pImpl->memoryLeakDetectionEnabled) return;
    
    // Find and remove the allocation
    for (auto it = pImpl->activeAllocations.begin(); it != pImpl->activeAllocations.end(); ++it) {
        if (it->address == address) {
            MemoryAllocation dealloc = *it;
            dealloc.allocationTime = std::chrono::steady_clock::now();
            pImpl->deallocations.push_back(dealloc);
            pImpl->activeAllocations.erase(it);
            pImpl->totalDeallocations++;
            break;
        }
    }
    
    // Call memory callback if set
    if (pImpl->memoryCallback) {
        pImpl->memoryCallback(pImpl->activeAllocations);
    }
    
    if (pImpl->debugOutputEnabled && pImpl->outputLevel >= 2) {
        NLM_LOG_DEBUG("Memory deallocation: " + std::to_string(reinterpret_cast<uintptr_t>(address)));
    }
}

void DebugInterface::enablePerformanceProfiling(bool enable) {
    pImpl->performanceProfilingEnabled = enable;
    NLM_LOG_INFO("Performance profiling " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isPerformanceProfilingEnabled() const {
    return pImpl->performanceProfilingEnabled;
}

PerformanceMetrics DebugInterface::getPerformanceMetrics() const {
    // This is a simplified implementation
    PerformanceMetrics metrics;
    metrics.cpuUsage = 50.0; // placeholder
    metrics.memoryUsage = 60.0; // placeholder
    metrics.totalMemory = 16 * 1024 * 1024 * 1024ULL; // 16GB placeholder
    metrics.usedMemory = 9 * 1024 * 1024 * 1024ULL; // 9GB placeholder
    metrics.peakMemory = 12 * 1024 * 1024 * 1024ULL; // 12GB placeholder
    metrics.networkBytesSent = 1024 * 1024; // 1MB placeholder
    metrics.networkBytesReceived = 2048 * 1024; // 2MB placeholder
    metrics.diskUsage = 70.0; // 70% placeholder
    metrics.timestamp = std::chrono::steady_clock::now();
    return metrics;
}

double DebugInterface::getCPUUsage() const {
    return pImpl->performanceProfilingEnabled ? 50.0 : 0.0;
}

double DebugInterface::getMemoryUsage() const {
    return pImpl->performanceProfilingEnabled ? 60.0 : 0.0;
}

double DebugInterface::getDiskUsage() const {
    return pImpl->performanceProfilingEnabled ? 70.0 : 0.0;
}

NeuralMetrics DebugInterface::getNeuralMetrics(const Brain& brain) const {
    NeuralMetrics metrics;
    metrics.totalNeuronCount = 1000; // placeholder
    metrics.activeNeuronCount = 450; // placeholder
    metrics.spikingNeuronCount = 150; // placeholder
    metrics.averageFiringRate = 8.5; // Hz placeholder
    metrics.excitationInhibitionRatio = 1.2; // placeholder
    metrics.meanMembranePotential = -65.0; // mV placeholder
    metrics.meanSynapticWeight = 0.5; // placeholder
    metrics.connectivityDensity = 0.15; // placeholder
    metrics.timestamp = std::chrono::steady_clock::now();
    return metrics;
}

float DebugInterface::getAverageFiringRate(const Brain& brain) const {
    return 8.5f; // placeholder
}

size_t DebugInterface::getActiveNeuronCount(const Brain& brain) const {
    return 450; // placeholder
}

std::vector<uint64_t> DebugInterface::getMostActiveNeurons(const Brain& brain, size_t count) const {
    std::vector<uint64_t> mostActive;
    for (size_t i = 0; i < std::min(count, static_cast<size_t>(10)); ++i) {
        mostActive.push_back(i);
    }
    return mostActive;
}

DebugStepInfo DebugInterface::getCurrentDebugStep() const {
    return pImpl->debugHistory.empty() ? DebugStepInfo() : pImpl->debugHistory.back();
}

DebugStepInfo DebugInterface::getDebugStep(uint64_t step) const {
    for (const auto& stepInfo : pImpl->debugHistory) {
        if (stepInfo.step == step) {
            return stepInfo;
        }
    }
    return DebugStepInfo();
}

std::string DebugInterface::inspectNeuron(const Neuron& neuron) const {
    return "Neuron ID: " + std::to_string(neuron.getId().value) +
           " | Membrane Potential: " + std::to_string(neuron.getMembranePotential()) +
           " | Threshold: " + std::to_string(neuron.getThreshold());
}

std::string DebugInterface::inspectSynapse(const Synapse& synapse) const {
    return "Synapse ID: " + std::to_string(synapse.getId().value) +
           " | Weight: " + std::to_string(synapse.getWeight()) +
           " | Pre: " + std::to_string(synapse.getSourceId().value) +
           " | Post: " + std::to_string(synapse.getTargetId().value);
}

std::string DebugInterface::inspectNetwork(const Brain& brain) const {
    std::stringstream ss;
    ss << "Network Statistics:" << std::endl;
    ss << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "  Active Regions: " << brain.getRegionCount() << std::endl;
    return ss.str();
}

void DebugInterface::enableStepByStepDebugging(bool enable) {
    pImpl->stepByStepDebuggingEnabled = enable;
    NLM_LOG_INFO("Step-by-step debugging " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isStepByStepDebuggingEnabled() const {
    return pImpl->stepByStepDebuggingEnabled;
}

void DebugInterface::setDebugStep(uint64_t step) {
    pImpl->currentDebugStep = step;
    NLM_LOG_INFO("Debug step set to: " + std::to_string(step));
}

uint64_t DebugInterface::getCurrentDebugStep() const {
    return pImpl->currentDebugStep;
}

void DebugInterface::stepForward() {
    if (pImpl->currentDebugStep < pImpl->maxDebugStep) {
        pImpl->currentDebugStep++;
        NLM_LOG_INFO("Debug step advanced to: " + std::to_string(pImpl->currentDebugStep));
    }
}

void DebugInterface::stepBackward() {
    if (pImpl->currentDebugStep > 0) {
        pImpl->currentDebugStep--;
        NLM_LOG_INFO("Debug step moved back to: " + std::to_string(pImpl->currentDebugStep));
    }
}

bool DebugInterface::hasMoreStepsForward() const {
    return pImpl->currentDebugStep < pImpl->maxDebugStep;
}

bool DebugInterface::hasMoreStepsBackward() const {
    return pImpl->currentDebugStep > 0;
}

void DebugInterface::enableErrorDetection(bool enable) {
    pImpl->errorDetectionEnabled = enable;
    NLM_LOG_INFO("Error detection " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isErrorDetectionEnabled() const {
    return pImpl->errorDetectionEnabled;
}

std::vector<ErrorInfo> DebugInterface::getRecentErrors() const {
    return pImpl->recentErrors;
}

std::vector<ErrorInfo> DebugInterface::getCriticalErrors() const {
    return pImpl->criticalErrors;
}

bool DebugInterface::hasRecentErrors() const {
    return !pImpl->recentErrors.empty();
}

void DebugInterface::logError(const std::string& errorCode, const std::string& errorMessage,
                             bool isCritical, const std::unordered_map<std::string, std::string>& context) {
    if (!pImpl->errorDetectionEnabled) return;
    
    ErrorInfo error;
    error.timestamp = std::chrono::steady_clock::now();
    error.errorType = "runtime";
    error.errorCode = errorCode;
    error.errorMessage = errorMessage;
    error.isRecovered = false;
    error.isCritical = isCritical;
    error.context = context;
    
    // Generate stack trace
    error.stackTrace = pImpl->generateStackTrace();
    
    if (isCritical) {
        pImpl->criticalErrors.push_back(error);
    } else {
        pImpl->recentErrors.push_back(error);
    }
    
    NLM_LOG_ERROR("Error " + errorCode + ": " + errorMessage);
}

ValidationResult DebugInterface::validateConfiguration(const Config& config) const {
    ValidationResult result;
    result.isValid = true;
    result.validationTime = std::chrono::steady_clock::now();
    
    // Basic validation - check for required keys in a demo configuration
    if (config.getConfigValue("neuron_count") == "0") {
        result.isValid = false;
        result.errors.push_back("neuron_count must be greater than 0");
        result.correctedValues["neuron_count"] = "1000";
    }
    
    if (config.getConfigValue("simulation_timestep") == "0.0") {
        result.isValid = false;
        result.errors.push_back("simulation_timestep must be greater than 0.0");
        result.correctedValues["simulation_timestep"] = "0.001";
    }
    
    return result;
}

std::vector<std::string> DebugInterface::getConfigurationErrors(const Config& config) const {
    ValidationResult validation = validateConfiguration(config);
    return validation.errors;
}

std::vector<std::string> DebugInterface::getConfigurationWarnings(const Config& config) const {
    std::vector<std::string> warnings;
    // Basic warning checks
    if (config.getConfigValue("neuron_count") == "100") {
        warnings.push_back("neuron_count is set to minimum - may limit simulation complexity");
    }
    return warnings;
}

ResourceUsage DebugInterface::getResourceUsage() const {
    ResourceUsage usage;
    usage.memoryPoolUsed = 1024 * 1024 * 1024ULL; // 1GB placeholder
    usage.memoryPoolAllocated = 1536 * 1024 * 1024ULL; // 1.5GB placeholder
    usage.eventQueueSize = 100; // placeholder
    usage.pendingSpikes = 50; // placeholder
    usage.checkpointSize = 512 * 1024 * 1024ULL; // 512MB placeholder
    usage.cpuTemperature = 65.0; // degrees C placeholder
    usage.memoryTemperature = 45.0; // degrees C placeholder
    usage.timestamp = std::chrono::steady_clock::now();
    return usage;
}

SimulationMetrics DebugInterface::getSimulationMetrics() const {
    SimulationMetrics metrics;
    metrics.currentStep = 1000; // placeholder
    metrics.simulationTime = 1.0; // seconds placeholder
    metrics.wallClockTime = 1.2; // seconds placeholder
    metrics.stepsPerSecond = 1000.0; // placeholder
    metrics.efficiency = 0.833; // placeholder
    metrics.spikeCount = 5000; // placeholder
    metrics.eventCount = 3000; // placeholder
    metrics.memoryPoolAllocationCount = 10000; // placeholder
    metrics.memoryPoolDeallocationCount = 9800; // placeholder
    metrics.timestamp = std::chrono::steady_clock::now();
    return metrics;
}

void DebugInterface::trackSystemEvents(const std::string& eventType,
                                       const std::unordered_map<std::string, std::string>& eventData) {
    // This would store events in a log for later analysis
    NLM_LOG_INFO("System event: " + eventType);
}

std::vector<std::unordered_map<std::string, std::string>> DebugInterface::getRecentEvents(const std::string& eventType) const {
    return std::vector<std::unordered_map<std::string, std::string>>();
}

void DebugInterface::addMonitoringHook(const MonitoringHook& hook) {
    pImpl->monitoringHooks.push_back(hook);
    NLM_LOG_INFO("Added monitoring hook: " + hook.hookName);
}

void DebugInterface::removeMonitoringHook(const std::string& hookName) {
    for (auto it = pImpl->monitoringHooks.begin(); it != pImpl->monitoringHooks.end(); ++it) {
        if (it->hookName == hookName) {
            pImpl->monitoringHooks.erase(it);
            NLM_LOG_INFO("Removed monitoring hook: " + hookName);
            return;
        }
    }
}

void DebugInterface::executeMonitoringHooks() {
    // Execute all active hooks
    for (auto& hook : pImpl->monitoringHooks) {
        if (hook.isActive) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
                now - hook.lastExecution
            );
            if (duration.count() >= hook.executionInterval) {
                // Execute hook
                std::unordered_map<std::string, double> metrics;
                // Collect metrics for hook execution
                hook.callback(metrics);
                hook.lastExecution = now;
            }
        }
    }
}

std::vector<MonitoringHook> DebugInterface::getMonitoringHooks() const {
    return pImpl->monitoringHooks;
}

void DebugInterface::enableCommandLineInterface(bool enable) {
    pImpl->commandLineInterfaceEnabled = enable;
    NLM_LOG_INFO("Command-line interface " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isCommandLineInterfaceEnabled() const {
    return pImpl->commandLineInterfaceEnabled;
}

bool DebugInterface::executeCommand(const std::string& commandLine) {
    if (!pImpl->commandLineInterfaceEnabled) {
        NLM_LOG_WARNING("Command-line interface is not enabled");
        return false;
    }
    
    // Simple command parsing
    if (commandLine == "help") {
        NLM_LOG_INFO("Available commands: help, quit, visualize [brain|world], debug [info|memory|perf], export [filename], record [start|stop]");
        return true;
    } else if (commandLine == "quit") {
        NLM_LOG_INFO("Shutting down...")
        return true; // Signal to quit
    } else if (commandLine.substr(0, 9) == "visualize ") {
        std::string target = commandLine.substr(9);
        NLM_LOG_INFO("Visualizing " + target);
        return true;
    }
    
    NLM_LOG_WARNING("Unknown command: " + commandLine);
    return false;
}

std::vector<DebugCommand> DebugInterface::getAvailableCommands() const {
    std::vector<DebugCommand> commands;
    
    DebugCommand helpCmd;
    helpCmd.commandName = "help";
    helpCmd.description = "Show available commands";
    helpCmd.requiredParameters = {};
    helpCmd.category = "general";
    commands.push_back(helpCmd);
    
    DebugCommand quitCmd;
    quitCmd.commandName = "quit";
    quitCmd.description = "Quit the application";
    quitCmd.requiredParameters = {};
    quitCmd.category = "general";
    commands.push_back(quitCmd);
    
    DebugCommand visualizeCmd;
    visualizeCmd.commandName = "visualize";
    visualizeCmd.description = "Enable visualization (brain|world|both)";
    visualizeCmd.requiredParameters = {"target"};
    visualizeCmd.category = "visualization";
    commands.push_back(visualizeCmd);
    
    DebugCommand debugCmd;
    debugCmd.commandName = "debug";
    debugCmd.description = "Debug information (info|memory|performance|neural)";
    debugCmd.requiredParameters = {"type"};
    debugCmd.category = "debug";
    commands.push_back(debugCmd);
    
    return commands;
}

std::string DebugInterface::getCommandHelp(const std::string& commandName) const {
    for (const auto& cmd : getAvailableCommands()) {
        if (cmd.commandName == commandName) {
            return cmd.description + "\nRequired parameters: " + 
                   (cmd.requiredParameters.empty() ? "none" : std::to_string(cmd.requiredParameters.size()) + " parameters") +
                   "\nCategory: " + cmd.category;
        }
    }
    return "Command not found: " + commandName;
}

void DebugInterface::enableDebugOutput(bool enable) {
    pImpl->debugOutputEnabled = enable;
    NLM_LOG_INFO("Debug output " + std::string(enable ? "enabled" : "disabled"));
}

bool DebugInterface::isDebugOutputEnabled() const {
    return pImpl->debugOutputEnabled;
}

void DebugInterface::setOutputLevel(int level) {
    pImpl->outputLevel = std::max(0, std::min(3, level));
    NLM_LOG_INFO("Output level set to: " + std::to_string(pImpl->outputLevel));
}

int DebugInterface::getOutputLevel() const {
    return pImpl->outputLevel;
}

std::string DebugInterface::generateDebugReport() const {
    std::stringstream report;
    
    report << "=== NLM Debug Report ===" << std::endl;
    report << "Generated at: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count() << " seconds since epoch" << std::endl;
    report << std::endl;
    
    report << "=== System Status ===" << std::endl;
    report << "Debug Interface Active: " << (isActive() ? "Yes" : "No") << std::endl;
    report << "Debug Mode: " << (pImpl->debugMode == DebugMode::Detailed ? "Detailed" :
                              pImpl->debugMode == DebugMode::Expert ? "Expert" :
                              pImpl->debugMode == DebugMode::Basic ? "Basic" : "None") << std::endl;
    report << "Monitoring Mode: " << (pImpl->monitoringMode == MonitoringMode::Active ? "Active" :
                                  pImpl->monitoringMode == MonitoringMode::Alerting ? "Alerting" :
                                  pImpl->monitoringMode == MonitoringMode::Passive ? "Passive" : "Disabled") << std::endl;
    report << std::endl;
    
    report << "=== Performance ===" << std::endl;
    PerformanceMetrics perf = getPerformanceMetrics();
    report << "CPU Usage: " << perf.cpuUsage << "%" << std::endl;
    report << "Memory Usage: " << perf.memoryUsage << "% (" << 
           perf.usedMemory / (1024*1024*1024) << " GB / " << 
           perf.totalMemory / (1024*1024*1024) << " GB)" << std::endl;
    report << "Disk Usage: " << perf.diskUsage << "%" << std::endl;
    report << std::endl;
    
    report << "=== Neural Activity ===" << std::endl;
    NeuralMetrics neural = getNeuralMetrics(*(new Brain(nullptr))); // This is a hack, should be passed
    report << "Total Neurons: " << neural.totalNeuronCount << std::endl;
    report << "Active Neurons: " << neural.activeNeuronCount << std::endl;
    report << "Average Firing Rate: " << neural.averageFiringRate << " Hz" << std::endl;
    report << std::endl;
    
    report << "=== Memory Management ===" << std::endl;
    report << "Total Allocations: " << pImpl->totalAllocations << std::endl;
    report << "Total Deallocations: " << pImpl->totalDeallocations << std::endl;
    report << "Active Allocations: " << pImpl->activeAllocations.size() << std::endl;
    
    report << "=== Errors ===" << std::endl;
    report << "Recent Errors: " << pImpl->recentErrors.size() << std::endl;
    report << "Critical Errors: " << pImpl->criticalErrors.size() << std::endl;
    
    report << "=== Debug History ===" << std::endl;
    report << "Current Debug Step: " << pImpl->currentDebugStep << std::endl;
    report << "Max Debug Step: " << pImpl->maxDebugStep << std::endl;
    report << "Debug History Size: " << pImpl->debugHistory.size() << std::endl;
    
    report << "=== End Report ===" << std::endl;
    
    return report.str();
}

void DebugInterface::exportDebugData(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateDebugReport() << std::endl;
        file << "\n=== Detailed Debug Data ===" << std::endl;
        
        // Export detailed debug data
        for (const auto& step : pImpl->debugHistory) {
            file << "Step " << step.step << ": " << step.systemStates.at("system") << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Debug data exported to: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to export debug data to: " + filename);
    }
}

void DebugInterface::setDebugEventCallback(std::function<void(const std::string&, const std::string&)> callback) {
    pImpl->debugEventCallback = callback;
}

void DebugInterface::setPerformanceCallback(std::function<void(const PerformanceMetrics&)> callback) {
    pImpl->performanceCallback = callback;
}

void DebugInterface::setMemoryCallback(std::function<void(const std::vector<MemoryAllocation>&)> callback) {
    pImpl->memoryCallback = callback;
}

void DebugInterface::setNeuralCallback(std::function<void(const NeuralMetrics&)> callback) {
    pImpl->neuralCallback = callback;
}

void DebugInterface::startRecording(const std::string& filename) {
    pImpl->isRecording = true;
    pImpl->recordingFilename = filename;
    pImpl->recordingStartStep = pImpl->currentDebugStep;
    NLM_LOG_INFO("Started debug recording to: " + filename);
}

void DebugInterface::stopRecording() {
    if (pImpl->isRecording) {
        pImpl->isRecording = false;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - pImpl->lastUpdateTime
        );
        NLM_LOG_INFO("Stopped debug recording. Duration: " + std::to_string(duration.count()) + " steps");
        
        // Save recorded data
        exportDebugData(pImpl->recordingFilename);
    }
}

bool DebugInterface::isRecording() const {
    return pImpl->isRecording;
}

void DebugInterface::replayDebugData(const std::string& filename, uint64_t startStep, uint64_t endStep) {
    // This would load and replay debug data from a file
    NLM_LOG_INFO("Replaying debug data from: " + filename + 
                 " (steps " + std::to_string(startStep) + " to " + std::to_string(endStep) + ")");
}

void DebugInterface::loadDebugConfiguration(const std::string& configFile) {
    // Load debug configuration from file
    NLM_LOG_INFO("Loading debug configuration from: " + configFile);
}

void DebugInterface::saveDebugConfiguration(const std::string& configFile) const {
    // Save debug configuration to file
    NLM_LOG_INFO("Saving debug configuration to: " + configFile);
}

void DebugInterface::initializeBackends() {
    // Initialize debug backends (file, network, etc.)
    NLM_LOG_INFO("Initializing debug backends");
}

void DebugInterface::cleanupBackends() {
    // Clean up debug backends
    NLM_LOG_INFO("Cleaning up debug backends");
}

bool DebugInterface::areBackendsInitialized() const {
    return true; // Simplified
}

} // namespace nlm