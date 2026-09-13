/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace nlm;

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Advanced command-line interface for Phase 6 integration testing" << std::endl << std::endl;
    std::cout << "Usage: ./Phase6Demo [options]" << std::endl << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  --mode <value>           Operation mode: quick|full|debug (default: quick)" << std::endl;
    std::cout << "  --config <file>          Load configuration from JSON/YAML file" << std::endl;
    std::cout << "  --output <format>        Output format: text|json|csv (default: text)" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --quick-mode              Run quick verification only (no full simulation)" << std::endl;
    std::cout << "  --save-config <file>      Save current configuration to file" << std::endl;
    std::cout << "  --list-checkpoints       List available checkpoints" << std::endl;
    std::cout << "  --load-checkpoint <file>  Load and run from checkpoint" << std::endl;
    std::cout << "  --checkpoint <file>      Checkpoint path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl << std::endl;
    std::cout << "Configuration options:" << std::endl;
    std::cout << "  --max-steps <N>           Number of simulation steps (default: 10000)" << std::endl;
    std::cout << "  --neuron-count <N>        Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count <N>        Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob <f>     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --enable-checkpointing    Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --enable-replay           Enable replay system (default: true)" << std::endl;
    std::cout << "  --enable-development      Enable development (default: true)" << std::endl << std::endl;
}

Phase6Config parseConfigFromCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        else if (arg == "--quick-mode") {
            config.quickMode = true;
        }
        else if (arg == "--verbose") {
            config.verbose = true;
        }
else if (arg == "--save-config" && i + 1 < argc) {
            config.saveConfiguration = true;
            std::string outputFile = argv[++i];
            config.saveConfigFilePath = outputFile;
        }
        else if (arg == "--list-checkpoints") {
            config.listCheckpoints = true;
        }
        else if (arg == "--load-checkpoint" && i + 1 < argc) {
            config.loadCheckpoint = true;
            config.checkpointFile = argv[++i];
        }
        else if (arg == "--mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "quick") config.quickMode = true;
            else if (mode == "full") config.quickMode = false;
            else if (mode == "debug") {
                config.quickMode = false;
                config.verbose = true;
            }
        }
        else if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            Config cfg;
            if (cfg.loadFromFile(configFile)) {
                // Convert config values to Phase6Config
                if (cfg.has("max_steps")) {
                    config.maxSteps = cfg.getOr<uint64_t>("max_steps", config.maxSteps);
                }
                if (cfg.has("neuron_count")) {
                    config.neuronCount = cfg.getOr<size_t>("neuron_count", config.neuronCount);
                }
                if (cfg.has("region_count")) {
                    config.regionCount = cfg.getOr<size_t>("region_count", config.regionCount);
                }
                if (cfg.has("connection_probability")) {
                    config.connectionProbability = cfg.getOr<float>("connection_probability", config.connectionProbability);
                }
                if (cfg.has("enable_checkpointing")) {
                    config.enableCheckpointing = cfg.getOr<bool>("enable_checkpointing", config.enableCheckpointing);
                }
                if (cfg.has("enable_replay")) {
                    config.enableReplay = cfg.getOr<bool>("enable_replay", config.enableReplay);
                }
                if (cfg.has("enable_development")) {
                    config.enableDevelopment = cfg.getOr<bool>("enable_development", config.enableDevelopment);
                }
                if (cfg.has("checkpoint_path")) {
                    config.checkpointPath = cfg.getOr<std::string>("checkpoint_path", config.checkpointPath);
                }
                if (cfg.has("output_format")) {
                    config.outputFormat = cfg.getOr<std::string>("output_format", config.outputFormat);
                }
            }
        }
        else if (arg == "--output" && i + 1 < argc) {
            config.outputFormat = argv[++i];
        }
        else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
        else if (arg == "--max-steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        }
        else if (arg == "--neuron-count" && i + 1 < argc) {
            config.neuronCount = static_cast<size_t>(std::stoull(argv[++i]));
        }
        else if (arg == "--region-count" && i + 1 < argc) {
            config.regionCount = static_cast<size_t>(std::stoull(argv[++i]));
        }
        else if (arg == "--connection-prob" && i + 1 < argc) {
            config.connectionProbability = std::stof(argv[++i]);
        }
        else if (arg == "--enable-checkpointing") {
            config.enableCheckpointing = true;
        }
        else if (arg == "--enable-replay") {
            config.enableReplay = true;
        }
        else if (arg == "--enable-development") {
            config.enableDevelopment = true;
        }
    }
    
    return config;
}

