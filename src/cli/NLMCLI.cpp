#include "NLMCLI.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

NLMCLI::NLMCLI() : version("0.4.0") {
    // Initialize default commands
    // Commands will be registered by the application
}

NLMCLI::~NLMCLI() {
}

int NLMCLI::run(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    processOptions(args);
    
    if (globalOptions.verbose) {
        std::cout << "NLM Neural Network Simulator v" << version << std::endl;
    }
    
    // Handle global options
    if (globalOptions.verbose && !globalOptions.quiet) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    if (globalOptions.logLevel.empty() && !globalOptions.verbose && !globalOptions.quiet) {
        globalOptions.logLevel = "info";
    }
    
    if (!globalOptions.simulationFile.empty()) {
        args.insert(args.begin(), globalOptions.simulationFile);
    }
    
    if (!globalOptions.experimentConfig.empty()) {
        args.insert(args.begin() + 1, "--experiment-config=" + globalOptions.experimentConfig);
    }
    
    if (args.empty()) {
        printHelp();
        return 1;
    }
    
    // Execute command
    std::string commandName = args[0];
    args.erase(args.begin());
    
    return executeCommand(commandName, args);
}

void NLMCLI::registerCommand(const std::string& name, std::unique_ptr<CLICommand> command) {
    commands[name] = std::move(command);
}

void NLMCLI::printHelp() const {
    std::cout << "NLM Neural Network Simulator v" << version << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "    nlm [OPTIONS] COMMAND [ARGS]..." << std::endl;
    std::cout << std::endl;
    std::cout << "COMMANDS:" << std::endl;
    
    for (const auto& pair : commands) {
        const auto& command = pair.second;
        std::cout << "    " << command->getName();
        if (!command->getDescription().empty()) {
            std::cout << "\t" << command->getDescription();
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "    --version                     Show version information" << std::endl;
    std::cout << "    --help                        Show global help" << std::endl;
    std::cout << "    --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << "    --quiet, -q                   Suppress non-essential output" << std::endl;
    std::cout << "    --config FILE, -c FILE        Configuration file path" << std::endl;
    std::cout << "    --output-dir DIR, -o DIR      Output directory for results" << std::endl;
    std::cout << "    --log-level LEVEL             Set logging level (debug, info, warning, error)" << std::endl;
    std::cout << "    --thread-count N              Set number of worker threads" << std::endl;
    std::cout << std::endl;
    std::cout << "GLOBAL ARGUMENTS:" << std::endl;
    std::cout << "    SIMULATION_FILE              Path to simulation configuration file" << std::endl;
    std::cout << "    EXPERIMENT_CONFIG            Path to experiment configuration" << std::endl;
}

void NLMCLI::printVersion() const {
    std::cout << "NLM Neural Network Simulator v" << version << std::endl;
}

void NLMCLI::parseArguments(int argc, char* argv[]) {
    // Arguments are parsed by processOptions
}

void NLMCLI::processOptions(std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::string& arg = args[i];
        
        if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--help") {
            printHelp();
            exit(0);
        } else if (arg == "-v" || arg == "--verbose") {
            globalOptions.verbose = true;
            args.erase(args.begin() + i);
            --i;
        } else if (arg == "-q" || arg == "--quiet") {
            globalOptions.quiet = true;
            args.erase(args.begin() + i);
            --i;
        } else if (arg.find("--config=") == 0 || arg.find("-c=") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                globalOptions.configFile = arg.substr(pos + 1);
                args.erase(args.begin() + i);
                --i;
            }
        } else if (arg.find("--output-dir=") == 0 || arg.find("-o=") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                globalOptions.outputDir = arg.substr(pos + 1);
                args.erase(args.begin() + i);
                --i;
            }
        } else if (arg.find("--log-level=") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                globalOptions.logLevel = arg.substr(pos + 1);
                args.erase(args.begin() + i);
                --i;
            }
        } else if (arg.find("--thread-count=") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                try {
                    globalOptions.threadCount = std::stoi(arg.substr(pos + 1));
                } catch (...) {
                    std::cerr << "Error: Invalid thread count" << std::endl;
                    exit(1);
                }
                args.erase(args.begin() + i);
                --i;
            }
        } else if (arg.find("--experiment-config=") == 0) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                globalOptions.experimentConfig = arg.substr(pos + 1);
                args.erase(args.begin() + i);
                --i;
            }
        } else if (arg != "--verbose" && arg != "--quiet" && 
                   arg.find("--") == 0 && arg.find("=") == std::string::npos) {
            // Check if it's a known option without value
            if (arg != "--help" && arg != "--version") {
                std::cerr << "Error: Unknown option " << arg << std::endl;
                exit(1);
            }
        }
    }
}

int NLMCLI::executeCommand(const std::string& commandName, const std::vector<std::string>& args) {
    auto it = commands.find(commandName);
    if (it == commands.end()) {
        std::cerr << "Error: Unknown command '" << commandName << "'" << std::endl;
        return 1;
    }
    
    return it->second->execute(args);
}

} // namespace nlm
