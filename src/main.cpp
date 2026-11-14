// NLM Unified Command Line Interface
// Main entry point providing unified interface to all demo modes and experiments

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <filesystem>

using namespace nlm;
namespace fs = std::filesystem;

// Global configuration management
struct NLMConfig {
    std::shared_ptr<Config> config;
    std::string outputDir;
    LogLevel logLevel;
    uint64_t randomSeed;
    bool verbose;
    bool quiet;
    std::string configFile;
    std::chrono::steady_clock::time_point startTime;
};

NLMConfig gConfig;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all systems into a coherent loop.     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] DEMO [DEMO_ARGS...]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM provides several demo and experiment modes:" << std::endl;
    std::cout << "  phase2    : Phase 2 - Real Neural Computation (LIF neurons, STDP, plasticity)" << std::endl;
    std::cout << "  phase3    : Phase 3 - World Interaction (experience -> action -> learning)" << std::endl;
    std::cout << "  phase4    : Phase 4 - Cognitive Integration (memory, prediction, planning)" << std::endl;
    std::cout << "  phase5    : Phase 5 - Personality & Sandbox (sandbox learning)" << std::endl;
    std::cout << "  phase6    : Phase 6 - Final Integration (comprehensive system test)" << std::endl;
    std::cout << "  benchmark : Performance benchmarks for different network sizes" << std::endl;
    std::cout << std::endl;
    std::cout << "Global Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --version, -v               Show NLM version" << std::endl;
    std::cout << "  --verbose, -V               Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                 Suppress non-essential output" << std::endl;
    std::cout << "  --seed N, -s N              Set random seed (default: 42)" << std::endl;
    std::cout << "  --config FILE, -c FILE       Load configuration from file" << std::endl;
    std::cout << "  --output-dir DIR             Set output directory (default: ./nlm_output)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " phase2                          # Run Phase 2 demo (default settings)" << std::endl;
    std::cout << "  " << programName << " phase3 10 500                    # Phase 3 with 10 episodes, 500 steps each" << std::endl;
    std::cout << "  " << programName << " benchmark --neuron-sizes '100,500,1000'" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg phase5" << std::endl;
}

void initializeConfig(int argc, char** argv) {
    gConfig.startTime = std::chrono::steady_clock::now();
    
    // Initialize global configuration with defaults
    gConfig.logLevel = LogLevel::Info;
    gConfig.randomSeed = 42;
    gConfig.verbose = false;
    gConfig.quiet = false;
    gConfig.outputDir = "./nlm_output";
    gConfig.configFile = "configs/default.cfg";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            std::cout << "NLM Neural Learning Machine v0.1.0 Phase 6" << std::endl;
            exit(0);
        } else if (arg == "--verbose" || arg == "-V") {
            gConfig.verbose = true;
            gConfig.logLevel = LogLevel::Debug;
        } else if (arg == "--quiet" || arg == "-q") {
            gConfig.quiet = true;
            gConfig.logLevel = LogLevel::Error;
        } else if (arg == "--seed" && i + 1 < argc) {
            gConfig.randomSeed = std::stoull(argv[++i]);
        } else if ((arg == "--config" || arg == "-c") && i + 1 < argc) {
            gConfig.configFile = argv[++i];
        } else if (arg == "--output-dir" && i + 1 < argc) {
            gConfig.outputDir = argv[++i];
        }
    }
    
    // Ensure output directory exists
    if (!fs::exists(gConfig.outputDir)) {
        fs::create_directories(gConfig.outputDir);
    }
    
    // Create and initialize configuration
    gConfig.config = std::make_shared<Config>();
    
    // Try to load from config file
    if (gConfig.config->loadFromFile(gConfig.configFile)) {
        if (gConfig.verbose) {
            NLM_LOG_INFO("Loaded configuration from: " + gConfig.configFile);
        }
    } else {
        if (gConfig.verbose) {
            NLM_LOG_INFO("Using default configuration (file not found: " + gConfig.configFile + ")");
        }
    }
    
    // Override with command line arguments
    gConfig.config->loadFromArgs(argc, argv);
    
    // Set random seed
    gConfig.config->set("random_seed", static_cast<int64_t>(gConfig.randomSeed), ConfigSource::Runtime);
    
    // Setup logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(gConfig.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void runPhase2Demo() {
    std::cout << "=== NLM Phase 2: Real Neural Computation Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Test 1: Basic connectivity
    std::cout << std::endl << "--- Test 1: Basic Neural Connectivity ---" << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "ERROR: No neurons found!" << std::endl;
        return;
    }
    
    auto neurons = region->getAllNeurons();
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
    
    // Test 2: Plasticity learning experiment
    std::cout << std::endl << "--- Test 2: Plasticity Learning Experiment ---" << std::endl;
    
    // Reset for plasticity test
    brain->reset();
    brain->initialize();
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Record initial state
    size_t initialSynapseCount = brain->getTotalSynapseCount();
    NLM_LOG_INFO("Initial synapses: " + std::to_string(initialSynapseCount));
    
    // Apply repeated input pattern
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    // Select 20 sensory neurons for stimulation
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Test 3: STDP verification
    std::cout << std::endl << "--- Test 3: STDP Verification ---" << std::endl;
    
    // Reset for STDP test
    brain->reset();
    brain->initialize();
    
    auto* region = brain->getRegion(RegionId(1));
    if (region && !region->getSynapses().empty()) {
        Synapse* syn = region->getSynapses()[0].get();
        syn->enablePlasticity(false, true, false); // STDP only
        
        float beforeWeight = syn->getWeight();
        NLM_LOG_INFO("Testing STDP on 1 synapse:");
        NLM_LOG_INFO("  Synapse before: " + std::to_string(beforeWeight) +
                    " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                    ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        // Create correlated pre->post activity
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
        
        if (delta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (delta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
    
    // Final brain status
    std::cout << std::endl << "=== Final Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << std::endl << "=== Phase 2 Demo Complete ===" << std::endl;
}

void runPhase3Demo(int episodes, int stepsPerEpisode) {
    std::cout << "=== NLM Phase 3: World Interaction Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(gConfig.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(gConfig.randomSeed);
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Enable all optional features by default
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    agentBrain.enablePlanning(true);
    agentBrain.enableConceptFormation(true);
    
    std::cout << "Initial brain state:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
    std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
    std::cout << std::endl;
    
    // Run episodes
    for (int episode = 0; episode < episodes; ++episode) {
        std::cout << "--- Episode " << episode << " ---" << std::endl;
        
        world.reset();
        agentBrain.reset();
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        uint64_t brainStepCounter = 0;
        
        for (int step = 0; step < stepsPerEpisode; ++step) {
            // Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            // Simulate brain (10 neural steps per action)
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(brainStepCounter++);
            }
            
            // Decode motor command from brain activity
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // Apply motor command to world
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            world.update(0.1);
            agentBrain.updateDevelopment(0.1);
            
            episodeReward += result.reward;
            
            if (step % 50 == 0) {
                std::cout << "  Step " << step 
                          << ": reward=" << result.reward
                          << ", action=" << (int)cmd
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", novelty=" << agentBrain.getNoveltyLevel()
                          << ", energy=" << world.getAgentBody().energy
                          << std::endl;
            }
        }
        
        std::cout << "  Episode complete: reward=" << episodeReward 
                  << ", final energy=" << world.getAgentBody().energy
                  << ", synapses=" << brain->getTotalSynapseCount()
                  << std::endl;
    }
    
    std::cout << std::endl << "=== Phase 3 Demo Complete ===" << std::endl;
}

void runPhase5Demo() {
    std::cout << "=== NLM Phase 5: Personality & Sandbox Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    float totalReward = 0.0f;
    int resources = 0, hazards = 0, emptyInteracts = 0;
    std::set<std::pair<int, int>> visited;
    
    // Pattern probe section
    std::cout << "--- Pattern Probe ---" << std::endl;
    // Note: Would need to implement probePattern function from Phase5Demo.cpp
    
    // Sandbox run
    std::cout << "\n--- Sandbox Run (100 steps) ---" << std::endl;
    uint64_t brainStep = 0;
    for (int step = 0; step < 100; ++step) {
        const SensoryPercept& percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        for (int i = 0; i < 10; ++i) brain->step(brainStep++);
        
        MotorCommand cmd = agent.decodeMotorCommand();
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        agent.applyRewardModulation(result.reward, 0.0f);
        world.update(0.1);
        
        totalReward += result.reward;
        if (cmd == MotorCommand::Interact) {
            if (result.message == "consumed_resource") ++resources;
            else if (result.message == "hit_hazard") ++hazards;
            else ++emptyInteracts;
        }
        
        const auto& body = world.getAgentBody();
        visited.insert({static_cast<int>(std::floor(body.x)),
                        static_cast<int>(std::floor(body.y))});
        
        if (step % 25 == 0) {
            std::cout << "  Step " << step << ": " << agent.getThoughtTrace()
                      << " wmNeurons=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
                      << " firing=" << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    std::cout << "Sandbox results: reward=" << totalReward
              << " resources=" << resources << " hazards=" << hazards
              << " emptyInteracts=" << emptyInteracts
              << " visitedCells=" << visited.size()
              << " avgRate=" << brain->getAverageFiringRate()
              << " totalSpikes=" << brain->getTotalSpikeCount()
              << " workingNeurons=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryNeuronCount() : 0)
              << " episodic=" << (brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0)
              << " preference=" << agent.getLearnedPreference() << std::endl;
    
    // Checkpoint test
    std::cout << "\n--- Checkpoint Test ---" << std::endl;
    const std::string ckpt = "/tmp/nlm_phase5_demo.ckpt";
    bool saved = brain->save(ckpt);
    auto brain2 = std::make_shared<Brain>(config);
    brain2->initialize();
    bool loaded = saved && brain2->load(ckpt);
    std::cout << "  Checkpoint: saved=" << (saved ? "yes" : "NO") 
              << " loaded=" << (loaded ? "yes" : "NO");
    
    if (loaded) {
        // Test RNG continuity
        uint32_t drawSaved = brain->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
        uint32_t drawReloaded = brain2->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(1000000));
        std::cout << " totalSpikes=" << brain2->getTotalSpikeCount()
                  << " workingNeurons=" << (brain2->getWorkingMemory() ? brain2->getWorkingMemory()->getMemoryNeuronCount() : 0)
                  << " episodic=" << (brain2->getEpisodicMemory() ? brain2->getEpisodicMemory()->getEpisodeCount() : 0)
                  << " preference=" << brain2->getLearnedPreference()
                  << " rngMatch=" << (drawSaved == drawReloaded ? "yes" : "NO");
    }
    
    std::cout << std::endl << "=== Phase 5 Demo Complete ===" << std::endl;
}

void runPhase6Demo() {
    std::cout << "=== NLM Phase 6: Final Integration Demo ===" << std::endl;
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // Quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full integration experiment
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
}

void runBenchmarkDemo() {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    // Initialize benchmark
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200},
    };
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    for (const auto& [neurons, steps] : benchmarks) {
        std::cout << "  Running benchmark: " << neurons << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            for (size_t i = 0; i < std::min(size_t(10), neurons / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << (steps / totalTime) << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << (static_cast<double>(brain->getTotalSpikeCount()) / steps) << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== Benchmark Complete ===" << std::endl;
}

int main(int argc, char** argv) {
    try {
        printBanner();
        
        // Initialize global configuration
        initializeConfig(argc, argv);
        
        if (argc < 2) {
            printUsage(argv[0]);
            std::cout << std::endl << "No command specified. Use '--help' for usage." << std::endl;
            return 1;
        }
        
        std::string command = argv[1];
        
        // Process based on command
        if (command == "demo") {
            if (argc < 3) {
                std::cerr << "ERROR: No demo mode specified." << std::endl;
                printUsage(argv[0]);
                return 1;
            }
            
            std::string mode = argv[2];
            
            if (mode == "phase2") {
                runPhase2Demo();
            } else if (mode == "phase3") {
                int episodes = 5;
                int stepsPerEpisode = 200;
                
                // Parse optional arguments
                for (int i = 3; i < argc; ++i) {
                    std::string arg = argv[i];
                    if (arg == "--episodes" && i + 1 < argc) {
                        episodes = std::atoi(argv[++i]);
                    } else if (arg == "--steps-per-episode" && i + 1 < argc) {
                        stepsPerEpisode = std::atoi(argv[++i]);
                    } else if (arg == "--world-size" && i + 1 < argc) {
                        int worldSize = std::atoi(argv[++i]);
                        // Could store world size in config
                    }
                }
                
                runPhase3Demo(episodes, stepsPerEpisode);
            } else if (mode == "phase5") {
                runPhase5Demo();
            } else if (mode == "phase6") {
                runPhase6Demo();
            } else {
                std::cerr << "ERROR: Unknown demo mode: " << mode << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else if (command == "benchmark") {
            runBenchmarkDemo();
        } else if (command == "experiment") {
            std::cout << "Experiment command not yet implemented. Available: demo, benchmark" << std::endl;
            printUsage(argv[0]);
            return 1;
        } else {
            std::cerr << "ERROR: Unknown command: " << command << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        
        std::cout << std::endl << "=== NLM execution completed ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
}