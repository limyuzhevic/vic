#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

using namespace nlm;

class CommandLineInterface {
public:
    CommandLineInterface() : running_(false), brain_(nullptr) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void run() {
        printWelcome();
        running_ = true;
        
        while (running_) {
            printPrompt();
            std::string command;
            std::getline(std::cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            handleCommand(command);
        }
    }
    
    void stop() {
        running_ = false;
    }
    
private:
    void printWelcome() {
        std::cout << "==============================================" << std::endl;
        std::cout << "  NLM — 熙然  Command Line Interface" << std::endl;
        std::cout << "  Neural Learning Machine - Phase 6" << std::endl;
        std::cout << "==============================================" << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl << std::endl;
    }
    
    void printPrompt() {
        std::cout << ">>> ";
    }
    
    void handleCommand(const std::string& command) {
        std::vector<std::string> tokens = tokenize(command);
        if (tokens.empty()) {
            return;
        }
        
        std::string cmd = toLower(tokens[0]);
        
        if (cmd == "help") {
            showHelp(tokens.size() > 1 ? tokens[1] : "");
        } else if (cmd == "exit" || cmd == "quit") {
            stop();
        } else if (cmd == "status") {
            showStatus();
        } else if (cmd == "init") {
            initBrain(tokens.size() > 1 ? tokens[1] : "");
        } else if (cmd == "save") {
            saveBrain(tokens.size() > 1 ? tokens[1] : "");
        } else if (cmd == "load") {
            loadBrain(tokens.size() > 1 ? tokens[1] : "");
        } else if (cmd == "step") {
            runStep(tokens.size() > 1 ? stoi(tokens[1]) : 1);
        } else if (cmd == "run") {
            runExperiment(tokens.size() > 1 ? stoi(tokens[1]) : 1000);
        } else if (cmd == "config") {
            showConfig();
        } else if (cmd == "set") {
            setConfig(tokens);
        } else if (cmd == "memories") {
            showMemories();
        } else if (cmd == "neuromod") {
            showNeuromodulation();
        } else if (cmd == "develop") {
            developBrain();
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    std::vector<std::string> tokenize(const std::string& command) {
        std::vector<std::string> tokens;
        std::string token;
        for (char c : command) {
            if (std::isspace(c)) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        for (char& c : result) {
            c = std::tolower(c);
        }
        return result;
    }
    
    void showHelp(const std::string& topic) {
        if (topic.empty()) {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  help [topic]      - Show help for a specific topic" << std::endl;
            std::cout << "  exit|quit         - Exit the program" << std::endl;
            std::cout << "  status             - Show current brain status" << std::endl;
            std::cout << "  init [config]      - Initialize brain with config file" << std::endl;
            std::cout << "  save [filename]    - Save brain state to file" << std::endl;
            std::cout << "  load [filename]    - Load brain state from file" << std::endl;
            std::cout << "  step [n]           - Run n simulation steps" << std::endl;
            std::cout << "  run [steps]        - Run integration experiment" << std::endl;
            std::cout << "  config             - Show current configuration" << std::endl;
            std::cout << "  set key value       - Set configuration value" << std::endl;
            std::cout << "  memories            - Show memory system status" << std::endl;
            std::cout << "  neuromod            - Show neuromodulation status" << std::endl;
            std::cout << "  develop            - Advance development stage" << std::endl;
            std::cout << "" << std::endl;
        } else if (topic == "config") {
            std::cout << "Configuration settings:" << std::endl;
            std::cout << "  neuron_count - Number of neurons (default: 1000)" << std::endl;
            std::cout << "  region_count - Number of brain regions (default: 1)" << std::endl;
            std::cout << "  connection_probability - Synapse connection probability (default: 0.1)" << std::endl;
            std::cout << "  stdp_ltp_weight - STDP LTP weight (default: 0.01)" << std::endl;
            std::cout << "  stdp_ltd_weight - STDP LTD weight (default: 0.012)" << std::endl;
            std::cout << "  synaptogenesis_rate - Structural plasticity rate (default: 0.0001)" << std::endl;
            std::cout << "  pruning_rate - Structural plasticity pruning rate (default: 0.00001)" << std::endl;
        } else if (topic == "experiment") {
            std::cout << "Experiment commands:" << std::endl;
            std::cout << "  run [steps] - Run Phase 6 integration experiment" << std::endl;
            std::cout << "    Runs the complete integration test with memory, neuromodulation, " << std::endl;
            std::cout << "    prediction, cognition, and development systems." << std::endl;
        }
    }
    
    void showStatus() {
        if (!brain_) {
            std::cout << "Brain not initialized. Use 'init' command to create brain." << std::endl;
            return;
        }
        
        std::cout << "=== NLM Brain Status ===" << std::endl;
        brain_->logStatus();
    }
    
    void initBrain(const std::string& configFile) {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            std::cout << "Loading configuration from: " << configFile << std::endl;
            if (!config->loadFromFile(configFile)) {
                std::cout << "Warning: Could not load config file, using defaults." << std::endl;
            }
        }
        
        // Set some defaults for Phase 6
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        brain_ = std::make_shared<Brain>(config);
        if (brain_->initialize()) {
            std::cout << "Brain initialized successfully!" << std::endl;
            std::cout << "Neurons: " << brain_->getTotalNeuronCount() << std::endl;
            std::cout << "Regions: " << brain_->getRegionCount() << std::endl;
            std::cout << "Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        } else {
            std::cout << "Failed to initialize brain." << std::endl;
            brain_.reset();
        }
    }
    
    void saveBrain(const std::string& filename) {
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        std::string path = filename.empty() ? "/tmp/nlm_brain_checkpoint.bin" : filename;
        if (brain_->save(path)) {
            std::cout << "Brain saved successfully to: " << path << std::endl;
        } else {
            std::cout << "Failed to save brain." << std::endl;
        }
    }
    
    void loadBrain(const std::string& filename) {
        if (!brain_) {
            std::cout << "Brain not initialized. Use 'init' command first." << std::endl;
            return;
        }
        
        std::string path = filename.empty() ? "/tmp/nlm_brain_checkpoint.bin" : filename;
        if (brain_->load(path)) {
            std::cout << "Brain loaded successfully from: " << path << std::endl;
        } else {
            std::cout << "Failed to load brain." << std::endl;
        }
    }
    
    void runStep(int steps) {
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain_->step(i, i * 0.001);
            if ((i + 1) % 100 == 0) {
                std::cout << "  Step " << (i + 1) << "/" << steps 
                         << " | Spikes: " << brain_->getTotalSpikeCount()
                         << " | Firing: " << brain_->getFiringNeuronCount() << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "Completed " << steps << " steps in " << duration.count() << " ms" << std::endl;
        std::cout << "Final spikes: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
    }
    
    void runExperiment(int maxSteps) {
        if (!brain_) {
            std::cout << "Brain not initialized. Use 'init' command first." << std::endl;
            return;
        }
        
        std::cout << "Running Phase 6 integration experiment with " << maxSteps << " steps..." << std::endl;
        
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = maxSteps;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "/tmp/nlm_experiment_checkpoint.bin";
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        std::cout << "\n=== Experiment Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall time: " << result.totalWallClockTime << "s" << std::endl;
        
        std::cout << "\n=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    }
    
    void showConfig() {
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        auto config = brain_->getConfig();
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << "neuron_count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
        std::cout << "region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
        std::cout << "stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
        std::cout << "stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
        std::cout << "synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
        std::cout << "pruning_rate: " << config->getOr<float>("pruning_rate", 0.00001f) << std::endl;
        std::cout << "replay_interval: " << config->getOr<size_t>("replay_interval", 100) << std::endl;
        std::cout << "consolidation_interval: " << config->getOr<size_t>("consolidation_interval", 1000) << std::endl;
    }
    
    void setConfig(const std::vector<std::string>& tokens) {
        if (tokens.size() < 3) {
            std::cout << "Usage: set key value" << std::endl;
            return;
        }
        
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        std::string key = tokens[1];
        std::string value = tokens[2];
        
        // Try to parse as different types
        auto config = brain_->getConfig();
        
        // Try float first
        try {
            float floatVal = std::stof(value);
            config->set(key, floatVal, ConfigSource::Runtime);
            std::cout << "Set " << key << " = " << floatVal << std::endl;
            return;
        } catch (...) {}
        
        // Try int64
        try {
            int64_t intVal = std::stoll(value);
            config->set(key, intVal, ConfigSource::Runtime);
            std::cout << "Set " << key << " = " << intVal << std::endl;
            return;
        } catch (...) {}
        
        // Try bool
        if (value == "true") {
            config->set(key, true, ConfigSource::Runtime);
            std::cout << "Set " << key << " = true" << std::endl;
            return;
        } else if (value == "false") {
            config->set(key, false, ConfigSource::Runtime);
            std::cout << "Set " << key << " = false" << std::endl;
            return;
        }
        
        // Try string
        config->set(key, value, ConfigSource::Runtime);
        std::cout << "Set " << key << " = '" << value << "'" << std::endl;
    }
    
    void showMemories() {
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        auto* wm = brain_->getWorkingMemory();
        auto* em = brain_->getEpisodicMemory();
        auto* am = brain_->getAssociativeMemory();
        
        std::cout << "=== Memory System Status ===" << std::endl;
        
        if (wm) {
            std::cout << "Working Memory: " << wm->getActiveTraces() << " active traces" << std::endl;
            std::cout << "  Capacity: " << wm->getCapacity() << std::endl;
        } else {
            std::cout << "Working Memory: NOT CONNECTED" << std::endl;
        }
        
        if (em) {
            std::cout << "Episodic Memory: " << em->getEpisodeCount() << " episodes" << std::endl;
            std::cout << "  Max episodes: " << em->getMaxEpisodes() << std::endl;
            auto episodes = em->getEpisodesForReplay(3);
            std::cout << "  Available for replay: " << episodes.size() << std::endl;
        } else {
            std::cout << "Episodic Memory: NOT CONNECTED" << std::endl;
        }
        
        if (am) {
            std::cout << "Associative Memory: CONNECTED" << std::endl;
        } else {
            std::cout << "Associative Memory: NOT CONNECTED" << std::endl;
        }
    }
    
    void showNeuromodulation() {
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        auto* dopamine = brain_->getDopamine();
        auto* curiosity = brain_->getCuriosity();
        auto* novelty = brain_->getNovelty();
        auto* predictionError = brain_->getPredictionErrorSignal();
        
        std::cout << "=== Neuromodulation Status ===" << std::endl;
        
        if (dopamine) {
            std::cout << "Dopamine: " << dopamine->getLevel() << " (plasticity factor: " << dopamine->getPlasticityFactor() << ")" << std::endl;
        } else {
            std::cout << "Dopamine: NOT CONNECTED" << std::endl;
        }
        
        if (curiosity) {
            std::cout << "Curiosity: " << curiosity->getLevel() << std::endl;
        } else {
            std::cout << "Curiosity: NOT CONNECTED" << std::endl;
        }
        
        if (novelty) {
            std::cout << "Novelty: " << novelty->getLevel() << std::endl;
        } else {
            std::cout << "Novelty: NOT CONNECTED" << std::endl;
        }
        
        if (predictionError) {
            std::cout << "Prediction Error: " << predictionError->getLevel() << std::endl;
        } else {
            std::cout << "Prediction Error: NOT CONNECTED" << std::endl;
        }
    }
    
    void developBrain() {
        if (!brain_) {
            std::cout << "Brain not initialized." << std::endl;
            return;
        }
        
        brain_->develop();
        std::cout << "Development stage advanced." << std::endl;
        std::cout << "Current stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    }
    
    // CommandLineInterface(const CommandLineInterface&) = delete;
    // CommandLineInterface& operator=(const CommandLineInterface&) = delete;
};
