// NLM Enhanced Command-Line Interface
// CLIInterface implementation for Phase 6

#include "core/CLI/CLIInterface.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>

namespace nlm {

CLIInterface::CLIInterface(int argc, char** argv) : argc(argc), argv(argv) {
    config = std::make_shared<Config>();
    setDefaults();
}

CLIInterface::~CLIInterface() = default;

bool CLIInterface::parseArguments() {
    // Standard argument parsing
    if (!parseStandardArgs()) {
        return false;
    }
    
    // Advanced argument parsing
    if (!parseAdvancedArgs()) {
        return false;
    }
    
    // Load and apply configuration file
    if (!loadConfigFile()) {
        return false;
    }
    
    // Apply CLI options to config
    applyCLIToConfig(config);
    
    // Validate configuration
    if (!validateConfiguration()) {
        return false;
    }
    
    return true;
}

bool CLIInterface::parseStandardArgs() {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Help requests
        if (arg == "--help" || arg == "-h") {
            std::cout << getHelpText();
            return false; // Exit after help
        }
        
        if (arg == "--advanced-help" || arg == "--help-advanced") {
            std::cout << getAdvancedHelpText();
            return false; // Exit after help
        }
        
        // Configuration file
        if (arg.substr(0, 8) == "--config" || arg.substr(0, 8) == "-config") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                cliConfig.configFile = argv[++i];
            }
        }
        
        // Verbose output
        if (arg == "--verbose" || arg == "-v") {
            cliConfig.verbose = true;
            cliConfig.quiet = false;
        }
        
        // Quiet output
        if (arg == "--quiet" || arg == "-q") {
            cliConfig.quiet = true;
            cliConfig.verbose = false;
        }
        
        // Log file
        if (arg.substr(0, 11) == "--log-file" || arg.substr(0, 11) == "-log-file") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.logFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                cliConfig.logFile = argv[++i];
            }
        }
        
        // Log level
        if (arg.substr(0, 12) == "--log-level" || arg.substr(0, 12) == "-log-level") {
            if (arg.find('=') != std::string::npos) {
                std::string valueStr = arg.substr(arg.find('=') + 1);
                cliConfig.logLevel = std::stoi(valueStr);
            } else if (i + 1 < argc) {
                cliConfig.logLevel = std::stoi(argv[++i]);
            }
        }
        
        // Output directory
        if (arg.substr(0, 13) == "--output-dir" || arg.substr(0, 13) == "-output-dir") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.outputDir = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                cliConfig.outputDir = argv[++i];
            }
        }
        
        // Save results
        if (arg == "--save-results" || arg == "-save") {
            cliConfig.saveResults = true;
        }
        
        // Result format
        if (arg.substr(0, 13) == "--result-fmt" || arg.substr(0, 13) == "-result-fmt") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.resultFormat = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                cliConfig.resultFormat = argv[++i];
            }
        }
        
        // Max steps
        if (arg.substr(0, 11) == "--max-steps" || arg.substr(0, 11) == "-max-steps") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.maxSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.maxSteps = std::stoi(argv[++i]);
            }
        }
        
        // Time limit
        if (arg.substr(0, 12) == "--time-limit" || arg.substr(0, 12) == "-time-limit") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.timeLimit = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.timeLimit = std::stod(argv[++i]);
            }
        }
        
        // Progress updates
        if (arg == "--show-progress" || arg == "-progress") {
            cliConfig.showProgress = true;
        }
        
        // Experiment type
        if (arg.substr(0, 16) == "--experiment-type" || arg.substr(0, 16) == "-experiment-type") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.experimentType = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                cliConfig.experimentType = argv[++i];
            }
        }
        
        // Environment type
        if (arg.substr(0, 13) == "--environment" || arg.substr(0, 13) == "-environment") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.environment = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                cliConfig.environment = argv[++i];
            }
        }
        
        // Advanced feature flags
        if (arg == "--reward-modulation") {
            cliConfig.enableRewardModulation = true;
        }
        
        if (arg == "--structural-plasticity") {
            cliConfig.enableStructuralPlasticity = true;
        }
        
        if (arg == "--development") {
            cliConfig.enableDevelopment = true;
        }
        
        if (arg == "--curiosity") {
            cliConfig.enableCuriosity = true;
        }
        
        if (arg == "--novelty-detection") {
            cliConfig.enableNoveltyDetection = true;
        }
        
        if (arg == "--prediction") {
            cliConfig.enablePrediction = true;
        }
        
        if (arg == "--checkpointing") {
            cliConfig.enableCheckpointing = true;
        }
        
        if (arg == "--replay") {
            cliConfig.enableReplay = true;
        }
    }
    
    return true;
}

