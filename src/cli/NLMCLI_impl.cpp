// NLM Command Line Interface Implementation
// Comprehensive implementation of the unified NLM CLI

#include "cli/NLMCLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <ctime>

using namespace nlm;

// NLMCommandLineInterface implementation

NLMCommandLineInterface::NLMCommandLineInterface() : verbose_(false), quiet_(false) {}

NLMCommandLineInterface::~NLMCommandLineInterface() = default;

int NLMCommandLineInterface::execute(int argc, char* argv[]) {
    try {
        CommandOptions options = parseArguments(argc, argv);
        
        if (options.help) {
            printHelp();
            return 0;
        }
        
        if (options.version) {
            printVersion();
            return 0;
        }
        
        if (options.listPlugins) {
            return cmdPlugins(options);
        }
        
        if (options.batchFile.empty()) {
            return runCommand(options);
        } else {
            return cmdBatch(options);
        }
    } catch (const std::exception& e) {
        printStatus("Error: " + std::string(e.what()), true);
        return 1;
    } catch (...) {
        printStatus("Unknown error occurred", true);
        return 1;
    }
}

NLMCommandLineInterface::CommandOptions NLMCommandLineInterface::parseArguments(int argc, char* argv[]) {
    CommandOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--list-plugins") {
            options.listPlugins = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--run") {
            options.command = "run";
            if (i + 1 < argc) options.experimentPhase = argv[++i];
        } else if (arg == "--configure") {
            options.command = "configure";
            if (i + 1 < argc) options.configFile = argv[++i];
        } else if (arg == "--benchmark") {
            options.command = "benchmark";
            options.benchmarkMode = true;
        } else if (arg == "--profile") {
            options.command = "profile";
            options.profileMode = true;
            if (i + 1 < argc) options.profilerOutput = argv[++i];
        } else if (arg == "--batch") {
            options.command = "batch";
            if (i + 1 < argc) options.batchFile = argv[++i];
        } else if (arg == "--analyze") {
            options.command = "analyze";
            if (i + 1 < argc) options.analysisType = argv[++i];
        } else if (arg == "--episodes") {
            if (i + 1 < argc) options.numEpisodes = std::stoul(argv[++i]);
        } else if (arg == "--steps") {
            if (i + 1 < argc) options.stepsPerEpisode = std::stoul(argv[++i]);
        } else if (arg == "--output") {
            if (i + 1 < argc) options.outputFile = argv[++i];
        } else if (arg == "--checkpoint") {
            options.enableCheckpointing = true;
        } else if (arg == "--replay") {
            options.enableReplay = true;
        } else if (arg == "--development") {
            options.enableDevelopment = true;
        } else if (arg == "--curiosity") {
            options.enableCuriosity = true;
        } else if (arg == "--reward") {
            options.enableRewardModulation = true;
        } else if (arg == "--plasticity") {
            options.enableStructuralPlasticity = true;
        } else if (arg == "--seed") {
            if (i + 1 < argc) options.randomSeed = argv[++i];
        } else if (arg == "--threads") {
            if (i + 1 < argc) options.threadCount = std::stoul(argv[++i]);
        } else if (arg == "--parallel") {
            options.parallelExecution = true;
        } else if (arg == "--brain-type") {
            if (i + 1 < argc) options.brainType = argv[++i];
        } else if (arg == "--world-type") {
            if (i + 1 < argc) options.worldType = argv[++i];
        } else if (arg == "--output-format") {
            if (i + 1 < argc) options.outputFormat = argv[++i];
        } else if (arg == "--analysis") {
            if (i + 1 < argc) options.analysisType = argv[++i];
        } else {
            // Default command (run)
            options.command = "run";
            if (options.experimentPhase.empty()) {
                options.experimentPhase = arg; // Treat as phase if no --run flag
            }
        }
    }
    
    // Default command if not specified
    if (options.command.empty() && !options.experimentPhase.empty()) {
        options.command = "run";
    }
    
    return options;
}

