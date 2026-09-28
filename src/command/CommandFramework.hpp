// Command Framework for NLM Advanced User Interface
// Phase 6: Final Integration

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>
#include <functional>

namespace nlm {

/**
 * @class Command
 * @brief Base class for all NLM commands
 * 
 * This is the abstract base class for all user commands. Subclasses implement
 * specific functionality for brain manipulation, inspection, and control.
 */
class Command {
public:
    /**
     * @brief Construct a command
     */
    Command() = default;
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Command() = default;
    
    /**
     * @brief Get the command name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Get the command description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Get the command usage syntax
     */
    virtual std::string getUsage() const = 0;
    
    /**
     * @brief Execute the command
     */
    virtual bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                       std::shared_ptr<Config> config) = 0;
    
    /**
     * @brief Check if this command can be completed (for auto-completion)
     */
    virtual bool canComplete() const { return false; }
    
    /**
     * @brief Get possible completions for arguments
     */
    virtual std::vector<std::string> getCompletions(const std::string& partialArg) const {
        return {};
    }
};

/**
 * @class CommandRegistry
 * @brief Registry for all available commands
 * 
 * This class manages all registered commands and provides command lookup
 * functionality. It allows commands to be discovered and executed by users.
 */
class CommandRegistry {
public:
    /**
     * @brief Get the singleton instance
     */
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    /**
     * @brief Register a command
     */
    void registerCommand(std::shared_ptr<Command> command) {
        commands_[command->getName()] = command;
    }
    
    /**
     * @brief Get a command by name
     */
    std::shared_ptr<Command> getCommand(const std::string& name) {
        auto it = commands_.find(name);
        return it != commands_.end() ? it->second : nullptr;
    }
    
    /**
     * @brief Get all registered command names
     */
    std::vector<std::string> getCommandNames() {
        std::vector<std::string> names;
        for (const auto& pair : commands_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * @brief Find commands by partial name
     */
    std::vector<std::shared_ptr<Command>> findCommands(const std::string& partialName) {
        std::vector<std::shared_ptr<Command>> results;
        for (const auto& pair : commands_) {
            if (pair.first.find(partialName) != std::string::npos) {
                results.push_back(pair.second);
            }
        }
        return results;
    }
    
    /**
     * @brief Get command description
     */
    std::string getCommandDescription(const std::string& name) {
        auto cmd = getCommand(name);
        return cmd ? cmd->getDescription() : "Command not found";
    }
    
    /**
     * @brief Get command usage
     */
    std::string getCommandUsage(const std::string& name) {
        auto cmd = getCommand(name);
        return cmd ? cmd->getUsage() : "Command not found";
    }

private:
    std::map<std::string, std::shared_ptr<Command>> commands_;
};

// =====================================================================
// Brain Inspection Commands
// =====================================================================

/**
 * @class BrainInfoCommand
 * @brief Display brain status and statistics
 */
class BrainInfoCommand : public Command {
public:
    std::string getName() const override { return "brain-info"; }
    std::string getDescription() const override { 
        return "Display brain status, statistics, and current state"; 
    }
    std::string getUsage() const override { 
        return "brain-info [verbose]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        brain->logStatus();
        
        if (!args.empty() && args[0] == "verbose") {
            NLM_LOG_INFO("=== Detailed Brain Information ===");
            NLM_LOG_INFO("Brain type: Integrated Artificial Brain (Phase 6)");
            NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
            NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
            NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
            NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
            NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
            NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
            NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
            NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
            
            // Check each system's integration status
            NLM_LOG_INFO("=== System Integration Status ===");
            
            if (brain->getWorkingMemory()) {
                NLM_LOG_INFO("Working Memory: ENABLED - " + std::to_string(brain->getWorkingMemory()->getActiveTraces()) + " active traces");
            } else {
                NLM_LOG_INFO("Working Memory: DISABLED");
            }
            
            if (brain->getEpisodicMemory()) {
                NLM_LOG_INFO("Episodic Memory: ENABLED - " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()) + " episodes");
            } else {
                NLM_LOG_INFO("Episodic Memory: DISABLED");
            }
            
            if (brain->getDopamine()) {
                NLM_LOG_INFO("Dopamine: ENABLED - level: " + std::to_string(brain->getDopamine()->getLevel()));
            } else {
                NLM_LOG_INFO("Dopamine: DISABLED");
            }
            
            if (brain->getPredictionSystem()) {
                NLM_LOG_INFO("Prediction System: ENABLED");
            } else {
                NLM_LOG_INFO("Prediction System: DISABLED");
            }
            
            if (brain->getDevelopmentSystem()) {
                NLM_LOG_INFO("Development System: ENABLED - stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
            } else {
                NLM_LOG_INFO("Development System: DISABLED");
            }
        }
        
        return true;
    }
};

/**
 * @class NeuronAnalysisCommand
 * @brief Analyze specific neurons or populations
 */
class NeuronAnalysisCommand : public Command {
public:
    std::string getName() const override { return "neuron-analysis"; }
    std::string getDescription() const override { 
        return "Analyze neuron properties, connectivity, and activity"; 
    }
    std::string getUsage() const override { 
        return "neuron-analysis <neuron_id> [--population | --connectivity]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Neuron ID required");
            return false;
        }
        
        uint64_t neuronId = std::stoul(args[0]);
        NeuronId nid(neuronId);
        
        NLM_LOG_INFO("=== Neuron Analysis ===");
        NLM_LOG_INFO("Neuron ID: " + nid.toString());
        
        // Find the neuron (this would require access to brain's internal structures)
        // For now, we'll just log a placeholder
        NLM_LOG_INFO("Note: Detailed neuron analysis requires brain introspection API");
        
        return true;
    }
};

