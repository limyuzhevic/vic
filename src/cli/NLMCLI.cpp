#include "cli/NLMCLI.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

NLMCLI::NLMCLI() : interactiveMode(true), scriptMode(false), outputFormat("pretty") {
    initializeLogger();
}

NLMCLI::~NLMCLI() {
    cleanupLogger();
}

int NLMCLI::run(int argc, char** argv) {
    printBanner();
    
    // Detect mode from arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--script" || arg == "-s") {
            scriptMode = true;
            interactiveMode = false;
        } else if (arg == "--format") {
            if (i + 1 < argc) {
                outputFormat = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                loadConfig(argv[++i]);
            }
        }
    }
    
    // Parse arguments
    std::vector<std::string> args = parseArguments(argc, argv);
    
    if (args.empty()) {
        showHelp();
        return 0;
    }
    
    std::string command = args[0];
    
    // Route to appropriate handler based on command
    if (command == "brain") {
        return handleBrainCommands(args);
    } else if (command == "simulation") {
        return handleSimulationCommands(args);
    } else if (command == "analysis") {
        return handleAnalysisCommands(args);
    } else if (command == "config") {
        return handleConfigCommands(args);
    } else if (command == "checkpoint") {
        return handleCheckpointCommands(args);
    } else if (command == "system") {
        return handleSystemCommands(args);
    } else if (command == "help") {
        return handleHelpCommands(args);
    } else {
        handleError("Unknown command: " + command);
        return 1;
    }
}

void NLMCLI::initializeCommandLine(int argc, char** argv) {
    // Initialize configuration
    config = std::make_shared<Config>();
    
    // Parse configuration from command line
    config->loadFromArgs(argc, argv);
    
    // Load configuration from file if specified
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            loadConfig(argv[++i]);
            break;
        }
    }
}

int NLMCLI::handleBrainCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        handleError("Brain subcommand required. Available: create, load, save, info, list, delete");
        return 1;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "create") {
        if (args.size() < 3) {
            handleError("Brain name required for create");
            return 1;
        }
        std::string brainName = args[2];
        std::string configFile = (args.size() >= 4) ? args[3] : "";
        return createBrain(brainName, configFile) ? 0 : 1;
    } else if (subcommand == "load") {
        if (args.size() < 3) {
            handleError("Brain name required for load");
            return 1;
        }
        std::string brainName = args[2];
        return loadBrain(brainName) ? 0 : 1;
    } else if (subcommand == "save") {
        if (args.size() < 3) {
            handleError("Brain name required for save");
            return 1;
        }
        std::string brainName = args[2];
        return saveBrain(brainName) ? 0 : 1;
    } else if (subcommand == "info") {
        if (args.size() < 3) {
            if (isBrainLoaded()) {
                std::string brainName = getCurrentBrainName();
                return infoBrain(brainName) ? 0 : 1;
            } else {
                handleError("Brain name required for info (or load a brain first)");
                return 1;
            }
        }
        std::string brainName = args[2];
        return infoBrain(brainName) ? 0 : 1;
    } else if (subcommand == "list") {
        return listBrains() ? 0 : 1;
    } else if (subcommand == "delete") {
        if (args.size() < 3) {
            handleError("Brain name required for delete");
            return 1;
        }
        std::string brainName = args[2];
        return deleteBrain(brainName) ? 0 : 1;
    } else {
        handleError("Unknown brain subcommand: " + subcommand);
        return 1;
    }
}

int NLMCLI::handleSimulationCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        handleError("Simulation subcommand required. Available: run, run-continuous, step, pause, resume, stop");
        return 1;
    }
    
    std::string subcommand = args[1];
    
    if (!isBrainLoaded()) {
        handleError("No brain loaded. Please load or create a brain first.");
        return 1;
    }
    
    if (subcommand == "run") {
        int steps = 100;
        double duration = 0.0;
        if (args.size() >= 3) {
            steps = std::stoi(args[2]);
        }
        if (args.size() >= 4) {
            duration = std::stod(args[3]);
        }
        return runSimulation(getCurrentBrainName(), steps, duration) ? 0 : 1;
    } else if (subcommand == "run-continuous") {
        int steps = -1;
        if (args.size() >= 3) {
            steps = std::stoi(args[2]);
        }
        return runContinuousSimulation(getCurrentBrainName(), steps) ? 0 : 1;
    } else if (subcommand == "step") {
        int steps = 1;
        if (args.size() >= 3) {
            steps = std::stoi(args[2]);
        }
        return stepSimulation(getCurrentBrainName(), steps) ? 0 : 1;
    } else if (subcommand == "pause") {
        return pauseSimulation() ? 0 : 1;
    } else if (subcommand == "resume") {
        return resumeSimulation() ? 0 : 1;
    } else if (subcommand == "stop") {
        return stopSimulation() ? 0 : 1;
    } else {
        handleError("Unknown simulation subcommand: " + subcommand);
        return 1;
    }
}

