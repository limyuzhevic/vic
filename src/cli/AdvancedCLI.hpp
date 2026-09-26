// NLM Advanced CLI Interface
// Phase 2: Comprehensive Power User Interface

#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>
#include <vector>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class AdvancedCLI {
public:
    AdvancedCLI(int argc, char** argv);
    ~AdvancedCLI() = default;

    bool execute();
    void printHelp() const;
    void printVersion() const;

private:
    struct Command {
        std::string name;
        std::string description;
        std::function<void()> handler;
        std::string usage;
    };

    // Core initialization
    bool initializeCoreComponents();

    // Configuration Management Commands
    void handleConfigExport() const;
    void handleConfigImport() const;
    void handleConfigValidate() const;
    void handleConfigDiff() const;
    void handleConfigList() const;

    // Simulation Control Commands
    void handleSimulateCustom() const;
    void handleSimulateRealTime() const;
    void handleSimulateMonitor() const;
    void handleSimulateCheckpoint() const;
    void handleSimulateRestart() const;
    void handleSimulateSchedule() const;

    // Power User Feature Commands
    void handleDebugAdvanced() const;
    void handleProfiler() const;
    void handleNeuronDebug() const;
    void handleSynapseMonitor() const;
    void handleNetworkStats() const;

    // Brain State Management Commands
    void handleStateManual() const;
    void handleStatePattern() const;
    void handleStateTopology() const;
    void handleStateVisualize() const;
    void handleStateAnalysis() const;

    // Helper methods
    std::string getCurrentTimestamp() const;
    void logCommand(const std::string& command) const;
    bool saveConfigToFile(const std::string& filepath, const std::string& format) const;
    bool loadConfigFromFile(const std::string& filepath, const std::string& format);
    void printJSONConfig() const;
    void printYAMLConfig() const;
    void printTableConfig() const;
    std::string getValueAsString(const std::string& key) const;

    // Member variables
    int argc;
    char** argv;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<SimulationClock> clock;
    std::map<std::string, Command> commands;
    std::string currentCommand;
    bool verbose;
    bool quiet;

    // Register all commands
    void registerCommands();
};

}} // namespace nlm