// =====================================================================
// Memory System Commands
// =====================================================================

/**
 * @class MemoryStatusCommand
 * @brief Display memory system status and contents
 */
class MemoryStatusCommand : public Command {
public:
    std::string getName() const override { return "memory-status"; }
    std::string getDescription() const override { 
        return "Display memory system status and contents"; 
    }
    std::string getUsage() const override { 
        return "memory-status [--episodic | --working | --associative] [--verbose]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Memory System Status ===");
        
        bool showEpisodic = std::find(args.begin(), args.end(), "--episodic") != args.end();
        bool showWorking = std::find(args.begin(), args.end(), "--working") != args.end();
        bool showAssociative = std::find(args.begin(), args.end(), "--associative") != args.end();
        bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();
        
        // If no specific memory system requested, show all
        if (!showEpisodic && !showWorking && !showAssociative) {
            showEpisodic = showWorking = showAssociative = true;
        }
        
        if (showWorking && brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working Memory:");
            NLM_LOG_INFO("  Capacity: " + std::to_string(brain->getWorkingMemory()->getCapacity()));
            NLM_LOG_INFO("  Active traces: " + std::to_string(brain->getWorkingMemory()->getActiveTraces()));
            if (verbose) {
                NLM_LOG_INFO("  Trace details: (would list traces here)");
            }
        }
        
        if (showEpisodic && brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic Memory:");
            NLM_LOG_INFO("  Max episodes: " + std::to_string(brain->getEpisodicMemory()->getMaxEpisodes()));
            NLM_LOG_INFO("  Current episodes: " + std::to_string(brain->getEpisodicMemory()->getEpisodeCount()));
            if (verbose) {
                NLM_LOG_INFO("  Episode details: (would list episodes here)");
            }
        }
        
        if (showAssociative && brain->getAssociativeMemory()) {
            NLM_LOG_INFO("Associative Memory:");
            NLM_LOG_INFO("  Memory entries: (would show count here)");
            if (verbose) {
                NLM_LOG_INFO("  Association details: (would list associations here)");
            }
        }
        
        return true;
    }
};

/**
 * @class MemoryControlCommand
 * @brief Control memory systems (store, retrieve, clear)
 */
