#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include "../brain/Brain.hpp"

namespace nlm {

// Advanced simulation mode
enum class SimulationMode {
    RealTime,        // Step-by-step with real-time pacing
    Accelerated,     // Fast simulation for testing
    SlowMotion,      // Detailed observation
    EventDriven,     // Only step when activity changes
    Deterministic    // Same seed produces same results
};

// Simulation configuration
struct AdvancedConfig {
    SimulationMode mode;
    double timestep;
    bool enableLogging;
    bool enableProfiling;
    bool saveCheckpoints;
    std::string checkpointPath;
    double checkpointInterval;
    bool validateConfiguration;
    bool exportData;
    std::string outputFormat;
    
    AdvancedConfig() 
        : mode(SimulationMode::RealTime)
        , timestep(0.001)
        , enableLogging(true)
        , enableProfiling(false)
        , saveCheckpoints(false)
        , checkpointPath(".")
        , checkpointInterval(1000.0)
        , validateConfiguration(true)
        , exportData(false)
        , outputFormat("csv") {}
};

// Command handler interface
class CommandHandler {
public:
    virtual ~CommandHandler() = default;
    virtual bool handleCommand(const std::string& command, const std::vector<std::string>& args) = 0;
    virtual void displayHelp() const = 0;
};

// Advanced simulation manager
class AdvancedSimulationManager {
public:
    AdvancedSimulationManager(std::shared_ptr<Brain> brain);
    ~AdvancedSimulationManager();
    
    // Run simulation with advanced features
    bool runSimulation(const AdvancedConfig& config);
    
    // Command processing
    bool processCommand(const std::string& commandLine);
    void displayCommands() const;
    
    // Configuration management
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    void validateConfiguration() const;
    
    // Performance analysis
    void startProfiling();
    void stopProfiling();
    void displayProfileStats() const;
    
    // Data export
    void exportSimulationData(const std::string& format) const;
    
    // Checkpoint management
    bool createCheckpoint(const std::string& name);
    bool loadCheckpoint(const std::string& name);
    void listCheckpoints() const;
    
    // Statistics and analysis
    void displayNetworkStats() const;
    void displayPlasticityStats() const;
    void displayLearningStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Advanced command handlers
class SimulationCommandHandler : public CommandHandler {
public:
    SimulationCommandHandler(std::shared_ptr<AdvancedSimulationManager> manager);
    
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    void displayHelp() const override;
    
private:
    std::shared_ptr<AdvancedSimulationManager> manager_;
};

class CheckpointCommandHandler : public CommandHandler {
public:
    CheckpointCommandHandler(std::shared_ptr<AdvancedSimulationManager> manager);
    
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    void displayHelp() const override;
    
private:
    std::shared_ptr<AdvancedSimulationManager> manager_;
};

class AnalysisCommandHandler : public CommandHandler {
public:
    AnalysisCommandHandler(std::shared_ptr<AdvancedSimulationManager> manager);
    
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    void displayHelp() const override;
    
private:
    std::shared_ptr<AdvancedSimulationManager> manager_;
};

class ExportCommandHandler : public CommandHandler {
public:
    ExportCommandHandler(std::shared_ptr<AdvancedSimulationManager> manager);
    
    bool handleCommand(const std::string& command, const std::vector<std::string>& args) override;
    void displayHelp() const override;
    
private:
    std::shared_ptr<AdvancedSimulationManager> manager_;
};

// Command-line interface
class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    bool initialize(std::shared_ptr<AdvancedSimulationManager> manager);
    void run();
    void stop();
    
    void displayWelcome() const;
    void displayPrompt() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
