#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <cmath>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Logger/ConsoleLogger.hpp"
#include "../src/core/Logger/FileLogger.hpp"
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/core/Types/Types.hpp"

using namespace nlm;

// Forward declarations for test functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

// Configuration management
std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic neural parameters
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("working_memory_capacity", 100, ConfigSource::Default);
    config->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_baseline", 0.5f, ConfigSource::Default);
    config->set("dopamine_max", 2.0f, ConfigSource::Default);
    
    // Development parameters
    config->set("development_enabled", true, ConfigSource::Default);
    config->set("plasticity_modulation_enabled", true, ConfigSource::Default);
    
    return config;
}

// Print help information
void printHelp(const std::string& programName) {
    std::cout << "NLM - Neural Learning Machine - Command Line Interface" << std::endl;
    std::cout << "Version 0.1.0 (Phase 6)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: " << programName << " <command> [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Commands:" << std::endl;
    std::cout << "  brain          Brain management commands" << std::endl;
    std::cout << "  experiment     Run experiments and simulations" << std::endl;
    std::cout << "  config         Configuration management" << std::endl;
    std::cout << "  log            Logging control" << std::endl;
    std::cout << "  checkpoint     Checkpoint save/load operations" << std::endl;
    std::cout << "  explore        Explore brain state and structure" << std::endl;
    std::cout << "  batch          Run batch operations" << std::endl;
    std::cout << "  export         Export brain data to various formats" << std::endl;
    std::cout << "  help           Show this help message" << std::endl;
    std::cout << "  version        Show version information" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Commands:" << std::endl;
    std::cout << "  create         Create a new brain instance" << std::endl;
    std::cout << "  initialize     Initialize the brain with configuration" << std::endl;
    std::cout << "  step <N>       Run N simulation steps" << std::endl;
    std::cout << "  reset          Reset the brain to initial state" << std::endl;
    std::cout << "  status         Display brain status" << std::endl;
    std::cout << "  inject         Inject current into neurons" << std::endl;
    std::cout << "  save <file>    Save brain state to checkpoint" << std::endl;
    std::cout << "  load <file>    Load brain state from checkpoint" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Commands:" << std::endl;
    std::cout << "  demo           Run Phase 6 integration demo" << std::endl;
    std::cout << "  test <name>    Run specific test suite" << std::endl;
    std::cout << "  explore        Run exploration experiment" << std::endl;
    std::cout << "  lifelong       Run lifelong learning experiment" << std::endl;
    std::cout << "  integrate      Run integration verification" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " brain create" << std::endl;
    std::cout << "  " << programName << " brain initialize" << std::endl;
    std::cout << "  " << programName << " brain step 1000" << std::endl;
    std::cout << "  " << programName << " experiment demo" << std::endl;
    std::cout << "  " << programName << " config load settings.json" << std::endl;
    std::cout << "  " << programName << " brain save brain_state.bin" << std::endl;
    std::cout << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0 (Phase 6)" << std::endl;
    std::cout << "Complete artificial developmental brain system" << std::endl;
    std::cout << "Copyright 2026 NLM Project" << std::endl;
    std::cout << std::endl;
}

