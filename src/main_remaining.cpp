
// Enhanced configuration loader
class EnhancedConfigLoader {
public:
    static std::string detectFileFormat(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return "unknown";
        
        char firstLine[256];
        file.getline(firstLine, sizeof(firstLine));
        file.close();
        
        std::string content = firstLine;
        std::transform(content.begin(), content.end(), content.begin(), ::tolower);
        
        if (content.find("{") != std::string::npos || content.find("[") != std::string::npos) {
            return "json";
        } else if (content.find("---") != std::string::npos || content.find("key:") != std::string::npos) {
            return "yaml";
        }
        
        return "unknown";
    }
    
    static bool loadFromFile(Config& config, const std::string& filepath) {
        std::string format = detectFileFormat(filepath);
        
        if (format == "json") {
            return config.loadFromFile(filepath);
        } else if (format == "yaml") {
            // For now, just try to load as JSON since that's what's supported
            return config.loadFromFile(filepath);
        }
        
        return false;
    }
    
    static bool loadFromEnv(Config& config) {
        const char* env_vars[] = {
            "NLM_NEURON_COUNT",
            "NLM_REGION_COUNT", 
            "NLM_SIMULATION_TIMESTEP",
            "NLM_RANDOM_SEED",
            "NLM_CONNECTION_PROBABILITY",
            "NLM_STDP_LTP_WEIGHT",
            "NLM_STDP_LTD_WEIGHT",
            "NLM_SYNAPTISSGENESIS_RATE",
            "NLM_PRUNING_RATE",
            nullptr
        };
        
        for (int i = 0; env_vars[i] != nullptr; ++i) {
            const char* value = std::getenv(env_vars[i]);
            if (value) {
                std::string key = env_vars[i];
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                std::erase(key, 'NLM_');
                
                config.set(key, std::string(value), ConfigSource::Runtime);
            }
        }
        
        return true;
    }
};

// Comprehensive error handling utility
class ErrorHandler {
public:
    static void logError(const std::string& context, const std::string& message, bool fatal = false) {
        std::cerr << Color::RED << "[ERROR]" << Color::RESET << " " << context << ": " << message << std::endl;
        if (fatal) {
            std::cerr << Color::YELLOW << "[!] Fatal error. Shutting down..." << Color::RESET << std::endl;
            std::exit(1);
        }
    }
    