int NLMCLI::handleAnalysisCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        handleError("Analysis subcommand required. Available: stats, memory, neuromodulation, prediction, cognition, development");
        return 1;
    }
    
    std::string subcommand = args[1];
    
    if (!isBrainLoaded()) {
        handleError("No brain loaded. Please load or create a brain first.");
        return 1;
    }
    
    std::string brainName = getCurrentBrainName();
    
    if (subcommand == "stats") {
        return analyzeStats(brainName) ? 0 : 1;
    } else if (subcommand == "memory") {
        return analyzeMemory(brainName) ? 0 : 1;
    } else if (subcommand == "neuromodulation") {
        return analyzeNeuromodulation(brainName) ? 0 : 1;
    } else if (subcommand == "prediction") {
        return analyzePrediction(brainName) ? 0 : 1;
    } else if (subcommand == "cognition") {
        return analyzeCognition(brainName) ? 0 : 1;
    } else if (subcommand == "development") {
        return analyzeDevelopment(brainName) ? 0 : 1;
    } else {
        handleError("Unknown analysis subcommand: " + subcommand);
        return 1;
    }
}

int NLMCLI::handleConfigCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        handleError("Config subcommand required. Available: show, set, load, save, validate, template");
        return 1;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "show") {
        std::string key = (args.size() >= 3) ? args[2] : "";
        return showConfig(key) ? 0 : 1;
    } else if (subcommand == "set") {
        if (args.size() < 4) {
            handleError("Key and value required for set");
            return 1;
        }
        std::string key = args[2];
        std::string value = args[3];
        return setConfig(key, value) ? 0 : 1;
    } else if (subcommand == "load") {
        if (args.size() < 3) {
            handleError("Config file path required for load");
            return 1;
        }
        std::string filepath = args[2];
        return loadConfigFile(filepath) ? 0 : 1;
    } else if (subcommand == "save") {
        if (args.size() < 3) {
            handleError("Config file path required for save");
            return 1;
        }
        std::string filepath = args[2];
        return saveConfigFile(filepath) ? 0 : 1;
    } else if (subcommand == "validate") {
        return validateConfig() ? 0 : 1;
    } else if (subcommand == "template") {
        std::string action = (args.size() >= 3) ? args[2] : "";
        std::string name = (args.size() >= 4) ? args[3] : "";
        return manageTemplates(action, name) ? 0 : 1;
    } else {
        handleError("Unknown config subcommand: " + subcommand);
        return 1;
    }
}

int NLMCLI::handleCheckpointCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        handleError("Checkpoint subcommand required. Available: create, list, load, delete, restore, compare");
        return 1;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "create") {
        if (args.size() < 3) {
            handleError("Checkpoint name required for create");
            return 1;
        }
        std::string checkpointName = args[2];
        return createCheckpoint(checkpointName) ? 0 : 1;
    } else if (subcommand == "list") {
        return listCheckpoints() ? 0 : 1;
    } else if (subcommand == "load") {
        if (args.size() < 3) {
            handleError("Checkpoint name required for load");
            return 1;
        }
        std::string checkpointName = args[2];
        return loadCheckpoint(checkpointName) ? 0 : 1;
    } else if (subcommand == "delete") {
        if (args.size() < 3) {
            handleError("Checkpoint name required for delete");
            return 1;
        }
        std::string checkpointName = args[2];
        return deleteCheckpoint(checkpointName) ? 0 : 1;
    } else if (subcommand == "restore") {
        if (args.size() < 3) {
            handleError("Checkpoint name required for restore");
            return 1;
        }
        std::string checkpointName = args[2];
        return restoreCheckpoint(checkpointName) ? 0 : 1;
    } else if (subcommand == "compare") {
        if (args.size() < 4) {
            handleError("Two checkpoint names required for compare");
            return 1;
        }
        std::string checkpoint1 = args[2];
        std::string checkpoint2 = args[3];
        return compareCheckpoints(checkpoint1, checkpoint2) ? 0 : 1;
    } else {
        handleError("Unknown checkpoint subcommand: " + subcommand);
        return 1;
    }
}

