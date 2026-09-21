#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

namespace nlm {

// Command line parser for NLM executables
class CommandLineParser {
public:
    static std::string getUsage(const std::string& programName) {
        return R"(Usage: " + programName + " [options]

Basic Options:
  --help                    Show this help message
  --version                 Show version information
  --config <file>           Load configuration from file
  --debug                   Enable debug logging
  --log-level <level>       Set logging level (DEBUG, INFO, WARNING, ERROR)

Simulation Options:
  --steps <num>             Number of simulation steps (default: 1000)
  --timestep <ms>           Simulation timestep in milliseconds (default: 1.0)
  --seed <num>              Random seed for reproducible experiments (default: 42)

Brain Configuration:
  --neuron_count <num>      Total number of neurons (default: 1000)
  --synapse_density <float>  Connection probability (default: 0.1)
  --connection_probability <float> Probability between 0.0 and 1.0
  --region_count <num>      Number of brain regions (default: 1)

Plasticity Configuration:
  --stdp_ltp_weight <float>  STDP LTP weight (default: 0.02)
  --stdp_ltd_weight <float>  STDP LTD weight (default: 0.015)
  --stdp_tau <ms>           STDP time constant in ms (default: 20.0)
  --plasticity_stdp <bool>  Enable STDP (default: true)
  --plasticity_hebbian <bool> Enable Hebbian learning (default: true)
  --plasticity_structural <bool> Enable structural plasticity (default: true)

Development Configuration:
  --synaptogenesis_rate <float> Rate of new synapse formation (default: 0.0001)
  --pruning_rate <float>     Rate of synapse elimination (default: 0.00001)
  --critical_period <ms>     Critical period duration in ms (default: 1000.0)

Neuromodulation Configuration:
  --dopamine_scale <float>     Dopamine modulation scale (default: 1.0)
  --curiosity_enabled <bool>   Enable curiosity-driven exploration (default: true)
  --novelty_enabled <bool>      Enable novelty detection (default: true)
  --reward_prediction_error <bool> Enable reward prediction error (default: true)

Output Options:
  --output <file>           Save simulation results to file
  --stats                   Print detailed statistics at the end
  --checkpoint <file>       Save brain state periodically

Advanced Options:
  --cpu-affinity <mask>    CPU core affinity for parallel processing
  --use-simd               Enable SIMD optimizations (default: true)
  --enable-checkpointing   Enable automatic checkpointing
  --max-memory <MB>        Maximum memory usage in MB
  --verbose                Verbose output

Examples:
  # Run with custom configuration
  " + programName + " --config my_config.cfg --steps 5000

  # Run with specific parameters for research
  " + programName + " --neuron_count 5000 --synapse_density 0.2 \
      --stdp_ltp_weight 0.05 --plasticity_stdp true --plasticity_hebbian false

  # Run fast test with minimal features
  " + programName + " --steps 100 --neuron_count 100 \
      --plasticity_stdp false --plasticity_hebbian false

  # Run with checkpointing for long simulations
  " + programName + " --steps 100000 --checkpoint brain.checkpoint \
      --enable-checkpointing

  # Run with specific random seed for reproducibility
  " + programName + " --seed 12345 --stdp_ltp_weight 0.01

  # Debug run with full logging
  " + programName + " --debug --log-level DEBUG --steps 1000

Copyright (c) 2026 NLM Research Project
Version: 0.1.0 - Phase 6 Integration
License: MIT
);"
    }

    static std::string getVersion() {
        return "NLM v0.1.0 (Phase 6 Integration)";
    }

