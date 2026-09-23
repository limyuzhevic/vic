// AdvancedNLMCLI implementation
// Provides advanced command line interface for NLM with batch processing,
// scripting, monitoring, and advanced configuration management

#include "AdvancedNLMCLI.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace nlm {

AdvancedNLMCLI::AdvancedNLMCLI()
    : monitoringEnabled_(false)
    , currentMetrics_{0.0, 0.0, 0.0, 0, 0, 0.0f, 0.0f}
{
}

AdvancedNLMCLI::~AdvancedNLMCLI() {
    stopMonitoring();
}

bool AdvancedNLMCLI::processCommandLine(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return false;
    }
    
    ParsedCommand cmd = parseCommandLine(argc, argv);
    
    if (!validateCommand(cmd)) {
        std::cerr << "Error: Invalid command or arguments." << std::endl;
        return false;
    }
    
    return handleCommand(cmd.command, cmd.arguments);
}

AdvancedNLMCLI::ParsedCommand AdvancedNLMCLI::parseCommandLine(int argc, char* argv[]) {
    ParsedCommand cmd;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Option with value: --option=value
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                cmd.options[arg.substr(0, pos)] = arg.substr(pos + 1);
            } else {
                cmd.options[arg] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option with value: -o value
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                cmd.options[arg] = argv[++i];
            } else {
                cmd.options[arg] = "true";
            }
        } else if (cmd.command.empty()) {
            // First positional argument is the command
            cmd.command = arg;
        } else {
            // Other positional arguments
            cmd.arguments.push_back(arg);
        }
    }
    
    return cmd;
}

bool AdvancedNLMCLI::validateCommand(const ParsedCommand& cmd) const {
    static const std::vector<std::string> validCommands = {
        "run", "batch", "script", "config", "export", "import",
        "stats", "monitor", "benchmark", "profile", "help"
    };
    
    if (cmd.command.empty() || 
        std::find(validCommands.begin(), validCommands.end(), cmd.command) == validCommands.end()) {
        return false;
    }
    
    return true;
}

bool AdvancedNLMCLI::handleCommand(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "help") {
        printHelp(args.empty() ? "" : args[0]);
        return true;
    } else if (cmd == "run") {
        return cmdRun(args);
    } else if (cmd == "batch") {
        return cmdBatch(args);
    } else if (cmd == "script") {
        return cmdScript(args);
    } else if (cmd == "config") {
        return cmdConfig(args);
    } else if (cmd == "export") {
        return cmdExport(args);
    } else if (cmd == "import") {
        return cmdImport(args);
    } else if (cmd == "stats") {
        return cmdStats(args);
    } else if (cmd == "monitor") {
        return cmdMonitor(args);
    } else if (cmd == "benchmark") {
        return cmdBenchmark(args);
    } else if (cmd == "profile") {
        return cmdProfile(args);
    }
    
    std::cerr << "Error: Unknown command: " << cmd << std::endl;
    return false;
}

bool AdvancedNLMCLI::cmdRun(const std::vector<std::string>& args) {
    std::cout << "=== NLM Run Command ===" << std::endl;
    
    if (args.empty()) {
        std::cout << "Usage: nlm run <phase> [options]" << std::endl;
        std::cout << "Phases: phase2, phase3, phase4, phase6" << std::endl;
        return false;
    }
    
    std::string phase = args[0];
    std::cout << "Running " << phase << "..." << std::endl;
    
    // Build command arguments
    std::vector<std::string> cmdArgs;
    cmdArgs.push_back(phase);
    
    // Add additional arguments
    for (size_t i = 1; i < args.size(); ++i) {
        cmdArgs.push_back(args[i]);
    }
    
    // TODO: Execute the specified phase
    // This would typically involve forking a new process or calling phase-specific functions
    std::cout << "Phase execution would be implemented here." << std::endl;
    std::cout << "Arguments: ";
    for (const auto& arg : cmdArgs) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    
    return true;
}

