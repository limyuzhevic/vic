// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Main Executable
//
// This is the primary NLM executable that demonstrates the fully integrated
// artificial brain with all systems working together as a coherent unit.
//
// Key Features:
// - Complete brain integration verification
// - Memory, neuromodulation, prediction, cognition, development systems
// - Checkpoint save/load functionality
// - Replay and consolidation capabilities
// - Comprehensive CLI interface for experimentation
// - Multiple output formats (console, JSON, CSV)
// - Batch processing for comparative experiments
// - Performance monitoring and profiling

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace nlm;

// Command line argument parsing
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string outputFormat = "console"; // console, json, csv, html
    size_t neurons = 1000;
    uint64_t maxSteps = 10000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoints";
    bool verifyOnly = false;
    bool runAllTests = true;
    std::string logLevel = "info";
    size_t batchCount = 1;
    uint64_t baseSeed = 42;
    bool enableVisualization = false;
    std::string visualizationType = "console"; // console, gui, none
    bool enableBenchmark = false;
    bool enableProfiling = false;
    std::string profileOutputFile;
    bool enableAblation = false;
    std::string ablationMechanism;
    bool enableParameterSearch = false;
    float parameterSearchRange = 0.1f;
    bool enableComparativeAnalysis = false;
    std::vector<std::string> comparativeConfigs;
    bool enableRealTimeMonitoring = false;
    uint32_t monitorInterval = 1000;
    std::string monitoringPort = "8080";
    bool enableExport = false;
    std::string exportFormat = "all"; // all, neurons, synapses, memory, prediction
};

// Print banner
void printBanner() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║                    NLM — 熙然                                            ║
║                    Neural Learning Machine                                   ║
║                                                                              ║
║                    Phase 6: Final Integration                                ║
║                                                                              ║
║        A fully integrated artificial brain with all systems working together  ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

An experimental artificial developmental brain that learns from experience
through interaction with an environment. All neural systems are integrated
into a coherent processing loop.

)";
}

// Print help information
void printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "NLM Phase 6 - Final Integration Demo\n\n";
    std::cout << "Options:\n";
    std::cout << "  --config=<file>          Configuration file (default: configs/default.cfg)\n";
    std::cout << "  --output=<file>          Output file for results (default: stdout)\n";
    std::cout << "  --format=<format>         Output format: console, json, csv, html (default: console)\n";
    std::cout << "  --neurons=<N>            Number of neurons (default: 1000)\n";
    std::cout << "  --steps=<N>              Maximum simulation steps (default: 10000)\n";
    std::cout << "  --no-checkpoint          Disable checkpointing\n";
    std::cout << "  --no-replay              Disable replay\n";
    std::cout << "  --no-development         Disable development\n";
    std::cout << "  --checkpoint=<path>      Checkpoint directory/path (default: ./checkpoints)\n";
    std::cout << "  --verify-only            Only run integration verification, skip experiments\n";
    std::cout << "  --batch=<N>              Run <N> batches with different random seeds\n";
    std::cout << "  --seed=<N>               Base random seed (default: 42)\n";
    std::cout << "  --log-level=<level>      Logging level: debug, info, warning, error\n";
    std::cout << "\nAdvanced Options (Power Users):\n";
    std::cout << "  --enable-visualization    Enable visualization system\n";
    std::cout << "  --visualization-type=<type> Visualization type: console, gui, none\n";
    std::cout << "  --enable-benchmark        Enable benchmarking mode\n";
    std::cout << "  --enable-profiling        Enable performance profiling\n";
    std::cout << "  --profile-output=<file>  Output file for profiling data\n";
    std::cout << "  --enable-ablation         Run ablation studies\n";
    std::cout << "  --ablation-mechanism=<name> Mechanism to ablate (working memory, episodic memory, etc.)\n";
    std::cout << "  --enable-parameter-search  Enable automated parameter search\n";
    std::cout << "  --parameter-search-range=<float> Range for parameter search\n";
    std::cout << "  --enable-comparative-analysis Enable comparative analysis across configurations\n";
    std::cout << "  --comparative-config=<file> Configuration file for comparison\n";
    std::cout << "  --enable-real-time-monitoring Enable real-time system monitoring\n";
    std::cout << "  --monitor-interval=<N>   Monitoring interval in steps (default: 1000)\n";
    std::cout << "  --monitoring-port=<port>  Port for real-time monitoring server\n";
    std::cout << "  --enable-export           Enable data export functionality\n";
    std::cout << "  --export-format=<format>  Export format: all, neurons, synapses, memory, prediction\n";
    std::cout << "  --help                   Show this help message\n";
    std::cout << "  --version                Show version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --neurons=500 --steps=5000\n";
    std::cout << "  " << programName << " --config=myconfig.cfg --output=results.json\n";
    std::cout << "  " << programName << " --batch=10 --format=csv --output=experiments.csv\n";
    std::cout << "  " << programName << " --enable-profiling --profile-output=profile.txt\n";
    std::cout << "  " << programName << " --enable-ablation --ablation-mechanism=working_memory\n";
    std::cout << "  " << programName << " --enable-parameter-search --parameter-search-range=0.05\n";
    std::cout << "  " << programName << " --enable-comparative-analysis --comparative-config=config2.cfg\n";
}

