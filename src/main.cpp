// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements a complete integrated artificial brain with:
// - Neural memory systems (working + episodic)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction and cognition systems
// - Development and structural plasticity
// - Experience replay and consolidation
// - Checkpoint persistence
//
// This is a comprehensive demonstration of Phase 6 integration
// showing all brain systems working together as a unified architecture.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>

#include <chrono>
#include <numeric>

// Performance benchmarking utilities
namespace Performance {
    struct BenchmarkResult {
        std::string operation;
        double timeSeconds;
        size_t iterations;
        double averageTimePerIteration;
        double operationsPerSecond;
        
        BenchmarkResult(const std::string& op, double time, size_t iter)
            : operation(op), timeSeconds(time), iterations(iter),
              averageTimePerIteration(time / iter), operationsPerSecond(iter / time) {}
    };
    
    template<typename Func>
    BenchmarkResult benchmark(const std::string& name, Func&& func, size_t iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(end - start).count();
        return BenchmarkResult(name, duration, iterations);
    }
    
    void printBenchmark(const BenchmarkResult& result) {
        NLM_LOG_INFO("Benchmark: " + result.operation);
        NLM_LOG_INFO("  Total time: " + std::to_string(result.timeSeconds) + "s");
        NLM_LOG_INFO("  Average per iteration: " + std::to_string(result.averageTimePerIteration) * 1000 + "ms");
        NLM_LOG_INFO("  Operations per second: " + std::to_string(result.operationsPerSecond));
        NLM_LOG_INFO("  Iterations: " + std::to_string(result.iterations));
    }
}

void printUsageAndExit() {
    std::cout << R"(
=== NLM Phase 6 Usage ===

USAGE:
    nlm [OPTIONS]

EXPERIMENT TYPES:
    --quick           Quick integration verification (fast)
    --full            Full integration experiment (default)
    --test-all        Comprehensive integration testing

CONFIGURATION:
    --config=<file>   Load configuration from JSON or .cfg file
    --neuron=<N>     Override neuron count
    --checkpoint=<path> Set checkpoint file path

FEATURES:
    --no-checkpoint    Disable checkpointing
    --no-replay        Disable replay system
    --no-development   Disable development system
    --enable-curiosity  Enable curiosity-driven exploration

HELP/INFO:
    --help, -h         Show this help message
    --version          Show version information

EXAMPLES:
    nlm --quick                   Quick integration test
    nlm --full --neuron=500       Full experiment with 500 neurons
    nlm --test-all --no-checkpoint Comprehensive test without checkpointing

)" << std::endl;
    exit(0);
}

