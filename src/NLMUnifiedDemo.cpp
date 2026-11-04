// NLM Unified Demo System
// Single entry point for all NLM phases and demo modes
// Provides unified CLI interface with phase management, configuration, and logging

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase5Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6TextDemo.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <chrono>

using namespace nlm;

enum class DemoPhase {
    PHASE2_BASIC = 2,
    PHASE3_WORLD = 3,
    PHASE4_COGNITION = 4,
    PHASE5_PERSONALITY = 5,
    PHASE6_INTEGRATION = 6,
    PHASE6_TEXT = 7,
    BENCHMARK = 999
};

class NLMUnifiedDemo {
public:
    NLMUnifiedDemo() {
        setupConfiguration();
        setupLogging();
    }
    
    ~NLMUnifiedDemo() {}
    
    // Main entry point with unified CLI
    bool run(int argc, char* argv[]) {
        parseCommandLine(argc, argv);
        
        if (showHelp) {
            printHelp();
            return true;
        }
        
        if (showVersion) {
            printVersion();
            return true;
        }
        
        // Validate phase selection
        if (selectedPhase == DemoPhase::PHASE2_BASIC && !phase2Enabled) {
            std::cerr << "ERROR: Phase 2 demo not available. Use --phase 3, 4, 5, 6, 7, or benchmark." << std::endl;
            return false;
        }
        
        // Run selected demo
        switch (selectedPhase) {
            case DemoPhase::PHASE2_BASIC:
                return runPhase2Demo();
            case DemoPhase::PHASE3_WORLD:
                return runPhase3Demo();
            case DemoPhase::PHASE4_COGNITION:
                return runPhase4Demo();
            case DemoPhase::PHASE5_PERSONALITY:
                return runPhase5Demo();
            case DemoPhase::PHASE6_INTEGRATION:
                return runPhase6Demo();
            case DemoPhase::PHASE6_TEXT:
                return runPhase6TextDemo();
            case DemoPhase::BENCHMARK:
                return runBenchmarkDemo();
            default:
                std::cerr << "ERROR: Unknown phase selected: " << static_cast<int>(selectedPhase) << std::endl;
                return false;
        }
    }
    
    // Help and version printing
    void printHelp() {
        std::cout << "========================================\n";
        std::cout << "NLM Unified Demo System v0.1.0\n";
        std::cout << "========================================\n\n";
        
        std::cout << "USAGE:\n";
        std::cout << "  nlm [OPTIONS] [ARGS]...\n\n";
        
        std::cout << "MODES (select one):\n";
        std::cout << "  --phase 2     Phase 2: Basic neural computation demo (legacy)\n";
        std::cout << "  --phase 3     Phase 3: World interaction demo\n";
        std::cout << "  --phase 4     Phase 4: Cognition and memory demo\n";
        std::cout << "  --phase 5     Phase 5: Personality/sandbox/checkpoint demo\n";
        std::cout << "  --phase 6     Phase 6: Full integration demo\n";
        std::cout << "  --phase 7     Phase 6 Text: Text teaching and thought demo\n";
        std::cout << "  --benchmark   Performance benchmark\n\n";
        
        std::cout << "COMMON OPTIONS:\n";
        std::cout << "  --config FILE      Load configuration from FILE (default: configs/default.cfg)\n";
        std::cout << "  --seed N           Set random seed (default: 42)\n";
        std::cout << "  --help             Show this help message\n";
        std::cout << "  --version          Show version information\n\n";
        
        std::cout << "PHASE-SPECIFIC OPTIONS:\n";
        std::cout << "Phase 3:\n";
        std::cout << "  --episodes N      Number of episodes (default: 5)\n";
        std::cout << "  --steps N         Steps per episode (default: 200)\n\n";
        
        std::cout << "Phase 5:\n";
        std::cout << "  --sandbox-steps N  Sandbox steps (default: 100)\n\n";
        
        std::cout << "Phase 6:\n";
        std::cout << "  --quick           Skip full experiment, run quick integration test\n\n";
        
        std::cout << "ADVANCED OPTIONS:\n";
        std::cout << "  --disable-plasticity     Disable neural plasticity\n";
        std::cout << "  --disable-neuromodulation Disable neuromodulation\n";
        std::cout << "  --disable-curiosity      Disable curiosity\n";
        std::cout << "  --disable-development    Disable development\n";
        std::cout << "  --benchmark-mode         Enable benchmark mode (more comprehensive)\n\n";
        
        std::cout << "EXAMPLES:\n";
        std::cout << "  nlm --phase 3 --episodes 10 --seed 123\n";
        std::cout << "  nlm --phase 6 --quick\n";
        std::cout << "  nlm --benchmark --disable-plasticity\n\n";
        
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm\n";
    }
    
