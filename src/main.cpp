#ifndef NLM_IMPROVED_MAIN_H
#define NLM_IMPROVED_MAIN_H

#include "core/ImprovedConfig.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>

namespace nlm {

class ImprovedMainApplication {
public:
    struct CommandLineOptions {
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        std::string checkpointPath;
        bool enableVisualization = false;
        bool enableLogging = true;
        bool enableCheckpointing = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        int experimentType = 1; // 1: Basic, 2: Plasticity, 3: STDP, 4: Full Integration
        size_t neuronCount = 500;
        size_t maxSteps = 2000;
        double timestep = 0.001;
        int64_t randomSeed = 42;
        float stdpLtpWeight = 0.01f;
        float stdpLtdWeight = 0.012f;
        float stdpTimeConstant = 20.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        int verbosity = 1; // 0: Silent, 1: Normal, 2: Verbose, 3: Debug
        std::unordered_map<std::string, std::string> extraConfig;
    };
    
    class ApplicationException : public std::runtime_error {
    public:
        explicit ApplicationException(const std::string& message) 
            : std::runtime_error("Application Error: " + message) {}
    };
    
    ImprovedMainApplication() {
        initializeDefaultConfigSchema();
    }
    
    bool parseCommandLine(int argc, char** argv, CommandLineOptions& options) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                
                if (arg == "--help" || arg == "-h") {
                    printUsage();
                    return false;
                }
                
                // Configuration file
                if (arg == "--config" || arg == "-c") {
                    if (i + 1 < argc) {
                        options.configFile = argv[++i];
                    } else {
                        throw ApplicationException("Missing configuration file after --config");
                    }
                }
                
                // Output file
                if (arg == "--output" || arg == "-o") {
                    if (i + 1 < argc) {
                        options.outputFile = argv[++i];
                    } else {
                        throw ApplicationException("Missing output file after --output");
                    }
                }
                
                // Checkpoint path
                if (arg == "--checkpoint" || arg == "-k") {
                    if (i + 1 < argc) {
                        options.checkpointPath = argv[++i];
                    } else {
                        throw ApplicationException("Missing checkpoint path after --checkpoint");
                    }
                }
                
                // Boolean flags
                if (arg == "--visualization" || arg == "-v") {
                    options.enableVisualization = true;
                }
                
                if (arg == "--checkpointing" || arg == "-k") {
                    options.enableCheckpointing = true;
                }
                
                if (arg == "--replay" || arg == "-r") {
                    options.enableReplay = true;
                }
                
                if (arg == "--development" || arg == "-d") {
                    options.enableDevelopment = true;
                }
                
                // Numeric options
                if (arg == "--experiment" || arg == "-e") {
                    if (i + 1 < argc) {
                        options.experimentType = std::stoi(argv[++i]);
                        if (options.experimentType < 1 || options.experimentType > 4) {
                            throw ApplicationException("Experiment type must be between 1 and 4");
                        }
                    } else {
                        throw ApplicationException("Missing experiment type after --experiment");
                    }
                }
                
                if (arg == "--neurons" || arg == "-n") {
                    if (i + 1 < argc) {
                        options.neuronCount = static_cast<size_t>(std::stoull(argv[++i]));
                    } else {
                        throw ApplicationException("Missing neuron count after --neurons");
                    }
                }
                
                if (arg == "--steps" || arg == "-s") {
                    if (i + 1 < argc) {
                        options.maxSteps = static_cast<size_t>(std::stoull(argv[++i]));
                    } else {
                        throw ApplicationException("Missing max steps after --steps");
                    }
                }
                
                if (arg == "--timestep") {
                    if (i + 1 < argc) {
                        options.timestep = std::stod(argv[++i]);
                        if (options.timestep <= 0.0) {
                            throw ApplicationException("Timestep must be positive");
                        }
                    } else {
                        throw ApplicationException("Missing timestep after --timestep");
                    }
                }
                
                if (arg == "--seed" || arg == "-S") {
                    if (i + 1 < argc) {
                        options.randomSeed = std::stoll(argv[++i]);
                    } else {
                        throw ApplicationException("Missing random seed after --seed");
                    }
                }
                
                if (arg == "--stdp-weights") {
                    if (i + 2 < argc) {
                        options.stdpLtpWeight = std::stof(argv[++i]);
                        options.stdpLtdWeight = std::stof(argv[++i]);
                    } else {
                        throw ApplicationException("Missing STDP weights after --stdp-weights");
                    }
                }
                
                // Verbosity level
                if (arg == "--verbose" || arg == "-V") {
                    options.verbosity++;
                }
                
                if (arg == "--quiet" || arg == "-q") {
                    options.verbosity = 0;
                }
                
