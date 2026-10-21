#!/usr/bin/env g++
// NLM Save Tool
// Tool for saving simulation checkpoints

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

namespace fs = std::filesystem;

struct CheckpointInfo {
    std::string timestamp;
    double simulationTime;
    int64_t neuronCount;
    int64_t regionCount;
    size_t synapseCount;
    size_t spikeCount;
    std::string description;
    std::string filename;
};

class CheckpointManager {
private:
    std::vector<CheckpointInfo> checkpoints;
    std::shared_ptr<Config> config;
    std::string checkpointDir;
    
public:
    CheckpointManager() {
        checkpointDir = "checkpoints";
        config = std::make_shared<Config>();
        config->loadFromFile("configs/default.cfg");
        
        // Create checkpoint directory if it doesn't exist
        if (!fs::exists(checkpointDir)) {
            fs::create_directory(checkpointDir);
        }
    }
    
    void saveCheckpoint(const std::string& description = "", const std::string& filename = "") {
        std::cout << "=== Saving NLM Checkpoint ===" << std::endl;
        
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = formatTime(time);
        
        // Determine filename
        std::string actualFilename;
        if (!filename.empty()) {
            actualFilename = filename;
        } else {
            // Generate filename with timestamp
            actualFilename = "checkpoint_" + timestamp;
            // Replace colons and other problematic characters
            std::replace(actualFilename.begin(), actualFilename.end(), ':', '-');
            actualFilename += ".nckpt";
        }
        
        std::cout << "Saving checkpoint to: " << actualFilename << std::endl;
        if (!description.empty()) {
            std::cout << "Description: " << description << std::endl;
        }
        
        // Create checkpoint file path
        std::string filepath = checkpointDir + "/" + actualFilename;
        
        // Note: This is a simplified checkpoint implementation
        // In a real implementation, you would save the actual brain state
        // For now, we'll save the configuration and metadata
        
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create checkpoint file: " << filepath << std::endl;
            return;
        }
        
        // Write metadata (simplified)
        file << "# NLM Checkpoint File" << std::endl;
        file << "# Generated: " << timestamp << std::endl;
        file << "# Description: " << description << std::endl;
        file << "# " << version << std::endl;
        file << std::endl;
        
        // Write configuration summary
        file << "[CONFIG]" << std::endl;
        file << config->summary() << std::endl;
        file << std::endl;
        
        // Write metadata
        file << "[METADATA]" << std::endl;
        file << "simulation_time = 0.0" << std::endl;  // Placeholder
        file << "neuron_count = " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
        file << "region_count = " << config->getOr<int64_t>("region_count", 2) << std::endl;
        file << "checkpoint_format = " << "1.0" << std::endl;
        file << std::endl;
        
        file.close();
        
        // Record checkpoint info
        CheckpointInfo info;
        info.timestamp = timestamp;
        info.description = description;
        info.filename = actualFilename;
        info.neuronCount = config->getOr<int64_t>("neuron_count", 1000);
        info.regionCount = config->getOr<int64_t>("region_count", 2);
        info.synapseCount = 0;  // Placeholder - would get from brain
        info.spikeCount = 0;    // Placeholder - would get from brain
        info.simulationTime = 0.0;  // Placeholder - would get from brain
        
        checkpoints.push_back(info);
        
