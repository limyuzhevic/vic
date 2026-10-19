// CLI implementation
#include "CLI/CLI.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace nlm {

CLIParser::CLIParser(int argc, char** argv) : argc_(argc), argv_(argv) {
    // Add default options
    addOption("--help", "Show help information", OptionType::FLAG, "", "-h", false, "General");
    addOption("--version", "Show version information", OptionType::FLAG, "", "-v", false, "General");
    addOption("--config", "Configuration file path", OptionType::STRING, "configs/default.cfg", "", false, "Configuration");
    addOption("--log-level", "Log level (Debug, Info, Warning, Error, Critical)", OptionType::STRING, "Info", "", false, "Logging");
    addOption("--log-file", "Log output file path", OptionType::STRING, "", "", false, "Logging");
    addOption("--no-colors", "Disable colored output", OptionType::FLAG, "", "", false, "Output");
    addOption("--batch", "Run in batch mode (non-interactive)", OptionType::FLAG, "", "", false, "Execution");
    addOption("--interactive", "Run in interactive mode", OptionType::FLAG, "", "", false, "Execution");
    addOption("--verbose", "Enable verbose output", OptionType::FLAG, "", "", false, "Output");
    addOption("--quiet", "Suppress all output except errors", OptionType::FLAG, "", "", false, "Output");
    addOption("--experiments", "Comma-separated list of experiments to run", OptionType::STRING, "all", "", false, "Execution");
    addOption("--steps", "Number of simulation steps to run", OptionType::INT, "1000", "", false, "Execution");
    addOption("--random-seed", "Random seed for reproducible results", OptionType::INT, "42", "", false, "Randomness");
}

void CLIParser::addOption(const std::string& name, const std::string& description,
                         OptionType type, const std::string& defaultValue,
                         const std::string& shortOption, bool required,
                         const std::string& category) {
    CLIOption option;
    option.name = name;
    option.description = description;
    option.type = type;
    option.defaultValue = defaultValue;
    option.shortOption = shortOption;
    option.required = required;
    option.category = category;
    options_[name] = option;
}

bool CLIParser::parse() {
    parseArgs();
    
    // Check for help flag
    if (hasOption("--help") || hasOption("-h")) {
        showHelp();
        return false;
    }
    
    // Check for version flag
    if (hasOption("--version") || hasOption("-v")) {
        std::cout << "NLM Phase 2 - Version 2.0.0" << std::endl;
        std::cout << "Neural Learning Machine - Real Neural Computation" << std::endl;
        return false;
    }
    
    return true;
}

std::string CLIParser::getString(const std::string& name) const {
    auto it = parsedValues_.find(name);
    if (it != parsedValues_.end()) {
        return it->second;
    }
    auto optIt = options_.find(name);
    if (optIt != options_.end()) {
        return optIt->second.defaultValue;
    }
    return "";
}

int CLIParser::getInt(const std::string& name) const {
    std::string value = getString(name);
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return 0;
    }
}

float CLIParser::getFloat(const std::string& name) const {
    std::string value = getString(name);
    try {
        return std::stof(value);
    } catch (const std::exception&) {
        return 0.0f;
    }
}

double CLIParser::getDouble(const std::string& name) const {
    std::string value = getString(name);
    try {
        return std::stod(value);
    } catch (const std::exception&) {
        return 0.0;
    }
}

bool CLIParser::getBool(const std::string& name) const {
    if (flagValues_.find(name) != flagValues_.end()) {
        return flagValues_.at(name);
    }
    std::string value = getString(name);
    return (value == "true" || value == "1" || value == "yes" || value == "on");
}

std::vector<std::string> CLIParser::getStringList(const std::string& name) const {
    std::string value = getString(name);
    std::vector<std::string> result;
    size_t pos = 0;
    size_t comma = value.find(',');
    while (comma != std::string::npos) {
        result.push_back(value.substr(pos, comma - pos));
        pos = comma + 1;
        comma = value.find(',', pos);
    }
    if (pos < value.length()) {
        result.push_back(value.substr(pos));
    }
    return result;
}

