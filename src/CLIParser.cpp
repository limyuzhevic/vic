// CLI Parser implementation for NLM (Neural Learning Machine)

#include "CLIParser.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace nlm {

CLIParser::CLIParser() {
    // Initialize option database
    initializeOptions();
}

void CLIParser::initializeOptions() {
    // Basic options
    addOption("help", 'h', "Show comprehensive help with all options", "", "Basic", "flag", "nlm --help", false);
    addOption("version", 'v', "Show version information", "", "Basic", "flag", "nlm --version", false);
    addOption("config", 'c', "Specify configuration file", "configs/default.cfg", "Basic", "file", "nlm --config=my.cfg", false);
    addOption("verbose", 'V', "Enable verbose logging (use multiple times for more verbose)", "", "Basic", "count", "nlm -VV", false);
    addOption("quiet", 'q', "Suppress non-essential output", "", "Basic", "flag", "nlm -q", false);
    addOption("no-colors", 'C', "Disable colored output", "", "Basic", "flag", "nlm --no-colors", false);
    addOption("output", 'o', "Set output file for results", "", "Output", "file", "nlm --output=results.txt", false);
    addOption("output-format", 'O', "Output format (text, json, yaml)", "text", "Output", "choice:text,json,yaml", "nlm --output-format=json", false);
    addOption("metrics", '', "Include detailed metrics", "", "Output", "flag", "nlm --metrics", false);
    addOption("stat-file", '', "Save statistics to file", "", "Output", "file", "nlm --stat-file=stats.csv", false);
    
    // Advanced configuration options
    addOption("set", '', "Set configuration value on command line (key=value)", "", "Advanced Config", "kv", "nlm --set=random_seed=42", false);
    addOption("unset", '', "Remove configuration value", "", "Advanced Config", "key", "nlm --unset=random_seed", false);
    addOption("validate", '', "Validate configuration", "", "Advanced Config", "flag", "nlm --validate", false);
    addOption("show-config", '', "Show current configuration", "", "Advanced Config", "flag", "nlm --show-config", false);
    
    // Execution control
    addOption("run-test", '', "Run specific test (1, 2, 3 for Phase 2 tests)", "", "Execution", "num", "nlm --run-test=2", false);
    addOption("steps", '', "Number of simulation steps", "1000", "Execution", "num", "nlm --steps=500", false);
    addOption("timestep", 't', "Simulation timestep", "0.001", "Execution", "float", "nlm --timestep=0.01", false);
    addOption("neurons", '', "Number of neurons", "500", "Execution", "num", "nlm --neurons=100", false);
    addOption("regions", '', "Number of regions", "1", "Execution", "num", "nlm --regions=2", false);
    addOption("connections", 'p', "Connection probability (0.0-1.0)", "0.15", "Execution", "float", "nlm --connections=0.5", false);
    
    // Memory and performance
    addOption("memory-limit", '', "Memory limit in MB", "", "Performance", "num", "nlm --memory-limit=1024", false);
    addOption("threads", '', "Number of threads", "", "Performance", "num", "nlm --threads=4", false);
    addOption("checkpoint-interval", '', "Checkpoint interval", "", "Performance", "num", "nlm --checkpoint-interval=100", false);
    addOption("checkpoint-file", '', "Checkpoint file", "", "Performance", "file", "nlm --checkpoint-file=checkpoint.bin", false);
    
    // Debugging and development
    addOption("debug", 'd', "Enable debug logging", "", "Debug", "flag", "nlm --debug", false);
    addOption("log-level", '', "Set log level (Debug, Info, Warning, Error, Critical)", "Info", "Debug", "choice:Debug,Info,Warning,Error,Critical", "nlm --log-level=Debug", false);
    addOption("log-file", '', "Set log output file", "", "Debug", "file", "nlm --log-file=debug.log", false);
    addOption("trace", '', "Enable tracing", "", "Debug", "flag", "nlm --trace", false);
    addOption("profile", '', "Enable profiling", "", "Debug", "flag", "nlm --profile", false);
    addOption("validate-config", '', "Validate configuration file", "", "Debug", "flag", "nlm --validate-config", false);
    addOption("dry-run", '', "Parse configuration but don't execute", "", "Debug", "flag", "nlm --dry-run", false);
    
    // Brain configuration
    addOption("brain-type", '', "Type of brain (standard, minimal, enhanced)", "standard", "Brain", "choice:standard,minimal,enhanced", "nlm --brain-type=enhanced", false);
    addOption("plasticity-enable", '', "Enable/disable plasticity", "true", "Brain", "bool", "nlm --plasticity-enable=false", false);
    addOption("novelty-threshold", '', "Novelty detection threshold", "0.5", "Brain", "float", "nlm --novelty-threshold=0.1", false);
    addOption("curiosity-gain", '', "Curiosity gain parameter", "0.1", "Brain", "float", "nlm --curiosity-gain=0.5", false);
    
    // Action and behavior control
    addOption("random-seed", '', "Random seed for reproducibility", "42", "Behavior", "num", "nlm --random-seed=123", false);
    addOption("deterministic", '', "Use deterministic operations", "", "Behavior", "flag", "nlm --deterministic", false);
    addOption("agent-mode", '', "Run in autonomous agent mode", "", "Behavior", "flag", "nlm --agent-mode", false);
    addOption("interactive", '', "Interactive mode", "", "Behavior", "flag", "nlm --interactive", false);
    
    // Format options (alias for output-format)
    addOption("format", 'f', "Output format (alias for --output-format)", "text", "Output", "choice:text,json,yaml", "nlm --format=json", false);
}

