#pragma once

#include <string>
#include <vector>

namespace nlm {

struct CliOptions {
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "info";
    std::string logFile;
    bool verbose = false;
    bool help = false;
    bool version = false;
    std::vector<std::string> enabledTests = {"all"};
    std::string benchmarkFile;
    int simulationSteps = 0;
    bool listTests = false;
};

struct TestResult {
    std::string name;
    double executionTime;
    bool success;
    std::map<std::string, std::string> metrics;
};

CliOptions parseCommandLine(int argc, char** argv);
void printHelp(const std::string& programName);
void printVersion();

} // namespace nlm