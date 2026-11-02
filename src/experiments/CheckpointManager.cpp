#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>

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
                      const std::string& path = "")
            : name(n), description(desc), neuralSteps(ns), simulationSteps(ss),
              finalReward(reward), status(stat), filePath(path) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<CheckpointInfo> checkpoints;
    std::shared_ptr<Config> config;
    std::string checkpointDirectory;
    bool verbose;
    bool autoCreate;
    
    // File system operations
    bool ensureCheckpointDirectory();
    std::string generateCheckpointFilename(const std::string& baseName);
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    
    // Checkpoint operations
    bool createCheckpoint(const std::string& name, const std::string& description,
                         std::shared_ptr<Brain> brain, float finalReward);
    bool restoreCheckpoint(const std::string& name);
    bool deleteCheckpoint(const std::string& name);
    
    // Listing and querying
    void listCheckpoints();
    void showCheckpointDetails(const std::string& name);
    void cleanupOldCheckpoints();
    
    // Configuration
    void setupCheckpointConfig();
    void loadCheckpointConfig();
    
    // Main command processing
    void runCheckpointManager(int argc, char* argv[]);
    void processCommand(int argc, char* argv[]);
    void showHelp();
    void showStatus();
    
public:
    CheckpointManager();
    ~CheckpointManager();
    
    void run(int argc, char* argv[]);
    void printBanner();
};

} // namespace nlm