int NLMCLI::handleSystemCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        handleError("System subcommand required. Available: info, logs, monitor, stats, performance");
        return 1;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "info") {
        return showSystemInfo() ? 0 : 1;
    } else if (subcommand == "logs") {
        int lines = 100;
        std::string level = "INFO";
        if (args.size() >= 3) {
            lines = std::stoi(args[2]);
        }
        if (args.size() >= 4) {
            level = args[3];
        }
        return showLogs(lines, level) ? 0 : 1;
    } else if (subcommand == "monitor") {
        int duration = 0;
        double interval = 1.0;
        if (args.size() >= 3) {
            duration = std::stoi(args[2]);
        }
        if (args.size() >= 4) {
            interval = std::stod(args[3]);
        }
        return monitorSystem(duration, interval) ? 0 : 1;
    } else if (subcommand == "stats") {
        return showSystemStats() ? 0 : 1;
    } else if (subcommand == "performance") {
        return showPerformanceMetrics() ? 0 : 1;
    } else {
        handleError("Unknown system subcommand: " + subcommand);
        return 1;
    }
}

int NLMCLI::handleHelpCommands(std::vector<std::string>& args) {
    if (args.size() < 2) {
        return showHelp() ? 0 : 1;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "examples") {
        return showExamples() ? 0 : 1;
    } else if (subcommand == "tutorial") {
        return showTutorial() ? 0 : 1;
    } else if (subcommand == "reference") {
        return showReference() ? 0 : 1;
    } else {
        handleError("Unknown help subcommand: " + subcommand);
        return 1;
    }
}

void NLMCLI::formatOutput(const std::string& data, const std::string& format) {
    if (format == "json") {
        // Parse as JSON if possible, otherwise output as string
        try {
            auto jsonData = json::parse(data);
            std::cout << jsonData.dump(2) << std::endl;
        } catch (...) {
            std::cout << data << std::endl;
        }
    } else if (format == "minimal") {
        std::cout << data << std::endl;
    } else {
        // Pretty format
        std::cout << "=== " << data << " ===" << std::endl;
    }
}

void NLMCLI::handleError(const std::string& error, int exitCode) {
    std::cerr << "Error: " << error << std::endl;
    if (scriptMode) {
        // In script mode, write error to log file
        std::ofstream logFile("nlm-cli-error.log", std::ios::app);
        if (logFile) {
            logFile << "[" << getCurrentTimestamp() << "] " << error << std::endl;
        }
    }
}

void NLMCLI::loadConfig(const std::string& filepath) {
    if (config->loadFromFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    } else {
        NLM_LOG_WARNING("Failed to load configuration from: " + filepath);
    }
}

void NLMCLI::saveConfig(const std::string& filepath) {
    if (config->saveToFile(filepath)) {
        NLM_LOG_INFO("Saved configuration to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
    }
}

bool NLMCLI::createBrain(const std::string& name, const std::string& configFile) {
    if (!validateBrainName(name)) {
        handleError("Invalid brain name: " + name);
        return false;
    }
    
    if (brainExists(name)) {
        handleError("Brain already exists: " + name);
        return false;
    }
    
    // Create brain with configuration
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        handleError("Failed to initialize brain");
        brain.reset();
        return false;
    }
    
    // Save brain if name provided
    if (!name.empty()) {
        if (!saveBrain(name)) {
            handleError("Failed to save brain: " + name);
            return false;
        }
    }
    
    NLM_LOG_INFO("Created brain: " + name);
    return true;
}

