// Command Line Parser Implementation

#include "CommandLineParser.hpp"
#include "NLMSession.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {
namespace cli {

CommandLineParser::CommandLineParser(std::shared_ptr<PluginManager> pluginManager,
                                    std::shared_ptr<Config> config,
                                    std::shared_ptr<Logger> logger)
    : pluginManager_(pluginManager), config_(config), logger_(logger) {
    if (logger_) {
        NLM_LOG_INFO("CommandLineParser initialized");
    }
    initializeCommands();
}

void CommandLineParser::initializeCommands() {
    // Brain state commands
    registerCommand({
        "save",
        "Save brain state to file",
        "save <filename>",
        [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                if (logger_) NLM_LOG_ERROR("Usage: save <filename>");
                return false;
            }
            return executeCommand("save", args, nullptr);
        },
        {"-f", "--file"},
        {{}, {"filename"}},
        false
    });
    
    registerCommand({
        "load",
        "Load brain state from file",
        "load <filename>",
        [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                if (logger_) NLM_LOG_ERROR("Usage: load <filename>");
                return false;
            }
            return executeCommand("load", args, nullptr);
        },
        {"-f", "--file"},
        {{}, {"filename"}},
        false
    });
    
    registerCommand({
        "reset",
        "Reset brain state",
        "reset",
        [this](const std::vector<std::string>& args) {
            return executeCommand("reset", args, nullptr);
        },
        {},
        {}
    });
    
    // Experiment commands
    registerCommand({
        "run",
        "Run experiment",
        "run <experiment_name> [args...]",
        [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                if (logger_) NLM_LOG_ERROR("Usage: run <experiment_name> [args...]");
                return false;
            }
            return executeCommand("run", args, nullptr);
        },
        {},
        {{}, {"experiment_name"}, {"args..."}},
        false
    });
    
    // Plugin commands
    registerCommand({
        "plugins",
        "List loaded plugins",
        "plugins",
        [this](const std::vector<std::string>& args) {
            return executeCommand("plugins", args, nullptr);
        },
        {},
        {}
    });
    
    registerCommand({
        "load-plugin",
        "Load a plugin",
        "load-plugin <plugin_name> [path]",
        [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                if (logger_) NLM_LOG_ERROR("Usage: load-plugin <plugin_name> [path]");
                return false;
            }
            return executeCommand("load-plugin", args, nullptr);
        },
        {},
        {{}, {"plugin_name"}, {"[path]"}},
        false
    });
    
    registerCommand({
        "unload-plugin",
        "Unload a plugin",
        "unload-plugin <plugin_name>",
        [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                if (logger_) NLM_LOG_ERROR("Usage: unload-plugin <plugin_name>");
                return false;
            }
            return executeCommand("unload-plugin", args, nullptr);
        },
        {},
        {{}, {"plugin_name"}},
        false
    });
    
    // Performance monitoring
    registerCommand({
        "profile",
        "Start performance profiling",
        "profile",
        [this](const std::vector<std::string>& args) {
            return executeCommand("profile", args, nullptr);
        },
        {},
        {}
    });
    
    registerCommand({
        "stats",
        "Show performance statistics",
        "stats",
        [this](const std::vector<std::string>& args) {
            return executeCommand("stats", args, nullptr);
        },
        {},
        {}
    });
    
    // Batch processing
    registerCommand({
        "batch",
        "Run batch script",
        "batch <script_file>",
        [this](const std::vector<std::string>& args) {
            if (args.size() < 1) {
                if (logger_) NLM_LOG_ERROR("Usage: batch <script_file>");
                return false;
            }
            return executeCommand("batch", args, nullptr);
        },
        {},
        {{}, {"script_file"}},
        false
    });
    
    registerCommand({
        "help",
        "Show help information",
        "help [command]",
        [this](const std::vector<std::string>& args) {
            if (args.empty()) {
                printHelp();
            } else {
                executeCommand("help", args, nullptr);
            }
            return true;
        },
        {},
        {{}, {"[command]"}},
        false
    });
}

bool CommandLineParser::parse(int argc, char** argv, std::shared_ptr<NLMSession> session) {
    if (argc < 2) {
        if (interactiveMode_) {
            runInteractiveMode(session);
            return true;
        } else {
            printHelp();
            return false;
        }
    }
    
    std::string command = argv[1];
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    return executeCommand(command, args, session);
}

