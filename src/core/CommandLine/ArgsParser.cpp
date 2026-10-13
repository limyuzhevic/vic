#include "ArgsParser.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace nlm {

struct CommandLineParser::Impl {
    int argc;
    char** argv;
    
    struct OptionInfo {
        std::string longName;
        char shortName;
        std::string description;
        bool requiresValue;
        bool found;
        std::string value;
    };
    
    std::vector<OptionInfo> options;
    std::vector<std::string> positionalArgs;
    
    Impl(int argc, char** argv) : argc(argc), argv(argv) {}
    
    void parse() {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                // Mark help as found
                for (auto& opt : options) {
                    if (opt.longName == "help") {
                        opt.found = true;
                        break;
                    }
                }
            } else if (arg.substr(0, 2) == "--") {
                std::string option = arg.substr(2);
                auto it = findOption(option);
                if (it != options.end()) {
                    it->found = true;
                    if (it->requiresValue) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            it->value = argv[++i];
                        } else {
                            throw std::runtime_error("Option --" + option + " requires a value");
                        }
                    } else {
                        // Check if value follows immediately (e.g., --option=value)
                        size_t equalsPos = option.find('=');
                        if (equalsPos != std::string::npos) {
                            it->value = option.substr(equalsPos + 1);
                            option = option.substr(0, equalsPos);
                            it = findOption(option);
                            if (it != options.end()) {
                                it->found = true;
                            }
                        }
                    }
                } else if (option.find('=') != std::string::npos) {
                    // Handle custom options with values
                    std::string key = option.substr(0, option.find('='));
                    std::string value = option.substr(option.find('=') + 1);
                    positionalArgs.push_back(key + "=" + value);
                } else {
                    // Treat unknown long option as positional
                    positionalArgs.push_back(arg);
                }
            } else if (arg.substr(0, 1) == "-") {
                // Short option
                for (size_t j = 1; j < arg.length(); ++j) {
                    char shortOpt = arg[j];
                    auto it = findShortOption(shortOpt);
                    if (it != options.end()) {
                        it->found = true;
                        if (it->requiresValue) {
                            if (j + 1 < arg.length()) {
                                it->value = arg.substr(j + 1);
                                break;
                            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                                it->value = argv[++i];
                            } else {
                                throw std::runtime_error(std::string("Option -") + shortOpt + " requires a value");
                            }
                        }
                    }
                }
            } else {
                // Positional argument
                positionalArgs.push_back(arg);
            }
        }
    }
    
    std::vector<OptionInfo>::iterator findOption(const std::string& name) {
        for (auto& opt : options) {
            if (opt.longName == name) {
                return &opt;
            }
        }
        return options.end();
    }
    
    std::vector<OptionInfo>::const_iterator findOption(const std::string& name) const {
        for (const auto& opt : options) {
            if (opt.longName == name) {
                return &opt;
            }
        }
        return options.end();
    }
    
    std::vector<OptionInfo>::iterator findShortOption(char shortOpt) {
        for (auto& opt : options) {
            if (opt.shortName == shortOpt) {
                return &opt;
            }
        }
        return options.end();
    }
    
    std::vector<OptionInfo>::const_iterator findShortOption(char shortOpt) const {
        for (const auto& opt : options) {
            if (opt.shortName == shortOpt) {
                return &opt;
            }
        }
        return options.end();
    }
    
    std::string getHelp() const {
        std::ostringstream oss;
        oss << "Usage: nlm [OPTIONS] [EXPERIMENT_TYPE]\n\n";
        oss << "Options:\n";
        for (const auto& opt : options) {
            oss << "  ";
            if (opt.shortName) {
                oss << "-" << opt.shortName << ", ";
            }
            oss << "--" << opt.longName;
            if (opt.requiresValue) {
                oss << "=<value>";
            }
            oss << "\n";
            
            // Indent description
            size_t indent = 8;
            if (opt.shortName) {
                indent += 5; // Account for "-X, "
            }
            oss << std::string(indent, ' ') << opt.description << "\n";
        }
        return oss.str();
    }
};

CommandLineParser::CommandLineParser(int argc, char** argv) : pImpl(new Impl(argc, argv)) {
    // Initialize with common options
    addOption("help", 'h', "Show this help message", false);
    addOption("batch", 'b', "Run all experiments in batch mode", false);
    addOption("interactive", 'i', "Run experiments in interactive mode", false);
    addOption("config", 'c', "Load configuration from file", true);
    addOption("save", 's', "Save brain state to file", true);
    addOption("load", 'l', "Load brain state from file", true);
    addOption("max-steps", 0, "Set maximum simulation steps", true);
    addOption("seed", 0, "Set random seed", true);
    addOption("metrics", 0, "Enable metrics collection", false);
    
    // Parse arguments
    pImpl->parse();
}

ParsedArgs CommandLineParser::parse() const {
    ParsedArgs args;
    
    // Extract experiment type (first positional argument)
    for (const auto& arg : pImpl->positionalArgs) {
        if (arg.find('=') == std::string::npos) {  // Not an option=value pair
            args.experimentType = arg;
            break;
        }
    }
    
    // Check options
    for (const auto& opt : pImpl->options) {
        if (!opt.found) continue;
        
        if (opt.longName == "batch") {
            args.enableBatch = true;
        } else if (opt.longName == "interactive") {
            args.enableInteractive = true;
        } else if (opt.longName == "help") {
            args.enableHelp = true;
        } else if (opt.longName == "config") {
            args.configFile = opt.value;
        } else if (opt.longName == "save") {
            args.saveFile = opt.value;
        } else if (opt.longName == "load") {
            args.loadFile = opt.value;
        } else if (opt.longName == "max-steps") {
            try {
                args.maxSteps = std::stoll(opt.value);
            } catch (...) {
                // Ignore parsing errors
            }
        } else if (opt.longName == "seed") {
            try {
                args.seed = std::stoull(opt.value);
            } catch (...) {
                // Ignore parsing errors
            }
        } else if (opt.longName == "metrics") {
            args.enableMetrics = true;
        } else {
            args.customArgs.push_back("--" + opt.longName);
            if (!opt.value.empty()) {
                args.customArgs.push_back(opt.value);
            }
            args.optionMap[opt.longName] = opt.value;
        }
    }
    
    return args;
}

void CommandLineParser::addOption(const std::string& longName, char shortName,
                                 const std::string& description, bool requiresValue) {
    Impl::OptionInfo opt;
    opt.longName = longName;
    opt.shortName = shortName;
    opt.description = description;
    opt.requiresValue = requiresValue;
    opt.found = false;
    
    pImpl->options.push_back(opt);
}

std::string CommandLineParser::getHelp() const {
    return pImpl->getHelp();
}

bool CommandLineParser::hasOption(const std::string& name) const {
    auto it = pImpl->findOption(name);
    return it != pImpl->options.end() && it->found;
}

std::string CommandLineParser::getOptionValue(const std::string& name) const {
    auto it = pImpl->findOption(name);
    if (it != pImpl->options.end() && it->found) {
        return it->value;
    }
    return "";
}

const std::vector<std::string>& CommandLineParser::getPositionalArgs() const {
    return pImpl->positionalArgs;
}

} // namespace nlm