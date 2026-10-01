#include "VisualizationAPI.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <numeric>

namespace nlm {

struct VisualizationAPI::Impl {
    bool active;
    bool realTimeVisualizationEnabled;
    bool realTimeMonitoringEnabled;
    double visualizationFPS;
    std::shared_ptr<VisualizationCore> visualizationCore;
    std::shared_ptr<DebugConsole> debugConsole;
    std::shared_ptr<DataAnalyzer> dataAnalyzer;
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    std::shared_ptr<Brain> brain;
    std::chrono::steady_clock::timePoint lastUpdateTime;
    double totalUpdateTime;
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    std::vector<std::string> infoMessages;
    std::string logLevel;
    std::string profileTarget;
    std::string configDirectory;
    std::unordered_map<std::string, std::string> config;
    std::unordered_map<std::string, std::function<void(const Brain&)>> customVisualizations;
    std::unordered_map<std::string, double> componentUpdateRates;
    bool shouldExit;
    std::chrono::steady_clock::time_point startTime;
    
    Impl() : active(false), realTimeVisualizationEnabled(false), realTimeMonitoringEnabled(false),
             visualizationFPS(60.0), totalUpdateTime(0.0), logLevel("info"),
             shouldExit(false), startTime(std::chrono::steady_clock::now()) {
        std::filesystem::path homeDir = std::filesystem::path(std::getenv("HOME"));
        configDirectory = (homeDir / ".nlm").string();
        std::filesystem::create_directories(configDirectory);
    }
};

VisualizationAPI::VisualizationAPI() : pImpl(std::make_unique<Impl>()) {}

VisualizationAPI::~VisualizationAPI() = default;

bool VisualizationAPI::initialize() {
    // Initialize all components
    pImpl->active = true;
    
    // Initialize visualization core
    pImpl->visualizationCore = std::make_shared<VisualizationCore>();
    if (!pImpl->visualizationCore->initialize()) {
        NLM_LOG_ERROR("VisualizationAPI: Failed to initialize visualization core");
        return false;
    }
    
    // Initialize debug console
    pImpl->debugConsole = std::make_shared<DebugConsole>();
    if (!pImpl->debugConsole->initialize()) {
        NLM_LOG_ERROR("VisualizationAPI: Failed to initialize debug console");
        return false;
    }
    
    // Initialize data analyzer
    pImpl->dataAnalyzer = std::make_shared<DataAnalyzer>();
    if (!pImpl->dataAnalyzer->initialize()) {
        NLM_LOG_ERROR("VisualizationAPI: Failed to initialize data analyzer");
        return false;
    }
    
    // Initialize performance profiler
    pImpl->performanceProfiler = std::make_shared<PerformanceProfiler>();
    if (!pImpl->performanceProfiler->initialize()) {
        NLM_LOG_ERROR("VisualizationAPI: Failed to initialize performance profiler");
        return false;
    }
    
    // Set brain if available
    if (pImpl->brain) {
        pImpl->debugConsole->setBrain(pImpl->brain);
    }
    
    NLM_LOG_INFO("VisualizationAPI: Initialized successfully");
    return true;
}

void VisualizationAPI::close() {
    if (!pImpl->active) return;
    
    // Close all components
    if (pImpl->visualizationCore) {
        pImpl->visualizationCore->close();
    }
    
    pImpl->active = false;
    NLM_LOG_INFO("VisualizationAPI: Closed successfully");
}

bool VisualizationAPI::isActive() const {
    return pImpl->active;
}

std::shared_ptr<VisualizationCore> VisualizationAPI::getVisualizationCore() const {
    return pImpl->visualizationCore;
}

std::shared_ptr<DebugConsole> VisualizationAPI::getDebugConsole() const {
    return pImpl->debugConsole;
}

std::shared_ptr<DataAnalyzer> VisualizationAPI::getDataAnalyzer() const {
    return pImpl->dataAnalyzer;
}

std::shared_ptr<PerformanceProfiler> VisualizationAPI::getPerformanceProfiler() const {
    return pImpl->performanceProfiler;
}

std::shared_ptr<Brain> VisualizationAPI::getBrain() const {
    return pImpl->brain;
}

void VisualizationAPI::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
    if (pImpl->debugConsole) {
        pImpl->debugConsole->setBrain(brain);
    }
}

