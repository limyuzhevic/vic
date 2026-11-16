# BatchSimTool.hpp

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Tools.hpp"
#include "ToolsCommon.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class BatchSimToolImpl;

struct BatchScenario {
    std::string name;
    std::string configFile;
    std::string outputDir;
    std::vector<std::string> extraArgs;
    bool dryRun;
    bool verbose;
    size_t parallelThreads;
};

class BatchSimTool {
public:
    BatchSimTool();
    ~BatchSimTool();
    
    int run(int argc, char** argv);
    
    int loadCommand(int argc, char** argv);
    int runCommand(int argc, char** argv);
    int statusCommand(int argc, char** argv);
    int resumeCommand(int argc, char** argv);
    int cancelCommand(int argc, char** argv);

private:
    ToolConfig toolConfig_;
    std::unique_ptr<BatchSimToolImpl> toolImpl_;
    
    void printHelp();
};

} // namespace nlm