// NLM Unified CLI Interface
// A unified command-line interface for all NLM demonstrations and operations

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <chrono>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

class NLMCLI {
public:
    enum class Command {
        Help,
        Version,
        ListDemos,
        Phase2,
        Phase3,
        Phase4,
        Phase5,
        Phase6,
        Phase6Text,
        Phase6Integrated,
        Benchmark,
        Config,
        Run,
        Unknown
    };

    enum class DemoLevel {
        Basic,
        Intermediate,
        Advanced,
        Expert
    };

    NLMCLI() = default;
    ~NLMCLI() = default;

    int run(int argc, char* argv[]);

private:
    Command parseCommand(const std::string& cmd);
    DemoLevel parseDemoLevel(const std::string& level);
    bool parseCommonArgs(int argc, char* argv[], std::string& configFile, 
                        uint64_t& neuronCount, uint64_t& steps, std::string& outputFile);
    
    // Demo execution methods
    int runPhase2Demo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runPhase3Demo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runPhase4Demo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runPhase5Demo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runPhase6Demo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runPhase6TextDemo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runPhase6IntegratedDemo(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    int runBenchmark(const std::string& configFile, uint64_t neuronCount, uint64_t steps);
    
    // Configuration management
    int runConfigCommand(const std::vector<std::string>& args);
    int generateConfig(const std::string& filename, bool jsonFormat = true);
    int validateConfig(const std::string& filename);
    
    // Utility methods
    void printHelp() const;
    void printVersion() const;
    void printDemosList() const;
    void printWelcome() const;
    
    // Advanced features
    int runAdvancedMode(const std::string& command, const std::map<std::string, std::string>& options);
    int runScriptingMode(const std::string& scriptFile);
    
    std::string getDefaultConfig() const;
    std::string getVersionString() const;
};

// Helper functions
NLMCLI::Command NLMCLI::parseCommand(const std::string& cmd) {
    static const std::map<std::string, Command> commandMap = {
        {"help", Command::Help},
        {"--help", Command::Help},
        {"-h", Command::Help},
        {"version", Command::Version},
        {"--version", Command::Version},
        {"-v", Command::Version},
        {"demos", Command::ListDemos},
        {"--demos", Command::ListDemos},
        {"phase2", Command::Phase2},
        {"--phase2", Command::Phase2},
        {"phase3", Command::Phase3},
        {"--phase3", Command::Phase3},
        {"phase4", Command::Phase4},
        {"--phase4", Command::Phase4},
        {"phase5", Command::Phase5},
        {"--phase5", Command::Phase5},
        {"phase6", Command::Phase6},
        {"--phase6", Command::Phase6},
        {"phase6-text", Command::Phase6Text},
        {"phase6-text", Command::Phase6Text},
        {"phase6-integrated", Command::Phase6Integrated},
        {"phase6-integrated", Command::Phase6Integrated},
        {"benchmark", Command::Benchmark},
        {"--benchmark", Command::Benchmark},
        {"config", Command::Config},
        {"--config", Command::Config},
        {"run", Command::Run},
        {"--run", Command::Run}
    };
    
    auto it = commandMap.find(cmd);
    return (it != commandMap.end()) ? it->second : Command::Unknown;
}

NLMCLI::DemoLevel NLMCLI::parseDemoLevel(const std::string& level) {
    if (level == "basic") return DemoLevel::Basic;
    if (level == "intermediate") return DemoLevel::Intermediate;
    if (level == "advanced") return DemoLevel::Advanced;
    if (level == "expert") return DemoLevel::Expert;
    return DemoLevel::Intermediate; // Default
}

bool NLMCLI::parseCommonArgs(int argc, char* argv[], std::string& configFile, 
                           uint64_t& neuronCount, uint64_t& steps, std::string& outputFile) {
    configFile = "configs/default.cfg";
    neuronCount = 1000;
    steps = 1000;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoull(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--level" && i + 1 < argc) {
            // Store demo level for later use
            // This would be handled by the specific demo runner
        }
    }
    return true;
}

