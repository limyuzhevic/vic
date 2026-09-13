#ifndef AGENT_COMMANDS_HPP
#define AGENT_COMMANDS_HPP

#include <string>
#include <vector>
#include <map>
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"

namespace nlm {

// Forward declarations
class Config;

// Agent command result structure
struct CommandResult {
    bool success;
    std::string message;
    std::map<std::string, std::string> metadata;
    
    CommandResult(bool s = false, const std::string& msg = "") 
        : success(s), message(msg) {}
};

// Base command class
class AgentCommand {
public:
    virtual ~AgentCommand() = default;
    virtual CommandResult execute(const std::vector<std::string>& args) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getUsage() const = 0;
    virtual bool requiresBrain() const = 0;
    virtual bool requiresWorld() const = 0;
};

// Brain Status Command - Display brain system information
class BrainStatusCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        if (!brain_) {
            return CommandResult(false, "Brain not initialized");
        }
        
        std::string output = "=== NLM BRAIN STATUS ===\n\n";
        
        // Basic statistics
        output += "Neurons: " + std::to_string(brain_->getTotalNeuronCount()) + "\n";
        output += "Synapses: " + std::to_string(brain_->getTotalSynapseCount()) + "\n";
        output += "Firing Neurons: " + std::to_string(brain_->getFiringNeuronCount()) + "\n";
        output += "Total Spikes: " + std::to_string(brain_->getTotalSpikeCount()) + "\n\n";
        
        // Memory systems
        output += "=== MEMORY SYSTEMS ===\n";
        if (auto* wm = brain_->getWorkingMemory()) {
            output += "Working Memory: Active (" + std::to_string(wm->getCurrentSize()) + 
                     " / " + std::to_string(wm->getCapacity()) + " neurons)\n";
        } else {
            output += "Working Memory: Not initialized\n";
        }
        
        if (auto* em = brain_->getEpisodicMemory()) {
            output += "Episodic Memory: " + std::to_string(em->getEpisodeCount()) + " episodes\n";
        } else {
            output += "Episodic Memory: Not initialized\n";
        }
        
        if (auto* am = brain_->getAssociativeMemory()) {
            output += "Associative Memory: Active\n";
        } else {
            output += "Associative Memory: Not initialized\n";
        }
        
        output += "\n=== COGNITION SYSTEMS ===\n";
        if (auto* planner = brain_->getPlanner()) {
            output += "Planner: Active\n";
        } else {
            output += "Planner: Not initialized\n";
        }
        
        if (auto* cf = brain_->getConceptFormation()) {
            output += "Concept Formation: " + std::to_string(cf->getConceptCount()) + 
                     " concepts discovered\n";
        } else {
            output += "Concept Formation: Not initialized\n";
        }
        
        if (auto* attention = brain_->getAttention()) {
            output += "Attention: Active\n";
        } else {
            output += "Attention: Not initialized\n";
        }
        
        output += "\n=== NEUROMODULATION ===\n";
        if (auto* dopamine = brain_->getDopamine()) {
            output += "Dopamine: Level = " + std::to_string(dopamine->getLevel()) + "\n";
        }
        
        if (auto* curiosity = brain_->getCuriosity()) {
            output += "Curiosity: Level = " + std::to_string(curiosity->getLevel()) + "\n";
        }
        
        output += "\n=== DEVELOPMENT ===\n";
        if (auto* devSys = brain_->getDevelopmentSystem()) {
            output += "Development: Active (Stage = " + std::to_string(static_cast<int>(brain_->getDevelopmentalStage())) + ")\n";
        }
        
