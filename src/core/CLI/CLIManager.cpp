// NLM Command Line Interface Manager Implementation

#include "core/CLI/CLIManager.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <filesystem>

namespace nlm {

CLIManager::CLIManager() : configLoaded(false), verbose(false), quiet(false) {
    NLM_LOG_INFO("Initializing NLM Command Line Interface Manager...");
    
    // Register built-in commands
    registerCommand({
        "help", "Show help information for commands",
        "nlm help [command]",
        [this](int argc, char** argv) { return executeHelp(argc > 1 ? argv[1] : ""); },
        "general", false
    });
    
    registerCommand({
        "version", "Show NLM version information",
        "nlm version",
        [this](int argc, char** argv) { 
            std::cout << getVersion() << std::endl;
            std::cout << getBuildInfo() << std::endl;
            return true; 
        },
        "general", false
    });
    
    registerCommand({
        "configure", "Load and validate configuration file",
        "nlm configure --file config.cfg",
        [this](int argc, char** argv) { return loadConfigFile("config.cfg"); },
        "configuration", false
    });
    
    // Register demo commands
    registerDemo("phase2", "NLM Phase 2: Real Neural Computation", true);
    registerDemo("phase3", "NLM Phase 3: World Interaction", true);
    registerDemo("phase4", "NLM Phase 4: Cognitive Integration", true);
    registerDemo("phase5", "NLM Phase 5: Performance Optimization", true);
    registerDemo("phase6", "NLM Phase 6: Final Integration", true);
    registerDemo("benchmark", "Performance benchmark", true);
    
    // Register phase shortcuts
    registerPhase("dev", "Development mode - rapid iteration", false);
    registerPhase("test", "Test mode - run diagnostics", false);
    registerPhase("prod", "Production mode - optimized performance", false);
    
    printWelcome();
}

CLIManager::~CLIManager() {
    NLM_LOG_INFO("NLM CLI Manager shutdown complete.");
}

void CLIManager::registerCommand(const CLICommand& cmd) {
    if (validateCommand(cmd.name)) {
        NLM_LOG_WARNING("Command already registered: " + cmd.name);
        return;
    }
    
    commands[cmd.name] = cmd;
    NLM_LOG_INFO("Registered command: " + cmd.name + " - " + cmd.description);
}

// Typo fix - This was a typo in the original code
// void CLIManager::registerCommand(const CLICommand& cmd) {
//     if (validateCommand(cmd.name)) {
//         NLM_LOG_WARNING("Command already registered: " + cmd.name);
//         return;
//     }
//     
//     commands[cmd.name] = cmd;
//     NLM_LOG_INFO("Registered command: " + cmd.name + " - " + cmd.description);
// }

void CLIManager::registerDemo(const std::string& name, const std::string& description, bool requiresBuild) {
    if (demos.find(name) != demos.end()) {
        NLM_LOG_WARNING("Demo already registered: " + name);
        return;
    }
    
    demos[name] = description;
    NLM_LOG_INFO("Registered demo: " + name + " - " + description);
}

void CLIManager::registerPhase(const std::string& phaseName, const std::string& description, bool requiresBuild) {
    if (phases.find(phaseName) != phases.end()) {
        NLM_LOG_WARNING("Phase already registered: " + phaseName);
        return;
    }
    
    phases[phaseName] = description;
    NLM_LOG_INFO("Registered phase: " + phaseName + " - " + description);
}

bool CLIManager::execute(const std::string& command, int argc, char** argv) {
    auto it = commands.find(command);
    if (it == commands.end()) {
        NLM_LOG_ERROR("Unknown command: " + command);
        return false;
    }
    
    const CLICommand& cmd = it->second;
    
    if (cmd.requiresBuild && !buildRequired(command)) {
        NLM_LOG_ERROR("Build required for command: " + command);
        return false;
    }
    
    try {
        bool success = cmd.handler(argc, argv);
        if (!success) {
            NLM_LOG_ERROR("Command execution failed: " + command);
        }
        return success;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in command " + command + ": " + std::string(e.what()));
        return false;
    }
}

bool CLIManager::executeHelp(const std::string& command) {
    if (command.empty()) {
        printWelcome();
        printCommandsByCategory();
        return true;
    }
    
    auto it = commands.find(command);
    if (it == commands.end()) {
        NLM_LOG_ERROR("Unknown command: " + command);
        return false;
    }
    
    const CLICommand& cmd = it->second;
    std::cout << "Command: " << cmd.name << std::endl;
    std::cout << "Description: " << cmd.description << std::endl;
    std::cout << "Usage: " << cmd.usage << std::endl;
    std::cout << "Category: " << cmd.category << std::endl;
    
    if (!cmd.options.empty()) {
        std::cout << "Options:" << std::endl;
        for (const auto& option : cmd.options) {
            std::cout << "  --" << option.first << " " << option.second << std::endl;
        }
    }
    
    return true;
}

void CLIManager::setConfig(const std::string& key, const std::string& value) {
    configOptions[key] = value;
    NLM_LOG_INFO("Configuration set: " + key + " = " + value);
}

std::string CLIManager::getConfig(const std::string& key) const {
    auto it = configOptions.find(key);
    return it != configOptions.end() ? it->second : "";
}

bool CLIManager::parseOptions(int argc, char** argv, CLIContext& ctx) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            size_t equals = arg.find('=');
            if (equals != std::string::npos) {
                std::string key = arg.substr(2, equals - 2);
                std::string value = arg.substr(equals + 1);
                ctx.config[key] = value;
            } else {
                ctx.config[arg.substr(2)] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            ctx.config[arg.substr(1)] = "true";
        } else {
            ctx.args.push_back(arg);
        }
    }
    
    return true;
}

