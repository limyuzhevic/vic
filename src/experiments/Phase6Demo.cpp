/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace nlm;

// Command-line argument parsing helper
struct DemoOptions {
    bool help = false;
    bool verbose = false;
    std::string configFile = "";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool skipVerification = false;
    bool runIndividualTests = true;
    std::string outputFormat = "text"; // text, json, csv
    float curiosityThreshold = 0.3f;
    float noveltyDecay = 0.99f;
    double simulationTimestep = 0.001;
    std::string experimentName = "Phase6Demo";
};

DemoOptions parseCommandLine(int argc, char* argv[]) {
    DemoOptions options;
    
    // Simple command-line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--skip-verification") {
            options.skipVerification = true;
        } else if (arg == "--no-individual-tests") {
            options.runIndividualTests = false;
        } else if (arg == "--output-format" && i + 1 < argc) {
            options.outputFormat = argv[++i];
        } else if (arg == "--curiosity-threshold" && i + 1 < argc) {
            options.curiosityThreshold = std::stof(argv[++i]);
        } else if (arg == "--novelty-decay" && i + 1 < argc) {
            options.noveltyDecay = std::stof(argv[++i]);
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            options.neuronCount = static_cast<size_t>(std::stoull(argv[++i]));
        } else if (arg == "--max-steps" && i + 1 < argc) {
            options.maxSteps = static_cast<size_t>(std::stoull(argv[++i]));
        } else if (arg == "--simulation-timestep" && i + 1 < argc) {
            options.simulationTimestep = std::stod(argv[++i]);
        } else if (arg == "--config-file" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--experiment-name" && i + 1 < argc) {
            options.experimentName = argv[++i];
        } else if (arg == "--no-checkpointing") {
            options.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            options.enableReplay = false;
        } else if (arg == "--no-development") {
            options.enableDevelopment = false;
        }
    }
    
    return options;
}

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --verbose                 Enable verbose output" << std::endl;
    std::cout << "      --skip-verification        Skip integration verification" << std::endl;
    std::cout << "      --no-individual-tests     Skip individual system tests" << std::endl;
    std::cout << "      --no-checkpointing         Disable checkpointing" << std::endl;
    std::cout << "      --no-replay                Disable replay feature" << std::endl;
    std::cout << "      --no-development           Disable development system" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "      --neuron-count N           Number of neurons (default: 500)" << std::endl;
    std::cout << "      --max-steps N              Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "      --simulation-timestep T     Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "      --curiosity-threshold F    Curiosity threshold (default: 0.3)" << std::endl;
    std::cout << "      --novelty-decay F          Novelty decay factor (default: 0.99)" << std::endl;
    std::cout << "      --output-format FMT        Output format: text, json, csv (default: text)" << std::endl;
    std::cout << "      --config-file FILE          Configuration file path" << std::endl;
    std::cout << "      --experiment-name NAME     Name for this experiment run" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm_phase6_demo --verbose --neuron-count 1000 --max-steps 5000 --experiment-name 'TestRun1'" << std::endl;
    std::cout << std::endl;
}

// Advanced agent configuration helper
void configureAgentBrain(nlm::AgentBrain& agent, const DemoOptions& options) {
    // Configure based on command line options
    agent.enableCuriosity(options.enableDevelopment && options.curiosityThreshold > 0.0f);
    agent.enableRewardModulation(options.enableDevelopment);
    agent.enableStructuralPlasticity(options.enableDevelopment);
    agent.enableDevelopment(options.enableDevelopment);
    
    // Set novelty decay based on configuration
    agent.setNoveltyDecay(options.noveltyDecay);
}

// Experiment result formatter
void formatResults(const nlm::Phase6Results& result, const std::string& format) {
    if (format == "json") {
        std::cout << "{\n";
        std::cout << "  \"total_reward\": " << result.totalReward << ",\n";
        std::cout << "  \"avg_firing_rate\": " << result.avgFiringRate << ",\n";
        std::cout << "  \"memory_episodes_stored\": " << result.memoryEpisodesStored << ",\n";
        std::cout << "  \"dopamine_level\": " << result.dopamineLevel << ",\n";
        std::cout << "  \"memory_working_memory_integrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"memory_episodic_memory_integrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"neuromodulation_integrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"prediction_integrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"development_integrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
        std::cout << "  \"checkpointing_works\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
        std::cout << "  \"total_wall_clock_time\": " << result.totalWallClockTime << "\n";
        std::cout << "}" << std::endl;
    } else if (format == "csv") {
        std::cout << "metric,value" << std::endl;
        std::cout << "total_reward," << result.totalReward << std::endl;
        std::cout << "avg_firing_rate," << result.avgFiringRate << std::endl;
        std::cout << "memory_episodes_stored," << result.memoryEpisodesStored << std::endl;
        std::cout << "dopamine_level," << result.dopamineLevel << std::endl;
        std::cout << "memory_working_memory_integrated," << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << std::endl;
        std::cout << "memory_episodic_memory_integrated," << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << std::endl;
        std::cout << "neuromodulation_integrated," << (result.neuromodulationIntegrated ? "true" : "false") << std::endl;
        std::cout << "prediction_integrated," << (result.predictionIntegrated ? "true" : "false") << std::endl;
        std::cout << "development_integrated," << (result.developmentIntegrated ? "true" : "false") << std::endl;
        std::cout << "checkpointing_works," << (result.checkpointingWorks ? "true" : "false") << std::endl;
        std::cout << "total_wall_clock_time," << result.totalWallClockTime << std::endl;
    } else { // text format (default)
        std::cout << "=== Phase 6 Integration Results ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Parse command line options
    DemoOptions options = parseCommandLine(argc, argv);
    
    // Show help if requested
    if (options.help) {
        printHelp();
        return 0;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Experiment: " << options.experimentName << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    
    if (options.verbose) {
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << options.neuronCount << std::endl;
        std::cout << "  Max Steps: " << options.maxSteps << std::endl;
        std::cout << "  Simulation Timestep: " << options.simulationTimestep << std::endl;
        std::cout << "  Checkpointing: " << (options.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (options.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (options.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Curiosity Threshold: " << options.curiosityThreshold << std::endl;
        std::cout << "  Novelty Decay: " << options.noveltyDecay << std::endl;
        std::cout << "  Output Format: " << options.outputFormat << std::endl;
    }
    
    // Initialize logging with appropriate level
    auto& logger = nlm::Logger::getInstance();
    if (options.verbose) {
        logger.setLevel(nlm::Logger::Level::Info);
    } else {
        logger.setLevel(nlm::Logger::Level::Warning); // Reduce noise for normal runs
    }
    
    // Create experiment
    nlm::Phase6IntegratedExperiment experiment;
    
    // Run integration verification if not skipped
    if (!options.skipVerification) {
        std::cout << std::endl;
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems if requested
    if (options.runIndividualTests) {
        std::cout << std::endl;
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << std::endl;
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        
        std::cout << std::endl;
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << std::endl;
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    // Run full experiment with configuration from command line
    std::cout << std::endl;
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    nlm::Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    
    // Apply agent configuration based on options
    auto agent = experiment.getAgentBrain();
    if (agent) {
        configureAgentBrain(*agent, options);
    }
    
    auto result = experiment.run(config);
    
    // Format and display results
    std::cout << std::endl;
    formatResults(result, options.outputFormat);
    
    return 0;
}