int NLMCLI::run(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }
    
    std::string commandStr = argv[1];
    Command command = parseCommand(commandStr);
    
    switch (command) {
        case Command::Help:
            printHelp();
            return 0;
            
        case Command::Version:
            printVersion();
            return 0;
            
        case Command::ListDemos:
            printDemosList();
            return 0;
            
        case Command::Phase2:
            return runPhase2Demo("", 1000, 1000);
            
        case Command::Phase3:
            return runPhase3Demo("", 1000, 1000);
            
        case Command::Phase4:
            return runPhase4Demo("", 1000, 1000);
            
        case Command::Phase5:
            return runPhase5Demo("", 1000, 1000);
            
        case Command::Phase6:
            return runPhase6Demo("", 1000, 1000);
            
        case Command::Phase6Text:
            return runPhase6TextDemo("", 1000, 1000);
            
        case Command::Phase6Integrated:
            return runPhase6IntegratedDemo("", 1000, 1000);
            
        case Command::Benchmark:
            return runBenchmark("", 1000, 1000);
            
        case Command::Config:
            return runConfigCommand(std::vector<std::string>(argv + 2, argv + argc));
            
        default:
            std::cerr << "Unknown command: " << commandStr << std::endl;
            printHelp();
            return 1;
    }
}

// Implement other methods...

void NLMCLI::printHelp() const {
    printWelcome();
    std::cout << R"(
USAGE:
  nlm [OPTIONS] [DEMO] [ARGS]

DEMOS:
  phase2           Phase 2: Basic neural computation (LIF neurons)
  phase3           Phase 3: World interaction and learning
  phase4           Phase 4: Advanced cognitive capabilities
  phase5           Phase 5: Performance optimizations and sandbox
  phase6           Phase 6: Full integration demo
  phase6-text      Phase 6: Text-based demo
  phase6-integrated Phase 6: Integration test with all systems
  benchmark        Performance benchmark

OPTIONS:
  --config FILE    Configuration file (default: configs/default.cfg)
  --neurons N      Number of neurons (default: 1000)
  --steps N        Number of simulation steps (default: 1000)
  --output FILE    Output file for checkpoints (default: none)
  --level LEVEL    Demo level: basic/intermediate/advanced/expert
  --help, -h       Show this help message
  --version, -v    Show version information
  --demos          List all available demos

ADVANCED USAGE:
  nlm run script.json   Run from a configuration script
  nlm config generate config.json   Generate default configuration
  nlm config validate config.json   Validate configuration file

EXAMPLES:
  nlm phase6 --config my_config.cfg --neurons 2000 --steps 5000
  nlm phase6-text --level advanced
  nlm benchmark --neurons 500 --steps 2000
  nlm config generate demo_config.json
  nlm --demos

For more information about a specific demo, run:
  nlm phaseX --help

";) << std::endl;
}

void NLMCLI::printVersion() const {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Version: 0.1.0 (Phase 6 Integration)
Build: Experimental Artificial Developmental Brain

Key Features:
  ✓ Real LIF neuron dynamics
  ✓ Event-driven spike propagation
  ✓ STDP and Hebbian plasticity
  ✓ Structural plasticity
  ✓ Memory systems (working, episodic, associative)
  ✓ Neuromodulation (dopamine, curiosity, novelty)
  ✓ Prediction systems
  ✓ Cognitive mechanisms (planner, concept formation, attention)
  ✓ Development system
  ✓ Checkpoint save/load
  ✓ Replay and consolidation

";) << std::endl;
}

