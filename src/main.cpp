// Helper functions for command-line interface
void generateConfigurationTemplate(const std::string& configType) {
    NLM_LOG_INFO("Generating configuration template: " + configType);
    
    auto config = std::make_shared<Config>();
    
    if (configType == "visualization") {
        NLM_LOG_INFO("Visualization configuration:")
        NLM_LOG_INFO("  - Higher neuron count for detailed visualization");
        NLM_LOG_INFO("  - Visualization enabled");
        NLM_LOG_INFO("  - Slower timestep for smooth rendering");
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.05, ConfigSource::Default);
        config->set("visualization_enabled", true, ConfigSource::Default);
        config->set("visualization_update_rate", 30.0, ConfigSource::Default);
        
    } else if (configType == "research") {
        NLM_LOG_INFO("Research configuration:")
        NLM_LOG_INFO("  - Large scale simulation for research");
        NLM_LOG_INFO("  - High plasticity rates for exploration");
        NLM_LOG_INFO("  - High precision timestep");
        
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("simulation_timestep", 0.0005, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.03, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.002, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.002, ConfigSource::Default);
        config->set("plasticity_learning_rate", 0.005, ConfigSource::Default);
        config->set("development_synaptogenesis_rate", 0.002, ConfigSource::Default);
        config->set("development_pruning_rate", 0.0002, ConfigSource::Default);
        
    } else if (configType == "test") {
        NLM_LOG_INFO("Test configuration:")
        NLM_LOG_INFO("  - Small scale for quick testing");
        NLM_LOG_INFO("  - High connectivity for reproducible results");
        
        config->set("random_seed", static_cast<int64_t>(99), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2, ConfigSource::Default);
        
    } else if (configType == "production") {
        NLM_LOG_INFO("Production configuration:")
        NLM_LOG_INFO("  - Balanced performance and accuracy");
        NLM_LOG_INFO("  - Optimized for steady-state operation");
        
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        
    } else {
        NLM_LOG_INFO("Unknown configuration type: " + configType);
        NLM_LOG_INFO("Generating default configuration instead.");
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
    }
    
    // Save the generated configuration
    std::string filename = "generated_" + configType + ".cfg";
    if (config->saveToFile(filename)) {
        NLM_LOG_INFO("Configuration template saved to: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to save configuration template!");
    }
}

void validateConfiguration(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== Validating Configuration ===");
    
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Basic validation
    auto validateKey = [&config, &errors, &warnings](const std::string& key, 
                                                     int64_t min = INT64_MIN, int64_t max = INT64_MAX,
                                                     double minDouble = -std::numeric_limits<double>::infinity(),
                                                     double maxDouble = std::numeric_limits<double>::infinity()) {
        if (config->has(key)) {
            auto valueOpt = config->get<int64_t>(key);
            if (valueOpt) {
                if (*valueOpt < min || *valueOpt > max) {
                    warnings.push_back("Key '" + key + "' value " + std::to_string(*valueOpt) +
                                     " is outside recommended range [" + std::to_string(min) +
                                     ", " + std::to_string(max) + "]");
                }
            } else {
                auto doubleOpt = config->get<double>(key);
                if (doubleOpt) {
                    if (*doubleOpt < minDouble || *doubleOpt > maxDouble) {
                        warnings.push_back("Key '" + key + "' value " + std::to_string(*doubleOpt) +
                                         " is outside recommended range [" + std::to_string(minDouble) +
                                         ", " + std::to_string(maxDouble) + "]");
                    }
                }
            }
        }
    };
    
    // Validate essential keys
    std::vector<std::string> requiredKeys = {"random_seed", "neuron_count", "simulation_timestep"};
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            errors.push_back("Missing required key: '" + key + "'");
        }
    }
    
    // Validate parameter ranges
    validateKey("random_seed", 0, 2147483647);
    validateKey("neuron_count", 1, 100000);
    validateKey("simulation_timestep", 0.000001, 1.0);
    validateKey("region_count", 1, 20);
    
    // Output results
    if (errors.empty() && warnings.empty()) {
        NLM_LOG_INFO("✓ Configuration is valid and contains all required keys.");
    } else {
        if (!errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed with errors:");
            for (const auto& error : errors) {
                NLM_LOG_ERROR("  - " + error);
            }
        }
        if (!warnings.empty()) {
            NLM_LOG_INFO("Configuration has warnings:");
            for (const auto& warning : warnings) {
                NLM_LOG_INFO("  - " + warning);
            }
        }
    }
}

