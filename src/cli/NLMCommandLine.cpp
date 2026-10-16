// NLM Command Line Interface
// Command-line tools and utilities for NLM (Neural Learning Machine)

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>

namespace nlm {

class NLMCommandLine {
public:
    static void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Command Line Interface                                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    static void printHelp() {
        std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE:" << std::endl;
        std::cout << "  nlm [options] [command] [arguments]" << std::endl;
        std::cout << std::endl;
        std::cout << "COMMANDS:" << std::endl;
        std::cout << "  run              Run a simulation" << std::endl;
        std::cout << "  create           Create a brain or configuration" << std::endl;
        std::cout << "  config           Configuration management" << std::endl;
        std::cout << "  export           Export brain state or data" << std::endl;
        std::cout << "  import           Import brain state or data" << std::endl;
        std::cout << "  stats            Show statistics" << std::endl;
        std::cout << "  test             Run tests" << std::endl;
        std::cout << "  help             Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --config FILE    Use configuration file" << std::endl;
        std::cout << "  --steps N        Number of simulation steps" << std::endl;
        std::cout << "  --neuron N       Number of neurons" << std::endl;
        std::cout << "  --region R       Number of regions" << std::endl;
        std::cout << "  --log LEVEL      Log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
        std::cout << "  --quiet          Suppress output" << std::endl;
        std::cout << "  --version        Show version" << std::endl;
        std::cout << "  --help           Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm run --steps 1000" << std::endl;
        std::cout << "  nlm create --neuron 500 --region 2" << std::endl;
        std::cout << "  nlm export --file brain.bin" << std::endl;
        std::cout << "  nlm --config my_config.cfg run" << std::endl;
    }
    
    static void printVersion() {
        std::cout << "NLM Version 0.1.0" << std::endl;
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << "Built with C++20 and pybind11" << std::endl;
        std::cout << "Git commit: " << getGitCommit() << std::endl;
    }
    
    static void printError(const std::string& message) {
        std::cerr << "ERROR: " << message << std::endl;
    }
    
    static void printWarning(const std::string& message) {
        std::cout << "WARNING: " << message << std::endl;
    }
    
    static void printInfo(const std::string& message) {
        std::cout << "INFO: " << message << std::endl;
    }
    
    static std::string getGitCommit() {
        std::string gitCommit = "unknown";
        std::string gitCmd = "git rev-parse HEAD 2>/dev/null || echo \"unknown\"";
        
        FILE* pipe = popen(gitCmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                gitCommit = buffer;
                gitCommit.erase(gitCommit.find_last_not_of("\n\r") + 1);
            }
            pclose(pipe);
        }
        
        return gitCommit;
    }
    
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static void printFormattedTime(const std::chrono::duration<double>& duration) {
        double seconds = duration.count();
        if (seconds < 60.0) {
            std::cout << seconds << " seconds";
        } else if (seconds < 3600.0) {
            double minutes = seconds / 60.0;
            std::cout << minutes << " minutes";
        } else {
            double hours = seconds / 3600.0;
            std::cout << hours << " hours";
        }
    }
    
    static std::string formatNumber(size_t num) {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }
    
