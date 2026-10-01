#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() : brain_(nullptr) {}
    ~CommandLineInterface() = default;
    
    // Initialize CLI with brain reference
    void initialize(Brain* brain) {
        brain_ = brain;
        parseConfig();
        registerCommands();
    }
    
    // Run CLI with given arguments
    void run(int argc, char* argv[]) {
        if (argc < 2) {
            printHelp();
            return;
        }
        
        std::string command = argv[1];
        std::vector<std::string> args(argv + 2, argv + argc);
        
        if (commandMap_.find(command) != commandMap_.end()) {
            commandMap_[command](args);
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            printHelp();
        }
    }
    
    // Print help for all commands
    void printHelp() const {
        std::cout << "NLM Brain Command Line Interface" << std::endl;
        std::cout << "=================================" << std::endl << std::endl;
        
        std::cout << "Available commands:" << std::endl;
        for (const auto& pair : commandDescriptions_) {
            std::cout << "  " << pair.first << " - " << pair.second << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "General commands:" << std::endl;
        std::cout << "  help                    - Show this help message" << std::endl;
        std::cout << "  version                 - Show NLM version" << std::endl;
        std::cout << "  status                  - Show brain system status" << std::endl;
        std::cout << "  config                  - Show current configuration" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Control commands:" << std::endl;
        std::cout << "  start                   - Start brain simulation" << std::endl;
        std::cout << "  stop                    - Stop brain simulation" << std::endl;
        std::cout << "  reset                   - Reset brain to initial state" << std::endl;
        std::cout << "  checkpoint [path]       - Save brain state to checkpoint" << std::endl;
        std::cout << "  restore [path]          - Load brain state from checkpoint" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Memory commands:" << std::endl;
        std::cout << "  memory clear            - Clear all memory systems" << std::endl;
        std::cout << "  memory stats            - Show memory system statistics" << std::endl;
        std::cout << "  memory info [type]      - Show detailed information about specific memory type" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Prediction commands:" << std::endl;
        std::cout << "  predict [steps]         - Make prediction for next steps" << std::endl;
        std::cout << "  prediction error        - Show current prediction error" << std::endl;
        std::cout << "  prediction confidence   - Show prediction confidence" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Neuromodulation commands:" << std::endl;
        std::cout << "  neuromod set <level>    - Set all neuromodulators to level (0.0-1.0)" << std::endl;
        std::cout << "  neuromod status          - Show neuromodulator status" << std::endl;
        std::cout << "  neuromod curiosity       - Show curiosity level" << std::endl;
        std::cout << "  neuromod novelty         - Show novelty level" << std::endl;
        std::cout << "  neuromod prediction_error - Show prediction error level" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Cognition commands:" << std::endl;
        std::cout << "  cognition planner       - Show planning status" << std::endl;
        std::cout << "  cognition attention     - Show attention status" << std::endl;
        std::cout << "  cognition self_model    - Show self model status" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Development commands:" << std::endl;
        std::cout << "  develop                 - Advance developmental stage" << std::endl;
        std::cout << "  development stage       - Show current developmental stage" << std::endl;
        std::cout << "  development info        - Show development system statistics" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Experiment commands:" << std::endl;
        std::cout << "  experiment run [name]   - Run a named experiment" << std::endl;
        std::cout << "  experiment list         - List available experiments" << std::endl;
        std::cout << "  experiment status       - Show current experiment status" << std::endl;
    }
    
    // Show NLM version
    void showVersion() const {
        std::cout << "NLM/熙然 vPhase 6.0" << std::endl;
        std::cout << "Artificial Developmental Brain" << std::endl;
        std::cout << "Built with modern C++20, CMake, and neural dynamics simulation" << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 6 - Final Integration" << std::endl;
        std::cout << "Features: LIF neurons, event-driven spikes, memory systems, prediction, cognition, development" << std::endl;
    }
    
    // Show brain system status
    void showStatus() const {
        if (!brain_) {
            std::cout << "Brain not initialized" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Brain System Status ===" << std::endl;
        std::cout << std::endl;
        
        // Basic stats
        std::cout << "System Information:" << std::endl;
        std::cout << "  Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "  Current step: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "  Excitatory/Inhibitory ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
        
        // Memory systems
        std::cout << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        
        auto* workingMemory = brain_->getWorkingMemory();
        std::cout << "  Working Memory: " << (workingMemory ? "Active" : "Inactive") << std::endl;
        
        auto* episodicMemory = brain_->getEpisodicMemory();
        std::cout << "  Episodic Memory: " << (episodicMemory ? "Active" : "Inactive") << std::endl;
        
        auto* associativeMemory = brain_->getAssociativeMemory();
        std::cout << "  Associative Memory: " << (associativeMemory ? "Active" : "Inactive") << std::endl;
        
        // Prediction system
        std::cout << std::endl;
        std::cout << "Prediction System:" << std::endl;
        auto* predictionSystem = brain_->getPredictionSystem();
        std::cout << "  Prediction System: " << (predictionSystem ? "Active" : "Inactive") << std::endl;
        
        // Neuromodulation
        std::cout << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        
        auto* dopamine = brain_->getDopamine();
        std::cout << "  Dopamine: " << (dopamine ? std::to_string(dopamine->getLevel()) : "Inactive") << std::endl;
        
        auto* curiosity = brain_->getCuriosity();
        std::cout << "  Curiosity: " << (curiosity ? std::to_string(curiosity->getLevel()) : "Inactive") << std::endl;
        
        auto* novelty = brain_->getNovelty();
        std::cout << "  Novelty: " << (novelty ? std::to_string(novelty->getLevel()) : "Inactive") << std::endl;
        
        auto* predictionError = brain_->getPredictionErrorSignal();
        std::cout << "  Prediction Error: " << (predictionError ? std::to_string(predictionError->getError()) : "Inactive") << std::endl;
        
        // Development
        std::cout << std::endl;
        std::cout << "Development:" << std::endl;
        auto stage = brain_->getDevelopmentalStage();
        std::cout << "  Developmental Stage: " << static_cast<int>(stage) << std::endl;
        
        // Performance
        std::cout << std::endl;
        std::cout << "Performance:" << std::endl;
        std::cout << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
    }
    
    // Show configuration
    void showConfig() const {
        if (!brain_) {
            std::cout << "Brain not initialized" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Brain Configuration ===" << std::endl;
        auto config = brain_->getConfig();
        
        // Core parameters
        std::cout << std::endl;
        std::cout << "Core Parameters:" << std::endl;
        std::cout << "  Neuron count: " << config->getOr<size_t>("neuron_count", 0) << std::endl;
        std::cout << "  Region count: " << config->getOr<size_t>("region_count", 0) << std::endl;
        std::cout << "  Connection probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
        std::cout << "  Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.0) << std::endl;
        
        // Memory parameters
        std::cout << std::endl;
        std::cout << "Memory Parameters:" << std::endl;
        std::cout << "  Working memory capacity: " << config->getOr<size_t>("working_memory_capacity", 0) << std::endl;
        std::cout << "  Episodic memory capacity: " << config->getOr<size_t>("episodic_memory_capacity", 0) << std::endl;
        std::cout << "  Associative memory capacity: " << config->getOr<size_t>("associative_memory_capacity", 0) << std::endl;
        
        // Prediction parameters
        std::cout << std::endl;
        std::cout << "Prediction Parameters:" << std::endl;
        std::cout << "  Prediction horizon: " << config->getOr<size_t>("prediction_horizon", 0) << std::endl;
        std::cout << "  Sequence memory size: " << config->getOr<size_t>("sequence_memory_size", 0) << std::endl;
        
        // Neuromodulation parameters
        std::cout << std::endl;
        std::cout << "Neuromodulation Parameters:" << std::endl;
        std::cout << "  Dopamine baseline: " << config->getOr<float>("dopamine_baseline", 0.0f) << std::endl;
        std::cout << "  Curiosity weight: " << config->getOr<float>("curiosity_weight", 0.0f) << std::endl;
        std::cout << "  Novelty weight: " << config->getOr<float>("novelty_weight", 0.0f) << std::endl;
        std::cout << "  Prediction error weight: " << config->getOr<float>("prediction_error_weight", 0.0f) << std::endl;
        
        // Development parameters
        std::cout << std::endl;
        std::cout << "Development Parameters:" << std::endl;
        std::cout << "  Development rate: " << config->getOr<float>("development_rate", 0.0f) << std::endl;
        std::cout << "  Plasticity decay rate: " << config->getOr<float>("plasticity_decay_rate", 0.0f) << std::endl;
    }
    
private:
    void parseConfig() {
        // Parse configuration and set up defaults
        if (!config_) {
            config_ = std::make_shared<Config>();
            // Set default values
            config_->set<float>("neuron_count", 1000);
            config_->set<size_t>("region_count", 1);
            config_->set<float>("connection_probability", 0.1f);
            config_->set<double>("simulation_timestep", 0.001);
            config_->set<size_t>("working_memory_capacity", 100);
            config_->set<size_t>("episodic_memory_capacity", 1000);
            config_->set<size_t>("associative_memory_capacity", 5000);
            config_->set<size_t>("prediction_horizon", 10);
            config_->set<size_t>("sequence_memory_size", 100);
            config_->set<float>("dopamine_baseline", 0.1f);
            config_->set<float>("curiosity_weight", 0.5f);
            config_->set<float>("novelty_weight", 0.3f);
            config_->set<float>("prediction_error_weight", 0.2f);
            config_->set<float>("development_rate", 0.01f);
            config_->set<float>("plasticity_decay_rate", 0.001f);
        }
    }
    
    void registerCommands() {
        // Register all commands with descriptions
        commandMap_ = {
            {"help", [this](const std::vector<std::string>&) { printHelp(); }},
            {"version", [this](const std::vector<std::string>&) { showVersion(); }},
            {"status", [this](const std::vector<std::string>&) { showStatus(); }},
            {"config", [this](const std::vector<std::string>&) { showConfig(); }},
        };
        
        commandDescriptions_ = {
            {"help", "Show help message"},
            {"version", "Show NLM version information"},
            {"status", "Show current brain system status"},
            {"config", "Show current configuration"},
        };
    }
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commandMap_;
    std::unordered_map<std::string, std::string> commandDescriptions_;
};

} // namespace nlm