                // Key=value pairs
                if (arg.find("--") == 0 && arg.find("=") != std::string::npos && 
                    arg != "--config" && arg != "--output" && arg != "--checkpoint" &&
                    arg != "--stdp-weights") {
                    
                    size_t pos = arg.find("=");
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    options.extraConfig[key] = value;
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing command line: " << e.what() << std::endl;
            return false;
        }
    }
    
    void printUsage() const {
        std::cout << "=== NLM - Improved Neural Learning Machine ===" << std::endl;
        std::cout << "Usage: nlm [--help] [--config <file>] [--output <file>] [--checkpoint <path>]" << std::endl;
        std::cout << "              [--visualization] [--checkpointing] [--replay] [--development]" << std::endl;
        std::cout << "              [--experiment <type>] [--neurons <count>] [--steps <steps>]" << std::endl;
        std::cout << "              [--timestep <dt>] [--seed <seed>] [--stdp-weights <ltp> <ltd>]" << std::endl;
        std::cout << "              [--verbose | -V | --quiet | -q] [key=value ...]" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Experiment Types:" << std::endl;
        std::cout << "  1: Basic Connectivity Test" << std::endl;
        std::cout << "  2: Plasticity Learning Experiment" << std::endl;
        std::cout << "  3: STDP Verification" << std::endl;
        std::cout << "  4: Full Integration Demo" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --experiment 1 --neurons 100 --steps 500" << std::endl;
        std::cout << "  nlm --experiment 2 --config my_config.cfg --checkpointing" << std::endl;
        std::cout << "  nlm --experiment 4 --verbose --development" << std::endl;
    }
    
    std::shared_ptr<ImprovedConfig::Config> createConfiguration(const CommandLineOptions& options) {
        try {
            // Create configuration with defaults
            auto config = std::make_shared<ImprovedConfig::Config>();
            
            // Apply Phase 2 defaults
            config->setInt("random_seed", options.randomSeed);
            config->setDouble("simulation_timestep", options.timestep);
            config->setInt64("neuron_count", static_cast<int64_t>(options.neuronCount));
            config->setInt64("region_count", 1);
            config->setDouble("connection_probability", 0.15f);
            
            // STDP parameters
            config->setDouble("stdp_ltp_weight", options.stdpLtpWeight);
            config->setDouble("stdp_ltd_weight", options.stdpLtdWeight);
            config->setDouble("stdp_tau", options.stdpTimeConstant);
            
            // Structural plasticity parameters
            config->setDouble("synaptogenesis_rate", options.synaptogenesisRate);
            config->setDouble("pruning_rate", options.pruningRate);
            
            // Integration parameters
            config->setInt64("replay_interval", 100);
            config->setInt64("consolidation_interval", 1000);
            
            // Optional features
            if (!options.checkpointPath.empty()) {
                config->setString("checkpoint_dir", options.checkpointPath);
                config->setInt64("checkpoint_interval", 10000);
            }
            
            // Load configuration file if specified
            if (options.configFile != "configs/default.cfg") {
                if (!config->loadFromFile(options.configFile)) {
                    Logger::warning("Failed to load configuration from file: " + options.configFile);
                }
            }
            
            // Apply extra configuration from command line
            for (const auto& [key, value] : options.extraConfig) {
                config->setFromString(key, value);
            }
            
            // Validate configuration
            if (!config->validate()) {
                throw ApplicationException("Configuration validation failed");
            }
            
            Logger::info("Configuration loaded successfully");
            return config;
            
        } catch (const std::exception& e) {
            Logger::error(std::string("Failed to create configuration: ") + e.what());
            throw;
        }
    }
    
    void runBasicConnectivityTest(std::shared_ptr<ImprovedConfig::Config> config) {
        Logger::info("=== Basic Connectivity Test ===");
        
        // Create brain with minimal configuration
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw ApplicationException("Failed to initialize brain for basic connectivity test");
        }
        
