#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../core/Logger/Logger.hpp"
#include "../visualization/VisualizationInterface.hpp"

namespace nlm {

/**
 * @file RealTimeMonitor.hpp
 * 
 * Real-time neural activity monitoring system.
 * 
 * This file provides the RealTimeMonitor class, which offers comprehensive
 * real-time monitoring of neural activity, system state, and performance
 * metrics. This tool is essential for debugging large-scale neural simulations,
 * identifying performance bottlenecks, and understanding system behavior
 * during execution.
 * 
 * Key features:
 * - Real-time neural activity monitoring
 * - System state tracking and analysis
 * - Performance metrics collection
 * - Interactive monitoring interface
 * - Configurable monitoring parameters
 * - Thread-safe operation
 * - Customizable alert thresholds
 * - Historical data storage
 * - Real-time visualization integration
 * - Export capabilities for analysis
 * - Debug event detection and logging
 * 
 * @note Integrates with NLM's logging system
 * @note Supports multiple monitoring modes
 * @see VisualizationInterface - For real-time visualization
 * @see DebugInterface - For debugging commands
 * @see PerformanceProfiler - For performance monitoring
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class RealTimeMonitor {
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    // Disable copying - PImpl idiom
    RealTimeMonitor(const RealTimeMonitor&) = delete;
    RealTimeMonitor& operator=(const RealTimeMonitor&) = delete;
    RealTimeMonitor(RealTimeMonitor&&) noexcept;
    RealTimeMonitor& operator=(RealTimeMonitor&&) noexcept;
    
    /**
     * @brief Initialize the real-time monitor
     * 
     * @param visualizationInterface Reference to visualization interface
     * @return true if initialization successful
     * @post Monitor is ready for real-time monitoring
     */
    bool initialize(VisualizationInterface& visualizationInterface);
    
    /**
     * @brief Start monitoring neural activity
     * 
     * @param brain Reference to brain system to monitor
     * @param updateInterval Update interval in seconds
     * @return true if monitoring started
     */
    bool startMonitoring(const Brain& brain, double updateInterval = 0.1);
    
    /**
     * @brief Stop monitoring
     * 
     * @return true if monitoring stopped
     */
    bool stopMonitoring();
    
    /**
     * @brief Add monitoring target
     * 
     * @param monitorType Type of monitoring to add
     * @param target Target identifier
     */
    void addMonitoringTarget(const std::string& monitorType, const std::string& target);
    
    /**
     * @brief Remove monitoring target
     * 
     * @param monitorType Type of monitoring to remove
     * @param target Target identifier
     */
    void removeMonitoringTarget(const std::string& monitorType, const std::string& target);
    
    /**
     * @brief Set alert threshold for a metric
     * 
     * @param metricName Metric name
     * @param threshold Alert threshold value
     * @param level Alert level (0=warning, 1=error, 2=critical)
     */
    void setAlertThreshold(const std::string& metricName, double threshold, int level = 1);
    
    /**
     * @brief Get current monitoring status
     * 
     * @return Current status information
     */
    std::unordered_map<std::string, double> getMonitoringStatus() const;
    
    /**
     * @brief Check if monitoring is active
     * 
     * @return true if monitoring is active
     */
    bool isMonitoring() const;
    
    /**
     * @brief Get recent monitoring data
     * 
     * @param monitorType Type of monitoring
     * @param count Number of data points to retrieve
     * @return Vector of recent data points
     */
    std::vector<std::pair<double, double>> getRecentData(const std::string& monitorType, size_t count = 100) const;
    
    /**
     * @brief Export monitoring data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set callback for alert events
     * 
     * @param callback Function to call on alert
     */
    void setAlertCallback(std::function<void(const std::string&, double)> callback);
    