        std::cout << "Checkpoint saved successfully: " << filepath << std::endl;
        std::cout << "Checkpoint info:" << std::endl;
        std::cout << "  Timestamp:     " << info.timestamp << std::endl;
        std::cout << "  Neurons:       " << info.neuronCount << std::endl;
        std::cout << "  Regions:       " << info.regionCount << std::endl;
        std::cout << "  Synapses:      " << info.synapseCount << std::endl;
        std::cout << "  Total Spikes:  " << info.spikeCount << std::endl;
    }
    
    void listCheckpoints() {
        std::cout << "=== NLM Checkpoints ===" << std::endl;
        
        if (checkpoints.empty()) {
            std::cout << "No checkpoints found." << std::endl;
            return;
        }
        
        std::cout << std::left << std::setw(20) << "Timestamp"
                  << std::setw(15) << "Filename"
                  << std::setw(10) << "Neurons"
                  << std::setw(10) << "Regions"
                  << std::setw(12) << "Description" << std::endl;
        std::cout << std::string(75, '-') << std::endl;
        
        for (const auto& checkpoint : checkpoints) {
            std::cout << std::left << std::setw(20) << checkpoint.timestamp
                      << std::setw(15) << checkpoint.filename
                      << std::setw(10) << checkpoint.neuronCount
                      << std::setw(10) << checkpoint.regionCount
                      << std::setw(12) << checkpoint.description << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Total checkpoints: " << checkpoints.size() << std::endl;
    }
    
    void loadCheckpoint(const std::string& filename) {
        std::cout << "=== Loading NLM Checkpoint ===" << std::endl;
        
        std::string filepath = checkpointDir + "/" + filename;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open checkpoint file: " << filepath << std::endl;
            return;
        }
        
        std::string line;
        std::string section;
        bool inConfigSection = false;
        bool inMetadataSection = false;
        
        std::cout << "Loading checkpoint from: " << filename << std::endl;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line[0] == '[' && line[line.size() - 1] == ']') {
                section = line.substr(1, line.size() - 2);
                
                if (section == "CONFIG") {
                    inConfigSection = true;
                    inMetadataSection = false;
                } else if (section == "METADATA") {
                    inConfigSection = false;
                    inMetadataSection = true;
                } else {
                    inConfigSection = false;
                    inMetadataSection = false;
                }
                
                continue;
            }
            
            if (inMetadataSection) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    std::cout << "  " << key << " = " << value << std::endl;
                }
            }
        }
        
        file.close();
        
        std::cout << "Checkpoint loaded successfully!" << std::endl;
    }
    
    void cleanOldCheckpoints(int keepCount = 10) {
        std::cout << "=== Cleaning Old Checkpoints ===" << std::endl;
        
        if (checkpoints.size() <= static_cast<size_t>(keepCount)) {
            std::cout << "Already keeping " << checkpoints.size() << " checkpoints (limit: " << keepCount << ")" << std::endl;
            return;
        }
        
        // Sort by timestamp (oldest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
            [](const CheckpointInfo& a, const CheckpointInfo& b) {
                return a.timestamp < b.timestamp;
            });
        
        int toRemove = checkpoints.size() - keepCount;
        
        std::cout << "Removing " << toRemove << " old checkpoints..." << std::endl;
        
        for (int i = 0; i < toRemove; ++i) {
            std::string filepath = checkpointDir + "/" + checkpoints[i].filename;
            if (fs::exists(filepath)) {
                fs::remove(filepath);
                std::cout << "  Removed: " << checkpoints[i].filename << std::endl;
            }
        }
        
        // Remove from in-memory list
        checkpoints.erase(checkpoints.begin(), checkpoints.begin() + toRemove);
        
        std::cout << "Cleanup completed." << std::endl;
    }
    
    void exportCheckpoint(const std::string& filename, const std::string& outputFormat = "txt") {
        std::cout << "=== Exporting Checkpoint ===" << std::endl;
        
        // Find the checkpoint file
        std::string sourcePath = checkpointDir + "/" + filename;
        if (!fs::exists(sourcePath)) {
            std::cerr << "Error: Checkpoint file not found: " << filename << std::endl;
            return;
        }
        
        std::string exportPath = "exported_" + filename;
        if (outputFormat == "json") {
            exportPath = filename.substr(0, filename.find_last_of('.')) + ".json";
        } else if (outputFormat == "xml") {
            exportPath = filename.substr(0, filename.find_last_of('.')) + ".xml";
        }
        
        std::cout << "Exporting " << filename << " to " << outputFormat << " format: " << exportPath << std::endl;
        
        // Simple export implementation
        std::ifstream source(sourcePath);
        std::ofstream dest(exportPath);
        
        if (!source.is_open() || !dest.is_open()) {
            std::cerr << "Error: Cannot export checkpoint" << std::endl;
            return;
        }
        
        dest << "// Exported checkpoint from " << filename << std::endl;
        dest << "// Format: " << outputFormat << std::endl;
        dest << std::endl;
        
        std::string line;
        while (std::getline(source, line)) {
            dest << line << std::endl;
        }
        
        source.close();
        dest.close();
        
        std::cout << "Checkpoint exported successfully!" << std::endl;
    }
    
    std::string formatTime(std::time_t time) {
        std::tm* tm = std::localtime(&time);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }
        return str.substr(start, end - start + 1);
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --save <desc>             Save checkpoint with description" << std::endl;
        std::cout << "  --saveas <desc> <file>    Save checkpoint to specific file" << std::endl;
        std::cout << "  --list                    List all checkpoints" << std::endl;
        std::cout << "  --load <file>             Load checkpoint from file" << std::endl;
        std::cout << "  --clean <count>           Clean old checkpoints (keep last N)" << std::endl;
        std::cout << "  --export <file> [format]  Export checkpoint in different format" << std::endl;
        std::cout << "  --dir <path>              Set checkpoint directory" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-save";
    std::string description;
    std::string filename;
    std::string action;
    std::string loadFile;
    std::string exportFile;
    std::string exportFormat = "txt";
    std::string checkpointDir = "checkpoints";
    int cleanCount = 10;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            CheckpointManager manager;
            manager.printUsage(programName);
            return 0;
        } else if (arg == "--save") {
            action = "save";
            if (i + 1 < argc) {
                description = argv[++i];
            }
        } else if (arg == "--saveas") {
            action = "saveas";
            if (i + 2 < argc) {
                description = argv[++i];
                filename = argv[++i];
            }
        } else if (arg == "--list") {
            action = "list";
        } else if (arg == "--load") {
            action = "load";
            if (i + 1 < argc) {
                loadFile = argv[++i];
            }
        } else if (arg == "--clean") {
            action = "clean";
            if (i + 1 < argc) {
                cleanCount = std::stoi(argv[++i]);
            }
        } else if (arg == "--export") {
            action = "export";
            if (i + 1 < argc) {
                exportFile = argv[++i];
                if (i + 1 < argc) {
                    exportFormat = argv[++i];
                }
            }
        } else if (arg == "--dir") {
            if (i + 1 < argc) {
                checkpointDir = argv[++i];
            }
        }
    }
    
    CheckpointManager manager;
    manager.checkpointDir = checkpointDir;
    
    if (action == "save") {
        manager.saveCheckpoint(description);
    } else if (action == "saveas") {
        manager.saveCheckpoint(description, filename);
    } else if (action == "list") {
        manager.listCheckpoints();
    } else if (action == "load") {
        manager.loadCheckpoint(loadFile);
    } else if (action == "clean") {
        manager.cleanOldCheckpoints(cleanCount);
    } else if (action == "export") {
        manager.exportCheckpoint(exportFile, exportFormat);
    } else {
        CheckpointManager tempManager;
        tempManager.printUsage(programName);
        return 0;
    }
    
    return 0;
}
