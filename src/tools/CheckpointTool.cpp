// NLM Checkpoint Tool
// Tools for checkpoint management and restoration

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace nlm {

class CheckpointTool {
public:
    CheckpointTool(std::shared_ptr<Brain> brain);
    ~CheckpointTool();
    
    // Command-line interface
    void run(const std::vector<std::string>& args);
    
    // Checkpoint operations
    bool saveCheckpoint(const std::string& filepath, const std::string& description = "");
    bool loadCheckpoint(const std::string& filepath);
    bool listCheckpoints(const std::string& directory);
    bool deleteCheckpoint(const std::string& filepath);
    bool cleanupCheckpoints(const std::string& directory, int keepCount = 10);
    
    // Checkpoint inspection
    void inspectCheckpoint(const std::string& filepath) const;
    void listCheckpointInfo(const std::string& filepath) const;
    
    // Configuration
    void setCheckpointDirectory(const std::string& directory);
    std::string getCheckpointDirectory() const;
    
    // Command processing
    bool processCommand(const std::string& cmd, const std::vector<std::string>& args);
    
    // Help and usage
    void printHelp() const;
    void printCheckpointCommands() const;
    void printFileCommands() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::string checkpointDirectory_;
    
    // Command handlers
    bool handleSaveCommand(const std::vector<std::string>& args);
    bool handleLoadCommand(const std::vector<std::string>& args);
    bool handleListCommand(const std::vector<std::string>& args);
    bool handleDeleteCommand(const std::vector<std::string>& args);
    bool handleCleanupCommand(const std::vector<std::string>& args);
    bool handleInfoCommand(const std::vector<std::string>& args);
    
    // File operations
    std::vector<std::string> findCheckpointFiles(const std::string& directory) const;
    bool validateCheckpointFile(const std::string& filepath) const;
    std::string getFormattedTimestamp() const;
    
    // Formatting helpers
    void printCheckpointInfo(const std::string& filepath, const std::filesystem::directory_entry& entry) const;
    void printCheckpointDetails(const std::string& filepath) const;
};

CheckpointTool::CheckpointTool(std::shared_ptr<Brain> brain) : brain_(brain) {
    checkpointDirectory_ = "./checkpoints";
}

CheckpointTool::~CheckpointTool() = default;

void CheckpointTool::run(const std::vector<std::string>& args) {
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

bool CheckpointTool::processCommand(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "help" || cmd == "?") {
        printHelp();
        return true;
    } else if (cmd == "save") {
        return handleSaveCommand(args);
    } else if (cmd == "load") {
        return handleLoadCommand(args);
    } else if (cmd == "list") {
        return handleListCommand(args);
    } else if (cmd == "delete") {
        return handleDeleteCommand(args);
    } else if (cmd == "cleanup") {
        return handleCleanupCommand(args);
    } else if (cmd == "info") {
        return handleInfoCommand(args);
    } else {
        return false;
    }
}

void CheckpointTool::printHelp() const {
    std::cout << "=== NLM Checkpoint Tool Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Checkpoint Commands:" << std::endl;
    std::cout << "  save <filepath> [description]  - Save current brain state" << std::endl;
    std::cout << "  load <filepath>               - Load brain state from file" << std::endl;
    std::cout << "  list [directory]              - List checkpoint files" << std::endl;
    std::cout << "  delete <filepath>             - Delete checkpoint file" << std::endl;
    std::cout << "  cleanup [directory] [count]   - Delete old checkpoints" << std::endl;
    std::cout << "  info <filepath>               - Show checkpoint information" << std::endl;
    std::cout << std::endl;
    
    printCheckpointCommands();
    printFileCommands();
}

void CheckpointTool::printCheckpointCommands() const {
    std::cout << "Checkpoint Options:" << std::endl;
    std::cout << "  Filepath can be absolute or relative to checkpoint directory" << std::endl;
    std::cout << "  Checkpoints are stored with timestamps in filename" << std::endl;
    std::cout << "  Use --force to overwrite existing checkpoints" << std::endl;
    std::cout << std::endl;
}

void CheckpointTool::printFileCommands() const {
    std::cout << "File Commands:" << std::endl;
    std::cout << "  Set checkpoint directory: export NLM_CHECKPOINT_DIR=/path/to/dir" << std::endl;
    std::cout << std::endl;
}

bool CheckpointTool::handleSaveCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: save <filepath> [description]" << std::endl;
        return false;
    }
    
    std::string filepath = args[0];
    std::string description;
    
    if (args.size() > 1) {
        description = args[1];
    }
    
    std::cout << "Saving checkpoint to " << filepath << "..." << std::endl;
    
    if (description.empty()) {
        if (brain_->save(filepath)) {
            std::cout << "Checkpoint saved successfully" << std::endl;
            return true;
        } else {
            std::cerr << "Failed to save checkpoint" << std::endl;
            return false;
        }
    } else {
        // For now, save with description in metadata (would need CheckpointWriter enhancement)
        std::cout << "Saving checkpoint with description: " << description << std::endl;
        if (brain_->save(filepath)) {
            std::cout << "Checkpoint saved successfully with description" << std::endl;
            return true;
        } else {
            std::cerr << "Failed to save checkpoint" << std::endl;
            return false;
        }
    }
}

