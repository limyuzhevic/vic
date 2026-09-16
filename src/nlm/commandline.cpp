// NLM Command Line Interface Implementation

#include "nlm/commandline.h"
#include "nlm/commandline_implementation.h"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

struct CommandLineInterface::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<ExperimentRunner> experimentRunner;
    std::vector<CLICommand> commands;
    std::string currentCommand;
    std::vector<std::string> parsedArgs;
    bool systemInitialized;
    std::chrono::high_resolution_clock::time_point startTime;
    
    Impl() : systemInitialized(false), startTime(std::chrono::high_resolution_clock::now()) {}
};

CommandLineInterface::CommandLineInterface() : pImpl(std::make_unique<Impl>()) {}
CommandLineInterface::~CommandLineInterface() {
    cleanup();
}

bool CommandLineInterface::initialize(const std::string& configFile) {
    try {
        pImpl->config = std::make_shared<Config>();
        
        // Try to load from file
        if (configFile != "configs/default.cfg" && pImpl->config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Initialize default values
        pImpl->config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        pImpl->config->set("simulation_timestep", 0.001, ConfigSource::Default);
        pImpl->config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        pImpl->config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        // Register all enhanced commands
        registerAllCommands();
        
        pImpl->systemInitialized = true;
        NLM_LOG_INFO("Command Line Interface initialized successfully.");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize CLI: " + std::string(e.what()));
        return false;
    }
}

void CommandLineInterface::cleanup() {
    if (pImpl->systemInitialized) {
        pImpl->brain.reset();
        pImpl->experimentRunner.reset();
        pImpl->config.reset();
        pImpl->systemInitialized = false;
        NLM_LOG_INFO("Command Line Interface cleaned up.");
    }
}

void CommandLineInterface::registerCommand(const CLICommand& command) {
    if (!pImpl->systemInitialized) {
        NLM_LOG_ERROR("CLI not initialized. Call initialize() first.");
        return;
    }
    pImpl->commands.push_back(command);
}

void CommandLineInterface::registerAllCommands() {
    // Enhanced configuration management commands
    registerCommand({
        "config-list",
        "List all configuration keys and their values",
        {"--keys-only", "--json"},
        ConfigCommands::configList
    });
    
    registerCommand({
        "config-export",
        "Export configuration to file",
        {"--format=json", "--format=yaml", "--format=xml"},
        ConfigCommands::configExport
    });
    
    registerCommand({
        "config-import",
        "Import configuration from file",
        {"--backup", "--merge"},
        ConfigCommands::configImport
    });
    
    registerCommand({
        "config-validate",
        "Validate current configuration",
        {"--strict", "--fix-errors"},
        ConfigCommands::configValidate
    });
    
    // Performance profiling and benchmarking
    registerCommand({
        "benchmark-run",
        "Run performance benchmarks",
        {"--neurons", "--steps", "--output-csv", "--detailed"},
        BenchmarkCommands::benchmarkRun
    });
    
    registerCommand({
        "benchmark-compare",
        "Compare benchmarks between configurations",
        {"--config1", "--config2", "--steps"},
        BenchmarkCommands::benchmarkCompare
    });
    
    registerCommand({
        "benchmark-profile",
        "Profile performance with detailed metrics",
        {"--duration", "--resolution", "--output-profile"},
        BenchmarkCommands::benchmarkProfile
    });
    
    // Advanced debugging and monitoring
    registerCommand({
        "debug-monitor",
        "Start real-time monitoring of neural simulation",
        {"--interval", "--output-file", "--verbose"},
        DebugCommands::debugMonitor
    });
    
    registerCommand({
        "debug-trace",
        "Trace execution with detailed logging",
        {"--level", "--neuron-id", "--trace-file"},
        DebugCommands::debugTrace
    });
    
    registerCommand({
        "debug-profile",
        "Profile simulation with performance metrics",
        {"--output", "--format"},
        DebugCommands::debugProfile
    });
    
    registerCommand({
        "debug-analyze",
        "Analyze brain state and patterns",
        {"--depth", "--neurons", "--output-analysis"},
        DebugCommands::debugAnalyze
    });
    
    registerCommand({
        "debug-inspect",
        "Inspect internal system components",
        {"--components", "--details"},
        DebugCommands::debugInspect
    });
    
    // Memory system inspection
    registerCommand({
        "memory-inspect",
        "Inspect memory systems",
        {"--working", "--episodic", "--associative", "--all"},
        MemoryCommands::memoryInspect
    });
    
    registerCommand({
        "memory-stats",
        "Get memory system statistics",
        {"--summary", "--detailed", "--compare"},
        MemoryCommands::memoryStats
    });
    
    registerCommand({
        "memory-compare",
        "Compare memory usage between runs",
        {"--baseline", "--output-diff"},
        MemoryCommands::memoryCompare
    });
    
    registerCommand({
        "memory-benchmark",
        "Benchmark memory operations",
        {"--iterations", "--output-metrics"},
        MemoryCommands::memoryBenchmark
    });
    
    // Neuromodulation analysis
    registerCommand({
        "neuromodulation-analyze",
        "Analyze neuromodulatory system",
        {"--dopamine", "--curiosity", "--novelty", "--prediction-error"},
        NeuromodulationCommands::neuromodulationAnalyze
    });
    
    registerCommand({
        "neuromodulation-profile",
        "Profile neuromodulation activity",
        {"--duration", "--output-profile"},
        NeuromodulationCommands::neuromodulationProfile
    });
    
    registerCommand({
        "neuromodulation-stats",
        "Get neuromodulation statistics",
        {"--summary", "--trend"},
        NeuromodulationCommands::neuromodulationStats
    });
    
    registerCommand({
        "neuromodulation-trace",
        "Trace neuromodulatory signals",
        {"--signals", "--output-trace"},
        NeuromodulationCommands::neuromodulationTrace
    });
    
    // Development stage management
    registerCommand({
        "development-status",
        "Get development stage information",
        {"--detailed", "--history"},
        DevelopmentCommands::developmentStatus
    });
    
    registerCommand({
        "development-stage",
        "Set or get development stage",
        {"--stage", "--auto-advance"},
        DevelopmentCommands::developmentStage
    });
    
    registerCommand({
        "development-run",
        "Run development simulations",
        {"--stage", "--iterations", "--output-progress"},
        DevelopmentCommands::developmentRun
    });
    
    registerCommand({
        "development-progress",
        "Track development progress",
        {"--milestone", "--output-report"},
        DevelopmentCommands::developmentProgress
    });
    
    // Experiment management
    registerCommand({
        "experiment-list",
        "List available experiments",
        {"--all", "--details"},
        ExperimentCommands::experimentList
    });
    
    registerCommand({
        "experiment-run",
        "Run a specific experiment",
        {"--name", "--iterations", "--output-results"},
        ExperimentCommands::experimentRun
    });
    
    registerCommand({
        "experiment-run-all",
        "Run all experiments",
        {"--output-comparison", "--skip-existing"},
        ExperimentCommands::experimentRunAll
    });
    
    registerCommand({
        "experiment-status",
        "Get experiment status",
        {"--all", "--verbose"},
        ExperimentCommands::experimentStatus
    });
    
    registerCommand({
        "experiment-save",
        "Save experiment results",
        {"--format", "--compression"},
        ExperimentCommands::experimentSave
    });
    
    registerCommand({
        "experiment-compare",
        "Compare experiment results",
        {"--experiments", "--metric"},
        ExperimentCommands::experimentCompare
    });
    
    registerCommand({
        "experiment-analyze",
        "Analyze experiment results",
        {"--metrics", "--output-analysis"},
        ExperimentCommands::experimentAnalyze
    });
    
    // Visualization and output generation
    registerCommand({
        "visualization-generate",
        "Generate visualizations",
        {"--type", "--output", "--duration"},
        VisualizationCommands::visualizationGenerate
    });
    
    registerCommand({
        "visualization-export",
        "Export visualizations to files",
        {"--formats", "--output-dir", "--include-all"},
        VisualizationCommands::visualizationExport
    });
    
    registerCommand({
        "visualization-stats",
        "Get visualization statistics",
        {"--all", "--performance"},
        VisualizationCommands::visualizationStats
    });
    
    registerCommand({
        "visualization-animate",
        "Create animated visualizations",
        {"--fps", "--duration", "--output"},
        VisualizationCommands::visualizationAnimate
    });
}

bool CommandLineInterface::parse(int argc, char** argv) {
    if (!pImpl->systemInitialized) {
        NLM_LOG_ERROR("CLI not initialized. Call initialize() first.");
        return false;
    }
    
    pImpl->currentCommand.clear();
    pImpl->parsedArgs.clear();
    
    if (argc < 2) {
        printAllHelp();
        return false;
    }
    
    // Parse command
    pImpl->currentCommand = argv[1];
    
    // Parse flags and arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--") == 0) {
            pImpl->parsedArgs.push_back(arg);
        } else {
            pImpl->parsedArgs.push_back(arg);
        }
    }
    
    return true;
}