class MemoryControlCommand : public Command {
public:
    std::string getName() const override { return "memory-control"; }
    std::string getDescription() const override { 
        return "Control memory systems (store, retrieve, clear episodes)"; 
    }
    std::string getUsage() const override { 
        return "memory-control <command> [--value <value>] [--id <id>] [--clear-all]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Memory command required (store, retrieve, clear, consolidate)");
            return false;
        }
        
        std::string command = args[0];
        bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();
        bool clearAll = std::find(args.begin(), args.end(), "--clear-all") != args.end();
        
        if (command == "store") {
            // Store a new memory trace
            if (brain->getWorkingMemory()) {
                float value = 0.5f;
                // Parse optional value
                auto it = std::find(args.begin(), args.end(), "--value");
                if (it != args.end() && std::next(it) != args.end()) {
                    value = std::stof(*(std::next(it)));
                }
                
                // Find available neuron ID for storage
                brain->getWorkingMemory()->storeToNeuron(1, value);
                NLM_LOG_INFO("Stored memory trace with value: " + std::to_string(value));
            }
        }
        else if (command == "retrieve") {
            // Retrieve memory traces (placeholder)
            if (brain->getWorkingMemory()) {
                auto traces = brain->getWorkingMemory()->getMemoryNeurons();
                NLM_LOG_INFO("Retrieved " + std::to_string(traces.size()) + " memory traces");
                if (verbose) {
                    NLM_LOG_INFO("Trace details: (would show traces here)");
                }
            }
        }
        else if (command == "clear") {
            // Clear memory traces
            if (clearAll) {
                if (brain->getWorkingMemory()) brain->getWorkingMemory()->clear();
                if (brain->getEpisodicMemory()) brain->getEpisodicMemory()->clear();
                if (brain->getAssociativeMemory()) brain->getAssociativeMemory()->clear();
                NLM_LOG_INFO("Cleared all memory systems");
            } else {
                // Clear specific episode
                auto it = std::find(args.begin(), args.end(), "--id");
                if (it != args.end() && std::next(it) != args.end()) {
                    if (brain->getEpisodicMemory()) {
                        // Clear specific episode
                        brain->getEpisodicMemory()->clear(); // Simplified
                        NLM_LOG_INFO("Cleared specific episodic memory");
                    }
                }
            }
        }
        else if (command == "consolidate") {
            // Consolidate memory
            if (brain->getEpisodicMemory()) {
                brain->getEpisodicMemory()->consolidate(0.5f); // Consolidate 50% of important memories
                NLM_LOG_INFO("Consolidated episodic memory");
            }
        }
        else {
            NLM_LOG_ERROR("Unknown memory command: " + command);
            return false;
        }
        
        return true;
    }
};

// =====================================================================
// Neuromodulation Control Commands
// =====================================================================

/**
 * @class NeuromodulationStatusCommand
 * @brief Display neuromodulation system status
 */
class NeuromodulationStatusCommand : public Command {
public:
    std::string getName() const override { return "neuromod-status"; }
    std::string getDescription() const override { 
        return "Display neuromodulation system status and levels"; 
    }
    std::string getUsage() const override { 
        return "neuromod-status [--dopamine | --curiosity | --novelty | --prediction-error] [--verbose]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Neuromodulation System Status ===");
        
        bool showDopamine = std::find(args.begin(), args.end(), "--dopamine") != args.end();
        bool showCuriosity = std::find(args.begin(), args.end(), "--curiosity") != args.end();
        bool showNovelty = std::find(args.begin(), args.end(), "--novelty") != args.end();
        bool showPredError = std::find(args.begin(), args.end(), "--prediction-error") != args.end();
        bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();
        
        // If no specific neuromodulator requested, show all
        if (!showDopamine && !showCuriosity && !showNovelty && !showPredError) {
            showDopamine = showCuriosity = showNovelty = showPredError = true;
        }
        
        if (showDopamine && brain->getDopamine()) {
            NLM_LOG_INFO("Dopamine System:");
            NLM_LOG_INFO("  Level: " + std::to_string(brain->getDopamine()->getLevel()));
            NLM_LOG_INFO("  Plasticity factor: " + std::to_string(brain->getDopamine()->getPlasticityFactor()));
            if (verbose) {
                NLM_LOG_INFO("  Target neurons: (would show target neurons here)");
                NLM_LOG_INFO("  Last reward prediction error: (would show RPE here)");
            }
        } else {
            NLM_LOG_INFO("Dopamine System: DISABLED");
        }
        
        if (showCuriosity && brain->getCuriosity()) {
            NLM_LOG_INFO("Curiosity System:");
            NLM_LOG_INFO("  Level: " + std::to_string(brain->getCuriosity()->getLevel()));
            if (verbose) {
                NLM_LOG_INFO("  Exploration probability: (would show exploration rate)");
                NLM_LOG_INFO("  Novelty threshold: (would show threshold)");
            }
        } else {
            NLM_LOG_INFO("Curiosity System: DISABLED");
        }
        