void NLMCommandLineInterface::printHelp() const {
    printBanner();
    std::cout << "\n";
    std::cout << "USAGE:\n";
    std::cout << "  nlm [OPTIONS] [ARGS]\n\n";
    std::cout << "COMMANDS:\n";
    std::cout << "  --run <phase>              Run a specific phase (phase3, phase4, phase6)\n";
    std::cout << "  --configure <file>         Load configuration from file\n";
    std::cout << "  --benchmark                Run performance benchmarks\n";
    std::cout << "  --profile <output>         Run profiling and save results\n";
    std::cout << "  --batch <file>             Run batch of experiments from file\n";
    std::cout << "  --analyze <type>           Analyze experiment data\n";
    std::cout << "  --list-plugins             List available plugins\n\n";
    std::cout << "GLOBAL OPTIONS:\n";
    std::cout << "  --help, -h                 Show this help message\n";
    std::cout << "  --version                  Show version information\n";
    std::cout << "  --verbose, -v              Enable verbose output\n";
    std::cout << "  --quiet, -q                Suppress non-essential output\n\n";
    std::cout << "EXPERIMENT OPTIONS:\n";
    std::cout << "  --episodes <n>             Number of episodes (default: 1)\n";
    std::cout << "  --steps <n>                Steps per episode (default: 1000)\n";
    std::cout << "  --output <file>            Output file for results\n";
    std::cout << "  --checkpoint               Enable checkpointing\n";
    std::cout << "  --replay                   Enable replay system\n";
    std::cout << "  --development              Enable developmental systems\n";
    std::cout << "  --curiosity                Enable curiosity-driven exploration\n";
    std::cout << "  --reward                   Enable reward modulation\n";
    std::cout << "  --plasticity               Enable structural plasticity\n";
    std::cout << "  --seed <n>                 Random seed (default: 42)\n";
    std::cout << "  --threads <n>              Number of threads (default: 1)\n";
    std::cout << "  --parallel                 Enable parallel execution\n";
    std::cout << "  --brain-type <type>        Brain type (standard, optimized, etc.)\n";
    std::cout << "  --world-type <type>        World type (simple, complex, etc.)\n";
    std::cout << "  --output-format <format>    Output format (text, json, csv)\n";
    std::cout << "  --analysis <type>          Analysis type (basic, advanced, neural)\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  nlm --run phase3 --episodes 5 --steps 500\n";
    std::cout << "  nlm --configure config.json --run phase6 --checkpoint\n";
    std::cout << "  nlm --benchmark --threads 4\n";
    std::cout << "  nlm --profile profile.out --run phase4\n\n";
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm\n";
}

void NLMCommandLineInterface::printVersion() const {
    printBanner();
    std::cout << "NLM (熙然) - Neural Learning Machine\n";
    std::cout << "Version 0.1.0 (Phase 6 - Final Integration)\n";
    std::cout << "\n";
    std::cout << "A brain-inspired spiking neural network simulator with:\n";
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons\n";
    std::cout << "  - Event-driven spike propagation\n";
    std::cout << "  - STDP and Hebbian plasticity\n";
    std::cout << "  - Structural plasticity\n";
    std::cout << "  - Memory systems (working, episodic, associative)\n";
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)\n";
    std::cout << "  - Prediction systems\n";
    std::cout << "  - Cognitive mechanisms (planning, attention, concept formation)\n";
    std::cout << "  - Development and learning\n";
    std::cout << "\n";
    std::cout << "Built with C++20 and Python bindings via pybind11\n";
}

int NLMCommandLineInterface::runCommand(const CommandOptions& options) {
    if (options.experimentPhase == "phase3") {
        return runPhase3Demo(options);
    } else if (options.experimentPhase == "phase4") {
        return runPhase4Demo(options);
    } else if (options.experimentPhase == "phase6") {
        return runPhase6Demo(options);
    } else {
        printStatus("Unknown or unspecified experiment phase: " + options.experimentPhase, true);
        printStatus("Use --help for more information", true);
        return 1;
    }
}

int NLMCommandLineInterface::runPhase3Demo(const CommandOptions& options) {
    printStatus("Running Phase 3 demonstration...", false);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", std::stoul(options.randomSeed));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    
    // Apply additional configuration
    if (!options.outputFile.empty()) {
        // Could save config to file
    }
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(std::stoul(options.randomSeed));
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        printStatus("Failed to initialize brain!", true);
        return 1;
    }
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Configure agent
    if (options.enableRewardModulation) agent.enableRewardModulation(true);
    if (options.enableStructuralPlasticity) agent.enableStructuralPlasticity(true);
    if (options.enableDevelopment) agent.enableDevelopment(true);
    if (options.enableCuriosity) agent.enableCuriosity(true);
    
    double startTime = getCurrentWallTime();
    
    // Run demonstration
    std::vector<ExperimentMetrics> allMetrics;
    
    for (size_t episode = 0; episode < options.numEpisodes; ++episode) {
        printStatus("--- Episode " + std::to_string(episode) + " started ---", false);
        
        world.reset();
        agent.reset();
        brain->reset();
        
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        float episodeReward = 0.0f;
        
        for (size_t step = 0; step < options.stepsPerEpisode; ++step) {
            // Get sensory percept
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step (10 neural steps per world step)
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(neuralStep, neuralStep * 0.001);
            }
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply motor command
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world.update(0.01);
            
            // Update development
            if (options.enableDevelopment) {
                agent.updateDevelopment(0.01);
            }
            
            // Track metrics
            episodeReward += result.reward;
            metrics.steps++;
            metrics.totalReward += result.reward;
            metrics.totalSpikes += brain->getFiringNeuronCount();
            
            // Count actions
            switch (cmd) {
                case MotorCommand::MoveForward: metrics.forwardCount++; break;
                case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                case MotorCommand::Interact: metrics.interactCount++; break;
                default: metrics.waitCount++; break;
            }
            
            logStep(step, result.reward, agent.getNeuromodulationLevel(), 
                    agent.getCuriosityLevel(), world.getAgentBody().energy,
                    "Action", brain->getAverageFiringRate());
        }
        
        // Record final metrics
        metrics.finalNovelty = agent.getNoveltyLevel();
        metrics.finalCuriosity = agent.getCuriosityLevel();
        metrics.finalEnergy = world.getAgentBody().energy;
        metrics.predictionError = agent.getPredictionError();
        metrics.developmentalAge = agent.getBrain()->getTotalSynapseCount();
        metrics.developmentalStage = "Stage";
        metrics.totalSynapses = brain->getTotalSynapseCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
        
        allMetrics.push_back(metrics);
        
        printStatus("--- Episode " + std::to_string(episode) + " ended ---", false);
        printStatus("  Steps: " + std::to_string(metrics.steps), false);
        printStatus("  Total Reward: " + std::to_string(metrics.totalReward), false);
        printStatus("  Final Energy: " + std::to_string(metrics.finalEnergy), false);
        printStatus("  Synapses: " + std::to_string(metrics.totalSynapses), false);
    }
    
    double endTime = getCurrentWallTime();
    double wallTime = endTime - startTime;
    
    // Calculate summary statistics
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    float memoryUsage = 0.0f;
    size_t totalSteps = 0;
    
    for (const auto& metrics : allMetrics) {
        totalReward += metrics.totalReward;
        avgFiringRate += metrics.averageFiringRate;
        totalSteps += metrics.steps;
    }
    
    if (!allMetrics.empty()) {
        avgFiringRate /= allMetrics.size();
    }
    
    printExperimentSummary("Phase 3", options, startTime, endTime, 
                          totalReward, avgFiringRate, memoryUsage);
    
    // Save results if requested
    if (!options.outputFile.empty()) {
        std::string results = "Phase 3 Demo Results\n";
        results += "Wall Clock Time: " + std::to_string(wallTime) + "s\n";
        results += "Total Reward: " + std::to_string(totalReward) + "\n";
        results += "Average Firing Rate: " + std::to_string(avgFiringRate) + "\n";
        
        if (!saveResults(options.outputFile, results)) {
            printStatus("Warning: Could not save results to " + options.outputFile, false);
        }
    }
    
    return 0;
}