bool CLIInterface::parseAdvancedArgs() {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Advanced neural parameters
        if (arg.substr(0, 16) == "--plasticity-rate") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.plasticityLearningRate = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.plasticityLearningRate = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 12) == "--stdp-ltp") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.stdpLTPWeight = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.stdpLTPWeight = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 13) == "--stdp-ltd") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.stdpLTDWeight = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.stdpLTDWeight = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 14) == "--stdp-tau") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.stdpTimeConstant = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.stdpTimeConstant = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 18) == "--synaptogenesis") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.developmentSynaptogenesisRate = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.developmentSynaptogenesisRate = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 13) == "--pruning-rate") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.developmentPruningRate = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.developmentPruningRate = std::stof(argv[++i]);
            }
        }
        
        // Memory parameters
        if (arg.substr(0, 13) == "--mem-capacity") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.workingMemoryCapacity = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.workingMemoryCapacity = std::stoull(argv[++i]);
            }
        }
        
        if (arg.substr(0, 18) == "--mem-episodes") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.episodicMemoryMaxEpisodes = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.episodicMemoryMaxEpisodes = std::stoull(argv[++i]);
            }
        }
        
        // Neuromodulation parameters
        if (arg.substr(0, 12) == "--dopamine-base") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.dopamineBaseline = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.dopamineBaseline = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 18) == "--curiosity-base") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.dopamineBaselineCuriosity = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.dopamineBaselineCuriosity = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 16) == "--novelty-base") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.dopamineBaselineNovelty = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.dopamineBaselineNovelty = std::stof(argv[++i]);
            }
        }
        
        // Reward parameters
        if (arg.substr(0, 15) == "--reward-discount") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.rewardDiscountFactor = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.rewardDiscountFactor = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 17) == "--exploration-bonus") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.explorationBonus = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.explorationBonus = std::stof(argv[++i]);
            }
        }
        
        if (arg.substr(0, 19) == "--exploitation-penalty") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.exploitationPenalty = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.exploitationPenalty = std::stof(argv[++i]);
            }
        }
        
        // Environment parameters
        if (arg.substr(0, 15) == "--env-width") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.environmentWidth = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.environmentWidth = std::stoi(argv[++i]);
            }
        }
        
        if (arg.substr(0, 16) == "--env-height") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.environmentHeight = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.environmentHeight = std::stoi(argv[++i]);
            }
        }
        
        // Performance tuning
        if (arg.substr(0, 15) == "--num-threads") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.numThreads = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.numThreads = std::stoi(argv[++i]);
            }
        }
        
        if (arg.substr(0, 18) == "--thread-gran") {
            if (arg.find('=') != std::string::npos) {
                cliConfig.threadGranularity = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                cliConfig.threadGranularity = std::stof(argv[++i]);
            }
        }
        
        if (arg == "--enable-simd") {
            cliConfig.useSIMD = true;
        }
    }
    
    return true;
}

bool CLIInterface::loadConfigFile() {
    if (cliConfig.configFile.empty()) {
        return true; // No config file specified, use defaults
    }
    
    if (config->loadFromFile(cliConfig.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + cliConfig.configFile);
        return true;
    } else {
        NLM_LOG_WARNING("Failed to load configuration from: " + cliConfig.configFile);
        return true; // Continue with defaults
    }
}

