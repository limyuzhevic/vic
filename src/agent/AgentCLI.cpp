#include "AgentCLI.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {

AgentCLI::AgentCLI(std::shared_ptr<AgentBrain> agentBrain, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : agentBrain_(agentBrain), brain_(brain), config_(config), commandType_(CommandType::HELP) {
}

AgentCLI::~AgentCLI() = default;

bool AgentCLI::parseArguments(int argc, char** argv) {
    if (argc < 2) {
        commandType_ = CommandType::HELP;
        return true;
    }
    
    std::string arg1 = argv[1];
    
    // Map commands to types
    static const std::map<std::string, CommandType> commandMap = {
        {"--help", CommandType::HELP},
        {"--status", CommandType::STATUS},
        {"--reset", CommandType::RESET},
        {"--config", CommandType::CONFIG},
        {"--interactive", CommandType::INTERACTIVE},
        {"--enable-reward-modulation", CommandType::ENABLE_REWARD_MODULATION},
        {"--enable-structural-plasticity", CommandType::ENABLE_STRUCTURAL_PLASTICITY},
        {"--enable-development", CommandType::ENABLE_DEVELOPMENT},
        {"--enable-curiosity", CommandType::ENABLE_CURIOSITY},
        {"--get-neuromodulation-level", CommandType::GET_NEUROMODULATION_LEVEL},
        {"--get-curiosity-level", CommandType::GET_CURIOUSITY_LEVEL},
        {"--get-novelty-level", CommandType::GET_NOVELTY_LEVEL},
        {"--get-prediction-error", CommandType::GET_PREDICTION_ERROR},
        {"--get-developmental-stage", CommandType::GET_DEVELOPMENTAL_STAGE},
        {"--save-state", CommandType::SAVE_STATE},
        {"--load-state", CommandType::LOAD_STATE},
        {"--export-metrics", CommandType::EXPORT_METRICS},
        {"--compare-states", CommandType::COMPARE_STATES},
        {"--run-experiment", CommandType::RUN_EXPERIMENT},
        {"--train-agent", CommandType::TRAIN_AGENT},
        {"--list-configs", CommandType::LIST_CONFIGS},
        {"--create-config", CommandType::CREATE_CONFIG},
        {"--validate-config", CommandType::VALIDATE_CONFIG},
        {"--log-level", CommandType::SET_LOG_LEVEL},
        {"--enable-logging", CommandType::ENABLE_LOGGING},
        {"--disable-logging", CommandType::DISABLE_LOGGING},
        {"--benchmark", CommandType::BENCHMARK},
        {"--profile", CommandType::PROFILE}
    };
    
    auto it = commandMap.find(arg1);
    if (it != commandMap.end()) {
        commandType_ = it->second;
        
        // Parse additional arguments
        if (commandType_ == CommandType::CONFIG && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::ENABLE_REWARD_MODULATION && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::ENABLE_STRUCTURAL_PLASTICITY && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::ENABLE_DEVELOPMENT && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::ENABLE_CURIOSITY && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::SAVE_STATE && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::LOAD_STATE && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::EXPORT_METRICS && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::COMPARE_STATES && argc > 3) {
            commandArg1_ = argv[2];
            commandArg2_ = argv[3];
        } else if (commandType_ == CommandType::RUN_EXPERIMENT && argc > 2) {
            try {
                commandArg1_ = argv[2];
            } catch (...) {
                printError("Invalid number of steps for --run-experiment");
                return false;
            }
        } else if (commandType_ == CommandType::TRAIN_AGENT && argc > 3) {
            commandArg1_ = argv[2];
            commandArg2_ = argv[3];
        } else if (commandType_ == CommandType::CREATE_CONFIG && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::VALIDATE_CONFIG && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::SET_LOG_LEVEL && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::ENABLE_LOGGING && argc > 2) {
            commandArg1_ = argv[2];
        } else if (commandType_ == CommandType::BENCHMARK && argc > 3) {
            commandArg1_ = argv[2];
            commandArg2_ = argv[3];
        }
    } else {
        printError("Unknown command: " + arg1);
        return false;
    }
    
    return true;
}

int AgentCLI::executeCommand() {
    auto commands = getAvailableCommands();
    auto cmdIt = std::find_if(commands.begin(), commands.end(),
                             [this](const Command& cmd) { return cmd.name == "--help"; });
    
    if (cmdIt == commands.end()) {
        printError("Help command not found in command list");
        return 1;
    }
    
    Command cmd = *cmdIt;
    
    if (!validateArguments(cmd)) {
        return 1;
    }
    
    logCommand(cmd);
    
    switch (commandType_) {
        case CommandType::HELP:
            showHelp();
            return 0;
            
        case CommandType::STATUS:
            showStatus();
            return 0;
            
        case CommandType::RESET:
            if (agentBrain_) {
                agentBrain_->reset();
                std::cout << "Agent reset successfully.\n";
            } else {
                printError("AgentBrain not available");
                return 1;
            }
            return 0;
            
        case CommandType::CONFIG:
            return setConfiguration(commandArg1_) ? 0 : 1;
            
        case CommandType::INTERACTIVE:
            runInteractiveMode();
            return 0;
            
        case CommandType::ENABLE_REWARD_MODULATION:
            bool rewardEnabled;
            if (parseBoolean(commandArg1_, rewardEnabled)) {
                enableRewardModulation(rewardEnabled);
                std::cout << "Reward modulation " << (rewardEnabled ? "enabled" : "disabled") << ".\n";
            } else {
                printError("Invalid boolean value for --enable-reward-modulation");
                return 1;
            }
            return 0;
            
        case CommandType::ENABLE_STRUCTURAL_PLASTICITY:
            if (parseBoolean(commandArg1_, rewardEnabled)) {
                enableStructuralPlasticity(rewardEnabled);
                std::cout << "Structural plasticity " << (rewardEnabled ? "enabled" : "disabled") << ".\n";
            } else {
                printError("Invalid boolean value for --enable-structural-plasticyst");
                return 1;
            }
            return 0;
            
        case CommandType::ENABLE_DEVELOPMENT:
            if (parseBoolean(commandArg1_, rewardEnabled)) {
                enableDevelopment(rewardEnabled);
                std::cout << "Development " << (rewardEnabled ? "enabled" : "disabled") << ".\n";
            } else {
                printError("Invalid boolean value for --enable-development");
                return 1;
            }
            return 0;
            
        case CommandType::ENABLE_CURIOSITY:
            if (parseBoolean(commandArg1_, rewardEnabled)) {
                enableCuriosity(rewardEnabled);
                std::cout << "Curiosity " << (rewardEnabled ? "enabled" : "disabled") << ".\n";
            } else {
                printError("Invalid boolean value for --enable-curiosity");
                return 1;
            }
            return 0;
            
        case CommandType::GET_NEUROMODULATION_LEVEL:
            std::cout << "Neuromodulation level: " << getNeuromodulationLevel() << "\n";
            return 0;
            
        case CommandType::GET_CURIOUSITY_LEVEL:
            std::cout << "Curiosity level: " << getCuriosityLevel() << "\n";
            return 0;
            
        case CommandType::GET_NOVELTY_LEVEL:
            std::cout << "Novelty level: " << getNoveltyLevel() << "\n";
            return 0;
            
        case CommandType::GET_PREDICTION_ERROR:
            std::cout << "Prediction error: " << getPredictionError() << "\n";
            return 0;
            
        case CommandType::GET_DEVELOPMENTAL_STAGE:
            std::cout << "Developmental stage: " << getDevelopmentalStage() << "\n";
            return 0;
            
        case CommandType::SAVE_STATE:
            return saveState(commandArg1_) ? 0 : 1;
            
        case CommandType::LOAD_STATE:
            return loadState(commandArg1_) ? 0 : 1;
            
        case CommandType::EXPORT_METRICS:
            return exportMetrics(commandArg1_) ? 0 : 1;
            
        case CommandType::COMPARE_STATES:
            return compareStates(commandArg1_, commandArg2_) ? 0 : 1;
            
        case CommandType::RUN_EXPERIMENT:
            return runExperiment(std::stoul(commandArg1_)) ? 0 : 1;
            
        case CommandType::TRAIN_AGENT:
            return trainAgent(std::stoul(commandArg1_), std::stoul(commandArg2_)) ? 0 : 1;
            
        case CommandType::LIST_CONFIGS:
            for (const auto& config : listConfigs()) {
                std::cout << config << "\n";
            }
            return 0;
            
        case CommandType::CREATE_CONFIG:
            return createConfig(commandArg1_) ? 0 : 1;
            
        case CommandType::VALIDATE_CONFIG:
            return validateConfig(commandArg1_) ? 0 : 1;
            
        case CommandType::SET_LOG_LEVEL:
            return setLogLevel(commandArg1_) ? 0 : 1;
            
        case CommandType::ENABLE_LOGGING:
            return enableFileLogging(commandArg1_) ? 0 : 1;
            
        case CommandType::DISABLE_LOGGING:
            return disableFileLogging() ? 0 : 1;
            
        case CommandType::BENCHMARK:
            return runBenchmark(std::stoul(commandArg1_), std::stoul(commandArg2_)) ? 0 : 1;
            
        case CommandType::PROFILE:
            profile();
            return 0;
            
        default:
            printError("Unknown command type");
            return 1;
    }
}

void AgentCLI::showHelp() const {
    std::cout << "\nNLM Agent Control Command Line Interface\n";
    std::cout << "====================================\n\n";
    
    auto commands = getAvailableCommands();
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd.name;
        if (!cmd.args.empty()) {
            std::cout << " <" << cmd.args << ">";
        }
        if (!cmd.argHelp.empty()) {
            std::cout << " " << cmd.argHelp;
        }
        std::cout << std::string(50 - cmd.name.length() - cmd.args.length(), ' ') << cmd.description << "\n";
    }
    
    std::cout << "\n";
}