bool NLMCLI::loadBrain(const std::string& name) {
    if (!validateBrainName(name)) {
        handleError("Invalid brain name: " + name);
        return false;
    }
    
    std::string filepath = getBrainPath(name);
    
    if (!brainExists(name)) {
        handleError("Brain does not exist: " + name);
        return false;
    }
    
    // Load brain from file
    brain = std::make_shared<Brain>(config);
    
    if (!brain->load(filepath)) {
        handleError("Failed to load brain: " + name);
        brain.reset();
        return false;
    }
    
    NLM_LOG_INFO("Loaded brain: " + name);
    return true;
}

bool NLMCLI::saveBrain(const std::string& name) {
    if (!brain) {
        handleError("No brain to save");
        return false;
    }
    
    if (!validateBrainName(name)) {
        handleError("Invalid brain name: " + name);
        return false;
    }
    
    std::string filepath = getBrainPath(name);
    
    if (brain->save(filepath)) {
        NLM_LOG_INFO("Saved brain to: " + filepath);
        return true;
    } else {
        handleError("Failed to save brain to: " + filepath);
        return false;
    }
}

bool NLMCLI::deleteBrain(const std::string& name) {
    if (!validateBrainName(name)) {
        handleError("Invalid brain name: " + name);
        return false;
    }
    
    std::string filepath = getBrainPath(name);
    
    if (std::filesystem::remove(filepath)) {
        NLM_LOG_INFO("Deleted brain: " + name);
        
        if (isBrainLoaded() && getCurrentBrainName() == name) {
            brain.reset();
        }
        
        return true;
    } else {
        handleError("Failed to delete brain: " + name);
        return false;
    }
}

bool NLMCLI::infoBrain(const std::string& name) {
    if (brain) {
        // Display brain information
        brain->logStatus();
        
        if (outputFormat != "minimal") {
            std::cout << "Brain name: " << name << std::endl;
            std::cout << "Brain loaded: " << (isBrainLoaded() ? "yes" : "no") << std::endl;
            
            if (config) {
                std::cout << "Configuration keys: " << config->getKeys().size() << std::endl;
            }
        }
        
        return true;
    } else {
        handleError("No brain loaded");
        return false;
    }
}

bool NLMCLI::listBrains() {
    std::vector<std::string> brains = listBrainFiles();
    
    if (outputFormat == "json") {
        json brainList = json::array();
        for (const auto& brainName : brains) {
            brainList.push_back(brainName);
        }
        std::cout << brainList.dump(2) << std::endl;
    } else {
        std::cout << "Saved brains:" << std::endl;
        for (size_t i = 0; i < brains.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << brains[i] << std::endl;
        }
    }
    
    return true;
}

bool NLMCLI::runSimulation(const std::string& brainName, int steps, double duration) {
    if (!brain) {
        handleError("No brain loaded for simulation");
        return false;
    }
    
    NLM_LOG_INFO("Starting simulation for brain: " + brainName);
    NLM_LOG_INFO("Steps: " + std::to_string(steps) + ", Duration: " + std::to_string(duration) + "s");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        Timestamp currentTime = static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001);
        
        brain->step(step, currentTime);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(steps));
        }
    }
    
    NLM_LOG_INFO("Simulation completed for brain: " + brainName);
    return true;
}

bool NLMCLI::runContinuousSimulation(const std::string& brainName, int steps) {
    if (!brain) {
        handleError("No brain loaded for continuous simulation");
        return false;
    }
    
    NLM_LOG_INFO("Starting continuous simulation for brain: " + brainName);
    if (steps > 0) {
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
    } else {
        NLM_LOG_INFO("Running until interrupted (Ctrl+C)");
    }
    
    SimulationStep step = 0;
    while (steps <= 0 || step < steps) {
        Timestamp currentTime = static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001);
        
        brain->step(step, currentTime);
        step++;
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step));
        }
    }
    
    NLM_LOG_INFO("Continuous simulation completed for brain: " + brainName);
    return true;
}

bool NLMCLI::stepSimulation(const std::string& brainName, int steps) {
    if (!brain) {
        handleError("No brain loaded for simulation step");
        return false;
    }
    
    NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation step(s) for brain: " + brainName);
    
    for (SimulationStep step = 0; step < steps; ++step) {
        Timestamp currentTime = static_cast<Timestamp>(step) * config->getOr<double>("simulation_timestep", 0.001);
        
        brain->step(step, currentTime);
    }
    
    NLM_LOG_INFO("Simulation steps completed for brain: " + brainName);
    return true;
}

