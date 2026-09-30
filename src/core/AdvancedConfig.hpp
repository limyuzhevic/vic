// Enhanced configuration loading with validation and error recovery
class AdvancedConfig {
public:
    static std::shared_ptr<Config> loadConfig(int argc, char** argv) {
        #include "ErrorHandling.hpp"
        #include "CommandLineParser.hpp"
        
        auto config = std::make_shared<Config>();
        CommandLineParser parser;
        
        // Add comprehensive command line arguments
        addStandardArguments(parser);
        addPlasticityArguments(parser);
        addMemoryArguments(parser);
        addNeuromodulationArguments(parser);
        addExperimentalArguments(parser);
        
        // Parse command line
        std::map<std::string, std::string> parsedArgs;
        NLM_LOG_INFO("Parsing command line arguments with enhanced validation...");
        
        if (!parser.parse(argc, argv, parsedArgs)) {
            NLM_LOG_ERROR("Failed to parse command line arguments", 
                         NLMErrorCategory::Configuration, "Use --help for usage information");
            return nullptr;
        }
        
        // Handle help and version flags
        if (parsedArgs.find("help") != parsedArgs.end()) {
            parser.printHelp();
            return nullptr;
        }
        
        if (parsedArgs.find("version") != parsedArgs.end()) {
            parser.printVersion();
            return nullptr;
        }
        
        // Load configuration from file if specified
        std::string configFile = "configs/default.cfg";
        if (parsedArgs.find("config") != parsedArgs.end()) {
            configFile = parsedArgs["config"];
            NLM_LOG_INFO("Loading configuration from: " + configFile);
            
            if (!config->loadFromFile(configFile)) {
                NLM_LOG_WARNING("Failed to load configuration from: " + configFile + ". Using defaults.");
            }
        }
        
        // Apply configuration defaults
        applyDefaults(config);
        
        // Apply command line overrides with validation
        bool configValid = applyCommandLineArgs(config, parsedArgs);
        
        if (!configValid) {
            NLM_LOG_ERROR("Configuration validation failed", 
                         NLMErrorCategory::Configuration, "Check command line arguments");
            return nullptr;
        }
        
        // Perform final validation
        auto validation = validateConfiguration(config);
        if (!validation.valid) {
            NLM_LOG_ERROR("Final configuration validation failed: " + validation.getSummary(),
                         NLMErrorCategory::Validation, "Configuration contains invalid values");
            return nullptr;
        }
        
        NLM_LOG_INFO("Configuration loaded successfully with " + std::to_string(config->getKeys().size()) + " entries");
        return config;
    }
    
private:
    static void addStandardArguments(CommandLineParser& parser) {
        parser.addArgument("help", 'h', "Show help information", "", false, false);
        parser.addArgument("version", 'v', "Show version information", "", false, false);
        parser.addArgument("verbose", '', "Enable verbose logging", "false", false, false);
        parser.addArgument("quiet", '', "Disable most logging", "false", false, false);
        parser.addArgument("timestep", 't', "Simulation timestep (seconds)", "0.001", false, true);
        parser.addArgument("steps", 's', "Number of simulation steps to run", "100", false, true);
        parser.addArgument("neuron-count", 'n', "Number of neurons in network", "500", false, true);
        parser.addArgument("region-count", 'r', "Number of brain regions", "1", false, true);
        parser.addArgument("seed", 'S', "Random seed (0 for random)", "42", false, true);
        parser.addArgument("config", 'c', "Load configuration from file", "configs/default.cfg", false, true);
        parser.addArgument("config-save", '', "Save current configuration to file", "", false, true);
    }
    
