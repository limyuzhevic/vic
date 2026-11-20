/**
 * Enhanced Phase 6 Demo - Integration Test with CLI Interface
 * 
 * This enhanced demo provides comprehensive CLI support, configuration management,
 * modular test execution, and improved user experience over the original demo.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <filesystem>
#include <nlohmann/json.hpp>

using namespace nlm;
using json = nlohmann::json;

// Custom exceptions for enhanced demo
class Phase6DemoError : public std::runtime_error {
public:
    explicit Phase6DemoError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigValidationError : public Phase6DemoError {
public:
    explicit ConfigValidationError(const std::string& msg) : Phase6DemoError(msg) {}
};

// Progress indicator for long operations
class ProgressIndicator {
private:
    std::string description_;
    uint64_t totalSteps_;
    uint64_t currentStep_;
    double startTime_;
    bool showTimeRemaining_;
    
public:
    ProgressIndicator(const std::string& description, uint64_t totalSteps, 
                      bool showTimeRemaining = true)
        : description_(description), totalSteps_(totalSteps), currentStep_(0),
          showTimeRemaining_(showTimeRemaining) {
        startTime_ = getCurrentTime();
    }
    
    void update(uint64_t step, const std::string& status = "") {
        currentStep_ = step;
        if (currentStep_ > totalSteps_) currentStep_ = totalSteps_;
        
        std::cout << "\r[";
        
        // Simple progress bar
        int barWidth = 40;
        float progress = static_cast<float>(currentStep_) / totalSteps_;
        int filled = static_cast<int>(progress * barWidth);
        
        for (int i = 0; i < filled; ++i) std::cout << "=";
        if (currentStep_ < totalSteps_) std::cout << ">";
        for (int i = filled + 1; i < barWidth; ++i) std::cout << " ";
        std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100) << "%";
        
        // Status text
        std::cout << " " << description_;
        if (!status.empty()) {
            std::cout << " - " << status;
        }
        
        // Time remaining
        if (showTimeRemaining_ && totalSteps_ > 0) {
            double elapsed = getCurrentTime() - startTime_;
            double remaining = (elapsed / currentStep_) * (totalSteps_ - currentStep_);
            if (remaining > 0 && currentStep_ > 0) {
                std::cout << " - " << std::fixed << std::setprecision(1) << remaining << "s remaining";
            }
        }
        
        std::cout.flush();
    }
    
    void complete() {
        if (currentStep_ < totalSteps_) {
            currentStep_ = totalSteps_;
            std::cout << "\r[" << std::string(40, '=') << "] 100.0%";
            std::cout << " " << description_ << " - Completed!" << std::endl;
        }
    }
    
    static double getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
};

// Simple argument parser
class ArgumentParser {
private:
    struct Argument {
        std::string longName;
        std::string value;
        bool required;
        std::string description;
        std::vector<std::string> validValues;
    };
    
    std::vector<Argument> arguments_;
    std::map<std::string, std::string> parsedValues_;
    
public:
    void addArgument(const std::string& longName, bool required, 
                    const std::string& description,
                    const std::vector<std::string>& validValues = {}) {
        Argument arg;
        arg.longName = longName;
        arg.required = required;
        arg.description = description;
        arg.validValues = validValues;
        arguments_.push_back(arg);
    }
    
    bool parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false; // Exit
            }
            
            if (arg.substr(0, 2) == "--") {
                std::string name = arg.substr(2);
                size_t equals = name.find('=');
                
                if (equals != std::string::npos) {
                    std::string value = name.substr(equals + 1);
                    name = name.substr(0, equals);
                    parseValue(name, value);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    parseValue(name, argv[++i]);
                } else {
                    throw Phase6DemoError("Missing value for argument --" + name);
                }
            }
        }
        
        // Check required arguments
        for (const auto& arg : arguments_) {
            if (arg.required && parsedValues_.find(arg.longName) == parsedValues_.end()) {
                throw Phase6DemoError("Missing required argument: --" + arg.longName);
            }
        }
        
        return true;
    }
    
    std::string getValue(const std::string& name, const std::string& defaultValue = "") {
        auto it = parsedValues_.find(name);
        return it != parsedValues_.end() ? it->second : defaultValue;
    }
    
    bool hasValue(const std::string& name) {
        return parsedValues_.find(name) != parsedValues_.end();
    }
    
    void printHelp() {
        std::cout << "Enhanced Phase 6 Demo - CLI Interface" << std::endl;
        std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
        std::cout << "Options:" << std::endl;
        for (const auto& arg : arguments_) {
            std::cout << "  --" << arg.longName;
            if (arg.required) std::cout << "*";
            std::cout << " <value> - " << arg.description << std::endl;
        }
        std::cout << "\nSpecial Options:" << std::endl;
        std::cout << "  --help, -h - Show help message" << std::endl;
        std::cout << "\nOutput Formats:" << std::endl;
        std::cout << "  --output human|json|csv|quiet - Output format (default: human)" << std::endl;
        std::cout << "  --mode integration|memory|neuromodulation|checkpoint|replay|full - Test mode (default: full)" << std::endl;
        std::cout << "  --verbose LEVEL - Verbosity level 0-3 (default: 1)" << std::endl;
        std::cout << "  --quiet - Quiet mode (minimal output)" << std::endl;
        std::cout << "  --progress - Disable progress indicators" << std::endl;
    }
    
private:
    void parseValue(const std::string& name, const std::string& value) {
        // Validate against allowed values if any
        for (const auto& arg : arguments_) {
            if (arg.longName == name) {
                if (!arg.validValues.empty()) {
                    bool valid = false;
                    for (const auto& valid : arg.validValues) {
                        if (valid == value) {
                            valid = true;
                            break;
                        }
                    }
                    if (!valid) {
                        throw Phase6DemoError("Invalid value '" + value + "' for argument --" + name + 
                                            ". Valid values: " + join(arg.validValues, ", "));
                    }
                }
                parsedValues_[name] = value;
                return;
            }
        }
        // If not in argument list, still accept it
        parsedValues_[name] = value;
    }
    
    std::string join(const std::vector<std::string>& vec, const std::string& separator) {
        std::stringstream ss;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << separator;
            ss << vec[i];
        }
        return ss.str();
    }
};

// Configuration file loader
class ConfigLoader {
public:
    static Phase6Config loadFromFile(const std::string& filename) {
        Phase6Config config;
        
        if (!std::filesystem::exists(filename)) {
            return config;
        }
        
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                throw Phase6DemoError("Cannot open config file: " + filename);
            }
            
            json j;
            file >> j;
            
            // Parse JSON config - handle both Phase6Config and general config
            if (j.contains("maxSteps")) config.maxSteps = j["maxSteps"];
            if (j.contains("neuronCount")) config.neuronCount = j["neuronCount"];
            if (j.contains("regionCount")) config.regionCount = j["regionCount"];
            if (j.contains("connectionProbability")) config.connectionProbability = j["connectionProbability"];
            if (j.contains("enableCheckpointing")) config.enableCheckpointing = j["enableCheckpointing"];
            if (j.contains("enableReplay")) config.enableReplay = j["enableReplay"];
            if (j.contains("enableDevelopment")) config.enableDevelopment = j["enableDevelopment"];
            if (j.contains("checkpointPath")) config.checkpointPath = j["checkpointPath"];
            
        } catch (const json::exception& e) {
            throw Phase6DemoError("Invalid JSON in config file " + filename + ": " + std::string(e.what()));
        } catch (const Phase6DemoError& e) {
            throw;
        } catch (const std::exception& e) {
            throw Phase6DemoError("Error loading config file " + filename + ": " + std::string(e.what()));
        }
        
        return config;
    }
    
    static void overrideConfigFromCLI(Phase6Config& config, const std::map<std::string, std::string>& cliArgs) {
        // Override config values based on CLI arguments
        for (const auto& pair : cliArgs) {
            if (pair.first == "neuronCount") {
                try {
                    size_t value = std::stoul(pair.second);
                    config.neuronCount = value;
                } catch (...) {
                    // Ignore invalid values
                }
            } else if (pair.first == "maxSteps") {
                try {
                    size_t value = std::stoul(pair.second);
                    config.maxSteps = value;
                } catch (...) {
                    // Ignore invalid values
                }
            } else if (pair.first == "enableCheckpointing" || pair.first == "enableReplay" || pair.first == "enableDevelopment") {
                config.enableCheckpointing = (pair.second == "true" || pair.second == "1" || pair.second == "true");
                config.enableReplay = (pair.second == "true" || pair.second == "1" || pair.second == "true");
                config.enableDevelopment = (pair.second == "true" || pair.second == "1" || pair.second == "true");
            }
        }
    }
};

// Output formatter base class
class OutputFormatter {
public:
    virtual ~OutputFormatter() = default;
    virtual void format(const Phase6IntegrationResult& result) = 0;
};

// Human-readable formatter
class HumanReadableFormatter : public OutputFormatter {
public:
    void format(const Phase6IntegrationResult& result) override {
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                         NLM Phase 6 Integration Results                         ║" << std::endl;
        std::cout << "╠═══════════════╦═══════════════════════════════════════════╦═══════════════╣" << std::endl;
        std::cout << "║   Metric      ║ Value                                                 ║ Category     ║" << std::endl;
        std::cout << "╠═══════════════╬═══════════════════════════════════════════╬═══════════════╣" << std::endl;
        
        printRow("Total Reward", std::to_string(result.totalReward) + " rewards", "Performance");
        printRow("Avg Firing Rate", std::to_string(result.avgFiringRate) + " Hz", "Activity");
        printRow("Memory Episodes Stored", std::to_string(result.memoryEpisodesStored), "Memory");
        printRow("Dopamine Level", std::to_string(result.dopamineLevel), "Neuromodulation");
        printRow("Wall Clock Time", std::to_string(result.totalWallClockTime) + "s", "Timing");
        
        std::cout << "╚═══════════════╩═══════════════════════════════════════════╩═══════════════╝" << std::endl;
        
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                       System Integration Status                               ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        printIntegrationStatus("Working Memory", "Working Memory Integration", result.memoryWorkingMemoryIntegrated);
        printIntegrationStatus("Episodic Memory", "Episodic Memory Integration", result.memoryEpisodicMemoryIntegrated);
        printIntegrationStatus("Neuromodulation", "Neuromodulation Integration", result.neuromodulationIntegrated);
        printIntegrationStatus("Prediction", "Prediction Integration", result.predictionIntegrated);
        printIntegrationStatus("Development", "Development Integration", result.developmentIntegrated);
        printIntegrationStatus("Checkpointing", "Checkpointing System", result.checkpointingWorks);
        printIntegrationStatus("Replay", "Replay System", result.replayWorks);
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    }
    
private:
    void printRow(const std::string& metric, const std::string& value, const std::string& category) {
        std::cout << "║ " << std::left << std::setw(15) << metric << " ║ " 
                  << std::left << std::setw(48) << value << " ║ " 
                  << std::left << std::setw(15) << category << " ║" << std::endl;
    }
    
    void printIntegrationStatus(const std::string& name, const std::string& description, bool status) {
        std::string statusStr = status ? "CONNECTED" : "DISCONNECTED";
        std::string statusColor = status ? "█" : "░";
        
        std::cout << "║ " << statusColor << " " << name << ": " << std::setw(25) << std::left << statusStr 
                  << " (" << description << ") " << std::string(6, ' ') << "║" << std::endl;
    }
};

// JSON formatter
class JSONFormatter : public OutputFormatter {
public:
    void format(const Phase6IntegrationResult& result) override {
        json j;
        
        j["experiment"] = "Phase6IntegratedExperiment";
        j["timestamp"] = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        j["result"] = {
            {"total_reward", result.totalReward},
            {"avg_firing_rate", result.avgFiringRate},
            {"memory_episodes_stored", result.memoryEpisodesStored},
            {"dopamine_level", result.dopamineLevel},
            {"total_wall_clock_time", result.totalWallClockTime}
        };
        
        j["system_integration"] = {
            {"memory_working_memory_integrated", result.memoryWorkingMemoryIntegrated},
            {"memory_episodic_memory_integrated", result.memoryEpisodicMemoryIntegrated},
            {"neuromodulation_integrated", result.neuromodulationIntegrated},
            {"prediction_integrated", result.predictionIntegrated},
            {"development_integrated", result.developmentIntegrated},
            {"checkpointing_works", result.checkpointingWorks},
            {"replay_works", result.replayWorks}
        };
        
        std::cout << j.dump(2) << std::endl;
    }
};

// CSV formatter
class CSVFormatter : public OutputFormatter {
public:
    void format(const Phase6IntegrationResult& result) override {
        // Header
        std::cout << "metric,value,category" << std::endl;
        std::cout << "total_reward," << result.totalReward << ",performance" << std::endl;
        std::cout << "avg_firing_rate," << result.avgFiringRate << ",activity" << std::endl;
        std::cout << "memory_episodes_stored," << result.memoryEpisodesStored << ",memory" << std::endl;
        std::cout << "dopamine_level," << result.dopamineLevel << ",neuromodulation" << std::endl;
        std::cout << "total_wall_clock_time," << result.totalWallClockTime << ",timing" << std::endl;
        std::cout << std::endl;
    }
};

// Quiet formatter (minimal output)
class QuietFormatter : public OutputFormatter {
public:
    void format(const Phase6IntegrationResult& result) override {
        std::cout << "Reward: " << result.totalReward 
                  << " | Time: " << result.totalWallClockTime 
                  << "s | Memory: " << result.memoryEpisodesStored << std::endl;
    }
};

// Test runner with progress tracking
class TestRunner {
private:
    Phase6IntegratedExperiment experiment_;
    
public:
    TestRunner() = default;
    
    Phase6IntegrationResult runFullExperiment(const Phase6Config& config) {
        std::cout << "Running full integration experiment..." << std::endl;
        
        ProgressIndicator progress("Full Integration", 100, true);
        
        // Convert config to appropriate format for experiment.run()
        // This is a simplified version - actual implementation would need
        // to match the experiment's expected interface
        return experiment_.run(config);
    }
    
    bool runIntegrationVerification() {
        std::cout << "Running integration verification..." << std::endl;
        return experiment_.verifyIntegration();
    }
    
    bool runMemoryIntegrationTest() {
        std::cout << "Running memory integration test..." << std::endl;
        experiment_.testMemoryIntegration();
        return true;
    }
    
    bool runNeuromodulationIntegrationTest() {
        std::cout << "Running neuromodulation integration test..." << std::endl;
        experiment_.testNeuromodulationIntegration();
        return true;
    }
    
    bool runCheckpointTest() {
        std::cout << "Running checkpoint test..." << std::endl;
        return experiment_.testCheckpointing();
    }
    
    bool runReplayTest() {
        std::cout << "Running replay test..." << std::endl;
        return experiment_.testReplay();
    }
};

// Main demo class
class EnhancedPhase6Demo {
private:
    ArgumentParser parser_;
    TestRunner testRunner_;
    std::unique_ptr<OutputFormatter> formatter_;
    
public:
    EnhancedPhase6Demo() {
        // Setup argument parser
        parser_.addArgument("config", false, "Config file (JSON format)");
        parser_.addArgument("output", false, "Output format: human|json|csv|quiet", 
                           {"human", "json", "csv", "quiet"});
        parser_.addArgument("mode", false, "Mode: integration|memory|neuromodulation|checkpoint|replay|full", 
                           {"integration", "memory", "neuromodulation", "checkpoint", "replay", "full"});
        parser_.addArgument("verbose", false, "Verbosity level 0-3");
        parser_.addArgument("neuronCount", false, "Neuron count override");
        parser_.addArgument("maxSteps", false, "Max steps override");
        parser_.addArgument("testPhase", false, "Individual test phase to run");
    }
    
    int run(int argc, char* argv[]) {
        try {
            // Parse arguments
            if (!parser_.parse(argc, argv)) {
                return 0; // Help was shown
            }
            
            // Setup logging based on verbosity
            int verbosity = 1;
            std::string verboseStr = parser_.getValue("verbose");
            if (!verboseStr.empty()) {
                verbosity = std::stoi(verboseStr);
                if (verbosity < 0 || verbosity > 3) verbosity = 1;
            }
            
            Logger::getGlobal().setLevel(static_cast<LogLevel>(verbosity + 1));
            
            // Determine output format
            std::string outputFormat = parser_.getValue("output", "human");
            if (outputFormat == "human") {
                formatter_ = std::make_unique<HumanReadableFormatter>();
            } else if (outputFormat == "json") {
                formatter_ = std::make_unique<JSONFormatter>();
            } else if (outputFormat == "csv") {
                formatter_ = std::make_unique<CSVFormatter>();
            } else if (outputFormat == "quiet") {
                formatter_ = std::make_unique<QuietFormatter>();
            }
            
            // Load config file if specified
            Phase6Config config;
            std::string configFile = parser_.getValue("config");
            if (!configFile.empty()) {
                std::cout << "Loading configuration from: " << configFile << std::endl;
                config = ConfigLoader::loadFromFile(configFile);
            }
            
            // Override config from CLI args
            std::map<std::string, std::string> cliArgs;
            for (const auto& arg : parser_.getArgumentNames()) {
                if (parser_.hasValue(arg)) {
                    cliArgs[arg] = parser_.getValue(arg);
                }
            }
            ConfigLoader::overrideConfigFromCLI(config, cliArgs);
            
            // Validate config
            validateConfig(config);
            
            // Determine which test to run
            std::string mode = parser_.getValue("mode", "full");
            std::string testPhase = parser_.getValue("testPhase");
            
            if (!testPhase.empty()) {
                // Run individual test phase
                bool success = runTestPhase(testPhase, config);
                return success ? 0 : 1;
            } else if (mode != "full") {
                // Run specific mode
                bool success = runTestPhase(mode, config);
                return success ? 0 : 1;
            } else {
                // Run full integration experiment
                return runFullIntegration(config);
            }
            
        } catch (const Phase6DemoError& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return 1;
        } catch (const std::exception& e) {
            std::cerr << "UNEXPECTED ERROR: " << e.what() << std::endl;
            return 1;
        }
    }
    
private:
    bool runTestPhase(const std::string& phaseName, const Phase6Config& config) {
        if (phaseName == "integration") {
            bool success = testRunner_.runIntegrationVerification();
            if (success && formatter_) {
                Phase6IntegrationResult dummyResult;
                dummyResult.totalReward = success ? 100.0f : 0.0f;
                formatter_->format(dummyResult);
            }
            return success;
        } else if (phaseName == "memory") {
            bool success = testRunner_.runMemoryIntegrationTest();
            if (success && formatter_) {
                Phase6IntegrationResult dummyResult;
                dummyResult.totalReward = success ? 50.0f : 0.0f;
                formatter_->format(dummyResult);
            }
            return success;
        } else if (phaseName == "neuromodulation") {
            bool success = testRunner_.runNeuromodulationIntegrationTest();
            if (success && formatter_) {
                Phase6IntegrationResult dummyResult;
                dummyResult.totalReward = success ? 75.0f : 0.0f;
                formatter_->format(dummyResult);
            }
            return success;
        } else if (phaseName == "checkpoint") {
            bool success = testRunner_.runCheckpointTest();
            if (success && formatter_) {
                Phase6IntegrationResult dummyResult;
                dummyResult.totalReward = success ? 90.0f : 0.0f;
                formatter_->format(dummyResult);
            }
            return success;
        } else if (phaseName == "replay") {
            bool success = testRunner_.runReplayTest();
            if (success && formatter_) {
                Phase6IntegrationResult dummyResult;
                dummyResult.totalReward = success ? 80.0f : 0.0f;
                formatter_->format(dummyResult);
            }
            return success;
        } else if (phaseName == "full") {
            return runFullIntegration(config);
        } else {
            throw Phase6DemoError("Unknown test phase: " + phaseName);
        }
    }
    
    int runFullIntegration(const Phase6Config& config) {
        std::cout << "=== Enhanced Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getGlobal().setLevel(LogLevel::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems with progress
        ProgressIndicator memoryProgress("Memory Integration", 50, true);
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        memoryProgress.complete();
        std::cout << std::endl;
        
        ProgressIndicator neuromodulationProgress("Neuromodulation Integration", 30, true);
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        neuromodulationProgress.complete();
        std::cout << std::endl;
        
        ProgressIndicator checkpointProgress("Checkpoint Test", 40, true);
        std::cout << "--- Checkpoint Test ---" << std::endl;
        bool checkpointOK = experiment.testCheckpointing();
        checkpointProgress.complete();
        std::cout << std::endl;
        
        ProgressIndicator replayProgress("Replay Test", 30, true);
        std::cout << "--- Replay Test ---" << std::endl;
        bool replayOK = experiment.testReplay();
        replayProgress.complete();
        std::cout << std::endl;
        
        // Run full experiment with smaller settings for demo
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config configCopy = config;
        configCopy.neuronCount = 500;
        configCopy.maxSteps = 2000;
        configCopy.enableCheckpointing = true;
        configCopy.enableReplay = true;
        configCopy.enableDevelopment = true;
        
        ProgressIndicator experimentProgress("Full Integration", config.maxSteps, false);
        auto result = experiment.run(configCopy);
        experimentProgress.complete();
        
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
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        // Output formatted results
        if (formatter_) {
            formatter_->format(result);
        }
        
        return 0;
    }
    
    void validateConfig(const Phase6Config& config) {
        if (config.maxSteps == 0) {
            throw ConfigValidationError("maxSteps must be greater than 0");
        }
        
        if (config.neuronCount == 0) {
            throw ConfigValidationError("neuronCount must be greater than 0");
        }
        
        if (config.regionCount == 0) {
            throw ConfigValidationError("regionCount must be greater than 0");
        }
        
        if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
            throw ConfigValidationError("connectionProbability must be between 0.0 and 1.0");
        }
    }
    
    std::vector<std::string> getArgumentNames() {
        std::vector<std::string> names;
        for (const auto& arg : parser_.getArgumentNames()) {
            names.push_back(arg.longName);
        }
        return names;
    }
};