#include "CommandLineParser.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>

namespace nlm {

CommandLineParser::CommandLineParser() {
    // Initialize with default categories
    categories_ = {
        {"general", {}},
        {"brain", {}},
        {"neural", {}},
        {"simulation", {}},
        {"memory", {}},
        {"output", {}},
        {"debug", {}},
        {"profiling", {}},
        {"config", {}}
    };
}

void CommandLineParser::addOption(const std::string& longName, 
                                 const std::string& shortName,
                                 const std::string& description, 
                                 const std::string& type,
                                 const std::string& defaultValue,
                                 const std::string& envVar,
                                 const std::function<bool(const std::string&)>& validator,
                                 bool required,
                                 const std::string& category) {
    CommandLineOption option;
    option.longName = longName;
    option.shortName = shortName;
    option.description = description;
    option.type = type;
    option.defaultValue = defaultValue;
    option.envVar = envVar;
    option.validator = validator;
    option.required = required;
    option.hasValue = !defaultValue.empty();
    option.category = category;
    
    options_.push_back(option);
    
    // Add to categories
    if (categories_.find(category) != categories_.end()) {
        categories_[category].push_back(option);
    }
}

bool CommandLineParser::parse(int argc, char** argv, ParsedArgs& args) {
    args.startTime = std::chrono::system_clock::now();
    
    std::map<std::string, std::string> parsedOptions;
    std::string currentKey;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --help and --version early
        if (arg == "--help") {
            args.help = true;
            continue;
        }
        if (arg == "--version") {
            args.version = true;
            continue;
        }
        
        // Handle --config file
        if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
            continue;
        }
        
        // Handle --output-file
        if (arg.substr(0, 13) == "--output-file") {
            if (arg.find('=') != std::string::npos) {
                args.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
            continue;
        }
        
        // Handle --log-file
        if (arg.substr(0, 10) == "--log-file") {
            if (arg.find('=') != std::string::npos) {
                args.logFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.logFile = argv[++i];
            }
            continue;
        }
        
        // Handle --log-level
        if (arg.substr(0, 12) == "--log-level") {
            if (arg.find('=') != std::string::npos) {
                args.logLevel = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.logLevel = argv[++i];
            }
            continue;
        }
        
        // Handle boolean flags
        if (arg == "--verbose") {
            args.verbose = true;
            continue;
        }
        if (arg == "--quiet") {
            args.quiet = true;
            continue;
        }
        if (arg == "--debug") {
            args.debug = true;
            continue;
        }
        if (arg == "--profile") {
            args.profile = true;
            continue;
        }
        if (arg == "--benchmark") {
            args.benchmark = true;
            continue;
        }
        if (arg == "--validation") {
            args.validation = true;
            continue;
        }
        if (arg == "--list") {
            args.list = true;
            continue;
        }
        if (arg == "--json") {
            args.jsonOutput = true;
            args.textOutput = false;
            continue;
        }
        if (arg == "--csv") {
            args.csvOutput = true;
            args.textOutput = false;
            continue;
        }
        if (arg == "--xml") {
            args.xmlOutput = true;
            args.textOutput = false;
            continue;
        }
        if (arg == "--show-timing") {
            args.showTiming = true;
            continue;
        }
        if (arg == "--metrics") {
            args.enableMetrics = true;
            continue;
        }
        
        // Parse regular options
        if (parseOption(arg, currentKey, parsedOptions, options_)) {
            if (!resolveOptionValue(arg, currentKey, options_)) {
                NLM_LOG_ERROR("Failed to resolve option value: " + arg);
                return false;
            }
        }
    }
    
    // Convert parsed options to ParsedArgs
    for (const auto& pair : parsedOptions) {
        args.options[pair.first] = pair.second;
    }
    
    // Load environment variables
    auto envVars = EnvVarHelper::getNLMEnvVars();
    for (const auto& pair : envVars) {
        // Convert environment variable name to option key
        std::string key = pair.first;
        // Remove prefix and convert to option format
        for (const auto& prefix : {"BRAIN_", "NEURAL_", "SIM_", "MEM_", "OUT_", "DEBUG_", "PROF_"}) {
            if (key.find(prefix) == 0) {
                key = key.substr(prefix.size());
                break;
            }
        }
        // Convert to option key format (--option-name)
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        args.options["--" + key] = pair.second;
    }
    
    // Load configuration file if specified
    if (!args.configFile.empty()) {
        if (!loadOptionsFromConfig(args.configFile, args)) {
            NLM_LOG_WARNING("Failed to load configuration file: " + args.configFile);
        }
    }
    
    // Validate parsed arguments
    std::vector<std::string> errors;
    if (!validate(args, errors)) {
        NLM_LOG_ERROR("Command line validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("Command line parsing completed successfully");
    return true;
}

std::string CommandLineParser::generateHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Command Line Help ===\n\n";
    
    // General options
    oss << "General Options:" << std::endl;
    for (const auto& option : options_) {
        if (option.category == "general") {
            std::string optionStr = "  ";
            if (!option.shortName.empty()) {
                optionStr += "-" + option.shortName;
                if (!option.longName.empty()) {
                    optionStr += ", ";
                }
            }
            if (!option.longName.empty()) {
                optionStr += "--" + option.longName;
            }
            if (!option.defaultValue.empty()) {
                optionStr += " [=default: " + option.defaultValue + "]";
            }
            optionStr += "\n    " + option.description;
            if (!option.envVar.empty()) {
                optionStr += " (environment: " + option.envVar + ")";
            }
            oss << optionStr << std::endl;
        }
    }
    
    // Other categories
    for (const auto& categoryPair : categories_) {
        if (categoryPair.first != "general" && !categoryPair.second.empty()) {
            oss << "\n" << categoryPair.first << " Options:" << std::endl;
            for (const auto& option : categoryPair.second) {
                std::string optionStr = "  ";
                if (!option.shortName.empty()) {
                    optionStr += "-" + option.shortName;
                    if (!option.longName.empty()) {
                        optionStr += ", ";
                    }
                }
                if (!option.longName.empty()) {
                    optionStr += "--" + option.longName;
                }
                if (!option.defaultValue.empty()) {
                    optionStr += " [=default: " + option.defaultValue + "]";
                }
                optionStr += "\n    " + option.description;
                if (!option.envVar.empty()) {
                    optionStr += " (environment: " + option.envVar + ")";
                }
                oss << optionStr << std::endl;
            }
        }
    }
    
    // Configuration options
    oss << "\nConfiguration Options:" << std::endl;
    oss << "  --config=<file>          Load configuration from file" << std::endl;
    oss << "  --merge=<file>           Merge additional configuration file" << std::endl;
    oss << "  --backup=<path>          Create backup before saving" << std::endl;
    
    // Output options
    oss << "\nOutput Options:" << std::endl;
    oss << "  --json                    Output in JSON format" << std::endl;
    oss << "  --csv                     Output in CSV format" << std::endl;
    oss << "  --xml                     Output in XML format" << std::endl;
    oss << "  --text                    Output in text format (default)" << std::endl;
    oss << "  --output-file=<file>      Write output to file" << std::endl;
    oss << "  --show-timing             Show timing information" << std::endl;
    oss << "  --metrics                 Enable metrics collection" << std::endl;
    
    // Logging options
    oss << "\nLogging Options:" << std::endl;
    oss << "  --log-level=<level>       Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
    oss << "  --log-file=<file>         Log to file in addition to console" << std::endl;
    oss << "  --verbose                 Enable verbose logging" << std::endl;
    oss << "  --quiet                   Suppress non-error output" << std::endl;
    oss << "  --debug                   Enable debug mode" << std::endl;
    
    // Debug and profiling options
    oss << "\nDebug and Profiling Options:" << std::endl;
    oss << "  --profile                 Enable performance profiling" << std::endl;
    oss << "  --benchmark              Run benchmarks" << std::endl;
    oss << "  --validation             Run validation checks" << std::endl;
    oss << "  --list                   List available configurations" << std::endl;
    
    oss << "\nFor more information, see documentation or run specific tests." << std::endl;
    return oss.str();
}

std::string CommandLineParser::generateCategoryHelp(const std::string& category) const {
    std::ostringstream oss;
    oss << "=== " << category << " Options ===\n\n";
    
    auto it = categories_.find(category);
    if (it != categories_.end()) {
        for (const auto& option : it->second) {
            std::string optionStr = "  ";
            if (!option.shortName.empty()) {
                optionStr += "-" + option.shortName;
                if (!option.longName.empty()) {
                    optionStr += ", ";
                }
            }
            if (!option.longName.empty()) {
                optionStr += "--" + option.longName;
            }
            if (!option.defaultValue.empty()) {
                optionStr += " [=default: " + option.defaultValue + "]";
            }
            optionStr += "\n    " + option.description;
            if (!option.envVar.empty()) {
                optionStr += " (environment: " + option.envVar + ")";
            }
            oss << optionStr << std::endl;
        }
    }
    
    return oss.str();
}

bool CommandLineParser::isHelpRequested(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == "--help" || argv[i] == "-h") {
            return true;
        }
    }
    return false;
}