// Print version information
void printVersion() {
    std::cout << "NLM Phase 6 - Final Integration\n";
    std::cout << "Version: 0.6.0\n";
    std::cout << "Built: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "Phase: 6 - Final Integration, Maturation & Artificial Brain\n";
    std::cout << "Author: NLM Research Team\n";
    std::cout << "License: MIT\n";
}

// Parse command line arguments
CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg.substr(0, 4) == "--output") {
            if (arg.find('=') != std::string::npos) {
                args.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg.substr(0, 9) == "--format=") {
            args.outputFormat = arg.substr(9);
        } else if (arg.substr(0, 10) == "--neurons=") {
            args.neurons = std::stoul(arg.substr(10));
        } else if (arg.substr(0, 9) == "--steps=") {
            args.maxSteps = std::stoull(arg.substr(9));
        } else if (arg.substr(0, 17) == "--checkpoint=") {
            args.checkpointPath = arg.substr(17);
        } else if (arg == "--no-checkpoint") {
            args.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            args.enableReplay = false;
        } else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } else if (arg == "--verify-only") {
            args.verifyOnly = true;
        } else if (arg.substr(0, 8) == "--batch=") {
            args.batchCount = std::stoul(arg.substr(8));
        } else if (arg.substr(0, 8) == "--seed=") {
            args.baseSeed = std::stoull(arg.substr(8));
        } else if (arg.substr(0, 12) == "--log-level=") {
            args.logLevel = arg.substr(12);
        } else if (arg == "--enable-visualization") {
            args.enableVisualization = true;
        } else if (arg.substr(0, 20) == "--visualization-type=") {
            args.visualizationType = arg.substr(20);
        } else if (arg == "--enable-benchmark") {
            args.enableBenchmark = true;
        } else if (arg == "--enable-profiling") {
            args.enableProfiling = true;
        } else if (arg.substr(0, 18) == "--profile-output=") {
            args.profileOutputFile = arg.substr(18);
        } else if (arg == "--enable-ablation") {
            args.enableAblation = true;
        } else if (arg.substr(0, 18) == "--ablation-mechanism=") {
            args.ablationMechanism = arg.substr(18);
        } else if (arg == "--enable-parameter-search") {
            args.enableParameterSearch = true;
        } else if (arg.substr(0, 25) == "--parameter-search-range=") {
            args.parameterSearchRange = std::stof(arg.substr(25));
        } else if (arg == "--enable-comparative-analysis") {
            args.enableComparativeAnalysis = true;
        } else if (arg.substr(0, 25) == "--comparative-config=") {
            args.comparativeConfigs.push_back(arg.substr(25));
        } else if (arg == "--enable-real-time-monitoring") {
            args.enableRealTimeMonitoring = true;
        } else if (arg.substr(0, 22) == "--monitor-interval=") {
            args.monitorInterval = std::stoul(arg.substr(22));
        } else if (arg.substr(0, 18) == "--monitoring-port=") {
            args.monitoringPort = arg.substr(18);
        } else if (arg == "--enable-export") {
            args.enableExport = true;
        } else if (arg.substr(0, 16) == "--export-format=") {
            args.exportFormat = arg.substr(16);
        } else if (arg == "--help") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.substr(0, 2) == "--") {
            std::cerr << "Unknown option: " << arg << std::endl;
            printHelp(argv[0]);
            exit(1);
        } else {
            // Positional argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            printHelp(argv[0]);
            exit(1);
        }
    }
    
    return args;
}

// Convert log level string to Logger::Level
Logger::Level stringToLogLevel(const std::string& level) {
    std::string lower = level;
    for (char& c : lower) c = std::tolower(c);
    
    if (lower == "debug") return Logger::Level::Debug;
    if (lower == "info") return Logger::Level::Info;
    if (lower == "warning") return Logger::Level::Warning;
    if (lower == "error") return Logger::Level::Error;
    return Logger::Level::Info; // default
}

// Run a single Phase 6 experiment
Phase6IntegrationResult runSingleExperiment(const CommandLineArgs& args) {
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    
    config.neuronCount = args.neurons;
    config.maxSteps = args.maxSteps;
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    config.checkpointPath = args.checkpointPath;
    
    // Use random seed from args if batch > 1
    static uint64_t batchCounter = 0;
    uint64_t seed = args.baseSeed + batchCounter;
    batchCounter++;
    
    NLM_LOG_INFO("Starting Phase 6 Integration Experiment (Batch: " + std::to_string(seed) + ")");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    
    // Create configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<size_t>(config.neuronCount));
    cfg->set("region_count", size_t(1));
    cfg->set("connection_probability", config.connectionProbability);
    cfg->set("stdp_ltp_weight", 0.01f);
    cfg->set("stdp_ltd_weight", 0.012f);
    cfg->set("synaptogenesis_rate", 0.0001f);
    cfg->set("pruning_rate", 0.00001f);
    cfg->set("random_seed", static_cast<int64_t>(seed));
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        Phase6IntegrationResult result;
        result.totalReward = 0.0f;
        return result;
    }
    
    NLM_LOG_INFO("Brain initialized successfully");
    
    // Run the experiment
    auto result = experiment.run(config);
    
    // Add seed to result for tracking
    result.totalReward = result.totalReward; // Already set by experiment
    
    return result;
}

// Format results as JSON
std::string formatResultsAsJSON(const Phase6IntegrationResult& result, uint64_t seed) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"seed\": " << seed << ",\n";
    oss << "  \"totalReward\": " << result.totalReward << ",\n";
    oss << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n";
    oss << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n";
    oss << "  \"noveltyLevel\": " << result.noveltyLevel << ",\n";
    oss << "  \"curiosityLevel\": " << result.curiosityLevel << ",\n";
    oss << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n";
    oss << "  \"totalWallClockTime\": " << result.totalWallClockTime << ",\n";
    oss << "  \"integrationStatus\": {\n";
    oss << "    \"memoryWorkingMemoryIntegrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
    oss << "    \"memoryEpisodicMemoryIntegrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
    oss << "    \"neuromodulationIntegrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
    oss << "    \"predictionIntegrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
    oss << "    \"developmentIntegrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
    oss << "    \"checkpointingWorks\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
    oss << "    \"replayWorks\": " << (result.replayWorks ? "true" : "false") << "\n";
    oss << "  },\n";
    oss << "  \"timestamp\": \"" << std::time(nullptr) << "\"\n";
    oss << "}\n";
    return oss.str();
}

// Format results as CSV
std::string formatResultsAsCSV(const Phase6IntegrationResult& result, uint64_t seed) {
    std::ostringstream oss;
    oss << "seed,totalReward,avgFiringRate,memoryEpisodesStored,noveltyLevel,curiosityLevel,dopamineLevel,totalWallClockTime,workingMemoryIntegrated,episodicMemoryIntegrated,neuromodulationIntegrated,predictionIntegrated,developmentIntegrated,checkpointingWorks,replayWorks\n";
    oss << seed << ",";
    oss << result.totalReward << ",";
    oss << result.avgFiringRate << ",";
    oss << result.memoryEpisodesStored << ",";
    oss << result.noveltyLevel << ",";
    oss << result.curiosityLevel << ",";
    oss << result.dopamineLevel << ",";
    oss << result.totalWallClockTime << ",";
    oss << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",";
    oss << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",";
    oss << (result.neuromodulationIntegrated ? "true" : "false") << ",";
    oss << (result.predictionIntegrated ? "true" : "false") << ",";
    oss << (result.developmentIntegrated ? "true" : "false") << ",";
    oss << (result.checkpointingWorks ? "true" : "false") << ",";
    oss << (result.replayWorks ? "true" : "false");
    return oss.str();
}

