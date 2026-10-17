// NLM Command Line Interface
// Phase 6: Brain Control and Debugging Interface

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>

using namespace nlm;

class CommandLineInterface {
public:
    CommandLineInterface() : brain_(nullptr), config_(nullptr), running_(true) {}
    
    void run() {
        printWelcome();
        initializeBrain();
        
        while (running_) {
            printPrompt();
            std::string line;
            std::getline(std::cin, line);
            
            if (line.empty()) continue;
            
            processCommand(line);
        }
        
        printGoodbye();
    }
    
private:
    void printWelcome() {
        std::cout << "========================================\n";
        std::cout << "NLM Brain Control Interface (Phase 6)\n";
        std::cout << "========================================\n";
        std::cout << "Commands:\n";
        std::cout << "  help - Show this help message\n";
        std::cout << "  status - Show current brain status\n";
        std::cout << "  step <count> - Run simulation for N steps\n";
        std::cout << "  sensory <values> - Inject sensory input\n";
        std::cout << "  save <filename> - Save brain state to file\n";
        std::cout << "  load <filename> - Load brain state from file\n";
        std::cout << "  memory - Show memory system status\n";
        std::cout << "  neuromod - Show neuromodulation levels\n";
        std::cout << "  quit - Exit interface\n";
        std::cout << "\n";
    }
    
    void initializeBrain() {
        std::cout << "Initializing brain...\n";
        
        // Create configuration
        config_ = std::make_shared<Config>();
        
        // Load default config
        if (!config_->loadFromFile("configs/default.cfg")) {
            std::cout << "Warning: Could not load config file, using defaults.\n";
        }
        
        // Set NLM-specific defaults for Phase 6
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config_->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Initialize brain
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            std::cerr << "Error: Failed to initialize brain!\n";
            running_ = false;
            return;
        }
        
