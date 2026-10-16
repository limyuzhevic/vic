/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Features:
 * - Command-line argument parsing for experiment configuration
 * - Advanced configuration presets
 * - Better error handling and validation
 * - Performance monitoring
 * - Checkpoint management
 * - Multiple output formats
 * - Visualization support
 * - Advanced experiment configurations
 */

#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace nlm;

// Forward declarations
Phase6Config parseArguments(int argc, char* argv[]);
void printHelp();
void applyPreset(Phase6Config& config, const std::string& preset);
void applyAdvancedPreset(Phase6Config& config, const std::string& preset);
void validateConfig(const Phase6Config& config);
void runPerformanceBenchmarks(const Phase6Config& config);
void exportResults(const Phase6IntegrationResult& result, const Phase6Config& config, const std::string& format);
void generateVisualizationData(const Phase6IntegrationResult& result, const Phase6Config& config);
void runPhase6FullExperiment(Phase6IntegratedExperiment& experiment, const Phase6Config& config);

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    Phase6Config config = parseArguments(argc, argv);
    
    std::cout << "=== Phase 6 Integration Demo Configuration ===" << std::endl;
    std::cout << "Neuron count: " << config.neuronCount << std::endl;
    std::cout << "Max steps: " << config.maxSteps << std::endl;
    std::cout << "Region count: " << config.regionCount << std::endl;
    std::cout << "Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "Enable checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "Enable replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "Enable development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "Checkpoint path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
    
    // Run performance benchmarks
    runPerformanceBenchmarks(config);
    
    // Initialize visualization if requested
    VisualizationInterface viz;
    if (config.neuronCount >= 1000) { // Only enable visualization for larger networks
        std::cout << "Initializing visualization..." << std::endl;
        viz.initialize();
        viz.setUpdateRate(10.0); // 10 Hz update rate
    }
    
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
    
    // Only run checkpoint test if enabled
    if (config.enableCheckpointing) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
    }
    
    // Only run replay test if enabled
    if (config.enableReplay) {
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    auto result = experiment.run(config);
    
    // Generate visualization data if visualization is active
    if (viz.isActive()) {
        std::cout << "Generating visualization data..." << std::endl;
        generateVisualizationData(result, config);
        viz.visualizeNetwork(*experiment.getBrain()); // This would need proper implementation
        std::cout << "Visualization data generated." << std::endl;
    }
    
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
    
    // Export results in multiple formats
    std::cout << "=== Exporting Results ===" << std::endl;
    
    // Default JSON export
    exportResults(result, config, "json");
    
    // CSV export for data analysis
    exportResults(result, config, "csv");
    
    // XML export for interoperability
    exportResults(result, config, "xml");
    
    // Text summary
    exportResults(result, config, "text");
    
    std::cout << "=== All output formats generated successfully ===" << std::endl;
    
    // Close visualization if active
    if (viz.isActive()) {
        viz.close();
    }
    
    return 0;
}

// Parse command line arguments
Phase6Config parseArguments(int argc, char* argv[]) {
    Phase6Config config;
    
    // Default preset
    std::string preset = "medium";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        else if (arg == "--quick") {
            preset = "quick";
        }
        else if (arg == "--comprehensive") {
            preset = "comprehensive";
        }
        else if (arg == "--ultra" || arg == "--extreme") {
            preset = "extreme";
        }
        else if (arg == "--advanced") {
            preset = "advanced";
        }
        else if (arg.substr(0, 2) == "--") {
            std::string param = arg.substr(2);
            size_t equals = param.find('=');
            if (equals != std::string::npos) {
                std::string key = param.substr(0, equals);
                std::string value = param.substr(equals + 1);
                
                try {
                    if (key == "neurons") {
                        config.neuronCount = std::stoul(value);
                    }
                    else if (key == "steps") {
                        config.maxSteps = std::stoull(value);
                    }
                    else if (key == "regionCount") {
                        config.regionCount = std::stoul(value);
                    }
                    else if (key == "connectionProbability") {
                        config.connectionProbability = std::stof(value);
                    }
                    else if (key == "checkpointPath") {
                        config.checkpointPath = value;
                    }
                    else {
                        std::cerr << "Warning: Unknown parameter --" << key << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing value for --" << key << ": " << e.what() << std::endl;
                }
            } else {
                std::cerr << "Warning: Invalid parameter format: " << arg << std::endl;
            }
        } else {
            std::cerr << "Warning: Unrecognized argument: " << arg << std::endl;
        }
    }
    
    // Apply preset
    applyPreset(config, preset);
    
    // Validate configuration
    validateConfig(config);
    
    return config;
}

// Print help information
void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  --quick                 Quick test with minimal neurons (200, 500 steps)" << std::endl;
    std::cout << "  --comprehensive         Standard test (500 neurons, 2000 steps)" << std::endl;
    std::cout << "  --advanced              Advanced test with full features (1000 neurons)" << std::endl;
    std::cout << "  --extreme, --ultra      Full integration test (2000 neurons, 10000 steps)" << std::endl;
    std::cout << "  --neurons=N             Set neuron count" << std::endl;
    std::cout << "  --steps=N               Set max simulation steps" << std::endl;
    std::cout << "  --regionCount=N         Set number of brain regions" << std::endl;
    std::cout << "  --connectionProbability=V Set connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --checkpointPath=PATH    Set checkpoint file path" << std::endl;
    std::cout << std::endl;
    std::cout << "Preset examples:" << std::endl;
    std::cout << "  --quick                 Fast integration verification" << std::endl;
    std::cout << "  --comprehensive         Default balanced test" << std::endl;
    std::cout << "  --advanced              Advanced test with full features" << std::endl;
    std::cout << "  --extreme               Maximum test coverage" << std::endl;
    std::cout << std::endl;
}

// Apply standard configuration preset
void applyPreset(Phase6Config& config, const std::string& preset) {
    if (preset == "quick") {
        config.neuronCount = 200;
        config.maxSteps = 500;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
    }
    else if (preset == "extreme" || preset == "ultra") {
        config.neuronCount = 2000;
        config.maxSteps = 10000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.regionCount = 2;
        config.connectionProbability = 0.05f;
    }
    else if (preset == "advanced") {
        config.neuronCount = 1000;
        config.maxSteps = 5000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.regionCount = 2;
        config.connectionProbability = 0.1f;
    }
    else { // medium or default
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
    }
}

// Apply advanced configuration preset
void applyAdvancedPreset(Phase6Config& config, const std::string& preset) {
    if (preset == "quick") {
        // Enhanced quick preset with additional features
        config.neuronCount = 200;
        config.maxSteps = 500;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableCuriosity = false;
        config.enableNovelty = false;
        config.enablePrediction = false;
    }
    else if (preset == "comprehensive") {
        // Comprehensive preset with all core features
        config.neuronCount = 800;
        config.maxSteps = 3000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.enableCuriosity = true;
        config.enableNovelty = true;
        config.enablePrediction = true;
        config.enablePlanning = true;
        config.regionCount = 2;
        config.connectionProbability = 0.08f;
        config.neuronTypes = 4; // Including sensory, motor, association neurons
    }
    else if (preset == "extreme") {
        // Extreme preset with maximum features
        config.neuronCount = 3000;
        config.maxSteps = 15000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.enableCuriosity = true;
        config.enableNovelty = true;
        config.enablePrediction = true;
        config.enablePlanning = true;
        config.enableConceptFormation = true;
        config.enableAttention = true;
        config.regionCount = 4;
        config.connectionProbability = 0.03f;
        config.neuronTypes = 6;
        config.stdpLtpWeight = 0.08f;
        config.stdpLtdWeight = 0.06f;
        config.stdpTau = 8.0f;
    }
    else { // default advanced
        config.neuronCount = 1500;
        config.maxSteps = 8000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.enableCuriosity = true;
        config.enableNovelty = true;
        config.enablePrediction = true;
        config.enablePlanning = true;
        config.regionCount = 3;
        config.connectionProbability = 0.06f;
        config.neuronTypes = 4;
    }
}

// Validate configuration
void validateConfig(const Phase6Config& config) {
    if (config.neuronCount < 100) {
        std::cerr << "Warning: Very low neuron count may not properly test all systems" << std::endl;
    }
    if (config.maxSteps < 100) {
        std::cerr << "Warning: Very low step count may not complete integration testing" << std::endl;
    }
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        std::cerr << "Error: Connection probability must be between 0.0 and 1.0" << std::endl;
    }
    if (config.checkpointPath.empty()) {
        std::cerr << "Error: Checkpoint path cannot be empty" << std::endl;
    }
}

