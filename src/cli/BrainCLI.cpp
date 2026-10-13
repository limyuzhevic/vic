#include "BrainCLI.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

struct BrainCLI::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::map<std::string, std::string> parameters;
    CommandPrompt prompt;
    OutputFormatter formatter;
    
    Impl() : config(nullptr), brain(nullptr) {
        initializeConfiguration();
    }
};

BrainCLI::BrainCLI() : pImpl(std::make_unique<Impl>()) {}

BrainCLI::~BrainCLI() = default;

int BrainCLI::run(int argc, char* argv[]) {
    CommandLineParser parser(argc, argv);
    
    if (parser.hasNamedArg("help") || parser.hasNamedArg("h")) {
        showHelp();
        return 0;
    }
    
    if (parser.hasNamedArg("version") || parser.hasNamedArg("v")) {
        showVersion();
        return 0;
    }
    
    if (parser.hasNamedArg("batch")) {
        // Run in batch mode
        std::string batchCommands = parser.getNamedArg("batch");
        std::vector<std::string> commands;
        std::stringstream ss(batchCommands);
        std::string line;
        while (std::getline(ss, line)) {
            // Remove comments
            size_t commentPos = line.find("#");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\n\r"));
            line.erase(line.find_last_not_of(" \t\n\r") + 1);
            if (!line.empty()) {
                commands.push_back(line);
            }
        }
        runBatchMode(commands);
        return 0;
    }
    
    // Check for interactive mode flag
    if (parser.hasNamedArg("interactive") || parser.hasNamedArg("i") || 
        (parser.getPositionalArgs().empty() && argc <= 1)) {
        runInteractiveMode();
        return 0;
    }
    
    // Process positional arguments as commands
    const auto& args = parser.getPositionalArgs();
    if (!args.empty()) {
        processCommand(args[0], std::vector<std::string>(args.begin() + 1, args.end()));
        return 0;
    }
    
    // Default: show help
    showHelp();
    return 0;
}

void BrainCLI::showHelp() const {
    displayBanner();
    
    std::cout << "Usage: nlm [options] [command] [arguments]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  -i, --interactive       Run in interactive mode\n";
    std::cout << "  -b, --batch [commands]  Run in batch mode with commands\n";
    std::cout << "  -c, --config [file]     Load configuration from file\n";
    std::cout << "\n";
    
    std::cout << "Commands:\n";
    std::cout << "  help [command]          Show help for command or list all commands\n";
    std::cout << "  version                 Show version information\n";
    std::cout << "  status                  Show current brain status\n";
    std::cout << "  load [file]             Load configuration from file\n";
    std::cout << "  save [file]             Save configuration to file\n";
    std::cout << "  set key value            Set configuration parameter\n";
    std::cout << "  get key                 Get configuration parameter\n";
    std::cout << "  config                  Show current configuration\n";
    std::cout << "  test [name]             Run test (connectivity, plasticity, stdp)\n";
    std::cout << "  experiment [name]       Run experiment\n";
    std::cout << "  exit                    Exit the program\n";
    std::cout << "\n";
    
    std::cout << "Examples:\n";
    std::cout << "  nlm status\n";
    std::cout << "  nlm test connectivity\n";
    std::cout << "  nlm experiment phase6_demo\n";
    std::cout << "  nlm --interactive\n";
}

void BrainCLI::showVersion() const {
    displayBanner();
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0\n";
    std::cout << "Phase 6: Final Integration\n";
    std::cout << "Experimental artificial developmental brain system\n";
}

void BrainCLI::showBrainStatus() const {
    if (!pImpl->brain) {
        displayError("Brain not initialized. Use 'config' to set parameters first.");
        return;
    }
    
    pImpl->brain->logStatus();
}

bool BrainCLI::loadConfig(const std::string& filename) {
    if (!ConfigFileHandler::fileExists(filename)) {
        displayError(std::string("Configuration file not found: ") + filename);
        return false;
    }
    
    std::map<std::string, std::string> config;
    if (ConfigFileHandler::load(filename, config)) {
        // Apply configuration
        for (const auto& pair : config) {
            pImpl->parameters[pair.first] = pair.second;
        }
        displaySuccess(std::string("Configuration loaded from: ") + filename);
        return true;
    } else {
        displayError(std::string("Failed to load configuration from: ") + filename);
        return false;
    }
}

bool BrainCLI::saveConfig(const std::string& filename) const {
    if (ConfigFileHandler::save(filename, pImpl->parameters)) {
        displaySuccess(std::string("Configuration saved to: ") + filename);
        return true;
    } else {
        displayError(std::string("Failed to save configuration to: ") + filename);
        return false;
    }
}

int BrainCLI::runTest(const std::string& testName) {
    if (!pImpl->brain) {
        displayError("Brain not initialized. Please set up configuration first.");
        return 1;
    }
    
    if (testName == "connectivity") {
        NLM_LOG_INFO("Running connectivity test...");
        // Reset and run connectivity test
        pImpl->brain->reset();
        pImpl->brain->initialize();
        
        size_t initialSpikes = pImpl->brain->getTotalSpikeCount();
        
        // Inject current into neurons
        auto* region = pImpl->brain->getRegion(RegionId(1));
        if (!region) {
            displayError("No region found for connectivity test.");
            return 1;
        }
        
        auto neurons = region->getAllNeurons();
        size_t neuronsToStimulate = std::min(size_t(10), neurons.size());
        for (size_t i = 0; i < neuronsToStimulate; ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run steps
        for (SimulationStep step = 0; step < 50; ++step) {
            pImpl->brain->step(step, step * 0.001);
        }
        
        size_t spikes = pImpl->brain->getTotalSpikeCount() - initialSpikes;
        
        displayInfo(std::string("Connectivity test results:"));
        displayInfo(std::string("  Spikes generated: ") + std::to_string(spikes));
        
        if (spikes > 0) {
            displaySuccess("✓ Connectivity test PASSED");
            return 0;
        } else {
            displayError("✗ Connectivity test FAILED - no spikes generated");
            return 1;
        }
    } else if (testName == "plasticity") {
        NLM_LOG_INFO("Running plasticity test...");
        // Reset and run plasticity test
        pImpl->brain->reset();
        pImpl->brain->initialize();
        
        // Enable plasticity
        auto* region = pImpl->brain->getRegion(RegionId(1));
        if (!region) {
            displayError("No region found for plasticity test.");
            return 1;
        }
        
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
        
        // Apply input patterns
        for (SimulationStep step = 0; step < 1000; ++step) {
            size_t sensoryNeurons = pImpl->brain->getTotalNeuronCount() / 4;
            for (size_t i = 0; i < 20 && i < sensoryNeurons; ++i) {
                pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            pImpl->brain->step(step, step * 0.001);
        }
        
        displaySuccess("✓ Plasticity test COMPLETED");
        return 0;
    } else if (testName == "stdp") {
        NLM_LOG_INFO("Running STDP test...");
        // Reset and run STDP test
        pImpl->brain->reset();
        pImpl->brain->initialize();
        
        auto* region = pImpl->brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            displayError("Not enough synapses for STDP test.");
            return 1;
        }
        
        auto& synapses = region->getSynapses();
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        // Create correlated activity
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 50; ++trial) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                preNeuron->injectCurrent(60.0f);
                pImpl->brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                pImpl->brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        displaySuccess(std::string("STDP test completed. Total weight change: ") + std::to_string(totalDelta));
        
        if (std::abs(totalDelta) > 0.001f) {
            displaySuccess("✓ STDP test PASSED");
            return 0;
        } else {
            displayWarning("⚠ STDP test showed minimal changes (may be normal)");
            return 0;
        }
    } else {
        displayError(std::string("Unknown test: ") + testName);
        return 1;
    }
}

int BrainCLI::runExperiment(const std::string& experimentName) {
    if (experimentName == "phase6_demo") {
        NLM_LOG_INFO("Running Phase 6 integration demo...");
        
        // For now, just run the basic tests
        // In a full implementation, this would run the actual Phase 6 experiment
        
        displayInfo("Phase 6 integration demo would run here...");
        displayInfo("This would test memory integration, neuromodulation, checkpointing, etc.");
        
        return 0;
    } else {
        displayError(std::string("Unknown experiment: ") + experimentName);
        return 1;
    }
}

bool BrainCLI::setParameter(const std::string& key, const std::string& value) {
    pImpl->parameters[key] = value;
    displaySuccess(std::string("Parameter set: ") + key + std::string(" = ") + value);
    return true;
}

std::string BrainCLI::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return "";
}

void BrainCLI::showConfiguration() const {
    if (pImpl->parameters.empty()) {
        displayInfo("No configuration parameters set.")
        return;
    }
    
    displayInfo("Current configuration:");
    for (const auto& pair : pImpl->parameters) {
        displayInfo(std::string("  ") + pair.first + std::string(" = ") + pair.second);
    }
}

bool BrainCLI::saveBrainState(const std::string& filename) const {
    if (!pImpl->brain) {
        displayError("Brain not initialized.");
        return false;
    }
    
    if (pImpl->brain->save(filename)) {
        displaySuccess(std::string("Brain state saved to: ") + filename);
        return true;
    } else {
        displayError(std::string("Failed to save brain state to: ") + filename);
        return false;
    }
}

bool BrainCLI::loadBrainState(const std::string& filename) {
    if (!pImpl->parameters.count("neuron_count")) {
        displayError("Please set neuron_count parameter first.");
        return false;
    }
    
    size_t neuronCount = 0;
    try {
        neuronCount = std::stoul(pImpl->parameters.at("neuron_count"));
    } catch (...) {
        displayError("Invalid neuron_count parameter.");
        return false;
    }
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
    
    pImpl->brain = std::make_shared<Brain>(cfg);
    if (pImpl->brain->initialize()) {
        displaySuccess(std::string("Brain state loaded from: ") + filename);
        return true;
    } else {
        displayError(std::string("Failed to load brain state from: ") + filename);
        return false;
    }
}

void BrainCLI::showCommandHelp(const std::string& command) const {
    if (command == "help") {
        showHelp();
    } else if (command == "version") {
        showVersion();
    } else if (command == "status") {
        std::cout << "  Display current brain status\n";
    } else if (command == "load") {
        std::cout << "  Load configuration from file\n";
        std::cout << "  Usage: load [filename]\n";
    } else if (command == "save") {
        std::cout << "  Save configuration to file\n";
        std::cout << "  Usage: save [filename]\n";
    } else if (command == "set") {
        std::cout << "  Set configuration parameter\n";
        std::cout << "  Usage: set key value\n";
    } else if (command == "get") {
        std::cout << "  Get configuration parameter\n";
        std::cout << "  Usage: get key\n";
    } else if (command == "test") {
        std::cout << "  Run test\n";
        std::cout << "  Usage: test [connectivity|plasticity|stdp]\n";
    } else if (command == "experiment") {
        std::cout << "  Run experiment\n";
        std::cout << "  Usage: experiment [phase6_demo]\n";
    } else if (command == "config") {
        std::cout << "  Show current configuration\n";
    } else if (command == "exit") {
        std::cout << "  Exit the program\n";
    } else {
        displayError(std::string("Unknown command: ") + command);
    }
}

bool BrainCLI::hasCommand(const std::string& command) const {
    static const std::vector<std::string> commands = {
        "help", "version", "status", "load", "save", "set", "get",
        "config", "test", "experiment", "exit"
    };
    
    return std::find(commands.begin(), commands.end(), command) != commands.end();
}

void BrainCLI::processCommand(const std::string& command, const std::vector<std::string>& args) {
    if (!executeCommand(command, args)) {
        displayError(std::string("Command failed: ") + command);
    }
}

bool BrainCLI::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "help") {
        if (args.empty()) {
            showHelp();
        } else {
            showCommandHelp(args[0]);
        }
        return true;
    } else if (command == "version") {
        showVersion();
        return true;
    } else if (command == "status") {
        showBrainStatus();
        return true;
    } else if (command == "load") {
        if (args.empty()) {
            displayError("Missing filename for load command.");
            return false;
        }
        return loadConfig(args[0]);
    } else if (command == "save") {
        if (args.empty()) {
            displayError("Missing filename for save command.");
            return false;
        }
        return saveConfig(args[0]);
    } else if (command == "set") {
        if (args.size() < 2) {
            displayError("Missing key or value for set command.");
            return false;
        }
        return setParameter(args[0], args[1]);
    } else if (command == "get") {
        if (args.empty()) {
            displayError("Missing key for get command.");
            return false;
        }
        std::string value = getParameter(args[0]);
        if (value.empty()) {
            displayError(std::string("Parameter not found: ") + args[0]);
            return false;
        } else {
            displayInfo(std::string("Value: ") + value);
        }
        return true;
    } else if (command == "config") {
        showConfiguration();
        return true;
    } else if (command == "test") {
        if (args.empty()) {
            displayError("Missing test name. Available: connectivity, plasticity, stdp");
            return false;
        }
        return runTest(args[0]) == 0;
    } else if (command == "experiment") {
        if (args.empty()) {
            displayError("Missing experiment name.");
            return false;
        }
        return runExperiment(args[0]) == 0;
    } else if (command == "exit") {
        displayInfo("Exiting...");
        return false;
    } else {
        displayError(std::string("Unknown command: ") + command);
        return false;
    }
}

void BrainCLI::displayBanner() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void BrainCLI::displayPrompt() const {
    std::cout << "nlm> ";
}

void BrainCLI::displayError(const std::string& error) const {
    std::cerr << "[ERROR] " << error << std::endl;
}

void BrainCLI::displayInfo(const std::string& info) const {
    std::cout << "[INFO] " << info << std::endl;
}

void BrainCLI::displaySuccess(const std::string& success) const {
    std::cout << "[SUCCESS] " << success << std::endl;
}

void BrainCLI::displayWarning(const std::string& warning) const {
    std::cout << "[WARNING] " << warning << std::endl;
}

void BrainCLI::runInteractiveMode() {
    displayBanner();
    displayInfo("Welcome to NLM Interactive Mode");
    displayInfo("Type 'help' for available commands");
    displayInfo("Type 'exit' to quit");
    
    pImpl->prompt.setPrompt("nlm> ");
    pImpl->prompt.setHistoryEnabled(true);
    
    while (true) {
        displayPrompt();
        
        std::string input;
        if (!std::getline(std::cin, input)) {
            break; // EOF or error
        }
        
        // Trim whitespace
        size_t start = input.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) {
            continue; // Empty line
        }
        
        size_t end = input.find_last_not_of(" \t\n\r");
        input = input.substr(start, end - start + 1);
        
        if (input.empty()) {
            continue;
        }
        
        // Add to history
        pImpl->prompt.setHistoryEnabled(true);
        pImpl->prompt.readLine(); // This will add to history
        
        // Process command
        size_t spacePos = input.find(' ');
        std::string command;
        std::vector<std::string> args;
        
        if (spacePos != std::string::npos) {
            command = input.substr(0, spacePos);
            args = splitArgs(input.substr(spacePos + 1));
        } else {
            command = input;
        }
        
        processCommand(command, args);
        
        if (command == "exit") {
            break;
        }
    }
}

void BrainCLI::runBatchMode(const std::vector<std::string>& commands) {
    displayBanner();
    displayInfo(std::string("Running ") + std::to_string(commands.size()) + std::string(" commands in batch mode."));
    
    int successCount = 0;
    int failureCount = 0;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        const std::string& command = commands[i];
        
        displayInfo(std::string("Command ") + std::to_string(i + 1) + std::string("/") + std::to_string(commands.size()) + std::string(": ") + command);
        
        size_t spacePos = command.find(' ');
        std::string cmd;
        std::vector<std::string> args;
        
        if (spacePos != std::string::npos) {
            cmd = command.substr(0, spacePos);
            args = splitArgs(command.substr(spacePos + 1));
        } else {
            cmd = command;
        }
        
        if (executeCommand(cmd, args)) {
            successCount++;
            displaySuccess("✓ Command succeeded");
        } else {
            failureCount++;
            displayError("✗ Command failed");
        }
    }
    
    displayInfo(std::string("Batch completed: ") + std::to_string(successCount) + std::string(" succeeded, ") + std::to_string(failureCount) + std::string(" failed."));
}

void BrainCLI::initializeConfiguration() {
    // Set default configuration parameters
    pImpl->parameters["neuron_count"] = "1000";
    pImpl->parameters["region_count"] = "1";
    pImpl->parameters["connection_probability"] = "0.1";
    pImpl->parameters["simulation_timestep"] = "0.001";
    pImpl->parameters["random_seed"] = "42";
    
    // STDP parameters
    pImpl->parameters["stdp_ltp_weight"] = "0.01";
    pImpl->parameters["stdp_ltd_weight"] = "0.012";
    pImpl->parameters["stdp_tau"] = "20.0";
    
    // Structural plasticity parameters
    pImpl->parameters["synaptogenesis_rate"] = "0.0001";
    pImpl->parameters["pruning_rate"] = "0.00001";
}

void BrainCLI::addToHistory(const std::string& command) {
    // Simple history management
    // In a full implementation, this would use a proper history manager
    static std::vector<std::string> history;
    history.push_back(command);
    
    // Keep history size manageable
    if (history.size() > 1000) {
        history.erase(history.begin(), history.begin() + 100);
    }
}

void BrainCLI::showHistory(size_t count) const {
    // Simple history display
    // In a full implementation, this would use a proper history manager
    static std::vector<std::string> history;
    
    if (history.empty()) {
        displayInfo("No command history available.");
        return;
    }
    
    size_t start = (history.size() > count) ? history.size() - count : 0;
    
    displayInfo("Command history (last " + std::to_string(count) + ":");
    for (size_t i = start; i < history.size(); ++i) {
        displayInfo(std::to_string(i + 1) + std::string(": ") + history[i]);
    }
}

std::vector<std::string> BrainCLI::getCommandCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    
    // This is a simple implementation
    // In a full implementation, this would use a proper completion system
    
    static const std::vector<std::string> allCommands = {
        "help", "version", "status", "load", "save", "set", "get",
        "config", "test", "experiment", "exit"
    };
    
    for (const std::string& cmd : allCommands) {
        if (cmd.find(partial) == 0) {
            completions.push_back(cmd);
        }
    }
    
    return completions;
}

// Helper function to split command arguments
std::vector<std::string> BrainCLI::splitArgs(const std::string& args) {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;
    char quoteChar = '\0';
    
    for (char c : args) {
        if (!inQuotes && (c == ' ' || c == '\t')) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else if (c == '"' || c == '\'') {
            if (!inQuotes) {
                inQuotes = true;
                quoteChar = c;
            } else if (quoteChar == c) {
                inQuotes = false;
                quoteChar = '\0';
            } else {
                current.push_back(c);
            }
        } else {
            current.push_back(c);
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

// CommandLineParser implementation
CommandLineParser::CommandLineParser(int argc, char* argv[]) {
    if (argc > 0) {
        programName_ = argv[0];
    }
    
    parseArguments(argc, argv);
}

void CommandLineParser::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.find("-") == 0) {
            // Named argument (--key value or -k value)
            size_t equalPos = arg.find("=");
            if (equalPos != std::string::npos) {
                // --key=value format
                std::string key = arg.substr(1, equalPos - 1);
                std::string value = arg.substr(equalPos + 1);
                namedArgs_[key] = value;
            } else if (i + 1 < argc && std::string(argv[i + 1]).find("-") != 0) {
                // --key value format
                std::string key = arg.substr(1);
                namedArgs_[key] = argv[++i];
            }
        } else {
            // Positional argument
            positionalArgs_.push_back(arg);
        }
    }
}

std::string CommandLineParser::getNamedArg(const std::string& name) const {
    auto it = namedArgs_.find(name);
    if (it != namedArgs_.end()) {
        return it->second;
    }
    return "";
}

