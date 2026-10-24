// CLI20-based Command Line Interface for NLM Neural Learning Machine
// Modern C++20 CLI with subcommands, JSON output, and advanced features

#ifndef CLI_HPP
#define CLI_HPP

#include "cli/argument.hh"
#include "cli/parser.hh"
#include "cli/subcommand.hh"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>
#include <thread>

// Forward declarations
class Brain;
class Config;
class Logger;

namespace nlm {
namespace cli {

// Configuration for NLM CLI
struct NLMConfig {
    std::string logLevel = "info";
    std::string logFile = "";
    bool verbose = false;
    bool quiet = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointDir = "./checkpoints";
    bool jsonOutput = false;
    bool humanReadable = true;
    std::chrono::milliseconds timeout{0};
    int maxSteps = 1000;
    bool debugMode = false;
    bool dryRun = false;
};

// Main CLI command structure
struct NLMCli {
    // Global options
    cli::Flag<"verbose", 'v"> verbose{ {.help = "Enable verbose logging"} };
    cli::Flag<"quiet", 'q"> quiet{ {.help = "Suppress output"} };
    cli::Flag<"json", 'j"> json{ {.help = "Output JSON"} };
    cli::StringOption<"config", 'c"> configFile{ {
        .help = "Configuration file path",
        .presence = cli::optional
    } };
    cli::StringOption<"output", 'o'> outputFile{ {
        .help = "Output file for results",
        .presence = cli::optional
    } };
    cli::Flag<"debug", 'd"> debugMode{ {.help = "Enable debug mode"} };
    cli::Flag<"dry-run"> dryRun{ {.help = "Dry run without executing"} };
    
    // Subcommands
    cli::Command<"brain", BrainCli> brain{ {.help = "Brain operations"} };
    cli::Command<"experiment", ExperimentCli> experiment{ {.help = "Experiment controls"} };
    cli::Command<"config", ConfigCli> config{ {.help = "Configuration management"} };
    cli::Command<"monitor", MonitorCli> monitor{ {.help = "Monitoring and statistics"} };
    cli::Command<"export", ExportCli> export{ {.help = "Export brain state"} };
    cli::Command<"import", ImportCli> import{ {.help = "Import brain state"} };
    cli::Command<"help", HelpCli> help{ {.help = "Show help information"} };
};

// Brain operations CLI
struct BrainCli {
    cli::StringOption<"name", 'n"> name{ {
        .help = "Brain name",
        .presence = cli::optional
    } };
    cli::IntOption<"neurons", 'N"> neuronCount{ {
        .help = "Number of neurons (default: 1000)",
        .default_value = 1000
    } };
    cli::IntOption<"regions", 'R"> regionCount{ {
        .help = "Number of regions (default: 1)",
        .default_value = 1
    } };
    cli::Flag<"plasticity"> enablePlasticity{ {.help = "Enable synaptic plasticity"} };
    cli::Flag<"stdp"> enableStdp{ {.help = "Enable STDP plasticity"} };
    cli::Flag<"hebbian"> enableHebbian{ {.help = "Enable Hebbian plasticity"} };
    cli::Flag<"structural"> enableStructural{ {.help = "Enable structural plasticity"} };
    cli::IntOption<"seed", 's"> randomSeed{ {
        .help = "Random seed (default: 42)",
        .default_value = 42
    } };
    cli::Flag<"run"> run{ {.help = "Create and run brain"} };
    cli::Flag<"reset"> reset{ {.help = "Create new brain (reset)"} };
};

// Experiment operations CLI
struct ExperimentCli {
    cli::StringOption<"name", 'n'> name{ {
        .help = "Experiment name",
        .presence = cli::optional
    } };
    cli::IntOption<"steps", 'S'> maxSteps{ {
        .help = "Maximum simulation steps",
        .default_value = 1000
    } };
    cli::StringOption<"type", 't'> experimentType{ {
        .help = "Experiment type (connectivity, plasticity, stdp, learning)",
        .presence = cli::optional
    } };
    cli::Flag<"detailed"> detailedResults{ {.help = "Generate detailed results"} };
    cli::StringOption<"results-dir"> resultsDir{ {
        .help = "Directory for experiment results",
        .presence = cli::optional
    } };
    cli::Flag<"run"> run{ {.help = "Run experiment"} };
};

// Configuration management CLI
struct ConfigCli {
    cli::StringOption<"key", 'k'> key{ {
        .help = "Configuration key",
        .presence = cli::optional
    } };
    cli::StringOption<"value", 'v'> value{ {
        .help = "Configuration value",
        .presence = cli::optional
    } };
    cli::Flag<"list"> listAll{ {.help = "List all configuration values"} };
    cli::Flag<"set"> setValue{ {.help = "Set configuration value"} };
    cli::Flag<"load"> loadFromFile{ {.help = "Load from configuration file"} };
    cli::Flag<"save"> saveToFile{ {.help = "Save to configuration file"} };
    cli::StringOption<"file"> configPath{ {
        .help = "Configuration file path",
        .presence = cli::optional
    } };
};