bool CommandLineInterface::execute(const std::string& commandName) {
    if (!pImpl->systemInitialized) {
        NLM_LOG_ERROR("CLI not initialized. Call initialize() first.");
        return false;
    }
    
    // Find the command
    auto it = std::find_if(pImpl->commands.begin(), pImpl->commands.end(),
        [&commandName](const CLICommand& cmd) {
            return cmd.name == commandName;
        });
    
    if (it == pImpl->commands.end()) {
        NLM_LOG_ERROR("Unknown command: " + commandName);
        printAllHelp();
        return false;
    }
    
    const CLICommand& command = *it;
    NLM_LOG_INFO("Executing command: " + commandName);
    
    // Initialize brain if needed for most commands
    if (!pImpl->brain) {
        if (!initializeSystem("configs/default.cfg", pImpl->brain, pImpl->experimentRunner)) {
            NLM_LOG_ERROR("Failed to initialize brain system.");
            return false;
        }
    }
    
    // Execute the command
    return command.execute(*pImpl->config, pImpl->parsedArgs);
}

void CommandLineInterface::printCommandHelp(const std::string& commandName) const {
    if (!pImpl->systemInitialized) {
        NLM_LOG_ERROR("CLI not initialized. Call initialize() first.");
        return;
    }
    
    auto it = std::find_if(pImpl->commands.begin(), pImpl->commands.end(),
        [&commandName](const CLICommand& cmd) {
            return cmd.name == commandName;
        });
    
    if (it == pImpl->commands.end()) {
        NLM_LOG_ERROR("Unknown command: " + commandName);
        return;
    }
    
    const CLICommand& command = *it;
    
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ Command: " << std::setw(60) << command.name << " ║" << std::endl;
    std::cout << "║ Description: " << std::setw(59) << command.description << " ║" << std::endl;
    std::cout << "║ Flags: " << std::endl;
    for (const auto& flag : command.flags) {
        std::cout << "║   - " << flag << std::endl;
    }
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

void CommandLineInterface::printAllHelp() const {
    if (!pImpl->systemInitialized) {
        NLM_LOG_ERROR("CLI not initialized. Call initialize() first.");
        return;
    }
    
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                       NLM Enhanced Command Line Interface                     ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl << std::endl;
    
    std::cout << "Available Commands:" << std::endl << std::endl;
    
    for (const auto& command : pImpl->commands) {
        std::cout << "  " << std::left << std::setw(25) << command.name << " - " << command.description << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Use 'nlm --help <command>' for detailed information about a specific command." << std::endl;
    std::cout << "Use 'nlm --version' for version information." << std::endl;
}

void CommandLineInterface::printVersion() const {
    std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cout << "C++ Standard: C++20" << std::endl;
}

void CommandLineInterface::printConfigSummary(const Config& config) const {
    std::cout << config.summary() << std::endl;
}

std::shared_ptr<Brain> CommandLineInterface::getBrain() const {
    return pImpl->brain;
}

std::shared_ptr<ExperimentRunner> CommandLineInterface::getExperimentRunner() const {
    return pImpl->experimentRunner;
}

std::shared_ptr<const Config> CommandLineInterface::getConfig() const {
    return pImpl->config;
}

std::string CommandLineInterface::getCurrentCommand() const {
    return pImpl->currentCommand;
}

const std::vector<std::string>& CommandLineInterface::getParsedArgs() const {
    return pImpl->parsedArgs;
}

} // namespace nlm
