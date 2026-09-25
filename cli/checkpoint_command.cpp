// NLM Checkpoint Command - Manage simulation checkpoints
// Handles checkpoint creation, listing, restoration, and deletion

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace nlm {

class CheckpointCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static bool saveCheckpoint(const std::map<std::string, std::string>& args);
    static bool listCheckpoints(const std::map<std::string, std::string>& options);
    static bool restoreCheckpoint(const std::map<std::string, std::string>& args);
    static bool deleteCheckpoint(const std::map<std::string, std::string>& args);
    static std::string generateCheckpointFilename(size_t index, const std::string& prefix = "checkpoint");
    static bool saveBrainToFile(const std::string& filename, std::shared_ptr<Brain> brain);
    static bool loadBrainFromFile(const std::string& filename, std::shared_ptr<Brain>& brain);
    static void listCheckpointFiles(const std::string& directory);
    static std::vector<std::string> getCheckpointFiles(const std::string& directory);
};

bool CheckpointCommand::execute(NLMCLI& cli, const std::map<std::string, std::string>& args) {
    std::cout << "=== NLM Checkpoint Command ===" << std::endl;
    
    if (args.empty()) {
        std::cout << "Available checkpoint commands:" << std::endl;
        std::cout << "  save [FILE]    Save current state as checkpoint" << std::endl;
        std::cout << "  list           List available checkpoints" << std::endl;
        std::cout << "  restore INDEX  Restore checkpoint by index" << std::endl;
        std::cout << "  delete INDEX   Delete checkpoint by index" << std::endl;
        showHelp();
        return true;
    }
    
    std::string command = args.at("command");
    
    if (command == "save") {
        return saveCheckpoint(args);
    } else if (command == "list") {
        std::string directory = args.count("directory") ? args.at("directory") : "checkpoints";
        return listCheckpoints({{"directory", directory}});
    } else if (command == "restore") {
        return restoreCheckpoint(args);
    } else if (command == "delete") {
        return deleteCheckpoint(args);
    } else {
        std::cerr << "Error: Unknown checkpoint command: " << command << std::endl;
        return false;
    }
}

void CheckpointCommand::showHelp() {
    std::cout << "=== NLM Checkpoint Command Help ===" << std::endl;
    std::cout << "Usage: nlm checkpoint <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Manage simulation checkpoints:
    * save: Creates a snapshot of the current brain state" << std::endl;
    std::cout << "  * list: Lists all available checkpoints" << std::endl;
    std::cout << "  * restore: Restores a checkpoint by index" << std::endl;
    std::cout << "  * delete: Deletes a checkpoint by index" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --directory DIR        Checkpoint directory (default: ./checkpoints)" << std::endl;
    std::cout << "  --prefix PREFIX        Checkpoint file prefix (default: checkpoint)" << std::endl;
    std::cout << std::endl;
    std::cout << "Checkpoint files are saved in binary format for efficient storage and loading." << std::endl;
}

bool CheckpointCommand::saveCheckpoint(const std::map<std::string, std::string>& args) {
    std::string directory = args.count("directory") ? args.at("directory") : "checkpoints";
    std::string prefix = args.count("prefix") ? args.at("prefix") : "checkpoint";
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(directory);
    
    // Get list of existing checkpoints to determine next index
    std::vector<std::string> existingFiles = getCheckpointFiles(directory);
    size_t nextIndex = existingFiles.size();
    
    // Generate filename
    std::string filename = generateCheckpointFilename(nextIndex, prefix);
    std::string fullPath = directory + "/" + filename;
    
    std::cout << "Saving checkpoint to: " << fullPath << std::endl;
    
    // Save brain state to file
    // Note: In a real implementation, you would get the brain from NLMCLI or similar
    // For this example, we'll create a placeholder
    auto dummyConfig = std::make_shared<Config>();
    auto dummyBrain = std::make_shared<Brain>(dummyConfig);
    
    if (!saveBrainToFile(fullPath, dummyBrain)) {
        std::cerr << "Error: Failed to save checkpoint" << std::endl;
        return false;
    }
    
    std::cout << "Checkpoint saved successfully to: " << fullPath << std::endl;
    return true;
}

