#include "DebuggingSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>

namespace nlm {

struct DebuggingSystem::Impl {
    // Hook management
    std::vector<std::shared_ptr<DebugHook>> hooks;
    mutable std::mutex hooksMutex;
    
    // Performance metrics
    std::unordered_map<std::string, std::vector<double>> performanceMetrics;
    mutable std::mutex metricsMutex;
    
    // Resource usage history
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> resourceHistory;
    mutable std::mutex historyMutex;
    
    // Integration pointers
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
    std::shared_ptr<VisualizationInterface> visualization;
    
    // Remote debugging
    bool remoteDebuggerActive{false};
    std::thread remoteDebuggerThread;
    std::mutex remoteMutex;
    
    // Multi-session support
    std::string currentSessionName{"default"};
    std::vector<std::string> sessionNames;
    
    // Anomaly detection
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> anomalyHistory;
    mutable std::mutex anomalyMutex;
    
    // Performance profiling data
    struct PerformanceSnapshot {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        double duration;
        double memoryUsage;
    };
    
    std::vector<PerformanceSnapshot> performanceHistory;
    mutable std::mutex historyMutex;
    
    // Memory tracking
    struct MemorySnapshot {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        size_t size;
        std::string context;
    };
    
    std::vector<MemorySnapshot> memoryHistory;
    mutable std::mutex memoryMutex;
};

DebuggingSystem::DebuggingSystem() : status_(DebugStatus::Inactive) {
    pImpl = std::make_unique<Impl>();
    trace_ = std::make_shared<DebugTrace>();
}

DebuggingSystem::DebuggingSystem(const DebugConfiguration& config) : config_(config), status_(DebugStatus::Inactive) {
    pImpl = std::make_unique<Impl>();
    trace_ = std::make_shared<DebugTrace>();
    
    // Apply configuration
    if (config_.enableTrace) {
        trace_->setMaxEvents(config_.maxTraceEvents);
    }
    
    if (config_.enableEventFiltering && !config_.eventFilter.empty()) {
        for (auto type : config_.eventFilter) {
            trace_->addEventFilter(type, true);
        }
    }
}

DebuggingSystem::~DebuggingSystem() = default;

void DebuggingSystem::configure(const DebugConfiguration& config) {
    config_ = config;
    
    if (config_.enableTrace) {
        trace_->setMaxEvents(config_.maxTraceEvents);
    }
    
    if (config_.enableEventFiltering && !config_.eventFilter.empty()) {
        trace_->clearEventFilters();
        for (auto type : config_.eventFilter) {
            trace_->addEventFilter(type, true);
        }
    }
}

void DebuggingSystem::start() {
    if (status_ == DebugStatus::Active || status_ == DebugStatus::Initializing) {
        return;
    }
    
    DebugStatus oldStatus = status_;
    status_ = DebugStatus::Initializing;
    
    notifyStatusChangeHooks(oldStatus, status_);
    
    // Initialize trace
    if (config_.enableTrace) {
        trace_->clearEventCallbacks();
        trace_->addEventCallback([this](const DebugEvent& event) {
            notifyHooks(event);
        });
    }
    
    status_ = DebugStatus::Active;
    
    notifyStatusChangeHooks(oldStatus, status_);
    
    // Start remote debugger if enabled
    if (config_.enableRemoteDebugging) {
        startRemoteDebugger();
    }
    
    Logger::getGlobal().info("Debugging system started");
}

void DebuggingSystem::stop() {
    if (status_ != DebugStatus::Active && status_ != DebugStatus::Paused) {
        return;
    }
    
    DebugStatus oldStatus = status_;
    status_ = DebugStatus::ShuttingDown;
    
    notifyStatusChangeHooks(oldStatus, status_);
    
    // Stop remote debugger
    stopRemoteDebugger();
    
    // Export data if enabled
    if (config_.enableExport && config_.exportToFile) {
        exportDebugData(config_.logFile);
    }
    
    status_ = DebugStatus::Inactive;
    
    notifyStatusChangeHooks(oldStatus, status_);
    
    Logger::getGlobal().info("Debugging system stopped");
}

void DebuggingSystem::pause() {
    if (status_ != DebugStatus::Active) {
        return;
    }
    
    DebugStatus oldStatus = status_;
    status_ = DebugStatus::Paused;
    
    notifyStatusChangeHooks(oldStatus, status_);
}

void DebuggingSystem::resume() {
    if (status_ != DebugStatus::Paused) {
        return;
    }
    
    DebugStatus oldStatus = status_;
    status_ = DebugStatus::Active;
    
    notifyStatusChangeHooks(oldStatus, status_);
}

void DebuggingSystem::reset() {
    if (trace_) {
        trace_->clearEventCallbacks();
        trace_->clearEventFilters();
    }
    
    pImpl->performanceMetrics.clear();
    pImpl->resourceHistory.clear();
    pImpl->anomalyHistory.clear();
    pImpl->performanceHistory.clear();
    pImpl->memoryHistory.clear();
    
    Logger::getGlobal().info("Debugging system reset");
}

void DebuggingSystem::addHook(std::shared_ptr<DebugHook> hook) {
    if (!hook) return;
    
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    pImpl->hooks.push_back(hook);
    
    // Sort by priority (higher priority first)
    std::sort(pImpl->hooks.begin(), pImpl->hooks.end(),
        [](const std::shared_ptr<DebugHook>& a, const std::shared_ptr<DebugHook>& b) {
            return a->getPriority() > b->getPriority();
        });
}

void DebuggingSystem::removeHook(std::shared_ptr<DebugHook> hook) {
    if (!hook) return;
    
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    pImpl->hooks.erase(std::remove(pImpl->hooks.begin(), pImpl->hooks.end(), hook), pImpl->hooks.end());
}

void DebuggingSystem::clearHooks() {
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    pImpl->hooks.clear();
}

void DebuggingSystem::recordEvent(DebugEventType type, const std::string& source, const std::string& message) {
    if (!isActive()) return;
    
    DebugEvent event(type, source, message);
    recordEvent(std::move(event));
}

void DebuggingSystem::recordEvent(DebugEvent event) {
    if (!isActive()) return;
    
    // Notify hooks
    notifyHooks(event);
    
    // Add to trace
    if (trace_) {
        trace_->addEvent(std::move(event));
    }
}

void DebuggingSystem::recordPerformanceMetric(const std::string& name, double value, const std::string& unit) {
    if (!isActive()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->performanceMetrics[name].push_back(value);
    
    notifyPerformanceHooks(name, value);
}

double DebuggingSystem::getAveragePerformance(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->performanceMetrics.find(name);
    if (it == pImpl->performanceMetrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (double value : it->second) {
        sum += value;
    }
    return sum / it->second.size();
}

double DebuggingSystem::getMaxPerformance(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->performanceMetrics.find(name);
    if (it == pImpl->performanceMetrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

void DebuggingSystem::recordResourceUsage(double cpu, double memory, size_t memoryAllocations, size_t threadCount) {
    if (!isActive()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    std::ostringstream oss;
    oss << "CPU: " << cpu << "%, Memory: " << memory << "%, "
        << "Allocations: " << memoryAllocations << ", Threads: " << threadCount;
    
    pImpl->resourceHistory.emplace_back(std::chrono::system_clock::now(), oss.str());
    
    // Keep only last 1000 entries
    if (pImpl->resourceHistory.size() > 1000) {
        pImpl->resourceHistory.erase(pImpl->resourceHistory.begin());
    }
}

void DebuggingSystem::recordMemoryAllocation(size_t size, const std::string& context) {
    if (!isActive()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    MemorySnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.operation = "allocation";
    snapshot.size = size;
    snapshot.context = context;
    
    pImpl->memoryHistory.push_back(snapshot);
    
    // Keep only last 1000 entries
    if (pImpl->memoryHistory.size() > 1000) {
        pImpl->memoryHistory.erase(pImpl->memoryHistory.begin());
    }
    
    notifyMemoryHooks("allocation", size);
}

void DebuggingSystem::recordMemoryDeallocation(size_t size, const std::string& context) {
    if (!isActive()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    MemorySnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.operation = "deallocation";
    snapshot.size = size;
    snapshot.context = context;
    
    pImpl->memoryHistory.push_back(snapshot);
    
    // Keep only last 1000 entries
    if (pImpl->memoryHistory.size() > 1000) {
        pImpl->memoryHistory.erase(pImpl->memoryHistory.begin());
    }
    
    notifyMemoryHooks("deallocation", size);
}

void DebuggingSystem::recordState(const std::string& stateName, const std::string& stateData) {
    if (trace_) {
        trace_->recordState(stateName, stateData);
    }
}

std::string DebuggingSystem::getState(const std::string& stateName) const {
    if (trace_) {
        return trace_->getState(stateName);
    }
    return "";
}

void DebuggingSystem::integrateWithBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
    
    if (config_.integrateWithBrain && isActive()) {
        // Register brain-specific hooks
        auto hook = std::make_shared<DebugHook>(
            [this](const DebugEvent& event) {
                // Handle brain-specific events
                if (event.getType() == DebugEventType::NeuronSpike || 
                    event.getType() == DebugEventType::SynapseFire) {
                    // Log neuron/synapse activity
                }
            },
            [this](const std::string& name, double value) {
                // Handle performance metrics
                if (name.find("neuron") != std::string::npos || 
                    name.find("synapse") != std::string::npos) {
                    recordPerformanceMetric(name, value);
                }
            },
            [this](const std::string& operation, size_t size) {
                // Handle memory events
                if (operation == "allocation" || operation == "deallocation") {
                    recordMemoryAllocation(size);
                }
            },
            [this](DebugStatus oldStatus, DebugStatus newStatus) {
                // Handle status changes
            }
        );
        addHook(hook);
    }
}

void DebuggingSystem::integrateWithAgent(std::shared_ptr<AgentBrain> agent) {
    pImpl->agentBrain = agent;
    
    if (config_.integrateWithAgent && isActive()) {
        // Register agent-specific hooks
        auto hook = std::make_shared<DebugHook>(
            [this](const DebugEvent& event) {
                // Handle agent-specific events
                if (event.getType() == DebugEventType::SensorInput || 
                    event.getType() == DebugEventType::MotorOutput) {
                    // Log agent input/output
                }
            },
            [this](const std::string& name, double value) {
                // Handle performance metrics
                if (name.find("sensor") != std::string::npos || 
                    name.find("motor") != std::string::npos) {
                    recordPerformanceMetric(name, value);
                }
            },
            [this](const std::string& operation, size_t size) {
                // Handle memory events
                if (operation == "allocation" || operation == "deallocation") {
                    recordMemoryAllocation(size);
                }
            },
            [this](DebugStatus oldStatus, DebugStatus newStatus) {
                // Handle status changes
            }
        );
        addHook(hook);
    }
}

void DebuggingSystem::integrateWithVisualization(std::shared_ptr<VisualizationInterface> visualization) {
    pImpl->visualization = visualization;
    
    if (config_.integrateWithVisualization && isActive()) {
        // Register visualization-specific hooks
        auto hook = std::make_shared<DebugHook>(
            [this](const DebugEvent& event) {
                // Handle visualization events
                if (pImpl->visualization) {
                    // Send events to visualization system
                }
            },
            [this](const std::string& name, double value) {
                // Handle performance metrics for visualization
                if (pImpl->visualization) {
                    // Update visualization with performance metrics
                }
            },
            [this](const std::string& operation, size_t size) {
                // Handle memory events
            },
            [this](DebugStatus oldStatus, DebugStatus newStatus) {
                // Handle status changes
                if (pImpl->visualization && newStatus == DebugStatus::Active) {
                    // Initialize visualization
                }
            }
        );
        addHook(hook);
    }
}

void DebuggingSystem::analyzeAnomalies() {
    if (!isActive()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->anomalyMutex);
    
    // Simple anomaly detection based on performance metrics
    for (const auto& pair : pImpl->performanceMetrics) {
        const auto& values = pair.second;
        if (values.size() < 5) continue;
        
        double sum = 0.0, sumSq = 0.0;
        for (double value : values) {
            sum += value;
            sumSq += value * value;
        }
        
        double mean = sum / values.size();
        double stddev = std::sqrt((sumSq / values.size()) - (mean * mean));
        
        if (stddev > mean * 2.0) { // If standard deviation is more than 2x mean
            std::ostringstream oss;
            oss << "Anomaly detected in " << pair.first << ": "
                << "mean=" << mean << ", stddev=" << stddev;
            
            pImpl->anomalyHistory.emplace_back(std::chrono::system_clock::now(), oss.str());
        }
    }
}

void DebuggingSystem::detectAnomalies() {
    if (!isActive()) return;
    
    analyzeAnomalies();
    
    // Log anomalies if any
    std::lock_guard<std::mutex> lock(pImpl->anomalyMutex);
    if (!pImpl->anomalyHistory.empty()) {
        for (const auto& anomaly : pImpl->anomalyHistory) {
            Logger::getGlobal().warning("Anomaly: " + anomaly.second);
        }
    }
}

bool DebuggingSystem::exportDebugData(const std::string& filepath) const {
    if (!trace_) return false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Export trace
        trace_->saveToFile(filepath + ".trace");
        
        // Export configuration
        std::ostringstream configOss;
        configOss << "Debug Configuration:\n";
        configOss << "  Enable Trace: " << (config_.enableTrace ? "Yes" : "No") << "\n";
        configOss << "  Max Trace Events: " << config_.maxTraceEvents << "\n";
        configOss << "  Enable Performance Profiling: " << (config_.enablePerformanceProfiling ? "Yes" : "No") << "\n";
        configOss << "  Enable Memory Tracking: " << (config_.enableMemoryTracking ? "Yes" : "No") << "\n";
        configOss << "  Enable Logging: " << (config_.enableLogging ? "Yes" : "No") << "\n";
        
        file << configOss.str();
        file << "\nPerformance Metrics:\n";
        for (const auto& pair : pImpl->performanceMetrics) {
            file << "  " << pair.first << ": " << getAveragePerformance(pair.first) << " (avg), "
                 << getMaxPerformance(pair.first) << " (max)\n";
        }
        
        file << "\nMemory History:\n";
        std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
        for (const auto& snapshot : pImpl->memoryHistory) {
            file << "  " << snapshot.timestamp.time_since_epoch().count() << ": "
                 << snapshot.operation << " size=" << snapshot.size 
                 << " context=" << snapshot.context << "\n";
        }
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string DebuggingSystem::exportDebugDataAsString() const {
    std::ostringstream oss;
    
    if (trace_) {
        oss << "Trace Data:\n" << trace_->serialize() << "\n";
    }
    
    oss << "\nConfiguration:\n";
    oss << "Status: " << static_cast<int>(status_) << "\n";
    
    oss << "\nPerformance Metrics:\n";
    for (const auto& pair : pImpl->performanceMetrics) {
        oss << "  " << pair.first << ": " << getAveragePerformance(pair.first) << " (avg), "
            << getMaxPerformance(pair.first) << " (max)\n";
    }
    
    return oss.str();
}

std::vector<DebugData> DebuggingSystem::getDebugData(const std::string& category) const {
    std::vector<DebugData> data;
    
    if (category == "trace") {
        if (trace_) {
            DebugData debugData("trace", "events", std::to_string(trace_->getEventCount()));
            data.push_back(debugData);
        }
    } else if (category == "performance") {
        for (const auto& pair : pImpl->performanceMetrics) {
            DebugData debugData(pair.first, "performance", std::to_string(getAveragePerformance(pair.first)));
            data.push_back(debugData);
        }
    } else if (category == "memory") {
        std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
        DebugData debugData("memory", "allocations", std::to_string(pImpl->memoryHistory.size()));
        data.push_back(debugData);
    }
    
    return data;
}

void DebuggingSystem::startRemoteDebugger() {
    if (pImpl->remoteDebuggerActive) return;
    
    pImpl->remoteDebuggerActive = true;
    pImpl->remoteDebuggerThread = std::thread([this]() {
        // Simulate remote debugger (placeholder implementation)
        while (pImpl->remoteDebuggerActive && isActive()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void DebuggingSystem::stopRemoteDebugger() {
    pImpl->remoteDebuggerActive = false;
    if (pImpl->remoteDebuggerThread.joinable()) {
        pImpl->remoteDebuggerThread.join();
    }
}

void DebuggingSystem::startNewSession(const std::string& sessionName) {
    if (trace_) {
        trace_->saveToFile(sessionName + ".trace");
    }
    
    pImpl->currentSessionName = sessionName;
    pImpl->sessionNames.push_back(sessionName);
    
    Logger::getGlobal().info("Started new debugging session: " + sessionName);
}

void DebuggingSystem::endCurrentSession() {
    Logger::getGlobal().info("Ended debugging session: " + pImpl->currentSessionName);
}

std::string DebuggingSystem::getCurrentSessionName() const {
    return pImpl->currentSessionName;
}

void DebuggingSystem::saveConfiguration(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) return;
        
        file << "enableTrace=" << (config_.enableTrace ? "1" : "0") << "\n";
        file << "maxTraceEvents=" << config_.maxTraceEvents << "\n";
        file << "enablePerformanceProfiling=" << (config_.enablePerformanceProfiling ? "1" : "0") << "\n";
        file << "profilingInterval=" << config_.profilingInterval << "\n";
        file << "enableMemoryTracking=" << (config_.enableMemoryTracking ? "1" : "0") << "\n";
        file << "detectMemoryLeaks=" << (config_.detectMemoryLeaks ? "1" : "0") << "\n";
        file << "memoryCheckInterval=" << config_.memoryCheckInterval << "\n";
        file << "enableLogging=" << (config_.enableLogging ? "1" : "0") << "\n";
        file << "logFile=" << config_.logFile << "\n";
        file << "enableConsoleOutput=" << (config_.enableConsoleOutput ? "1" : "0") << "\n";
        
        file.close();
    } catch (...) {
        // Ignore errors
    }
}

bool DebuggingSystem::loadConfiguration(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "enableTrace") config_.enableTrace = (value == "1");
            else if (key == "maxTraceEvents") config_.maxTraceEvents = std::stoi(value);
            else if (key == "enablePerformanceProfiling") config_.enablePerformanceProfiling = (value == "1");
            else if (key == "profilingInterval") config_.profilingInterval = std::stod(value);
            else if (key == "enableMemoryTracking") config_.enableMemoryTracking = (value == "1");
            else if (key == "detectMemoryLeaks") config_.detectMemoryLeaks = (value == "1");
            else if (key == "memoryCheckInterval") config_.memoryCheckInterval = std::stoi(value);
            else if (key == "enableLogging") config_.enableLogging = (value == "1");
            else if (key == "logFile") config_.logFile = value;
            else if (key == "enableConsoleOutput") config_.enableConsoleOutput = (value == "1");
        }
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

void DebuggingSystem::logStatus() const {
    std::ostringstream oss;
    oss << "Debugging System Status:\n";
    oss << "  Status: " << static_cast<int>(status_) << "\n";
    oss << "  Session: " << pImpl->currentSessionName << "\n";
    oss << "  Trace Events: " << (trace_ ? trace_->getEventCount() : 0) << "\n";
    oss << "  Performance Metrics: " << pImpl->performanceMetrics.size() << "\n";
    oss << "  Memory Snapshots: " << pImpl->memoryHistory.size() << "\n";
    oss << "  Anomalies Detected: " << pImpl->anomalyHistory.size() << "\n";
    
    if (config_.enableConsoleOutput) {
        Logger::getGlobal().info(oss.str());
    }
}

void DebuggingSystem::notifyHooks(const DebugEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    for (const auto& hook : pImpl->hooks) {
        try {
            hook->onDebugEvent(event);
        } catch (...) {
            // Ignore hook exceptions
        }
    }
}

void DebuggingSystem::notifyPerformanceHooks(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    for (const auto& hook : pImpl->hooks) {
        try {
            hook->onPerformanceMetric(name, value);
        } catch (...) {
            // Ignore hook exceptions
        }
    }
}

void DebuggingSystem::notifyMemoryHooks(const std::string& operation, size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    for (const auto& hook : pImpl->hooks) {
        try {
            hook->onMemoryEvent(operation, size);
        } catch (...) {
            // Ignore hook exceptions
        }
    }
}

void DebuggingSystem::notifyStatusChangeHooks(DebugStatus oldStatus, DebugStatus newStatus) {
    std::lock_guard<std::mutex> lock(pImpl->hooksMutex);
    for (const auto& hook : pImpl->hooks) {
        try {
            hook->onDebugStatusChange(oldStatus, newStatus);
        } catch (...) {
            // Ignore hook exceptions
        }
    }
}

} // namespace nlm