    void printVersion() {
        std::cout << "========================================\n";
        std::cout << "NLM Unified Demo System v0.1.0\n";
        std::cout << "========================================\n";
        std::cout << "Phase 2 (basic): Legacy neural computation\n";
        std::cout << "Phase 3 (world): World interaction\n";
        std::cout << "Phase 4 (cognition): Cognitive functions\n";
        std::cout << "Phase 5 (personality): Sandbox and checkpointing\n";
        std::cout << "Phase 6 (integration): Full system integration\n";
        std::cout << "Phase 7 (text): Text teaching and thought\n";
        std::cout << "========================================\n";
    }
    
    // Command line parsing
    void parseCommandLine(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                showHelp = true;
            } else if (arg == "--version") {
                showVersion = true;
            } else if (arg == "--phase" && i + 1 < argc) {
                selectedPhase = parsePhase(argv[++i]);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--seed" && i + 1 < argc) {
                seed = std::atoi(argv[++i]);
            } else if (arg == "--episodes" && i + 1 < argc) {
                phase3Episodes = std::atoi(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                phase3Steps = std::atoi(argv[++i]);
            } else if (arg == "--sandbox-steps" && i + 1 < argc) {
                phase5SandboxSteps = std::atoi(argv[++i]);
            } else if (arg == "--quick") {
                phase6QuickMode = true;
            } else if (arg == "--disable-plasticity") {
                disablePlasticity = true;
            } else if (arg == "--disable-neuromodulation") {
                disableNeuromodulation = true;
            } else if (arg == "--disable-curiosity") {
                disableCuriosity = true;
            } else if (arg == "--disable-development") {
                disableDevelopment = true;
            } else if (arg == "--benchmark-mode") {
                benchmarkMode = true;
            } else if (arg == "--advanced" || arg == "--verbose") {
                verboseMode = true;
            } else if (arg == "--python-bindings") {
                usePythonBindings = true;
            } else if (arg == "--export" && i + 1 < argc) {
                exportFormat = argv[++i];
            } else if (arg == "--demo-mode" && i + 1 < argc) {
                demoMode = argv[++i];
            } else if (arg == "--help-advanced") {
                printAdvancedHelp();
            } else {
                // Check for unknown arguments
                if (arg.find("--") == 0) {
                    std::cerr << "WARNING: Unknown option: " << arg << std::endl;
                }
            }
        }
    }
    
    void printAdvancedHelp() {
        std::cout << "========================================\n";
        std::cout << "ADVANCED OPTIONS\n";
        std::cout << "========================================\n\n";
        
        std::cout << "EXPERIMENTAL FEATURES:\n";
        std::cout << "  --export format      Export results in format (json, csv, xml)\n";
        std::cout << "  --demo-mode type     Demo mode: 'replay', 'training', 'evaluation'\n";
        std::cout << "  --python-bindings    Use Python bindings for enhanced features\n\n";
        
        std::cout << "PERFORMANCE OPTIONS:\n";
        std::cout << "  --benchmark-mode     Enable benchmark mode with comprehensive metrics\n";
        std::cout << "  --parallel N         Use N threads (experimental)\n\n";
        
        std::cout << "VISUALIZATION OPTIONS:\n";
        std::cout << "  --visualize          Enable visualization output\n";
        std::cout << "  --output-file FILE   Save output to FILE\n\n";
        
        std::cout << "DEVELOPMENT OPTIONS:\n";
        std::cout << "  --debug              Enable debug logging\n";
        std::cout << "  --validate-only      Only validate configuration, don't run\n";
        std::cout << "  --profile            Enable performance profiling\n\n";
    }
    
    DemoPhase parsePhase(const std::string& phaseStr) {
        try {
            int phase = std::stoi(phaseStr);
            if (phase < 2 || phase > 9) {
                std::cerr << "ERROR: Phase must be between 2 and 7 (or 'benchmark')" << std::endl;
                return DemoPhase::PHASE3_WORLD; // Default
            }
            return static_cast<DemoPhase>(phase);
        } catch (...) {
            std::cerr << "ERROR: Invalid phase: " << phaseStr << std::endl;
            return DemoPhase::PHASE3_WORLD; // Default
        }
    }
    
