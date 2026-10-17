// NLM Command Line Interface Framework Implementation
// Provides modern argument parsing and unified interface for all NLM executables

#include "CLIFramework.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <ctime>

namespace nlm {

// CLIParser implementation
CLIParser::CLIParser() {
    initializeHelpTexts();
    versionString = "NLM Phase 6 Final Integration - v0.1.0";
}

CLIParser::~CLIParser() = default;

void CLIParser::initializeHelpTexts() {
    helpTexts["general"] = 
        "\nGeneral Options:\n"
        "  --help, -h           Show this help message\n"
        "  --version            Show version information\n"
        "  --config <file>      Load configuration from file\n"
        "  --log-level <level>  Set logging level (Trace, Debug, Info, Warning, Error, Critical, Off)\n"
        "  --log-file <file>    Log output to file\n"
        "  --quiet, -q          Quiet mode (suppress output)\n"
        "  --verbose, -v        Verbose mode (show detailed output)\n"
        "  --dry-run            Don't run commands, just show what would be executed\n";
    
    helpTexts["commands"] = 
        "\nCommands:\n"
        "  demo                 Run a demo mode (default)\n"
        "  benchmark            Run performance benchmarks\n"
        "  script               Run batch processing script\n"
        "  test                 Run system tests\n"
        "  validate             Validate configuration files\n";
    
    helpTexts["demo"] = 
        "\nDemo Options:\n"
        "  --demo-type <type>   Demo type: phase3, phase4, phase6 (default: phase6)\n"
        "  --episodes <n>       Number of episodes for demo (default: 5)\n"
        "  --steps <n>          Steps per episode (default: 200)\n";
    
    helpTexts["benchmark"] = 
        "\nBenchmark Options:\n"
        "  --benchmark-config <neurons,steps> Specify benchmark configuration\n"
        "  --benchmark-output <file>    Save benchmark results to file\n"
        "  --detailed           Run detailed benchmark analysis\n";
    
    helpTexts["script"] = 
        "\nScript Options:\n"
        "  --script-file <file>           Load commands from script file\n"
        "  --script-mode <mode>           Script execution mode: file, sweep, grid, sequential, parallel\n"
        "  --sweep-parameter <param>       Parameter sweep: \"name value1,value2,...\"\n"
        "  --grid-search <param>           Grid search: \"name value1,value2,...\"\n"
        "  --script-verbose, -v            Enable verbose output\n"
        "  --script-output-format <format> Output format: json, csv, text\n"
        "  --script-output <file>          Save output to file\n"
        "  --script-timeout <seconds>      Timeout for script execution\n"
        "  --script-parallel, -p           Execute in parallel\n"
        "  --script-max-threads <n>        Maximum threads for parallel execution\n"
        "  --script-parameter-type <type>  Parameter generation type: linear, geometric, random, grid, fixed\n"
        "  --script-start <value>          Start value for parameter generation\n"
        "  --script-end <value>            End value for parameter generation\n"
        "  --script-steps <n>              Number of steps for parameter generation\n"
        "  --script-seed <seed>            Random seed for parameter generation\n"
        "  --continue                      Continue from last checkpoint\n";
    
    helpTexts["usage"] = 
        "\nUsage:\n"
        "  nlm [options] [command] [args...]\n"
        "\nExamples:\n"
        "  nlm                          # Run Phase 6 demo with defaults\n"
        "  nlm demo --demo-type phase3  # Run Phase 3 demo\n"
        "  nlm benchmark --benchmark-config 500,1000  # Benchmark with 500 neurons, 1000 steps\n"
        "  nlm --config myconfig.cfg validate            # Validate a configuration file\n"
        "  nlm --log-level debug demo                      # Run demo with debug logging\n"
        "  nlm script --script-file experiment_suite.json  # Run batch processing script\n"
        "  nlm script --sweep-parameter \"neuron_count 500,1000,2000\" --experiment phase6  # Parameter sweep\n"
        "  nlm script --grid-search \"random_seed 42,123,456\" \"neuron_count 1000,2000\" --experiment phase6  # Grid search\n"
        "  nlm script --sequential experiment1.json experiment2.json  # Sequential execution\n"
        "  nlm script --parallel --script-file batch.json  # Parallel execution\n";
    
    helpTexts["error_prefix"] = "Error: ";
}

bool CLIParser::parse(int argc, char** argv, CLIConfig& config) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    return parseCommand(args, config) && parseOptions(args, config) && parseCommandArgs(args, config);
}

bool CLIParser::parseCommand(const std::vector<std::string>& args, CLIConfig& config) {
    if (args.empty()) {
        config.command = "demo";
        config.demo_type = "phase6";
        return true;
    }
    
    std::string firstArg = args[0];
    if (firstArg == "--help" || firstArg == "-h") {
        config.command = "help";
        return true;
    }
    
    if (firstArg == "--version") {
        config.command = "version";
        return true;
    }
    
    config.command = firstArg;
    config.command_args = std::vector<std::string>(args.begin() + 1, args.end());
    return true;
}

bool CLIParser::parseOptions(const std::vector<std::string>& args, CLIConfig& config) {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--version") {
            config.version = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--dry-run") {
            config.dry_run = true;
        } else if (arg == "--benchmark") {
            config.benchmark_mode = true;
        } else if (arg == "--script") {
            config.script_mode = true;
        } else if (arg == "--test") {
            config.test_mode = true;
        } else if (arg == "--validate") {
            config.validate_mode = true;
        } else if (arg == "--debug") {
            config.debug_mode = true;
        } else if (startsWith(arg, "--config")) {
            config.config_file = extractValue(arg);
        } else if (startsWith(arg, "--log-level")) {
            config.log_level = extractValue(arg);
        } else if (startsWith(arg, "--log-file")) {
            config.log_file = extractValue(arg);
        } else if (startsWith(arg, "--demo-type")) {
            config.demo_type = extractValue(arg);
        } else if (startsWith(arg, "--episodes")) {
            config.demo_episodes = std::stoi(extractValue(arg));
        } else if (startsWith(arg, "--steps")) {
            config.demo_steps = std::stoi(extractValue(arg));
        } else if (startsWith(arg, "--benchmark-config")) {
            std::string value = extractValue(arg);
            parseBenchmarkConfig(value, config.benchmark_configs);
        } else if (startsWith(arg, "--benchmark-output")) {
            config.benchmark_output = extractValue(arg);
        } else if (arg == "--detailed") {
            config.benchmark_detailed = true;
        } else if (startsWith(arg, "--script-file")) {
            config.script_file = extractValue(arg);
        } else if (arg == "--script-verbose" || arg == "-v") {
            config.script_verbose = true;
        } else if (startsWith(arg, "--script-output-format")) {
            config.script_output_format = extractValue(arg);
        } else if (startsWith(arg, "--script-output")) {
            config.benchmark_output = extractValue(arg);
        } else if (startsWith(arg, "--script-timeout")) {
            config.script_timeout = std::stod(extractValue(arg));
        } else if (arg == "--script-parallel" || arg == "-p") {
            config.script_parallel = true;
            config.script_mode = "parallel";
        } else if (startsWith(arg, "--script-max-threads")) {
            config.script_max_threads = std::stoull(extractValue(arg));
        } else if (startsWith(arg, "--script-parameter-type")) {
            config.script_parameter_type = extractValue(arg);
        } else if (startsWith(arg, "--script-start")) {
            config.script_start_value = extractValue(arg);
        } else if (startsWith(arg, "--script-end")) {
            config.script_end_value = extractValue(arg);
        } else if (startsWith(arg, "--script-steps")) {
            config.script_steps = std::stoull(extractValue(arg));
        } else if (startsWith(arg, "--script-seed")) {
            config.script_seed = extractValue(arg);
        } else if (arg == "--script-sequential") {
            config.script_mode = "sequential";
        } else if (arg == "--script-sweep") {
            config.script_mode = "sweep";
        } else if (arg == "--script-grid") {
            config.script_mode = "grid";
        } else if (startsWith(arg, "--sweep-parameter")) {
            std::string value = extractValue(arg);
            parseSweepParameter(value, config.sweep_parameters);
        } else if (startsWith(arg, "--grid-search")) {
            std::string value = extractValue(arg);
            parseGridSearch(value, config.grid_parameters);
        } else if (arg == "--continue") {
            config.script_continue = true;
        } else if (arg == "--validate-configs") {
            config.validate_configs = true;
        } else if (arg == "--validate-format") {
            config.validate_format = true;
        }
    }
    
    return true;
}

bool CLIParser::parseCommandArgs(const std::vector<std::string>& args, CLIConfig& config) {
    // Remove already parsed options and commands
    std::vector<std::string> filteredArgs;
    bool inCommand = false;
    
    for (const std::string& arg : args) {
        if (arg == "help" || arg == "version") {
            continue;
        }
        
        if (arg == "demo" || arg == "benchmark" || arg == "script" || 
            arg == "test" || arg == "validate") {
            inCommand = true;
            config.command = arg;
            continue;
        }
        
        if (inCommand) {
            filteredArgs.push_back(arg);
        }
    }
    
    config.command_args = filteredArgs;
    return true;
}

void CLIParser::printHelp(const std::string& programName) const {
    printVersion();
    std::cout << helpTexts.at("usage");
    std::cout << helpTexts.at("general");
    std::cout << helpTexts.at("commands");
    printCommandHelp(stringToCommand(config.command));
    std::cout << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << versionString << std::endl;
}

void CLIParser::printCommandHelp(Command cmd) const {
    switch (cmd) {
        case Command::Demo:
            std::cout << helpTexts.at("demo");
            break;
        case Command::Benchmark:
            std::cout << helpTexts.at("benchmark");
            break;
        case Command::Script:
            std::cout << helpTexts.at("script");
            break;
        case Command::Test:
            std::cout << helpTexts.at("test");
            break;
        case Command::Validate:
            std::cout << helpTexts.at("test");  // Reuse test help for validate
            break;
        default:
            break;
    }
}

bool CLIParser::validate(const CLIConfig& config) const {
    // Validate configuration
    if (config.config_file.empty() && !config.command.empty()) {
        std::string defaultConfig = "configs/default.cfg";
        std::ifstream file(defaultConfig);
        if (!file.good()) {
            printError("Default configuration file not found: " + defaultConfig);
            return false;
        }
    }
    
    if (!config.log_level.empty()) {
        auto level = stringToLogLevel(config.log_level);
        if (level == LogLevel::Off) {
            printError("Invalid log level: " + config.log_level);
            return false;
        }
    }
    
    if (config.command == "demo" && !config.demo_type.empty()) {
        auto demoTypes = getAvailableDemoTypes();
        if (std::find(demoTypes.begin(), demoTypes.end(), config.demo_type) == demoTypes.end()) {
            printError("Invalid demo type: " + config.demo_type);
            return false;
        }
    }
    
    if (config.command == "benchmark") {
        if (config.benchmark_configs.empty()) {
            config.benchmark_configs = getDefaultBenchmarkConfigs();
        }
    }
    
    return true;
}

std::string CLIParser::commandToString(Command cmd) {
    switch (cmd) {
        case Command::None: return "none";
        case Command::Demo: return "demo";
        case Command::Benchmark: return "benchmark";
        case Command::Script: return "script";
        case Command::Test: return "test";
        case Command::Validate: return "validate";
        case Command::Help: return "help";
        case Command::Version: return "version";
        default: return "unknown";
    }
}

Command CLIParser::stringToCommand(const std::string& str) {
    static const std::unordered_map<std::string, Command> commandMap = {
        {"none", Command::None},
        {"demo", Command::Demo},
        {"benchmark", Command::Benchmark},
        {"script", Command::Script},
        {"test", Command::Test},
        {"validate", Command::Validate},
        {"help", Command::Help},
        {"version", Command::Version}
    };
    
    auto it = commandMap.find(str);
    return (it != commandMap.end()) ? it->second : Command::None;
}

ConfigFormat CLIParser::detectConfigFormat(const std::string& filepath) {
    if (filepath.empty()) return ConfigFormat::Unknown;
    
    // Check file extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return ConfigFormat::JSON;
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yaml") {
        return ConfigFormat::YAML;
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yml") {
        return ConfigFormat::YAML;
    }
    
    // Try to detect from content
    std::ifstream file(filepath);
    if (!file.is_open()) return ConfigFormat::Unknown;
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    // Remove comments and trim
    size_t commentPos = firstLine.find("#");
    if (commentPos != std::string::npos) {
        firstLine = firstLine.substr(0, commentPos);
    }
    
    // Check if it looks like JSON
    if (firstLine.find("{[") != std::string::npos) {
        return ConfigFormat::JSON;
    }
    
    // Check if it looks like YAML
    if (firstLine.find(": ") != std::string::npos || firstLine.find("\n  ") != std::string::npos) {
        return ConfigFormat::YAML;
    }
    
    // Default to simple key-value
    return ConfigFormat::SimpleKeyValue;
}

LogLevel CLIParser::stringToLogLevel(const std::string& level) {
    static const std::unordered_map<std::string, LogLevel> levelMap = {
        {"trace", LogLevel::Trace},
        {"debug", LogLevel::Debug},
        {"info", LogLevel::Info},
        {"warning", LogLevel::Warning},
        {"error", LogLevel::Error},
        {"critical", LogLevel::Critical},
        {"off", LogLevel::Off}
    };
    
    std::string lowerLevel = toLower(level);
    auto it = levelMap.find(lowerLevel);
    return (it != levelMap.end()) ? it->second : LogLevel::Info;
}

std::vector<std::string> CLIParser::getAvailableCommands() {
    return {"demo", "benchmark", "script", "test", "validate", "help", "version"};
}

std::vector<std::string> CLIParser::getAvailableDemoTypes() {
    return {"phase3", "phase4", "phase6"};
}

std::vector<std::pair<size_t, size_t>> CLIParser::getDefaultBenchmarkConfigs() {
    return {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200}
    };
}

// CLIFactory implementation
std::unique_ptr<CLIParser> CLIFactory::createParser() {
    return std::make_unique<CLIParser>();
}

std::shared_ptr<Config> CLIFactory::createConfig(const CLIConfig& cliConfig) {
    auto config = std::make_shared<Config>();
    config->setCriticalDefaults();
    return config;
}

std::shared_ptr<Logger> CLIFactory::createLogger(const CLIConfig& cliConfig) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    return logger;
}

int CLIFactory::executeCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
    if (config.help) {
        std::unique_ptr<CLIParser> parser = createParser();
        parser->printHelp();
        return 0;
    }
    
    if (config.version) {
        std::unique_ptr<CLIParser> parser = createParser();
        parser->printVersion();
        return 0;
    }
    
    if (config.command == "demo") {
        return executeDemoCommand(config, nlmConfig);
    } else if (config.command == "benchmark") {
        return executeBenchmarkCommand(config, nlmConfig);
    } else if (config.command == "script") {
        return executeScriptCommand(config, nlmConfig);
    } else if (config.command == "test") {
        return executeTestCommand(config, nlmConfig);
    } else if (config.command == "validate") {
        return executeValidateCommand(config, nlmConfig);
    }
    
    return 1;
}

std::shared_ptr<Config> CLIFactory::loadConfiguration(const std::string& configFile, const CLIConfig& cliConfig) {
    auto config = std::make_shared<Config>();
    
    if (!configFile.empty()) {
        try {
            if (config->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            } else {
                std::cout << "Warning: Could not load configuration from " << configFile << ". Using defaults." << std::endl;
            }
        } catch (const ConfigError& e) {
            std::cerr << "Error loading configuration: " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // Set CLI overrides if any
    // This would be populated from command line parsing
    
    config->setCriticalDefaults();
    return config;
}

void CLIFactory::setupLogging(const CLIConfig& config) {
    if (!config.log_level.empty()) {
        auto level = CLIParser::stringToLogLevel(config.log_level);
        Logger::getInstance().setLevel(level);
    }
    
    if (!config.log_file.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(config.log_file);
        Logger::getInstance().addLogger(fileLogger);
    }
}

void CLIFactory::printConfigurationSummary(const std::shared_ptr<Config>& config) {
    if (!config) return;
    
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "  Random seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  Neuron count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
    std::cout << "  Region count: " << config->getOr<int64_t>("region_count", 2) << std::endl;
    std::cout << "  Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
}

bool CLIFactory::saveConfiguration(const std::shared_ptr<Config>& config, const std::string& outputFile) {
    if (!config) return false;
    
    try {
        ConfigFormat format = CLIParser::detectConfigFormat(outputFile);
        return config->saveToFile(outputFile, format);
    } catch (const std::exception& e) {
        std::cerr << "Error saving configuration: " << e.what() << std::endl;
        return false;
    }
}

// Command execution implementations
int CLIFactory::executeDemoCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
    std::cout << "Running demo: " << config.demo_type << std::endl;
    std::cout << "Episodes: " << config.demo_episodes << std::endl;
    std::cout << "Steps per episode: " << config.demo_steps << std::endl;
    
    if (config.demo_type == "phase3") {
        std::cout << "Phase 3 demo would run here..." << std::endl;
        return 0;
    } else if (config.demo_type == "phase4") {
        std::cout << "Phase 4 demo would run here..." << std::endl;
        return 0;
    } else if (config.demo_type == "phase6") {
        std::cout << "Phase 6 demo would run here..." << std::endl;
        return 0;
    }
    
    return 1;
}

int CLIFactory::executeBenchmarkCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
    std::cout << "Running benchmark mode..." << std::endl;
    std::cout << "Benchmark configurations: " << config.benchmark_configs.size() << std::endl;
    std::cout << "Detailed mode: " << (config.benchmark_detailed ? "Yes" : "No") << std::endl;
    std::cout << "Output file: " << (config.benchmark_output.empty() ? "Not specified" : config.benchmark_output) << std::endl;
    
    if (config.benchmark_detailed) {
        std::cout << "Detailed benchmark analysis enabled." << std::endl;
    }
    
    return 0;
}

int CLIFactory::executeScriptCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
    std::cout << "Running script mode..." << std::endl;
    std::cout << "Script file: " << (config.script_file.empty() ? "Not specified" : config.script_file) << std::endl;
    std::cout << "Continue from checkpoint: " << (config.script_continue ? "Yes" : "No") << std::endl;
    std::cout << "Script mode: " << config.script_mode << std::endl;
    std::cout << "Script verbose: " << (config.script_verbose ? "Yes" : "No") << std::endl;
    
    if (!config.script_output_format.empty()) {
        std::cout << "Output format: " << config.script_output_format << std::endl;
    }
    if (!config.script_output_file.empty()) {
        std::cout << "Output file: " << config.script_output_file << std::endl;
    }
    
    if (!config.script_file.empty()) {
        // Use the new script processing system
        nlm::ScriptProcessor scriptProcessor;
        nlm::ScriptOptions scriptOptions;
        
        // Convert CLI config to script options
        scriptOptions.scriptFile = config.script_file;
        scriptOptions.verbose = config.script_verbose;
        scriptOptions.outputFormat = config.script_output_format;
        scriptOptions.outputFile = config.script_output_file;
        scriptOptions.continueFromCheckpoint = config.script_continue;
        
        // Set mode based on command line flags
        if (config.script_mode == "sweep") {
            scriptOptions.sweepMode = true;
            scriptOptions.sweepParameters = config.sweep_parameters;
        } else if (config.script_mode == "grid") {
            scriptOptions.gridSearchMode = true;
            scriptOptions.gridSearch = config.grid_parameters;
        } else if (config.script_mode == "sequential") {
            // For sequential mode, add script files from command args
            // For now, just use the main script file
            scriptOptions.scriptFile = config.script_file;
        } else {
            // Default mode
            scriptOptions.scriptFile = config.script_file;
        }
        
        // Execute script
        nlm::ScriptExecutionResult result = scriptProcessor.execute(scriptOptions);
        
        if (result.success) {
            std::cout << "Script execution completed successfully!" << std::endl;
            std::cout << "Experiments run: " << result.experimentsRun << std::endl;
            std::cout << "Execution time: " << result.executionTime << " seconds" << std::endl;
            return 0;
        } else {
            std::cout << "Script execution failed: " << result.errorMessage << std::endl;
            return 1;
        }
    } else {
        std::cout << "Error: No script file specified" << std::endl;
        return 1;
    }
}