void CLIInterface::setDefaults() {
    // Set default values for CLI configuration
    cliConfig.outputDir = ".";
    cliConfig.resultFormat = "txt";
    cliConfig.showProgress = true;
    
    // Set advanced defaults
    cliConfig.numThreads = 0; // Auto-detect
    cliConfig.threadGranularity = 0.1;
    cliConfig.useSIMD = true;
    
    // Advanced neural parameters
    cliConfig.plasticityLearningRate = 0.01f;
    cliConfig.stdpLTPWeight = 0.01f;
    cliConfig.stdpLTDWeight = 0.012f;
    cliConfig.stdpTimeConstant = 20.0f;
    cliConfig.developmentSynaptogenesisRate = 0.001f;
    cliConfig.developmentPruningRate = 0.0001f;
    
    // Memory parameters
    cliConfig.workingMemoryCapacity = 100;
    cliConfig.episodicMemoryMaxEpisodes = 1000;
    
    // Neuromodulation parameters
    cliConfig.dopamineBaseline = 0.1f;
    cliConfig.dopamineBaselineCuriosity = 0.0f;
    cliConfig.dopamineBaselineNovelty = 0.0f;
    
    // Reward parameters
    cliConfig.rewardDiscountFactor = 0.99f;
    cliConfig.explorationBonus = 0.1f;
    cliConfig.exploitationPenalty = 0.01f;
    
    // Environment parameters
    cliConfig.environmentWidth = 10;
    cliConfig.environmentHeight = 10;
    cliConfig.visionWidth = 8.0f;
    cliConfig.visionHeight = 8.0f;
    
    // Apply CLI config to the config object
    applyCLIToConfig(config);
}

void CLIInterface::applyCLIToConfig(std::shared_ptr<Config>& cfg) {
    // Apply CLI configuration to the config object
    if (!cliConfig.outputDir.empty()) {
        cfg->set("output_dir", cliConfig.outputDir, ConfigSource::CommandLine);
    }
    
    if (cliConfig.maxSteps > 0) {
        cfg->set("max_simulation_steps", static_cast<int64_t>(cliConfig.maxSteps), ConfigSource::CommandLine);
    }
    
    if (cliConfig.timeLimit > 0.0) {
        cfg->set("simulation_time_limit", cliConfig.timeLimit, ConfigSource::CommandLine);
    }
    
    // Apply advanced options
    cfg->set("enable_reward_modulation", cliConfig.enableRewardModulation, ConfigSource::CommandLine);
    cfg->set("enable_structural_plasticity", cliConfig.enableStructuralPlasticity, ConfigSource::CommandLine);
    cfg->set("enable_development", cliConfig.enableDevelopment, ConfigSource::CommandLine);
    cfg->set("enable_curiosity", cliConfig.enableCuriosity, ConfigSource::CommandLine);
    cfg->set("enable_novelty_detection", cliConfig.enableNoveltyDetection, ConfigSource::CommandLine);
    cfg->set("enable_prediction", cliConfig.enablePrediction, ConfigSource::CommandLine);
    cfg->set("enable_checkpointing", cliConfig.enableCheckpointing, ConfigSource::CommandLine);
    cfg->set("enable_replay", cliConfig.enableReplay, ConfigSource::CommandLine);
    
    // Apply neural parameters
    cfg->set("plasticity_learning_rate", cliConfig.plasticityLearningRate, ConfigSource::CommandLine);
    cfg->set("stdp_ltp_weight", cliConfig.stdpLTPWeight, ConfigSource::CommandLine);
    cfg->set("stdp_ltd_weight", cliConfig.stdpLTDWeight, ConfigSource::CommandLine);
    cfg->set("stdp_time_constant", cliConfig.stdpTimeConstant, ConfigSource::CommandLine);
    cfg->set("synaptogenesis_rate", cliConfig.developmentSynaptogenesisRate, ConfigSource::CommandLine);
    cfg->set("pruning_rate", cliConfig.developmentPruningRate, ConfigSource::CommandLine);
    
    // Apply memory parameters
    cfg->set("working_memory_capacity", static_cast<int64_t>(cliConfig.workingMemoryCapacity), ConfigSource::CommandLine);
    cfg->set("episodic_memory_max_episodes", static_cast<int64_t>(cliConfig.episodicMemoryMaxEpisodes), ConfigSource::CommandLine);
    
    // Apply neuromodulation parameters
    cfg->set("dopamine_baseline", cliConfig.dopamineBaseline, ConfigSource::CommandLine);
    cfg->set("dopamine_baseline_curiosity", cliConfig.dopamineBaselineCuriosity, ConfigSource::CommandLine);
    cfg->set("dopamine_baseline_novelty", cliConfig.dopamineBaselineNovelty, ConfigSource::CommandLine);
    
    // Apply reward parameters
    cfg->set("reward_discount_factor", cliConfig.rewardDiscountFactor, ConfigSource::CommandLine);
    cfg->set("exploration_bonus", cliConfig.explorationBonus, ConfigSource::CommandLine);
    cfg->set("exploitation_penalty", cliConfig.exploitationPenalty, ConfigSource::CommandLine);
    
    // Apply environment parameters
    cfg->set("environment_width", static_cast<int64_t>(cliConfig.environmentWidth), ConfigSource::CommandLine);
    cfg->set("environment_height", static_cast<int64_t>(cliConfig.environmentHeight), ConfigSource::CommandLine);
    cfg->set("vision_width", cliConfig.visionWidth, ConfigSource::CommandLine);
    cfg->set("vision_height", cliConfig.visionHeight, ConfigSource::CommandLine);
}

std::string CLIInterface::getHelpText() {
    return R"(
=== NLM Command-Line Interface Help ===

Standard Usage:
  nlm [options]
  nlm --config=filename.cfg [options]
  nlm -c filename.cfg [options]

Basic Options:
  -h, --help                    Show this help message
  -v, --verbose                 Enable verbose logging
  -q, --quiet                   Suppress non-essential output
  --log-file=FILE              Log output to file
  --log-level=LEVEL            Set log level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
  -c, --config=FILE             Load configuration from file
  --output-dir=DIR             Output directory for results
  --save-results               Save simulation results to disk
  --result-fmt=FORMAT          Result format: txt, json, csv
  --max-steps=N                Maximum simulation steps (0 = unlimited)
  --time-limit=SECONDS         Time limit in seconds (0 = unlimited)
  --show-progress              Show progress updates during simulation
  --experiment-type=TYPE        Experiment type: demo, integration, full
  --environment=TYPE           Environment type: simple, complex, custom

Advanced Options:
  --reward-modulation          Enable reward-based learning
  --structural-plasticity      Enable synaptic growth and pruning
  --development                Enable developmental processes
  --curiosity                  Enable curiosity-driven exploration
  --novelty-detection          Enable novelty detection
  --prediction                  Enable predictive coding
  --checkpointing               Enable checkpoint saving
  --replay                      Enable memory replay

Performance Options:
  --num-threads=N              Number of threads for parallel processing
  --thread-gran=GRAIN          Thread granularity for work distribution
  --enable-simd                Enable SIMD optimizations

Neural Parameters:
  --plasticity-rate=RATE        Plasticity learning rate (default: 0.01)
  --stdp-ltp=WEIGHT            STDP LTP weight (default: 0.01)
  --stdp-ltd=WEIGHT            STDP LTD weight (default: 0.012)
  --stdp-tau=TAU               STDP time constant (default: 20.0)
  --synaptogenesis=RATE         Synaptogenesis rate (default: 0.001)
  --pruning-rate=RATE          Pruning rate (default: 0.0001)

Memory Parameters:
  --mem-capacity=N             Working memory capacity (default: 100)
  --mem-episodes=N             Maximum episodic memory episodes (default: 1000)

Neuromodulation Parameters:
  --dopamine-base=VALUE        Baseline dopamine level (default: 0.1)
  --curiosity-base=VALUE        Curiosity-related baseline (default: 0.0)
  --novelty-base=VALUE          Novelty-related baseline (default: 0.0)

Reward Parameters:
  --reward-discount=VALUE       Reward discount factor (default: 0.99)
  --exploration-bonus=VALUE     Exploration bonus (default: 0.1)
  --exploitation-penalty=VALUE  Exploitation penalty (default: 0.01)

Environment Parameters:
  --env-width=N                Environment width (default: 10)
  --env-height=N                Environment height (default: 10)
  --vision-width=WIDTH         Vision width (default: 8.0)
  --vision-height=HEIGHT       Vision height (default: 8.0)

Example Usage:
  nlm --verbose --max-steps=1000 --environment=complex
  nlm --experiment-type=integration --reward-modulation --development
  nlm -c myconfig.cfg --save-results --output-dir=./results
  nlm --help-advanced               Show advanced options help
  nlm --help                      Show basic options help

For more detailed information, see the documentation.
)";
}