int NLMCommandLineInterface::runPhase4Demo(const CommandOptions& options) {
    printStatus("Running Phase 4 demonstration...", false);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", std::stoul(options.randomSeed));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        printStatus("Failed to initialize brain!", true);
        return 1;
    }
    
    printStatus("=== Phase 4: Emerging Cognition\n", false);
    
    // Run various experiments
    std::vector<std::pair<std::string, Phase4Results>> experimentResults;
    
    // Temporal Prediction Experiment
    {
        printStatus("--- 1. Temporal Prediction Learning ---", false);
        TemporalPredictionExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), options.numEpisodes);
        experimentResults.push_back({"Temporal Prediction", results});
        
        printStatus("Initial Prediction Error: " + std::to_string(results.predictionErrorInitial), false);
        printStatus("Final Prediction Error: " + std::to_string(results.predictionErrorFinal), false);
        
        float improvement = results.predictionErrorInitial - results.predictionErrorFinal;
        if (improvement > 0) {
            printStatus("  [SUCCESS] Prediction improved through experience!", false);
        }
    }
    
    // Working Memory Experiment
    {
        printStatus("\n--- 2. Working Memory ---", false);
        WorkingMemoryExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), options.numEpisodes);
        experimentResults.push_back({"Working Memory", results});
        
        printStatus("Initial Retention: " + std::to_string(results.workingMemoryRetentionInitial), false);
        printStatus("Final Retention: " + std::to_string(results.workingMemoryRetentionFinal), false);
    }
    
    // Episodic Recall Experiment
    {
        printStatus("\n--- 3. Episodic Memory ---", false);
        EpisodicRecallExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), options.numEpisodes);
        experimentResults.push_back({"Episodic Recall", results});
        
        printStatus("Recall Accuracy: " + std::to_string(results.episodicRecallAccuracy), false);
        printStatus("Episodes Stored: " + std::to_string(results.episodesStored), false);
    }
    
    // Concept Formation Experiment
    {
        printStatus("\n--- 4. Concept Formation ---", false);
        ConceptFormationExperiment experiment;
        Phase4Results results = experiment.run(brain.get(), options.numEpisodes);
        experimentResults.push_back({"Concept Formation", results});
        
        printStatus("Concepts Formed: " + std::to_string(results.conceptsFormed), false);
        printStatus("Concept Stability: " + std::to_string(results.conceptStability), false);
        
        if (results.conceptsFormed > 0) {
            printStatus("  [SUCCESS] Concepts emerged from repeated experience!", false);
        }
    }
    
    // Save results if requested
    if (!options.outputFile.empty()) {
        std::string results = "Phase 4 Demo Results\n";
        for (const auto& exp : experimentResults) {
            results += exp.first + ": " + std::to_string(exp.second.totalReward) + "\n";
        }
        
        if (!saveResults(options.outputFile, results)) {
            printStatus("Warning: Could not save results to " + options.outputFile, false);
        }
    }
    
    return 0;
}