void CLIParser::addOption(const std::string& longName, char shortName, const std::string& description,
                         const std::string& defaultValue, const std::string& category,
                         const std::string& type, const std::string& example,
                         bool required, const std::vector<std::string>& validValues,
                         std::function<bool(const std::string&)> validator) {
    Option option;
    option.longName = longName;
    option.shortName = shortName;
    option.description = description;
    option.defaultValue = defaultValue;
    option.category = category;
    option.type = type;
    option.example = example;
    option.required = required;
    option.validValues = validValues;
    option.validator = validator;
    options.push_back(option);
}

void CLIParser::printHelp(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
    std::cout << "NLM — 熙然" << std::endl;
    std::cout << "Neural Learning Machine" << std::endl << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl << std::endl;
    
    // Group options by category
    std::map<std::string, std::vector<Option>> categorizedOptions;
    for (const auto& option : options) {
        if (!option.longName.empty()) {
            categorizedOptions[option.category].push_back(option);
        }
    }
    
    // Print each category
    for (const auto& [category, opts] : categorizedOptions) {
        std::cout << category << " Options:" << std::endl;
        for (const auto& option : opts) {
            std::string flag;
            if (option.shortName != '\0') {
                flag = "-" + std::string(1, option.shortName);
                if (!option.longName.empty()) {
                    flag += ", ";
                }
            }
            if (!option.longName.empty()) {
                flag += "--" + option.longName;
            }
            std::cout << "  " << std::left << std::setw(30) << flag << " " << option.description;
            if (!option.defaultValue.empty()) {
                std::cout << " [default: " << option.defaultValue << "]";
            }
            if (!option.example.empty()) {
                std::cout << " Example: " << option.example;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "Additional Notes:" << std::endl;
    std::cout << "  - Options marked with '=' require a value (e.g., --key=value)" << std::endl;
    std::cout << "  - Options can be specified multiple times where applicable (e.g., -vv for verbose)" << std::endl;
    std::cout << "  - Configuration files can be loaded with --config=file.cfg" << std::endl;
    std::cout << "  - Configuration values can be set with --set=key=value" << std::endl;
    std::cout << "  - Configuration values can be removed with --unset=key" << std::endl;
}

std::string CLIParser::getVersion() const {
    return "NLM Phase 2.0.0 - Real Neural Computation";
}

CLIParser::ParsedArgs CLIParser::parseArgs(int argc, char** argv) const {
    ParsedArgs parsed;
    
    // Setup getopt_long
    struct option longOpts[256]; // Large enough for our options
    int optCount = 0;
    
    for (size_t i = 0; i < options.size(); ++i) {
        if (options[i].longName.empty()) {
            longOpts[optCount].name = nullptr;
        } else {
            longOpts[optCount].name = options[i].longName.c_str();
        }
        longOpts[optCount].has_arg = (options[i].type.find("flag") == std::string::npos && 
                                     options[i].type.find("count") == std::string::npos &&
                                     options[i].type != "bool") ? 1 : 0;
        longOpts[optCount].flag = nullptr;
        longOpts[optCount].val = 0;
        optCount++;
    }
    
    // Parse arguments
    int optIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, getShortOptions().c_str(), 
                           longOpts, &optIndex)) != -1) {
        switch (c) {
            case 'h':
                parsed.helpRequested = true;
                break;
            case 'v':
                parsed.versionRequested = true;
                break;
            case '?':
                parsed.error = true;
                parsed.errorMessage = "Invalid option or missing argument";
                break;
            default:
                auto it = findOptionByChar(c);
                if (it != options.end()) {
                    if (it->type.find("count") != std::string::npos) {
                        parsed.counts[it->longName]++;
                    } else if (optarg) {
                        parsed.values[it->longName] = optarg;
                    } else {
                        parsed.flags.insert(it->longName);
                    }
                }
                break;
        }
    }
    
    // Collect remaining arguments
    for (int i = optind; i < argc; ++i) {
        parsed.positionalArgs.push_back(argv[i]);
    }
    
    return parsed;
}