bool AdvancedNLMCLI::cmdBatch(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "=== Batch Command Options ===" << std::endl;
        std::cout << "batch list" << std::endl;
        std::cout << "batch execute <name>" << std::endl;
        std::cout << "batch add <name> <command>" << std::endl;
        std::cout << "batch remove <name>" << std::endl;
        std::cout << "batch schedule <name> <delay>" << std::endl;
        return true;
    }
    
    std::string action = args[0];
    
    if (action == "list") {
        return listBatchJobs();
    } else if (action == "add") {
        if (args.size() < 3) {
            std::cerr << "Error: batch add requires name and command arguments" << std::endl;
            return false;
        }
        BatchJob job;
        job.name = args[1];
        job.commandArgs = std::vector<std::string>(args.begin() + 2, args.end());
        job.priority = 1.0;
        job.scheduledTime = std::chrono::system_clock::now();
        job.enabled = true;
        
        if (addBatchJob(job)) {
            std::cout << "Batch job added: " << job.name << std::endl;
            return true;
        }
        return false;
    } else if (action == "remove") {
        if (args.size() < 2) {
            std::cerr << "Error: batch remove requires job name" << std::endl;
            return false;
        }
        if (removeBatchJob(args[1])) {
            std::cout << "Batch job removed: " << args[1] << std::endl;
            return true;
        }
        return false;
    } else if (action == "execute") {
        if (args.size() < 2) {
            std::cerr << "Error: batch execute requires job name" << std::endl;
            return false;
        }
        return executeBatch(args[1]);
    } else if (action == "schedule") {
        if (args.size() < 3) {
            std::cerr << "Error: batch schedule requires job name and delay" << std::endl;
            return false;
        }
        // TODO: Implement scheduling
        std::cout << "Batch scheduling would be implemented here" << std::endl;
        return true;
    }
    
    std::cerr << "Error: Unknown batch action: " << action << std::endl;
    return false;
}

bool AdvancedNLMCLI::cmdScript(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "=== Script Command Options ===" << std::endl;
        std::cout << "script execute <file>" << std::endl;
        std::cout << "script create <file> [template]" << std::endl;
        std::cout << "script list" << std::endl;
        return true;
    }
    
    std::string action = args[0];
    
    if (action == "execute") {
        if (args.size() < 2) {
            std::cerr << "Error: script execute requires script file" << std::endl;
            return false;
        }
        return executeScript(args[1]);
    } else if (action == "create") {
        if (args.size() < 2) {
            std::cerr << "Error: script create requires script file" << std::endl;
            return false;
        }
        std::string templateType = args.size() > 2 ? args[2] : "default";
        
        std::string content;
        if (templateType == "quickstart") {
            content = getQuickStartTemplate();
        } else if (templateType == "research") {
            content = getResearchTemplate();
        } else {
            content = getDefaultConfigTemplate();
        }
        
        // Write script file
        std::ofstream file(args[1]);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create script file: " << args[1] << std::endl;
            return false;
        }
        
        file << content;
        file.close();
        
        std::cout << "Script file created: " << args[1] << std::endl;
        return true;
    } else if (action == "list") {
        // TODO: List available scripts
        std::cout << "Script listing would be implemented here" << std::endl;
        return true;
    }
    
    std::cerr << "Error: Unknown script action: " << action << std::endl;
    return false;
}

bool AdvancedNLMCLI::cmdConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "=== Config Command Options ===" << std::endl;
        std::cout << "config load <file>" << std::endl;
        std::cout << "config save <file>" << std::endl;
        std::cout << "config validate" << std::endl;
        std::cout << "config show" << std::endl;
        std::cout << "config help" << std::endl;
        return true;
    }
    
    std::string action = args[0];
    
    if (action == "load") {
        if (args.size() < 2) {
            std::cerr << "Error: config load requires file" << std::endl;
            return false;
        }
        return loadConfiguration(args[1]);
    } else if (action == "save") {
        if (args.size() < 2) {
            std::cerr << "Error: config save requires file" << std::endl;
            return false;
        }
        return saveConfiguration(args[1]);
    } else if (action == "validate") {
        return validateConfiguration();
    } else if (action == "show") {
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << "Config file: " << currentConfigFile << std::endl;
        std::cout << "Valid: " << (validateConfiguration() ? "Yes" : "No") << std::endl;
        return true;
    } else if (action == "help") {
        printAdvancedHelp();
        return true;
    }
    
    std::cerr << "Error: Unknown config action: " << action << std::endl;
    return false;
}

