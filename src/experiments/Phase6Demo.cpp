#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

using namespace nlm;

// Command-line argument parsing
struct DemoArgs {
    std::string configPath = "configs/default.cfg";
    uint64_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_demo.bin";
    bool verbose = false;
    std::string visualizationType = "basic";
    std::string visualizationFormat = "json";
    bool enableVisualization = false;
    std::string visualizationDir = "visualization_output";
};

DemoArgs parseArgs(int argc, char* argv[]) {
    DemoArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Help:" << std::endl;
            std::cout << "  --config=<path>        Configuration file path (default: configs/default.cfg)" << std::endl;
            std::cout << "  --neurons=<count>      Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps=<count>        Maximum simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint        Disable checkpointing" << std::endl;
            std::cout << "  --no-replay            Disable replay" << std::endl;
            std::cout << "  --no-development       Disable development" << std::endl;
            std::cout << "  --checkpoint=<path>    Checkpoint file path" << std::endl;
            std::cout << "  --verbose              Enable verbose logging" << std::endl;
            std::cout << "  --visualization        Enable visualization" << std::endl;
            std::cout << "  --visualization-type=<type>  Visualization type (basic, neural_activity, memory_integration, comprehensive)" << std::endl;
            std::cout << "  --visualization-format=<format>  Visualization output format (json, csv)" << std::endl;
            std::cout << "  -h, --help             Show this help" << std::endl;
            exit(0);
        }
        
        // Handle --key=value format
        if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t pos = arg.find("=");
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            if (key == "config") args.configPath = value;
            else if (key == "neurons") args.neuronCount = std::stoull(value);
            else if (key == "steps") args.maxSteps = std::stoull(value);
            else if (key == "checkpoint") args.checkpointPath = value;
            else if (key == "visualization-type") args.visualizationType = value;
            else if (key == "visualization-format") args.visualizationFormat = value;
        }
        else if (arg.find("--") == 0) {
            std::string key = arg.substr(2);
            if (key == "no-checkpoint") args.enableCheckpointing = false;
            else if (key == "no-replay") args.enableReplay = false;
            else if (key == "no-development") args.enableDevelopment = false;
            else if (key == "verbose") args.verbose = true;
            else if (key == "visualization") args.enableVisualization = true;
        }
    }
    
    return args;
}

void printConfiguration(const DemoArgs& args) {
    std::cout << "=== Phase 6 Demo Configuration ===" << std::endl;
    std::cout << "Config file: " << args.configPath << std::endl;
    std::cout << "Neurons: " << args.neuronCount << std::endl;
    std::cout << "Max steps: " << args.maxSteps << std::endl;
    std::cout << "Checkpointing: " << (args.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "Replay: " << (args.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "Development: " << (args.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "Checkpoint path: " << args.checkpointPath << std::endl;
    std::cout << "Verbose: " << (args.verbose ? "Yes" : "No") << std::endl;
    std::cout << "Visualization: " << (args.enableVisualization ? "Yes" : "No") << std::endl;
    if (args.enableVisualization) {
        std::cout << "Visualization type: " << args.visualizationType << std::endl;
        std::cout << "Visualization format: " << args.visualizationFormat << std::endl;
        std::cout << "Visualization dir: " << args.visualizationDir << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    DemoArgs args = parseArgs(argc, argv);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging with verbosity control
    Logger::getInstance().setLevel(args.verbose ? Logger::Level::Debug : Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with user-specified settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = args.neuronCount;
    config.maxSteps = args.maxSteps;
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    config.checkpointPath = args.checkpointPath;
    
    // Display configuration
    printConfiguration(args);
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
