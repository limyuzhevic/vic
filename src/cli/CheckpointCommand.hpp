// Checkpoint command implementation
#pragma once

#include "BaseCommand.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace nlm {
namespace cli {

class CheckpointCommand : public BaseCommand {
public:
    CheckpointCommand() : BaseCommand("checkpoint", "Checkpoint management commands") {}
    
    CommandResult execute() override {
        return CommandResult(0, "Checkpoint command requires subcommand: save, load, list, or remove\n");
    }
};

class CheckpointSaveCommand : public BaseCommand {
public:
    CheckpointSaveCommand() : BaseCommand("save", "Save checkpoint to file") {
        addArgument(Argument("file", "Checkpoint file path", "string", "", true));
        addArgument(Argument("description", "Checkpoint description", "string", ""));
    }
    
    void addArguments() override {
        addArgument(Argument("file", "Checkpoint file path", "string", "", true));
        addArgument(Argument("description", "Checkpoint description", "string", ""));
    }
    
    CommandResult execute() override {
        std::string filepath = getConfigValue("file");
        std::string description = getConfigValue("description", "");
        
        if (filepath.empty()) {
            return CommandResult(1, "Checkpoint file path cannot be empty");
        }
        
        std::stringstream output;
        output << "Saving checkpoint to: " << filepath << std::endl;
        
        if (!description.empty()) {
            output << "Description: " << description << std::endl;
        }
        
        // Create checkpoint file
        try {
            std::ofstream file(filepath, std::ios::binary);
            if (!file.is_open()) {
                return CommandResult(1, "Failed to create checkpoint file: " + filepath);
            }
            
            // Write checkpoint header
            file << "NLM_CHECKPOINT_V1" << std::endl;
            file << "Created: " << getCurrentTimestamp() << std::endl;
            
            if (!description.empty()) {
                file << "Description: " << description << std::endl;
            }
            
            // Write configuration if available
            if (config) {
                file << "Config:" << std::endl;
                auto keys = config->getKeys();
                for (const auto& key : keys) {
                    std::string valueStr;
                    if (config->get<std::string>(key)) {
                        valueStr = *config->get<std::string>(key);
                    } else if (config->get<int>(key)) {
                        valueStr = std::to_string(*config->get<int>(key));
                    } else if (config->get<double>(key)) {
                        valueStr = std::to_string(*config->get<double>(key));
                    } else if (config->get<bool>(key)) {
                        valueStr = *config->get<bool>(key) ? "true" : "false";
                    }
                    
                    file << "  " << key << " = " << valueStr << std::endl;
                }
            }
            
            file << "END_CHECKPOINT" << std::endl;
            
            output << "Checkpoint saved successfully" << std::endl;
            
        } catch (const std::exception& e) {
            return CommandResult(1, std::string("Error saving checkpoint: ") + e.what());
        }
        
        return CommandResult(0, output.str());
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch() % std::chrono::seconds(1)).count();
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms;
        return ss.str();
    }
};

class CheckpointLoadCommand : public BaseCommand {
public:
    CheckpointLoadCommand() : BaseCommand("load", "Load checkpoint from file") {
        addArgument(Argument("file", "Checkpoint file path", "string", "", true));
        addArgument(Argument("merge", "Merge with current configuration", "bool", "true"));
    }
    
    void addArguments() override {
        addArgument(Argument("file", "Checkpoint file path", "string", "", true));
        addArgument(Argument("merge", "Merge with current configuration", "bool", "true"));
    }
    
    CommandResult execute() override {
        std::string filepath = getConfigValue("file");
        bool merge = getConfigValue("merge", true);
        
        if (filepath.empty()) {
            return CommandResult(1, "Checkpoint file path cannot be empty");
        }
        
        if (!std::filesystem::exists(filepath)) {
            return CommandResult(1, "Checkpoint file not found: " + filepath);
        }
        
        std::stringstream output;
        output << "Loading checkpoint from: " << filepath << std::endl;
        
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return CommandResult(1, "Failed to open checkpoint file: " + filepath);
            }
            
            std::string line;
            bool inConfig = false;
            int lineNum = 0;
            
