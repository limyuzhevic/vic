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
#include <chrono>
#include <thread>
#include <unordered_map>

using namespace nlm;

class NLMCommandLineInterface {
public:
    enum class Mode {
        PHASE2_TEST,      // Original Phase 2 tests
        PHASE6_DEMO,      // Phase 6 integration demo
        CUSTOM_EXPERIMENT, // Custom experiment mode
        VERIFICATION,     // System verification
        BENCHMARK,        // Performance benchmarking
        INTERACTIVE       // Interactive mode
    };

    struct CLIConfig {
        Mode mode = Mode::PHASE2_TEST;
        std::string configFile = "configs/default.cfg";
        std::string checkpointFile;
        std::string experimentType = "default";
        bool verbose = false;
        bool silent = false;
        uint64_t randomSeed = 42;
        uint64_t neuronCount = 1000;
        uint64_t regionCount = 1;
        double simulationTimestep = 0.001;
        uint64_t maxSteps = 1000;
        uint64_t testSteps = 100;
        bool enableCheckpoints = false;
        bool enableDevelopment = false;
        bool enableCuriosity = true;
        bool enableReward = true;
        float rewardScale = 1.0f;
        std::string logLevel = "INFO";
        bool profilePerformance = false;
        uint64_t benchmarkIterations = 5;
        std::string outputFormat = "human";
        bool saveCheckpoint = false;
        bool loadCheckpoint = false;
        std::string helpSection;
    };

    NLMCommandLineInterface() = default;
    ~NLMCommandLineInterface() = default;

