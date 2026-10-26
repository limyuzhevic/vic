#include "CommandLineParser.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace nlm {

struct CommandLineParser::Impl {
    std::map<std::string, std::string> defaultFlags;
    std::map<std::string, std::function<void(CommandLineParser&, const std::string&)>> subcommands;
};

CommandLineParser::CommandLineParser() : mode(CommandMode::Help) {
    pImpl = std::make_unique<Impl>();
    config = std::make_shared<Config>();
    
    // Initialize default flags
    pImpl->defaultFlags["-h"] = "help";
    pImpl->defaultFlags["--help"] = "help";
    pImpl->defaultFlags["-v"] = "version";
    pImpl->defaultFlags["--version"] = "version";
    pImpl->defaultFlags["-c"] = "config";
    pImpl->defaultFlags["--config"] = "config";
    pImpl->defaultFlags["-d"] = "demo";
    pImpl->defaultFlags["--demo"] = "demo";
    
    // Initialize subcommands
    pImpl->subcommands["demo"] = [this](CommandLineParser& parser, const std::string& arg) {
        if (arg.empty()) {
            parser.mode = CommandMode::Help;
        } else {
            if (arg == "phase3" || arg == "p3") {
                parser.mode = CommandMode::Phase3Demo;
            } else if (arg == "phase4" || arg == "p4") {
                parser.mode = CommandMode::Phase4Demo;
            } else if (arg == "phase6" || arg == "p6") {
                parser.mode = CommandMode::Phase6Demo;
            } else {
                parser.mode = CommandMode::Custom;
            }
        }
    };
    
    pImpl->subcommands["config"] = [this](CommandLineParser& parser, const std::string& arg) {
        if (!arg.empty()) {
            // Load configuration from file
            if (parser.config->loadFromFile(arg)) {
                std::cout << "Configuration loaded from: " << arg << std::endl;
            } else {
                std::cerr << "Failed to load configuration from: " << arg << std::endl;
            }
        } else {
            parser.mode = CommandMode::Config;
        }
    };
    
    pImpl->subcommands["debug"] = [this](CommandLineParser& parser, const std::string& arg) {
        parser.mode = CommandMode::Debug;
        // Enable debug output
        // This could be extended to support various debug options
    };
    
    pImpl->subcommands["batch"] = [this](CommandLineParser& parser, const std::string& arg) {
        parser.mode = CommandMode::Batch;
        // Process batch operations
        // This could be extended to support batch processing options
    };
}

CommandLineParser::~CommandLineParser() = default;

bool CommandLineParser::parse(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parseArguments(args);
}

bool CommandLineParser::parseArguments(const std::vector<std::string>& args) {
    if (args.empty()) {
        mode = CommandMode::Help;
        return true;
    }
    
    bool modeParsed = false;
    bool configParsed = false;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        // Check if it's a flag
        if (arg.substr(0, 2) == "--" || (arg.size() == 2 && arg[0] == '-')) {
            // Handle flags
            std::string flag = arg;
            if (flag.substr(0, 2) == "--") {
                flag = flag.substr(2);
            } else {
                flag = flag.substr(1);
            }
            
            // Check if it's a known flag
            if (pImpl->defaultFlags.find(flag) != pImpl->defaultFlags.end()) {
                std::string flagAction = pImpl->defaultFlags[flag];
                
                if (flagAction == "help") {
                    mode = CommandMode::Help;
                    modeParsed = true;
                } else if (flagAction == "version") {
                    mode = CommandMode::Version;
                    modeParsed = true;
                } else if (flagAction == "config") {
                    // Next argument should be config file path
                    if (i + 1 < args.size()) {
                        std::string configFile = args[++i];
                        if (config->loadFromFile(configFile)) {
                            std::cout << "Configuration loaded from: " << configFile << std::endl;
                        } else {
                            std::cerr << "Failed to load configuration from: " << configFile << std::endl;
                        }
                        configParsed = true;
                    }
                } else if (flagAction == "demo") {
                    // Next argument should be demo type
                    if (i + 1 < args.size()) {
                        pImpl->subcommands["demo"](*this, args[++i]);
                        modeParsed = true;
                    }
                }
            } else {
                // Unknown flag
                std::cerr << "Unknown flag: " << arg << std::endl;
                return false;
            }
        } else {
            // Check if it matches a subcommand
            bool subcommandMatched = false;
            for (const auto& [cmd, func] : pImpl->subcommands) {
                if (arg == cmd) {
                    func(*this, ""); // Call with empty argument to check mode
                    subcommandMatched = true;
                    break;
                }
            }
            
            if (!subcommandMatched) {
                // It's an additional argument
                additionalArgs.push_back(arg);
                
                // Check if there's a subcommand expecting an argument
                if (i + 1 < args.size() && pImpl->subcommands.find(args[i + 1]) != pImpl->subcommands.end()) {
                    // Skip the subcommand argument as it will be handled separately
                    ++i;
                }
            }
        }
    }
    
    if (!modeParsed) {
        // Default to demo mode if no specific mode was specified
        mode = CommandMode::Phase3Demo;
    }
    
    return true;
}

bool CommandLineParser::hasFlag(const std::string& flag) const {
    return flags.find(flag) != flags.end();
}

std::string CommandLineParser::getFlag(const std::string& flag) const {
    auto it = flags.find(flag);
    if (it != flags.end()) {
        return it->second;
    }
    return "";
}

std::string CommandLineParser::getArgument(size_t index) const {
    if (index < additionalArgs.size()) {
        return additionalArgs[index];
    }
    return "";
}

std::string CommandLineParser::getHelp(const std::string& programName) {
    std::ostringstream help;
    
    help << "Usage: " << programName << " [OPTIONS] [ARGS]" << std::endl;
    help << std::endl;
    help << "Commands:" << std::endl;
    help << "  demo[PHASE3|PHASE4|PHASE6|CUSTOM]  Run demo mode" << std::endl;
    help << "  config[FILE]                 Load configuration from file" << std::endl;
    help << "  debug                         Enable interactive debugging" << std::endl;
    help << "  batch                         Run batch processing" << std::endl;
    help << std::endl;
    help << "Options:" << std::endl;
    help << "  -h, --help                   Show this help message" << std::endl;
    help << "  -v, --version                Show version information" << std::endl;
    help << "  -c, --config FILE            Load configuration from file" << std::endl;
    help << "  -d, --demo TYPE              Specify demo type (default: phase3)" << std::endl;
    help << std::endl;
    help << "Examples:" << std::endl;
    help << "  " << programName << " demo phase3                    Run Phase 3 demo" << std::endl;
    help << "  " << programName << " config myconfig.json             Load configuration" << std::endl;
    help << "  " << programName << " debug                          Start interactive debugger" << std::endl;
    help << std::endl;
    help << "For more information, visit: https://nlm-project.org/docs" << std::endl;
    
    return help.str();
}

std::string CommandLineParser::getVersion() {
    return "NLM (熙然) Neural Learning Machine v0.1.0";
}

void CommandLineParser::printHelp(const std::string& programName) const {
    std::cout << getHelp(programName);
}

void CommandLineParser::printVersion() const {
    std::cout << getVersion();
}

} // namespace nlm