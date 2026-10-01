#pragma once

/**
 * Debugging and Profiling Infrastructure
 * 
 * Phase 5 introduces comprehensive debugging and profiling capabilities for large-scale neural simulation:
 * 
 * 1. Advanced Debugging Tools
 *    - Memory leak detection
 *    - Reference counting and garbage collection tracking
 *    - Thread synchronization analysis
 *    - Performance bottleneck identification
 *    
 * 2. Performance Profiling
 *    - Time profiling for critical sections
 *    - Memory usage tracking and analysis
 *    - Event flow tracing
 *    - Network communication profiling
 *    
 * 3. Development Aids
 *    - Interactive debugging interface
 *    - State inspection and modification
 *    - Breakpoint and watchpoint support
 *    - Visual debugging tools
 *    
 * This infrastructure supports both development and production debugging scenarios.
 */

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <functional>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stacktrace>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
class Config;

/**
 * Memory leak detection and tracking
 */
class MemoryTracker {
public:
    struct AllocationInfo {
        void* address;
        size_t size;
        std::string type;
        std::string file;
        int line;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
        
        AllocationInfo(void* addr, size_t sz, const std::string& t, 
                      const std::string& f, int l)
            : address(addr), size(sz), type(t), file(f), line(l),
              timestamp(std::chrono::high_resolution_clock::now()),
              threadId(std::this_thread::get_id()) {}
    };
    
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    // Track allocation
    void trackAllocation(void* address, size_t size, const std::string& type,
                         const std::string& file = "", int line = 0);
    
    // Track deallocation
    void trackDeallocation(void* address);
    
    // Check for memory leaks
    std::vector<AllocationInfo> checkForLeaks() const;
    
    // Get memory statistics
    size_t getTotalAllocated() const;
    size_t getTotalDeallocated() const;
    size_t getCurrentUsage() const;
    size_t getPeakUsage() const;
    
    // Clear tracking
    void clear();
    
    // Set up leak checking for specific types
    void trackType(const std::string& type);
    void stopTrackingType(const std::string& type);
    
private:
    MemoryTracker() = default;
    ~MemoryTracker() = default;
    
    // Disable copying and assignment
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Time profiling for performance analysis
 */
class TimeProfiler {
public:
    struct ProfileData {
        std::string functionName;
        std::chrono::duration<double> totalTime;
        std::chrono::duration<double> minTime;
        std::chrono::duration<double> maxTime;
        size_t callCount;
        std::chrono::duration<double> avgTime;
        std::chrono::duration<double> stdDevTime;
        
        ProfileData(const std::string& name)
            : functionName(name), totalTime(0), minTime(0), maxTime(0),
              callCount(0), avgTime(0), stdDevTime(0) {}
    };
    
    static TimeProfiler& getInstance() {
        static TimeProfiler instance;
        return instance;
    }
    
    // Start timing a function
    void startProfile(const std::string& functionName);
    
    // Stop timing and record results
    void stopProfile(const std::string& functionName);
    
    // Get profile data for a function
    const ProfileData* getProfile(const std::string& functionName) const;
    
    // Get all profile data
    std::vector<ProfileData> getAllProfiles() const;
    
    // Reset all profiles
    void reset();
    
    // Enable/disable profiling
    void enable(bool enable = true) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Get total time spent in all functions
    double getTotalTime() const;
    
    // Print profile report
    void printReport(std::ostream& out = std::cout) const;
    
    // Export profile data to JSON
    void exportToJson(const std::string& filename) const;
    
private:
    TimeProfiler() : enabled_(true) {}
    ~TimeProfiler() = default;
    
    // Disable copying and assignment
    TimeProfiler(const TimeProfiler&) = delete;
    TimeProfiler& operator=(const TimeProfiler&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool enabled_;
};

/**
 * Event flow tracer for understanding system dynamics
 */
class EventTracer {
public:
    struct Event {
        enum Type {
            SIMULATION_STEP,
            NEURON_SPIKE,
            SYNAPSE_ACTIVATION,
            PLASTICITY_UPDATE,
            MEMORY_ACCESS,
            THREAD_SYNC,
            CONFIGURATION_CHANGE,
            USER_ACTION
        };
        