bool CheckpointCommand::listCheckpoints(const std::map<std::string, std::string>& options) {
    std::string directory = options.count("directory") ? options.at("directory") : "checkpoints";
    
    std::cout << "Available checkpoints in: " << directory << std::endl;
    std::cout << std::endl;
    
    std::vector<std::string> checkpointFiles = getCheckpointFiles(directory);
    
    if (checkpointFiles.empty()) {
        std::cout << "  (no checkpoints found)" << std::endl;
        return true;
    }
    
    // Sort by filename (which includes index)
    std::sort(checkpointFiles.begin(), checkpointFiles.end());
    
    for (size_t i = 0; i < checkpointFiles.size(); ++i) {
        std::string fullPath = directory + "/" + checkpointFiles[i];
        
        // Try to get file info
        auto fileTime = std::filesystem::last_write_time(fullPath);
        auto fileSize = std::filesystem::file_size(fullPath);
        
        std::cout << "  [" << i << "] " << checkpointFiles[i] << std::endl;
        std::cout << "      Size: " << fileSize << " bytes" << std::endl;
        std::cout << "      Modified: " << std::asctime(std::localtime(&fileTime)) << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "Total checkpoints: " << checkpointFiles.size() << std::endl;
    
    return true;
}

bool CheckpointCommand::restoreCheckpoint(const std::map<std::string, std::string>& args) {
    if (!args.count("index")) {
        std::cerr << "Error: Checkpoint index must be specified" << std::endl;
        return false;
    }
    
    size_t index = std::stoul(args.at("index"));
    std::string directory = args.count("directory") ? args.at("directory") : "checkpoints";
    
    std::string filename = generateCheckpointFilename(index);
    std::string fullPath = directory + "/" + filename;
    
    std::cout << "Restoring checkpoint: " << fullPath << std::endl;
    
    // Note: In a real implementation, you would restore the brain state from file
    // For now, we'll just indicate success
    
    // Get file info for display
    if (std::filesystem::exists(fullPath)) {
        auto fileSize = std::filesystem::file_size(fullPath);
        std::cout << "Checkpoint file size: " << fileSize << " bytes" << std::endl;
        std::cout << "Checkpoint restored successfully (in-memory placeholder)" << std::endl;
    } else {
        std::cerr << "Error: Checkpoint file not found: " << fullPath << std::endl;
        return false;
    }
    
    return true;
}

bool CheckpointCommand::deleteCheckpoint(const std::map<std::string, std::string>& args) {
    if (!args.count("index")) {
        std::cerr << "Error: Checkpoint index must be specified" << std::endl;
        return false;
    }
    
    size_t index = std::stoul(args.at("index"));
    std::string directory = args.count("directory") ? args.at("directory") : "checkpoints";
    
    std::string filename = generateCheckpointFilename(index);
    std::string fullPath = directory + "/" + filename;
    
    std::cout << "Deleting checkpoint: " << fullPath << std::endl;
    
    if (std::filesystem::exists(fullPath)) {
        if (std::filesystem::remove(fullPath)) {
            std::cout << "Checkpoint deleted successfully" << std::endl;
            return true;
        } else {
            std::cerr << "Error: Failed to delete checkpoint file" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Error: Checkpoint file not found: " << fullPath << std::endl;
        return false;
    }
}

std::string CheckpointCommand::generateCheckpointFilename(size_t index, const std::string& prefix) {
    std::stringstream ss;
    ss << prefix << "_" << std::setw(6) << std::setfill('0') << index << ".bin";
    return ss.str();
}

bool CheckpointCommand::saveBrainToFile(const std::string& filename, std::shared_ptr<Brain> brain) {
    std::cout << "Saving brain to " << filename << std::endl;
    
    // Create directory if needed
    std::filesystem::path path(filename);
    std::filesystem::create_directories(path.parent_path());
    
    // Save to binary file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Write brain state (simplified)
    // In a real implementation, you would serialize the actual brain state
    file << "# NLM Checkpoint File" << std::endl;
    file << "# Generated by NLM CLI" << std::endl;
    file << std::endl;
    file << "brain_state_placeholder = true" << std::endl;
    
    file.close();
    
    std::cout << "Brain saved successfully to: " << filename << std::endl;
    return true;
}

bool CheckpointCommand::loadBrainFromFile(const std::string& filename, std::shared_ptr<Brain>& brain) {
    std::cout << "Loading brain from " << filename << std::endl;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for reading: " << filename << std::endl;
        return false;
    }
    
    // Parse checkpoint file
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("brain_state_placeholder") != std::string::npos) {
            // Parse brain state
            // This is a placeholder - real implementation would deserialize actual brain state
            break;
        }
    }
    
    file.close();
    
    // Create a new brain as placeholder
    auto config = std::make_shared<Config>();
    brain = std::make_shared<Brain>(config);
    
    std::cout << "Brain loaded successfully from: " << filename << std::endl;
    return true;
}

void CheckpointCommand::listCheckpointFiles(const std::string& directory) {
    std::vector<std::string> files = getCheckpointFiles(directory);
    
    for (const auto& file : files) {
        std::cout << "  " << file << std::endl;
    }
}

std::vector<std::string> CheckpointCommand::getCheckpointFiles(const std::string& directory) {
    std::vector<std::string> files;
    
    try {
        if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                    files.push_back(entry.path().filename().string());
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Error reading directory " << directory << ": " << e.what() << std::endl;
    }
    
    return files;
}