bool NLMCLI::pauseSimulation() {
    if (!brain) {
        handleError("No brain loaded for pause");
        return false;
    }
    
    // Implementation depends on simulation state management
    NLM_LOG_INFO("Simulation paused");
    return true;
}

bool NLMCLI::resumeSimulation() {
    if (!brain) {
        handleError("No brain loaded for resume");
        return false;
    }
    
    // Implementation depends on simulation state management
    NLM_LOG_INFO("Simulation resumed");
    return true;
}

bool NLMCLI::stopSimulation() {
    if (!brain) {
        handleError("No brain loaded for stop");
        return false;
    }
    
    // Implementation depends on simulation state management
    NLM_LOG_INFO("Simulation stopped");
    return true;
}

bool NLMCLI::analyzeStats(const std::string& brainName) {
    if (!brain) {
        handleError("No brain loaded for analysis");
        return false;
    }
    
    NLM_LOG_INFO("=== Brain Statistics ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Excitation/Inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    return true;
}

bool NLMCLI::analyzeMemory(const std::string& brainName) {
    if (!brain) {
        handleError("No brain loaded for memory analysis");
        return false;
    }
    
    NLM_LOG_INFO("=== Memory Analysis ===");
    
    if (auto* workingMemory = brain->getWorkingMemory()) {
        NLM_LOG_INFO("Working Memory: Active");
        // Add working memory specific analysis
    } else {
        NLM_LOG_INFO("Working Memory: Not available");
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic Memory: Active");
        // Add episodic memory specific analysis
    } else {
        NLM_LOG_INFO("Episodic Memory: Not available");
    }
    
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        NLM_LOG_INFO("Associative Memory: Active");
        // Add associative memory specific analysis
    } else {
        NLM_LOG_INFO("Associative Memory: Not available");
    }
    
    return true;
}

bool NLMCLI::analyzeNeuromodulation(const std::string& brainName) {
    if (!brain) {
        handleError("No brain loaded for neuromodulation analysis");
        return false;
    }
    
    NLM_LOG_INFO("=== Neuromodulation Analysis ===");
    
    if (auto* dopamine = brain->getDopamine()) {
        NLM_LOG_INFO("Dopamine system: Active");
        // Add dopamine specific analysis
    } else {
        NLM_LOG_INFO("Dopamine system: Not available");
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        NLM_LOG_INFO("Curiosity system: Active");
        // Add curiosity specific analysis
    } else {
        NLM_LOG_INFO("Curiosity system: Not available");
    }
    
    if (auto* novelty = brain->getNovelty()) {
        NLM_LOG_INFO("Novelty system: Active");
        // Add novelty specific analysis
    } else {
        NLM_LOG_INFO("Novelty system: Not available");
    }
    
    if (auto* predictionError = brain->getPredictionErrorSignal()) {
        NLM_LOG_INFO("Prediction Error system: Active");
        // Add prediction error specific analysis
    } else {
        NLM_LOG_INFO("Prediction Error system: Not available");
    }
    
    return true;
}

bool NLMCLI::analyzePrediction(const std::string& brainName) {
    if (!brain) {
        handleError("No brain loaded for prediction analysis");
        return false;
    }
    
    NLM_LOG_INFO("=== Prediction Analysis ===");
    
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        NLM_LOG_INFO("Prediction system: Active");
        // Add prediction system specific analysis
    } else {
        NLM_LOG_INFO("Prediction system: Not available");
    }
    
    return true;
}

bool NLMCLI::analyzeCognition(const std::string& brainName) {
    if (!brain) {
        handleError("No brain loaded for cognition analysis");
        return false;
    }
    
    NLM_LOG_INFO("=== Cognition Analysis ===");
    
    if (auto* planner = brain->getPlanner()) {
        NLM_LOG_INFO("Neural Planner: Active");
        // Add neural planner specific analysis
    } else {
        NLM_LOG_INFO("Neural Planner: Not available");
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        NLM_LOG_INFO("Concept Formation: Active");
        // Add concept formation specific analysis
    } else {
        NLM_LOG_INFO("Concept Formation: Not available");
    }
    
    if (auto* attention = brain->getAttention()) {
        NLM_LOG_INFO("Attentional Selection: Active");
        // Add attentional selection specific analysis
    } else {
        NLM_LOG_INFO("Attentional Selection: Not available");
    }
    
    return true;
}