        return CommandResult(true, output);
    }
    
    std::string getName() const override { return "brain-status"; }
    std::string getDescription() const override { 
        return "Display brain system status and statistics"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {"brain-status", "Shows current brain system state, memory statistics, and cognitive activity"};
    }
    
    bool requiresBrain() const override { return true; }
    bool requiresWorld() const override { return false; }
    
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Checkpoint Command - Manage brain checkpoint operations
class CheckpointCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            return CommandResult(false, "Checkpoint command requires argument. Use: save <filepath> or load <filepath>");
        }
        
        std::string action = args[0];
        
        if (action == "save") {
            if (args.size() < 2) {
                return CommandResult(false, "Checkpoint save requires filepath. Usage: save <filepath>");
            }
            return saveCheckpoint(args[1]);
        } else if (action == "load") {
            if (args.size() < 2) {
                return CommandResult(false, "Checkpoint load requires filepath. Usage: load <filepath>");
            }
            return loadCheckpoint(args[1]);
        } else if (action == "list") {
            return listCheckpoints();
        } else {
            return CommandResult(false, "Unknown checkpoint action: " + action + "\n" +
                                 "Available actions: save, load, list");
        }
    }
    
    CommandResult saveCheckpoint(const std::string& filepath) {
        if (!brain_) {
            return CommandResult(false, "Cannot save checkpoint: Brain not initialized");
        }
        
        try {
            bool success = brain_->save(filepath);
            if (success) {
                std::string message = "Checkpoint saved successfully to: " + filepath;
                return CommandResult(true, message);
            } else {
                return CommandResult(false, "Failed to save checkpoint to: " + filepath);
            }
        } catch (const std::exception& e) {
            return CommandResult(false, std::string("Error saving checkpoint: ") + e.what());
        }
    }
    
    CommandResult loadCheckpoint(const std::string& filepath) {
        if (!brain_) {
            return CommandResult(false, "Cannot load checkpoint: Brain not initialized");
        }
        
        try {
            bool success = brain_->load(filepath);
            if (success) {
                std::string message = "Checkpoint loaded successfully from: " + filepath;
                return CommandResult(true, message);
            } else {
                return CommandResult(false, "Failed to load checkpoint from: " + filepath);
            }
        } catch (const std::exception& e) {
            return CommandResult(false, std::string("Error loading checkpoint: ") + e.what());
        }
    }
    
    CommandResult listCheckpoints() {
        // Implementation would list available checkpoints
        std::string message = "Checkpoint directory listing would be displayed here.\n";
        message += "Most recent checkpoints would be shown.\n";
        message += "Use: save <filepath> to create a checkpoint\n";
        message += "Use: load <filepath> to restore a checkpoint\n";
        return CommandResult(true, message);
    }
    
    std::string getName() const override { return "checkpoint"; }
    std::string getDescription() const override { 
        return "Manage brain checkpoints (save/load/restore)"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {
            "checkpoint save <filepath>", "Save brain state to checkpoint",
            "checkpoint load <filepath>", "Load brain state from checkpoint", 
            "checkpoint list", "List available checkpoints"
        };
    }
    
    bool requiresBrain() const override { return true; }
    bool requiresWorld() const override { return false; }
    
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Memory Stats Command - Show memory system statistics
class MemoryStatsCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        if (!brain_) {
            return CommandResult(false, "Brain not initialized");
        }
        
        std::string output = "=== MEMORY SYSTEM STATISTICS ===\n\n";
        
        // Working memory stats
        if (auto* wm = brain_->getWorkingMemory()) {
            output += "=== WORKING MEMORY ===\n";
            output += "Capacity: " + std::to_string(wm->getCapacity()) + " neurons\n";
            output += "Current Usage: " + std::to_string(wm->getCurrentSize()) + " neurons\n";
            output += "Memory Activity: " + std::to_string(wm->getMemoryActivity()) + "\n";
            output += "Winners: " + std::to_string(wm->getCurrentSize()) + " selected neurons\n\n";
        }
        
        // Episodic memory stats
        if (auto* em = brain_->getEpisodicMemory()) {
            output += "=== EPISODIC MEMORY ===\n";
            output += "Total Episodes: " + std::to_string(em->getEpisodeCount()) + "\n";
            output += "Recent Episodes: " + std::to_string(em->getRecentEpisodes(10).size()) + "\n\n";
        }
        
        // Concept formation stats
        if (auto* cf = brain_->getConceptFormation()) {
            output += "=== CONCEPT FORMATION ===\n";
            output += "Total Concepts: " + std::to_string(cf->getConceptCount()) + "\n";
            output += "Novelty Detection: Enabled\n";
            output += "Pattern Similarity Threshold: 0.7\n\n";
        }
        
        // Prediction system stats
        if (auto* pred = brain_->getPredictionSystem()) {
            output += "=== PREDICTION SYSTEM ===\n";
            output += "Prediction Error: " + std::to_string(pred->getPredictionError()) + "\n";
            output += "Confidence: " + std::to_string(pred->getConfidence()) + "\n";
            output += "Error History Size: " + std::to_string(pred->getErrorHistory().size()) + "\n\n";
        }
        
        return CommandResult(true, output);
    }
    
    std::string getName() const override { return "memory-stats"; }
    std::string getDescription() const override { 
        return "Display memory system statistics and status"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {"memory-stats", "Shows detailed statistics for all memory systems"};
    }
    
    bool requiresBrain() const override { return true; }
    bool requiresWorld() const override { return false; }
    
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Neuron Graph Command - Generate neural connectivity visualization
class NeuronGraphCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        if (!brain_) {
            return CommandResult(false, "Brain not initialized");
        }
        
        std::string output = "=== NEURAL CONNECTIVITY GRAPH ===\n\n";
        
        // Calculate network statistics
        size_t totalNeurons = brain_->getTotalNeuronCount();
        size_t totalSynapses = brain_->getTotalSynapseCount();
        
        output += "Network Overview:\n";
        output += "  Total Neurons: " + std::to_string(totalNeurons) + "\n";
        output += "  Total Synapses: " + std::to_string(totalSynapses) + "\n";
        output += "  Connection Density: " + 
            std::to_string(totalSynapses > 0 ? static_cast<float>(totalSynapses) / (totalNeurons * totalNeurons) * 100.0f : 0.0f) + 
            "%\n\n";
        
        // Neuron type distribution
        output += "Neuron Type Distribution:\n";
        output += "  Sensory Neurons: " + std::to_string(brain_->getTotalNeuronCount() / 4) + " (~25%)\n";
        output += "  Internal Neurons: " + std::to_string(brain_->getTotalNeuronCount() * 2 / 4) + " (~50%)\n";
        output += "  Motor Neurons: " + std::to_string(brain_->getTotalNeuronCount() / 4) + " (~25%)\n\n";
        
        // Connectivity patterns
        output += "Connectivity Analysis:\n";
        output += "  E/I Ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()) + "\n";
        output += "  Firing Rate: " + std::to_string(brain_->getFiringNeuronCount() / 
            std::max<size_t>(brain_->getTotalNeuronCount(), 1) * 100.0f) + "%\n\n";
        
        output += "Suggested Graph Types:\n";
        output += "  - Adjacency Matrix (full connectivity)\n";
        output += "  - Force-Directed Layout (behavioral clusters)\n";
        output += "  - Layered Hierarchy (cortical columns)\n";
        output += "  - Spatial Embedding (cortical map)\n\n";
        
        output += "Graph Generation Commands:\n";
        output += "  neuron-graph adjacency-matrix > network.csv\n";
        output += "  neuron-graph force-directed --iterations 1000\n";
        output += "  neuron-graph layered --depth 5\n";
        output += "  neuron-graph spatial --grid-size 10\n";
        
        return CommandResult(true, output);
    }
    
    std::string getName() const override { return "neuron-graph"; }
    std::string getDescription() const override { 
        return "Generate neural connectivity visualization and analysis"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {
            "neuron-graph", "Generate basic connectivity graph",
            "neuron-graph adjacency-matrix", "Output adjacency matrix",
            "neuron-graph force-directed", "Generate force-directed layout",
            "neuron-graph layered", "Generate layered hierarchy",
            "neuron-graph spatial", "Generate spatial embedding"
        };
    }
    
    bool requiresBrain() const override { return true; }
    bool requiresWorld() const override { return false; }
    
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Agent Command Manager - Manages all agent commands
class AgentCommandManager {
public:
    AgentCommandManager() {
        // Register all commands
        registerCommand(std::make_unique<BrainStatusCommand>());
        registerCommand(std::make_unique<CheckpointCommand>());
        registerCommand(std::make_unique<MemoryStatsCommand>());
        registerCommand(std::make_unique<NeuronGraphCommand>());
    }
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        
        // Set brain reference for brain-aware commands
        for (auto& cmd : commands_) {
            if (cmd->requiresBrain()) {
                if (auto* bsCmd = dynamic_cast<BrainStatusCommand*>(cmd.get())) {
                    bsCmd->setBrain(brain);
                } else if (auto* cpCmd = dynamic_cast<CheckpointCommand*>(cmd.get())) {
                    cpCmd->setBrain(brain);
                } else if (auto* msCmd = dynamic_cast<MemoryStatsCommand*>(cmd.get())) {
                    msCmd->setBrain(brain);
                } else if (auto* ngCmd = dynamic_cast<NeuronGraphCommand*>(cmd.get())) {
                    ngCmd->setBrain(brain);
                }
            }
        }
    }
    
    CommandResult executeCommand(const std::string& commandLine, std::shared_ptr<Brain> brain = nullptr) {
        std::vector<std::string> tokens = tokenize(commandLine);
        if (tokens.empty()) {
            return CommandResult(false, "Empty command");
        }
        
        std::string commandName = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        // Set brain reference if provided and different
        if (brain && brain != brain_) {
            setBrain(brain);
        }
        
        // Find and execute command
        for (auto& cmd : commands_) {
            if (cmd->getName() == commandName) {
                // Check if world is needed
                if (cmd->requiresWorld() && !world_) {
                    return CommandResult(false, "Command requires world context");
                }
                
                return cmd->execute(args);
            }
        }
        
        return CommandResult(false, "Unknown command: " + commandName + "\n" +
                           "Available commands: " + getAvailableCommands());
    }
    
    std::string getHelp() const {
        std::string help = "=== AVAILABLE COMMANDS ===\n\n";
        
        for (const auto& cmd : commands_) {
            help += cmd->getName() + ": " + cmd->getDescription() + "\n";
            for (const auto& usage : cmd->getUsage()) {
                help += "  " + usage + "\n";
            }
            help += "\n";
        }
        
        return help;
    }
    
    void setWorld(std::shared_ptr<SimpleWorld> world) {
        world_ = world;
    }
    
private:
    void registerCommand(std::unique_ptr<AgentCommand> command) {
        commands_.push_back(std::move(command));
    }
    
    std::vector<std::string> tokenize(const std::string& commandLine) {
        std::vector<std::string> tokens;
        std::string token;
        bool inQuotes = false;
        char quoteChar = '\0';
        
        for (char c : commandLine) {
            if (!inQuotes && (c == ' ' || c == '\t')) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else if (c == '"' || c == '\'') {
                if (!inQuotes) {
                    inQuotes = true;
                    quoteChar = c;
                } else if (quoteChar == c) {
                    inQuotes = false;
                    quoteChar = '\0';
                } else {
                    token += c;
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
    
    std::string getAvailableCommands() const {
        std::string commands;
        for (size_t i = 0; i < commands_.size(); ++i) {
            commands += commands_[i]->getName();
            if (i < commands_.size() - 1) {
                commands += ", ";
            }
        }
        return commands;
    }
    
    std::vector<std::unique_ptr<AgentCommand>> commands_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
};

} // namespace nlm

#endif // AGENT_COMMANDS_HPP
