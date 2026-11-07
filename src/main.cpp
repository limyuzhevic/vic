int main(int argc, char** argv) {
    // Print advanced banner
    printBannerAdvanced();
    
    // Parse command line options
    CommandLineOptions options = parseCommandLine(argc, argv);
    
    // Show help if requested
    if (options.showAdvancedOptions) {
        printAdvancedHelp();
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.verbose ? LogLevel::Info : LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(options.configFile)) {
        std::cerr << "Warning: Failed to load configuration from " << options.configFile << ". Using defaults." << std::endl;
    }
    
    // Apply custom parameters
    for (const auto& param : options.customParams) {
        std::string key = param.first;
        std::string value = param.second;
        
        // Try to convert to appropriate type based on key
        if (key.find("float") != std::string::npos || key.find("rate") != std::string::npos || 
            key.find("weight") != std::string::npos || key.find("time") != std::string::npos) {
            try {
                float floatValue = std::stof(value);
                config->set(key, floatValue, ConfigSource::CommandLine);
            } catch (...) {
                std::cerr << "Warning: Could not parse float parameter: " << key << " = " << value << std::endl;
            }
        } else if (key.find("int") != std::string::npos || key.find("count") != std::string::npos || 
                   key.find("steps") != std::string::npos || key.find("interval") != std::string::npos) {
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::CommandLine);
            } catch (...) {
                std::cerr << "Warning: Could not parse int parameter: " << key << " = " << value << std::endl;
            }
        } else {
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Override with command line args (for backward compatibility)
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (maintained for backward compatibility)
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
    
    // Log configuration summary
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
        std::cout << "  experiment_mode: " << options.experimentMode << std::endl;
        std::cout << std::endl;
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run advanced experiment
    AdvancedExperimentEngine engine(brain, options);
    engine.run();
    
    // Final brain status
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    FINAL BRAIN STATUS                           ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Total neurons:    " << brain->getTotalNeuronCount() << "                    ║" << std::endl;
        std::cout << "║ Total synapses:   " << brain->getTotalSynapseCount() << "                    ║" << std::endl;
        std::cout << "║ Total spikes:     " << brain->getTotalSpikeCount() << "                    ║" << std::endl;
        std::cout << "║ E/I ratio:        " << brain->getExcitationInhibitionRatio() << "                    ║" << std::endl;
        std::cout << "║ Firing rate:      " << brain->getAverageFiringRate() << " Hz                  ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "🎉 NLM Advanced Experiment Complete!" << std::endl;
    
    return 0;
}