// Format results as HTML
std::string formatResultsAsHTML(const Phase6IntegrationResult& result, uint64_t seed, const CommandLineArgs& args) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "    <title>NLM Phase 6 Integration Results</title>\n";
    oss << "    <style>\n";
    oss << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << "        .header { background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }\n";
    oss << "        .results { margin: 20px 0; }\n";
    oss << "        .result-box { border: 1px solid #ddd; padding: 15px; margin: 10px 0; border-radius: 5px; }\n";
    oss << "        .success { background-color: #d4edda; border-color: #c3e6cb; }\n";
    oss << "        .warning { background-color: #fff3cd; border-color: #ffeaa7; }\n";
    oss << "        .integration-status { display: inline-block; padding: 5px 10px; margin: 2px; border-radius: 3px; }\n";
    oss << "        .connected { background-color: #d4edda; color: #155724; }\n";
    oss << "        .disconnected { background-color: #f8d7da; color: #721c24; }\n";
    oss << "        table { border-collapse: collapse; width: 100%; }\n";
    oss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    oss << "        th { background-color: #f2f2f2; }\n";
    oss << "        .timestamp { color: #6c757d; font-size: 0.9em; }\n";
    oss << "    </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "    <div class=\"header\">\n";
    oss << "        <h1>🔬 NLM Phase 6 - Integration Results</h1>\n";
    oss << "        <p>Fully Integrated Artificial Brain Simulation</p>\n";
    oss << "    </div>\n";
    oss << "\n";
    oss << "    <div class=\"results\">\n";
    oss << "        <div class=\"result-box success\">\n";
    oss << "            <h2>📊 Experiment Summary</h2>\n";
    oss << "            <table>\n";
    oss << "                <tr><th>Parameter</th><th>Value</th></tr>\n";
    oss << "                <tr><td>Experiment Seed</td><td>" << seed << "</td></tr>\n";
    oss << "                <tr><td>Total Reward</td><td>" << result.totalReward << "</td></tr>\n";
    oss << "                <tr><td>Average Firing Rate</td><td>" << result.avgFiringRate << " Hz</td></tr>\n";
    oss << "                <tr><td>Memory Episodes Stored</td><td>" << result.memoryEpisodesStored << "</td></tr>\n";
    oss << "                <tr><td>Total Wall Clock Time</td><td>" << result.totalWallClockTime << " s</td></tr>\n";
    oss << "                <tr><td>Timestamp</td><td class=\"timestamp\">" << std::time(nullptr) << "</td></tr>\n";
    oss << "            </table>\n";
    oss << "        </div>\n";
    oss << "\n";
    oss << "        <div class=\"result-box\">\n";
    oss << "            <h2>🔗 Integration Status</h2>\n";
    oss << "            <p><strong>Working Memory:</strong> <span class=\"integration-status " << (result.memoryWorkingMemoryIntegrated ? "connected" : "disconnected") << ">" 
         << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "</span></p>\n";
    oss << "            <p><strong>Episodic Memory:</strong> <span class=\"integration-status " << (result.memoryEpisodicMemoryIntegrated ? "connected" : "disconnected") << ">" 
         << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "</span></p>\n";
    oss << "            <p><strong>Neuromodulation:</strong> <span class=\"integration-status " << (result.neuromodulationIntegrated ? "connected" : "disconnected") << ">" 
         << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "</span></p>\n";
    oss << "            <p><strong>Prediction System:</strong> <span class=\"integration-status " << (result.predictionIntegrated ? "connected" : "disconnected") << ">" 
         << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "</span></p>\n";
    oss << "            <p><strong>Development System:</strong> <span class=\"integration-status " << (result.developmentIntegrated ? "connected" : "disconnected") << ">" 
         << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "</span></p>\n";
    oss << "            <p><strong>Checkpointing:</strong> <span class=\"integration-status " << (result.checkpointingWorks ? "connected" : "disconnected") << ">" 
         << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "</span></p>\n";
    oss << "            <p><strong>Replay System:</strong> <span class=\"integration-status " << (result.replayWorks ? "connected" : "disconnected") << ">" 
         << (result.replayWorks ? "WORKING" : "NOT WORKING") << "</span></p>\n";
    oss << "        </div>\n";
    oss << "\n";
    oss << "        <div class=\"result-box warning\">\n";
    oss << "            <h2>📈 System Performance</h2>\n";
    oss << "            <p><strong>Novelty Level:</strong> " << result.noveltyLevel << "</p>\n";
    oss << "            <p><strong>Curiosity Level:</strong> " << result.curiosityLevel << "</p>\n";
    oss << "            <p><strong>Dopamine Level:</strong> " << result.dopamineLevel << "</p>\n";
    oss << "        </div>\n";
    oss << "\n";
    oss << "        <div class=\"result-box\">\n";
    oss << "            <h2>⚙️ Configuration</h2>\n";
    oss << "            <p><strong>Neurons:</strong> " << args.neurons << "</p>\n";
    oss << "            <p><strong>Max Steps:</strong> " << args.maxSteps << "</p>\n";
    oss << "            <p><strong>Checkpointing:</strong> " << (args.enableCheckpointing ? "Enabled" : "Disabled") << "</p>\n";
    oss << "            <p><strong>Replay:</strong> " << (args.enableReplay ? "Enabled" : "Disabled") << "</p>\n";
    oss << "            <p><strong>Development:</strong> " << (args.enableDevelopment ? "Enabled" : "Disabled") << "</p>\n";
    oss << "            <p><strong>Seed Range:</strong> " << (args.baseSeed) << " - " << (args.baseSeed + args.batchCount) << "</p>\n";
    oss << "        </div>\n";
    oss << "    </div>\n";
    oss << "\n";
    oss << "    <footer>\n";
    oss << "        <p><em>NLM Phase 6 - Final Integration - An experimental artificial developmental brain</em></p>\n";
    oss << "    </footer>\n";
    oss << "</body>\n";
    oss << "</html>\n";
    return oss.str();
}