    /**
     * @brief Set callback for monitoring events
     * 
     * @param callback Function to call on monitoring update
     */
    void setUpdateCallback(std::function<void(const std::unordered_map<std::string, double>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file DebugInterface.hpp
 * 
 * Interactive debugging interface for NLM.
 * 
 * This file provides the DebugInterface class, which offers comprehensive
 * debugging capabilities for NLM applications. It provides step-by-step
 * execution control, error visualization, and interactive debugging tools
 * to help developers understand and troubleshoot neural simulation issues.
 * 
 * Key features:
 * - Step-by-step execution control
 * - Error visualization and analysis
 * - Breakpoint management
 * - Execution history and state inspection
 * - Variable watch and inspection
 * - Call stack navigation
 * - Memory inspection
 * - Thread debugging
 * - Performance profiling integration
 * - Real-time debugging support
 * - Export debugging sessions
 * - Interactive debugging console
 * - Debug script execution
 * - Automated test integration
 * 
 * @note Provides comprehensive debugging capabilities
 * @see RealTimeMonitor - For real-time monitoring
 * @see VisualizationInterface - For visualization
 * @see VisualizationEngine - For integration
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class DebugInterface {
public:
    DebugInterface();
    ~DebugInterface();
    
    // Disable copying - PImpl idiom
    DebugInterface(const DebugInterface&) = delete;
    DebugInterface& operator=(const DebugInterface&) = delete;
    DebugInterface(DebugInterface&&) noexcept;
    DebugInterface& operator=(DebugInterface&&) noexcept;
    
    /**
     * @brief Initialize the debug interface
     * 
     * @param visualizationInterface Reference to visualization interface
     * @return true if initialization successful
     */
    bool initialize(VisualizationInterface& visualizationInterface);
    
    /**
     * @brief Start debugging session
     * 
     * @param brain Reference to brain system to debug
     * @return true if debugging session started
     */
    bool startDebugSession(const Brain& brain);
    
    /**
     * @brief Stop debugging session
     * 
     * @return true if debugging session stopped
     */
    bool stopDebugSession();
    
    /**
     * @brief Execute one simulation step
     * 
     * @param stepNumber Step number to execute
     * @param time Current simulation time
     * @return true if step executed successfully
     */
    bool executeStep(int64_t stepNumber, double time);
    
    /**
     * @brief Set breakpoint at a simulation step
     * 
     * @param stepNumber Step number for breakpoint
     */
    void setBreakpoint(int64_t stepNumber);
    
    /**
     * @brief Remove breakpoint at a simulation step
     * 
     * @param stepNumber Step number for breakpoint
     */
    void removeBreakpoint(int64_t stepNumber);
    
    /**
     * @brief Get current execution state
     * 
     * @return Current execution state information
     */
    std::unordered_map<std::string, std::string> getExecutionState() const;
    
    /**
     * @brief Get execution history
     * 
     * @param count Number of steps to retrieve
     * @return Vector of execution states
     */
    std::vector<std::unordered_map<std::string, std::string>> getExecutionHistory(size_t count = 100) const;
    
    /**
     * @brief Get current call stack
     * 
     * @return Vector of stack frames
     */
    std::vector<std::string> getCallStack() const;
    
    /**
     * @brief Inspect variable value
     * 
     * @param variablePath Path to variable
     * @return Variable value as string
     */
    std::string inspectVariable(const std::string& variablePath) const;
    
    /**
     * @brief Set watch on a variable
     * 
     * @param variablePath Path to variable to watch
     */
    void setVariableWatch(const std::string& variablePath);
    
    /**
     * @brief Get watch list
     * 
     * @return Vector of watched variables
     */
    std::vector<std::string> getWatchList() const;
    
    /**
     * @brief Get error visualization data
     * 
     * @return Error visualization data
     */
    std::vector<std::unordered_map<std::string, std::string>> getErrorVisualization() const;
    
    /**
     * @brief Export debug session
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportDebugSession(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set callback for step execution
     * 
     * @param callback Function to call on step execution
     */
    void setStepCallback(std::function<void(int64_t, double)> callback);
    
    /**
     * @brief Set callback for error events
     * 
     * @param callback Function to call on error
     */
    void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    /**
     * @brief Set callback for breakpoint events
     * 
     * @param callback Function to call on breakpoint
     */
    void setBreakpointCallback(std::function<void(int64_t)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file StepDebugger.hpp
 * 
 * Step-by-step debugging tool for neural simulation.
 * 
 * This file provides the StepDebugger class, which offers granular
 * control over neural simulation execution. It allows developers to
 * execute simulations step-by-step, inspect system state at each step,
 * and understand the progression of neural computations.
 * 
 * Key features:
 * - Step-by-step simulation execution
 * - State inspection at each step
 * - Performance analysis per step
 * - Comparison between steps
 * - Performance bottleneck identification
 * - Memory usage monitoring
 * - Spike activity analysis
 * - Synaptic weight tracking
 * - Variable history logging
 * - Export step-by-step data
 * - Interactive step control
 * - Performance profiling per step
 * - Automated step analysis
 * 
 * @note Essential for debugging complex neural simulations
 * @see DebugInterface - For integration
 * @see RealTimeMonitor - For integration
 * @see VisualizationInterface - For visualization
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class StepDebugger {
public:
    StepDebugger();
    ~StepDebugger();
    
    // Disable copying - PImpl idiom
    StepDebugger(const StepDebugger&) = delete;
    StepDebugger& operator=(const StepDebugger&) = delete;
    StepDebugger(StepDebugger&&) noexcept;
    StepDebugger& operator=(StepDebugger&&) noexcept;
    
    /**
     * @brief Initialize the step debugger
     * 
     * @param brain Reference to brain system to debug
     * @return true if initialization successful
     */
    bool initialize(const Brain& brain);
    
    /**
     * @brief Execute a single step
     * 
     * @param stepNumber Step number to execute
     * @param time Current simulation time
     * @return true if step executed successfully
     */
    bool executeStep(int64_t stepNumber, double time);
    
    /**
     * @brief Get state at a specific step
     * 
     * @param stepNumber Step number
     * @return State information at that step
     */
    std::unordered_map<std::string, std::string> getStepState(int64_t stepNumber) const;
    
    /**
     * @brief Get step history
     * 
     * @param count Number of steps to retrieve
     * @return Vector of step states
     */
    std::vector<std::unordered_map<std::string, std::string>> getStepHistory(size_t count = 100) const;
    
    /**
     * @brief Compare two steps
     * 
     * @param step1 First step number
     * @param step2 Second step number
     * @return Comparison results
     */
    std::unordered_map<std::string, double> compareSteps(int64_t step1, int64_t step2) const;
    
    /**
     * @brief Get step performance metrics
     * 
     * @param stepNumber Step number
     * @return Performance metrics for that step
     */
    std::unordered_map<std::string, double> getStepPerformance(int64_t stepNumber) const;
    
    /**
     * @brief Get spike activity at a step
     * 
     * @param stepNumber Step number
     * @return Spike activity information
     */
    std::vector<std::pair<int64_t, double>> getStepSpikeActivity(int64_t stepNumber) const;
    
    /**
     * @brief Get synaptic weight changes at a step
     * 
     * @param stepNumber Step number
     * @return Synaptic weight change information
     */
    std::unordered_map<std::string, double> getStepSynapticChanges(int64_t stepNumber) const;
    
    /**
     * @brief Export step-by-step data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportStepData(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set callback for step completion
     * 
     * @param callback Function to call on step completion
     */
    void setStepCallback(std::function<void(int64_t, double)> callback);
    
    /**
     * @brief Get current step count
     * 
     * @return Current step count
     */
    int64_t getCurrentStep() const;
    
    /**
     * @brief Check if debugging is active
     * 
     * @return true if debugging is active
     */
    bool isDebugging() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm