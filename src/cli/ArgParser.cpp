// NLM CLI Argument Parser implementation
// Comprehensive command-line interface with validation and suggestions

#include "cli/ArgParser.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace nlm::cli {

// Initialize ArgParser with program info
ArgParser::ArgParser(const std::string& name, const std::string& desc, const std::string& ver) {
    setProgramInfo(name, desc, ver);
    
    // Add common arguments
    addArgument(Argument("config", "c", "Configuration file path", ArgumentType::FILEPATH, false, "Configuration"));
    addArgument(Argument("output", "o", "Output file path", ArgumentType::FILEPATH, false, "Output"));
    addArgument(Argument("verbose", "v", "Enable verbose output", ArgumentType::BOOLEAN, false, "General"));
    addArgument(Argument("quiet", "q", "Suppress non-essential output", ArgumentType::BOOLEAN, false, "General"));
    addArgument(Argument("seed", "s", "Random seed for reproducible results", ArgumentType::INTEGER64, false, "General"));
    addArgument(Argument("neurons", "n", "Number of neurons", ArgumentType::INTEGER, false, "Brain"));
    addArgument(Argument("timestep", "t", "Simulation timestep", ArgumentType::FLOAT, false, "Simulation"));
    addArgument(Argument("steps", "", "Number of simulation steps", ArgumentType::INTEGER, false, "Simulation"));
    
    // Add environment variables
    addEnvironmentVariable("NLM_CONFIG", "Path to default configuration file", ArgumentType::FILEPATH);
    addEnvironmentVariable("NLM_LOG_LEVEL", "Logging level (debug, info, warning, error)", ArgumentType::STRING);
    addEnvironmentVariable("NLM_SEED", "Random seed for reproducible results", ArgumentType::INTEGER64);
    addEnvironmentVariable("NLM_VERBOSE", "Enable verbose output", ArgumentType::BOOLEAN);
    addEnvironmentVariable("NLM_QUIET", "Suppress non-essential output", ArgumentType::BOOLEAN);
    
    // Add examples
    addExample("Basic Usage", "nlm --config configs/default.cfg");
    addExample("Verbose Mode", "nlm --verbose --config myconfig.json");
    addExample("Custom Settings", "nlm --neurons 2000 --timestep 0.001 --steps 5000");
    addExample("Quick Test", "nlm --seed 42 --verbose");
}

// Initialize with more organized structure
void ArgParser::initialize(const std::string& name, const std::string& desc, const std::string& ver) {
    setProgramInfo(name, desc, ver);
    
    // Configuration arguments
    addArgument(Argument("config", "c", "Path to configuration file (JSON, YAML, or key=value format)", 
                       ArgumentType::FILEPATH, false, "Configuration"));
    addArgument(Argument("output", "o", "Output file path for results", 
                       ArgumentType::FILEPATH, false, "Output"));
    
    // Execution arguments
    addArgument(Argument("neurons", "n", "Number of neurons (default: 1000)", 
                       ArgumentType::INTEGER, false, "Execution"));
    addArgument(Argument("regions", "r", "Number of brain regions (default: 2)", 
                       ArgumentType::INTEGER, false, "Execution"));
    addArgument(Argument("timestep", "t", "Simulation timestep in seconds (default: 0.001)", 
                       ArgumentType::FLOAT, false, "Execution"));
    addArgument(Argument("steps", "", "Number of simulation steps (default: 1000)", 
                       ArgumentType::INTEGER, false, "Execution"));
    
    // Output and debugging
    addArgument(Argument("verbose", "v", "Enable verbose output (detailed logging)", 
                       ArgumentType::BOOLEAN, false, "Output"));
    addArgument(Argument("quiet", "q", "Suppress non-essential output (quiet mode)", 
                       ArgumentType::BOOLEAN, false, "Output"));
    addArgument(Argument("log_level", "", "Set log level: debug, info, warning, error", 
                       ArgumentType::ENUM, false, "Output"), 
                       std::vector<std::string>{"debug", "info", "warning", "error"});
    
    // Random seed and reproducibility
    addArgument(Argument("seed", "s", "Random seed for reproducible results", 
                       ArgumentType::INTEGER64, false, "General"));
    
    // Performance
    addArgument(Argument("performance", "p", "Performance mode (fast, balanced, quality)", 
                       ArgumentType::ENUM, false, "Performance"),
                       std::vector<std::string>{"fast", "balanced", "quality"));
    
    // Environment variables
    addEnvironmentVariable("NLM_CONFIG", "Path to default configuration file", ArgumentType::FILEPATH);
    addEnvironmentVariable("NLM_LOG_LEVEL", "Logging level (debug, info, warning, error)", ArgumentType::STRING);
    addEnvironmentVariable("NLM_SEED", "Random seed for reproducible results", ArgumentType::INTEGER64);
    addEnvironmentVariable("NLM_VERBOSE", "Enable verbose output", ArgumentType::BOOLEAN);
    addEnvironmentVariable("NLM_QUIET", "Suppress non-essential output", ArgumentType::BOOLEAN);
    addEnvironmentVariable("NLM_NEURONS", "Number of neurons (overrides command line)", ArgumentType::INTEGER);
    addEnvironmentVariable("NLM_REGIONS", "Number of brain regions (overrides command line)", ArgumentType::INTEGER);
    
    // Examples
    addExample("Basic Usage", "nlm --config configs/default.cfg");
    addExample("Custom Brain", "nlm --neurons 2000 --regions 3 --timestep 0.002");
    addExample("Verbose Debug", "nlm --verbose --seed 123 --config test.cfg");
    addExample("Performance Mode", "nlm --performance balanced");
    addExample("Quick Test", "nlm --steps 100 --verbose");
    addExample("Output Results", "nlm --output results.json --verbose");
}