bool CLIParser::hasOption(const std::string& name) const {
    auto it = parsedValues_.find(name);
    if (it != parsedValues_.end()) {
        return true;
    }
    auto optIt = options_.find(name);
    if (optIt != options_.end()) {
        return optIt->second.defaultValue != "" || optIt->second.required;
    }
    return false;
}

void CLIParser::parseArgs() {
    std::vector<std::string> args;
    for (int i = 1; i < argc_; ++i) {
        args.push_back(argv_[i]);
    }
    
    bool extraArgsMode = false;
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        // Check for -- separator
        if (arg == "--") {
            extraArgsMode = true;
            continue;
        }
        
        if (extraArgsMode) {
            extraArgs_.push_back(arg);
            continue;
        }
        
        // Check for positional arguments (arguments that are not options)
        if (arg.substr(0, 2) == "--" || (arg.size() > 0 && arg[0] == '-')) {
            if (parseOption(arg)) {
                // Option was parsed successfully
                continue;
            } else {
                unknownArgs_.push_back(arg);
            }
        } else {
            // Positional argument
            positionalArgs_.push_back(arg);
        }
    }
}

bool CLIParser::parseOption(const std::string& arg) {
    std::string option = arg;
    std::string value = "";
    
    // Handle --option=value format
    size_t eqPos = option.find('=');
    if (eqPos != std::string::npos) {
        value = option.substr(eqPos + 1);
        option = option.substr(0, eqPos);
    } else if (option != "--help" && option != "-h" && option != "--version" && option != "-v") {
        // Check if this is a flag option or has a separate value
        auto optIt = options_.find(option);
        if (optIt != options_.end() && optIt->second.type != OptionType::FLAG) {
            // Need to check next argument
            if (optIt->second.type == OptionType::FLAG) {
                // Flag option - just set to true
                flagValues_[option] = true;
                return true;
            }
        }
    }
    
    // Look for option in registered options
    for (const auto& pair : options_) {
        const CLIOption& opt = pair.second;
        if ((option == opt.name) || (opt.shortOption != "" && option == "-" + opt.shortOption)) {
            if (opt.type == OptionType::FLAG) {
                flagValues_[option] = true;
                return true;
            } else {
                parsedValues_[option] = value.empty() ? opt.defaultValue : value;
                return true;
            }
        }
    }
    
    return false;
}

void CLIParser::showHelp() const {
    std::cout << "=== NLM Phase 2 - Neural Learning Machine ===" << std::endl;
    std::cout << "Real Spiking Neural Computation with Learning and Development" << std::endl;
    std::cout << std::endl;
    
    // Group options by category
    std::map<std::string, std::vector<const CLIOption*>> optionsByCategory;
    for (const auto& pair : options_) {
        const CLIOption& opt = pair.second;
        optionsByCategory[opt.category].push_back(&opt);
    }
    
    for (const auto& categoryPair : optionsByCategory) {
        std::cout << categoryPair.first << " Options:" << std::endl;
        for (const CLIOption* opt : categoryPair.second) {
            std::cout << "  ";
            if (opt->shortOption != "") {
                std::cout << "-" << opt->shortOption << ", ";
            }
            std::cout << "--" << opt->name;
            
            if (opt->type != OptionType::FLAG) {
                std::cout << "=<value>";
            }
            
            std::cout << std::endl;
            std::cout << "      " << opt->description;
            if (!opt->defaultValue.empty() && opt->defaultValue != "true" && opt->defaultValue != "false") {
                std::cout << " (default: " << opt->defaultValue << ")";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
    
    std::cout << "=== Examples ===" << std::endl;
    std::cout << "  nlm --help" << std::endl;
    std::cout << "  nlm --config my_config.cfg --steps 5000" << std::endl;
    std::cout << "  nlm --interactive" << std::endl;
    std::cout << "  nlm --batch --experiments plasticity,stdp,development" << std::endl;
    std::cout << std::endl;
    std::cout << "=== Files ===" << std::endl;
    std::cout << "  configs/default.cfg - Default configuration file" << std::endl;
    std::cout << "  nlm.log - Log file (if enabled)" << std::endl;
    std::cout << "=== End of Help ===" << std::endl;
}

std::string CLIParser::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool CLIParser::startsWith(const std::string& str, const std::string& prefix) const {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string CLIParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace nlm