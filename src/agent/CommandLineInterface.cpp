#include "CommandLineInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace nlm {

CommandLineInterface::CommandLineInterface() {
    sessionStart_ = std::chrono::steady_clock::now();
    initializeDefaultCommands();
    initializeDefaultConfig();
}

CommandLineInterface::~CommandLineInterface() = default;

void CommandLineInterface::initializeDefaultCommands() {
    // Basic information commands
    registerCommand({
        "help", "Show help information", "[command]",
        "Display help information about available commands",
        {"nlm-agent help", "nlm-agent --help", "nlm-agent help info"},
        [this](const std::vector<std::string>& args) { return handleHelp(args); }
    });
    
    registerCommand({
        "version", "Show version information", "",
        "Display version and build information",
        {"nlm-agent version", "nlm-agent --version"},
        [this](const std::vector<std::string>& args) { return handleVersion(args); }
    });
    
    // Neural network configuration
    registerCommand({
        "neuron", "Configure neural network neuron count", "<count>",
        "Set the number of neurons in the network",
        {"nlm-agent neuron 1000", "nlm-agent --neuron 2000"},
        [this](const std::vector<std::string>& args) { return handleNeuronConfig(args); }
    });
    
    registerCommand({
        "region", "Configure brain regions", "<count>",
        "Set the number of neural regions",
        {"nlm-agent region 2", "nlm-agent --region 3"},
        [this](const std::vector<std::string>& args) { return handleRegionConfig(args); }
    });
    
    registerCommand({
        "connections", "Configure connection probability", "<probability>",
        "Set the probability of synaptic connections between neurons (0.0-1.0)",
        {"nlm-agent connections 0.2", "nlm-agent --connections 0.3"},
        [this](const std::vector<std::string>& args) { return handleConnectionConfig(args); }
    });
    
    // Agent behavior configuration
    registerCommand({
        "curiosity", "Configure curiosity parameters", "<threshold> [max_chance]",
        "Set curiosity threshold and exploration chance",
        {"nlm-agent curiosity 0.4 0.5", "nlm-agent --curiosity 0.5"},
        [this](const std::vector<std::string>& args) { return handleCuriosityConfig(args); }
    });
    
    registerCommand({
        "plasticity", "Configure plasticity parameters", "[plasticity_factor] [learning_rate]",
        "Set plasticity base factor and neuromodulation learning rate",
        {"nlm-agent plasticity 0.6 0.1", "nlm-agent --plasticity 0.7"},
        [this](const std::vector<std::string>& args) { return handlePlasticityConfig(args); }
    });
    
    // Feature toggles
    registerCommand({
        "reward", "Enable/disable reward modulation", "[enabled]",
        "Enable or disable reward-based neuromodulation",
        {"nlm-agent reward true", "nlm-agent --reward false"},
        [this](const std::vector<std::string>& args) { return handleRewardModulation(args); }
    });
    
    registerCommand({
        "development", "Enable/disable development system", "[enabled]",
        "Enable or disable developmental processes",
        {"nlm-agent development true", "nlm-agent --development false"},
        [this](const std::vector<std::string>& args) { return handleDevelopment(args); }
    });
    
    // Performance and debugging
    registerCommand({
        "performance", "Configure performance monitoring", "[enabled] [timestep] [max_steps]",
        "Enable performance monitoring and set timestep/max steps",
        {"nlm-agent performance true 0.001 5000", "nlm-agent --performance false"},
        [this](const std::vector<std::string>& args) { return handlePerformanceConfig(args); }
    });
    
    registerCommand({
        "debug", "Enable/disable debug mode", "[enabled]",
        "Enable or disable debug logging and verbose output",
        {"nlm-agent debug true", "nlm-agent --debug false"},
        [this](const std::vector<std::string>& args) { return handleDebugConfig(args); }
    });
    
    registerCommand({
        "benchmark", "Run benchmark test", "[filename]",
        "Run performance benchmark and optionally save results",
        {"nlm-agent benchmark", "nlm-agent benchmark results.txt"},
        [this](const std::vector<std::string>& args) { return handleBenchmark(args); }
    });
    
    // Configuration file management
    registerCommand({
        "export", "Export configuration to file", "<filename>",
        "Export current configuration to a file",
        {"nlm-agent export config.json", "nlm-agent --export myconfig.cfg"},
        [this](const std::vector<std::string>& args) { return handleExportConfig(args); }
    });
    
    registerCommand({
        "import", "Import configuration from file", "<filename>",
        "Import configuration from a file",
        {"nlm-agent import config.json", "nlm-agent --import settings.cfg"},
        [this](const std::vector<std::string>& args) { return handleImportConfig(args); }
    });
    
    registerCommand({
        "reset", "Reset configuration to defaults", "",
        "Reset all configuration to default values",
        {"nlm-agent reset", "nlm-agent --reset"},
        [this](const std::vector<std::string>& args) { return handleResetConfig(args); }
    });
    
    // Information commands
    registerCommand({
        "list", "List available commands", "",
        "List all available commands with their descriptions",
        {"nlm-agent list", "nlm-agent --list-commands"},
        [this](const std::vector<std::string>& args) { return handleListCommands(args); }
    });
}

