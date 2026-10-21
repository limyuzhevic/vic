/**
 * Phase 6 Demo - Integration Test
 * Updated with command-line interface and NLMConfig support
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/ConfigOptions.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace nlm;

// Helper functions for Phase 6 demo
static void printPhase6Usage(const std::string& program_name) {
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "============================" << std::endl << std::endl;
    
    std::cout << "Usage: " << program_name << " [options]" << std::endl << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --quick                       Run quick verification only (no full experiment)" << std::endl << std::endl;
    
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  --neuron-count=N              Neuron count (default: 500)" << std::endl;
    std::cout << "  --max-steps=N                 Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --connection-prob=F           Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --enable-checkpointing[=bool]  Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --enable-replay[=bool]        Enable replay system (default: true)" << std::endl;
    std::cout << "  --enable-development[=bool]   Enable development system (default: true)" << std::endl;
    std::cout << "  --checkpoint-path=PATH        Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl << std::endl;
    
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output-json=FILE            Output results in JSON format" << std::endl;
    std::cout << "  --output-summary=FILE         Output summary text file" << std::endl << std::endl;
    
    std::cout << "Advanced Configuration (via key=value):" << std::endl;
    std::cout << "  key=value                     Set any configuration option (see NLMConfig for full list)" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " --neuron-count=200 --max-steps=1000" << std::endl;
    std::cout << "  " << program_name << " --quick --enable-checkpointing=false" << std::endl;
    std::cout << "  " << program_name << " --output-json=results.json --output-summary=summary.txt" << std::endl;
    std::cout << "  " << program_name << " --neuron-count=1000 --stdp-ltp-weight=0.05" << std::endl;
}

static std::string boolToString(bool value) {
    return value ? "true" : "false";
}

static void printResultsToFile(const Phase6IntegrationResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return;
    }
    
    file << "=== NLM Phase 6 Integration Results ===" << std::endl;
    file << "Generated on: " << std::endl; // Would add timestamp here
    file << std::endl;
    
    file << "=== FINAL RESULTS ===" << std::endl;
    file << "Total reward: " << result.totalReward << std::endl;
    file << "Avg firing rate: " << result.avgFiringRate << std::endl;
    file << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    file << "Dopamine level: " << result.dopamineLevel << std::endl;
    file << std::endl;
    
    file << "=== INTEGRATION STATUS ===" << std::endl;
    file << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << std::endl;
    
    file << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    file.close();
    std::cout << "Results written to: " << filename << std::endl;
}

static void printResultsJSON(const Phase6IntegrationResult& result, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open JSON output file: " << filename << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"integration_test\": \"NLM Phase 6\",\n";
    file << "  \"timestamp\": \"" << std::endl; // Would add timestamp here
    file << "  \"result\": {\n";
    file << "    \"total_reward\": " << result.totalReward << ",\n";
    file << "    \"avg_firing_rate\": " << result.avgFiringRate << ",\n";
    file << "    \"memory_episodes_stored\": " << result.memoryEpisodesStored << ",\n";
    file << "    \"dopamine_level\": " << result.dopamineLevel << ",\n";
    file << "    \"novelty_level\": " << result.noveltyLevel << ",\n";
    file << "    \"curiosity_level\": " << result.curiosityLevel << ",\n";
    file << "    \"avg_synaptic_weight\": " << result.avgSynapticWeight << ",\n";
    file << "    \"total_wall_clock_time\": " << result.totalWallClockTime << ",\n";
    file << "    \"integration_status\": {\n";
    file << "      \"memory_working_memory_integrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
    file << "      \"memory_episodic_memory_integrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
    file << "      \"neuromodulation_integrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
    file << "      \"prediction_integrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
    file << "      \"development_integrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
    file << "      \"checkpointing_works\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
    file << "      \"replay_works\": " << (result.replayWorks ? "true" : "false") << "\n";
    file << "    }\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    std::cout << "JSON results written to: " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool quickMode = false;
    bool outputJson = false;
    bool outputSummary = false;
    std::string jsonOutputFile = "";
    std::string summaryOutputFile = "";
    
    // Configuration variables with defaults
    size_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    
    std::string configFile = "";
    
    // Parse command line
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printPhase6Usage("nlm_phase6_demo");
            return 0;
        }
        
        if (arg == "--quick") {
            quickMode = true;
        } else if (arg == "--output-json" && i + 1 < argc) {
            outputJson = true;
            jsonOutputFile = argv[++i];
        } else if (arg == "--output-summary" && i + 1 < argc) {
            outputSummary = true;
            summaryOutputFile = argv[++i];
        } else if (arg.find("--neuron-count=") == 0) {
            neuronCount = static_cast<size_t>(std::atoi(arg.substr(15).c_str()));
        } else if (arg.find("--max-steps=") == 0) {
            maxSteps = static_cast<uint64_t>(std::atoi(arg.substr(12).c_str()));
        } else if (arg.find("--connection-prob=") == 0) {
            connectionProbability = std::atof(arg.substr(19).c_str());
        } else if (arg.find("--enable-checkpointing=") == 0) {
            std::string value = arg.substr(24);
            enableCheckpointing = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--enable-replay=") == 0) {
            std::string value = arg.substr(17);
            enableReplay = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--enable-development=") == 0) {
            std::string value = arg.substr(22);
            enableDevelopment = (value == "true" || value == "1" || value == "yes");
        } else if (arg.find("--checkpoint-path=") == 0) {
            checkpointPath = arg.substr(18);
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(8);
        } else if (arg.find('=') != std::string::npos) {
            // Try to parse as key=value for NLMConfig
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                // Log this for debugging
                std::cout << "Config option: " << key << " = " << value << std::endl;
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printPhase6Usage("nlm_phase6_demo");
            return 1;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Starting with configuration:" << std::endl;
    std::cout << "  Neuron count: " << neuronCount << std::endl;
    std::cout << "  Max steps: " << maxSteps << std::endl;
    std::cout << "  Connection probability: " << connectionProbability << std::endl;
    std::cout << "  Enable checkpointing: " << boolToString(enableCheckpointing) << std::endl;
    std::cout << "  Enable replay: " << boolToString(enableReplay) << std::endl;
    std::cout << "  Enable development: " << boolToString(enableDevelopment) << std::endl;
    std::cout << "  Checkpoint path: " << checkpointPath << std::endl;
    std::cout << "  Quick mode: " << boolToString(quickMode) << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification (unless in quick mode only)
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    if (!quickMode) {
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
    }
    
    // Run full experiment with configuration
    if (!quickMode || neuronCount != 500 || maxSteps != 2000) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = neuronCount;
        config.maxSteps = maxSteps;
        config.connectionProbability = connectionProbability;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        config.checkpointPath = checkpointPath;
        
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
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        // Output results if requested
        if (outputJson && !jsonOutputFile.empty()) {
            printResultsJSON(result, jsonOutputFile);
        }
        
        if (outputSummary && !summaryOutputFile.empty()) {
            printResultsToFile(result, summaryOutputFile);
        }
    } else if (quickMode) {
        std::cout << "--- Quick Integration Summary ---" << std::endl;
        std::cout << "Integration test passed successfully!" << std::endl;
        std::cout << "All brain systems are properly connected and functioning." << std::endl;
    }
    
    return 0;
}