bool CommandLineParser::isVersionRequested(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == "--version" || argv[i] == "-v") {
            return true;
        }
    }
    return false;
}

std::vector<std::string> CommandLineParser::getCategories() const {
    std::vector<std::string> categories;
    for (const auto& categoryPair : categories_) {
        if (!categoryPair.second.empty()) {
            categories.push_back(categoryPair.first);
        }
    }
    return categories;
}

bool CommandLineParser::loadOptionsFromConfig(const std::string& configFile, ParsedArgs& args) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + configFile);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse option=value format
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Store in options
            args.options["--" + key] = value;
        }
    }
    
    NLM_LOG_INFO("Loaded options from configuration file: " + configFile);
    return true;
}

bool CommandLineParser::mergeConfigs(const std::vector<std::string>& configFiles, ParsedArgs& args) {
    for (const auto& configFile : configFiles) {
        ParsedArgs tempArgs;
        if (!loadOptionsFromConfig(configFile, tempArgs)) {
            NLM_LOG_WARNING("Failed to load configuration file: " + configFile);
            continue;
        }
        
        // Merge with existing args (command line args take precedence)
        for (const auto& pair : tempArgs.options) {
            if (args.options.find(pair.first) == args.options.end()) {
                args.options[pair.first] = pair.second;
            }
        }
    }
    
    return true;
}

bool CommandLineParser::validate(const ParsedArgs& args, 
                               std::vector<std::string>& errors) const {
    // Validate configuration file if specified
    if (!args.configFile.empty()) {
        std::ifstream file(args.configFile);
        if (!file.is_open()) {
            errors.push_back("Configuration file not found: " + args.configFile);
        }
    }
    
    // Validate output file path
    if (!args.outputFile.empty()) {
        // Check if path is writable
        std::ofstream testFile(args.outputFile, std::ios::app);
        if (!testFile.is_open()) {
            errors.push_back("Cannot write to output file: " + args.outputFile);
        }
        testFile.close();
    }
    
    // Validate log file path
    if (!args.logFile.empty()) {
        std::ofstream testFile(args.logFile, std::ios::app);
        if (!testFile.is_open()) {
            errors.push_back("Cannot write to log file: " + args.logFile);
        }
        testFile.close();
    }
    
    // Validate log level
    if (!args.logLevel.empty()) {
        std::string level = toLower(args.logLevel);
        if (level != "debug" && level != "info" && level != "warning" && 
            level != "error" && level != "critical") {
            errors.push_back("Invalid log level: " + args.logLevel);
        }
    }
    
    // Validate output format
    if (!args.textOutput && !args.jsonOutput && !args.csvOutput && !args.xmlOutput) {
        errors.push_back("No valid output format specified");
    }
    
    return errors.empty();
}

std::string CommandLineParser::generateEnvHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Environment Variables ===\n\n";
    
    oss << "NLM configuration can also be set via environment variables:" << std::endl;
    oss << "  BRAIN_<param>: Brain configuration parameters" << std::endl;
    oss << "  NEURAL_<param>: Neural network parameters" << std::endl;
    oss << "  SIM_<param>: Simulation parameters" << std::endl;
    oss << "  MEM_<param>: Memory system parameters" << std::endl;
    oss << "  OUT_<param>: Output parameters" << std::endl;
    oss << "  DEBUG_<param>: Debug parameters" << std::endl;
    oss << "  PROF_<param>: Profiling parameters" << std::endl;
    
    oss << "\nExample:" << std::endl;
    oss << "  export NLM_BRAIN_NEURON_COUNT=1024" << std::endl;
    oss << "  export NLM_SIM_TIMESTEP=0.001" << std::endl;
    
    return oss.str();
}

