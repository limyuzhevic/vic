// Command line argument parsing utilities
#ifndef NLM_COMMAND_LINE_UTILS_H
#define NLM_COMMAND_LINE_UTILS_H

#include <string>
#include <vector>
#include <map>

namespace nlm {

struct CommandLineOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string logFile;
    std::string experimentType = "basic";
    std::vector<std::string> experimentArgs;
    bool verbose = false;
    bool debug = false;
    bool benchmark = false;
    bool saveConfig = false;
    bool enableLearning = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enableVisualization = false;
    int64_t maxSteps = 1000;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    float learningRate = 0.001f;
    float curiosity = 1.0f;
    std::map<std::string, std::string> customSettings;
};

class CommandLineParser {
public:
    static CommandLineOptions parse(int argc, char** argv);
    static bool validateOptions(const CommandLineOptions& options);
    static void printUsage(const char* programName);
    static void printHelp(const char* programName);
    static bool saveOptionsToFile(const CommandLineOptions& options, const std::string& filename);
};

} // namespace nlm

#endif // NLM_COMMAND_LINE_UTILS_H