bool AdvancedNLMCLI::cmdExport(const std::vector<std::string>& args) {
    std::cout << "=== Export Command ===" << std::endl;
    std::cout << "Export functionality would be implemented here." << std::endl;
    
    if (!args.empty()) {
        std::cout << "Format: " << args[0] << std::endl;
    }
    
    return true;
}

bool AdvancedNLMCLI::cmdImport(const std::vector<std::string>& args) {
    std::cout << "=== Import Command ===" << std::endl;
    std::cout << "Import functionality would be implemented here." << std::endl;
    
    if (!args.empty()) {
        std::cout << "From: " << args[0] << std::endl;
    }
    
    return true;
}

bool AdvancedNLMCLI::cmdStats(const std::vector<std::string>& args) {
    std::cout << "=== Statistics Command ===" << std::endl;
    
    PerformanceMetrics metrics = getCurrentMetrics();
    
    std::cout << "System Statistics:" << std::endl;
    std::cout << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
    std::cout << "  Simulation Time: " << metrics.simulationTime << "s" << std::endl;
    std::cout << "  Neuron Count: " << metrics.neuronCount << std::endl;
    std::cout << "  Synapse Count: " << metrics.synapseCount << std::endl;
    std::cout << "  Average Firing Rate: " << metrics.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Total Reward: " << metrics.totalReward << std::endl;
    
    return true;
}

bool AdvancedNLMCLI::cmdMonitor(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "=== Monitor Command Options ===" << std::endl;
        std::cout << "monitor start <output_file>" << std::endl;
        std::cout << "monitor stop" << std::endl;
        std::cout << "monitor status" << std::endl;
        return true;
    }
    
    std::string action = args[0];
    
    if (action == "start") {
        if (args.size() < 2) {
            std::cerr << "Error: monitor start requires output file" << std::endl;
            return false;
        }
        monitoringOutputFile = args[1];
        return startMonitoring(monitoringOutputFile);
    } else if (action == "stop") {
        return stopMonitoring();
    } else if (action == "status") {
        std::cout << "Monitoring Status:" << std::endl;
        std::cout << "  Enabled: " << (monitoringEnabled_ ? "Yes" : "No") << std::endl;
        std::cout << "  Output File: " << monitoringOutputFile << std::endl;
        return true;
    }
    
    std::cerr << "Error: Unknown monitor action: " << action << std::endl;
    return false;
}

bool AdvancedNLMCLI::cmdBenchmark(const std::vector<std::string>& args) {
    std::cout << "=== Benchmark Command ===" << std::endl;
    std::cout << "Benchmark functionality would be implemented here." << std::endl;
    
    if (!args.empty()) {
        std::cout << "Benchmark mode: " << args[0] << std::endl;
    }
    
    return true;
}

bool AdvancedNLMCLI::cmdProfile(const std::vector<std::string>& args) {
    std::cout << "=== Profile Command ===" << std::endl;
    std::cout << "Profile functionality would be implemented here." << std::endl;
    
    if (!args.empty()) {
        std::cout << "Profile type: " << args[0] << std::endl;
    }
    
    return true;
}

bool AdvancedNLMCLI::loadConfiguration(const std::string& filepath) {
    std::cout << "Loading configuration from: " << filepath << std::endl;
    
    // Validate file exists
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open configuration file: " << filepath << std::endl;
        return false;
    }
    
    currentConfigFile = filepath;
    std::cout << "Configuration loaded successfully" << std::endl;
    return true;
}