void VisualizationAPI::startRealTimeVisualization(double fps) {
    pImpl->realTimeVisualizationEnabled = true;
    pImpl->visualizationFPS = fps;
    if (pImpl->visualizationCore) {
        pImpl->visualizationCore->setUpdateRate(fps);
    }
    NLM_LOG_INFO("VisualizationAPI: Started real-time visualization at " + std::to_string(fps) + " FPS");
}

void VisualizationAPI::stopRealTimeVisualization() {
    pImpl->realTimeVisualizationEnabled = false;
    NLM_LOG_INFO("VisualizationAPI: Stopped real-time visualization");
}

void VisualizationAPI::setRealTimeVisualizationEnabled(bool enabled) {
    if (enabled) {
        startRealTimeVisualization();
    } else {
        stopRealTimeVisualization();
    }
}

bool VisualizationAPI::isRealTimeVisualizationEnabled() const {
    return pImpl->realTimeVisualizationEnabled;
}

void VisualizationAPI::startRealTimeMonitoring() {
    pImpl->realTimeMonitoringEnabled = true;
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startRealTimeMonitoring();
    }
    NLM_LOG_INFO("VisualizationAPI: Started real-time monitoring");
}

void VisualizationAPI::stopRealTimeMonitoring() {
    pImpl->realTimeMonitoringEnabled = false;
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->stopRealTimeMonitoring();
    }
    NLM_LOG_INFO("VisualizationAPI: Stopped real-time monitoring");
}

void VisualizationAPI::setRealTimeMonitoringEnabled(bool enabled) {
    if (enabled) {
        startRealTimeMonitoring();
    } else {
        stopRealTimeMonitoring();
    }
}

bool VisualizationAPI::isRealTimeMonitoringEnabled() const {
    return pImpl->realTimeMonitoringEnabled;
}

void VisualizationAPI::update(double deltaTime) {
    if (!pImpl->active) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Update all components if brain is available
    if (pImpl->brain) {
        pImpl->dataAnalyzer->processBrainData(*pImpl->brain);
        
        if (pImpl->realTimeVisualizationEnabled) {
            if (pImpl->visualizationCore) {
                pImpl->visualizationCore->update(*pImpl->brain, deltaTime);
            }
        }
    }
    
    // Process debug console input
    if (pImpl->debugConsole && pImpl->debugConsole->getExecutionState() == DebugConsole::ExecutionState::Paused) {
        // Wait for user input in paused state
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->totalUpdateTime = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->lastUpdateTime = endTime;
}

void VisualizationAPI::render() {
    if (!pImpl->active) return;
    
    // Render visualizations
    if (pImpl->visualizationCore) {
        pImpl->visualizationCore->render();
    }
    
    // Update performance profiler
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->updateFPS();
    }
}

bool VisualizationAPI::processDebugInput(const std::string& command) {
    if (!pImpl->debugConsole) return false;
    
    return pImpl->debugConsole->processInput(command);
}

bool VisualizationAPI::exportAllData(const std::string& directory, const std::vector<std::string>& formats) {
    bool success = true;
    
    // Export data in specified formats
    for (const auto& format : formats) {
        if (!pImpl->dataAnalyzer->exportData(directory + "/brain_data." + format, format)) {
            NLM_LOG_ERROR("VisualizationAPI: Failed to export data in format '" + format + "'");
            success = false;
        }
        
        if (!pImpl->dataAnalyzer->exportStatistics(directory + "/statistics." + format, format)) {
            NLM_LOG_ERROR("VisualizationAPI: Failed to export statistics in format '" + format + "'");
            success = false;
        }
    }
    
    return success;
}

