// Enhanced command-line options for advanced users
struct AdvancedOptions {
    bool enableAdvancedLearning = false;
    bool enableDetailedLogging = false;
    bool enablePerformanceProfiling = false;
    bool enableVisualization = false;
    float learningRateOverride = 0.0f;
    float explorationRate = 0.0f;
    size_t maxMemoryUsage = 0; // MB
    std::string outputDirectory = "./nlm_output";
    bool saveIntermediateStates = false;
    bool enableAdvancedPlasticity = false;
    bool enableNeuromodulationTuning = false;
};

// Function to parse advanced command-line options
AdvancedOptions parseAdvancedOptions(int argc, char** argv) {
    AdvancedOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Advanced learning mode
        if (arg == "--advanced-learning" || arg == "--enable-learning") {
            options.enableAdvancedLearning = true;
        }
        // Detailed logging
        else if (arg == "--verbose" || arg == "--detailed-logging") {
            options.enableDetailedLogging = true;
        }
        // Performance profiling
        else if (arg == "--profile" || arg == "--performance-profiling") {
            options.enablePerformanceProfiling = true;
        }
        // Visualization
        else if (arg == "--visualization" || arg == "--enable-viz") {
            options.enableVisualization = true;
        }
        // Override learning rate
        else if (arg.find("--learning-rate=") != std::string::npos) {
            std::string value = arg.substr(arg.find("=") + 1);
            options.learningRateOverride = std::stof(value);
        }
        // Set exploration rate
        else if (arg.find("--exploration-rate=") != std::string::npos) {
            std::string value = arg.substr(arg.find("=") + 1);
            options.explorationRate = std::stof(value);
        }
        // Set memory limit
        else if (arg.find("--memory-limit=") != std::string::npos) {
            std::string value = arg.substr(arg.find("=") + 1);
            options.maxMemoryUsage = std::stoi(value);
        }
        // Set output directory
        else if (arg.find("--output-dir=") != std::string::npos) {
            options.outputDirectory = arg.substr(arg.find("=") + 1);
        }
        // Enable intermediate state saving
        else if (arg == "--save-intermediate" || arg == "--checkpoint-every") {
            options.saveIntermediateStates = true;
        }
        // Enable advanced plasticity
        else if (arg == "--advanced-plasticity" || arg == "--enable-plasticity") {
            options.enableAdvancedPlasticity = true;
        }
        // Enable neuromodulation tuning
        else if (arg == "--neuromodulation-tuning" || arg == "--enable-tuning") {
            options.enableNeuromodulationTuning = true;
        }
    }
    
    return options;
}

// Function to apply advanced options to brain configuration
void applyAdvancedOptions(std::shared_ptr<Config> config, const AdvancedOptions& options) {
    if (options.enableAdvancedLearning) {
        config->set("learning.advanced.enabled", "true", ConfigSource::CommandLine);
        config->set("learning.advanced.exploration_rate", std::to_string(options.explorationRate), ConfigSource::CommandLine);
    }
    
    if (options.learningRateOverride > 0.0f) {
        config->set("plasticity.stdp.learning_rate", std::to_string(options.learningRateOverride), ConfigSource::CommandLine);
        config->set("plasticity.hebbian.learning_rate", std::to_string(options.learningRateOverride), ConfigSource::CommandLine);
    }
    
    if (options.enableDetailedLogging) {
        config->set("logging.level", "DEBUG", ConfigSource::CommandLine);
        config->set("logging.detailed", "true", ConfigSource::CommandLine);
    }
    
    if (options.enablePerformanceProfiling) {
        config->set("performance.profiling.enabled", "true", ConfigSource::CommandLine);
        config->set("performance.profiling.output", options.outputDirectory + "/profile.csv", ConfigSource::CommandLine);
    }
    
    if (options.enableVisualization) {
        config->set("visualization.enabled", "true", ConfigSource::CommandLine);
        config->set("visualization.output", options.outputDirectory + "/visualization", ConfigSource::CommandLine);
    }
    
    if (options.maxMemoryUsage > 0) {
        config->set("system.memory_limit_mb", std::to_string(options.maxMemoryUsage), ConfigSource::CommandLine);
    }
    
    if (options.saveIntermediateStates) {
        config->set("checkpoint.save_intermediate", "true", ConfigSource::CommandLine);
        config->set("checkpoint.intermediate_interval", "1000", ConfigSource::CommandLine); // Every 1000 steps
    }
    
    if (options.enableAdvancedPlasticity) {
        config->set("plasticity.advanced.enabled", "true", ConfigSource::CommandLine);
        config->set("plasticity.advanced.stdp_variant", "adaptive", ConfigSource::CommandLine);
        config->set("plasticity.advanced.hebbian_variant", "context_dependent", ConfigSource::CommandLine);
    }
    
    if (options.enableNeuromodulationTuning) {
        config->set("neuromodulation.tuning.enabled", "true", ConfigSource::CommandLine);
        config->set("neuromodulation.tuning.dopamine_sensitivity", "2.0", ConfigSource::CommandLine);
        config->set("neuromodulation.tuning.curiosity_sensitivity", "1.5", ConfigSource::CommandLine);
    }
}

