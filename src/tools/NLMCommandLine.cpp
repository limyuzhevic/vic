// NLM Command Line Interface
// Provides enhanced command-line capabilities for NLM users

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../experiments/Phase3Experiment.hpp"
#include "../experiments/Phase4Experiment.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

using namespace nlm;

class NLMCommandLine {
public:
    NLMCommandLine() {}
    ~NLMCommandLine() {}
    
    // Parse and execute command line arguments
    bool execute(int argc, char* argv[]) {
        if (argc <= 1) {
            printHelp();
            return false;
        }
        
        std::string command = argv[1];
        
        // Map commands to functions
        std::map<std::string, std::function<bool(int, char*[])>> commands = {
            {"help", &NLMCommandLine::handleHelp},
            {"run", &NLMCommandLine::handleRun},
            {"config", &NLMCommandLine::handleConfig},
            {"demo", &NLMCommandLine::handleDemo},
            {"test", &NLMCommandLine::handleTest},
            {"benchmark", &NLMCommandLine::handleBenchmark},
            {"explore", &NLMCommandLine::handleExplore},
            {"save", &NLMCommandLine::handleSave},
            {"load", &NLMCommandLine::handleLoad},
            {"analyze", &NLMCommandLine::handleAnalyze},
            {"compare", &NLMCommandLine::handleCompare}
        };
        
        auto it = commands.find(command);
        if (it != commands.end()) {
            return (it->second)(argc, argv);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cerr << "Use 'nlm help' for available commands" << std::endl;
            return false;
        }
    }
    
private:
    // Handle help command
    bool handleHelp(int argc, char* argv[]) {
        printHelp();
        return true;
    }
    