void NLMCLI::printDemosList() const {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                  AVAILABLE NLM DEMOS                          ║
╠═══════════════════════════════════════════════════════════════╣
║                                                               ║
║  phase2         Phase 2: Real Neural Computation              ║
║                 • LIF neuron dynamics                       ║
║                 • Event-driven spike propagation             ║
║                 • STDP and Hebbian plasticity                 ║
║                 • Structural plasticity                      ║
║                                                               ║
║  phase3         Phase 3: World Interaction                    ║
║                 • Sensory input & motor output               ║
║                 • Reward prediction error                     ║
║                 • Developmental stages                      ║
║                 • Novelty and curiosity                      ║
║                                                               ║
║  phase4         Phase 4: Cognitive Capabilities              ║
║                 • Neural prediction system                   ║
║                 • Working memory                             ║
║                 • Episodic memory                            ║
║                 • Concept formation                          ║
║                 • Neural planning                            ║
║                 • Attention mechanisms                        ║
║                 • Social learning                            ║
║                                                               ║
║  phase5         Phase 5: Performance Optimizations           ║
║                 • SIMD vectorization                        ║
║                 • Parallel processing                         ║
║                 • Memory pools and event queues               ║
║                 • Checkpoint system                           ║
║                                                               ║
║  phase6         Phase 6: Integration Test                     ║
║                 • Complete system integration                 ║
║                 • All memory systems connected                ║
║                 • Neuromodulation integrated                   ║
║                 • Development affects plasticity              ║
║                                                               ║
║  phase6-text    Phase 6: Text-based Demo                     ║
║                 • Text as a sensory modality                  ║
║                 • Association probes                          ║
║                 • Concept/social probes                       ║
║                                                               ║
║  phase6-integrated Phase 6: Final Integration                ║
║                 • Comprehensive integration test              ║
║                 • All systems work together                   ║
║                 • Performance benchmarks                      ║
║                                                               ║
║  benchmark      Performance Benchmark                        ║
║                 • Measure simulation speed                   ║
║                 • Scale with different network sizes          ║
║                 • Report spikes/sec, neurons/synapse ratios     ║
║                                                               ║
╠═══════════════════════════════════════════════════════════════╣
║                                                               ║
║  Advanced Options (accessible via --run command)            ║
║  • Custom configuration files                             ║
║  • Script-based execution                                   ║
║  • Batch processing                                         ║
║  • Export capabilities                                      ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

";) << std::endl;
}

void NLMCLI::printWelcome() const {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║           NLM (熙然) - Neural Learning Machine                  ║
║                                                               ║
║          An Experimental Artificial Developmental Brain       ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

Phase 6: FINAL INTEGRATION - All Systems Unified

";) << std::endl;
}

std::string NLMCLI::getDefaultConfig() const {
    return R"({
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0,
    "plasticity": {
      "stdp": {
        "enable": true,
        "learning_rate": 0.001,
        "tau_plus": 20.0,
        "tau_minus": 20.0
      },
      "hebbian": {
        "enable": true,
        "learning_rate": 0.01
      },
      "structural": {
        "enable": true,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001
      }
    },
    "neuromod": {
      "dopamine": {
        "scale": 1.0,
        "baseline": 0.0
      },
      "curiosity": {
        "enable": true,
        "scale": 1.0
      },
      "novelty": {
        "enable": true,
        "threshold": 0.5
      }
    },
    "memory": {
      "working": {
        "capacity": 100,
        "decay_rate": 0.01
      },
      "episodic": {
        "capacity": 1000,
        "replay_interval": 100
      }
    },
    "prediction": {
      "enable": true,
      "prediction_horizon": 10
    }
  },
  "environment": {
    "name": "SimpleWorld",
    "width": 20,
    "height": 20,
    "vision_width": 8,
    "vision_height": 8
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "checkpoint_path": "./checkpoint.bin"
  }
}
";) << std::endl;
}

std::string NLMCLI::getVersionString() const {
    return "NLM 0.1.0 (Phase 6 Integration) - Build 2026-11-09";
}

// Main function to run the unified CLI
int main(int argc, char* argv[]) {
    try {
        NLMCLI cli;
        return cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
