#include "NLMCLI.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <stdexcept>

namespace nlm {

// CLIParser Implementation

CLIParser::CLIParser(const std::string& programName) 
    : programName_(programName), currentCommand_(nullptr) {}

void CLIParser::addCommand(const std::string& name, const std::string& description,
                          const std::string& usage,
                          std::function<void(const std::map<std::string, std::string>&)> handler,
                          const std::map<std::string, std::string>& defaults,
                          const std::vector<std::string>& required) {
    commands_[name] = Command{name, description, usage, handler, defaults, required};
}

bool CLIParser::parse(int argc, char* argv[]) {
    if (argc < 2) {
        currentCommand_ = nullptr;
        return false;
    }
    
    std::string commandName = argv[1];
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        std::cerr << "Error: Unknown command '" << commandName << "'\n";
        showHelp();
        return false;
    }
    
    currentCommand_ = &it->second;
    
    // Parse key=value arguments
    parseArgs(argc, argv);
    
    // Validate required arguments
    for (const auto& required : currentCommand_->requiredArgs) {
        if (parsedArgs_.find(required) == parsedArgs_.end()) {
            std::cerr << "Error: Required argument --" << required << " missing for command '" 
                     << commandName << "'\n";
            showCommandHelp(commandName);
            return false;
        }
    }
    
    return true;
}

void CLIParser::showHelp() const {
    std::cout << "Usage: " << programName_ << " <command> [options]\n\n";
    std::cout << "Available commands:\n";
    
    for (const auto& pair : commands_) {
        const auto& cmd = pair.second;
        std::cout << "  " << std::left << std::setw(20) << pair.first 
                 << "- " << cmd.description << "\n";
    }
    
    std::cout << "\nUse \"" << programName_ << " <command> --help\" for detailed help on a command.\n";
}

void CLIParser::showCommandHelp(const std::string& commandName) const {
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        std::cerr << "Error: Unknown command '" << commandName << "'\n";
        return;
    }
    
    const auto& cmd = it->second;
    std::cout << "Usage: " << programName_ << " " << commandName << " " << cmd.usage << "\n\n";
    std::cout << cmd.description << "\n\n";
    
    if (!cmd.defaultArgs.empty()) {
        std::cout << "Default arguments:\n";
        for (const auto& pair : cmd.defaultArgs) {
            std::cout << "  --" << pair.first << " = " << pair.second << "\n";
        }
    }
    
    if (!cmd.requiredArgs.empty()) {
        std::cout << "Required arguments:\n";
        for (const auto& arg : cmd.requiredArgs) {
            std::cout << "  --" << arg << "\n";
        }
    }
}

void CLIParser::parseArgs(int argc, char* argv[]) {
    parsedArgs_.clear();
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parsedArgs_[key] = value;
            } else {
                // Handle boolean flags
                parsedArgs_[arg.substr(2)] = "true";
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                parsedArgs_[key] = value;
            } else {
                std::cerr << "Error: Missing value for option '" << arg << "'\n";
                exit(1);
            }
        }
    }
    
    // Apply defaults
    for (const auto& pair : currentCommand_->defaultArgs) {
        if (parsedArgs_.find(pair.first) == parsedArgs_.end()) {
            parsedArgs_[pair.first] = pair.second;
        }
    }
}

void CLIParser::printUsage() const {
    std::cout << "Usage: " << programName_ << " <command> [options]\n";
}

// AdvancedConfig Implementation

bool AdvancedConfig::loadYAML(const std::string& filepath, std::shared_ptr<Config> config) {
    if (filepath.empty() || !config) {
        return false;
    }
    
    // For now, use the existing file parser
    return config->loadFromFile(filepath);
}

bool AdvancedConfig::saveYAML(const std::string& filepath, const std::shared_ptr<Config>& config) {
    if (filepath.empty() || !config) {
        return false;
    }
    
    return config->saveToFile(filepath);
}

bool AdvancedConfig::loadJSON(const std::string& filepath, std::shared_ptr<Config> config) {
    // Placeholder for JSON parsing
    // For now, redirect to file loading
    return loadYAML(filepath, config);
}

bool AdvancedConfig::saveJSON(const std::string& filepath, const std::shared_ptr<Config>& config) {
    // Placeholder for JSON saving
    // For now, redirect to YAML saving
    return saveYAML(filepath, config);
}

bool AdvancedConfig::validateConfig(const std::shared_ptr<Config>& config, std::vector<std::string>& errors) {
    if (!config) {
        errors.push_back("Config object is null");
        return false;
    }
    
    errors.clear();
    
    // Basic validation - check for required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count"};
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate numeric ranges
    if (config->has("neuron_count")) {
        auto val = config->get<size_t>("neuron_count");
        if (val < 1 || val > 1000000) {
            errors.push_back("neuron_count must be between 1 and 1,000,000");
        }
    }
    
    if (config->has("region_count")) {
        auto val = config->get<size_t>("region_count");
        if (val < 1 || val > 100) {
            errors.push_back("region_count must be between 1 and 100");
        }
    }
    
    if (config->has("connection_probability")) {
        auto val = config->get<float>("connection_probability");
        if (val < 0.0f || val > 1.0f) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return errors.empty();
}

bool AdvancedConfig::applyConfigPresets(std::shared_ptr<Config> config, const std::string& preset) {
    if (!config) {
        return false;
    }
    
    // Clear existing configuration
    config->clear();
    
    // Apply preset
    if (preset == "small") {
        config->set("neuron_count", size_t(100));
        config->set("region_count", size_t(1));
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", uint64_t(42));
    } else if (preset == "medium") {
        config->set("neuron_count", size_t(1000));
        config->set("region_count", size_t(2));
        config->set("connection_probability", 0.05f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", uint64_t(42));
    } else if (preset == "large") {
        config->set("neuron_count", size_t(10000));
        config->set("region_count", size_t(4));
        config->set("connection_probability", 0.01f);
        config->set("simulation_timestep", 0.0001);
        config->set("random_seed", uint64_t(42));
    } else {
        return false; // Unknown preset
    }
    
    return true;
}

// Benchmarker Implementation

Benchmarker::BenchmarkResults Benchmarker::runBenchmark(const std::shared_ptr<Config>& config, 
                                                         int durationSteps,
                                                         int warmupSteps) {
    BenchmarkResults results;
    results.timeSeconds = 0.0;
    results.neurons = 0;
    results.synapses = 0;
    results.spikes = 0;
    results.averageFiringRate = 0.0;
    results.details = "Benchmark results not yet implemented";
    
    return results;
}

void Benchmarker::compareConfigurations(const std::vector<std::shared_ptr<Config>>& configs,
                                       const std::string& benchmarkName) {
    // Placeholder for configuration comparison
    std::cout << "Configuration comparison: " << benchmarkName << "\n";
    std::cout << "Not yet implemented\n";
}

// Visualizer Implementation

void Visualizer::generateNeuralSnapshot(const std::shared_ptr<Config>& config, 
                                       const std::string& outputFile,
                                       const std::string& format) {
    std::cout << "Generating neural snapshot: " << outputFile << " (format: " << format << ")\n";
    std::cout << "Not yet implemented\n";
}

void Visualizer::createNetworkGraph(const std::shared_ptr<Config>& config,
                                   const std::string& outputFile,
                                   bool includeWeights) {
    std::cout << "Creating network graph: " << outputFile << " (include_weights: " << includeWeights << ")\n";
    std::cout << "Not yet implemented\n";
}

void Visualizer::generateStatisticsReport(const std::shared_ptr<Config>& config,
                                         const std::string& outputFile,
                                         const std::string& format) {
    std::cout << "Generating statistics report: " << outputFile << " (format: " << format << ")\n";
    std::cout << "Not yet implemented\n";
}

// NLMApplication Implementation

NLMApplication::NLMApplication(const std::string& programName) 
    : programName_(programName), parser_(programName), config_(nullptr), 
      brain_(nullptr), isProfiling_(false), startTime_(0.0) {
    setupCommands();
}

void NLMApplication::setupCommands() {
    // Run command - execute simulation
    parser_.addCommand("run", "Run simulation with current configuration",
                      "[--steps N] [--episodes N] [--preset NAME]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleRunCommand(args);
                      },
                      {{}, {"steps", "episodes"}});
    
    // Config command - manage configuration
    parser_.addCommand("config", "Manage configuration files and presets",
                      "<command> [--file FILE] [--key VALUE] [--preset NAME]",
                      [this](const std::map<std::string, std::string>& args) {
                          // This command has subcommands, handled in handleConfigCommand
                          this->handleConfigCommand(args);
                      });
    
    // Benchmark command - run performance benchmarks
    parser_.addCommand("benchmark", "Run performance benchmarks",
                      "[--duration STEPS] [--warmup STEPS] [--compare]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleBenchmarkCommand(args);
                      });
    
    // Save command - save brain state
    parser_.addCommand("save", "Save brain state to file",
                      "--file FILE [--format FORMAT]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleSaveCommand(args);
                      },
                      {{}, {"file"}});
    
    // Load command - load brain state
    parser_.addCommand("load", "Load brain state from file",
                      "--file FILE [--format FORMAT]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleLoadCommand(args);
                      },
                      {{}, {"file"}});
    
    // Profile command - start/stop profiling
    parser_.addCommand("profile", "Start/stop performance profiling",
                      "[on|off|--output FILE]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleProfileCommand(args);
                      });
    
    // Stats command - show statistics
    parser_.addCommand("stats", "Show current statistics and metrics",
                      "[--detailed]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleStatsCommand(args);
                      });
    
    // Help command - show help
    parser_.addCommand("help", "Show help information",
                      "[command]",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleHelpCommand(args);
                      });
    
    // Version command - show version info
    parser_.addCommand("version", "Show version and build information",
                      "",
                      [this](const std::map<std::string, std::string>& args) {
                          this->handleVersionCommand(args);
                      });
}

int NLMApplication::run(int argc, char* argv[]) {
    if (!parser_.parse(argc, argv)) {
        return 1;
    }
    
    const auto* cmd = parser_.getCurrentCommand();
    if (!cmd) {
        parser_.showHelp();
        return 1;
    }
    
    try {
        cmd->handler(parser_.getArgs());
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

bool NLMApplication::loadConfig(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    config_ = std::make_shared<Config>();
    
    // Determine file format from extension
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    
    if (ext == "yaml" || ext == "yml") {
        return AdvancedConfig::loadYAML(filepath, config_);
    } else if (ext == "json") {
        return AdvancedConfig::loadJSON(filepath, config_);
    } else {
        // Default to simple key=value format
        return config_->loadFromFile(filepath);
    }
}

bool NLMApplication::saveConfig(const std::string& filepath) {
    if (filepath.empty() || !config_) {
        return false;
    }
    
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    
    if (ext == "yaml" || ext == "yml") {
        return AdvancedConfig::saveYAML(filepath, config_);
    } else if (ext == "json") {
        return AdvancedConfig::saveJSON(filepath, config_);
    } else {
        // Default to simple key=value format
        return config_->saveToFile(filepath);
    }
}

void NLMApplication::printConfig() const {
    if (!config_) {
        std::cout << "No configuration loaded.\n";
        return;
    }
    
    std::cout << config_->summary() << "\n";
}

bool NLMApplication::createBrain() {
    if (!config_) {
        std::cerr << "Error: No configuration loaded.\n";
        return false;
    }
    
    brain_ = std::make_shared<Brain>(config_);
    if (!brain_->initialize()) {
        std::cerr << "Error: Failed to initialize brain.\n";
        return false;
    }
    
    return true;
}

void NLMApplication::runSimulation(int steps, int episodes) {
    ensureBrainInitialized();
    
    std::cout << "Running simulation for " << steps << " steps, " 
              << episodes << " episodes...\n";
    
    for (int episode = 0; episode < episodes; ++episode) {
        std::cout << "Episode " << episode << ":\n";
        
        for (int step = 0; step < steps; ++step) {
            brain_->step(step);
            
            if (step % 100 == 0) {
                std::cout << "  Step " << step << ", "
                         << brain_->getFiringNeuronCount() << " firing\n";
            }
        }
        
        std::cout << "  Completed.\n";
    }
    
    std::cout << "Simulation completed.\n";
}

void NLMApplication::saveBrainState(const std::string& filepath) const {
    ensureBrainInitialized();
    
    if (!brain_->save(filepath)) {
        throw std::runtime_error("Failed to save brain state");
    }
    
    std::cout << "Brain state saved to " << filepath << "\n";
}

void NLMApplication::loadBrainState(const std::string& filepath) {
    ensureBrainInitialized();
    
    if (!brain_->load(filepath)) {
        throw std::runtime_error("Failed to load brain state");
    }
    
    std::cout << "Brain state loaded from " << filepath << "\n";
}

void NLMApplication::runPhase3Demo(int episodes, int steps) {
    Phase3Demo demo;
    demo.run(episodes, steps);
}

void NLMApplication::runPhase4Demo(int trials) {
    Phase4Demo demo;
    demo.run(trials);
}

void NLMApplication::runPhase6Demo(int steps) {
    Phase6Demo demo;
    demo.run(steps);
}

void NLMApplication::startProfiling() const {
    isProfiling_ = true;
    startTime_ = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
    std::cout << "Profiling started.\n";
}

void NLMApplication::stopProfiling() const {
    if (!isProfiling_) {
        return;
    }
    
    double endTime = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
    double duration = endTime - startTime_;
    
    std::cout << "Profiling stopped. Duration: " << duration << " seconds\n";
    isProfiling_ = false;
}

void NLMApplication::showPerformanceStats() const {
    std::cout << "Performance Statistics:\n";
    std::cout << "  Profiling active: " << (isProfiling_ ? "Yes" : "No") << "\n";
    
    if (brain_) {
        std::cout << "  Brain state:\n";
        brain_->logStatus();
    }
}

void NLMApplication::showVersion() const {
    std::cout << "NLM (熙然) - Neural Learning Machine\n";
    std::cout << "Version: 0.1.0\n";
    std::cout << "Phase 6: Final Integration\n";
    std::cout << "\nExperimental Artificial Developmental Brain\n";
}

void NLMApplication::showBuildInfo() const {
    std::cout << "Build Information:\n";
    std::cout << "  C++ Standard: C++20\n";
    std::cout << "  Build Type: Release\n";
    std::cout << "  Platform: Linux\n";
    std::cout << "  Architecture: x86_64\n";
}

void NLMApplication::showSupportedFormats() const {
    std::cout << "Supported Configuration Formats:\n";
    std::cout << "  YAML (.yaml, .yml) - Human-readable, structured format\n";
    std::cout << "  JSON (.json) - Web-friendly, widely supported\n";
    std::cout << "  Key=value (.cfg) - Simple text format\n";
    
    std::cout << "\nSupported File Formats:\n";
    std::cout << "  Brain state: Binary checkpoint format\n";
    std::cout << "  Statistics: TXT, CSV, JSON\n";
    std::cout << "  Visualizations: PNG, SVG\n";
}

// Command handlers implementation

void NLMApplication::handleRunCommand(const std::map<std::string, std::string>& args) {
    // Parse arguments
    int steps = 1000;
    int episodes = 1;
    std::string preset = "medium";
    
    auto it = args.find("steps");
    if (it != args.end()) {
        steps = std::stoi(it->second);
    }
    
    it = args.find("episodes");
    if (it != args.end()) {
        episodes = std::stoi(it->second);
    }
    
    it = args.find("preset");
    if (it != args.end()) {
        preset = it->second;
    }
    
    // Apply preset if provided
    if (!preset.empty() && preset != "medium") {
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
        AdvancedConfig::applyConfigPresets(config_, preset);
    }
    
    // Run simulation
    runSimulation(steps, episodes);
}

void NLMApplication::handleConfigCommand(const std::map<std::string, std::string>& args) {
    // This would have subcommands for config management
    // For now, just print basic info
    std::cout << "Configuration Management Command\n";
    std::cout << "Available subcommands: list, load, save, set, get, preset\n";
    std::cout << "Use 'nlm config --help' for more information.\n";
}

void NLMApplication::handleBenchmarkCommand(const std::map<std::string, std::string>& args) {
    // Parse arguments
    int duration = 1000;
    int warmup = 100;
    bool compare = false;
    
    auto it = args.find("duration");
    if (it != args.end()) {
        duration = std::stoi(it->second);
    }
    
    it = args.find("warmup");
    if (it != args.end()) {
        warmup = std::stoi(it->second);
    }
    
    it = args.find("compare");
    if (it != args.end() && (it->second == "true" || it->second == "1")) {
        compare = true;
    }
    
    std::cout << "Running benchmark with duration=" << duration 
              << ", warmup=" << warmup << ", compare=" << compare << "\n";
    
    // Run benchmark
    if (config_) {
        auto results = Benchmarker::runBenchmark(config_, duration, warmup);
        std::cout << "Benchmark Results:\n";
        std::cout << "  Time: " << results.timeSeconds << " seconds\n";
        std::cout << "  Neurons: " << results.neurons << "\n";
        std::cout << "  Synapses: " << results.synapses << "\n";
        std::cout << "  Total spikes: " << results.spikes << "\n";
        std::cout << "  Average firing rate: " << results.averageFiringRate << " Hz\n";
    }
}

void NLMApplication::handleSaveCommand(const std::map<std::string, std::string>& args) {
    auto it = args.find("file");
    if (it == args.end()) {
        throw std::invalid_argument("Missing required argument: --file");
    }
    
    std::string filepath = it->second;
    std::string format = "binary";
    
    it = args.find("format");
    if (it != args.end()) {
        format = it->second;
    }
    
    if (format == "txt" || format == "text") {
        // For now, just use binary format for everything
        format = "binary";
    }
    
    saveBrainState(filepath);
}

void NLMApplication::handleLoadCommand(const std::map<std::string, std::string>& args) {
    auto it = args.find("file");
    if (it == args.end()) {
        throw std::invalid_argument("Missing required argument: --file");
    }
    
    std::string filepath = it->second;
    std::string format = "binary";
    
    it = args.find("format");
    if (it != args.end()) {
        format = it->second;
    }
    
    if (format == "txt" || format == "text") {
        // For now, only support binary format
        format = "binary";
    }
    
    loadBrainState(filepath);
}

void NLMApplication::handleProfileCommand(const std::map<std::string, std::string>& args) {
    auto it = args.find("on");
    if (it != args.end() && (it->second == "true" || it->second == "1")) {
        startProfiling();
    } else {
        it = args.find("off");
        if (it != args.end() && (it->second == "true" || it->second == "1")) {
            stopProfiling();
        } else {
            // Just show current status
            showPerformanceStats();
        }
    }
    
    it = args.find("output");
    if (it != args.end()) {
        std::cout << "Profile output to " << it->second << "\n";
        std::cout << "(Output saving not yet implemented)\n";
    }
}

void NLMApplication::handleStatsCommand(const std::map<std::string, std::string>& args) {
    bool detailed = false;
    auto it = args.find("detailed");
    if (it != args.end() && (it->second == "true" || it->second == "1")) {
        detailed = true;
    }
    
    showPerformanceStats();
    
    if (detailed && config_) {
        std::cout << "\nDetailed Configuration:\n";
        printConfig();
    }
}

void NLMApplication::handleHelpCommand(const std::map<std::string, std::string>& args) {
    auto it = args.find("command");
    if (it != args.end()) {
        parser_.showCommandHelp(it->second);
    } else {
        parser_.showHelp();
    }
}

void NLMApplication::handleVersionCommand(const std::map<std::string, std::string>& args) {
    showVersion();
    showBuildInfo();
    showSupportedFormats();
}

void NLMApplication::ensureBrainInitialized() const {
    if (!brain_) {
        throw std::runtime_error("Brain not initialized. Load or create configuration first.");
    }
}

std::string NLMApplication::formatTime(double seconds) const {
    std::stringstream ss;
    if (seconds >= 3600) {
        ss << std::fixed << std::setprecision(1) << (seconds / 3600) << "h ";
        seconds = fmod(seconds, 3600);
    }
    if (seconds >= 60) {
        ss << std::fixed << std::setprecision(0) << (seconds / 60) << "m ";
        seconds = fmod(seconds, 60);
    }
    ss << std::fixed << std::setprecision(2) << seconds << "s";
    return ss.str();
}

void NLMApplication::validateArguments(const std::map<std::string, std::string>& args,
                                      const std::vector<std::string>& required) const {
    for (const auto& arg : required) {
        if (args.find(arg) == args.end()) {
            throw std::invalid_argument("Missing required argument: --" + arg);
        }
    }
}

} // namespace nlm
