#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Command-line interface for NLM simulation control
 * 
 * Provides programmatic control over simulation parameters,
 * debugging capabilities, and performance monitoring.
 */
class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Initialize CLI with configuration and brain
    bool initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    
    // Parse and execute command
    bool parseCommand(const std::string& commandLine);
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Command handlers
    bool handleHelp(const std::vector<std::string>& args);
    bool handleSet(const std::vector<std::string>& args);
    bool handleGet(const std::vector<std::string>& args);
    bool handleRun(const std::vector<std::string>& args);
    bool handleStats(const std::vector<std::string>& args);
    bool handleSave(const std::vector<std::string>& args);
    bool handleLoad(const std::vector<std::string>& args);
    bool handleReset(const std::vector<std::string>& args);
    bool handleStep(const std::vector<std::string>& args);
    bool handleExit(const std::vector<std::string>& args);
    
    // Simulation control
    bool runSimulation(size_t steps, double timestep = 0.001);
    bool stepSimulation(size_t steps = 1);
    
    // Configuration management
    bool setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    bool saveConfigToFile(const std::string& filepath) const;
    bool loadConfigFromFile(const std::string& filepath);
    
    // Simulation state management
    void resetSimulation();
    void printSimulationStatus() const;
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    void printPerformanceStats() const;
    
    // Interactive mode
    void startInteractiveMode();
    std::string getPrompt() const;
    
    // Command history
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory() const;
    
    // Event handling
    void registerCommandHandler(const std::string& command, 
                               std::function<bool(const std::vector<std::string>&)> handler);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
