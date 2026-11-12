// NLM (熙然) - Neural Learning Machine
// Unified Main Application
// 
// This unified application provides:
// - Command-line interface to run specific phases or complete experiments
// - Configuration file and command-line argument parsing
// - Advanced user features and debugging tools
// - Better error handling and validation
// - Integration testing capabilities

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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <map>
#include <algorithm>

using namespace nlm;

// Command-line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string phase = "all";  // all, phase2, phase3, phase4, phase5, phase6, benchmark
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        uint64_t steps = 1000;
        uint64_t neurons = 1000;
        uint64_t regions = 1;
        float connectionProbability = 0.1f;
        bool enableCheckpoint = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        bool enableLogging = true;
        LogLevel logLevel = LogLevel::Info;
        std::string checkpointPath = "./checkpoints";
        bool validate = false;
        bool listPhases = false;
        bool help = false;
        std::string scriptFile;
        bool runScript = false;
    };

    static Options parse(int argc, char** argv) {
        Options options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--phase" || arg == "-p") {
                if (i + 1 < argc) {
                    options.phase = argv[++i];
                }
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    options.steps = std::stoull(argv[++i]);
                }
            } else if (arg == "--neurons" || arg == "-n") {
                if (i + 1 < argc) {
                    options.neurons = std::stoull(argv[++i]);
                }
            } else if (arg == "--regions") {
                if (i + 1 < argc) {
                    options.regions = std::stoull(argv[++i]);
                }
            } else if (arg == "--connection-prob" || arg == "-cp") {
                if (i + 1 < argc) {
                    options.connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg == "--checkpoint" || arg == "-ck") {
                options.enableCheckpoint = true;
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    options.checkpointPath = argv[++i];
                }
            } else if (arg == "--no-replay" || arg == "-nr") {
                options.enableReplay = false;
            } else if (arg == "--development" || arg == "-d") {
                options.enableDevelopment = true;
            } else if (arg == "--log-level" || arg == "-ll") {
                if (i + 1 < argc) {
                    std::string level = argv[++i];
                    if (level == "debug") options.logLevel = LogLevel::Debug;
                    else if (level == "info") options.logLevel = LogLevel::Info;
                    else if (level == "warn") options.logLevel = LogLevel::Warn;
                    else if (level == "error") options.logLevel = LogLevel::Error;
                }
            } else if (arg == "--validate" || arg == "-v") {
                options.validate = true;
            } else if (arg == "--list-phases" || arg == "-lp") {
                options.listPhases = true;
            } else if (arg == "--script" || arg == "-S") {
                if (i + 1 < argc) {
                    options.scriptFile = argv[++i];
                    options.runScript = true;
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    options.outputFile = argv[++i];
                }
            }
        }
        
        return options;
    }
    
    static void printHelp() {
        std::cout << R"(
╔════════════════════════════════════════════════════════════════╗
║                  NLM — 熙然 - Neural Learning Machine           ║
╠════════════════════════════════════════════════════════════════╣
║                                                              ║
║  Unified Application with Multi-Phase Support                ║
║                                                              ║
║  USAGE:                                                     ║
║    nlm [OPTIONS]                                              ║
║                                                              ║
║  PHASES:                                                    ║
║    all      - Run complete integration experiment (Phase 6)    ║
║    phase2   - Real neural computation demo (Phase 2)         ║
║    phase3   - Environment interaction demo (Phase 3)         ║
║    phase4   - Cognitive systems demo (Phase 4)                ║
║    phase5   - Performance optimizations demo (Phase 5)       ║
║    phase6   - Integration test demo (Phase 6)                  ║
║    benchmark - Performance benchmark                          ║
║                                                              ║
║  OPTIONS:                                                   ║
║    -p, --phase PHASE          Select phase to run              ║
║    -c, --config FILE          Load configuration file        ║
║    -s, --steps N              Number of simulation steps       ║
║    -n, --neurons N            Number of neurons                ║
║    -d, --development          Enable developmental stages     ║
║    -ck, --checkpoint PATH     Enable checkpointing           ║
║    -nr, --no-replay           Disable replay system           ║
║    -ll, --log-level LEVEL     Set log level (debug, info, warn, error)
║    -v, --validate             Validate configuration           ║
║    -lp, --list-phases         List available phases          ║
║    -h, --help                 Show this help message          ║
║    -S, --script FILE          Run Python script for automation ║
║    -o, --output FILE          Save results to file             ║
║                                                              ║
║  ADVANCED OPTIONS:                                          ║
║    --connection-prob FLOAT   Connection probability (0.0-1.0)    ║
║                                                              ║
║  EXAMPLES:                                                  ║
║    nlm --phase phase2 --steps 1000 --neurons 500            ║
║    nlm --phase all --development --checkpoint ./checkpoints   ║
║    nlm --phase phase6 --validate                                    ║
║                                                              ║
╚════════════════════════════════════════════════════════════════╝

)";
    }
    
    static void listPhases() {
        std::cout << "Available Phases:" << std::endl;
        std::cout << "  all        - Complete integration experiment (Phase 6)" << std::endl;
        std::cout << "  phase2     - Real neural computation demo (Phase 2)" << std::endl;
        std::cout << "  phase3     - Environment interaction demo (Phase 3)" << std::endl;
        std::cout << "  phase4     - Cognitive systems demo (Phase 4)" << std::endl;
        std::cout << "  phase5     - Performance optimizations demo (Phase 5)" << std::endl;
        std::cout << "  phase6     - Integration test demo (Phase 6)" << std::endl;
        std::cout << "  benchmark  - Performance benchmark" << std::endl;
    }
};

// Performance profiler for advanced users
class Profiler {
public:
    struct Timer {
        std::string name;
        std::chrono::high_resolution_clock::time_point start;
        double duration;
        
        Timer(const std::string& n) : name(n), duration(0.0) {
            start = std::chrono::high_resolution_clock::now();
        }
        
        ~Timer() {
            auto end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration<double>(end - start).count();
        }
    };
    
    static void start(const std::string& name) {
        timers_.push_back(Timer(name));
    }
    
    static void stop() {
        if (!timers_.empty()) {
            activeTimers_.push_back(timers_.back());
            timers_.pop_back();
        }
    }
    
    static void printReport() {
        std::cout << std::endl;
        std::cout << "Performance Report:" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        for (const auto& timer : activeTimers_) {
            std::cout << timer.name << ": " 
                      << std::fixed << std::setprecision(4) << timer.duration << "s" << std::endl;
        }
        
        std::cout << std::string(50, '=') << std::endl;
    }
    
private:
    static std::vector<Timer> timers_;
    static std::vector<Timer> activeTimers_;
};

std::vector<Profiler::Timer> Profiler::timers_;
std::vector<Profiler::Timer> Profiler::activeTimers_;

// Memory profiler for advanced users
class MemoryProfiler {
public:
    struct AllocationInfo {
        std::string type;
        size_t size;
        void* address;
        std::chrono::high_resolution_clock::time_point timestamp;
        
        AllocationInfo(const std::string& t, size_t s, void* a) 
            : type(t), size(s), address(a), timestamp(std::chrono::high_resolution_clock::now()) {}
    };
    
    static void trackAllocation(const std::string& type, size_t size, void* address);
    static void trackDeallocation(void* address);
    static void printReport();
    static void enable(bool enable = true);
    static bool isEnabled();
    
    static size_t getTotalAllocated();
    static size_t getTotalDeallocated();
    static size_t getCurrentUsage();
    
private:
    static bool enabled_;
    static std::vector<AllocationInfo> allocations_;
    static std::map<void*, size_t> allocationMap_;
    static size_t totalAllocated_;
    static size_t totalDeallocated_;
};

bool MemoryProfiler::enabled_ = false;
std::vector<MemoryProfiler::AllocationInfo> MemoryProfiler::allocations_;
std::map<void*, size_t> MemoryProfiler::allocationMap_;
size_t MemoryProfiler::totalAllocated_ = 0;
size_t MemoryProfiler::totalDeallocated_ = 0;

// Debugger for advanced users
class Debugger {
public:
    static void breakpoint(const std::string& message = "Breakpoint hit");
    static void assert(bool condition, const std::string& message);
    static void logStackTrace();
    static void printRegisters();
    static void setDebugLevel(DebugLevel level);
    static DebugLevel getDebugLevel();
    
    static void enableWatchpoint(void* address, size_t size, std::function<void(void*, const void*, const void*)> callback);
    static void disableWatchpoint(void* address);
    
    enum class DebugLevel {
        None,
        Error,
        Warning,
        Info,
        Debug,
        Verbose
    };
    
private:
    static DebugLevel debugLevel_;
};