bool VisualizationAPI::importData(const std::string& directory, const std::string& format) {
    NLM_LOG_INFO("VisualizationAPI: Importing data from " + directory + " in format " + format);
    // Import implementation would go here
    return true;
}

std::string VisualizationAPI::getStatus() const {
    std::stringstream ss;
    ss << "VisualizationAPI Status:\n";
    ss << "  Active: " << (pImpl->active ? "Yes" : "No") << "\n";
    ss << "  Brain: " << (pImpl->brain ? "Yes" : "No") << "\n";
    ss << "  Real-time Visualization: " << (pImpl->realTimeVisualizationEnabled ? "Yes" : "No") << "\n";
    ss << "  Real-time Monitoring: " << (pImpl->realTimeMonitoringEnabled ? "Yes" : "No") << "\n";
    ss << "  Update Time: " << pImpl->totalUpdateTime << " seconds\n";
    ss << "  Data Points: " << (pImpl->dataAnalyzer ? std::to_string(pImpl->dataAnalyzer->getDataPointCount()) : "0") << "\n";
    return ss.str();
}

std::string VisualizationAPI::getPerformanceStats() const {
    std::stringstream ss;
    ss << "Performance Statistics:\n";
    
    if (pImpl->performanceProfiler) {
        ss << "  Total Execution Time: " << pImpl->performanceProfiler->getTotalExecutionTime() << " seconds\n";
        ss << "  Memory Usage: " << pImpl->performanceProfiler->getMemoryUsage() << " bytes\n";
        ss << "  CPU Usage: " << pImpl->performanceProfiler->getCPUUsage() << "%\n";
        ss << "  FPS: " << pImpl->performanceProfiler->getFPS() << "\n";
    }
    
    ss << "  Last Update Time: " << pImpl->totalUpdateTime << " seconds\n";
    return ss.str();
}

std::string VisualizationAPI::getSystemInfo() const {
    std::stringstream ss;
    ss << "System Information:\n";
    ss << "  Version: NLM 0.1.0\n";
    ss << "  Build Type: Release\n";
    ss << "  C++ Standard: C++20\n";
    ss << "  Platform: Linux\n";
    ss << "  Uptime: " << getUptime() << " seconds\n";
    return ss.str();
}

void VisualizationAPI::enableVisualizationComponent(const std::string& componentName) {
    if (pImpl->visualizationCore) {
        pImpl->visualizationCore->enableVisualization(componentName);
    }
}

void VisualizationAPI::disableVisualizationComponent(const std::string& componentName) {
    if (pImpl->visualizationCore) {
        pImpl->visualizationCore->disableVisualization(componentName);
    }
}

bool VisualizationAPI::isVisualizationComponentEnabled(const std::string& componentName) const {
    if (pImpl->visualizationCore) {
        return pImpl->visualizationCore->isVisualizationEnabled(componentName);
    }
    return false;
}

bool VisualizationAPI::registerCustomVisualization(const std::string& name, 
                                                    std::function<void(const Brain&)> visualizationFunc) {
    if (!visualizationFunc) {
        NLM_LOG_ERROR("VisualizationAPI: Invalid visualization function");
        return false;
    }
    
    pImpl->customVisualizations[name] = visualizationFunc;
    NLM_LOG_INFO("VisualizationAPI: Registered custom visualization '" + name + "'");
    return true;
}

bool VisualizationAPI::unregisterCustomVisualization(const std::string& name) {
    auto it = pImpl->customVisualizations.find(name);
    if (it != pImpl->customVisualizations.end()) {
        pImpl->customVisualizations.erase(it);
        NLM_LOG_INFO("VisualizationAPI: Unregistered custom visualization '" + name + "'");
        return true;
    }
    return false;
}

std::vector<std::string> VisualizationAPI::getCustomVisualizations() const {
    std::vector<std::string> result;
    for (const auto& pair : pImpl->customVisualizations) {
        result.push_back(pair.first);
    }
    return result;
}

