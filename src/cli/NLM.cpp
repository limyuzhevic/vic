#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>

namespace nlm {

class CLI {
public:
    CLI() {
        printBanner();
        initializeLogger();
        printHelp();
    }
    
    void run(int argc, char** argv) {
        if (argc < 2) {
            return;
        }
        
        std::string command = argv[1];
        
        if (command == "--help" || command == "-h") {
            printHelp();
        } else if (command == "run") {
            runCommand(argc, argv);
        } else if (command == "config") {
            configCommand(argc, argv);
        } else if (command == "experiment") {
            experimentCommand(argc, argv);
        } else if (command == "list") {
            listAvailableExperiments();
        } else if (command == "version") {
            printVersion();
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printHelp();
        }
    }
    
private:
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void printHelp() {
        std::cout << "\n=== NLM Command Line Interface ===\n" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  nlm run [config_file] [--neuron_count N] [--steps N] [--seed N]" << std::endl;
        std::cout << "  nlm config [save|load] <file> [--key value]" << std::endl;
        std::cout << "  nlm experiment <demo|phase6> [--config file]" << std::endl;
        std::cout << "  nlm list                          List available experiments" << std::endl;
        std::cout << "  nlm version                        Show version information" << std::endl;
        std::cout << "  nlm help                         Show this help message" << std::endl;
        std::cout << "\nExamples:" << std::endl;
        std::cout << "  nlm run --neuron_count 500 --steps 2000" << std::endl;
        std::cout << "  nlm config save my_config.cfg --neuron_count 1000" << std::endl;
        std::cout << "  nlm experiment phase6 --config my_config.cfg" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM - Neural Learning Machine v0.1.0 (Phase 2)" << std::endl;
        std::cout << "Built with real LIF neuron dynamics and integrated brain systems." << std::endl;
    }
    
    void listAvailableExperiments() {
        std::cout << "\n=== Available Experiments ===\n" << std::endl;
        std::cout << "1. Phase 6 Integration Demo" << std::endl;
        std::cout << "   - Tests integration of all brain systems" << std::endl;
        std::cout << "   - Runs memory, neuromodulation, prediction, development" << std::endl;
        std::cout << "   - Includes checkpoint save/load and replay" << std::endl;
        std::cout << "\n2. Basic Connectivity Test" << std::endl;
        std::cout << "   - Verifies neural propagation" << std::endl;
        std::cout << "   - Tests spike generation and transmission" << std::endl;
        std::cout << "\n3. Plasticity Experiment" << std::endl;
        std::cout << "   - Demonstrates synaptic learning" << std::endl;
        std::cout << "   - Tests STDP and Hebbian plasticity" << std::endl;
        std::cout << "\n4. STDP Verification" << std::endl;
        std::cout << "   - Tests spike-timing dependent plasticity" << std::endl;
        std::cout << "   - Verifies LTP/LTD mechanisms" << std::endl;
    }
    
    void runCommand(int argc, char** argv) {
        std::string configFile = "configs/default.cfg";
        uint64_t neuronCount = 500;
        size_t steps = 2000;
        uint64_t seed = 42;
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg.substr(0, 7) == "--config" && arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (arg.substr(0, 16) == "--neuron_count" && i + 1 < argc) {
                neuronCount = std::stoull(argv[++i]);
            } else if (arg.substr(0, 8) == "--steps" && i + 1 < argc) {
                steps = std::stoull(argv[++i]);
            } else if (arg.substr(0, 7) == "--seed" && i + 1 < argc) {
                seed = std::stoull(argv[++i]);
            }
        }
        
        std::cout << "\n=== Running NLM Simulation ===" << std::endl;
        std::cout << "Config: " << configFile << std::endl;
        std::cout << "Neurons: " << neuronCount << std::endl;
        std::cout << "Steps: " << steps << std::endl;
        std::cout << "Seed: " << seed << std::endl;
        
        // Create and run experiment
        auto experiment = std::make_shared<Phase6IntegratedExperiment>();
        Phase6Config config;
        config.neuronCount = neuronCount;
        config.maxSteps = steps;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment->run(config);
        
        std::cout << "\n=== Simulation Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        std::cout << "\n=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    }
    
    void configCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: config command requires action (save|load)" << std::endl;
            return;
        }
        
        std::string action = argv[2];
        if (action != "save" && action != "load") {
            std::cerr << "Error: action must be 'save' or 'load'" << std::endl;
            return;
        }
        
        std::string configFile = "configs/default.cfg";
        
        for (int i = 3; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config" && arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            }
        }
        
        if (action == "save") {
            std::cout << "\nSaving configuration to: " << configFile << std::endl;
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 1000);
            config->set("region_count", 1);
            config->set("connection_probability", 0.1f);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            
            if (config->saveToFile(configFile)) {
                std::cout << "✓ Configuration saved successfully" << std::endl;
            } else {
                std::cerr << "✗ Failed to save configuration" << std::endl;
            }
        } else {
            std::cout << "\nLoading configuration from: " << configFile << std::endl;
            
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(configFile)) {
                std::cout << "✓ Configuration loaded successfully" << std::endl;
                std::cout << "Keys in config: ";
                auto keys = config->getKeys();
                for (size_t i = 0; i < keys.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << keys[i];
                }
                std::cout << std::endl;
            } else {
                std::cerr << "✗ Failed to load configuration" << std::endl;
            }
        }
    }
    
    void experimentCommand(int argc, char** argv) {
        std::string experimentType = "phase6";
        std::string configFile = "configs/default.cfg";
        
        for (int i = 3; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 8) == "--config" && arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            }
        }
        
        std::cout << "\n=== Running " << experimentType << " experiment ===" << std::endl;
        
        if (experimentType == "phase6") {
            auto experiment = std::make_shared<Phase6IntegratedExperiment>();
            Phase6Config config;
            
            auto cfg = std::make_shared<Config>();
            if (cfg->loadFromFile(configFile)) {
                std::cout << "Loaded config from: " << configFile << std::endl;
            }
            
            auto result = experiment->run(config);
            
            std::cout << "\n=== Experiment Results ===" << std::endl;
            std::cout << "Success rate: " << (result.totalReward > 0 ? "High" : "Low") << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    CLI cli;
    cli.run(argc, argv);
    return 0;
}