void CommandLineParser::printHelp() const {
    std::cout << "NLM Advanced CLI Commands:" << std::endl;
    std::cout << "=========================" << std::endl << std::endl;
    
    for (const auto& pair : commands_) {
        const auto& cmd = pair.second;
        std::cout << "  " + cmd.name + " - " + cmd.description << std::endl;
        std::cout << "    Usage: " + cmd.usage << std::endl;
        if (!cmd.flags.empty()) {
            std::cout << "    Flags:" << std::endl;
            for (const auto& flag : cmd.flags) {
                std::cout << "      -" + flag << std::endl;
            }
        }
        if (!cmd.options.empty()) {
            std::cout << "    Options:" << std::endl;
            for (const auto& opt : cmd.options) {
                if (opt.first.empty()) {
                    std::cout << "      " + opt.second << std::endl;
                } else {
                    std::cout << "      -" + opt.first + ", --" + opt.second << std::endl;
                }
            }
        }
        std::cout << std::endl;
    }
}

void CommandLineParser::printVersion() const {
    std::cout << "NLM Advanced CLI" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
}

void CommandLineParser::registerCommand(const Command& cmd) {
    commands_[cmd.name] = cmd;
}

bool CommandLineParser::executeCommand(const std::string& cmdName, 
                                      const std::vector<std::string>& args,
                                      std::shared_ptr<NLMSession> session) {
    logCommand(cmdName);
    
    auto it = commands_.find(cmdName);
    if (it == commands_.end()) {
        if (logger_) {
            NLM_LOG_ERROR("Unknown command: " + cmdName);
        }
        std::cout << "Unknown command: " + cmdName << std::endl;
        printHelp();
        return false;
    }
    
    const auto& cmd = it->second;
    
    if (!validateArguments(cmd, args)) {
        if (logger_) {
            NLM_LOG_ERROR("Invalid arguments for command: " + cmdName);
        }
        std::cout << "Invalid arguments for command: " + cmdName << std::endl;
        return false;
    }
    
    if (cmd.requiresSession && !session) {
        if (logger_) {
            NLM_LOG_ERROR("Command requires an active session");
        }
        std::cout << "Error: Command requires an active session" << std::endl;
        return false;
    }
    
    try {
        return cmd.callback(args);
    } catch (const std::exception& e) {
        if (logger_) {
            NLM_LOG_ERROR(std::string("Command execution error: ") + e.what());
        }
        std::cout << "Error: " + std::string(e.what()) << std::endl;
        return false;
    }
}

std::string CommandLineParser::autoComplete(const std::string& partial) const {
    std::string result;
    for (const auto& pair : commands_) {
        if (pair.first.find(partial) == 0) {
            result += pair.first + " ";
        }
    }
    return result;
}

void CommandLineParser::logCommand(const std::string& command) const {
    commandHistory_.push_back(command);
    if (logger_) {
        NLM_LOG_INFO("CLI command: " + command);
    }
}

void CommandLineParser::logStatus() const {
    if (!logger_) return;
    
    NLM_LOG_INFO("=== Command Line Parser Status ===");
    NLM_LOG_INFO("Commands registered: " + std::to_string(commands_.size()));
    NLM_LOG_INFO("Interactive mode: " + std::string(interactiveMode_ ? "enabled" : "disabled"));
    NLM_LOG_INFO("Command history size: " + std::to_string(commandHistory_.size()));
}

bool CommandLineParser::validateArguments(const Command& cmd, const std::vector<std::string>& args) {
    // Basic validation - can be extended with more sophisticated checking
    size_t minArgs = 0;
    size_t maxArgs = 0;
    
    // Estimate based on options and flags
    for (const auto& opt : cmd.options) {
        if (opt.first.empty()) minArgs++;
    }
    
    return args.size() >= minArgs && args.size() <= maxArgs + 100; // +100 for flexible args
}

void CommandLineParser::runInteractiveMode(std::shared_ptr<NLMSession> session) {
    std::string line;
    std::cout << "NLM Interactive Mode" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl << std::endl;
    
    while (interactiveMode_) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        if (line == "exit" || line == "quit") {
            break;
        }
        
        if (line == "history") {
            std::cout << "Command history:" << std::endl;
            for (size_t i = 0; i < commandHistory_.size(); ++i) {
                std::cout << "  " + std::to_string(i + 1) + ". " + commandHistory_[i] << std::endl;
            }
            continue;
        }
        
        runCommandLine(line, session);
    }
}

void CommandLineParser::runCommandLine(const std::string& line, std::shared_ptr<NLMSession> session) {
    std::istringstream iss(line);
    std::string word;
    std::vector<std::string> args;
    
    while (iss >> word) {
        args.push_back(word);
    }
    
    if (!args.empty()) {
        executeCommand(args[0], std::vector<std::string>(args.begin() + 1, args.end()), session);
    }
}

} // namespace cli
} // namespace nlm
