#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace nlm {

// Enhanced debug logging system
class DebugLogger {
public:
    enum class Level {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };
    
    struct LogEntry {
        Level level;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        uint64_t sessionId = 0;
        
        // Helper to format log entry
        std::string format() const {
            std::stringstream ss;
            auto time = std::chrono::system_clock::to_time_t(timestamp);
            ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] ";
            ss << "[" << levelToString(level) << "] ";
            ss << message;
            if (!file.empty()) {
                ss << " [" << file << ":" << line << "]";
            }
            return ss.str();
        }
        
    private:
        std::string levelToString(Level level) const {
            switch (level) {
                case Level::Debug: return "DEBUG";
                case Level::Info: return "INFO";
                case Level::Warning: return "WARNING";
                case Level::Error: return "ERROR";
                case Level::Critical: return "CRITICAL";
                default: return "UNKNOWN";
            }
        }
    };
    
    DebugLogger() = default;
    ~DebugLogger() = default;
    
    // Log a message
    void log(Level level, const std::string& message, 
             const std::string& file = "", int line = 0,
             const std::string& function = "", uint64_t sessionId = 0) {
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.sessionId = sessionId;
        
        logEntries_.push_back(entry);
        
        // Output to console if enabled
        if (consoleOutputEnabled_ && level >= minLevel_) {
            std::cout << entry.format() << std::endl;
        }
        
        // Output to file if enabled
        if (fileOutputEnabled_ && logFile_) {
            *logFile_ << entry.format() << std::endl;
        }
    }
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", uint64_t sessionId = 0) {
        if (isEnabled(Level::Debug)) {
            log(Level::Debug, message, file, line, function, sessionId);
        }
    }
    
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", uint64_t sessionId = 0) {
        if (isEnabled(Level::Info)) {
            log(Level::Info, message, file, line, function, sessionId);
        }
    }
    
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", uint64_t sessionId = 0) {
        if (isEnabled(Level::Warning)) {
            log(Level::Warning, message, file, line, function, sessionId);
        }
    }
    
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", uint64_t sessionId = 0) {
        if (isEnabled(Level::Error)) {
            log(Level::Error, message, file, line, function, sessionId);
        }
    }
    
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", uint64_t sessionId = 0) {
        log(Level::Critical, message, file, line, function, sessionId);
    }
    
    // Check if level is enabled
    bool isEnabled(Level level) const {
        return static_cast<int>(level) >= static_cast<int>(minLevel_);
    }
    
    // Set minimum log level
    void setLevel(Level level) {
        minLevel_ = level;
    }
    
    Level getLevel() const {
        return minLevel_;
    }
    
    // Enable/disable console output
    void setConsoleOutputEnabled(bool enabled) {
        consoleOutputEnabled_ = enabled;
    }
    
    bool getConsoleOutputEnabled() const {
        return consoleOutputEnabled_;
    }
    
    // Enable/disable file output
    void setFileOutputEnabled(bool enabled) {
        fileOutputEnabled_ = enabled;
        if (enabled && !logFile_) {
            logFile_ = std::make_unique<std::ofstream>("nlm_debug.log", std::ios::app);
        } else if (!enabled && logFile_) {
            logFile_.reset();
        }
    }
    
    // Set log file path
    void setLogFile(const std::string& filepath) {
        closeLogFile();
        logFile_ = std::make_unique<std::ofstream>(filepath, std::ios::app);
        fileOutputEnabled_ = true;
    }
    
    void closeLogFile() {
        if (logFile_) {
            logFile_->close();
            logFile_.reset();
        }
        fileOutputEnabled_ = false;
    }
    
    // Get log entries
    const std::vector<LogEntry>& getLogEntries() const {
        return logEntries_;
    }
    
    // Clear log entries
    void clearLogEntries() {
        logEntries_.clear();
    }
    
    // Get log entries by level
    std::vector<LogEntry> getLogEntriesByLevel(Level level) const {
        std::vector<LogEntry> filtered;
        for (const auto& entry : logEntries_) {
            if (entry.level == level) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
    // Get log entries by session
    std::vector<LogEntry> getLogEntriesBySession(uint64_t sessionId) const {
        std::vector<LogEntry> filtered;
        for (const auto& entry : logEntries_) {
            if (entry.sessionId == sessionId) {
                filtered.push_back(entry);
            }
        }
        return filtered;
    }
    
    // Generate log report
    std::string generateLogReport() const {
        std::stringstream ss;
        ss << "Debug Log Report" << std::endl;
        ss << "=================" << std::endl;
        ss << "Total entries: " << logEntries_.size() << std::endl;
        
        // Count by level
        std::map<Level, int> levelCounts;
        for (const auto& entry : logEntries_) {
            levelCounts[entry.level]++;
        }
        
        ss << "Debug: " << levelCounts[Level::Debug] << std::endl;
        ss << "Info: " << levelCounts[Level::Info] << std::endl;
        ss << "Warning: " << levelCounts[Level::Warning] << std::endl;
        ss << "Error: " << levelCounts[Level::Error] << std::endl;
        ss << "Critical: " << levelCounts[Level::Critical] << std::endl;
        
        return ss.str();
    }
    
    // Get global debug logger instance
    static DebugLogger& getGlobal() {
        static DebugLogger instance;
        return instance;
    }
    
    // Set global debug logger
    static void setGlobal(std::shared_ptr<DebugLogger> logger) {
        globalLogger_ = logger;
    }

private:
    std::vector<LogEntry> logEntries_;
    Level minLevel_ = Level::Info;
    bool consoleOutputEnabled_ = true;
    bool fileOutputEnabled_ = false;
    std::unique_ptr<std::ofstream> logFile_;
    static std::shared_ptr<DebugLogger> globalLogger_;
};

// State inspector for brain analysis
class StateInspector {
public:
    struct BrainState {
        uint64_t sessionId = 0;
        std::string timestamp;
        std::string developmentalStage;
        size_t totalNeurons = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        double averageFiringRate = 0.0;
        double excitationInhibitionRatio = 0.0;
        size_t totalSpikes = 0;
        size_t totalSynapticEvents = 0;
        double totalEnergy = 0.0;
        double averageHealth = 0.0;
        size_t totalRegions = 0;
        
        // Helper to format state
        std::string format() const {
            std::stringstream ss;
            ss << "Brain State Report" << std::endl;
            ss << "==================" << std::endl;
            ss << "Session ID: " << sessionId << std::endl;
            ss << "Timestamp: " << timestamp << std::endl;
            ss << "Developmental Stage: " << developmentalStage << std::endl;
            ss << "Total Neurons: " << totalNeurons << std::endl;
            ss << "Active Neurons: " << activeNeurons << std::endl;
            ss << "Firing Neurons: " << firingNeurons << std::endl;
            ss << "Average Firing Rate: " << std::fixed << std::setprecision(2) << averageFiringRate << std::endl;
            ss << "E/I Ratio: " << std::fixed << std::setprecision(2) << excitationInhibitionRatio << std::endl;
            ss << "Total Spikes: " << totalSpikes << std::endl;
            ss << "Total Synaptic Events: " << totalSynapticEvents << std::endl;
            ss << "Total Energy: " << std::fixed << std::setprecision(2) << totalEnergy << std::endl;
            ss << "Average Health: " << std::fixed << std::setprecision(2) << averageHealth << std::endl;
            ss << "Total Regions: " << totalRegions << std::endl;
            return ss.str();
        }
    };
    
    StateInspector() = default;
    ~StateInspector() = default;
    
    // Inspect brain state
    BrainState inspectState(const std::string& developmentalStage,
                           size_t totalNeurons,
                           size_t activeNeurons,
                           size_t firingNeurons,
                           double averageFiringRate,
                           double excitationInhibitionRatio,
                           size_t totalSpikes,
                           size_t totalSynapticEvents,
                           double totalEnergy,
                           double averageHealth,
                           size_t totalRegions,
                           uint64_t sessionId = 0) {
        BrainState state;
        state.sessionId = sessionId;
        
        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        state.timestamp = ss.str();
        
        state.developmentalStage = developmentalStage;
        state.totalNeurons = totalNeurons;
        state.activeNeurons = activeNeurons;
        state.firingNeurons = firingNeurons;
        state.averageFiringRate = averageFiringRate;
        state.excitationInhibitionRatio = excitationInhibitionRatio;
        state.totalSpikes = totalSpikes;
        state.totalSynapticEvents = totalSynapticEvents;
        state.totalEnergy = totalEnergy;
        state.averageHealth = averageHealth;
        state.totalRegions = totalRegions;
        
        return state;
    }
    
    // Compare two brain states
    std::string compareStates(const BrainState& state1, const BrainState& state2) {
        std::stringstream ss;
        ss << "Brain State Comparison" << std::endl;
        ss << "======================" << std::endl;
        ss << "Session 1: " << state1.sessionId << " vs Session 2: " << state2.sessionId << std::endl;
        ss << std::endl;
        
        ss << "Firing Rate: " << std::fixed << std::setprecision(2)
           << "S1: " << state1.averageFiringRate << " S2: " << state2.averageFiringRate
           << " (diff: " << (state1.averageFiringRate - state2.averageFiringRate) << ")" << std::endl;
        
        ss << "E/I Ratio: " << std::fixed << std::setprecision(2)
           << "S1: " << state1.excitationInhibitionRatio << " S2: " << state2.excitationInhibitionRatio
           << " (diff: " << (state1.excitationInhibitionRatio - state2.excitationInhibitionRatio) << ")" << std::endl;
        
        ss << "Total Spikes: "
           << "S1: " << state1.totalSpikes << " S2: " << state2.totalSpikes
           << " (diff: " << (state1.totalSpikes - state2.totalSpikes) << ")" << std::endl;
        
        ss << "Health: " << std::fixed << std::setprecision(2)
           << "S1: " << state1.averageHealth << " S2: " << state2.averageHealth
           << " (diff: " << (state1.averageHealth - state2.averageHealth) << ")" << std::endl;
        
        return ss.str();
    }
    
    // Get developmental stage string
    static std::string developmentalStageToString(const std::string& stage) {
        if (stage == "initial") return "Initial";
        else if (stage == "critical_period") return "CriticalPeriod";
        else if (stage == "maturation") return "Maturation";
        else if (stage == "adult") return "Adult";
        else if (stage == "aging") return "Aging";
        else return stage;
    }
};

// Memory dumper
class MemoryDumper {
public:
    struct MemoryBlock {
        void* address = nullptr;
        size_t size = 0;
        std::string type;
        uint64_t timestamp = 0;
        bool isAccessible = true;
        
        std::string format() const {
            std::stringstream ss;
            ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << (uint64_t)address
               << " " << std::dec << std::setw(12) << size << " bytes "
               << std::left << std::setw(20) << type
               << " at " << timestamp;
            return ss.str();
        }
    };
    
    MemoryDumper() = default;
    ~MemoryDumper() = default;
    
    // Dump memory regions
    std::vector<MemoryBlock> dumpMemoryRegions() {
        std::vector<MemoryBlock> blocks;
        
        // Basic memory dump - in real implementation would use system APIs
        // This is a simplified version for demonstration
        
        // Add some dummy memory blocks for testing
        MemoryBlock block1;
        block1.address = (void*)0x1000;
        block1.size = 1024;
        block1.type = "NeuronPool";
        block1.timestamp = nowNanoseconds();
        block1.isAccessible = true;
        blocks.push_back(block1);
        
        MemoryBlock block2;
        block2.address = (void*)0x2000;
        block2.size = 2048;
        block2.type = "SynapsePool";
        block2.timestamp = nowNanoseconds();
        block2.isAccessible = true;
        blocks.push_back(block2);
        
        MemoryBlock block3;
        block3.address = (void*)0x3000;
        block3.size = 4096;
        block3.type = "StringPool";
        block3.timestamp = nowNanoseconds();
        block3.isAccessible = true;
        blocks.push_back(block3);
        
        return blocks;
    }
    
    // Dump specific address range
    std::vector<MemoryBlock> dumpAddressRange(void* start, size_t size) {
        std::vector<MemoryBlock> blocks;
        
        // Validate address range
        if (!isAccessible(start, size)) {
            return blocks; // Return empty if not accessible
        }
        
        MemoryBlock block;
        block.address = start;
        block.size = size;
        block.type = "CustomRange";
        block.timestamp = nowNanoseconds();
        block.isAccessible = true;
        
        blocks.push_back(block);
        return blocks;
    }
    
    // Check if memory region is accessible
    bool isAccessible(void* address, size_t size) {
        // In real implementation would use system calls
        // For now, return true for demonstration
        return true;
    }
    
    // Save memory dump to file
    bool saveMemoryDump(const std::vector<MemoryBlock>& blocks, const std::string& filepath) {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Write header
        uint32_t numBlocks = static_cast<uint32_t>(blocks.size());
        file.write(reinterpret_cast<const char*>(&numBlocks), sizeof(numBlocks));
        
        // Write blocks
        for (const auto& block : blocks) {
            // Write block data (simplified)
            file.write(reinterpret_cast<const char*>(&block.address), sizeof(block.address));
            file.write(reinterpret_cast<const char*>(&block.size), sizeof(block.size));
            
            // Write type string
            uint32_t typeSize = static_cast<uint32_t>(block.type.size());
            file.write(reinterpret_cast<const char*>(&typeSize), sizeof(typeSize));
            file.write(block.type.c_str(), typeSize);
        }
        
        file.close();
        return true;
    }
    
    // Load memory dump from file
    std::vector<MemoryBlock> loadMemoryDump(const std::string& filepath) {
        std::vector<MemoryBlock> blocks;
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return blocks;
        }
        
        // Read header
        uint32_t numBlocks;
        file.read(reinterpret_cast<char*>(&numBlocks), sizeof(numBlocks));
        
        // Read blocks
        for (uint32_t i = 0; i < numBlocks; ++i) {
            MemoryBlock block;
            file.read(reinterpret_cast<char*>(&block.address), sizeof(block.address));
            file.read(reinterpret_cast<char*>(&block.size), sizeof(block.size));
            
            // Read type string
            uint32_t typeSize;
            file.read(reinterpret_cast<char*>(&typeSize), sizeof(typeSize));
            std::vector<char> typeBuffer(typeSize);
            file.read(typeBuffer.data(), typeSize);
            block.type = std::string(typeBuffer.begin(), typeBuffer.end());
            
            block.timestamp = nowNanoseconds();
            block.isAccessible = isAccessible(block.address, block.size);
            
            blocks.push_back(block);
        }
        
        file.close();
        return blocks;
    }
    
    // Generate memory report
    std::string generateMemoryReport(const std::vector<MemoryBlock>& blocks) {
        std::stringstream ss;
        ss << "Memory Dump Report" << std::endl;
        ss << "==================" << std::endl;
        ss << "Total blocks: " << blocks.size() << std::endl;
        
        size_t totalSize = 0;
        for (const auto& block : blocks) {
            totalSize += block.size;
        }
        
        ss << "Total size: " << totalSize << " bytes (" 
           << (totalSize / (1024.0 * 1024.0)) << " MB)" << std::endl;
        
        ss << std::left << std::setw(20) << "Type" << std::right << "Size (bytes)" << std::endl;
        ss << std::left << std::setw(20) << "----" << std::right << "------------" << std::endl;
        
        std::map<std::string, size_t> typeSizes;
        for (const auto& block : blocks) {
            typeSizes[block.type] += block.size;
        }
        
        for (const auto& pair : typeSizes) {
            ss << std::left << std::setw(20) << pair.first << std::right << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
private:
    static uint64_t nowNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
};

// Debug helper functions
class DebugHelper {
public:
    DebugHelper() = default;
    ~DebugHelper() = default;
    
    // Get current call stack (simplified)
    std::vector<std::string> getCallStack() {
        std::vector<std::string> stack;
        // In real implementation would use system-specific stack walking
        // For now, return a placeholder
        stack.push_back("main");
        stack.push_back("simulateBrain");
        stack.push_back("processSensoryInput");
        return stack;
    }
    
    // Find function by name in call stack
    bool findFunctionInStack(const std::string& functionName, 
                            const std::vector<std::string>& stack) {
        for (const auto& func : stack) {
            if (func == functionName) {
                return true;
            }
        }
        return false;
    }
    
    // Get memory usage in human-readable format
    std::string formatMemorySize(size_t bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        } else if (bytes < 1024 * 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else if (bytes < 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        }
    }
    
    // Format time in human-readable format
    std::string formatTime(double seconds) {
        if (seconds < 1.0) {
            return std::to_string(seconds * 1000) + " ms";
        } else if (seconds < 60.0) {
            return std::to_string(seconds) + " s";
        } else if (seconds < 3600.0) {
            return std::to_string(seconds / 60.0) + " min";
        } else {
            return std::to_string(seconds / 3600.0) + " hours";
        }
    }
    
    // Generate unique session ID
    uint64_t generateSessionId() {
        static uint64_t counter = 0;
        return ++counter;
    }
    
    // Hex dump for debugging
    std::string hexDump(const void* address, size_t size) {
        std::stringstream ss;
        const uint8_t* data = static_cast<const uint8_t*>(address);
        
        for (size_t i = 0; i < size; i += 16) {
            ss << std::hex << std::setw(8) << std::setfill('0') << (uint64_t)address + i << ": ";
            
            for (size_t j = 0; j < 16 && i + j < size; ++j) {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i + j] << " ";
                if (j == 7) ss << " ";
            }
            
            ss << " ";
            for (size_t j = 0; j < 16 && i + j < size; ++j) {
                char c = data[i + j];
                if (std::isprint(c)) {
                    ss << c;
                } else {
                    ss << ".";
                }
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    // Check for common issues
    std::vector<std::string> checkForIssues(const std::string& context) {
        std::vector<std::string> issues;
        
        // Example checks
        if (context.find("memory") != std::string::npos) {
            issues.push_back("Memory usage high in " + context);
        }
        
        if (context.find("performance") != std::string::npos) {
            issues.push_back("Performance degradation detected in " + context);
        }
        
        if (context.find("brain") != std::string::npos) {
            issues.push_back("Potential instability in brain state for " + context);
        }
        
        return issues;
    }
};

} // namespace nlm