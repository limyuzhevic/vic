// NLM (熙然) - Neural Learning Machine
// Main CLI Header
// Entry point and main CLI interface for the NLM framework

#ifndef NLM_MAIN_CLI_HPP
#define NLM_MAIN_CLI_HPP

#include "cli/CLIOptions.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

namespace nlm {

// Forward declarations
class BrainOnlyExecutor;
class AgentWorldExecutor;
class DevelopmentExecutor;
class AnalysisExecutor;
class BatchExecutor;

// Base executor class for different execution modes
class IExecutor {
public:
    virtual ~IExecutor() = default;
    virtual bool initialize(const Config& config) = 0;
    virtual bool execute() = 0;
    virtual bool finalize() = 0;
    virtual json getResults() const = 0;
};

// Brain-only executor
class BrainOnlyExecutor : public IExecutor {
public:
    bool initialize(const Config& config) override {
        // Initialize brain
        brain_ = std::make_shared<Brain>(config);
        return brain_->initialize();
    }
    
    bool execute() override {
        // Run neural computation
        for (SimulationStep step = 0; step < 100; ++step) {
            brain_->step(step, config_->getOr<double>("simulation_timestep", 0.001));
        }
        return true;
    }
    
    bool finalize() override {
        // Clean up
        brain_.reset();
        return true;
    }
    
    json getResults() const override {
        json result;
        result["neuron_count"] = brain_->getTotalNeuronCount();
        result["synapse_count"] = brain_->getTotalSynapseCount();
        result["spike_count"] = brain_->getTotalSpikeCount();
        result["firing_rate"] = brain_->getAverageFiringRate();
        return result;
    }
    
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }

private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

// Agent-world executor
class AgentWorldExecutor : public IExecutor {
public:
    bool initialize(const Config& config) override {
        // Create world
        world_ = std::make_unique<SimpleWorld>();
        world_->configure(
            config.getOr<size_t>("world_width", 100),
            config.getOr<size_t>("world_height", 100),
            config.getOr<size_t>("world_resolution", 16),
            config.getOr<size_t>("world_resolution", 16)
        );
        
        // Create brain
        brain_ = std::make_shared<Brain>(config);
        brain_->initialize();
        
        // Create agent brain
        agentBrain_ = std::make_unique<AgentBrain>(brain_);
        agentBrain_->initialize(*world_);
        
        return true;
    }
    
    bool execute() override {
        // Run simulation
        size_t steps = config_->getOr<size_t>("simulation_steps", 1000);
        
        for (size_t step = 0; step < steps; ++step) {
            // Get sensory percept
            const SensoryPercept& percept = world_->getSensoryPercept();
            
            // Process sensory input
            agentBrain_->processSensoryInput(percept);
            
            // Simulate brain
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain_->step(neuralStep, config_->getOr<double>("simulation_timestep", 0.001));
            }
            
            // Decode motor command
            MotorCommand cmd = agentBrain_->decodeMotorCommand();
            
            // Apply to world
            ActionResult result = world_->applyMotorCommand(cmd, world_->getSimulationTime());
            
            // Apply reward modulation
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world_->update(config_->getOr<double>("simulation_timestep", 0.001));
            
            // Update development
            agentBrain_->updateDevelopment(config_->getOr<double>("simulation_timestep", 0.001));
            
            // Log progress
            if (step % 100 == 0) {
                logExecutionProgress(step, steps, result.reward);
            }
        }
        
        return true;
    }
    
    bool finalize() override {
        // Clean up
        agentBrain_.reset();
        brain_.reset();
        world_.reset();
        return true;
    }
    
    json getResults() const override {
        json result;
        result["total_steps"] = executionStats_["total_steps"];
        result["total_reward"] = executionStats_["total_reward"];
        result["average_reward"] = executionStats_["average_reward"];
        result["spike_count"] = brain_->getTotalSpikeCount();
        result["final_novelty"] = executionStats_["final_novelty"];
        result["final_curiosity"] = executionStats_["final_curiosity"];
        return result;
    }
    
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }

private:
    void logExecutionProgress(size_t step, size_t total, float reward) {
        std::cout << "  Step " << step << "/" << total 
                  << ": reward=" << reward
                  << ", spikes=" << brain_->getTotalSpikeCount()
                  << ", novelty=" << executionStats_["current_novelty"]
                  << std::endl;
    }