            while (std::getline(file, line)) {
                lineNum++;
                line = trim(line);
                
                if (line == "NLM_CHECKPOINT_V1") {
                    output << "  Checkpoint version: NLM_CHECKPOINT_V1" << std::endl;
                } else if (line.find("Created: ") == 0) {
                    output << "  Created: " << line.substr(9) << std::endl;
                } else if (line.find("Description: ") == 0) {
                    output << "  Description: " << line.substr(13) << std::endl;
                } else if (line == "Config:") {
                    inConfig = true;
                    output << "  Configuration:" << std::endl;
                } else if (line == "END_CHECKPOINT") {
                    inConfig = false;
                    output << "  Checkpoint loaded successfully" << std::endl;
                } else if (inConfig && line.find("  ") == 0) {
                    // Parse config line
                    size_t pos = line.find('=');
                    if (pos != std::string::npos) {
                        std::string key = trim(line.substr(2, pos - 2));
                        std::string value = trim(line.substr(pos + 1));
                        
                        // TODO: Load configuration values
                        output << "    " << key << " = " << value << std::endl;
                    }
                }
            }
            
        } catch (const std::exception& e) {
            return CommandResult(1, std::string("Error loading checkpoint: ") + e.what());
        }
        
        return CommandResult(0, output.str());
    }
    
private:
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
};

class CheckpointListCommand : public BaseCommand {
public:
    CheckpointListCommand() : BaseCommand("list", "List available checkpoints") {}
    
    CommandResult execute() override {
        std::stringstream output;
        output << "Available Checkpoints:" << std::endl;
        
        // Try to list files in current directory with checkpoint extensions
        std::vector<std::string> extensions = {".chk", ".checkpoint", ".nlm"};
        bool found = false;
        
        for (const auto& ext : extensions) {
            std::string pattern = "*" + ext;
            for (const auto& entry : std::filesystem::directory_iterator(".")) {
                if (entry.path().extension() == ext) {
                    found = true;
                    auto fileTime = std::filesystem::last_write_time(entry.path());
                    auto age = std::chrono::system_clock::now() - 
                             std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                                 fileTime - std::filesystem::file_time_type::clock::now() + 
                                 std::chrono::system_clock::now().time_since_epoch());
                    
                    output << "  " << entry.path().filename().string() 
                           << " (age: " << std::chrono::duration_cast<std::chrono::hours>(age).count() << " hours)" << std::endl;
                }
            }
        }
        
        if (!found) {
            output << "  No checkpoints found" << std::endl;
            output << "  To create checkpoints: nlm checkpoint save <file>" << std::endl;
        }
        
        return CommandResult(0, output.str());
    }
};

class CheckpointRemoveCommand : public BaseCommand {
public:
    CheckpointRemoveCommand() : BaseCommand("remove", "Remove checkpoint") {
        addArgument(Argument("file", "Checkpoint file path", "string", "", true));
    }
    
    void addArguments() override {
        addArgument(Argument("file", "Checkpoint file path", "string", "", true));
    }
    
    CommandResult execute() override {
        std::string filepath = getConfigValue("file");
        
        if (filepath.empty()) {
            return CommandResult(1, "Checkpoint file path cannot be empty");
        }
        
        if (!std::filesystem::exists(filepath)) {
            return CommandResult(1, "Checkpoint file not found: " + filepath);
        }
        
        try {
            std::filesystem::remove(filepath);
            std::stringstream output;
            output << "Checkpoint removed: " << filepath << std::endl;
            return CommandResult(0, output.str());
        } catch (const std::exception& e) {
            return CommandResult(1, std::string("Error removing checkpoint: ") + e.what());
        }
    }
};

// Command factory function
inline void registerCheckpointCommands(CommandProcessor& processor) {
    processor.registerCommand(new CheckpointCommand());
    processor.registerCommand(new CheckpointSaveCommand());
    processor.registerCommand(new CheckpointLoadCommand());
    processor.registerCommand(new CheckpointListCommand());
    processor.registerCommand(new CheckpointRemoveCommand());
}

} // namespace cli
} // namespace nlm