    static bool parseArgs(int argc, char** argv, std::string& configFile, 
                         std::string& logLevel, bool& debug,
                         std::string& outputFile, bool& stats,
                         bool& checkpointing, std::string& checkpointFile,
                         int64_t& steps, double& timestep, int64_t& seed,
                         int64_t& neuronCount, float& synapseDensity,
                         float& stdpLtpWeight, float& stdpLtdWeight,
                         float& stdpTau, bool& plasticityStdp,
                         bool& plasticityHebbian, bool& plasticityStructural,
                         float& synaptogenesisRate, float& pruningRate,
                         float& criticalPeriod, float& dopamineScale,
                         bool& curiosityEnabled, bool& noveltyEnabled,
                         bool& rewardPredictionError, bool& useSimd,
                         std::string& cpuAffinity, int64_t& maxMemoryMB) {
        
        // Default values
        steps = 1000;
        timestep = 1.0;
        seed = 42;
        neuronCount = 1000;
        synapseDensity = 0.1f;
        stdpLtpWeight = 0.02f;
        stdpLtdWeight = 0.015f;
        stdpTau = 20.0f;
        plasticityStdp = true;
        plasticityHebbian = true;
        plasticityStructural = true;
        synaptogenesisRate = 0.0001f;
        pruningRate = 0.00001f;
        criticalPeriod = 1000.0f;
        dopamineScale = 1.0f;
        curiosityEnabled = true;
        noveltyEnabled = true;
        rewardPredictionError = true;
        useSimd = true;
        maxMemoryMB = 1024;  // 1GB default
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                std::cout << getUsage(argv[0]) << std::endl;
                return true;
            } else if (arg == "--version") {
                std::cout << getVersion() << std::endl;
                return true;
            } else if (arg.substr(0, 8) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 11) == "--log-level") {
                if (arg.find('=') != std::string::npos) {
                    logLevel = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    logLevel = argv[++i];
                }
            } else if (arg == "--debug") {
                debug = true;
            } else if (arg == "--stats") {
                stats = true;
            } else if (arg == "--enable-checkpointing") {
                checkpointing = true;
            } else if (arg.substr(0, 13) == "--checkpoint") {
                if (arg.find('=') != std::string::npos) {
                    checkpointFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    checkpointFile = argv[++i];
                }
            } else if (arg.substr(0, 7) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    steps = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    steps = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 9) == "--timestep") {
                if (arg.find('=') != std::string::npos) {
                    timestep = std::stod(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    timestep = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 6) == "--seed") {
                if (arg.find('=') != std::string::npos) {
                    seed = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    seed = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 14) == "--neuron_count") {
                if (arg.find('=') != std::string::npos) {
                    neuronCount = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    neuronCount = std::stoll(argv[++i]);
                }
            } else if (arg.substr(0, 20) == "--synapse_density" || arg.substr(0, 23) == "--connection_probability") {
                if (arg.find('=') != std::string::npos) {
                    synapseDensity = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    synapseDensity = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 17) == "--stdp_ltp_weight") {
                if (arg.find('=') != std::string::npos) {
                    stdpLtpWeight = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    stdpLtpWeight = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 18) == "--stdp_ltd_weight") {
                if (arg.find('=') != std::string::npos) {
                    stdpLtdWeight = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    stdpLtdWeight = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 10) == "--stdp_tau") {
                if (arg.find('=') != std::string::npos) {
                    stdpTau = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    stdpTau = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 19) == "--plasticity_stdp") {
                if (arg.find('=') != std::string::npos) {
                    std::string val = arg.substr(arg.find('=') + 1);
                    plasticityStdp = (val == "true" || val == "1" || val == "yes");
                } else if (i + 1 < argc) {
                    std::string val = argv[++i];
                    plasticityStdp = (val == "true" || val == "1" || val == "yes");
                }
            } else if (arg.substr(0, 24) == "--plasticity_hebbian") {
                if (arg.find('=') != std::string::npos) {
                    std::string val = arg.substr(arg.find('=') + 1);
                    plasticityHebbian = (val == "true" || val == "1" || val == "yes");
                } else if (i + 1 < argc) {
                    std::string val = argv[++i];
                    plasticityHebbian = (val == "true" || val == "1" || val == "yes");
                }
            } else if (arg.substr(0, 26) == "--plasticity_structural") {
                if (arg.find('=') != std::string::npos) {
                    std::string val = arg.substr(arg.find('=') + 1);
                    plasticityStructural = (val == "true" || val == "1" || val == "yes");
                } else if (i + 1 < argc) {
                    std::string val = argv[++i];
                    plasticityStructural = (val == "true" || val == "1" || val == "yes");
                }
            } else if (arg.substr(0, 23) == "--synaptogenesis_rate") {
                if (arg.find('=') != std::string::npos) {
                    synaptogenesisRate = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    synaptogenesisRate = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 17) == "--pruning_rate") {
                if (arg.find('=') != std::string::npos) {
                    pruningRate = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    pruningRate = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 18) == "--critical_period") {
                if (arg.find('=') != std::string::npos) {
                    criticalPeriod = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    criticalPeriod = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 18) == "--dopamine_scale") {
                if (arg.find('=') != std::string::npos) {
                    dopamineScale = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    dopamineScale = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 24) == "--curiosity_enabled") {
                if (arg.find('=') != std::string::npos) {
                    std::string val = arg.substr(arg.find('=') + 1);
                    curiosityEnabled = (val == "true" || val == "1" || val == "yes");
                } else if (i + 1 < argc) {
                    std::string val = argv[++i];
                    curiosityEnabled = (val == "true" || val == "1" || val == "yes");
                }
            } else if (arg.substr(0, 21) == "--novelty_enabled") {
                if (arg.find('=') != std::string::npos) {
                    std::string val = arg.substr(arg.find('=') + 1);
                    noveltyEnabled = (val == "true" || val == "1" || val == "yes");
                } else if (i + 1 < argc) {
                    std::string val = argv[++i];
                    noveltyEnabled = (val == "true" || val == "1" || val == "yes");
                }
            } else if (arg.substr(0, 27) == "--reward_prediction_error") {
                if (arg.find('=') != std::string::npos) {
                    std::string val = arg.substr(arg.find('=') + 1);
                    rewardPredictionError = (val == "true" || val == "1" || val == "yes");
                } else if (i + 1 < argc) {
                    std::string val = argv[++i];
                    rewardPredictionError = (val == "true" || val == "1" || val == "yes");
                }
            } else if (arg == "--use-simd") {
                useSimd = true;
            } else if (arg.substr(0, 16) == "--cpu-affinity") {
                if (arg.find('=') != std::string::npos) {
                    cpuAffinity = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    cpuAffinity = argv[++i];
                }
            } else if (arg.substr(0, 14) == "--max-memory") {
                if (arg.find('=') != std::string::npos) {
                    maxMemoryMB = std::stoll(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    maxMemoryMB = std::stoll(argv[++i]);
                }
            } else if (arg == "--verbose") {
                // Already covered by debug flag
                debug = true;
            } else if (arg.substr(0, 9) == "--output") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                std::cout << getUsage(argv[0]) << std::endl;
                return false;
            }
        }
        
        return true;
    }
};