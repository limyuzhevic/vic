#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Experiment tool base
class ExperimentTool {
public:
    virtual ~ExperimentTool() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};

// Tool result
struct ToolResult {
    std::string toolName;
    std::string operation;
    bool success;
    std::map<std::string, std::string> resultData;
    std::chrono::system_clock::time_point timestamp;
    std::string errorMessage;
    
    ToolResult() : success(false), timestamp(std::chrono::system_clock::now()) {}
    ToolResult(const std::string& toolName, const std::string& operation, bool success)
        : toolName(toolName), operation(operation), success(success),
          timestamp(std::chrono::system_clock::now()) {}
};

class ExperimentTools {
public:
    ExperimentTools();
    ~ExperimentTools();
    
    // Disable copying, enable moving
    ExperimentTools(const ExperimentTools&) = delete;
    ExperimentTools& operator=(const ExperimentTools&) = delete;
    ExperimentTools(ExperimentTools&&) noexcept;
    ExperimentTools& operator=(ExperimentTools&&) noexcept;
    
    // Tool management
    bool registerTool(const std::string& name, std::shared_ptr<ExperimentTool> tool);
    bool unregisterTool(const std::string& name);
    bool updateTool(const std::string& name, std::shared_ptr<ExperimentTool> tool);
    std::shared_ptr<ExperimentTool> getTool(const std::string& name) const;
    std::vector<std::string> getAllToolNames() const;
    
    // Tool lifecycle
    bool initializeTool(const std::string& name, const Config& config);
    bool shutdownTool(const std::string& name);
    bool updateTool(const std::string& name, double timestep);
    bool enableTool(const std::string& name);
    bool disableTool(const std::string& name);
    
    // Tool operations
    ToolResult executeTool(const std::string& name, const std::map<std::string, std::string>& parameters);
    std::vector<ToolResult> executeAllTools(const std::map<std::string, std::map<std::string, std::string>>& parameters);
    bool configureTool(const std::string& name, const Config& config);
    Config getToolConfig(const std::string& name) const;
    
    // Tool status
    bool isToolEnabled(const std::string& name) const;
    bool isToolInitialized(const std::string& name) const;
    std::chrono::system_clock::time_point getToolLastUpdate(const std::string& name) const;
    std::string getToolStatus(const std::string& name) const;
    
    // Tool analysis
    std::map<std::string, double> getToolPerformance(const std::string& name) const;
    std::vector<std::string> getToolDependencies(const std::string& name) const;
    std::vector<std::string> getToolsByType(const std::string& type) const;
    
    // Tool backup and recovery
    bool backupToolData(const std::string& name, const std::string& backupPath) const;
    bool restoreToolData(const std::string& name, const std::string& backupPath);
    
    // Tool cleanup
    bool cleanupToolData(const std::string& name);
    bool cleanupAllToolData();
    
    // Tool system management
    bool enableToolMonitoring(const std::string& name, bool enable);
    bool isToolMonitoringEnabled(const std::string& name) const;
    bool enableRealTimeToolUpdates(bool enable);
    bool isRealTimeToolUpdatesEnabled() const;
    
    // Tool utilities
    static std::string generateToolName(const std::string& prefix = "tool") const;
    static bool isValidToolName(const std::string& name) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm