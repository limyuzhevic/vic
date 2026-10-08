/**
 * NLMCAdvancedCLI Implementation
 *
 * Implementation of the advanced command line interface for NLM
 */

#include "advanced_cli.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <filesystem>

namespace nlm {

void NLMCAdvancedCLI::setupDefaultConfiguration() {
    // Setup default configuration
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
    
    // Set default paths
    config_.configFile = "configs/default.cfg";
    config_.outputFile = "";
    config_.batchOutputDir = "batch_output";
}

void NLMCAdvancedCLI::setupCommandLineOptions() {
    commandLineOptions_ = {
        // Basic options
        {"-h", "--help", "Show help information", "", nullptr},
        {"-v", "--version", "Show version information", "", nullptr},
        {"--verbose", "Enable verbose output", "", "", nullptr},
        {"--quiet", "Enable quiet mode", "", "", nullptr},
        {"--output-format", "Output format (text|json|csv|tsv)", "text", nullptr},
        {"--experiment-mode", "Experiment mode (verification|quick|full|benchmark)", "full", nullptr},
        {"--analysis-mode", "Analysis mode (basic|detailed|comprehensive)", "detailed", nullptr},
        
        // Configuration options
        {"--config", "Configuration file path", "default.cfg", nullptr},
        {"--save-config", "Save configuration to file", "", nullptr},
        
        // Experiment options
        {"--neurons", "Number of neurons", "500", nullptr},
        {"--steps", "Maximum simulation steps", "2000", nullptr},
        {"--timestep", "Simulation timestep", "0.001", nullptr},
        {"--trials", "Number of trials", "1", nullptr},
        
        // Feature toggles
        {"--no-integration-tests", "Skip integration tests", "", nullptr},
        {"--no-system-tests", "Skip system tests", "", nullptr},
        {"--no-checkpoint-tests", "Skip checkpoint tests", "", nullptr},
        {"--no-replay-tests", "Skip replay tests", "", nullptr},
        {"--no-development", "Disable development", "", nullptr},
        {"--no-plastic", "Disable structural plasticity", "", nullptr},
        {"--no-curiosity", "Disable curiosity", "", nullptr},
        {"--no-prediction", "Disable prediction", "", nullptr},
        
        // Performance options
        {"--profile", "Enable performance profiling", "", nullptr},
        {"--no-timing", "Disable timing", "", nullptr},
        {"--memory-monitoring", "Enable memory monitoring", "", nullptr},
        
        // Advanced options
        {"--batch", "Enable batch processing", "", nullptr},
        {"--batch-output", "Batch output directory", "batch_output", nullptr},
        {"--generate-template", "Generate configuration template", "template.cfg", nullptr},
        {"--validate-config", "Validate configuration and exit", "", nullptr},
    };
}

bool NLMCAdvancedCLI::parseArguments(int argc, char* argv[]) {
    // Parse positional arguments first
    std::vector<std::string> positionalArgs;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return true; // Help flag means exit
        }
        else if (arg == "-v" || arg == "--version") {
            printVersion();
            return true; // Version flag means exit
        }
        else if (arg == "--verbose") {
            config_.verbose = true;
        }
        else if (arg == "--quiet") {
            config_.quiet = true;
        }
        else if (arg == "--validate-config") {
            if (loadConfiguration(config_.configFile)) {
                if (validateConfiguration()) {
                    std::cout << "Configuration is valid!" << std::endl;
                    return true; // Valid config, exit
                } else {
                    std::cerr << "Configuration is invalid!" << std::endl;
                    return 1; // Invalid config, error
                }
            } else {
                std::cerr << "Failed to load configuration file!" << std::endl;
                return 1;
            }
        }
        else if (arg == "--generate-template") {
            std::string templateFile = "template.cfg";
            if (i + 1 < argc) {
                templateFile = argv[++i];
            }
            generateConfigurationTemplate(templateFile);
            return true;
        }
        else if (arg == "--save-config") {
            std::string saveFile = "advanced_config.cfg";
            if (i + 1 < argc) {
                saveFile = argv[++i];
            }
            saveConfiguration(saveFile);
            return true;
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
        else if (arg == "--analysis-mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "comprehensive") config_.analysisMode = AnalysisMode::COMPREHENSIVE;
            else if (mode == "basic") config_.analysisMode = AnalysisMode::BASIC;
            else config_.analysisMode = AnalysisMode::DETAILED;
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
        else if (arg == "--trials" && i + 1 < argc) {
            config_.numTrials = parseUint64(argv[++i]);
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
        else if (arg == "--no-development") {
            config_.enableDevelopment = false;
        }
        else if (arg == "--no-plastic") {
            config_.enableStructuralPlasticity = false;
        }
        else if (arg == "--no-curiosity") {
            config_.enableCuriosity = false;
        }
        else if (arg == "--no-prediction") {
            config_.enablePrediction = false;
        }
        else if (arg == "--profile") {
            config_.enableProfiling = true;
        }
        else if (arg == "--no-timing") {
            config_.enableTiming = false;
        }
        else if (arg == "--memory-monitoring") {
            config_.enableMemoryMonitoring = true;
        }
        else if (arg == "--batch") {
            config_.batchMode = true;
        }
        else if (arg == "--batch-output" && i + 1 < argc) {
            config_.batchOutputDir = argv[++i];
        }
        else {
            // Positional argument - save for later use
            positionalArgs.push_back(arg);
        }
    }
    
    return false;
}

int NLMCAdvancedCLI::runExperiment() {
    // Load configuration
    if (!loadConfiguration(config_.configFile)) {
        std::cerr << "Warning: Failed to load configuration file: " << config_.configFile << std::endl;
    }
    
    // Setup logging
    setupLogging();
    
    // Validate configuration
    if (!validateConfiguration()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        return 1;
    }
    
    // Run batch processing if enabled
    if (config_.batchMode) {
        return runBatchProcessing() ? 0 : 1;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "=== NLM Phase 6 Advanced Demo ===" << std::endl;
    
    // Print experiment info
    printExperimentInfo();
    
    // Run experiments based on mode
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
        std::cout << "\n--- Running Checkpoint Tests ---" << std::endl;
        success &= runExtendedPerformanceTests(experiment);
    }
    
    if (config_.runReplayTests) {
        std::cout << "\n--- Running Replay Tests ---" << std::endl;
        // Replay tests are part of the performance tests
    }
    
    // Run main experiment
    std::cout << "\n--- Main Integration Experiment ---" << std::endl;
    Phase6Config phase6Config;
    applyConfiguration(experiment, phase6Config);
    
    Phase6IntegrationResult result;
    if (config_.experimentMode != ExperimentMode::VERIFICATION_ONLY) {
        result = experiment.run(phase6Config);
        
        // Generate and output report
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
                break;
        }
        
        // Write report if output file specified
        if (!config_.outputFile.empty()) {
            if (!writeReport(report)) {
                std::cerr << "Failed to write report to file!" << std::endl;
                success = false;
            }
        }
        
        // Also print to console
        printOutput(report);
    }
    
    // Performance benchmark if requested
    if (config_.experimentMode == ExperimentMode::PERFORMANCE_BENCHMARK) {
        std::cout << "\n--- Running Performance Benchmark ---" << std::endl;
        runPerformanceBenchmark();
    }
    
    // Print system metrics if profiling enabled
    if (config_.enableProfiling) {
        std::cout << "\n--- System Metrics ---" << std::endl;
        auto metrics = getSystemMetrics();
        for (const auto& metric : metrics) {
            std::cout << metric.first << ": " << metric.second << std::endl;
        }
    }
    
    if (success) {
        std::cout << "\n=== Experiment Completed Successfully ===" << std::endl;
        return 0;
    } else {
        std::cerr << "\n=== Experiment Failed ===" << std::endl;
        return 1;
    }
}

std::string NLMCAdvancedCLI::generateReport(const Phase6IntegrationResult& result) const {
    std::ostringstream oss;
    
    oss << "=== NLM Phase 6 Integration Results ===" << std::endl;
    oss << "Generated: " << currentTimestamp() << std::endl;
    oss << std::endl;
    
    // Basic metrics
    oss << "Basic Metrics:" << std::endl;
    oss << "  Total Reward: " << formatNumber(result.totalReward) << std::endl;
    oss << "  Average Firing Rate: " << formatNumber(result.avgFiringRate) << std::endl;
    oss << "  Average Synaptic Weight: " << formatNumber(result.avgSynapticWeight) << std::endl;
    oss << "  Episodes Stored: " << formatNumber(result.memoryEpisodesStored) << std::endl;
    oss << "  Novelty Level: " << formatNumber(result.noveltyLevel) << std::endl;
    oss << "  Curiosity Level: " << formatNumber(result.curiosityLevel) << std::endl;
    oss << "  Dopamine Level: " << formatNumber(result.dopamineLevel) << std::endl;
    oss << std::endl;
    
    // Integration status
    oss << "Integration Status:" << std::endl;
    oss << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    oss << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    oss << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    oss << std::endl;
    
    // Timing information
    if (result.totalWallClockTime > 0) {
        oss << "Timing:" << std::endl;
        oss << "  Wall Clock Time: " << formatNumber(result.totalWallClockTime) << "s" << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

std::string NLMCAdvancedCLI::generateJSONReport(const Phase6IntegrationResult& result) const {
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

std::string NLMCAdvancedCLI::generateCSVReport(const Phase6IntegrationResult& result) const {
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

std::string NLMCAdvancedCLI::generateTSVReport(const Phase6IntegrationResult& result) const {
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

bool NLMCAdvancedCLI::writeReport(const std::string& content) const {
    if (config_.outputFile.empty()) {
        return false;
    }
    
    std::ofstream file(config_.outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

void NLMCAdvancedCLI::printHelp() const {
    std::cout << "=== NLM Advanced CLI Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --version                 Show version information" << std::endl;
    std::cout << "  --verbose                     Enable verbose output" << std::endl;
    std::cout << "  --quiet                       Enable quiet mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>              Configuration file path (default: configs/default.cfg)" << std::endl;
    std::cout << "  --save-config <file>         Save configuration to file" << std::endl;
    std::cout << "  --generate-template <file>   Generate configuration template" << std::endl;
    std::cout << "  --validate-config             Validate configuration and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --neurons <count>            Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <count>              Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --timestep <time>             Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --trials <count>             Number of trials (default: 1)" << std::endl;
    std::cout << "  --experiment-mode <mode>     Experiment mode (verification|quick|full|benchmark)" << std::endl;
    std::cout << "  --analysis-mode <mode>       Analysis mode (basic|detailed|comprehensive)" << std::endl;
    std::cout << std::endl;
    std::cout << "Feature Toggles:" << std::endl;
    std::cout << "  --no-integration-tests       Skip integration tests" << std::endl;
    std::cout << "  --no-system-tests            Skip system tests" << std::endl;
    std::cout << "  --no-checkpoint-tests        Skip checkpoint tests" << std::endl;
    std::cout << "  --no-replay-tests            Skip replay tests" << std::endl;
    std::cout << "  --no-development             Disable development" << std::endl;
    std::cout << "  --no-plastic                Disable structural plasticity" << std::endl;
    std::cout << "  --no-curiosity               Disable curiosity" << std::endl;
    std::cout << "  --no-prediction              Disable prediction" << std::endl;
    std::cout << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output-format <format>      Output format (text|json|csv|tsv)" << std::endl;
    std::cout << "  --output <file>              Output file path" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance Options:" << std::endl;
    std::cout << "  --profile                    Enable performance profiling" << std::endl;
    std::cout << "  --no-timing                  Disable timing" << std::endl;
    std::cout << "  --memory-monitoring          Enable memory monitoring" << std::endl;
    std::cout << std::endl;
    std::cout << "Batch Processing:" << std::endl;
    std::cout << "  --batch                     Enable batch processing" << std::endl;
    std::cout << "  --batch-output <dir>        Batch output directory" << std::endl;
    std::cout << std::endl;
}

void NLMCAdvancedCLI::printVersion() const {
    std::cout << "=== NLM Phase 6 Advanced Demo ===" << std::endl;
    std::cout << "Version: 6.0.0" << std::endl;
    std::cout << "Build: Advanced Edition with Enhanced Features" << std::endl;
    std::cout << "Phase: FINAL INTEGRATION" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Advanced command-line interface" << std::endl;
    std::cout << "  - Multiple output formats (text, JSON, CSV, TSV)" << std::endl;
    std::endl;
    std::cout << "  - Batch processing" << std::endl;
    std::cout << "  - Configuration validation" << std::endl;
    std::cout << "  - Performance monitoring" << std::endl;
    std::cout << "  - Template generation" << std::endl;
}

bool NLMCAdvancedCLI::loadConfiguration(const std::string& filepath) {
    // For now, just check if file exists
    // In a real implementation, this would parse the configuration file
    return std::filesystem::exists(filepath);
}

bool NLMCAdvancedCLI::saveConfiguration(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Advanced Configuration Template" << std::endl;
    file << "# Generated by NLMCAdvancedCLI" << std::endl;
    file << std::endl;
    file << "# Basic settings" << std::endl;
    file << "neuron_count = " << config_.neuronCount << std::endl;
    file << "max_steps = " << config_.maxSteps << std::endl;
    file << "timestep = " << config_.timestep << std::endl;
    file << "num_trials = " << config_.numTrials << std::endl;
    file << std::endl;
    file << "# Feature toggles" << std::endl;
    file << "enable_development = " << (config_.enableDevelopment ? "true" : "false") << std::endl;
    file << "enable_structural_plasticity = " << (config_.enableStructuralPlasticity ? "true" : "false") << std::endl;
    file << "enable_curiosity = " << (config_.enableCuriosity ? "true" : "false") << std::endl;
    file << "enable_prediction = " << (config_.enablePrediction ? "true" : "false") << std::endl;
    file << std::endl;
    
    file.close();
    return true;
}

bool NLMCAdvancedCLI::validateConfiguration() const {
    // Basic validation checks
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

void NLMCAdvancedCLI::applyConfiguration(Phase6IntegratedExperiment& experiment, Phase6Config& phase6Config) const {
    phase6Config.neuronCount = config_.neuronCount;
    phase6Config.maxSteps = config_.maxSteps;
    phase6Config.regionCount = 1; // Default to 1 region
    phase6Config.connectionProbability = 0.1f;
    phase6Config.enableCheckpointing = config_.runCheckpointTests;
    phase6Config.enableReplay = config_.runReplayTests;
    phase6Config.enableDevelopment = config_.enableDevelopment;
    phase6Config.checkpointPath = "./checkpoint_test.bin";
}

bool NLMCAdvancedCLI::runBatchProcessing() {
    std::cout << "Running batch processing..." << std::endl;
    std::cout << "Output directory: " << config_.batchOutputDir << std::endl;
    
    // Create output directory if it doesn't exist
    if (!std::filesystem::exists(config_.batchOutputDir)) {
        if (!std::filesystem::create_directory(config_.batchOutputDir)) {
            std::cerr << "Failed to create output directory!" << std::endl;
            return false;
        }
    }
    
    // Run multiple experiments with different configurations
    int experimentCount = std::min(config_.numTrials, static_cast<uint64_t>(5));
    
    for (int i = 0; i < experimentCount; ++i) {
        std::cout << "\n--- Batch Experiment " << (i + 1) << "/" << experimentCount << " ---" << std::endl;
        
        // Create slightly different configuration for each run
        Phase6Config batchConfig;
        batchConfig.neuronCount = config_.neuronCount + (i * 50); // Vary neuron count
        batchConfig.maxSteps = config_.maxSteps;
        batchConfig.enableCheckpointing = config_.runCheckpointTests;
        batchConfig.enableReplay = config_.runReplayTests;
        batchConfig.enableDevelopment = config_.enableDevelopment;
        batchConfig.checkpointPath = config_.batchOutputDir + "/checkpoint_" + std::to_string(i) + ".bin";
        
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result = experiment.run(batchConfig);
        
        // Generate output file for this experiment
        std::string outputFile = config_.batchOutputDir + "/result_" + std::to_string(i) + ".json";
        
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << generateJSONReport(result);
            file.close();
            std::cout << "Results saved to: " << outputFile << std::endl;
        }
    }
    
    std::cout << "\nBatch processing completed!" << std::endl;
    return true;
}

bool NLMCAdvancedCLI::runPerformanceBenchmark() {
    std::cout << "Running performance benchmark..." << std::endl;
    
    // Measure startup time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    bool integrationOK = experiment.verifyIntegration();
    
    // Measure performance
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Integration Verification: " << (integrationOK ? "PASSED" : "FAILED") << std::endl;
    std::cout << "  Total Time: " << duration.count() << "ms" << std::endl;
    std::cout << "  Average Time per Test: " << duration.count() / 10 << "ms" << std::endl;
    
    return integrationOK;
}

void NLMCAdvancedCLI::generateConfigurationTemplate(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to create template file: " << filepath << std::endl;
        return;
    }
    
    file << "# NLM Phase 6 Configuration Template" << std::endl;
    file << "# Copy this file and modify values as needed" << std::endl;
    file << "# All settings can be overridden via command line arguments" << std::endl;
    file << std::endl;
    
    file << "[BASIC SETTINGS]" << std::endl;
    file << "neuron_count = 500" << std::endl;
    file << "region_count = 1" << std::endl;
    file << "connection_probability = 0.1" << std::endl;
    file << "simulation_timestep = 0.001" << std::endl;
    file << "random_seed = 42" << std::endl;
    file << "stdp_ltp_weight = 0.01" << std::endl;
    file << "stdp_ltd_weight = 0.012" << std::endl;
    file << "stdp_time_constant = 20.0" << std::endl;
    file << std::endl;
    
    file << "[MEMORY SETTINGS]" << std::endl;
    file << "working_memory_capacity = 100" << std::endl;
    file << "episodic_memory_capacity = 1000" << std::endl;
    file << "semantic_memory_capacity = 500" << std::endl;
    file << "procedural_memory_capacity = 200" << std::endl;
    file << "associative_memory_strength = 0.5" << std::endl;
    file << std::endl;
    
    file << "[NEUROMODULATION SETTINGS]" << std::endl;
    file << "dopamine_baseline = 0.1" << std::endl;
    file << "dopamine_sensitivity = 1.0" << std::endl;
    file << "curiosity_baseline = 1.0" << std::endl;
    file << "curiosity_sensitivity = 1.0" << std::endl;
    file << "novelty_threshold = 0.5" << std::endl;
    file << "novelty_sensitivity = 1.0" << std::endl;
    file << std::endl;
    
    file << "[PREDICTION SETTINGS]" << std::endl;
    file << "prediction_enabled = true" << std::endl;
    file << "prediction_confidence_threshold = 0.7" << std::endl;
    file << "prediction_memory_capacity = 100" << std::endl;
    file << "prediction_update_rate = 0.1" << std::endl;
    file << std::endl;
    
    file << "[DEVELOPMENT SETTINGS]" << std::endl;
    file << "development_enabled = true" << std::endl;
    file << "synaptogenesis_rate = 0.0001" << std::endl;
    file << "pruning_rate = 0.00001" << std::endl;
    file << "critical_period_start = 0.1" << std::endl;
    file << "critical_period_end = 1.0" << std::endl;
    file << "plasticity_rate_multiplier = 1.0" << std::endl;
    file << std::endl;
    
    file << "[EXPERIMENT SETTINGS]" << std::endl;
    file << "max_simulation_steps = 10000" << std::endl;
    file << "simulation_time_limit = 0.0" << std::endl;
    file << "enable_checkpointing = true" << std::endl;
    file << "checkpoint_path = ./checkpoint.bin" << std::endl;
    file << "enable_replay = true" << std::endl;
    file << "replay_steps = 100" << std::endl;
    file << std::endl;
    
    file << "[LOGGING SETTINGS]" << std::endl;
    file << "log_level = INFO" << std::endl;
    file << "log_to_file = false" << std::endl;
    file << "log_filename = nlm.log" << std::endl;
    file << "log_file_rotation = 10" << std::endl;
    file << "log_file_size_limit = 100" << std::endl;
    file << std::endl;
    
    file.close();
    
    std::cout << "Configuration template generated: " << filepath << std::endl;
}

void NLMCAdvancedCLI::setupLogging() {
    // Setup logging based on configuration
    Logger::getGlobal().setLevel(config_.verbose ? Logger::Level::Debug : Logger::Level::Info);
}

std::string NLMCAdvancedCLI::formatOutput(const std::string& content) const {
    return content;
}

void NLMCAdvancedCLI::printOutput(const std::string& content) const {
    std::cout << content;
}

std::string NLMCAdvancedCLI::formatNumber(float value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

std::string NLMCAdvancedCLI::formatPercentage(float value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value << "%";
    return oss.str();
}

bool NLMCAdvancedCLI::parseBoolean(const std::string& str) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

float NLMCAdvancedCLI::parseFloat(const std::string& str) const {
    return std::stof(str);
}

uint64_t NLMCAdvancedCLI::parseUint64(const std::string& str) const {
    return std::stoull(str);
}

std::string NLMCAdvancedCLI::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

void NLMCAdvancedCLI::printSystemInfo() const {
    std::cout << "=== System Information ===" << std::endl;
    std::cout << "Platform: " << (std::filesystem::path("/").string() == "//" ? "Unknown" : "Linux") << std::endl;
    std::cout << "Architecture: " << (sizeof(void*) * 8) << "-bit" << std::endl;
    std::cout << "Compiler: " << (defined(__GNUC__) ? "GCC" : "Unknown") << std::endl;
    std::cout << "C++ Standard: " << (defined(__cplusplus) ? std::to_string(__cplusplus) : "Unknown") << std::endl;
    std::cout << std::endl;
}

void NLMCAdvancedCLI::printExperimentInfo() const {
    std::cout << "=== NLM Phase 6 Advanced Demo ===" << std::endl;
    std::cout << "Version: 6.0.0 Advanced Edition" << std::endl;
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

std::unordered_map<std::string, std::string> NLMCAdvancedCLI::getSystemMetrics() const {
    std::unordered_map<std::string, std::string> metrics;
    
    // Get basic system info
    metrics["Platform"] = "Linux";
    metrics["Architecture"] = std::to_string(sizeof(void*) * 8) + "-bit";
    metrics["Compiler"] = "GCC";
    metrics["Memory Available"] = "Unknown";
    metrics["CPU Cores"] = "Unknown";
    
    // Get memory usage
    std::ifstream memInfo("/proc/meminfo");
    if (memInfo.is_open()) {
        std::string line;
        while (std::getline(memInfo, line)) {
            if (line.find("MemAvailable:") != std::string::npos) {
                metrics["Memory Available"] = line.substr(line.find(':') + 1);
                break;
            }
        }
        memInfo.close();
    }
    
    // Get CPU info
    std::ifstream cpuInfo("/proc/cpuinfo");
    if (cpuInfo.is_open()) {
        int coreCount = 0;
        std::string line;
        while (std::getline(cpuInfo, line)) {
            if (line.find("processor") != std::string::npos && line.find(":") != std::string::npos) {
                coreCount++;
            }
        }
        if (coreCount > 0) {
            metrics["CPU Cores"] = std::to_string(coreCount);
        }
        cpuInfo.close();
    }
    
    return metrics;
}

bool NLMCAdvancedCLI::runExtendedVerification(Phase6IntegratedExperiment& experiment) {
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

bool NLMCAdvancedCLI::runExtendedSystemTests(Phase6IntegratedExperiment& experiment) {
    std::cout << "Running extended system tests..." << std::endl;
    
    bool success = true;
    
    try {
        // Test memory integration
        if (config_.verbose) {
            std::cout << "Testing memory integration..." << std::endl;
        }
        success &= experiment.testMemoryIntegration();
        
        // Test neuromodulation integration
        if (config_.verbose) {
            std::cout << "Testing neuromodulation integration..." << std::endl;
        }
        success &= experiment.testNeuromodulationIntegration();
        
        // Test checkpointing
        if (config_.verbose && config_.runCheckpointTests) {
            std::cout << "Testing checkpointing..." << std::endl;
        }
        success &= experiment.testCheckpointing();
        
        // Test replay
        if (config_.verbose && config_.runReplayTests) {
            std::cout << "Testing replay..." << std::endl;
        }
        success &= experiment.testReplay();
        
    } catch (const std::exception& e) {
        std::cerr << "System tests failed with exception: " << e.what() << std::endl;
        success = false;
    }
    
    return success;
}

bool NLMCAdvancedCLI::runExtendedPerformanceTests(Phase6IntegratedExperiment& experiment) {
    std::cout << "Running extended performance tests..." << std::endl;
    
    try {
        success &= experiment.testCheckpointing();
        success &= experiment.testReplay();
        
    } catch (const std::exception& e) {
        std::cerr << "Performance tests failed with exception: " << e.what() << std::endl;
        return false;
    }
    
    return success;
}
    
    oss << "Performance Metrics:" << std::endl;
    oss << "  - Reward Efficiency: " << formatNumber(result.totalReward / config_.maxSteps) << " per step" << std::endl;
    oss << "  - Processing Speed: " << formatNumber(result.avgFiringRate) << " Hz average" << std::endl;
    oss << "  - Memory Utilization: " << formatNumber(result.memoryEpisodesStored) << " episodes stored" << std::endl;
    oss << "  - Neuromodulation Levels:" << std::endl;
    oss << "    * Dopamine: " << formatNumber(result.dopamineLevel) << std::endl;
    oss << "    * Curiosity: " << formatNumber(result.curiosityLevel) << std::endl;
    oss << "    * Novelty: " << formatNumber(result.noveltyLevel) << std::endl;
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
    
    // Overall assessment
    oss << std::endl;
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

std::string NLMCAdvancedCLI::generateComprehensiveAnalysis(const Phase6IntegrationResult& result) const {
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
    
    if (result.totalWallClockTime > 300) { // 5 minutes
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
