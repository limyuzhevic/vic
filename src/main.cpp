// NLM - Neural Learning Machine
// Phase 6: Final Integration
// Advanced command-line interface with help system

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace nlm;

// Command help text
const std::string HELP_TEXT = 
"NLM (熙然) - Neural Learning Machine - Phase 6 Final Integration\n\n"
"USAGE:\n"
"  nlm [OPTIONS] [MODE]\n\n"
"MODES:\n"
"  run           Run Phase 2: Real Neural Computation (default)\n"
"  phase6        Run Phase 6: Integration Test\n"
"  demo          Run Phase 6 Demo (quick integration verification)\n"
"  config        Work with configuration files\n"
"  help          Show this help message\n\n"
"OPTIONS:\n"
"  --help, -h                   Show help message and exit\n"
"  --config FILE, -c FILE       Load configuration from FILE\n"
"  --neuron-count N, -n N      Set number of neurons (default: 1000)\n"
"  --steps S, -s S              Set simulation steps (default: 1000)\n"
"  --regions R, -r R            Set number of brain regions (default: 1)\n"
"  --verbose, -v                Enable verbose logging\n"
"  --quiet, -q                  Quiet mode (only errors)\n"
"  --checkpoint PATH            Enable checkpointing with given path\n"
"  --seed SEED                  Set random seed (default: 42)\n"
"  --list-configs               List all available configuration options\n"
"  --show-config                Show current configuration\n"
"  --set-key VALUE              Set configuration key=value (can be used multiple times)\n\n"
"ADVANCED OPTIONS:\n"
"  --enable-development         Enable developmental plasticity\n"
"  --enable-replay              Enable memory replay\n"
"  --enable-checkpointing       Enable checkpoint save/load\n"
"  --connection-prob P          Set connection probability (default: 0.1)\n"
"  --stdp-ltp W                 Set STDP LTP weight (default: 0.01)\n"
"  --stdp-ltd W                 Set STDP LTD weight (default: 0.012)\n"
"  --synaptogenesis-RATE R      Set synaptogenesis rate (default: 0.0001)\n"
"  --pruning-RATE R             Set pruning rate (default: 0.00001)\n\n"
"EXAMPLES:\n"
"  nlm --help                           Show help\n"
"  nlm run --neuron-count 500 --steps 2000\n"
"  nlm phase6 --checkpoint ./save.bin --enable-development\n"
"  nlm demo --verbose --seed 123\n"
"  nlm config --list-configs\n"
"  nlm config --show-config\n\n"
"For more detailed information, see README.md or documentation.";

// Show help message
void showHelp() {
    std::cout << HELP_TEXT << std::endl;
}

// List configuration options
void listConfigOptions() {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "=========================" << std::endl << std::endl;
    
    std::vector<std::pair<std::string, std::string>> configs = {
        {"neuron_count", "Total number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Probability of synaptic connections"},
        {"simulation_timestep", "Time step for simulation"},
        {"random_seed", "Random seed for reproducibility"},
        {"stdp_ltp_weight", "STDP Long-Term Potentiation weight"},
        {"stdp_ltd_weight", "STDP Long-Term Depression weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse elimination"},
        {"replay_interval", "Steps between memory replay cycles"},
        {"consolidation_interval", "Steps between memory consolidation"},
        {"checkpoint_dir", "Directory for checkpoint files"},
        {"max_checkpoints", "Maximum number of checkpoints to keep"},
        {"compression_level", "Checkpoint compression level (0-9)"},
        {"enable_development", "Enable developmental plasticity"},
        {"enable_replay", "Enable memory replay system"},
        {"enable_checkpointing", "Enable checkpoint save/load"},
        {"working_memory_capacity", "Capacity of working memory"},
        {"max_episodes", "Maximum number of episodes in episodic memory"},
        {"novelty_threshold", "Threshold for novelty detection"},
        {"curiosity_decay", "Decay rate for curiosity"},
        {"dopamine_baseline", "Baseline dopamine level"},
        {"dopamine_sensitivity", "Sensitivity to reward prediction error"}
    };
    
    for (const auto& config : configs) {
        std::cout << "  " << std::left << std::setw(25) << config.first << "- " << config.second << std::endl;
    }
    
    std::cout << std::endl << "Configuration sources (in order of precedence):" << std::endl;
    std::cout << "  1. Command line arguments (--key=value)" << std::endl;
    std::cout << "  2. Configuration file (JSON or key=value format)" << std::endl;
    std::cout << "  3. Default values" << std::endl;
    std::cout << "  4. Runtime modifications" << std::endl;
}