bool NLMCLI::analyzeDevelopment(const std::string& brainName) {
    if (!brain) {
        handleError("No brain loaded for development analysis");
        return false;
    }
    
    NLM_LOG_INFO("=== Development Analysis ===");
    
    if (auto* developmentSystem = brain->getDevelopmentSystem()) {
        NLM_LOG_INFO("Development System: Active");
        DevelopmentStage stage = brain->getDevelopmentalStage();
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(stage)));
        // Add development system specific analysis
    } else {
        NLM_LOG_INFO("Development System: Not available");
    }
    
    return true;
}

bool NLMCLI::showConfig(const std::string& key) {
    if (!config) {
        handleError("No configuration loaded");
        return false;
    }
    
    if (key.empty()) {
        // Show all configuration
        NLM_LOG_INFO("Configuration summary:");
        std::cout << config->summary() << std::endl;
    } else {
        // Show specific key
        auto value = config->get<std::string>(key);
        if (value) {
            NLM_LOG_INFO("Config[" + key + "]: " + *value);
        } else {
            NLM_LOG_WARNING("Key not found: " + key);
        }
    }
    
    return true;
}

bool NLMCLI::setConfig(const std::string& key, const std::string& value) {
    if (!config) {
        handleError("No configuration loaded");
        return false;
    }
    
    if (!validateConfigValue(key, value)) {
        handleError("Invalid configuration value: " + key + " = " + value);
        return false;
    }
    
    // Try to parse value based on key type
    try {
        // Try integer
        int intValue = std::stoi(value);
        config->set(key, intValue);
        NLM_LOG_INFO("Set config[" + key + "] = " + std::to_string(intValue));
        return true;
    } catch (...) {
        try {
            // Try double
            double doubleValue = std::stod(value);
            config->set(key, doubleValue);
            NLM_LOG_INFO("Set config[" + key + "] = " + std::to_string(doubleValue));
            return true;
        } catch (...) {
            try {
                // Try boolean
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    config->set(key, boolValue);
                    NLM_LOG_INFO("Set config[" + key + "] = " + (boolValue ? "true" : "false"));
                    return true;
                }
            } catch (...) {
                // Treat as string
                config->set(key, value);
                NLM_LOG_INFO("Set config[" + key + "] = " + value);
                return true;
            }
        }
    }
    
    handleError("Could not parse config value: " + value);
    return false;
}

bool NLMCLI::loadConfigFile(const std::string& filepath) {
    if (config->loadFromFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
        return true;
    } else {
        handleError("Failed to load configuration from file: " + filepath);
        return false;
    }
}

bool NLMCLI::saveConfigFile(const std::string& filepath) {
    if (config->saveToFile(filepath)) {
        NLM_LOG_INFO("Saved configuration to file: " + filepath);
        return true;
    } else {
        handleError("Failed to save configuration to file: " + filepath);
        return false;
    }
}

