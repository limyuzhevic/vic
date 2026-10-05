#pragma once

#include <string>
#include <map>
#include <chrono>

namespace nlm {

class CLI {
public:
    enum class Mode {
        DEFAULT,
        CONNECTIVITY,
        PLASTICITY,
        STDP,
        ALL,
        CONFIG,
        HELP,
        VERSION
    };
    
    struct Options {
        Mode mode = Mode::DEFAULT;
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool quiet = false;
        bool timing = false;
        bool saveResults = false;
        std::string outputDir = ".";
        size_t duration = 10000; // Default duration for some modes
        double timestep = 0.001;
        int64_t neuronCount = 500;
        int64_t regionCount = 1;
        float connectionProbability = 0.15f;
        bool enableAdvancedFeatures = false;
        std::string customExperiment;
        std::map<std::string, std::string> advancedParams;
    };
    
    static Options parseArgs(int argc, char** argv);
    static void printHelp();
    static void printVersion();
    static Mode detectMode(const std::string& arg);
};

} // namespace nlm