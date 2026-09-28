// Copyright (c) 2026 NLM Project
// Memory tracking and leak detection implementation

#pragma once

#include "MemoryTracker.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

class MemoryTracker {
public:
    MemoryTracker() : currentMemoryUsage(0), peakMemoryUsage(0), trackingEnabled(false), leakDetectionEnabled(true), allocationThreshold(1024) {
        allocator = std::make_shared<std::unordered_map<void*, AllocationRecord>>();
        deallocatedAllocations = std::make_shared<std::unordered_set<void*>>();
    }
    
    ~MemoryTracker() {
        clearTracking();
    }
    
    void trackAllocation(void* pointer, size_t bytes, const std::string& context) {
        if (!trackingEnabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        AllocationRecord record;
        record.pointer = pointer;
        record.bytes = bytes;
        record.context = context;
        record.timestamp = std::chrono::system_clock::now();
        record.tracked = true;
        
        (*allocator)[pointer] = record;
        currentMemoryUsage += bytes;
        
        if (currentMemoryUsage > peakMemoryUsage) {
            peakMemoryUsage = currentMemoryUsage;
        }
        
        // Log allocation if logger is integrated
        if (loggerManager) {
            LogEntry entry;
            entry.level = LogLevel::Debug;
            entry.message = "Memory allocation: " + std::to_string(bytes) + " bytes";
            entry.module = "memory_tracker";
            entry.addMetric("allocation_bytes", static_cast<double>(bytes));
            entry.addTag("context", context);
            
            loggerManager->log(entry);
        }
    }
    
    void trackDeallocation(void* pointer) {
        if (!trackingEnabled) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = (*allocator).find(pointer);
        if (it != (*allocator).end()) {
            currentMemoryUsage -= it->second.bytes;
            
            // Check if it's a leak
            if (leakDetectionEnabled && it->second.tracked && 
                !deallocatedAllocations->count(pointer)) {
                // Check if pointer should be ignored
                bool shouldIgnore = false;
                for (const auto& pattern : ignorePatterns) {
                    if (it->second.context.find(pattern) != std::string::npos) {
                        shouldIgnore = true;
                        break;
                    }
                }
                
                if (!shouldIgnore) {
                    // Log as potential leak
                    if (loggerManager) {
                        LogEntry entry;
                        entry.level = LogLevel::Warning;
                        entry.message = "Potential memory leak detected: " + std::to_string(it->second.bytes) + " bytes not freed";
                        entry.module = "memory_tracker";
                        entry.addMetric("leak_bytes", static_cast<double>(it->second.bytes));
                        entry.addTag("context", it->second.context);
                        entry.addTag("timestamp", getCurrentTimestamp());
                        
                        loggerManager->log(entry);
                    }
                }
            }
            
            // Remove from allocator
            (*allocator).erase(it);
            deallocatedAllocations->insert(pointer);
        }
    }
    
    void trackReallocation(void* oldPointer, void* newPointer, size_t newSize, const std::string& context) {
        if (!trackingEnabled) return;
        
        trackDeallocation(oldPointer);
        trackAllocation(newPointer, newSize, context);
    }
    
    bool detectMemoryLeaks() const {
        if (!leakDetectionEnabled) return false;
        
        std::lock_guard<std::mutex> lock(mutex);
        size_t totalLeakSize = 0;
        
        for (const auto& pair : (*allocator)) {
            if (pair.second.tracked && !deallocatedAllocations->count(pair.first)) {
                bool shouldIgnore = false;
                for (const auto& pattern : ignorePatterns) {
                    if (pair.second.context.find(pattern) != std::string::npos) {
                        shouldIgnore = true;
                        break;
                    }
                }
                
                if (!shouldIgnore) {
                    totalLeakSize += pair.second.bytes;
                }
            }
        }
        
        return totalLeakSize >= allocationThreshold;
    }
    
    std::vector<std::pair<void*, size_t>> getMemoryLeaks() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::pair<void*, size_t>> leaks;
        
        for (const auto& pair : (*allocator)) {
            if (pair.second.tracked && !deallocatedAllocations->count(pair.first)) {
                bool shouldIgnore = false;
                for (const auto& pattern : ignorePatterns) {
                    if (pair.second.context.find(pattern) != std::string::npos) {
                        shouldIgnore = true;
                        break;
                    }
                }
                
                if (!shouldIgnore) {
                    leaks.push_back({pair.first, pair.second.bytes});
                }
            }
        }
        
        return leaks;
    }
    
    std::vector<std::pair<std::string, size_t>> getMemoryLeaksByContext() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::map<std::string, size_t> leaksByContext;
        
        for (const auto& pair : (*allocator)) {
            if (pair.second.tracked && !deallocatedAllocations->count(pair.first)) {
                bool shouldIgnore = false;
                for (const auto& pattern : ignorePatterns) {
                    if (pair.second.context.find(pattern) != std::string::npos) {
                        shouldIgnore = true;
                        break;
                    }
                }
                
                if (!shouldIgnore) {
                    leaksByContext[pair.second.context] += pair.second.bytes;
                }
            }
        }
        