void CommandLineInterface::initializeDefaultConfig() {
    config_.neuronCount = 500;
    config_.regionCount = 1;
    config_.connectionProbability = 0.15f;
    config_.curiosityThreshold = 0.3f;
    config_.explorationChanceMax = 0.3f;
    config_.motorActivationThreshold = 0.5f;
    config_.enableRewardModulation = true;
    config_.enableStructuralPlasticity = true;
    config_.enableDevelopment = true;
    config_.enableCuriosity = true;
    config_.simulationTimestep = 0.001;
    config_.maxSteps = 10000;
    config_.enablePerformanceMonitoring = false;
    config_.configFile = "configs/default.cfg";
    config_.validateConfig = false;
    config_.outputFormat = "json";
    config_.enableDebugMode = false;
    config_.enableBenchmark = false;
    config_.benchmarkFile = "benchmark.json";
    config_.logLevel = "info";
    
    // Initialize statistics
    stats_.startTime = 0.0;
    stats_.endTime = 0.0;
    stats_.totalSteps = 0;
    stats_.spikeCount = 0;
    stats_.firingNeuronCount = 0;
    stats_.synapseCount = 0;
    stats_.averageSpikesPerSecond = 0.0f;
    stats_.averageFiringRate = 0.0f;
}

bool CommandLineInterface::parse(int argc, char** argv) {
    try {
        parseArguments(argc, argv);
        
        if (!config_.configFile.empty()) {
            parseConfigFile(config_.configFile);
        }
        
        return validateConfig();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("CLI Error: ") + e.what());
        printHelp();
        return false;
    }
}

void CommandLineInterface::parseArguments(int argc, char** argv) {
    std::vector<std::string> remainingArgs;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Long option
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string option = arg.substr(2, equalsPos - 2);
                std::string value = arg.substr(equalsPos + 1);
                processArgument("--" + option, {value});
            } else {
                processArgument(arg, {});
            }
        } else if (arg[0] == '-') {
            // Short option
            std::string option = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                processArgument("-" + option, {argv[++i]});
            } else {
                processArgument("-" + option, {});
            }
        } else if (arg[0] == '@') {
            // Configuration file
            processArgument("@" + arg.substr(1), {});
        } else {
            // Positional argument
            remainingArgs.push_back(arg);
        }
    }
    
    // Process remaining positional arguments
    if (!remainingArgs.empty()) {
        processArgument("@", remainingArgs);
    }
}

void CommandLineInterface::processArgument(const std::string& arg, std::vector<std::string> remainingArgs) {
    if (arg == "@") {
        // Process positional arguments
        for (const auto& posArg : remainingArgs) {
            if (isConfigFile(posArg)) {
                config_.configFile = posArg;
            } else if (posArg == "--help" || posArg == "-h") {
                handleHelp({});
                exit(0);
            } else if (posArg == "--version" || posArg == "-v") {
                handleVersion({});
                exit(0);
            }
        }
    } else if (arg == "--help" || arg == "-h") {
        printHelp();
        exit(0);
    } else if (arg == "--version" || arg == "-v") {
        printVersion();
        exit(0);
    } else if (arg == "@" && !remainingArgs.empty()) {
        if (isConfigFile(remainingArgs[0])) {
            config_.configFile = remainingArgs[0];
        }
    }
}

