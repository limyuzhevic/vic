// Improved NLM main.cpp with command-line interface and better error handling
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <chrono>

using namespace nlm;

// Help and version functions for command-line interface
void printHelp() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: ./nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --batch-mode            Run in automated mode" << std::endl;
    std::cout << "  --interactive           Run in interactive mode" << std::endl;
    std::cout << "  --experiment=TYPE      Set experiment type (phase2, phase3, phase4, phase5, phase6)" << std::endl;
    std::cout << "  --config=FILE           Load configuration from FILE" << std::endl;
    std::cout << "  --output=FILE           Write output to FILE" << std::endl;
    std::cout << "  --checkpoint=FILE       Save/load checkpoint from FILE" << std::endl;
    std::cout << "  --neurons=N             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=N               Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-development        Disable developmental system" << std::endl;
    std::cout << "  --no-neuromodulation    Disable neuromodulation" << std::endl;
    std::cout << "  --no-memory             Disable memory systems" << std::endl;
    std::cout << "  --no-prediction         Disable prediction systems" << std::endl;
    std::cout << "  --no-learning            Disable learning mechanisms" << std::endl;
    std::cout << "  --visualizations         Enable visualization output" << std::endl;
    std::cout << "  --threads=N             Number of threads (default: auto)" << std::endl;
    std::cout << "  --learning-rate=F       Learning rate (default: 0.01)" << std::endl;
    std::cout << "  --novelty-threshold=F   Novelty detection threshold (default: 0.1)" << std::endl;
    std::cout << "  --curiosity-scale=F     Curiosity scaling factor (default: 0.5)" << std::endl;
    std::cout << "  --dopamine-gain=F       Dopamine gain factor (default: 1.0)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --experiment=phase3 --neurons=1000 --steps=5000" << std::endl;
    std::cout << "  ./nlm --batch-mode --config=my_config.cfg --output=results.json" << std::endl;
    std::cout << "  ./nlm --experiment=phase6 --checkpoint=save_state.bin" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 - Version 0.1.0" << std::endl;
    std::cout << "Build date: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++ Standard: C++20" << std::endl;
    std::cout << "Architecture: Neural Learning Machine with artificial developmental brain" << std::endl;
}

// Command line parser structure
struct CommandLineOptions {
    std::string experimentType = "phase2";
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointPath = "";
    std::string logLevel = "info";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enableMemory = true;
    bool enablePrediction = true;
    bool enableLearning = true;
    bool enableVisualizations = false;
    size_t threadCount = 0;
    float learningRate = 0.01f;
    float noveltyThreshold = 0.1f;
    float curiosityScale = 0.5f;
    float dopamineGain = 1.0f;
    bool batchMode = false;
    bool interactiveMode = false;
    std::vector<std::string> customArgs;
};

CommandLineOptions parseCommandLine(int argc, char** argv) {
    CommandLineOptions opts;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--batch-mode") {
            opts.batchMode = true;
            opts.interactiveMode = false;
        } else if (arg == "--interactive") {
            opts.interactiveMode = true;
            opts.batchMode = false;
        } else if (arg.find("--experiment=") == 0) {
            opts.experimentType = arg.substr(13);
        } else if (arg.find("--config=") == 0) {
            opts.configFile = arg.substr(9);
        } else if (arg.find("--output=") == 0) {
            opts.outputFile = arg.substr(9);
        } else if (arg.find("--checkpoint=") == 0) {
            opts.checkpointPath = arg.substr(12);
        } else if (arg.find("--neurons=") == 0) {
            opts.neuronCount = std::stoull(arg.substr(10));
        } else if (arg.find("--steps=") == 0) {
            opts.maxSteps = std::stoull(arg.substr(8));
        } else if (arg.find("--learning-rate=") == 0) {
            opts.learningRate = std::stof(arg.substr(16));
        } else if (arg.find("--novelty-threshold=") == 0) {
            opts.noveltyThreshold = std::stof(arg.substr(19));
        } else if (arg.find("--curiosity-scale=") == 0) {
            opts.curiosityScale = std::stof(arg.substr(18));
        } else if (arg.find("--dopamine-gain=") == 0) {
            opts.dopamineGain = std::stof(arg.substr(16));
        } else if (arg == "--no-development") {
            opts.enableDevelopment = false;
        } else if (arg == "--no-neuromodulation") {
            opts.enableNeuromodulation = false;
        } else if (arg == "--no-memory") {
            opts.enableMemory = false;
        } else if (arg == "--no-prediction") {
            opts.enablePrediction = false;
        } else if (arg == "--no-learning") {
            opts.enableLearning = false;
        } else if (arg == "--visualizations") {
            opts.enableVisualizations = true;
        } else if (arg.find("--threads=") == 0) {
            opts.threadCount = std::stoull(arg.substr(10));
        } else {
            opts.customArgs.push_back(arg);
        }
    }
    
    return opts;
}

// Enhanced logging setup with multiple output options
void setupLogging(const std::string& level) {
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel;
    if (level == "debug") logLevel = LogLevel::Debug;
    else if (level == "info") logLevel = LogLevel::Info;
    else if (level == "warning") logLevel = LogLevel::Warning;
    else logLevel = LogLevel::Error;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    
    // Add file logger if needed
    if (level != "error") {
        auto fileLogger = std::make_shared<FileLogger>("nlm.log", logLevel);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
}

// Advanced configuration loading with validation and defaults
std::shared_ptr<Config> loadConfiguration(const std::string& configFile, const CommandLineOptions& opts) {
    auto config = std::make_shared<Config>();
    
    // Try to load from file if specified
    if (!opts.configFile.empty() && opts.configFile != "configs/default.cfg") {
        if (config->loadFromFile(opts.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + opts.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + opts.configFile);
            NLM_LOG_INFO("Using defaults");
        }
    }
    
    // Set experiment-specific defaults based on type
    if (opts.experimentType == "phase2") {
        config->set("neuron_count", static_cast<int64_t>(opts.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
    } else if (opts.experimentType == "phase3") {
        config->set("neuron_count", static_cast<int64_t>(opts.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        config->set("max_steps", static_cast<int64_t>(opts.maxSteps), ConfigSource::Default);
    } else if (opts.experimentType == "phase4") {
        config->set("neuron_count", static_cast<int64_t>(opts.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("prediction_enabled", opts.enablePrediction, ConfigSource::Default);
        config->set("working_memory_enabled", opts.enableMemory, ConfigSource::Default);
        config->set("concept_formation_enabled", true, ConfigSource::Default);
        config->set("planner_enabled", true, ConfigSource::Default);
    } else if (opts.experimentType == "phase5") {
        config->set("neuron_count", static_cast<int64_t>(opts.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("enable_multithreading", opts.threadCount > 0, ConfigSource::Default);
        config->set("thread_count", static_cast<int64_t>(opts.threadCount), ConfigSource::Default);
        config->set("enable_development", opts.enableDevelopment, ConfigSource::Default);
        config->set("enable_visualizations", opts.enableVisualizations, ConfigSource::Default);
    } else if (opts.experimentType == "phase6") {
        config->set("neuron_count", static_cast<int64_t>(opts.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);\n        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("enable_checkpointing", !opts.checkpointPath.empty(), ConfigSource::Default);
        config->set("enable_replay", true, ConfigSource::Default);
        config->set("enable_visualizations", opts.enableVisualizations, ConfigSource::Default);
    }
    
    // Apply command-line overrides
    config->set("learning_rate", opts.learningRate, ConfigSource::CommandLine);
    config->set("novelty_threshold", opts.noveltyThreshold, ConfigSource::CommandLine);
    config->set("curiosity_scale", opts.curiosityScale, ConfigSource::CommandLine);
    config->set("dopamine_gain", opts.dopamineGain, ConfigSource::CommandLine);
    
    // Validate configuration
    bool configValid = true;
    auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(neuronCount));
        configValid = false;
    }
    
    if (opts.maxSteps == 0) {
        NLM_LOG_ERROR("Invalid max steps: " + std::to_string(opts.maxSteps));
        configValid = false;
    }
    
    return configValid ? config : nullptr;
}

// Performance metrics collection with detailed statistics
struct PerformanceMetrics {
    double wallClockTime;
    double simulationTime;
    size_t totalSteps;
    size_t totalSpikes;
    size_t memoryUsage;
    float avgFiringRate;
    float peakFiringRate;
    float energyConsumption;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    std::vector<float> firingRateHistory;
    std::vector<float> rewardHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> curiosityHistory;
};

// Learning state tracking for analysis
struct LearningState {
    float totalReward;
    float cumulativeNovelty;
    float cumulativeCuriosity;
    float cumulativeDopamine;
    float predictionError;
    size_t learningEvents;
    std::vector<float> rewardHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> curiosityHistory;
    std::vector<float> dopamineHistory;
    float getAverageReward() const {
        return rewardHistory.empty() ? 0.0f : 
            std::accumulate(rewardHistory.begin(), rewardHistory.end(), 0.0f) / rewardHistory.size();
    }
    
    float getMaxReward() const {
        if (rewardHistory.empty()) return 0.0f;
        return *std::max_element(rewardHistory.begin(), rewardHistory.end());
    }
    
    float getMinReward() const {
        if (rewardHistory.empty()) return 0.0f;
        return *std::min_element(rewardHistory.begin(), rewardHistory.end());
    }
    
    float getRewardVariance() const {
        if (rewardHistory.size() < 2) return 0.0f;
        float mean = getAverageReward();
        float sumSq = 0.0f;
        for (float r : rewardHistory) {
            float diff = r - mean;
            sumSq += diff * diff;
        }
        return sumSq / rewardHistory.size();
    }
};

// Experiment runner with context management
class ExperimentRunner {
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    PerformanceMetrics perfMetrics_;
    LearningState learningState_;
    bool running_;

public:
    ExperimentRunner(const std::shared_ptr<Config>& config) : config_(config), running_(false) {
        initializeMetrics();
    }

    void initializeMetrics() {
        perfMetrics_.startTime = std::chrono::high_resolution_clock::now();
        perfMetrics_.wallClockTime = 0.0;
        perfMetrics_.simulationTime = 0.0;
        perfMetrics_.totalSteps = 0;
        perfMetrics_.totalSpikes = 0;
        perfMetrics_.memoryUsage = 0;
        perfMetrics_.avgFiringRate = 0.0f;
        perfMetrics_.peakFiringRate = 0.0f;
        perfMetrics_.energyConsumption = 0.0f;
        perfMetrics_.firingRateHistory.clear();
        perfMetrics_.rewardHistory.clear();
        perfMetrics_.noveltyHistory.clear();
        perfMetrics_.curiosityHistory.clear();
        
        learningState_.totalReward = 0.0f;
        learningState_.cumulativeNovelty = 0.0f;
        learningState_.cumulativeCuriosity = 0.0f;
        learningState_.cumulativeDopamine = 0.0f;
        learningState_.predictionError = 0.0f;
        learningState_.learningEvents = 0;
        learningState_.rewardHistory.clear();
        learningState_.noveltyHistory.clear();
        learningState_.curiosityHistory.clear();
        learningState_.dopamineHistory.clear();
    }

    bool initialize() {
        NLM_LOG_INFO("=== Initializing NLM " + config_->getOr<std::string>("experiment_type", "Phase2") + " ===");
        
        // Create brain with configuration
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        // Create agent brain interface
        agent_ = std::make_shared<AgentBrain>(brain_);
        
        // Create world
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(20.0f, 20.0f, 16, 16);
        
        // Initialize agent
        agent_->initialize(*world_);
        
        // Configure based on experiment type
        if (config_->getOr<bool>("enable_development", true)) {
            agent_->enableDevelopment(true);
        }
        if (config_->getOr<bool>("enable_neuromodulation", true)) {
            agent_->enableRewardModulation(true);
        }
        agent_->enableStructuralPlasticity(true);
        agent_->enableCuriosity(true);
        
        // Initialize memory systems if enabled
        auto* workingMemory = brain_->getWorkingMemory();
        auto* episodicMemory = brain_->getEpisodicMemory();
        
        running_ = true;
        NLM_LOG_INFO("Initialization complete");
        return true;
    }

    void runExperiment(size_t maxSteps) {
        NLM_LOG_INFO("Running experiment for " + std::to_string(maxSteps) + " steps...");
        
        SimulationClock clock(config_->getOr<double>("simulation_timestep", 0.001));
        
        for (size_t step = 0; step < maxSteps && running_; ++step) {
            runStep(step, clock.getTime());
            
            // Log progress periodically
            if (step % 500 == 0) {
                logProgress(step);
            }
        }
        
        finalizeMetrics();
        NLM_LOG_INFO("Experiment completed");
    }

    void runStep(size_t step, double time) {
        // Get sensory input from world
        const SensoryPercept& percept = world_->getSensoryPercept();
        
        // Process sensory input through agent brain
        agent_->processSensoryInput(percept);
        
        // Run brain simulation
        brain_->step(step, time);
        
        // Decode motor command
        MotorCommand cmd = agent_->decodeMotorCommand();
        
        // Apply action to world
        ActionResult actionResult = world_->applyMotorCommand(cmd, time);
        
        // Apply reward modulation
        agent_->applyRewardModulation(actionResult.reward, 0.0f);
        
        // Update development
        agent_->updateDevelopment(0.001);
        
        // Collect metrics
        collectMetrics(step, actionResult);
    }

    void collectMetrics(size_t step, const ActionResult& actionResult) {
        perfMetrics_.totalSteps++;
        perfMetrics_.totalSpikes += brain_->getTotalSpikeCount();
        perfMetrics_.avgFiringRate = brain_->getAverageFiringRate();
        perfMetrics_.firingRateHistory.push_back(perfMetrics_.avgFiringRate);
        
        // Update peak firing rate
        if (perfMetrics_.avgFiringRate > perfMetrics_.peakFiringRate) {
            perfMetrics_.peakFiringRate = perfMetrics_.avgFiringRate;
        }
        
        // Track learning state
        learningState_.totalReward += actionResult.reward;
        learningState_.cumulativeNovelty += agent_->getNoveltyLevel();
        learningState_.cumulativeCuriosity += agent_->getCuriosityLevel();
        learningState_.cumulativeDopamine += agent_->getNeuromodulationLevel();
        learningState_.predictionError += std::abs(agent_->getPredictionError());
        learningState_.learningEvents++;
        
        // Store history for analysis
        learningState_.rewardHistory.push_back(actionResult.reward);
        learningState_.noveltyHistory.push_back(agent_->getNoveltyLevel());
        learningState_.curiosityHistory.push_back(agent_->getCuriosityLevel());
        learningState_.dopamineHistory.push_back(agent_->getNeuromodulationLevel());
        
        // Limit history size to prevent memory issues
        const size_t maxHistorySize = 10000;
        if (perfMetrics_.firingRateHistory.size() > maxHistorySize) {
            perfMetrics_.firingRateHistory.erase(perfMetrics_.firingRateHistory.begin());
        }
        if (learningState_.rewardHistory.size() > maxHistorySize) {
            learningState_.rewardHistory.erase(learningState_.rewardHistory.begin());
        }
    }

    void logProgress(size_t step) {
        NLM_LOG_INFO("Step " + std::to_string(step) + 
                    " | Reward: " + std::to_string(learningState_.totalReward / (step + 1)) +
                    " | Firing: " + std::to_string(brain_->getAverageFiringRate()) +
                    " | Novelty: " + std::to_string(agent_->getNoveltyLevel()) +
                    " | Curiosity: " + std::to_string(agent_->getCuriosityLevel()));
    }

    void finalizeMetrics() {
        perfMetrics_.endTime = std::chrono::high_resolution_clock::now();
        perfMetrics_.wallClockTime = std::chrono::duration<double>(perfMetrics_.endTime - perfMetrics_.startTime).count();
        
        // Calculate averages from history
        if (!perfMetrics_.firingRateHistory.empty()) {
            perfMetrics_.avgFiringRate = std::accumulate(perfMetrics_.firingRateHistory.begin(), 
                                                       perfMetrics_.firingRateHistory.end(), 0.0f) / perfMetrics_.firingRateHistory.size();
        }
        
        if (!learningState_.rewardHistory.empty()) {
            learningState_.totalReward = std::accumulate(learningState_.rewardHistory.begin(),
                                                         learningState_.rewardHistory.end(), 0.0f);
        }
    }

    void saveResults(const std::string& outputFile) {
        NLM_LOG_INFO("Saving results to: " + outputFile);
        
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            NLM_LOG_ERROR("Failed to open output file: " + outputFile);
            return;
        }
        
        outFile << "=== NLM Experiment Results ===\n";
        outFile << "Total Reward: " << learningState_.totalReward << "\n";
        outFile << "Average Reward: " << learningState_.getAverageReward() << "\n";
        outFile << "Max Reward: " << learningState_.getMaxReward() << "\n";
        outFile << "Min Reward: " << learningState_.getMinReward() << "\n";
        outFile << "Reward Variance: " << learningState_.getRewardVariance() << "\n";
        outFile << "Final Firing Rate: " << perfMetrics_.avgFiringRate << " Hz\n";
        outFile << "Peak Firing Rate: " << perfMetrics_.peakFiringRate << " Hz\n";
        outFile << "Total Spikes: " << perfMetrics_.totalSpikes << "\n";
        outFile << "Wall Clock Time: " << perfMetrics_.wallClockTime << "s\n";
        outFile << "Experiment Type: " << config_->getOr<std::string>("experiment_type", "phase2") << "\n";
        outFile << "\n=== Learning Metrics ===\n";
        outFile << "Cumulative Novelty: " << learningState_.cumulativeNovelty << "\n";
        outFile << "Cumulative Curiosity: " << learningState_.cumulativeCuriosity << "\n";
        outFile << "Cumulative Dopamine: " << learningState_.cumulativeDopamine << "\n";
        outFile << "Total Learning Events: " << learningState_.learningEvents << "\n";
        
        outFile.close();
        NLM_LOG_INFO("Results saved successfully");
    }

    void stop() {
        running_ = false;
    }

    const PerformanceMetrics& getPerformanceMetrics() const { return perfMetrics_; }
    const LearningState& getLearningState() const { return learningState_; }
    bool isRunning() const { return running_; }
};

int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineOptions opts = parseCommandLine(argc, argv);
    
    // Setup logging
    setupLogging(opts.logLevel);
    
    NLM_LOG_INFO("NLM Neural Learning Machine starting...");
    NLM_LOG_INFO("Version: 0.1.0 | Build: " << __DATE__ << " " << __TIME__ << " | C++20");
    NLM_LOG_INFO("Experiment Type: " << opts.experimentType);
    NLM_LOG_INFO("Batch Mode: " << (opts.batchMode ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Interactive Mode: " << (opts.interactiveMode ? "Enabled" : "Disabled"));
    
    // Load configuration
    auto config = loadConfiguration(opts.configFile, opts);
    if (!config) {
        NLM_LOG_ERROR("Configuration loading failed");
        return 1;
    }
    
    // Set experiment type in config
    config->set("experiment_type", opts.experimentType, ConfigSource::Default);
    
    // Create experiment runner
    ExperimentRunner runner(config);
    
    // Initialize experiment
    if (!runner.initialize()) {
        NLM_LOG_ERROR("Experiment initialization failed");
        return 1;
    }
    
    // Run experiment
    runner.runExperiment(opts.maxSteps);
    
    // Save results if output file specified
    if (!opts.outputFile.empty()) {
        runner.saveResults(opts.outputFile);
    }
    
    // Save checkpoint if requested
    if (!opts.checkpointPath.empty()) {
        if (auto* checkpointSystem = runner.brain_->getCheckpointSystem()) {
            if (checkpointSystem->save(opts.checkpointPath)) {
                NLM_LOG_INFO("Checkpoint saved: " << opts.checkpointPath);
            } else {
                NLM_LOG_ERROR("Failed to save checkpoint");
            }
        }
    }
    
    // Log final results
    const auto& perfMetrics = runner.getPerformanceMetrics();
    const auto& learningState = runner.getLearningState();
    
    NLM_LOG_INFO("=== Final Results ===");
    NLM_LOG_INFO("Total Reward: " << learningState.totalReward);
    NLM_LOG_INFO("Average Reward: " << learningState.getAverageReward());
    NLM_LOG_INFO("Average Firing Rate: " << perfMetrics.avgFiringRate << " Hz");
    NLM_LOG_INFO("Total Spikes: " << perfMetrics.totalSpikes);
    NLM_LOG_INFO("Wall Clock Time: " << perfMetrics.wallClockTime << "s");
    NLM_LOG_INFO("Learning Events: " << learningState.learningEvents);
    
    NLM_LOG_INFO("=== NLM Experiment Complete ===");
    
    return 0;
}

} // namespace nlm
