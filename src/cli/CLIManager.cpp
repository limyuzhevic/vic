// CLIManager implementation
// Core CLI manager that orchestrates argument parsing, help, and configuration

#include "cli/CLIManager.hpp"
#include "cli/ConfigValidator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm::cli {

// Implementation of argument parsing
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

// Implementation of argument parsing
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

// Implementation of option processing
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

// Implementation of option validation
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

// Implementation of positional argument parsing
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

// Implementation of environment variable application
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

// Implementation of similarity suggestion
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

// Implementation of Levenshtein distance
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

// Implementation of configuration conversion
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

// Implementation of configuration loading
bool CLIManager::parse(int argc, char** argv) {
    // Parse arguments
    if (!arg_parser->parse(argc, argv)) {
        return false;
    }
    
    // Update settings
    verbose = arg_parser->has("verbose") && arg_parser->getValue("verbose") == "true";
    quiet = arg_parser->has("quiet") && arg_parser->getValue("quiet") == "true";
    help_requested = arg_parser->isHelpRequested();
    version_requested = arg_parser->isVersionRequested();
    
    // Get configuration file
    std::string config_file = arg_parser->getConfigFile();
    
    // Load configuration
    if (!config_file.empty()) {
        if (config->loadFromFile(config_file)) {
            std::cout << "Loaded configuration from: " << config_file << std::endl;
        } else {
            std::cout << "Using default configuration (file not found: " << config_file << ")" << std::endl;
        }
    }
    
    // Override with command line arguments
    if (arg_parser->has("seed")) {
        config->set("random_seed", std::stoll(arg_parser->getValue("seed")), ConfigSource::CommandLine);
    }
    
    if (arg_parser->has("neurons")) {
        config->set("neuron_count", std::stoll(arg_parser->getValue("neurons")), ConfigSource::CommandLine);
    }
    
    if (arg_parser->has("regions")) {
        config->set("region_count", std::stoll(arg_parser->getValue("regions")), ConfigSource::CommandLine);
    }
    
    if (arg_parser->has("timestep")) {
        config->set("simulation_timestep", std::stod(arg_parser->getValue("timestep")), ConfigSource::CommandLine);
    }
    
    if (arg_parser->has("steps")) {
        config->set("max_simulation_steps", std::stoll(arg_parser->getValue("steps")), ConfigSource::CommandLine);
    }
    
    if (arg_parser->has("verbose")) {
        config->set("log_level", "INFO", ConfigSource::CommandLine);
    }
    
    // Set defaults for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    return true;
}

// Implementation of configuration validation
bool CLIManager::validate() const {
    // Check for conflicts
    if (verbose && quiet) {
        std::cerr << "Error: --verbose and --quiet cannot be used together" << std::endl;
        return false;
    }
    
    // Validate configuration values
    if (config->has("neuron_count")) {
        int64_t neurons = config->getOr<int64_t>("neuron_count", 1);
        if (neurons <= 0) {
            std::cerr << "Error: neuron_count must be positive" << std::endl;
            return false;
        }
    }
    
    if (config->has("simulation_timestep")) {
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0) {
            std::cerr << "Error: simulation_timestep must be positive" << std::endl;
            return false;
        }
    }
    
    if (config->has("max_simulation_steps")) {
        int64_t steps = config->getOr<int64_t>("max_simulation_steps", 1000);
        if (steps <= 0) {
            std::cerr << "Error: max_simulation_steps must be positive" << std::endl;
            return false;
        }
    }
    
    return true;
}

// Implementation of help system
void CLIManager::showHelp() const {
    help_system->showHelp();
}

// Implementation of version display
void CLIManager::showVersion() const {
    help_system->showVersion();
}