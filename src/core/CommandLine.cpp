#include "CommandLine.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {

CommandLine::CommandLine(int argc, char** argv) : argc(argc), argv(argv) {
    parseArguments();
}

void CommandLine::parseArguments() {
    // Add standard options first
    addStandardOptions();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check if it's a help request
        if (arg == "--help" || arg == "-h") {
            addBooleanOption("--help", "Print help information");
            continue;
        }
        
        // Check if it's a version request
        if (arg == "--version") {
            addBooleanOption("--version", "Print version information");
            continue;
        }
        
        // Check if it's a config file option
        if (arg == "--config") {
            if (i + 1 < argc) {
                addOption("--config", "Configuration file path", true, argv[++i]);
            }
            continue;
        }
        
        // Check for --option=value format
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos && arg[0] == '-') {
            std::string option = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            
            // Check if option exists
            bool optionExists = false;
            for (const auto& opt : options) {
                if (opt.name == option && opt.hasValue) {
                    optionExists = true;
                    break;
                }
            }
            
            if (optionExists) {
                addOption(option, "", true, value);
            } else {
                std::cerr << "Warning: Unknown option " << option << std::endl;
            }
        } else if (arg[0] == '-') {
            // Check for --option value format (option without value)
            std::string option = arg;
            
            // Check if option exists and has value
            bool optionExists = false;
            for (const auto& opt : options) {
                if (opt.name == option) {
                    optionExists = true;
                    if (opt.hasValue && i + 1 < argc && argv[i + 1][0] != '-') {
                        // Next argument is the value
                        addOption(option, opt.description, true, argv[++i]);
                    } else {
                        // Boolean option
                        addBooleanOption(option, opt.description);
                    }
                    break;
                }
            }
            
            if (!optionExists) {
                std::cerr << "Warning: Unknown option " << option << std::endl;
            }
        }
    }
    
    validateArguments();
}

void CommandLine::addOption(const std::string& name, const std::string& description,
                           bool hasValue, const std::string& defaultValue) {
    // Check if option already exists
    for (auto& opt : options) {
        if (opt.name == name) {
            opt.description = description;
            opt.hasValue = hasValue;
            opt.defaultValue = defaultValue;
            return;
        }
    }
    
    options.push_back(OptionInfo(name, description, hasValue, defaultValue));
}

void CommandLine::addBooleanOption(const std::string& name, const std::string& description) {
    addOption(name, description, false, "");
}

bool CommandLine::hasOption(const std::string& name) const {
    for (const auto& opt : options) {
        if (opt.name == name && (opt.providedValue.has_value() || !opt.hasValue)) {
            return true;
        }
    }
    return false;
}

std::optional<std::string> CommandLine::getRawValue(const std::string& name) const {
    for (const auto& opt : options) {
        if (opt.name == name) {
            if (opt.providedValue.has_value()) {
                return opt.providedValue;
            } else if (!opt.hasValue) {
                // Boolean option
                return "true";
            }
        }
    }
    return std::nullopt;
}

void CommandLine::addStandardOptions(bool hasConfig, bool hasHelp, bool hasVersion) {
    if (hasConfig) {
        addOption("--config", "Configuration file path", true, "configs/default.cfg");
    }
    
    if (hasHelp) {
        addBooleanOption("--help", "Print help information");
        addBooleanOption("-h", "Print help information (short form)");
    }
    
    if (hasVersion) {
        addBooleanOption("--version", "Print version information");
    }
}

void CommandLine::validateArguments() const {
    // Check for conflicting options
    if (hasOption("--help") && hasOption("--version")) {
        std::cerr << "Warning: Both --help and --version options provided. Help will be shown." << std::endl;
    }
    
    // Additional validation can be added here
}

std::string CommandLine::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string CommandLine::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool CommandLine::stringToBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    
    throw std::runtime_error("Cannot convert string to boolean: " + str);
}

bool CommandLine::isBoolString(const std::string& str) {
    std::string lower = toLower(trim(str));
    return lower == "true" || lower == "false" || lower == "1" || lower == "0" ||
           lower == "yes" || lower == "no" || lower == "on" || lower == "off";
}