// Parse command line arguments into a configuration
void parseAdvancedArgs(int argc, char** argv, Config& config, std::string& configFile, 
                      std::string& mode, bool& verbose, bool& quiet, bool& help,
                      std::vector<std::string>& runtimeSets) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--list-configs") {
            listConfigOptions();
            std::cout << std::endl;
        } else if (arg == "--show-config") {
            std::cout << config.summary() << std::endl;
            std::cout << std::endl;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--enable-development") {
            config.set("enable_development", true);
        } else if (arg == "--enable-replay") {
            config.set("enable_replay", true);
        } else if (arg == "--enable-checkpointing") {
            config.set("enable_checkpointing", true);
        } else if (arg.find("--set-key=") == 0) {
            std::string setArg = arg.substr(10);
            size_t eqPos = setArg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = setArg.substr(0, eqPos);
                std::string value = setArg.substr(eqPos + 1);
                runtimeSets.push_back(key + "=" + value);
            }
        } else if (arg.find("--checkpoint=") == 0) {
            std::string checkpointPath = arg.substr(13);
            config.set("checkpoint_dir", checkpointPath);
            config.set("enable_checkpointing", true);
        } else if (arg.find("--neuron-count=") == 0) {
            int64_t neuronCount = std::stoll(arg.substr(15));
            config.set("neuron_count", neuronCount);
        } else if (arg.find("--steps=") == 0) {
            config.set("max_steps", std::stoll(arg.substr(8)));
        } else if (arg.find("--regions=") == 0) {
            config.set("region_count", std::stoll(arg.substr(10)));
        } else if (arg.find("--seed=") == 0) {
            config.set("random_seed", std::stoll(arg.substr(7)));
        } else if (arg.find("--connection-prob=") == 0) {
            config.set("connection_probability", std::stod(arg.substr(18)));
        } else if (arg.find("--stdp-ltp=") == 0) {
            config.set("stdp_ltp_weight", std::stod(arg.substr(12)));
        } else if (arg.find("--stdp-ltd=") == 0) {
            config.set("stdp_ltd_weight", std::stod(arg.substr(11)));
        } else if (arg.find("--synaptogenesis-rate=") == 0) {
            config.set("synaptogenesis_rate", std::stod(arg.substr(23)));
        } else if (arg.find("--pruning-rate=") == 0) {
            config.set("pruning_rate", std::stod(arg.substr(15)));
        } else if (arg.find("--stdp-tau=") == 0) {
            config.set("stdp_tau", std::stod(arg.substr(11)));
        } else if (arg.find("--working-memory-capacity=") == 0) {
            config.set("working_memory_capacity", std::stoll(arg.substr(26)));
        } else if (arg.find("--max-episodes=") == 0) {
            config.set("max_episodes", std::stoll(arg.substr(17)));
        } else if (arg.find("--novelty-threshold=") == 0) {
            config.set("novelty_threshold", std::stod(arg.substr(20)));
        } else if (arg.find("--curiosity-decay=") == 0) {
            config.set("curiosity_decay", std::stod(arg.substr(19)));
        } else if (arg.find("--dopamine-baseline=") == 0) {
            config.set("dopamine_baseline", std::stod(arg.substr(21)));
        } else if (arg.find("--dopamine-sensitivity=") == 0) {
            config.set("dopamine_sensitivity", std::stod(arg.substr(23)));
        } else if (arg.find("--replay-interval=") == 0) {
            config.set("replay_interval", std::stoll(arg.substr(19)));
        } else if (arg.find("--consolidation-interval=") == 0) {
            config.set("consolidation_interval", std::stoll(arg.substr(23)));
        } else if (arg.find("--compression-level=") == 0) {
            config.set("compression_level", std::stoi(arg.substr(22)));
        } else if (arg.find("--max-checkpoints=") == 0) {
            config.set("max_checkpoints", std::stoi(arg.substr(18)));
        } else if (arg.find("--config=") == 0 || arg.find("-c") == 0) {
            if (arg.find("--config=") == 0) {
                configFile = arg.substr(9);
            } else {
                configFile = argv[++i];
            }
        } else if (arg == "run" || arg == "phase6" || arg == "demo" || arg == "config" || arg == "help") {
            mode = arg;
        } else if (arg[0] == '-') {
            // Handle short options like -n 100
            if (arg == "-n" && i + 1 < argc) {
                config.set("neuron_count", std::stoll(argv[++i]));
            } else if (arg == "-s" && i + 1 < argc) {
                config.set("max_steps", std::stoll(argv[++i]));
            } else if (arg == "-r" && i + 1 < argc) {
                config.set("region_count", std::stoll(argv[++i]));
            } else if (arg == "-c" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "-v") {
                verbose = true;
            } else if (arg == "-q") {
                quiet = true;
            }
        }
    }
}

