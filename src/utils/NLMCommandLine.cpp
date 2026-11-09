NLMCommandLine.cpp - Implementation of advanced NLM command-line tools

#include "NLMCommandLine.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <filesystem>

namespace nlm {

NLMCommandLine::NLMCommandLine() : defaultConfig(std::make_shared<Config>()) {}

NLMCommandLine::~NLMCommandLine() = default;

bool NLMCommandLine::parseArguments(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return false;
    }
    
    CommandOptions options;
    options.verbose = false;
    options.quiet = false;
    options.outputFormat = OutputFormat::PLAIN_TEXT;
    options.neuronCount = 500;
    options.regionCount = 1;
    options.steps = 1000;
    options.timestep = 0.001;
    options.enableFeatures = false;
    
    // Simple argument parsing for demonstration
    std::string command = argv[1];
    options.command = parseCommand(command);
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--json") {
            options.outputFormat = OutputFormat::JSON;
        } else if (arg == "--steps") {
            if (i + 1 < argc) options.steps = std::stoul(argv[++i]);
        } else if (arg == "--neurons") {
            if (i + 1 < argc) options.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--regions") {
            if (i + 1 < argc) options.regionCount = std::stoul(argv[++i]);
        } else if (arg == "--output") {
            if (i + 1 < argc) options.outputFile = argv[++i];
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) options.checkpointPath = argv[++i];
        } else if (arg == "--create") {
            if (i + 1 < argc) options.brainName = argv[++i];
            options.command = Command::CREATE_BRAIN;
        } else if (arg == "--run") {
            options.command = Command::RUN_BRAIN;
        } else if (arg == "--analyze") {
            options.command = Command::ANALYZE_BRAIN;
        }
    }
    
    // Execute the command
    return executeCommand(options);
}

void NLMCommandLine::runCommand() {
    printHelp();
}

NLMCommandLine::Command NLMCommandLine::parseCommand(const std::string& cmd) {
    std::string lowerCmd = cmd;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
    
    if (lowerCmd == "help" || lowerCmd == "--help") return Command::HELP;
    if (lowerCmd == "create" || lowerCmd == "create-brain") return Command::CREATE_BRAIN;
    if (lowerCmd == "initialize" || lowerCmd == "init") return Command::INITIALIZE_BRAIN;
    if (lowerCmd == "run" || lowerCmd == "simulate") return Command::RUN_BRAIN;
    if (lowerCmd == "save") return Command::SAVE_BRAIN;
    if (lowerCmd == "load") return Command::LOAD_BRAIN;
    if (lowerCmd == "analyze") return Command::ANALYZE_BRAIN;
    if (lowerCmd == "benchmark") return Command::BENCHMARK;
    if (lowerCmd == "config") return Command::CONFIG_EDITOR;
    
    return Command::HELP;
}

NLMCommandLine::OutputFormat NLMCommandLine::parseOutputFormat(const std::string& format) {
    std::string lowerFormat = format;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);
    
    if (lowerFormat == "json") return OutputFormat::JSON;
    if (lowerFormat == "yaml" || lowerFormat == "yml") return OutputFormat::YAML;
    
    return OutputFormat::PLAIN_TEXT;
}

bool NLMCommandLine::executeCommand(const CommandOptions& options) {
    switch (options.command) {
        case Command::CREATE_BRAIN:
            return createBrainWithFeatures(options.brainName, defaultConfig, "");
        case Command::INITIALIZE_BRAIN:
            return initializeBrainWithLogging(currentBrain, options.steps);
        case Command::RUN_BRAIN:
            return runBrainWithAnalysis(currentBrain, currentWorld, options);
        case Command::SAVE_BRAIN:
            if (currentBrain) {
                return currentBrain->save(options.checkpointPath.empty() ? "./brain.bin" : options.checkpointPath);
            }
            break;
        case Command::LOAD_BRAIN:
            // Implementation for loading brain
            break;
        case Command::ANALYZE_BRAIN:
            if (currentBrain) {
                analyzeBrainPerformance(*currentBrain, options.steps);
            }
            break;
        case Command::BENCHMARK:
            return benchmarkBrain(options.brainName, options);
        case Command::CONFIG_EDITOR:
            // Implementation for config editing
            break;
        case Command::HELP:
        default:
            printHelp();
            break;
    }
    return false;
}