bool NLMCLI::validateConfig() {
    if (!config) {
        handleError("No configuration to validate");
        return false;
    }
    
    ValidationResult validation = config->validateConfiguration();
    
    if (validation.isValid) {
        NLM_LOG_INFO("Configuration is valid");
        return true;
    } else {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : validation.errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
}

bool NLMCLI::manageTemplates(const std::string& action, const std::string& name) {
    if (action == "list") {
        std::vector<std::string> templateNames = config->getTemplateNames();
        NLM_LOG_INFO("Available templates:");
        for (const auto& templateName : templateNames) {
            NLM_LOG_INFO("  " + templateName);
        }
        return true;
    } else if (action == "create" && !name.empty()) {
        // This would require template data, simplified for now
        handleError("Template creation not yet implemented");
        return false;
    } else if (action == "load" && !name.empty()) {
        // This would require template file, simplified for now
        handleError("Template loading not yet implemented");
        return false;
    } else if (action == "delete" && !name.empty()) {
        if (config->removeTemplate(name)) {
            NLM_LOG_INFO("Removed template: " + name);
            return true;
        } else {
            handleError("Failed to remove template: " + name);
            return false;
        }
    } else {
        handleError("Invalid template action or missing name");
        return false;
    }
}

bool NLMCLI::createCheckpoint(const std::string& name) {
    if (!brain) {
        handleError("No brain loaded for checkpoint creation");
        return false;
    }
    
    std::string filepath = getCheckpointPath(name);
    
    if (brain->save(filepath)) {
        NLM_LOG_INFO("Created checkpoint: " + name + " at " + filepath);
        return true;
    } else {
        handleError("Failed to create checkpoint: " + name);
        return false;
    }
}

bool NLMCLI::listCheckpoints() {
    // This would need to scan for checkpoint files
    NLM_LOG_INFO("Checkpoint listing not yet implemented");
    return true;
}

bool NLMCLI::loadCheckpoint(const std::string& name) {
    if (!brain) {
        handleError("No brain loaded for checkpoint loading");
        return false;
    }
    
    std::string filepath = getCheckpointPath(name);
    
    if (brain->load(filepath)) {
        NLM_LOG_INFO("Loaded checkpoint: " + name + " from " + filepath);
        return true;
    } else {
        handleError("Failed to load checkpoint: " + name);
        return false;
    }
}

bool NLMCLI::deleteCheckpoint(const std::string& name) {
    std::string filepath = getCheckpointPath(name);
    
    if (std::filesystem::remove(filepath)) {
        NLM_LOG_INFO("Deleted checkpoint: " + name);
        return true;
    } else {
        handleError("Failed to delete checkpoint: " + name);
        return false;
    }
}

bool NLMCLI::restoreCheckpoint(const std::string& name) {
    return loadCheckpoint(name); // Same as load for now
}

bool NLMCLI::compareCheckpoints(const std::string& name1, const std::string& name2) {
    // This would require loading both checkpoints and comparing them
    NLM_LOG_INFO("Checkpoint comparison not yet implemented");
    return true;
}

bool NLMCLI::showSystemInfo() {
    NLM_LOG_INFO("=== NLM System Information ===");
    NLM_LOG_INFO("Version: " + getVersion());
    NLM_LOG_INFO("Build: " + getBuildInfo());
    NLM_LOG_INFO("Platform: Linux x86_64");
    NLM_LOG_INFO("Compiler: C++20");
    
    if (config) {
        NLM_LOG_INFO("Configuration loaded: yes");
        NLM_LOG_INFO("Configuration keys: " + std::to_string(config->getKeys().size()));
    } else {
        NLM_LOG_INFO("Configuration loaded: no");
    }
    
    if (brain) {
        NLM_LOG_INFO("Brain loaded: yes");
        NLM_LOG_INFO("Brain neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Brain synapses: " + std::to_string(brain->getTotalSynapseCount()));
    } else {
        NLM_LOG_INFO("Brain loaded: no");
    }
    
    return true;
}

bool NLMCLI::showLogs(int lines, const std::string& level) {
    NLM_LOG_INFO("Log display not yet implemented (would read from log file)");
    return true;
}

bool NLMCLI::monitorSystem(int duration, double interval) {
    NLM_LOG_INFO("System monitoring not yet implemented");
    return true;
}

bool NLMCLI::showSystemStats() {
    NLM_LOG_INFO("=== System Statistics ===");
    NLM_LOG_INFO("System stats not yet implemented");
    return true;
}

bool NLMCLI::showPerformanceMetrics() {
    NLM_LOG_INFO("=== Performance Metrics ===");
    NLM_LOG_INFO("Performance metrics not yet implemented");
    return true;
}

bool NLMCLI::showHelp(const std::string& topic) {
    if (topic.empty()) {
        printBanner();
        std::cout << "\nUsage: nlm-cli [OPTIONS] COMMAND [ARGS...]\n\n";
        std::cout << "Commands:\n";
        std::cout << "  brain       Brain management commands\n";
        std::cout << "  simulation  Simulation commands\n";
        std::cout << "  analysis    Analysis commands\n";
        std::cout << "  config      Configuration management\n";
        std::cout << "  checkpoint  Checkpoint management\n";
        std::cout << "  system      System management\n";
        std::cout << "  help        Help and documentation\n\n";
        std::cout << "Options:\n";
        std::cout << "  --script, -s           Script mode (non-interactive)\n";
        std::cout << "  --format FORMAT         Output format (pretty, json, minimal)\n";
        std::cout << "  --config FILE           Configuration file\n";
        std::cout << "  --help                 Show help\n";
        std::cout << "  --version              Show version\n\n";
        std::cout << "For command-specific help: nlm-cli help COMMAND\n";
    } else {
        std::cout << "Help for \"" << topic << "\" not yet implemented\n";
    }
    
    return true;
}

bool NLMCLI::showExamples() {
    NLM_LOG_INFO("=== Examples ===");
    NLM_LOG_INFO("1. Create a new brain:");
    NLM_LOG_INFO("   nlm-cli brain create mybrain --config configs/default.cfg");
    NLM_LOG_INFO("\n2. Load an existing brain:");
    NLM_LOG_INFO("   nlm-cli brain load mybrain");
    NLM_LOG_INFO("\n3. Run a simulation:");
    NLM_LOG_INFO("   nlm-cli brain load mybrain && nlm-cli simulation run 1000");
    NLM_LOG_INFO("\n4. Show brain statistics:");
    NLM_LOG_INFO("   nlm-cli brain load mybrain && nlm-cli analysis stats");
    NLM_LOG_INFO("\n5. Save configuration:");
    NLM_LOG_INFO("   nlm-cli config save myconfig.json");
    NLM_LOG_INFO("\n6. Run in script mode:");
    NLM_LOG_INFO("   nlm-cli --script brain create mybrain -c config.json");
    
    return true;
}

bool NLMCLI::showTutorial() {
    NLM_LOG_INFO("=== Tutorial ===");
    NLM_LOG_INFO("NLM CLI Tutorial (Not yet implemented)");
    return true;
}

bool NLMCLI::showReference() {
    NLM_LOG_INFO("=== Reference ===");
    NLM_LOG_INFO("NLM CLI Reference (Not yet implemented)");
    return true;
}

std::string NLMCLI::getVersion() const {
    return "NLM Phase 2 - 1.0.0";
}

std::string NLMCLI::getBuildInfo() const {
    return "Built with C++20, SIMD optimizations";
}

bool NLMCLI::isBrainLoaded() const {
    return brain != nullptr;
}

std::string NLMCLI::getCurrentBrainName() const {
    if (isBrainLoaded()) {
        return "current_brain";
    }
    return "";
}

std::vector<std::string> NLMCLI::parseArguments(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--script" || arg == "-s" || arg == "--format" || arg == "--config" || arg == "--help" || arg == "--version") {
            // Skip argument value for these options
            i++;
        }
        args.push_back(arg);
    }
    return args;
}

