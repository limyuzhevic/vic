#include "CLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace nlm;

// CLI Arguments Structure
struct CLIArgs {
    std::string mode = "auto";
    std::string configFile = "configs/default.cfg";
    std::string experimentName = "";
    std::string outputFile = "";
    bool verbose = false;
    bool help = false;
    bool version = false;
    int trialCount = -1;
    double timestep = -1.0;
    int neuronCount = -1;
    int regionCount = -1;
    float connectionProb = -1.0f;
    bool enableCheckpoint = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
    std::string checkpointPath = "/tmp/nlm_checkpoint.bin";
    std::string logLevel = "info";
    
    bool parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                help = true;
                continue;
            }
            
            if (arg == "--version") {
                version = true;
                continue;
            }
            
            if (arg.find("--mode=") == 0) {
                mode = arg.substr(7);
            } else if (arg.find("--config=") == 0) {
                configFile = arg.substr(9);
            } else if (arg.find("--experiment=") == 0) {
                experimentName = arg.substr(13);
            } else if (arg.find("--output=") == 0) {
                outputFile = arg.substr(9);
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg.find("--trials=") == 0) {
                trialCount = std::stoi(arg.substr(8));
            } else if (arg.find("--timestep=") == 0) {
                timestep = std::stod(arg.substr(11));
            } else if (arg.find("--neurons=") == 0) {
                neuronCount = std::stoi(arg.substr(10));
            } else if (arg.find("--regions=") == 0) {
                regionCount = std::stoi(arg.substr(10));
            } else if (arg.find("--connection-prob=") == 0) {
                connectionProb = std::stof(arg.substr(19));
            } else if (arg == "--checkpoint") {
                enableCheckpoint = true;
            } else if (arg == "--no-replay") {
                enableReplay = false;
            } else if (arg == "--development") {
                enableDevelopment = true;
            } else if (arg.find("--checkpoint-path=") == 0) {
                checkpointPath = arg.substr(18);
            } else if (arg.find("--log-level=") == 0) {
                logLevel = arg.substr(12);
            }
        }
        return true;
    }
    
    void printHelp() {
        std::cout << "NLM - Neural Learning Machine\n";
        std::cout << "Usage: nlm [options]\n\n";
        std::cout << "Modes:\n";
        std::cout << "  --mode=PHASE2    Phase 2 neural computation demo (default)\n";
        std::cout << "  --mode=PHASE4    Phase 4 cognitive experiments\n";
        std::cout << "  --mode=PHASE6    Phase 6 integration experiment (default)\n";
        std::cout << "  --mode=AUTO      Auto-select best experiment based on system\n\n";
        std::cout << "Configuration:\n";
        std::cout << "  --config=file.cfg    Load configuration from file\n";
        std::cout << "  --neurons=N          Number of neurons (default: 500)\n";
        std::cout << "  --regions=N          Number of regions (default: 1)\n";
        std::cout << "  --connection-prob=X   Connection probability (default: 0.15)\n";
        std::cout << "  --timestep=DT        Simulation timestep (default: 0.001)\n\n";
        std::cout << "Experiment control:\n";
        std::cout << "  --experiment=name     Specific experiment to run\n";
        std::cout << "  --trials=N            Number of trials for repeatable experiments\n";
        std::cout << "  --checkpoint          Enable checkpointing for long experiments\n";
        std::cout << "  --no-replay           Disable replay features\n";
        std::cout << "  --development         Enable developmental plasticity\n\n";
        std::cout << "Output and debugging:\n";
        std::cout << "  --output=file         Save results to file\n";
        std::cout << "  --verbose, -v        Verbose output\n";
        std::cout << "  --checkpoint-path=path Path for checkpoint files\n";
        std::cout << "  --log-level=level     Logging level (debug, info, warning, error)\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm --mode=PHASE6 --neurons=1000 --trials=20\n";
        std::cout << "  nlm --mode=PHASE4 --output=phase4_results.json\n";
        std::cout << "  nlm --mode=PHASE2 --checkpoint\n";
        std::cout << "  nlm --help\n";
    }
};

void printVersion() {
    std::cout << "NLM - Neural Learning Machine v2.0\n";
    std::cout << "Phase 6 Integration Ready\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Architecture: Spiking neural computation with integrated memory, prediction, and cognition\n\n";
}

void printHelp(const CLIArgs& args) {
    args.printHelp();
}