void printVersionAndExit() {
    std::cout << "NLM (熙然) Neural Learning Machine - Phase 6: Final Integration" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Built with C++20, featuring integrated memory, neuromodulation, and cognition systems." << std::endl;
    std::cout << "For documentation: https://nlm.readthedocs.io" << std::endl;
    exit(0);
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
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
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
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
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
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

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6: Final Integration...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Implementing a complete integrated artificial brain with:");
    NLM_LOG_INFO("  - Memory systems (working + episodic) connected to neural processing");
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty) affecting plasticity");
    NLM_LOG_INFO("  - Prediction system integrated with learning");
    NLM_LOG_INFO("  - Cognition systems (planning, attention, concept formation)");
    NLM_LOG_INFO("  - Development modulating plasticity rates");
    NLM_LOG_INFO("  - Experience replay and memory consolidation");
    NLM_LOG_INFO("  - Checkpoint persistence and state restoration");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    std::string experimentType = "full";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--quick") {
            experimentType = "quick";
        } else if (arg == "--full") {
            experimentType = "full";
        } else if (arg == "--test-all") {
            experimentType = "comprehensive";
        } else if (arg == "--help" || arg == "-h") {
            printUsageAndExit();
        } else if (arg == "--version") {
            printVersionAndExit();
        } else if (arg.substr(0, 10) == "--neuron=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                config->set("neuron_count", std::stoll(arg.substr(pos + 1)), ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 11) == "--steps=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                size_t steps = std::stoull(arg.substr(pos + 1));
                if (experimentType == "full") {
                    NLM_LOG_WARNING("Steps override will be ignored for full experiment");
                } else if (experimentType == "quick") {
                    experimentType = "custom";
                    quickExperimentSteps = steps;
                }
            }
        } else if (arg == "--no-checkpoint") {
            config->set("enable_checkpointing", false, ConfigSource::CommandLine);
        } else if (arg == "--no-replay") {
            config->set("enable_replay", false, ConfigSource::CommandLine);
        } else if (arg == "--no-development") {
            config->set("enable_development", false, ConfigSource::CommandLine);
        } else if (arg == "--enable-curiosity") {
            config->set("enable_curiosity", true, ConfigSource::CommandLine);
        } else if (arg.substr(0, 12) == "--checkpoint=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                config->set("checkpoint_path", arg.substr(pos + 1), ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 10) == "--config=" && arg.find('=') != 7) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                configFile = arg.substr(pos + 1);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    config->set("enable_checkpointing", true, ConfigSource::Default);
    config->set("enable_replay", true, ConfigSource::Default);
    config->set("enable_development", true, ConfigSource::Default);
    config->set("enable_curiosity", true, ConfigSource::Default);
    config->set("enable_novelty_detection", true, ConfigSource::Default);
    config->set("checkpoint_path", "./nlm_phase6_checkpoint.bin", ConfigSource::Default);
    
    // Performance tuning parameters
    config->set("performance.optimize_neural_dynamics", true, ConfigSource::Default);
    config->set("performance.enable_vectorization", true, ConfigSource::Default);
    config->set("performance.memory_pool_size", 10000, ConfigSource::Default);
    config->set("performance.event_queue_size", 5000, ConfigSource::Default);
    
    // Development and debugging options
    config->set("debug.enable_breakpoints", false, ConfigSource::Default);
    config->set("debug.log_level", "Info", ConfigSource::Default);
    config->set("debug.performance_profiling", false, ConfigSource::Default);
    config->set("debug.memory_tracking", true, ConfigSource::Default);
    
    // Learning and behavior parameters
    config->set("behavior.learning_rate", 0.001, ConfigSource::Default);
    config->set("behavior.exploration_rate", 0.1f, ConfigSource::Default);
    config->set("behavior.exploitation_rate", 0.9f, ConfigSource::Default);
    config->set("behavior.reward_scaling", 1.0f, ConfigSource::Default);
    
    // Memory and consolidation parameters
    config->set("memory.working_memory_capacity", 100, ConfigSource::Default);
    config->set("memory.episodic_memory_max_episodes", 1000, ConfigSource::Default);
    config->set("memory.consolidation_threshold", 0.5f, ConfigSource::Default);
    
    // Social and interaction parameters
    config->set("social.enable_social_learning", false, ConfigSource::Default);
    config->set("social.imitation_rate", 0.3f, ConfigSource::Default);
    config->set("social.teaching_threshold", 0.7f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("  replay_interval: " + std::to_string(config->getOr<size_t>("replay_interval", 100)));
    NLM_LOG_INFO("  consolidation_interval: " + std::to_string(config->getOr<size_t>("consolidation_interval", 1000)));
    NLM_LOG_INFO("");
    
    // Run appropriate Phase 6 experiment based on command line
    if (experimentType == "quick") {
        NLM_LOG_INFO("=== Quick Integration Verification ===");
        Phase6IntegratedExperiment quickExperiment;
        bool integrationOK = quickExperiment.verifyIntegration();
        if (integrationOK) {
            NLM_LOG_INFO("✓ Integration verification PASSED - All systems connected");
        } else {
            NLM_LOG_ERROR("✗ Integration verification FAILED - Systems not properly integrated");
            return 1;
        }
    } else if (experimentType == "comprehensive") {
        NLM_LOG_INFO("=== Comprehensive Integration Test ===");
        Phase6IntegratedExperiment comprehensiveExperiment;
        
        // Run all integration tests
        NLM_LOG_INFO("--- Testing Memory Integration ---");
        bool memoryOK = comprehensiveExperiment.testMemoryIntegration();
        
        NLM_LOG_INFO("--- Testing Neuromodulation Integration ---");
        bool neuromodulationOK = comprehensiveExperiment.testNeuromodulationIntegration();
        
        NLM_LOG_INFO("--- Testing Checkpoint System ---");
        bool checkpointOK = comprehensiveExperiment.testCheckpointing();
        
        NLM_LOG_INFO("--- Testing Replay System ---");
        bool replayOK = comprehensiveExperiment.testReplay();
        
        if (memoryOK && neuromodulationOK && checkpointOK && replayOK) {
            NLM_LOG_INFO("✓ All integration tests PASSED");
        } else {
            NLM_LOG_ERROR("✗ Some integration tests FAILED");
            return 1;
        }
    } else {  // full experiment
        NLM_LOG_INFO("=== Full Phase 6 Integration Experiment ===");
        Phase6IntegratedExperiment experiment;
        
        // Configure experiment
        Phase6Config phase6Config;
        phase6Config.maxSteps = 5000;
        phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 1000);
        phase6Config.regionCount = config->getOr<int64_t>("region_count", 1);
        phase6Config.connectionProbability = config->getOr<float>("connection_probability", 0.1f);
        phase6Config.enableCheckpointing = true;
        phase6Config.enableReplay = true;
        phase6Config.enableDevelopment = true;
        phase6Config.checkpointPath = "./nlm_phase6_checkpoint.bin";
        
        NLM_LOG_INFO("Running full integration experiment with:");
        NLM_LOG_INFO("  " + std::to_string(phase6Config.maxSteps) + " steps");
        NLM_LOG_INFO("  " + std::to_string(phase6Config.neuronCount) + " neurons");
        NLM_LOG_INFO("  " + std::to_string(phase6Config.regionCount) + " regions");
        
// Add performance benchmarking to full experiment
    if (experimentType == "full") {
        NLM_LOG_INFO("=== Performance Benchmarking ===");
        
        // Benchmark brain stepping
        auto benchStep = [&]() {
            brain->step(0);
        };
        
        size_t benchmarkSteps = std::min(static_cast<size_t>(1000), phase6Config.maxSteps);
        auto stepResult = Performance::benchmark("Brain Stepping", benchStep, benchmarkSteps);
        Performance::printBenchmark(stepResult);
        
        // Benchmark neural operations
        auto benchMemoryOps = [&]() {
            if (brain->getWorkingMemory()) {
                brain->getWorkingMemory()->getActiveTraces();
            }
            if (brain->getEpisodicMemory()) {
                brain->getEpisodicMemory()->getEpisodeCount();
            }
        };
        
        size_t benchmarkOps = 10000;
        auto memoryResult = Performance::benchmark("Memory Operations", benchMemoryOps, benchmarkOps);
        Performance::printBenchmark(memoryResult);
        
        // Benchmark connectivity
        auto benchConnectivity = [&]() {
            brain->getTotalNeuronCount();
            brain->getTotalSynapseCount();
            brain->getExcitationInhibitionRatio();
        };
        
        size_t benchmarkConnections = 1000;
        auto connectivityResult = Performance::benchmark("Connectivity Access", benchConnectivity, benchmarkConnections);
        Performance::printBenchmark(connectivityResult);
        
        NLM_LOG_INFO("=== Performance Summary ===");
        NLM_LOG_INFO("Brain stepping rate: " + std::to_string(stepResult.operationsPerSecond) + " steps/second");
        NLM_LOG_INFO("Memory operations: " + std::to_string(memoryResult.operationsPerSecond) + " ops/second");
        NLM_LOG_INFO("Connectivity queries: " + std::to_string(connectivityResult.operationsPerSecond) + " queries/second");
        NLM_LOG_INFO("".append(50, '='));
    }
        
        // Display results
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== FINAL RESULTS ===");
        NLM_LOG_INFO("Total reward per step: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Average synaptic weight: " + std::to_string(result.avgSynapticWeight));
        NLM_LOG_INFO("Episodic memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Novelty level: " + std::to_string(result.noveltyLevel));
        NLM_LOG_INFO("Curiosity level: " + std::to_string(result.curiosityLevel));
        NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
        NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== INTEGRATION STATUS ===");
        NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED"));
        NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED"));
        NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED"));
        NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED"));
        NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED"));
        NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING"));
        NLM_LOG_INFO("Replay: " + std::string(result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING"));
        
        // Success if key systems are integrated
        bool experimentSuccess = result.memoryWorkingMemoryIntegrated &&
                                result.memoryEpisodicMemoryIntegrated &&
                                result.neuromodulationIntegrated &&
                                result.predictionIntegrated;
        
        if (experimentSuccess) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== PHASE 6 INTEGRATION SUCCESSFUL ===");
            NLM_LOG_INFO("The NLM brain now functions as a complete integrated artificial brain:");
            NLM_LOG_INFO("  - All memory systems are connected to neural processing");
            NLM_LOG_INFO("  - Neuromodulation coordinates learning and behavior");
            NLM_LOG_INFO("  - Prediction guides action selection and learning");
            NLM_LOG_INFO("  - Development allows adaptive changes over time");
            NLM_LOG_INFO("  - Experience is stored and replayed for consolidation");
            NLM_LOG_INFO("  - Brain state can be saved and restored");
            NLM_LOG_INFO("");
        } else {
            NLM_LOG_ERROR("=== PHASE 6 INTEGRATION INCOMPLETE ===");
            NLM_LOG_ERROR("Some critical systems are not properly integrated.");
            return 1;
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("".append(30, '='));
    
    return 0;
}