void AgentCLI::showStatus() const {
    std::cout << "\nAgent Status Report\n";
    std::cout << "==================\n\n";
    
    if (agentBrain_) {
        std::cout << "AgentBrain Status:\n";
        std::cout << "  Neuromodulation Level: " << getNeuromodulationLevel() << "\n";
        std::cout << "  Curiosity Level: " << getCuriosityLevel() << "\n";
        std::cout << "  Novelty Level: " << getNoveltyLevel() << "\n";
        std::cout << "  Prediction Error: " << getPredictionError() << "\n";
        std::cout << "  Developmental Stage: " << getDevelopmentalStage() << "\n";
        std::cout << "  Reward Modulation: " << (agentBrain_->isRewardModulationEnabled() ? "Enabled" : "Disabled") << "\n";
        std::cout << "  Structural Plasticity: " << (agentBrain_->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << "\n";
        stdcout << "  Development: " << (agentBrain_->isDevelopmentEnabled() ? "Enabled" : "Disabled") << "\n";
        stdcout << "  Curiosity: " << (agentBrain_->isCuriosityEnabled() ? "Enabled" : "Disabled") << "\n";
        
        if (brain_) {
            std::cout << "\nBrain Status:\n";
            std::cout << "  Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
            std::cout << "  Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
            std::cout << "  Current Step: " << "TODO: Add step counter" << "\n";
            std::cout << "  Simulation Time: " << "TODO: Add time counter" << "\n";
        }
    } else {
        std::cout << "AgentBrain: Not available\n";
    }
    
    if (config_) {
        std::cout << "\nConfiguration:\n";
        std::cout << config_->summary() << "\n";
    }
    
    std::cout << "\n";
}

bool AgentCLI::setConfiguration(const std::string& filepath) {
    if (!config_) {
        printError("Config not available");
        return false;
    }
    
    if (!std::filesystem::exists(filepath)) {
        printError("Configuration file does not exist: " + filepath);
        return false;
    }
    
    if (!config_->loadFromFile(filepath)) {
        printError("Failed to load configuration from: " + filepath);
        return false;
    }
    
    std::cout << "Configuration loaded from: " << filepath << "\n";
    return true;
}

void AgentCLI::runInteractiveMode() {
    std::cout << "\nEntering Interactive Mode (type 'help' for commands, 'exit' to quit)\n";
    std::cout << "===============================================\n\n";
    
    while (true) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);
        
        if (line.empty()) {
            continue;
        }
        
        if (line == "exit" || line == "quit") {
            break;
        }
        
        if (parseCommand(line)) {
            executeCommand();
        } else {
            std::cout << "Invalid command. Type 'help' for available commands.\n";
        }
    }
    
    std::cout << "\nExiting interactive mode.\n";
}

