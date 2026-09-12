// Command-line Interface for NLM
// Provides command-line access to NLM functionality

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() = default;
    
    // Parse and execute command-line arguments
    bool parseAndExecute(int argc, char** argv) {
        std::vector<std::string> args(argv + 1, argv + argc);
        return executeCommands(args);
    }
    
    // Execute a sequence of commands
    bool executeCommands(const std::vector<std::string>& commands) {
        for (const auto& cmd : commands) {
            if (!executeCommand(cmd)) {
                return false;
            }
        }
        return true;
    }
    
    // Execute a single command
    bool executeCommand(const std::string& cmd) {
        std::istringstream iss(cmd);
        std::string token;
        iss >> token;
        
        if (token.empty()) return true;
        
        // Command dispatch
        if (token == "help" || token == "?") {
            printHelp();
        } else if (token == "version") {
            printVersion();
        } else if (token == "config") {
            handleConfigCommand(iss);
        } else if (token == "brain") {
            handleBrainCommand(iss);
        } else if (token == "world") {
            handleWorldCommand(iss);
        } else if (token == "agent") {
            handleAgentCommand(iss);
        } else if (token == "neuron") {
            handleNeuronCommand(iss);
        } else if (token == "synapse") {
            handleSynapseCommand(iss);
        } else if (token == "run") {
            handleRunCommand(iss);
        } else if (token == "save") {
            handleSaveCommand(iss);
        } else if (token == "load") {
            handleLoadCommand(iss);
        } else if (token == "stats") {
            handleStatsCommand();
        } else if (token == "quit" || token == "exit") {
            return false;  // Exit
        } else {
            std::cerr << "Unknown command: " << token << std::endl;
            std::cerr << "Type 'help' for available commands" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Set current brain instance
    void setBrain(std::shared_ptr<Brain> brain) {
        currentBrain_ = brain;
    }
    
    // Set current world instance
    void setWorld(std::shared_ptr<SimpleWorld> world) {
        currentWorld_ = world;
    }
    
    // Set current agent instance
    void setAgent(std::shared_ptr<AgentBrain> agent) {
        currentAgent_ = agent;
    }
    
    // Run interactive CLI
    void runInteractive() {
        std::string line;
        std::cout << "NLM Command Line Interface - Type 'help' for commands" << std::endl;
        
        while (std::getline(std::cin, line)) {
            if (!line.empty()) {
                if (!executeCommand(line)) {
                    break;  // Exit on error
                }
            }
        }
        
        std::cout << "Exiting NLM CLI" << std::endl;
    }
    
private:
    std::shared_ptr<Brain> currentBrain_;
    std::shared_ptr<SimpleWorld> currentWorld_;
    std::shared_ptr<AgentBrain> currentAgent_;
    
    void printHelp() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Config Commands:" << std::endl;
        std::cout << "  config show           - Show current configuration" << std::endl;
        std::cout << "  config set <key> <value> - Set configuration value" << std::endl;
        std::cout << "  config load <file>   - Load configuration from file" << std::endl;
        std::cout << "  config save <file>   - Save configuration to file" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Brain Commands:" << std::endl;
        std::cout << "  brain init [neuron_count] - Initialize brain" << std::endl;
        std::cout << "  brain step <steps>     - Run simulation steps" << std::endl;
        std::cout << "  brain reset            - Reset brain state" << std::endl;
        std::cout << "  brain status           - Show brain statistics" << std::endl;
        std::cout << "  brain info             - Show brain details" << std::endl;
        std::cout << std::endl;
        
        std::cout << "World Commands:" << std::endl;
        std::cout << "  world init [width] [height] - Initialize world" << std::endl;
        std::cout << "  world update <time>    - Update world" << std::endl;
        std::cout << "  world reset            - Reset world" << std::endl;
        std::cout << "  world status           - Show world status" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Agent Commands:" << std::endl;
        std::cout << "  agent init             - Initialize agent" << std::endl;
        std::cout << "  agent enable <feature>  - Enable agent feature" << std::endl;
        std::cout << "  agent disable <feature> - Disable agent feature" << std::endl;
        std::cout << "  agent status           - Show agent status" << std::endl;
        std::endl
        
        std::cout << "Neuron Commands:" << std::endl;
        std::cout << "  neuron list [type]     - List neurons by type" << std::endl;
        std::cout << "  neuron inject <id> <current> - Inject current into neuron" << std::endl;
        std::cout << "  neuron stats <id>      - Get neuron statistics" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Synapse Commands:" << std::endl;
        std::cout << "  synapse list [type]    - List synapses by type" << std::endl;
        std::cout << "  synapse stats <id>     - Get synapse statistics" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Execution Commands:" << std::endl;
        std::cout << "  run <steps> [time]    - Run simulation" << std::endl;
        std::cout << "  save <file>            - Save brain state" << std::endl;
        std::cout << "  load <file>            - Load brain state" << std::endl;
        std::cout << "  stats                  - Show system statistics" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Control Commands:" << std::endl;
        std::cout << "  help, ?               - Show this help" << std::endl;
        std::cout << "  version               - Show version info" << std::endl;
        std::cout << "  quit, exit            - Exit" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM Phase 2 - Version 0.1.0" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    }
    
    void handleConfigCommand(std::istringstream& iss) {
        std::string subcommand;
        iss >> subcommand;
        
        if (subcommand == "show") {
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                std::cout << "=== Configuration ===" << std::endl;
                for (const auto& key : config->getKeys()) {
                    std::cout << key << " = " << config->summary() << std::endl;
                }
            } else {
                std::cout << "No brain initialized" << std::endl;
            }
        } else if (subcommand == "set") {
            std::string key, value;
            iss >> key >> value;
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                // Try to determine value type
                try {
                    size_t pos;
                    int intVal = std::stoi(value);
                    config->set(key, intVal);
                } catch (...) {
                    try {
                        float floatVal = std::stof(value);
                        config->set(key, floatVal);
                    } catch (...) {
                        config->set(key, value);
                    }
                }
                std::cout << "Set " << key << " = " << value << std::endl;
            }
        } else if (subcommand == "load") {
            std::string filename;
            iss >> filename;
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                if (config->loadFromFile(filename)) {
                    std::cout << "Loaded configuration from " << filename << std::endl;
                } else {
                    std::cerr << "Failed to load configuration from " << filename << std::endl;
                }
            }
        } else if (subcommand == "save") {
            std::string filename;
            iss >> filename;
            if (currentBrain_) {
                auto config = currentBrain_->getConfig();
                if (config->saveToFile(filename)) {
                    std::cout << "Saved configuration to " << filename << std::endl;
                } else {
                    std::cerr << "Failed to save configuration to " << filename << std::endl;
                }
            }
        } else {
            std::cerr << "Unknown config command: " << subcommand << std::endl;
        }
    }
    
    void handleBrainCommand(std::istringstream& iss) {
        std::string subcommand;
        iss >> subcommand;
        
        if (subcommand == "init") {
            int neuronCount = 100;
            iss >> neuronCount;
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            currentBrain_ = std::make_shared<Brain>(config);
            if (currentBrain_->initialize()) {
                std::cout << "Brain initialized with " << neuronCount << " neurons" << std::endl;
            } else {
                std::cerr << "Failed to initialize brain" << std::endl;
            }
        } else if (subcommand == "step") {
            int steps;
            iss >> steps;
            if (currentBrain_) {
                for (int i = 0; i < steps; ++i) {
                    currentBrain_->step(i);
                }
                std::cout << "Ran " << steps << " simulation steps" << std::endl;
            }
        } else if (subcommand == "reset") {
            if (currentBrain_) {
                currentBrain_->reset();
                std::cout << "Brain reset" << std::endl;
            }
        } else if (subcommand == "status") {
            handleStatsCommand();
        } else if (subcommand == "info") {
            if (currentBrain_) {
                currentBrain_->logStatus();
            }
        } else {
            std::cerr << "Unknown brain command: " << subcommand << std::endl;
        }
    }
    
    void handleWorldCommand(std::istringstream& iss) {
        std::string subcommand;
        iss >> subcommand;
        
        if (subcommand == "init") {
            int width = 20, height = 20;
            iss >> width >> height;
            currentWorld_ = std::make_shared<SimpleWorld>();
            currentWorld_->configure(width, height, 8, 8);
            currentWorld_->reset();
            std::cout << "World initialized " << width << "x" << height << std::endl;
        } else if (subcommand == "update") {
            double time;
            iss >> time;
            if (currentWorld_) {
                currentWorld_->update(time);
                std::cout << "World updated by " << time << " seconds" << std::endl;
            }
        } else if (subcommand == "reset") {
            if (currentWorld_) {
                currentWorld_->reset();
                std::cout << "World reset" << std::endl;
            }
        } else if (subcommand == "status") {
            handleStatsCommand();
        } else {
            std::cerr << "Unknown world command: " << subcommand << std::endl;
        }
    }
    
    void handleAgentCommand(std::istringstream& iss) {
        std::string subcommand;
        iss >> subcommand;
        
        if (subcommand == "init") {
            if (!currentBrain_ || !currentWorld_) {
                std::cerr << "Need both brain and world initialized" << std::endl;
                return;
            }
            
            currentAgent_ = std::make_shared<AgentBrain>(currentBrain_);
            currentAgent_->initialize(*currentWorld_);
            std::cout << "Agent initialized" << std::endl;
        } else if (subcommand == "enable") {
            std::string feature;
            iss >> feature;
            if (currentAgent_) {
                if (feature == "reward") {
                    currentAgent_->enableRewardModulation(true);
                } else if (feature == "plasticity") {
                    currentAgent_->enableStructuralPlasticity(true);
                } else if (feature == "development") {
                    currentAgent_->enableDevelopment(true);
                } else if (feature == "curiosity") {
                    currentAgent_->enableCuriosity(true);
                }
                std::cout << "Enabled " << feature << " for agent" << std::endl;
            }
        } else if (subcommand == "disable") {
            std::string feature;
            iss >> feature;
            if (currentAgent_) {
                if (feature == "reward") {
                    currentAgent_->enableRewardModulation(false);
                } else if (feature == "plasticity") {
                    currentAgent_->enableStructuralPlasticity(false);
                } else if (feature == "development") {
                    currentAgent_->enableDevelopment(false);
                } else if (feature == "curiosity") {
                    currentAgent_->enableCuriosity(false);
                }
                std::cout << "Disabled " << feature << " for agent" << std::endl;
            }
        } else if (subcommand == "status") {
            handleStatsCommand();
        } else {
            std::cerr << "Unknown agent command: " << subcommand << std::endl;
        }
    }
    
    void handleNeuronCommand(std::istringstream& iss) {
        std::string subcommand;
        iss >> subcommand;
        
        if (subcommand == "list") {
            std::string typeStr;
            iss >> typeStr;
            if (currentBrain_) {
                std::cout << "Neurons:" << std::endl;
                for (auto* region : currentBrain_->getRegions()) {
                    for (auto* neuron : region->getAllNeurons()) {
                        std::cout << "  Neuron " << neuron->getId().value 
                                 << " (type: " << static_cast<int>(neuron->getType()) << ")" << std::endl;
                    }
                }
            }
        } else if (subcommand == "inject") {
            int id, current;
            iss >> id >> current;
            if (currentBrain_) {
                // Find and inject current
                for (auto* region : currentBrain_->getRegions()) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId().value == static_cast<uint64_t>(id)) {
                            neuron->injectCurrent(current);
                            std::cout << "Injected " << current << " into neuron " << id << std::endl;
                            return;
                        }
                    }
                }
                std::cerr << "Neuron " << id << " not found" << std::endl;
            }
        } else if (subcommand == "stats") {
            int id;
            iss >> id;
            if (currentBrain_) {
                // Find neuron and show stats
                for (auto* region : currentBrain_->getRegions()) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuron->getId().value == static_cast<uint64_t>(id)) {
                            std::cout << "Neuron " << id << " stats:" << std::endl;
                            std::cout << "  Type: " << static_cast<int>(neuron->getType()) << std::endl;
                            std::cout << "  V: " << neuron->getMembranePotential() << std::endl;
                            std::cout << "  Threshold: " << neuron->getThreshold() << std::endl;
                            std::cout << "  Current: " << neuron->getTotalCurrent() << std::endl;
                            return;
                        }
                    }
                }
                std::cerr << "Neuron " << id << " not found" << std::endl;
            }
        } else {
            std::cerr << "Unknown neuron command: " << subcommand << std::endl;
        }
    }
    
    void handleSynapseCommand(std::istringstream& iss) {
        std::string subcommand;
        iss >> subcommand;
        
        if (subcommand == "list") {
            std::string typeStr;
            iss >> typeStr;
            if (currentBrain_) {
                std::cout << "Synapses:" << std::endl;
                for (auto* region : currentBrain_->getRegions()) {
                    for (auto* synapse : region->getSynapses()) {
                        std::cout << "  Synapse " << synapse->getId().value 
                                 << " (type: " << static_cast<int>(synapse->getType()) << ")" 
                                 << " weight: " << synapse->getWeight() << std::endl;
                    }
                }
            }
        } else if (subcommand == "stats") {
            int id;
            iss >> id;
            if (currentBrain_) {
                // Find synapse and show stats
                for (auto* region : currentBrain_->getRegions()) {
                    for (auto* synapse : region->getSynapses()) {
                        if (synapse->getId().value == static_cast<uint64_t>(id)) {
                            std::cout << "Synapse " << id << " stats:" << std::endl;
                            std::cout << "  Type: " << static_cast<int>(synapse->getType()) << std::endl;
                            std::cout << "  Weight: " << synapse->getWeight() << std::endl;
                            std::cout << "  Pre: " << synapse->getSourceNeuronId().value << std::endl;
                            std::cout << "  Post: " << synapse->getTargetNeuronId().value << std::endl;
                            return;
                        }
                    }
                }
                std::cerr << "Synapse " << id << " not found" << std::endl;
            }
        } else {
            std::cerr << "Unknown synapse command: " << subcommand << std::endl;
        }
    }
    
    void handleRunCommand(std::istringstream& iss) {
        int steps;
        double time = 0.1;
        iss >> steps;
        iss >> time;
        
        if (currentAgent_ && currentWorld_) {
            for (int i = 0; i < steps; ++i) {
                currentWorld_->update(time);
                auto percept = currentWorld_->getSensoryPercept();
                if (percept) {
                    currentAgent_->processSensoryInput(*percept);
                }
                currentBrain_->step(i);
                auto action = currentAgent_->decodeMotorCommand();
                if (action) {
                    currentWorld_->applyMotorCommand(*action, currentWorld_->getSimulationTime());
                }
            }
            std::cout << "Ran " << steps << " steps (time step: " << time << ")" << std::endl;
        }
    }
    
    void handleSaveCommand(std::istringstream& iss) {
        std::string filename;
        iss >> filename;
        if (currentBrain_) {
            currentBrain_->save(filename);
            std::cout << "Brain state saved to " << filename << std::endl;
        }
    }
    
    void handleLoadCommand(std::istringstream& iss) {
        std::string filename;
        iss >> filename;
        if (currentBrain_) {
            currentBrain_->load(filename);
            std::cout << "Brain state loaded from " << filename << std::endl;
        }
    }
    
    void handleStatsCommand() {
        std::cout << "=== System Statistics ===" << std::endl;
        if (currentBrain_) {
            std::cout << "Brain:" << std::endl;
            std::cout << "  Neurons: " << currentBrain_->getTotalNeuronCount() << std::endl;
            std::cout << "  Synapses: " << currentBrain_->getTotalSynapseCount() << std::endl;
            std::cout << "  Active Neurons: " << currentBrain_->getFiringNeuronCount() << std::endl;
            std::cout << "  Average Firing Rate: " << currentBrain_->getAverageFiringRate() << std::endl;
            std::cout << "  Total Spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
            std::cout << "  E/I Ratio: " << currentBrain_->getExcitationInhibitionRatio() << std::endl;
        }
        if (currentWorld_) {
            std::cout << "World:" << std::endl;
            std::cout << "  Time: " << currentWorld_->getSimulationTime() << std::endl;
            std::cout << "  Width: " << currentWorld_->getWidth() << std::endl;
            std::cout << "  Height: " << currentWorld_->getHeight() << std::endl;
            std::cout << "  Agent X: " << currentWorld_->getAgentBody()->x << std::endl;
            std::cout << "  Agent Y: " << currentWorld_->getAgentBody()->y << std::endl;
        }
        if (currentAgent_) {
            std::cout << "Agent:" << std::endl;
            std::cout << "  Curiosity: " << currentAgent_->getCuriosityLevel() << std::endl;
            std::cout << "  Novelty: " << currentAgent_->getNoveltyLevel() << std::endl;
            std::cout << "  Prediction Error: " << currentAgent_->getPredictionError() << std::endl;
            std::cout << "  Neuromodulation Level: " << currentAgent_->getNeuromodulationLevel() << std::endl;
        }
    }
};

} // namespace nlm