std::string CommandLineParser::generateCompletion() const {
    std::ostringstream oss;
    oss << "# Command line completion for NLM" << std::endl;
    oss << "_nlm_completion() {" << std::endl;
    oss << "  local cur prev" << std::endl;
    oss << "  COMPREPLY=()" << std::endl;
    oss << "  prev=${COMP_WORDS[COMP_CWORD-1]}" << std::endl;
    oss << "  cur=${COMP_WORDS[COMP_CWORD]}" << std::endl;
    oss << "  
  case "${prev}" in" << std::endl;
    
    // Add completion for options
    for (const auto& option : options_) {
        if (!option.longName.empty()) {
            oss << "    --" + option.longName + ")" << std::endl;
            oss << "      COMPREPLY=( $(compgen -W \"${option.longName}\" -- ${cur}) )" << std::endl;
            oss << "      return 0" << std::endl;
        }
        if (!option.shortName.empty()) {
            oss << "    -" + option.shortName + ")" << std::endl;
            oss << "      COMPREPLY=( $(compgen -W \"${option.shortName}\" -- ${cur}) )" << std::endl;
            oss << "      return 0" << std::endl;
        }
    }
    
    oss << "  esac" << std::endl;
    oss << "}" << std::endl;
    oss << "complete -F _nlm_completion nlm" << std::endl;
    
    return oss.str();
}

// Static helper functions
bool CommandLineParser::parseOption(const std::string& arg, 
                                   std::string& currentKey,
                                   std::map<std::string, std::string>& options,
                                   const std::vector<CommandLineOption>& validOptions) {
    if (arg.substr(0, 2) == "--") {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            options[key] = value;
            currentKey = "";
            return true;
        } else {
            currentKey = arg.substr(2);
            return true;
        }
    } else if (arg[0] == '-') {
        std::string key = arg.substr(1);
        currentKey = key;
        return true;
    }
    return false;
}

bool CommandLineParser::resolveOptionValue(const std::string& arg,
                                          std::string& currentKey,
                                          const std::vector<CommandLineOption>& validOptions) {
    // This is a simplified implementation
    // In a real implementation, you would parse the next argument as the value
    return true;
}

bool CommandLineParser::isOption(const std::string& arg) {
    return (arg.substr(0, 2) == "--" || (arg[0] == '-' && arg.size() > 1));
}

bool CommandLineParser::isShortOption(const std::string& arg) {
    return (arg[0] == '-' && arg.size() > 1 && arg[1] != '-');
}

bool CommandLineParser::isLongOption(const std::string& arg) {
    return (arg.substr(0, 2) == "--");
}

std::string CommandLineParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string CommandLineParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string CommandLineParser::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// BrainConfigOptions implementation
void BrainConfigOptions::addBrainOptions(CommandLineParser& parser) {
    parser.addOption("neurons", "n", "Number of neurons", "int", "500",
                    "BRAIN_NEURONS", 
                    [](const std::string& v) { return !v.empty(); },
                    true, "brain");
    
    parser.addOption("regions", "r", "Number of brain regions", "int", "1",
                    "BRAIN_REGIONS",
                    [](const std::string& v) { return !v.empty(); },
                    false, "brain");
    
    parser.addOption("connections", "c", "Connection probability (0.0-1.0)", "float", "0.1",
                    "BRAIN_CONNECTIONS",
                    [](const std::string& v) { return !v.empty(); },
                    false, "brain");
    
    parser.addOption("plasticity", "", "Enable plasticity", "bool", "true",
                    "BRAIN_PLASTICITY",
                    [](const std::string& v) { return v == "true" || v == "false"; },
                    false, "brain");
}

