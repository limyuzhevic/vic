#include "Phase6Demo.cpp"
#include "AdvancedAnalysisTools.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace nlm;

class CommandLineInterface {
public:
    void run(int argc, char* argv[]) {
        std::cout << "NLM Phase 6 Command Line Interface" << std::endl;
        std::cout << "===================================" << std::endl;
        
        // Parse command line arguments
        std::vector<std::string> args(argv + 1, argv + argc);
        
        if (args.empty()) {
            showHelp();
            return;
        }
        
        std::string command = args[0];
        
        if (command == "help" || command == "--help" || command == "-h") {
            showHelp();
        } else if (command == "run" || command == "demo") {
            runDemo(args);
        } else if (command == "benchmark" || command == "bench") {
            runBenchmark(args);
        } else if (command == "config" || command == "configure") {
            showConfigOptions(args);
        } else if (command == "checkpoints" || command == "ckpt") {
            showCheckpoints(args);
        } else if (command == "metrics" || command == "stats") {
            showMetrics(args);
        } else if (command == "analyze") {
            runAnalysis(args);
        } else if (command == "interactive" || command == "interactive") {
            runInteractiveMode(args);
        } else if (command == "version" || command == "--version") {
            showVersion();
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
private:
    void showHelp() {
        std::cout << "\nNLM Phase 6 Command Line Interface - Available Commands:" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "  help, -h, --help                    Show this help message" << std::endl;
        std::cout << "  version, --version                  Show version information" << std::endl;
        std::cout << "  run, demo                           Run the Phase 6 integration demo" << std::endl;
        std::cout << "  benchmark, bench                    Run performance benchmarks" << std::endl;
        std::cout << "  config, configure                    Show and modify configuration options" << std::endl;
        std::cout << "  checkpoints, ckpt                   List and manage saved checkpoints" << std::endl;
        std::cout << "  metrics, stats                      Show current metrics and statistics" << std::endl;
        std::cout << "  analyze                             Run advanced analysis tools" << std::endl;
        std::cout << "  interactive                          Run in interactive mode with live updates" << std::endl;
        std::cout << "\nExample usage:" << std::endl;
        std::cout << "  nlm_demo_cli run --episodes 10 --steps 500" << std::endl;
        std::cout << "  nlm_demo_cli benchmark --duration 60" << std::endl;
        std::cout << "  nlm_demo_cli config neuron_count 1000" << std::endl;
    }
    
    void showVersion() {
        std::cout << "NLM Phase 6 Integration Demo - Version 0.1.0" << std::endl;
        std::cout << "Experimental Artificial Developmental Brain" << std::endl;
        std::cout << "Built with C++20, featuring integrated memory, prediction, and neuromodulation systems." << std::endl;
    }
    
    void runDemo(std::vector<std::string>& args) {
        std::cout << "\nRunning Phase 6 Integration Demo..." << std::endl;
        
        // Parse demo configuration
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        // Parse command line arguments for configuration
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--neuron-count" && i + 1 < args.size()) {
                config.neuronCount = std::stoi(args[i + 1]);
                i++;
            } else if (args[i] == "--max-steps" && i + 1 < args.size()) {
                config.maxSteps = std::stoi(args[i + 1]);
                i++;
            } else if (args[i] == "--no-checkpoint") {
                config.enableCheckpointing = false;
            } else if (args[i] == "--no-replay") {
                config.enableReplay = false;
            } else if (args[i] == "--no-development") {
                config.enableDevelopment = false;
            } else if (args[i] == "--help") {
                showDemoHelp();
                return;
            }
        }
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // Run the experiment
        auto result = experiment.run(config);
        
        // Display results
        std::cout << "\n=== EXPERIMENT RESULTS ===" << std::endl;
        std::cout << "Total Reward: " << result.totalReward << std::endl;
        std::cout << "Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    }
    
    void showDemoHelp() {
        std::cout << "\nPhase 6 Demo Configuration Options:" << std::endl;
        std::cout << "--neuron-count N      Set number of neurons (default: 500)" << std::endl;
        std::cout << "--max-steps N         Set maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "--no-checkpoint        Disable checkpointing" << std::endl;
        std::cout << "--no-replay            Disable replay system" << std::endl;
        std::cout << "--no-development       Disable development system" << std::endl;
    }
    
    void runBenchmark(std::vector<std::string>& args) {
        std::cout << "\nRunning Performance Benchmark..." << std::endl;
        
        // Create minimal configuration for benchmarking
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.05f);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for benchmark" << std::endl;
            return;
        }
        
        // Run benchmark
        uint64_t totalSpikes = 0;
        double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        
        for (uint64_t step = 0; step < 10000; ++step) {
            brain->step(step, step * 0.001);
            totalSpikes += brain->getTotalSpikeCount();
        }
        
        double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        double duration = (endTime - startTime) / 1e9; // Convert to seconds
        
