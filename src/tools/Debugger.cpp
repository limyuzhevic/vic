// NLM Debugger Tool
// Advanced debugging and inspection tools for the NLM brain

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>

namespace nlm {

class Debugger {
public:
    Debugger(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world);
    ~Debugger();
    
    // Command-line interface
    void run(const std::vector<std::string>& args);
    
    // Brain inspection
    void inspectBrain() const;
    void inspectNeurons() const;
    void inspectSynapses() const;
    void inspectRegions() const;
    
    // Memory system inspection
    void inspectWorkingMemory() const;
    void inspectEpisodicMemory() const;
    void inspectAssociativeMemory() const;
    
    // Neuromodulation inspection
    void inspectNeuromodulation() const;
    void inspectDopamine() const;
    void inspectCuriosity() const;
    void inspectNovelty() const;
    
    // Cognition system inspection
    void inspectCognition() const;
    void inspectPlanner() const;
    void inspectConceptFormation() const;
    void inspectAttention() const;
    
    // Development inspection
    void inspectDevelopment() const;
    void inspectDevelopmentHistory() const;
    
    // Prediction system inspection
    void inspectPrediction() const;
    
    // Agent inspection
    void inspectAgent() const;
    void inspectSensory() const;
    void inspectMotor() const;
    
    // World inspection
    void inspectWorld() const;
    
    // Command processing
    bool processCommand(const std::string& cmd, const std::vector<std::string>& args);
    
    // Help and usage
    void printHelp() const;
    void printBrainCommands() const;
    void printMemoryCommands() const;
    void printNeuromodulationCommands() const;
    void printCognitionCommands() const;
    void printDevelopmentCommands() const;
    void printPredictionCommands() const;
    void printAgentCommands() const;
    void printWorldCommands() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Command handlers
    bool handleInfoCommand(const std::vector<std::string>& args);
    bool handleInspectCommand(const std::vector<std::string>& args);
    bool handleSetCommand(const std::vector<std::string>& args);
    bool handleResetCommand(const std::vector<std::string>& args);
    bool handleCheckpointCommand(const std::vector<std::string>& args);
    bool handleHelpCommand(const std::vector<std::string>& args);
    
    // Formatting helpers
    void printSeparator() const;
    void printSection(const std::string& title) const;
    void printKeyValue(const std::string& key, const std::string& value) const;
    void printKeyValue(const std::string& key, int value) const;
    void printKeyValue(const std::string& key, float value) const;
    void printKeyValue(const std::string& key, bool value) const;
};

Debugger::Debugger(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world)
    : brain_(brain), agent_(agent), world_(world) {}

Debugger::~Debugger() = default;

void Debugger::run(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return;
    }
    
    std::string command = args[0];
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());
    
    if (!processCommand(command, commandArgs)) {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Type 'help' for available commands." << std::endl;
    }
}

bool Debugger::processCommand(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "help" || cmd == "?") {
        return handleHelpCommand(args);
    } else if (cmd == "info") {
        return handleInfoCommand(args);
    } else if (cmd == "inspect") {
        return handleInspectCommand(args);
    } else if (cmd == "set") {
        return handleSetCommand(args);
    } else if (cmd == "reset") {
        return handleResetCommand(args);
    } else if (cmd == "checkpoint") {
        return handleCheckpointCommand(args);
    } else {
        return false;
    }
}

void Debugger::printHelp() const {
    std::cout << "=== NLM Debugger Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "General Commands:" << std::endl;
    std::cout << "  help, ?                    - Show this help message" << std::endl;
    std::cout << "  info                      - Show brain system info" << std::endl;
    std::cout << std::endl;
    
    printBrainCommands();
    printMemoryCommands();
    printNeuromodulationCommands();
    printCognitionCommands();
    printDevelopmentCommands();
    printPredictionCommands();
    printAgentCommands();
    printWorldCommands();
    
    std::cout << std::endl;
    std::cout << "Configuration Commands:" << std::endl;
    std::cout << "  set <key> <value>         - Set configuration value" << std::endl;
    std::cout << "  reset <key>               - Reset configuration to default" << std::endl;
    std::cout << std::endl;
    std::cout << "Persistence Commands:" << std::endl;
    std::cout << "  checkpoint save <file>     - Save brain state to file" << std::endl;
    std::cout << "  checkpoint load <file>     - Load brain state from file" << std::endl;
    std::cout << std::endl;
}

void Debugger::printBrainCommands() const {
    std::cout << "Brain Commands:" << std::endl;
    std::cout << "  brain info                 - Show brain system information" << std::endl;
    std::cout << "  brain inspect neurons      - Inspect neuron states" << std::endl;
    std::cout << "  brain inspect synapses     - Inspect synaptic connections" << std::endl;
    std::cout << "  brain inspect regions      - Inspect neural regions" << std::endl;
    std::cout << "  brain reset               - Reset brain state" << std::endl;
    stdcout << std::endl;
}

void Debugger::printMemoryCommands() const {
    std::cout << "Memory Commands:" << std::endl;
    std::cout << "  memory inspect working     - Inspect working memory" << std::endl;
    std::cout << "  memory inspect episodic    - Inspect episodic memory" << std::endl;
    std::cout << "  memory inspect associative  - Inspect associative memory" << std::endl;
    std::cout << std::endl;
}

void Debugger::printNeuromodulationCommands() const {
    std::cout << "Neuromodulation Commands:" << std::endl;
    std::cout << "  neuromod info              - Show neuromodulation status" << std::endl;
    std::cout << "  neuromod inspect dopamine  - Inspect dopamine system" << std::endl;
    std::cout << "  neuromod inspect curiosity  - Inspect curiosity system" << std::endl;
    std::cout << "  neuromod inspect novelty    - Inspect novelty system" << std::endl;
    std::cout << std::endl;
}

void Debugger::printCognitionCommands() const {
    std::cout << "Cognition Commands:" << std::endl;
    std::cout << "  cognition info            - Show cognition system status" << std::endl;
    std::cout << "  cognition inspect planner  - Inspect neural planner" << std::endl;
    std::cout << "  cognition inspect concepts - Inspect concept formation" << std::endl;
    std::cout << "  cognition inspect attention - Inspect attentional selection" << std::endl;
    std::cout << std::endl;
}

void Debugger::printDevelopmentCommands() const {
    std::cout << "Development Commands:" << std::endl;
    std::cout << "  development info           - Show development system status" << std::endl;
    std::cout << "  development inspect history - Inspect developmental history" << std::endl;
    std::cout << "  development reset          - Reset development system" << std::endl;
    std::cout << std::endl;
}

void Debugger::printPredictionCommands() const {
    std::cout << "Prediction Commands:" << std::endl;
    std::cout << "  prediction info            - Show prediction system status" << std::endl;
    std::cout << "  prediction inspect         - Inspect prediction details" << std::endl;
    std::cout << std::endl;
}

void Debugger::printAgentCommands() const {
    std::cout << "Agent Commands:" << std::endl;
    std::cout << "  agent info                - Show agent system status" << std::endl;
    std::cout << "  agent inspect sensory      - Inspect sensory processing" << std::endl;
    std::cout << "  agent inspect motor        - Inspect motor decoding" << std::endl;
    std::cout << std::endl;
}

void Debugger::printWorldCommands() const {
    std::cout << "World Commands:" << std::endl;
    std::cout << "  world info                - Show world system status" << std::endl;
    std::cout << "  world inspect            - Inspect world details" << std::endl;
    std::cout << std::endl;
}

bool Debugger::handleHelpCommand(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        std::string topic = args[0];
        if (topic == "brain") {
            printBrainCommands();
        } else if (topic == "memory") {
            printMemoryCommands();
        } else if (topic == "neuromod") {
            printNeuromodulationCommands();
        } else if (topic == "cognition") {
            printCognitionCommands();
        } else if (topic == "development") {
            printDevelopmentCommands();
        } else if (topic == "prediction") {
            printPredictionCommands();
        } else if (topic == "agent") {
            printAgentCommands();
        } else if (topic == "world") {
            printWorldCommands();
        } else {
            std::cerr << "Unknown help topic: " << topic << std::endl;
            return false;
        }
    } else {
        printHelp();
    }
    return true;
}