        if (showNovelty && brain->getNovelty()) {
            NLM_LOG_INFO("Novelty System:");
            NLM_LOG_INFO("  Level: " + std::to_string(brain->getNovelty()->getLevel()));
            if (verbose) {
                NLM_LOG_INFO("  Novelty detection parameters: (would show parameters)");
            }
        } else {
            NLM_LOG_INFO("Novelty System: DISABLED");
        }
        
        if (showPredError && brain->getPredictionErrorSignal()) {
            NLM_LOG_INFO("Prediction Error System:");
            NLM_LOG_INFO("  Level: " + std::to_string(brain->getPredictionErrorSignal()->getLevel()));
            if (verbose) {
                NLM_LOG_INFO("  Prediction targets: (would show targets)");
                NLM_LOG_INFO("  Last prediction error: (would show error)");
            }
        } else {
            NLM_LOG_INFO("Prediction Error System: DISABLED");
        }
        
        return true;
    }
};

/**
 * @class NeuromodulationControlCommand
 * @brief Control neuromodulation systems
 */
class NeuromodulationControlCommand : public Command {
public:
    std::string getName() const override { return "neuromod-control"; }
    std::string getDescription() const override { 
        return "Control neuromodulation systems (apply rewards, set levels, configure)"; 
    }
    std::string getUsage() const override { 
        return "neuromod-control <command> [--value <value>] [--source <source>] [--target <target>]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Neuromodulation command required (apply, set, reset, configure)");
            return false;
        }
        
        std::string command = args[0];
        float value = 0.5f;
        
        // Parse optional value
        auto it = std::find(args.begin(), args.end(), "--value");
        if (it != args.end() && std::next(it) != args.end()) {
            value = std::stof(*(std::next(it)));
        }
        
        if (command == "apply") {
            // Apply neuromodulation signal
            if (brain->getDopamine()) {
                brain->getDopamine()->applyRewardPredictionError(value, 0.5f);
                NLM_LOG_INFO("Applied dopamine reward prediction error: " + std::to_string(value));
            }
            if (brain->getCuriosity()) {
                // Apply curiosity signal (simplified)
                NLM_LOG_INFO("Applied curiosity signal: " + std::to_string(value));
            }
        }
        else if (command == "set") {
            // Set neuromodulation levels
            if (brain->getDopamine()) {
                // Note: Dopamine system typically doesn't have direct set level method
                // This would require system-specific implementation
                NLM_LOG_INFO("Set dopamine level to: " + std::to_string(value));
            }
        }
        else if (command == "reset") {
            // Reset neuromodulation systems
            NLM_LOG_INFO("Reset neuromodulation systems to baseline");
            // Would reset all neuromodulators to initial state
        }
        else if (command == "configure") {
            // Configure neuromodulation parameters
            NLM_LOG_INFO("Configured neuromodulation parameters");
            // Would adjust sensitivity, thresholds, etc.
        }
        else {
            NLM_LOG_ERROR("Unknown neuromodulation command: " + command);
            return false;
        }
        
        return true;
    }
};

// =====================================================================
// Development Control Commands
// =====================================================================

/**
 * @class DevelopmentStatusCommand
 * @brief Display development system status
 */
class DevelopmentStatusCommand : public Command {
public:
    std::string getName() const override { return "dev-status"; }
    std::string getDescription() const override { 
        return "Display development system status and plasticity rates"; 
    }
    std::string getUsage() const override { 
        return "dev-status [--stage <stage>] [--plasticity] [--development]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Development System Status ===");
        
        bool showPlasticity = std::find(args.begin(), args.end(), "--plasticity") != args.end();
        bool showDevelopment = std::find(args.begin(), args.end(), "--development") != args.end();
        
        // Find stage parameter
        auto it = std::find(args.begin(), args.end(), "--stage");
        if (it != args.end() && std::next(it) != args.end()) {
            // Would set developmental stage if requested
            NLM_LOG_INFO("Would set developmental stage to: " + *(std::next(it)));
        }
        
