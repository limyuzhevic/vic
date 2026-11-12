// Run Command Implementation
// Handles nlm run subcommand

#include "nlm/Subcommands/RunCommand.hpp"
#include "nlm/CliCommand.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <cmath>

namespace nlm {

    // Include all necessary demo headers
    #include "brain/Brain.hpp"
    #include "agent/AgentBrain.hpp"
    #include "world/SimpleWorld.hpp"
    #include "environment/Environment.hpp"
    #include "experiments/ExperimentRunner.hpp"
    #include "experiments/Phase3Demo.hpp"
    #include "experiments/Phase4Demo.hpp"
    #include "experiments/Phase5Demo.hpp"
    #include "experiments/Phase6Demo.hpp"
    #include "experiments/Phase6TextDemo.hpp"
    #include "benchmark.hpp"
    
    // Forward declarations for helper functions
    void printBanner();
    void loadDefaultConfig(std::shared_ptr<Config> config);
    int runPhase3DemoInternal(int numEpisodes, int stepsPerEpisode);
    int runPhase4DemoInternal(int numTrials);
    int runPhase5DemoInternal(int numEpisodes, int stepsPerEpisode);
    int runPhase6DemoInternal(int numTrials);
    int runBenchmarkDemoInternal(int numEpisodes, int stepsPerEpisode);
    
    int RunCommand::execute(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cerr << "Error: No mode specified for run command." << std::endl;
            std::cerr << "Usage: nlm run <mode> [options]" << std::endl;
            printHelp();
            return 1;
        }
        
        std::string mode = args[0];
        std::vector<std::string> options(args.begin() + 1, args.end());
        
        // Parse options
        int numEpisodes = 5;
        int stepsPerEpisode = 200;
        bool helpMode = false;
        
        for (size_t i = 0; i < options.size(); ++i) {
            if (options[i] == "--episodes" && i + 1 < options.size()) {
                numEpisodes = std::atoi(options[i + 1].c_str());
                i++;
            } else if (options[i] == "--steps" && i + 1 < options.size()) {
                stepsPerEpisode = std::atoi(options[i + 1].c_str());
                i++;
            } else if (options[i] == "--help" || options[i] == "-h") {
                helpMode = true;
            }
        }
        
        if (helpMode) {
            printHelp();
            return 0;
        }
        
        // Dispatch to appropriate demo based on mode
        if (mode == "phase3") {
            return runPhase3DemoInternal(numEpisodes, stepsPerEpisode);
        } else if (mode == "phase4") {
            return runPhase4DemoInternal(numEpisodes);
        } else if (mode == "phase5") {
            return runPhase5DemoInternal(numEpisodes, stepsPerEpisode);
        } else if (mode == "phase6") {
            return runPhase6DemoInternal(numEpisodes);
        } else if (mode == "benchmark") {
            return runBenchmarkDemoInternal(numEpisodes, stepsPerEpisode);
        } else {
            std::cerr << "Error: Unknown run mode '" << mode << "'" << std::endl;
            std::cerr << "Available modes: phase3, phase4, phase5, phase6, benchmark" << std::endl;
            return 1;
        }
    }
    
    const char* RunCommand::helpText = R"(
NLM Run Command Help
=====================

Run NLM demonstrations and experiments. The run command supports multiple modes:

  nlm run phase3 [options]    - Run Phase 3 world interaction demo
  nlm run phase4 [options]    - Run Phase 4 cognitive emergence demo
  nlm run phase5 [options]    - Run Phase 5 personality/sandbox demo
  nlm run phase6 [options]    - Run Phase 6 integration demo
  nlm run benchmark [options] - Run performance benchmarks

Options:
  --episodes N         Number of episodes (default: 5)
  --steps N            Number of steps per episode (default: 200)
  --help, -h           Show this help message

Examples:
  nlm run phase3 --episodes 10 --steps 500
  nlm run phase4
  nlm run benchmark --episodes 1 --steps 100

The run command maintains full backward compatibility with the original demo executables.
When you run a mode without options, it uses the original demo behavior with sensible defaults.
)";
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void loadDefaultConfig(std::shared_ptr<Config> config) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    int runPhase3DemoInternal(int numEpisodes, int stepsPerEpisode) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        try {
            // Create and run Phase 3 demo
            Phase3Demo demo;
            demo.run(numEpisodes, stepsPerEpisode);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error running Phase 3 demo: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runPhase4DemoInternal(int numTrials) {
        std::cout << "NLM Phase 4: Emerging Cognition\n";
        std::cout << "================================\n\n";
        
        try {
            // Create and run Phase 4 demo
            Phase4Demo demo;
            demo.run(numTrials);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error running Phase 4 demo: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runPhase5DemoInternal(int numEpisodes, int stepsPerEpisode) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 5 - BRAIN-PERSONALITY / SANDBOX DEMO\n";
        std::cout << "===========================================\n\n";
        
        try {
            // Create and run Phase 5 demo
            Phase5Demo demo;
            demo.run(numEpisodes, stepsPerEpisode);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error running Phase 5 demo: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runPhase6DemoInternal(int numTrials) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        try {
            // Create and run Phase 6 demo
            Phase6Demo demo;
            demo.run(numTrials);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error running Phase 6 demo: " << e.what() << std::endl;
            return 1;
        }
    }
    
    int runBenchmarkDemoInternal(int numEpisodes, int stepsPerEpisode) {
        std::cout << "NLM Performance Benchmark\n";
        std::cout << "=========================\n\n";
        
        try {
            // Create and run benchmark
            Benchmark benchmark;
            benchmark.run(numEpisodes, stepsPerEpisode);
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error running benchmark: " << e.what() << std::endl;
            return 1;
        }
    }
    
} // namespace nlm