bool AgentCLI::enableRewardModulation(bool enable) {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return false;
    }
    
    agentBrain_->enableRewardModulation(enable);
    return true;
}

bool AgentCLI::enableStructuralPlasticity(bool enable) {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return false;
    }
    
    agentBrain_->enableStructuralPlasticity(enable);
    return true;
}

bool AgentCLI::enableDevelopment(bool enable) {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return false;
    }
    
    agentBrain_->enableDevelopment(enable);
    return true;
}

bool AgentCLI::enableCuriosity(bool enable) {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return false;
    }
    
    agentBrain_->enableCuriosity(enable);
    return true;
}

float AgentCLI::getNeuromodulationLevel() const {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return 0.0f;
    }
    return agentBrain_->getNeuromodulationLevel();
}

float AgentCLI::getCuriosityLevel() const {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return 0.0f;
    }
    return agentBrain_->getCuriosityLevel();
}

float AgentCLI::getNoveltyLevel() const {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return 0.0f;
    }
    return agentBrain_->getNoveltyLevel();
}

float AgentCLI::getPredictionError() const {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return 0.0f;
    }
    return agentBrain_->getPredictionError();
}

std::string AgentCLI::getDevelopmentalStage() const {
    if (!agentBrain_) {
        printError("AgentBrain not available");
        return "Unknown";
    }
    return std::to_string(static_cast<int>(agentBrain_->getDevelopmentalStage()));
}