    static void logWarning(const std::string& context, const std::string& message) {
        std::cerr << Color::YELLOW << "[WARNING]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
    
    static void logInfo(const std::string& context, const std::string& message) {
        std::cout << Color::BLUE << "[INFO]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
    
    static void logSuccess(const std::string& context, const std::string& message) {
        std::cout << Color::GREEN << "[SUCCESS]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
    
    static void logProgress(const std::string& context, const std::string& message) {
        std::cout << Color::CYAN << "[PROGRESS]" << Color::RESET << " " << context << ": " << message << std::endl;
    }
};

// Enhanced configuration loader
class EnhancedConfigLoader {
public:
    static std::string detectFileFormat(const std::string& filepath) {
        if (!std::filesystem::exists(filepath)) return "unknown";
        if (!std::filesystem::is_regular_file(filepath)) return "unknown";
        
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) return "unknown";
            
            char firstLine[256];
            file.getline(firstLine, sizeof(firstLine));
            file.close();
            
            std::string content = firstLine;
            std::transform(content.begin(), content.end(), content.begin(), ::tolower);
            
            if (content.find("{") != std::string::npos || content.find("[") != std::string::npos) {
                return "json";
            } else if (content.find("---") != std::string::npos || content.find("key:") != std::string::npos) {
                return "yaml";
            }
            
            return "unknown";
        } catch (const std::exception&) {
            return "unknown";
        }
    }
    
    static bool loadFromFile(Config& config, const std::string& filepath) {
        std::string format = detectFileFormat(filepath);
        
        if (format == "json") {
            return config.loadFromFile(filepath);
        } else if (format == "yaml") {
            // For now, just try to load as JSON since that's what's supported
            return config.loadFromFile(filepath);
        }
        
        return false;
    }
    
    static bool loadFromEnv(Config& config) {
        const char* env_vars[] = {
            "NLM_NEURON_COUNT",
            "NLM_REGION_COUNT", 
            "NLM_SIMULATION_TIMESTEP",
            "NLM_RANDOM_SEED",
            "NLM_CONNECTION_PROBABILITY",
            "NLM_STDP_LTP_WEIGHT",
            "NLM_STDP_LTD_WEIGHT",
            "NLM_SYNAPTISSGENESIS_RATE",
            "NLM_PRUNING_RATE",
            nullptr
        };
        
        bool success = true;
        
        for (int i = 0; env_vars[i] != nullptr; ++i) {
            const char* value = std::getenv(env_vars[i]);
            if (value) {
                std::string key = env_vars[i];
                std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                std::erase(key, 'NLM_');
                
                try {
                    // Validate environment variable name
                    bool isValidPrefix = false;
                    for (const auto& prefix : {"NLM_", "BRAIN_"}) {
                        if (std::string(env_vars[i]).find(prefix) == 0) {
                            isValidPrefix = true;
                            break;
                        }
                    }
                    
                    if (isValidPrefix) {
                        config.set(key, std::string(value), ConfigSource::Runtime);
                    } else {
                        ErrorHandler::logWarning("ENV", std::string("Ignoring invalid environment variable prefix: ") + env_vars[i]);
                    }
                } catch (const std::exception& e) {
                    ErrorHandler::logWarning("ENV", std::string("Failed to set environment variable ") + key + ": " + e.what());
                    success = false;
                }
            }
        }
        
        return success;
    }
};

// Enhanced configuration validation with detailed error reporting
bool validateConfiguration(const Config& config) {
    bool valid = true;
    
    try {
        // Check neuron count with enhanced validation
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        if (neuronCountOpt.has_value()) {
            int64_t neuronCount = *neuronCountOpt;
            if (neuronCount < 10) {
                ErrorHandler::logWarning("CONFIG", "Neuron count very low (" + std::to_string(neuronCount) + "). May affect simulation quality.");
            } else if (neuronCount > 10000000) {
                ErrorHandler::logWarning("CONFIG", "Neuron count very high (" + std::to_string(neuronCount) + "). May impact performance severely.");
                valid = false;
            }
        }
        
        // Check region count with enhanced validation
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (regionCountOpt.has_value()) {
            int64_t regionCount = *regionCountOpt;
            if (regionCount < 1 || regionCount > 100) {
                ErrorHandler::logError("CONFIG", "Invalid region count: " + std::to_string(regionCount) + ". Must be between 1 and 100.", false);
                valid = false;
            }
        }
        
        // Check simulation timestep with enhanced validation
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (timestepOpt.has_value()) {
            double timestep = *timestepOpt;
            if (timestep <= 0.0) {
                ErrorHandler::logError("CONFIG", "Invalid simulation timestep: " + std::to_string(timestep) + ". Must be positive.", false);
                valid = false;
            } else if (timestep < 0.0001) {
                ErrorHandler::logWarning("CONFIG", "Simulation timestep very small (" + std::to_string(timestep) + "). May cause numerical instability.");
            } else if (timestep > 1.0) {
                ErrorHandler::logWarning("CONFIG", "Simulation timestep very large (" + std::to_string(timestep) + "). May cause inaccurate simulation.");
            }
        }
        
        // Check connection probability with enhanced validation
        auto connectionProbOpt = config.get<double>("connection_probability");
        if (connectionProbOpt.has_value()) {
            double connProb = *connectionProbOpt;
            if (connProb < 0.0 || connProb > 1.0) {
                ErrorHandler::logError("CONFIG", "Invalid connection probability: " + std::to_string(connProb) + ". Must be between 0.0 and 1.0.", false);
                valid = false;
            }
        }
        
        // Validate random seed
        auto seedOpt = config.get<int64_t>("random_seed");
        if (seedOpt.has_value()) {
            int64_t seed = *seedOpt;
            if (seed < 0 || seed > std::numeric_limits<int64_t>::max()) {
                ErrorHandler::logError("CONFIG", "Invalid random seed: " + std::to_string(seed) + ". Must be positive.", false);
                valid = false;
            }
        }
        
        // Check memory system parameters
        auto memCapOpt = config.get<int64_t>("working_memory_capacity");
        if (memCapOpt.has_value()) {
            int64_t memCap = *memCapOpt;
            if (memCap < 0) {
                ErrorHandler::logError("CONFIG", "Invalid working memory capacity: " + std::to_string(memCap) + ". Must be non-negative.", false);
                valid = false;
            }
        }
        
        auto maxEpOpt = config.get<int64_t>("episodic_memory_max_episodes");
        if (maxEpOpt.has_value()) {
            int64_t maxEp = *maxEpOpt;
            if (maxEp < 0) {
                ErrorHandler::logError("CONFIG", "Invalid max episodes: " + std::to_string(maxEp) + ". Must be non-negative.", false);
                valid = false;
            }
        }
        
        // Check plasticity parameters
        auto synaptogenesisOpt = config.get<double>("synaptogenesis_rate");
        if (synaptogenesisOpt.has_value()) {
            double synaptogenesis = *synaptogenesisOpt;
            if (synaptogenesis < 0.0 || synaptogenesis > 1.0) {
                ErrorHandler::logError("CONFIG", "Invalid synaptogenesis rate: " + std::to_string(synaptogenesis) + ". Must be between 0.0 and 1.0.", false);
                valid = false;
            }
        }
        
        auto pruningOpt = config.get<double>("pruning_rate");
        if (pruningOpt.has_value()) {
            double pruning = *pruningOpt;
            if (pruning < 0.0 || pruning > 1.0) {
                ErrorHandler::logError("CONFIG", "Invalid pruning rate: " + std::to_string(pruning) + ". Must be between 0.0 and 1.0.", false);
                valid = false;
            }
        }
        
    } catch (const std::exception& e) {
        ErrorHandler::logError("CONFIG", std::string("Configuration validation error: ") + e.what(), false);
        valid = false;
    }
    
    return valid;
}

int main(int argc, char** argv) {
    // Set up signal handling with enhanced error handling
    try {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        ErrorHandler::logInfo("MAIN", "Signal handlers registered");
    } catch (const std::exception& e) {
        ErrorHandler::logWarning("MAIN", std::string("Failed to register signal handlers: ") + e.what());
    }
    
    // Initialize global error flag
    g_errorExit = false;
    
    // Parse command line arguments with enhanced validation
    EnhancedCLI cli(argc, argv);
    
    try {
        // Print banner
        printBanner();
        
        // Check for help flag
        if (cli.hasFlag("help")) {
            printHelp();
            return 0;
        }
        
        // Validate command-line arguments
        int argCount = 0;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            argCount++;
            
            // Check for potential argument length issues
            if (arg.length() > 1024) {
                ErrorHandler::logWarning("CLI", std::string("Argument too long (truncated): ") + arg.substr(0, 50) + "...");
            }
            
            // Check for malformed arguments
            if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos && arg.find(' ') == std::string::npos &&
                i + 1 < argc && argv[i + 1][0] != '-') {
                // This might be --key value format, which is valid
            } else if (arg.substr(0, 2) == "--") {
                // Check key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos && pos > 2) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key
                    if (key.empty() || key.find(' ') != std::string::npos || key.find('=') != std::string::npos) {
                        ErrorHandler::logWarning("CLI", std::string("Invalid configuration key: ") + key);
                    }
                }
            }
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration from file if specified with enhanced error handling
        if (cli.hasFlag("config")) {
            ErrorHandler::logProgress("MAIN", "Loading configuration...");
            std::string configFile = cli.getArg("config");
            if (!configFile.empty()) {
                configFile = "configs/" + configFile;
                
                try {
                    if (EnhancedConfigLoader::loadFromFile(*config, configFile)) {
                        ErrorHandler::logSuccess("MAIN", "Configuration loaded from: " + configFile);
                    } else {
                        ErrorHandler::logWarning("MAIN", "Failed to load configuration from: " + configFile + ". Using defaults.");
                    }
                } catch (const std::exception& e) {
                    ErrorHandler::logWarning("MAIN", std::string("Exception loading configuration: ") + e.what() + ". Using defaults.");
                }
            }
        }
        