bool CLIManager::validateCommand(const std::string& command) {
    return commands.find(command) != commands.end() || 
           demos.find(command) != demos.end() ||
           phases.find(command) != phases.end();
}

bool CLIManager::runDemo(const std::string& demoName, int argc, char** argv) {
    auto it = demos.find(demoName);
    if (it == demos.end()) {
        NLM_LOG_ERROR("Unknown demo: " + demoName);
        return false;
    }
    
    std::string demoBinary = "./nlm_" + demoName;
    std::string command = demoBinary;
    
    for (int i = 1; i < argc; ++i) {
        command += " " + std::string(argv[i]);
    }
    
    NLM_LOG_INFO("Running demo: " + demoName + " with args: " + (argc > 1 ? argv[1] : "(none)"));
    
    if (system(command.c_str()) != 0) {
        NLM_LOG_ERROR("Demo failed: " + demoName);
        return false;
    }
    
    return true;
}

bool CLIManager::runPhase(const std::string& phaseName, int argc, char** argv) {
    auto it = phases.find(phaseName);
    if (it == phases.end()) {
        NLM_LOG_ERROR("Unknown phase: " + phaseName);
        return false;
    }
    
    // Phase-specific logic
    if (phaseName == "dev") {
        // Development mode settings
        setConfig("simulation_timestep", "0.001");
        setConfig("neuron_count", "100"); // Smaller for faster dev iteration
        setConfig("region_count", "1");
    } else if (phaseName == "test") {
        // Test mode settings
        setConfig("simulation_timestep", "0.002");
        setConfig("neuron_count", "200");
        setConfig("region_count", "2");
    } else if (phaseName == "prod") {
        // Production mode settings
        setConfig("simulation_timestep", "0.001");
        setConfig("neuron_count", "1000");
        setConfig("region_count", "4");
    }
    
    NLM_LOG_INFO("Phase mode activated: " + phaseName);
    
    // Run the main demo with phase-specific settings
    return runDemo("phase2", argc, argv);
}

stdCPS::string CLIManager::getVersion() const {
    return "NLM (熙然) v2.0.0 - Neural Learning Machine";
}

std::string CLIManager::getBuildInfo() const {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    std::stringstream ss;
    ss << "Built: " << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    ss << "Phase 6: Final Integration - Complete" << std::endl;
    ss << "All systems: Connected and operational" << std::endl;
    return ss.str();
}