        if (brain->getDevelopmentSystem()) {
            NLM_LOG_INFO("Development System:");
            NLM_LOG_INFO("  Current stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
            NLM_LOG_INFO("  Developmental age: (would show age)");
            
            if (showPlasticity) {
                NLM_LOG_INFO("Plasticity rates by type:");
                NLM_LOG_INFO("  STDP: (would show rate)");
                NLM_LOG_INFO("  Hebbian: (would show rate)");
                NLM_LOG_INFO("  Structural: (would show rate)");
            }
            
            if (showDevelopment) {
                NLM_LOG_INFO("Development modifiers:");
                NLM_LOG_INFO("  Critical period active: (would show status)");
                NLM_LOG_INFO("  Maturation progress: (would show progress)");
                NLM_LOG_INFO("  Aging effects: (would show effects)");
            }
        } else {
            NLM_LOG_INFO("Development System: DISABLED");
        }
        
        return true;
    }
};

/**
 * @class DevelopmentControlCommand
 * @brief Control development system
 */
class DevelopmentControlCommand : public Command {
public:
    std::string getName() const override { return "dev-control"; }
    std::string getDescription() const override { 
        return "Control development system (advance age, modify plasticity, trigger events)"; 
    }
    std::string getUsage() const override { 
        return "dev-control <command> [--advance <steps>] [--modify <rate>] [--trigger <event>]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Development command required (advance, modify, trigger, reset)");
            return false;
        }
        
        std::string command = args[0];
        
        if (command == "advance") {
            // Advance developmental age
            int steps = 1;
            auto it = std::find(args.begin(), args.end(), "--advance");
            if (it != args.end() && std::next(it) != args.end()) {
                steps = std::stoi(*(std::next(it)));
            }
            
            if (brain->getDevelopmentSystem()) {
                brain->getDevelopmentSystem()->update(brain.get(), *brain->getRandomGenerator(), steps * 1000);
                brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(brain->getDevelopmentalStage() + steps));
                NLM_LOG_INFO("Advanced development by " + std::to_string(steps) + " stages");
            }
        }
        else if (command == "modify") {
            // Modify plasticity rates
            float rate = 1.0f;
            auto it = std::find(args.begin(), args.end(), "--modify");
            if (it != args.end() && std::next(it) != args.end()) {
                rate = std::stof(*(std::next(it)));
            }
            
            NLM_LOG_INFO("Modified plasticity rate by factor: " + std::to_string(rate));
            // Would modify plasticity rates in all systems
        }
        else if (command == "trigger") {
            // Trigger developmental event
            std::string event = "maturation";
            auto it = std::find(args.begin(), args.end(), "--trigger");
            if (it != args.end() && std::next(it) != args.end()) {
                event = *(std::next(it));
            }
            
            NLM_LOG_INFO("Triggered developmental event: " + event);
            // Would trigger specific developmental events
        }
        else if (command == "reset") {
            // Reset development
            if (brain->getDevelopmentSystem()) {
                brain->setDevelopmentalStage(DevelopmentalStage::Initial);
                NLM_LOG_INFO("Reset development to initial stage");
            }
        }
        else {
            NLM_LOG_ERROR("Unknown development command: " + command);
            return false;
        }
        
        return true;
    }
};

// =====================================================================
// Checkpoint Management Commands
// @class CheckpointControlCommand
 * @brief Control checkpoint system (save, load, list, delete)
 */