void VisualizationAPI::setUpdateRate(const std::string& componentName, double hz) {
    pImpl->componentUpdateRates[componentName] = hz;
}

double VisualizationAPI::getUpdateRate(const std::string& componentName) const {
    auto it = pImpl->componentUpdateRates.find(componentName);
    return (it != pImpl->componentUpdateRates.end()) ? it->second : 0.0;
}

bool VisualizationAPI::hasComponentData(const std::string& componentName) const {
    // Check if any component has data
    if (componentName == "brain") {
        return pImpl->brain != nullptr;
    }
    else if (componentName == "visualization") {
        return pImpl->visualizationCore != nullptr && pImpl->visualizationCore->isActive();
    }
    else if (componentName == "debug") {
        return pImpl->debugConsole != nullptr && pImpl->debugConsole->isActive();
    }
    else if (componentName == "analysis") {
        return pImpl->dataAnalyzer != nullptr;
    }
    else if (componentName == "profiler") {
        return pImpl->performanceProfiler != nullptr && pImpl->performanceProfiler->isActive();
    }
    return false;
}

std::string VisualizationAPI::getComponentData(const std::string& componentName) const {
    if (componentName == "brain" && pImpl->brain) {
        return "Brain with " + std::to_string(pImpl->brain->getTotalNeuronCount()) + " neurons";
    }
    else if (componentName == "visualization" && pImpl->visualizationCore) {
        return "VisualizationCore active: " + std::to_string(pImpl->visualizationCore->isActive());
    }
    else if (componentName == "debug" && pImpl->debugConsole) {
        return "DebugConsole active: " + std::to_string(pImpl->debugConsole->isActive());
    }
    else if (componentName == "analysis" && pImpl->dataAnalyzer) {
        return "DataAnalyzer has " + std::to_string(pImpl->dataAnalyzer->getDataPointCount()) + " data points";
    }
    else if (componentName == "profiler" && pImpl->performanceProfiler) {
        return "PerformanceProfiler active: " + std::to_string(pImpl->performanceProfiler->isActive());
    }
    return "Component not found: " + componentName;
}

void VisualizationAPI::setConfig(const std::string& key, const std::string& value) {
    pImpl->config[key] = value;
}

std::string VisualizationAPI::getConfig(const std::string& key) const {
    auto it = pImpl->config.find(key);
    return (it != pImpl->config.end()) ? it->second : "";
}

bool VisualizationAPI::saveConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    for (const auto& pair : pImpl->config) {
        file << pair.first << "=" << pair.second << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("VisualizationAPI: Saved config to " + filename);
    return true;
}

bool VisualizationAPI::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    pImpl->config.clear();
    std::string key, value, line;
    
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
            pImpl->config[key] = value;
        }
    }
    
    file.close();
    NLM_LOG_INFO("VisualizationAPI: Loaded config from " + filename);
    return true;
}

void VisualizationAPI::resetConfig() {
    pImpl->config.clear();
    NLM_LOG_INFO("VisualizationAPI: Reset to default configuration");
}

bool VisualizationAPI::hasErrors() const {
    return !pImpl->errorMessages.empty();
}

std::vector<std::string> VisualizationAPI::getErrorMessages() const {
    return pImpl->errorMessages;
}

void VisualizationAPI::clearErrors() {
    pImpl->errorMessages.clear();
}

std::vector<std::string> VisualizationAPI::getWarningMessages() const {
    return pImpl->warningMessages;
}

void VisualizationAPI::clearWarnings() {
    pImpl->warningMessages.clear();
}

std::vector<std::string> VisualizationAPI::getInfoMessages() const {
    return pImpl->infoMessages;
}

void VisualizationAPI::clearInfoMessages() {
    pImpl->infoMessages.clear();
}

void VisualizationAPI::logMessage(const std::string& message, const std::string& level) {
    if (level == "error") {
        pImpl->errorMessages.push_back(message);
    } else if (level == "warning") {
        pImpl->warningMessages.push_back(message);
    } else {
        pImpl->infoMessages.push_back(message);
    }
    
    NLM_LOG_INFO(message);
}

void VisualizationAPI::setLogLevel(const std::string& level) {
    pImpl->logLevel = level;
}

std::string VisualizationAPI::getLogLevel() const {
    return pImpl->logLevel;
}

std::vector<std::string> VisualizationAPI::getAvailableMetrics() const {
    return {"totalNeuronCount", "totalSpikeCount", "activeNeuronCount", "firingNeuronCount", 
            "averageFiringRate", "eIratio", "workingMemory", "episodicMemory", 
            "dopamineLevel", "developmentalStage"};
}

std::vector<std::string> VisualizationAPI::getAvailableVisualizations() const {
    return {"network", "activity", "spikes", "weights", "state_space"};
}

std::vector<std::string> VisualizationAPI::getAvailableDebugCommands() const {
    return {"help", "status", "watch", "break", "continue", "step", "next", "back", "stack", "eval", "var", "clear"};
}

std::vector<std::string> VisualizationAPI::getAvailableDataFormats() const {
    return {"csv", "json", "hdf5", "parquet"};
}

std::vector<std::string> VisualizationAPI::getAvailableExportFormats() const {
    return {"csv", "json", "hdf5", "parquet", "png", "jpg", "pdf", "svg"};
}

std::string VisualizationAPI::getSystemHealth() const {
    std::stringstream ss;
    ss << "System Health:\n";
    ss << "  Active Components: " << (pImpl->active ? "All" : "None") << "\n";
    ss << "  Error Count: " << pImpl->errorMessages.size() << "\n";
    ss << "  Warning Count: " << pImpl->warningMessages.size() << "\n";
    ss << "  Info Count: " << pImpl->infoMessages.size() << "\n";
    ss << "  Uptime: " << getUptime() << " seconds\n";
    return ss.str();
}

VisualizationAPI::ResourceUsage VisualizationAPI::getResourceUsage() const {
    ResourceUsage usage;
    // Mock resource usage - in real implementation would query system
    usage.cpuUsage = 50.0;  // 50%
    usage.memoryUsage = 512;  // 512 MB
    usage.networkUsage = 10.0;  // 10 MB/s
    usage.diskUsage = 25.0;  // 25%
    return usage;
}

double VisualizationAPI::getUptime() const {
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration<double>(now - pImpl->startTime).count();
    return uptime;
}

std::string VisualizationAPI::getVersion() const {
    return "NLM 0.1.0";
}

std::string VisualizationAPI::getBuildInfo() const {
    return "Release Build (C++20)";
}

std::string VisualizationAPI::getCompilationInfo() const {
    return "GCC 11.4.0 / Clang 14.0.0";
}

std::string VisualizationAPI::getLicenseInfo() const {
    return "MIT License";
}

std::string VisualizationAPI::getCopyrightInfo() const {
    return "Copyright (c) 2026 NLM Project";
}

std::string VisualizationAPI::getHelp() const {
    return "VisualizationAPI Help:\n"
           "  Available commands: status, update, render, export, import, help\n"
           "  Use processDebugInput() for debug commands\n"
           "  Use getAvailableVisualizations() to see available visualizations\n"
           "  Use getAvailableMetrics() to see available metrics\n";
}

std::string VisualizationAPI::getAbout() const {
    return "NLM VisualizationAPI\n"
           "Phase 2: Real Neural Computation\n"
           "Comprehensive visualization and debugging system for neural networks";
}

void VisualizationAPI::exit() {
    pImpl->shouldExit = true;
    NLM_LOG_INFO("VisualizationAPI: Exit requested");
}

void VisualizationAPI::restart() {
    NLM_LOG_INFO("VisualizationAPI: Restart requested");
    // Restart implementation would go here
}

bool VisualizationAPI::shouldExit() const {
    return pImpl->shouldExit;
}

} // namespace nlm