void BrainConfigOptions::addNeuralNetworkOptions(CommandLineParser& parser) {
    parser.addOption("network-type", "", "Neural network type", "string", "lif",
                    "NEURAL_TYPE",
                    [](const std::string& v) { return !v.empty(); },
                    false, "neural");
    
    parser.addOption("weight-init", "", "Weight initialization method", "string", "random",
                    "NEURAL_WEIGHT_INIT",
                    [](const std::string& v) { return !v.empty(); },
                    false, "neural");
}

void BrainConfigOptions::addSimulationOptions(CommandLineParser& parser) {
    parser.addOption("timestep", "dt", "Simulation timestep in seconds", "float", "0.001",
                    "SIM_TIMESTEP",
                    [](const std::string& v) { return !v.empty(); },
                    false, "simulation");
    
    parser.addOption("steps", "s", "Number of simulation steps", "int", "1000",
                    "SIM_STEPS",
                    [](const std::string& v) { return !v.empty(); },
                    false, "simulation");
}

void BrainConfigOptions::addMemoryOptions(CommandLineParser& parser) {
    parser.addOption("working-mem", "", "Working memory size", "int", "1000",
                    "MEM_WORKING_MEM",
                    [](const std::string& v) { return !v.empty(); },
                    false, "memory");
    
    parser.addOption("episodic-mem", "", "Episodic memory size", "int", "100",
                    "MEM_EPISODIC_MEM",
                    [](const std::string& v) { return !v.empty(); },
                    false, "memory");
}

void BrainConfigOptions::addOutputOptions(CommandLineParser& parser) {
    parser.addOption("output-dir", "", "Output directory", "string", "./output",
                    "OUT_DIR",
                    [](const std::string& v) { return !v.empty(); },
                    false, "output");
    
    parser.addOption("output-prefix", "", "Output file prefix", "string", "nlm",
                    "OUT_PREFIX",
                    [](const std::string& v) { return !v.empty(); },
                    false, "output");
}

void BrainConfigOptions::addDebugOptions(CommandLineParser& parser) {
    parser.addOption("debug-mode", "", "Enable debug mode", "bool", "false",
                    "DEBUG_MODE",
                    [](const std::string& v) { return v == "true" || v == "false"; },
                    false, "debug");
    
    parser.addOption("log-level", "", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", 
                    "string", "INFO", "DEBUG_LEVEL",
                    [](const std::string& v) { return !v.empty(); },
                    false, "debug");
}

void BrainConfigOptions::addProfilingOptions(CommandLineParser& parser) {
    parser.addOption("profile-steps", "", "Profile every N steps", "int", "100",
                    "PROF_STEPS",
                    [](const std::string& v) { return !v.empty(); },
                    false, "profiling");
    
    parser.addOption("enable-metrics", "", "Enable metrics collection", "bool", "false",
                    "PROF_METRICS",
                    [](const std::string& v) { return v == "true" || v == "false"; },
                    false, "profiling");
}

// ConfigFileUtils implementation
bool ConfigFileUtils::mergeFiles(const std::vector<std::string>& files,
                                 std::map<std::string, std::string>& mergedConfig) {
    for (const auto& file : files) {
        std::ifstream infile(file);
        if (!infile.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(infile, line)) {
            line = CommandLineParser::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = CommandLineParser::trim(line.substr(0, pos));
                std::string value = CommandLineParser::trim(line.substr(pos + 1));
                
                // Remove quotes
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                mergedConfig[key] = value;
            }
        }
    }
    
    return true;
}