    static void addPlasticityArguments(CommandLineParser& parser) {
        parser.addArgument("enable-hebbian", '', "Enable Hebbian plasticity", "false", false, false);
        parser.addArgument("enable-stdp", '', "Enable Spike-Timing-Dependent Plasticity", "false", false, false);
        parser.addArgument("enable-reward", '', "Enable reward-modulated plasticity", "false", false, false);
        parser.addArgument("stdp-ltp-weight", '', "STDP LTP weight (default: 0.02)", "0.02", false, true);
        parser.addArgument("stdp-ltd-weight", '', "STDP LTD weight (default: 0.015)", "0.015", false, true);
        parser.addArgument("stdp-tau", '', "STDP time constant (ms)", "20.0", false, true);
        parser.addArgument("synaptogenesis-rate", '', "Synaptogenesis rate (default: 0.0001)", "0.0001", false, true);
        parser.addArgument("pruning-rate", '', "Pruning rate (default: 0.00001)", "0.00001", false, true);
    }
    
    static void addMemoryArguments(CommandLineParser& parser) {
        parser.addArgument("enable-working-memory", '', "Enable working memory system", "false", false, false);
        parser.addArgument("enable-episodic-memory", '', "Enable episodic memory system", "false", false, false);
        parser.addArgument("max-episodes", '', "Maximum number of episodes", "1000", false, true);
        parser.addArgument("enable-development", '', "Enable developmental stages", "false", false, false);
        parser.addArgument("developmental-stage", '', "Developmental stage (0=Initial, 1=Critical, 2=Maturation, 3=Adult)", "0", false, true);
    }
    
    static void addNeuromodulationArguments(CommandLineParser& parser) {
        parser.addArgument("enable-dopamine", '', "Enable dopamine system", "false", false, false);
        parser.addArgument("enable-curiosity", '', "Enable curiosity system", "false", false, false);
        parser.addArgument("enable-prediction-error", '', "Enable prediction error system", "false", false, false);
        parser.addArgument("dopamine-level", '', "Dopamine level (0.0-1.0)", "0.5", false, true);
        parser.addArgument("curiosity-threshold", '', "Curiosity threshold", "0.1", false, true);
        parser.addArgument("novelty-threshold", '', "Novelty threshold", "0.05", false, true);
    }
    
    static void addExperimentalArguments(CommandLineParser& parser) {
        parser.addArgument("save-checkpoint", '', "Save brain state to checkpoint file", "", false, true);
        parser.addArgument("load-checkpoint", '', "Load brain state from checkpoint file", "", false, true);
        parser.addArgument("benchmark", '', "Run benchmark tests", "false", false, false);
        parser.addArgument("diagnose", '', "Run diagnostic tests", "false", false, false);
        parser.addArgument("stats", '', "Save statistics to file (json, txt, csv)", "", false, true);
        parser.addArgument("plot", '', "Generate plots (neuro, memory, learning)", "", false, true);
        parser.addArgument("debug-mode", '', "Enable debug mode with additional logging", "false", false, false);
        parser.addArgument("experimental-mode", '', "Enable experimental features", "false", false, false);
    }
    
    static void applyDefaults(std::shared_ptr<Config>& config) {
        // Apply default configuration values for Phase 6
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Plasticity defaults
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Memory system defaults
        config->set("working_memory_enabled", false, ConfigSource::Default);
        config->set("episodic_memory_enabled", false, ConfigSource::Default);
        config->set("max_episodes", 1000, ConfigSource::Default);
        
        // Neuromodulation defaults
        config->set("dopamine_enabled", false, ConfigSource::Default);
        config->set("curiosity_enabled", false, ConfigSource::Default);
        config->set("prediction_error_enabled", false, ConfigSource::Default);
        config->set("dopamine_level", 0.5f, ConfigSource::Default);
        config->set("curiosity_threshold", 0.1f, ConfigSource::Default);
        
        // Development defaults
        config->set("development_enabled", false, ConfigSource::Default);
        config->set("developmental_stage", 0, ConfigSource::Default);
        
        // Experimental features
        config->set("benchmark_mode", false, ConfigSource::Default);
        config->set("diagnose_mode", false, ConfigSource::Default);
        config->set("debug_mode", false, ConfigSource::Default);
        config->set("experimental_mode", false, ConfigSource::Default);
    }
    
    static bool applyCommandLineArgs(std::shared_ptr<Config>& config, 
                                     const std::map<std::string, std::string>& parsedArgs) {
        #include "ErrorHandling.hpp"
        
        bool success = true;
        
        for (const auto& pair : parsedArgs) {
            const std::string& name = pair.first;
            const std::string& value = pair.second;
            
            try {
                if (name == "seed") {
                    long seed = std::stol(value);
                    config->set("random_seed", seed, ConfigSource::CommandLine);
                } else if (name == "timestep") {
                    double timestep = std::stod(value);
                    if (timestep <= 0.0) {
                        NLM_LOG_ERROR("Invalid timestep: " + value + ". Must be positive", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
                    }
                } else if (name == "steps") {
                    long steps = std::stol(value);
                    config->set("simulation_steps", steps, ConfigSource::CommandLine);
                } else if (name == "neuron-count") {
                    long neuronCount = std::stol(value);
                    if (neuronCount <= 0) {
                        NLM_LOG_ERROR("Invalid neuron count: " + value + ". Must be positive", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
                    }
                } else if (name == "region-count") {
                    long regionCount = std::stol(value);
                    if (regionCount <= 0) {
                        NLM_LOG_ERROR("Invalid region count: " + value + ". Must be positive", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("region_count", regionCount, ConfigSource::CommandLine);
                    }
                } else if (name == "stdp-ltp-weight") {
                    float weight = std::stof(value);
                    if (weight < 0) {
                        NLM_LOG_ERROR("Invalid STDP LTP weight: " + value + ". Must be non-negative", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("stdp_ltp_weight", weight, ConfigSource::CommandLine);
                    }
                } else if (name == "stdp-ltd-weight") {
                    float weight = std::stof(value);
                    if (weight < 0) {
                        NLM_LOG_ERROR("Invalid STDP LTD weight: " + value + ". Must be non-negative", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("stdp_ltd_weight", weight, ConfigSource::CommandLine);
                    }
                } else if (name == "stdp-tau") {
                    float tau = std::stof(value);
                    if (tau <= 0) {
                        NLM_LOG_ERROR("Invalid STDP tau: " + value + ". Must be positive", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("stdp_tau", tau, ConfigSource::CommandLine);
                    }
                } else if (name == "synaptogenesis-rate") {
                    float rate = std::stof(value);
                    if (rate < 0) {
                        NLM_LOG_ERROR("Invalid synaptogenesis rate: " + value + ". Must be non-negative", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("synaptogenesis_rate", rate, ConfigSource::CommandLine);
                    }
                } else if (name == "pruning-rate") {
                    float rate = std::stof(value);
                    if (rate < 0) {
                        NLM_LOG_ERROR("Invalid pruning rate: " + value + ". Must be non-negative", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("pruning_rate", rate, ConfigSource::CommandLine);
                    }
                } else if (name == "dopamine-level") {
                    float level = std::stof(value);
                    if (level < 0 || level > 1) {
                        NLM_LOG_ERROR("Invalid dopamine level: " + value + ". Must be in range [0.0, 1.0]", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("dopamine_level", level, ConfigSource::CommandLine);
                    }
                } else if (name == "curiosity-threshold") {
                    float threshold = std::stof(value);
                    if (threshold < 0) {
                        NLM_LOG_ERROR("Invalid curiosity threshold: " + value + ". Must be non-negative", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("curiosity_threshold", threshold, ConfigSource::CommandLine);
                    }
                } else if (name == "developmental-stage") {
                    long stage = std::stol(value);
                    if (stage < 0 || stage > 3) {
                        NLM_LOG_ERROR("Invalid developmental stage: " + value + ". Must be in range [0, 3]", 
                                     NLMErrorCategory::Validation, "Command line argument validation");
                        success = false;
                    } else {
                        config->set("developmental_stage", stage, ConfigSource::CommandLine);
                    }
                } else if (name == "enable-hebbian" || name == "enable-stdp" || name == "enable-reward" ||
                          name == "enable-working-memory" || name == "enable-episodic-memory" ||
                          name == "enable-development" || name == "enable-dopamine" ||
                          name == "enable-curiosity" || name == "enable-prediction-error" ||
                          name == "verbose" || name == "quiet" || name == "benchmark" ||
                          name == "diagnose" || name == "debug-mode" || name == "experimental-mode") {
                    // Boolean flags - conversion done in parser
                    config->set(name, value, ConfigSource::CommandLine);
                } else if (name == "config-save") {
                    // Save configuration to file
                    if (config->saveToFile(value)) {
                        NLM_LOG_INFO("Configuration saved to: " + value);
                    } else {
                        NLM_LOG_ERROR("Failed to save configuration to: " + value, 
                                     NLMErrorCategory::IO, "Configuration save failed");
                        success = false;
                    }
                } else if (name == "load-checkpoint") {
                    // Load checkpoint will be handled in main()
                    config->set("load_checkpoint", value, ConfigSource::CommandLine);
                } else if (name == "stats") {
                    // Save stats will be handled in main()
                    config->set("stats_output", value, ConfigSource::CommandLine);
                } else if (name == "plot") {
                    // Plot will be handled in main()
                    config->set("plot_output", value, ConfigSource::CommandLine);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error processing command line argument --" + name + " = " + value + ": " + e.what(),
                             NLMErrorCategory::Configuration, "Command line argument parsing");
                success = false;
            }
        }
        
        return success;
    }
    
    static Validation::ValidationResult validateConfiguration(std::shared_ptr<Config>& config) {
        Validation::ValidationResult result;
        
        try {
            // Validate simulation parameters
            double timestep = config->getOr<double>("simulation_timestep", 0.001);
            long neuronCount = config->getOr<long>("neuron_count", 1000);
            long regionCount = config->getOr<long>("region_count", 1);
            float connectionProb = config->getOr<float>("connection_probability", 0.1f);
            
            result.addErrors(Validation::validateSimulationParameters(
                timestep, neuronCount, regionCount, connectionProb).errors);
            
            // Validate plasticity parameters
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.02f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.015f);
            float stdpTau = config->getOr<float>("stdp_tau", 20.0f);
            
            if (ltpWeight < 0 || ltdWeight < 0) {
                result.addError("STDP weights must be non-negative");
            }
            
            if (stdpTau <= 0) {
                result.addError("STDP tau must be positive");
            }
            
            // Validate neuromodulation parameters
            float dopamineLevel = config->getOr<float>("dopamine_level", 0.5f);
            if (dopamineLevel < 0 || dopamineLevel > 1) {
                result.addError("Dopamine level must be in range [0.0, 1.0]");
            }
            
            float curiosityThreshold = config->getOr<float>("curiosity_threshold", 0.1f);
            if (curiosityThreshold < 0) {
                result.addError("Curiosity threshold must be non-negative");
            }
            
            // Validate developmental stage
            long developmentalStage = config->getOr<long>("developmental_stage", 0);
            if (developmentalStage < 0 || developmentalStage > 3) {
                result.addError("Developmental stage must be in range [0, 3]");
            }
            
            // Check for performance issues
            result.addWarnings(Validation::checkPerformanceIssues(
                "Neuron network", neuronCount, "Phase 6 implementation").warnings);
            
            // Check memory system configuration
            if (config->getOr<bool>("working_memory_enabled", false)) {
                NLM_LOG_INFO("Working memory system enabled");
            }
            
            if (config->getOr<bool>("episodic_memory_enabled", false)) {
                NLM_LOG_INFO("Episodic memory system enabled");
            }
            
        } catch (const std::exception& e) {
            result.addError("Configuration validation error: " + std::string(e.what()));
        }
        
        return result;
    }
};

} // namespace nlm
