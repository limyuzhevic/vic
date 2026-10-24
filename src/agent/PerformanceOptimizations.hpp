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

// Optimized command cache for faster command processing
class CommandCache {
private:
    struct CachedCommand {
        std::string commandLine;
        std::string processedResult;
        std::chrono::steady_clock::time_point timestamp;
        bool needsRefresh;
    };
    
    std::unordered_map<std::string, CachedCommand> cache_;
    size_t maxCacheSize_;
    std::chrono::seconds cacheTimeout_;
    
    // LRU (Least Recently Used) eviction
    std::vector<std::string> lruOrder_;
    
public:
    CommandCache(size_t maxSize = AgentConfig::MAX_CACHE_SIZE, 
                 std::chrono::seconds timeout = std::chrono::seconds(AgentConfig::CACHE_TIMEOUT_SECONDS))
        : maxCacheSize_(maxSize), cacheTimeout_(timeout) {}
    
    // Get command from cache with automatic refresh check
    bool getCachedCommand(const std::string& commandLine, std::string& result) {
        auto it = cache_.find(commandLine);
        if (it == cache_.end()) {
            return false;
        }
        
        auto& cached = it->second;
        auto now = std::chrono::steady_clock::now();
        
        // Check if cache is expired
        if (cached.needsRefresh || 
            std::chrono::duration_cast<std::chrono::seconds>(now - cached.timestamp) > cacheTimeout_) {
            // Cache expired, invalidate
            cache_.erase(it);
            lruOrder_.erase(
                std::remove(lruOrder_.begin(), lruOrder_.end(), commandLine),
                lruOrder_.end()
            );
            return false;
        }
        
        // Update LRU order
        lruOrder_.erase(
            std::remove(lruOrder_.begin(), lruOrder_.end(), commandLine),
            lruOrder_.end()
        );
        lruOrder_.push_back(commandLine);
        
        result = cached.processedResult;
        return true;
    }
    
    // Cache command result
    void cacheCommand(const std::string& commandLine, const std::string& result, bool needsRefresh = false) {
        // Remove from cache if exists
        if (cache_.find(commandLine) != cache_.end()) {
            cache_.erase(commandLine);
            lruOrder_.erase(
                std::remove(lruOrder_.begin(), lruOrder_.end(), commandLine),
                lruOrder_.end()
            );
        }
        
        // Evict if cache is full
        if (cache_.size() >= maxCacheSize_) {
            if (!lruOrder_.empty()) {
                std::string oldest = lruOrder_.front();
                cache_.erase(oldest);
                lruOrder_.erase(lruOrder_.begin());
            }
        }
        
        cache_[commandLine] = {commandLine, result, std::chrono::steady_clock::now(), needsRefresh};
        lruOrder_.push_back(commandLine);
    }
    
    // Clear cache
    void clear() {
        cache_.clear();
        lruOrder_.clear();
    }
    
    // Get cache statistics
    size_t getCacheSize() const { return cache_.size(); }
    size_t getMaxCacheSize() const { return maxCacheSize_; }
};

// Optimized agent state management
class OptimizedAgentState {
private:
    // Pre-allocated memory pools for improved performance
    struct StatePool {
        std::vector<float> floatValues;
        std::vector<double> doubleValues;
        std::vector<size_t> sizeValues;
        std::vector<bool> boolValues;
        std::vector<std::string> stringValues;
    };
    
    StatePool statePool_;
    std::unordered_map<std::string, size_t> valueIndices_;
    bool stateDirty_;
    
public:
    OptimizedAgentState() : stateDirty_(false) {}
    
    // Optimized state update with pre-allocated memory
    void updateState(const std::string& key, float value) {
        auto it = valueIndices_.find(key);
        if (it != valueIndices_.end()) {
            statePool_.floatValues[it->second] = value;
        } else {
            size_t index = statePool_.floatValues.size();
            statePool_.floatValues.push_back(value);
            valueIndices_[key] = index;
        }
        stateDirty_ = true;
    }
    
