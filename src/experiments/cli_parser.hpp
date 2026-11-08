#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace nlm {

/**
 * Standardized argument parsing for experiments
 */
namespace CLIParser {
    /**
     * Parse command line arguments for experiment configuration
     */
    struct ExperimentArgs {
        std::string experimentType;
        int numEpisodes{5};
        int stepsPerEpisode{200};
        int numTrials{50};
        float explorationRate{0.1f};
        float rewardScale{1.0f};
        std::string configFile;
        std::string outputDir;
        std::string logLevel{"info"};
        bool enableVerbose{false};
        bool enableDebug{false};
        bool saveCheckpoints{false};
        bool generateVisualizations{false};
        bool runAblations{false};
        
        /**
         * Parse command line arguments
         */
        static ExperimentArgs parse(int argc, char* argv[]) {
            ExperimentArgs args;
            
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--type" && i + 1 < argc) {
                    args.experimentType = argv[++i];
                } else if (arg == "--episodes" && i + 1 < argc) {
                    args.numEpisodes = std::atoi(argv[++i]);
                } else if (arg == "--steps" && i + 1 < argc) {
                    args.stepsPerEpisode = std::atoi(argv[++i]);
                } else if (arg == "--trials" && i + 1 < argc) {
                    args.numTrials = std::atoi(argv[++i]);
                } else if (arg == "--exploration" && i + 1 < argc) {
                    args.explorationRate = std::atof(argv[++i]);
                } else if (arg == "--reward-scale" && i + 1 < argc) {
                    args.rewardScale = std::atof(argv[++i]);
                } else if (arg == "--config" && i + 1 < argc) {
                    args.configFile = argv[++i];
                } else if (arg == "--output" && i + 1 < argc) {
                    args.outputDir = argv[++i];
                } else if (arg == "--log-level" && i + 1 < argc) {
                    args.logLevel = argv[++i];
                } else if (arg == "--verbose") {
                    args.enableVerbose = true;
                } else if (arg == "--debug") {
                    args.enableDebug = true;
                } else if (arg == "--checkpoints") {
                    args.saveCheckpoints = true;
                } else if (arg == "--visualizations") {
                    args.generateVisualizations = true;
                } else if (arg == "--ablations") {
                    args.runAblations = true;
                } else if (arg == "--help") {
                    printHelp();
                    exit(0);
                }
            }
            
            return args;
        }
        
        /**
         * Print help message
         */
        static void printHelp() {
            std::cout << "Experiment CLI Arguments:" << std::endl;
            std::cout << "  --type <type>                Experiment type (phase3, phase4, phase5, phase6)" << std::endl;
            std::cout << "  --episodes <n>               Number of episodes (default: 5)" << std::endl;
            std::cout << "  --steps <n>                  Steps per episode (default: 200)" << std::endl;
            std::cout << "  --trials <n>                 Number of trials (default: 50)" << std::endl;
            std::cout << "  --exploration <rate>         Exploration rate (default: 0.1)" << std::endl;
            std::cout << "  --reward-scale <scale>       Reward scaling factor (default: 1.0)" << std::endl;
            std::cout << "  --config <file>              Configuration file (default: auto)" << std::endl;
            std::cout << "  --output <dir>               Output directory (default: ./results)" << std::endl;
            std::cout << "  --log-level <level>          Log level (debug, info, warn, error)" << std::endl;
            std::cout << "  --verbose                    Enable verbose output" << std::endl;
            std::cout << "  --debug                      Enable debug output" << std::endl;
            std::cout << "  --checkpoints                Save checkpoints" << std::endl;
            std::cout << "  --visualizations             Generate visualizations" << std::endl;
            std::cout << "  --ablations                  Run ablation studies" << std::endl;
            std::cout << "  --help                       Show this help message" << std::endl;
        }
        
        /**
         * Validate arguments
         */
        bool validate() const {
            if (experimentType.empty()) {
                std::cerr << "Error: Experiment type not specified" << std::endl;
                return false;
            }
            
            if (numEpisodes <= 0) {
                std::cerr << "Error: Number of episodes must be positive" << std::endl;
                return false;
            }
            
            if (stepsPerEpisode <= 0) {
                std::cerr << "Error: Steps per episode must be positive" << std::endl;
                return false;
            }
            
            if (explorationRate < 0.0f || explorationRate > 1.0f) {
                std::cerr << "Error: Exploration rate must be between 0 and 1" << std::endl;
                return false;
            }
            
            return true;
        }
        
        /**
         * Get default experiment configuration
         */
        static std::unordered_map<std::string, std::string> getDefaultConfig() {
            return {
                {"neuron_count", "500"},
                {"region_count", "2"},
                {"connection_probability", "0.1"},
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"stdp_ltp_weight", "0.01"},
                {"stdp_ltd_weight", "0.012"},
                {"synaptogenesis_rate", "0.0001"},
                {"pruning_rate", "0.00001"}
            };
        }
    };
    
    /**
     * Create experiment configuration from arguments
     */
    inline std::unordered_map<std::string, std::string> createConfigFromArgs(const ExperimentArgs& args) {
        auto config = ExperimentArgs::getDefaultConfig();
        
        // Override with config file if specified
        if (!args.configFile.empty()) {
            // Load config from file
            // This would typically read from a file
        }
        
        // Add experiment-specific settings
        config["exploration_rate"] = std::to_string(args.explorationRate);
        config["reward_scale"] = std::to_string(args.rewardScale);
        config["verbose"] = args.enableVerbose ? "true" : "false";
        config["debug"] = args.enableDebug ? "true" : "false";
        config["checkpoints"] = args.saveCheckpoints ? "true" : "false";
        config["visualizations"] = args.generateVisualizations ? "true" : "false";
        config["ablations"] = args.runAblations ? "true" : "false";
        
        return config;
    }
    
    /**
     * Process command line arguments and return validated experiment configuration
     */
    inline ExperimentArgs processArgs(int argc, char* argv[]) {
        ExperimentArgs args = ExperimentArgs::parse(argc, argv);
        
        if (!args.validate()) {
            throw std::runtime_error("Invalid command line arguments");
        }
        
        return args;
    }
}

} // namespace nlm