int CLIFactory::executeTestCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
    std::cout << "Running test mode..." << std::endl;
    std::cout << "Validate configurations: " << (config.validate_configs ? "Yes" : "No") << std::endl;
    std::cout << "Validate format: " << (config.validate_format ? "Yes" : "No") << std::endl;
    
    return 0;
}

int CLIFactory::executeValidateCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
    std::cout << "Validating configuration..." << std::endl;
    
    if (config.config_file.empty()) {
        std::cout << "No configuration file specified for validation." << std::endl;
        return 1;
    }
    
    // Validate configuration
    auto configPtr = loadConfiguration(config.config_file, config);
    if (configPtr && configPtr->validate()) {
        std::cout << "Configuration validation passed." << std::endl;
        return 0;
    } else {
        std::cout << "Configuration validation failed." << std::endl;
        return 1;
    }
}

// Helper function implementations
bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string extractValue(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

void parseBenchmarkConfig(const std::string& value, std::vector<std::pair<size_t, size_t>>& configs) {
    std::stringstream ss(value);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        std::stringstream itemStream(item);
        size_t neurons, steps;
        char comma;
        if (itemStream >> neurons >> comma >> steps) {
            configs.push_back({neurons, steps});
        }
    }
void parseBenchmarkConfig(const std::string& value, std::vector<std::pair<size_t, size_t>>& configs) {
    std::stringstream ss(value);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        std::stringstream itemStream(item);
        size_t neurons, steps;
        char comma;
        if (itemStream >> neurons >> comma >> steps) {
            configs.push_back({neurons, steps});
        }
    }
}

void parseSweepParameter(const std::string& value, std::vector<std::pair<std::string, std::string>>& parameters) {
    // Parse sweep parameter like "neuron_count 500,1000,2000"
    std::istringstream iss(value);
    std::string paramName;
    
    if (std::getline(iss, paramName, ' ')) {
        std::string paramValues;
        if (std::getline(iss, paramValues)) {
            parameters.push_back({paramName, paramValues});
        }
    }
}

void parseGridSearch(const std::string& value, std::vector<std::pair<std::string, std::string>>& parameters) {
    // Parse grid search parameter like "neuron_count 1000,2000"
    // Similar to parseSweepParameter for now
    parseSweepParameter(value, parameters);
}

std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

// CLIAdapter implementation
std::unique_ptr<CLIParser> CLIAdapter::parser = createParser();
std::unique_ptr<CLIFactory> CLIAdapter::factory = std::make_unique<CLIFactory>();

int CLIAdapter::adaptPhase3Demo(int argc, char* argv[]) {
    return adaptMain(argc, argv);
}

int CLIAdapter::adaptPhase4Demo(int argc, char* argv[]) {
    return adaptMain(argc, argv);
}

int CLIAdapter::adaptPhase6Demo(int argc, char* argv[]) {
    return adaptMain(argc, argv);
}

int CLIAdapter::adaptBenchmark(int argc, char* argv[]) {
    return adaptMain(argc, argv);
}

int CLIAdapter::adaptMain(int argc, char* argv[]) {
    if (shouldShowHelpOrVersion(argc, argv)) {
        parser->printHelp();
        return 0;
    }
    
    CLIConfig config;
    if (!parser->parse(argc, argv, config)) {
        std::cerr << "Failed to parse command line arguments." << std::endl;
        return 1;
    }
    
    if (!parser->validate(config)) {
        return 1;
    }
    
    // Create configuration
    auto nlmConfig = factory->createConfig(config);
    
    // Setup logging
    factory->setupLogging(config);
    
    // Load configuration from file
    if (!config.config_file.empty()) {
        auto loadedConfig = factory->loadConfiguration(config.config_file, config);
        if (loadedConfig) {
            nlmConfig = loadedConfig;
        }
    }
    
    // Execute command
    return factory->executeCommand(config, nlmConfig);
}

bool CLIAdapter::shouldShowHelpOrVersion(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "--version") {
            return true;
        }
    }
    return false;
}

} // namespace nlm