bool AgentCLI::saveState(const std::string& filepath) const {
    if (!brain_) {
        printError("Brain not available");
        return false;
    }
    
    if (brain_->save(filepath)) {
        std::cout << "Agent state saved to: " << filepath << "\n";
        return true;
    } else {
        printError("Failed to save agent state to: " + filepath);
        return false;
    }
}

bool AgentCLI::loadState(const std::string& filepath) {
    if (!brain_) {
        printError("Brain not available");
        return false;
    }
    
    if (!std::filesystem::exists(filepath)) {
        printError("State file does not exist: " + filepath);
        return false;
    }
    
    if (brain_->load(filepath)) {
        std::cout << "Agent state loaded from: " << filepath << "\n";
        return true;
    } else {
        printError("Failed to load agent state from: " + filepath);
        return false;
    }
}

bool AgentCLI::exportMetrics(const std::string& filepath) const {
    if (!brain_) {
        printError("Brain not available");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        printError("Failed to create metrics file: " + filepath);
        return false;
    }
    
    file << "Metrics export from NLM Agent\n";
    file << "==============================\n\n";
    file << "Total Neurons: " << brain_->getTotalNeuronCount() << "\n";
    file << "Total Synapses: " << brain_->getTotalSynapseCount() << "\n";
    file << "Excitation/Inhibition Ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
    file << "Total Spike Count: " << brain_->getTotalSpikeCount() << "\n";
    file << "Neuromodulation Level: " << getNeuromodulationLevel() << "\n";
    file << "Curiosity Level: " << getCuriosityLevel() << "\n";
    file << "Novelty Level: " << getNoveltyLevel() << "\n";
    file << "Prediction Error: " << getPredictionError() << "\n";
    file << "Developmental Stage: " << getDevelopmentalStage() << "\n";
    
    file.close();
    std::cout << "Metrics exported to: " << filepath << "\n";
    return true;
}