void runBenchmarkMode(const std::string& mode, const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== Running Benchmark Mode: " + mode + " ===");
    
    if (mode == "small") {
        NLM_LOG_INFO("Running small benchmark (100 neurons)...");
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        
    } else if (mode == "medium") {
        NLM_LOG_INFO("Running medium benchmark (1000 neurons)...");
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        
    } else if (mode == "large") {
        NLM_LOG_INFO("Running large benchmark (5000 neurons)...");
        config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        
    } else if (mode == "performance") {
        NLM_LOG_INFO("Running performance benchmark with high connectivity...");
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
        config->set("connection_probability", 0.15f, ConfigSource::Runtime);
        
    } else {
        NLM_LOG_INFO("Unknown benchmark mode: " + mode);
        NLM_LOG_INFO("Running default benchmark mode instead.");
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
    }
    
    NLM_LOG_INFO("Benchmark configuration applied.");
    NLM_LOG_INFO("To run the benchmark, execute: ./nlm_benchmark --mode " + mode);
}

void compareBrainsWithFile(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    NLM_LOG_INFO("=== Comparing Brain with File: " + filepath + " ===");
    
    auto fileConfig = std::make_shared<Config>();
    if (!fileConfig->loadFromFile(filepath)) {
        NLM_LOG_ERROR("Failed to load configuration from file: " + filepath);
        return;
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    NLM_LOG_INFO("Brain configuration:");
    brain->logStatus();
    
    NLM_LOG_INFO("File configuration:");
    NLM_LOG_INFO(fileConfig->summary());
    
    // Compare key parameters
    NLM_LOG_INFO("=== Comparison Results ===");
    
    auto compareParam = [brain, fileConfig](const std::string& key, const std::string& paramName) {
        if (brain->getConfig()->has(key) && fileConfig->has(key)) {
            auto brainVal = brain->getConfig()->get<std::string>(key);
            auto fileVal = fileConfig->get<std::string>(key);
            
            if (brainVal && fileVal) {
                if (*brainVal == *fileVal) {
                    NLM_LOG_INFO("✓ " + paramName + ": Match (" + *brainVal + ")");
                } else {
                    NLM_LOG_INFO("⚠ " + paramName + ": Different (Brain: " + *brainVal +
                               ", File: " + *fileVal + ")");
                }
            }
        }
    };
    
    compareParam("random_seed", "Random Seed");
    compareParam("neuron_count", "Neuron Count");
    compareParam("simulation_timestep", "Simulation Timestep");
    compareParam("region_count", "Region Count");
    compareParam("connection_probability", "Connection Probability");
    
    NLM_LOG_INFO("Comparison complete.");
}

void runCustomSteps(const std::shared_ptr<Brain>& brain, SimulationStep steps) {
    NLM_LOG_INFO("=== Running Custom Steps: " + std::to_string(steps) + " ===");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                       std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
        }
    }
    
    NLM_LOG_INFO("Custom steps completed successfully.");
    NLM_LOG_INFO("Final statistics:");
    brain->logStatus();
}

void exportSimulationData(const std::shared_ptr<Brain>& brain, const std::string& prefix) {
    NLM_LOG_INFO("=== Exporting Simulation Data: " + prefix + " ===");
    
    std::vector<std::string> filesGenerated;
    
    // Export brain state
    std::string brainFile = prefix + "_brain.bin";
    brain->save(brainFile);
    filesGenerated.push_back(brainFile);
    
    // Export statistics report
    std::string statsFile = prefix + "_stats.txt";
    std::ofstream stats(statsFile);
    if (stats.is_open()) {
        stats << "=== Brain Statistics Export ===" << std::endl;
        stats << "Export time: " << std::chrono::system_clock::now() << std::endl;
        stats << brain->getStatisticsReport();
        stats.close();
        filesGenerated.push_back(statsFile);
    }
    
    // Export configuration
    std::string configFile = prefix + "_config.cfg";
    if (brain->getConfig()->saveToFile(configFile)) {
        filesGenerated.push_back(configFile);
    }
    
    NLM_LOG_INFO("Export completed successfully.");
    NLM_LOG_INFO("Generated files:");
    for (const auto& file : filesGenerated) {
        NLM_LOG_INFO("  - " + file);
    }
}