        // Load environment variables with enhanced error handling
        try {
            ErrorHandler::logProgress("MAIN", "Loading environment variables...");
            if (EnhancedConfigLoader::loadFromEnv(*config)) {
                ErrorHandler::logSuccess("MAIN", "Environment variables loaded");
            } else {
                ErrorHandler::logWarning("MAIN", "Some environment variables failed to load");
            }
        } catch (const std::exception& e) {
            ErrorHandler::logWarning("MAIN", std::string("Exception loading environment variables: ") + e.what());
        }
        
        // Load from command line arguments (overrides file) with enhanced validation
        try {
            ErrorHandler::logProgress("MAIN", "Loading command line arguments...");
            if (!config->loadFromArgs(argc, argv)) {
                ErrorHandler::logWarning("MAIN", "Failed to load all command line arguments");
            } else {
                ErrorHandler::logSuccess("MAIN", "Command line arguments loaded");
            }
        } catch (const std::exception& e) {
            ErrorHandler::logWarning("MAIN", std::string("Exception loading command line arguments: ") + e.what());
        }
        
        // Apply default values for Phase 2/6 compatibility with enhanced validation
        try {
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
            
            ErrorHandler::logInfo("MAIN", "Default configuration values applied");
            
        } catch (const std::exception& e) {
            ErrorHandler::logError("MAIN", std::string("Failed to apply default configuration: ") + e.what(), true);
            return 1;
        }
        