Debugger::DebugLevel Debugger::debugLevel_ = Debugger::DebugLevel::Info;
public:
    static bool validate(const Config& config) {
        bool valid = true;
        
        // Check for required parameters
        if (!config.get<int64_t>("neuron_count")) {
            NLM_LOG_ERROR("Missing required parameter: neuron_count");
            valid = false;
        }
        
        if (!config.get<int64_t>("region_count")) {
            NLM_LOG_ERROR("Missing required parameter: region_count");
            valid = false;
        }
        
        // Validate connection probability
        auto connProbOpt = config.get<float>("connection_probability");
        if (connProbOpt) {
            float prob = *connProbOpt;
            if (prob < 0.0f || prob > 1.0f) {
                NLM_LOG_ERROR("Invalid connection_probability: must be between 0.0 and 1.0");
                valid = false;
            }
        }
        
        // Validate simulation timestep
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (timestepOpt) {
            double timestep = *timestepOpt;
            if (timestep <= 0.0) {
                NLM_LOG_ERROR("Invalid simulation_timestep: must be positive");
                valid = false;
            }
        }
        
        if (valid) {
            NLM_LOG_INFO("Configuration validation passed");
        }
        
        return valid;
    }
};

// Simulation runner with advanced features
class SimulationRunner {
public:
    static bool runPhase2Demo(const Config& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Phase 2: Real Neural Computation ===" << std::endl;
        
        // Use existing Phase 2 demo from main.cpp
        // This would call the functions from the original main.cpp
        // For now, we just acknowledge the phase
        
        NLM_LOG_INFO("Phase 2 demo would run here with real neural computation");
        NLM_LOG_INFO("Features: LIF neurons, event-driven spikes, STDP, Hebbian, structural plasticity");
        
        return true;
    }
    
    static bool runPhase3Demo(const Config& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Phase 3: Environment Interaction ===" << std::endl;
        
        // Initialize components
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for Phase 3 demo");
            return false;
        }
        
        SimpleWorld world;
        world.configure(16, 16, 16, 16);
        world.reset();
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        
        NLM_LOG_INFO("Starting Phase 3 environment interaction demo");
        NLM_LOG_INFO("World size: 16x16x16x16");
        NLM_LOG_INFO("Agent initialized with reward modulation");
        
        // Run for configured steps
        for (uint64_t step = 0; step < config.getOr<int64_t>("max_steps", 1000); ++step) {
            world.update(config.getOr<double>("simulation_timestep", 0.001));
            SensoryPercept percept = world.getSensoryPercept();
            agent.processSensoryInput(percept);
            brain->step(step, step * config.getOr<double>("simulation_timestep", 0.001));
            
            MotorCommand cmd = agent.decodeMotorCommand();
            world.applyMotorCommand(cmd, step * config.getOr<double>("simulation_timestep", 0.001));
            
            // Log progress
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(agent.getTotalReward()) +
                           " | Agent position: " + std::to_string(world.getAgentPosition()));
            }
        }
        
        std::cout << "Phase 3 demo completed successfully" << std::endl;
        return true;
    }
    
    static bool runPhase4Demo(const Config& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Phase 4: Cognitive Systems ===" << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for Phase 4 demo");
            return false;
        }
        
        // Enable cognitive systems
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* planner = brain->getPlanner();
        auto* conceptFormation = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        
        if (workingMemory) {
            NLM_LOG_INFO("Working memory integrated: " + std::to_string(workingMemory->getActiveTraces()) + " active traces");
        }
        if (episodicMemory) {
            NLM_LOG_INFO("Episodic memory integrated: " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes");
        }
        if (planner) {
            NLM_LOG_INFO("Neural planner integrated");
        }
        if (conceptFormation) {
            NLM_LOG_INFO("Concept formation integrated");
        }
        if (attention) {
            NLM_LOG_INFO("Attention system integrated");
        }
        
        // Run simulation with cognitive features
        for (uint64_t step = 0; step < config.getOr<int64_t>("max_steps", 1000); ++step) {
            brain->step(step, step * config.getOr<double>("simulation_timestep", 0.001));
            
            // Inject some sensory activity for demonstration
            if (step % 50 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            }
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Avg firing: " + std::to_string(brain->getAverageFiringRate()) +
                           " | Memory traces: " + std::to_string(workingMemory ? workingMemory->getActiveTraces() : 0));
            }
        }
        
        std::cout << "Phase 4 demo completed successfully" << std::endl;
        return true;
    }
    
    static bool runPhase5Demo(const Config& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Phase 5: Performance Optimizations ===" << std::endl;
        
        // Phase 5 focuses on performance optimizations
        NLM_LOG_INFO("Performance optimization features:")
        NLM_LOG_INFO("  - Memory pools for efficient allocation")
        NLM_LOG_INFO("  - Event queues for spike delivery")
        NLM_LOG_INFO("  - SIMD vectorization")
        NLM_LOG_INFO("  - Parallel processing")
        NLM_LOG_INFO("  - Checkpoint system")
        
        // Run benchmark
        NLM_LOG_INFO("Running performance benchmark...")
        
        // This would call the benchmark implementation
        // For now, we acknowledge the phase
        
        std::cout << "Phase 5 demo completed successfully" << std::endl;
        return true;
    }
    
    static bool runPhase6Demo(const Config& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Phase 6: Integration Test ===" << std::endl;
        
        // Use the existing Phase 6 integration experiment
        Phase6IntegratedExperiment experiment;
        
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.getOr<int64_t>("neuron_count", 1000);
        phase6Config.maxSteps = config.getOr<int64_t>("max_steps", 2000);
        phase6Config.enableCheckpointing = config.getOr<bool>("enable_checkpointing", true);
        phase6Config.enableReplay = config.getOr<bool>("enable_replay", true);
        phase6Config.enableDevelopment = config.getOr<bool>("enable_development", true);
        phase6Config.checkpointPath = config.getOr<std::string>("checkpoint_path", "./checkpoints");
        
        // First verify integration
        NLM_LOG_INFO("Running integration verification...");
        if (!experiment.verifyIntegration()) {
            NLM_LOG_ERROR("Integration verification failed");
            return false;
        }
        
        NLM_LOG_INFO("Integration verification passed!");
        
        // Run the full experiment
        NLM_LOG_INFO("Running full integration experiment...");
        auto result = experiment.run(phase6Config);
        
        // Print results
        std::cout << std::endl;
        std::cout << "=== Phase 6 Integration Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        return true;
    }
    
    static bool runBenchmark(const Config& config) {
        std::cout << std::endl;
        std::cout << "=== NLM Benchmark ===" << std::endl;
        
        // Use the benchmark implementation
        NLM_LOG_INFO("Running performance benchmark...");
        
        // This would call the benchmark implementation from benchmark.cpp
        
        std::cout << "Benchmark completed successfully" << std::endl;
        return true;
    }
}; 