// Parse command line arguments with enhanced error handling
bool ArgParser::parse(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Help and version flags
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return false;
        }
        
        if (arg == "--version") {
            showVersion();
            return false;
        }
        
        // Parse options
        if (parseArgument(arg, argv, i)) {
            continue;
        }
        
        // Positional arguments
        std::string positional = parsePositional(arg, i, argc, argv);
        if (!positional.empty()) {
            positional_args.push_back(positional);
        } else {
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            suggestSimilar(arg);
            success = false;
        }
    }
    
    // Apply environment variables if not overridden by command line
    applyEnvironmentVariables();
    
    return success;
}

// Parse individual argument
bool ArgParser::parseArgument(const std::string& arg, char** argv, int& i) {
    if (arg.substr(0, 2) == "--") {
        // Long option: --name=value or --name value
        std::string key;
        std::string value;
        
        size_t equals_pos = arg.find('=');
        if (equals_pos != std::string::npos) {
            key = arg.substr(2, equals_pos - 2);
            value = arg.substr(equals_pos + 1);
        } else {
            key = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
        }
        
        return processOption(key, value);
    } else if (arg.substr(0, 1) == "-") {
        // Short option: -n or -abc
        std::string key;
        std::string value;
        
        if (arg.size() == 2) {
            key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
        } else {
            // Handle multiple short options
            for (char c : arg.substr(1)) {
                key = std::string(1, c);
                processOption(key, "");
            }
            return true;
        }
        
        return processOption(key, value);
    }
    
    return false;
}

// Process option after parsing
bool ArgParser::processOption(const std::string& key, const std::string& value) {
    // Find matching argument
    for (const auto& arg : arguments) {
        if (arg.name == key || arg.short_name == key) {
            if (validateOption(arg, value)) {
                values[arg.name] = value;
                return true;
            }
            return false;
        }
    }
    
    // Check environment variables
    for (const auto& env : environment_variables) {
        if (env.name == key) {
            if (validateOption(env, value)) {
                values[env.name] = value;
                return true;
            }
            return false;
        }
    }
    
    std::cerr << "Error: Unknown option --" << key << std::endl;
    suggestSimilar(key);
    return false;
}

