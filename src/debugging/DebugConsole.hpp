#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Brain;
class VisualizationEngine;
class DataExporter;
class PerformanceProfiler;

// Command execution context
struct DebugCommandContext {
    Brain* brain;
    VisualizationEngine* visualizationEngine;
    DataExporter* dataExporter;
    PerformanceProfiler* profiler;
    double currentTime;
    size_t currentStep;
    
    DebugCommandContext() : brain(nullptr), visualizationEngine(nullptr), 
                           dataExporter(nullptr), profiler(nullptr),
                           currentTime(0.0), currentStep(0) {}
};

// Command result structure
struct DebugCommandResult {
    bool success;
    std::string output;
    std::map<std::string, std::string> metadata;
    
    DebugCommandResult() : success(false) {}
    DebugCommandResult(bool s, const std::string& out) : success(s), output(out) {}
};

// Command handler type
declare typedef std::function<DebugCommandResult(const std::vector<std::string>&, DebugCommandContext&)> CommandHandler;

// Debug console configuration
struct DebugConsoleConfig {
    bool enabled = true;
    bool autoRefresh = true;
    double refreshInterval = 0.1; // seconds
    bool enableHistory = true;
    size_t commandHistorySize = 1000;
    bool enableBreakpoints = true;
    bool enableWatchVariables = true;
    bool enableMemoryProfiling = true;
    bool enablePerformanceProfiling = true;
    std::string prompt = "nlm> ";
    std::string title = "NLM Debug Console";
};

// Breakpoint information
struct Breakpoint {
    size_t step;
    std::string description;
    bool active;
    std::string condition;  // Optional condition
    
    Breakpoint(size_t s = 0, const std::string& desc = "", bool act = true)
        : step(s), description(desc), active(act) {}
};

// Watch variable information
struct WatchVariable {
    std::string expression;
    std::string description;
    std::string currentValue;
    std::string type;
    size_t updateCount;
    bool enabled;
    
    WatchVariable(const std::string& expr, const std::string& desc = "")
        : expression(expr), description(desc), updateCount(0), enabled(true) {}
};

// Memory dump information
struct MemoryDump {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    double averageFiringRate;
    float excitationInhibitionRatio;
    size_t memoryUsage;
    
    std::vector<std::pair<std::string, size_t>> componentMemoryUsage;
    std::vector<std::pair<std::string, double>> componentTimeUsage;
};

// Debug console class
class DebugConsole {
public:
    DebugConsole();
    DebugConsole(const DebugConsoleConfig& config);
    ~DebugConsole();
    
    // Disable copying
    DebugConsole(const DebugConsole&) = delete;
    DebugConsole& operator=(const DebugConsole&) = delete;
    
    // Initialize console
    bool initialize(Brain* brain, VisualizationEngine* visualizationEngine = nullptr,
                   DataExporter* dataExporter = nullptr, PerformanceProfiler* profiler = nullptr);
    
    // Update console (for real-time updates)
    void update(double deltaTime);
    
    // Process user input
    void processInput(const std::string& input);
    
    // Execute command
    DebugCommandResult executeCommand(const std::string& command);
    
    // Command management
    void registerCommand(const std::string& name, const std::string& description,
                        const std::string& usage, CommandHandler handler);
    
    // Breakpoint management
    void addBreakpoint(size_t step, const std::string& description, bool active = true);
    void removeBreakpoint(size_t step);
    void clearBreakpoints();
    const std::vector<Breakpoint>& getBreakpoints() const;
    
    // Watch variable management
    void addWatchVariable(const std::string& expression, const std::string& description = "");
    void removeWatchVariable(const std::string& expression);
    void clearWatchVariables();
    const std::vector<WatchVariable>& getWatchVariables() const;
    
    // Memory analysis
    MemoryDump getMemoryDump() const;
    void dumpMemory(const std::string& filepath = "");
    
    // State control
    void pause();
    void resume();
    void step();
    bool isPaused() const;
    
    // Configuration
    void updateConfig(const DebugConsoleConfig& config);
    const DebugConsoleConfig& getConfig() const;
    
    // Output management
    void print(const std::string& message);
    void printInfo(const std::string& message);
    void printWarning(const std::string& message);
    void printError(const std::string& message);
    
    // Get command history
    const std::vector<std::string>& getCommandHistory() const;
    
    // Check if console should display
    bool shouldDisplay() const;
    
    // Logging integration
    void setLogger(class Logger* logger);
    
    // Visualization integration
    void setVisualizationEngine(class VisualizationEngine* engine);
    
    // Data export integration
    void setDataExporter(class DataExporter* exporter);
    
    // Performance profiling integration
    void setPerformanceProfiler(class PerformanceProfiler* profiler);
    
    // Get debug information
    std::string getDebugInfo() const;
    
    // Export configuration
    void exportConfiguration(const std::string& filepath) const;
    
    // Import configuration
    bool importConfiguration(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