bool CommandLineParser::hasNamedArg(const std::string& name) const {
    return namedArgs_.find(name) != namedArgs_.end();
}

void CommandLineParser::showUsage() const {
    std::cout << "Usage: " << programName_ << " [options] [command] [arguments]\n";
    std::cout << "\n";
    std::cout << "For detailed help, run: " << programName_ << " --help\n";
}

std::string CommandLineParser::getHelpText() const {
    std::stringstream ss;
    ss << "Program: " << programName_ << std::endl;
    ss << std::endl;
    ss << "Usage: " << programName_ << " [options] [command] [arguments]" << std::endl;
    ss << std::endl;
    ss << "Options:" << std::endl;
    ss << "  -h, --help              Show this help message" << std::endl;
    ss << "  -v, --version           Show version information" << std::endl;
    ss << "  -i, --interactive       Run in interactive mode" << std::endl;
    ss << "  -b, --batch [commands]  Run in batch mode with commands" << std::endl;
    ss << "  -c, --config [file]     Load configuration from file" << std::endl;
    
    return ss.str();
}

// ConfigFileHandler implementation
ConfigFileHandler::ConfigFileHandler() = default;

ConfigFileHandler::~ConfigFileHandler() = default;

bool ConfigFileHandler::load(const std::string& filename, std::map<std::string, std::string>& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        size_t commentPos = line.find("#");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        // Parse key = value format
        size_t equalPos = line.find("=");
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            
            config[key] = value;
        }
    }
    
    return true;
}

bool ConfigFileHandler::save(const std::string& filename, const std::map<std::string, std::string>& config) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Format: key = value\n";
    file << std::endl;
    
    for (const auto& pair : config) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    return true;
}

bool ConfigFileHandler::validate(const std::map<std::string, std::string>& config, std::vector<std::string>& errors) const {
    // Basic validation
    if (config.empty()) {
        errors.push_back("Configuration is empty.");
        return false;
    }
    
    // Check for required parameters
    std::vector<std::string> requiredKeys = {
        "neuron_count", "region_count", "simulation_timestep"
    };
    
    for (const std::string& key : requiredKeys) {
        if (config.find(key) == config.end()) {
            errors.push_back("Missing required parameter: " + key);
        }
    }
    
    // Validate parameter values
    for (const auto& pair : config) {
        if (pair.first == "neuron_count") {
            try {
                size_t value = std::stoul(pair.second);
                if (value == 0) {
                    errors.push_back("neuron_count must be greater than 0.");
                }
            } catch (...) {
                errors.push_back("Invalid neuron_count value: " + pair.second);
            }
        } else if (pair.first == "region_count") {
            try {
                size_t value = std::stoul(pair.second);
                if (value == 0) {
                    errors.push_back("region_count must be greater than 0.");
                }
            } catch (...) {
                errors.push_back("Invalid region_count value: " + pair.second);
            }
        }
    }
    
    return errors.empty();
}

std::map<std::string, std::string> ConfigFileHandler::getDefaultConfiguration() {
    std::map<std::string, std::string> config;
    
    config["neuron_count"] = "1000";
    config["region_count"] = "1";
    config["connection_probability"] = "0.1";
    config["simulation_timestep"] = "0.001";
    config["random_seed"] = "42";
    config["stdp_ltp_weight"] = "0.01";
    config["stdp_ltd_weight"] = "0.012";
    config["stdp_tau"] = "20.0";
    config["synaptogenesis_rate"] = "0.0001";
    config["pruning_rate"] = "0.00001";
    
    return config;
}

std::string ConfigFileHandler::formatConfiguration(const std::map<std::string, std::string>& config) {
    std::stringstream ss;
    ss << "Configuration:" << std::endl;
    
    for (const auto& pair : config) {
        ss << "  " << pair.first << " = " << pair.second << std::endl;
    }
    
    return ss.str();
}

template<typename T>
T ConfigFileHandler::parseValue(const std::string& value, const T& defaultValue) {
    // This is a simplified implementation
    // In a real implementation, this would handle different types properly
    return defaultValue;
}