std::string CLIParser::getShortOptions() const {
    std::string shortOpts;
    for (const auto& option : options) {
        if (option.shortName != '\0') {
            shortOpts += option.shortName;
            if (option.type.find("flag") == std::string::npos && 
                option.type.find("count") == std::string::npos &&
                option.type != "bool") {
                shortOpts += ":";
            }
        }
    }
    return shortOpts;
}

std::vector<CLIParser::Option>::const_iterator CLIParser::findOptionByChar(char c) const {
    for (auto it = options.begin(); it != options.end(); ++it) {
        if (it->shortName == c) {
            return it;
        }
    }
    return options.end();
}

std::vector<CLIParser::Option>::const_iterator CLIParser::findOptionByName(const std::string& name) const {
    for (auto it = options.begin(); it != options.end(); ++it) {
        if (it->longName == name || (it->shortName != '\0' && std::string(1, it->shortName) == name)) {
            return it;
        }
    }
    return options.end();
}

bool CLIParser::validateParsedArgs(const ParsedArgs& parsed) const {
    // Validate option values
    for (const auto& [key, value] : parsed.values) {
        auto it = findOptionByName(key);
        if (it == options.end()) continue;
        
        const Option& option = *it;
        
        // Check valid values if specified
        if (!option.validValues.empty()) {
            if (std::find(option.validValues.begin(), option.validValues.end(), value) == option.validValues.end()) {
                std::cerr << "Error: Invalid value for --" << key << ": '" << value << "'" << std::endl;
                std::cerr << "  Valid values: ";
                for (size_t i = 0; i < option.validValues.size(); ++i) {
                    if (i > 0) std::cerr << ", ";
                    std::cerr << option.validValues[i];
                }
                std::cerr << std::endl;
                return false;
            }
        }
        
        // Run custom validator if provided
        if (option.validator && !option.validator(value)) {
            std::cerr << "Error: Invalid value for --" << key << ": '" << value << "'" << std::endl;
            return false;
        }
        
        // Type validation
        if (option.type.find("float") != std::string::npos) {
            try {
                std::stod(value);
            } catch (...) {
                std::cerr << "Error: Invalid float value for --" << key << ": '" << value << "'" << std::endl;
                return false;
            }
        } else if (option.type.find("num") != std::string::npos) {
            try {
                std::stoll(value);
            } catch (...) {
                std::cerr << "Error: Invalid integer value for --" << key << ": '" << value << "'" << std::endl;
                return false;
            }
        } else if (option.type == "bool") {
            if (value != "true" && value != "false" && value != "1" && value != "0") {
                std::cerr << "Error: Invalid boolean value for --" << key << ": '" << value << "'" << std::endl;
                std::cerr << "  Use 'true', 'false', '1', or '0'" << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

void CLIParser::printConfig(const Config& config) const {
    std::cout << "Current Configuration:" << std::endl;
    std::cout << config.summary() << std::endl;
}

bool CLIParser::validateConfig(const Config& config) const {
    std::cout << "Validating configuration..." << std::endl;
    
    // Check essential configuration values
    std::vector<std::string> requiredKeys = {"random_seed", "simulation_timestep", "neuron_count", 
                                            "region_count", "connection_probability"};
    
    bool valid = true;
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            std::cout << "Warning: Missing required config key: " << key << std::endl;
            valid = false;
        }
    }
    
    // Validate value ranges
    auto seed = config.getOr<int64_t>("random_seed", 0);
    auto timestep = config.getOr<double>("simulation_timestep", 0.001);
    auto neuronCount = config.getOr<int64_t>("neuron_count", 1);
    auto regionCount = config.getOr<int64_t>("region_count", 1);
    auto connProb = config.getOr<float>("connection_probability", 0.15f);
    
    if (timestep <= 0.0) {
        std::cout << "Error: simulation_timestep must be positive" << std::endl;
        valid = false;
    }
    
    if (neuronCount <= 0) {
        std::cout << "Error: neuron_count must be positive" << std::endl;
        valid = false;
    }
    
    if (regionCount <= 0) {
        std::cout << "Error: region_count must be positive" << std::endl;
        valid = false;
    }
    
    if (connProb < 0.0f || connProb > 1.0f) {
        std::cout << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
        valid = false;
    }
    
    if (valid) {
        std::cout << "Configuration validation passed." << std::endl;
    }
    
    return valid;
}

} // namespace nlm