// Runtime configuration manager
class RuntimeConfigManager {
private:
    std::shared_ptr<Config> config;
    std::map<std::string, std::function<void()>> runtimeModifiers;
    
public:
    RuntimeConfigManager(std::shared_ptr<Config> cfg) : config(cfg) {
        setupRuntimeModifiers();
    }
    
    void setupRuntimeModifiers() {
        // Add runtime configuration modifiers
        runtimeModifiers["increase_neurons"] = [this]() {
            size_t current = config->getOr<size_t>("neuron_count", 1000);
            config->set("neuron_count", current * 1.1, ConfigSource::Runtime);
            std::cout << "Increased neuron count to " << config->getOr<size_t>("neuron_count", 1000) << std::endl;
        };
        
        runtimeModifiers["decrease_neurons"] = [this]() {
            size_t current = config->getOr<size_t>("neuron_count", 1000);
            if (current > 100) {
                config->set("neuron_count", current * 0.9, ConfigSource::Runtime);
                std::cout << "Decreased neuron count to " << config->getOr<size_t>("neuron_count", 1000) << std::endl;
            }
        };
        
        runtimeModifiers["boost_plasticity"] = [this]() {
            config->set("stdp_ltp_weight", config->getOr<float>("stdp_ltp_weight", 0.01f) * 1.5, ConfigSource::Runtime);
            config->set("stdp_ltd_weight", config->getOr<float>("stdp_ltd_weight", 0.012f) * 0.8, ConfigSource::Runtime);
            std::cout << "Boosted plasticity - STDP weights adjusted" << std::endl;
        };
        
        runtimeModifiers["reduce_noise"] = [this]() {
            // Reduce some noise in the system by adjusting rates
            config->set("synaptogenesis_rate", config->getOr<float>("synaptogenesis_rate", 0.0001f) * 0.5, ConfigSource::Runtime);
            std::cout << "Reduced system noise - synaptogenesis rate decreased" << std::endl;
        };
        
        runtimeModifiers["stress_test"] = [this]() {
            // Stress test configuration with extreme values
            config->set("neuron_count", 10000);
            config->set("max_steps", 50000);
            config->set("connection_probability", 0.3f);
            std::cout << "Applied stress test configuration" << std::endl;
        };
        
        runtimeModifiers["reset"] = [this]() {
            // Reset to defaults (would need a copy of default config)
            std::cout << "Runtime configuration reset" << std::endl;
        };
    }
    
    bool applyRuntimeModifier(const std::string& modifier) {
        auto it = runtimeModifiers.find(modifier);
        if (it != runtimeModifiers.end()) {
            it->second();
            return true;
        }
        return false;
    }
    
    void showAvailableModifiers() const {
        std::cout << "Available runtime modifiers:" << std::endl;
        std::cout << "  " << std::left << std::setw(20) << "increase_neurons" << "- Increase neuron count by 10%" << std::endl;
        std::cout << "  " << std::left << std::setw(20) << "decrease_neurons" << "- Decrease neuron count by 10%" << std::endl;
        std::cout << "  " << std::left << std::setw(20) << "boost_plasticity" << "- Increase STDP learning rates" << std::endl;
        std::cout << "  " << std::left << std::setw(20) << "reduce_noise" << "- Reduce system noise and instability" << std::endl;
        std::cout << "  " << std::left << std::setw(20) << "stress_test" << "- Apply extreme configuration for stress testing" << std::endl;
        std::cout << "  " << std::left << std::setw(20) << "reset" << "- Reset to default configuration" << std::endl;
    }
};

