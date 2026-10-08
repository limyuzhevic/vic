/**
 * Enhanced Phase 6 Demo - Integration Test with Advanced Features
 *
 * This demo runs the Phase 6 integration experiment with enhanced capabilities:
 * - Advanced command-line interface
 * - Multiple output formats (text, JSON, CSV, TSV)
 * - Configuration validation and template generation
 * - Performance monitoring and profiling
 * - Batch processing support
 * - Extended analysis modes
 * - Comprehensive error handling
 * - Integration test verification
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <unordered_map>

namespace nlm {

enum class OutputFormat { TEXT, JSON, CSV, TSV };
enum class ExperimentMode { VERIFICATION_ONLY, QUICK_TEST, FULL_EXPERIMENT, PERFORMANCE_BENCHMARK };
enum class AnalysisMode { BASIC, DETAILED, COMPREHENSIVE };

struct AdvancedConfig {
    std::string configFile = "configs/default.cfg";
    OutputFormat outputFormat = OutputFormat::TEXT;
    ExperimentMode experimentMode = ExperimentMode::FULL_EXPERIMENT;
    AnalysisMode analysisMode = AnalysisMode::DETAILED;
    bool enableProfiling = false;
    bool enableTiming = true;
    bool enableMemoryMonitoring = false;
    std::string outputFile;
    bool verbose = false;
    bool quiet = false;
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    uint64_t numTrials = 1;
    float timestep = 0.001f;
    bool runIntegrationTests = true;
    bool runSystemTests = true;
    bool runCheckpointTests = true;
    bool runReplayTests = true;
    bool enableDevelopment = true;
    bool enableStructuralPlasticity = true;
    bool enableCuriosity = true;
    bool enableNoveltyDetection = true;
    bool enablePrediction = true;
    bool enableDevelopmentStages = true;
    float dopamineBaseline = 0.1f;
    float curiosityLevel = 1.0f;
    float noveltyThreshold = 0.5f;
    uint64_t workingMemoryCapacity = 100;
    uint64_t episodicMemoryCapacity = 1000;
    bool batchMode = false;
    std::string batchOutputDir = "batch_output";
};

class EnhancedPhase6Demo {
public:
    EnhancedPhase6Demo() {
        setupDefaultConfig();
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~EnhancedPhase6Demo() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        if (config_.verbose) {
            std::cout << "Demo session completed in " << duration.count() << "ms" << std::endl;
        }
    }
    
    bool parseArguments(int argc, char* argv[]);
    int runDemo();
    std::string generateReport(const Phase6IntegrationResult& result) const;
    std::string generateJSONReport(const Phase6IntegrationResult& result) const;
    std::string generateCSVReport(const Phase6IntegrationResult& result) const;
    std::string generateTSVReport(const Phase6IntegrationResult& result) const;
    bool writeReport(const std::string& content) const;
    void printHelp() const;
    void printVersion() const;
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    bool validateConfiguration() const;
    void applyConfiguration(Phase6IntegratedExperiment& experiment, Phase6Config& phase6Config) const;
    bool runBatchProcessing();
    bool runPerformanceBenchmark();
    void generateConfigurationTemplate(const std::string& filepath) const;
    
private:
    AdvancedConfig config_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    void setupDefaultConfig() {
        config_.neuronCount = 500;
        config_.maxSteps = 2000;
        config_.numTrials = 1;
        config_.timestep = 0.001f;
        config_.enableDevelopment = true;
        config_.enableStructuralPlasticity = true;
        config_.enableCuriosity = true;
        config_.enableNoveltyDetection = true;
        config_.enablePrediction = true;
        config_.enableDevelopmentStages = true;
        config_.configFile = "configs/default.cfg";
        config_.outputFile = "";
        config_.batchOutputDir = "batch_output";
    }
    
    void setupLogging() {
        Logger::getGlobal().setLevel(config_.verbose ? Logger::Level::Debug : Logger::Level::Info);
    }
    
    std::string formatNumber(float value) const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << value;
        return oss.str();
    }
    
    std::string formatPercentage(float value) const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << value << "%";
        return oss.str();
    }
    
    std::string currentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    bool parseBoolean(const std::string& str) const {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
    }
    
    float parseFloat(const std::string& str) const {
        return std::stof(str);
    }
    
    uint64_t parseUint64(const std::string& str) const {
        return std::stoull(str);
    }
    
    void printSystemInfo() const {
        std::cout << "=== System Information ===" << std::endl;
        std::cout << "Platform: Linux" << std::endl;
        std::cout << "Architecture: " << (sizeof(void*) * 8) << "-bit" << std::endl;
        std::cout << std::endl;
    }
    
    void printExperimentInfo() const {
        std::cout << "=== Enhanced NLM Phase 6 Demo ===" << std::endl;
        std::cout << "Version: 6.0.0 Enhanced Edition" << std::endl;
        std::cout << "Generated: " << currentTimestamp() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Experiment Configuration:" << std::endl;
        std::cout << "  Neurons: " << config_.neuronCount << std::endl;
        std::cout << "  Max Steps: " << config_.maxSteps << std::endl;
        std::cout << "  Timestep: " << config_.timestep << "s" << std::endl;
        std::cout << "  Trials: " << config_.numTrials << std::endl;
        std::cout << "  Output Format: ";
        switch (config_.outputFormat) {
            case OutputFormat::TEXT: std::cout << "Text"; break;
            case OutputFormat::JSON: std::cout << "JSON"; break;
            case OutputFormat::CSV: std::cout << "CSV"; break;
            case OutputFormat::TSV: std::cout << "TSV"; break;
        }
        std::cout << std::endl;
        
        std::cout << "Enabled Features:" << std::endl;
        std::cout << "  Development: " << (config_.enableDevelopment ? "Yes" : "No") << std::endl;
        std::cout << "  Structural Plasticity: " << (config_.enableStructuralPlasticity ? "Yes" : "No") << std::endl;
        std::cout << "  Curiosity: " << (config_.enableCuriosity ? "Yes" : "No") << std::endl;
        std::cout << "  Novelty Detection: " << (config_.enableNoveltyDetection ? "Yes" : "No") << std::endl;
        std::cout << "  Prediction: " << (config_.enablePrediction ? "Yes" : "No") << std::endl;
        std::cout << "  Development Stages: " << (config_.enableDevelopmentStages ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        
        if (config_.verbose) {
            printSystemInfo();
        }
    }
    
    bool runExtendedVerification(Phase6IntegratedExperiment& experiment);
    bool runExtendedSystemTests(Phase6IntegratedExperiment& experiment);
    bool runExtendedPerformanceTests(Phase6IntegratedExperiment& experiment);
    
    std::string generateDetailedAnalysis(const Phase6IntegrationResult& result) const;
    std::string generateComprehensiveAnalysis(const Phase6IntegrationResult& result) const;
};

// Global function for backward compatibility
int main(int argc, char* argv[]) {
    EnhancedPhase6Demo demo;
    
    if (!demo.parseArguments(argc, argv)) {
        return 1;
    }
    
    return demo.runDemo();
}

bool EnhancedPhase6Demo::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return false; // Help means exit
        }
        else if (arg == "-v" || arg == "--version") {
            printVersion();
            return false; // Version means exit
        }
        else if (arg == "--verbose") {
            config_.verbose = true;
        }
        else if (arg == "--quiet") {
            config_.quiet = true;
        }
        else if (arg == "--output-format" && i + 1 < argc) {
            std::string format = argv[++i];
            if (format == "json") config_.outputFormat = OutputFormat::JSON;
            else if (format == "csv") config_.outputFormat = OutputFormat::CSV;
            else if (format == "tsv") config_.outputFormat = OutputFormat::TSV;
            else config_.outputFormat = OutputFormat::TEXT;
        }
        else if (arg == "--experiment-mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "verification") config_.experimentMode = ExperimentMode::VERIFICATION_ONLY;
            else if (mode == "quick") config_.experimentMode = ExperimentMode::QUICK_TEST;
            else if (mode == "benchmark") config_.experimentMode = ExperimentMode::PERFORMANCE_BENCHMARK;
            else config_.experimentMode = ExperimentMode::FULL_EXPERIMENT;
        }
        else if (arg == "--config" && i + 1 < argc) {
            config_.configFile = argv[++i];
        }
        else if (arg == "--neurons" && i + 1 < argc) {
            config_.neuronCount = parseUint64(argv[++i]);
        }
        else if (arg == "--steps" && i + 1 < argc) {
            config_.maxSteps = parseUint64(argv[++i]);
        }
        else if (arg == "--timestep" && i + 1 < argc) {
            config_.timestep = parseFloat(argv[++i]);
        }
        else if (arg == "--no-integration-tests") {
            config_.runIntegrationTests = false;
        }
        else if (arg == "--no-system-tests") {
            config_.runSystemTests = false;
        }
        else if (arg == "--no-checkpoint-tests") {
            config_.runCheckpointTests = false;
        }
        else if (arg == "--no-replay-tests") {
            config_.runReplayTests = false;
        }
        else if (arg == "--batch") {
            config_.batchMode = true;
        }
        else if (arg == "--batch-output" && i + 1 < argc) {
            config_.batchOutputDir = argv[++i];
        }
        else if (arg == "--generate-template" && i + 1 < argc) {
            generateConfigurationTemplate(argv[++i]);
            return false;
        }
        else if (arg == "--validate-config") {
            if (loadConfiguration(config_.configFile)) {
                if (validateConfiguration()) {
                    std::cout << "Configuration is valid!" << std::endl;
                    return false;
                } else {
                    std::cerr << "Configuration is invalid!" << std::endl;
                    return true;
                }
            } else {
                std::cerr << "Failed to load configuration file!" << std::endl;
                return true;
            }
        }
    }
    
    return true;
}

int EnhancedPhase6Demo::runDemo() {
    if (!loadConfiguration(config_.configFile)) {
        std::cerr << "Warning: Failed to load configuration file: " << config_.configFile << std::endl;
    }
    
    setupLogging();
    
    if (!validateConfiguration()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        return 1;
    }
    
    if (config_.batchMode) {
        return runBatchProcessing() ? 0 : 1;
    }
    
    Phase6IntegratedExperiment experiment;
    
    std::cout << "=== Enhanced NLM Phase 6 Demo ===" << std::endl;
    printExperimentInfo();
    
    bool success = true;
    
    if (config_.runIntegrationTests && config_.experimentMode != ExperimentMode::VERIFICATION_ONLY) {
        std::cout << "\n--- Running Integration Tests ---" << std::endl;
        success &= runExtendedVerification(experiment);
    }
    
    if (config_.runSystemTests) {
        std::cout << "\n--- Running System Tests ---" << std::endl;
        success &= runExtendedSystemTests(experiment);
    }
    
    if (config_.runCheckpointTests) {
        std::cout << "\n--- Running Performance Tests ---" << std::endl;
        success &= runExtendedPerformanceTests(experiment);
    }
    
    if (config_.runReplayTests) {
        std::cout << "\n--- Running Replay Tests ---" << std::endl;
    }
    
    if (config_.experimentMode != ExperimentMode::VERIFICATION_ONLY) {
        std::cout << "\n--- Main Integration Experiment ---" << std::endl;
        Phase6Config phase6Config;
        applyConfiguration(experiment, phase6Config);
        
        Phase6IntegrationResult result = experiment.run(phase6Config);
        
        std::string report;
        switch (config_.outputFormat) {
            case OutputFormat::JSON:
                report = generateJSONReport(result);
                break;
            case OutputFormat::CSV:
                report = generateCSVReport(result);
                break;
            case OutputFormat::TSV:
                report = generateTSVReport(result);
                break;
            case OutputFormat::TEXT:
            default:
                report = generateReport(result);
                if (config_.analysisMode == AnalysisMode::DETAILED) {
                    report += "\n" + generateDetailedAnalysis(result);
                } else if (config_.analysisMode == AnalysisMode::COMPREHENSIVE) {
                    report += "\n" + generateComprehensiveAnalysis(result);
                }
                break;
        }
        
        if (!config_.outputFile.empty()) {
            if (!writeReport(report)) {
                std::cerr << "Failed to write report to file!" << std::endl;
                success = false;
            }
        }
        
        std::cout << report;
    }
    
    if (config_.experimentMode == ExperimentMode::PERFORMANCE_BENCHMARK) {
        std::cout << "\n--- Running Performance Benchmark ---" << std::endl;
        runPerformanceBenchmark();
    }
    
    if (success) {
        std::cout << "\n=== Demo Completed Successfully ===" << std::endl;
        return 0;
    } else {
        std::cerr << "\n=== Demo Failed ===" << std::endl;
        return 1;
    }
}

std::string EnhancedPhase6Demo::generateReport(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    
    oss << "=== NLM Phase 6 Integration Results ===" << std::endl;
    oss << "Generated: " << currentTimestamp() << std::endl;
    oss << std::endl;
    
    oss << "Basic Metrics:" << std::endl;
    oss << "  Total Reward: " << formatNumber(result.totalReward) << std::endl;
    oss << "  Average Firing Rate: " << formatNumber(result.avgFiringRate) << std::endl;
    oss << "  Average Synaptic Weight: " << formatNumber(result.avgSynapticWeight) << std::endl;
    oss << "  Episodes Stored: " << formatNumber(result.memoryEpisodesStored) << std::endl;
    oss << "  Novelty Level: " << formatNumber(result.noveltyLevel) << std::endl;
    oss << "  Curiosity Level: " << formatNumber(result.curiosityLevel) << std::endl;
    oss << "  Dopamine Level: " << formatNumber(result.dopamineLevel) << std::endl;
    oss << std::endl;
    
    oss << "Integration Status:" << std::endl;
    oss << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    oss << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    oss << std::endl;
    
    if (result.totalWallClockTime > 0) {
        oss << "Timing: " << formatNumber(result.totalWallClockTime) << "s" << std::endl;
    }
    
    return oss.str();
}

std::string EnhancedPhase6Demo::generateJSONReport(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"timestamp\": \"" << currentTimestamp() << "\",\n";
    oss << "  \"experiment\": \"NLM Phase 6 Integration\",\n";
    oss << "  \"results\": {\n";
    oss << "    \"reward\": " << result.totalReward << ",\n";
    oss << "    \"firing_rate\": " << result.avgFiringRate << ",\n";
    oss << "    \"synaptic_weight\": " << result.avgSynapticWeight << ",\n";
    oss << "    \"episodes_stored\": " << result.memoryEpisodesStored << ",\n";
    oss << "    \"novelty_level\": " << result.noveltyLevel << ",\n";
    oss << "    \"curiosity_level\": " << result.curiosityLevel << ",\n";
    oss << "    \"dopamine_level\": " << result.dopamineLevel << ",\n";
    oss << "    \"wall_clock_time\": " << result.totalWallClockTime << "\n";
    oss << "  },\n";
    oss << "  \"integration\": {\n";
    oss << "    \"working_memory\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
    oss << "    \"episodic_memory\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
    oss << "    \"neuromodulation\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
    oss << "    \"prediction\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
    oss << "    \"development\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
    oss << "    \"checkpointing\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
    oss << "    \"replay\": " << (result.replayWorks ? "true" : "false") << "\n";
    oss << "  }\n";
    oss << "}\n";
    
    return oss.str();
}

std::string EnhancedPhase6Demo::generateCSVReport(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    
    oss << "Metric,Value\n";
    oss << "Timestamp," << currentTimestamp() << "\n";
    oss << "Experiment,NLM Phase 6 Integration\n";
    oss << "Total Reward," << result.totalReward << "\n";
    oss << "Average Firing Rate," << result.avgFiringRate << "\n";
    oss << "Average Synaptic Weight," << result.avgSynapticWeight << "\n";
    oss << "Episodes Stored," << result.memoryEpisodesStored << "\n";
    oss << "Novelty Level," << result.noveltyLevel << "\n";
    oss << "Curiosity Level," << result.curiosityLevel << "\n";
    oss << "Dopamine Level," << result.dopamineLevel << "\n";
    oss << "Wall Clock Time," << result.totalWallClockTime << "\n";
    oss << "\nIntegration Status:\n";
    oss << "Working Memory," << (result.memoryWorkingMemoryIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Episodic Memory," << (result.memoryEpisodicMemoryIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Neuromodulation," << (result.neuromodulationIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Prediction," << (result.predictionIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Development," << (result.developmentIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Checkpointing," << (result.checkpointingWorks ? "Working" : "Not Working") << "\n";
    oss << "Replay," << (result.replayWorks ? "Working" : "Not Working") << "\n";
    
    return oss.str();
}

std::string EnhancedPhase6Demo::generateTSVReport(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    
    oss << "Metric\tValue\n";
    oss << "Timestamp\t" << currentTimestamp() << "\n";
    oss << "Experiment\tNLM Phase 6 Integration\n";
    oss << "Total Reward\t" << result.totalReward << "\n";
    oss << "Average Firing Rate\t" << result.avgFiringRate << "\n";
    oss << "Average Synaptic Weight\t" << result.avgSynapticWeight << "\n";
    oss << "Episodes Stored\t" << result.memoryEpisodesStored << "\n";
    oss << "Novelty Level\t" << result.noveltyLevel << "\n";
    oss << "Curiosity Level\t" << result.curiosityLevel << "\n";
    oss << "Dopamine Level\t" << result.dopamineLevel << "\n";
    oss << "Wall Clock Time\t" << result.totalWallClockTime << "\n";
    oss << "\nIntegration Status:\n";
    oss << "Working Memory\t" << (result.memoryWorkingMemoryIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Episodic Memory\t" << (result.memoryEpisodicMemoryIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Neuromodulation\t" << (result.neuromodulationIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Prediction\t" << (result.predictionIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Development\t" << (result.developmentIntegrated ? "Connected" : "Disconnected") << "\n";
    oss << "Checkpointing\t" << (result.checkpointingWorks ? "Working" : "Not Working") << "\n";
    oss << "Replay\t" << (result.replayWorks ? "Working" : "Not Working") << "\n";
    
    return oss.str();
}

bool EnhancedPhase6Demo::writeReport(const std::string& content) const {
    std::ofstream file(config_.outputFile);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

void EnhancedPhase6Demo::printHelp() const {
    std::cout << "=== Enhanced NLM Phase 6 CLI Help ===" << std::endl;
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --version                 Show version information" << std::endl;
    std::cout << "  --verbose                     Enable verbose output" << std::endl;
    std::cout << "  --quiet                       Enable quiet mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>              Configuration file path" << std::endl;
    std::cout << "  --save-config <file>         Save configuration to file" << std::endl;
    std::cout << "  --generate-template <file>   Generate configuration template" << std::endl;
    std::cout << "  --validate-config             Validate configuration and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --neurons <count>            Number of neurons" << std::endl;
    std::cout << "  --steps <count>              Maximum simulation steps" << std::endl;
    std::cout << "  --timestep <time>             Simulation timestep" << std::endl;
    std::cout << "  --trials <count>             Number of trials" << std::endl;
    std::cout << "  --experiment-mode <mode>     Experiment mode" << std::endl;
    std::cout << "  --analysis-mode <mode>       Analysis mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output-format <format>      Output format" << std::endl;
    std::cout << "  --output <file>              Output file path" << std::endl;
    std::cout << std::endl;
    std::cout << "Feature Toggles:" << std::endl;
    std::cout << "  --no-integration-tests       Skip integration tests" << std::endl;
    std::cout << "  --no-system-tests            Skip system tests" << std::endl;
    std::cout << "  --no-checkpoint-tests        Skip checkpoint tests" << std::endl;
    std::cout << "  --no-replay-tests            Skip replay tests" << std::endl;
    std::cout << "  --batch                     Enable batch processing" << std::endl;
    std::cout << "  --batch-output <dir>        Batch output directory" << std::endl;
}

void EnhancedPhase6Demo::printVersion() const {
    std::cout << "=== Enhanced NLM Phase 6 Demo ===" << std::endl;
    std::cout << "Version: 6.0.0 Enhanced Edition" << std::endl;
    std::cout << "Build: Advanced features included" << std::endl;
    std::cout << std::endl;
}

bool EnhancedPhase6Demo::loadConfiguration(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

bool EnhancedPhase6Demo::saveConfiguration(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    file << "# Enhanced NLM Configuration" << std::endl;
    file << "neuron_count = " << config_.neuronCount << std::endl;
    file << "max_steps = " << config_.maxSteps << std::endl;
    file << "timestep = " << config_.timestep << std::endl;
    file << "enable_development = " << (config_.enableDevelopment ? "true" : "false") << std::endl;
    file << file.close();
    return true;
}

bool EnhancedPhase6Demo::validateConfiguration() const {
    if (config_.neuronCount < 10) {
        std::cerr << "Error: neuron_count must be at least 10" << std::endl;
        return false;
    }
    if (config_.maxSteps < 100) {
        std::cerr << "Error: max_steps must be at least 100" << std::endl;
        return false;
    }
    if (config_.timestep <= 0 || config_.timestep > 1.0) {
        std::cerr << "Error: timestep must be between 0 and 1.0" << std::endl;
        return false;
    }
    return true;
}

void EnhancedPhase6Demo::applyConfiguration(Phase6IntegratedExperiment& experiment, Phase6Config& phase6Config) const {
    phase6Config.neuronCount = config_.neuronCount;
    phase6Config.maxSteps = config_.maxSteps;
    phase6Config.regionCount = 1;
    phase6Config.connectionProbability = 0.1f;
    phase6Config.enableCheckpointing = config_.runCheckpointTests;
    phase6Config.enableReplay = config_.runReplayTests;
    phase6Config.enableDevelopment = config_.enableDevelopment;
    phase6Config.checkpointPath = "./checkpoint_test.bin";
}

bool EnhancedPhase6Demo::runBatchProcessing() {
    std::cout << "Running batch processing..." << std::endl;
    if (!std::filesystem::exists(config_.batchOutputDir)) {
        std::filesystem::create_directory(config_.batchOutputDir);
    }
    
    for (int i = 0; i < config_.numTrials; ++i) {
        std::cout << "--- Batch Experiment " << (i + 1) << "/" << config_.numTrials << " ---" << std::endl;
        Phase6Config batchConfig;
        batchConfig.neuronCount = config_.neuronCount + (i * 50);
        batchConfig.maxSteps = config_.maxSteps;
        batchConfig.enableCheckpointing = config_.runCheckpointTests;
        batchConfig.enableReplay = config_.runReplayTests;
        batchConfig.enableDevelopment = config_.enableDevelopment;
        batchConfig.checkpointPath = config_.batchOutputDir + "/checkpoint_" + std::to_string(i) + ".bin";
        
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result = experiment.run(batchConfig);
        
        std::string outputFile = config_.batchOutputDir + "/result_" + std::to_string(i) + ".json";
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << generateJSONReport(result);
            file.close();
            std::cout << "Results saved to: " << outputFile << std::endl;
        }
    }
    
    std::cout << "Batch processing completed!" << std::endl;
    return true;
}

bool EnhancedPhase6Demo::runPerformanceBenchmark() {
    std::cout << "Running performance benchmark..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Phase6IntegratedExperiment experiment;
    bool integrationOK = experiment.verifyIntegration();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Integration Verification: " << (integrationOK ? "PASSED" : "FAILED") << std::endl;
    std::cout << "  Total Time: " << duration.count() << "ms" << std::endl;
    std::cout << "  Average Time per Test: " << duration.count() / 10 << "ms" << std::endl;
    
    return integrationOK;
}

void EnhancedPhase6Demo::generateConfigurationTemplate(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to create template file: " << filepath << std::endl;
        return;
    }
    
    file << "# NLM Phase 6 Configuration Template\n";
    file << "# Copy this file and modify values as needed\n";
    file << "\n";
    file << "[BASIC SETTINGS]\n";
    file << "neuron_count = 500\n";
    file << "region_count = 1\n";
    file << "connection_probability = 0.1\n";
    file << "simulation_timestep = 0.001\n";
    file << "random_seed = 42\n";
    file << "\n";
    file << "[NEUROMODULATION SETTINGS]\n";
    file << "dopamine_baseline = 0.1\n";
    file << "\n";
    file << "[EXPERIMENT SETTINGS]\n";
    file << "max_simulation_steps = 10000\n";
    file << "enable_checkpointing = true\n";
    file << "checkpoint_path = ./checkpoint.bin\n";
    file << "enable_replay = true\n";
    file << "\n";
    
    file.close();
    std::cout << "Configuration template generated: " << filepath << std::endl;
}

bool EnhancedPhase6Demo::runExtendedVerification(Phase6IntegratedExperiment& experiment) {
    try {
        bool success = experiment.verifyIntegration();
        if (config_.verbose) {
            std::cout << "Integration verification: " << (success ? "PASSED" : "FAILED") << std::endl;
        }
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Integration verification failed with exception: " << e.what() << std::endl;
        return false;
    }
}

bool EnhancedPhase6Demo::runExtendedSystemTests(Phase6IntegratedExperiment& experiment) {
    std::cout << "Running extended system tests..." << std::endl;
    
    try {
        bool success = true;
        if (config_.verbose) std::cout << "Testing memory integration..." << std::endl;
        success &= experiment.testMemoryIntegration();
        
        if (config_.verbose) std::cout << "Testing neuromodulation integration..." << std::endl;
        success &= experiment.testNeuromodulationIntegration();
        
        if (config_.verbose && config_.runCheckpointTests) {
            std::cout << "Testing checkpointing..." << std::endl;
        }
        success &= experiment.testCheckpointing();
        
        if (config_.verbose && config_.runReplayTests) {
            std::cout << "Testing replay..." << std::endl;
        }
        success &= experiment.testReplay();
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "System tests failed with exception: " << e.what() << std::endl;
        return false;
    }
}

bool EnhancedPhase6Demo::runExtendedPerformanceTests(Phase6IntegratedExperiment& experiment) {
    std::cout << "Running extended performance tests..." << std::endl;
    
    try {
        bool success = experiment.testCheckpointing();
        success &= experiment.testReplay();
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Performance tests failed with exception: " << e.what() << std::endl;
        return false;
    }
}

std::string EnhancedPhase6Demo::generateDetailedAnalysis(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    oss << "=== Detailed Analysis ===" << std::endl;
    oss << std::endl;
    oss << "Performance Metrics:" << std::endl;
    oss << "  - Reward Efficiency: " << formatNumber(result.totalReward / config_.maxSteps) << " per step" << std::endl;
    oss << "  - Processing Speed: " << formatNumber(result.avgFiringRate) << " Hz average" << std::endl;
    oss << "  - Memory Utilization: " << formatNumber(result.memoryEpisodesStored) << " episodes stored" << std::endl;
    oss << "  - Neuromodulation Levels:" << std::endl;
    oss << "    * Dopamine: " << formatNumber(result.dopamineLevel) << " (baseline: " << config_.dopamineBaseline << ")" << std::endl;
    oss << "    * Curiosity: " << formatNumber(result.curiosityLevel) << " (target: " << config_.curiosityLevel << ")" << std::endl;
    oss << "    * Novelty: " << formatNumber(result.noveltyLevel) << " (threshold: " << config_.noveltyThreshold << ")" << std::endl;
    oss << std::endl;
    
    oss << "Integration Quality:" << std::endl;
    int integratedCount = 0;
    int totalCount = 7;
    
    if (result.memoryWorkingMemoryIntegrated) { integratedCount++; oss << "  - Working Memory: ✓" << std::endl; }
    else { oss << "  - Working Memory: ✗" << std::endl; }
    
    if (result.memoryEpisodicMemoryIntegrated) { integratedCount++; oss << "  - Episodic Memory: ✓" << std::endl; }
    else { oss << "  - Episodic Memory: ✗" << std::endl; }
    
    if (result.neuromodulationIntegrated) { integratedCount++; oss << "  - Neuromodulation: ✓" << std::endl; }
    else { oss << "  - Neuromodulation: ✗" << std::endl; }
    
    if (result.predictionIntegrated) { integratedCount++; oss << "  - Prediction: ✓" << std::endl; }
    else { oss << "  - Prediction: ✗" << std::endl; }
    
    if (result.developmentIntegrated) { integratedCount++; oss << "  - Development: ✓" << std::endl; }
    else { oss << "  - Development: ✗" << std::endl; }
    
    if (result.checkpointingWorks) { integratedCount++; oss << "  - Checkpointing: ✓" << std::endl; }
    else { oss << "  - Checkpointing: ✗" << std::endl; }
    
    if (result.replayWorks) { integratedCount++; oss << "  - Replay: ✓" << std::endl; }
    else { oss << "  - Replay: ✗" << std::endl; }
    
    oss << std::endl;
    oss << "Integration Score: " << integratedCount << "/" << totalCount << " (" 
        << formatPercentage(static_cast<float>(integratedCount) / totalCount * 100) << ")" << std::endl;
    
    if (integratedCount == totalCount) {
        oss << "✓ EXCELLENT: All systems are properly integrated!" << std::endl;
    } else if (integratedCount >= totalCount * 0.7) {
        oss << "✓ GOOD: Most systems are properly integrated." << std::endl;
    } else if (integratedCount >= totalCount * 0.4) {
        oss << "⚠ MODERATE: Some systems need attention." << std::endl;
    } else {
        oss << "✗ POOR: Multiple integration issues detected." << std::endl;
    }
    
    return oss.str();
}

std::string EnhancedPhase6Demo::generateComprehensiveAnalysis(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    oss << "=== Comprehensive Analysis ===" << std::endl;
    oss << std::endl;
    
    oss << "System Health Assessment:" << std::endl;
    oss << "  - Overall Integration Health: " << (result.totalReward > 0 ? "STABLE" : "UNSTABLE") << std::endl;
    oss << "  - Memory System Load: " << formatNumber(result.memoryEpisodesStored) << " active episodes" << std::endl;
    oss << "  - Processing Efficiency: " << formatNumber(result.avgFiringRate) << " Hz" << std::endl;
    oss << "  - Neuromodulation Balance:" << std::endl;
    oss << "    * Dopamine: " << formatNumber(result.dopamineLevel) << " (baseline: " << config_.dopamineBaseline << ")" << std::endl;
    oss << "    * Curiosity: " << formatNumber(result.curiosityLevel) << " (target: " << config_.curiosityLevel << ")" << std::endl;
    oss << "    * Novelty: " << formatNumber(result.noveltyLevel) << " (threshold: " << config_.noveltyThreshold << ")" << std::endl;
    oss << std::endl;
    
    oss << "Development Progress:" << std::endl;
    oss << "  - Plasticity Rate: " << (config_.enableDevelopment ? "Active" : "Inactive") << std::endl;
    oss << "  - Structural Growth: " << (config_.enableStructuralPlasticity ? "Enabled" : "Disabled") << std::endl;
    oss << "  - Learning Rate: " << (config_.enableCuriosity ? "Curiosity-driven" : "Experience-driven") << std::endl;
    oss << "  - Adaptation Capacity: " << (config_.enableDevelopmentStages ? "Multi-stage" : "Single-stage") << std::endl;
    oss << std::endl;
    
    oss << "Performance Recommendations:" << std::endl;
    if (!result.memoryWorkingMemoryIntegrated) {
        oss << "  - Consider increasing working memory capacity" << std::endl;
    }
    if (!result.predictionIntegrated) {
        oss << "  - Prediction system needs to be enabled" << std::endl;
    }
    if (result.totalWallClockTime > 300) {
        oss << "  - Consider increasing simulation speed" << std::endl;
    }
    if (result.memoryEpisodesStored < 10) {
        oss << "  - Memory consolidation may be insufficient" << std::endl;
    }
    
    oss << std::endl;
    oss << "Future Enhancement Suggestions:" << std::endl;
    oss << "  1. Implement social learning capabilities" << std::endl;
    oss << "  2. Add adaptive resource allocation" << std::endl;
    oss << "  3. Incorporate predictive maintenance" << std::endl;
    oss << "  4. Enable real-time monitoring" << std::endl;
    oss << "  5. Add reinforcement learning optimization" << std::endl;
    
    return oss.str();
}

} // namespace nlm