void printResults(const Phase6IntegrationResult& result, const std::string& format) {
    if (format == "json") {
        std::cout << "{\n";
        std::cout << "  \"totalReward\": " << result.totalReward << ",\n";
        std::cout << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n";
        std::cout << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n";
        std::cout << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n";
        std::cout << "  \"memoryWorkingMemoryIntegrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"memoryEpisodicMemoryIntegrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"neuromodulationIntegrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"predictionIntegrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"developmentIntegrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"checkpointingWorks\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
        std::cout << "  \"replayWorks\": " << (result.replayWorks ? "true" : "false") << ",\n";
        std::cout << "  \"totalWallClockTime\": " << result.totalWallClockTime << "\n";
        std::cout << "}" << std::endl;
    } else if (format == "csv") {
        std::cout << "totalReward,avgFiringRate,memoryEpisodesStored,dopamineLevel,memoryWorkingMemoryIntegrated,memoryEpisodicMemoryIntegrated,neuromodulationIntegrated,predictionIntegrated,developmentIntegrated,checkpointingWorks,replayWorks,totalWallClockTime" << std::endl;
        std::cout << result.totalReward << ","
                  << result.avgFiringRate << ","
                  << result.memoryEpisodesStored << ","
                  << result.dopamineLevel << ","
                  << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ","
                  << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ","
                  << (result.neuromodulationIntegrated ? "true" : "false") << ","
                  << (result.predictionIntegrated ? "true" : "false") << ","
                  << (result.developmentIntegrated ? "true" : "false") << ","
                  << (result.checkpointingWorks ? "true" : "false") << ","
                  << (result.replayWorks ? "true" : "false") << ","
                  << result.totalWallClockTime << std::endl;
    } else {
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
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    Phase6Config config = parseConfigFromCommandLine(argc, argv);
    
    if (config.listCheckpoints) {
        std::cout << "=== Available Checkpoints ===" << std::endl;
        // Implementation for listing checkpoints
        std::cout << "(Checkpoint listing not yet implemented)" << std::endl;
        return 0;
    }
    
    if (config.loadCheckpoint && !config.checkpointFile.empty()) {
        std::cout << "=== Loading Checkpoint ===" << std::endl;
        std::cout << "Loading from: " << config.checkpointFile << std::endl;
        // Implementation for loading checkpoint
        std::cout << "(Checkpoint loading not yet implemented)" << std::endl;
        return 0;
    }
    
    if (config.saveConfiguration && !config.saveConfigFilePath.empty()) {
        std::cout << "=== Saving Configuration ===" << std::endl;
        std::cout << "Saving to: " << config.saveConfigFilePath << std::endl;
        // Implementation for saving configuration
        std::cout << "(Configuration saving not yet implemented)" << std::endl;
        return 0;
    }
    
    // Set logging level based on verbosity
    Logger::getInstance().setLevel(config.verbose ? Logger::Level::Debug : Logger::Level::Info);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Mode: " << (config.quickMode ? "Quick Test" : "Full Experiment") << std::endl;
    if (config.verbose) {
        std::cout << "Verbose logging enabled" << std::endl;
    }
    std::cout << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification (unless quickMode is false and we have a loaded checkpoint)
    if (config.quickMode || !config.loadCheckpoint) {
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    if (config.enableCheckpointing) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
    }
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with settings from config
    if (!config.quickMode) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        auto result = experiment.run(config);
        printResults(result, config.outputFormat);
    }
    
    return 0;
}