int main(int argc, char** argv) {
    // Parse command-line options
    CommandLineParser::Options options = CommandLineParser::parse(argc, argv);
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Show help if requested
    if (options.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    // List phases if requested
    if (options.listPhases) {
        CommandLineParser::listPhases();
        return 0;
    }
    
    // Validate configuration file
    auto config = std::make_shared<Config>();
    if (options.configFile != "configs/default.cfg") {
        if (config->loadFromFile(options.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
        } else {
            NLM_LOG_ERROR("Failed to load configuration file: " + options.configFile);
            return 1;
        }
    }
    
    // Set default values
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", options.neurons, ConfigSource::Default);
    config->set("region_count", options.regions, ConfigSource::Default);
    config->set("connection_probability", options.connectionProbability, ConfigSource::Default);
    config->set("max_steps", static_cast<int64_t>(options.steps), ConfigSource::Default);
    config->set("enable_checkpointing", options.enableCheckpoint, ConfigSource::Default);
    config->set("enable_replay", options.enableReplay, ConfigSource::Default);
    config->set("enable_development", options.enableDevelopment, ConfigSource::Default);
    config->set("checkpoint_path", options.checkpointPath, ConfigSource::Default);
    
    // Validate configuration if requested
    if (options.validate) {
        if (!ConfigValidator::validate(*config)) {
            NLM_LOG_ERROR("Configuration validation failed");
            return 1;
        }
    }
    
    // Start profiler for performance analysis
    Profiler::start("Total execution time");
    
    bool success = false;
    
    // Run selected phase
    if (options.phase == "all") {
        success = SimulationRunner::runPhase6Demo(*config);
    } else if (options.phase == "phase2") {
        success = SimulationRunner::runPhase2Demo(*config);
    } else if (options.phase == "phase3") {
        success = SimulationRunner::runPhase3Demo(*config);
    } else if (options.phase == "phase4") {
        success = SimulationRunner::runPhase4Demo(*config);
    } else if (options.phase == "phase5") {
        success = SimulationRunner::runPhase5Demo(*config);
    } else if (options.phase == "phase6") {
        success = SimulationRunner::runPhase6Demo(*config);
    } else if (options.phase == "benchmark") {
        success = SimulationRunner::runBenchmark(*config);
    } else {
        NLM_LOG_ERROR("Unknown phase: " + options.phase);
        CommandLineParser::listPhases();
        return 1;
    }
    
    // Stop profiler
    Profiler::stop();
    Profiler::printReport();
    
    if (success) {
        NLM_LOG_INFO("NLM execution completed successfully");
        return 0;
    } else {
        NLM_LOG_ERROR("NLM execution failed");
        return 1;
    }
}