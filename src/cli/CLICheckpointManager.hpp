#pragma once

#include <string>
#include <vector>
#include <ctime>

namespace nlm {

struct CheckpointInfo {
    std::string name;
    uint64_t step;
    double simulationTime;
    uint64_t neuronCount;
    uint64_t synapseCount;
    uint64_t fileSize;
    time_t timestamp;
};

class CLICheckpointManager {
public:
    CLICheckpointManager();
    ~CLICheckpointManager();

    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);

    // Handle specific commands
    bool handleSaveCommand(int argc, char** argv);
    bool handleLoadCommand(int argc, char** argv);
    bool handleListCommand();
    bool handleDeleteCommand(int argc, char** argv);
    bool handleConfigureCommand(int argc, char** argv);

    // Print help messages
    void printHelp();
    void printSaveHelp();
    void printLoadHelp();
    void printListHelp();
    void printDeleteHelp();
    void printConfigureHelp();

    // Checkpoint operations
    bool saveCheckpoint(const std::string& name, int step, bool compress,
                        const std::string& configFile);
    bool loadCheckpoint(const std::string& name);
    bool deleteCheckpoint(const std::string& name);
    std::vector<CheckpointInfo> listCheckpoints() const;

private:
    std::string checkpointDir_;
    size_t maxCheckpoints_;
    bool compression_;
};

} // namespace nlm