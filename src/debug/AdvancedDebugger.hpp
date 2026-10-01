#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"

namespace nlm {

// ===============================================================
// DEBUGGING AND PROFILING TYPES
// ===============================================================

enum class DebugLevel {
    Debug = 0,           // Detailed debugging information
    Info = 1,            // General information
    Warning = 2,         // Warnings
    Error = 3,           // Errors
    Critical = 4,        // Critical errors
    Trace = -1,          // Very detailed trace information
    Performance = 5,     // Performance metrics
    Memory = 6,          // Memory usage information
    Neural = 7,          // Neural network debugging
    Integration = 8,     // Integration debugging
    Development = 9,     // Development debugging
};

enum class ProfilerType {
    Time,               // Time profiling
    Memory,             // Memory profiling
    System,             // System resource profiling
    Neural,             // Neural activity profiling
    Integration,        // Integration profiling
    Custom              // Custom profiling
};

enum class BreakpointType {
    Line,               // Break on specific line
    Function,           // Break on specific function
    Condition,          // Break when condition is met
    Event,              // Break on specific event
    Custom              // Custom breakpoint
};

enum class ProfilingMode {
    Simple,             // Simple profiling
    Detailed,           // Detailed profiling with context
    Comprehensive,       // Comprehensive profiling with all metrics
    Custom              // Custom profiling configuration
};

// ===============================================================
// BREAKPOINT AND DEBUGGING DEFINITIONS
// ===============================================================

struct Breakpoint {
    std::string id;                          // Unique breakpoint identifier
    std::string name;                        // Human-readable name
    BreakpointType type;                     // Type of breakpoint
    std::string file;                        // File where breakpoint is set
    int line;                                // Line number where breakpoint is set
    std::string function;                    // Function where breakpoint is set
    
    std::string condition;                   // Condition for the breakpoint (for Condition type)
    std::string expression;                  // Expression to evaluate (for Condition type)
    std::string message;                     // Message when breakpoint is hit
    
    std::unordered_map<std::string, std::string> parameters;  // Breakpoint-specific parameters
    
    // State
    bool enabled = true;                     // Whether breakpoint is enabled
    bool hit = false;                        // Whether breakpoint has been hit
    int hitCount = 0;                        // Number of times breakpoint has been hit
    std::chrono::system_clock::time_point lastHitTime;  // Last hit time
    
    // Actions
    std::function<void()> action;           // Action to perform when breakpoint is hit
    std::function<bool()> conditionCheck;   // Condition check function
    
    // Tags
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
};

struct DebugSession {
    std::string id;                          // Session identifier
    std::string name;                        // Session name
    DebugLevel level;                        // Debug level
    ProfilingMode profilingMode;              // Profiling mode
    std::chrono::system_clock::time_point startTime;  // Session start time
    std::chrono::system_clock::time_point endTime;    // Session end time
    
    std::unordered_map<std::string, Breakpoint> breakpoints;  // Session breakpoints
    std::vector<std::string> enabledBreakpoints;  // Enabled breakpoints
    
    // Profiling data
    std::unordered_map<ProfilerType, std::vector<double>> profiles;  // Profiling data by type
    std::unordered_map<std::string, std::vector<std::chrono::system_clock::time_point>> events;  // Events by type
    
    // Debugging state
    std::unordered_map<std::string, std::string> variables;     // Debugged variables
    std::unordered_map<std::string, std::function<void()>> functions;  // Debugged functions
    std::unordered_map<std::string, std::string> callStack;      // Call stack
    
    // Session state
    bool isActive = false;                   // Whether session is active
    bool isPaused = false;                   // Whether session is paused
    size_t stepCount = 0;                    // Number of steps taken
    
    // Output
    std::string outputFile;                  // Output file
    bool enableColors = true;                // Whether to enable colors in output
    
    DebugSession() : level(DebugLevel::Info), profilingMode(ProfilingMode::Detailed) {}
};

struct ProfilingData {
    ProfilerType type;                       // Type of profiling
    std::string name;                        // Profile name
    std::chrono::system_clock::time_point timestamp;  // Timestamp
    
    // Performance data
    double value;                            // Measured value
    double min;                              // Minimum value
    double max;                              // Maximum value
    double average;                          // Average value
    double median;                           // Median value
    double standardDeviation;                // Standard deviation
    
    // Context
    std::unordered_map<std::string, std::string> context;      // Context information
    std::unordered_map<std::string, double> metrics;           // Additional metrics
    std::vector<std::string> tags;           // Tags for categorization
    