        std::vector<std::pair<std::string, size_t>> result;
        for (const auto& pair : leaksByContext) {
            result.push_back(pair);
        }
        
        return result;
    }
    
    void updateMemoryUsage() {
        // This would typically call system functions to get actual memory usage
        // For now, it's a placeholder
        static size_t lastUsage = 0;
        static std::chrono::system_clock::time_point lastUpdate = std::chrono::system_clock::now();
        
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count();
        
        if (elapsed >= 1) { // Update every second
            // Simulate memory usage update
            currentMemoryUsage = peakMemoryUsage; // Simplified
            lastUpdate = now;
        }
    }
    
    uint64_t getCurrentMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex);
        return currentMemoryUsage;
    }
    
    uint64_t getPeakMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex);
        return peakMemoryUsage;
    }
    
    void integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager) {
        std::lock_guard<std::mutex> lock(mutex);
        this->loggerManager = loggerManager;
    }
    
    void integrateWithDebugConsole(std::shared_ptr<IDebugConsole> console) {
        std::lock_guard<std::mutex> lock(mutex);
        this->debugConsole = console;
    }
    
    void integrateWithSystemMonitor(std::shared_ptr<ISystemMonitor> monitor) {
        std::lock_guard<std::mutex> lock(mutex);
        this->systemMonitor = monitor;
    }
    
    void setLeakDetectionEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex);
        leakDetectionEnabled = enabled;
    }
    
    void setAllocationTrackingEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex);
        trackingEnabled = enabled;
    }
    
    void setThresholdForLeakDetection(size_t minBytes) {
        std::lock_guard<std::mutex> lock(mutex);
        allocationThreshold = minBytes;
    }
    
    void addIgnorePattern(const std::string& pattern) {
        std::lock_guard<std::mutex> lock(mutex);
        ignorePatterns.insert(pattern);
    }
    
    void removeIgnorePattern(const std::string& pattern) {
        std::lock_guard<std::mutex> lock(mutex);
        ignorePatterns.erase(pattern);
    }
    
    void exportMemoryReport(const std::string& filepath) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        file << "Memory Report\n";
        file << "============\n\n";
        file << "Current Memory Usage: " << currentMemoryUsage << " bytes\n";
        file << "Peak Memory Usage: " << peakMemoryUsage << " bytes\n\n";
        
        file << "Active Allocations:\n";
        for (const auto& pair : (*allocator)) {
            const auto& record = pair.second;
            file << "  Pointer: " << record.pointer << "\n";
            file << "    Context: " << record.context << "\n";
            file << "    Size: " << record.bytes << " bytes\n";
            file << "    Timestamp: " << getCurrentTimestamp(record.timestamp) << "\n\n";
        }
        
        file << "Deallocated Allocations: " << deallocatedAllocations->size() << "\n\n";
        
        if (detectMemoryLeaks()) {
            file << "MEMORY LEAKS DETECTED:\n";
            auto leaks = getMemoryLeaks();
            for (const auto& leak : leaks) {
                file << "  Pointer: " << leak.first << "\n";
                file << "    Size: " << leak.second << " bytes\n";
            }
        } else {
            file << "No memory leaks detected.\n";
        }
    }
    
    void exportAllocationTimeline(const std::string& filepath) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        file << "Timestamp,Pointer,Context,Size,Status\n";
        
        for (const auto& pair : (*allocator)) {
            const auto& record = pair.second;
            file << getCurrentTimestamp(record.timestamp) << ",";
            file << record.pointer << ",";
            file << record.context << ",";
            file << record.bytes << ",";
            file << "ACTIVE\n";
        }
    }
    
    void clearTracking() {
        std::lock_guard<std::mutex> lock(mutex);
        (*allocator).clear();
        deallocatedAllocations->clear();
        currentMemoryUsage = 0;
        peakMemoryUsage = 0;
        
        // Log clearing
        if (loggerManager) {
            LogEntry entry;
            entry.level = LogLevel::Info;
            entry.message = "Memory tracking cleared";
            entry.module = "memory_tracker";
            
            loggerManager->log(entry);
        }
    }
    
private:
    struct AllocationRecord {
        void* pointer;
        size_t bytes;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        bool tracked;
    };
    
    mutable std::mutex mutex;
    std::shared_ptr<std::unordered_map<void*, AllocationRecord>> allocator;
    std::shared_ptr<std::unordered_set<void*>> deallocatedAllocations;
    uint64_t currentMemoryUsage;
    uint64_t peakMemoryUsage;
    bool trackingEnabled;
    bool leakDetectionEnabled;
    size_t allocationThreshold;
    std::unordered_set<std::string> ignorePatterns;
    
    std::shared_ptr<LoggerManager> loggerManager;
    std::shared_ptr<IDebugConsole> debugConsole;
    std::shared_ptr<ISystemMonitor> systemMonitor;
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getCurrentTimestamp(const std::chrono::system_clock::time_point& time) const {
        auto time_t = std::chrono::system_clock::to_time_t(time);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};