bool CommandLineInterface::validateConfig() const {
    if (config_.neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(config_.neuronCount));
        return false;
    }
    
    if (config_.regionCount <= 0) {
        NLM_LOG_ERROR("Invalid region count: " + std::to_string(config_.regionCount));
        return false;
    }
    
    if (config_.connectionProbability < 0.0f || config_.connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection probability: " + std::to_string(config_.connectionProbability));
        return false;
    }
    
    if (config_.curiosityThreshold < 0.0f || config_.curiosityThreshold > 1.0f) {
        NLM_LOG_ERROR("Invalid curiosity threshold: " + std::to_string(config_.curiosityThreshold));
        return false;
    }
    
    if (config_.explorationChanceMax < 0.0f || config_.explorationChanceMax > 1.0f) {
        NLM_LOG_ERROR("Invalid exploration chance max: " + std::to_string(config_.explorationChanceMax));
        return false;
    }
    
    if (config_.motorActivationThreshold < 0.0f || config_.motorActivationThreshold > 1.0f) {
        NLM_LOG_ERROR("Invalid motor activation threshold: " + std::to_string(config_.motorActivationThreshold));
        return false;
    }
    
    if (config_.simulationTimestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation timestep: " + std::to_string(config_.simulationTimestep));
        return false;
    }
    
    if (config_.maxSteps <= 0) {
        NLM_LOG_ERROR("Invalid max steps: " + std::to_string(config_.maxSteps));
        return false;
    }
    
    return true;
}

void CommandLineInterface::printHelp(const std::string& command) const {
    printBanner();
    
    if (command.empty()) {
        std::cout << "\nAvailable commands:\n\n";
        for (const auto& cmd : commands_) {
            if (!cmd.hidden) {
                std::cout << "  " << cmd.name << "\n";
                std::cout << "    " << cmd.description << "\n";
                if (!cmd.usage.empty()) {
                    std::cout << "    Usage: " << cmd.usage << "\n";
                }
                std::cout << "\n";
            }
        }
        
        std::cout << "Use 'nlm-agent help <command>' for detailed information about a specific command.\n";
        std::cout << "Use 'nlm-agent --help' or 'nlm-agent -h' for this help message.\n";
    } else {
        const Command* cmd = findCommand(command);
        if (cmd) {
            std::cout << "\nCommand: " << cmd->name << "\n";
            std::cout << "Description: " << cmd->description << "\n";
            if (!cmd->usage.empty()) {
                std::cout << "Usage: " << cmd->usage << "\n";
            }
            std::cout << "\n";
            if (!cmd->longDescription.empty()) {
                std::cout << cmd->longDescription << "\n\n";
            }
            if (!cmd->examples.empty()) {
                std::cout << "Examples:\n";
                for (const auto& example : cmd->examples) {
                    std::cout << "  " << example << "\n";
                }
                std::cout << "\n";
            }
        } else {
            std::cout << "Unknown command: " << command << "\n\n";
            printHelp();
        }
    }
}

void CommandLineInterface::printVersion() const {
    printBanner();
    std::cout << "\nNLM Agent Version: 0.1.0\n";
    std::cout << "Build: Phase 2 - Real Neural Computation\n";
    std::cout << "Description: Experimental artificial developmental brain simulation\n";
    std::cout << "\nFeatures:\n";
    std::cout << "  - Real LIF neuron dynamics\n";
    std::cout << "  - Event-driven spike propagation\n";
    std::cout << "  - STDP and Hebbian plasticity\n";
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)\n";
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)\n";
    std::cout << "  - Developmental stages and plasticity\n";
    std::cout << "  - Working and episodic memory systems\n";
    std::cout << "  - Prediction and planning systems\n";
    std::cout << "  - Attention and concept formation\n";
    std::cout << "\nFor more information, visit: https://github.com/nlm-project/nlm\n";
}

bool CommandLineInterface::handleHelp(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return true;
    } else {
        printHelp(args[0]);
        return true;
    }
}

bool CommandLineInterface::handleVersion(const std::vector<std::string>& args) {
    printVersion();
    return true;
}

bool CommandLineInterface::handleNeuronConfig(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        std::cout << "Error: neuron command requires a count\n";
        std::cout << "Usage: nlm-agent neuron <count>\n";
        return false;
    }
    
    try {
        int64_t count = std::stoll(args[0]);
        config_.neuronCount = count;
        NLM_LOG_INFO("Set neuron count to: " + std::to_string(config_.neuronCount));
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid neuron count: " << args[0] << "\n";
        return false;
    }
}

bool CommandLineInterface::handleRegionConfig(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        std::cout << "Error: region command requires a count\n";
        std::cout << "Usage: nlm-agent region <count>\n";
        return false;
    }
    
    try {
        int64_t count = std::stoll(args[0]);
        config_.regionCount = count;
        NLM_LOG_INFO("Set region count to: " + std::to_string(config_.regionCount));
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid region count: " << args[0] << "\n";
        return false;
    }
}

bool CommandLineInterface::handleConnectionConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Error: connections command requires a probability\n";
        std::cout << "Usage: nlm-agent connections <probability>\n";
        return false;
    }
    
    try {
        float prob = std::stof(args[0]);
        config_.connectionProbability = prob;
        NLM_LOG_INFO("Set connection probability to: " + std::to_string(config_.connectionProbability));
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid connection probability: " << args[0] << "\n";
        return false;
    }
}

bool CommandLineInterface::handleCuriosityConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Error: curiosity command requires at least a threshold\n";
        std::cout << "Usage: nlm-agent curiosity <threshold> [max_chance]\n";
        return false;
    }
    
    try {
        float threshold = std::stof(args[0]);
        config_.curiosityThreshold = threshold;
        
        if (args.size() > 1) {
            float maxChance = std::stof(args[1]);
            config_.explorationChanceMax = maxChance;
        }
        
        NLM_LOG_INFO("Set curiosity threshold to: " + std::to_string(config_.curiosityThreshold));
        if (args.size() > 1) {
            NLM_LOG_INFO("Set exploration chance max to: " + std::to_string(config_.explorationChanceMax));
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid curiosity parameters\n";
        return false;
    }
}

bool CommandLineInterface::handlePlasticityConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Error: plasticity command requires parameters\n";
        std::cout << "Usage: nlm-agent plasticity [plasticity_factor] [learning_rate]\n";
        return false;
    }
    
    try {
        if (args.size() > 0) {
            float plasticityFactor = std::stof(args[0]);
            config_.plasticityBaseFactor = plasticityFactor;
        }
        
        if (args.size() > 1) {
            config_.neuromodulationLearningRate = std::stof(args[1]);
        }
        
        NLM_LOG_INFO("Set plasticity base factor to: " + std::to_string(config_.plasticityBaseFactor));
        if (args.size() > 1) {
            NLM_LOG_INFO("Set neuromodulation learning rate to: " + std::to_string(config_.neuromodulationLearningRate));
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid plasticity parameters\n";
        return false;
    }
}

bool CommandLineInterface::handleRewardModulation(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Reward modulation is currently: " << (config_.enableRewardModulation ? "enabled" : "disabled") << "\n";
        return true;
    }
    
    try {
        bool enabled = (args[0] == "true" || args[0] == "1" || args[0] == "yes");
        config_.enableRewardModulation = enabled;
        NLM_LOG_INFO("Set reward modulation to: " + std::string(enabled ? "enabled" : "disabled"));
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid value for reward modulation\n";
        return false;
    }
}

bool CommandLineInterface::handleDevelopment(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Development system is currently: " << (config_.enableDevelopment ? "enabled" : "disabled") << "\n";
        return true;
    }
    
    try {
        bool enabled = (args[0] == "true" || args[0] == "1" || args[0] == "yes");
        config_.enableDevelopment = enabled;
        NLM_LOG_INFO("Set development system to: " + std::string(enabled ? "enabled" : "disabled"));
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid value for development system\n";
        return false;
    }
}

bool CommandLineInterface::handlePerformanceConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Performance monitoring is currently: " << (config_.enablePerformanceMonitoring ? "enabled" : "disabled") << "\n";
        return true;
    }
    
    try {
        bool enabled = (args[0] == "true" || args[0] == "1" || args[0] == "yes");
        config_.enablePerformanceMonitoring = enabled;
        
        if (args.size() > 1) {
            config_.simulationTimestep = std::stof(args[1]);
        }
        
        if (args.size() > 2) {
            config_.maxSteps = std::stoll(args[2]);
        }
        
        NLM_LOG_INFO("Set performance monitoring to: " + std::string(enabled ? "enabled" : "disabled"));
        if (args.size() > 1) {
            NLM_LOG_INFO("Set simulation timestep to: " + std::to_string(config_.simulationTimestep));
        }
        if (args.size() > 2) {
            NLM_LOG_INFO("Set max steps to: " + std::to_string(config_.maxSteps));
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid performance parameters\n";
        return false;
    }
}

bool CommandLineInterface::handleDebugConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Debug mode is currently: " << (config_.enableDebugMode ? "enabled" : "disabled") << "\n";
        return true;
    }
    
    try {
        bool enabled = (args[0] == "true" || args[0] == "1" || args[0] == "yes");
        config_.enableDebugMode = enabled;
        NLM_LOG_INFO("Set debug mode to: " + std::string(enabled ? "enabled" : "disabled"));
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid value for debug mode\n";
        return false;
    }
}

bool CommandLineInterface::handleBenchmark(const std::vector<std::string>& args) {
    config_.enableBenchmark = true;
    
    if (!args.empty()) {
        config_.benchmarkFile = args[0];
    }
    
    NLM_LOG_INFO("Benchmark mode enabled");
    if (!config_.benchmarkFile.empty()) {
        NLM_LOG_INFO("Benchmark results will be saved to: " + config_.benchmarkFile);
    }
    return true;
}

bool CommandLineInterface::handleExportConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Error: export command requires a filename\n";
        std::cout << "Usage: nlm-agent export <filename>\n";
        return false;
    }
    
    exportConfig(args[0]);
    return true;
}

bool CommandLineInterface::handleImportConfig(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Error: import command requires a filename\n";
        std::cout << "Usage: nlm-agent import <filename>\n";
        return false;
    }
    
    importConfig(args[0]);
    return true;
}

bool CommandLineInterface::handleResetConfig(const std::vector<std::string>& args) {
    initializeDefaultConfig();
    NLM_LOG_INFO("Configuration reset to defaults");
    return true;
}

bool CommandLineInterface::handleListCommands(const std::vector<std::string>& args) {
    std::cout << "\nAvailable commands:\n\n";
    for (const auto& cmd : commands_) {
        if (!cmd.hidden) {
            std::cout << "  " << cmd.name;
            if (!cmd.usage.empty()) {
                std::cout << " " << cmd.usage;
            }
            std::cout << "\n    " << cmd.description << "\n\n";
        }
    }
    return true;
}

void CommandLineInterface::registerCommand(const Command& cmd) {
    commands_.push_back(cmd);
}

const CommandLineInterface::Command* CommandLineInterface::findCommand(const std::string& name) const {
    for (const auto& cmd : commands_) {
        if (cmd.name == name) {
            return &cmd;
        }
    }
    return nullptr;
}

void CommandLineInterface::printBanner() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void CommandLineInterface::printSection(const std::string& title) const {
    std::cout << "\n" << title << ":\n";
    std::cout << std::string(title.length() + 2, '=') << "\n";
}

void CommandLineInterface::printOption(const std::string& option, const std::string& description) const {
    std::cout << "  " << option << "  " << description << "\n";
}

std::string CommandLineInterface::formatTime(double seconds) const {
    std::stringstream ss;
    if (seconds >= 60.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remaining = seconds - (minutes * 60.0);
        ss << minutes << "m " << std::fixed << std::setprecision(2) << remaining << "s";
    } else {
        ss << std::fixed << std::setprecision(2) << seconds << "s";
    }
    return ss.str();
}

std::string CommandLineInterface::formatFloat(float value) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << value;
    return ss.str();
}

bool CommandLineInterface::fileExists(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good();
}

std::string CommandLineInterface::getCurrentWorkingDirectory() const {
    char buffer[FILENAME_MAX];
    getcwd(buffer, FILENAME_MAX);
    return std::string(buffer);
}

double CommandLineInterface::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - sessionStart_);
    return duration.count();
}

float CommandLineInterface::calculateAverageSpikesPerSecond() const {
    if (stats_.endTime <= stats_.startTime) return 0.0f;
    double elapsed = stats_.endTime - stats_.startTime;
    if (elapsed <= 0.0) return 0.0f;
    return static_cast<float>(stats_.spikeCount) / elapsed;
}

float CommandLineInterface::calculateAverageFiringRate() const {
    if (stats_.totalSteps <= 0) return 0.0f;
    return static_cast<float>(stats_.firingNeuronCount) / stats_.totalSteps;
}

void CommandLineInterface::recordStep(size_t spikes, size_t firingNeurons) {
    stats_.spikeCount += spikes;
    stats_.firingNeuronCount += firingNeurons;
    stats_.totalSteps++;
}

void CommandLineInterface::finalizeStatistics() {
    stats_.endTime = getElapsedTime();
    stats_.averageSpikesPerSecond = calculateAverageSpikesPerSecond();
    stats_.averageFiringRate = calculateAverageFiringRate();
}

