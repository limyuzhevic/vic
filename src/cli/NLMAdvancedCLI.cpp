#include "NLMAdvancedCLI.hpp"

namespace nlm {
namespace cli {

NLMAdvancedCLI::NLMAdvancedCLI(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain_(brain), config_(config), interactiveMode_(false) {
    cli_ = AdvancedCLI(brain_, config_);
}

void NLMAdvancedCLI::initialize() {
    // Register all command categories
    registerMemoryCommands(cli_);
    registerNeuromodulationCommands(cli_);
    registerStateCommands(cli_);
    registerSimulationCommands(cli_);
    registerScriptingAndDiagnosticsCommands(cli_);
}

int NLMAdvancedCLI::processArguments(int argc, char** argv) {
    // Check for advanced CLI mode
    bool advancedMode = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--advanced" || arg == "--cli" || arg == "--interactive" || 
            arg == "-a" || arg == "-c" || arg == "-i") {
            advancedMode = true;
            break;
        }
    }
    
    if (advancedMode) {
        // Run advanced CLI
        return runInteractiveSession();
    }
    
    // Check for help commands
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "help") {
            showHelp();
            return 0;
        }
    }
    
    // Run normal NLM execution
    return 0;
}

int NLMAdvancedCLI::runInteractiveSession() {
    interactiveMode_ = true;
    
    if (!quiet_) {
        std::cout << "=== NLM Advanced CLI ===" << std::endl;
        std::cout << "Type 'help' for available commands, 'exit' or 'quit' to leave." << std::endl;
        std::cout << std::endl;
    }
    
    std::string line;
    while (true) {
        if (!quiet_) {
            if (verbose_) {
                std::cout << ">>> " << std::flush;
            } else {
                std::cout << ">>> " << std::flush;
            }
        }
        
        if (!std::getline(std::cin, line)) {
            break; // EOF
        }
        
        if (!processCommandLine(line)) {
            break; // Exit command
        }
    }
    
    interactiveMode_ = false;
    return 0;
}

bool NLMAdvancedCLI::processCommandLine(const std::string& line) {
    if (line.empty()) {
        return true;
    }
    
    if (line == "exit" || line == "quit" || line == "q") {
        if (!quiet_) {
            std::cout << "Exiting NLM Advanced CLI..." << std::endl;
        }
        return false;
    }
    
    if (line == "clear" || line == "cls") {
        std::system("clear");
        showHelp();
        return true;
    }
    
    try {
        return executeCommand(parseCommandLine(split(line)));
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return true;
    }
}

void NLMAdvancedCLI::showHelp() const {
    std::cout << "=== NLM Advanced CLI Help ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help              - Show this help message" << std::endl;
    std::cout << "  commands          - List all available commands" << std::endl;
    std::cout << "  exit/quit/q      - Exit CLI" << std::endl;
    std::cout << "  clear/cls         - Clear screen" << std::endl;
    std::cout << std::endl;
    std::cout << "Command Categories:" << std::endl;
    std::cout << "  memory            - Working memory and episodic memory commands" << std::endl;
    std::cout << "  neuromodulation    - Neuromodulation control and monitoring" << std::endl;
    std::cout << "  state_analysis     - Brain state inspection and analysis" << std::endl;
    std::cout << "  simulation_control - Simulation control (pause, resume, step, jump)" << std::endl;
    std::cout << "  visualization      - Real-time monitoring and visualization" << std::endl;
    std::cout << "  configuration      - Advanced configuration management" << std::endl;
    std::cout << "  script            - Script-based control and batch processing" << std::endl;
    std::cout << "  diagnostic         - Error detection and system health monitoring" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  working_memory info" << std::endl;
    std::cout << "  neuromodulation get_levels" << std::endl;
    std::cout << "  state_analysis neurons --limit 10" << std::endl;
    std::cout << "  simulation_control step --count 100" << std::endl;
    std::cout << "  script execute --commands \"working_memory info; neuromodulation get_levels\"" << std::endl;
    std::cout << "  diagnostic system" << std::endl;
    std::cout << std::endl;
    std::cout << "For detailed help on a specific command, type: help <command_name>" << std::endl;
}

NLMAdvancedCLI::ParsedCommand NLMAdvancedCLI::parseCommandLine(const std::vector<std::string>& tokens) const {
    ParsedCommand parsed;
    
    if (tokens.empty()) {
        return parsed;
    }
    
    parsed.commandName = toLower(tokens[0]);
    
    for (size_t i = 1; i < tokens.size(); ++i) {
        std::string token = tokens[i];
        
        // Check for option (--key value or --key=value)
        if (token.size() >= 2 && token.substr(0, 2) == "--") {
            std::string key = token.substr(2);
            std::string value;
            
            // Check for --key=value format
            size_t equalsPos = key.find('=');
            if (equalsPos != std::string::npos) {
                value = key.substr(equalsPos + 1);
                key = key.substr(0, equalsPos);
            } else if (i + 1 < tokens.size() && tokens[i + 1].size() >= 2 && 
                      tokens[i + 1].substr(0, 2) != "--") {
                // Check next token for value
                value = tokens[++i];
            }
            
            parsed.options[key] = value;
        } else {
            // Treat as argument
            parsed.arguments.push_back(token);
        }
    }
    
    return parsed;
}

bool NLMAdvancedCLI::executeCommand(const ParsedCommand& parsed) const {
    if (parsed.commandName == "help") {
        if (parsed.arguments.empty()) {
            cli_.showHelp();
        } else {
            cli_.showHelp(parsed.arguments[0]);
        }
        return true;
    }
    
    if (parsed.commandName == "commands") {
        cli_.showCommands();
        return true;
    }
    
    return cli_.executeCommand(parsed.commandName, parsed.arguments);
}

void NLMAdvancedCLI::setupCommandOptions(ParsedCommand& parsed, const std::vector<std::string>& tokens) const {
    // This is a helper method that would be used by commands
    // to set up their options from the parsed tokens
}

} // namespace cli
} // namespace nlm