// Function to display help for advanced options
void displayAdvancedHelp() {
    std::cout << "\n=== ADVANCED NLM OPTIONS ===" << std::endl;
    std::cout << "Advanced learning mode for more sophisticated behavior:" << std::endl;
    std::cout << "  --advanced-learning, --enable-learning          Enable advanced learning algorithms" << std::endl;
    
    std::cout << "\nVerbose logging and debugging:" << std::endl;
    std::cout << "  --verbose, --detailed-logging                    Enable detailed logging (DEBUG level)" << std::endl;
    
    std::cout << "\nPerformance profiling:" << std::endl;
    std::cout << "  --profile, --performance-profiling              Enable performance profiling" << std::endl;
    
    std::cout << "\nVisualization:
  --visualization, --enable-viz                       Enable visualization system" << std::endl;
    
    std::cout << "\nLearning rate control:" << std::endl;
    std::cout << "  --learning-rate=<value>                         Override learning rate (default: 0.0 to use config)" << std::endl;
    
    std::cout << "\nExploration behavior:" << std::endl;
    std::cout << "  --exploration-rate=<value>                      Set exploration rate (0.0-1.0)" << std::endl;
    
    std::cout << "\nResource management:" << std::endl;
    std::cout << "  --memory-limit=<mb>                             Set memory limit in MB" << std::endl;
    
    std::cout << "\nOutput and checkpointing:" << std::endl;
    std::cout << "  --output-dir=<path>                             Set output directory" << std::endl;
    std::cout << "  --save-intermediate                               Enable intermediate state saving" << std::endl;
    
    std::cout << "\nAdvanced plasticity:
  --advanced-plasticity, --enable-plasticity         Enable advanced plasticity rules" << std::endl;
    
    std::cout << "\nNeuromodulation tuning:" << std::endl;
    std::cout << "  --neuromodulation-tuning, --enable-tuning       Enable neuromodulation parameter tuning" << std::endl;
    
    std::cout << "\nExample usage:" << std::endl;
    std::cout << "./nlm --config=advanced.cfg --advanced-learning --verbose --learning-rate=0.01 --exploration-rate=0.2" << std::endl;
}

// Enhanced main function with advanced options support
int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM with enhanced options...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse advanced options first
    AdvancedOptions advancedOpts = parseAdvancedOptions(argc, argv);
    
    // Display help if requested
    bool showHelp = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h" || arg == "--advanced-help") {
            showHelp = true;
            break;
        }
    }
    
    if (showHelp) {
        printBanner();
        std::cout << "=== BASIC NLM HELP ===" << std::endl;
        // Basic help text...
        std::cout << "\nFor advanced options help, use:" << std::endl;
        std::cout << "  ./nlm --advanced-help" << std::endl;
        displayAdvancedHelp();
        return 0;
    }
    
    // Initialize logging based on advanced options
    if (advancedOpts.enableDetailedLogging) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply advanced options
    applyAdvancedOptions(config, advancedOpts);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Log advanced options if any are enabled
    if (advancedOpts.enableAdvancedLearning) {
        NLM_LOG_INFO("Advanced Learning: Enabled");
    }
    if (advancedOpts.enableDetailedLogging) {
        NLM_LOG_INFO("Detailed Logging: Enabled");
    }
    if (advancedOpts.enablePerformanceProfiling) {
        NLM_LOG_INFO("Performance Profiling: Enabled (output to " + advancedOpts.outputDirectory + "/profile.csv)");
    }
    if (advancedOpts.enableVisualization) {
        NLM_LOG_INFO("Visualization: Enabled (output to " + advancedOpts.outputDirectory + "/visualization)");
    }
    if (advancedOpts.learningRateOverride > 0.0f) {
        NLM_LOG_INFO("Learning Rate Override: " + std::to_string(advancedOpts.learningRateOverride));
    }
    if (advancedOpts.explorationRate > 0.0f) {
        NLM_LOG_INFO("Exploration Rate: " + std::to_string(advancedOpts.explorationRate));
    }
    if (advancedOpts.maxMemoryUsage > 0) {
        NLM_LOG_INFO("Memory Limit: " + std::to_string(advancedOpts.maxMemoryUsage) + " MB");
    }
    
    NLM_LOG_INFO("");
    
    // Continue with existing tests...
    // (rest of the main function would continue here)
    
    return 0;
}