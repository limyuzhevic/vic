# ProfilerTool.hpp

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "Tools.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class ProfilerToolImpl;

struct ProfilerMetric {
    std::string name;
    std::string category;
    std::chrono::duration<double> value;
    std::chrono::duration<double> min;
    std::chrono::duration<double> max;
    std::chrono::duration<double> avg;
    double count;
};

struct ProfileResult {
    std::string scenarioName;
    std::string configFile;
    std::chrono::duration<double> totalWallTime;
    std::chrono::duration<double> totalCPUTime;
    uint64_t memoryUsage;
    std::vector<ProfilerMetric> metrics;
    std::map<std::string, std::string> metadata;
};

class ProfilerTool {
public:
    ProfilerTool();
    ~ProfilerTool();
    
    int run(int argc, char** argv);
    
    int profileCommand(int argc, char** argv);
    int compareCommand(int argc, char** argv);
    int optimizationCommand(int argc, char** argv);
    int bottleneckCommand(int argc, char** argv);

private:
    ToolConfig toolConfig_;
    std::unique_ptr<ProfilerToolImpl> toolImpl_;
    
    void printHelp();
};

} // namespace nlm