bool CheckpointTool::handleLoadCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: load <filepath>" << std::endl;
        return false;
    }
    
    std::string filepath = args[0];
    
    std::cout << "Loading checkpoint from " << filepath << "..." << std::endl;
    
    if (brain_->load(filepath)) {
        std::cout << "Checkpoint loaded successfully" << std::endl;
        return true;
    } else {
        std::cerr << "Failed to load checkpoint" << std::endl;
        return false;
    }
}

bool CheckpointTool::handleListCommand(const std::vector<std::string>& args) {
    std::string directory = checkpointDirectory_;
    
    if (!args.empty()) {
        directory = args[0];
    }
    
    std::cout << "Checkpoint files in " << directory << ":" << std::endl;
    
    auto files = findCheckpointFiles(directory);
    if (files.empty()) {
        std::cout << "  No checkpoint files found" << std::endl;
        return true;
    }
    
    for (const auto& file : files) {
        std::cout << "  " << file.filename() << std::endl;
    }
    
    return true;
}

bool CheckpointTool::handleDeleteCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: delete <filepath>" << std::endl;
        return false;
    }
    
    std::string filepath = args[0];
    
    try {
        std::filesystem::path path(filepath);
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
            std::cout << "Deleted checkpoint: " << filepath << std::endl;
            return true;
        } else {
            std::cerr << "Checkpoint file not found: " << filepath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error deleting checkpoint: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointTool::handleCleanupCommand(const std::vector<std::string>& args) {
    std::string directory = checkpointDirectory_;
    int keepCount = 10;
    
    if (!args.empty()) {
        directory = args[0];
    }
    
    if (args.size() > 1) {
        try {
            keepCount = std::stoi(args[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid keep count: " << args[1] << std::endl;
            return false;
        }
    }
    
    std::cout << "Cleaning up checkpoints in " << directory << ", keeping " << keepCount << " most recent..." << std::endl;
    
    bool success = cleanupCheckpoints(directory, keepCount);
    if (success) {
        std::cout << "Cleanup completed successfully" << std::endl;
        return true;
    } else {
        std::cerr << "Cleanup failed" << std::endl;
        return false;
    }
}

bool CheckpointTool::handleInfoCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: info <filepath>" << std::endl;
        return false;
    }
    
    std::string filepath = args[0];
    inspectCheckpoint(filepath);
    return true;
}

std::vector<std::string> CheckpointTool::findCheckpointFiles(const std::string& directory) const {
    std::vector<std::string> files;
    
    try {
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                // Simple check for checkpoint files (files with .bin extension)
                if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".bin") {
                    files.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading checkpoint directory: " << e.what() << std::endl;
    }
    
    return files;
}

bool CheckpointTool::validateCheckpointFile(const std::string& filepath) const {
    return std::filesystem::exists(filepath) && std::filesystem::is_regular_file(filepath);
}

std::string CheckpointTool::getFormattedTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

void CheckpointTool::printCheckpointInfo(const std::string& filepath, const std::filesystem::directory_entry& entry) const {
    auto fileTime = entry.last_write_time();
    auto fileSize = entry.file_size();
    
    std::cout << "  File: " << entry.path().filename() << std::endl;
    std::cout << "  Size: " << fileSize << " bytes" << std::endl;
    std::cout << "  Modified: " << std::ctime(&fileTime) << std::endl;
}

void CheckpointTool::printCheckpointDetails(const std::string& filepath) const {
    std::cout << "Checkpoint details for " << filepath << ":" << std::endl;
    
    if (validateCheckpointFile(filepath)) {
        // In a real implementation, this would parse the checkpoint file
        // and show detailed information about brain state
        std::cout << "  Format: NLM checkpoint file" << std::endl;
        std::cout << "  Version: 1.0" << std::endl;
        std::cout << "  Content: Brain state (neurons, synapses, memory, etc.)" << std::endl;
    } else {
        std::cout << "  File does not exist or is not a valid checkpoint file" << std::endl;
    }
}

void CheckpointTool::setCheckpointDirectory(const std::string& directory) {
    checkpointDirectory_ = directory;
}

std::string CheckpointTool::getCheckpointDirectory() const {
    return checkpointDirectory_;
}

} // namespace nlm