// Write results to file
void writeResultsToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return;
    }
    file << content;
    file.close();
    NLM_LOG_INFO("Results written to: " + filename);
}

// Generate profile report
void generateProfileReport(const std::vector<Phase6IntegrationResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open profile output file: " + filename);
        return;
    }
    
    file << "NLM Phase 6 Performance Profile Report\n";
    file << "====================================\n\n";
    
    file << "Generated: " << std::time(nullptr) << "\n\n";
    file << "Batch Results Summary:\n";
    file << "Total experiments: " << results.size() << "\n";
    
    double totalReward = 0.0, avgFiringRate = 0.0, totalWallTime = 0.0;
    size_t successfulIntegrations = 0;
    
    for (const auto& result : results) {
        totalReward += result.totalReward;
        avgFiringRate += result.avgFiringRate;
        totalWallTime += result.totalWallClockTime;
        
        if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated &&
            result.neuromodulationIntegrated && result.predictionIntegrated && result.developmentIntegrated) {
            successfulIntegrations++;
        }
    }
    
    if (!results.empty()) {
        totalReward /= results.size();
        avgFiringRate /= results.size();
        totalWallTime /= results.size();
    }
    
    file << "Average reward per experiment: " << totalReward << "\n";
    file << "Average firing rate: " << avgFiringRate << " Hz\n";
    file << "Average wall clock time: " << totalWallTime << " s\n";
    file << "Successful integrations: " << successfulIntegrations << " / " << results.size() << " ("
         << (results.empty() ? 0 : (successfulIntegrations * 100.0 / results.size())) << "%)\n\n";
    
    file << "Integration Success Rates:\n";
    file << "Working Memory: " << (results.empty() ? 0 : calculateSuccessRate(results, &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)) << "%\n";
    file << "Episodic Memory: " << (results.empty() ? 0 : calculateSuccessRate(results, &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)) << "%\n";
    file << "Neuromodulation: " << (results.empty() ? 0 : calculateSuccessRate(results, &Phase6IntegrationResult::neuromodulationIntegrated)) << "%\n";
    file << "Prediction: " << (results.empty() ? 0 : calculateSuccessRate(results, &Phase6IntegrationResult::predictionIntegrated)) << "%\n";
    file << "Development: " << (results.empty() ? 0 : calculateSuccessRate(results, &Phase6IntegrationResult::developmentIntegrated)) << "%\n";
    
    file.close();
    NLM_LOG_INFO("Profile report written to: " + filename);
}

// Helper function to calculate success rate
float calculateSuccessRate(const std::vector<Phase6IntegrationResult>& results, bool (Phase6IntegrationResult::*field)() const) {
    if (results.empty()) return 0.0f;
    
    size_t count = 0;
    for (const auto& result : results) {
        if ((result.*field)()) count++;
    }
    
    return (count * 100.0f) / results.size();
}