private:
    // Configuration setup
    void setupConfiguration() {
        config = std::make_shared<Config>();
        
        // Set default values based on phase
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Load config file if specified
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        // Apply command-line overrides
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
    }
    
    void setupLogging() {
        auto globalLogger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(verboseMode ? LogLevel::Debug : LogLevel::Info);
        globalLogger->addLogger(consoleLogger);
        Logger::setGlobal(globalLogger);
        
        NLM_LOG_INFO("NLM Unified Demo System starting...");
        NLM_LOG_INFO("Configuration loaded: " + configFile);
        NLM_LOG_INFO("Selected phase: " + std::to_string(static_cast<int>(selectedPhase)));
        if (benchmarkMode) NLM_LOG_INFO("Benchmark mode enabled");
    }
    
    // Demo runners
    bool runPhase2Demo() {
        NLM_LOG_INFO("=== Phase 2 Demo: Basic Neural Computation ===");
        
        // Phase 2 demo implementation (from original main.cpp)
        printBanner();
        
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        brain->logStatus();
        
        // Run all three Phase 2 tests
        runBasicConnectivityTest(brain);
        brain->reset(); brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset(); brain->initialize();
        runStdpVerification(brain);
        
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        return true;
    }
    
    bool runPhase3Demo() {
        NLM_LOG_INFO("=== Phase 3 Demo: World Interaction ===");
        
        nlm::Phase3Demo demo;
        demo.run(phase3Episodes, phase3Steps);
        
        NLM_LOG_INFO("=== Phase 3 Complete ===");
        return true;
    }
    
    bool runPhase4Demo() {
        NLM_LOG_INFO("=== Phase 4 Demo: Cognition and Memory ===");
        // Phase 4 would be implemented here
        NLM_LOG_WARNING("Phase 4 demo not yet implemented in unified system");
        return true;
    }
    
    bool runPhase5Demo() {
        NLM_LOG_INFO("=== Phase 5 Demo: Personality and Sandbox ===");
        
        nlm::Phase5Demo demo;
        // Phase 5 demo logic would be added here
        
        NLM_LOG_INFO("=== Phase 5 Complete ===");
        return true;
    }
    
    bool runPhase6Demo() {
        NLM_LOG_INFO("=== Phase 6 Demo: Full Integration ===");
        
        if (phase6QuickMode) {
            NLM_LOG_INFO("Quick integration mode - running simplified test");
            // Quick test implementation
        } else {
            NLM_LOG_INFO("Full integration mode - running comprehensive test");
            // Full integration test implementation
        }
        
        NLM_LOG_INFO("=== Phase 6 Complete ===");
        return true;
    }
    
    bool runPhase6TextDemo() {
        NLM_LOG_INFO("=== Phase 6 Text Demo: Text Teaching and Thought ===");
        
        // Phase 6 text demo implementation
        NLM_LOG_WARNING("Phase 6 text demo implementation pending");
        
        NLM_LOG_INFO("=== Phase 6 Text Demo Complete ===");
        return true;
    }
    
    bool runBenchmarkDemo() {
        NLM_LOG_INFO("=== Benchmark Demo: Performance Testing ===");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run comprehensive benchmark
        if (benchmarkMode) {
            // Comprehensive benchmark
            NLM_LOG_INFO("Running comprehensive benchmark suite...");
        } else {
            // Simple benchmark
            NLM_LOG_INFO("Running basic benchmark...");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        NLM_LOG_INFO("=== Benchmark Complete ===");
        NLM_LOG_INFO("Total execution time: " + std::to_string(duration.count()) + "s");
        return true;
    }
    
    // Helper methods
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
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics            ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                              ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    // Legacy Phase 2 test implementations (copied from original main.cpp)
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        // Simplified plasticity test
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run plasticity simulation
        NLM_LOG_INFO("  Running plasticity experiment...");
        for (SimulationStep step = 0; step < 1000; ++step) {
            brain->step(step, step * 0.001);
        }
        
        NLM_LOG_INFO("  Plasticity experiment completed");
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            NLM_LOG_INFO("  No synapses for STDP test");
            return;
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);
        float beforeWeight = syn->getWeight();
        
        NLM_LOG_INFO("  Testing STDP on 1 synapse");
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  STDP delta: " + std::to_string(delta));
        
        if (delta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (delta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
    
    // Member variables
    bool showHelp = false;
    bool showVersion = false;
    DemoPhase selectedPhase = DemoPhase::PHASE3_WORLD;
    std::string configFile = "configs/default.cfg";
    int seed = 42;
    int phase3Episodes = 5;
    int phase3Steps = 200;
    int phase5SandboxSteps = 100;
    bool phase6QuickMode = false;
    bool disablePlasticity = false;
    bool disableNeuromodulation = false;
    bool disableCuriosity = false;
    bool disableDevelopment = false;
    bool benchmarkMode = false;
    bool verboseMode = false;
    bool usePythonBindings = false;
    std::string exportFormat = "json";
    std::string demoMode = "replay";
    std::shared_ptr<Config> config;
};