// Run performance benchmarks
void runPerformanceBenchmarks(const Phase6Config& config) {
    std::cout << "=== Performance Benchmarks ===" << std::endl;
    
    // Simple performance metrics
    double stepsPerSecond = static_cast<double>(config.maxSteps) / 10.0; // Assume 10 seconds
    size_t memoryFootprint = config.neuronCount * 100; // Approximate memory per neuron
    
    std::cout << "  Simulated performance metrics:" << std::endl;
    std::cout << "    Estimated steps per second: " << stepsPerSecond << std::endl;
    std::cout << "    Estimated memory footprint: " << memoryFootprint / 1024 << " KB" << std::endl;
    std::cout << "    System integration complexity: High" << std::endl;
    
    // Additional benchmarks for advanced configurations
    if (config.neuronCount >= 1000) {
        std::cout << "    Multi-threading potential: Enabled" << std::endl;
        std::cout << "    Parallel processing: Recommended for best performance" << std::endl;
    }
    
    std::cout << std::endl;
}

// Export results in multiple formats
void exportResults(const Phase6IntegrationResult& result, const Phase6Config& config, const std::string& format) {
    std::cout << "  Exporting results in " << format << " format..." << std::endl;
    
    std::string filename = "nlm_phase6_results." + format;
    
    if (format == "json") {
        // JSON export
        std::ofstream file(filename);
        file << "{\n";
        file << "  \"totalReward\": " << result.totalReward << ",\n";
        file << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n";
        file << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n";
        file << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n";
        file << "  \"configuration\": {\n";
        file << "    \"neuronCount\": " << config.neuronCount << ",\n";
        file << "    \"maxSteps\": " << config.maxSteps << ",\n";
        file << "    \"regionCount\": " << config.regionCount << ",\n";
        file << "    \"connectionProbability\": " << config.connectionProbability << "\n";
        file << "  }\n";
        file << "}\n";
        file.close();
    }
    else if (format == "csv") {
        // CSV export
        std::ofstream file(filename);
        file << "metric,value\n";
        file << "total_reward," << result.totalReward << "\n";
        file << "avg_firing_rate," << result.avgFiringRate << "\n";
        file << "memory_episodes_stored," << result.memoryEpisodesStored << "\n";
        file << "dopamine_level," << result.dopamineLevel << "\n";
        file << "neuron_count," << config.neuronCount << "\n";
        file << "max_steps," << config.maxSteps << "\n";
        file.close();
    }
    else if (format == "xml") {
        // XML export
        std::ofstream file(filename);
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<results>\n";
        file << "  <experiment>Phase 6 Integration</experiment>\n";
        file << "  <timestamp>" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "</timestamp>\n";
        file << "  <results>\n";
        file << "    <totalReward>" << result.totalReward << "</totalReward>\n";
        file << "    <avgFiringRate>" << result.avgFiringRate << "</avgFiringRate>\n";
        file << "    <memoryEpisodesStored>" << result.memoryEpisodesStored << "</memoryEpisodesStored>\n";
        file << "    <dopamineLevel>" << result.dopamineLevel << "</dopamineLevel>\n";
        file << "  </results>\n";
        file << "  <configuration>\n";
        file << "    <neuronCount>" << config.neuronCount << "</neuronCount>\n";
        file << "    <maxSteps>" << config.maxSteps << "</maxSteps>\n";
        file << "    <regionCount>" << config.regionCount << "</regionCount>\n";
        file << "    <connectionProbability>" << config.connectionProbability << "</connectionProbability>\n";
        file << "  </configuration>\n";
        file << "</results>\n";
        file.close();
    }
    else { // text format
        // Text export
        std::ofstream file(filename);
        file << "NLM Phase 6 Integration Results" << std::endl;
        file << "=====================================" << std::endl;
        file << "Experiment: Phase 6 Integration Test" << std::endl;
        file << "Date: " << std::put_time(std::localtime(&result.startTime), "%Y-%m-%d %H:%M:%S") << std::endl;
        file << "Total Reward: " << result.totalReward << std::endl;
        file << "Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        file << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        file << "Dopamine Level: " << result.dopamineLevel << std::endl;
        file << "Wall Clock Time: " << result.totalWallClockTime << " seconds" << std::endl;
        file.close();
    }
    
    std::cout << "    Exported to: " << filename << std::endl;
}

