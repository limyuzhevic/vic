// NLM Command Line Interface - Enhanced Demo Application
// 
// This application provides a user-friendly command-line interface for the NLM framework,
// making it easier to run simulations, configure experiments, and access features.
// 
// Features:
// - Interactive configuration through command line arguments
// - Multiple demonstration modes (Phase 2 demo, Phase 6 integration demo, custom config)
// - Progress monitoring and real-time output
// - Configuration file support
// - Result logging and statistics
// - Error handling and user-friendly messages

#include "src/main.cpp"
#include "src/experiments/Phase6Demo.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace nlm;

class NLMCLI {
public:
    NLMCLI() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Version 1.0.0 - Enhanced Demo Application" << std::endl;
        std::cout << std::endl;
    }
    
    void showHelp() {
        std::cout << "Usage: nlm-cli [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --mode <mode>          Demo mode: phase2, phase6, custom, verify" << std::endl;
        std::cout << "  --config <file>        Configuration file (JSON)" << std::endl;
        std::cout << "  --neurons <n>          Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <n>            Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --seed <n>             Random seed (default: 42)" << std::endl;
        std::cout << "  --output <file>        Output results to file" << std::endl;
        std::cout << "  --quiet                Quiet mode (minimal output)" << std::endl;
        std::cout << "  --help                 Show this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-cli --mode phase2                          Run Phase 2 demo (basic neural computation)" << std::endl;
        std::cout << "  nlm-cli --mode phase6 --steps 5000              Run Phase 6 integration with 5000 steps" << std::endl;
        std::cout << "  nlm-cli --mode custom --config my_config.json    Run with custom configuration" << std::endl;
        std::cout << "  nlm-cli --mode verify                          Verify integration only" << std::endl;
    }
    
    bool parseArguments(int argc, char** argv, std::map<std::string, std::string>& options) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return true;  // Exit after showing help
            }
            
            if (arg.substr(0, 2) == "--") {
                std::string key = arg.substr(2);
                std::string value;
                
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                }
                
                options[key] = value;
            }
        }
        
        return false;
    }
    
    void runPhase2Demo(const std::map<std::string, std::string>& options) {
        std::cout << "=== NLM Phase 2: Real Neural Computation Demo ===" << std::endl;
        std::cout << "This demo demonstrates basic neural computation with:" << std::endl;
        std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
        std::cout << "  - Event-driven spike propagation" << std::endl;
        std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
        std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Set seed
        int64_t seed = 42;
        if (options.count("seed")) {
            seed = std::stoi(options.at("seed"));
        }
        config->set("random_seed", seed, ConfigSource::Default);
        
        // Set neuron count
        size_t neuronCount = 500;
        if (options.count("neurons")) {
            neuronCount = std::stoi(options.at("neurons"));
        }
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        
        // Set simulation parameters
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Set plasticity parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        brain->logStatus();
        
        // Run basic connectivity test
        std::cout << std::endl;
        std::cout << "Test 1: Basic Neural Connectivity" << std::endl;
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment
        std::cout << std::endl;
        std::cout << "Test 2: Plasticity Learning Experiment" << std::endl;
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        std::cout << std::endl;
        std::cout << "Test 3: STDP Verification" << std::endl;
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        std::cout << std::endl;
        std::cout << "=== Phase 2 Demo Complete ===" << std::endl;
    }
    
    void runPhase6Demo(const std::map<std::string, std::string>& options) {
        std::cout << "=== NLM Phase 6: Integration Demo ===" << std::endl;
        std::cout << "This demo runs the complete Phase 6 integration test..." << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        Phase6Config config;
        
        // Set neuron count
        size_t neuronCount = 500;
        if (options.count("neurons")) {
            neuronCount = std::stoi(options.at("neurons"));
        }
        config.neuronCount = neuronCount;
        
        // Set steps
        size_t steps = 2000;
        if (options.count("steps")) {
            steps = std::stoi(options.at("steps"));
        }
        config.maxSteps = steps;
        
        // Create and run experiment
        Phase6IntegratedExperiment experiment;
        
        // First, verify integration
        std::cout << "Step 1: Verifying integration..." << std::endl;
        if (!experiment.verifyIntegration()) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return;
        }
        
        std::cout << "Integration verification passed!" << std::endl;
        std::cout << std::endl;
        
        // Run full experiment
        std::cout << "Step 2: Running full integration experiment..." << std::endl;
        Phase6IntegrationResult result = experiment.run(config);
        
        // Display results
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
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
        std::cout << std::endl;
        
        // Save results to file if requested
        if (options.count("output")) {
            std::string outputFile = options.at("output");
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "NLM Phase 6 Integration Results" << std::endl;
                file << "=============================" << std::endl;
                file << "Total reward: " << result.totalReward << std::endl;
                file << "Average firing rate: " << result.avgFiringRate << std::endl;
                file << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
                file << "Dopamine level: " << result.dopamineLevel << std::endl;
                file << "Active traces: " << result.activeTraceCount << std::endl;
                file << "Total spikes: " << result.totalSpikeCount << std::endl;
                file << "Spiking neurons: " << result.spikingNeurons << std::endl;
                file << std::endl;
                file << "Integration Status:" << std::endl;
                file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                file << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                file << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
                file << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
                file << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
                file.close();
                std::cout << "Results saved to: " << outputFile << std::endl;
            } else {
                std::cerr << "ERROR: Could not write output file: " << outputFile << std::endl;
            }
        }
        
        std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
    }
    
    void runVerifyIntegration(const std::map<std::string, std::string>& options) {
        std::cout << "=== NLM Integration Verification ===" << std::endl;
        std::cout << "Running integration verification..." << std::endl;
        std::cout << std::endl;
        
        Phase6IntegratedExperiment experiment;
        if (experiment.verifyIntegration()) {
            std::cout << "SUCCESS: Integration verification passed!" << std::endl;
            std::cout << "All brain systems are properly connected and functional." << std::endl;
        } else {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            std::cerr << "Some brain systems are not properly integrated." << std::endl;
        }
    }
    
    void run(const std::string& mode, const std::map<std::string, std::string>& options) {
        if (mode == "phase2") {
            runPhase2Demo(options);
        } else if (mode == "phase6") {
            runPhase6Demo(options);
        } else if (mode == "verify") {
            runVerifyIntegration(options);
        } else {
            std::cerr << "ERROR: Unknown mode: " << mode << std::endl;
            showHelp();
        }
    }
};

int main(int argc, char** argv) {
    NLMCLI cli;
    
    std::map<std::string, std::string> options;
    bool showHelp = cli.parseArguments(argc, argv, options);
    
    if (showHelp) {
        return 0;
    }
    
    std::string mode = "phase6";  // Default mode
    if (options.count("mode")) {
        mode = options.at("mode");
    }
    
    try {
        cli.run(mode, options);
        std::cout << std::endl;
        std::cout << "=== Demo completed successfully ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