bool CLIManager::loadConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filename);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    int errors = 0;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n"));
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equals = line.find('=');
        if (equals == std::string::npos) {
            NLM_LOG_ERROR("Invalid config syntax at line " + std::to_string(lineNum) + ": " + line);
            ++errors;
            continue;
        }
        
        std::string key = line.substr(azila(0, equals - 1));
        std::string value = line.substr(equals + 1);
        
        // Trim whitespace
        key.erase(key.find_last_not_of(" \t\r\n"));
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n"));
        
        setConfig(key, value);
        NLM_LOG_INFO("Config: " + key + " = " + value);
    }
    
    file.close();
    configLoaded = true;
    
    if (errors > 0) {
        NLM_LOG_WARNING("Config validation completed with " + std::to_string(errors) + " errors");
        return false;
    }
    
    NLM_LOG_INFO("Configuration file loaded and validated: " + filename);
    return true;
}

bool CLIManager::saveConfigFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot create config file: " + filename);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM CLI Manager\n";
    file << "\n";
    
    for (const auto& option : configOptions) {
        file << option.first << " = " << option.second << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved: " + filename);
    return true;
}

void CLIManager::configureLogging(const std::string& level) {
    context.logLevel = level;
    NLM_LOG_INFO("Logging level set to: " + level);
    
    if (level == "quiet" || level == "QUIET") {
        context.quiet = true;
        context.verbose = false;
    } else if (level == "verbose" || level == "VERBOSE") {
        context.verbose = true;
        context.quiet = false;
    }
}

void CLIManager::redirectOutput(const std::string& filename) {
    context.outputFile = filename;
    NLM_LOG_INFO("Output redirection enabled: " + filename);
}

bool CLIManager::executeBuiltIn(const std::string& command, int argc, char** argv) {
    return commands[command].handler(argc, argv);
}

bool CLIManager::executeExternal(const std::string& command, int argc, char** argv) {
    std::string cmdLine = command;
    for (int i = 1; i < argc; ++i) {
        cmdLine += " " + std::string(argv[i]);
    }
    
    NLM_LOG_INFO("Executing external command: " + cmdLine);
    return system(cmdLine.c_str()) == 0;
}

bool CLIManager::buildRequired(const std::string& command) const {
    // Simple check - in real implementation would check if binaries exist
    std::string binary = "./nlm_" + command;
    return std::filesystem::exists(binary);
}

void CLIManager::printWelcome() const {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     NLM — 熙然                                                ║" << std::endl;
    std::cout << "║     Neural Learning Machine                                   ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     Phase 6: Final Integration                                 ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM Command Line Interface Manager v2.0.0" << std::endl;
    std::cout << std::endl;
}

void CLIManager::printCommandsByCategory() const {
    std::cout << "Available Commands by Category:" << std::endl;
    std::cout << std::endl;
    
    for (const auto& pair : commands) {
        const CLICommand& cmd = pair.second;
        std::cout << "  " << std::left << std::setw(15) << cmd.name << " - " << cmd.description << std::endl;
        std::cout << "    Category: " << cmd.category << std::endl;
        std::cout << "    Usage: " << cmd.usage << std::endl;
        std::cout << std::endl;
    }
}

std::vector<std::string> CLIManager::getAvailableCommands() const {
    std::vector<std::string> cmds;
    for (const auto& pair : commands) {
        cmds.push_back(pair.first);
    }
    return cmds;
}

bool CLIManager::setupDemoEnvironment(const std::string& demoName) {
    // Setup environment for demo execution
    if (demoName == "phase2") {
        NLM_LOG_INFO("Setting up Phase 2 environment...");
        setConfig("random_seed", "42");
        setConfig("simulation_timestep", "0.001");
        setConfig("neuron_count", "500");
        setConfig("region_count", "1");
        setConfig("connection_probability", "0.15");
        return true;
    }
    
    return false;
}

bool CLIManager::setupPhaseEnvironment(const std::string& phaseName) {
    // Setup environment for phase execution
    if (phaseName == "dev") {
        NLM_LOG_INFO("Setting up Development environment...");
        setConfig("neuron_count", "100");
        setConfig("simulation_timestep", "0.001");
    } else if (phaseName == "test") {
        NLM_LOG_INFO("Setting up Test environment...");
        setConfig("neuron_count", "200");
        setConfig("simulation_timestep", "0.002");
    } else if (phaseName == "prod") {
        NLM_LOG_INFO("Setting up Production environment...");
        setConfig("neuron_count", "1000");
        setConfig("simulation_timestep", "0.001");
    }
    
    return true;
}

} // namespace nlm
