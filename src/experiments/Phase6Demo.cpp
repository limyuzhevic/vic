/**
 * Phase 6 Demo - Enhanced Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected. Features an
 * enhanced command-line interface with configuration options,
 * progress tracking, and comprehensive reporting.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace nlm;

// Enhanced configuration parser for command-line arguments
class Phase6DemoConfig {
public:
    uint64_t maxSteps = 10000;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string environmentType = "SimpleWorld";
    size_t environmentSize = 16;
    std::string logLevel = "Info";
    bool verbose = false;
    bool benchmark = false;
    std::string outputFile = "";
    
    bool parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            } else if (arg == "--version") {
                printVersion();
                return false;
            } else if (arg.substr(0, 2) == "--") {
                parseOption(arg, (i + 1 < argc) ? argv[i + 1] : "");
                if (arg.find('=') != std::string::npos) {
                    i++; // Skip the value
                }
            } else if (arg[0] == '-') {
                parseShortOption(arg, (i + 1 < argc) ? argv[i + 1] : "");
                if (arg.length() == 2) { // -v, -h, etc.
                    i++; // Skip the value
                }
            }
        }
        return true;
    }
    
    bool parseOption(const std::string& option, const std::string& value) {
        size_t pos = option.find('=');
        if (pos == std::string::npos) {
            return false;
        }
        
        std::string key = option.substr(2, pos - 2);
        std::string val = value;
        
        if (key == "max-steps") maxSteps = std::stoull(val);
        else if (key == "neuron-count") neuronCount = std::stoul(val);
        else if (key == "region-count") regionCount = std::stoul(val);
        else if (key == "connection-prob") connectionProbability = std::stof(val);
        else if (key == "checkpoint-path") checkpointPath = val;
        else if (key == "env-type") environmentType = val;
        else if (key == "env-size") environmentSize = std::stoul(val);
        else if (key == "log-level") logLevel = val;
        else {
            std::cerr << "Unknown option: " << option << std::endl;
            return false;
        }
        return true;
    }
    
    bool parseShortOption(const std::string& option, const std::string& value) {
        if (option == "-v" || option == "--verbose") verbose = true;
        else if (option == "-b" || option == "--benchmark") benchmark = true;
        else if (option == "-o" || option == "--output") outputFile = value;
        else {
            std::cerr << "Unknown short option: " << option << std::endl;
            return false;
        }
        return true;
    }
    
    static void printHelp() {
        std::cout << "=== NLM Phase 6 Integration Demo ===\n\n";
        std::cout << "Usage: nlm_phase6_demo [OPTIONS]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h              Show this help message\n";
        std::cout << "  --version               Show version information\n";
        std::cout << "  --max-steps=N           Number of simulation steps (default: 10000)\n";
        std::cout << "  --neuron-count=N        Number of neurons (default: 1000)\n";
        std::cout << "  --region-count=N        Number of brain regions (default: 1)\n";
        std::cout << "  --connection-prob=N     Connection probability (default: 0.1)\n";
        std::cout << "  --checkpoint-path=PATH  Checkpoint file path (default: ./checkpoint_test.bin)\n";
        std::cout << "  --env-type=TYPE         Environment type (default: SimpleWorld)\n";
        std::cout << "  --env-size=N            Environment size (default: 16)\n";
        std::cout << "  --log-level=LEVEL       Logging level (Debug, Info, Warning, Error)\n";
        std::cout << "  -v, --verbose           Enable verbose output\n";
        std::cout << "  -b, --benchmark         Run in benchmark mode\n";
        std::cout << "  -o, --output=FILE       Save results to file\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm_phase6_demo --neuron-count=500 --max-steps=2000\n";
        std::cout << "  nlm_phase6_demo --env-type=GridWorld --env-size=20 --verbose\n";
        std::cout << "  nlm_phase6_demo --benchmark -o results.json\n";
    }
    
    static void printVersion() {
        std::cout << "NLM Phase 6 Integration Demo v1.0.0\n";
        std::cout << "Built for the Neural Learning Machine project\n";
        std::cout << "Phase 6: Final Integration of Artificial Brain Systems\n";
    }
    
    void validate() {
        if (maxSteps == 0) {
            std::cerr << "Error: max-steps must be greater than 0" << std::endl;
            exit(1);
        }
        if (neuronCount == 0) {
            std::cerr << "Error: neuron-count must be greater than 0" << std::endl;
            exit(1);
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            std::cerr << "Error: connection-prob must be between 0.0 and 1.0" << std::endl;
            exit(1);
        }
    }
    
    void printConfiguration() const {
        std::cout << "=== Phase 6 Demo Configuration ===\n";
        std::cout << "Max Steps: " << maxSteps << "\n";
        std::cout << "Neuron Count: " << neuronCount << "\n";
        std::cout << "Region Count: " << regionCount << "\n";
        std::cout << "Connection Probability: " << connectionProbability << "\n";
        std::cout << "Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << "\n";
        std::cout << "Replay: " << (enableReplay ? "Enabled" : "Disabled") << "\n";
        std::cout << "Development: " << (enableDevelopment ? "Enabled" : "Disabled") << "\n";
        std::cout << "Environment Type: " << environmentType << "\n";
        std::cout << "Environment Size: " << environmentSize << "\n";
        std::cout << "Output File: " << (outputFile.empty() ? "Not specified" : outputFile) << "\n";
        std::cout << "\n";
    }
    
    Phase6Config toPhase6Config() const {
        Phase6Config config;
        config.maxSteps = maxSteps;
        config.neuronCount = neuronCount;
        config.regionCount = regionCount;
        config.connectionProbability = connectionProbability;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        config.checkpointPath = checkpointPath;
        return config;
    }
};

// Progress bar utility
class ProgressBar {
public:
    static void show(float progress, const std::string& message = "Progress") {
        const int barWidth = 50;
        std::cout << "\r" << message << ": [";
        
        int pos = static_cast<int>(barWidth * progress);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        
        std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
        std::cout.flush();
        
        if (progress >= 1.0f) {
            std::cout << "\n";
        }
    }
};

// Enhanced result formatter
class ResultFormatter {
public:
    static std::string formatResults(const Phase6IntegrationResult& result) {
        std::ostringstream oss;
        oss << "=== Phase 6 Integration Results ===\n\n";
        oss << "Integration Quality: " << result.getIntegrationStatus() << "\n";
        oss << "Integration Score: " << (result.getIntegrationScore() * 100.0) << "%\n\n";
        
        oss << "=== System Integration Status ===\n";
        oss << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n\n";
        
        oss << "=== Performance Metrics ===\n";
        oss << "Total Reward: " << result.totalReward << "\n";
        oss << "Average Firing Rate: " << result.avgFiringRate << " Hz\n";
        oss << "Average Synaptic Weight: " << result.avgSynapticWeight << "\n";
        oss << "Memory Episodes Stored: " << result.memoryEpisodesStored << "\n";
        oss << "Novelty Level: " << result.noveltyLevel << "\n";
        oss << "Curiosity Level: " << result.curiosityLevel << "\n";
        oss << "Dopamine Level: " << result.dopamineLevel << "\n\n";
        
        oss << "=== System Statistics ===\n";
        oss << "Active Neurons: " << result.activeNeuronCount << "\n";
        oss << "Total Neurons: " << result.totalNeuronCount << "\n";
        oss << "Total Synapses: " << result.totalSynapseCount << "\n";
        oss << "Firing Neurons: " << result.firingNeuronCount << "\n";
        oss << "Working Memory Traces: " << result.workingMemoryActiveTraces << "\n";
        oss << "Episodic Memory Episodes: " << result.episodicMemoryEpisodeCount << "\n";
        oss << "Associative Memory Associations: " << result.associativeMemoryAssociationCount << "\n\n";
        
        oss << "=== Quality Metrics ===\n";
        oss << "Memory Coherence: " << result.memoryCoherence << "\n";
        oss << "Behavioral Consistency: " << result.behavioralConsistency << "\n";
        oss << "Learning Progress: " << result.learningProgress << "\n";
        oss << "Memory Retention: " << result.memoryRetention << "\n\n";
        
        oss << "=== Timing Information ===\n";
        oss << "Start Time: " << result.startTime << "\n";
        oss << "End Time: " << result.endTime << "\n";
        oss << "Wall Clock Time: " << result.totalWallClockTime << "s\n";
        if (result.maxSteps > 0) {
            oss << "Average Step Time: " << (result.totalWallClockTime / result.maxSteps) << "s\n";
        }
        
        return oss.str();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Enhanced neural brain system integration test" << std::endl;
    std::cout << "Building artificial brain with all systems integrated..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6DemoConfig config;
    if (!config.parseArgs(argc, argv)) {
        return 1;
    }
    
    // Validate configuration
    config.validate();
    
    // Initialize logging with specified level
    Logger::getInstance().setLevel(parseLogLevel(config.logLevel));
    
    if (config.verbose) {
        std::cout << "Verbose mode enabled.\n" << std::endl;
    }
    
    if (config.benchmark) {
        std::cout << "Benchmark mode enabled.\n" << std::endl;
    }
    
    // Display configuration
    config.printConfiguration();
    
    // Record start time for benchmarking
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "Running integration verification..." << std::endl;
    
    // First, run the quick integration verification
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems are not properly integrated." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems if verbose mode is enabled
    if (config.verbose) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
    }
    
    // Run checkpoint test if enabled
    if (config.enableCheckpointing) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        Phase6Config phase6Config = config.toPhase6Config();
        bool checkpointOK = experiment.testCheckpointing(phase6Config);
        if (!checkpointOK) {
            std::cout << "Warning: Checkpoint test failed (may be normal for some environments)" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Run replay test if enabled
    if (config.enableReplay) {
        std::cout << "--- Replay Test ---" << std::endl;
        Phase6Config phase6Config = config.toPhase6Config();
        bool replayOK = experiment.testReplay(phase6Config);
        if (!replayOK) {
            std::cout << "Warning: Replay test failed (may be normal for some environments)" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Run full integration experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config phase6Config = config.toPhase6Config();
    
    if (config.benchmark) {
        std::cout << "Running benchmark with " << phase6Config.maxSteps << " steps..." << std::endl;
    } else {
        std::cout << "Running integration test with " << phase6Config.maxSteps << " steps..." << std::endl;
    }
    
    auto experimentStart = std::chrono::high_resolution_clock::now();
    Phase6IntegrationResult result = experiment.run(phase6Config);
    auto experimentEnd = std::chrono::high_resolution_clock::now();
    
    double experimentDuration = std::chrono::duration<double>(experimentEnd - experimentStart).count();
    
    std::cout << std::endl;
    
    // Calculate derived metrics
    result.calculateDerivedMetrics();
    
    // Display results
    std::cout << ResultFormatter::formatResults(result) << std::endl;
    
    // Display performance summary
    std::cout << "=== Performance Summary ===" << std::endl;
    std::cout << "Experiment duration: " << experimentDuration << "s" << std::endl;
    std::cout << "Steps per second: " << (result.maxSteps / experimentDuration) << " Hz" << std::endl;
    std::cout << "Integration quality score: " << (result.getIntegrationScore() * 100.0) << "%" << std::endl;
    std::cout << "Overall integration status: " << result.getIntegrationStatus() << std::endl;
    std::cout << std::endl;
    
    // Save results if output file specified
    if (!config.outputFile.empty()) {
        std::cout << "Saving results to: " << config.outputFile << std::endl;
        if (Phase6IntegratedExperiment::saveResults(result, config.outputFile)) {
            std::cout << "Results saved successfully!" << std::endl;
        } else {
            std::cerr << "Error: Failed to save results to " << config.outputFile << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Final verdict
    std::cout << "=== FINAL VERDICT ===" << std::endl;
    if (result.getIntegrationScore() >= 0.8f) {
        std::cout << "✅ SUCCESS: The NLM brain systems are well integrated!" << std::endl;
        std::cout << "The artificial brain demonstrates proper functionality and integration." << std::endl;
    } else if (result.getIntegrationScore() >= 0.6f) {
        std::cout << "⚠️  PARTIAL SUCCESS: The NLM brain systems show reasonable integration." << std::endl;
        std::cout << "Some systems may need further development or tuning." << std::endl;
    } else {
        std::cout << "❌ FAILURE: The NLM brain systems are not properly integrated." << std::endl;
        std::cout << "Significant issues detected that require investigation." << std::endl;
    }
    
    return (result.getIntegrationScore() >= 0.6f) ? 0 : 1;
}

Logger::Level parseLogLevel(const std::string& levelStr) {
    std::string level = levelStr;
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);
    
    if (level == "debug") return Logger::Level::Debug;
    if (level == "info") return Logger::Level::Info;
    if (level == "warning") return Logger::Level::Warning;
    if (level == "error") return Logger::Level::Error;
    
    // Default to Info if invalid level specified
    return Logger::Level::Info;
}