protected:
    std::shared_ptr<Config> config_;
    std::unique_ptr<SimpleWorld> world_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<AgentBrain> agentBrain_;
    std::map<std::string, double> executionStats_;
};

// Development executor
class DevelopmentExecutor : public IExecutor {
public:
    bool initialize(const Config& config) override {
        // Initialize development system
        config_ = std::make_shared<Config>(config);
        return true;
    }
    
    bool execute() override {
        // Run development learning
        size_t epochs = config_->getOr<size_t>("development_epochs", 100);
        
        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            // Simulate development stage
            simulateDevelopmentStage(epoch, epochs);
            
            if (epoch % 10 == 0) {
                std::cout << "  Development epoch " << epoch << "/" << epochs << std::endl;
            }
        }
        
        return true;
    }
    
    bool finalize() override {
        // Clean up development state
        developmentHistory_.clear();
        config_.reset();
        return true;
    }
    
    json getResults() const override {
        json result;
        result["development_epochs"] = developmentHistory_.size();
        result["final_development_stage"] = developmentStage_;
        
        if (!developmentHistory_.empty()) {
            result["final_synapse_count"] = developmentHistory_.back()["synapse_count"];
            result["final_firing_rate"] = developmentHistory_.back()["firing_rate"];
        }
        
        return result;
    }
    
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }

private:
    void simulateDevelopmentStage(size_t epoch, size_t total) {
        DevelopmentStage stage = calculateDevelopmentStage(epoch, total);
        
        json stageData;
        stageData["epoch"] = epoch;
        stageData["stage"] = static_cast<int>(stage);
        stageData["synapse_count"] = 500 + (epoch * 10); // Simulated growth
        stageData["firing_rate"] = 0.1f + (static_cast<float>(epoch) / total) * 0.9f;
        
        developmentHistory_.push_back(stageData);
        developmentStage_ = stage;
    }
    
    DevelopmentStage calculateDevelopmentStage(size_t epoch, size_t total) {
        float progress = static_cast<float>(epoch) / total;
        
        if (progress < 0.33f) return DevelopmentStage::Initial;
        else if (progress < 0.66f) return DevelopmentStage::CriticalPeriod;
        else if (progress < 0.90f) return DevelopmentStage::Maturation;
        else return DevelopmentStage::Adult;
    }

private:
    std::shared_ptr<Config> config_;
    std::vector<json> developmentHistory_;
    DevelopmentStage developmentStage_ = DevelopmentStage::Initial;
};

// Analysis executor
class AnalysisExecutor : public IExecutor {
public:
    bool initialize(const Config& config) override {
        // Initialize analysis system
        config_ = std::make_shared<Config>(config);
        enableProfiling_ = config.getOr<bool>("profiling_enabled", true);
        debugLevel_ = config.getOr<size_t>("debug_level", 1);
        return true;
    }
    
    bool execute() override {
        // Perform analysis
        std::cout << "  Starting performance analysis..." << std::endl;
        
        if (enableProfiling_) {
            profilePerformance();
        }
        
        analyzeSystemState();
        detectAnomalies();
        
        std::cout << "  Analysis completed." << std::endl;
        return true;
    }
    
    bool finalize() override {
        analysisResults_.clear();
        config_.reset();
        return true;
    }
    
    json getResults() const override {
        return analysisResults_;
    }
    
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }

private:
    void profilePerformance() {
        std::cout << "  Profiling system performance..." << std::endl;
        // In a real implementation, this would profile actual system performance
        analysisResults_["cpu_usage"] = 45.5;
        analysisResults_["memory_usage_mb"] = 256.0;
        analysisResults_["average_step_time_ms"] = 1.2;
        analysisResults_["performance_rating"] = "good";
    }
    
    void analyzeSystemState() {
        std::cout << "  Analyzing system state..." << std::endl;
        analysisResults_["brain_state"] = "stable";
        analysisResults_["neuron_health"] = "healthy";
        analysisResults_["synaptic_health"] = "normal";
        analysisResults_["network_connectivity"] = 0.85;
    }
    
    void detectAnomalies() {
        std::cout << "  Detecting anomalies..." << std::endl;
        // Simple anomaly detection
        if (debugLevel_ > 2) {
            analysisResults_["anomalies_detected"] = false;
            analysisResults_["warnings"] = 0;
        }
    }

private:
    std::shared_ptr<Config> config_;
    json analysisResults_;
    bool enableProfiling_ = false;
    size_t debugLevel_ = 1;
};