void NLMCommandLine::printHelp() const {
    std::cout << "=== NLM Advanced Command Line Tool ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  create <name>          Create a new brain with given name" << std::endl;
    std::cout << "  initialize <steps>      Initialize and run brain for specified steps" << std::endl;
    std::cout << "  run <steps>             Run an existing brain for specified steps" << std::endl;
    std::cout << "  save <path>             Save brain state to file" << std::endl;
    std::cout << "  load <path>             Load brain state from file" << std::endl;
    std::cout << "  analyze <steps>          Analyze brain performance for specified steps" << std::endl;
    std::cout << "  benchmark <name>        Run benchmark test on brain" << std::endl;
    std::cout << "  config                   Open configuration editor" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --verbose            Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet              Suppress non-essential output" << std::endl;
    std::cout << "  --json                   Output in JSON format" << std::endl;
    std::cout << "  --steps <n>              Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --neurons <n>            Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions <n>            Number of regions (default: 1)" << std::endl;
    std::cout << "  --output <file>          Output file path" << std::endl;
    std::cout << "  --checkpoint <path>      Checkpoint file path" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm create mybrain" << std::endl;
    std::cout << "  ./nlm initialize 1000 --neurons 1000 --verbose" << std::endl;
    std::cout << "  ./nlm run 500 --steps 5000" << std::endl;
    std::cout << "  ./nlm save ./mybrain.bin" << std::endl;
    std::cout << "  ./nlm load ./mybrain.bin" << std::endl;
    std::cout << "  ./nlm analyze 100 --json --output stats.json" << std::endl;
    std::cout << "" << std::endl;
}

void NLMCommandLine::printBrainInfo(const Brain& brain) const {
    std::cout << "Brain Information:" << std::endl;
    std::cout << "  Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons (this step): " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Excitation/Inhibition ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    std::cout << "  Pending spike events: " << brain.getPendingSpikeEventCount() << std::endl;
}

void NLMCommandLine::analyzeBrainPerformance(const Brain& brain, uint64_t steps) const {
    std::cout << "Analyzing brain performance for " << steps << " steps..." << std::endl;
    
    // Simulate brain for specified steps and collect statistics
    uint64_t totalSpikes = 0;
    uint64_t totalActive = 0;
    uint64_t totalFiring = 0;
    float totalFiringRate = 0.0f;
    
    for (uint64_t step = 0; step < steps; ++step) {
        brain.step(step);
        totalSpikes += brain.getTotalSpikeCount();
        totalActive += brain.getActiveNeuronCount();
        totalFiring += brain.getFiringNeuronCount();
        totalFiringRate += brain.getAverageFiringRate();
    }
    
    std::cout << "Performance Analysis:" << std::endl;
    std::cout << "  Average total spikes per step: " << (totalSpikes / (steps > 0 ? steps : 1)) << std::endl;
    std::cout << "  Average active neurons per step: " << (totalActive / (steps > 0 ? steps : 1)) << std::endl;
    std::cout << "  Average firing neurons per step: " << (totalFiring / (steps > 0 ? steps : 1)) << std::endl;
    std::cout << "  Average firing rate: " << (totalFiringRate / (steps > 0 ? steps : 1)) << " Hz" << std::endl;
    std::cout << "  Activity consistency: " << std::fixed << std::setprecision(2) << 
        (static_cast<float>(totalFiring) / (steps * brain.getTotalNeuronCount()) * 100.0f) << "%" << std::endl;
}