// Validate option value
bool ArgParser::validateOption(const Argument& arg, const std::string& value) {
    if (arg.required && value.empty()) {
        std::cerr << "Error: Argument --" << arg.name << " is required but missing." << std::endl;
        return false;
    }
    
    if (arg.type == ArgumentType::ENUM && !value.empty()) {
        bool valid = std::find(arg.valid_values.begin(), arg.valid_values.end(), value) != arg.valid_values.end();
        if (!valid) {
            std::cerr << "Error: Invalid value '" << value << "' for --" << arg.name << "." << std::endl;
            std::cerr << "  Valid values: ";
            for (size_t i = 0; i < arg.valid_values.size(); ++i) {
                if (i > 0) std::cerr << ", ";
                std::cerr << arg.valid_values[i];
            }
            std::cerr << std::endl;
            return false;
        }
    }
    
    if (arg.type == ArgumentType::FILEPATH && !value.empty()) {
        std::ifstream file(value);
        if (!file) {
            std::cerr << "Error: Cannot access file --" << arg.name << ": " << value << std::endl;
            return false;
        }
    }
    
    if (arg.type == ArgumentType::DIRECTORY && !value.empty()) {
        struct stat info;
        if (stat(value.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
            std::cerr << "Error: Not a directory --" << arg.name << ": " << value << std::endl;
            return false;
        }
    }
    
    return true;
}

// Parse positional argument
std::string ArgParser::parsePositional(const std::string& arg, int& i, int argc, char** argv) {
    // Check if this is a value following a known option
    for (const auto& option : values) {
        auto it = std::find_if(arguments.begin(), arguments.end(),
                               [&option](const Argument& a) { return a.name == option.first; });
        if (it != arguments.end()) {
            if (it->type == ArgumentType::STRING || it->type == ArgumentType::FILEPATH || 
                it->type == ArgumentType::INTEGER || it->type == ArgumentType::INTEGER64 ||
                it->type == ArgumentType::FLOAT || it->type == ArgumentType::BOOLEAN ||
                it->type == ArgumentType::ENUM) {
                // This could be a value for a previous option
                // We'll handle this in applyEnvironmentVariables
                break;
            }
        }
    }
    
    // Not a positional argument
    return "";
}

// Apply environment variables
void ArgParser::applyEnvironmentVariables() {
    for (const auto& env : environment_variables) {
        if (!has(env.name) && !env.default_value.empty()) {
            // Check environment variable
            const char* env_value = std::getenv(env.name.c_str());
            if (env_value != nullptr) {
                values[env.name] = std::string(env_value);
            } else if (!env.default_value.empty()) {
                values[env.name] = env.default_value;
            }
        }
    }
}

// Suggest similar options
void ArgParser::suggestSimilar(const std::string& input, const std::vector<std::string>& options) {
    std::vector<std::string> all_options;
    
    // Collect all known options
    for (const auto& arg : arguments) {
        all_options.push_back("--" + arg.name);
        if (!arg.short_name.empty()) {
            all_options.push_back("-" + arg.short_name);
        }
    }
    
    for (const auto& env : environment_variables) {
        all_options.push_back("--" + env.name);
    }
    
    if (!options.empty()) {
        all_options.insert(all_options.end(), options.begin(), options.end());
    }
    
    // Find suggestions
    std::vector<std::string> suggestions;
    for (const auto& option : all_options) {
        if (levenshteinDistance(input, option) <= 2 && !input.empty()) {
            suggestions.push_back(option);
        }
    }
    
    if (!suggestions.empty()) {
        std::cerr << "  Did you mean:" << std::endl;
        for (const auto& suggestion : suggestions) {
            std::cerr << "    --" << suggestion.substr(2) << std::endl;
        }
    }
}

// Levenshtein distance calculation
int ArgParser::levenshteinDistance(const std::string& s1, const std::string& s2) {
    int m = s1.length(), n = s2.length();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i-1][j] + 1,         // deletion
                                dp[i][j-1] + 1,         // insertion
                                dp[i-1][j-1] + cost}); // substitution
        }
    }
    
    return dp[m][n];
}

// Convert parsed values to Config object
std::shared_ptr<Config> ArgParser::toConfig() const {
    auto config = std::make_shared<Config>();
    
    // Add parsed values
    for (const auto& pair : values) {
        config->set(pair.first, pair.second, ConfigSource::CommandLine);
    }
    
    // Add environment variables
    for (const auto& env : environment_variables) {
        std::string value = getValue(env.name);
        if (!value.empty()) {
            config->set(env.name, value, ConfigSource::Environment);
        } else if (!env.default_value.empty()) {
            config->set(env.name, env.default_value, ConfigSource::Default);
        }
    }
    
    // Add positional arguments
    if (!positional_args.empty()) {
        config->set("positional_args", positional_args, ConfigSource::CommandLine);
    }
    
    return config;
}

