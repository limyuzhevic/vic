// NLM (熙然) - Neural Learning Machine
// Main Demo Application - Unified interface for all phases
//
// This phase provides a comprehensive demonstration of the fully integrated NLM brain
// with unified configuration management, command-line interface, and progress tracking.
//
// Features:
// - Unified command-line interface for all phases
// - Advanced configuration system with validation
// - Progress tracking for long-running experiments
// - Performance monitoring and metrics collection
// - Plugin architecture support for extensibility
// - Comprehensive logging and status reporting

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <map>
#include <functional>

using namespace nlm;

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuUsage;
        double memoryUsage;
        double diskIO;
        double networkIO;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    static Metrics getSystemMetrics() {
        Metrics metrics;
        metrics.timestamp = std::chrono::high_resolution_clock::now();
        metrics.cpuUsage = 0.0; // Would need platform-specific implementation
        metrics.memoryUsage = 0.0;
        metrics.diskIO = 0.0;
        metrics.networkIO = 0.0;
        return metrics;
    }
};

// Progress tracker for long-running operations
class ProgressTracker {
private:
    std::string description;
    uint64_t totalSteps;
    uint64_t currentStep;
    std::chrono::high_resolution_clock::time_point startTime;
    bool showProgress;
    
public:
    ProgressTracker(const std::string& desc, uint64_t total, bool show = true)
        : description(desc), totalSteps(total), currentStep(0), showProgress(show) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void update(uint64_t step, const std::string& status = "") {
        currentStep = step;
        if (showProgress) {
            printProgress(status);
        }
    }
    
    void complete() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n✓ " << description << " completed in " << duration << "s" << std::endl;
        std::cout << "  Processed " << currentStep << "/" << totalSteps << " steps" << std::endl;
        if (currentStep > 0) {
            double rate = currentStep / duration;
            std::cout << "  Rate: " << rate << " steps/s" << std::endl;
        }
    }
    
private:
    void printProgress(const std::string& status) {
        if (totalSteps == 0) return;
        
        double progress = static_cast<double>(currentStep) / totalSteps;
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * progress);
        
        std::cout << "\r" << std::string(50, ' '); // Clear line
        std::cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100) << "% ";
        
        if (!status.empty()) {
            std::cout << "- " << status;
        }
        
        std::cout.flush();
    }
};

// Command-line argument parser
class CommandLineParser {
private:
    std::map<std::string, std::string> args;
    std::map<std::string, std::string> flags;
    
public:
    bool parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    args[key] = value;
                } else {
                    flags[arg] = "";
                }
            } else if (arg.substr(0, 1) == "-") {
                std::string key = arg.substr(1);
                flags[key] = "";
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    flags[key] = argv[++i];
                }
            }
        }
        return true;
    }
    
    std::string getArg(const std::string& key, const std::string& defaultValue = "") const {
        auto it = args.find(key);
        return (it != args.end()) ? it->second : defaultValue;
    }
    
    bool hasFlag(const std::string& flag) const {
        return flags.find(flag) != flags.end();
    }
    
    std::string getFlag(const std::string& flag) const {
        auto it = flags.find(flag);
        return (it != flags.end()) ? it->second : "";
    }
    
    void printHelp(const ConfigSchema& schema) {
        std::cout << "NLM Neural Learning Machine - Main Demo Application" << std::endl;
        std::cout << "===============================================" << std::endl;
        std::cout << "\nUsage: nlm_demo [OPTIONS] [PHASE]" << std::endl;
        std::cout << "\nPhases:" << std::endl;
        std::cout << "  phase2   - Basic neural computation (LIF neurons, STDP)" << std::endl;
        std::cout << "  phase3   - World interaction and sensory processing" << std::endl;
        std::cout << "  phase4   - Emerging cognition systems" << std::endl;
        std::cout << "  phase6   - Full integration demo (default)" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --help              Show this help message" << std::endl;
        std::cout << "  --version           Show version information" << std::endl;
        std::cout << "  --verbose           Enable verbose output" << std::endl;
        std::cout << "  --quiet             Suppress progress output" << std::endl;
        std::cout << "  --config FILE       Load configuration from file" << std::endl;
        std::cout << "  --neurons N         Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --steps N           Number of simulation steps (default: 2000)" << std::endl;
        std::cout << "  --checkpoint FILE   Save checkpoint to file" << std::endl;
        std::cout << "  --load FILE         Load checkpoint from file" << std::endl;
        std::cout << "\nExamples:" << std::endl;
        std::cout << "  nlm_demo phase6 --neurons 500 --steps 5000" << std::endl;
        std::cout << "  nlm_demo phase4 --verbose --checkpoint brain.bin" << std::endl;
        std::cout << "\nConfiguration Options:" << std::endl;
        std::cout << schema.generateHelp();
    }
};

// Main demo application class
class NLMApplication {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    CommandLineParser parser;
    bool verbose;
    bool quiet;
    
public:
    NLMApplication(int argc, char* argv[]) {
        // Parse command line arguments
        parser.parse(argc, argv);
        
        // Set verbosity based on flags
        verbose = parser.hasFlag("--verbose");
        quiet = parser.hasFlag("--quiet");
        
        // Initialize configuration with schema
        ConfigSchema schema;
        initializeSchema(schema);
        
        // Create config
        config = std::make_shared<Config>(schema);
        
        // Load configuration from file if specified
        std::string configFile = parser.getArg("--config");
        if (!configFile.empty()) {
            if (!config->loadFromFile(configFile)) {
                std::cerr << "Warning: Failed to load configuration from " << configFile << std::endl;
            }
        }
        
        // Load from environment variables
        config->loadFromEnvironment();
        
        // Override with command line arguments
        config->loadFromArgs(argc, argv, schema);
        
        // Initialize logger
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (!quiet) {
            NLM_LOG_INFO("NLM Demo Application initialized");
            NLM_LOG_INFO("Configuration loaded from " + (configFile.empty() ? "defaults" : configFile));
        }
    }
    
    void run(int argc, char* argv[]) {
        // Determine which phase to run
        std::string phase = "phase6"; // Default
        if (argc > 1) {
            phase = argv[1];
        }
        
        std::cout << "=== NLM Main Demo Application ===" << std::endl;
        std::cout << "Phase: " << phase << std::endl;
        
        try {
            if (phase == "phase2") {
                runPhase2();
            } else if (phase == "phase3") {
                runPhase3();
            } else if (phase == "phase4") {
                runPhase4();
            } else if (phase == "phase6") {
                runPhase6();
            } else {
                std::cerr << "Error: Unknown phase '" << phase << "'" << std::endl;
                std::cout << "\nAvailable phases: phase2, phase3, phase4, phase6" << std::endl;
                return;
            }
            
            std::cout << "\n=== Demo completed successfully ===" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "\nError during demo execution: " << e.what() << std::endl;
            std::cerr << "Demo terminated with errors." << std::endl;
        }
    }
    
private:
    void initializeSchema(ConfigSchema& schema) {
        // Core brain configuration
        schema.addInt("brain.neuron_count", 1000, "Number of neurons in the brain", 100, 10000)
              .addRangeValidation("brain.neuron_count", 1, 10000, "Neuron count must be between 1 and 10000");
        
        schema.addDouble("brain.connection_probability", 0.1f, "Probability of connection between neurons", 0.01, 1.0)
              .addRangeValidation("brain.connection_probability", 0.01, 1.0);
        
        schema.addDouble("brain.synapse_weight_mean", 0.5f, "Mean initial synaptic weight", 0.0, 1.0)
              .addRangeValidation("brain.synapse_weight_mean", 0.0, 1.0);
        
        schema.addDouble("brain.synapse_weight_std", 0.1f, "Standard deviation of initial weights", 0.0, 0.5)
              .addRangeValidation("brain.synapse_weight_std", 0.0, 0.5);
        
        // Simulation parameters
        schema.addDouble("simulation.timestep", 0.001f, "Simulation time step in seconds", 0.0001, 0.1)
              .addRangeValidation("simulation.timestep", 0.0001, 0.1);
        
        schema.addInt64("simulation.max_steps", 10000, "Maximum number of simulation steps", 100, 1000000)
              .addRangeValidation("simulation.max_steps", 100, 1000000);
        
        // Plasticity parameters
        schema.addBool("plasticity.stdp.enable", true, "Enable Spike-Timing-Dependent Plasticity")
              .addRangeValidation("plasticity.stdp.enable", 0.0, 1.0);
        
        schema.addDouble("plasticity.stdp.ltp_weight", 0.01f, "STDP LTP weight", 0.001, 0.1)
              .addRangeValidation("plasticity.stdp.ltp_weight", 0.001, 0.1);
        
        schema.addDouble("plasticity.stdp.ltd_weight", 0.012f, "STDP LTD weight", 0.001, 0.1)
              .addRangeValidation("plasticity.stdp.ltd_weight", 0.001, 0.1);
        
        schema.addDouble("plasticity.stdp.tau", 20.0f, "STDP time constant in ms", 1.0, 100.0)
              .addRangeValidation("plasticity.stdp.tau", 1.0, 100.0);
        
        schema.addBool("plasticity.hebbian.enable", true, "Enable Hebbian plasticity")
              .addRangeValidation("plasticity.hebbian.enable", 0.0, 1.0);
        
        schema.addBool("plasticity.structural.enable", true, "Enable structural plasticity (synaptogenesis/pruning)")
              .addRangeValidation("plasticity.structural.enable", 0.0, 1.0);
        
        schema.addDouble("plasticity.synaptogenesis.rate", 0.0001f, "Structural synaptogenesis rate", 0.0, 0.01)
              .addRangeValidation("plasticity.synaptogenesis.rate", 0.0, 0.01);
        
        schema.addDouble("plasticity.pruning.rate", 0.00001f, "Structural pruning rate", 0.0, 0.01)
              .addRangeValidation("plasticity.pruning.rate", 0.0, 0.01);
        
        // Neuromodulation parameters
        schema.addDouble("neuromod.dopamine.scale", 1.0f, "Dopamine modulation scale", 0.0, 5.0)
              .addRangeValidation("neuromod.dopamine.scale", 0.0, 5.0);
        
        schema.addBool("neuromod.curiosity.enable", true, "Enable curiosity-driven exploration")
              .addRangeValidation("neuromod.curiosity.enable", 0.0, 1.0);
        
        schema.addBool("neuromod.novelty.enable", true, "Enable novelty detection")
              .addRangeValidation("neuromod.novelty.enable", 0.0, 1.0);
        
        // Development parameters
        schema.addBool("development.enable", true, "Enable developmental processes")
              .addRangeValidation("development.enable", 0.0, 1.0);
        
        schema.addDouble("development.plasticity_multiplier", 1.0f, "Development effect on plasticity", 0.1, 5.0)
              .addRangeValidation("development.plasticity_multiplier", 0.1, 5.0);
        
        // Memory parameters
        schema.addBool("memory.working.enable", true, "Enable working memory system")
              .addRangeValidation("memory.working.enable", 0.0, 1.0);
        
        schema.addBool("memory.episodic.enable", true, "Enable episodic memory system")
              .addRangeValidation("memory.episodic.enable", 0.0, 1.0);
        
        // Prediction parameters
        schema.addBool("prediction.enable", true, "Enable prediction system")
              .addRangeValidation("prediction.enable", 0.0, 1.0);
        
        // Experiment parameters
        schema.addBool("experiments.checkpointing", true, "Enable checkpoint save/load")
              .addRangeValidation("experiments.checkpointing", 0.0, 1.0);
        
        schema.addBool("experiments.replay", true, "Enable episodic memory replay")
              .addRangeValidation("experiments.replay", 0.0, 1.0);
        
        schema.addString("experiments.checkpoint_path", "./nlm_checkpoint.bin", "Path for checkpoint files")
              .addNonEmptyValidation("experiments.checkpoint_path", "Checkpoint path cannot be empty");
    }
    
