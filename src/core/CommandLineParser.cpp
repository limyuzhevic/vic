#include "CommandLineParser.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>

namespace nlm {

CommandLineParser::CommandLineParser(int argc, char* argv[]) : argc(argc), argv(argv) {
}

bool CommandLineParser::parse() {
    // Define built-in options if not already done
    static bool optionsInitialized = false;
    if (!optionsInitialized) {
        defineBuiltInOptions();
        optionsInitialized = true;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;  // Exit after help
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;  // Exit after version
        } else if (arg.substr(0, 2) == "--") {
            parseLongOption(arg.substr(2));
        } else if (arg.substr(0, 1) == "-") {
            parseShortOption(arg.substr(1));
        } else {
            positionalArgs.push_back(arg);
        }
    }
    
    return validateOptions();
}

bool CommandLineParser::parseShortOption(const std::string& arg) {
    size_t pos = 0;
    while (pos < arg.length()) {
        char flag = arg[pos++];
        std::string flagStr(1, flag);
        
        // Find option definition
        bool found = false;
        for (const auto& option : options) {
            if (option.flags.find(flagStr) != std::string::npos) {
                found = true;
                
                if (option.hasValue) {
                    // Check if next arg is not a flag
                    if (pos < arg.length()) {
                        // Multiple short options with values (e.g., -f value -d)
                        optionValues[option.flags] = arg.substr(pos);
                        break;
                    } else if (i + 1 < argc) {
                        // Next argument is the value
                        i++;
                        optionValues[option.flags] = argv[i];
                    } else {
                        errorMessage = "Option " + option.flags + " requires a value";
                        return false;
                    }
                } else {
                    optionValues[option.flags] = "true";
                }
                break;
            }
        }
        
        if (!found) {
            errorMessage = "Unknown option: -" + flagStr;
            return false;
        }
    }
    return true;
}

bool CommandLineParser::parseLongOption(const std::string& arg) {
    size_t equalsPos = arg.find('=');
    std::string flag;
    std::string value;
    
    if (equalsPos != std::string::npos) {
        flag = arg.substr(0, equalsPos);
        value = arg.substr(equalsPos + 1);
    } else {
        flag = arg;
        value = "true";
    }
    
    // Find option definition
    bool found = false;
    for (const auto& option : options) {
        if (option.flags == flag) {
            found = true;
            
            if (option.hasValue && value == "true") {
                // Check if we need to look at next argument
                // This would be handled by the caller
                optionValues[option.flags] = value;
            } else {
                optionValues[option.flags] = value;
            }
            break;
        }
    }
    
    if (!found) {
        errorMessage = "Unknown option: --" + flag;
        return false;
    }
    
    return true;
}

bool CommandLineParser::validateOptions() const {
    // Check required options
    for (const auto& option : options) {
        if (option.required && optionValues.find(option.flags) == optionValues.end()) {
            errorMessage = "Required option --" + option.flags + " is missing";
            return false;
        }
    }
    return true;
}

bool CommandLineParser::hasOption(const std::string& flag) const {
    return optionValues.find(flag) != optionValues.end();
}

std::string CommandLineParser::getOption(const std::string& flag) const {
    auto it = optionValues.find(flag);
    if (it != optionValues.end()) {
        return it->second;
    }
    return "";
}

int CommandLineParser::getOptionAsInt(const std::string& flag) const {
    auto it = optionValues.find(flag);
    if (it != optionValues.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

float CommandLineParser::getOptionAsFloat(const std::string& flag) const {
    auto it = optionValues.find(flag);
    if (it != optionValues.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return 0.0f;
        }
    }
    return 0.0f;
}

bool CommandLineParser::getOptionAsBool(const std::string& flag) const {
    auto it = optionValues.find(flag);
    if (it != optionValues.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return value == "true" || value == "1" || value == "yes" || value == "on";
    }
    return false;
}

std::vector<std::string> CommandLineParser::getArgs() const {
    return positionalArgs;
}

void CommandLineParser::printHelp() const {
    generateHelp();
}

void CommandLineParser::printVersion() const {
    std::cout << "NLM (熙然) v0.1.0 - Neural Learning Machine" << std::endl;
    std::cout << "Phase 6: Final Integration of Artificial Developmental Brain" << std::endl;
    std::cout << "An experimental computational brain project." << std::endl;
}

void CommandLineParser::addOption(const std::string& flags, const std::string& description,
                                 const std::string& defaultValue, bool required) {
    Option option;
    option.flags = flags;
    option.description = description;
    option.defaultValue = defaultValue;
    option.required = required;
    option.hasValue = !defaultValue.empty();
    
    options.push_back(option);
}

void CommandLineParser::defineBuiltInOptions() {
    // Define standard command line options
    addOption("help,h", "Show this help message", "", false);
    addOption("version,v", "Show version information", "", false);
    addOption("config", "Load configuration file", "");
    addOption("neurons", "Number of neurons in the brain", "1000");
    addOption("regions", "Number of brain regions", "1");
    addOption("steps", "Number of simulation steps", "1000");
    addOption("seed", "Random seed for reproducible simulations", "42");
    addOption("verbose", "Enable verbose logging", "false");
    addOption("quiet", "Suppress output (except errors)", "false");
    addOption("profile", "Enable profiling and performance monitoring", "false");
    addOption("checkpoint", "Save checkpoint to specified path", "");
    addOption("load", "Load brain state from checkpoint", "");
    addOption("develop", "Enable developmental plasticity", "true");
    addOption("memory", "Enable memory systems", "true");
    addOption("neuromodulation", "Enable neuromodulation systems", "true");
    addOption("prediction", "Enable prediction system", "true");
    addOption("cognition", "Enable cognition systems", "true");
}

void CommandLineParser::generateHelp() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Phase 6: Final Integration of Artificial Developmental Brain" << std::endl;
    std::cout << "Usage: nlm [options] [experiment_type]" << std::endl;
    std::cout << "Experiment types: phase2, phase3, phase4, phase6, benchmark" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    
    for (const auto& option : options) {
        std::string flagText;
        if (option.flags.find(',') != std::string::npos) {
            // Multiple flags
            flagText = "  -";
            size_t pos = 0;
            while (pos < option.flags.length()) {
                size_t commaPos = option.flags.find(',', pos);
                if (commaPos != std::string::npos) {
                    flagText += option.flags.substr(pos, commaPos - pos);
                    pos = commaPos + 1;
                    if (pos < option.flags.length()) {
                        flagText += ", ";
                    }
                } else {
                    flagText += option.flags.substr(pos);
                    break;
                }
            }
        } else {
            if (option.flags.length() == 1) {
                flagText = "  -" + option.flags;
            } else {
                flagText = "  --" + option.flags;
            }
        }
        
        // Add short form if available
        size_t commaPos = option.flags.find(',');
        if (commaPos != std::string::npos) {
            std::string shortForm = option.flags.substr(0, commaPos);
            flagText += ", --" + option.flags.substr(commaPos + 1);
        } else {
            if (option.flags.length() > 1) {
                flagText += ", -" + option.flags.substr(0, 1);
            }
        }
        
        flagText += std::string(50 - flagText.length(), ' ');
        std::cout << flagText << option.description << std::endl;
        
        if (!option.defaultValue.empty()) {
            std::cout << std::string(60, ' ') << "[default: " << option.defaultValue << "]" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --neurons 500 --steps 2000" << std::endl;
    std::cout << "  nlm --config my_config.cfg --checkpoint saved_brain.bin" << std::endl;
    std::cout << "  nlm phase6 --develop --memory --neuromodulation" << std::endl;
}

} // namespace nlm
