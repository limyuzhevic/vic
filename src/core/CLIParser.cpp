#include "CLIParser.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

CLIParser::CLIParser(const std::string& progName) : programName(progName) {}

void CLIParser::addArgument(const std::string& longName, char shortName,
                           const std::string& description, const std::string& defaultValue,
                           bool required) {
    Argument arg;
    arg.longName = longName;
    arg.shortName = shortName;
    arg.description = description;
    arg.defaultValue = defaultValue;
    arg.required = required;
    arg.action = nullptr;
    
    arguments.push_back(arg);
}

void CLIParser::addArgumentAction(const std::string& longName,
                                 std::function<void(const std::string&, Config&)> action) {
    for (auto& arg : arguments) {
        if (arg.longName == longName) {
            arg.action = action;
            break;
        }
    }
}

bool CLIParser::parse(int argc, char** argv, Config& config) {
    NLM_LOG_INFO("Parsing command line arguments...");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            handleHelp();
            return false;
        }
        
        parseStandardArgs(arg, config);
    }
    
    return true;
}

void CLIParser::parseStandardArgs(const std::string& arg, Config& config) {
    if (arg.substr(0, 2) == "--") {
        // Long argument format: --key=value or --config file
        std::string keyValue = arg.substr(2);
        size_t pos = keyValue.find('=');
        
        if (pos != std::string::npos) {
            std::string key = keyValue.substr(0, pos);
            std::string value = keyValue.substr(pos + 1);
            handleConfigArg(key, value, config);
        } else {
            handleFileArg(keyValue, config);
        }
    } else if (arg[0] == '-' && arg.size() > 1) {
        // Short argument format: -k value or -n 100
        std::string key = arg.substr(1);
        
        if (i + 1 < argc && argv[i + 1][0] != '-') {
            // Next argument is the value
            std::string value = argv[++i];
            handleConfigArg(key, value, config);
        } else {
            // No value provided
            NLM_LOG_WARNING("Missing value for argument: " + key);
        }
    }
}

void CLIParser::handleFileArg(const std::string& filepath, Config& config) {
    if (filepath == "config" || filepath == "c") {
        if (i + 1 < argc) {
            std::string configFile = argv[++i];
            if (config.loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else {
                NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
            }
        } else {
            NLM_LOG_WARNING("Missing configuration file path after --config");
        }
    } else {
        NLM_LOG_WARNING("Unknown file argument: " + filepath);
    }
}

void CLIParser::handleConfigArg(const std::string& key, const std::string& value, Config& config) {
    // Convert key to lowercase for case-insensitive lookup
    std::string lowerKey = key;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
    
    // Handle special arguments
    if (lowerKey == "neuron" || lowerKey == "n") {
        try {
            int neuronCount = std::stoi(value);
            config.set("neuron_count", neuronCount, ConfigSource::CommandLine);
            NLM_LOG_INFO("Set neuron count: " + std::to_string(neuronCount));
        } catch (...) {
            NLM_LOG_ERROR("Invalid neuron count value: " + value);
        }
    } else if (lowerKey == "step" || lowerKey == "s") {
        try {
            int steps = std::stoi(value);
            config.set("max_simulation_steps", steps, ConfigSource::CommandLine);
            NLM_LOG_INFO("Set max simulation steps: " + std::to_string(steps));
        } catch (...) {
            NLM_LOG_ERROR("Invalid step count value: " + value);
        }
    } else if (lowerKey == "stdp" || lowerKey == "stdp_ltp") {
        try {
            double stdpValue = std::stod(value);
            config.set("stdp_ltp_weight", stdpValue, ConfigSource::CommandLine);
            NLM_LOG_INFO("Set STDP LTP weight: " + std::to_string(stdpValue));
        } catch (...) {
            NLM_LOG_ERROR("Invalid STDP value: " + value);
        }
    } else {
        // Set generic configuration value
        config.set(key, value, ConfigSource::CommandLine);
        NLM_LOG_INFO("Set configuration: " + key + " = " + value);
    }
}

void CLIParser::handleHelp() const {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                    NLM — 熙然 CLI Argument Parser             ║
╠════════════════════════════════════════════════════════════════╣
║                                                               ║
║ Command Line Arguments:                                        ║
║                                                               ║
║  --help, -h                    Display this help message       ║
║  --config <file> or -c <file>   Load configuration from file  ║
║  --neuron <n> or -n <n>         Set neuron count (default: 500)║
║  --step <steps> or -s <steps>    Set max simulation steps      ║
║  --stdp <value> or -stdp <val>   Set STDP LTP weight          ║
║  --key=value                    Set arbitrary configuration     ║
║                                                               ║
║ Examples:                                                      ║
║                                                               ║
║  nlm --neuron 1000 --step 2000                               ║
║  nlm --c config.json --stdp 0.05                            ║
║  nlm --brain.neuron_count 2000 --plasticity.learning_rate 0.1 ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

)" << std::endl;
}

void CLIParser::showHelp() const {
    handleHelp();
}

} // namespace nlm