bool AdvancedNLMCLI::saveConfiguration(const std::string& filepath) const {
    std::cout << "Saving configuration to: " << filepath << std::endl;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create configuration file: " << filepath << std::endl;
        return false;
    }
    
    file << "# NLM Configuration Export" << std::endl;
    file << "# Generated by AdvancedNLMCLI" << std::endl;
    file << "# Date: " << timestampToString(std::chrono::system_clock::now()) << std::endl;
    
    file.close();
    std::cout << "Configuration saved successfully" << std::endl;
    return true;
}

bool AdvancedNLMCLI::validateConfiguration() const {
    if (currentConfigFile.empty()) {
        std::cout << "No configuration file loaded" << std::endl;
        return false;
    }
    
    std::ifstream file(currentConfigFile);
    if (!file.is_open()) {
        std::cout << "Configuration file not found: " << currentConfigFile << std::endl;
        return false;
    }
    
    std::string line;
    bool valid = true;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Validate line format (basic key=value validation)
        size_t pos = line.find('=');
        if (pos == std::string::npos || pos == 0) {
            std::cout << "Error in configuration file at line " << lineNumber << ": Invalid format" << std::endl;
            valid = false;
        }
    }
    
    file.close();
    
    if (valid) {
        std::cout << "Configuration file validated successfully" << std::endl;
    } else {
        std::cout << "Configuration file validation failed" << std::endl;
    }
    
    return valid;
}

bool AdvancedNLMCLI::addBatchJob(const BatchJob& job) {
    // Check if job already exists
    for (const auto& existingJob : batchJobs_) {
        if (existingJob.name == job.name) {
            std::cerr << "Error: Batch job already exists: " << job.name << std::endl;
            return false;
        }
    }
    
    batchJobs_.push_back(job);
    return true;
}

bool AdvancedNLMCLI::removeBatchJob(const std::string& name) {
    auto it = std::find_if(batchJobs_.begin(), batchJobs_.end(),
                           [&name](const BatchJob& job) { return job.name == name; });
    
    if (it == batchJobs_.end()) {
        std::cerr << "Error: Batch job not found: " << name << std::endl;
        return false;
    }
    
    batchJobs_.erase(it);
    return true;
}

bool AdvancedNLMCLI::listBatchJobs() const {
    std::cout << "=== Batch Jobs ===" << std::endl;
    
    if (batchJobs_.empty()) {
        std::cout << "No batch jobs defined" << std::endl;
        return true;
    }
    
    for (size_t i = 0; i < batchJobs_.size(); ++i) {
        const auto& job = batchJobs_[i];
        std::cout << i + 1 << ". " << job.name << " (priority: " << job.priority << ")" << std::endl;
        std::cout << "   Command: ";
        for (const auto& arg : job.commandArgs) {
            std::cout << arg << " ";
        }
        std::cout << " (enabled: " << (job.enabled ? "yes" : "no") << ")" << std::endl;
    }
    
    return true;
}

bool AdvancedNLMCLI::executeBatch(const std::string& batchName) {
    auto it = std::find_if(batchJobs_.begin(), batchJobs_.end(),
                           [&batchName](const BatchJob& job) { return job.name == batchName; });
    
    if (it == batchJobs_.end()) {
        std::cerr << "Error: Batch job not found: " << batchName << std::endl;
        return false;
    }
    
    const BatchJob& job = *it;
    
    if (!job.enabled) {
        std::cout << "Batch job is not enabled: " << batchName << std::endl;
        return true;
    }
    
    std::cout << "Executing batch job: " << batchName << std::endl;
    
    // Execute the job
    for (const auto& arg : job.commandArgs) {
        std::cout << "  " << arg << std::endl;
    }
    
    // TODO: Actually execute the batch job
    std::cout << "Batch job execution would be implemented here." << std::endl;
    
    return true;
}