// Monitoring CLI
struct MonitorCli {
    cli::Flag<"stats"> showStats{ {.help = "Show brain statistics"} };
    cli::Flag<"activity"> showActivity{ {.help = "Show neural activity"} };
    cli::Flag<"plasticity"> showPlasticity{ {.help = "Show plasticity metrics"} };
    cli::Flag<"memory"> showMemory{ {.help = "Show memory usage"} };
    cli::IntOption<"refresh", 'r'> refreshInterval{ {
        .help = "Refresh interval in seconds",
        .default_value = 5
    } };
    cli::Flag<"live"> liveMode{ {.help = "Live monitoring mode"} };
    cli::StringOption<"format"> outputFormat{ {
        .help = "Output format (table, json, csv)",
        .default_value = "table"
    } };
};

// Export CLI
struct ExportCli {
    cli::StringOption<"file", 'f'> filePath{ {
        .help = "Export file path",
        .presence = cli::optional
    } };
    cli::Flag<"format", 'F'> exportFormat{ {.help = "Export format (checkpoint, json, csv)"} };
    cli::Flag<"neurons"> exportNeurons{ {.help = "Export neuron data"} };
    cli::Flag<"synapses"> exportSynapses{ {.help = "Export synapse data"} };
    cli::Flag<"plasticity"> exportPlasticity{ {.help = "Export plasticity data"} };
    cli::Flag<"memory"> exportMemory{ {.help = "Export memory data"} };
};

// Import CLI
struct ImportCli {
    cli::StringOption<"file", 'f'> filePath{ {
        .help = "Import file path",
        .presence = cli::optional
    } };
    cli::Flag<"format", 'F'> importFormat{ {.help = "Import format (checkpoint, json, csv)"} };
    cli::Flag<"neurons"> importNeurons{ {.help = "Import neuron data"} };
    cli::Flag<"synapses"> importSynapses{ {.help = "Import synapse data"} };
    cli::Flag<"plasticity"> importPlasticity{ {.help = "Import plasticity data"} };
    cli::Flag<"memory"> importMemory{ {.help = "Import memory data"} };
    cli::Flag<"overwrite"> overwrite{ {.help = "Overwrite existing data"} };
};

// Help CLI
struct HelpCli {
    cli::StringOption<"section"> section{ {
        .help = "Help section (brain, experiment, config, monitor)",
        .presence = cli::optional
    } };
    cli::Flag<"examples"> showExamples{ {.help = "Show usage examples"} };
    cli::Flag<"advanced"> advancedHelp{ {.help = "Show advanced help"} };
};

// Main CLI implementation
class NLMCommandLine {
public:
    NLMCommandLine() = default;
    ~NLMCommandLine() = default;
    
    // Parse command line arguments
    static std::pair<bool, std::string> parse(int argc, char* argv[]);
    
    // Execute parsed command
    bool execute();
    
    // Get configuration
    const NLMConfig& getConfig() const { return config_; }
    
    // Get help text
    static std::string getHelpText(const std::string& section = "");
    
private:
    NLMConfig config_;
    NLMCli cliArgs_;
    
    // Brain operations
    bool handleBrainCommand(const BrainCli& args);
    bool createBrain(const BrainCli& args);
    bool runBrain(const BrainCli& args);
    bool exploreBrain(const BrainCli& args);
    
    // Experiment operations
    bool handleExperimentCommand(const ExperimentCli& args);
    bool runExperiment(const ExperimentCli& args);
    bool createExperiment(const ExperimentCli& args);
    
    // Configuration management
    bool handleConfigCommand(const ConfigCli& args);
    bool listConfig(const ConfigCli& args);
    bool setConfig(const ConfigCli& args);
    bool loadConfig(const ConfigCli& args);
    bool saveConfig(const ConfigCli& args);
    
    // Monitoring
    bool handleMonitorCommand(const MonitorCli& args);
    bool showStats(const MonitorCli& args);
    bool startLiveMonitoring(const MonitorCli& args);
    
    // Export/Import
    bool handleExportCommand(const ExportCli& args);
    bool exportBrain(const ExportCli& args);
    bool handleImportCommand(const ImportCli& args);
    bool importBrain(const ImportCli& args);
    
    // Help
    bool handleHelpCommand(const HelpCli& args);
    void showGeneralHelp() const;
    void showBrainHelp() const;
    void showExperimentHelp() const;
    void showConfigHelp() const;
    void showMonitorHelp() const;
    void showExportImportHelp() const;
    
    // Output formatting
    void outputJson(const std::string& data) const;
    void outputHumanReadable(const std::string& data) const;
    void outputTable(const std::string& data) const;
    
    // Utility functions
    std::string formatTime(const std::chrono::system_clock::time_point& tp) const;
    std::string escapeJsonString(const std::string& str) const;
};

} // namespace cli
} // namespace nlm

#endif // CLI_HPP