void CommandLineInterface::printConfig(const AgentConfig& config) const {
    printSection("Configuration Summary");
    
    std::cout << "Neural Network:\n";
    printOption("  Neurons: ", std::to_string(config.neuronCount));
    printOption("  Regions: ", std::to_string(config.regionCount));
    printOption("  Connection Probability: ", formatFloat(config.connectionProbability));
    
    std::cout << "\nAgent Behavior:\n";
    printOption("  Curiosity Threshold: ", formatFloat(config.curiosityThreshold));
    printOption("  Exploration Chance Max: ", formatFloat(config.explorationChanceMax));
    printOption("  Motor Activation Threshold: ", formatFloat(config.motorActivationThreshold));
    printOption("  Reward Modulation: ", config.enableRewardModulation ? "Enabled" : "Disabled");
    printOption("  Structural Plasticity: ", config.enableStructuralPlasticity ? "Enabled" : "Disabled");
    printOption("  Development System: ", config.enableDevelopment ? "Enabled" : "Disabled");
    printOption("  Curiosity System: ", config.enableCuriosity ? "Enabled" : "Disabled");
    
    std::cout << "\nPerformance:\n";
    printOption("  Simulation Timestep: ", formatFloat(config.simulationTimestep) + "s");
    printOption("  Max Steps: ", std::to_string(config.maxSteps));
    printOption("  Performance Monitoring: ", config.enablePerformanceMonitoring ? "Enabled" : "Disabled");
    
    std::cout << "\nAdvanced:\n";
    printOption("  Config File: ", config.configFile.empty() ? "Default" : config.configFile);
    printOption("  Validate Config: ", config.validateConfig ? "Yes" : "No");
    printOption("  Output Format: ", config.outputFormat);
    printOption("  Debug Mode: ", config.enableDebugMode ? "Enabled" : "Disabled");
    printOption("  Benchmark Mode: ", config.enableBenchmark ? "Enabled" : "Disabled");
    if (config.enableBenchmark && !config.benchmarkFile.empty()) {
        printOption("  Benchmark File: ", config.benchmarkFile);
    }
    printOption("  Log Level: ", config.logLevel);
}

void CommandLineInterface::exportConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "# NLM Agent Configuration\n";
    file << "# Exported on: " << std::ctime(&stats_.startTime) << "\n\n";
    
    file << "[neural_network]\n";
    file << "neuron_count = " << config_.neuronCount << "\n";
    file << "region_count = " << config_.regionCount << "\n";
    file << "connection_probability = " << config_.connectionProbability << "\n\n";
    
    file << "[agent_behavior]\n";
    file << "curiosity_threshold = " << config_.curiosityThreshold << "\n";
    file << "exploration_chance_max = " << config_.explorationChanceMax << "\n";
    file << "motor_activation_threshold = " << config_.motorActivationThreshold << "\n";
    file << "enable_reward_modulation = " << (config_.enableRewardModulation ? "true" : "false") << "\n";
    file << "enable_structural_plasticity = " << (config_.enableStructuralPlasticity ? "true" : "false") << "\n";
    file << "enable_development = " << (config_.enableDevelopment ? "true" : "false") << "\n";
    file << "enable_curiosity = " << (config_.enableCuriosity ? "true" : "false") << "\n\n";
    
    file << "[performance]\n";
    file << "simulation_timestep = " << config_.simulationTimestep << "\n";
    file << "max_steps = " << config_.maxSteps << "\n";
    file << "enable_performance_monitoring = " << (config_.enablePerformanceMonitoring ? "true" : "false") << "\n\n";
    
    file << "[advanced]\n";
    file << "config_file = " << config_.configFile << "\n";
    file << "validate_config = " << (config_.validateConfig ? "true" : "false") << "\n";
    file << "output_format = " << config_.outputFormat << "\n";
    file << "enable_debug_mode = " << (config_.enableDebugMode ? "true" : "false") << "\n";
    file << "enable_benchmark = " << (config_.enableBenchmark ? "true" : "false") << "\n";
    if (!config_.benchmarkFile.empty()) {
        file << "benchmark_file = " << config_.benchmarkFile << "\n";
    }
    file << "log_level = " << config_.logLevel << "\n";
    
    NLM_LOG_INFO("Configuration exported to: " + filename);
}