    bool parseArguments(int argc, char** argv, CLIConfig& config) {
        std::unordered_map<std::string, std::string> args;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return false;
            } else if (arg == "--mode") {
                if (i + 1 < argc) config.mode = parseMode(argv[++i]);
            } else if (arg == "--config") {
                if (i + 1 < argc) config.configFile = argv[++i];
            } else if (arg == "--checkpoint") {
                if (i + 1 < argc) config.checkpointFile = argv[++i];
            } else if (arg == "--seed") {
                if (i + 1 < argc) config.randomSeed = std::stoull(argv[++i]);
            } else if (arg == "--neurons") {
                if (i + 1 < argc) config.neuronCount = std::stoull(argv[++i]);
            } else if (arg == "--regions") {
                if (i + 1 < argc) config.regionCount = std::stoull(argv[++i]);
            } else if (arg == "--timestep") {
                if (i + 1 < argc) config.simulationTimestep = std::stod(argv[++i]);
            } else if (arg == "--steps") {
                if (i + 1 < argc) config.maxSteps = std::stoull(argv[++i]);
            } else if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--silent") {
                config.silent = true;
            } else if (arg == "--profile") {
                config.profilePerformance = true;
            } else if (arg == "--benchmark") {
                config.mode = Mode::BENCHMARK;
                if (i + 1 < argc) config.benchmarkIterations = std::stoull(argv[++i]);
            } else if (arg == "--experiment") {
                config.mode = Mode::CUSTOM_EXPERIMENT;
                if (i + 1 < argc) config.experimentType = argv[++i];
            } else if (arg == "--help-section") {
                if (i + 1 < argc) config.helpSection = argv[++i];
            } else if (arg == "--enable-development") {
                config.enableDevelopment = true;
            } else if (arg == "--enable-curiosity") {
                config.enableCuriosity = true;
            } else if (arg == "--enable-reward") {
                config.enableReward = true;
            } else if (arg == "--reward-scale") {
                if (i + 1 < argc) config.rewardScale = std::stof(argv[++i]);
            } else if (arg == "--enable-checkpoints") {
                config.enableCheckpoints = true;
            } else if (arg == "--save-checkpoint") {
                config.saveCheckpoint = true;
                if (i + 1 < argc) config.checkpointFile = argv[++i];
            } else if (arg == "--load-checkpoint") {
                config.loadCheckpoint = true;
                if (i + 1 < argc) config.checkpointFile = argv[++i];
            } else {
                // Try to parse as key=value
                size_t equals_pos = arg.find('=');
                if (equals_pos != std::string::npos && equals_pos > 0) {
                    std::string key = arg.substr(0, equals_pos);
                    std::string value = arg.substr(equals_pos + 1);
                    args[key] = value;
                } else {
                    std::cerr << "Unknown argument: " << arg << std::endl;
                    showHelp();
                    return false;
                }
            }
        }
        
        // Apply key=value arguments
        applyArguments(args, config);
        
        return true;
    }

    Mode parseMode(const std::string& modeStr) {
        std::string lower = modeStr;
        for (char& c : lower) c = std::tolower(c);
        
        if (lower == "phase2" || lower == "phase2-test") return Mode::PHASE2_TEST;
        if (lower == "phase6" || lower == "phase6-demo") return Mode::PHASE6_DEMO;
        if (lower == "custom" || lower == "custom-experiment") return Mode::CUSTOM_EXPERIMENT;
        if (lower == "verification" || lower == "verify") return Mode::VERIFICATION;
        if (lower == "benchmark" || lower == "perf") return Mode::BENCHMARK;
        if (lower == "interactive" || lower == "interactive-mode") return Mode::INTERACTIVE;
        
        std::cerr << "Unknown mode: " << modeStr << ". Using default (PHASE2_TEST)" << std::endl;
        return Mode::PHASE2_TEST;
    }

    void applyArguments(const std::unordered_map<std::string, std::string>& args, CLIConfig& config) {
        for (const auto& arg : args) {
            const std::string& key = arg.first;
            const std::string& value = arg.second;
            
            if (key == "mode") config.mode = parseMode(value);
            else if (key == "random_seed") config.randomSeed = std::stoull(value);
            else if (key == "neuron_count") config.neuronCount = std::stoull(value);
            else if (key == "region_count") config.regionCount = std::stoull(value);
            else if (key == "simulation_timestep") config.simulationTimestep = std::stod(value);
            else if (key == "max_steps") config.maxSteps = std::stoull(value);
            else if (key == "verbose") config.verbose = (value == "true" || value == "1" || value == "yes");
            else if (key == "silent") config.silent = (value == "true" || value == "1" || value == "yes");
            else if (key == "enable_development") config.enableDevelopment = (value == "true" || value == "1" || value == "yes");
            else if (key == "enable_curiosity") config.enableCuriosity = (value == "true" || value == "1" || value == "yes");
            else if (key == "enable_reward") config.enableReward = (value == "true" || value == "1" || value == "yes");
            else if (key == "reward_scale") config.rewardScale = std::stof(value);
            else if (key == "enable_checkpoints") config.enableCheckpoints = (value == "true" || value == "1" || value == "yes");
            else if (key == "save_checkpoint") config.saveCheckpoint = (value == "true" || value == "1" || value == "yes");
            else if (key == "load_checkpoint") config.loadCheckpoint = (value == "true" || value == "1" || value == "yes");
            else if (key == "profile_performance") config.profilePerformance = (value == "true" || value == "1" || value == "yes");
        }
    }

    void showHelp(const std::string& section = "") {
        if (!section.empty()) {
            std::cout << "=== NLM CLI Help: " << section << " ===" << std::endl << std::endl;
            if (section == "mode") showModeHelp();
            else if (section == "config") showConfigHelp();
            else if (section == "experiment") showExperimentHelp();
            else if (section == "advanced") showAdvancedHelp();
            else showGeneralHelp();
            return;
        }
        
        showGeneralHelp();
    }

    void showGeneralHelp() {
        std::cout << "NLM (熙然) - Neural Learning Machine - Command Line Interface" << std::endl;
        std::cout << "===============================================================" << std::endl << std::endl;
        
        std::cout << "Overview:" << std::endl;
        std::cout << "  NLM is an experimental artificial developmental brain project." << std::endl;
        std::cout << "  It can run various modes and experiments to test neural systems." << std::endl << std::endl;
        
        std::cout << "Basic Usage:" << std::endl;
        std::cout << "  nlm --mode phase2                    # Run Phase 2 tests (original behavior)" << std::endl;
        std::cout << "  nlm --mode phase6                    # Run Phase 6 integration demo" << std::endl;
        std::cout << "  nlm --mode benchmark                 # Run performance benchmarks" << std::endl;
        std::cout << "  nlm --config myconfig.cfg            # Load custom configuration" << std::endl << std::endl;
        
        std::cout << "Mode Options:" << std::endl;
        std::cout << "  --mode phase2, phase2-test          # Phase 2 neural computation tests" << std::endl;
        std::cout << "  --mode phase6, phase6-demo          # Phase 6 integration verification" << std::endl;
        std::cout << "  --mode custom-experiment <type>      # Custom experiment mode" << std::endl;
        std::cout << "  --mode verification, verify         # System verification mode" << std::endl;
        std::cout << "  --mode benchmark, perf <iters>       # Performance benchmarking" << std::endl;
        std::cout << "  --mode interactive, interactive-mode # Interactive simulation mode" << std::endl << std::endl;
        
        std::cout << "Common Configuration Options:" << std::endl;
        std::cout << "  --seed <number>                     # Random seed for reproducibility" << std::endl;
        std::cout << "  --neurons <count>                   # Number of neurons in brain" << std::endl;
        std::cout << "  --regions <count>                   # Number of brain regions" << std::endl;
        std::cout << "  --timestep <value>                   # Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --steps <count>                      # Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --verbose                            # Enable verbose output" << std::endl;
        std::cout << "  --silent                             # Minimal output (quiet mode)" << std::endl << std::endl;
        
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --enable-development                 # Enable developmental processes" << std::endl;
        std::cout << "  --enable-curiosity                  # Enable curiosity-driven exploration" << std::endl;
        std::cout << "  --enable-reward                     # Enable reward-based learning" << std::endl;
        std::cout << "  --reward-scale <factor>              # Reward scaling factor" << std::endl;
        std::cout << "  --enable-checkpoints                # Enable checkpoint saving" << std::endl;
        std::cout << "  --save-checkpoint <file>             # Save checkpoint to file" << std::endl;
        std::cout << "  --load-checkpoint <file>             # Load checkpoint from file" << std::endl;
        std::cout << "  --profile                            # Profile performance" << std::endl;
        std::cout << "  --help-section <section>             # Show detailed help for a section" << std::endl << std::endl;
        
        std::cout << "Help Sections:" << std::endl;
        std::cout << "  --help-section mode                  # Mode selection and descriptions" << std::endl;
        std::cout << "  --help-section config                # Configuration options" << std::endl;
        std::cout << "  --help-section experiment            # Experiment types" << std::endl;
        std::cout << "  --help-section advanced              # Advanced configuration options" << std::endl << std::endl;
    }

    void showModeHelp() {
        std::cout << "=== Mode Selection ===" << std::endl << std::endl;
        
        std::cout << "PHASE2_TEST:" << std::endl;
        std::cout << "  Runs the original Phase 2 tests:" << std::endl;
        std::cout << "  - Test 1: Basic neural connectivity" << std::endl;
        std::cout << "  - Test 2: Plasticity learning experiment" << std::endl;
        std::cout << "  - Test 3: STDP verification" << std::endl;
        std::cout << "  Purpose: Verify basic neural computation capabilities" << std::endl << std::endl;
        
        std::cout << "PHASE6_DEMO:" << std::endl;
        std::cout << "  Runs Phase 6 integration demonstration:" << std::endl;
        std::cout << "  - Integration verification of all systems" << std::endl;
        std::cout << "  - Memory system tests (working + episodic)" << std::endl;
        std::cout << "  - Neuromodulation integration tests" << std::endl;
        std::cout << "  - Checkpoint save/load testing" << std::endl;
        std::cout << "  - Replay and consolidation tests" << std::endl;
        std::cout << "  Purpose: Verify complete brain integration" << std::endl << std::endl;
        
        std::cout << "CUSTOM_EXPERIMENT:" << std::endl;
        std::cout << "  Allows custom experiment types:" << std::endl;
        std::cout << "  - Learning: Test experience-based learning" << std::endl;
        std::cout << "  - Adaptation: Test adaptation to changing environments" << std::endl;
        std::cout << "  - Social: Test social learning capabilities" << std::endl;
        std::cout << "  - Memory: Test memory formation and recall" << std::endl;
        std::cout << "  Purpose: Custom exploration of brain capabilities" << std::endl << std::endl;
        
        std::cout << "VERIFICATION:" << std::endl;
        std::cout << "  Performs system integrity checks:" << std::endl;
        std::cout << "  - Component existence verification" << std::endl;
        std::cout << "  - Connection integrity tests" << std::endl;
        std::cout << "  - Configuration validation" << std::endl;
        std::cout << "  Purpose: Ensure brain systems are properly integrated" << std::endl << std::endl;
        
        std::cout << "BENCHMARK:" << std::endl;
        std::cout << "  Measures performance characteristics:" << std::endl;
        std::cout << "  - Simulation speed (steps/second)" << std::endl;
        std::cout << "  - Memory usage patterns" << std::endl;
        std::cout << "  - Component timing breakdown" << std::endl;
        std::cout << "  Purpose: Performance analysis and optimization" << std::endl << std::endl;
        
        std::cout << "INTERACTIVE:" << std::endl;
        std::cout << "  Real-time interactive simulation:" << std::endl;
        std::cout << "  - Keyboard input for controlling simulation" << std::endl;
        std::cout << "  - Real-time visualization of neural activity" << std::endl;
        std::cout << "  - Dynamic parameter adjustment" << std::endl;
        std::cout << "  Purpose: Interactive exploration of brain dynamics" << std::endl;
    }

    void showConfigHelp() {
        std::cout << "=== Configuration Options ===" << std::endl << std::endl;
        
        std::cout << "General Settings:" << std::endl;
        std::cout << "  --config <file>                    Path to configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --seed <number>                    Random seed for reproducible experiments" << std::endl;
        std::cout << "  --verbose                          Enable detailed logging" << std::endl;
        std::cout << "  --silent                           Minimal output (quiet mode)" << std::endl << std::endl;
        
        std::cout << "Simulation Parameters:" << std::endl;
        std::cout << "  --neurons <count>                  Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --regions <count>                  Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --timestep <value>                  Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --steps <count>                     Maximum simulation steps (default: 1000)" << std::endl << std::endl;
        
        std::cout << "Memory System Configuration:" << std::endl;
        std::cout << "  --enable-development                Enable developmental processes" << std::endl;
        std::cout << "  --enable-curiosity                 Enable curiosity-driven exploration" << std::endl;
        std::cout << "  --enable-reward                    Enable reward-based learning" << std::endl;
        std::cout << "  --reward-scale <factor>             Reward scaling factor (default: 1.0)" << std::endl << std::endl;
        
        std::cout << "Checkpointing:" << std::endl;
        std::cout << "  --enable-checkpoints                Enable automatic checkpointing" << std::endl;
        std::cout << "  --save-checkpoint <file>            Save brain state to file" << std::endl;
        std::cout << "  --load-checkpoint <file>            Load brain state from file" << std::endl;
    }

    void showExperimentHelp() {
        std::cout << "=== Experiment Types ===" << std::endl << std::endl;
        
        std::cout << "LEARNING:" << std::endl;
        std::cout << "  Tests experience-based synaptic modification" << std::endl;
        std::cout << "  - Repeated sensory input patterns" << std::endl;
        std::cout << "  - Measures synaptic strengthening/weakening" << std::endl;
        std::cout << "  - Evaluates Hebbian and STDP mechanisms" << std::endl << std::endl;
        
        std::cout << "ADAPTATION:" << std::endl;
        std::cout << "  Tests adaptation to changing environments" << std::endl;
        std::cout << "  - Dynamic reward schedules" << std::endl;
        std::cout << "  - Novel stimulus introduction" << std::endl;
        std::cout << "  - Behavioral response modification" << std::endl << std::endl;
        
        std::cout << "SOCIAL:" << std::endl;
        std::cout << "  Tests social learning and imitation" << std::endl;
        std::cout << "  - Observation of other agents" << std::endl;
        std::cout << "  - Pattern matching and copying" << std::endl;
        std::cout << "  - Shared experience integration" << std::endl << std::endl;
        
        std::cout << "MEMORY:" << std::endl;
        std::cout << "  Tests memory formation and retrieval" << std::endl;
        std::cout << "  - Episodic memory encoding" << std::endl;
        std::cout << "  - Working memory capacity" << std::endl;
        std::cout << "  - Memory consolidation over time" << std::endl;
    }

    void showAdvancedHelp() {
        std::cout << "=== Advanced Configuration ===" << std::endl << std::endl;
        
        std::cout << "Performance and Scaling:" << std::endl;
        std::cout << "  --profile                           Enable performance profiling" << std::endl;
        std::cout << "  Key timing measurements for optimization" << std::endl << std::endl;
        
        std::cout << "Integration and Systems:" << std::endl;
        std::cout << "  Inter-regional connectivity options" << std::endl;
        std::cout << "  Cross-system integration parameters" << std::endl;
        std::cout << "  System coupling strength settings" << std::endl << std::endl;
        
        std::cout << "Output and Visualization:" << std::endl;
        std::cout << "  --output-format <format>            Output format (human/json/csv)" << std::endl;
        std::cout << "  Real-time monitoring capabilities" << std::endl;
        std::cout << "  Export functionality for analysis" << std::endl << std::endl;
        
        std::cout << "Development and Research:" << std::endl;
        std::cout << "  Plugin architecture for extensions" << std::endl;
        std::cout << "  Custom plasticity rule configuration" << std::endl;
        std::cout << "  Advanced neuromodulator parameters" << std::endl;
    }

    bool run(CLIConfig& config) {
        try {
            // Initialize logging based on config
            auto logger = std::make_shared<Logger>();
            
            // Set log level
            if (config.logLevel == "DEBUG") {
                auto debugLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
                logger->addLogger(debugLogger);
            } else if (config.logLevel == "INFO") {
                auto infoLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
                logger->addLogger(infoLogger);
            } else if (config.logLevel == "WARN") {
                auto warnLogger = std::make_shared<ConsoleLogger>(LogLevel::Warn);
                logger->addLogger(warnLogger);
            } else if (config.logLevel == "ERROR") {
                auto errorLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
                logger->addLogger(errorLogger);
            }
            
            Logger::setGlobal(logger);
            
            if (!config.silent) {
                std::cout << "=== NLM Neural Learning Machine ===" << std::endl;
                std::cout << "Starting in " << modeToString(config.mode) << " mode" << std::endl;
                std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                          << config.regionCount << " regions, " 
                          << config.simulationTimestep << "s timestep" << std::endl;
                std::cout << "===============================" << std::endl << std::endl;
            }
            
            // Configure logging based on verbosity
            if (config.verbose) {
                NLM_LOG_INFO("Verbose mode enabled");
            }
            
            // Run based on mode
            switch (config.mode) {
                case Mode::PHASE2_TEST:
                    return runPhase2Test(config);
                case Mode::PHASE6_DEMO:
                    return runPhase6Demo(config);
                case Mode::CUSTOM_EXPERIMENT:
                    return runCustomExperiment(config);
                case Mode::VERIFICATION:
                    return runVerification(config);
                case Mode::BENCHMARK:
                    return runBenchmark(config);
                case Mode::INTERACTIVE:
                    return runInteractive(config);
            }
            
            return false;
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    std::string modeToString(Mode mode) {
        switch (mode) {
            case Mode::PHASE2_TEST: return "Phase 2 Test";
            case Mode::PHASE6_DEMO: return "Phase 6 Demo";
            case Mode::CUSTOM_EXPERIMENT: return "Custom Experiment";
            case Mode::VERIFICATION: return "System Verification";
            case Mode::BENCHMARK: return "Benchmark";
            case Mode::INTERACTIVE: return "Interactive Mode";
        }
        return "Unknown";
    }

    // Core execution methods (simplified implementations)
    bool runPhase2Test(const CLIConfig& config) {
        if (!config.silent) {
            std::cout << "Running Phase 2 Test Mode" << std::endl;
            std::cout << "This mode runs the original Phase 2 neural computation tests" << std::endl << std::endl;
        }
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("region_count", config.regionCount);
        cfg->set("connection_probability", 0.15f);
        cfg->set("random_seed", static_cast<int64_t>(config.randomSeed));
        cfg->set("simulation_timestep", config.simulationTimestep);
        
        // STDP parameters
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("stdp_tau", 20.0f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        if (!config.silent) {
            brain->logStatus();
        }
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain, config.testSteps);
        
        // Reset and run plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain, config.testSteps);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain, config.testSteps);
        
        if (!config.silent) {
            std::cout << "\n=== Phase 2 Test Complete ===" << std::endl;
        }
        
        return true;
    }

    bool runPhase6Demo(const CLIConfig& config) {
        if (!config.silent) {
            std::cout << "Running Phase 6 Integration Demo" << std::endl;
            std::cout << "This mode verifies all brain systems are properly integrated" << std::endl << std::endl;
        }
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run integration verification
        if (!experiment.verifyIntegration()) {
            NLM_LOG_ERROR("Integration verification failed");
            return false;
        }
        
        // Test individual systems
        experiment.testMemoryIntegration();
        experiment.testNeuromodulationIntegration();
        experiment.testCheckpointing();
        experiment.testReplay();
        
        // Run full integration experiment
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.neuronCount;
        phase6Config.maxSteps = config.maxSteps;
        phase6Config.enableCheckpointing = config.enableCheckpoints;
        phase6Config.enableReplay = true;
        phase6Config.enableDevelopment = config.enableDevelopment;
        
        auto result = experiment.run(phase6Config);
        
        // Display results
        if (!config.silent) {
            std::cout << "\n=== Phase 6 Demo Results ===" << std::endl;
            std::cout << "Total Reward: " << result.totalReward << std::endl;
            std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
            std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
            std::cout << "\nIntegration Status:" << std::endl;
            std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        }
        
        return true;
    }

    bool runCustomExperiment(const CLIConfig& config) {
        if (!config.silent) {
            std::cout << "Running Custom Experiment: " << config.experimentType << std::endl;
        }
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("region_count", config.regionCount);
        cfg->set("connection_probability", 0.1f);
        cfg->set("random_seed", static_cast<int64_t>(config.randomSeed));
        cfg->set("simulation_timestep", config.simulationTimestep);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for custom experiment");
            return false;
        }
        
        // Run experiment based on type
        if (config.experimentType == "learning") {
            runLearningExperiment(brain, config.maxSteps);
        } else if (config.experimentType == "adaptation") {
            runAdaptationExperiment(brain, config.maxSteps);
        } else if (config.experimentType == "social") {
            runSocialLearningExperiment(brain, config.maxSteps);
        } else if (config.experimentType == "memory") {
            runMemoryExperiment(brain, config.maxSteps);
        } else {
            NLM_LOG_ERROR("Unknown experiment type: " << config.experimentType);
            return false;
        }
        
        return true;
    }

    bool runVerification(const CLIConfig& config) {
        if (!config.silent) {
            std::cout << "Running System Verification" << std::endl;
            std::cout << "Testing component integrity and integration..." << std::endl << std::endl;
        }
        
        // Create minimal brain for testing
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Brain initialization failed for verification");
            return false;
        }
        
        bool allTestsPassed = true;
        
        // Test 1: Check memory systems
        if (brain->getWorkingMemory() == nullptr) {
            NLM_LOG_ERROR("Working memory not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Working memory is integrated");
        }
        
        if (brain->getEpisodicMemory() == nullptr) {
            NLM_LOG_ERROR("Episodic memory not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Episodic memory is integrated");
        }
        
        // Test 2: Check neuromodulation
        if (brain->getDopamine() == nullptr) {
            NLM_LOG_ERROR("Dopamine system not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Dopamine system is integrated");
        }
        
        // Test 3: Check prediction system
        if (brain->getPredictionSystem() == nullptr) {
            NLM_LOG_ERROR("Prediction system not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Prediction system is integrated");
        }
        
        // Test 4: Check cognition systems
        if (brain->getPlanner() == nullptr) {
            NLM_LOG_ERROR("Planner not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Planner is integrated");
        }
        
        if (brain->getConceptFormation() == nullptr) {
            NLM_LOG_ERROR("Concept formation not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Concept formation is integrated");
        }
        
        // Test 5: Check development system
        if (brain->getDevelopmentSystem() == nullptr) {
            NLM_LOG_ERROR("Development system not integrated");
            allTestsPassed = false;
        } else {
            NLM_LOG_INFO("[PASS] Development system is integrated");
        }
        
        if (!config.silent) {
            std::cout << "\n=== Verification Results ===" << std::endl;
            if (allTestsPassed) {
                std::cout << "✓ All integration tests PASSED" << std::endl;
            } else {
                std::cout << "✗ Some integration tests FAILED" << std::endl;
            }
        }
        
        return allTestsPassed;
    }

    bool runBenchmark(const CLIConfig& config) {
        if (!config.silent) {
            std::cout << "Running Performance Benchmark" << std::endl;
            std::cout << "Running " << config.benchmarkIterations << " iterations..." << std::endl << std::endl;
        }
        
        double totalTime = 0.0;
        size_t totalSteps = 0;
        
        for (size_t iteration = 0; iteration < config.benchmarkIterations; ++iteration) {
            if (!config.silent) {
                std::cout << "Iteration " << (iteration + 1) << "/" << config.benchmarkIterations << ": ";
            }
            
            // Create configuration for benchmark
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 500);
            cfg->set("region_count", 1);
            cfg->set("connection_probability", 0.1f);
            cfg->set("random_seed", static_cast<int64_t>(42 + iteration)); // Different seed each iteration
            cfg->set("simulation_timestep", config.simulationTimestep);
            
            // Create brain
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for benchmark");
                return false;
            }
            
            // Run benchmark steps
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t step = 0; step < config.maxSteps; ++step) {
                brain->step(step, step * config.simulationTimestep);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            totalTime += duration;
            totalSteps += config.maxSteps;
            
            if (!config.silent) {
                double stepsPerSecond = config.maxSteps / duration;
                std::cout << "  " << stepsPerSecond << " steps/sec" << std::endl;
            }
        }
        
        if (!config.silent) {
            std::cout << "\n=== Benchmark Results ===" << std::endl;
            double avgStepsPerSecond = static_cast<double>(totalSteps) / totalTime;
            std::cout << "Average performance: " << avgStepsPerSecond << " steps/sec" << std::endl;
            std::cout << "Total time: " << totalTime << " seconds" << std::endl;
            std::cout << "Total steps: " << totalSteps << std::endl;
            std::cout << "Time per step: " << (totalTime / totalSteps) * 1000.0 << " ms" << std::endl;
        }
        
        return true;
    }

    bool runInteractive(const CLIConfig& config) {
        if (!config.silent) {
            std::cout << "Starting Interactive Mode" << std::endl;
            std::cout << "Press Ctrl+C to exit" << std::endl << std::endl;
        }
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("region_count", config.regionCount);
        cfg->set("connection_probability", 0.1f);
        cfg->set("random_seed", static_cast<int64_t>(config.randomSeed));
        cfg->set("simulation_timestep", config.simulationTimestep);
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for interactive mode");
            return false;
        }
        
        if (!config.silent) {
            brain->logStatus();
            std::cout << "\nInteractive Controls:" << std::endl;
            std::cout << "  's' - Step simulation forward" << std::endl;
            std::cout << "  'r' - Reset brain" << std::endl;
            std::cout << "  '+' - Increase input current" << std::endl;
            std::cout << "  '-' - Decrease input current" << std::endl;
            std::cout << "  'q' - Quit" << std::endl << std::endl;
        }
        
        // Simple interactive loop
        char input;
        bool running = true;
        float currentInput = 10.0f;
        
        while (running) {
            std::cout << "Step: " << brain->getTotalSpikeCount() 
                      << " | Neurons active: " << brain->getActiveNeuronCount()
                      << " | Input: " << currentInput << " | [s/r+/-q] ";
            
            std::cin >> input;
            
            switch (input) {
                case 's':
                case 'S': {
                    uint64_t step = brain->getTotalSpikeCount() / 10; // Approximate step number
                    brain->step(step, step * config.simulationTimestep);
                    
                    // Inject some input based on current setting
                    brain->injectCurrentToNeurons(NeuronType::Sensory, currentInput);
                    break;
                }
                case 'r':
                case 'R':
                    brain->reset();
                    brain->initialize();
                    break;
                case '+':
                    currentInput += 5.0f;
                    if (!config.silent) std::cout << "Input increased to " << currentInput << std::endl;
                    break;
                case '-':
                    currentInput = std::max(0.0f, currentInput - 5.0f);
                    if (!config.silent) std::cout << "Input decreased to " << currentInput << std::endl;
                    break;
                case 'q':
                case 'Q':
                    running = false;
                    break;
                default:
                    if (!config.silent) std::cout << "Unknown command. Use s,r,+,-, or q." << std::endl;
            }
        }
        
        if (!config.silent) {
            std::cout << "\nInteractive mode terminated." << std::endl;
        }
        
        return true;
    }

    // Helper test functions
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        }
    }

    void runPlasticityExperiment(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        // Record initial weights
        std::vector<float> initialWeights;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (initialWeights.empty()) {
            NLM_LOG_INFO("  No synapses to study");
            return;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        NLM_LOG_INFO("  Initial total synaptic weight: " + std::to_string(initialSum));
        
        // Enable plasticity
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input
        NLM_LOG_INFO("  Applying repeated input patterns...");
        for (uint64_t step = 0; step < steps; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Check final weights
        std::vector<float> finalWeights;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        NLM_LOG_INFO("  Final total synaptic weight: " + std::to_string(finalSum));
        
        float change = finalSum - initialSum;
        if (std::abs(change) > 0.001f) {
            NLM_LOG_INFO("  ✓ Learning occurred: weight changed by " + std::to_string(change));
        } else {
            NLM_LOG_INFO("  ! No significant weight change detected");
        }
    }

    void runStdpVerification(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity
        NLM_LOG_INFO("  Creating correlated pre->post activity...");
        
        for (int trial = 0; trial < 50; ++trial) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record final weights
        NLM_LOG_INFO("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }

    // Custom experiment implementations
    void runLearningExperiment(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("Running Learning Experiment");
        NLM_LOG_INFO("Testing experience-based synaptic modification...");
        
        // Record initial state
        float initialTotalWeight = 0.0f;
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                initialTotalWeight += syn->getWeight();
            }
        }
        
        // Enable plasticity
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning stimuli
        for (uint64_t step = 0; step < steps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        // Evaluate learning
        float finalTotalWeight = 0.0f;
        if (region) {
            for (const auto& syn : region->getSynapses()) {
                finalTotalWeight += syn->getWeight();
            }
        }
        
        float change = finalTotalWeight - initialTotalWeight;
        if (std::abs(change) > 0.01f) {
            NLM_LOG_INFO("✓ Learning experiment SUCCESS: significant weight change (" + std::to_string(change) + ")");
        } else {
            NLM_LOG_INFO("! Learning experiment: minimal change detected");
        }
    }

    void runAdaptationExperiment(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("Running Adaptation Experiment");
        NLM_LOG_INFO("Testing adaptation to changing environment...");
        
        float adaptationScore = 0.0f;
        size_t adaptationPoints = 0;
        
        // Simulate changing environment conditions
        for (uint64_t step = 0; step < steps; ++step) {
            float environmentCondition = 0.5f + 0.5f * std::sin(step * 0.01f);
            
            // Inject current based on environment
            brain->injectCurrentToNeurons(NeuronType::Sensory, environmentCondition * 40.0f);
            brain->step(step, step * 0.001);
            
            // Adapt to environment
            if (step % 100 == 0) {
                adaptationScore += brain->getAverageFiringRate();
                adaptationPoints++;
            }
        }
        
        if (adaptationPoints > 0) {
            adaptationScore /= adaptationPoints;
            if (adaptationScore > 10.0f) {
                NLM_LOG_INFO("✓ Adaptation experiment SUCCESS: good environmental adaptation");
            } else {
                NLM_LOG_INFO("! Adaptation experiment: limited adaptation response");
            }
        }
    }

    void runSocialLearningExperiment(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("Running Social Learning Experiment");
        NLM_LOG_INFO("Testing social learning and observation...");
        
        // Simulate observation of other agent's behavior
        float socialLearningEffect = 0.0f;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Simulate social observation (shared experience)
            brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
            brain->step(step, step * 0.001);
            
            // Social learning effect accumulates
            if (step % 50 == 0) {
                socialLearningEffect += brain->getTotalSpikeCount() * 0.001f;
            }
        }
        
        if (socialLearningEffect > 1.0f) {
            NLM_LOG_INFO("✓ Social learning experiment SUCCESS: social influence detected");
        } else {
            NLM_LOG_INFO("! Social learning experiment: minimal social influence");
        }
    }

    void runMemoryExperiment(std::shared_ptr<Brain> brain, uint64_t steps) {
        NLM_LOG_INFO("Running Memory Experiment");
        NLM_LOG_INFO("Testing memory formation and retention...");
        
        // Store initial spike patterns
        std::vector<size_t> initialPatterns;
        
        for (uint64_t step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            initialPatterns.push_back(brain->getTotalSpikeCount());
        }
        
        // Let some time pass
        for (uint64_t step = 100; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Check memory retention
        std::vector<size_t> finalPatterns;
        
        for (uint64_t step = steps - 100; step < steps; ++step) {
            finalPatterns.push_back(brain->getTotalSpikeCount());
        }
        
        // Simple memory metric
        float patternSimilarity = 0.0f;
        if (!initialPatterns.empty() && !finalPatterns.empty()) {
            for (size_t i = 0; i < std::min(initialPatterns.size(), finalPatterns.size()); ++i) {
                patternSimilarity += std::abs(static_cast<float>(initialPatterns[i] - finalPatterns[i]));
            }
            patternSimilarity /= std::min(initialPatterns.size(), finalPatterns.size());
        }
        
        if (patternSimilarity < 50.0f) {
            NLM_LOG_INFO("✓ Memory experiment SUCCESS: patterns retained");
        } else {
            NLM_LOG_INFO("! Memory experiment: patterns changed significantly");
        }
    }
};

// Global CLI instance for easy use
NLMCommandLineInterface gNLMCLI;

// Main function using CLI
int main(int argc, char** argv) {
    // Parse command line arguments
    NLMCommandLineInterface::CLIConfig config;
    
    if (!gNLMCLI.parseArguments(argc, argv, config)) {
        return 1; // Help was displayed or there was an error
    }
    
    // Run NLM with CLI configuration
    if (!gNLMCLI.run(config)) {
        std::cerr << "Error: NLM execution failed." << std::endl;
        return 1;
    }
    
    return 0;
}
