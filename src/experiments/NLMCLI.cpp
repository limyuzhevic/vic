// NLM Advanced Command-Line Interface
// Provides advanced commands for controlling the NLM brain
// Supports batch operations, configuration management, and system exploration

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include <filesystem>
#include <iostream>
#include <unordered_map>

namespace nlm {

// Advanced CLI command parser and executor
class NLMCommandLineInterface {
public:
    enum class Command {
        None,
        Help,
        Version,
        CreateBrain,
        LoadBrain,
        SaveBrain,
        ExportConfig,
        ImportConfig,
        ListRegions,
        InspectRegion,
        ListNeurons,
        InspectNeuron,
        ListSynapses,
        InspectSynapse,
        RunTest,
        RunExperiment,
        SetParameter,
        GetParameter,
        ListParameters,
        ResetBrain,
        InitializeBrain,
        Step,
        Profile,
        Statistics,
        HelpCommand
    };

    struct ParsedCommand {
        Command command;
        std::vector<std::string> args;
        std::map<std::string, std::string> options;
    };

    NLMCommandLineInterface(std::shared_ptr<Config> config);
    ~NLMCommandLineInterface();

    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Execute parsed command
    bool executeCommand();
    
    // Get help text for command
    std::string getHelpText(Command command) const;
    
    // Get general help text
    std::string getGeneralHelp() const;
    
    // Set brain reference for commands that need it
    void setBrain(std::shared_ptr<Brain> brain);

private:
    // Command execution methods
    bool executeHelp(const ParsedCommand& cmd);
    bool executeVersion(const ParsedCommand& cmd);
    bool executeCreateBrain(const ParsedCommand& cmd);
    bool executeLoadBrain(const ParsedCommand& cmd);
    bool executeSaveBrain(const ParsedCommand& cmd);
    bool executeExportConfig(const ParsedCommand& cmd);
    bool executeImportConfig(const ParsedCommand& cmd);
    bool executeListRegions(const ParsedCommand& cmd);
    bool executeInspectRegion(const ParsedCommand& cmd);
    bool executeListNeurons(const ParsedCommand& cmd);
    bool executeInspectNeuron(const ParsedCommand& cmd);
    bool executeListSynapses(const ParsedCommand& cmd);
    bool executeInspectSynapse(const ParsedCommand& cmd);
    bool executeRunTest(const ParsedCommand& cmd);
    bool executeRunExperiment(const ParsedCommand& cmd);
    bool executeSetParameter(const ParsedCommand& cmd);
    bool executeGetParameter(const ParsedCommand& cmd);
    bool executeListParameters(const ParsedCommand& cmd);
    bool executeResetBrain(const ParsedCommand& cmd);
    bool executeInitializeBrain(const ParsedCommand& cmd);
    bool executeStep(const ParsedCommand& cmd);
    bool executeProfile(const ParsedCommand& cmd);
    bool executeStatistics(const ParsedCommand& cmd);

    // Helper methods
    Command parseCommand(const std::string& cmdStr) const;
    std::vector<std::string> tokenize(const std::string& input) const;
    std::map<std::string, std::string> parseOptions(const std::vector<std::string>& tokens) const;
    
    // Validation methods
    bool validateCommand(Command command, const std::vector<std::string>& args) const;
    
    // Brain operation helpers
    void listAllRegions() const;
    void inspectAllNeurons() const;
    void inspectAllSynapses() const;
    
    // Configuration helpers
    std::string formatConfigValue(const ConfigValue& value) const;
    
    // File operations
    bool saveConfigToFile(const std::string& filepath) const;
    bool loadConfigFromFile(const std::string& filepath);

    // State
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    ParsedCommand parsedCmd_;
    std::string inputFile_;
    std::string outputFile_;
    bool verbose_;
    bool quiet_;
    size_t steps_;
};

// Factory function for creating CLI instance
std::unique_ptr<NLMCommandLineInterface> createNLMCLI(std::shared_ptr<Config> config);

// Batch operation executor for running multiple commands
class NLMBatchExecutor {
public:
    struct BatchCommand {
        std::string command;
        std::vector<std::string> args;
        std::map<std::string, std::string> options;
        std::string description;
    };

    NLMBatchExecutor(std::shared_ptr<Config> config);
    ~NLMBatchExecutor();

    // Load batch commands from file
    bool loadBatchFile(const std::string& filepath);
    
    // Execute batch commands
    bool executeBatch();
    
    // Add command to batch
    void addCommand(const std::string& command, const std::vector<std::string>& args = {}, 
                   const std::map<std::string, std::string>& options = {});

    // Set brain reference
    void setBrain(std::shared_ptr<Brain> brain);

private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::vector<BatchCommand> commands_;
    bool dryRun_;
    bool verbose_;
};

// Configuration file formatter for advanced configuration
class ConfigFormatter {
public:
    enum class Format {
        Plain,
        JSON,
        YAML,
        XML,
        Python,
        CPlusPlus
    };

    // Format configuration to string
    static std::string format(const Config& config, Format format = Format::Plain);
    
    // Format configuration value
    static std::string formatValue(const ConfigValue& value, Format format = Format::Plain);
    
    // Parse configuration from string
    static bool parse(const std::string& input, Config& config, Format format = Format::Plain);
    
    // Get available formats
    static std::vector<std::string> getAvailableFormats();
};

// Brain state explorer for detailed inspection
class BrainExplorer {
public:
    BrainExplorer(std::shared_ptr<Brain> brain);
    ~BrainExplorer();

    // Generate brain state report
    std::string generateReport() const;
    
    // Generate JSON brain state
    std::string generateJSON() const;
    
    // Generate network statistics
    std::string generateNetworkStats() const;
    
    // Generate memory state report
    std::string generateMemoryReport() const;
    
    // Generate prediction state report
    std::string generatePredictionReport() const;
    
    // Generate neuromodulation report
    std::string generateNeuromodulationReport() const;

private:
    std::shared_ptr<Brain> brain_;
    std::string generateNeuronStats(RegionId regionId) const;
    std::string generateSynapseStats(RegionId regionId) const;
    std::string generateConnectivityStats() const;
    std::string generateActivityStats() const;
};

// Command line interface for Phase-specific operations
class PhaseCLI {
public:
    PhaseCLI(std::shared_ptr<Config> config);
    ~PhaseCLI();

    // Run Phase 3 operations
    void runPhase3() const;
    
    // Run Phase 4 operations
    void runPhase4() const;
    
    // Run Phase 5 operations
    void runPhase5() const;
    
    // Run Phase 6 operations
    void runPhase6() const;
    
    // Set brain reference
    void setBrain(std::shared_ptr<Brain> brain);

private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm