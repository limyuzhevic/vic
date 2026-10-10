// NLM Advanced Features - Profile Mode
// CPU/memory profiling mode for performance analysis and optimization

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace advanced {

// Profile event types
enum class ProfileEventType {
    FunctionCall,
    MemoryAllocation,
    MemoryDeallocation,
    ThreadStart,
    ThreadEnd,
    SystemCall,
    FileIO,
    NetworkIO,
    CPUUsage,
    Custom
};

// Profile sampling mode
enum class ProfileSamplingMode {
    None,
    EventBased,
    TimeBased,
    FrequencyBased
};

// Profile data structure
struct ProfileData {
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    ProfileEventType eventType;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    std::string moduleName;
    uint64_t threadIdHash;
    
    // Event-specific data
    std::string eventName;
    double eventValue;
    std::map<std::string, std::string> metadata;
    
    // Memory profiling
    uint64_t memoryAddress;
    size_t memorySize;
    std::string memoryOperation;
    
    // Performance metrics
    double cpuTime;
    double wallTime;
    uint64_t systemCalls;
    uint64_t fileOperations;
    uint64_t networkOperations;
    
    ProfileData() : eventType(ProfileEventType::Custom), eventValue(0.0), 
                    memoryAddress(0), memorySize(0), cpuTime(0.0), 
                    wallTime(0.0), systemCalls(0), fileOperations(0), networkOperations(0) {
        timestamp = std::chrono::system_clock::now();
        sourceLine = 0;
        threadIdHash = std::hash<std::string>{}(
            std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()))
        );
    }
};

// Profile statistics
struct ProfileStatistics {
    uint64_t totalEvents;
    uint64_t eventsByType[10]; // Index by ProfileEventType
    uint64_t eventsByThread[100]; // Index by thread hash
    std::chrono::system_clock::time_point firstEventTime;
    std::chrono::system_clock::time_point lastEventTime;
    double eventRatePerMinute;
    std::string mostFrequentEvent;
    uint64_t mostFrequentEventCount;
    double totalCpuTime;
    double totalWallTime;
    uint64_t totalMemoryAllocated;
    uint64_t totalMemoryDeallocated;
    std::map<std::string, double> functionTimes;
    std::map<std::string, uint64_t> functionCallCounts;
    std::map<std::string, double> memoryUsage;
    std::map<std::string, double> systemCallTimes;
    
    ProfileStatistics() : totalEvents(0), eventRatePerMinute(0.0),
                          totalCpuTime(0.0), totalWallTime(0.0),
                          totalMemoryAllocated(0), totalMemoryDeallocated(0) {
        for (int i = 0; i < 10; ++i) eventsByType[i] = 0;
        for (int i = 0; i < 100; ++i) eventsByThread[i] = 0;
    }
};

// Profile configuration
struct ProfileConfig {
    ProfileSamplingMode samplingMode;
    double samplingInterval; // in seconds
    double profileDuration; // in seconds
    bool profileMemory;
    bool profileSystemCalls;
    bool profileFileIO;
    bool profileNetworkIO;
    bool profileThreading;
    std::vector<std::string> includePatterns;
    std::vector<std::string> excludePatterns;
    std::map<std::string, double> thresholds;
    std::string outputFormat;
    std::string outputFile;
    bool exportToDatabase;
    std::string databaseConnection;
    std::function<bool(const ProfileData&)> filter;
    std::function<void(const ProfileData&)> callback;
    
    ProfileConfig() : samplingMode(ProfileSamplingMode::TimeBased), 
                      samplingInterval(0.1), profileDuration(-1.0), // -1 = unlimited
                      profileMemory(true), profileSystemCalls(false),
                      profileFileIO(false), profileNetworkIO(false),
                      profileThreading(true), exportToDatabase(false),
                      outputFormat("text") {
    }
};

// Profile mode class
class ProfileMode {
public:
    ProfileMode();
    ~ProfileMode();
    
    // Configuration
    void configure(const ProfileConfig& config);
    ProfileConfig getConfig() const;
    
    // Profile control
    bool start();
    void stop();
    bool isActive() const;
    bool isRecording() const;
    
    // Event recording
    void recordEvent(const ProfileData& event);
    void recordFunctionEntry(const std::string& functionName,
                            const std::string& file, int line);
    void recordFunctionExit(const std::string& functionName,
                           const std::string& file, int line);
    void recordMemoryEvent(const std::string& operation,
                          uint64_t address, size_t size,
                          const std::string& file = "", int line = 0);
    void recordSystemCall(const std::string& callName, double timeTaken,
                         const std::string& file = "", int line = 0);
    void recordFileIO(const std::string& filePath, const std::string& operation,
                      double size, double timeTaken, const std::string& file = "",
                      int line = 0);
    void recordNetworkIO(const std::string& endpoint, const std::string& protocol,
                        double size, double timeTaken, const std::string& file = "",
                        int line = 0);
    