void CommandLineInterface::importConfig(const std::string& filename) {
    if (!fileExists(filename)) {
        throw std::runtime_error("Configuration file not found: " + filename);
    }
    
    std::ifstream file(filename);
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Remove comments and trim
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            applyConfigOverrides({{key, value}});
        }
    }
    
    NLM_LOG_INFO("Configuration imported from: " + filename);
}

void CommandLineInterface::applyConfigOverrides(const std::map<std::string, std::string>& overrides) {
    for (const auto& pair : overrides) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        if (key == "neuron_count") {
            config_.neuronCount = std::stoll(value);
        } else if (key == "region_count") {
            config_.regionCount = std::stoll(value);
        } else if (key == "connection_probability") {
            config_.connectionProbability = std::stof(value);
        } else if (key == "curiosity_threshold") {
            config_.curiosityThreshold = std::stof(value);
        } else if (key == "exploration_chance_max") {
            config_.explorationChanceMax = std::stof(value);
        } else if (key == "motor_activation_threshold") {
            config_.motorActivationThreshold = std::stof(value);
        } else if (key == "enable_reward_modulation") {
            config_.enableRewardModulation = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enable_structural_plasticity") {
            config_.enableStructuralPlasticity = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enable_development") {
            config_.enableDevelopment = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enable_curiosity") {
            config_.enableCuriosity = (value == "true" || value == "1" || value == "yes");
        } else if (key == "simulation_timestep") {
            config_.simulationTimestep = std::stof(value);
        } else if (key == "max_steps") {
            config_.maxSteps = std::stoll(value);
        } else if (key == "enable_performance_monitoring") {
            config_.enablePerformanceMonitoring = (value == "true" || value == "1" || value == "yes");
        } else if (key == "config_file") {
            config_.configFile = value;
        } else if (key == "validate_config") {
            config_.validateConfig = (value == "true" || value == "1" || value == "yes");
        } else if (key == "output_format") {
            config_.outputFormat = value;
        } else if (key == "enable_debug_mode") {
            config_.enableDebugMode = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enable_benchmark") {
            config_.enableBenchmark = (value == "true" || value == "1" || value == "yes");
        } else if (key == "benchmark_file") {
            config_.benchmarkFile = value;
        } else if (key == "log_level") {
            config_.logLevel = value;
        }
    }
}

bool CommandLineInterface::isConfigFile(const std::string& filename) {
    if (filename.empty()) return false;
    
    // Check file extension
    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == "cfg" || ext == "json" || ext == "yaml" || ext == "yml" || ext == "ini";
}

std::string CommandLineInterface::expandConfigPath(const std::string& filename) {
    if (filename.empty()) return "configs/default.cfg";
    
    // Check if file exists in current directory
    if (fileExists(filename)) {
        return filename;
    }
    
    // Check in configs directory
    std::string configPath = "configs/" + filename;
    if (fileExists(configPath)) {
        return configPath;
    }
    
    // Return original filename (will be checked later)
    return filename;
}

void CommandLineInterface::printStatistics() const {
    printSection("Performance Statistics");
    
    std::cout << "Simulation Time: " << formatTime(stats_.endTime - stats_.startTime) << "\n";
    std::cout << "Total Steps: " << stats_.totalSteps << "\n";
    std::cout << "Total Spikes: " << stats_.spikeCount << "\n";
    std::cout << "Total Firing Neurons: " << stats_.firingNeuronCount << "\n";
    std::cout << "Average Spikes/Second: " << formatFloat(stats_.averageSpikesPerSecond) << "\n";
    std::cout << "Average Firing Rate: " << formatFloat(stats_.averageFiringRate) << "\n";
}

void CommandLineInterface::exportStatistics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "{\n";
    file << "  \"startTime\": " << stats_.startTime << ",\n";
    file << "  \"endTime\": " << stats_.endTime << ",\n";
    file << "  \"elapsedTime\": " << (stats_.endTime - stats_.startTime) << ",\n";
    file << "  \"totalSteps\": " << stats_.totalSteps << ",\n";
    file << "  \"spikeCount\": " << stats_.spikeCount << ",\n";
    file << "  \"firingNeuronCount\": " << stats_.firingNeuronCount << ",\n";
    file << "  \"averageSpikesPerSecond\": " << stats_.averageSpikesPerSecond << ",\n";
    file << "  \"averageFiringRate\": " << stats_.averageFiringRate << "\n";
    file << "}\n";
    
    NLM_LOG_INFO("Statistics exported to: " + filename);
}

} // namespace nlm