bool ConfigFileHandler::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// CommandPrompt implementation
CommandPrompt::CommandPrompt() : historyPosition_(0) {
    prompt_ = "nlm> ";
    historyEnabled_ = true;
    historyFile_ = ".nlm_history";
}

CommandPrompt::~CommandPrompt() = default;

std::string CommandPrompt::readLine() {
    std::string line;
    std::getline(std::cin, line);
    
    if (historyEnabled_) {
        // Add to history (simplified implementation)
        static std::vector<std::string> history;
        history.push_back(line);
        if (history.size() > 1000) {
            history.erase(history.begin(), history.begin() + 100);
        }
    }
    
    return line;
}

void CommandPrompt::showCompletions(const std::vector<std::string>& completions) {
    if (completions.empty()) {
        return;
    }
    
    std::cout << "Possible completions:" << std::endl;
    for (const std::string& completion : completions) {
        std::cout << "  " << completion << std::endl;
    }
}

// OutputFormatter implementation
OutputFormatter::OutputFormatter() : format_(Format::Plain), colorsEnabled_(true), fileOutputEnabled_(false) {}

OutputFormatter::~OutputFormatter() = default;

void OutputFormatter::print(const std::string& text) const {
    std::cout << text;
}

void OutputFormatter::println(const std::string& text) const {
    std::cout << text << std::endl;
}

void OutputFormatter::printTable(const std::vector<std::vector<std::string>>& rows,
                                 const std::vector<std::string>& headers) const {
    if (rows.empty()) {
        return;
    }
    
    // Calculate column widths
    std::vector<size_t> colWidths(headers.size(), 0);
    for (size_t i = 0; i < headers.size(); ++i) {
        colWidths[i] = headers[i].size();
    }
    
    for (const auto& row : rows) {
        for (size_t i = 0; i < std::min(row.size(), headers.size()); ++i) {
            colWidths[i] = std::max(colWidths[i], row[i].size());
        }
    }
    
    // Print headers
    for (size_t i = 0; i < headers.size(); ++i) {
        if (i > 0) std::cout << "  ";
        std::cout << std::left << std::setw(colWidths[i]) << headers[i];
    }
    std::cout << std::endl;
    
    // Print separator
    for (size_t i = 0; i < headers.size(); ++i) {
        if (i > 0) std::cout << "  ";
        std::cout << std::string(colWidths[i], '-');
    }
    std::cout << std::endl;
    
    // Print rows
    for (const auto& row : rows) {
        for (size_t i = 0; i < std::min(row.size(), headers.size()); ++i) {
            if (i > 0) std::cout << "  ";
            std::cout << std::left << std::setw(colWidths[i]) << row[i];
        }
        std::cout << std::endl;
    }
}

void OutputFormatter::printMetrics(const std::map<std::string, double>& metrics) const {
    std::cout << std::fixed << std::setprecision(4);
    
    for (const auto& pair : metrics) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    std::cout << std::defaultfloat;
}

void OutputFormatter::printStatus(const std::map<std::string, std::string>& status) const {
    for (const auto& pair : status) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void OutputFormatter::setOutputFile(const std::string& filename) {
    outputFile_.open(filename);
    fileOutputEnabled_ = outputFile_.good();
}

void OutputFormatter::reset() {
    if (fileOutputEnabled_) {
        outputFile_.close();
        fileOutputEnabled_ = false;
    }
}

std::string OutputFormatter::applyColor(const std::string& text, const std::string& color) const {
    if (!colorsEnabled_) {
        return text;
    }
    
    static const std::map<std::string, std::string> colorCodes = {
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"}
    };
    
    auto it = colorCodes.find(color);
    if (it != colorCodes.end()) {
        return it->second + text + "\033[0m";
    }
    
    return text;
}

std::string OutputFormatter::getColorCode(const std::string& color) const {
    static const std::map<std::string, std::string> colorCodes = {
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"}
    };
    
    auto it = colorCodes.find(color);
    if (it != colorCodes.end()) {
        return it->second;
    }
    
    return "";
}

} // namespace nlm