bool CommandLine::isIntegerString(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    size_t start = 0;
    if (str[0] == '-') {
        start = 1;
    }
    
    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool CommandLine::isDoubleString(const std::string& str) {
    try {
        double value = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::string CommandLine::getCommandLineString() const {
    std::stringstream ss;
    for (int i = 0; i < argc; ++i) {
        if (i > 0) {
            ss << " ";
        }
        ss << argv[i];
    }
    return ss.str();
}

std::vector<std::string> CommandLine::getOptionNames() const {
    std::vector<std::string> names;
    for (const auto& opt : options) {
        if (opt.providedValue.has_value() || !opt.hasValue) {
            names.push_back(opt.name);
        }
    }
    return names;
}

bool CommandLine::helpRequested() const {
    return hasOption("--help");
}

std::string CommandLine::getVersion() {
    return "NLM 0.1.0";
}

std::vector<std::string> CommandLine::getUsageExamples() {
    return {
        "nlm --help",
        "nlm --config configs/myconfig.cfg",
        "nlm --neuron-count 2000 --region-count 3",
        "nlm -h  # Short help"
    };
}

void CommandLine::printHelp(const std::string& programName) const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version " << getVersion() << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Standard options:" << std::endl;
    for (const auto& opt : options) {
        if (opt.name == "--help" || opt.name == "-h") {
            std::cout << "  " << opt.name << "\t" << opt.description << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    std::cout << "Configuration options:" << std::endl;
    for (const auto& opt : options) {
        if (opt.name == "--config") {
            std::cout << "  " << opt.name << " <file>\t" << opt.description << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    std::cout << "Advanced options:" << std::endl;
    for (const auto& opt : options) {
        if (opt.name != "--help" && opt.name != "-h" && opt.name != "--config") {
            std::cout << "  " << opt.name;
            if (opt.hasValue) {
                std::cout << " <value>";
            }
            std::cout << "\t" << opt.description;
            if (!opt.defaultValue.empty()) {
                std::cout << " [default: " << opt.defaultValue << "]";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    for (const auto& example : getUsageExamples()) {
        std::cout << "  " << example << std::endl;
    }
}

void CommandLine::printUsage(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Use " << programName << " --help for more information." << std::endl;
}

std::string CommandLine::getConfigFile(const std::string& defaultConfig) const {
    return getOr("--config", defaultConfig);
}

bool loadConfigFromFile(const std::string& configFile) {
    // This would typically use the Config class to load configuration
    // For now, we'll just check if the file exists
    std::ifstream file(configFile);
    return file.good();
}

bool saveConfigToFile(const std::string& configFile) {
    // This would typically use the Config class to save configuration
    // For now, we'll just return true if we can create the file
    std::ofstream file(configFile);
    return file.good();
}

BrainConfig parseBrainConfig(const CommandLine& cli) {
    BrainConfig config;
    
    if (cli.hasOption("--neuron-count")) {
        config.neuronCount = cli.getOr("--neuron-count", config.neuronCount);
    }
    
    if (cli.hasOption("--region-count")) {
        config.regionCount = cli.getOr("--region-count", config.regionCount);
    }
    
    if (cli.hasOption("--connection-probability")) {
        config.connectionProbability = cli.getOr("--connection-probability", config.connectionProbability);
    }
    
    if (cli.hasOption("--random-seed")) {
        config.randomSeed = cli.getOr("--random-seed", config.randomSeed);
    }
    
    if (cli.hasOption("--simulation-timestep")) {
        config.simulationTimestep = cli.getOr("--simulation-timestep", config.simulationTimestep);
    }
    
    if (cli.hasOption("--stdp-ltp-weight")) {
        config.stdpLtpWeight = cli.getOr("--stdp-ltp-weight", config.stdpLtpWeight);
    }
    
    if (cli.hasOption("--stdp-ltd-weight")) {
        config.stdpLtdWeight = cli.getOr("--stdp-ltd-weight", config.stdpLtdWeight);
    }
    
    if (cli.hasOption("--stdp-tau")) {
        config.stdpTau = cli.getOr("--stdp-tau", config.stdpTau);
    }
    
    if (cli.hasOption("--synaptogenesis-rate")) {
        config.synaptogenesisRate = cli.getOr("--synaptogenesis-rate", config.synaptogenesisRate);
    }
    
    if (cli.hasOption("--pruning-rate")) {
        config.pruningRate = cli.getOr("--pruning-rate", config.pruningRate);
    }
    
    return config;
}

void printBrainConfig(const BrainConfig& config) {
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  Neuron count: " << config.neuronCount << std::endl;
    std::cout << "  Region count: " << config.regionCount << std::endl;
    std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "  Random seed: " << config.randomSeed << std::endl;
    std::cout << "  Simulation timestep: " << config.simulationTimestep << "s" << std::endl;
    std::cout << "  STDP LTP weight: " << config.stdpLtpWeight << std::endl;
    std::cout << "  STDP LTD weight: " << config.stdpLtdWeight << std::endl;
    std::cout << "  STDP tau: " << config.stdpTau << std::endl;
    std::cout << "  Synaptogenesis rate: " << config.synaptogenesisRate << std::endl;
    std::cout << "  Pruning rate: " << config.pruningRate << std::endl;
}

} // namespace nlm