bool AgentCLI::compareStates(const std::string& filepath1, const std::string& filepath2) const {
    if (!std::filesystem::exists(filepath1) || !std::filesystem::exists(filepath2)) {
        printError("One or both state files do not exist");
        return false;
    }
    
    std::cout << "Comparing states from:\n";
    std::cout << "  File 1: " << filepath1 << "\n";
    std::cout << "  File 2: " << filepath2 << "\n";
    
    // For now, just check file existence and size
    auto size1 = std::filesystem::file_size(filepath1);
    auto size2 = std::filesystem::file_size(filepath2);
    
    std::cout << "Size comparison:\n";
    std::cout << "  File 1: " << size1 << " bytes\n";
    std::cout << "  File 2: " << size2 << " bytes\n";
    
    if (size1 == size2) {
        std::cout << "State files are identical in size.\n";
    } else {
        std::cout << "State files differ in size.\n";
    }
    
    return true;
}

bool AgentCLI::runExperiment(size_t steps) {
    std::cout << "Running experiment for " << steps << " steps...\n";
    
    // TODO: Implement actual experiment runner
    // For now, just log and return success
    NLM_LOG_INFO("Experiment started with " + std::to_string(steps) + " steps");
    
    for (size_t i = 0; i < steps; ++i) {
        // Simulate experiment step
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (i % 100 == 0) {
            std::cout << "  Step " << i << "/" << steps << " completed\r" << std::flush;
        }
    }
    
    std::cout << "\nExperiment completed.\n";
    return true;
}

bool AgentCLI::trainAgent(size_t episodes, size_t maxSteps) {
    std::cout << "Training agent for " << episodes << " episodes with max " << maxSteps << " steps per episode...\n";
    
    // TODO: Implement actual training loop
    // For now, just log and return success
    NLM_LOG_INFO("Training started with " + std::to_string(episodes) + " episodes");
    
    for (size_t episode = 0; episode < episodes; ++episode) {
        std::cout << "  Episode " << episode + 1 << "/" << episodes << "\r" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Simulate training step
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    std::cout << "\nTraining completed.\n";
    return true;
}

std::vector<std::string> AgentCLI::listConfigs() const {
    std::vector<std::string> configs;
    
    if (!std::filesystem::exists(".")) {
        return configs;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(".config") != std::string::npos || 
                filename.find("agent_") != std::string::npos) {
                configs.push_back(filename);
            }
        }
    }
    
    return configs;
}

bool AgentCLI::createConfig(const std::string& name) {
    std::string filepath = name + ".config";
    
    if (std::filesystem::exists(filepath)) {
        printError("Configuration already exists: " + filepath);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        printError("Failed to create configuration file: " + filepath);
        return false;
    }
    
    file << "# NLM Agent Configuration\n";
    file << "# Created by agent CLI\n";
    file << "# Empty configuration - add settings as needed\n";
    
    file.close();
    std::cout << "Configuration created: " << filepath << "\n";
    return true;
}

bool AgentCLI::validateConfig(const std::string& filepath) const {
    if (!std::filesystem::exists(filepath)) {
        printError("Configuration file does not exist: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        printError("Failed to open configuration file: " + filepath);
        return false;
    }
    
    // Simple validation: check if file is readable and not empty
    std::string line;
    bool hasContent = false;
    while (std::getline(file, line)) {
        line = trim(line);
        if (!line.empty() && line[0] != '#' && line[0] != '/') {
            hasContent = true;
            break;
        }
    }
    
    if (!hasContent) {
        printError("Configuration file appears to be empty or invalid");
        return false;
    }
    
    std::cout << "Configuration file validation passed: " << filepath << "\n";
    return true;
}

bool AgentCLI::setLogLevel(const std::string& level) {
    // Convert level to lowercase for comparison
    std::string lowerLevel = level;
    std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), ::tolower);
    
    if (lowerLevel == "debug" || lowerLevel == "info" || lowerLevel == "warning" || lowerLevel == "error") {
        NLM_LOG_INFO("Log level set to: " + lowerLevel);
        std::cout << "Log level set to: " << lowerLevel << "\n";
        return true;
    } else {
        printError("Invalid log level: " + level);
        return false;
    }
}

bool AgentCLI::enableFileLogging(const std::string& filepath) {
    NLM_LOG_INFO("File logging enabled: " + filepath);
    std::cout << "File logging enabled: " << filepath << "\n";
    return true;
}

