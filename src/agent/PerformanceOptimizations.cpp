// Simplified performance optimizations for the agent system
// Focus on practical improvements rather than complex infrastructure

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <optional>
#include "AgentBrain.hpp"
#include "AgentConfig.hpp"

namespace nlm {

// Simplified command cache for improved performance
class SimpleCommandCache {
private:
    struct CacheEntry {
        std::string commandLine;
        std::string result;
        std::chrono::steady_clock::time_point timestamp;
        bool isValid;
    };
    
    std::unordered_map<std::string, CacheEntry> cache_;
    size_t maxCacheSize_;
    std::chrono::seconds cacheTimeout_;
    
public:
    SimpleCommandCache(size_t maxSize = 100, 
                       std::chrono::seconds timeout = std::chrono::seconds(30))
        : maxCacheSize_(maxSize), cacheTimeout_(timeout) {}
    
    bool getCachedCommand(const std::string& commandLine, std::string& result) {
        auto it = cache_.find(commandLine);
        if (it == cache_.end()) return false;
        
        auto& entry = it->second;
        auto now = std::chrono::steady_clock::now();
        
        // Check if cache entry is valid
        if (entry.isValid && 
            std::chrono::duration_cast<std::chrono::seconds>(now - entry.timestamp) <= cacheTimeout_) {
            result = entry.result;
            return true;
        }
        
        return false;
    }
    
    void cacheCommand(const std::string& commandLine, const std::string& result, bool isValid = true) {
        // Remove existing entry
        cache_.erase(commandLine);
        
        // Evict if cache is full
        if (cache_.size() >= maxCacheSize_) {
            cache_.clear();
        }
        
        cache_[commandLine] = {commandLine, result, std::chrono::steady_clock::now(), isValid};
    }
    
    size_t getCacheSize() const { return cache_.size(); }
    void clear() { cache_.clear(); }
};

// Optimized agent executor with command batching
class OptimizedAgentExecutor {
private:
    SimpleCommandCache commandCache_;
    bool enableCaching_;
    
public:
    OptimizedAgentExecutor(bool enableCaching = true) : enableCaching_(enableCaching) {}
    
    bool executeCommand(AgentBrain& agent, const std::string& commandLine) {
        // Try cache first
        std::string cachedResult;
        if (enableCaching_ && commandCache_.getCachedCommand(commandLine, cachedResult)) {
            return true;
        }
        
        // Execute command
        bool success = agent.processCommand(commandLine);
        
        // Cache result if successful
        if (enableCaching_ && success) {
            commandCache_.cacheCommand(commandLine, "Command executed successfully");
        }
        
        return success;
    }
    
    std::vector<bool> executeBatch(AgentBrain& agent, const std::vector<std::string>& commands) {
        std::vector<bool> results;
        results.reserve(commands.size());
        
        for (const auto& command : commands) {
            results.push_back(executeCommand(agent, command));
        }
        
        return results;
    }
    
    void clearCache() { commandCache_.clear(); }
    size_t getCacheSize() const { return commandCache_.getCacheSize(); }
};

// Optimized command parser with better performance
class OptimizedCommandParser {
public:
    std::vector<std::string> parseCommandLine(const std::string& commandLine) {
        std::vector<std::string> tokens;
        std::string token;
        
        for (char c : commandLine) {
            if (std::isspace(static_cast<unsigned char>(c))) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
};

// Simplified performance monitoring
class PerformanceMonitor {
private:
    struct Metrics {
        size_t commandCount;
        double avgExecutionTime;
        size_t cacheHits;
        size_t cacheMisses;
        double uptimePercentage;
    };
    
    Metrics metrics_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastCommandTime_;
    
public:
    PerformanceMonitor() {
        resetMetrics();
    }
    
    void recordCommand(bool cacheHit = false) {
        metrics_.commandCount++;
        if (cacheHit) {
            metrics_.cacheHits++;
        } else {
            metrics_.cacheMisses++;
        }
        
        auto now = std::chrono::steady_clock::now();
        if (lastCommandTime_.time_since_epoch().count() > 0) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastCommandTime_);
            // Simple running average
            metrics_.avgExecutionTime = 
                (metrics_.avgExecutionTime * (metrics_.commandCount - 1) + 
                 duration.count()) / metrics_.commandCount;
        }
        lastCommandTime_ = now;
    }
    
    void resetMetrics() {
        metrics_ = {0, 0.0, 0, 0, 100.0};
        startTime_ = std::chrono::steady_clock::now();
        lastCommandTime_ = std::chrono::steady_clock::time_point();
    }
    
    std::string getMetricsReport() const {
        std::stringstream ss;
        ss << "Performance Metrics:" << std::endl;
        ss << "  Commands executed: " << metrics_.commandCount << std::endl;
        ss << "  Average execution time: " << metrics_.avgExecutionTime << " ms" << std::endl;
        ss << "  Cache hits: " << metrics_.cacheHits << std::endl;
        ss << "  Cache misses: " << metrics_.cacheMisses << std::endl;
        if (metrics_.cacheHits + metrics_.cacheMisses > 0) {
            double hitRate = (double)metrics_.cacheHits / 
                           (metrics_.cacheHits + metrics_.cacheMisses) * 100.0;
            ss << "  Cache hit rate: " << hitRate << "%" << std::endl;
        }
        return ss.str();
    }
};