        // Validate configuration with enhanced error handling
        ErrorHandler::logProgress("MAIN", "Validating configuration...");
        if (!validateConfiguration(*config)) {
            ErrorHandler::logError("MAIN", "Configuration validation failed!", true);
            return 1;
        }
        
        ErrorHandler::logSuccess("MAIN", "Configuration validated successfully");
        
        // Set up logging based on verbosity
        auto logger = std::make_shared<Logger>();
        auto logLevel = cli.hasFlag("quiet") ? LogLevel::Error : 
                       cli.hasFlag("verbose") ? LogLevel::Info : LogLevel::Warning;
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Log configuration summary
        ErrorHandler::logInfo("MAIN", "Configuration loaded and validated");
        ErrorHandler::logProgress("MAIN", "Starting NLM enhanced execution...");
        
        // Run based on mode flags (Priority: demo-mode > phase6 > phase2)
        try {
            if (cli.hasFlag("demo-mode")) {
                ErrorHandler::logSuccess("MAIN", "Phase 6 demo mode requested");
                runPhase6Demo(config, cli);
            } else if (cli.hasFlag("phase6")) {
                ErrorHandler::logSuccess("MAIN", "Phase 6 integration mode requested");
                runPhase6Demo(config, cli);
            } else if (cli.hasFlag("phase2")) {
                ErrorHandler::logSuccess("MAIN", "Phase 2 backward compatibility mode requested");
                runOriginalPhase2Tests(config);
            } else {
                // Default: Run Phase 2 tests (backward compatibility)
                ErrorHandler::logInfo("MAIN", "No mode specified. Running Phase 2 tests (backward compatibility)");
                runOriginalPhase2Tests(config);
            }
            
        } catch (const BrainExecutionError& e) {
            ErrorHandler::logError("MAIN", std::string("Brain execution error: ") + e.what() + " (Step: " + std::to_string(e.getStep()) + ")", true);
            return 1;
        } catch (const BrainInitializationError& e) {
            ErrorHandler::logError("MAIN", std::string("Brain initialization error: ") + e.what() + " (Component: " + e.getComponent() + ")", true);
            return 1;
        } catch (const BrainIOError& e) {
            ErrorHandler::logError("MAIN", std::string("Brain I/O error: ") + e.what() + " (Type: " + std::to_string(static_cast<int>(e.getType())) + ")", true);
            return 1;
        } catch (const ValidationError& e) {
            ErrorHandler::logError("MAIN", std::string("Configuration validation error: ") + e.what() + " (Field: " + e.getField() + ")", true);
            return 1;
        } catch (const std::exception& e) {
            ErrorHandler::logError("MAIN", std::string("Unexpected error during execution: ") + e.what(), true);
            return 1;
        }
        
        ErrorHandler::logSuccess("MAIN", "=== NLM Execution Complete ===");
        return 0;
        
    } catch (const std::exception& e) {
        ErrorHandler::logError("MAIN", std::string("Fatal error in main: ") + e.what(), true);
        return 1;
    }
}