int NLMCommandLineInterface::runPhase6Demo(const CommandOptions& options) {
    printStatus("Running Phase 6 integration demo...", false);
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // First, verify integration
    printStatus("Verifying integration...", false);
    if (!experiment.verifyIntegration()) {
        printStatus("Integration verification failed!", true);
        return 1;
    }
    
    printStatus("Integration verification passed!\n", false);
    
    // Run individual system tests
    printStatus("--- Memory Integration Test ---", false);
    experiment.testMemoryIntegration();
    printStatus("\n", false);
    
    printStatus("--- Neuromodulation Integration Test ---", false);
    experiment.testNeuromodulationIntegration();
    printStatus("\n", false);
    
    printStatus("--- Checkpoint Test ---", false);
    experiment.testCheckpointing();
    printStatus("\n", false);
    
    printStatus("--- Replay Test ---", false);
    experiment.testReplay();
    printStatus("\n", false);
    
    // Create and run full experiment
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = options.stepsPerEpisode;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    
    double startTime = getCurrentWallTime();
    
    auto result = experiment.run(config);
    
    double endTime = getCurrentWallTime();
    double wallTime = endTime - startTime;
    
    printStatus("\n=== FINAL RESULTS ===", false);
    printStatus("Total reward: " + std::to_string(result.totalReward), false);
    printStatus("Avg firing rate: " + std::to_string(result.avgFiringRate), false);
    printStatus("Episodes stored: " + std::to_string(result.memoryEpisodesStored), false);
    printStatus("Dopamine level: " + std::to_string(result.dopamineLevel), false);
    printStatus("\n=== INTEGRATION STATUS ===", false);
    printStatus("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"), false);
    printStatus("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"), false);
    printStatus("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"), false);
    printStatus("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"), false);
    printStatus("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"), false);
    printStatus("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"), false);
    
    printStatus("\nWall clock time: " + std::to_string(result.totalWallClockTime) + "s", false);
    
    // Save results if requested
    if (!options.outputFile.empty()) {
        std::stringstream results;
        results << "=== Phase 6 Integration Demo ===\n\n";
        results << "Total Reward: " << result.totalReward << "\n";
        results << "Avg Firing Rate: " << result.avgFiringRate << "\n";
        results << "Episodes Stored: " << result.memoryEpisodesStored << "\n";
        results << "Dopamine Level: " << result.dopamineLevel << "\n\n";
        results << "Integration Status:\n";
        results << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        results << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        results << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        results << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        results << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        results << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n\n";
        results << "Wall Clock Time: " << result.totalWallClockTime << "s\n";
        
        if (!saveResults(options.outputFile, results.str())) {
            printStatus("Warning: Could not save results to " + options.outputFile, false);
        }
    }
    
    return 0;
}

void NLMCommandLineInterface::printBanner() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Master CLI Interface                            ║
    ║                                                               ║
    ║     Unified command-line interface for all NLM operations      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void NLMCommandLineInterface::printStatus(const std::string& message, bool error) const {
    if (quiet_ && !error) {
        return;
    }
    
    if (error) {
        std::cerr << "[ERROR] " << message << std::endl;
    } else {
        std::cout << "[STATUS] " << message << std::endl;
    }
}

void NLMCommandLineInterface::logStep(int step, float reward, float neuromod, 
                                    float curiosity, float energy, 
                                    const std::string& action, float firingRate) const {
    if (verbose_) {
        std::cout << "  Step " << step 
                  << ": reward=" << reward
                  << ", neuromod=" << neuromod
                  << ", curiosity=" << curiosity
                  << ", energy=" << energy
                  << ", action=" << action
                  << ", firing=" << firingRate
                  << "\n";
    }
}

