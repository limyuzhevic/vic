#include "CommandLineParser.hpp"

namespace nlm {

CommandLineParser::CommandLineParser() {
}

void CommandLineParser::addOption(const std::string& shortOpt, const std::string& longOpt,
                                  const std::string& description, const std::string& defaultValue) {
    Option option;
    option.shortOpt = shortOpt;
    option.longOpt = longOpt;
    option.description = description;
    option.defaultValue = defaultValue;
    option.hasValue = true;
    option.exists = false;
    option.value = defaultValue;
    options_.push_back(option);
}

void CommandLineParser::addOption(const std::string& longOpt, const std::string& description,
                                  const std::string& defaultValue) {
    Option option;
    option.shortOpt = "";
    option.longOpt = longOpt;
    option.description = description;
    option.defaultValue = defaultValue;
    option.hasValue = true;
    option.exists = false;
    option.value = defaultValue;
    options_.push_back(option);
}

void CommandLineParser::addSubcommand(const std::string& name, const std::string& description) {
    Subcommand subcommand;
    subcommand.name = name;
    subcommand.description = description;
    subcommand.isCurrent = false;
    subcommands_.push_back(subcommand);
}

bool CommandLineParser::parse(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    currentSubcommand_ = "";
    
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        
        // Check for help
        if (arg == "-h" || arg == "--help") {
            for (auto& option : options_) {
                if (option.exists) {
                    option.value.clear();
                    option.exists = false;
                }
            }
            currentSubcommand_ = "help";
            return true;
        }
        
        // Check for subcommand (first non-option argument)
        if (currentSubcommand_.empty() && arg[0] != '-') {
            // Check if it's a valid subcommand
            bool isSubcommand = false;
            for (auto& subcommand : subcommands_) {
                if (subcommand.name == arg) {
                    subcommand.isCurrent = true;
                    currentSubcommand_ = arg;
                    isSubcommand = true;
                    break;
                }
            }
            if (!isSubcommand) {
                std::cerr << "Error: Unknown subcommand: " << arg << std::endl;
                return false;
            }
            continue;
        }
        
        // Check for options
        bool optionFound = false;
        for (auto& option : options_) {
            std::string optLong = "--" + option.longOpt;
            std::string optShort = "-" + option.shortOpt;
            
            if (arg == optLong || arg == optShort) {
                option.exists = true;
                optionFound = true;
                
                // Get value if option has one
                if (option.hasValue && i + 1 < args.size()) {
                    std::string nextArg = args[i + 1];
                    if (nextArg[0] != '-') {
                        option.value = nextArg;
                        i++;  // Skip next argument
                    }
                }
                break;
            }
        }
        
        if (!optionFound && arg[0] == '-') {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

bool CommandLineParser::hasOption(const std::string& opt) const {
    for (const auto& option : options_) {
        if (option.longOpt == opt && option.exists) {
            return true;
        }
        if (!option.shortOpt.empty() && option.shortOpt == opt && option.exists) {
            return true;
        }
    }
    return false;
}

std::string CommandLineParser::getOptionAsString(const std::string& opt) const {
    for (const auto& option : options_) {
        if ((option.longOpt == opt || (!option.shortOpt.empty() && option.shortOpt == opt)) && option.exists) {
            return option.value;
        }
    }
    return "";
}

size_t CommandLineParser::getOptionAs<size_t>(const std::string& opt) const {
    std::string value = getOptionAsString(opt);
    try {
        return std::stoul(value);
    } catch (...) {
        return 0;
    }
}

int CommandLineParser::getOptionAs<int>(const std::string& opt) const {
    std::string value = getOptionAsString(opt);
    try {
        return std::stoi(value);
    } catch (...) {
        return 0;
    }
}

float CommandLineParser::getOptionAs<float>(const std::string& opt) const {
    std::string value = getOptionAsString(opt);
    try {
        return std::stof(value);
    } catch (...) {
        return 0.0f;
    }
}

double CommandLineParser::getOptionAs<double>(const std::string& opt) const {
    std::string value = getOptionAsString(opt);
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

bool CommandLineParser::getOptionAs<bool>(const std::string& opt) const {
    return hasOption(opt);
}

std::string CommandLineParser::getHelpText() const {
    std::string help;
    
    help += "OPTIONS:\n";
    for (const auto& option : options_) {
        if (!option.shortOpt.empty()) {
            help += "  -" + option.shortOpt + ", ";
        }
        help += "--" + option.longOpt;
        
        if (option.hasValue) {
            help += " <value>";
        }
        
        help += " - " + option.description;
        if (!option.defaultValue.empty()) {
            help += " (default: " + option.defaultValue + ")";
        }
        help += "\n";
    }
    
    help += "\nSUBCOMMANDS:\n";
    for (const auto& subcommand : subcommands_) {
        help += "  " + subcommand.name + " - " + subcommand.description + "\n";
    }
    
    return help;
}

} // namespace nlm