        // Get region and neurons
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw ApplicationException("Failed to get region");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            throw ApplicationException("No neurons found in region");
        }
        
        // Inject current into first few neurons
        size_t testNeurons = std::min<size_t>(10, neurons.size());
        Logger::info("Injecting current into " + std::to_string(testNeurons) + " neurons...");
        
        for (size_t i = 0; i < testNeurons; ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run simulation steps
        size_t spikes = 0;
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            if (brain->getFiringNeuronCount() > 0) {
                spikes++;
            }
        }
        
        Logger::info("Basic connectivity test results:");
        Logger::info("  Steps: 100");
        Logger::info("  Spiking steps: " + std::to_string(spikes));
        Logger::info("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + "/step");
        
        if (spikes > 0) {
            Logger::info("✓ Basic connectivity: Network shows activity-dependent firing");
        } else {
            Logger::warning("✗ No spikes detected - checking neuron parameters");
            for (size_t i = 0; i < std::min<size_t>(3, neurons.size()); ++i) {
                const auto& state = neurons[i]->getState();
                Logger::info("    Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(state.membranePotential) +
                           " thresh=" + std::to_string(state.threshold));
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<ImprovedConfig::Config> config) {
        Logger::info("=== Plasticity Learning Experiment ===");
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw ApplicationException("Failed to initialize brain for plasticity experiment");
        }
        
        // Record initial weights
        std::vector<float> initialWeights;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (initialWeights.empty()) {
            throw ApplicationException("No synapses found for plasticity experiment");
        }
        
        float initialMean = Statistics::mean(initialWeights);
        Logger::info("Initial mean weight: " + std::to_string(initialMean));
        
        // Enable plasticity on all synapses
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern
        Logger::info("Applying repeated input patterns (500 steps)...");
        
        float totalReward = 0.0f;
        for (SimulationStep step = 0; step < 500; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            // Simulate reward based on network activity
            float firingRate = brain->getAverageFiringRate();
            float reward = std::min(firingRate / 10.0f, 1.0f);
            totalReward += reward;
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        float finalMean = Statistics::mean(finalWeights);
        Logger::info("Final mean weight: " + std::to_string(finalMean));
        
        // Compute statistics
        float weightChange = finalMean - initialMean;
        Logger::info("Weight change: " + std::to_string(weightChange));
        
        // Count strengthened/weakened synapses
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) strengthened++;
            else if (delta < -0.01f) weakened++;
            else unchanged++;
        }
        
        Logger::info("Plasticity experiment results:");
        Logger::info("  Initial mean weight: " + std::to_string(initialMean));
        Logger::info("  Final mean weight: " + std::to_string(finalMean));
        Logger::info("  Weight change: " + std::to_string(weightChange));
        Logger::info("  Strengthened synapses: " + std::to_string(strengthened));
        Logger::info("  Weakened synapses: " + std::to_string(weakened));
        Logger::info("  Unchanged synapses: " + std::to_string(unchanged));
        Logger::info("  Average reward per step: " + std::to_string(totalReward / 500.0f));
        
        bool learningOccurred = std::abs(weightChange) > 0.001f || 
                               (strengthened > 0 || weakened > 0);
        
        if (learningOccurred) {
            Logger::info("✓ Plasticity: Network demonstrates measurable synaptic changes");
        } else {
            Logger::warning("✗ No significant learning detected");
        }
    }
    
    void runSTDPExperiment(std::shared_ptr<ImprovedConfig::Config> config) {
        Logger::info("=== STDP Verification Experiment ===");
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw ApplicationException("Failed to initialize brain for STDP experiment");
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw ApplicationException("Failed to get region");
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            Logger::warning("Not enough synapses for STDP test, creating minimal network");
            return;
        }
        
        Logger::info("Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            Logger::info("  Synapse " + std::to_string(i) + 
                         " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post
        Logger::info("Creating correlated pre->post activity (50 trials)...");
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            Logger::warning("Not enough neurons for STDP test");
            return;
        }
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        Logger::info("After correlated activity:");
        float totalDelta = 0.0f;
        
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            Logger::info("  Synapse " + std::to_string(i) + 
                         " after: " + std::to_string(synapses[i]->getWeight()) +
                         " (Δ=" + std::to_string(delta) + ")");
        }
        
        Logger::info("");
        if (totalDelta > 0.001f) {
            Logger::info("✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            Logger::info("✗ STDP reversed: Check parameters");
        } else {
            Logger::info("✗ No change: STDP may not be triggering");
        }
    }
    
    void runFullIntegrationDemo(std::shared_ptr<ImprovedConfig::Config> config, 
                               const CommandLineOptions& options) {
        Logger::info("=== Full Integration Demo (Phase 6) ===");
        
        // Import Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = config->getOr<int64_t>("neuron_count", 500);
        phaseConfig.maxSteps = options.maxSteps;
        phaseConfig.enableCheckpointing = options.enableCheckpointing;
        phaseConfig.enableReplay = options.enableReplay;
        phaseConfig.enableDevelopment = options.enableDevelopment;
        phaseConfig.connectionProbability = config->getOr<float>("connection_probability", 0.15f);
        
        if (options.enableCheckpointing && !options.checkpointPath.empty()) {
            phaseConfig.checkpointPath = options.checkpointPath;
        }
        
        // Run the integration experiment
        auto result = experiment.run(phaseConfig);
        
        Logger::info("=== FINAL INTEGRATION RESULTS ===");
        Logger::info("Total reward: " + std::to_string(result.totalReward));
        Logger::info("Avg firing rate: " + std::to_string(result.avgFiringRate));
        Logger::info("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        Logger::info("Dopamine level: " + std::to_string(result.dopamineLevel));
        Logger::info("");
Logger::info("=== INTEGRATION STATUS ===");
        Logger::info("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        Logger::info("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        Logger::info("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
        Logger::info("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        Logger::info("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
        Logger::info("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
        Logger::info("")
        Logger::info("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        
        // Export results if requested
        if (!options.outputFile.empty()) {
            exportResults(result, options.outputFile);
            Logger::info("Results exported to: " + options.outputFile);
        }
    }
    
    void exportResults(const Phase6IntegrationResult& result, const std::string& filepath) {
        // Simple JSON export for results
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ApplicationException("Failed to open output file: " + filepath);
        }
        
        json jsonResult;
        jsonResult["total_reward"] = result.totalReward;
        jsonResult["avg_firing_rate"] = result.avgFiringRate;
        jsonResult["memory_episodes_stored"] = result.memoryEpisodesStored;
        jsonResult["dopamine_level"] = result.dopamineLevel;
        jsonResult["novelty_level"] = result.noveltyLevel;
        jsonResult["curiosity_level"] = result.curiosityLevel;
        jsonResult["wall_clock_time"] = result.totalWallClockTime;
        jsonResult["memory_working_memory_integrated"] = result.memoryWorkingMemoryIntegrated;
        jsonResult["memory_episodic_memory_integrated"] = result.memoryEpisodicMemoryIntegrated;
        jsonResult["neuromodulation_integrated"] = result.neuromodulationIntegrated;
        jsonResult["prediction_integrated"] = result.predictionIntegrated;
        jsonResult["development_integrated"] = result.developmentIntegrated;
        jsonResult["checkpointing_works"] = result.checkpointingWorks;
        jsonResult["timestamp"] = std::time(nullptr);
        
        file << jsonResult.dump(2) << std::endl;
    }
    
    bool runApplication(int argc, char** argv) {
        try {
            // Parse command line
            CommandLineOptions options;
            if (!parseCommandLine(argc, argv, options)) {
                return false;  // Help was shown
            }
            
            // Initialize logging
            if (options.enableLogging) {
                Logger::getGlobal().setLevel(options.verbosity == 3 ? LogLevel::Debug : 
                                           options.verbosity == 2 ? LogLevel::Info : 
                                           LogLevel::Warning);
            } else {
                Logger::getGlobal().setLevel(LogLevel::Error);
            }
            
            Logger::info("Starting NLM improved application...");
            Logger::info("Experiment type: " + std::to_string(options.experimentType));
            
            // Create configuration
            auto config = createConfiguration(options);
            
            // Set verbosity level in config
            config->setInt("verbosity", options.verbosity);
            
            // Run selected experiment
            switch (options.experimentType) {
                case 1:
                    runBasicConnectivityTest(config);
                    break;
                case 2:
                    runPlasticityExperiment(config);
                    break;
                case 3:
                    runSTDPExperiment(config);
                    break;
                case 4:
                    runFullIntegrationDemo(config, options);
                    break;
                default:
                    throw ApplicationException("Invalid experiment type: " + std::to_string(options.experimentType));
            }
            
            Logger::info("Application completed successfully");
            return true;
            
        } catch (const std::exception& e) {
            Logger::error(std::string("Application failed: ") + e.what());
            return false;
        }
    }
    
private:
    void initializeDefaultConfigSchema() {
        // Initialize default configuration schema
        // This is a simplified schema for demonstration
        // A real implementation would have more comprehensive schema validation
    }
    
    std::unordered_map<std::string, std::string> getDescriptionMap() const {
        return {
            {"random_seed", "Random seed for reproducible simulations"},
            {"simulation_timestep", "Time step for simulation (in seconds)"},
            {"neuron_count", "Number of neurons in the network"},
            {"region_count", "Number of neural regions"},
            {"connection_probability", "Probability of synapse formation between neurons"},
            {"stdp_ltp_weight", "STDP Long-Term Potentiation weight"},
            {"stdp_ltd_weight", "STDP Long-Term Depression weight"},
            {"stdp_tau", "STDP time constant (in ms)"},
            {"synaptogenesis_rate", "Rate of new synapse formation"},
            {"pruning_rate", "Rate of synapse elimination"},
            {"verbosity", "Logging verbosity level (0=silent, 1=normal, 2=verbose, 3=debug)"}
        };
    }
};

} // namespace nlm

#endif // NLM_IMPROVED_MAIN_H