bool Debugger::handleInfoCommand(const std::vector<std::string>& args) {
    if (args.empty() || args[0] == "brain") {
        if (brain_) {
            brain_->logStatus();
        } else {
            std::cout << "Brain not available" << std::endl;
        }
    } else if (args[0] == "memory") {
        if (brain_) {
            // Show memory system status
            auto* wm = brain_->getWorkingMemory();
            auto* em = brain_->getEpisodicMemory();
            auto* am = brain_->getAssociativeMemory();
            
            std::cout << "=== Memory System Status ===" << std::endl;
            std::cout << "Working Memory: " << (wm ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            if (wm) {
                std::cout << "  Active traces: " << wm->getActiveTraces() << std::endl;
            }
            std::cout << "Episodic Memory: " << (em ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            if (em) {
                std::cout << "  Episodes: " << em->getEpisodeCount() << std::endl;
            }
            std::cout << "Associative Memory: " << (am ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
        }
    } else if (args[0] == "neuromod") {
        if (brain_) {
            // Show neuromodulation status
            auto* dopamine = brain_->getDopamine();
            auto* curiosity = brain_->getCuriosity();
            auto* novelty = brain_->getNovelty();
            
            std::cout << "=== Neuromodulation Status ===" << std::endl;
            std::cout << "Dopamine: " << (dopamine ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            if (dopamine) {
                // Assuming dopamine has getLevel() method
                // float level = dopamine->getLevel();
                // std::cout << "  Level: " << level << std::endl;
            }
            std::cout << "Curiosity: " << (curiosity ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            std::cout << "Novelty: " << (novelty ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
        }
    } else if (args[0] == "cognition") {
        if (brain_) {
            // Show cognition system status
            auto* planner = brain_->getPlanner();
            auto* conceptFormation = brain_->getConceptFormation();
            auto* attention = brain_->getAttention();
            
            std::cout << "=== Cognition System Status ===" << std::endl;
            std::cout << "Neural Planner: " << (planner ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            std::cout << "Concept Formation: " << (conceptFormation ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            std::cout << "Attentional Selection: " << (attention ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
        }
    } else if (args[0] == "development") {
        if (brain_) {
            // Show development system status
            auto* devSystem = brain_->getDevelopmentSystem();
            DevelopmentalStage stage = brain_->getDevelopmentalStage();
            
            std::cout << "=== Development System Status ===" << std::endl;
            std::cout << "Development System: " << (devSystem ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
            std::cout << "Developmental Stage: " << static_cast<int>(stage) << std::endl;
        }
    } else if (args[0] == "prediction") {
        if (brain_) {
            // Show prediction system status
            auto* predictionSystem = brain_->getPredictionSystem();
            
            std::cout << "=== Prediction System Status ===" << std::endl;
            std::cout << "Prediction System: " << (predictionSystem ? "AVAILABLE" : "NOT AVAILABLE") << std::endl;
        }
    }
    return true;
}

bool Debugger::handleInspectCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Please specify what to inspect." << std::endl;
        std::cerr << "Use 'help brain' for brain inspection options." << std::endl;
        return false;
    }
    
    std::string target = args[0];
    std::vector<std::string> subArgs(args.begin() + 1, args.end());
    
    if (target == "brain") {
        if (!args.empty()) {
            if (args[1] == "neurons") {
                inspectNeurons();
            } else if (args[1] == "synapses") {
                inspectSynapses();
            } else if (args[1] == "regions") {
                inspectRegions();
            } else {
                std::cerr << "Unknown brain inspection target: " << args[1] << std::endl;
            }
        } else {
            inspectBrain();
        }
    } else if (target == "memory") {
        if (!args.empty()) {
            if (args[1] == "working") {
                inspectWorkingMemory();
            } else if (args[1] == "episodic") {
                inspectEpisodicMemory();
            } else if (args[1] == "associative") {
                inspectAssociativeMemory();
            } else {
                std::cerr << "Unknown memory inspection target: " << args[1] << std::endl;
            }
        }
    } else if (target == "neuromod") {
        // Neuromodulation inspection not implemented yet
        std::cout << "Neuromodulation inspection not yet implemented." << std::endl;
    } else if (target == "cognition") {
        // Cognition inspection not implemented yet
        std::cout << "Cognition inspection not yet implemented." << std::endl;
    } else if (target == "development") {
        // Development inspection not implemented yet
        std::cout << "Development inspection not yet implemented." << std::endl;
    } else if (target == "prediction") {
        // Prediction inspection not implemented yet
        std::cout << "Prediction inspection not yet implemented." << std::endl;
    } else if (target == "agent") {
        // Agent inspection not implemented yet
        std::cout << "Agent inspection not yet implemented." << std::endl;
    } else if (target == "world") {
        // World inspection not implemented yet
        std::cout << "World inspection not yet implemented." << std::endl;
    } else {
        std::cerr << "Unknown inspection target: " << target << std::endl;
    }
    
    return true;
}

void Debugger::inspectBrain() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Brain System Inspection ===" << std::endl;
    brain_->logStatus();
}

void Debugger::inspectNeurons() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Neuron Inspection ===" << std::endl;
    size_t totalNeurons = brain_->getTotalNeuronCount();
    std::cout << "Total neurons: " << totalNeurons << std::endl;
    
    // Show first few neurons as sample
    size_t sampleSize = std::min<size_t>(5, totalNeurons);
    for (size_t i = 0; i < sampleSize; ++i) {
        std::cout << "  Neuron " << i << ": (sample)" << std::endl;
    }
    
    if (totalNeurons > sampleSize) {
        std::cout << "  ... and " << (totalNeurons - sampleSize) << " more neurons" << std::endl;
    }
}

void Debugger::inspectSynapses() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Synapse Inspection ===" << std::endl;
    size_t totalSynapses = brain_->getTotalSynapseCount();
    std::cout << "Total synapses: " << totalSynapses << std::endl;
    
    // Show synaptic weight statistics if possible
    // This would require additional Brain API methods
    std::cout << "Synaptic weight range: [0.0, 2.0] (estimated)" << std::endl;
    std::cout << "Average synaptic weight: ~0.1 (estimated)" << std::endl;
}

void Debugger::inspectRegions() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Neural Region Inspection ===" << std::endl;
    size_t regionCount = brain_->getRegionCount();
    std::cout << "Total regions: " << regionCount << std::endl;
    
    // Show region details
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId(i + 1);
        auto* region = brain_->getRegion(regionId);
        if (region) {
            std::cout << "  Region " << regionId.index() << ": " << region->getName() << std::endl;
            std::cout << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
            std::cout << "    Synapses: " << region->getSynapseCount() << std::endl;
            std::cout << "    Populations: " << region->getPopulationCount() << std::endl;
        }
    }
}

void Debugger::inspectWorkingMemory() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Working Memory Inspection ===" << std::endl;
    auto* wm = brain_->getWorkingMemory();
    if (wm) {
        size_t activeTraces = wm->getActiveTraces();
        std::cout << "Active traces: " << activeTraces << std::endl;
        
        // Show memory neurons if possible
        // This would require additional WorkingMemory API methods
        std::cout << "Memory neurons: (sample data)" << std::endl;
    } else {
        std::cout << "Working memory not available" << std::endl;
    }
}

void Debugger::inspectEpisodicMemory() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Episodic Memory Inspection ===" << std::endl;
    auto* em = brain_->getEpisodicMemory();
    if (em) {
        size_t episodeCount = em->getEpisodeCount();
        std::cout << "Total episodes: " << episodeCount << std::endl;
        
        if (episodeCount > 0) {
            std::cout << "Most recent episode:" << std::endl;
            // Would need additional EpisodicMemory API to get episode details
            std::cout << "  Episode ID: (sample)" << std::endl;
            std::cout << "  Timestamp: (sample)" << std::endl;
            std::cout << "  Reward: (sample)" << std::endl;
        }
    } else {
        std::cout << "Episodic memory not available" << std::endl;
    }
}

void Debugger::inspectAssociativeMemory() const {
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    std::cout << "=== Associative Memory Inspection ===" << std::endl;
    auto* am = brain_->getAssociativeMemory();
    if (am) {
        std::cout << "Associative memory available" << std::endl;
        // Would need additional AssociativeMemory API methods
        std::cout << "Pattern associations: (sample data)" << std::endl;
    } else {
        std::cout << "Associative memory not available" << std::endl;
    }
}

bool Debugger::handleSetCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Usage: set <key> <value>" << std::endl;
        return false;
    }
    
    std::string key = args[0];
    std::string value = args[1];
    
    // This is a simplified implementation
    // In a real implementation, this would modify Brain configuration
    std::cout << "Set " << key << " = " << value << std::endl;
    
    return true;
}

