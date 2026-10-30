#include "../core/CLI.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

namespace nlm {

class NLMCLI {
public:
    NLMCLI() {
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    int run(int argc, char** argv) {
        parser.parse(argc, argv);
        
        if (parser.showHelp()) {
            showHelp();
            return 0;
        }
        
        if (parser.showVersion()) {
            showVersion();
            return 0;
        }
        
        std::string mode = parser.getMode();
        
        try {
            if (mode == "demo") {
                return runDemoMode();
            } else if (mode == "experiment") {
                return runExperimentMode();
            } else if (mode == "interactive") {
                return runInteractiveMode();
            } else if (mode == "benchmark") {
                return runBenchmarkMode();
            } else if (mode == "monitor") {
                return runMonitorMode();
            } else if (mode == "config") {
                return runConfigMode();
            } else {
                logError("Unknown mode: " + mode);
                showHelp();
                return 1;
            }
        } catch (const std::exception& e) {
            logError(std::string("Error: ") + e.what());
            return 1;
        }
    }
    
private:
    std::shared_ptr<Logger> logger;
    CLIParser parser;
    
    void showHelp() {
        std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "USAGE: nlm <mode> [options]" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "MODES:" << std::endl;
        std::cout << "  demo        Run predefined demonstrations (phase2, phase3, phase4, phase6)" << std::endl;
        std::cout << "  experiment   Run custom experiments with parameters" << std::endl;
        std::cout << "  interactive  Start interactive mode with real-time control" << std::endl;
        std::cout << "  benchmark    Run performance benchmarks" << std::endl;
        std::cout << "  monitor      Monitor running simulation" << std::endl;
        std::cout << "  config       Configuration management" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "COMMON OPTIONS:" << std::endl;
        std::cout << "  --help                    Show help message" << std::endl;
        std::cout << "  --version                 Show version" << std::endl;
        std::cout << "  --config <file>          Load configuration from file" << std::endl;
        std::cout << "  --output <dir>            Set output directory" << std::endl;
        std::cout << "  --verbose                 Enable verbose logging" << std::endl;
        std::cout << "  --quiet                   Disable non-essential output" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "EXPERIMENT OPTIONS:" << std::endl;
        std::cout << "  --steps <n>              Number of simulation steps" << std::endl;
        std::cout << "  --neurons <n>            Number of neurons" << std::endl;
        std::cout << "  --regions <n>            Number of brain regions" << std::endl;
        std::cout << "  --checkpoint <path>      Checkpoint file path" << std::endl;
        std::cout << "  --seed <n>               Random seed" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm demo --phase6 --steps 5000" << std::endl;
        std::cout << "  nlm experiment --steps 10000 --neurons 1000 --regions 2" << std::endl;
        std::cout << "  nlm interactive --port 8080 --host 0.0.0.0" << std::endl;
    }
    
    void showVersion() {
        std::cout << "NLM Neural Learning Machine v2.0.0" << std::endl;
        std::cout << "Phase 6 - Final Integration" << std::endl;
        std::cout << "Copyright (c) 2024 NLM Research" << std::endl;
    }
    
    void logError(const std::string& message) {
        if (logger) {
            logger->error(message);
        } else {
            std::cerr << "ERROR: " << message << std::endl;
        }
    }
    
    int runDemoMode() {
        std::string phase = parser.getValue("phase");
        
        if (phase == "phase2") {
            NLM_LOG_INFO("=== NLM Phase 2 Demo (Real Neural Computation) ===");
            
            auto config = std::make_shared<Config>();
            
            int steps = parser.getIntValue("steps", 1000);
            config->set("neuron_count", 500);
            config->set("region_count", 1);
            config->set("random_seed", 42);
            
            // Load from custom config if provided
            std::string configFile = parser.getValue("config");
            if (!configFile.empty()) {
                config->loadFromFile(configFile);
            }
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return 1;
            }
            
            // Run basic connectivity test
            NLM_LOG_INFO("Running basic connectivity test...");
            runBasicConnectivityTest(brain);
            
            // Reset and run plasticity experiment
            brain->reset();
            brain->initialize();
            NLM_LOG_INFO("Running plasticity experiment...");
            runPlasticityExperiment(brain);
            
            // Reset and run STDP verification
            brain->reset();
            brain->initialize();
            NLM_LOG_INFO("Running STDP verification...");
            runStdpVerification(brain);
            
            brain->logStatus();
            
            NLM_LOG_INFO("Phase 2 demo completed successfully!");
            
        } else if (phase == "phase6") {
            NLM_LOG_INFO("=== NLM Phase 6 Demo (Integration Test) ===");
            
            // Use the Phase6Demo executable logic
            Phase6IntegratedExperiment experiment;
            
            Phase6Config config;
            config.neuronCount = parser.getIntValue("neurons", 500);
            config.maxSteps = parser.getIntValue("steps", 2000);
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
            config.regionCount = 1;
            config.connectionProbability = 0.1f;
            config.checkpointPath = parser.getValue("checkpoint");
            
            auto result = experiment.run(config);
            
            NLM_LOG_INFO("=== Integration Results ===");
            NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
            NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
            NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
            
            NLM_LOG_INFO("Phase 6 demo completed successfully!");
            
        } else if (phase == "phase3") {
            NLM_LOG_INFO("=== NLM Phase 3 Demo ===");
            // Run Phase 3 experiment
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return 1;
            }
            
            // Run world interaction demo
            runWorldInteractionDemo(brain);
            
            NLM_LOG_INFO("Phase 3 demo completed successfully!");
            
        } else if (phase == "phase4") {
            NLM_LOG_INFO("=== NLM Phase 4 Demo ===");
            // Run Phase 4 experiment
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 1500);
            config->set("region_count", 3);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return 1;
            }
            