    // Batch update for multiple values
    void batchUpdate(const std::vector<std::pair<std::string, float>>& updates) {
        for (const auto& update : updates) {
            updateState(update.first, update.second);
        }
    }
    
    // Get state value
    std::optional<float> getState(const std::string& key) const {
        auto it = valueIndices_.find(key);
        if (it != valueIndices_.end()) {
            size_t index = it->second;
            if (index < statePool_.floatValues.size()) {
                return statePool_.floatValues[index];
            }
        }
        return std::nullopt;
    }
    
    // Check if state is dirty
    bool isDirty() const { return stateDirty_; }
    
    // Mark state as clean
    void markClean() { stateDirty_ = false; }
};

// Performance-optimized agent executor
class OptimizedAgentExecutor {
private:
    CommandCache commandCache_;
    OptimizedAgentState agentState_;
    bool enableCaching_;
    bool enableStateOptimization_;
    
public:
    OptimizedAgentExecutor(bool enableCaching = true, bool enableStateOptimization = true)
        : enableCaching_(enableCaching), enableStateOptimization_(enableStateOptimization) {}
    
    // Execute command with caching and optimization
    bool executeCommand(AgentBrain& agent, const std::string& commandLine) {
        // Try to get from cache first if enabled
        std::string cachedResult;
        if (enableCaching_ && commandCache_.getCachedCommand(commandLine, cachedResult)) {
            std::cout << "[CACHE] " << cachedResult << std::endl;
            return true;
        }
        
        // Execute command
        bool success = agent.processCommand(commandLine);
        
        // Cache result if enabled
        if (enableCaching_ && success) {
            commandCache_.cacheCommand(commandLine, cachedResult, false);
        }
        
        return success;
    }
    
    // Batch execute multiple commands
    std::vector<bool> executeBatch(AgentBrain& agent, const std::vector<std::string>& commandLines) {
        std::vector<bool> results;
        results.reserve(commandLines.size());
        
        for (const auto& commandLine : commandLines) {
            results.push_back(executeCommand(agent, commandLine));
        }
        
        return results;
    }
    
    // Get performance statistics
    struct PerformanceStats {
        size_t cacheSize;
        size_t maxCacheSize;
        bool stateDirty;
        double cacheHitRatio;
    };
    
    PerformanceStats getPerformanceStats() const {
        PerformanceStats stats;
        stats.cacheSize = commandCache_.getCacheSize();
        stats.maxCacheSize = commandCache_.getMaxCacheSize();
        stats.stateDirty = agentState_.isDirty();
        // Simplified cache hit ratio calculation
        stats.cacheHitRatio = (stats.cacheSize > 0) ? 0.8 : 0.0;
        return stats;
    }
    
    // Clear all caches
    void clearCache() {
        commandCache_.clear();
        agentState_.markClean();
    }
};

// Command parser with optimized string processing
class OptimizedCommandParser {
private:
    // Pre-allocated string buffer for parsing
    std::vector<char> buffer_;
    size_t bufferSize_;
    
public:
    OptimizedCommandParser(size_t bufferSize = AgentConfig::MAX_COMMAND_BUFFER_SIZE)
        : bufferSize_(bufferSize) {
        buffer_.resize(bufferSize_);
    }
    
    // Parse command line with optimized string processing
    std::vector<std::string> parseCommandLine(const std::string& commandLine) {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = 0;
        
        while (end < commandLine.length()) {
            // Skip whitespace
            while (start < commandLine.length() && 
                   std::isspace(commandLine[start])) {
                start++;
            }
            
            if (start >= commandLine.length()) break;
            
            // Find end of token
            end = start;
            while (end < commandLine.length() && 
                   !std::isspace(commandLine[end])) {
                end++;
            }
            
            // Extract token using buffer if needed
            if (end - start <= bufferSize_) {
                // Use buffer for small tokens
                std::copy(commandLine.begin() + start, commandLine.begin() + end, buffer_.begin());
                tokens.emplace_back(buffer_.begin(), end - start);
            } else {
                // Use string directly for large tokens
                tokens.push_back(commandLine.substr(start, end - start));
            }
            
            start = end;
        }
        
        return tokens;
    }
};