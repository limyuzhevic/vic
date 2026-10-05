#include "CommandLineParser.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>

namespace nlm {

struct CommandLineParser::Impl {
    std::string validationErrors;
    std::string lastError;
};

CommandLineParser::CommandLineParser() : pImpl(std::make_unique<Impl>()) {}
CommandLineParser::~CommandLineParser() = default;

void CommandLineParser::addSubcommand(const std::string& name, const std::string& description, SubcommandHandler handler) {
    subcommands_[name] = std::make_pair(description, handler);
}

void CommandLineParser::addOption(const std::string& name, const std::string& description, char shortForm,
                                 const std::string& defaultValue, const std::string& type,
                                 bool isFlag, bool required) {
    CLIOption option;
    option.name = name;
    option.description = description;
    option.shortForm = shortForm;
    option.required = required;
    option.defaultValue = defaultValue;
    option.type = type;
    option.isFlag = isFlag;
    options_[name] = option;
}

bool CommandLineParser::parse(int argc, char** argv, std::ostream& output) {
    // Clear previous state
    currentSubcommand_.clear();
    subcommandArgs_.clear();
    parsedValues_.clear();
    parsedFlags_.clear();
    pImpl->validationErrors.clear();
    pImpl->lastError.clear();
    
    std::vector<std::string> tokens;
    for (int i = 1; i < argc; ++i) {
        tokens.push_back(argv[i]);
    }
    
    return parseArgs(tokens, output);
}

bool CommandLineParser::parseArgs(std::vector<std::string>& tokens, std::ostream& output) {
    // Check for --help
    for (const auto& token : tokens) {
        if (token == "--help" || token == "-h") {
            printHelp(output);
            return false;
        }
    }
    
    // Check for version
    for (const auto& token : tokens) {
        if (token == "--version") {
            output << "NLM Phase 6.0 - Neural Learning Machine" << std::endl;
            return false;
        }
    }
    
    // Check for backward compatibility
    if (hasBackwardCompatArgs(tokens.size(), nullptr)) {
        return parseBackwardCompatArgs(tokens.size(), nullptr, output);
    }
    
    // Parse subcommand and options
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string token = tokens[i];
        
        // Subcommand
        if (token.substr(0, 2) == "--" && subcommands_.find(token.substr(2)) != subcommands_.end()) {
            currentSubcommand_ = token.substr(2);
            i++;
            continue;
        }
        
        // Long option (--option)
        if (token.substr(0, 2) == "--") {
            std::string optionName = token.substr(2);
            auto optionIt = options_.find(optionName);
            if (optionIt != options_.end()) {
                const CLIOption& option = optionIt->second;
                if (option.isFlag) {
                    parsedFlags_[optionName] = true;
                } else {
                    // Get next token as value
                    if (i + 1 < tokens.size() && tokens[i + 1].substr(0, 2) != "--") {
                        parsedValues_[optionName] = tokens[++i];
                    } else if (!option.defaultValue.empty()) {
                        parsedValues_[optionName] = option.defaultValue;
                    }
                }
            }
        }
        // Short option (-o)
        else if (token.size() == 2 && token[0] == '-' && token[1] != '-') {
            char shortOpt = token[1];
            bool found = false;
            for (auto& option : options_) {
                if (option.second.shortForm == shortOpt) {
                    if (option.second.isFlag) {
                        parsedFlags_[option.first] = true;
                    } else {
                        if (i + 1 < tokens.size() && tokens[i + 1].substr(0, 2) != "--") {
                            parsedValues_[option.first] = tokens[++i];
                        } else if (!option.second.defaultValue.empty()) {
                            parsedValues_[option.first] = option.second.defaultValue;
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                pImpl->lastError = "Unknown option: " + token;
            }
        }
        // Subcommand argument
        else if (!currentSubcommand_.empty()) {
            // Find the current subcommand
            if (subcommandArgs_.find(currentSubcommand_) == subcommandArgs_.end()) {
                subcommandArgs_[currentSubcommand_] = std::map<std::string, std::string>();
            }
            subcommandArgs_[currentSubcommand_][currentSubcommand_] = token;
        }
    }
    
    // Validate required options
    for (const auto& option : options_) {
        if (option.second.required) {
            bool found = false;
            if (option.second.isFlag) {
                if (parsedFlags_.find(option.first) != parsedFlags_.end()) {
                    found = true;
                }
            } else {
                if (parsedValues_.find(option.first) != parsedValues_.end()) {
                    found = true;
                }
            }
            if (!found) {
                pImpl->validationErrors += "Required option --" + option.first + " is missing.\n";
            }
        }
    }
    
    return validate();
}

std::string CommandLineParser::getValue(const std::string& key, const std::string& defaultValue) const {
    auto it = parsedValues_.find(key);
    if (it != parsedValues_.end()) {
        return it->second;
    }
    return defaultValue;
}

bool CommandLineParser::getFlag(const std::string& key) const {
    auto it = parsedFlags_.find(key);
    if (it != parsedFlags_.end()) {
        return it->second;
    }
    return false;
}

bool CommandLineParser::hasSubcommand(const std::string& name) const {
    return currentSubcommand_ == name;
}

const std::map<std::string, std::string>& CommandLineParser::getArgs(const std::string& subcommand) const {
    auto it = subcommandArgs_.find(subcommand);
    if (it != subcommandArgs_.end()) {
        return it->second;
    }
    static const std::map<std::string, std::string> emptyMap;
    return emptyMap;
}

void CommandLineParser::printHelp(std::ostream& output) const {
    output << "NLM - Neural Learning Machine" << std::endl;
    output << "=" << std::string(50, '=') << std::endl << std::endl;
    
    output << "Usage:" << std::endl;
    output << "  nlm [OPTIONS] [--SUBCOMMAND [ARGS]]" << std::endl << std::endl;
    
    if (!subcommands_.empty()) {
        output << "Subcommands:" << std::endl;
        for (const auto& subcmd : subcommands_) {
            output << "  --" << subcmd.first << "  " << subcmd.second.first << std::endl;
        }
        output << std::endl;
    }
    
    if (!options_.empty()) {
        output << "Global Options:" << std::endl;
        for (const auto& option : options_) {
            std::string optStr = "  --" + option.second.name;
            if (option.second.shortForm) {
                optStr += ", -" + std::string(1, option.second.shortForm);
            }
            optStr += std::string(50 - optStr.length(), ' ');
            output << optStr << option.second.description;
            if (!option.second.defaultValue.empty()) {
                output << " [default: " << option.second.defaultValue << "]";
            }
            if (option.second.required) {
                output << " [required]";
            }
            output << std::endl;
        }
        output << std::endl;
    }
    
    output << "Examples:" << std::endl;
    output << "  nlm --config=my_config.cfg --log-level=debug run" << std::endl;
    output << "  nlm benchmark --duration=60 --output=benchmark.json" << std::endl;
    output << "  nlm visualize --config=exp1.cfg --output=plot.png" << std::endl;
    output << std::endl;
    output << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

bool CommandLineParser::validate() const {
    // Basic validation - check for required values
    return pImpl->validationErrors.empty();
}

std::string CommandLineParser::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool CommandLineParser::hasBackwardCompatArgs(int argc, char** argv) const {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            return true;
        }
        if (arg.substr(0, 1) == "-") {
            return true;
        }
    }
    return false;
}

bool CommandLineParser::parseBackwardCompatArgs(int argc, char** argv, std::ostream& output) {
    // Parse backward compatible --config and --key=value syntax
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                std::string configFile = arg.substr(arg.find('=') + 1);
                parsedValues_["config"] = configFile;
            } else if (i + 1 < argc) {
                parsedValues_["config"] = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--") {
            if (arg.find('=') != std::string::npos) {
                std::string key = arg.substr(2, arg.find('=') - 2);
                std::string value = arg.substr(arg.find('=') + 1);
                parsedValues_[key] = value;
            }
        } else if (arg[0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            parsedValues_[key] = value;
        }
    }
    return validate();
}

bool CommandLineParser::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open configuration file: " + filepath;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            parsedValues_[key] = value;
        }
    }
    
    return validate();
}

bool CommandLineParser::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Command Line Parser" << std::endl;
    file << std::endl;
    
    for (const auto& option : options_) {
        file << "# " << option.second.description << std::endl;
        if (parsedFlags_.find(option.first) != parsedFlags_.end()) {
            file << option.first << " = true" << std::endl;
        } else if (parsedValues_.find(option.first) != parsedValues_.end()) {
            file << option.first << " = " << parsedValues_.at(option.first) << std::endl;
        } else if (!option.second.defaultValue.empty()) {
            file << option.first << " = " << option.second.defaultValue << std::endl;
        }
        file << std::endl;
    }
    
    return true;
}

} // namespace nlm