    // Query and analysis
    std::vector<ProfileData> getEvents() const;
    std::vector<ProfileData> getEventsByType(ProfileEventType type) const;
    std::vector<ProfileData> getEventsByThread(const std::string& threadId) const;
    std::vector<ProfileData> getEventsByTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    ProfileStatistics getStatistics() const;
    double getTotalProfileTime() const;
    uint64_t getEventCount() const;
    
    // Analysis functions
    std::string analyzeHotFunctions(size_t topN = 10) const;
    std::string analyzeMemoryUsage() const;
    std::string analyzeSystemCallUsage() const;
    std::string analyzeThreading() const;
    std::string generateReport() const;
    
    // Export
    bool exportEvents(const std::string& filename) const;
    bool importEvents(const std::string& filename);
    bool exportStatistics(const std::string& filename) const;
    bool exportToDatabase(const std::string& connection) const;
    
    // Filtering
    void addIncludePattern(const std::string& pattern);
    void addExcludePattern(const std::string& pattern);
    void setFilter(const std::function<bool(const ProfileData&)>& filter);
    
    // Callbacks
    void setEventCallback(const std::function<void(const ProfileData&)>& callback);
    void setCompletionCallback(const std::function<void()>& callback);
    
    // Singleton pattern
    static ProfileMode& getInstance();
    static void resetInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    bool shouldRecordEvent(const ProfileData& event) const;
    void updateStatistics(const ProfileData& event);
    std::string formatTime(double seconds) const;
    std::string formatBytes(size_t bytes) const;
    void startSamplingThread();
    void stopSamplingThread();
    void samplingWorker();
    std::chrono::system_clock::time_point getCurrentTime() const;
};

} // namespace advanced
} // namespace nlm

// Export ProfileMode functions for external use
namespace nlm {
    namespace advanced {
        // Global profile mode functions
        inline void startProfiling() {
            ProfileMode::getInstance().start();
        }
        
        inline void stopProfiling() {
            ProfileMode::getInstance().stop();
        }
        
        inline bool isProfiling() {
            return ProfileMode::getInstance().isActive();
        }
        
        inline void profileFunction(const std::string& functionName) {
            ProfileMode::getInstance().recordFunctionEntry(functionName, __FILE__, __LINE__);
        }
        
        inline void unprofileFunction(const std::string& functionName) {
            ProfileMode::getInstance().recordFunctionExit(functionName, __FILE__, __LINE__);
        }
        
        // Convenience macros
        #define NLM_PROFILE_FUNCTION() nlm::advanced::ProfileScope profileScope(__func__)
        
        class ProfileScope {
        public:
            ProfileScope(const std::string& functionName) {
                ProfileMode::getInstance().recordFunctionEntry(functionName, __FILE__, __LINE__);
            }
            
            ~ProfileScope() {
                ProfileMode::getInstance().recordFunctionExit("", __FILE__, __LINE__);
            }
        };
        
    } // namespace advanced
} // namespace nlm

// Include implementation
#include "ProfileMode.cpp"

// Initialize global profile macros
namespace nlm {
    namespace advanced {
        // Global profiling macros for use throughout the codebase
        #define NLM_PROFILE_FUNCTION_SCOPE(name) nlm::advanced::ProfileScope name##_scope(name)
        
        // Event-based profiling macros
        #define NLM_PROFILE_EVENT(name, value) { \
            nlm::advanced::ProfileData event; \
            event.eventType = nlm::advanced::ProfileEventType::Custom; \
            event.eventName = name; \
            event.eventValue = value; \
            ProfileMode::getInstance().recordEvent(event); \
        }
        
        #define NLM_PROFILE_MEMORY_OP(op, addr, size) { \
            nlm::advanced::ProfileData event; \
            event.eventType = nlm::advanced::ProfileEventType::MemoryAllocation; \
            event.memoryOperation = op; \
            event.memoryAddress = addr; \
            event.memorySize = size; \
            ProfileMode::getInstance().recordEvent(event); \
        }
        
        #define NLM_PROFILE_SYSTEM_CALL(call, time) { \
            nlm::advanced::ProfileData event; \
            event.eventType = nlm::advanced::ProfileEventType::SystemCall; \
            event.eventName = call; \
            event.eventValue = time; \
            ProfileMode::getInstance().recordEvent(event); \
        }
        
        #define NLM_PROFILE_FILE_IO(file, op, size, time) { \
            nlm::advanced::ProfileData event; \
            event.eventType = nlm::advanced::ProfileEventType::FileIO; \
            event.eventName = op; \
            event.eventValue = size; \
            event.metadata["file"] = file; \
            event.metadata["time"] = std::to_string(time); \
            ProfileMode::getInstance().recordEvent(event); \
        }
        
        #define NLM_PROFILE_NETWORK_IO(endpoint, protocol, size, time) { \
            nlm::advanced::ProfileData event; \
            event.eventType = nlm::advanced::ProfileEventType::NetworkIO; \
            event.eventName = protocol; \
            event.eventValue = size; \
            event.metadata["endpoint"] = endpoint; \
            event.metadata["time"] = std::to_string(time); \
            ProfileMode::getInstance().recordEvent(event); \
        }
        
    } // namespace advanced
} // namespace nlm