// Handle brain commands
void handleBrainCommand(std::vector<std::string>& args, std::shared_ptr<Config> config) {
    if (args.empty()) {
        std::cout << "Brain commands:" << std::endl;
        std::cout << "  create         Create a new brain instance" << std::endl;
        std::cout << "  initialize     Initialize the brain with configuration" << std::endl;
        std::cout << "  step <N>       Run N simulation steps" << std::endl;
        std::cout << "  reset          Reset the brain to initial state" << std::endl;
        std::cout << "  status         Display brain status" << std::endl;
        std::cout << "  inject         Inject current into neurons" << std::endl;
        std::cout << "  save <file>    Save brain state to checkpoint" << std::endl;
        std::cout << "  load <file>    Load brain state from checkpoint" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "create") {
        std::cout << "Creating new brain with Phase 6 architecture..." << std::endl;
        // Create brain with current config
        auto brain = std::make_shared<Brain>(config);
        std::cout << "Brain created successfully." << std::endl;
    }
    else if (command == "initialize") {
        std::cout << "Initializing brain..." << std::endl;
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            std::cout << "Brain initialized successfully." << std::endl;
            brain->logStatus();
        } else {
            std::cout << "Failed to initialize brain." << std::endl;
        }
    }
    else if (command == "step") {
        if (args.size() < 2) {
            std::cout << "Error: Step count required." << std::endl;
            return;
        }
        int steps = std::stoi(args[1]);
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Completed " << steps << " steps in " << duration.count() << " ms." << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    }
    else if (command == "reset") {
        std::cout << "Resetting brain..." << std::endl;
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        brain->reset();
        std::cout << "Brain reset complete." << std::endl;
    }
    else if (command == "status") {
        std::cout << "Brain Status:" << std::endl;
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        brain->logStatus();
    }
    else if (command == "inject") {
        std::cout << "Injecting current into neurons..." << std::endl;
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Inject current into sensory neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        brain->step(0);
        
        std::cout << "Current injection complete." << std::endl;
    }
    else if (command == "save") {
        if (args.size() < 2) {
            std::cout << "Error: Output file required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Saving brain state to " << filepath << "..." << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (brain->save(filepath)) {
            std::cout << "Brain state saved successfully." << std::endl;
        } else {
            std::cout << "Failed to save brain state." << std::endl;
        }
    }
    else if (command == "load") {
        if (args.size() < 2) {
            std::cout << "Error: Input file required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Loading brain state from " << filepath << "..." << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->load(filepath)) {
            std::cout << "Brain state loaded successfully." << std::endl;
            brain->logStatus();
        } else {
            std::cout << "Failed to load brain state." << std::endl;
        }
    }
    else {
        std::cout << "Unknown brain command: " << command << std::endl;
    }
}

// Handle experiment commands
void handleExperimentCommand(std::vector<std::string>& args, std::shared_ptr<Config> config) {
    if (args.empty()) {
        std::cout << "Experiment commands:" << std::endl;
        std::cout << "  demo           Run Phase 6 integration demo" << std::endl;
        std::cout << "  test <name>    Run specific test suite" << std::endl;
        std::cout << "  explore        Run exploration experiment" << std::endl;
        std::cout << "  lifelong       Run lifelong learning experiment" << std::endl;
        std::cout << "  integrate      Run integration verification" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "demo") {
        std::cout << "=== Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
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
        
        // Run full experiment with demo settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config phase6Config;
        phase6Config.neuronCount = 500;
        phase6Config.maxSteps = 2000;
        phase6Config.enableCheckpointing = true;
        phase6Config.enableReplay = true;
        phase6Config.enableDevelopment = true;
        
        auto result = experiment.run(phase6Config);
        
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
    }
    else if (command == "test") {
        if (args.size() < 2) {
            std::cout << "Error: Test name required." << std::endl;
            return;
        }
        std::string testName = args[1];
        std::cout << "Running test: " << testName << std::endl;
        
        if (testName == "connectivity") {
            // Run basic connectivity test
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            runBasicConnectivityTest(brain);
        }
        else if (testName == "plasticity") {
            // Run plasticity experiment
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            runPlasticityExperiment(brain);
        }
        else if (testName == "stdp") {
            // Run STDP verification
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            runStdpVerification(brain);
        }
        else {
            std::cout << "Unknown test: " << testName << std::endl;
        }
    }
    else if (command == "explore") {
        std::cout << "=== Exploration Experiment ===" << std::endl;
        std::cout << "Exploring brain capabilities..." << std::endl;
        
        // Create brain for exploration
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run exploration with different parameters
        for (int steps = 100; steps <= 1000; steps += 100) {
            brain->reset();
            brain->initialize();
            
            for (int i = 0; i < steps; ++i) {
                // Add some sensory input
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                brain->step(i);
            }
            
            std::cout << "Step " << steps << ": " 
                      << "Spikes: " << brain->getTotalSpikeCount() << ", "
                      << "Active: " << brain->getActiveNeuronCount() << " neurons" << std::endl;
        }
        
        std::cout << "Exploration complete." << std::endl;
    }
    else if (command == "lifelong") {
        std::cout << "=== Lifelong Learning Experiment ===" << std::endl;
        std::cout << "Running lifelong learning simulation..." << std::endl;
        
        // This would implement a lifelong learning experiment
        // For now, just run the Phase 6 integration demo
        Phase6IntegratedExperiment experiment;
        Phase6Config phase6Config;
        phase6Config.maxSteps = 5000;
        phase6Config.enableDevelopment = true;
        
        auto result = experiment.run(phase6Config);
        
        std::cout << "Lifelong learning simulation completed." << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
    }
    else if (command == "integrate") {
        std::cout << "=== Integration Verification ===" << std::endl;
        
        Phase6IntegratedExperiment experiment;
        bool integrationOK = experiment.verifyIntegration();
        
        if (integrationOK) {
            std::cout << "Integration verification PASSED!" << std::endl;
            std::cout << "All brain systems are properly connected." << std::endl;
        } else {
            std::cout << "Integration verification FAILED!" << std::endl;
            std::cout << "Some systems may not be properly connected." << std::endl;
        }
    }
    else {
        std::cout << "Unknown experiment command: " << command << std::endl;
    }
}

// Handle configuration commands
void handleConfigCommand(std::vector<std::string>& args, std::shared_ptr<Config> config) {
    if (args.empty()) {
        std::cout << "Configuration commands:" << std::endl;
        std::cout << "  load <file>    Load configuration from file" << std::endl;
        std::cout << "  save <file>    Save configuration to file" << std::endl;
        std::cout << "  show           Display current configuration" << std::endl;
        std::cout << "  set <key> <value> Set configuration value" << std::endl;
        std::cout << "  reset          Reset to default configuration" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "load") {
        if (args.size() < 2) {
            std::cout << "Error: File path required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Loading configuration from " << filepath << "..." << std::endl;
        
        if (config->loadFromFile(filepath)) {
            std::cout << "Configuration loaded successfully." << std::endl;
        } else {
            std::cout << "Failed to load configuration." << std::endl;
        }
    }
    else if (command == "save") {
        if (args.size() < 2) {
            std::cout << "Error: File path required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Saving configuration to " << filepath << "..." << std::endl;
        
        if (config->saveToFile(filepath)) {
            std::cout << "Configuration saved successfully." << std::endl;
        } else {
            std::cout << "Failed to save configuration." << std::endl;
        }
    }
    else if (command == "show") {
        std::cout << "Current Configuration:" << std::endl;
        std::cout << config->summary() << std::endl;
    }
    else if (command == "set") {
        if (args.size() < 3) {
            std::cout << "Error: Key and value required." << std::endl;
            return;
        }
        std::string key = args[1];
        std::string value = args[2];
        std::cout << "Setting " << key << " = " << value << std::endl;
        
        // Try to parse value as different types
        bool success = false;
        
        // Try integer
        try {
            int64_t intValue = std::stoll(value);
            config->set(key, intValue, ConfigSource::User);
            success = true;
        } catch (...) {}
        
        // Try float
        if (!success) {
            try {
                float floatValue = std::stof(value);
                config->set(key, floatValue, ConfigSource::User);
                success = true;
            } catch (...) {}
        }
        
        // Try string
        if (!success) {
            config->set(key, value, ConfigSource::User);
            success = true;
        }
        
        if (success) {
            std::cout << "Configuration updated successfully." << std::endl;
        } else {
            std::cout << "Failed to set configuration value." << std::endl;
        }
    }
    else if (command == "reset") {
        std::cout << "Resetting configuration to defaults..." << std::endl;
        *config = *createDefaultConfig();
        std::cout << "Configuration reset successfully." << std::endl;
    }
    else {
        std::cout << "Unknown config command: " << command << std::endl;
    }
}

// Handle logging commands
void handleLogCommand(std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Logging commands:" << std::endl;
        std::cout << "  level <level>  Set log level (trace, debug, info, warn, error, critical)" << std::endl;
        std::cout << "  file <path>    Add file logger" << std::endl;
        std::cout << "  status         Show logging status" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "level") {
        if (args.size() < 2) {
            std::cout << "Error: Log level required." << std::endl;
            return;
        }
        std::string levelStr = args[1];
        LogLevel level;
        
        if (levelStr == "trace") level = LogLevel::Trace;
        else if (levelStr == "debug") level = LogLevel::Debug;
        else if (levelStr == "info") level = LogLevel::Info;
        else if (levelStr == "warn") level = LogLevel::Warning;
        else if (levelStr == "error") level = LogLevel::Error;
        else if (levelStr == "critical") level = LogLevel::Critical;
        else {
            std::cout << "Unknown log level: " << levelStr << std::endl;
            return;
        }
        
        Logger::getInstance().setLevel(level);
        std::cout << "Log level set to: " << levelStr << std::endl;
    }
    else if (command == "file") {
        if (args.size() < 2) {
            std::cout << "Error: File path required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        auto fileLogger = std::make_shared<FileLogger>(filepath);
        Logger::getInstance().addLogger(fileLogger);
        std::cout << "Added file logger: " << filepath << std::endl;
    }
    else if (command == "status") {
        std::cout << "Logging status:" << std::endl;
        std::cout << "Global log level: " << static_cast<int>(Logger::getInstance().getLevel()) << std::endl;
        std::cout << "Number of loggers: " << Logger::getInstance().getLoggerCount() << std::endl;
    }
    else {
        std::cout << "Unknown log command: " << command << std::endl;
    }
}

// Handle checkpoint commands
void handleCheckpointCommand(std::vector<std::string>& args, std::shared_ptr<Brain> brain) {
    if (args.empty()) {
        std::cout << "Checkpoint commands:" << std::endl;
        std::cout << "  save <file>    Save brain state" << std::endl;
        std::cout << "  load <file>    Load brain state" << std::endl;
        std::cout << "  list           List available checkpoints" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "save") {
        if (args.size() < 2) {
            std::cout << "Error: File path required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Saving brain state to " << filepath << "..." << std::endl;
        
        if (brain->save(filepath)) {
            std::cout << "Brain state saved successfully." << std::endl;
        } else {
            std::cout << "Failed to save brain state." << std::endl;
        }
    }
    else if (command == "load") {
        if (args.size() < 2) {
            std::cout << "Error: File path required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Loading brain state from " << filepath << "..." << std::endl;
        
        if (brain->load(filepath)) {
            std::cout << "Brain state loaded successfully." << std::endl;
            brain->logStatus();
        } else {
            std::cout << "Failed to load brain state." << std::endl;
        }
    }
    else if (command == "list") {
        std::cout << "Available checkpoints:" << std::endl;
        // List files in current directory that look like checkpoints
        std::cout << "  (Checkpoint listing requires implementation)" << std::endl;
    }
    else {
        std::cout << "Unknown checkpoint command: " << command << std::endl;
    }
}

// Handle explore commands
void handleExploreCommand(std::vector<std::string>& args, std::shared_ptr<Brain> brain) {
    if (args.empty()) {
        std::cout << "Explore commands:" << std::endl;
        std::cout << "  brain          Explore brain structure" << std::endl;
        std::cout << "  neurons        Explore neurons" << std::endl;
        std::cout << "  synapses       Explore synapses" << std::endl;
        std::cout << "  memory         Explore memory systems" << std::endl;
        std::cout << "  neuromodulation Explore neuromodulation" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "brain") {
        std::cout << "=== Brain Structure ===" << std::endl;
        brain->logStatus();
    }
    else if (command == "neurons") {
        std::cout << "=== Neuron Analysis ===" << std::endl;
        size_t totalNeurons = brain->getTotalNeuronCount();
        std::cout << "Total neurons: " << totalNeurons << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    else if (command == "synapses") {
        std::cout << "=== Synapse Analysis ===" << std::endl;
        size_t totalSynapses = brain->getTotalSynapseCount();
        std::cout << "Total synapses: " << totalSynapses << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    else if (command == "memory") {
        std::cout << "=== Memory Systems ===" << std::endl;
        if (brain->getWorkingMemory()) {
            std::cout << "Working Memory: Active" << std::endl;
            // Additional memory system exploration would go here
        } else {
            std::cout << "Working Memory: Inactive" << std::endl;
        }
    }
    else if (command == "neuromodulation") {
        std::cout << "=== Neuromodulation Systems ===" << std::endl;
        // Neuromodulation system exploration would go here
        std::cout << "(Neuromodulation exploration requires implementation)" << std::endl;
    }
    else {
        std::cout << "Unknown explore command: " << command << std::endl;
    }
}

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
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

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
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
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
    NLM_LOG_INFO("");
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
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
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
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
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
    
    if (command == "simulate") {
        if (args.size() < 2) {
            std::cout << "Error: Step count required." << std::endl;
            return;
        }
        int steps = std::stoi(args[1]);
        std::cout << "Running " << steps << " simulation steps in batch mode..." << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < steps; ++i) {
            // Add random sensory input
            if (i % 10 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
            brain->step(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Batch simulation complete." << std::endl;
        std::cout << "Steps: " << steps << ", Duration: " << duration.count() << " ms" << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    }
    else if (command == "checkpoint") {
        std::cout << "Running checkpoint batch operation..." << std::endl;
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run some steps then checkpoint
        for (int i = 0; i < 500; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(i);
        }
        
        if (brain->save("./checkpoint_batch.bin")) {
            std::cout << "Checkpoint saved: checkpoint_batch.bin" << std::endl;
        }
    }
    else if (command == "train") {
        if (args.size() < 2) {
            std::cout << "Error: Epoch count required." << std::endl;
            return;
        }
        int epochs = std::stoi(args[1]);
        std::cout << "Training for " << epochs << " epochs..." << std::endl;
        
        // Simple training simulation
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int epoch = 0; epoch < epochs; ++epoch) {
            brain->reset();
            brain->initialize();
            
            // Simulate learning
            for (int step = 0; step < 1000; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step);
            }
            
            if ((epoch + 1) % 10 == 0) {
                std::cout << "Epoch " << (epoch + 1) << ", Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        
        std::cout << "Training complete after " << epochs << " epochs." << std::endl;
    }
    else if (command == "test") {
        if (args.size() < 2) {
            std::cout << "Error: Step count required." << std::endl;
            return;
        }
        int steps = std::stoi(args[1]);
        std::cout << "Running test with " << steps << " steps..." << std::endl;
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run basic test
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        std::cout << "Test complete. Results:" << std::endl;
        std::cout << "Steps: " << steps << ", Spikes: " << brain->getTotalSpikeCount() << std::endl;
    }
    else {
        std::cout << "Unknown batch command: " << command << std::endl;
    }
}

// Handle export commands
void handleExportCommand(std::vector<std::string>& args, std::shared_ptr<Brain> brain) {
    if (args.empty()) {
        std::cout << "Export commands:" << std::endl;
        std::cout << "  neurons <file>   Export neuron data" << std::endl;
        std::cout << "  synapses <file>  Export synapse data" << std::endl;
        std::cout << "  config <file>    Export configuration" << std::endl;
        std::cout << "  stats <file>     Export statistics" << std::endl;
        return;
    }
    
    std::string command = args[0];
    
    if (command == "neurons") {
        if (args.size() < 2) {
            std::cout << "Error: Output file required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Exporting neuron data to " << filepath << "..." << std::endl;
        
        // Implementation would export neuron data
        std::cout << "Neuron export requires implementation." << std::endl;
    }
    else if (command == "synapses") {
        if (args.size() < 2) {
            std::cout << "Error: Output file required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Exporting synapse data to " << filepath << "..." << std::endl;
        
        // Implementation would export synapse data
        std::cout << "Synapse export requires implementation." << std::endl;
    }
    else if (command == "config") {
        if (args.size() < 2) {
            std::cout << "Error: Output file required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Exporting configuration to " << filepath << "..." << std::endl;
        
        if (brain->getConfig()) {
            brain->getConfig()->saveToFile(filepath);
            std::cout << "Configuration exported successfully." << std::endl;
        }
    }
    else if (command == "stats") {
        if (args.size() < 2) {
            std::cout << "Error: Output file required." << std::endl;
            return;
        }
        std::string filepath = args[1];
        std::cout << "Exporting statistics to " << filepath << "..." << std::endl;
        
        std::ofstream outfile(filepath);
        if (outfile.is_open()) {
            outfile << "=== NLM Brain Statistics ===" << std::endl;
            outfile << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            outfile << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            outfile << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            outfile << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
            outfile << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            outfile << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
            outfile << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            outfile << "=== End Statistics ===" << std::endl;
            
            std::cout << "Statistics exported successfully." << std::endl;
        } else {
            std::cout << "Failed to open output file." << std::endl;
        }
    }
    else {
        std::cout << "Unknown export command: " << command << std::endl;
    }
}

// Parse command line arguments
std::vector<std::string> parseArgs(int argc, char* argv[]) {
    std::vector<std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle quoted arguments
        if (arg.find('"') != std::string::npos) {
            std::string quotedArg = "";
            size_t start = arg.find('"');
            size_t end = arg.find('"', start + 1);
            if (end != std::string::npos) {
                quotedArg = arg.substr(start + 1, end - start - 1);
                args.push_back(quotedArg);
                i++; // Skip the closing quote if it's separate
                continue;
            }
        }
        
        // Handle arguments starting with --
        if (arg.find("--") == 0) {
            size_t equalPos = arg.find('=');
            if (equalPos != std::string::npos) {
                args.push_back(arg.substr(0, equalPos));
                args.push_back(arg.substr(equalPos + 1));
            } else {
                args.push_back(arg);
            }
        } else {
            args.push_back(arg);
        }
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    std::string programName = "nlm-cli";
    if (argc > 0) {
        programName = argv[0];
    }
    
    // Initialize logger
    Logger::getInstance().setLevel(LogLevel::Info);
    
    // Parse command line arguments
    std::vector<std::string> args = parseArgs(argc, argv);
    
    if (args.empty()) {
        printHelp(programName);
        return 0;
    }
    
    std::string command = args[0];
    args.erase(args.begin()); // Remove command from args
    
    std::shared_ptr<Config> config = createDefaultConfig();
    std::shared_ptr<Brain> brain;
    
    // Initialize logging
    Logger::getInstance().setLevel(LogLevel::Info);
    
    try {
        if (command == "brain") {
            handleBrainCommand(args, config);
        }
        else if (command == "experiment") {
            handleExperimentCommand(args, config);
        }
        else if (command == "config") {
            handleConfigCommand(args, config);
        }
        else if (command == "log") {
            handleLogCommand(args);
        }
        else if (command == "checkpoint") {
            if (!brain) {
                brain = std::make_shared<Brain>(config);
                brain->initialize();
            }
            handleCheckpointCommand(args, brain);
        }
        else if (command == "explore") {
            if (!brain) {
                brain = std::make_shared<Brain>(config);
                brain->initialize();
            }
            handleExploreCommand(args, brain);
        }
        else if (command == "batch") {
            handleBatchCommand(args, config);
        }
        else if (command == "export") {
            if (!brain) {
                brain = std::make_shared<Brain>(config);
                brain->initialize();
            }
            handleExportCommand(args, brain);
        }
        else if (command == "help") {
            printHelp(programName);
        }
        else if (command == "version") {
            printVersion();
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Use 'help' for available commands." << std::endl;
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}