        Type type;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string description;
        std::unordered_map<std::string, std::string> metadata;
        std::thread::id threadId;
        std::string sourceFile;
        int sourceLine;
        
        Event(Type t, const std::string& desc)
            : type(t), timestamp(std::chrono::high_resolution_clock::now()),
              description(desc), threadId(std::this_thread::get_id()),
              sourceFile(""), sourceLine(0) {}
    };
    
    static EventTracer& getInstance() {
        static EventTracer instance;
        return instance;
    }
    
    // Record an event
    void recordEvent(Event::Type type, const std::string& description);
    
    // Record event with metadata
    template<typename... Args>
    void recordEvent(Event::Type type, const std::string& description,
                    const std::string& key1, const std::string& value1,
                    const Args&... args) {
        if (!enabled_) return;
        
        Event event(type, description);
        event.metadata[key1] = value1;
        recordEventInternal(event);
    }
    
    // Filter events by type
    std::vector<Event> getEventsByType(Event::Type type) const;
    
    // Get events within time range
    std::vector<Event> getEventsInRange(
        const std::chrono::high_resolution_clock::time_point& start,
        const std::chrono::high_resolution_clock::time_point& end
    ) const;
    
    // Get events by thread
    std::vector<Event> getEventsByThread(std::thread::id threadId) const;
    
    // Get event statistics
    size_t getEventCount(Event::Type type) const;
    double getEventFrequency(Event::Type type) const; // events per second
    
    // Clear all events
    void clear();
    
    // Enable/disable tracing
    void enable(bool enable = true) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Start/stop tracing
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Export events to JSON
    void exportToJson(const std::string& filename) const;
    
    // Analyze event patterns
    std::string analyzePatterns() const;
    
private:
    EventTracer() : enabled_(true), running_(false) {}
    ~EventTracer() = default;
    
    // Disable copying and assignment
    EventTracer(const EventTracer&) = delete;
    EventTracer& operator=(const EventTracer&) = delete;
    
    void recordEventInternal(const Event& event);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool enabled_;
    bool running_;
};

/**
 * Interactive debugging interface
 */
class Debugger {
public:
    struct Breakpoint {
        std::string condition;
        std::string description;
        bool active;
        size_t hitCount;
        
        Breakpoint(const std::string& cond, const std::string& desc)
            : condition(cond), description(desc), active(true), hitCount(0) {}
    };
    
    struct Watchpoint {
        std::string expression;
        std::string description;
        bool active;
        size_t triggerCount;
        
        Watchpoint(const std::string& expr, const std::string& desc)
            : expression(expr), description(desc), active(true), triggerCount(0) {}
    };
    
    static Debugger& getInstance() {
        static Debugger instance;
        return instance;
    }
    
    // Set up breakpoints
    void addBreakpoint(const std::string& condition, const std::string& description);
    void removeBreakpoint(const std::string& condition);
    void enableBreakpoint(const std::string& condition);
    void disableBreakpoint(const std::string& condition);
    
    // Set up watchpoints
    void addWatchpoint(const std::string& expression, const std::string& description);
    void removeWatchpoint(const std::string& expression);
    void enableWatchpoint(const std::string& expression);
    void disableWatchpoint(const std::string& expression);
    
    // Debug commands
    std::string evaluateExpression(const std::string& expression);
    std::string inspectVariable(const std::string& name);
    std::string inspectObject(const std::string& type, size_t id);
    std::string listVariables();
    std::string listBreakpoints();
    std::string listWatchpoints();
    
    // State management
    std::string saveState();
    bool loadState(const std::string& state);
    std::string getCurrentStateHash();
    
    // Debugging control
    void stepOver();
    void stepInto();
    void stepOut();
    void continueExecution();
    bool isPaused() const { return paused_; }
    
    // Output
    void print(std::ostream& out = std::cout);
    
private:
    Debugger() : paused_(false) {}
    ~Debugger() = default;
    
    // Disable copying and assignment
    Debugger(const Debugger&) = delete;
    Debugger& operator=(const Debugger&) = delete;
    
    bool evaluateBreakpointCondition(const std::string& condition);
    bool evaluateWatchpointExpression(const std::string& expression);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool paused_;
};

/**
 * Visual debugging and analysis tools
 */
class VisualDebugger {
public:
    static VisualDebugger& getInstance() {
        static VisualDebugger instance;
        return instance;
    }
    
    // Generate network visualization
    std::string generateNetworkVisualization(const Brain& brain) const;
    
    // Generate activity heatmap
    std::string generateActivityHeatmap(const Brain& brain) const;
    
    // Generate connectivity matrix
    std::string generateConnectivityMatrix(const Brain& brain) const;
    
    // Generate time series visualization
    std::string generateTimeSeries(const Brain& brain, const std::string& metric) const;
    
    // Generate memory usage graph
    std::string generateMemoryUsageGraph() const;
    
    // Generate performance timeline
    std::string generatePerformanceTimeline() const;
    
    // Generate interactive HTML report
    void generateInteractiveReport(const Brain& brain, const std::string& filename) const;
    
private:
    VisualDebugger() = default;
    ~VisualDebugger() = default;
    
    // Disable copying and assignment
    VisualDebugger(const VisualDebugger&) = delete;
    VisualDebugger& operator=(const VisualDebugger&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Memory leak detector with detailed reporting
 */
class LeakDetector {
public:
    struct LeakInfo {
        std::string type;
        std::string file;
        int line;
        size_t size;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
        
        LeakInfo(const std::string& t, const std::string& f, int l, 
                size_t sz, const std::chrono::high_resolution_clock::time_point& tp,
                std::thread::id id)
            : type(t), file(f), line(l), size(sz), timestamp(tp), threadId(id) {}
    };
    
    static LeakDetector& getInstance() {
        static LeakDetector instance;
        return instance;
    }
    
    // Check for leaks
    std::vector<LeakInfo> detectLeaks() const;
    
    // Generate leak report
    void generateLeakReport(std::ostream& out = std::cout) const;
    
    // Check specific type for leaks
    std::vector<LeakInfo> detectLeaksForType(const std::string& type) const;
    
    // Filter leaks by file
    std::vector<LeakInfo> filterLeaksByFile(
        const std::vector<LeakInfo>& leaks,
        const std::string& filename
    ) const;
    
    // Filter leaks by thread
    std::vector<LeakInfo> filterLeaksByThread(
        const std::vector<LeakInfo>& leaks,
        std::thread::id threadId
    ) const;
    
private:
    LeakDetector() = default;
    ~LeakDetector() = default;
    
    // Disable copying and assignment
    LeakDetector(const LeakDetector&) = delete;
    LeakDetector& operator=(const LeakDetector&) = delete;
};

/**
 * Debugging utilities
 */
namespace DebugUtils {
    // Stack trace generation
    std::string getStackTrace();
    
    // Object introspection
    template<typename T>
    std::string getObjectInfo(const T& obj);
    
    // Memory info
    std::string getMemoryInfo();
    
    // Thread info
    std::string getThreadInfo();
    
    // System info
    std::string getSystemInfo();
    
    // Performance benchmark
    double benchmarkFunction(std::function<void()> func, size_t iterations = 1000);
    
    // Find objects by type
    template<typename T>
    std::vector<T*> findObjects(const std::vector<std::unique_ptr<T>>& container);
    
    // Safe deletion
    template<typename T>
    void safeDelete(T*& ptr);
    
    // Assert with message
    void assert(bool condition, const std::string& message);
    
    // Log debug message
    void logDebug(const std::string& message);
    
    // Enable/disable debug output
    void setDebugOutputEnabled(bool enabled);
    bool isDebugOutputEnabled();
}

} // namespace nlm