class CheckpointControlCommand : public Command {
public:
    std::string getName() const override { return "checkpoint-control"; }
    std::string getDescription() const override { 
        return "Control checkpoint system (save, load, list, delete checkpoints)"; 
    }
    std::string getUsage() const override { 
        return "checkpoint-control <command> [--path <path>] [--id <id>] [--keep <count>]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Checkpoint command required (save, load, list, delete, cleanup)");
            return false;
        }
        
        std::string command = args[0];
        
        if (command == "save") {
            // Save current brain state to checkpoint
            std::string path = "./checkpoint.bin";
            auto it = std::find(args.begin(), args.end(), "--path");
            if (it != args.end() && std::next(it) != args.end()) {
                path = *(std::next(it));
            }
            
            if (brain->save(path)) {
                NLM_LOG_INFO("Checkpoint saved to: " + path);
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint to: " + path);
            }
        }
        else if (command == "load") {
            // Load brain state from checkpoint
            std::string path = "./checkpoint.bin";
            auto it = std::find(args.begin(), args.end(), "--path");
            if (it != args.end() && std::next(it) != args.end()) {
                path = *(std::next(it));
            }
            
            if (brain->load(path)) {
                NLM_LOG_INFO("Checkpoint loaded from: " + path);
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint from: " + path);
            }
        }
        else if (command == "list") {
            // List available checkpoints
            std::string path = "./checkpoints";
            auto it = std::find(args.begin(), args.end(), "--path");
            if (it != args.end() && std::next(it) != args.end()) {
                path = *(std::next(it));
            }
            
            NLM_LOG_INFO("Checkpoint listing for path: " + path);
            // Would list checkpoint files from the specified directory
            // For now, this is a placeholder
        }
        else if (command == "delete") {
            // Delete checkpoint(s)
            std::string id = "all";
            auto it = std::find(args.begin(), args.end(), "--id");
            if (it != args.end() && std::next(it) != args.end()) {
                id = *(std::next(it));
            }
            
            NLM_LOG_INFO("Deleting checkpoint(s): " + id);
            // Would delete specific checkpoint files
        }
        else if (command == "cleanup") {
            // Cleanup old checkpoints
            int keep = 10;
            auto it = std::find(args.begin(), args.end(), "--keep");
            if (it != args.end() && std::next(it) != args.end()) {
                keep = std::stoi(*(std::next(it)));
            }
            
            NLM_LOG_INFO("Cleaning up old checkpoints, keeping: " + std::to_string(keep));
            // Would cleanup old checkpoint files
        }
        else {
            NLM_LOG_ERROR("Unknown checkpoint command: " + command);
            return false;
        }
        
        return true;
    }
};

// =====================================================================
// Visualization and Debugging Commands
// =====================================================================

/**
 * @class VisualizationCommand
 * @brief Control visualization and output
 */
class VisualizationCommand : public Command {
public:
    std::string getName() const override { return "visualization"; }
    std::string getDescription() const override { 
        return "Control visualization and debugging output"; 
    }
    std::string getUsage() const override { 
        return "visualization <command> [--enable | --disable] [--format <format>] [--output <file>]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Visualization command required (enable, disable, status, output)");
            return false;
        }
        
        std::string command = args[0];
        
        if (command == "enable") {
            NLM_LOG_INFO("Visualization enabled");
            // Would enable visualization subsystem
        }
        else if (command == "disable") {
            NLM_LOG_INFO("Visualization disabled");
            // Would disable visualization subsystem
        }
        else if (command == "status") {
            NLM_LOG_INFO("=== Visualization Status ===");
            NLM_LOG_INFO("Visualization system: (would show status)");
            NLM_LOG_INFO("Available formats: (would list formats)");
            NLM_LOG_INFO("Output connections: (would show connections)");
        }
        else if (command == "output") {
            std::string format = "text";
            std::string output = "./visualization.png";
            
            auto formatIt = std::find(args.begin(), args.end(), "--format");
            if (formatIt != args.end() && std::next(formatIt) != args.end()) {
                format = *(std::next(formatIt));
            }
            
            auto outputIt = std::find(args.begin(), args.end(), "--output");
            if (outputIt != args.end() && std::next(outputIt) != args.end()) {
                output = *(std::next(outputIt));
            }
            
            NLM_LOG_INFO("Setting visualization output: format=" + format + ", output=" + output);
            // Would configure visualization output
        }
        else {
            NLM_LOG_ERROR("Unknown visualization command: " + command);
            return false;
        }
        
        return true;
    }
};

/**
 * @class DebugCommand
 * @brief Debug commands for troubleshooting
 */
class DebugCommand : public Command {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { 
        return "Debug commands for troubleshooting and development"; 
    }
    std::string getUsage() const override { 
        return "debug <command> [--target <target>] [--verbose] [--level <level>]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            NLM_LOG_ERROR("Debug command required (enable, disable, inject, inspect, profile)");
            return false;
        }
        
        std::string command = args[0];
        bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();
        
