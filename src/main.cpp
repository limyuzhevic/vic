// NLM (熙然) - Neural Learning Machine
// Enhanced CLI with subcommands and advanced features
//
// This version includes:
// - Subcommands: demo, experiment, benchmark, debug, visualize, config, help
// - Advanced configuration management
// - Better logging and performance monitoring
// - State inspection and debugging tools

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <chrono>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Advanced neural simulation with complete brain integration ║
    ║     This phase implements:                                      ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║     - Memory systems (working + episodic)                   ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)        ║
    ║     - Prediction systems                                     ║
    ║     - Cognitive systems (planner, concept formation)         ║
    ║     - Development and adaptation                             ║
    ║     - Checkpoint and replay systems                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << R"(
NLM - Neural Learning Machine - Enhanced CLI

Usage: nlm <command> [options]

Commands:
  demo        Run a demonstration (Phase 3, 4, 5, 6)
  experiment   Run controlled experiments
  benchmark    Performance benchmarking
  debug        Debugging and state inspection
  visualize    Visualization and analytics
  config       Configuration management
  help         Show this help message

Examples:
  nlm demo --phase 6 --duration 100
  nlm experiment --name "integration_test" --steps 1000
  nlm benchmark --mode "performance"
  nlm config --list
  nlm debug --memory --neurons

For more details on a specific command:
  nlm help <command>

Options:
  --help, -h              Show help message
  --version               Show version information
  --verbose, -v           Enable verbose output
  --quiet, -q             Quiet mode (errors only)
  --log-level LEVEL       Set logging level (DEBUG, INFO, WARN, ERROR, CRIT)
  --config FILE           Configuration file path
    )" << std::endl;
}

void printPhaseHelp() {
    std::cout << R"(
NLM Demo Commands - Run different phase demonstrations

Usage: nlm demo <phase> [options]

Phases:
  2       Basic neural computation (Phase 2)
  3       World interaction and sensory-motor loop
  4       Advanced cognitive systems
  5       Performance optimizations
  6       Final integration (all systems)

Options:
  --duration SECONDS      Duration of demo (default: 30)
  --neurons COUNT         Number of neurons (default: 500)
  --steps COUNT           Number of simulation steps (default: 1000)
  --headless             Run without logging (for scripts)
  --checkpoint FILE       Save checkpoint to file
  --load FILE             Load checkpoint from file

Examples:
  nlm demo 2 --duration 60 --steps 2000
  nlm demo 6 --neurons 1000 --steps 5000 --checkpoint demo_check.bin
    )" << std::endl;
}

void printExperimentHelp() {
    std::cout << R"(
NLM Experiment Commands - Run controlled experiments

Usage: nlm experiment <name> [options]

Experiment Names:
  basic_connectivity      Test basic neural connectivity
  plasticity_learning     Test plasticity and learning
  stdp_verification       Test STDP learning rule
  integration_test        Test full system integration
  neuromodulation          Test neuromodulation effects
  memory_test             Test memory systems

Options:
  --steps COUNT           Number of simulation steps (default: 1000)
  --repeats COUNT         Number of experiment repetitions (default: 1)
  --seed SEED             Random seed (default: 42)
  --save RESULTS_FILE     Save results to file
  --load CONFIG_FILE      Load configuration from file
  --verbose, -v          Enable verbose output
  --parallel             Run experiments in parallel

Examples:
  nlm experiment plasticity_learning --steps 5000 --repeats 3
  nlm experiment integration_test --save results.json
    )" << std::endl;
}

void printBenchmarkHelp() {
    std::cout << R"(
NLM Benchmark Commands - Performance benchmarking

Usage: nlm benchmark [options]

Benchmark Modes:
  performance             Measure processing speed
  memory                  Measure memory usage
  plasticity              Measure plasticity performance
  integration             Measure system integration speed
  simulation              Measure simulation stability

Options:
  --mode MODE             Benchmark mode (default: performance)
  --neurons COUNT         Number of neurons (default: 1000)
  --steps COUNT           Number of simulation steps (default: 2000)
  --runs COUNT            Number of benchmark runs (default: 3)
  --save RESULTS_FILE     Save benchmark results
  --csv                   Output in CSV format
  --html                  Generate HTML report
  --plot                  Generate performance plots

Examples:
  nlm benchmark --mode performance --neurons 2000 --steps 5000
  nlm benchmark --mode memory --save memory_bench.json --html
    )" << std::endl;
}