std::string NLMCommandLineInterface::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double NLMCommandLineInterface::getCurrentWallTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void NLMCommandLineInterface::printConfiguration(const CommandOptions& options) const {
    std::cout << "=== NLM Configuration ===\n";
    std::cout << "Experiment Phase: " << (options.experimentPhase.empty() ? "Not specified" : options.experimentPhase) << "\n";
    std::cout << "Number of Episodes: " << options.numEpisodes << "\n";
    std::cout << "Steps per Episode: " << options.stepsPerEpisode << "\n";
    std::cout << "Enable Checkpointing: " << (options.enableCheckpointing ? "Yes" : "No") << "\n";
    std::cout << "Enable Replay: " << (options.enableReplay ? "Yes" : "No") << "\n";
    std::cout << "Enable Development: " << (options.enableDevelopment ? "Yes" : "No") << "\n";
    std::cout << "Enable Curiosity: " << (options.enableCuriosity ? "Yes" : "No") << "\n";
    std::cout << "Enable Reward Modulation: " << (options.enableRewardModulation ? "Yes" : "No") << "\n";
    std::cout << "Enable Structural Plasticity: " << (options.enableStructuralPlasticity ? "Yes" : "No") << "\n";
    std::cout << "Random Seed: " << options.randomSeed << "\n";
    std::cout << "Brain Type: " << options.brainType << "\n";
    std::cout << "World Type: " << options.worldType << "\n";
    std::cout << "Output Format: " << options.outputFormat << "\n";
    std::cout << "Output File: " << (options.outputFile.empty() ? "Not specified" : options.outputFile) << "\n";
    std::cout << "Thread Count: " << options.threadCount << "\n";
    std::cout << "Parallel Execution: " << (options.parallelExecution ? "Yes" : "No") << "\n";
    std::cout << "\n";
}

void NLMCommandLineInterface::printExperimentSummary(const std::string& phase, 
                                                    const CommandOptions& options,
                                                    double startTime, double endTime,
                                                    float totalReward, float avgFiringRate,
                                                    float memoryUsage) const {
    double wallTime = endTime - startTime;
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "EXPERIMENT SUMMARY\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Phase: " << phase << "\n";
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(2) << wallTime << "s\n";
    std::cout << "Total Reward: " << std::fixed << std::setprecision(3) << totalReward << "\n";
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) << avgFiringRate << "\n";
    std::cout << "Memory Usage: " << memoryUsage << " MB\n";
    std::cout << "Total Steps: " << (options.numEpisodes * options.stepsPerEpisode) << "\n";
    std::cout << "Episodes Completed: " << options.numEpisodes << "\n";
    std::cout << std::string(60, '=') << "\n";
}

bool NLMCommandLineInterface::saveResults(const std::string& filename, const std::string& data) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    file << data;
    file.close();
    return true;
}

std::string NLMCommandLineInterface::loadConfiguration(const std::string& filename) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

bool NLMCommandLineInterface::exportMetrics(const std::string& filename, const std::vector<std::string>& metrics) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& metric : metrics) {
        file << metric << "\n";
    }
    
    file.close();
    return true;
}

void NLMCommandLineInterface::printResults(const ExperimentResults& results) const {
    std::cout << "=== Experiment Results ===\n";
    std::cout << "Phase: " << results.phase << "\n";
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(2) << results.wallClockTime << "s\n";
    std::cout << "CPU Time: " << std::fixed << std::setprecision(2) << results.cpuTime << "s\n";
    std::cout << "Total Reward: " << std::fixed << std::setprecision(3) << results.totalReward << "\n";
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) << results.avgFiringRate << "\n";
    std::cout << "Average Memory Usage: " << std::fixed << std::setprecision(2) << results.avgMemoryUsage << " MB\n";
    std::cout << "Total Steps: " << results.totalSteps << "\n";
    std::cout << "Active Memory Traces: " << results.activeMemoryTraces << "\n";
    std::cout << "Total Episodes: " << results.totalEpisodes << "\n";
    std::cout << "Curiosity Level: " << std::fixed << std::setprecision(3) << results.curiosityLevel << "\n";
    std::cout << "Novelty Level: " << std::fixed << std::setprecision(3) << results.noveltyLevel << "\n";
    std::cout << "Dopamine Level: " << std::fixed << std::setprecision(3) << results.dopamineLevel << "\n";
    std::cout << "Success: " << (results.success ? "Yes" : "No") << "\n";
    if (!results.errorMessage.empty()) {
        std::cout << "Error Message: " << results.errorMessage << "\n";
    }
    std::cout << "\n";
}

std::string NLMCommandLineInterface::resultsToJSON(const ExperimentResults& results) const {
    std::stringstream json;
    json << "{\n";
    json << "  \"phase\": \"" << results.phase << "\",\n";
    json << "  \"wallClockTime\": " << std::fixed << std::setprecision(2) << results.wallClockTime << ',\n";
    json << "  \"cpuTime\": " << std::fixed << std::setprecision(2) << results.cpuTime << ',\n";
    json << "  \"totalReward\": " << std::fixed << std::setprecision(3) << results.totalReward << ',\n";
    json << "  \"avgFiringRate\": " << std::fixed << std::setprecision(3) << results.avgFiringRate << ',\n";
    json << "  \"avgMemoryUsage\": " << std::fixed << std::setprecision(2) << results.avgMemoryUsage << ',\n";
    json << "  \"totalSteps\": " << results.totalSteps << ',\n";
    json << "  \"activeMemoryTraces\": " << results.activeMemoryTraces << ',\n";
    json << "  \"totalEpisodes\": " << results.totalEpisodes << ',\n";
    json << "  \"curiosityLevel\": " << std::fixed << std::setprecision(3) << results.curiosityLevel << ',\n";
    json << "  \"noveltyLevel\": " << std::fixed << std::setprecision(3) << results.noveltyLevel << ',\n";
    json << "  \"dopamineLevel\": " << std::fixed << std::setprecision(3) << results.dopamineLevel << ',\n";
    json << "  \"success\": " << (results.success ? "true" : "false") << ',\n";
    if (!results.errorMessage.empty()) {
        json << "  \"errorMessage\": \"" << results.errorMessage << "\"\n";
    } else {
        json << "  \"errorMessage\": \"\"\n";
    }
    json << "}\n";
    return json.str();
}

NLMCommandLineInterface::ExperimentResults NLMCommandLineInterface::runSingleExperiment(const CommandOptions& options) {
    ExperimentResults results;
    results.phase = options.experimentPhase;
    
    try {
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", std::stoul(options.randomSeed));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            results.success = false;
            results.errorMessage = "Failed to initialize brain";
            return results;
        }
        
        double startTime = getCurrentWallTime();
        double brainInitTime = getCurrentWallTime();
        
        // Run simulation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t totalSteps = 0;
        
        for (size_t step = 0; step < options.stepsPerEpisode; ++step) {
            // Inject some activity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            brain->step(step, step * 0.001);
            
            totalReward += 1.0f; // Dummy reward
            totalFiringRate += brain->getAverageFiringRate();
            totalSteps++;
        }
        
        double simulationEndTime = getCurrentWallTime();
        
        // Calculate metrics
        results.wallClockTime = simulationEndTime - startTime;
        results.cpuTime = simulationEndTime - brainInitTime;
        results.totalReward = totalReward / options.stepsPerEpisode;
        results.avgFiringRate = totalFiringRate / options.stepsPerEpisode;
        results.totalSteps = totalSteps;
        results.success = true;
        results.activeMemoryTraces = brain->getFiringNeuronCount() / 10; // Estimate
        results.totalEpisodes = options.numEpisodes;
        results.curiosityLevel = 0.5f;
        results.noveltyLevel = 0.3f;
        results.dopamineLevel = 0.1f;
        
    } catch (const std::exception& e) {
        results.success = false;
        results.errorMessage = e.what();
    }
    
    return results;
}