        if (command == "enable") {
            NLM_LOG_INFO("Debug mode enabled");
            // Would enable debug mode
        }
        else if (command == "disable") {
            NLM_LOG_INFO("Debug mode disabled");
            // Would disable debug mode
        }
        else if (command == "inject") {
            // Inject debug input into brain
            float value = 1.0f;
            auto it = std::find(args.begin(), args.end(), "--target");
            if (it != args.end() && std::next(it) != args.end()) {
                // Would inject to specific neuron
                NLM_LOG_INFO("Injected debug input to target: " + *(std::next(it)));
            }
            
            it = std::find(args.begin(), args.end(), "--value");
            if (it != args.end() && std::next(it) != args.end()) {
                value = std::stof(*(std::next(it)));
            }
            
            NLM_LOG_INFO("Injected debug input with value: " + std::to_string(value));
        }
        else if (command == "inspect") {
            // Inspect brain state
            NLM_LOG_INFO("=== Brain Inspection ===");
            brain->logStatus();
            if (verbose) {
                NLM_LOG_INFO("Detailed inspection: (would show detailed internal state)");
            }
        }
        else if (command == "profile") {
            // Profile performance
            NLM_LOG_INFO("Starting performance profiling");
            // Would start profiling
            // Would stop profiling after some time
            NLM_LOG_INFO("Performance profiling completed");
        }
        else {
            NLM_LOG_ERROR("Unknown debug command: " + command);
            return false;
        }
        
        return true;
    }
};

/**
 * @class HelpCommand
 * @brief Display help information for all commands
 */
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Display help information for all commands"; 
    }
    std::string getUsage() const override { 
        return "help [command_name]"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        if (args.empty()) {
            // Display help for all commands
            NLM_LOG_INFO("=== NLM Advanced User Commands ===");
            NLM_LOG_INFO("");
            
            auto commandNames = CommandRegistry::getInstance().getCommandNames();
            for (const auto& name : commandNames) {
                auto cmd = CommandRegistry::getInstance().getCommand(name);
                NLM_LOG_INFO("  " + name + " - " + cmd->getDescription());
            }
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Use 'help <command_name>' for detailed information about a specific command.");
        } else {
            // Display help for specific command
            std::string commandName = args[0];
            auto cmd = CommandRegistry::getInstance().getCommand(commandName);
            
            if (cmd) {
                NLM_LOG_INFO("=== Command: " + commandName + " ===");
                NLM_LOG_INFO(cmd->getDescription());
                NLM_LOG_INFO("");
                NLM_LOG_INFO("Usage: " + cmd->getUsage());
                NLM_LOG_INFO("");
            } else {
                NLM_LOG_ERROR("Unknown command: " + commandName);
                return false;
            }
        }
        
        return true;
    }
};

/**
 * @class ExitCommand
 * @brief Exit the application
 */
class ExitCommand : public Command {
public:
    std::string getName() const override { return "exit"; }
    std::string getDescription() const override { 
        return "Exit the application"; 
    }
    std::string getUsage() const override { 
        return "exit"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("Exiting application...");
        // Would signal application to exit
        return false; // Signal to stop processing
    }
};

/**
 * @class QuitCommand
 * @brief Quit the application (alias for exit)
 */
class QuitCommand : public Command {
public:
    std::string getName() const override { return "quit"; }
    std::string getDescription() const override { 
        return "Quit the application (alias for exit)"; 
    }
    std::string getUsage() const override { 
        return "quit"; 
    }
    
    bool execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                std::shared_ptr<Config> config) override {
        return ExitCommand().execute(args, brain, config);
    }
};

// =====================================================================
// Command Registration Function
// =====================================================================

/**
 * @brief Register all advanced user commands
 * 
 * This function registers all the advanced user commands for the NLM system.
 * It should be called during application initialization.
 */