    // System information
    int threadId = 0;                        // Thread ID
    std::string threadName;                  // Thread name
    std::string processName;                 // Process name
    
    // Neural network information
    int neuronCount = 0;                     // Number of neurons
    int synapseCount = 0;                     // Number of synapses
    int spikeCount = 0;                      // Number of spikes
    float firingRate = 0.0f;                  // Firing rate
    
    // Memory information
    size_t memoryUsage = 0;                  // Memory usage
    size_t memoryAllocation = 0;             // Memory allocation
    size_t memoryPeak = 0;                   // Peak memory usage
    
    ProfilingData() : type(ProfilerType::Time), value(0.0), min(0.0), max(0.0), 
                     average(0.0), median(0.0), standardDeviation(0.0) {}
};

struct DebugReport {
    std::string id;                          // Report identifier
    std::string sessionId;                   // Session identifier
    std::chrono::system_clock::time_point timestamp;  // Report timestamp
    DebugLevel level;                        // Report level
    
    std::string title;                       // Report title
    std::string description;                 // Report description
    
    // Statistics
    size_t totalBreakpoints = 0;              // Total breakpoints
    size_t hitBreakpoints = 0;               // Hit breakpoints
    size_t enabledBreakpoints = 0;           // Enabled breakpoints
    size_t totalSteps = 0;                   // Total steps
    double sessionDuration = 0.0;            // Session duration
    
    // Performance metrics
    double averageStepTime = 0.0;            // Average step time
    double maxStepTime = 0.0;                // Maximum step time
    double minStepTime = 0.0;                // Minimum step time
    double performanceScore = 0.0;           // Performance score
    
    // Memory metrics
    size_t peakMemoryUsage = 0;              // Peak memory usage
    size_t averageMemoryUsage = 0;           // Average memory usage
    
    // Neural metrics
    size_t totalSpikes = 0;                  // Total spikes
    float averageFiringRate = 0.0f;          // Average firing rate
    
    // Integration metrics
    size_t successfulIntegrations = 0;      // Successful integrations
    size_t failedIntegrations = 0;           // Failed integrations
    float integrationScore = 0.0f;           // Integration score
    
    // Issues and problems
    std::vector<std::string> issues;         // List of issues
    std::vector<std::string> warnings;       // List of warnings
    std::vector<std::string> errors;         // List of errors
    
    // Recommendations
    std::vector<std::string> recommendations;  // Recommendations for improvement
    std::vector<std::string> suggestions;     // Suggestions for optimization
    
    // Debug information
    std::string stackTrace;                  // Stack trace
    std::string systemInfo;                  // System information
    std::string configurationInfo;           // Configuration information
    
    // Export
    std::string jsonReport;                  // JSON report
    std::string textReport;                  // Text report
    std::string htmlReport;                  // HTML report
    
    DebugReport() : level(DebugLevel::Info) {}
};

// ===============================================================
// DEBUGGING AND PROFILING INTERFACES
// ===============================================================

class IDebugger {
public:
    virtual ~IDebugger() = default;
    
    // Core debugging methods
    virtual void startSession(const std::string& name, DebugLevel level = DebugLevel::Info) = 0;
    virtual void endSession(const std::string& sessionId) = 0;
    virtual DebugSession getSession(const std::string& sessionId) const = 0;
    virtual std::vector<DebugSession> getAllSessions() const = 0;
    
    // Breakpoint management
    virtual void addBreakpoint(const Breakpoint& breakpoint) = 0;
    virtual void removeBreakpoint(const std::string& breakpointId) = 0;
    virtual void enableBreakpoint(const std::string& breakpointId) = 0;
    virtual void disableBreakpoint(const std::string& breakpointId) = 0;
    virtual Breakpoint getBreakpoint(const std::string& breakpointId) const = 0;
    virtual std::vector<Breakpoint> getAllBreakpoints() const = 0;
    
    // Session control
    virtual void pauseSession(const std::string& sessionId) = 0;
    virtual void resumeSession(const std::string& sessionId) = 0;
    virtual void stepSession(const std::string& sessionId, size_t steps = 1) = 0;
    
    // Variable inspection
    virtual std::unordered_map<std::string, std::string> inspectVariables(const std::string& sessionId) const = 0;
    virtual std::string getVariable(const std::string& sessionId, const std::string& variableName) const = 0;
    virtual void setVariable(const std::string& sessionId, const std::string& variableName, const std::string& value) = 0;
    