std::string CLIInterface::getAdvancedHelpText() {
    return R"(
=== NLM Advanced Command-Line Options ===

Advanced neural configuration parameters:
  --plasticity-rate=RATE        Control overall learning rate across all plasticity rules
  --stdp-ltp=WEIGHT            Set STDP LTP (Long-Term Potentiation) weight
  --stdp-ltd=WEIGHT            Set STDP LTD (Long-Term Depression) weight
  --stdp-tau=TAU               Set STDP temporal constant (time window for spike timing)
  --synaptogenesis=RATE         Control rate of new synapse formation
  --pruning-rate=RATE          Control rate of synapse elimination

Advanced memory system parameters:
  --mem-capacity=N             Set working memory capacity (number of active traces)
  --mem-episodes=N             Set maximum number of episodic memory episodes to store

Advanced neuromodulation parameters:
  --dopamine-base=VALUE        Set baseline dopamine level affecting overall excitability
  --curiosity-base=VALUE        Set curiosity-related baseline (motivation to explore)
  --novelty-base=VALUE          Set novelty-related baseline (response to new stimuli)

Advanced reward parameters:
  --reward-discount=VALUE       Set temporal discount factor for future rewards
  --exploration-bonus=VALUE     Set bonus for exploring novel states/behaviors
  --exploitation-penalty=VALUE  Set penalty for over-exploiting known states

Performance tuning parameters:
  --num-threads=N              Set number of threads for parallel processing (0 = auto-detect)
  --thread-gran=GRAIN          Set granularity of work distribution among threads
  --enable-simd                Enable SIMD (Single Instruction Multiple Data) optimizations

Multi-experiment configuration:
  --experiment-type=TYPE        Set experiment type: "demo" (quick), "integration" (comprehensive), "full" (complete)
  --environment=TYPE           Set environment type: "simple" (basic), "complex" (rich), "custom" (user-defined)

Expert mode options:
  --enable-reward-modulation    Enable reward-based neuromodulation and learning
  --enable-structural-plasticity Enable structural plasticity (synaptogenesis/pruning)
  --enable-development         Enable developmental processes (maturation, critical periods)
  --enable-curiosity           Enable curiosity-driven exploration motivation
  --enable-novelty-detection   Enable novelty detection for adaptive behavior
  --enable-prediction          Enable predictive coding and forward models
  --enable-checkpointing       Enable checkpoint saving for recovery and analysis
  --enable-replay              Enable episodic memory replay and consolidation

Configuration file options:
  Configuration files can override command-line settings. Command-line settings override defaults.
  Config files support key=value pairs with comments starting with # or /.

For comprehensive documentation, see:
  - docs/ARCHITECTURE.md
  - docs/SCIENCE.md
  - Phase6IntegrationExperiment.h (for advanced experiment configuration)

Advanced users can also directly modify configuration files and use command-line overrides.
)";
}

std::vector<std::string> CLIInterface::getAvailableKeys() {
    return {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "plasticity_learning_rate", "stdp_ltp_weight",
        "stdp_ltd_weight", "stdp_time_constant", "synaptogenesis_rate",
        "pruning_rate", "dopamine_baseline", "dopamine_baseline_curiosity",
        "dopamine_baseline_novelty", "reward_discount_factor",
        "environment_width", "environment_height", "vision_width", "vision_height",
        "working_memory_capacity", "episodic_memory_max_episodes",
        "exploration_bonus", "exploitation_penalty", "enable_reward_modulation",
        "enable_structural_plasticity", "enable_development", "enable_curiosity",
        "enable_novelty_detection", "enable_prediction", "enable_checkpointing",
        "enable_replay", "output_dir", "max_simulation_steps",
        "simulation_time_limit", "log_level", "log_to_file", "log_filename"
    };
}

