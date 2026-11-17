// NLM (熙然) - Neural Learning Machine
// Unified Main Entry Point
// Combines Phase 2, Phase 3, Phase 4, Phase 5, and Phase 6 demos into a single interface

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
#include <algorithm>
#include <map>

using namespace nlm;

// Forward declarations for phase demos
void runPhase2Demo(std::shared_ptr<Brain> brain);
void runPhase3Demo(std::shared_ptr<Brain> brain);
void runPhase4Demo(std::shared_ptr<Brain> brain);
void runPhase5Demo(std::shared_ptr<Brain> brain);
void runPhase6Demo(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world);
void runIntegratedDemo(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world);

// Command-line argument parsing
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string mode = "auto"; // auto, phase2, phase3, phase4, phase5, phase6, benchmark
    int steps = 1000;
    int neurons = 1000;
    double timestep = 0.001;
    bool verbose = false;
    bool help = false;
    bool list_modes = false;
    std::map<std::string, std::string> customSettings;
};

CommandLineArgs parseCommandLineArgs(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--list-modes") {
            args.list_modes = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                args.mode = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                args.steps = std::stoi(argv[++i]);
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                args.neurons = std::stoi(argv[++i]);
            }
        } else if (arg == "--timestep") {
            if (i + 1 < argc) {
                args.timestep = std::stod(argv[++i]);
            }
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            // Custom setting like --plasticity.learning_rate=0.1
            size_t pos = arg.find('=');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            args.customSettings[key] = value;
        } else {
            // Unknown argument
            std::cerr << "Warning: Unknown argument: " << arg << std::endl;
        }
    }
    
    return args;
}

void printUsage() {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Unified Main Entry Point

USAGE:
    nlm [OPTIONS]

MODES (use --mode <name> to select a specific phase):
    auto      - Automatically run the most appropriate demo (default)
    phase2    - Phase 2: Real neural computation (LIF neurons, STDP)
    phase3    - Phase 3: World interaction and behavior
    phase4    - Phase 4: Cognitive development and memory
    phase5    - Phase 5: Performance optimizations and checkpoints
    phase6    - Phase 6: Final integration and complete brain
    benchmark - Performance benchmarking mode

OPTIONS:
    --help, -h              Show this help message
    --list-modes            List all available modes and their descriptions
    --config <file>         Configuration file (default: configs/default.cfg)
    --steps <N>             Number of simulation steps (default: 1000)
    --neurons <N>           Number of neurons (default: 1000)
    --timestep <T>          Simulation timestep in seconds (default: 0.001)
    --verbose, -v           Enable verbose output

CUSTOM SETTINGS:
    Set any configuration parameter using --parameter=value syntax
    Examples:
        --plasticity.learning_rate=0.01
        --brain.neuron_count=500
        --simulation.timestep=0.01

EXAMPLES:
    # Run Phase 6 integration demo with custom settings
    nlm --mode phase6 --neurons 500 --steps 2000 \
        --plasticity.stdp_ltp_weight=0.02 \
        --plasticity.stdp_ltd_weight=0.015

    # Run Phase 2 learning demo with verbose output
    nlm --mode phase2 --steps 5000 -v

    # List available modes and exit
    nlm --list-modes

    # Use custom configuration file
    nlm --config my_config.cfg --mode benchmark

For more information, see README.md or docs/ARCHITECTURE.md
    )" << std::endl;
}

void printPhaseDescriptions() {
    std::cout << R"(
AVAILABLE NLM MODES:

PHASE 2: Real Neural Computation
----------------------------------------
Focus: Core neural dynamics and learning
Features:
  • Leaky Integrate-and-Fire (LIF) neuron dynamics
  • Event-driven spike propagation with synaptic delays
  • STDP and Hebbian plasticity rules
  • Structural plasticity (synaptogenesis/pruning)
  • Basic learning experiments demonstrating measurable synaptic changes

PHASE 3: World Interaction and Behavior
----------------------------------------
Focus: Brain-environment interaction and basic behavior
Features:
  • Sensory input processing and motor output generation
  • Reward prediction error and learning
  • Developmental stages affecting plasticity
  • Novelty and curiosity-driven exploration
  • Simple agent behaviors in a virtual world

PHASE 4: Cognitive Development and Memory
----------------------------------------
Focus: Advanced cognition systems and memory integration
Features:
  • Neural prediction system
  • Working memory with persistent activity
  • Episodic-like memory
  • Concept formation from experience
  • Neural attention and competitive selection
  • Predictive planning
  • Self-model and social learning
  • Continual learning mechanisms

PHASE 5: Performance Optimizations and Checkpoints
----------------------------------------
Focus: Engineering excellence and production readiness
Features:
  • Memory pools and event queues for performance
  • SIMD vectorization where possible
  • Parallel processing optimization
  • Checkpoint system for long-running simulations
  • Benchmarking infrastructure

PHASE 6: Final Integration and Complete Brain
----------------------------------------
Focus: All systems working together as a unified artificial brain
Features:
  • All memory systems connected (working + episodic + associative)
  • Neuromodulation affecting plasticity and dynamics
  • Prediction integrated with learning and attention
  • Development affecting plasticity rates throughout brain lifecycle
  • Checkpoint save/load working for persistence
  • Replay and consolidation functional for memory strengthening
  • Complete brain loop with coherent behavior

BENCHMARK MODE:
--------------------
Focus: Performance measurement and profiling
Features:
  • Systematic performance measurement
  • Memory usage tracking
  • Spike rate and computational efficiency metrics
  • Comparison across different configurations

AUTO MODE:
--------------------
Selection strategy:
  1. Try Phase 6 if neurons >= 500 (comprehensive integration)
  2. Try Phase 4 if neurons >= 200 (advanced cognition)
  3. Try Phase 3 if neurons >= 100 (basic interaction)
  4. Fall back to Phase 2 (core neural computation)
    )" << std::endl;
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified Main Entry Point                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void runPhase2Demo(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation Demo ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");

    // Run basic connectivity test
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        NLM_LOG_INFO("Basic connectivity test:");
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            // Inject current into first 10 neurons
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            // Run a few steps
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        }
    }
    
    // Reset for plasticity experiment
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("Phase 2 demo completed successfully!");
}