    // Function inspection
    virtual std::function<void()> getFunction(const std::string& sessionId, const std::string& functionName) const = 0;
    virtual void addFunction(const std::string& sessionId, const std::string& functionName, std::function<void()> function) = 0;
    
    // Profiling
    virtual void startProfiling(ProfilerType type, const std::string& name = "") = 0;
    virtual void stopProfiling(ProfilerType type, const std::string& name = "") = 0;
    virtual ProfilingData getProfile(ProfilerType type, const std::string& name = "") const = 0;
    virtual std::vector<ProfilingData> getAllProfiles(ProfilerType type = ProfilerType::Time) const = 0;
    
    // Event handling
    virtual void addEventHandler(const std::string& eventType, std::function<void(const std::unordered_map<std::string, std::string>&)> handler) = 0;
    virtual void removeEventHandler(const std::string& eventType) = 0;
    virtual void triggerEvent(const std::string& eventType, const std::unordered_map<std::string, std::string>& data) = 0;
    
    // Reporting
    virtual DebugReport generateReport(const std::string& sessionId) const = 0;
    virtual std::string exportReport(const DebugReport& report, const std::string& format = "json") const = 0;
    virtual bool importReport(const std::string& data, DebugReport& report, const std::string& format = "json") = 0;
    
    // Configuration
    virtual void setDebugLevel(DebugLevel level) = 0;
    virtual DebugLevel getDebugLevel() const = 0;
    virtual void setProfilingMode(ProfilingMode mode) = 0;
    virtual ProfilingMode getProfilingMode() const = 0;
    
    // Statistics
    virtual size_t getTotalSessions() const = 0;
    virtual size_t getActiveSessions() const = 0;
    virtual size_t getTotalBreakpoints() const = 0;
    virtual size_t getHitBreakpoints() const = 0;
    virtual double getTotalDebugTime() const = 0;
};

class AdvancedDebugger : public IDebugger {
public:
    AdvancedDebugger();
    ~AdvancedDebugger() override;
    
    // Core debugging methods
    void startSession(const std::string& name, DebugLevel level = DebugLevel::Info) override;
    void endSession(const std::string& sessionId) override;
    DebugSession getSession(const std::string& sessionId) const override;
    std::vector<DebugSession> getAllSessions() const override;
    
    // Breakpoint management
    void addBreakpoint(const Breakpoint& breakpoint) override;
    void removeBreakpoint(const std::string& breakpointId) override;
    void enableBreakpoint(const std::string& breakpointId) override;
    void disableBreakpoint(const std::string& breakpointId) override;
    Breakpoint getBreakpoint(const std::string& breakpointId) const override;
    std::vector<Breakpoint> getAllBreakpoints() const override;
    
    // Session control
    void pauseSession(const std::string& sessionId) override;
    void resumeSession(const std::string& sessionId) override;
    void stepSession(const std::string& sessionId, size_t steps = 1) override;
    
    // Variable inspection
    std::unordered_map<std::string, std::string> inspectVariables(const std::string& sessionId) const override;
    std::string getVariable(const std::string& sessionId, const std::string& variableName) const override;
    void setVariable(const std::string& sessionId, const std::string& variableName, const std::string& value) override;
    
    // Function inspection
    std::function<void()> getFunction(const std::string& sessionId, const std::string& functionName) const override;
    void addFunction(const std::string& sessionId, const std::string& functionName, std::function<void()> function) override;
    
    // Profiling
    void startProfiling(ProfilerType type, const std::string& name = "") override;
    void stopProfiling(ProfilerType type, const std::string& name = "") override;
    ProfilingData getProfile(ProfilerType type, const std::string& name = "") const override;
    std::vector<ProfilingData> getAllProfiles(ProfilerType type = ProfilerType::Time) const override;
    
    // Event handling
    void addEventHandler(const std::string& eventType, std::function<void(const std::unordered_map<std::string, std::string>&)> handler) override;
    void removeEventHandler(const std::string& eventType) override;
    void triggerEvent(const std::string& eventType, const std::unordered_map<std::string, std::string>& data) override;
    
    // Reporting
    DebugReport generateReport(const std::string& sessionId) const override;
    std::string exportReport(const DebugReport& report, const std::string& format = "json") const override;
    bool importReport(const std::string& data, DebugReport& report, const std::string& format = "json") override;
    
    // Configuration
    void setDebugLevel(DebugLevel level) override;
    DebugLevel getDebugLevel() const override;
    void setProfilingMode(ProfilingMode mode) override;
    ProfilingMode getProfilingMode() const override;
    
    // Statistics
    size_t getTotalSessions() const override;
    size_t getActiveSessions() const override;
    size_t getTotalBreakpoints() const override;
    size_t getHitBreakpoints() const override;
    double getTotalDebugTime() const override;
    
    // Brain integration
    void attachBrain(std::shared_ptr<Brain> brain);
    void attachWorld(std::shared_ptr<SimpleWorld> world);
    void attachAgent(std::shared_ptr<AgentBrain> agent);
    
    // Debug event handling
    void onStepCompleted(const std::string& sessionId, size_t step, double stepTime);
    void onMemoryUpdate(const std::string& sessionId, size_t memoryUsage);
    void onNeuralEvent(const std::string& sessionId, const std::string& eventType, const std::unordered_map<std::string, double>& data);
    void onSystemEvent(const std::string& sessionId, const std::string& eventType, const std::unordered_map<std::string, std::string>& data);
    
    // Breakpoint checking
    void checkBreakpoints(const std::string& sessionId, const std::unordered_map<std::string, std::string>& context);
    bool shouldBreak(const Breakpoint& breakpoint, const std::unordered_map<std::string, std::string>& context) const;
    
    // Profiling data collection
    void collectProfilingData(const std::string& sessionId);
    void updateProfile(ProfilerType type, const std::string& name, double value, const std::unordered_map<std::string, std::string>& context);
    
    // Session management
    void cleanupInactiveSessions();
    void resetSession(const std::string& sessionId);
    
    // Report generation
    std::string generateDebugReport(const std::string& sessionId) const;
    std::string generatePerformanceReport(const std::string& sessionId) const;
    std::string generateIntegrationReport(const std::string& sessionId) const;
    std::string generateErrorReport(const std::string& sessionId) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeDebugSystem();
    void setupBreakpoints();
    void setupEventHandlers();
    
    bool matchesBreakpointCondition(const Breakpoint& breakpoint, const std::unordered_map<std::string, std::string>& context) const;
    std::string evaluateBreakpointExpression(const std::string& expression, const std::unordered_map<std::string, std::string>& context) const;
    
    void collectSystemMetrics(const std::string& sessionId);
    void collectNeuralMetrics(const std::string& sessionId, const std::shared_ptr<Brain>& brain);
    void collectMemoryMetrics(const std::string& sessionId);
    void collectIntegrationMetrics(const std::string& sessionId);
    
    void updateSessionStats(DebugSession& session);
    double calculatePerformanceScore(const DebugSession& session) const;
    std::string determineDebugLevel(double issuesCount) const;
    
    std::string generateJsonReport(const DebugReport& report) const;
    std::string generateTextReport(const DebugReport& report) const;
    std::string generateHtmlReport(const DebugReport& report) const;
    
    void writeReport(const std::string& filename, const std::string& content) const;
    bool isReportFormatSupported(const std::string& format) const;
};

// ===============================================================
// DEBUGGING FACTORY AND UTILITIES
// ===============================================================

class DebuggerFactory {
public:
    static std::shared_ptr<IDebugger> createStandardDebugger();
    static std::shared_ptr<IDebugger> createAdvancedDebugger();
    static std::shared_ptr<IDebugger> createPerformanceDebugger();
    static std::shared_ptr<IDebugger> createIntegrationDebugger();
    static std::shared_ptr<IDebugger> createCustomDebugger(const std::vector<Breakpoint>& customBreakpoints);
    
    static void setupGlobalDebugger(std::shared_ptr<IDebugger> debugger);
    static std::shared_ptr<IDebugger> getGlobalDebugger();
    
    static Breakpoint createLineBreakpoint(const std::string& id, const std::string& name, const std::string& file, int line);
    static Breakpoint createFunctionBreakpoint(const std::string& id, const std::string& name, const std::string& function);
    static Breakpoint createConditionBreakpoint(const std::string& id, const std::string& name, const std::string& condition);
    static Breakpoint createEventBreakpoint(const std::string& id, const std::string& name, const std::string& eventType);
    
    static void setupDevelopmentDebugger();
    static void setupProductionDebugger();
    static void setupResearchDebugger();
    
    static std::string getDebuggerVersion();
    static std::vector<std::string> getAvailableDebuggerTypes();
    static bool isDebuggerAvailable(const std::string& type);
    