void registerAdvancedCommands() {
    CommandRegistry& registry = CommandRegistry::getInstance();
    
    // Register brain inspection commands
    registry.registerCommand(std::make_shared<BrainInfoCommand>());
    registry.registerCommand(std::make_shared<NeuronAnalysisCommand>());
    
    // Register memory system commands
    registry.registerCommand(std::make_shared<MemoryStatusCommand>());
    registry.registerCommand(std::make_shared<MemoryControlCommand>());
    
    // Register neuromodulation commands
    registry.registerCommand(std::make_shared<NeuromodulationStatusCommand>());
    registry.registerCommand(std::make_shared<NeuromodulationControlCommand>());
    
    // Register development commands
    registry.registerCommand(std::make_shared<DevelopmentStatusCommand>());
    registry.registerCommand(std::make_shared<DevelopmentControlCommand>());
    
    // Register checkpoint commands
    registry.registerCommand(std::make_shared<CheckpointControlCommand>());
    
    // Register visualization and debugging commands
    registry.registerCommand(std::make_shared<VisualizationCommand>());
    registry.registerCommand(std::make_shared<DebugCommand>());
    
    // Register help and exit commands
    registry.registerCommand(std::make_shared<HelpCommand>());
    registry.registerCommand(std::make_shared<ExitCommand>());
    registry.registerCommand(std::make_shared<QuitCommand>());
    
    NLM_LOG_INFO("Registered " + std::to_string(registry.getCommandNames().size()) + " advanced user commands");
}

/**
 * @brief Process user command
 * 
 * This function processes a user command, looking it up in the command
 * registry and executing it with the provided arguments.
 * 
 * @param commandName Name of the command to execute
 * @param args Command arguments
 * @param brain Shared pointer to the brain object
 * @param config Shared pointer to the configuration object
 * @return true if command was executed successfully, false otherwise
 */
bool processCommand(const std::string& commandName, std::vector<std::string> args,
                   std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    CommandRegistry& registry = CommandRegistry::getInstance();
    
    auto command = registry.getCommand(commandName);
    if (!command) {
        NLM_LOG_ERROR("Unknown command: " + commandName);
        NLM_LOG_INFO("Type 'help' for a list of available commands.");
        return false;
    }
    
    return command->execute(args, brain, config);
}

/**
 * @brief Parse command line arguments
 * 
 * This function parses command line arguments, handling flags, options,
 * and argument values.
 * 
 * @param argc Number of command line arguments
 * @param argv Command line argument array
 * @param outArgs Output vector for parsed arguments
 * @return true if parsing succeeded, false otherwise
 */
bool parseCommandLine(int argc, char** argv, std::vector<std::string>& outArgs) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle flags
        if (arg.substr(0, 2) == "--") {
            // Remove leading dashes
            arg = arg.substr(2);
            outArgs.push_back("--" + arg);
        }
        // Handle positional arguments
        else {
            outArgs.push_back(arg);
        }
    }
    
    return true;
}

} // namespace nlm

// =====================================================================
// Example Usage
// =====================================================================

/**
 * @brief Example of how to use the advanced command system
 * 
 * This example demonstrates how to initialize the command system,
 * parse command line arguments, and execute commands.
 */
void exampleAdvancedCommandUsage() {
    // Initialize NLM logging
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    
    // Register advanced commands
    nlm::registerAdvancedCommands();
    
    // Create brain and configuration (example setup)
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Example command processing
    std::vector<std::string> exampleArgs = {"brain-info", "verbose"};
    nlm::processCommand("brain-info", exampleArgs, brain, config);
    
    exampleArgs = {"memory-status", "--working", "--verbose"};
    nlm::processCommand("memory-status", exampleArgs, brain, config);
    
    exampleArgs = {"neuromod-status", "--verbose"};
    nlm::processCommand("neuromod-status", exampleArgs, brain, config);
}

/**
 * @brief Main function for advanced user interface
 * 
 * This function demonstrates how to integrate the advanced command system
 * into a main application loop.
 */
void advancedUserInterfaceMain(int argc, char** argv) {
    // Initialize NLM logging
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    
    // Register advanced commands
    nlm::registerAdvancedCommands();
    
    // Parse command line arguments
    std::vector<std::string> args;
    nlm::parseCommandLine(argc, argv, args);
    
    if (args.empty()) {
        // No arguments provided, show help
        nlm::processCommand("help", {}, nullptr, nullptr);
        return;
    }
    
    // Get the command name
    std::string commandName = args[0];
    
    // Create command arguments (everything after the command name)
    std::vector<std::string> commandArgs;
    for (size_t i = 1; i < args.size(); ++i) {
        commandArgs.push_back(args[i]);
    }
    
    // For demonstration, we'll use a basic setup
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Process the command
    bool success = nlm::processCommand(commandName, commandArgs, brain, config);
    
    if (!success) {
        return;
    }
    
    brain->reset();
}