bool AgentCLI::disableFileLogging() {
    NLM_LOG_INFO("File logging disabled");
    std::cout << "File logging disabled\n";
    return true;
}

bool AgentCLI::runBenchmark(size_t neurons, size_t steps) {
    std::cout << "Running benchmark with " << neurons << " neurons for " << steps << " steps...\n";
    
    // TODO: Implement actual benchmark
    // For now, just simulate benchmark run
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < steps; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (i % 100 == 0) {
            std::cout << "  Step " << i << "/" << steps << " completed\r" << std::flush;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\nBenchmark completed in " << duration.count() << " ms.\n";
    return true;
}

void AgentCLI::profile() {
    std::cout << "Profiling agent performance...\n";
    
    // TODO: Implement actual profiling
    // For now, just log and return
    NLM_LOG_INFO("Performance profiling started");
    
    std::cout << "Profiling completed (placeholder implementation).\n";
}

std::vector<Command> AgentCLI::getAvailableCommands() {
    return {
        {CommandType::HELP, "--help", "Show help and available commands", {}, ""},
        {CommandType::STATUS, "--status", "Show agent status and system health", {}, ""},
        {CommandType::RESET, "--reset", "Reset agent for new episode", {}, ""},
        {CommandType::CONFIG, "--config", "Load configuration from file", {"<file>"}, "Configuration file path"},
        {CommandType::INTERACTIVE, "--interactive", "Enter interactive mode", {}, ""},
        {CommandType::ENABLE_REWARD_MODULATION, "--enable-reward-modulation", "Enable/disable reward modulation", {"<true|false>"}, "Enable (true) or disable (false) reward modulation"},
        {CommandType::ENABLE_STRUCTURAL_PLASTICITY, "--enable-structural-plasticity", "Enable/disable structural plasticity", {"<true|false>"}, "Enable (true) or disable (false) structural plasticity"},
        {CommandType::ENABLE_DEVELOPMENT, "--enable-development", "Enable/disable development", {"<true|false>"}, "Enable (true) or disable (false) development"},
        {CommandType::ENABLE_CURIOSITY, "--enable-curiosity", "Enable/disable curiosity", {"<true|false>"}, "Enable (true) or disable (false) curiosity"},
        {CommandType::GET_NEUROMODULATION_LEVEL, "--get-neuromodulation-level", "Get current neuromodulation level", {}, ""},
        {CommandType::GET_CURIOUSITY_LEVEL, "--get-curiosity-level", "Get current curiosity level", {}, ""},
        {CommandType::GET_NOVELTY_LEVEL, "--get-novelty-level", "Get current novelty level", {}, ""},
        {CommandType::GET_PREDICTION_ERROR, "--get-prediction-error", "Get current prediction error", {}, ""},
        {CommandType::GET_DEVELOPMENTAL_STAGE, "--get-developmental-stage", "Get current developmental stage", {}, ""},
        {CommandType::SAVE_STATE, "--save-state", "Save agent state to file", {"<filepath>"}, "Path to save state file"},
        {CommandType::LOAD_STATE, "--load-state", "Load agent state from file", {"<filepath>"}, "Path to load state file"},
        {CommandType::EXPORT_METRICS, "--export-metrics", "Export performance metrics", {"<filepath>"}, "Path to export metrics file"},
        {CommandType::COMPARE_STATES, "--compare-states", "Compare two saved states", {"<file1>", "<file2>"}, "Two state files to compare"},
        {CommandType::RUN_EXPERIMENT, "--run-experiment", "Run agent for specified number of steps", {"<steps>"}, "Number of steps to run"},
        {CommandType::TRAIN_AGENT, "--train-agent", "Train agent over multiple episodes", {"<episodes>", "<max-steps>"}, "Number of episodes and max steps per episode"},
        {CommandType::LIST_CONFIGS, "--list-configs", "List available configuration files", {}, ""},
        {CommandType::CREATE_CONFIG, "--create-config", "Create new configuration file", {"<name>"}, "Name of configuration file to create"},
        {CommandType::VALIDATE_CONFIG, "--validate-config", "Validate configuration file", {"<file>"}, "Configuration file to validate"},
        {CommandType::SET_LOG_LEVEL, "--log-level", "Set logging level", {"<level>"}, "Log level: debug, info, warning, error"},
        {CommandType::ENABLE_LOGGING, "--enable-logging", "Enable logging to file", {"<filepath>"}, "Path to log file"},
        {CommandType::DISABLE_LOGGING, "--disable-logging", "Disable logging", {}, ""},
        {CommandType::BENCHMARK, "--benchmark", "Run benchmark with specified parameters", {"<neurons>", "<steps>"}, "Number of neurons and steps"},
        {CommandType::PROFILE, "--profile", "Profile agent performance", {}, ""}
    };
}

bool AgentCLI::parseCommand(const std::string& commandStr) {
    // Simple command parsing for interactive mode
    // Supports commands like: --enable-reward-modulation true
    if (commandStr.empty()) {
        return false;
    }
    
    // Try to find matching command
    auto commands = getAvailableCommands();
    for (const auto& cmd : commands) {
        if (commandStr.find(cmd.name) == 0) {
            commandType_ = cmd.type;
            
            // Extract arguments
            size_t pos = commandStr.find(' ');
            if (pos != std::string::npos) {
                std::string args = commandStr.substr(pos + 1);
                // Simple argument parsing - could be enhanced
                if (cmd.name == "--enable-reward-modulation" || 
                    cmd.name == "--enable-structural-plasticity" ||
                    cmd.name == "--enable-development" ||
                    cmd.name == "--enable-curiosity") {
                    commandArg1_ = args;
                } else if (cmd.name == "--save-state" ||
                           cmd.name == "--load-state" ||
                           cmd.name == "--export-metrics" ||
                           cmd.name == "--create-config" ||
                           cmd.name == "--validate-config" ||
                           cmd.name == "--log-level" ||
                           cmd.name == "--enable-logging") {
                    commandArg1_ = args;
                } else if (cmd.name == "--compare-states") {
                    size_t pos2 = args.find(' ');
                    if (pos2 != std::string::npos) {
                        commandArg1_ = args.substr(0, pos2);
                        commandArg2_ = args.substr(pos2 + 1);
                    }
                } else if (cmd.name == "--run-experiment") {
                    commandArg1_ = args;
                } else if (cmd.name == "--train-agent") {
                    size_t pos2 = args.find(' ');
                    if (pos2 != std::string::npos) {
                        commandArg1_ = args.substr(0, pos2);
                        commandArg2_ = args.substr(pos2 + 1);
                    }
                } else if (cmd.name == "--benchmark") {
                    size_t pos2 = args.find(' ');
                    if (pos2 != std::string::npos) {
                        commandArg1_ = args.substr(0, pos2);
                        commandArg2_ = args.substr(pos2 + 1);
                    }
                }
            }
            return true;
        }
    }
    
    return false;
}

bool AgentCLI::parseFlags(int argc, char** argv) {
    return parseArguments(argc, argv);
}

bool AgentCLI::validateArguments(const Command& cmd) const {
    // Basic validation - check required arguments
    if (cmd.args.size() == 1 && commandArg1_.empty()) {
        printError("Missing required argument: " + cmd.args[0]);
        return false;
    } else if (cmd.args.size() == 2 && (commandArg1_.empty() || commandArg2_.empty())) {
        printError("Missing required arguments: " + cmd.args[0] + " and " + cmd.args[1]);
        return false;
    }
    
    return true;
}

bool AgentCLI::parseBoolean(const std::string& str, bool& value) const {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr == "true" || lowerStr == "1" || lowerStr == "yes") {
        value = true;
        return true;
    } else if (lowerStr == "false" || lowerStr == "0" || lowerStr == "no") {
        value = false;
        return true;
    }
    
    return false;
}

void AgentCLI::logCommand(const Command& cmd) const {
    NLM_LOG_INFO("Agent CLI command executed: " + cmd.name);
}

void AgentCLI::printError(const std::string& message) const {
    NLM_LOG_ERROR("Agent CLI error: " + message);
}

std::string AgentCLI::getCommandHelp(const Command& cmd) {
    return cmd.description;
}

} // namespace nlm