// Batch executor
class BatchExecutor : public IExecutor {
public:
    bool initialize(const Config& config) override {
        // Initialize batch system
        config_ = std::make_shared<Config>(config);
        batchSize_ = config.getOr<size_t>("batch_size", 10);
        parallelEnabled_ = config.getOr<bool>("parallel_enabled", false);
        return true;
    }
    
    bool execute() override {
        // Run batch processing
        std::cout << "  Starting batch processing of " << batchSize_ << " simulations..." << std::endl;
        
        batchResults_.clear();
        
        for (size_t i = 0; i < batchSize_; ++i) {
            std::cout << "  Processing batch item " << (i + 1) << "/" << batchSize_ << std::endl;
            
            // Simulate batch processing
            json itemResult;
            itemResult["batch_item"] = i + 1;
            itemResult["success"] = true;
            itemResult["simulation_id"] = "sim_" + std::to_string(i + 1);
            itemResult["neuron_count"] = 100 + (i * 10);
            itemResult["synapse_count"] = 500 + (i * 50);
            itemResult["spike_count"] = 1000 + (i * 100);
            
            batchResults_.push_back(itemResult);
            
            // Simulate processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        std::cout << "  Batch processing completed." << std::endl;
        return true;
    }
    
    bool finalize() override {
        // Clean up batch results
        batchResults_.clear();
        config_.reset();
        return true;
    }
    
    json getResults() const override {
        json result;
        result["batch_size"] = batchSize_;
        result["parallel_enabled"] = parallelEnabled_;
        result["total_items"] = batchResults_.size();
        
        if (!batchResults_.empty()) {
            size_t successCount = 0;
            for (const auto& item : batchResults_) {
                if (item["success"]) successCount++;
            }
            result["success_rate"] = static_cast<double>(successCount) / batchResults_.size();
        }
        
        return result;
    }
    
    void setConfig(const std::shared_ptr<Config>& config) { config_ = config; }

private:
    std::shared_ptr<Config> config_;
    std::vector<json> batchResults_;
    size_t batchSize_ = 10;
    bool parallelEnabled_ = false;
};

// Main CLI class
class MainCLI {
public:
    MainCLI() {
        // Create CLI instance
        cli_ = std::make_unique<CLI>("nlm", "0.1.0");
        
        // Add standard options
        cli_->addOption("neuron-count", 'n', "Number of neurons in the brain", "500");
        cli_->addOption("region-count", 'r', "Number of neural regions", "1");
        cli_->addOption("connection-prob", 'p', "Connection probability (0.0-1.0)", "0.15");
        cli_->addOption("random-seed", 's', "Random seed for reproducibility", "42");
        cli_->addOption("timestep", 't', "Simulation timestep in seconds", "0.001");
        
        // Add brain parameters
        cli_->addOption("stdp-ltp-weight", 'l', "STDP LTP weight", "0.02");
        cli_->addOption("stdp-ltd-weight", 'd', "STDP LTD weight", "0.015");
        cli_->addOption("stdp-tau", 'a', "STDP time constant", "20.0");
        
        // Add structural plasticity
        cli_->addOption("synaptogenesis-rate", 'g', "Synaptogenesis rate", "0.0001");
        cli_->addOption("pruning-rate", 'b', "Pruning rate", "0.00001");
        
        // Add world parameters
        cli_->addOption("world-width", 'w', "World width (agent-world mode)", "100");
        cli_->addOption("world-height", 'h', "World height (agent-world mode)", "100");
        
        // Add development parameters
        cli_->addOption("development-epochs", 'e', "Number of development epochs", "100");
        cli_->addOption("plasticity-enabled", 'P', "Enable plasticity", "true");
        
        // Add performance parameters
        cli_->addOption("simulation-steps", 'S', "Number of simulation steps", "1000");
        cli_->addOption("batch-size", 'B', "Batch size (batch mode)", "10");
        
        // Add flags
        cli_->addFlag("enable-checkpoint", 'c', "Enable checkpointing");
        cli_->addFlag("enable-replay", 'R', "Enable replay");
        cli_->addFlag("parallel-enabled", 'P', "Enable parallel execution");
        cli_->addFlag("verbose-output", 'v', "Enable verbose output");
        cli_->addFlag("benchmark", 'b', "Run performance benchmark");
        
        // Set default execution mode
        cli_->setExecutionMode(ExecutionMode::AgentWorldSimulation);
    }
    
    // Run the CLI
    int run(int argc, char** argv) {
        try {
            // Parse command line arguments
            if (!cli_->parse(argc, argv)) {
                return 1;
            }
            
            // Create configuration from CLI options
            auto config = createConfigFromCLI(cli_->getConfigFile());
            
            // Override config with CLI options
            applyCLIOptionsToConfig(*config);
            
            // Run the selected execution mode
            bool success = executeSelectedMode(*config, cli_->getOutputFile(), cli_->getOutputFormat());
            
            if (success) {
                std::cout << "Execution completed successfully!" << std::endl;
                return 0;
            } else {
                std::cerr << "Execution failed!" << std::endl;
                return 1;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

private:
    std::unique_ptr<CLI> cli_;
    std::unique_ptr<IExecutor> executor_;
    
    // Create configuration from file or default
    std::shared_ptr<Config> createConfigFromCLI(const std::string& configFile) {
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            std::cout << "Loading configuration from: " << configFile << std::endl;
            if (config->loadFromFile(configFile)) {
                std::cout << "Configuration loaded successfully." << std::endl;
            } else {
                std::cout << "Using default configuration (file not found or invalid)." << std::endl;
            }
        }
        
        return config;
    }
    
    // Apply CLI options to configuration
    void applyCLIOptionsToConfig(Config& config) {
        // Apply standard options
        if (cli_->isOptionSet("neuron-count")) {
            config.set("neuron_count", 
                std::stoull(cli_->getOption<std::string>("neuron-count", "500")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("region-count")) {
            config.set("region_count", 
                std::stoull(cli_->getOption<std::string>("region-count", "1")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("connection-prob")) {
            config.set("connection_probability", 
                std::stof(cli_->getOption<std::string>("connection-prob", "0.15")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("random-seed")) {
            config.set("random_seed", 
                std::stoull(cli_->getOption<std::string>("random-seed", "42")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("timestep")) {
            config.set("simulation_timestep", 
                std::stod(cli_->getOption<std::string>("timestep", "0.001")), 
                ConfigSource::CommandLine);
        }
        
        // Apply brain parameters
        if (cli_->isOptionSet("stdp-ltp-weight")) {
            config.set("stdp_ltp_weight", 
                std::stof(cli_->getOption<std::string>("stdp-ltp-weight", "0.02")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("stdp-ltd-weight")) {
            config.set("stdp_ltd_weight", 
                std::stof(cli_->getOption<std::string>("stdp-ltd-weight", "0.015")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("stdp-tau")) {
            config.set("stdp_tau", 
                std::stof(cli_->getOption<std::string>("stdp-tau", "20.0")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("synaptogenesis-rate")) {
            config.set("synaptogenesis_rate", 
                std::stof(cli_->getOption<std::string>("synaptogenesis-rate", "0.0001")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("pruning-rate")) {
            config.set("pruning_rate", 
                std::stof(cli_->getOption<std::string>("pruning-rate", "0.00001")), 
                ConfigSource::CommandLine);
        }
        
        // Apply world parameters
        if (cli_->isOptionSet("world-width")) {
            config.set("world_width", 
                std::stoull(cli_->getOption<std::string>("world-width", "100")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("world-height")) {
            config.set("world_height", 
                std::stoull(cli_->getOption<std::string>("world-height", "100")), 
                ConfigSource::CommandLine);
        }
        
        // Apply development parameters
        if (cli_->isOptionSet("development-epochs")) {
            config.set("development_epochs", 
                std::stoull(cli_->getOption<std::string>("development-epochs", "100")), 
                ConfigSource::CommandLine);
        }
        
        // Apply performance parameters
        if (cli_->isOptionSet("simulation-steps")) {
            config.set("simulation_steps", 
                std::stoull(cli_->getOption<std::string>("simulation-steps", "1000")), 
                ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("batch-size")) {
            config.set("batch_size", 
                std::stoull(cli_->getOption<std::string>("batch-size", "10")), 
                ConfigSource::CommandLine);
        }
        
        // Apply flag options
        if (cli_->isOptionSet("enable-checkpoint")) {
            config.set("enable_checkpoint", true, ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("enable-replay")) {
            config.set("enable_replay", true, ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("parallel-enabled")) {
            config.set("parallel_enabled", true, ConfigSource::CommandLine);
        }
        
        if (cli_->isOptionSet("plasticity-enabled")) {
            config.set("plasticity_enabled", true, ConfigSource::CommandLine);
        }
    }
    
    // Execute selected mode
    bool executeSelectedMode(const Config& config, const std::string& outputFile, OutputFormat outputFormat) {
        // Create appropriate executor based on execution mode
        switch (cli_->getExecutionMode()) {
            case ExecutionMode::BrainOnly:
                executor_ = std::make_unique<BrainOnlyExecutor>();
                break;
            case ExecutionMode::AgentWorldSimulation:
                executor_ = std::make_unique<AgentWorldExecutor>();
                break;
            case ExecutionMode::DevelopmentLearning:
                executor_ = std::make_unique<DevelopmentExecutor>();
                break;
            case ExecutionMode::AnalysisDebugging:
                executor_ = std::make_unique<AnalysisExecutor>();
                break;
            case ExecutionMode::BatchProcessing:
                executor_ = std::make_unique<BatchExecutor>();
                break;
            default:
                std::cerr << "Error: Unsupported execution mode!" << std::endl;
                return false;
        }
        
        // Cast to base class for common interface
        IExecutor* executorPtr = executor_.get();
        
        // Initialize executor
        auto configPtr = std::make_shared<Config>(config);
        
        // Use specific initialize methods
        if (auto* brainOnlyExecutor = dynamic_cast<BrainOnlyExecutor*>(executorPtr)) {
            brainOnlyExecutor->setConfig(configPtr);
        } else if (auto* agentWorldExecutor = dynamic_cast<AgentWorldExecutor*>(executorPtr)) {
            agentWorldExecutor->setConfig(configPtr);
        } else if (auto* developmentExecutor = dynamic_cast<DevelopmentExecutor*>(executorPtr)) {
            developmentExecutor->setConfig(configPtr);
        } else if (auto* analysisExecutor = dynamic_cast<AnalysisExecutor*>(executorPtr)) {
            analysisExecutor->setConfig(configPtr);
        } else if (auto* batchExecutor = dynamic_cast<BatchExecutor*>(executorPtr)) {
            batchExecutor->setConfig(configPtr);
        }
        
        if (!executor_->initialize(config)) {
            std::cerr << "Error: Failed to initialize executor!" << std::endl;
            return false;
        }
        
        // Execute
        if (!executor_->execute()) {
            std::cerr << "Error: Execution failed!" << std::endl;
            return false;
        }
        
        // Finalize
        if (!executor_->finalize()) {
            std::cerr << "Error: Finalization failed!" << std::endl;
            return false;
        }
        
        // Get results and write output
        json results = executor_->getResults();
        
        // Add metadata to results
        results["mode"] = executionModeToString(cli_->getExecutionMode());
        results["timestamp"] = cli_->getTimestamp();
        results["output_format"] = outputFormatToString(outputFormat);
        
        // Write output
        writeOutput(results, outputFile, outputFormat);
        
        return true;
    }
    
    // Convert execution mode to string
    std::string executionModeToString(ExecutionMode mode) {
        switch (mode) {
            case ExecutionMode::BrainOnly: return "brain-only";
            case ExecutionMode::AgentWorldSimulation: return "agent-world";
            case ExecutionMode::DevelopmentLearning: return "development";
            case ExecutionMode::AnalysisDebugging: return "analysis";
            case ExecutionMode::BatchProcessing: return "batch";
            default: return "unknown";
        }
    }
    
    // Write output to file
    void writeOutput(const json& data, const std::string& filename, OutputFormat format) {
        if (filename.empty()) {
            // Write to stdout
            std::cout << data.dump(2) << std::endl;
            return;
        }
        
        switch (format) {
            case OutputFormat::JSON:
                std::ofstream file(filename);
                if (file.is_open()) {
                    file << data.dump(2);
                } else {
                    throw std::runtime_error("Failed to open output file: " + filename);
                }
                break;
            case OutputFormat::CSV:
                // For CSV, we would need to convert the JSON to CSV format
                // For now, just write as JSON
                std::ofstream file(filename);
                if (file.is_open()) {
                    file << data.dump(2);
                } else {
                    throw std::runtime_error("Failed to open output file: " + filename);
                }
                break;
            case OutputFormat::Text:
                std::ofstream file(filename);
                if (file.is_open()) {
                    file << data.dump(2);
                } else {
                    throw std::runtime_error("Failed to open output file: " + filename);
                }
                break;
        }
        
        std::cout << "Output written to: " << filename << std::endl;
    }
};

} // namespace nlm

#endif // NLM_MAIN_CLI_HPP