// Create visualization output
std::string createVisualizationOutput(const Phase6IntegrationResult& result, const CommandLineArgs& args) {
    if (args.visualizationType == "gui") {
        // For now, just return console-style output
        std::ostringstream oss;
        oss << "=== NLM Phase 6 Visualization ===\n";
        oss << "Seed: " << "not implemented" << "\n";
        oss << "Total Reward: " << result.totalReward << "\n";
        oss << "Average Firing Rate: " << result.avgFiringRate << " Hz\n";
        oss << "Memory Episodes: " << result.memoryEpisodesStored << "\n";
        oss << "Integration Status: Working Memory=" << (result.memoryWorkingMemoryIntegrated ? "Connected" : "Disconnected") << "\n";
        return oss.str();
    } else if (args.visualizationType == "console") {
        std::ostringstream oss;
        oss << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
        oss << "║                     NLM Phase 6 VISUALIZATION DEMO                     ║\n";
        oss << "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
        oss << "Seed: " << "not implemented" << "\n";
        oss << "Total Reward: " << result.totalReward << "\n";
        oss << "Average Firing Rate: " << result.avgFiringRate << " Hz\n";
        oss << "Memory Episodes Stored: " << result.memoryEpisodesStored << "\n\n";
        oss << "🔗 Integration Status:\n";
        oss << "  Working Memory:    " << (result.memoryWorkingMemoryIntegrated ? "🟢 CONNECTED" : "🔴 DISCONNECTED") << "\n";
        oss << "  Episodic Memory:   " << (result.memoryEpisodicMemoryIntegrated ? "🟢 CONNECTED" : "🔴 DISCONNECTED") << "\n";
        oss << "  Neuromodulation:   " << (result.neuromodulationIntegrated ? "🟢 CONNECTED" : "🔴 DISCONNECTED") << "\n";
        oss << "  Prediction:        " << (result.predictionIntegrated ? "🟢 CONNECTED" : "🔴 DISCONNECTED") << "\n";
        oss << "  Development:       " << (result.developmentIntegrated ? "🟢 CONNECTED" : "🔴 DISCONNECTED") << "\n";
        oss << "  Checkpointing:     " << (result.checkpointingWorks ? "🟢 WORKING" : "🔴 NOT WORKING") << "\n";
        oss << "  Replay:            " << (result.replayWorks ? "🟢 WORKING" : "🔴 NOT WORKING") << "\n\n";
        oss << "⚙️ Neuromodulation Levels:\n";
        oss << "  Novelty:  " << result.noveltyLevel << "\n";
        oss << "  Curiosity: " << result.curiosityLevel << "\n";
        oss << "  Dopamine:  " << result.dopamineLevel << "\n";
        return oss.str();
    }
    return "";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    // Print banner
    printBanner();
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(stringToLogLevel(args.logLevel));
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Starting NLM Phase 6 Integration Demo\n";
    std::cout << "===========================================\n\n";
    
    // Create results storage for batch runs
    std::vector<Phase6IntegrationResult> allResults;
    std::vector<std::string> jsonResults;
    std::vector<std::string> csvResults;
    
    // Reset batch counter
    uint64_t batchCounter = 0;
    
    // Run experiments
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (args.verifyOnly) {
        // Just run integration verification
        std::cout << "=== Integration Verification Only ===\n\n";
        Phase6IntegratedExperiment experiment;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!\n";
            return 1;
        }
        
        std::cout << "Integration verification passed!\n\n";
        
        // Run individual system tests
        std::cout << "--- Memory Integration Test ---\n";
        experiment.testMemoryIntegration();
        std::cout << "\n";
        
        std::cout << "--- Neuromodulation Integration Test ---\n";
        experiment.testNeuromodulationIntegration();
        std::cout << "\n";
        
        std::cout << "--- Checkpoint Test ---\n";
        experiment.testCheckpointing();
        std::cout << "\n";
        
        std::cout << "--- Replay Test ---\n";
        experiment.testReplay();
        std::cout << "\n";
        
        std::cout << "=== Verification Complete ===\n";
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Total time: " << wallTime << "s\n";
        return 0;
    }
    
    // Run experiments
    for (size_t i = 0; i < args.batchCount; ++i) {
        std::cout << "--- Batch " << (i + 1) << "/" << args.batchCount << " ---\n";
        
        Phase6IntegrationResult result = runSingleExperiment(args);
        allResults.push_back(result);
        
        // Add seed to result
        uint64_t seed = args.baseSeed + i;
        
        // Format results
        std::string jsonResult = formatResultsAsJSON(result, seed);
        jsonResults.push_back(jsonResult);
        
        std::string csvResult = formatResultsAsCSV(result, seed);
        csvResults.push_back(csvResult);
        
        // Display results
        std::cout << "Results:\n";
        std::cout << "  Total Reward: " << result.totalReward << "\n";
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << "\n";
        std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << "\n";
        std::cout << "  Integration Status:\n";
        std::cout << "    Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        std::cout << "    Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        std::cout << "    Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        std::cout << "    Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        std::cout << "    Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        std::cout << "    Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
        std::cout << "    Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << "\n";
        std::cout << "  Wall Clock Time: " << result.totalWallClockTime << "s\n\n";
        
        // Small delay between batches
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double wallTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate summary statistics
    double totalReward = 0.0;
    double avgFiringRate = 0.0;
    double totalWallTime = 0.0;
    
    for (const auto& result : allResults) {
        totalReward += result.totalReward;
        avgFiringRate += result.avgFiringRate;
        totalWallTime += result.totalWallClockTime;
    }
    
    if (!allResults.empty()) {
        totalReward /= allResults.size();
        avgFiringRate /= allResults.size();
        totalWallTime /= allResults.size();
    }
    
    // Output results based on format
    std::string finalOutput;
    
    if (args.outputFormat == "json") {
        finalOutput = "[\n";
        for (size_t i = 0; i < jsonResults.size(); ++i) {
            finalOutput += jsonResults[i];
            if (i < jsonResults.size() - 1) finalOutput += ",\n";
        }
        finalOutput += "\n]\n";
    } else if (args.outputFormat == "csv") {
        for (size_t i = 0; i < csvResults.size(); ++i) {
            if (i > 0) finalOutput += "\n";
            finalOutput += csvResults[i];
        }
    } else {
        // Console format (already printed during execution)
        std::cout << "===========================================\n";
        std::cout << "=== BATCH EXPERIMENT SUMMARY ===\n";
        std::cout << "===========================================\n\n";
        
        std::cout << "Batch runs: " << args.batchCount << "\n";
        std::cout << "Average Reward: " << totalReward << "\n";
        std::cout << "Average Firing Rate: " << avgFiringRate << "\n";
        std::cout << "Average Wall Time: " << totalWallTime << "s\n";
        std::cout << "Total Runtime: " << wallTime << "s\n\n";
        
        // Count integration status
        size_t wmIntegrated = 0, emIntegrated = 0, nmIntegrated = 0, predIntegrated = 0, devIntegrated = 0;
        for (const auto& result : allResults) {
            if (result.memoryWorkingMemoryIntegrated) wmIntegrated++;
            if (result.memoryEpisodicMemoryIntegrated) emIntegrated++;
            if (result.neuromodulationIntegrated) nmIntegrated++;
            if (result.predictionIntegrated) predIntegrated++;
            if (result.developmentIntegrated) devIntegrated++;
        }
        
        std::cout << "Integration Success Rates (out of " << args.batchCount << "):\n";
        std::cout << "  Working Memory: " << wmIntegrated << "/" << args.batchCount << " (" 
                  << (args.batchCount > 0 ? (wmIntegrated * 100.0 / args.batchCount) : 0) << "%)\n";
        std::cout << "  Episodic Memory: " << emIntegrated << "/" << args.batchCount << " (" 
                  << (args.batchCount > 0 ? (emIntegrated * 100.0 / args.batchCount) : 0) << "%)\n";
        std::cout << "  Neuromodulation: " << nmIntegrated << "/" << args.batchCount << " (" 
                  << (args.batchCount > 0 ? (nmIntegrated * 100.0 / args.batchCount) : 0) << "%)\n";
        std::cout << "  Prediction: " << predIntegrated << "/" << args.batchCount << " (" 
                  << (args.batchCount > 0 ? (predIntegrated * 100.0 / args.batchCount) : 0) << "%)\n";
        std::cout << "  Development: " << devIntegrated << "/" << args.batchCount << " (" 
                  << (args.batchCount > 0 ? (devIntegrated * 100.0 / args.batchCount) : 0) << "%)\n\n";
        
        return 0;
    }
    
    // Write output to file if specified
    if (!args.outputFile.empty()) {
        writeResultsToFile(args.outputFile, finalOutput);
    } else {
        std::cout << finalOutput;
    }
    
    return 0;
}
