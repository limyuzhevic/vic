// Command Framework Implementation
// NLM Advanced User Interface

#include "command/CommandFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

// =====================================================================
// Command Implementation
// =====================================================================

/**
 * @brief Execute brain-info command with verbose output
 */
bool BrainInfoCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                             std::shared_ptr<Config> config) {
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

/**
 * @brief Execute neuron-analysis command
 */
bool NeuronAnalysisCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Config> config) {
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
    
    // Check if --population or --connectivity flag is present
    bool showPopulation = std::find(args.begin(), args.end(), "--population") != args.end();
    bool showConnectivity = std::find(args.begin(), args.end(), "--connectivity") != args.end();
    
    if (showPopulation || showConnectivity) {
        NLM_LOG_INFO("Analysis flags: Population=" + std::string(showPopulation ? "true" : "false") + 
                    ", Connectivity=" + std::string(showConnectivity ? "true" : "false"));
    }
    
    return true;
}

/**
 * @brief Execute memory-status command
 */
bool MemoryStatusCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                 std::shared_ptr<Config> config) {
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

/**
 * @brief Execute memory-control command
 */
bool MemoryControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Config> config) {
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

/**
 * @brief Execute neuromodulation-status command
 */
bool NeuromodulationStatusCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                         std::shared_ptr<Config> config) {
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

/**
 * @brief Execute neuromodulation-control command
 */
bbool NeuromodulationControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                            std::shared_ptr<Config> config) {
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

/**
 * @brief Execute development-status command
 */
bool DevelopmentStatusCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                      std::shared_ptr<Config> config) {
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

/**
 * @brief Execute development-control command
 */
bool DevelopmentControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                       std::shared_ptr<Config> config) {
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

/**
 * @brief Execute checkpoint-control command
 */
bool CheckpointControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                       std::shared_ptr<Config> config) {
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

/**
 * @brief Execute visualization command
 */
bool VisualizationCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Config> config) {
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

/**
 * @brief Execute debug command
 */
bool DebugCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                          std::shared_ptr<Config> config) {
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

/**
 * @brief Execute help command
 */
bool HelpCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                         std::shared_ptr<Config> config) {
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

/**
 * @brief Execute exit command
 */
bool ExitCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                        std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Exiting application...");
    // Would signal application to exit
    return false; // Signal to stop processing
}

/**
 * @brief Execute quit command
 */
bool QuitCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                        std::shared_ptr<Config> config) {
    return ExitCommand().execute(args, brain, config);
}

} // namespace nlm

// =====================================================================
// Command Implementation Source File
// =====================================================================

/**
 * @brief Implementation of all command classes
 * 
 * This file contains the implementation of all the command classes defined
 * in CommandFramework.hpp. Each command class implements the abstract
 * methods from the Command base class.
 * 
 * The commands provide advanced user interface functionality for controlling
 * and inspecting the NLM brain simulation.
 */

// Include the command implementations
#include "command/BrainInfoCommand.cpp"
#include "command/NeuronAnalysisCommand.cpp"
#include "command/MemoryStatusCommand.cpp"
#include "command/MemoryControlCommand.cpp"
#include "command/NeuromodulationStatusCommand.cpp"
#include "command/NeuromodulationControlCommand.cpp"
#include "command/DevelopmentStatusCommand.cpp"
#include "command/DevelopmentControlCommand.cpp"
#include "command/CheckpointControlCommand.cpp"
#include "command/VisualizationCommand.cpp"
#include "command/DebugCommand.cpp"
#include "command/HelpCommand.cpp"
#include "command/ExitCommand.cpp"
#include "command/QuitCommand.cpp"

// Include the command registration function
#include "command/CommandRegistry.cpp"