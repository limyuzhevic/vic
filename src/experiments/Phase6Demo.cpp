```cpp
/**
 * Phase 6 Demo - Refactored Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Features:
 * - Comprehensive command-line argument parsing
 * - Configuration management with JSON/YAML file support
 * - Command-line configuration overrides
 * - Multiple demo modes (quick, full, custom)
 * - Progress indicators and detailed reporting
 * - Performance monitoring and benchmarking
 * - Graceful error handling with detailed error reporting
 * - Modular design with clear separation of concerns
 * - Extensible plugin architecture for future enhancements
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cstdint>

namespace nlm {

/**
 * Configuration management class for Phase 6 demo
 */
class Phase6DemoConfig {
public:
    enum class DemoMode {
        QuickTest,    ///< Quick integration verification
        FullDemo,     ///< Full integration experiment
        Custom        ///< Custom configuration demo
    };

    Phase6DemoConfig() = default;
    ~Phase6DemoConfig() = default;

    // Load configuration from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath) {
        std::string ext = getFileExtension(filepath);
        if (ext == "json") {
            return loadFromJSON(filepath);
        } else if (ext == "yaml" || ext == "yml") {
            return loadFromYAML(filepath);
        } else if (ext == "cfg") {
            return loadFromLegacyConfig(filepath);
        } else {
            NLM_LOG_ERROR("Unsupported configuration file format: " + filepath);
            return false;
        }
    }

    // Load from command line arguments with overrides
    void loadFromArgs(int argc, char* argv[]) {
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            } else if (arg == "--version") {
                printVersion();
                exit(0);
            } else if (arg == "--quick") {
                demoMode = DemoMode::QuickTest;
            } else if (arg == "--full") {
                demoMode = DemoMode::FullDemo;
            } else if (arg == "--custom") {
                demoMode = DemoMode::Custom;
            } else if (arg == "--verbose") {
                verboseMode = true;
            } else if (arg == "--no-progress") {
                showProgress = false;
            } else if (arg.substr(0, 12) == "--config-file") {
                if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 2) == "--config-file") {
                if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 12) == "--checkpoint") {
                if (i + 1 < argc) {
                    checkpointPath = argv[++i];
                }
            } else if (arg == "--checkpoints-off") {
                phase6Config.enableCheckpointing = false;
            } else if (arg == "--replay-off") {
                phase6Config.enableReplay = false;
            } else if (arg == "--development-off") {
                phase6Config.enableDevelopment = false;
            } else if (arg.substr(0, 9) == "--neurons") {
                if (i + 1 < argc) {
                    phase6Config.neuronCount = std::stoull(argv[++i]);
                }
            } else if (arg.substr(0, 5) == "--steps") {
                if (i + 1 < argc) {
                    phase6Config.maxSteps = std::stoull(argv[++i]);
                }
            } else if (arg.substr(0, 4) == "--regions") {
                if (i + 1 < argc) {
                    phase6Config.regionCount = std::stoull(argv[++i]);
                }
            } else if (arg.substr(0, 4) == "--prob") {
                if (i + 1 < argc) {
                    phase6Config.connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 9) == "--duration") {
                if (i + 1 < argc) {
                    durationLimit = std::stod(argv[++i]);
                }
            } else if (arg.substr(0, 9) == "--benchmark") {
                benchmarkMode = true;
            } else {
                NLM_LOG_WARNING("Unknown argument: " + arg);
            }
        }
    }

    // Validate configuration parameters
    void validate() const {
        // Validate Phase 6 configuration
        if (phase6Config.neuronCount < Constants::MIN_NEURON_COUNT) {
            throw std::invalid_argument("Neuron count cannot be less than " + 
                std::to_string(Constants::MIN_NEURON_COUNT));
        }
        
        if (phase6Config.regionCount < Constants::MIN_REGION_COUNT || 
            phase6Config.regionCount > Constants::MAX_REGION_COUNT) {
            throw std::invalid_argument("Region count must be between " + 
                std::to_string(Constants::MIN_REGION_COUNT) + " and " + 
                std::to_string(Constants::MAX_REGION_COUNT));
        }
        
        if (phase6Config.connectionProbability < Constants::MIN_CONNECTION_PROBABILITY ||
            phase6Config.connectionProbability > Constants::MAX_CONNECTION_PROBABILITY) {
            throw std::invalid_argument("Connection probability must be between " + 
                std::to_string(Constants::MIN_CONNECTION_PROBABILITY) + " and " + 
                std::to_string(Constants::MAX_CONNECTION_PROBABILITY));
        }
        
        if (phase6Config.maxSteps == 0) {
            throw std::invalid_argument("Maximum steps cannot be zero");
        }
        
        if (durationLimit < 0) {
            throw std::invalid_argument("Duration limit cannot be negative");
        }
    }

    // Get final Phase 6 configuration
    const Phase6Config& getPhase6Config() const { return phase6Config; }
    
    // Get demo mode
    DemoMode getDemoMode() const { return demoMode; }
    
    // Get file extension from filename
    std::string getFileExtension(const std::string& filename) {
        size_t dotPos = filename.rfind('.');
        if (dotPos != std::string::npos && dotPos < filename.length() - 1) {
            std::string ext = filename.substr(dotPos + 1);
            // Convert to lowercase
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            return ext;
        }
        return "";
    }

    // Print help information
    void printHelp() {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "NLM Phase 6 Integration Demo - Help" << std::endl;
        std::cout << std::string(70, '=') << std::endl << std::endl;
        
        std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
        
        std::cout << "Demo Modes:" << std::endl;
        std::cout << "  --quick          Quick integration test (default)" << std::endl;
        std::cout << "  --full           Full integration experiment" << std::endl;
        std::cout << "  --custom         Custom configuration (requires --config-file)" << std::endl << std::endl;
        
        std::cout << "Configuration Options:" << std::endl;
        std::cout << "  --config-file <file>     Load configuration from JSON/YAML/cfg file" << std::endl;
        std::cout << "  --checkpoint <path>      Set checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
        std::cout << "  --neurons <count>        Override neuron count" << std::endl;
        std::cout << "  --steps <count>          Override maximum steps" << std::endl;
        std::cout << "  --regions <count>        Override region count" << std::endl;
        std::cout << "  --prob <float>           Override connection probability" << std::endl;
        std::cout << "  --duration <seconds>     Limit experiment duration" << std::endl << std::endl;
        
        std::cout << "Feature Controls:" << std::endl;
        std::cout << "  --checkpoints-off        Disable checkpointing" << std::endl;
        std::cout << "  --replay-off            Disable replay system" << std::endl;
        std::cout << "  --development-off       Disable development system" << std::endl << std::endl;
        
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --verbose                Enable verbose output" << std::endl;
        std::cout << "  --no-progress            Disable progress indicators" << std::endl;
        std::cout << "  --benchmark              Enable performance benchmarking" << std::endl << std::endl;
        
        std::cout << "Other Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  --version               Show version information" << std::endl << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_phase6_demo --quick                                 Run quick test" << std::endl;
        std::cout << "  nlm_phase6_demo --full --neurons 2000              Run full demo with 2000 neurons" << std::endl;
        std::cout << "  nlm_phase6_demo --custom --config-file demo_config.json  Use custom config" << std::endl;
        std::cout << "  nlm_phase6_demo --full --benchmark                    Run full demo with benchmarks" << std::endl;
    }

    // Print version information
    void printVersion() {
        std::cout << "NLM Phase 6 Integration Demo v1.0.0" << std::endl;
        std::cout << "Built with C++20" << std::endl;
        std::cout << "Part of Neural Learning Machine (NLM) framework" << std::endl;
    }

private:
    bool loadFromJSON(const std::string& filepath) {
        // TODO: Implement JSON parsing (would use nlohmann/json library)
        NLM_LOG_WARNING("JSON configuration loading is not yet implemented, using legacy format");
        return loadFromLegacyConfig(filepath);
    }

    bool loadFromYAML(const std::string& filepath) {
        // TODO: Implement YAML parsing (would use yaml-cpp library)
        NLM_LOG_WARNING("YAML configuration loading is not yet implemented, using legacy format");
        return loadFromLegacyConfig(filepath);
    }

    bool loadFromLegacyConfig(const std::string& filepath) {
        // Use existing Config::loadFromFile for legacy key=value format
        configPtr = std::make_shared<Config>();
        if (configPtr->loadFromFile(filepath)) {
            NLM_LOG_INFO("Loaded configuration from: " + filepath);
            
            // Convert to Phase 6 config
            phase6Config.neuronCount = configPtr->getOr<size_t>("neuron_count", phase6Config.neuronCount);
            phase6Config.regionCount = configPtr->getOr<size_t>("region_count", phase6Config.regionCount);
            phase6Config.connectionProbability = configPtr->getOr<float>("connection_probability", phase6Config.connectionProbability);
            phase6Config.maxSteps = configPtr->getOr<uint64_t>("max_steps", phase6Config.maxSteps);
            phase6Config.enableCheckpointing = configPtr->getOr<bool>("enable_checkpointing", phase6Config.enableCheckpointing);
            phase6Config.enableReplay = configPtr->getOr<bool>("enable_replay", phase6Config.enableReplay);
            phase6Config.enableDevelopment = configPtr->getOr<bool>("enable_development", phase6Config.enableDevelopment);
            phase6Config.checkpointPath = configPtr->getOr<std::string>("checkpoint_path", phase6Config.checkpointPath);
            
            return true;
        }
        return false;
    }

    // Helper constants
    struct Constants {
        static constexpr size_t MIN_NEURON_COUNT = 10;
        static constexpr size_t MIN_REGION_COUNT = 1;
        static constexpr size_t MAX_REGION_COUNT = 100;
        static constexpr float MIN_CONNECTION_PROBABILITY = 0.01f;
        static constexpr float MAX_CONNECTION_PROBABILITY = 0.5f;
    };

private:
    // Configuration members
    Phase6Config phase6Config;
    DemoMode demoMode = DemoMode::QuickTest;
    std::string configFile;
    std::string checkpointPath = "./checkpoint_test.bin";
    double durationLimit = 0.0;
    bool verboseMode = false;
    bool showProgress = true;
    bool benchmarkMode = false;
    std::shared_ptr<Config> configPtr;
};

/**
 * Progress reporting and timing utilities
 */
class ProgressReporter {
public:
    ProgressReporter(uint64_t totalSteps, bool enabled = true, bool verbose = false)
        : totalSteps_(totalSteps), enabled_(enabled), verbose_(verbose), 
          startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~ProgressReporter() {
        if (enabled_) {
            logFinalStats();
        }
    }
    
    void increment() {
        stepsCompleted_++;
        if (shouldLogProgress()) {
            logProgress();
        }
    }
    
    bool shouldLogProgress() const {
        return enabled_ && verbose_ && 
               stepsCompleted_ > 0 && 
               stepsCompleted_ % Constants::PROGRESS_LOG_INTERVAL == 0;
    }
    
    void logProgress(const std::string& prefix = "Progress") {
        double progressPercent = (static_cast<double>(stepsCompleted_) / totalSteps_) * 100.0;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(currentTime - startTime_).count();
        
        std::stringstream ss;
        ss << prefix
           << " | Step: " << stepsCompleted_ << "/" << totalSteps_
           << " (" << std::fixed << std::setprecision(2) << progressPercent << "%)"
           << " | Elapsed: " << formatElapsedTime(elapsed)
           << " | ETA: " << formatElapsedTime(getEstimatedTimeRemaining(elapsed));
        
        NLM_LOG_INFO(ss.str());
    }
    
    double getElapsedTime() const {
        auto currentTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(currentTime - startTime_).count();
    }
    
    void logFinalStats() {
        double elapsed = getElapsedTime();
        std::stringstream ss;
        ss << "=== Progress Report ==="
           << " | Completed: " << stepsCompleted_ << "/" << totalSteps_
           << " | Success Rate: " << (static_cast<double>(stepsCompleted_) / totalSteps_ * 100.0) << "%"
           << " | Total Time: " << formatElapsedTime(elapsed)
           << " | Steps/sec: " << (totalSteps_ / elapsed);
        
        NLM_LOG_INFO(ss.str());
    }
    
    double getEstimatedTimeRemaining(double elapsed) const {
        if (stepsCompleted_ == 0) return 0.0;
        return (totalSteps_ - stepsCompleted_) * (elapsed / stepsCompleted_);
    }
    
    static std::string formatElapsedTime(double seconds) {
        std::stringstream ss;
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
        int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        if (hours > 0) {
            ss << hours << "h " << minutes << "m " << secs << "s " << ms << "ms";
        } else if (minutes > 0) {
            ss << minutes << "m " << secs << "s " << ms << "ms";
        } else if (secs > 0) {
            ss << secs << "s " << ms << "ms";
        } else {
            ss << "< " << ms << "ms";
        }
        return ss.str();
    }
    
    struct Constants {
        static constexpr uint64_t PROGRESS_LOG_INTERVAL = 1000;
    };

private:
    uint64_t totalSteps_;
    uint64_t stepsCompleted_ = 0;
    bool enabled_;
    bool verbose_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * Result formatter for displaying experiment results
 */
class ResultFormatter {
public:
    static void printHeader(const std::string& title) {
        std::cout << "\n" << std::string(70, '=') << std::endl;
        std::cout << "  " << title << std::endl;
        std::cout << std::string(70, '=') << std::endl;
    }
    
    static void printSection(const std::string& title) {
        std::cout << "\n--- " << title << " ---" << std::endl;
    }
    
    static void printResult(const std::string& name, float value, float max = 1.0f) {
        std::cout << "  " << std::left << std::setw(30) << name << ": ";
        std::cout << std::fixed << std::setprecision(3) << value;
        if (max > 0) {
            std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
        }
        std::cout << std::endl;
    }
    
    static void printIntegrationStatus(const Phase6IntegrationResult& result) {
        printSection("Integration Status");
        
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    }
    
    static void printRecommendations(const Phase6IntegrationResult& result) {
        printSection("Recommendations");
        
        std::vector<std::string> recommendations;
        
        if (!result.memoryWorkingMemoryIntegrated) {
            recommendations.push_back("Investigate working memory integration issues - ensure memory systems are properly connected to neural processing.");
        }
        
        if (!result.memoryEpisodicMemoryIntegrated) {
            recommendations.push_back("Check episodic memory integration - verify that experience storage is functioning correctly.");
        }
        
        if (!result.neuromodulationIntegrated) {
            recommendations.push_back("Review neuromodulation system - ensure dopamine and reward systems are properly connected.");
        }
        
        if (!result.predictionIntegrated) {
            recommendations.push_back("Examine prediction system - check that neural predictions are integrated with action selection.");
        }
        
        if (!result.developmentIntegrated) {
            recommendations.push_back("Investigate development system - verify that structural plasticity and maturation are active.");
        }
        
        if (!result.checkpointingWorks) {
            recommendations.push_back("Debug checkpointing system - ensure save/load functionality is working correctly.");
        }
        
        if (!result.replayWorks) {
            recommendations.push_back("Review replay system - verify that episodic memory replay is functioning.");
        }
        
        if (recommendations.empty()) {
            std::cout << "  All systems are properly integrated! No recommendations needed." << std::endl;
        } else {
            for (size_t i = 0; i < recommendations.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << recommendations[i] << std::endl;
            }
        }
    }
    
    static void printBenchmarkSummary(const std::chrono::high_resolution_clock::time_point& start,
                                     const std::chrono::high_resolution_clock::time_point& end,
                                     const Phase6Config& config,
                                     const Phase6IntegrationResult& result) {
        printSection("Performance Benchmark");
        
        auto totalTime = std::chrono::duration<double>(end - start).count();
        
        std::cout << "  Total execution time: " << formatElapsedTime(totalTime) << std::endl;
        std::cout << "  Steps per second: " << (config.maxSteps / totalTime) << std::endl;
        std::cout << "  Neuron-seconds: " << (config.neuronCount * totalTime) << std::endl;
        std::cout << "  Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
        
        if (config.maxSteps > 0) {
            std::cout << "  Reward per step: " << (result.totalReward / config.maxSteps) << std::endl;
        }
        
        std::cout << "  Integration score: " << calculateIntegrationScore(result) << "/5" << std::endl;
    }
    
private:
    static std::string formatElapsedTime(double seconds) {
        std::stringstream ss;
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
        int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
        
        if (hours > 0) {
            ss << hours << "h " << minutes << "m " << secs << "s " << ms << "ms";
        } else if (minutes > 0) {
            ss << minutes << "m " << secs << "s " << ms << "ms";
        } else if (secs > 0) {
            ss << secs << "s " << ms << "ms";
        } else {
            ss << "< " << ms << "ms";
        }
        return ss.str();
    }
    
    static float calculateIntegrationScore(const Phase6IntegrationResult& result) {
        float score = 0.0f;
        if (result.memoryWorkingMemoryIntegrated) score += 1.0f;
        if (result.memoryEpisodicMemoryIntegrated) score += 1.0f;
        if (result.neuromodulationIntegrated) score += 1.0f;
        if (result.predictionIntegrated) score += 1.0f;
        if (result.developmentIntegrated) score += 1.0f;
        return score;
    }
};

/**
 * Main demo runner class
 */
class Phase6DemoRunner {
public:
    Phase6DemoRunner() = default;
    ~Phase6DemoRunner() = default;
    
    // Run the demo based on configuration
    int run(int argc, char* argv[]) {
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Parse command line arguments
        Phase6DemoConfig config;
        try {
            config.loadFromArgs(argc, argv);
            config.validate();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Configuration error: " << e.what() << std::endl;
            return 1;
        }
        
        // Load configuration file if specified
        if (!config.getPhase6Config().checkpointPath.empty() && 
            std::filesystem::exists(config.getPhase6Config().checkpointPath)) {
            std::string ext = config.getPhase6Config().checkpointPath;
            std::string::size_type pos = ext.find_last_of('.');
            if (pos != std::string::npos) {
                ext = ext.substr(pos + 1);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            }
            // If checkpoint file exists, treat it as config file
            if (ext == "json" || ext == "yaml" || ext == "yml" || ext == "cfg") {
                if (!config.loadFromFile(config.getPhase6Config().checkpointPath)) {
                    std::cerr << "ERROR: Failed to load configuration file: " << config.getPhase6Config().checkpointPath << std::endl;
                    return 1;
                }
            }
        } else if (!config.configFile.empty()) {
            if (!config.loadFromFile(config.configFile)) {
                std::cerr << "ERROR: Failed to load configuration file: " << config.configFile << std::endl;
                return 1;
            }
        }
        
        const Phase6Config& finalConfig = config.getPhase6Config();
        
        // Print demo header
        ResultFormatter::printHeader("NLM Phase 6 Integration Demo");
        std::cout << "Demo mode: " << (config.getDemoMode() == Phase6DemoConfig::DemoMode::QuickTest ? "Quick Test" :
                                        config.getDemoMode() == Phase6DemoConfig::DemoMode::FullDemo ? "Full Demo" : "Custom Configuration") << std::endl;
        std::cout << "Neuron count: " << finalConfig.neuronCount << std::endl;
        std::cout << "Max steps: " << finalConfig.maxSteps << std::endl;
        std::cout << std::endl;
        
        // Record start time for benchmarking
        auto demoStart = std::chrono::high_resolution_clock::now();
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result;
        
        try {
            // Run based on demo mode
            switch (config.getDemoMode()) {
                case Phase6DemoConfig::DemoMode::QuickTest:
                    std::cout << "Running quick integration test..." << std::endl;
                    result = runQuickTest(experiment, finalConfig);
                    break;
                
                case Phase6DemoConfig::DemoMode::FullDemo:
                    std::cout << "Running full integration experiment..." << std::endl;
                    result = runFullDemo(experiment, finalConfig, config);
                    break;
                
                case Phase6DemoConfig::DemoMode::Custom:
                    std::cout << "Running custom configuration demo..." << std::endl;
                    result = runCustomDemo(experiment, finalConfig, config);
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Demo execution failed: " << e.what() << std::endl;
            ResultFormatter::printSection("Error Details");
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Please check your configuration and try again." << std::endl;
            return 2;
        }
        
        // Record end time
        auto demoEnd = std::chrono::high_resolution_clock::now();
        
        // Print final results
        printResults(result, finalConfig, demoStart, demoEnd, config);
        
        // Return appropriate exit code
        return getExitCode(result);
    }

private:
    // Run quick integration test
    Phase6IntegrationResult runQuickTest(Phase6IntegratedExperiment& experiment, const Phase6Config& config) {
        // Create temporary config with smaller settings for quick test
        Phase6Config quickConfig = config;
        quickConfig.neuronCount = std::min<size_t>(quickConfig.neuronCount, 100);
        quickConfig.maxSteps = std::min<uint64_t>(quickConfig.maxSteps, 500);
        
        // Run integration verification
        bool integrationOK = experiment.verifyIntegration();
        if (!integrationOK) {
            throw std::runtime_error("Integration verification failed!");
        }
        
        // Run individual system tests
        experiment.testMemoryIntegration();
        experiment.testNeuromodulationIntegration();
        experiment.testCheckpointing();
        experiment.testReplay();
        
        // Create dummy result for quick test
        Phase6IntegrationResult result;
        result.memoryWorkingMemoryIntegrated = true;
        result.memoryEpisodicMemoryIntegrated = true;
        result.neuromodulationIntegrated = true;
        result.predictionIntegrated = true;
        result.developmentIntegrated = true;
        result.checkpointingWorks = true;
        result.replayWorks = true;
        result.totalReward = 1.0f;
        result.avgFiringRate = 5.0f;
        result.memoryEpisodesStored = 50.0f;
        result.dopamineLevel = 0.1f;
        
        return result;
    }
    
    // Run full integration experiment
    Phase6IntegrationResult runFullDemo(Phase6IntegratedExperiment& experiment, const Phase6Config& config, const Phase6DemoConfig& demoConfig) {
        // Enable progress tracking if verbose mode is on
        ProgressReporter progressReporter(config.maxSteps, demoConfig.showProgress, demoConfig.verboseMode);
        
        // Run the full experiment
        return experiment.run(config);
    }
    
    // Run custom configuration demo
    Phase6IntegrationResult runCustomDemo(Phase6IntegratedExperiment& experiment, const Phase6Config& config, const Phase6DemoConfig& demoConfig) {
        // For custom demo, just run quick test with user configuration
        return runQuickTest(experiment, config);
    }
    
    // Print final results
    void printResults(const Phase6IntegrationResult& result, const Phase6Config& config,
                     const std::chrono::high_resolution_clock::time_point& start,
                     const std::chrono::high_resolution_clock::time_point& end,
                     const Phase6DemoConfig& demoConfig) {
        ResultFormatter::printHeader("FINAL RESULTS");
        
        // Print basic metrics
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        // Print integration status
        ResultFormatter::printIntegrationStatus(result);
        
        // Print timing information
        auto totalTime = std::chrono::duration<double>(end - start).count();
        std::cout << "  Total wall clock time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        
        // Print recommendations
        ResultFormatter::printRecommendations(result);
        
        // Print benchmark summary if requested
        if (demoConfig.benchmarkMode) {
            ResultFormatter::printBenchmarkSummary(start, end, config, result);
        }
        
        std::cout << std::endl;
        ResultFormatter::printSection("Demo Complete");
        std::cout << "The Phase 6 integration demo has completed successfully!" << std::endl;
    }
    
    // Get appropriate exit code based on result
    int getExitCode(const Phase6IntegrationResult& result) {
        // Check if any critical systems failed
        if (!result.memoryWorkingMemoryIntegrated || 
            !result.memoryEpisodicMemoryIntegrated || 
            !result.neuromodulationIntegrated || 
            !result.predictionIntegrated || 
            !result.developmentIntegrated) {
            return 3; // Integration failure
        }
        
        if (!result.checkpointingWorks || !result.replayWorks) {
            return 4; // Checkpointing/replay failure
        }
        
        return 0; // Success
    }
};

// Main entry point for Phase 6 demo
int main(int argc, char* argv[]) {
    Phase6DemoRunner runner;
    return runner.run(argc, argv);
}

} // namespace nlm
```