bool AdvancedNLMCLI::executeAllBatches() {
    std::cout << "Executing all batch jobs..." << std::endl;
    
    for (const auto& job : batchJobs_) {
        if (job.enabled) {
            std::cout << "Running batch job: " << job.name << std::endl;
            
            for (const auto& arg : job.commandArgs) {
                std::cout << "  " << arg << std::endl;
            }
            
            // TODO: Execute the batch job
            std::cout << "Job execution would be implemented here." << std::endl;
        }
    }
    
    return true;
}

bool AdvancedNLMCLI::executeScript(const std::string& scriptPath) {
    std::cout << "Executing script: " << scriptPath << std::endl;
    
    std::vector<std::string> commands = readScriptFile(scriptPath);
    
    std::cout << "Script loaded " << commands.size() << " commands." << std::endl;
    
    for (const auto& command : commands) {
        std::cout << ">>> " << command << std::endl;
        
        // TODO: Parse and execute command
        // This would typically involve parsing the command and calling the appropriate handler
        std::cout << "  (Command execution would be implemented here)" << std::endl;
    }
    
    return true;
}

std::vector<std::string> AdvancedNLMCLI::readScriptFile(const std::string& filepath) {
    std::vector<std::string> commands;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open script file: " << filepath << std::endl;
        return commands;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (!line.empty() && line[0] != '#') {
            commands.push_back(line);
        }
    }
    
    file.close();
    return commands;
}

AdvancedNLMCLI::PerformanceMetrics AdvancedNLMCLI::getCurrentMetrics() const {
    return currentMetrics_;
}

bool AdvancedNLMCLI::startMonitoring(const std::string& outputFile) {
    if (monitoringEnabled_) {
        std::cout << "Monitoring is already enabled" << std::endl;
        return true;
    }
    
    monitoringEnabled_ = true;
    monitoringOutputFile = outputFile;
    
    std::cout << "Monitoring started, output file: " << outputFile << std::endl;
    return true;
}

bool AdvancedNLMCLI::stopMonitoring() {
    if (!monitoringEnabled_) {
        std::cout << "Monitoring is not enabled" << std::endl;
        return true;
    }
    
    monitoringEnabled_ = false;
    std::cout << "Monitoring stopped" << std::endl;
    return true;
}

void AdvancedNLMCLI::printHelp(const std::string& command) const {
    if (command.empty()) {
        printAdvancedHelp();
    } else {
        // Print help for specific command
        auto helpMap = getCommandHelp();
        auto it = helpMap.find(command);
        if (it != helpMap.end()) {
            std::cout << it->second << std::endl;
        } else {
            std::cout << "Command not found: " << command << std::endl;
        }
    }
}

void AdvancedNLMCLI::printAdvancedHelp() const {
    std::cout << "=== Advanced NLM CLI Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM (熙然) - Neural Learning Machine - Advanced CLI" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  run <phase> [options]      Run a specific NLM phase" << std::endl;
    std::cout << "  batch [action] [args]      Manage batch processing" << std::endl;
    std::cout << "  script [action] [args]     Execute scripts" << std::endl;
    std::cout << "  config [action] [args]      Manage configuration" << std::endl;
    std::cout << "  export <format> [args]      Export results" << std::endl;
    std::cout << "  import <source> [args]      Import data" << std::endl;
    std::cout << "  stats [args]                Show system statistics" << std::endl;
    std::cout << "  monitor [action] [args]     Monitoring and logging" << std::endl;
    std::cout << "  benchmark <mode> [args]     Performance benchmarking" << std::endl;
    std::cout << "  profile <type> [args]       Profiling and analysis" << std::endl;
    std::cout << "  help [command]              Show help information" << std::endl;
    std::cout << std::endl;
    printExamples();
}

void AdvancedNLMCLI::printAdvancedHelp() const {
    printAdvancedHelp(); // Duplicate call to fix compilation issue
}