bool NLMCommandLine::benchmarkBrain(const std::string& brainName, const CommandOptions& options) {
    std::cout << "Running benchmark for brain: " << brainName << std::endl;
    
    // Create a benchmark brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount));
    config->set("region_count", static_cast<int64_t>(options.regionCount));
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return false;
    }
    
    // Run benchmark test
    auto startTime = std::chrono::high_resolution_clock::now();
    uint64_t totalSteps = options.steps;
    
    for (uint64_t step = 0; step < totalSteps; ++step) {
        brain->step(step);
        
        // Progress reporting
        if (!options.quiet && (step % 100 == 0)) {
            std::cout << "Step " << step << "/" << totalSteps 
                      << " | Total spikes: " << brain->getTotalSpikeCount()
                      << " | Active: " << brain->getActiveNeuronCount()
                      << " | Firing: " << brain->getFiringNeuronCount()
                      << " | Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\nBenchmark Results:" << std::endl;
    std::cout << "  Brain: " << brainName << std::endl;
    std::cout << "  Steps: " << totalSteps << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average spikes per step: " << (brain->getTotalSpikeCount() / (totalSteps > 0 ? totalSteps : 1)) << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Execution time: " << duration.count() << " seconds" << std::endl;
    std::cout << "  Steps per second: " << (totalSteps / (duration.count() > 0 ? duration.count() : 1)) << std::endl;
    
    // Save benchmark results
    if (!options.outputFile.empty()) {
        std::ofstream outputFile(options.outputFile);
        if (outputFile.is_open()) {
            outputFile << "Benchmark Results:" << std::endl;
            outputFile << "  Brain: " << brainName << std::endl;
            outputFile << "  Steps: " << totalSteps << std::endl;
            outputFile << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            outputFile << "  Average spikes per step: " << (brain->getTotalSpikeCount() / (totalSteps > 0 ? totalSteps : 1)) << std::endl;
            outputFile << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            outputFile << "  Execution time: " << duration.count() << " seconds" << std::endl;
            outputFile << "  Steps per second: " << (totalSteps / (duration.count() > 0 ? duration.count() : 1)) << std::endl;
        }
    }
    
    return true;
}

bool NLMCommandLine::createBrainWithFeatures(const std::string& name, const std::shared_ptr<Config>& config, const std::string& features) {
    std::cout << "Creating brain: " << name << std::endl;
    
    // Parse features
    bool enableRewardModulation = features.find("reward") != std::string::npos;
    bool enableStructuralPlasticity = features.find("plasticity") != std::string::npos;
    bool enableDevelopment = features.find("development") != std::string::npos;
    bool enableCuriosity = features.find("curiosity") != std::string::npos;
    
    // Create world and agent
    currentWorld = std::make_shared<SimpleWorld>();
    currentWorld->configure(16, 16, 16, 16);
    currentWorld->reset();
    
    currentAgent = std::make_shared<AgentBrain>(std::make_shared<Brain>(config));
    currentAgent->initialize(*currentWorld);
    
    if (enableRewardModulation) currentAgent->enableRewardModulation(true);
    if (enableStructuralPlasticity) currentAgent->enableStructuralPlasticity(true);
    if (enableDevelopment) currentAgent->enableDevelopment(true);
    if (enableCuriosity) currentAgent->enableCuriosity(true);
    
    std::cout << "Brain created successfully with features:" << std::endl;
    std::cout << "  Reward modulation: " << (enableRewardModulation ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Structural plasticity: " << (enableStructuralPlasticity ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Curiosity: " << (enableCuriosity ? "ENABLED" : "DISABLED") << std::endl;
    
    return true;
}

bool NLMCommandLine::initializeBrainWithLogging(const std::shared_ptr<Brain>& brain, uint64_t steps) {
    if (!brain) {
        std::cerr << "No brain to initialize" << std::endl;
        return false;
    }
    
    std::cout << "Initializing brain and running " << steps << " steps..." << std::endl;
    
    // Log initial status
    brain->logStatus();
    
    // Run simulation
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step);
        
        if (!steps) continue; // Avoid division by zero
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << "/" << steps 
                      << " | Total spikes: " << brain->getTotalSpikeCount()
                      << " | Active: " << brain->getActiveNeuronCount()
                      << " | Firing: " << brain->getFiringNeuronCount()
                      << " | Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    // Log final status
    std::cout << "Initialization complete:" << std::endl;
    brain->logStatus();
    
    return true;
}

bool NLMCommandLine::runBrainWithAnalysis(const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world, const CommandOptions& options) {
    if (!brain || !world) {
        std::cerr << "Brain or world not initialized" << std::endl;
        return false;
    }
    
    std::cout << "Running brain simulation for " << options.steps << " steps..." << std::endl;
    
    // Create agent if not already created
    if (!currentAgent) {
        currentAgent = std::make_shared<AgentBrain>(brain);
        currentAgent->initialize(*world);
    }
    
    // Run simulation with world updates
    for (uint64_t step = 0; step < options.steps; ++step) {
        // Update world
        world->update(0.001);
        
        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process sensory input
        currentAgent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = currentAgent->decodeMotorCommand();
        
        // Apply action to world
        ActionResult result = world->applyMotorCommand(cmd, step * 0.001);
        
        // Apply reward modulation
        currentAgent->applyRewardModulation(result.reward, 0.0f);
        
        // Update development
        if (options.enableFeatures) {
            currentAgent->updateDevelopment(0.001);
        }
        
        // Progress reporting
        if (!options.quiet && (step % 500 == 0)) {
            std::cout << "Step " << step << "/" << options.steps 
                      << " | Reward: " << result.reward
                      << " | Firing: " << brain->getFiringNeuronCount()
                      << " | Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    std::cout << "Simulation complete:" << std::endl;
    printBrainInfo(*brain);
    
    return true;
}

} // namespace nlm