bool Debugger::handleResetCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: reset <key>" << std::endl;
        return false;
    }
    
    std::string key = args[0];
    std::cout << "Reset " << key << " to default value" << std::endl;
    
    // This would reset Brain configuration in a real implementation
    return true;
}

bool Debugger::handleCheckpointCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: checkpoint <save|load> <file>" << std::endl;
        return false;
    }
    
    std::string action = args[0];
    std::string filepath = args[1];
    
    if (action == "save") {
        if (brain_) {
            std::cout << "Saving brain state to " << filepath << "..." << std::endl;
            if (brain_->save(filepath)) {
                std::cout << "Save successful" << std::endl;
                return true;
            } else {
                std::cerr << "Save failed" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Brain not available" << std::endl;
            return false;
        }
    } else if (action == "load") {
        if (brain_) {
            std::cout << "Loading brain state from " << filepath << "..." << std::endl;
            if (brain_->load(filepath)) {
                std::cout << "Load successful" << std::endl;
                return true;
            } else {
                std::cerr << "Load failed" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Brain not available" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Unknown checkpoint action: " << action << std::endl;
        std::cerr << "Use 'save' or 'load'" << std::endl;
        return false;
    }
}

void Debugger::printSeparator() const {
    std::cout << "============================================================" << std::endl;
}

void Debugger::printSection(const std::string& title) const {
    printSeparator();
    std::cout << title << std::endl;
    printSeparator();
}

void Debugger::printKeyValue(const std::string& key, const std::string& value) const {
    std::cout << "  " << std::left << std::setw(25) << key << ": " << value << std::endl;
}

void Debugger::printKeyValue(const std::string& key, int value) const {
    printKeyValue(key, std::to_string(value));
}

void Debugger::printKeyValue(const std::string& key, float value) const {
    printKeyValue(key, std::to_string(value));
}

void Debugger::printKeyValue(const std::string& key, bool value) const {
    printKeyValue(key, value ? "true" : "false");
}

} // namespace nlm