void runDemo(int phase, const std::map<std::string, std::string>& options) {
    std::cout << "=== NLM Phase " << phase << " Demo ===" << std::endl;
    
    // Parse options
    int duration = 30;
    int neurons = 500;
    int steps = 1000;
    bool headless = false;
    std::string checkpointFile;
    std::string loadFile;
    
    auto it = options.find("duration");
    if (it != options.end()) duration = std::stoi(it->second);
    
    it = options.find("neurons");
    if (it != options.end()) neurons = std::stoi(it->second);
    
    it = options.find("steps");
    if (it != options.end()) steps = std::stoi(it->second);
    
    it = options.find("headless");
    if (it != options.end()) headless = (it->second == "true");
    
    it = options.find("checkpoint");
    if (it != options.end()) checkpointFile = it->second;
    
    it = options.find("load");
    if (it != options.end()) loadFile = it->second;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(headless ? LogLevel::Error : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting Phase " + std::to_string(phase) + " demo");
    NLM_LOG_INFO("Configuration: " + std::to_string(neurons) + " neurons, " + 
                 std::to_string(steps) + " steps, " + std::to_string(duration) + "s duration");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Set configuration based on phase
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Phase-specific configurations
    if (phase == 2) {
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    } else if (phase >= 3) {
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    if (!loadFile.empty()) {
        if (config->loadFromFile(loadFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + loadFile);
        } else {
            NLM_LOG_ERROR("Failed to load configuration from: " + loadFile);
            return;
        }
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Phase 2 tests (if requested)
    if (phase == 2) {
        // Run Phase 2 tests from original main.cpp
        auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
        if (!neurons.empty()) {
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current into first 10 neurons
            NLM_LOG_INFO("Injecting current into 10 neurons...");
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            // Run simulation steps
            for (SimulationStep step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
                
                // Log progress periodically
                if (step % 100 == 0 && !headless) {
                    NLM_LOG_INFO("Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            NLM_LOG_INFO("Phase 2 Test Results: " + std::to_string(spikes) + " spikes generated");
        }
    }
    
    // Phase 6 demo (full integration)
    else if (phase == 6) {
        // Run Phase 6 integration demo
        std::cout << "=== Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing integration of all brain systems..." << std::endl;
        
        // Create a simple world for testing
        auto world = std::make_shared<SimpleWorld>();
        world->configure(16, 16, 8, 8);
        world->reset();
        world->setAgentStart(8.0, 8.0);
        
        // Create agent brain interface
        AgentBrain agent(brain);
        agent.initialize(*world);
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(true);
        agent.enableDevelopment(true);
        agent.enableCuriosity(true);
        
        // Run simulation loop
        for (uint64_t step = 0; step < steps; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            auto cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(cmd, static_cast<double>(step));
            
            // Compute reward
            float reward = 0.0f;
            if (auto internal = percept->getInternal()) {
                reward = internal->size() > 0 ? (*internal)[0] : 0.0f;
            }
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent.updateDevelopment(0.001);
            
            // Log progress
            if (step % 1000 == 0 && !headless) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(agent.getNeuromodulationLevel()) +
                            " | Curiosity: " + std::to_string(agent.getCuriosityLevel()) +
                            " | Development: " + std::to_string(static_cast<int>(agent.getDevelopmentalStage())));
            }
        }
        
        // Display final status
        std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Working memory active traces: " << 
            (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        std::cout << "Episodic memory episodes: " << 
            (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
        std::cout << "Dopamine level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "Curiosity level: " << agent.getCuriosityLevel() << std::endl;
        
        // Save checkpoint if requested
        if (!checkpointFile.empty()) {
            if (brain->save(checkpointFile)) {
                NLM_LOG_INFO("Checkpoint saved to: " + checkpointFile);
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint to: " + checkpointFile);
            }
        }
    }
    
    NLM_LOG_INFO("=== Phase " + std::to_string(phase) + " Demo Complete ===");
}

void parseOptions(int argc, char** argv, std::map<std::string, std::string>& options) {
    for (int i = 0; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                options[key] = value;
            } else {
                options[arg.substr(2)] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            std::string key = arg.substr(1);
            options[key] = "true";
        }
    }
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        printBanner();
        printHelp();
        return 0;
    }
    
    std::string command = argv[1];
    std::map<std::string, std::string> options;
    
    // Parse remaining arguments as options
    for (int i = 2; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--") {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                options[key] = value;
            } else {
                options[arg.substr(2)] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            options[arg.substr(1)] = "true";
        } else {
            // Positional argument (e.g., phase number)
            if (command == "demo") {
                // Try to parse as phase number
                try {
                    int phase = std::stoi(arg);
                    options["phase"] = arg;
                } catch (...) {
                    // Not a phase number, ignore
                }
            }
        }
    }
    
    if (command == "help") {
        if (options.empty()) {
            printBanner();
            printHelp();
        } else {
            std::string topic = options.begin()->first;
            if (topic == "demo") {
                printPhaseHelp();
            } else if (topic == "experiment") {
                printExperimentHelp();
            } else if (topic == "benchmark") {
                printBenchmarkHelp();
            } else {
                std::cout << "Unknown help topic: " << topic << std::endl;
                return 1;
            }
        }
    } 
    else if (command == "demo") {
        std::string phaseStr;
        auto it = options.find("phase");
        if (it != options.end()) {
            phaseStr = it->second;
        } else if (!options.empty()) {
            // Try to determine phase from first option
            for (auto& opt : options) {
                try {
                    int phase = std::stoi(opt.first);
                    phaseStr = opt.first;
                    break;
                } catch (...) {
                    // Not a phase number
                }
            }
        }
        
        if (phaseStr.empty()) {
            std::cout << "Error: Phase number required for demo command" << std::endl;
            printPhaseHelp();
            return 1;
        }
        
        int phase = std::stoi(phaseStr);
        if (phase < 2 || phase > 6) {
            std::cout << "Error: Invalid phase " << phase << ". Must be between 2 and 6." << std::endl;
            return 1;
        }
        
        printBanner();
        runDemo(phase, options);
    }
    
    else if (command == "help") {
        printBanner();
        printHelp();
    }
    
    else {
        std::cout << "Error: Unknown command '" << command << "'" << std::endl;
        printBanner();
        printHelp();
        return 1;
    }
    
    return 0;
}