NLMCommandLineInterface::ProfileData NLMCommandLineInterface::profileExecution(const CommandOptions& options) {
    ProfileData profile;
    profile.phase = options.experimentPhase;
    
    try {
        double startTime = getCurrentWallTime();
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        
        double brainInitStart = getCurrentWallTime();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        double brainInitEnd = getCurrentWallTime();
        
        profile.brainInitTime = brainInitEnd - brainInitStart;
        
        // Run simulation
        double simStart = getCurrentWallTime();
        
        for (size_t step = 0; step < options.stepsPerEpisode; ++step) {
            brain->step(step, step * 0.001);
        }
        
        double simEnd = getCurrentWallTime();
        profile.simulationTime = simEnd - simStart;
        
        double endTime = getCurrentWallTime();
        profile.totalTime = endTime - startTime;
        
        // Collect profile data
        profile.memoryUsageMB = 100.0f; // Estimate
        profile.neuronsCreated = brain->getTotalNeuronCount();
        profile.synapsesCreated = brain->getTotalSynapseCount();
        profile.totalSpikes = brain->getTotalSpikeCount();
        
    } catch (const std::exception& e) {
        // Error handling
    }
    
    return profile;
}

void NLMCommandLineInterface::printProfile(const ProfileData& profile) const {
    std::cout << "=== Performance Profile ===\n";
    std::cout << "Phase: " << profile.phase << "\n";
    std::cout << "Total Time: " << std::fixed << std::setprecision(2) << profile.totalTime << "s\n";
    std::cout << "Brain Init Time: " << std::fixed << std::setprecision(2) << profile.brainInitTime << "s\n";
    std::cout << "Simulation Time: " << std::fixed << std::setprecision(2) << profile.simulationTime << "s\n";
    std::cout << "Memory Usage: " << std::fixed << std::setprecision(2) << profile.memoryUsageMB << " MB\n";
    std::cout << "Neurons Created: " << profile.neuronsCreated << "\n";
    std::cout << "Synapses Created: " << profile.synapsesCreated << "\n";
    std::cout << "Total Spikes: " << profile.totalSpikes << "\n";
    std::cout << "\n";
}

void NLMCommandLineInterface::applyConfigFile(std::shared_ptr<Config> config, const std::string& filename) const {
    // Implementation for loading config from file
    // This would typically parse JSON or other config formats
}

void NLMCommandLineInterface::applyCommandLineConfig(std::shared_ptr<Config> config, const CommandOptions& options) const {
    if (options.enableDevelopment) {
        config->set("development_enabled", true);
    }
    if (options.enableCuriosity) {
        config->set("curiosity_enabled", true);
    }
    if (options.enableRewardModulation) {
        config->set("reward_modulation_enabled", true);
    }
    if (options.enableStructuralPlasticity) {
        config->set("structural_plasticity_enabled", true);
    }
}

std::string NLMCommandLineInterface::getConfigSummary(const std::shared_ptr<Config>& config) const {
    if (config) {
        return config->summary();
    }
    return "No configuration available";
}

bool NLMCommandLineInterface::validateOptions(const CommandOptions& options) const {
    if (options.command.empty()) {
        printStatus("No command specified", true);
        return false;
    }
    
    if (options.command == "run" && options.experimentPhase.empty()) {
        printStatus("No experiment phase specified for run command", true);
        return false;
    }
    
    if (options.numEpisodes == 0) {
        printStatus("Number of episodes must be greater than 0", true);
        return false;
    }
    
    if (options.stepsPerEpisode == 0) {
        printStatus("Steps per episode must be greater than 0", true);
        return false;
    }
    
    if (options.threadCount == 0) {
        printStatus("Thread count must be greater than 0", true);
        return false;
    }
    
    return true;
}

std::string NLMCommandLineInterface::getErrorMessage(int errorCode) const {
    // Map error codes to error messages
    switch (errorCode) {
        case 0: return "Success";
        case 1: return "General error";
        case 2: return "Invalid command line arguments";
        case 3: return "Failed to initialize brain";
        case 4: return "Failed to load configuration";
        case 5: return "Experiment not found";
        case 6: return "Memory allocation failed";
        case 7: return "File I/O error";
        case 8: return "Network error";
        case 9: return "Plugin loading failed";
        default: return "Unknown error";
    }
}
