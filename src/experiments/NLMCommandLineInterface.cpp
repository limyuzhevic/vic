namespace nlm {
namespace Phase2 {

// Static data initialization
const std::map<std::string, ExperimentMode> NLMCommandLineInterface::EXPERIMENT_MAP = {
    {"test1", ExperimentMode::BASIC_CONNECTIVITY},
    {"test2", ExperimentMode::PLASTICITY_LEARNING},
    {"test3", ExperimentMode::STDP_VERIFICATION},
    {"all", ExperimentMode::ALL_TESTS},
    {"benchmark", ExperimentMode::BENCHMARK}
};

const std::map<ExperimentMode, std::string> NLMCommandLineInterface::EXPERIMENT_NAMES = {
    {ExperimentMode::BASIC_CONNECTIVITY, "basic_connectivity"},
    {ExperimentMode::PLASTICITY_LEARNING, "plasticity_learning"},
    {ExperimentMode::STDP_VERIFICATION, "stdp_verification"},
    {ExperimentMode::ALL_TESTS, "all_tests"},
    {ExperimentMode::BENCHMARK, "benchmark"}
};

const std::map<ExperimentMode, NLMCommandLineInterface::ExperimentInfo> NLMCommandLineInterface::EXPERIMENT_INFO = {
    {ExperimentMode::BASIC_CONNECTIVITY, {
        ExperimentMode::BASIC_CONNECTIVITY,
        "Basic Connectivity Test",
        "Tests if neural activity can propagate through the network",
        {"neuron_count", "region_count", "connection_probability"},
        {"simulation_timestep", "random_seed"},
        true,
        nullptr
    }},
    {ExperimentMode::PLASTICITY_LEARNING, {
        ExperimentMode::PLASTICITY_LEARNING,
        "Plasticity Learning Experiment",
        "Tests if synapses can strengthen and weaken through experience",
        {"neuron_count", "stdp_ltp_weight", "stdp_ltd_weight"},
        {"simulation_timestep", "random_seed"},
        true,
        nullptr
    }},
    {ExperimentMode::STDP_VERIFICATION, {
        ExperimentMode::STDP_VERIFICATION,
        "STDP Verification Test",
        "Tests spike-timing-dependent plasticity specifically",
        {"neuron_count", "stdp_ltp_weight", "stdp_ltd_weight"},
        {"simulation_timestep", "random_seed"},
        true,
        nullptr
    }},
    {ExperimentMode::ALL_TESTS, {
        ExperimentMode::ALL_TESTS,
        "All Tests",
        "Run all available experiments",
        {},
        {},
        true,
        nullptr
    }},
    {ExperimentMode::BENCHMARK, {
        ExperimentMode::BENCHMARK,
        "Benchmark Test",
        "Performance benchmark of the neural system",
        {"neuron_count", "simulation_timestep"},
        {"max_steps", "parallel_enabled"},
        false,
        nullptr
    }}
};

const std::vector<std::string> NLMCommandLineInterface::AVAILABLE_EXPERIMENTS = {
    "test1", "test2", "test3", "all", "benchmark"
};

const std::vector<std::string> NLMCommandLineInterface::COMMON_CONFIG_FILES = {
    "configs/default.cfg",
    "configs/phase2.cfg",
    "configs/experiment.cfg"
};

const std::string NLMCommandLineInterface::DEFAULT_RESULTS_FILE = "nlm_results.txt";
const std::string NLMCommandLineInterface::DEFAULT_LOG_FILE = "nlm.log";
const std::string NLMCommandLineInterface::DEFAULT_TEMP_DIR = "/tmp";

bool NLMCommandLineInterface::parseCommandLine(int argc, char** argv, CommandLineArgs& args) {
    std::vector<std::string> argStrings;
    for (int i = 0; i < argc; ++i) {
        argStrings.push_back(argv[i]);
    }
    
    return parseCommandLine(argStrings, args);
}

bool NLMCommandLineInterface::parseCommandLine(const std::string& commandLine, CommandLineArgs& args) {
    // Parse command line string into arguments
    std::vector<std::string> argStrings;
    std::string token;
    std::istringstream tokenStream(commandLine);
    
    while (tokenStream >> token) {
        argStrings.push_back(token);
    }
    
    return parseCommandLine(argStrings, args);
}

bool NLMCommandLineInterface::parseCommandLine(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    // Clear previous parsing results
    parsedArgs = CommandLineArgs();
    
    // Parse positional arguments
    if (!parsePositionalArgs(args, parsedArgs)) {
        return false;
    }
    
    // Parse option arguments
    if (!parseOptionArgs(args, parsedArgs)) {
        return false;
    }
    
    // Parse experiment arguments
    if (!parseExperimentArgs(args, parsedArgs)) {
        return false;
    }
    
    // Parse configuration arguments
    if (!parseConfigArgs(args, parsedArgs)) {
        return false;
    }
    
    // Parse output arguments
    if (!parseOutputArgs(args, parsedArgs)) {
        return false;
    }
    
    // Parse performance arguments
    if (!parsePerformanceArgs(args, parsedArgs)) {
        return false;
    }
    
    // Parse debug arguments
    if (!parseDebugArgs(args, parsedArgs)) {
        return false;
    }
    
    // Validate arguments
    if (!validateArgs(parsedArgs)) {
        return false;
    }
    
    return true;
}

bool NLMCommandLineInterface::parsePositionalArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            parsedArgs.help = true;
        } else if (arg == "--version") {
            parsedArgs.version = true;
        } else if (arg.find("--") == 0 && i + 1 < args.size() && args[i + 1] != "--") {
            // Long option with value
            std::string option = arg.substr(2);
            std::string value = args[++i];
            parsedArgs.configOverrides[option] = value;
        } else if (arg.find("-") == 0 && i + 1 < args.size() && args[i + 1] != "--") {
            // Short option with value
            std::string option = arg.substr(1);
            std::string value = args[++i];
            parsedArgs.configOverrides[option] = value;
        } else if (arg.find("--") == 0) {
            // Flag option
            std::string option = arg.substr(2);
            if (option == "verbose") {
                parsedArgs.verbose = true;
            } else if (option == "quiet") {
                parsedArgs.quiet = true;
            } else if (option == "benchmark") {
                parsedArgs.benchmarkMode = true;
            } else if (option == "dry-run") {
                parsedArgs.dryRun = true;
            } else if (option == "validate-only") {
                parsedArgs.validateOnly = true;
            } else if (option == "save-config") {
                parsedArgs.saveConfig = true;
            } else if (option == "enable-monitoring") {
                parsedArgs.enableMonitoring = true;
            } else if (option == "enable-profiling") {
                parsedArgs.enableProfiling = true;
            } else if (option == "debug-mode") {
                parsedArgs.debugMode = true;
            } else if (option == "trace-mode") {
                parsedArgs.traceMode = true;
            } else if (option == "enable-plugins") {
                parsedArgs.enablePlugins = true;
            } else {
                printError("Unknown option: " + arg);
                return false;
            }
        } else {
            // Positional argument (experiment name)
            parsedArgs.experimentName = arg;
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::parseOptionArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            parsedArgs.help = true;
        } else if (arg == "--version") {
            parsedArgs.version = true;
        } else if (arg.find("--config=") == 0) {
            parsedArgs.configFile = arg.substr(8);
        } else if (arg == "--config" && i + 1 < args.size()) {
            parsedArgs.configFile = args[++i];
        } else if (arg.find("--output=") == 0) {
            parsedArgs.outputFile = arg.substr(9);
        } else if (arg.find("--results=") == 0) {
            parsedArgs.resultsFile = arg.substr(11);
        } else if (arg.find("--timestep=") == 0) {
            parsedArgs.timestep = std::stod(arg.substr(11));
        } else if (arg.find("--neurons=") == 0) {
            parsedArgs.neuronCount = std::stoll(arg.substr(10));
        } else if (arg.find("--regions=") == 0) {
            parsedArgs.regionCount = std::stoi(arg.substr(10));
        } else if (arg.find("--connection=") == 0) {
            parsedArgs.connectionProbability = std::stof(arg.substr(13));
        } else if (arg.find("--episodes=") == 0) {
            parsedArgs.numEpisodes = std::stoi(arg.substr(11));
        } else if (arg.find("--steps=") == 0) {
            parsedArgs.maxSteps = std::stoi(arg.substr(7));
        } else if (arg.find("--monitoring=") == 0) {
            parsedArgs.monitoringInterval = std::stod(arg.substr(13));
        } else if (arg.find("--monitoring-file=") == 0) {
            parsedArgs.monitoringFile = arg.substr(18);
        } else if (arg.find("--result-format=") == 0) {
            parsedArgs.resultFormat = arg.substr(16);
        } else if (arg.find("--log-level=") == 0) {
            parsedArgs.logLevel = arg.substr(12);
        } else if (arg.find("--log-file=") == 0) {
            parsedArgs.logFile = arg.substr(12);
        } else if (arg.find("--temp-dir=") == 0) {
            parsedArgs.tempDirectory = arg.substr(11);
        } else if (arg.find("--cpu-cores=") == 0) {
            parsedArgs.cpuCores = std::stoi(arg.substr(13));
        } else if (arg.find("--memory-limit=") == 0) {
            parsedArgs.memoryLimitMB = std::stoul(arg.substr(16));
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::parseExperimentArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    // Parse experiment selection
    std::vector<ExperimentMode> experiments;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.find("--experiment=") == 0) {
            std::string experimentList = arg.substr(14);
            experiments = parseExperimentList(experimentList);
        } else if (arg.find("--experiment-list=") == 0) {
            std::string experimentList = arg.substr(18);
            experiments = parseExperimentList(experimentList);
        } else if (arg.find("--experiment=") == 0) {
            std::string experimentName = arg.substr(14);
            ExperimentMode mode = getExperimentMode(experimentName);
            if (mode != ExperimentMode::ALL_TESTS) {
                experiments.push_back(mode);
            }
        } else if (arg.find("--experiments=") == 0) {
            std::string experimentList = arg.substr(14);
            experiments = parseExperimentList(experimentList);
        }
    }
    
    if (!experiments.empty()) {
        parsedArgs.experiments = experiments;
        parsedArgs.runAllExperiments = (experiments.size() == 1 && experiments[0] == ExperimentMode::ALL_TESTS);
        parsedArgs.primaryExperiment = experiments[0];
    }
    
    return true;
}

bool NLMCommandLineInterface::parseConfigArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    // Parse configuration overrides
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.find("--set=") == 0) {
            std::string configEntry = arg.substr(6);
            size_t pos = configEntry.find('=');
            if (pos != std::string::npos) {
                std::string key = configEntry.substr(0, pos);
                std::string value = configEntry.substr(pos + 1);
                parsedArgs.configOverrides[key] = value;
            }
        } else if (arg.find("--experiment-config=") == 0) {
            std::string experimentConfig = arg.substr(20);
            size_t pos = experimentConfig.find('=');
            if (pos != std::string::npos) {
                std::string experimentName = experimentConfig.substr(0, pos);
                std::string configFile = experimentConfig.substr(pos + 1);
                parsedArgs.experimentConfigs[experimentName] = configFile;
            }
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::parseOutputArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    // Parse output options
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.find("--output-format=") == 0) {
            parsedArgs.resultFormat = arg.substr(17);
        } else if (arg == "--show-progress") {
            parsedArgs.showProgress = true;
        } else if (arg == "--show-detailed") {
            parsedArgs.showDetailedOutput = true;
        } else if (arg == "--save-results") {
            parsedArgs.saveResults = true;
        } else if (arg.find("--results-file=") == 0) {
            parsedArgs.resultsFile = arg.substr(16);
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::parsePerformanceArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    // Parse performance options
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--enable-monitoring") {
            parsedArgs.enableMonitoring = true;
        } else if (arg.find("--monitoring-interval=") == 0) {
            parsedArgs.monitoringInterval = std::stod(arg.substr(22));
        } else if (arg.find("--monitoring-file=") == 0) {
            parsedArgs.monitoringFile = arg.substr(18);
        } else if (arg == "--enable-profiling") {
            parsedArgs.enableProfiling = true;
        } else if (arg.find("--cpu-cores=") == 0) {
            parsedArgs.cpuCores = std::stoi(arg.substr(12));
        } else if (arg.find("--memory-limit=") == 0) {
            parsedArgs.memoryLimitMB = std::stoul(arg.substr(16));
        } else if (arg == "--optimize-performance") {
            // Will be handled in optimization phase
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::parseDebugArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs) {
    // Parse debug options
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--debug-mode") {
            parsedArgs.debugMode = true;
        } else if (arg == "--trace-mode") {
            parsedArgs.traceMode = true;
        } else if (arg.find("--log-level=") == 0) {
            parsedArgs.logLevel = arg.substr(12);
        } else if (arg.find("--log-file=") == 0) {
            parsedArgs.logFile = arg.substr(12);
        } else if (arg.find("--temp-dir=") == 0) {
            parsedArgs.tempDirectory = arg.substr(11);
        } else if (arg == "--capture-logs") {
            parsedArgs.captureLogs = true;
        } else if (arg == "--continue-on-error") {
            parsedArgs.continueOnError = true;
        } else if (arg.find("--retry-count=") == 0) {
            parsedArgs.retryCount = std::stoi(arg.substr(15));
        } else if (arg.find("--timeout=") == 0) {
            parsedArgs.timeout = std::stod(arg.substr(10));
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::validateArgs(const CommandLineArgs& args) const {
    std::string validationErrors = getValidationErrors(args);
    if (!validationErrors.empty()) {
        printError("Argument validation failed:\n" + validationErrors);
        return false;
    }
    
    return true;
}

bool NLMCommandLineInterface::validateExperimentArgs(const CommandLineArgs& args) const {
    if (args.runAllExperiments && !args.experiments.empty() && args.experiments.size() > 1) {
        printWarning("Multiple experiments specified with --run-all flag - using --run-all mode");
    }
    
    for (const auto& mode : args.experiments) {
        if (!validateExperimentPrerequisites(mode, Config())) {
            return false;
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::validateConfigArgs(const CommandLineArgs& args) const {
    if (!args.configFile.empty() && !fileExists(args.configFile)) {
        printError("Configuration file not found: " + args.configFile);
        return false;
    }
    
    for (const auto& experimentConfig : args.experimentConfigs) {
        if (!fileExists(experimentConfig.second)) {
            printError("Experiment configuration file not found: " + experimentConfig.second);
            return false;
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::validateOutputArgs(const CommandLineArgs& args) const {
    if (!args.outputFile.empty() && !args.outputFile.empty()) {
        printError("Output file path is empty");
        return false;
    }
    
    if (!args.resultsFile.empty() && !args.resultsFile.empty()) {
        printError("Results file path is empty");
        return false;
    }
    
    if (args.resultFormat != "text" && args.resultFormat != "json" && args.resultFormat != "csv") {
        printError("Unsupported result format: " + args.resultFormat);
        return false;
    }
    
    return true;
}

bool NLMCommandLineInterface::validatePerformanceArgs(const CommandLineArgs& args) const {
    if (args.timestep > 0 && !Constants::isValidTimestep(args.timestep)) {
        printError("Invalid timestep: " + std::to_string(args.timestep));
        return false;
    }
    
    if (args.neuronCount > 0 && !Constants::isValidNeuronCount(args.neuronCount)) {
        printError("Invalid neuron count: " + std::to_string(args.neuronCount));
        return false;
    }
    
    if (args.regionCount > 0 && !Constants::isValidRegionCount(args.regionCount)) {
        printError("Invalid region count: " + std::to_string(args.regionCount));
        return false;
    }
    
    if (args.connectionProbability > 0 && !Constants::isValidConnectionProbability(args.connectionProbability)) {
        printError("Invalid connection probability: " + std::to_string(args.connectionProbability));
        return false;
    }
    
    return true;
}

bool NLMCommandLineInterface::validateDebugArgs(const CommandLineArgs& args) const {
    if (args.logLevel != "debug" && args.logLevel != "info" && args.logLevel != "warning" && args.logLevel != "error") {
        printError("Invalid log level: " + args.logLevel);
        return false;
    }
    
    if (args.memoryLimitMB > 0 && args.memoryLimitMB < 100) {
        printError("Memory limit too low: " + std::to_string(args.memoryLimitMB) + "MB");
        return false;
    }
    
    if (args.timeout > 0 && (args.timeout < 10.0 || args.timeout > 3600.0)) {
        printError("Timeout out of range: " + std::to_string(args.timeout) + "s");
        return false;
    }
    
    return true;
}

std::string NLMCommandLineInterface::getValidationErrors(const CommandLineArgs& args) const {
    std::ostringstream oss;
    
    // Validate experiment arguments
    if (!validateExperimentArgs(args)) {
        oss << "Experiment argument validation failed.\n";
    }
    
    // Validate configuration arguments
    if (!validateConfigArgs(args)) {
        oss << "Configuration argument validation failed.\n";
    }
    
    // Validate output arguments
    if (!validateOutputArgs(args)) {
        oss << "Output argument validation failed.\n";
    }
    
    // Validate performance arguments
    if (!validatePerformanceArgs(args)) {
        oss << "Performance argument validation failed.\n";
    }
    
    // Validate debug arguments
    if (!validateDebugArgs(args)) {
        oss << "Debug argument validation failed.\n";
    }
    
    return oss.str();
}

bool NLMCommandLineInterface::loadConfigFile(Config& config, const std::string& configFile) const {
    if (configFile.empty()) {
        return true; // No config file specified
    }
    
    if (!fileExists(configFile)) {
        printError("Configuration file not found: " + configFile);
        return false;
    }
    
    return config.loadFromFile(configFile);
}

bool NLMCommandLineInterface::applyConfigOverrides(Config& config, const std::map<std::string, std::string>& overrides) const {
    for (const auto& override : overrides) {
        const std::string& key = override.first;
        const std::string& value = override.second;
        
        // Try to parse value based on key type hints
        bool success = false;
        
        // Integer
        try {
            int64_t intValue = std::stoll(value);
            config.set(key, intValue, ConfigSource::CommandLine);
            success = true;
        } catch (...) {}
        
        // Double
        if (!success) {
            try {
                double doubleValue = std::stod(value);
                config.set(key, doubleValue, ConfigSource::CommandLine);
                success = true;
            } catch (...) {}
        }
        
        // Boolean
        if (!success) {
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                config.set(key, boolValue, ConfigSource::CommandLine);
                success = true;
            }
        }
        
        // String
        if (!success) {
            config.set(key, value, ConfigSource::CommandLine);
            success = true;
        }
        
        if (!success) {
            printWarning("Failed to apply config override for key: " + key + " with value: " + value);
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::applyExperimentConfig(Config& config, const std::string& experimentName) const {
    std::string configFile = getConfigFilePath(experimentName);
    if (configFile.empty()) {
        return true; // No experiment-specific config found
    }
    
    Config experimentConfig;
    if (loadConfigFile(experimentConfig, configFile)) {
        mergeConfigs(config, experimentConfig);
        return true;
    }
    
    return false;
}

std::vector<ExperimentMode> NLMCommandLineInterface::parseExperimentList(const std::string& experimentList) const {
    std::vector<ExperimentMode> experiments;
    std::stringstream ss(experimentList);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        token = trim(token);
        ExperimentMode mode = getExperimentMode(token);
        if (mode != ExperimentMode::ALL_TESTS) {
            experiments.push_back(mode);
        }
    }
    
    return experiments;
}

ExperimentMode NLMCommandLineInterface::getExperimentMode(const std::string& experimentName) const {
    auto it = EXPERIMENT_MAP.find(experimentName);
    if (it != EXPERIMENT_MAP.end()) {
        return it->second;
    }
    
    // Try case-insensitive lookup
    for (const auto& pair : EXPERIMENT_MAP) {
        if (toLower(experimentName) == toLower(pair.first)) {
            return pair.second;
        }
    }
    
    printWarning("Unknown experiment name: " + experimentName + ". Using default.");
    return ExperimentMode::ALL_TESTS;
}

std::string NLMCommandLineInterface::getExperimentName(ExperimentMode mode) const {
    auto it = EXPERIMENT_NAMES.find(mode);
    if (it != EXPERIMENT_NAMES.end()) {
        return it->second;
    }
    
    // Fallback for unknown mode
    for (const auto& pair : EXPERIMENT_NAMES) {
        if (pair.first == ExperimentMode::ALL_TESTS) {
            return pair.second;
        }
    }
    
    return "unknown";
}

bool NLMCommandLineInterface::runExperiments(const CommandLineArgs& args) {
    // Create base configuration
    Config config;
    
    // Load main configuration file if specified
    if (!args.configFile.empty()) {
        if (!loadConfigFile(config, args.configFile)) {
            return false;
        }
    }
    
    // Apply experiment-specific configuration
    if (!args.experimentName.empty()) {
        applyExperimentConfig(config, args.experimentName);
    }
    
    // Apply command-line overrides
    applyConfigOverrides(config, args.configOverrides);
    
    // Override with command-line specific parameters
    if (args.timestep > 0) {
        config.set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    }
    
    if (args.neuronCount > 0) {
        config.set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
    }
    
    if (args.regionCount > 0) {
        config.set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::CommandLine);
    }
    
    if (args.connectionProbability > 0) {
        config.set("connection_probability", args.connectionProbability, ConfigSource::CommandLine);
    }
    
    // Optimize performance if requested
    if (args.enableMonitoring || args.enableProfiling) {
        optimizePerformance(config, args);
    }
    
    // Validate configuration
    if (args.validateOnly) {
        return validateConfigSchema(config, args.primaryExperiment);
    }
    
    // Run experiments
    std::vector<ExperimentResult> results;
    
    if (args.benchmarkMode) {
        return executeBenchmark(config, args);
    } else if (args.runAllExperiments || args.experiments.empty()) {
        return executeAllExperiments(config, args);
    } else {
        bool allSuccess = true;
        for (const auto& mode : args.experiments) {
            bool success = executeExperiment(mode, config, args);
            if (!success && !args.continueOnError) {
                return false;
            }
            allSuccess = allSuccess && success;
        }
        return allSuccess;
    }
}

void NLMCommandLineInterface::printHelp(const UsageInfo& usage) const {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "\n";
    
    if (!usage.description.empty()) {
        std::cout << usage.description << "\n\n";
    }
    
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << usage.programName << " [OPTIONS] [EXPERIMENT_NAME]" << std::endl;
    std::cout << "\n";
    
    // Print experiment options
    printExperimentList();
    std::cout << "\n";
    
    // Print configuration options
    std::cout << "Configuration Options:" << std::endl;
    for (const auto& option : getConfigOptions()) {
        std::cout << "  --" << option.first << " " << option.second << std::endl;
    }
    std::cout << "\n";
    
    // Print output options
    std::cout << "Output Options:" << std::endl;
    for (const auto& option : getOutputOptions()) {
        std::cout << "  --" << option.first << " " << option.second << std::endl;
    }
    std::cout << "\n";
    
    // Print performance options
    std::cout << "Performance Options:" << std::endl;
    for (const auto& option : getPerformanceOptions()) {
        std::cout << "  --" << option.first << " " << option.second << std::endl;
    }
    std::cout << "\n";
    
    // Print debug options
    std::cout << "Debug Options:" << std::endl;
    for (const auto& option : getDebugOptions()) {
        std::cout << "  --" << option.first << " " << option.second << std::endl;
    }
    std::cout << "\n";
    
    // Print common examples
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << usage.programName << " run                  # Run all experiments" << std::endl;
    std::cout << "  " << usage.programName << " test1                   # Run basic connectivity test" << std::endl;
    std::cout << "  " << usage.programName << " test2                    # Run plasticity learning experiment" << std::endl;
    std::cout << "  " << usage.programName << " test3                    # Run STDP verification test" << std::endl;
    std::cout << "  " << usage.programName << " benchmark                 # Run performance benchmark" << std::endl;
    std::cout << "  " << usage.programName << " test1,test2,test3 --verbose # Run multiple experiments" << std::endl;
    std::cout << "\n";
    
    std::cout << std::string(80, '=') << std::endl;
}

void NLMCommandLineInterface::printVersion(const UsageInfo& usage) const {
    std::cout << usage.programName << " " << usage.version << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Description: " << usage.description << std::endl;
}

void NLMCommandLineInterface::printExperimentList() const {
    std::cout << "Experiment Options:" << std::endl;
    
    for (const auto& mode : EXPERIMENT_INFO) {
        std::cout << "  " << getExperimentName(mode.first) << ": " << mode.second.name << std::endl;
        std::cout << "    " << mode.second.description << std::endl;
        if (!mode.second.requiredConfigKeys.empty()) {
            std::cout << "    Required config keys: ";
            for (size_t i = 0; i < mode.second.requiredConfigKeys.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << mode.second.requiredConfigKeys[i];
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << "\n";
}

void NLMCommandLineInterface::printConfiguration(const Config& config) const {
    std::cout << "Current Configuration:" << std::endl;
    std::cout << config.summary() << std::endl;
}

void NLMCommandLineInterface::printResults(const std::vector<ExperimentResult>& results) const {
    std::cout << "\nExperiment Results:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    for (const auto& result : results) {
        std::cout << getStatusSymbol(result.success) << " " << getModeName(result.mode) << ": ";
        std::cout << result.message << " (" << result.getDurationString() << ")" << std::endl;
        
        if (result.success && showDetailedOutput) {
            std::cout << "  Metrics: " << result.metrics.spikesGenerated << " spikes, "
                      << std::fixed << std::setprecision(3) << result.metrics.meanWeightChange << " mean weight change" << std::endl;
        }
    }
    
    std::cout << std::string(60, '=') << std::endl;
}

void NLMCommandLineInterface::printPerformanceReport(const std::vector<ExperimentResult>& results) const {
    std::cout << "\nPerformance Report:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    double totalTime = 0.0;
    size_t totalSpikes = 0;
    
    for (const auto& result : results) {
        totalTime += result.executionTime;
        totalSpikes += result.metrics.spikesGenerated;
    }
    
    std::cout << "Total experiments: " << results.size() << std::endl;
    std::cout << "Total execution time: " << formatTime(totalTime) << std::endl;
    std::cout << "Total spikes generated: " << totalSpikes << std::endl;
    
    if (!results.empty()) {
        double avgTime = totalTime / results.size();
        std::cout << "Average execution time: " << formatTime(avgTime) << std::endl;
        double avgSpikes = static_cast<double>(totalSpikes) / results.size();
        std::cout << "Average spikes per experiment: " << avgSpikes << std::endl;
    }
    
    std::cout << std::string(60, '=') << std::endl;
}

void NLMCommandLineInterface::printError(const std::string& message) const {
    std::cerr << "ERROR: " << message << std::endl;
}

void NLMCommandLineInterface::printWarning(const std::string& message) const {
    std::cerr << "WARNING: " << message << std::endl;
}

void NLMCommandLineInterface::printInfo(const std::string& message) const {
    std::cout << "INFO: " << message << std::endl;
}

bool NLMCommandLineInterface::validateExperimentPrerequisites(ExperimentMode mode, const Config& config) const {
    const auto& experimentInfo = EXPERIMENT_INFO.find(mode);
    if (experimentInfo == EXPERIMENT_INFO.end()) {
        return false;
    }
    
    // Check required configuration keys
    for (const auto& key : experimentInfo->second.requiredConfigKeys) {
        if (!config.has(key)) {
            printWarning("Missing required configuration key: " + key + " for experiment " + getExperimentName(mode));
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::checkSystemResources(const CommandLineArgs& args) const {
    // Check if system resources are sufficient
    if (args.memoryLimitMB > 0) {
        // Memory check (simplified)
        if (args.memoryLimitMB < 512) {
            printWarning("Memory limit may be too low for large experiments: " + std::to_string(args.memoryLimitMB) + "MB");
        }
    }
    
    // Check CPU cores if specified
    if (args.cpuCores > 0) {
        // CPU check (simplified)
        if (args.cpuCores > 32) {
            printWarning("Large number of CPU cores may impact performance: " + std::to_string(args.cpuCores));
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::checkExperimentCompatibility(ExperimentMode mode, const Config& config) const {
    // Check if experiment is compatible with current configuration
    switch (mode) {
        case ExperimentMode::BASIC_CONNECTIVITY:
            if (config.getOr<int64_t>("neuron_count", 0) < 10) {
                printWarning("Basic connectivity test with small neuron count may not be meaningful");
            }
            break;
        case ExperimentMode::PLASTICITY_LEARNING:
            if (!config.has("stdp_ltp_weight") || !config.has("stdp_ltd_weight")) {
                printWarning("Plasticity learning experiment requires STDP parameters");
            }
            break;
        case ExperimentMode::STDP_VERIFICATION:
            if (!config.has("stdp_ltp_weight") || !config.has("stdp_ltd_weight")) {
                printWarning("STDP verification requires STDP parameters");
            }
            break;
        default:
            break;
    }
    
    return true;
}

bool NLMCommandLineInterface::loadDefaultConfig(Config& config) const {
    for (const auto& configFile : COMMON_CONFIG_FILES) {
        if (fileExists(configFile)) {
            if (config.loadFromFile(configFile)) {
                return true;
            }
        }
    }
    
    return false;
}

bool NLMCommandLineInterface::loadExperimentPreset(Config& config, const std::string& experimentName) const {
    std::string presetFile = getConfigFilePath(experimentName);
    if (presetFile.empty()) {
        return false;
    }
    
    Config presetConfig;
    if (presetConfig.loadFromFile(presetFile)) {
        mergeConfigs(config, presetConfig);
        return true;
    }
    
    return false;
}

bool NLMCommandLineInterface::mergeConfigs(Config& baseConfig, const Config& overrideConfig) const {
    for (const auto& key : overrideConfig.getKeys()) {
        baseConfig.set(key, overrideConfig.getOr(key, std::string("")));
    }
    
    return true;
}

bool NLMCommandLineInterface::validateConfigSchema(const Config& config, ExperimentMode mode) const {
    ExperimentValidator validator;
    auto validationResult = validator.validateConfig(config, mode);
    
    if (!validationResult.valid) {
        printError("Configuration validation failed:");
        for (const auto& error : validationResult.errors) {
            printError("  " + error);
        }
        return false;
    }
    
    if (!validationResult.warnings.empty()) {
        printInfo("Configuration validation warnings:");
        for (const auto& warning : validationResult.warnings) {
            printInfo("  " + warning);
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::optimizePerformance(Config& config, const CommandLineArgs& args) const {
    // Apply performance optimizations based on arguments
    if (args.enableMonitoring) {
        config.set("monitoring_enabled", true, ConfigSource::CommandLine);
        config.set("monitoring_interval", args.monitoringInterval, ConfigSource::CommandLine);
        if (!args.monitoringFile.empty()) {
            config.set("monitoring_file", args.monitoringFile, ConfigSource::CommandLine);
        }
    }
    
    if (args.enableProfiling) {
        config.set("profiling_enabled", true, ConfigSource::CommandLine);
    }
    
    if (args.cpuCores > 0) {
        config.set("cpu_cores", args.cpuCores, ConfigSource::CommandLine);
    }
    
    if (args.memoryLimitMB > 0) {
        config.set("memory_limit_mb", args.memoryLimitMB, ConfigSource::CommandLine);
    }
    
    return true;
}

bool NLMCommandLineInterface::enableParallelProcessing(Config& config, const CommandLineArgs& args) const {
    // Enable parallel processing if resources allow
    if (args.cpuCores > 0) {
        config.set("parallel_enabled", true, ConfigSource::CommandLine);
        config.set("cpu_cores", args.cpuCores, ConfigSource::CommandLine);
    }
    
    return true;
}

bool NLMCommandLineInterface::configureMemoryUsage(Config& config, const CommandLineArgs& args) const {
    // Configure memory usage based on arguments
    if (args.memoryLimitMB > 0) {
        config.set("memory_limit_mb", args.memoryLimitMB, ConfigSource::CommandLine);
        
        // Adjust other settings based on memory limit
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
        if (neuronCount > 0 && args.memoryLimitMB < 512) {
            // Reduce neuron count if memory is limited
            config.set("neuron_count", std::min(int64_t(100), neuronCount), ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool NLMCommandLineInterface::loadPlugins(const std::vector<std::string>& plugins, const std::vector<std::string>& args) {
    // Load plugins (placeholder implementation)
    return true;
}

bool NLMCommandLineInterface::executePlugins(const std::vector<std::string>& plugins, const std::vector<std::string>& args) {
    // Execute plugins (placeholder implementation)
    return true;
}

bool NLMCommandLineInterface::unloadPlugins() {
    // Unload plugins (placeholder implementation)
    return true;
}

void NLMCommandLineInterface::cleanup() const {
    // Cleanup resources (placeholder implementation)
}

std::string NLMCommandLineInterface::formatArgsHelp(const CommandLineArgs& args) const {
    return ""; // Placeholder
}

std::string NLMCommandLineInterface::formatExperimentHelp() const {
    return ""; // Placeholder
}

std::string NLMCommandLineInterface::formatConfigHelp() const {
    return ""; // Placeholder
}

std::string NLMCommandLineInterface::formatOutputHelp() const {
    return ""; // Placeholder
}

std::string NLMCommandLineInterface::formatPerformanceHelp() const {
    return ""; // Placeholder
}

std::string NLMCommandLineInterface::formatDebugHelp() const {
    return ""; // Placeholder
}

void NLMCommandLineInterface::updateStatistics(const CommandLineArgs& args, const std::vector<ExperimentResult>& results) {
    // Update statistics (placeholder implementation)
}

void NLMCommandLineInterface::generatePerformanceReport(const CommandLineArgs& args, const std::vector<ExperimentResult>& results) const {
    // Generate performance report (placeholder implementation)
}

void NLMCommandLineInterface::saveStatistics(const CommandLineArgs& args, const std::vector<ExperimentResult>& results) const {
    // Save statistics to file (placeholder implementation)
}

void NLMCommandLineInterface::setupLogging(const CommandLineArgs& args) const {
    // Setup logging (placeholder implementation)
}

void NLMCommandLineInterface::logCommandLineArgs(const CommandLineArgs& args) const {
    // Log command line arguments (placeholder implementation)
}

void NLMCommandLineInterface::logConfiguration(const Config& config) const {
    // Log configuration (placeholder implementation)
}

void NLMCommandLineInterface::logExperimentResults(const std::vector<ExperimentResult>& results) const {
    // Log experiment results (placeholder implementation)
}

bool NLMCommandLineInterface::executeExperiment(ExperimentMode mode, const Config& config, const CommandLineArgs& args) {
    ExperimentRunner runner;
    
    ExperimentResult result = runner.runExperiment(mode, config);
    
    // Handle errors with retry logic
    if (!result.success && args.retryCount > 0) {
        for (int attempt = 1; attempt <= args.retryCount; ++attempt) {
            NLM_LOG_INFO("Retrying experiment " + getModeName(mode) + " (attempt " + std::to_string(attempt) + ")");
            
            result = runner.runExperiment(mode, config);
            
            if (result.success) {
                NLM_LOG_INFO("Experiment succeeded on retry " + std::to_string(attempt));
                return true;
            }
            
            if (attempt < args.retryCount) {
                NLM_LOG_INFO("Waiting before retry...");
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }
    
    return result.success;
}

bool NLMCommandLineInterface::executeBenchmark(const Config& config, const CommandLineArgs& args) {
    ExperimentRunner runner;
    
    ExperimentResult result = runner.runExperiment(ExperimentMode::BENCHMARK, config);
    
    if (result.success) {
        printPerformanceReport(std::vector<ExperimentResult>{result});
    }
    
    return result.success;
}

bool NLMCommandLineInterface::executeAllExperiments(const Config& config, const CommandLineArgs& args) {
    ExperimentRunner runner;
    
    std::vector<ExperimentResult> results = runner.runAllExperiments(config);
    
    printResults(results);
    
    if (args.saveResults) {
        saveResults(results, args.resultsFile.empty() ? DEFAULT_RESULTS_FILE : args.resultsFile);
    }
    
    // Check if all experiments succeeded
    bool allSuccess = true;
    for (const auto& result : results) {
        if (!result.success && !args.continueOnError) {
            return false;
        }
        allSuccess = allSuccess && result.success;
    }
    
    if (args.enableMonitoring || args.enableProfiling) {
        generatePerformanceReport(args, results);
    }
    
    return allSuccess;
}

bool NLMCommandLineInterface::handleExecutionError(ExperimentMode mode, const std::exception& e, const CommandLineArgs& args) {
    printError("Error executing experiment " + getModeName(mode) + ": " + std::string(e.what()));
    
    if (args.continueOnError) {
        printWarning("Continuing with next experiment due to continue-on-error flag");
        return true;
    }
    
    return false;
}

bool NLMCommandLineInterface::retryExperiment(ExperimentMode mode, const Config& config, const CommandLineArgs& args, int attempt) {
    if (attempt < args.retryCount) {
        NLM_LOG_INFO("Retrying experiment " + getModeName(mode) + " (attempt " + std::to_string(attempt) + ")");
        return executeExperiment(mode, config, args);
    }
    
    return false;
}

UsageInfo NLMCommandLineInterface::getUsageInfo() const {
    UsageInfo usage;
    usage.programName = "nlm";
    usage.version = "Phase 2 (v0.1.0)";
    usage.description = "NLM Phase 2: Real Neural Computation - Advanced neural simulation with configurable experiments";
    
    // Build sections
    usage.sections = {
        "Experiments",
        "Configuration",
        "Output",
        "Performance",
        "Debugging"
    };
    
    // Build options
    usage.options = {
        {"help", "Display help information"},
        {"version", "Display version information"},
        {"experiment", "Experiment name or comma-separated list (e.g., test1,test2,test3)"},
        {"config", "Configuration file path"},
        {"set", "Set configuration override (KEY=VALUE)"},
        {"timestep", "Simulation timestep (default: 0.001)"},
        {"neurons", "Number of neurons (default: 500)"},
        {"regions", "Number of brain regions (default: 1)"},
        {"connection", "Connection probability (default: 0.15)"},
        {"output", "Output file path"},
        {"results", "Results file path"},
        {"output-format", "Output format (text, json, csv)"},
        {"verbose", "Enable verbose logging"},
        {"quiet", "Suppress output"},
        {"benchmark", "Run performance benchmark"},
        {"dry-run", "Validate configuration without running experiments"},
        {"validate-only", "Validate configuration and exit"},
        {"save-config", "Save configuration to file"},
        {"enable-monitoring", "Enable performance monitoring"},
        {"monitoring-interval", "Monitoring interval in seconds"},
        {"monitoring-file", "Monitoring output file path"},
        {"enable-profiling", "Enable performance profiling"},
        {"cpu-cores", "Number of CPU cores to use"},
        {"memory-limit", "Memory limit in MB"},
        {"debug-mode", "Enable debug mode"},
        {"trace-mode", "Enable trace mode"},
        {"log-level", "Log level (debug, info, warning, error)"},
        {"log-file", "Log file path"},
        {"capture-logs", "Capture logs to file"},
        {"continue-on-error", "Continue running experiments even if some fail"},
        {"retry-count", "Number of retry attempts for failed experiments"},
        {"timeout", "Experiment timeout in seconds"},
        {"temp-dir", "Temporary directory path"},
        {"enable-plugins", "Enable plugin system"}
    };
    
    return usage;
}

std::vector<NLMCommandLineInterface::ExperimentInfo> NLMCommandLineInterface::getAvailableExperiments() const {
    std::vector<ExperimentInfo> experiments;
    
    for (const auto& pair : EXPERIMENT_INFO) {
        experiments.push_back(pair.second);
    }
    
    return experiments;
}

std::map<std::string, std::string> NLMCommandLineInterface::getCommonOptions() const {
    return {
        {"help", "Display help information"},
        {"version", "Display version information"},
        {"verbose", "Enable verbose logging"},
        {"quiet", "Suppress output"},
        {"benchmark", "Run performance benchmark"}
    };
}

std::map<std::string, std::string> NLMCommandLineInterface::getExperimentOptions() const {
    return {
        {"experiment", "Experiment name or comma-separated list"},
        {"experiments", "Comma-separated list of experiments"},
        {"experiment-list", "Comma-separated list of experiments"},
        {"run-all", "Run all available experiments"},
        {"validate-only", "Validate configuration only"},
        {"dry-run", "Validate configuration without running experiments"}
    };
}

std::map<std::string, std::string> NLMCommandLineInterface::getConfigOptions() const {
    return {
        {"config", "Configuration file path"},
        {"set", "Set configuration override (KEY=VALUE)"},
        {"experiment-config", "Experiment-specific configuration file"},
        {"timestep", "Simulation timestep (default: 0.001)"},
        {"neurons", "Number of neurons (default: 500)"},
        {"regions", "Number of brain regions (default: 1)"},
        {"connection", "Connection probability (default: 0.15)"}
    };
}

std::map<std::string, std::string> NLMCommandLineInterface::getOutputOptions() const {
    return {
        {"output", "Output file path"},
        {"results", "Results file path"},
        {"output-format", "Output format (text, json, csv)"},
        {"save-results", "Save results to file"},
        {"save-config", "Save configuration to file"},
        {"show-progress", "Show progress during execution"},
        {"show-detailed", "Show detailed output"}
    };
}

std::map<std::string, std::string> NLMCommandLineInterface::getPerformanceOptions() const {
    return {
        {"enable-monitoring", "Enable performance monitoring"},
        {"monitoring-interval", "Monitoring interval in seconds"},
        {"monitoring-file", "Monitoring output file path"},
        {"enable-profiling", "Enable performance profiling"},
        {"cpu-cores", "Number of CPU cores to use"},
        {"memory-limit", "Memory limit in MB"},
        {"optimize-performance", "Apply performance optimizations"},
        {"parallel-enabled", "Enable parallel processing"}
    };
}

std::map<std::string, std::string> NLMCommandLineInterface::getDebugOptions() const {
    return {
        {"debug-mode", "Enable debug mode"},
        {"trace-mode", "Enable trace mode"},
        {"log-level", "Log level (debug, info, warning, error)"},
        {"log-file", "Log file path"},
        {"capture-logs", "Capture logs to file"},
        {"temp-dir", "Temporary directory path"},
        {"continue-on-error", "Continue on error"},
        {"retry-count", "Number of retry attempts"},
        {"timeout", "Experiment timeout in seconds"}
    };
}

bool NLMCommandLineInterface::fileExists(const std::string& path) const {
    std::ifstream file(path);
    return file.good();
}

bool NLMCommandLineInterface::directoryExists(const std::string& path) const {
    std::filesystem::path dir(path);
    return std::filesystem::exists(dir) && std::filesystem::is_directory(dir);
}

std::string NLMCommandLineInterface::getAbsolutePath(const std::string& path) const {
    std::filesystem::path absPath = std::filesystem::absolute(path);
    return absPath.string();
}

std::string NLMCommandLineInterface::getConfigFilePath(const std::string& experimentName) const {
    std::string configDir = "configs";
    std::string configFile = configDir + "/" + experimentName + ".cfg";
    
    if (fileExists(configFile)) {
        return getAbsolutePath(configFile);
    }
    
    // Try alternative config file names
    std::vector<std::string> altNames = {
        experimentName + ".cfg",
        "phase2_" + experimentName + ".cfg",
        "default_" + experimentName + ".cfg"
    };
    
    for (const auto& altName : altNames) {
        if (fileExists(altName)) {
            return getAbsolutePath(altName);
        }
    }
    
    return "";
}

std::string NLMCommandLineInterface::getResultsFilePath(const std::string& baseName) const {
    if (baseName.empty()) {
        return DEFAULT_RESULTS_FILE;
    }
    
    // Ensure file extension is appropriate
    std::string extension;
    if (baseName.find(".txt") != std::string::npos) {
        extension = ".txt";
    } else if (baseName.find(".json") != std::string::npos) {
        extension = ".json";
    } else if (baseName.find(".csv") != std::string::npos) {
        extension = ".csv";
    } else {
        extension = ".txt";
    }
    
    if (baseName.find(extension) == std::string::npos) {
        return baseName + extension;
    }
    
    return baseName;
}

std::string NLMCommandLineInterface::getLogFilePath(const std::string& logName) const {
    if (logName.empty()) {
        return DEFAULT_LOG_FILE;
    }
    
    if (logName.find(".log") == std::string::npos) {
        return logName + ".log";
    }
    
    return logName;
}

bool NLMCommandLineInterface::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return false;
    size_t end = str.find_last_not_of(" \t\r\n");
    return end > start;
}

std::string NLMCommandLineInterface::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace Phase2
} // namespace nlm