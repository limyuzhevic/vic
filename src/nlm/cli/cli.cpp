// nlm/cli/cli.cpp - Implementation of NLM command-line interface
#include "cli.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <filesystem>

namespace nlm {
namespace cli {

// CLIArguments implementation
CLIArguments::CLIArguments(int argc, char** argv) {
    helpRequested = false;
    versionRequested = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help/version flags
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--version" || arg == "-v") {
            versionRequested = true;
        } else if (arg.substr(0, 2) == "--") {
            // Handle --key=value or --key value
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                // Next argument is not a flag
                std::string key = arg.substr(2);
                options[key] = argv[++i];
            } else {
                // Flag without value
                options[arg.substr(2)] = "true";
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            // Handle -key value
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options[key] = argv[++i];
            } else {
                options[key] = "true";
            }
        } else {
            // Positional argument
            positionalArgs.push_back(arg);
        }
    }
    
    // First positional argument is the command
    if (!positionalArgs.empty()) {
        command = positionalArgs[0];
        positionalArgs.erase(positionalArgs.begin());
    }
}

bool CLIArguments::hasOption(const std::string& option) const {
    return options.find(option) != options.end();
}

std::string CLIArguments::getOption(const std::string& option, const std::string& defaultValue) const {
    auto it = options.find(option);
    if (it != options.end()) {
        return it->second;
    }
    return defaultValue;
}