// Generate comprehensive help text
std::string ArgParser::generateHelp() const {
    std::stringstream help;
    
    help << program_name << " " << version << std::endl;
    help << program_description << std::endl;
    help << std::endl;
    
    help << "Usage:" << std::endl;
    help << "  " << program_name;
    for (const auto& arg : arguments) {
        if (!arg.short_name.empty()) {
            help << " [-" << arg.short_name << "]";
        }
        help << " [--" << arg.name << "]";
    }
    help << " [options] [arguments]" << std::endl;
    help << std::endl;
    
    // Group arguments by category
    std::map<std::string, std::vector<Argument>> grouped_args;
    for (const auto& arg : arguments) {
        grouped_args[arg.group].push_back(arg);
    }
    
    // Configuration options
    if (grouped_args.find("Configuration") != grouped_args.end()) {
        help << "Configuration:" << std::endl;
        for (const auto& arg : grouped_args["Configuration"]) {
            help << "  --" << arg.name;
            if (!arg.short_name.empty()) {
                help << ", -" << arg.short_name;
            }
            help << std::endl;
            help << "    " << arg.description << std::endl;
            if (!arg.default_value.empty()) {
                help << "    Default: " << arg.default_value << std::endl;
            }
            help << std::endl;
        }
    }
    
    // Execution options
    if (grouped_args.find("Execution") != grouped_args.end()) {
        help << "Execution:" << std::endl;
        for (const auto& arg : grouped_args["Execution"]) {
            help << "  --" << arg.name;
            if (!arg.short_name.empty()) {
                help << ", -" << arg.short_name;
            }
            help << std::endl;
            help << "    " << arg.description << std::endl;
            if (!arg.default_value.empty()) {
                help << "    Default: " << arg.default_value << std::endl;
            }
            help << std::endl;
        }
    }
    
    // Output options
    if (grouped_args.find("Output") != grouped_args.end()) {
        help << "Output:" << std::endl;
        for (const auto& arg : grouped_args["Output"]) {
            help << "  --" << arg.name;
            if (!arg.short_name.empty()) {
                help << ", -" << arg.short_name;
            }
            help << std::endl;
            help << "    " << arg.description << std::endl;
            help << std::endl;
        }
    }
    
    // General options
    if (grouped_args.find("General") != grouped_args.end()) {
        help << "General:" << std::endl;
        for (const auto& arg : grouped_args["General"]) {
            help << "  --" << arg.name;
            if (!arg.short_name.empty()) {
                help << ", -" << arg.short_name;
            }
            help << std::endl;
            help << "    " << arg.description << std::endl;
            if (!arg.default_value.empty()) {
                help << "    Default: " << arg.default_value << std::endl;
            }
            help << std::endl;
        }
    }
    
    // Performance options
    if (grouped_args.find("Performance") != grouped_args.end()) {
        help << "Performance:" << std::endl;
        for (const auto& arg : grouped_args["Performance"]) {
            help << "  --" << arg.name;
            if (!arg.short_name.empty()) {
                help << ", -" << arg.short_name;
            }
            help << std::endl;
            help << "    " << arg.description << std::endl;
            if (!arg.default_value.empty()) {
                help << "    Default: " << arg.default_value << std::endl;
            }
            help << std::endl;
        }
    }
    
    // Environment variables
    help << "Environment Variables:" << std::endl;
    for (const auto& env : environment_variables) {
        help << "  --" << env.name << std::endl;
        help << "    " << env.description << std::endl;
        if (!env.default_value.empty()) {
            help << "    Default: " << env.default_value << std::endl;
        }
        help << std::endl;
    }
    
    // Examples
    if (!examples.empty()) {
        help << "Examples:" << std::endl;
        for (const auto& example : examples) {
            help << "  " << example.first << ":" << std::endl;
            help << "    $ " << program_name << " " << example.second << std::endl;
            help << std::endl;
        }
    }
    
    // Common usage patterns
    help << "Common Usage Patterns:" << std::endl;
    help << std::endl;
    help << "  Basic run:" << std::endl;
    help << "    $ " << program_name << " --config configs/default.cfg" << std::endl;
    help << std::endl;
    help << "  Custom configuration:" << std::endl;
    help << "    $ " << program_name << " --neurons 2000 --regions 3 --timestep 0.001" << std::endl;
    help << std::endl;
    help << "  Verbose debugging:" << std::endl;
    help << "    $ " << program_name << " --verbose --seed 123" << std::endl;
    help << std::endl;
    help << "  Performance mode:" << std::endl;
    help << "    $ " << program_name << " --performance balanced --quiet" << std::endl;
    help << std::endl;
    help << "  Output results:" << std::endl;
    help << "    $ " << program_name << " --output results.json --verbose" << std::endl;
    help << std::endl;
    
    help << "Options:" << std::endl;
    help << "  -h, --help" << std::endl;
    help << "    Show this help message and exit." << std::endl;
    help << std::endl;
    help << "  --version" << std::endl;
    help << "    Show version information and exit." << std::endl;
    help << std::endl;
    help << "  --config <file>" << std::endl;
    help << "    Load configuration from file (JSON, YAML, or key=value format)." << std::endl;
    help << std::endl;
    
    return help.str();
}

// Show help to stdout
void ArgParser::showHelp() const {
    std::cout << generateHelp();
}

// Show version information
void ArgParser::showVersion() const {
    std::cout << program_name << " version " << version << std::endl;
    std::cout << std::endl;
    std::cout << program_description << std::endl;
    std::cout << std::endl;
    std::cout << "License: MIT" << std::endl;
    std::cout << "Copyright: NLM Project" << std::endl;
}