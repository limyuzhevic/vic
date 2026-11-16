# CheckpointTool.hpp

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "Tools.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class CheckpointToolImpl;

struct CheckpointInfo {
    std::string filename;
    std::string path;
    std::chrono::system_clock::time_point createdTime;
    std::chrono::system_clock::time_point modifiedTime;
    uint64_t sizeBytes;
    bool compressed;
    std::string description;
};

class CheckpointTool {
public:
    CheckpointTool();
    ~CheckpointTool();
    
    int run(int argc, char** argv);
    
    int saveCommand(int argc, char** argv);
    int loadCommand(int argc, char** argv);
    int listCommand(int argc, char** argv);
    int infoCommand(int argc, char** argv);
    int deleteCommand(int argc, char** argv);
    int validateCommand(int argc, char** argv);
    int restoreCommand(int argc, char** argv);
    int snapshotCommand(int argc, char** argv);

private:
    ToolConfig toolConfig_;
    std::unique_ptr<CheckpointToolImpl> toolImpl_;
    
    void printHelp();
};

} // namespace nlm