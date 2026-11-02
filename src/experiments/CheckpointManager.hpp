#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

class CheckpointManager {
private:
    struct CheckpointInfo {
        std::string name;
        std::chrono::system_clock::time_point timestamp;
        std::string description;
        uint64_t neuralSteps;
        uint64_t simulationSteps;
        float finalReward;
        std::string status;
        std::string filePath;
        
        CheckpointInfo(const std::string& n, const std::string& desc = "",
                      uint64_t ns = 0, uint64_t ss = 0, float reward = 0.0f,
                      const std::string& stat = "completed",
                      const std::string& path = "");
    };
    
    std::vector<CheckpointInfo> checkpoints;
    std::shared_ptr<Config> config;
    std::string checkpointDirectory;
    bool verbose;
    bool autoCreate;
    
public:
    CheckpointManager();
    ~CheckpointManager();
    
    void run(int argc, char* argv[]);
    void printBanner();
    
private:
    bool ensureCheckpointDirectory();
    std::string generateCheckpointFilename(const std::string& baseName);
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    bool createCheckpoint(const std::string& name, const std::string& description,
                         std::shared_ptr<Brain> brain, float finalReward);
    bool restoreCheckpoint(const std::string& name);
    bool deleteCheckpoint(const std::string& name);
    void listCheckpoints();
    void showCheckpointDetails(const std::string& name);
    void cleanupOldCheckpoints();
    void setupCheckpointConfig();
    void loadCheckpointConfig();
    void runCheckpointManager(int argc, char* argv[]);
    void processCommand(int argc, char* argv[]);
    void showHelp();
    void showStatus();
};

} // namespace nlm