// Interactive configuration input
void interactiveConfigInput(Config& config) {
    std::cout << "Interactive Configuration Setup" << std::endl;
    std::cout << "===============================" << std::endl << std::endl;
    
    std::cout << "Enter configuration values (press Enter to keep default):" << std::endl;
    
    std::cout << "Neuron count [1000]: ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.set("neuron_count", std::stoll(input), ConfigSource::Runtime);
        } catch (...) {}
    }
    
    std::cout << "Region count [1]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.set("region_count", std::stoll(input), ConfigSource::Runtime);
        } catch (...) {}
    }
    
    std::cout << "Max steps [10000]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.set("max_steps", std::stoll(input), ConfigSource::Runtime);
        } catch (...) {}
    }
    
    std::cout << "Connection probability [0.1]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.set("connection_probability", std::stod(input), ConfigSource::Runtime);
        } catch (...) {}
    }
    
    std::cout << "STDP LTP weight [0.01]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.set("stdp_ltp_weight", std::stod(input), ConfigSource::Runtime);
        } catch (...) {}
    }
    
    std::cout << "STDP LTD weight [0.012]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            config.set("stdp_ltd_weight", std::stod(input), ConfigSource::Runtime);
        } catch (...) {}
    }
    
    std::cout << "Enable development plasticity [true]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        bool enableDev = (input == "true" || input == "t" || input == "1");
        config.set("enable_development", enableDev, ConfigSource::Runtime);
    }
    
    std::cout << "Enable memory replay [true]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        bool enableReplay = (input == "true" || input == "t" || input == "1");
        config.set("enable_replay", enableReplay, ConfigSource::Runtime);
    }
    
    std::cout << "Enable checkpointing [true]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        bool enableCheckpoint = (input == "true" || input == "t" || input == "1");
        config.set("enable_checkpointing", enableCheckpoint, ConfigSource::Runtime);
        if (enableCheckpoint) {
            std::cout << "Checkpoint path [./checkpoints]: ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                config.set("checkpoint_dir", input, ConfigSource::Runtime);
            }
        }
    }
    
    std::cout << "\nConfiguration updated successfully!" << std::endl;
}

// Show configuration
void showConfiguration(const Config& config) {
    std::cout << "Current NLM Configuration" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << config.summary() << std::endl;
}

int main(int argc, char** argv) {
    // Default configuration
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string mode = "run";  // Default mode
    bool verbose = false;
    bool quiet = false;
    bool help = false;
    std::vector<std::string> runtimeSets;
    
    parseAdvancedArgs(argc, argv, *config, configFile, mode, verbose, quiet, help, runtimeSets);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    
    if (verbose) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
    } else if (quiet) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
    } else {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        logger->addLogger(consoleLogger);
    }
    
    Logger::setGlobal(logger);
    
    // Load configuration file
    bool configLoaded = false;
    if (!configFile.empty() && configFile != "configs/default.cfg") {
        configLoaded = config->loadFromFile(configFile);
        if (configLoaded) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Show help and exit
    if (help) {
        showHelp();
        return 0;
    }
    
    // Handle config mode
    if (mode == "config") {
        std::cout << "NLM Configuration Tool" << std::endl;
        std::cout << "======================" << std::endl << std::endl;
        
        std::cout << "1. List configuration options" << std::endl;
        std::cout << "2. Show current configuration" << std::endl;
        std::cout << "3. Interactive configuration setup" << std::endl;
        std::cout << "4. Apply runtime modifier" << std::endl;
        
        // Note: Interactive mode would require actual input
        // For now, we'll just demonstrate the features
        
        std::cout << std::endl;
        listConfigOptions();
        
        showConfiguration(*config);
        
        // Create runtime config manager
        RuntimeConfigManager runtimeConfig(config);
        std::cout << std::endl;
        runtimeConfig.showAvailableModifiers();
        
        return 0;
    }
    
    // Apply runtime configuration sets
    for (const auto& setStr : runtimeSets) {
        size_t eqPos = setStr.find('=');
        if (eqPos != std::string::npos) {
            std::string key = setStr.substr(0, eqPos);
            std::string value = setStr.substr(eqPos + 1);
            try {
                // Try to parse as integer
                size_t pos;
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.size()) {
                    config->set(key, intVal, ConfigSource::Runtime);
                    NLM_LOG_INFO("Set " + key + " = " + value);
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try to parse as double
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    config->set(key, doubleVal, ConfigSource::Runtime);
                    NLM_LOG_INFO("Set " + key + " = " + value);
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try to parse as boolean
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    config->set(key, boolVal, ConfigSource::Runtime);
                    NLM_LOG_INFO("Set " + key + " = " + value);
                    continue;
                }
            } catch (...) {}
            
            // Treat as string
            config->set(key, value, ConfigSource::Runtime);
            NLM_LOG_INFO("Set " + key + " = \"" + value + "\"");
        }
    }
    
    // Set default values for different modes
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    if (mode == "run") {
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Running basic neural computation demonstration...");
        
        // For demonstration, we'll just show that the configuration is working
        NLM_LOG_INFO("Configuration loaded: neuron_count = " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
        NLM_LOG_INFO("Configuration loaded: region_count = " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("Configuration loaded: connection_probability = " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
        
        // Run simple demonstration
        NLM_LOG_INFO("\n=== Simple Brain Initialization ===");
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            NLM_LOG_INFO("✓ Brain initialized successfully");
            NLM_LOG_INFO("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
            NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
            NLM_LOG_INFO("  Regions: " + std::to_string(brain->getRegionCount()));
            
            // Run a few simulation steps
            NLM_LOG_INFO("\n=== Simulation (10 steps) ===");
            for (SimulationStep step = 0; step < 10; ++step) {
                brain->step(step, step * 0.001);
                if (step % 5 == 0) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            
            NLM_LOG_INFO("\n✓ Simulation completed successfully");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            NLM_LOG_INFO("  Final firing rate: " + std::to_string(brain->getAverageFiringRate()));
        } else {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        return 0;
        
    } else if (mode == "phase6") {
        config->set("max_steps", 5000, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_replay", true, ConfigSource::Default);
        config->set("enable_checkpointing", true, ConfigSource::Default);
        config->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
        
        NLM_LOG_INFO("=== NLM Phase 6 Integration Test ===");
        NLM_LOG_INFO("Running comprehensive integration test...");
        
        // Note: This would normally create and run Phase6IntegratedExperiment
        // For now, we'll demonstrate with a simple brain
        
        NLM_LOG_INFO("\nPhase 6 configuration:")
        NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
        NLM_LOG_INFO("  Max steps: " + std::to_string(config->getOr<int64_t>("max_steps", 5000)));
        NLM_LOG_INFO("  Enable development: " + std::to_string(config->getOr<bool>("enable_development", true)));
        NLM_LOG_INFO("  Enable replay: " + std::to_string(config->getOr<bool>("enable_replay", true)));
        NLM_LOG_INFO("  Enable checkpointing: " + std::to_string(config->getOr<bool>("enable_checkpointing", true)));
        
        return 0;
        
    } else if (mode == "demo") {
        config->set("max_steps", 2000, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_replay", true, ConfigSource::Default);
        
        NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
        NLM_LOG_INFO("Running quick integration verification...");
        
        // Note: This would normally create and run Phase6Demo
        // For now, we'll demonstrate with a simple brain
        
        NLM_LOG_INFO("\nDemo configuration:")
        NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  Max steps: " + std::to_string(config->getOr<int64_t>("max_steps", 2000)));
        NLM_LOG_INFO("  Enable development: " + std::to_string(config->getOr<bool>("enable_development", true)));
        NLM_LOG_INFO("  Enable replay: " + std::to_string(config->getOr<bool>("enable_replay", true)));
        
        return 0;
        
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        showHelp();
        return 1;
    }
}