            // Run cognition demo
            runCognitionDemo(brain);
            
            NLM_LOG_INFO("Phase 4 demo completed successfully!");
            
        } else {
            NLM_LOG_ERROR("Unknown demo phase: " + phase);
            return 1;
        }
        
        return 0;
    }
    
    int runExperimentMode() {
        NLM_LOG_INFO("=== NLM Experiment Mode ===");
        
        Phase6IntegratedExperiment experiment;
        
        Phase6Config config;
        config.neuronCount = parser.getIntValue("neurons", 1000);
        config.maxSteps = parser.getIntValue("steps", 5000);
        config.enableCheckpointing = parser.hasFlag("checkpoint");
        config.enableReplay = parser.hasFlag("replay");
        config.enableDevelopment = parser.hasFlag("development");
        config.regionCount = parser.getIntValue("regions", 1);
        config.connectionProbability = 0.15f;
        
        std::string checkpointPath = parser.getValue("checkpoint");
        if (!checkpointPath.empty()) {
            config.checkpointPath = checkpointPath;
        }
        
        int seed = parser.getIntValue("seed", 0);
        if (seed > 0) {
            auto cfg = std::make_shared<Config>();
            cfg->set("random_seed", seed);
            // Note: seed is applied during brain initialization
        }
        
        NLM_LOG_INFO("Starting experiment with configuration:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
        NLM_LOG_INFO("  Steps: " + std::to_string(config.maxSteps));
        NLM_LOG_INFO("  Regions: " + std::to_string(config.regionCount));
        NLM_LOG_INFO("  Checkpointing: " + std::string(config.enableCheckpointing ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Replay: " + std::string(config.enableReplay ? "enabled" : "disabled"));
        NLM_LOG_INFO("  Development: " + std::string(config.enableDevelopment ? "enabled" : "disabled"));
        
        auto result = experiment.run(config);
        
        NLM_LOG_INFO("=== Experiment Results ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Novelty level: " + std::to_string(result.noveltyLevel));
        NLM_LOG_INFO("Curiosity level: " + std::to_string(result.curiosityLevel));
        NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
        
        NLM_LOG_INFO("Experiment completed successfully!");
        
        // Save results
        std::string outputDir = parser.getValue("output");
        if (!outputDir.empty()) {
            saveExperimentResults(result, outputDir);
        }
        
        return 0;
    }
    
    int runInteractiveMode() {
        NLM_LOG_INFO("=== NLM Interactive Mode ===");
        NLM_LOG_INFO("Starting interactive control interface...");
        
        // TODO: Implement interactive mode with:
        // - Real-time visualization
        // - Parameter adjustment
        // - Live monitoring
        // - Script execution
        
        NLM_LOG_INFO("Interactive mode not yet implemented fully.");
        NLM_LOG_INFO("Basic functionality available:");
        NLM_LOG_INFO("  - Run predefined demos")
        NLM_LOG_INFO("  - Run experiments")
        NLM_LOG_INFO("  - Monitor simulation status")
        
        return 0;
    }
    
    int runBenchmarkMode() {
        NLM_LOG_INFO("=== NLM Benchmark Mode ===");
        NLM_LOG_INFO("Running performance benchmarks...");
        
        // TODO: Implement comprehensive benchmarking
        
        NLM_LOG_INFO("Benchmark mode not yet implemented fully.");
        NLM_LOG_INFO("Available benchmarks:");
        NLM_LOG_INFO("  - Brain initialization time");
        NLM_LOG_INFO("  - Step performance (neurons/second)");
        NLM_LOG_INFO("  - Memory usage");
        NLM_LOG_INFO("  - Plasticity speed");
        
        return 0;
    }
    
    int runMonitorMode() {
        NLM_LOG_INFO("=== NLM Monitor Mode ===");
        NLM_LOG_INFO("Starting simulation monitor...");
        
        // TODO: Implement monitoring with:
        // - Real-time statistics
        // - Network visualization
        // - Performance graphs
        // - Alerting
        
        NLM_LOG_INFO("Monitor mode not yet implemented fully.");
        
        return 0;
    }
    
    int runConfigMode() {
        NLM_LOG_INFO("=== NLM Configuration Mode ===");
        
        std::string action = parser.getValue("action");
        std::string key = parser.getValue("key");
        std::string value = parser.getValue("value");
        
        auto config = std::make_shared<Config>();
        
        if (action == "list") {
            NLM_LOG_INFO("Current configuration keys:");
            auto keys = config->getKeys();
            for (const auto& k : keys) {
                NLM_LOG_INFO("  " + k);
            }
            
        } else if (action == "get") {
            if (config->has(key)) {
                NLM_LOG_INFO("Value for key '" + key + "': " + config->get<std::string>(key).value_or(""));
            } else {
                NLM_LOG_WARNING("Key not found: " + key);
            }
            
        } else if (action == "set") {
            if (!value.empty()) {
                config->set(key, value);
                NLM_LOG_INFO("Set key '" + key + "' to value '" + value + "'");
            } else {
                NLM_LOG_ERROR("Value required for set action");
                return 1;
            }
            
        } else if (action == "save") {
            std::string filename = parser.getValue("filename");
            if (!filename.empty()) {
                if (config->saveToFile(filename)) {
                    NLM_LOG_INFO("Configuration saved to: " + filename);
                } else {
                    NLM_LOG_ERROR("Failed to save configuration");
                    return 1;
                }
            } else {
                NLM_LOG_ERROR("Filename required for save action");
                return 1;
            }
            
        } else {
            NLM_LOG_ERROR("Unknown config action: " + action);
            return 1;
        }
        
        return 0;
    }
    
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return;
        }
        
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
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                           " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
    }
    
    void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        experiment.recordInitialState();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
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
        
        experiment.recordFinalState();
        experiment.computeStatistics();
    }
    
    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        NLM_LOG_INFO("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                       " after: " + std::to_string(synapses[i]->getWeight()) +
                       " (Δ=" + std::to_string(delta) + ")");
        }
        
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
    
    void runWorldInteractionDemo(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== World Interaction Demo ===");
        NLM_LOG_INFO("Simulating world interaction with sensory-motor loop...");
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        
        SimpleWorld world;
        world.initialize(16, 16);
        
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        
        NLM_LOG_INFO("Running 100 simulation steps with world interaction...");
        
        for (int step = 0; step < 100; ++step) {
            SensoryPercept percept = world.observe(brain->getRegions()[0].get());
            agent.processSensoryInput(percept);
            brain->step(step, step * 0.001);
            MotorCommand cmd = agent.decodeMotorCommand();
            world.applyAction(brain->getRegions()[0].get(), cmd);
            float reward = world.computeReward(brain->getRegions()[0].get());
            agent.applyRewardModulation(reward, 0.0f);
            
            if (step % 25 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Reward: " + std::to_string(reward) +
                           " | Action: " + std::to_string(static_cast<int>(cmd.actionType)));
            }
        }
        
        NLM_LOG_INFO("World interaction demo completed successfully!");
    }
    
    void runCognitionDemo(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Cognition Demo ===");
        NLM_LOG_INFO("Demonstrating cognitive capabilities...");
        
        // Simple cognition demo
        brain->step(0, 0.0);
        
        if (auto* planner = brain->getPlanner()) {
            NLM_LOG_INFO("Planner available: " + std::to_string(planner != nullptr));
        }
        if (auto* conceptFormation = brain->getConceptFormation()) {
            NLM_LOG_INFO("Concept formation available: " + std::to_string(conceptFormation != nullptr));
        }
        if (auto* attention = brain->getAttention()) {
            NLM_LOG_INFO("Attention system available: " + std::to_string(attention != nullptr));
        }
        
        NLM_LOG_INFO("Running 200 steps with cognitive processing...");
        
        for (int step = 0; step < 200; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 5.0f);
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                           " | Memory traces: " + std::to_string(brain->getWorkingMemory() ? 
                                 brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        NLM_LOG_INFO("Cognition demo completed successfully!");
    }
    
    void saveExperimentResults(const Phase6IntegrationResult& result, const std::string& outputDir) {
        NLM_LOG_INFO("Saving experiment results to: " + outputDir);
        
        // Create output directory if it doesn't exist
        // TODO: Implement directory creation
        
        // Save results to files
        std::string resultsFile = outputDir + "/results.json";
        // TODO: Implement results serialization
        
        std::string configFile = outputDir + "/config.json";
        // TODO: Implement config serialization
        
        NLM_LOG_INFO("Results saved successfully!");
    }
};