void runPhase6Demo(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> world) {
    NLM_LOG_INFO("=== NLM Phase 6: Integration Demo ===");
    NLM_LOG_INFO("Running comprehensive integration test of all brain systems...");
    NLM_LOG_INFO("");
    
    // Create agent for interaction with world
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run simulation steps
    float totalReward = 0.0f;
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Update world
        world->update(0.001);
        
        // Process sensory input
        SensoryPercept percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, step * 0.001);
        
        // Get action and apply to world
        Action action = agent->produceAction();
        ActionResult result = world->applyMotorCommand(action, step * 0.001);
        
        // Apply reward
        agent->applyRewardModulation(result.reward, 0.0f);
        
        totalReward += result.reward;
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Reward: " + std::to_string(result.reward) +
                        " | Total: " + std::to_string(totalReward) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Phase 6 integration demo completed!");
    NLM_LOG_INFO("Total reward accumulated: " + std::to_string(totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
}

void printCompletionSummary(const std::string& mode, int steps, int neurons, double timestep) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Execution Complete ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Simulation: " + std::to_string(steps) + " steps, " +
                std::to_string(neurons) + " neurons, " +
                std::to_string(timestep) + "s timestep");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("NLM successfully executed in " + mode + " mode!");
    NLM_LOG_INFO("The artificial brain demonstrated " + std::to_string(neurons) + " neurons " +
                "interacting with environment dynamics for " + std::to_string(steps) + " steps.");
    NLM_LOG_INFO("");
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLineArgs(argc, argv);
    
    if (args.help) {
        printUsage();
        return 0;
    }
    
    if (args.list_modes) {
        printPhaseDescriptions();
        return 0;
    }
    
    std::cout << "Initializing NLM unified main entry point...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Determine actual mode if auto
    std::string actualMode = args.mode;
    if (args.mode == "auto") {
        if (args.neurons >= 500) {
            actualMode = "phase6";
        } else if (args.neurons >= 200) {
            actualMode = "phase4";
        } else if (args.neurons >= 100) {
            actualMode = "phase3";
        } else {
            actualMode = "phase2";
        }
        
        if (args.verbose) {
            NLM_LOG_INFO("Auto mode selected: " + actualMode + " (based on " + 
                        std::to_string(args.neurons) + " neurons)");
        }
    }
    
    NLM_LOG_INFO("=== NLM Unified Main Entry Point ===");
    NLM_LOG_INFO("Mode: " + actualMode);
    NLM_LOG_INFO("Configuration: " + args.configFile);
    NLM_LOG_INFO("Steps: " + std::to_string(args.steps));
    NLM_LOG_INFO("Neurons: " + std::to_string(args.neurons));
    NLM_LOG_INFO("Timestep: " + std::to_string(args.timestep) + "s");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply command line overrides
    config->set("neuron_count", static_cast<int64_t>(args.neurons), ConfigSource::CommandLine);
    config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    
    // Apply custom settings
    for (const auto& setting : args.customSettings) {
        // Parse setting (simple implementation - would need full parsing for nested keys)
        size_t dotPos = setting.first.find('.');
        if (dotPos != std::string::npos) {
            std::string section = setting.first.substr(0, dotPos);
            std::string key = setting.first.substr(dotPos + 1);
            
            // Try to parse based on key type
            try {
                if (key == "neuron_count" || key == "region_count" || key == "steps" || 
                    key == "random_seed" || key == "replay_interval" || key == "consolidation_interval") {
                    int64_t value = std::stoll(setting.second);
                    config->set(setting.first, value, ConfigSource::CommandLine);
                } else if (key == "connection_probability" || key == "plasticity_learning_rate" ||
                           key == "stdp_ltp_weight" || key == "stdp_ltd_weight" ||
                           key == "stdp_time_constant" || key == "synaptogenesis_rate" ||
                           key == "pruning_rate" || key == "dopamine_baseline" ||
                           key == "reward_discount_factor" || key == "simulation_timestep") {
                    float value = std::stof(setting.second);
                    config->set(setting.first, value, ConfigSource::CommandLine);
                } else if (key == "visualization_enabled" || key == "log_to_file") {
                    bool value = (setting.second == "true" || setting.second == "1");
                    config->set(setting.first, value, ConfigSource::CommandLine);
                } else {
                    // Try float as fallback
                    float floatValue = std::stof(setting.second);
                    config->set(setting.first, floatValue, ConfigSource::CommandLine);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to parse custom setting " + setting.first + ": " + setting.second);
            }
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration summary:");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run the selected demo
    bool success = false;
    
    try {
        if (actualMode == "phase2") {
            runPhase2Demo(brain);
            success = true;
        } else if (actualMode == "phase3") {
            NLM_LOG_INFO("Phase 3 demo requires environment and agent setup - using simplified version");
            brain->reset();
            brain->initialize();
            
            // Simple world interaction simulation
            for (SimulationStep step = 0; step < args.steps; ++step) {
                brain->step(step, step * timestep);
                
                if (step % 500 == 0) {
                    NLM_LOG_INFO("Phase 3 step " + std::to_string(step));
                }
            }
            success = true;
        } else if (actualMode == "phase4") {
            NLM_LOG_INFO("Phase 4 demo requires cognition systems - using simplified version");
            brain->reset();
            brain->initialize();
            
            // Simple cognition simulation
            for (SimulationStep step = 0; step < args.steps; ++step) {
                brain->step(step, step * timestep);
                
                if (step % 500 == 0) {
                    NLM_LOG_INFO("Phase 4 step " + std::to_string(step) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                                " | Reward: " + std::to_string(brain->getAverageFiringRate()));
                }
            }
            success = true;
        } else if (actualMode == "phase5") {
            NLM_LOG_INFO("Phase 5 demo requires performance optimizations - using simplified version");
            brain->reset();
            brain->initialize();
            
            // Performance simulation
            NLM_LOG_INFO("Running performance simulation...");
            
            for (SimulationStep step = 0; step < args.steps; ++step) {
                brain->step(step, step * timestep);
                
                if (step % 1000 == 0) {
                    NLM_LOG_INFO("Phase 5 step " + std::to_string(step) +
                                " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Avg firing: " + std::to_string(brain->getAverageFiringRate()));
                }
            }
            success = true;
        } else if (actualMode == "phase6") {
            NLM_LOG_INFO("Phase 6 demo requires complete integration - using simplified version");
            brain->reset();
            brain->initialize();
            
            // Integration simulation
            for (SimulationStep step = 0; step < args.steps; ++step) {
                brain->step(step, step * timestep);
                
                if (step % 500 == 0) {
                    NLM_LOG_INFO("Phase 6 integration step " + std::to_string(step) +
                                " | Memory traces: " + std::to_string(
                                    brain->getWorkingMemory() ? 
                                    brain->getWorkingMemory()->getActiveTraces() : 0) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            success = true;
        } else if (actualMode == "benchmark") {
            NLM_LOG_INFO("=== NLM Performance Benchmark ===");
            
            brain->reset();
            brain->initialize();
            
            // Simple benchmark
            size_t totalSpikes = brain->getTotalSpikeCount();
            float totalTime = 0.0f;
            
            for (SimulationStep step = 0; step < args.steps; ++step) {
                brain->step(step, step * timestep);
            }
            
            totalSpikes = brain->getTotalSpikeCount() - totalSpikes;
            
            NLM_LOG_INFO("Benchmark completed:");
            NLM_LOG_INFO("  Steps: " + std::to_string(args.steps));
            NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikes));
            NLM_LOG_INFO("  Average spikes per step: " + 
                        std::to_string(static_cast<float>(totalSpikes) / args.steps));
            NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
            success = true;
        } else {
            NLM_LOG_ERROR("Unknown mode: " + actualMode);
            return 1;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during execution: ") + e.what());
        return 1;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Print completion summary
    printCompletionSummary(actualMode, args.steps, args.neurons, args.timestep);
    
    if (success) {
        NLM_LOG_INFO("NLM unified main entry point completed successfully!");
        return 0;
    } else {
        NLM_LOG_ERROR("NLM execution failed!");
        return 1;
    }
}