bool ConfigFileUtils::validateFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = CommandLineParser::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return false;
        }
        
        std::string key = CommandLineParser::trim(line.substr(0, pos));
        std::string value = CommandLineParser::trim(line.substr(pos + 1));
        
        if (key.empty() || value.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigFileUtils::getBackupPath(const std::string& originalPath) {
    size_t pos = originalPath.find_last_of('.');
    if (pos == std::string::npos) {
        return originalPath + ".backup";
    }
    return originalPath.substr(0, pos) + ".backup" + originalPath.substr(pos);
}

bool ConfigFileUtils::createBackup(const std::string& originalPath) {
    std::string backupPath = getBackupPath(originalPath);
    std::ifstream src(originalPath, std::ios::binary);
    std::ofstream dst(backupPath, std::ios::binary);
    
    if (!src.is_open() || !dst.is_open()) {
        return false;
    }
    
    dst << src.rdbuf();
    return true;
}

bool ConfigFileUtils::restoreFromBackup(const std::string& originalPath) {
    std::string backupPath = getBackupPath(originalPath);
    std::ifstream backup(backupPath, std::ios::binary);
    std::ofstream dst(originalPath, std::ios::binary);
    
    if (!backup.is_open() || !dst.is_open()) {
        return false;
    }
    
    dst << backup.rdbuf();
    return true;
}

std::vector<std::string> ConfigFileUtils::listBackups(const std::string& directory) {
    std::vector<std::string> backups;
    
    // Implementation would list backup files in directory
    // For now, return empty vector
    return backups;
}

// CommandLineCompletion implementation
std::string CommandLineCompletion::generateBashCompletion(const CommandLineParser& parser) {
    std::ostringstream oss;
    oss << "# Bash completion for NLM" << std::endl;
    oss << "" << std::endl;
    oss << "_nlm_bash_completion() {" << std::endl;
    oss << "  local cur prev words cword" << std::endl;
    oss << "  COMPREPLY=()" << std::endl;
    oss << "  # Get the current command line words" << std::endl;
    oss << "  _get_comp_words_by_ref cur prev words cword || return" << std::endl;
    oss << "  
  case "$prev" in" << std::endl;
    
    // Add options for completion
    for (const auto& option : parser.getOptions()) {
        if (!option.longName.empty()) {
            oss << "    --" + option.longName + ")" << std::endl;
            oss << "      COMPREPLY=( $(compgen -W \"$(echo \"$(sed -n 's/\\(^[^=]*\\).*/\\1/p' <<< " + option.defaultValue + ")\" \" -- ${cur}) ) )" << std::endl;
            oss << "      return 0" << std::endl;
        }
        if (!option.shortName.empty()) {
            oss << "    -" + option.shortName + ")" << std::endl;
            oss << "      COMPREPLY=( $(compgen -W \"${option.shortName}\" -- ${cur}) )" << std::endl;
            oss << "      return 0" << std::endl;
        }
    }
    
    oss << "  esac" << std::endl;
    oss << "}" << std::endl;
    oss << "complete -F _nlm_bash_completion nlm" << std::endl;
    
    return oss.str();
}

std::string CommandLineCompletion::generateZshCompletion(const CommandLineParser& parser) {
    std::ostringstream oss;
    oss << "# Zsh completion for NLM" << std::endl;
    oss << "" << std::endl;
    oss << "_nlm_zsh_completion() {" << std::endl;
    oss << "  local -a commands" << std::endl;
    oss << "  local -a options" << std::endl;
    oss << "  
  # Get options from CommandLineParser" << std::endl;
    // This would need to be implemented properly
    
    oss << "}" << std::endl;
    oss << "compdef _nlm_zsh_completion nlm" << std::endl;
    
    return oss.str();
}

std::string CommandLineCompletion::generateFishCompletion(const CommandLineParser& parser) {
    std::ostringstream oss;
    oss << "# Fish completion for NLM" << std::endl;
    oss << "" << std::endl;
    oss << "function __fish_nlm_complete" << std::endl;
    oss << "  # Get options from CommandLineParser" << std::endl;
    // This would need to be implemented properly
    
    oss << "end" << std::endl;
    oss << "complete -c nlm -f -a \"\$(__fish_nlm_complete)\"" << std::endl;
    
    return oss.str();
}

bool CommandLineCompletion::isCompletionSupported() {
    // Check if any completion mechanism is available
    return true; // Simplified check
}

// EnvVarHelper implementation
template<typename T>
T EnvVarHelper::parseEnvVar(const std::string& value, const std::string& key) {
    // Simplified type conversion
    T result;
    std::istringstream iss(value);
    iss >> result;
    return result;
}

// ProgressReporter implementation
void ProgressReporter::start(const std::string& description, int totalSteps) {
    if (!enabled_) return;
    
    startTime_ = std::chrono::system_clock::now();
    totalSteps_ = totalSteps;
    currentStep_ = 0;
    
    NLM_LOG_INFO("Progress: " + description + " (0/" + std::to_string(totalSteps_) + ")");
}

void ProgressReporter::update(int currentStep, const std::string& status) {
    if (!enabled_) return;
    
    currentStep_ = currentStep;
    
    double percentage = (totalSteps_ > 0) ? (static_cast<double>(currentStep_) / totalSteps_) * 100.0 : 0.0;
    
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - startTime_).count();
    
    std::string output = format_;
    // Replace placeholders
    // Note: Simplified implementation
    NLM_LOG_INFO("Progress: " + std::to_string(percentage) + "% " + status);
}