        std::cout << "Brain initialized successfully.\n";
        std::cout << "Type 'help' for available commands.\n\n";
    }
    
    void printPrompt() {
        std::cout << ">>> ";
        std::cout.flush();
    }
    
    void processCommand(const std::string& line) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        if (command == "help") {
            showHelp();
        } else if (command == "status") {
            showStatus();
        } else if (command == "step") {
            int steps = 1;
            if (iss >> steps) {
                runSteps(steps);
            } else {
                std::cout << "Error: Please specify number of steps.\n";
            }
        } else if (command == "sensory") {
            processSensoryCommand(iss);
        } else if (command == "save") {
            processSaveCommand(iss);
        } else if (command == "load") {
            processLoadCommand(iss);
        } else if (command == "memory") {
            showMemoryStatus();
        } else if (command == "neuromod") {
            showNeuromodStatus();
        } else if (command == "quit") {
            running_ = false;
        } else {
            std::cout << "Error: Unknown command: " << command << "\n";
            std::cout << "Type 'help' for available commands.\n";
        }
    }
    
    void showHelp() {
        std::cout << "\nBrain Control Commands:\n";
        std::cout << "  status - Show current brain status and statistics\n";
        std::cout << "  step <N> - Run simulation for N steps (default: 1)\n";
        std::cout << "  sensory <v1> <v2> ... - Inject sensory values (comma-separated)\n";
        std::cout << "  save <file> - Save brain state to file\n";
        std::cout << "  load <file> - Load brain state from file\n";
        std::cout << "  memory - Show memory system information\n";
        std::cout << "  neuromod - Show neuromodulator levels\n";
        std::cout << "  quit - Exit the interface\n\n";
    }
    
    void showStatus() {
        std::cout << "\n=== Brain Status ===\n";
        brain_->logStatus();
        std::cout << "\n";
    }
    
    void runSteps(int steps) {
        if (steps <= 0) {
            std::cout << "Error: Number of steps must be positive.\n";
            return;
        }
        
        std::cout << "Running " << steps << " simulation steps...\n";
        
        for (int i = 0; i < steps; ++i) {
            brain_->step(i, i * config_->getOr<double>("simulation_timestep", 0.001));
            
            if (i % 100 == 0) {
                std::cout << "  Step " << i << ": " 
                          << brain_->getTotalSpikeCount() << " spikes, "
                          << brain_->getFiringNeuronCount() << " firing neurons\n";
            }
        }
        
        std::cout << "Completed " << steps << " steps.\n";
    }
    
    void processSensoryCommand(std::istringstream& iss) {
        std::string token;
        std::vector<float> values;
        
        while (iss >> token) {
            // Handle comma-separated values
            std::replace(token.begin(), token.end(), ',', ' ');
            std::istringstream valStream(token);
            float value;
            while (valStream >> value) {
                values.push_back(value);
            }
        }
        
        if (values.empty()) {
            std::cout << "Error: No sensory values provided.\n";
            return;
        }
        
        // Create sensory input
        SensoryInput input(values);
        
        // Inject into brain
        brain_->receiveSensoryInput(input);
        
        std::cout << "Sensory input injected: ";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << values[i];
        }
        std::cout << "\n";
    }
    
    void processSaveCommand(std::istringstream& iss) {
        std::string filename;
        if (iss >> filename) {
            if (brain_->save(filename)) {
                std::cout << "Brain state saved to: " << filename << "\n";
            } else {
                std::cout << "Error: Failed to save brain state.\n";
            }
        } else {
            std::cout << "Error: Please specify filename.\n";
        }
    }
    
    void processLoadCommand(std::istringstream& iss) {
        std::string filename;
        if (iss >> filename) {
            if (brain_->load(filename)) {
                std::cout << "Brain state loaded from: " << filename << "\n";
            } else {
                std::cout << "Error: Failed to load brain state.\n";
            }
        } else {
            std::cout << "Error: Please specify filename.\n";
        }
    }
    
    void showMemoryStatus() {
        std::cout << "\n=== Memory System Status ===\n";
        
        if (auto* workingMemory = brain_->getWorkingMemory()) {
            std::cout << "Working Memory:\n";
            std::cout << "  Active traces: " << workingMemory->getActiveTraces() << "\n";
            std::cout << "  Capacity: " << workingMemory->getCapacity() << "\n";
            std::cout << "  Memory neurons: " << workingMemory->getMemoryNeurons().size() << "\n";
            std::cout << "  Memory activity: " << workingMemory->getMemoryActivity() << "\n";
        } else {
            std::cout << "Working Memory: Not available\n";
        }
        
        if (auto* episodicMemory = brain_->getEpisodicMemory()) {
            std::cout << "Episodic Memory:\n";
            std::cout << "  Episodes: " << episodicMemory->getEpisodeCount() << "\n";
            std::cout << "  Average reward: " << episodicMemory->getAverageReward() << "\n";
            std::cout << "  Replay enabled: " << (episodicMemory->isReplayEnabled() ? "Yes" : "No") << "\n";
        } else {
            std::cout << "Episodic Memory: Not available\n";
        }
        
        if (auto* associativeMemory = brain_->getAssociativeMemory()) {
            std::cout << "Associative Memory:\n";
            std::cout << "  Associations: " << associativeMemory->getAssociationCount() << "\n";
        } else {
            std::cout << "Associative Memory: Not available\n";
        }
        
        std::cout << "\n";
    }
    
    void showNeuromodStatus() {
        std::cout << "\n=== Neuromodulation System Status ===\n";
        
        if (auto* dopamine = brain_->getDopamine()) {
            std::cout << "Dopamine:\n";
            std::cout << "  Level: " << dopamine->getLevel() << "\n";
            std::cout << "  Plasticity factor: " << dopamine->getPlasticityFactor() << "\n";
        } else {
            std::cout << "Dopamine: Not available\n";
        }
        
        if (auto* curiosity = brain_->getCuriosity()) {
            std::cout << "Curiosity:\n";
            std::cout << "  Level: " << curiosity->getLevel() << "\n";
            std::cout << "  Exploration drive: " << curiosity->getExplorationDrive() << "\n";
        } else {
            std::cout << "Curiosity: Not available\n";
        }
        
        if (auto* novelty = brain_->getNovelty()) {
            std::cout << "Novelty:\n";
            std::cout << "  Level: " << novelty->getLevel() << "\n";
        } else {
            std::cout << "Novelty: Not available\n";
        }
        
        if (auto* predictionError = brain_->getPredictionErrorSignal()) {
            std::cout << "Prediction Error:\n";
            std::cout << "  Error: " << predictionError->getError() << "\n";
            std::cout << "  Magnitude: " << predictionError->getMagnitude() << "\n";
        } else {
            std::cout << "Prediction Error: Not available\n";
        }
        
        std::cout << "\n";
    }
    
    void printGoodbye() {
        std::cout << "\nGoodbye! Brain interface closed.\n";
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    bool running_;
};

int main(int argc, char** argv) {
    CommandLineInterface cli;
    
    // Parse any command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Brain Control Interface - Phase 6\n";
            std::cout << "Usage: nlm_brain [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --help, -h  Show this help message\n";
            std::cout << "  --config <file>  Load configuration from file\n";
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            
            // Note: This would require modifying the CommandLineInterface
            // to accept a config file parameter
            std::cout << "Config file: " << configFile << " (Note: CLI doesn't support config files yet)\n";
        }
    }
    
    cli.run();
    return 0;
}
