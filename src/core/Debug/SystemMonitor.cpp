// Copyright (c) 2026 NLM Project
// Implementation of System Monitor

#pragma once

#include "SystemMonitor.hpp"
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

SystemMonitor::SystemMonitor() : monitoringEnabled(false) {
    // Initialize components
    for (const auto& component : {"brain", "environment", "sensory", "motor", "memory", "plasticity", "neuromodulation", "world"}) {
        componentStatus[component] = SystemStatus(component, "unknown", "Component not initialized");
    }
    
    startMonitoring();
}

SystemMonitor::~SystemMonitor() {
    stopMonitoring();
}

void SystemMonitor::update() {
    if (!monitoringEnabled) return;
    
    // Update CPU and memory usage
    updateCpuUsage(cpuInfo);
    updateMemoryUsage(memoryInfo);
    updateThreadUsage(threads);
    
    // Collect system metrics
    collectSystemMetrics();
    
    // Update logger if integrated
    if (loggerManager) {
        // Log system metrics
    }
    
    // Update debug console if integrated
    if (debugConsole) {
        // Update debug console
    }
}

SystemStatus SystemMonitor::getStatus(const std::string& component) const {
    std::lock_guard<std::mutex> lock(statusMutex);
    auto it = componentStatus.find(component);
    if (it != componentStatus.end()) {
        return it->second;
    }
    return SystemStatus(component, "unknown", "Component not found");
}

std::vector<SystemStatus> SystemMonitor::getAllStatus() const {
    std::lock_guard<std::mutex> lock(statusMutex);
    std::vector<SystemStatus> result;
    for (const auto& pair : componentStatus) {
        result.push_back(pair.second);
    }
    return result;
}

MemoryInfo SystemMonitor::getMemoryInfo() const {
    std::lock_guard<std::mutex> lock(memoryMutex);
    return memoryInfo;
}

CpuInfo SystemMonitor::getCpuInfo() const {
    std::lock_guard<std::mutex> lock(cpuMutex);
    return cpuInfo;
}

std::vector<ThreadInfo> SystemMonitor::getThreadInfo() const {
    std::lock_guard<std::mutex> lock(threadMutex);
    return threads;
}

void SystemMonitor::addCustomMetric(const std::string& name, double value, const std::string& unit) {
    std::lock_guard<std::mutex> lock(metricMutex);
    customMetrics[name] = {name, value, unit, std::chrono::system_clock::now()};
}

void SystemMonitor::setMonitoringEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(monitoringMutex);
    monitoringEnabled = enabled;
}

bool SystemMonitor::isMonitoringEnabled() const {
    std::lock_guard<std::mutex> lock(monitoringMutex);
    return monitoringEnabled;
}

void SystemMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(monitoringMutex);
    if (!monitoringEnabled) {
        monitoringEnabled = true;
        monitoringThread = std::thread(&SystemMonitor::monitoringThreadFunction, this);
    }
}

void SystemMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(monitoringMutex);
    monitoringEnabled = false;
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
}

void SystemMonitor::monitoringThreadFunction() {
    while (monitoringEnabled) {
        update();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SystemMonitor::updateCpuUsage(CpuInfo& cpu) const {
    // Simplified CPU usage update
    // In a real implementation, this would use system APIs
    cpu.usage = 0.0; // Placeholder
    cpu.timestamp = std::chrono::system_clock::now();
}

void SystemMonitor::updateMemoryUsage(MemoryInfo& memory) const {
    // Simplified memory usage update
    // In a real implementation, this would use system APIs
    memory.used = 0; // Placeholder
    memory.total = 0; // Placeholder
    memory.percent = 0.0; // Placeholder
    memory.timestamp = std::chrono::system_clock::now();
}

void SystemMonitor::updateThreadUsage(std::vector<ThreadInfo>& threads) const {
    // Simplified thread usage update
    // In a real implementation, this would use system APIs
    threads.clear(); // Placeholder
}

void SystemMonitor::collectSystemMetrics() {
    // Collect system metrics (simplified)
    // In a real implementation, this would collect various system metrics
}

void SystemMonitor::integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager) {
    std::lock_guard<std::mutex> lock(loggerIntegrationMutex);
    this->loggerManager = loggerManager;
}

void SystemMonitor::integrateWithDebugConsole(std::shared_ptr<IDebugConsole> console) {
    std::lock_guard<std::mutex> lock(consoleIntegrationMutex);
    this->debugConsole = console;
}

void SystemMonitor::recordOperation(const std::string& operation, double duration, const std::string& unit) {
    std::lock_guard<std::mutex> lock(metricMutex);
    operationTimes[operation].push_back({duration, unit, std::chrono::system_clock::now()});
}

void SystemMonitor::recordMemoryAllocation(uint64_t bytes) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    totalAllocations += bytes;
    lastAllocationTime = std::chrono::system_clock::now();
}

void SystemMonitor::recordNetworkTransfer(uint64_t bytes, const std::string& direction) {
    std::lock_guard<std::mutex> lock(networkMutex);
    if (direction == "up") {
        totalBytesSent += bytes;
    } else if (direction == "down") {
        totalBytesReceived += bytes;
    }
    lastNetworkTime = std::chrono::system_clock::now();
}

void SystemMonitor::recordDiskIO(uint64_t bytes, const std::string& operation) {
    std::lock_guard<std::mutex> lock(diskMutex);
    if (operation == "read") {
        totalBytesRead += bytes;
    } else if (operation == "write") {
        totalBytesWritten += bytes;
    }
    lastDiskTime = std::chrono::system_clock::now();
}

void SystemMonitor::updateComponentStatus(const std::string& component, const std::string& status, const std::string& message) {
    std::lock_guard<std::mutex> lock(statusMutex);
    componentStatus[component] = SystemStatus(component, status, message);
}

void SystemMonitor::removeComponentStatus(const std::string& component) {
    std::lock_guard<std::mutex> lock(statusMutex);
    componentStatus.erase(component);
}

bool SystemMonitor::runHealthCheck(const std::string& component) {
    // Run health check for component (simplified)
    // In a real implementation, this would run specific health checks
    std::lock_guard<std::mutex> lock(statusMutex);
    auto it = componentStatus.find(component);
    if (it != componentStatus.end()) {
        return it->second.status == "ok";
    }
    return false;
}

std::vector<std::string> SystemMonitor::getFailingComponents() const {
    std::lock_guard<std::mutex> lock(statusMutex);
    std::vector<std::string> failing;
    for (const auto& pair : componentStatus) {
        if (pair.second.status != "ok") {
            failing.push_back(pair.first);
        }
    }
    return failing;
}