        std::cout << "\n=== BENCHMARK RESULTS ===" << std::endl;
        std::cout << "Duration: " << duration << " seconds" << std::endl;
        std::cout << "Steps processed: 10000" << std::endl;
        std::cout << "Average steps per second: " << 10000.0 / duration << std::endl;
        std::cout << "Total spikes generated: " << totalSpikes << std::endl;
        std::cout << "Average spikes per step: " << static_cast<float>(totalSpikes) / 10000.0 << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    }
    
    void showConfigOptions(std::vector<std::string>& args) {
        std::cout << "\nNLM Configuration Options:" << std::endl;
        std::cout << "=======================" << std::endl;
        
        // Show default configuration
        auto config = std::make_shared<Config>();
        
        std::cout << "Default Configuration:" << std::endl;
        std::cout << "  neuron_count: 500" << std::endl;
        std::cout << "  region_count: 1" << std::endl;
        std::cout << "  connection_probability: 0.15" << std::endl;
        std::cout << "  random_seed: 42" << std::endl;
        std::cout << "  simulation_timestep: 0.001" << std::endl;
        std::cout << "\nPlasticity parameters:" << std::endl;
        std::cout << "  stdp_ltp_weight: 0.01" << std::endl;
        std::cout << "  stdp_ltd_weight: 0.012" << std::endl;
        std::cout << "  stdp_tau: 20.0" << std::endl;
        std::cout << "  synaptogenesis_rate: 0.0001" << std::endl;
        std::cout << "  pruning_rate: 0.00001" << std::endl;
        
        if (args.size() > 1) {
            std::cout << "\nSetting configuration option: " << args[1] << std::endl;
            // TODO: Implement configuration modification
        }
    }
    
    void showCheckpoints(std::vector<std::string>& args) {
        std::cout << "\nCheckpoint Management:" << std::endl;
        std::cout << "=====================" << std::endl;
        
        // Check for checkpoint directory
        std::string checkpointDir = "checkpoints";
        
        // TODO: Implement actual checkpoint listing
        std::cout << "Checkpoint directory: " << checkpointDir << std::endl;
        std::cout << "(Feature not yet implemented - requires CheckPointManager integration)" << std::endl;
    }
    
    void showMetrics(std::vector<std::string>& args) {
        std::cout << "\nCurrent System Metrics:" << std::endl;
        std::cout << "======================" << std::endl;
        
        // Create a minimal brain for metrics
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            std::cout << "System Status: Operational" << std::endl;
            std::cout << "Neuron Count: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Active Neuron Count: " << brain->getActiveNeuronCount() << std::endl;
            std::cout << "Firing Neuron Count: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            
            // Check memory systems
            auto* wm = brain->getWorkingMemory();
            auto* em = brain->getEpisodicMemory();
            auto* dopamine = brain->getDopamine();
            
            std::cout << "\nMemory Systems:" << std::endl;
            std::cout << "  Working Memory: " << (wm ? "ENABLED" : "DISABLED") << std::endl;
            std::cout << "  Episodic Memory: " << (em ? "ENABLED" : "DISABLED") << std::endl;
            std::cout << "  Neuromodulation (Dopamine): " << (dopamine ? "ENABLED" : "DISABLED") << std::endl;
        } else {
            std::cerr << "Failed to initialize brain for metrics" << std::endl;
        }
    }
    
    void runAnalysis(std::vector<std::string>& args) {
        std::cout << "\n=== Advanced Analysis Tools ===" << std::endl;
        
        if (args.empty()) {
            std::cout << "Available analysis modes:" << std::endl;
            std::cout << "  learning-curve    Analyze learning progression" << std::endl;
            std::cout << "  synaptic-dynamics Analyze synaptic changes" << std::endl;
            std::cout << "  firing-rate-analysis Analyze firing patterns" << std::endl;
            std::cout << "  network-connectivity Analyze network topology" << std::endl;
            std::cout << "  memory-analysis    Analyze memory systems" << std::endl;
            std::cout << "  plasticity-analysis Analyze plasticity effectiveness" << std::endl;
            std::cout << "  behavior-analysis  Analyze behavioral patterns" << std::endl;
            std::cout << "  export-data       Export analysis results" << std::endl;
            std::cout << "  visualization     Generate visualizations" << std::endl;
            return;
        }
        
        std::string mode = args[0];
        // Pass remaining arguments to analysis
        std::vector<std::string> analysisArgs(args.begin() + 1, args.end());
        
        analysisTools.runAnalysis(mode, analysisArgs);
    }
    
    void runInteractiveMode(std::vector<std::string>& args) {
        std::cout << "\nInteractive Mode:" << std::endl;
        std::cout << "===============" << std::endl;
        std::cout << "Starting interactive exploration mode..." << std::endl;
        std::cout << "(Feature not yet implemented - requires full interactive UI)" << std::endl;
        std::cout << "Available commands in interactive mode:" << std::endl;
        std::cout << "  status       - Show current system status" << std::endl;
        std::cout << "  simulate N   - Run N simulation steps" << std::endl;
        std::cout << "  inject N     - Inject N units of current" << std::endl;
        std::cout << "  checkpoint   - Save current state" << std::endl;
        std::cout << "  help         - Show help" << std::endl;
        std::cout << "  exit         - Exit interactive mode" << std::endl;
    }
};

AdvancedAnalysisTools analysisTools;

int main(int argc, char* argv[]) {
    CommandLineInterface cli;
    cli.run(argc, argv);
    return 0;
}