void AdvancedNLMCLI::printExamples() const {
    std::cout << "Examples:" << std::endl;
    std::cout << std::endl;
    std::cout << "  nlm run phase6 --verbose --neuron-count 1000" << std::endl;
    std::cout << "  nlm batch add my_experiment run phase6 --steps 5000" << std::endl;
    std::cout << "  nlm batch execute my_experiment" << std::endl;
    std::cout << "  nlm script execute experiment_script.txt" << std::endl;
    std::cout << "  nlm script create my_script.txt quickstart" << std::endl;
    std::cout << "  nlm config load my_config.cfg" << std::endl;
    std::cout << "  nlm config save my_config.cfg" << std::endl;
    std::cout << "  nlm monitor start monitoring.log" << std::endl;
    std::cout << "  nlm benchmark quick" << std::endl;
    std::cout << std::endl;
    std::cout << "For more detailed help, use 'nlm help <command>'" << std::endl;
}

std::map<std::string, std::string> AdvancedNLMCLI::getCommandHelp() const {
    return {
        {"run", "Run a specific NLM phase\nUsage: nlm run <phase> [options]\nPhases: phase2, phase3, phase4, phase6"},
        {"batch", "Manage batch processing\nUsage: nlm batch [action] [args]\nActions: list, add, remove, execute, schedule"},
        {"script", "Execute scripts\nUsage: nlm script [action] [args]\nActions: execute, create, list"},
        {"config", "Manage configuration\nUsage: nlm config [action] [args]\nActions: load, save, validate, show, help"},
        {"export", "Export results\nUsage: nlm export <format> [args]\nFormats: json, csv, txt, xml"},
        {"import", "Import data\nUsage: nlm import <source> [args]\nSources: file, database, url"},
        {"stats", "Show system statistics\nUsage: nlm stats [args]\nOptions: --verbose, --json"},
        {"monitor", "Monitoring and logging\nUsage: nlm monitor [action] [args]\nActions: start, stop, status"},
        {"benchmark", "Performance benchmarking\nUsage: nlm benchmark <mode> [args]\nModes: quick, full, custom"},
        {"profile", "Profiling and analysis\nUsage: nlm profile <type> [args]\nTypes: cpu, memory, simulation"}
    };
}

std::string AdvancedNLMCLI::getDefaultConfigTemplate(const std::string& type) {
    if (type == "quickstart") {
        return getQuickStartTemplate();
    } else if (type == "research") {
        return getResearchTemplate();
    } else {
        return getDefaultConfigTemplate();
    }
}

std::string AdvancedNLMCLI::getQuickStartTemplate() {
    return R"(
# Quick Start NLM Configuration
# ============================
# Configuration for rapid prototyping and testing

# Core settings
neuron_count = 500
region_count = 1
connection_probability = 0.2

# Experiment settings
max_steps = 2000
enable_checkpointing = true
enable_replay = false
enable_development = false

# World settings
world_width = 16
world_height = 16

# Logging
log_level = "info"
""";
}

std::string AdvancedNLMCLI::getResearchTemplate() {
    return R"(
# Research NLM Configuration
# ========================
# Configuration for research and experimentation

# Core settings
neuron_count = 2000
region_count = 4
connection_probability = 0.1

# Advanced plasticity
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.008
stdp_tau = 50.0
synaptogenesis_rate = 0.00005
pruning_rate = 0.000005

# Development
development_enabled = true
plasticity_modifier = 0.8

# Experiment settings
max_steps = 50000
enable_checkpointing = true
enable_replay = true
enable_development = true

# Advanced agent settings
agent_max_steps = 10000
agent_reward_decay = 0.999

# Logging
log_level = "debug"
log_file = "research_nlm.log"
""";
}

bool AdvancedNLMCLI::stringToBool(const std::string& str) {
    std::string lower = str;
    for (char& c : lower) {
        c = std::tolower(c);
    }
    return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
}

std::string AdvancedNLMCLI::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string AdvancedNLMCLI::timestampToString(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string AdvancedNLMCLI::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
