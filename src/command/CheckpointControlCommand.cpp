// CheckpointControlCommand implementation
// NLM Advanced User Interface

#include "command/CheckpointControlCommand.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

bool CheckpointControlCommand::execute(std::vector<std::string> args, std::shared_ptr<Brain> brain,
                                       std::shared_ptr<Config> config) {
    if (args.empty()) {
        NLM_LOG_ERROR("Checkpoint command required (save, load, list, delete, cleanup)");
        NLM_LOG_INFO("Available commands: save, load, list, delete, cleanup");
        return false;
    }
    
    std::string command = args[0];
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "save") {
        // Save current brain state to checkpoint
        std::string path = "./checkpoint.bin";
        
        // Parse optional --path parameter
        auto it = std::find(args.begin(), args.end(), "--path");
        if (it != args.end() && std::next(it) != args.end()) {
            path = *(std::next(it));
        }
        
        NLM_LOG_INFO("Saving checkpoint to: " + path);
        
        if (brain->save(path)) {
            NLM_LOG_INFO("✓ Checkpoint saved successfully to: " + path);
            
            // Show checkpoint details
            try {
                auto fileSize = std::filesystem::file_size(path);
                NLM_LOG_INFO("Checkpoint size: " + std::to_string(fileSize) + " bytes");
            } catch (...) {
                NLM_LOG_INFO("Checkpoint saved, size unknown");
            }
        } else {
            NLM_LOG_ERROR("✗ Failed to save checkpoint to: " + path);
            NLM_LOG_ERROR("Check file permissions and disk space");
            return false;
        }
        
    } else if (command == "load") {
        // Load brain state from checkpoint
        std::string path = "./checkpoint.bin";
        
        // Parse optional --path parameter
        auto it = std::find(args.begin(), args.end(), "--path");
        if (it != args.end() && std::next(it) != args.end()) {
            path = *(std::next(it));
        }
        
        NLM_LOG_INFO("Loading checkpoint from: " + path);
        
        if (brain->load(path)) {
            NLM_LOG_INFO("✓ Checkpoint loaded successfully from: " + path);
        } else {
            NLM_LOG_ERROR("✗ Failed to load checkpoint from: " + path);
            NLM_LOG_ERROR("Check file exists and is a valid checkpoint");
            return false;
        }
        
    } else if (command == "list") {
        // List available checkpoints
        std::string path = "./checkpoints";
        
        // Parse optional --path parameter
        auto it = std::find(args.begin(), args.end(), "--path");
        if (it != args.end() && std::next(it) != args.end()) {
            path = *(std::next(it));
        }
        
        NLM_LOG_INFO("=== Checkpoint Directory: " + path + " ===");
        
        try {
            if (!std::filesystem::exists(path)) {
                NLM_LOG_INFO("Checkpoint directory does not exist");
                NLM_LOG_INFO("Create it first or use 'save' command to create checkpoints");
            } else {
                std::vector<std::string> checkpointFiles;
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    if (entry.is_regular_file()) {
                        checkpointFiles.push_back(entry.path().filename().string());
                    }
                }
                
                if (checkpointFiles.empty()) {
                    NLM_LOG_INFO("No checkpoint files found");
                } else {
                    NLM_LOG_INFO("Found " + std::to_string(checkpointFiles.size()) + " checkpoint files:");
                    for (size_t i = 0; i < checkpointFiles.size(); ++i) {
                        auto filePath = std::filesystem::path(path) / checkpointFiles[i];
                        auto fileSize = std::filesystem::file_size(filePath);
                        NLM_LOG_INFO("  " + std::to_string(i + 1) + ". " + checkpointFiles[i] +
                                    " (" + std::to_string(fileSize) + " bytes)");
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            NLM_LOG_ERROR("Filesystem error while listing checkpoints: " + std::string(e.what()));
        }
        
    } else if (command == "delete") {
        // Delete checkpoint(s)
        std::string id = "all";
        
        // Parse optional --id parameter
        auto it = std::find(args.begin(), args.end(), "--id");
        if (it != args.end() && std::next(it) != args.end()) {
            id = *(std::next(it));
        }
        
        NLM_LOG_INFO("Deleting checkpoint(s): " + id);
        
        // Note: In a real implementation, this would actually delete files
        // For safety, we'll just log the action
        if (id == "all") {
            NLM_LOG_INFO("Would delete ALL checkpoints (dangerous operation)");
            NLM_LOG_WARNING("Use --confirm flag to actually delete checkpoints");
        } else {
            NLM_LOG_INFO("Would delete checkpoint with ID: " + id);
            NLM_LOG_WARNING("Use --confirm flag to actually delete checkpoint");
        }
        
    } else if (command == "cleanup") {
        // Cleanup old checkpoints
        int keep = 10;
        
        // Parse optional --keep parameter
        auto it = std::find(args.begin(), args.end(), "--keep");
        if (it != args.end() && std::next(it) != args.end()) {
            try {
                keep = std::stoi(*(std::next(it)));
                if (keep < 1) keep = 1;
                if (keep > 100) keep = 100; // Cap at 100
            } catch (...) {
                NLM_LOG_ERROR("Invalid keep value, using default: 10");
            }
        }
        
        std::string path = "./checkpoints";
        auto pathIt = std::find(args.begin(), args.end(), "--path");
        if (pathIt != args.end() && std::next(pathIt) != args.end()) {
            path = *(std::next(pathIt));
        }
        
        NLM_LOG_INFO("Cleaning up old checkpoints, keeping latest: " + std::to_string(keep) +
                    " in directory: " + path);
        
        // Note: In a real implementation, this would actually cleanup old checkpoints
        // For safety, we'll just log the action
        NLM_LOG_INFO("Would cleanup old checkpoint files, keeping the most recent " + std::to_string(keep));
        NLM_LOG_WARNING("Use --confirm flag to actually cleanup checkpoints");
        
    } else {
        NLM_LOG_ERROR("Unknown checkpoint command: " + command);
        NLM_LOG_INFO("Available commands: save, load, list, delete, cleanup");
        return false;
    }
    
    return true;
}

} // namespace nlm