std::string NLMCLI::detectOutputFormat(const std::string& arg) {
    if (arg == "pretty" || arg == "json" || arg == "minimal") {
        return arg;
    }
    return "pretty";
}

std::string NLMCLI::detectMode(const std::string& arg) {
    if (arg == "--script" || arg == "-s") {
        return "script";
    }
    return "interactive";
}

bool NLMCLI::brainExists(const std::string& name) const {
    std::string filepath = getBrainPath(name);
    return std::filesystem::exists(filepath);
}

std::vector<std::string> NLMCLI::listBrainFiles() const {
    std::vector<std::string> brains;
    std::string brainDir = "brains/";
    
    if (std::filesystem::exists(brainDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(brainDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".nlm") {
                brains.push_back(entry.path().stem().string());
            }
        }
    }
    
    return brains;
}

std::string NLMCLI::getBrainPath(const std::string& name) const {
    return "brains/" + name + ".nlm";
}

std::string NLMCLI::getCheckpointPath(const std::string& name) const {
    return "checkpoints/" + name + ".ckpt";
}

bool NLMCLI::validateBrainName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // Check for valid characters
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool NLMCLI::validateConfigValue(const std::string& key, const std::string& value) const {
    // Basic validation - could be extended
    return !value.empty();
}

void NLMCLI::printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void NLMCLI::initializeLogger() {
    // Initialize logger
}

void NLMCLI::cleanupLogger() {
    // Cleanup logger
}

std::string NLMCLI::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm