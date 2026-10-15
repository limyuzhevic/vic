// NLM Advanced Debugging and Profiling Tools - Breakpoint Debugger

#pragma once

#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <functional>

namespace nlm {

// Debug breakpoint condition
struct BreakpointCondition {
    std::string id;
    std::string description;
    std::string condition; // Expression to evaluate
    std::string file;
    int line = 0;
    bool enabled = true;
    bool persistent = false; // Should it trigger once or repeatedly
    std::function<bool(const std::map<std::string, double>&)> conditionEvaluator;
    
    BreakpointCondition(const std::string& id, const std::string& desc, const std::string& cond)
        : id(id), description(desc), condition(cond), enabled(true), persistent(false) {}
};

// Debug breakpoint
struct Breakpoint {
    std::string id;
    BreakpointCondition condition;
    std::chrono::system_clock::time_point triggeredTime;
    int triggerCount = 0;
    bool isActive = true;
    
    Breakpoint(const BreakpointCondition& cond) : condition(cond), triggerCount(0), isActive(true) {}
};

// Breakpoint debugger class
class BreakpointDebugger {
public:
    BreakpointDebugger();
    ~BreakpointDebugger();
    
    // Breakpoint management
    void addBreakpoint(const BreakpointCondition& condition);
    void removeBreakpoint(const std::string& id);
    void enableBreakpoint(const std::string& id);
    void disableBreakpoint(const std::string& id);
    bool hasBreakpoint(const std::string& id) const;
    
    // Breakpoint checking
    bool shouldBreak(const std::map<std::string, double>& variables) const;
    void handleBreakpoint(const std::string& id);
    std::vector<Breakpoint> getActiveBreakpoints() const;
    
    // Breakpoint evaluation
    bool evaluateCondition(const BreakpointCondition& condition, 
                          const std::map<std::string, double>& variables) const;
    
    // Control methods
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const { return running_; }
    
    // Statistics
    size_t getBreakpointHits(const std::string& id) const;
    size_t getTotalBreakpointHits() const;
    std::string getBreakpointStatistics() const;
    
    // Event handling
    void setBreakpointTriggeredCallback(std::function<void(const Breakpoint&)> callback);
    
    // Integration
    void attachToSimulation(const std::string& simulationId);
    void attachToComponent(const std::string& componentId);
    
    // Configuration
    void setVariable(const std::string& name, double value);
    double getVariable(const std::string& name) const;
    std::map<std::string, double> getAllVariables() const;
    
private:
    std::atomic<bool> running_;
    mutable std::mutex mutex_;
    
    std::map<std::string, BreakpointCondition> conditions_;
    std::map<std::string, Breakpoint> breakpoints_;
    std::map<std::string, int> hitCounts_;
    
    std::map<std::string, double> variables_;
    std::vector<std::string> attachedComponents_;
    
    std::function<void(const Breakpoint&)> breakpointTriggeredCallback_;
    
    // Internal methods
    BreakpointCondition createConditionFromString(const std::string& condition);
    bool parseExpression(const std::string& expression, const std::map<std::string, double>& variables) const;
    void triggerBreakpoint(const BreakpointCondition& condition);
    void logBreakpoint(const Breakpoint& breakpoint);
};

} // namespace nlm
