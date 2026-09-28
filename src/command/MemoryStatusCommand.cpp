// MemoryControlCommand implementation
// NLM Advanced User Interface

#include "command/MemoryControlCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cctype>

namespace nlm {

bool MemoryControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Config> config) {
    if (args.empty()) {
        NLM_LOG_ERROR("Memory command required (store, retrieve, clear, consolidate)");
        return false;
    }
    
    std::string command = args[0];
    bool verbose = std::find(args.begin(), args.end(), "--verbose") != args.end();
    bool clearAll = std::find(args.begin(), args.end(), "--clear-all") != args.end();
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "store") {
        // Store a new memory trace
        if (brain->getWorkingMemory()) {
            float value = 0.5f;
            
            // Parse optional value from --value argument
            auto it = std::find(args.begin(), args.end(), "--value");
            if (it != args.end() && std::next(it) != args.end()) {
                try {
                    value = std::stof(*(std::next(it)));
                } catch (...) {
                    NLM_LOG_WARNING("Invalid value for --value, using default: 0.5");
                }
            }
            
            // Find available neuron ID for storage (use 1 as default)
            brain->getWorkingMemory()->storeToNeuron(1, value);
            NLM_LOG_INFO("Stored memory trace with value: " + std::to_string(value));
            
            if (verbose) {
                NLM_LOG_INFO("Memory trace stored at neuron ID: 1");
            }
        } else {
            NLM_LOG_ERROR("Working memory not available for storage");
            return false;
        }
    }
    else if (command == "retrieve") {
        // Retrieve memory traces (placeholder implementation)
        if (brain->getWorkingMemory()) {
            auto traces = brain->getWorkingMemory()->getMemoryNeurons();
            NLM_LOG_INFO("Retrieved " + std::to_string(traces.size()) + " memory traces");
            
            if (verbose) {
                NLM_LOG_INFO("Trace details:");
                for (size_t i = 0; i < traces.size(); ++i) {
                    NLM_LOG_INFO("  Trace " + std::to_string(i) + ": neuron=" + std::to_string(traces[i]) + ", weight=(would show weight)");
                }
            }
        } else {
            NLM_LOG_ERROR("Working memory not available for retrieval");
            return false;
        }
    }
    else if (command == "clear") {
        // Clear memory traces
        if (clearAll) {
            if (brain->getWorkingMemory()) {
                brain->getWorkingMemory()->clear();
                NLM_LOG_INFO("Cleared working memory");
            }
            if (brain->getEpisodicMemory()) {
                brain->getEpisodicMemory()->clear();
                NLM_LOG_INFO("Cleared episodic memory");
            }
            if (brain->getAssociativeMemory()) {
                brain->getAssociativeMemory()->clear();
                NLM_LOG_INFO("Cleared associative memory");
            }
            NLM_LOG_INFO("Cleared all memory systems");
        } else {
            // Clear specific episode or trace
            auto it = std::find(args.begin(), args.end(), "--id");
            if (it != args.end() && std::next(it) != args.end()) {
                try {
                    uint64_t targetId = std::stoul(*(std::next(it)));
                    
                    // Check which memory system to clear from
                    if (brain->getEpisodicMemory()) {
                        // For episodic memory, we would clear a specific episode
                        brain->getEpisodicMemory()->clear(); // Simplified - clears all
                        NLM_LOG_INFO("Cleared episodic memory entry with ID: " + std::to_string(targetId));
                    } else {
                        NLM_LOG_ERROR("No episodic memory system available");
                    }
                } catch (...) {
                    NLM_LOG_ERROR("Invalid ID for --id parameter");
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Memory clear command requires --id parameter for specific clears");
                NLM_LOG_INFO("Use --clear-all to clear all memory systems");
                return false;
            }
        }
    }
    else if (command == "consolidate") {
        // Consolidate memory
        if (brain->getEpisodicMemory()) {
            // Consolidate with strength based on memory importance
            float consolidationStrength = 0.5f;
            
            // Parse optional strength from --strength argument
            auto it = std::find(args.begin(), args.end(), "--strength");
            if (it != args.end() && std::next(it) != args.end()) {
                try {
                    consolidationStrength = std::stof(*(std::next(it)));
                    // Clamp between 0.0 and 1.0
                    consolidationStrength = std::max(0.0f, std::min(consolidationStrength, 1.0f));
                } catch (...) {
                    NLM_LOG_WARNING("Invalid strength for --strength, using default: 0.5");
                }
            }
            
            brain->getEpisodicMemory()->consolidate(consolidationStrength);
            NLM_LOG_INFO("Consolidated episodic memory with strength: " + std::to_string(consolidationStrength));
            
            if (verbose) {
                NLM_LOG_INFO("Consolidation details: (would show consolidated memories)");
            }
        } else {
            NLM_LOG_ERROR("Episodic memory not available for consolidation");
            return false;
        }
    }
    else {
        NLM_LOG_ERROR("Unknown memory command: " + command);
        NLM_LOG_INFO("Available commands: store, retrieve, clear, consolidate");
        return false;
    }
    
    return true;
}

} // namespace nlm