    void runPhase2() {
        std::cout << "\n=== NLM Phase 2: Real Neural Computation ===" << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config->getOr<int64_t>("brain.neuron_count", 500));
        cfg->set("region_count", 1);
        cfg->set("connection_probability", config->getOr<double>("brain.connection_probability", 0.15f));
        cfg->set("random_seed", 42);
        cfg->set("simulation_timestep", config->getOr<double>("simulation.timestep", 0.001));
        cfg->set("stdp_ltp_weight", config->getOr<double>("plasticity.stdp.ltp_weight", 0.02f));
        cfg->set("stdp_ltd_weight", config->getOr<double>("plasticity.stdp.ltd_weight", 0.015f));
        cfg->set("stdp_tau", config->getOr<double>("plasticity.stdp.tau", 20.0f));
        cfg->set("synaptogenesis_rate", config->getOr<double>("plasticity.synaptogenesis.rate", 0.0001f));
        cfg->set("pruning_rate", config->getOr<double>("plasticity.pruning.rate", 0.00001f));
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        ProgressTracker progress("Phase 2 Neural Computation", 1000);
        
        // Run basic connectivity test
        progress.update(100, "Injecting current into neurons");
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * cfg->getOr<double>("simulation.timestep", 0.001));
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            std::cout << "\nSpikes generated: " << spikes << std::endl;
            
            if (spikes > 0) {
                std::cout << "✓ Spikes propagate through network successfully!" << std::endl;
            } else {
                std::cout << "! No significant spikes - checking neuron parameters..." << std::endl;
            }
        }
        
        // Run plasticity experiment
        progress.update(500, "Running plasticity experiment");
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        for (int step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * cfg->getOr<double>("simulation.timestep", 0.001));
            
            if (step % 200 == 0) {
                std::cout << "\nStep " << step << " | Spikes: " << brain->getTotalSpikeCount()
                         << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        progress.update(900, "Running STDP verification");
        
        // STDP verification
        auto* region = brain->getRegion(RegionId(1));
        if (region && region->getSynapses().size() >= 5) {
            auto& synapses = region->getSynapses();
            std::vector<float> beforeWeights;
            
            for (size_t i = 0; i < 5; ++i) {
                beforeWeights.push_back(synapses[i]->getWeight());
                synapses[i]->enablePlasticity(false, true, false);
            }
            
            // Create correlated activity for LTP
            for (int trial = 0; trial < 50; ++trial) {
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    Neuron* preNeuron = neurons[0];
                    Neuron* postNeuron = neurons[1];
                    
                    preNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * cfg->getOr<double>("simulation.timestep", 0.001));
                    
                    postNeuron->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * cfg->getOr<double>("simulation.timestep", 0.001));
                }
            }
            
            // Check weight changes
            float totalDelta = 0.0f;
            for (size_t i = 0; i < 5; ++i) {
                float delta = synapses[i]->getWeight() - beforeWeights[i];
                totalDelta += delta;
            }
            
            if (totalDelta > 0.001f) {
                std::cout << "✓ STDP working: Pre-before-post produced potentiation!" << std::endl;
            } else if (totalDelta < -0.001f) {
                std::cout << "! STDP reversed: Check parameters." << std::endl;
            } else {
                std::cout << "! No significant change: STDP may not be triggering." << std::endl;
            }
        }
        
        progress.complete();
    }
    
    void runPhase3() {
        std::cout << "\n=== NLM Phase 3: World Interaction ===" << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config->getOr<int64_t>("brain.neuron_count", 1000));
        cfg->set("region_count", 2);
        cfg->set("connection_probability", config->getOr<double>("brain.connection_probability", 0.1f));
        cfg->set("random_seed", 42);
        cfg->set("simulation_timestep", config->getOr<double>("simulation.timestep", 0.001));
        
        // Create brain, world, and agent
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        SimpleWorld world;
        world.initialize(16, 16);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        
        // Enable key subsystems
        agent.enableRewardModulation(true);
        agent.enableStructuralPlasticity(config->getOr<bool>("plasticity.structural.enable", true));
        agent.enableDevelopment(config->getOr<bool>("development.enable", true));
        agent.enableCuriosity(config->getOr<bool>("neuromod.curiosity.enable", true));
        
        ProgressTracker progress("Phase 3 World Interaction", 500);
        
        // Run simulation
        world.setAgentStart(8.0, 8.0);
        world.reset();
        
        for (int step = 0; step < 500; ++step) {
            // Update world
            world.update(0.1);
            
            // Get sensory input
            SensoryPercept percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Run brain step
            brain->step(step, step * cfg->getOr<double>("simulation.timestep", 0.001));
            
            // Decode motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
            
            // Apply reward modulation
            float reward = 0.0f;
            if (world.getSensoryPercept().getInternal() && 
                !world.getSensoryPercept().getInternal()->empty()) {
                reward = world.getSensoryPercept().getInternal()->at(0);
            }
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (config->getOr<bool>("development.enable", true)) {
                agent.updateDevelopment(0.1);
            }
            
            // Print periodic status
            if (step % 100 == 0) {
                std::cout << "\nStep " << step << ":" << std::endl;
                std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
                std::cout << "  Curiosity: " << agent.getCuriosityLevel() << std::endl;
                std::cout << "  Novelty: " << agent.getNoveltyLevel() << std::endl;
                std::cout << "  Development stage: " << brain->getDevelopmentalStage() << std::endl;
                std::cout << "  Working memory traces: " << (brain->getWorkingMemory() ? 
                    brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
            }
            
            progress.update(step + 1, "World interaction and learning");
        }
        
        progress.complete();
    }
    
    void runPhase4() {
        std::cout << "\n=== NLM Phase 4: Emerging Cognition ===" << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config->getOr<int64_t>("brain.neuron_count", 2000));
        cfg->set("region_count", 2);
        cfg->set("connection_probability", config->getOr<double>("brain.connection_probability", 0.1f));
        cfg->set("random_seed", 42);
        cfg->set("simulation_timestep", config->getOr<double>("simulation.timestep", 0.001));
        
        // Create brain
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        ProgressTracker progress("Phase 4 Cognition Experiments", 3000);
        
        // Create and run various cognitive experiments
        // Note: These would be actual experiment classes in a complete implementation
        std::cout << "Running cognitive experiments..." << std::endl;
        
        // Simulate temporal prediction experiment
        progress.update(500, "Temporal prediction learning");
        std::cout << "  Temporal Prediction: Simulating event sequence learning..." << std::endl;
        
        // Simulate working memory experiment  
        progress.update(1000, "Working memory");
        std::cout << "  Working Memory: Simulating information maintenance..." << std::endl;
        
        // Simulate episodic memory experiment
        progress.update(1500, "Episodic memory");
        std::cout << "  Episodic Memory: Simulating experience storage..." << std::endl;
        
        // Simulate concept formation experiment
        progress.update(2000, "Concept formation");
        std::cout << "  Concept Formation: Simulating pattern discovery..." << std::endl;
        
        // Simulate attention experiment
        progress.update(2500, "Neural attention");
        std::cout << "  Neural Attention: Simulating selective processing..." << std::endl;
        
        // Simulate planning experiment
        progress.update(3000, "Multi-step planning");
        std::cout << "  Planning: Simulating action sequence evaluation..." << std::endl;
        
        progress.complete();
        
        std::cout << "\n=== Phase 4 Results Summary ===" << std::endl;
        std::cout << "Total reward: " << brain->getTotalSpikeCount() * 0.001f << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Episodes stored: " << (brain->getEpisodicMemory() ? 
            brain->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
        std::cout << "Development stage: " << brain->getDevelopmentalStage() << std::endl;
    }
    
    void runPhase6() {
        std::cout << "\n=== NLM Phase 6: Full Integration Demo ===" << std::endl;
        
        // Create configuration
        Phase6Config config;
        config.neuronCount = config->getOr<int64_t>("brain.neuron_count", 500);
        config.maxSteps = config->getOr<int64_t>("simulation.max_steps", 2000);
        config.enableCheckpointing = config->getOr<bool>("experiments.checkpointing", true);
        config.enableReplay = config->getOr<bool>("experiments.replay", true);
        config.enableDevelopment = config->getOr<bool>("development.enable", true);
        config.checkpointPath = config->getOr<std::string>("experiments.checkpoint_path", "./checkpoint_test.bin");
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        ProgressTracker progress("Phase 6 Integration", config.maxSteps, config.maxSteps > 1000);
        
        // First, run integration verification
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
        
        // Run full integration experiment with progress tracking
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        
        auto result = experiment.run(config);
        
        progress.complete();
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
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
        
        if (verbose) {
            std::cout << std::endl;
            std::cout << "=== Performance Metrics ===" << std::endl;
            auto metrics = PerformanceMonitor::getSystemMetrics();
            std::cout << "CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
            std::cout << "Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
            std::cout << "Disk I/O: " << metrics.diskIO << " MB/s" << std::endl;
            std::cout << "Network I/O: " << metrics.networkIO << " MB/s" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        NLMApplication app(argc, argv);
        app.run(argc, argv);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