    // Handle run command
    bool handleRun(int argc, char* argv[]) {
        std::cout << "=== NLM Command Line Run ===" << std::endl;
        
        // Parse arguments
        int numSteps = 100;
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "run_output.txt";
        bool enableLearning = true;
        bool enableVisual = false;
        
        // Simple argument parsing
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--steps" && i + 1 < argc) {
                numSteps = std::stoi(argv[++i]);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg == "--no-learning") {
                enableLearning = false;
            } else if (arg == "--visual") {
                enableVisual = true;
            }
        }
        
        std::cout << "Configuration: " << configFile << std::endl;
        std::cout << "Steps: " << numSteps << std::endl;
        std::cout << "Learning: " << (enableLearning ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Output: " << outputFile << std::endl;
        
        // Create and run simulation
        runSimulation(numSteps, configFile, enableLearning, enableVisual, outputFile);
        
        return true;
    }
    
    // Handle config command
    bool handleConfig(int argc, char* argv[]) {
        std::cout << "=== NLM Configuration Management ===" << std::endl;
        
        if (argc < 3) {
            std::cout << "Available config commands:" << std::endl;
            std::cout << "  show    - Show current configuration" << std::endl;
            std::cout << "  set     - Set a configuration value" << std::endl;
            std::cout << "  load    - Load configuration from file" << std::endl;
            std::cout << "  save    - Save configuration to file" << std::endl;
            std::cout << "  list    - List all configuration keys" << std::endl;
            return true;
        }
        
        std::string subcommand = argv[2];
        
        if (subcommand == "show") {
            handleConfigShow();
        } else if (subcommand == "set") {
            handleConfigSet(argc, argv);
        } else if (subcommand == "load") {
            handleConfigLoad(argc, argv);
        } else if (subcommand == "save") {
            handleConfigSave(argc, argv);
        } else if (subcommand == "list") {
            handleConfigList();
        } else {
            std::cerr << "Unknown config command: " << subcommand << std::endl;
        }
        
        return true;
    }
    
    void handleConfigShow() {
        std::cout << "Current NLM Configuration:" << std::endl;
        auto config = std::make_shared<Config>();
        
        // Show some key configuration values
        std::vector<std::string> keys = {
            "neuron_count", "region_count", "connection_probability",
            "plasticity_learning_rate", "stdp_ltp_weight", "stdp_ltd_weight",
            "plasticity_stdp_enable", "plasticity_hebbian_enable",
            "random_seed", "simulation_timestep"
        };
        
        for (const auto& key : keys) {
            if (config->has(key)) {
                std::cout << "  " << key << ": " << config->getOr<std::string>(key, "N/A") << std::endl;
            }
        }
    }
    
    void handleConfigSet(int argc, char* argv[]) {
        if (argc < 5) {
            std::cerr << "Usage: nlm config set <key> <value>" << std::endl;
            return;
        }
        
        std::string key = argv[3];
        std::string value = argv[4];
        
        std::cout << "Setting \"" << key << "\" to \"" << value << "\"" << std::endl;
        
        // Parse numeric values
        try {
            if (value.find('.') != std::string::npos) {
                float floatValue = std::stof(value);
                auto config = std::make_shared<Config>();
                config->set(key, floatValue);
                std::cout << "Set as float: " << floatValue << std::endl;
            } else {
                int intValue = std::stoi(value);
                auto config = std::make_shared<Config>();
                config->set(key, intValue);
                std::cout << "Set as integer: " << intValue << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing value: " << e.what() << std::endl;
        }
    }
    
    void handleConfigLoad(int argc, char* argv[]) {
        if (argc < 4) {
            std::cerr << "Usage: nlm config load <filename>" << std::endl;
            return;
        }
        
        std::string filename = argv[3];
        std::cout << "Loading configuration from: " << filename << std::endl;
        
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filename)) {
            std::cout << "Configuration loaded successfully" << std::endl;
        } else {
            std::cerr << "Failed to load configuration" << std::endl;
        }
    }
    
    void handleConfigSave(int argc, char* argv[]) {
        std::string filename = "config.json";
        if (argc >= 4) {
            filename = argv[3];
        }
        
        std::cout << "Saving configuration to: " << filename << std::endl;
        
        auto config = std::make_shared<Config>();
        if (config->saveToFile(filename)) {
            std::cout << "Configuration saved successfully" << std::endl;
        } else {
            std::cerr << "Failed to save configuration" << std::endl;
        }
    }
    
    void handleConfigList() {
        auto config = std::make_shared<Config>();
        auto keys = config->getKeys();
        
        std::cout << "Configuration keys (" << keys.size() << "):" << std::endl;
        for (const auto& key : keys) {
            std::cout << "  " << key << std::endl;
        }
    }
    
    // Handle demo command
    bool handleDemo(int argc, char* argv[]) {
        std::cout << "=== NLM Demo Mode ===" << std::endl;
        
        std::string demoType = "phase3";
        if (argc >= 3) {
            demoType = argv[2];
        }
        
        if (demoType == "phase3") {
            std::cout << "Running Phase 3 demo (World Interaction)" << std::endl;
            runPhase3Demo();
        } else if (demoType == "phase4") {
            std::cout << "Running Phase 4 demo (Emerging Cognition)" << std::endl;
            runPhase4Demo();
        } else if (demoType == "phase6") {
            std::cout << "Running Phase 6 demo (Full Integration)" << std::endl;
            runPhase6Demo();
        } else {
            std::cerr << "Unknown demo type: " << demoType << std::endl;
            std::cerr << "Available: phase3, phase4, phase6" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Handle test command
    bool handleTest(int argc, char* argv[]) {
        std::cout << "=== NLM Testing Mode ===" << std::endl;
        
        std::string testType = "unit";
        if (argc >= 3) {
            testType = argv[2];
        }
        
        if (testType == "unit") {
            std::cout << "Running unit tests..." << std::endl;
            runUnitTests();
        } else if (testType == "integration") {
            std::cout << "Running integration tests..." << std::endl;
            runIntegrationTests();
        } else if (testType == "performance") {
            std::cout << "Running performance tests..." << std::endl;
            runPerformanceTests();
        } else {
            std::cerr << "Unknown test type: " << testType << std::endl;
            std::cerr << "Available: unit, integration, performance" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Handle benchmark command
    bool handleBenchmark(int argc, char* argv[]) {
        std::cout << "=== NLM Benchmarking ===" << std::endl;
        
        size_t numNeurons = 500;
        size_t numSteps = 100;
        
        if (argc >= 3) {
            numNeurons = std::stoul(argv[2]);
        }
        if (argc >= 4) {
            numSteps = std::stoul(argv[3]);
        }
        
        std::cout << "Benchmarking with " << numNeurons << " neurons for " << numSteps << " steps" << std::endl;
        runBenchmark(numNeurons, numSteps);
        
        return true;
    }
    
    // Handle explore command
    bool handleExplore(int argc, char* argv[]) {
        std::cout << "=== NLM Exploration Mode ===" << std::endl;
        
        printSystemInfo();
        exploreConfigurationOptions();
        exploreLearningCapabilities();
        
        return true;
    }
    
    // Handle save command
    bool handleSave(int argc, char* argv[]) {
        std::string filename = "nlm_state.bin";
        if (argc >= 3) {
            filename = argv[2];
        }
        
        std::cout << "Saving brain state to: " << filename << std::endl;
        saveBrainState(filename);
        
        return true;
    }
    
    // Handle load command
    bool handleLoad(int argc, char* argv[]) {
        std::string filename = "nlm_state.bin";
        if (argc >= 3) {
            filename = argv[2];
        }
        
        std::cout << "Loading brain state from: " << filename << std::endl;
        loadBrainState(filename);
        
        return true;
    }
    
    // Handle analyze command
    bool handleAnalyze(int argc, char* argv[]) {
        std::cout << "=== NLM Analysis Mode ===" << std::endl;
        
        analyzeBrainState();
        
        return true;
    }
    
    // Handle compare command
    bool handleCompare(int argc, char* argv[]) {
        std::cout << "=== NLM Comparison Mode ===" << std::endl;
        
        compareConfigurations(argc, argv);
        
        return true;
    }
    
    // Helper functions
    void printHelp() {
        std::cout << "====================================\n";
        std::cout << "NLM (Neural Learning Machine) Command Line\n";
        std::cout << "====================================\n\n";
        
        std::cout << "Usage:\n";
        std::cout << "  nlm <command> [options]\n\n";
        
        std::cout << "Commands:\n";
        std::cout << "  help          - Show this help message\n";
        std::cout << "  run           - Run a simulation\n";
        std::cout << "  config        - Configuration management\n";
        std::cout << "  demo          - Run demo scenarios\n";
        std::cout << "  test          - Run tests\n";
        std::cout << "  benchmark     - Performance benchmarking\n";
        std::cout << "  explore       - Explore system capabilities\n";
        std::cout << "  save [file]   - Save brain state\n";
        std::cout << "  load [file]   - Load brain state\n";
        std::cout << "  analyze       - Analyze brain state\n";
        std::cout << "  compare       - Compare configurations\n\n";
        
        std::cout << "Examples:\n";
        std::cout << "  nlm run --steps 1000 --config my_config.cfg\n";
        std::cout << "  nlm demo phase6 --steps 2000\n";
        std::cout << "  nlm benchmark 1000 500\n";
        std::cout << "  nlm config show\n";
        std::cout << "  nlm explore\n\n";
        
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm\n";
    }
    
    void runSimulation(int numSteps, const std::string& configFile, bool enableLearning, bool enableVisual, const std::string& outputFile) {
        std::cout << "Running simulation with " << numSteps << " steps" << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return;
        }
        
        // Run simulation
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            // Print progress every 10%
            if (step % (numSteps / 10) == 0 || step == numSteps - 1) {
                std::cout << "Progress: " << (step * 100) / numSteps << "% ";
                std::cout << "Spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        
        // Save output
        std::ofstream outfile(outputFile);
        if (outfile.is_open()) {
            outfile << "Simulation completed successfully\n";
            outfile << "Total steps: " << numSteps << "\n";
            outfile << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
            outfile << "Average firing rate: " << brain->getAverageFiringRate() << "\n";
            outfile.close();
            std::cout << "Results saved to: " << outputFile << std::endl;
        }
        
        std::cout << "Simulation complete!" << std::endl;
    }
    
    void runPhase3Demo() {
        Phase3Experiment experiment;
        experiment.run(3, 200);
    }
    
    void runPhase4Demo() {
        Phase4Experiment experiment;
        experiment.run(50);
    }
    
    void runPhase6Demo() {
        Phase6IntegratedExperiment experiment;
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 1000;
        
        auto result = experiment.run(config);
        
        std::cout << "\nPhase 6 Demo Results:" << std::endl;
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Integration score: " << 
            (result.memoryWorkingMemoryIntegrated + result.memoryEpisodicMemoryIntegrated + 
             result.neuromodulationIntegrated + result.predictionIntegrated + result.developmentIntegrated) << "/5" << std::endl;
    }
    
    void runUnitTests() {
        std::cout << "Unit tests would be run here" << std::endl;
        // This would invoke the actual test suite
    }
    
    void runIntegrationTests() {
        std::cout << "Integration tests would be run here" << std::endl;
        // This would run system integration tests
    }
    
    void runPerformanceTests() {
        std::cout << "Performance tests would be run here" << std::endl;
        // This would benchmark system performance
    }
    
    void runBenchmark(size_t numNeurons, size_t numSteps) {
        std::cout << "Benchmarking with " << numNeurons << " neurons for " << numSteps << " steps" << std::endl;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(numNeurons));
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for benchmarking!" << std::endl;
            return;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(static_cast<SimulationStep>(step));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = (numSteps * 1000.0) / duration.count();
        size_t totalSpikes = brain->getTotalSpikeCount();
        
        std::cout << "Benchmark Results:" << std::endl;
        std::cout << "  Steps per second: " << stepsPerSecond << std::endl;
        std::cout << "  Total spikes: " << totalSpikes << std::endl;
        std::cout << "  Time taken: " << duration.count() << " ms" << std::endl;
    }
    
    void printSystemInfo() {
        std::cout << "NLM System Information:" << std::endl;
        std::cout << "  Version: 0.1.0" << std::endl;
        std::cout << "  Build type: Release" << std::endl;
        std::cout << "  C++ standard: C++20" << std::endl;
        std::cout << "  Available memory: Unlimited (simulated)" << std::endl;
    }
    
    void exploreConfigurationOptions() {
        std::cout << "\nConfiguration Options:" << std::endl;
        std::cout << "  Brain settings:" << std::endl;
        std::cout << "    - neuron_count: Number of neurons (default: 1000)" << std::endl;
        std::cout << "    - region_count: Number of brain regions (default: 2)" << std::endl;
        std::cout << "    - connection_probability: Synapse formation probability (default: 0.1)" << std::endl;
        std::cout << "  Plasticity settings:" << std::endl;
        std::cout << "    - plasticity_learning_rate: General learning rate" << std::endl;
        std::cout << "    - stdp_ltp_weight: STDP long-term potentiation weight" << std::endl;
        std::cout << "    - stdp_ltd_weight: STDP long-term depression weight" << std::endl;
        std::cout << "  Development settings:" << std::endl;
        std::cout << "    - development_synaptogenesis_rate: Neurogenesis rate" << std::endl;
        std::cout << "    - development_pruning_rate: Pruning rate" << std::endl;
        std::cout << "  Neuromodulation settings:" << std::endl;
        std::cout << "    - dopamine_baseline: Baseline dopamine level" << std::endl;
    }
    
    void exploreLearningCapabilities() {
        std::cout << "\nLearning Capabilities:" << std::endl;
        std::cout << "  Plasticity mechanisms:" << std::endl;
        std::cout << "    - STDP: Spike-timing dependent plasticity" << std::endl;
        std::cout << "    - Hebbian: Hebbian learning (fire together, wire together)" << std::endl;
        std::cout << "    - Structural: Synaptogenesis and pruning" << std::endl;
        std::cout << "  Neuromodulation:" << std::endl;
        std::cout << "    - Dopamine: Reward prediction and action selection" << std::endl;
        std::cout << "    - Curiosity: Exploration and novelty detection" << std::endl;
        std::cout << "    - Novelty: Detecting unexpected events" << std::endl;
        std::cout << "    - Prediction error: Learning from prediction mismatches" << std::endl;
    }
    
    void saveBrainState(const std::string& filename) {
        std::cout << "Saving brain state to: " << filename << std::endl;
        
        // Create a simple brain for demonstration
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            std::cout << "Brain initialized successfully" << std::endl;
            
            // Save the brain state
            if (brain->save(filename)) {
                std::cout << "Brain state saved successfully to: " << filename << std::endl;
            } else {
                std::cerr << "Failed to save brain state" << std::endl;
            }
        } else {
            std::cerr << "Failed to initialize brain" << std::endl;
        }
    }
    
    void loadBrainState(const std::string& filename) {
        std::cout << "Loading brain state from: " << filename << std::endl;
        
        // Create a simple brain for demonstration
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            std::cout << "Brain initialized successfully" << std::endl;
            
            // Load the brain state
            if (brain->load(filename)) {
                std::cout << "Brain state loaded successfully from: " << filename << std::endl;
            } else {
                std::cerr << "Failed to load brain state" << std::endl;
            }
        } else {
            std::cerr << "Failed to initialize brain" << std::endl;
        }
    }
    
    void analyzeBrainState() {
        std::cout << "Analyzing brain state..." << std::endl;
        
        // Create a simple brain for demonstration
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->initialize()) {
            std::cout << "Brain analysis:" << std::endl;
            std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
            std::cout << "  Excitation/inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        } else {
            std::cerr << "Failed to initialize brain" << std::endl;
        }
    }
    
    void compareConfigurations(int argc, char* argv[]) {
        std::cout << "Comparing configurations..." << std::endl;
        
        if (argc < 4) {
            std::cerr << "Usage: nlm compare <config_file1> <config_file2>" << std::endl;
            return;
        }
        
        std::string config1 = argv[2];
        std::string config2 = argv[3];
        
        std::cout << "Comparing configurations:" << std::endl;
        std::cout << "  File 1: " << config1 << std::endl;
        std::cout << "  File 2: " << config2 << std::endl;
        
        // This would implement configuration comparison logic
        std::cout << "Configuration comparison feature would be implemented here" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    
    NLMCommandLine cli;
    bool success = cli.execute(argc, argv);
    
    if (!success) {
        std::cerr << "Command execution failed" << std::endl;
        return 1;
    }
    
    std::cout << "Command completed successfully" << std::endl;
    return 0;
}