    static std::string formatFloat(float value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4) << value;
        return ss.str();
    }
    
    static bool runSimulation(const std::map<std::string, std::string>& options) {
        try {
            // Get configuration
            auto config = Config::createDefaultConfig();
            
            // Override with command line options
            if (options.count("neuron")) {
                config->set("neuron_count", std::stoi(options.at("neuron")), ConfigSource::CommandLine);
            }
            if (options.count("region")) {
                config->set("region_count", std::stoi(options.at("region")), ConfigSource::CommandLine);
            }
            if (options.count("steps")) {
                config->set("max_simulation_steps", std::stoi(options.at("steps")), ConfigSource::CommandLine);
            }
            
            // Create and initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                printError("Failed to initialize brain");
                return false;
            }
            
            printInfo("Simulation started with " + formatNumber(config->getOr<size_t>("neuron_count", 1000)) + 
                     " neurons in " + formatNumber(config->getOr<size_t>("region_count", 2)) + " regions");
            printInfo("Running " + options.at("steps") + " simulation steps...");
            
            // Get start time
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run simulation
            size_t maxSteps = config->getOr<size_t>("max_simulation_steps", 1000);
            for (size_t step = 0; step < maxSteps; ++step) {
                brain->step(step);
                
                // Print progress every 10%
                if ((step + 1) % (maxSteps / 10) == 0 || step == maxSteps - 1) {
                    float progress = (float)(step + 1) / (float)maxSteps * 100.0f;
                    size_t spikes = brain->getTotalSpikeCount();
                    float rate = (float)spikes / (float)(step + 1);
                    
                    printInfo(std::string("Progress: ") + formatFloat(progress) + "% - " + 
                             formatNumber(spikes) + " spikes, " + formatFloat(rate) + " Hz");
                }
            }
            
            // Get end time and report duration
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            // Print final statistics
            printSimulationResults(brain);
            
            printInfo(std::string("Simulation completed in "));
            printFormattedTime(std::chrono::duration<double>(duration.count() / 1000.0));
            printInfo(std::string(" (" + formatNumber(duration.count()) + " ms)"));
            
            return true;
            
        } catch (const std::exception& e) {
            printError(std::string("Simulation failed: ") + e.what());
            return false;
        }
    }
    
    static bool createBrain(const std::map<std::string, std::string>& options) {
        try {
            // Get configuration
            auto config = Config::createDefaultConfig();
            
            // Override with command line options
            if (options.count("neuron")) {
                config->set("neuron_count", std::stoi(options.at("neuron")), ConfigSource::CommandLine);
            }
            if (options.count("region")) {
                config->set("region_count", std::stoi(options.at("region")), ConfigSource::CommandLine);
            }
            if (options.count("steps")) {
                config->set("max_simulation_steps", std::stoi(options.at("steps")), ConfigSource::CommandLine);
            }
            
            // Create brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                printError("Failed to initialize brain");
                return false;
            }
            
            printInfo("Brain created successfully");
            printInfo("  Neurons: " + formatNumber(brain->getTotalNeuronCount()));
            printInfo("  Synapses: " + formatNumber(brain->getTotalSynapseCount()));
            printInfo("  Regions: " + formatNumber(brain->getRegionCount()));
            printInfo("  Firing rate: " + formatFloat(brain->getAverageFiringRate()) + " Hz");
            
            // Save brain if requested
            if (options.count("output")) {
                if (!brain->save(options.at("output"))) {
                    printError("Failed to save brain to " + options.at("output"));
                    return false;
                }
                printInfo("Brain saved to " + options.at("output"));
            }
            
            return true;
            
        } catch (const std::exception& e) {
            printError(std::string("Brain creation failed: ") + e.what());
            return false;
        }
    }
    
    static bool exportBrain(const std::map<std::string, std::string>& options) {
        if (!options.count("input")) {
            printError("Missing required input file");
            return false;
        }
        
        std::string inputFile = options.at("input");
        
        try {
            // Create brain from configuration
            auto config = Config::createDefaultConfig();
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->load(inputFile)) {
                printError("Failed to load brain from " + inputFile);
                return false;
            }
            
            printInfo("Brain loaded successfully");
            
            // Export to different formats
            if (options.count("output")) {
                std::string outputFile = options.at("output");
                
                if (outputFile.substr(outputFile.find_last_of(".")) == ".json") {
                    // Export to JSON
                    std::ofstream file(outputFile);
                    file << brain->exportToJSON();
                    file.close();
                    printInfo("Brain exported to JSON: " + outputFile);
                } else {
                    // Export to binary
                    if (!brain->save(outputFile)) {
                        printError("Failed to save brain to " + outputFile);
                        return false;
                    }
                    printInfo("Brain exported to binary: " + outputFile);
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            printError(std::string("Brain export failed: ") + e.what());
            return false;
        }
    }
    
    static bool importBrain(const std::map<std::string, std::string>& options) {
        if (!options.count("input")) {
            printError("Missing required input file");
            return false;
        }
        
        std::string inputFile = options.at("input");
        
        try {
            // Create brain from configuration
            auto config = Config::createDefaultConfig();
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->load(inputFile)) {
                printError("Failed to load brain from " + inputFile);
                return false;
            }
            
            printInfo("Brain imported successfully");
            printInfo("  Neurons: " + formatNumber(brain->getTotalNeuronCount()));
            printInfo("  Synapses: " + formatNumber(brain->getTotalSynapseCount()));
            printInfo("  Regions: " + formatNumber(brain->getRegionCount()));
            
            return true;
            
        } catch (const std::exception& e) {
            printError(std::string("Brain import failed: ") + e.what());
            return false;
        }
    }
    
    static bool runConfigCommand(const std::map<std::string, std::string>& options) {
        if (!options.count("command")) {
            printError("Missing configuration command");
            return false;
        }
        
        std::string command = options.at("command");
        
        if (command == "show") {
            auto config = Config::createDefaultConfig();
            printInfo(config->summary());
            return true;
        } else if (command == "save") {
            if (!options.count("file")) {
                printError("Missing output file for config save");
                return false;
            }
            
            auto config = Config::createDefaultConfig();
            if (!config->saveToFile(options.at("file"))) {
                printError("Failed to save configuration to " + options.at("file"));
                return false;
            }
            
            printInfo("Configuration saved to " + options.at("file"));
            return true;
        } else if (command == "load") {
            if (!options.count("file")) {
                printError("Missing input file for config load");
                return false;
            }
            
            auto config = Config::createDefaultConfig();
            if (!config->loadFromFile(options.at("file"))) {
                printError("Failed to load configuration from " + options.at("file"));
                return false;
            }
            
            printInfo("Configuration loaded from " + options.at("file"));
            return true;
        } else if (command == "validate") {
            auto config = Config::createDefaultConfig();
            if (!config->validate()) {
                printError("Configuration validation failed");
                return false;
            }
            
            printInfo("Configuration validation passed");
            return true;
        } else {
            printError("Unknown configuration command: " + command);
            return false;
        }
    }
    
    static void printSimulationResults(std::shared_ptr<Brain> brain) {
        printInfo("=== Simulation Results ===");
        printInfo("  Total neurons: " + formatNumber(brain->getTotalNeuronCount()));
        printInfo("  Total synapses: " + formatNumber(brain->getTotalSynapseCount()));
        printInfo("  Total spikes: " + formatNumber(brain->getTotalSpikeCount()));
        printInfo("  Firing neurons (current): " + formatNumber(brain->getFiringNeuronCount()));
        printInfo("  Average firing rate: " + formatFloat(brain->getAverageFiringRate()) + " Hz");
        printInfo("  Excitation/Inhibition ratio: " + formatFloat(brain->getExcitationInhibitionRatio()));
        printInfo("  Pending spike events: " + formatNumber(brain->getPendingSpikeEventCount()));
        
        auto* stdp = brain->getSTDP();
        if (stdp) {
            printInfo("  STDP LTP weight: " + formatFloat(stdp->getLTPWeight()));
            printInfo("  STDP LTD weight: " + formatFloat(stdp->getLTDWeight()));
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            printInfo("  Curiosity level: " + formatFloat(curiosity->getLevel()));
        }
        
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            printInfo("  Dopamine level: " + formatFloat(dopamine->getLevel()));
        }
        
        printInfo("======================");
    }
    
    static bool runTests() {
        printInfo("Running NLM tests...");
        printWarning("Tests are not fully implemented in this build");
        printInfo("For comprehensive testing, please refer to the test directory");
        
        // TODO: Implement proper test suite
        return true;
    }
    
    static std::map<std::string, std::string> parseOptions(int argc, char** argv) {
        std::map<std::string, std::string> options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value format
            size_t pos = arg.find("=");
            if (pos != std::string::npos && arg[0] == '-') {
                std::string key = arg.substr(1, pos - 1);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
            }
            // Handle --key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string next = argv[i + 1];
                if (next[0] != '-') {
                    options[arg.substr(1)] = next;
                    i++;
                } else {
                    options[arg.substr(1)] = "true";
                }
            }
            // Handle command (first non-option argument)
            else if (arg[0] != '-' && options.empty()) {
                options["command"] = arg;
            }
        }
        
        return options;
    }
    
    static bool runCommand(const std::string& command, const std::map<std::string, std::string>& options) {
        if (command == "run") {
            return runSimulation(options);
        } else if (command == "create") {
            return createBrain(options);
        } else if (command == "export") {
            return exportBrain(options);
        } else if (command == "import") {
            return importBrain(options);
        } else if (command == "config") {
            return runConfigCommand(options);
        } else if (command == "stats") {
            printInfo("Statistics command not yet implemented");
            return true;
        } else if (command == "test") {
            return runTests();
        } else {
            printError("Unknown command: " + command);
            return false;
        }
    }
};