std::unordered_map<std::string, std::string> CLIInterface::getKeyDescriptions() {
    return {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "General plasticity learning rate"},
        {"stdp_ltp_weight", "STDP LTP (Long-Term Potentiation) weight"},
        {"stdp_ltd_weight", "STDP LTD (Long-Term Depression) weight"},
        {"stdp_time_constant", "STDP temporal constant (time window for spike timing)"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse elimination"},
        {"dopamine_baseline", "Baseline dopamine level affecting excitability"},
        {"dopamine_baseline_curiosity", "Curiosity-related baseline level"},
        {"dopamine_baseline_novelty", "Novelty-related baseline level"},
        {"reward_discount_factor", "Temporal discount factor for future rewards"},
        {"environment_width", "Environment width in world units"},
        {"environment_height", "Environment height in world units"},
        {"vision_width", "Agent vision field width"},
        {"vision_height", "Agent vision field height"},
        {"working_memory_capacity", "Working memory capacity (number of traces)"},
        {"episodic_memory_max_episodes", "Maximum episodic memory episodes"},
        {"exploration_bonus", "Bonus for exploring novel states"},
        {"exploitation_penalty", "Penalty for over-exploiting known states"},
        {"enable_reward_modulation", "Enable reward-based neuromodulation"},
        {"enable_structural_plasticity", "Enable synaptic growth/pruning"},
        {"enable_development", "Enable developmental processes"},
        {"enable_curiosity", "Enable curiosity-driven exploration"},
        {"enable_novelty_detection", "Enable novelty detection"},
        {"enable_prediction", "Enable predictive coding"},
        {"enable_checkpointing", "Enable checkpoint saving"},
        {"enable_replay", "Enable memory replay"},
        {"output_dir", "Output directory for results"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Time limit in seconds (0 = unlimited)"},
        {"log_level", "Log level: 0=ERROR, 1=WARN, 2=INFO, 3=DEBUG"},
        {"log_to_file", "Enable logging to file"},
        {"log_filename", "Log file name"}
    };
}

bool CLIInterface::validateConfiguration() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            printError(error);
        }
        return false;
    }
    return true;
}

std::vector<std::string> CLIInterface::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate configuration values
    if (cliConfig.maxSteps < 0) {
        errors.push_back("Error: maxSteps must be non-negative");
    }
    
    if (cliConfig.timeLimit < 0.0) {
        errors.push_back("Error: timeLimit must be non-negative");
    }
    
    if (cliConfig.logLevel < 0 || cliConfig.logLevel > 3) {
        errors.push_back("Error: logLevel must be between 0 and 3");
    }
    
    if (cliConfig.numThreads < 0) {
        errors.push_back("Error: numThreads must be non-negative");
    }
    
    if (cliConfig.threadGranularity <= 0.0) {
        errors.push_back("Error: threadGranularity must be positive");
    }
    
    if (cliConfig.neuronCount <= 0) {
        errors.push_back("Error: neuronCount must be positive");
    }
    
    if (cliConfig.connectionProbability < 0.0 || cliConfig.connectionProbability > 1.0) {
        errors.push_back("Error: connectionProbability must be between 0.0 and 1.0");
    }
    
    if (cliConfig.environmentWidth <= 0) {
        errors.push_back("Error: environmentWidth must be positive");
    }
    
    if (cliConfig.environmentHeight <= 0) {
        errors.push_back("Error: environmentHeight must be positive");
    }
    
    if (cliConfig.visionWidth <= 0) {
        errors.push_back("Error: visionWidth must be positive");
    }
    
    if (cliConfig.visionHeight <= 0) {
        errors.push_back("Error: visionHeight must be positive");
    }
    
    return errors;
}

void CLIInterface::printError(const std::string& error) const {
    std::cerr << "[ERROR] " << error << std::endl;
}

void CLIInterface::printWarning(const std::string& warning) const {
    std::cerr << "[WARNING] " << warning << std::endl;
}

void CLIInterface::printInfo(const std::string& info) const {
    if (cliConfig.verbose || cliConfig.quiet) {
        std::cout << "[INFO] " << info << std::endl;
    }
}

} // namespace nlm