// Generate visualization data
void generateVisualizationData(const Phase6IntegrationResult& result, const Phase6Config& config) {
    std::cout << "  Generating visualization data..." << std::endl;
    
    // In a real implementation, this would:
    // 1. Extract spike trains from brain simulation
    // 2. Calculate neural activity patterns
    // 3. Generate visualization datasets (JSON, CSV, etc.)
    // 4. Create network graphs for visualization
    
    std::string vizDataFile = "nlm_phase6_viz_data.json";
    std::ofstream file(vizDataFile);
    
    file << "{\n";
    file << "  \"spikeTrains\": [],\n";
    file << "  \"neuralActivity\": {\n";
    file << "    \"firingRates\": [],\n";
    file << "    \"networkTopology\": \"unstructured\",\n";
    file << "    \"regionCount\": " << config.regionCount << "\n";
    file << "  },\n";
    file << "  \"simulationParameters\": {\n";
    file << "    \"neuronCount\": " << config.neuronCount << ",\n";
    file << "    \"maxSteps\": " << config.maxSteps << ",\n";
    file << "    \"connectionProbability\": " << config.connectionProbability << "\n";
    file << "  },\n";
    file << "  \"results\": {\n";
    file << "    \"totalReward\": " << result.totalReward << ",\n";
    file << "    \"avgFiringRate\": " << result.avgFiringRate << ",\n";
    file << "    \"memoryEpisodesStored\": " << result.memoryEpisodesStored << "\n";
    file << "  }\n";
    file << "}\n";
    file.close();
    
    std::cout << "    Visualization data exported to: " << vizDataFile << std::endl;
}

void runPhase6FullExperiment(Phase6IntegratedExperiment& experiment, const Phase6Config& config) {
    // This function runs the full Phase 6 experiment with enhanced features
    // It includes additional monitoring and advanced analysis
    std::cout << "Running enhanced Phase 6 full experiment..." << std::endl;
    
    // Record start time
    time_t startTime = time(nullptr);
    
    // Run the experiment
    auto result = experiment.run(config);
    
    // Record end time
    time_t endTime = time(nullptr);
    double wallClockTime = std::difftime(endTime, startTime);
    
    // Enhanced analysis
    std::cout << "=== Enhanced Analysis ===" << std::endl;
    std::cout << "  Integration Score: " << calculateIntegrationScore(result) << "/100" << std::endl;
    std::cout << "  Learning Efficiency: " << (result.totalReward / config.maxSteps) * 100 << "%" << std::endl;
    std::cout << "  Memory Consolidation: " << (result.memoryEpisodesStored / (config.maxSteps / 100)) << " episodes per 100 steps" << std::endl;
    std::cout << "  Neuromodulation Stability: " << (result.dopamineLevel / 2.0) * 100 << "% of baseline" << std::endl;
}

// Helper function to calculate integration score
double calculateIntegrationScore(const Phase6IntegrationResult& result) {
    double score = 0.0;
    
    // Weight for different systems
    const double MEMORY_WEIGHT = 0.25;
    const double NEUROMODULATION_WEIGHT = 0.25;
    const double PREDICTION_WEIGHT = 0.20;
    const double DEVELOPMENT_WEIGHT = 0.15;
    const double CHECKPOINT_WEIGHT = 0.15;
    
    // Memory integration
    score += (result.memoryWorkingMemoryIntegrated ? 1.0 : 0.0) * MEMORY_WEIGHT;
    score += (result.memoryEpisodicMemoryIntegrated ? 1.0 : 0.0) * MEMORY_WEIGHT;
    
    // Neuromodulation integration
    score += (result.neuromodulationIntegrated ? 1.0 : 0.0) * NEUROMODULATION_WEIGHT;
    
    // Prediction integration
    score += (result.predictionIntegrated ? 1.0 : 0.0) * PREDICTION_WEIGHT;
    
    // Development integration
    score += (result.developmentIntegrated ? 1.0 : 0.0) * DEVELOPMENT_WEIGHT;
    
    // Checkpointing
    score += (result.checkpointingWorks ? 1.0 : 0.0) * CHECKPOINT_WEIGHT;
    
    return score * 100.0;
}