    static std::string getDefaultBreakpoints();
    static std::string getPerformanceBreakpoints();
    static std::string getIntegrationBreakpoints();
};

// ===============================================================
// DEBUGGING MACROS AND STREAMS
// ===============================================================

// Debugger macros
#define NLM_START_DEBUG_SESSION(name, level) nlm::DebuggerFactory::getGlobalDebugger()->startSession(name, level)
#define NLM_END_DEBUG_SESSION(sessionId) nlm::DebuggerFactory::getGlobalDebugger()->endSession(sessionId)
#define NLM_ADD_BREAKPOINT(breakpoint) nlm::DebuggerFactory::getGlobalDebugger()->addBreakpoint(breakpoint)
#define NLM_REMOVE_BREAKPOINT(id) nlm::DebuggerFactory::getGlobalDebugger()->removeBreakpoint(id)
#define NLM_ENABLE_BREAKPOINT(id) nlm::DebuggerFactory::getGlobalDebugger()->enableBreakpoint(id)
#define NLM_DISABLE_BREAKPOINT(id) nlm::DebuggerFactory::getGlobalDebugger()->disableBreakpoint(id)
#define NLM_GET_BREAKPOINT(id) nlm::DebuggerFactory::getGlobalDebugger()->getBreakpoint(id)
#define NLM_DEBUG_SESSION_EXISTS(sessionId) !nlm::DebuggerFactory::getGlobalDebugger()->getAllSessions().empty()

#define NLM_PAUSE_SESSION(sessionId) nlm::DebuggerFactory::getGlobalDebugger()->pauseSession(sessionId)
#define NLM_RESUME_SESSION(sessionId) nlm::DebuggerFactory::getGlobalDebugger()->resumeSession(sessionId)
#define NLM_STEP_SESSION(sessionId, steps) nlm::DebuggerFactory::getGlobalDebugger()->stepSession(sessionId, steps)

#define NLM_INSPECT_VARIABLES(sessionId) nlm::DebuggerFactory::getGlobalDebugger()->inspectVariables(sessionId)
#define NLM_GET_VARIABLE(sessionId, name) nlm::DebuggerFactory::getGlobalDebugger()->getVariable(sessionId, name)
#define NLM_SET_VARIABLE(sessionId, name, value) nlm::DebuggerFactory::getGlobalDebugger()->setVariable(sessionId, name, value)

#define NLM_START_PROFILING(type, name) nlm::DebuggerFactory::getGlobalDebugger()->startProfiling(type, name)
#define NLM_STOP_PROFILING(type, name) nlm::DebuggerFactory::getGlobalDebugger()->stopProfiling(type, name)
#define NLM_GET_PROFILE(type, name) nlm::DebuggerFactory::getGlobalDebugger()->getProfile(type, name)

#define NLM_TRIGGER_EVENT(eventType, data) nlm::DebuggerFactory::getGlobalDebugger()->triggerEvent(eventType, data)

#define NLM_GENERATE_DEBUG_REPORT(sessionId) nlm::DebuggerFactory::getGlobalDebugger()->generateReport(sessionId)
#define NLM_EXPORT_DEBUG_REPORT(report, format) nlm::DebuggerFactory::getGlobalDebugger()->exportReport(report, format)

// Debug stream for detailed logging
class DebugStream {
public:
    DebugStream(const std::string& sessionId, DebugLevel level, const std::string& file = "", int line = 0, const std::string& function = "");
    ~DebugStream();
    
    template<typename T>
    DebugStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    DebugStream& withBreakpoint(const std::string& breakpointId);
    DebugStream& withCondition(const std::string& condition);
    DebugStream& withAction(std::function<void()> action);
    
    DebugStream& withVariable(const std::string& name, const std::string& value);
    DebugStream& withFunction(const std::string& name, std::function<void()> function);
    
    std::string getMessage() const;

private:
    std::string sessionId_;
    DebugLevel level_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
    
    std::vector<std::string> breakpoints_;
    std::string condition_;
    std::function<void()> action_;
    
    std::unordered_map<std::string, std::string> variables_;
    std::unordered_map<std::string, std::function<void()>> functions_;
};

// Debug stream macro
#define NLM_DEBUG_STREAM(sessionId, level) nlm::DebugStream sessionId##_debug_stream(sessionId, level, __FILE__, __LINE__, __func__)

// ===============================================================
// DEBUGGING HELP AND UTILITIES
// ===============================================================

void showDebugHelp();
void showDebuggerSetupGuide();
void showBreakpointGuide();
void showProfilingGuide();
void showDebugReportGuide();

} // namespace nlm

// Include implementation header
#include "Debugger.ipp"
