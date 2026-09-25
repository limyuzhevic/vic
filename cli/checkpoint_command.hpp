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