void ProgressReporter::complete(const std::string& finalStatus) {
    if (!enabled_) return;
    
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - startTime_).count();
    
    NLM_LOG_INFO("Progress completed: " + finalStatus + " (" + std::to_string(elapsed) + "s)");
}

void ProgressReporter::setOutputFormat(const std::string& format) {
    format_ = format;
}

void ProgressReporter::enable(bool enable) {
    enabled_ = enable;
}

bool ProgressReporter::isEnabled() const {
    return enabled_;
}

// PerformanceProfiler implementation
void PerformanceProfiler::startProfile(const std::string& section) {
    startTimes_[section] = std::chrono::system_clock::now();
}

double PerformanceProfiler::endProfile(const std::string& section) {
    auto it = startTimes_.find(section);
    if (it == startTimes_.end()) {
        return 0.0;
    }
    
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - it->second).count();
    
    double seconds = static_cast<double>(duration) / 1000000.0;
    elapsedTimes_[section] = seconds;
    
    return seconds;
}

double PerformanceProfiler::getTotalTime() const {
    if (startTimes_.empty()) return 0.0;
    
    auto lastStart = std::prev(startTimes_.end())->second;
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - lastStart).count();
    
    return static_cast<double>(duration) / 1000000.0;
}

void PerformanceProfiler::printProfileResults() const {
    NLM_LOG_INFO("=== Performance Profile Results ===");
    for (const auto& pair : elapsedTimes_) {
        NLM_LOG_INFO("  " + pair.first + ": " + std::to_string(pair.second) + "s");
    }
    NLM_LOG_INFO("  Total: " + std::to_string(getTotalTime()) + "s");
}

bool PerformanceProfiler::saveProfileResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : elapsedTimes_) {
        file << pair.first << ": " << pair.second << "s" << std::endl;
    }
    file << "Total: " << getTotalTime() << "s" << std::endl;
    
    return true;
}

void PerformanceProfiler::reset() {
    startTimes_.clear();
    elapsedTimes_.clear();
    sessionStart_ = std::chrono::system_clock::now();
}

// MetricsCollector implementation
void MetricsCollector::recordMetric(const std::string& name, double value,
                                   const std::map<std::string, std::string>& tags) {
    metrics_[name] = value;
    if (!tags.empty()) {
        metricTags_[name] = tags;
    }
}

std::optional<double> MetricsCollector::getMetric(const std::string& name) const {
    auto it = metrics_.find(name);
    if (it != metrics_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::map<std::string, double> MetricsCollector::getAllMetrics() const {
    return metrics_;
}

bool MetricsCollector::saveMetrics(const std::string& filepath, 
                                  const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        file << "{\n";
        bool first = true;
        for (const auto& pair : metrics_) {
            if (!first) file << ",\n";
            file << "  \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        file << "\n}\n";
    } else {
        // Default text format
        for (const auto& pair : metrics_) {
            file << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    return true;
}

std::string MetricsCollector::generateReport() const {
    std::ostringstream oss;
    oss << "=== Metrics Report ===\n\n";
    
    for (const auto& pair : metrics_) {
        oss << pair.first << ": " << pair.second << std::endl;
        
        auto tagsIt = metricTags_.find(pair.first);
        if (tagsIt != metricTags_.end()) {
            oss << "  Tags:";
            for (const auto& tagPair : tagsIt->second) {
                oss << " " << tagPair.first << "=" << tagPair.second;
            }
            oss << std::endl;
        }
    }
    
    return oss.str();
}

void MetricsCollector::clear() {
    metrics_.clear();
    metricTags_.clear();
}

} // namespace nlm