int CLIArguments::getOptionAsInt(const std::string& option, int defaultValue) const {
    auto it = options.find(option);
    if (it != options.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float CLIArguments::getOptionAsFloat(const std::string& option, float defaultValue) const {
    auto it = options.find(option);
    if (it != options.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

double CLIArguments::getOptionAsDouble(const std::string& option, double defaultValue) const {
    auto it = options.find(option);
    if (it != options.end()) {
        try {
            return std::stod(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool CLIArguments::getOptionAsBool(const std::string& option, bool defaultValue) const {
    auto it = options.find(option);
    if (it != options.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes" || value == "on";
    }
    return defaultValue;
}

bool CLIArguments::hasPositionalArg(size_t index) const {
    return index < positionalArgs.size();
}

std::string CLIArguments::getPositionalArg(size_t index) const {
    if (index < positionalArgs.size()) {
        return positionalArgs[index];
    }
    return "";
}

size_t CLIArguments::getPositionalArgCount() const {
    return positionalArgs.size();
}

void CLIArguments::printUsage() const {
    std::cout << "Usage: nlm [command] [options]" << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  demo    - Run NLM demos (Phase 2, 3, 4, 5, 6)" << std::endl;
    std::cout << "  run     - Run brain experiments and simulations" << std::endl;
    std::cout << "  benchmark - Run performance benchmarks" << std::endl;
    std::cout << "  profile   - Profile brain performance and memory usage" << std::endl;
    std::cout << "  plugin   - Manage integration plugins" << std::endl;
    std::cout << "  config   - Manage configuration files and settings" << std::endl;
    std::cout << "\nUse 'nlm <command> --help' for more information about a specific command." << std::endl;
}

// DemoCommandHandler implementation
bool DemoCommandHandler::handleCommand(const CLIArguments& args) {
    std::string phase = args.getOption("phase", "2");
    int episodes = args.getOptionAsInt("episodes", 5);
    int steps = args.getOptionAsInt("steps", 200);
    std::string configFile = args.getOption("config", "");
    
    std::cout << "=== NLM Phase " << phase << " Demo ===" << std::endl;
    std::cout << "Running with " << episodes << " episodes, " << steps << " steps each." << std::endl;
    
    if (!configFile.empty()) {
        std::cout << "Using config file: " << configFile << std::endl;
    }
    
    // TODO: Implement actual demo execution
    std::cout << "Demo execution not yet implemented." << std::endl;
    
    return true;
}

// RunCommandHandler implementation
bool RunCommandHandler::handleCommand(const CLIArguments& args) {
    std::string configFile = args.getOption("config", "");
    int steps = args.getOptionAsInt("steps", 1000);
    std::string controllerConfig = args.getOption("controller", "");
    
    std::cout << "=== NLM Brain Run ===" << std::endl;
    std::cout << "Running brain simulation with " << steps << " steps." << std::endl;
    
    if (!configFile.empty()) {
        std::cout << "Using config file: " << configFile << std::endl;
    }
    
    // TODO: Implement actual brain run
    std::cout << "Brain run not yet implemented." << std::endl;
    
    return true;
}

// BenchmarkCommandHandler implementation
bool BenchmarkCommandHandler::handleCommand(const CLIArguments& args) {
    int neurons = args.getOptionAsInt("neurons", 500);
    int steps = args.getOptionAsInt("steps", 1000);
    int repeats = args.getOptionAsInt("repeats", 5);
    
    std::cout << "=== NLM Benchmark ===" << std::endl;
    std::cout << "Benchmarking with " << neurons << " neurons, " << steps << " steps, " << repeats << " repeats." << std::endl;
    
    // TODO: Implement actual benchmarking
    std::cout << "Benchmark not yet implemented." << std::endl;
    
    return true;
}

// ProfileCommandHandler implementation
bool ProfileCommandHandler::handleCommand(const CLIArguments& args) {
    std::string outputFile = args.getOption("output", "");
    double duration = args.getOptionAsDouble("duration", 60.0);
    std::string metricsFile = args.getOption("metrics", "");
    
    std::cout << "=== NLM Profile ===" << std::endl;
    std::cout << "Profiling for " << duration << " seconds." << std::endl;
    
    if (!outputFile.empty()) {
        std::cout << "Output file: " << outputFile << std::endl;
    }
    
    if (!metricsFile.empty()) {
        std::cout << "Metrics file: " << metricsFile << std::endl;
    }
    
    // TODO: Implement actual profiling
    std::cout << "Profiling not yet implemented." << std::endl;
    
    return true;
}

// PluginCommandHandler implementation
bool PluginCommandHandler::handleCommand(const CLIArguments& args) {
    std::string action = args.getPositionalArg(0);
    std::string pluginName = args.getPositionalArg(1);
    
    std::cout << "=== NLM Plugin " << action << " ===" << std::endl;
    
    if (action == "list") {
        std::cout << "Available plugins:" << std::endl;
        // TODO: List available plugins
        std::cout << "  (plugin listing not yet implemented)" << std::endl;
    } else if (action == "load") {
        if (pluginName.empty()) {
            std::cout << "Error: Plugin name required." << std::endl;
            return false;
        }
        std::cout << "Loading plugin: " << pluginName << std::endl;
        // TODO: Load plugin
        std::cout << "Plugin loading not yet implemented." << std::endl;
    } else if (action == "unload") {
        if (pluginName.empty()) {
            std::cout << "Error: Plugin name required." << std::endl;
            return false;
        }
        std::cout << "Unloading plugin: " << pluginName << std::endl;
        // TODO: Unload plugin
        std::cout << "Plugin unloading not yet implemented." << std::endl;
    } else if (action == "info") {
        if (pluginName.empty()) {
            std::cout << "Error: Plugin name required." << std::endl;
            return false;
        }
        std::cout << "Plugin info for: " << pluginName << std::endl;
        // TODO: Show plugin info
        std::cout << "Plugin info not yet implemented." << std::endl;
    } else {
        std::cout << "Unknown action: " << action << std::endl;
        return false;
    }
    
    return true;
}

// ConfigCommandHandler implementation
bool ConfigCommandHandler::handleCommand(const CLIArguments& args) {
    std::string action = args.getPositionalArg(0);
    std::string configFile = args.getOption("file", "");
    std::string key = args.getOption("key", "");
    
    std::cout << "=== NLM Config " << action << " ===" << std::endl;
    
    if (action == "save") {
        if (configFile.empty()) {
            std::cout << "Error: Output file required." << std::endl;
            return false;
        }
        std::cout << "Saving config to: " << configFile << std::endl;
        // TODO: Save config
        std::cout << "Config saving not yet implemented." << std::endl;
    } else if (action == "load") {
        if (configFile.empty()) {
            std::cout << "Error: Input file required." << std::endl;
            return false;
        }
        std::cout << "Loading config from: " << configFile << std::endl;
        // TODO: Load config
        std::cout << "Config loading not yet implemented." << std::endl;
    } else if (action == "show") {
        if (!key.empty()) {
            std::cout << "Showing config for key: " << key << std::endl;
            // TODO: Show specific config value
            std::cout << "Config show not yet implemented." << std::endl;
        } else {
            std::cout << "Showing all config values:" << std::endl;
            // TODO: Show all config values
            std::cout << "Config show not yet implemented." << std::endl;
        }
    } else {
        std::cout << "Unknown action: " << action << std::endl;
        return false;
    }
    
    return true;
}

// CLIApplication implementation
struct CLIApplication::Impl {
    std::vector<std::unique_ptr<CLICommandHandler>> commandHandlers;
    std::map<std::string, CLICommandHandler*> handlerMap;
    std::string defaultCommand;
    std::string helpText;
    std::string version;
    std::string workingDirectory;
    std::set<std::string> loadedPlugins;
    std::chrono::steady_clock::time_point startTime;
    size_t totalCommandsExecuted = 0;
    double totalExecutionTime = 0.0;
};

CLIApplication::CLIApplication() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->helpText = "NLM - Neural Learning Machine Command Line Interface";
    pImpl->version = "0.1.0";
    pImpl->workingDirectory = ".";
    
    // Register default command handlers
    addCommandHandler(std::make_unique<DemoCommandHandler>());
    addCommandHandler(std::make_unique<RunCommandHandler>());
    addCommandHandler(std::make_unique<BenchmarkCommandHandler>());
    addCommandHandler(std::make_unique<ProfileCommandHandler>());
    addCommandHandler(std::make_unique<PluginCommandHandler>());
    addCommandHandler(std::make_unique<ConfigCommandHandler>());
    
    NLM_LOG_INFO("CLIApplication initialized");
}

CLIApplication::~CLIApplication() {
    NLM_LOG_INFO("CLIApplication shutdown");
}

void CLIApplication::addCommandHandler(std::unique_ptr<CLICommandHandler> handler) {
    if (!handler) {
        return;
    }
    
    std::string commandName = handler->getCommandName();
    pImpl->commandHandlers.push_back(std::move(handler));
    pImpl->handlerMap[commandName] = pImpl->commandHandlers.back().get();
    
    NLM_LOG_INFO("Added command handler: " + commandName);
}

void CLIApplication::setDefaultCommand(const std::string& commandName) {
    pImpl->defaultCommand = commandName;
}

void CLIApplication::setHelpText(const std::string& helpText) {
    pImpl->helpText = helpText;
}

void CLIApplication::setVersion(const std::string& version) {
    pImpl->version = version;
}

bool CLIApplication::run(int argc, char** argv) {
    CLIArguments args(argc, argv);
    return run(args);
}

bool CLIApplication::run(const std::vector<std::string>& args) {
    // Convert std::vector to char* argv
    std::vector<std::string> argStrings;
    std::vector<char*> argPointers;
    
    for (const auto& arg : args) {
        argStrings.push_back(arg);
    }
    
    argStrings.insert(argStrings.begin(), "nlm"); // Add program name
    
    argPointers.reserve(argStrings.size() + 1);
    for (auto& arg : argStrings) {
        argPointers.push_back(&arg[0]);
    }
    argPointers.push_back(nullptr);
    
    return run(argPointers.size() - 1, argPointers.data());
}

bool CLIApplication::run(int argc, char** argv) {
    auto startTime = std::chrono::steady_clock::now();
    
    CLIArguments args(argc, argv);
    
    // Handle help/version requests
    if (args.helpRequested) {
        printHelp();
        return true;
    }
    
    if (args.versionRequested) {
        printVersion();
        return true;
    }
    
    // Find command handler
    CLICommandHandler* handler = nullptr;
    if (!args.command.empty()) {
        auto it = pImpl->handlerMap.find(args.command);
        if (it != pImpl->handlerMap.end()) {
            handler = it->second;
        }
    }
    
    if (!handler && !pImpl->defaultCommand.empty()) {
        auto it = pImpl->handlerMap.find(pImpl->defaultCommand);
        if (it != pImpl->handlerMap.end()) {
            handler = it->second;
        }
    }
    
    if (!handler) {
        std::cerr << "Error: No command handler found for command: " << args.command << std::endl;
        printHelp();
        return false;
    }
    
    // Execute command
    bool success = handler->handleCommand(args);
    
    // Update statistics
    pImpl->totalCommandsExecuted++;
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalExecutionTime += duration;
    
    return success;
}

std::vector<std::string> CLIApplication::getAvailableCommands() const {
    std::vector<std::string> commands;
    for (const auto& pair : pImpl->handlerMap) {
        commands.push_back(pair.first);
    }
    std::sort(commands.begin(), commands.end());
    return commands;
}

CLICommandHandler* CLIApplication::getCommandHandler(const std::string& commandName) {
    auto it = pImpl->handlerMap.find(commandName);
    if (it != pImpl->handlerMap.end()) {
        return it->second;
    }
    return nullptr;
}

void CLIApplication::printHelp() const {
    std::cout << pImpl->helpText << std::endl;
    std::cout << "Version: " << pImpl->version << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: nlm [command] [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available commands:" << std::endl;
    for (const auto& command : getAvailableCommands()) {
        CLICommandHandler* handler = getCommandHandler(command);
        if (handler) {
            std::cout << "  " << command << " - " << handler->getDescription() << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Use 'nlm <command> --help' for more information about a specific command." << std::endl;
}

void CLIApplication::printCommandHelp(const std::string& commandName) const {
    CLICommandHandler* handler = getCommandHandler(commandName);
    if (!handler) {
        std::cout << "Error: Unknown command: " << commandName << std::endl;
        return;
    }
    
    std::cout << "=== " << commandName << " command help ===" << std::endl;
    std::cout << handler->getDescription() << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm " << handler->getUsage() << std::endl;
    std::cout << std::endl;
}

void CLIApplication::printVersion() const {
    std::cout << "NLM Version " << pImpl->version << std::endl;
}

void CLIApplication::printWelcome() const {
    std::cout << "Welcome to NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version " << pImpl->version << std::endl;
    std::cout << "For help, use 'nlm --help' or 'nlm <command> --help'" << std::endl;
}

void CLIApplication::setWorkingDirectory(const std::string& directory) {
    pImpl->workingDirectory = directory;
    std::filesystem::current_path(directory);
}

std::string CLIApplication::getWorkingDirectory() const {
    return pImpl->workingDirectory;
}

void CLIApplication::loadPluginsFromDirectory(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        NLM_LOG_WARNING("Plugin directory does not exist: " + directory);
        return;
    }
    
    NLM_LOG_INFO("Loading plugins from directory: " + directory);
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".so" ||
                filename.size() > 3 && filename.substr(filename.size() - 3) == ".dll" ||
                filename.size() > 4 && filename.substr(filename.size() - 5) == ".dylib") {
                
                std::string pluginName = filename.substr(0, filename.find_last_of('.'));
                NLM_LOG_INFO("Found plugin library: " + pluginName);
                
                // TODO: Dynamically load plugin library
                pImpl->loadedPlugins.insert(pluginName);
            }
        }
    }
}

void CLIApplication::unloadPlugin(const std::string& name) {
    auto it = pImpl->loadedPlugins.find(name);
    if (it != pImpl->loadedPlugins.end()) {
        pImpl->loadedPlugins.erase(it);
        NLM_LOG_INFO("Unloaded plugin: " + name);
    }
}

bool CLIApplication::isPluginLoaded(const std::string& name) const {
    return pImpl->loadedPlugins.find(name) != pImpl->loadedPlugins.end();
}

size_t CLIApplication::getCommandCount() const {
    return pImpl->commandHandlers.size();
}

double CLIApplication::getTotalExecutionTime() const {
    return pImpl->totalExecutionTime;
}

void CLIApplication::printStatus() const {
    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration<double>(endTime - pImpl->startTime).count();
    
    std::cout << "=== NLM CLI Status ===" << std::endl;
    std::cout << "Total commands executed: " << pImpl->totalCommandsExecuted << std::endl;
    std::cout << "Total execution time: " << totalDuration << " seconds" << std::endl;
    std::cout << "Average execution time: " << (pImpl->totalCommandsExecuted > 0 ? 
        (pImpl->totalExecutionTime / pImpl->totalCommandsExecuted) : 0.0) << " seconds" << std::endl;
    std::cout << "Loaded plugins: " << pImpl->loadedPlugins.size() << std::endl;
    std::cout << "Available commands: " << getAvailableCommands().size() << std::endl;
    
    for (const auto& plugin : pImpl->loadedPlugins) {
        std::cout << "  - " << plugin << std::endl;
    }
    
    std::cout << "======================" << std::endl;
}

// CLITool implementation
namespace {
    CLIApplication globalCLIApp;
    bool cliInitialized = false;
}

int CLITool::main(int argc, char** argv) {
    return run(globalCLIApp, argc, argv);
}

int CLITool::run(CLIApplication& app, int argc, char** argv) {
    if (!initialize()) {
        std::cerr << "Failed to initialize CLI system" << std::endl;
        return 1;
    }
    
    app.printWelcome();
    
    bool success = app.run(argc, argv);
    
    shutdown();
    
    return success ? 0 : 1;
}

int CLITool::run(CLIApplication& app, const std::vector<std::string>& args) {
    return run(app, static_cast<int>(args.size()), const_cast<char**>(&args[0]));
}

bool CLITool::initialize() {
    if (cliInitialized) {
        return true;
    }
    
    // Initialize logging system
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("CLI system initialized");
    cliInitialized = true;
    
    return true;
}

bool CLITool::shutdown() {
    if (!cliInitialized) {
        return true;
    }
    
    NLM_LOG_INFO("CLI system shutdown");
    cliInitialized = false;
    
    return true;
}

bool CLITool::isInitialized() {
    return cliInitialized;
}

std::string CLITool::getVersion() {
    return "0.1.0";
}

void CLITool::printBanner() {
    std::cout << "==================================================" << std::endl;
    std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << "Version " << getVersion() << std::endl;
    std::cout << "==================================================" << std::endl;
}

CLIApplication& CLITool::getApplication() {
    return globalCLIApp;
}

void CLITool::setApplication(CLIApplication& app) {
    globalCLIApp = app;
}

bool CLITool::registerCommandHandler(std::unique_ptr<CLICommandHandler> handler) {
    return globalCLIApp.addCommandHandler(std::move(handler));
}

bool CLITool::unregisterCommandHandler(const std::string& commandName) {
    auto it = globalCLIApp.getCommandHandler(commandName);
    if (it) {
        // TODO: Remove from globalCLIApp's handler map
        return true;
    }
    return false;
}

std::vector<std::string> CLITool::getAvailableCommands() {
    return globalCLIApp.getAvailableCommands();
}

} // namespace

// CLI utility functions
namespace cli {
    bool initializeCLI() {
        return CLITool::initialize();
    }
    
    bool shutdownCLI() {
        return CLITool::shutdown();
    }
    
    int runCLI(int argc, char** argv) {
        return CLITool::main(argc, argv);
    }
    
    std::string getCLIVersion() {
        return CLITool::getVersion();
    }
    
    void printCLIHelp() {
        CLITool::getApplication().printHelp();
    }
    
    void printCommandHelp(const std::string& command) {
        CLITool::getApplication().printCommandHelp(command);
    }
}

} // namespace cli

// Global CLI instances
namespace {
    bool cliInitialized = false;
    cli::CLIApplication globalCLI;
}

CLIApplication& getGlobalCLI() {
    return globalCLI;
}

void setGlobalCLI(CLIApplication& app) {
    globalCLI = app;
}

// CLI entry point
int main(int argc, char** argv) {
    return cli::CLITool::main(argc, argv);
}

} // namespace nlm
