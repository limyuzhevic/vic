// Advanced NLM Feature Demo
// Demonstrates advanced features and commands for experienced users

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>

// Advanced command-line argument parser
class AdvancedArgumentParser {
private:
    struct Command {
        std::string name;
        std::string description;
        bool hasValue;
        std::string defaultValue;
        std::function<void(const std::string&)> action;
    };
    
    std::vector<Command> commands;
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    
public:
    AdvancedArgumentParser() {
        // Define available commands
        commands = {
            {"--help", "Show this help message", false, "", nullptr},
            {"--config", "Load configuration from file", true, "", nullptr},
            {"--demo-mode", "Demo mode with predefined scenarios", false, "", nullptr},
            {"--benchmark", "Run benchmark tests", false, "", nullptr},
            {"--compare", "Compare different configurations", false, "", nullptr},
            {"--record", "Record simulation to file", true, "", nullptr},
            {"--replay", "Replay from recorded file", true, "", nullptr},
            {"--profile", "Enable performance profiling", false, "", nullptr},
            {"--visualize", "Generate visualization data", false, "", nullptr},
            {"--batch", "Run batch of simulations", false, "", nullptr},
            {"--validate", "Validate configuration", false, "", nullptr},
        };
    }
    
    void parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for option with value (--option value)
            if (arg.find("--") == 0) {
                // Find the option
                for (const auto& cmd : commands) {
                    if (arg == cmd.name) {
                        if (cmd.hasValue && i + 1 < argc) {
                            options[arg] = argv[++i];
                        } else {
                            options[arg] = cmd.defaultValue;
                        }
                        break;
                    }
                }
            } else {
                // Positional argument
                positionalArgs.push_back(arg);
            }
        }
    }
    
    bool hasOption(const std::string& option) const {
        return options.find(option) != options.end();
    }
    
    std::string getOption(const std::string& option, const std::string& defaultValue = "") const {
        auto it = options.find(option);
        return (it != options.end()) ? it->second : defaultValue;
    }
    
    const std::vector<std::string>& getPositionalArgs() const {
        return positionalArgs;
    }
    
    void showHelp() const {
        std::cout << "=== NLM Advanced Feature Commands ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --config FILE             Load configuration from file" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Advanced Features:" << std::endl;
        std::cout << "  --demo-mode              Demo mode with predefined scenarios" << std::endl;
        std::cout << "  --benchmark              Run benchmark tests" << std::endl;
        std::cout << "  --compare                Compare different configurations" << std::endl;
        std::cout << "  --record FILE            Record simulation to file" << std::endl;
        std::cout << "  --replay FILE            Replay from recorded file" << std::endl;
        std::cout << "  --profile                Enable performance profiling" << std::endl;
        std::cout << "  --visualize              Generate visualization data" << std::endl;
        std::cout << "  --batch                  Run batch of simulations" << std::endl;
        std::cout << "  --validate               Validate configuration" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Example Usage:" << std::endl;
        std::cout << "  nlm_advanced --demo-mode --benchmark" << std::endl;
        std::cout << "  nlm_advanced --config my_config.json --visualize" << std::endl;
        std::cout << "  nlm_advanced --batch --compare --profile" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Predefined Demo Scenarios:" << std::endl;
        std::cout << "  1. Curiosity-Driven Exploration" << std::endl;
        std::cout << "  2. Reward Maximization" << std::endl;
        std::cout << "  3. Balanced Learning" << std::endl;
        std::cout << "  4. Development Simulation" << std::endl;
        std::cout << "  5. Memory Integration Test" << std::endl;
        std::cout << std::endl;
    }
};

// Advanced configuration helper
class AdvancedConfig {
public:
    static Phase6Config createDemoConfig(int scenario) {
        Phase6Config config;
        
        switch (scenario) {
            case 1: { // Curiosity-Driven Exploration
                config.neuronCount = 800;
                config.maxSteps = 3000;
                config.enableCheckpointing = true;
                config.enableReplay = true;
                config.enableDevelopment = true;
                config.setParameter("neuromodulation.curiosity.enable", 1.0f);
                config.setParameter("neuromodulation.novelty.enable", 1.0f);
                config.setParameter("plasticity.curiosity.weight", 0.3f);
                break;
            }
            case 2: { // Reward Maximization
                config.neuronCount = 1000;
                config.maxSteps = 2000;
                config.enableCheckpointing = false;
                config.enableReplay = false;
                config.enableDevelopment = false;
                config.setParameter("neuromodulation.dopamine.scale", 2.0f);
                config.setParameter("neuromodulation.reward.scale", 1.5f);
                config.setParameter("plasticity.stdp.learning_rate", 0.002f);
                break;
            }
            case 3: { // Balanced Learning
                config.neuronCount = 600;
                config.maxSteps = 2500;
                config.enableCheckpointing = true;
                config.enableReplay = true;
                config.enableDevelopment = true;
                config.setParameter("neuromodulation.curiosity.enable", 0.7f);
                config.setParameter("neuromodulation.dopamine.scale", 1.0f);
                config.setParameter("plasticity.stdp.learning_rate", 0.0015f);
                config.setParameter("plasticity.hebbian.learning_rate", 0.0005f);
                break;
            }
            case 4: { // Development Simulation
                config.neuronCount = 700;
                config.maxSteps = 4000;
                config.enableCheckpointing = true;
                config.enableReplay = false;
                config.enableDevelopment = true;
                config.setParameter("development.initial.plasticity", 1.5f);
                config.setParameter("development.critical_period.plasticity", 0.6f);
                config.setParameter("development.maturation.plasticity", 0.3f);
                config.setParameter("development.adult.plasticity", 0.1f);
                break;
            }
            case 5: { // Memory Integration Test
                config.neuronCount = 900;
                config.maxSteps = 3500;
                config.enableCheckpointing = true;
                config.enableReplay = true;
                config.enableDevelopment = true;
                config.setParameter("memory.working_memory.capacity", 50);
                config.setParameter("memory.episodic.capacity", 200);
                config.setParameter("memory.associative.strength", 0.8f);
                break;
            }
            default: { // Default balanced config
                config.neuronCount = 500;
                config.maxSteps = 2000;
                config.enableCheckpointing = true;
                config.enableReplay = true;
                config.enableDevelopment = true;
                break;
            }
        }
        
        return config;
    }
    
    static void applyAdvancedParameters(Phase6Config& config, const std::map<std::string, float>& params) {
        for (const auto& param : params) {
            config.setParameter(param.first, param.second);
        }
    }
};

// Performance profiler
class PerformanceProfiler {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::vector<std::chrono::duration<double>> stepTimes;
    double totalTime;
    
public:
    PerformanceProfiler() : totalTime(0.0) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        stepTimes.push_back(duration);
        totalTime += duration.count();
    }
    
    void printReport() const {
        std::cout << "=== Performance Profile ===" << std::endl;
        std::cout << "Total time: " << totalTime << "s" << std::endl;
        std::cout << "Number of steps: " << stepTimes.size() << std::endl;
        
        if (!stepTimes.empty()) {
            double avgTime = totalTime / stepTimes.size();
            double minTime = stepTimes[0].count();
            double maxTime = stepTimes[0].count();
            
            for (const auto& t : stepTimes) {
                double time = t.count();
                if (time < minTime) minTime = time;
                if (time > maxTime) maxTime = time;
            }
            
            std::cout << "Average step time: " << avgTime * 1000 << "ms" << std::endl;
            std::cout << "Min step time: " << minTime * 1000 << "ms" << std::endl;
            std::cout << "Max step time: " << maxTime * 1000 << "ms" << std::endl;
            std::cout << "Steps per second: " << stepTimes.size() / totalTime << std::endl;
        }
    }
};

// Visualization data generator
class VisualizationGenerator {
public:
    static void generateMetricsFile(const std::string& filename, const Phase6Result& result) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Advanced Visualization Data" << std::endl;
        file << "===============================" << std::endl;
        file << std::endl;
        
        file << "Final Results:" << std::endl;
        file << "  Total Reward: " << result.totalReward << std::endl;
        file << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
        file << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        file << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        file << std::endl;
        
        file << "Integration Status:" << std::endl;
        file << "  Working Memory Integrated: " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << std::endl;
        file << "  Episodic Memory Integrated: " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << std::endl;
        file << "  Neuromodulation Integrated: " << (result.neuromodulationIntegrated ? "true" : "false") << std::endl;
        file << "  Prediction Integrated: " << (result.predictionIntegrated ? "true" : "false") << std::endl;
        file << "  Development Integrated: " << (result.developmentIntegrated ? "true" : "false") << std::endl;
        file << "  Checkpointing Works: " << (result.checkpointingWorks ? "true" : "false") << std::endl;
        file << std::endl;
        
        file << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
        file << std::endl;
        
        file.close();
        std::cout << "Visualization data written to: " << filename << std::endl;
    }
};

// Simulation recorder
class SimulationRecorder {
private:
    std::vector<Phase6StepData> recordedSteps;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    void startRecording() {
        startTime = std::chrono::high_resolution_clock::now();
        recordedSteps.clear();
        std::cout << "Recording simulation..." << std::endl;
    }
    
    void recordStep(const Phase6StepData& step) {
        recordedSteps.push_back(step);
    }
    
    bool saveToFile(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }
        
        // Write header
        uint32_t stepCount = recordedSteps.size();
        file.write(reinterpret_cast<const char*>(&stepCount), sizeof(stepCount));
        
        // Write each step
        for (const auto& step : recordedSteps) {
            // In a real implementation, we would serialize the step data
            // For simplicity, we'll just write a placeholder
            file.write("STEP", 4);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        std::cout << "Recorded " << stepCount << " steps in " << duration.count() << "s" << std::endl;
        std::cout << "Data saved to: " << filename << std::endl;
        
        file.close();
        return true;
    }
};

int main(int argc, char* argv[]) {
    AdvancedArgumentParser parser;
    parser.parse(argc, argv);
    
    if (parser.hasOption("--help")) {
        parser.showHelp();
        return 0;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== NLM Advanced Feature Demo ===" << std::endl;
    
    // Check for config file
    std::string configFile = parser.getOption("--config");
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        // In a real implementation, we would load the config file
    }
    
    // Check for demo mode
    if (parser.hasOption("--demo-mode")) {
        int scenario = 1; // Default to curiosity-driven exploration
        
        if (!parser.getPositionalArgs().empty()) {
            try {
                scenario = std::stoi(parser.getPositionalArgs()[0]);
            } catch (...) {
                std::cerr << "Invalid scenario number. Using default (1)." << std::endl;
            }
        }
        
        std::cout << "Running Demo Scenario: " << scenario << std::endl;
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        
        // Create demo configuration
        Phase6Config config = AdvancedConfig::createDemoConfig(scenario);
        
        std::cout << "Running advanced demo..." << std::endl;
        Phase6Result result = experiment.run(config);
        
        std::cout << "=== Demo Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        
        // Generate visualization if requested
        if (parser.hasOption("--visualize")) {
            std::string filename = "demo_visualization.json";
            VisualizationGenerator::generateMetricsFile(filename, result);
        }
        
        return 0;
    }
    
    // Check for benchmark mode
    if (parser.hasOption("--benchmark")) {
        std::cout << "Running benchmark tests..." << std::endl;
        
        PerformanceProfiler profiler;
        Phase6IntegratedExperiment experiment;
        
        // Run multiple benchmarks
        for (int benchmark = 1; benchmark <= 3; ++benchmark) {
            std::cout << "Benchmark " << benchmark << "/3" << std::endl;
            
            Phase6Config config;
            config.neuronCount = 300 * benchmark;  // Different sizes
            config.maxSteps = 1000;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
            
            profiler.start();
            Phase6Result result = experiment.run(config);
            profiler.stop();
            
            std::cout << "  Result: " << result.totalReward << " reward" << std::endl;
        }
        
        profiler.printReport();
        return 0;
    }
    
    // Check for compare mode
    if (parser.hasOption("--compare")) {
        std::cout << "Comparing different configurations..." << std::endl;
        
        Phase6IntegratedExperiment experiment;
        
        // Define different configurations to compare
        struct ComparisonConfig {
            std::string name;
            Phase6Config config;
        };
        
        std::vector<ComparisonConfig> configs = {
            {"Small", AdvancedConfig::createDemoConfig(5)},
            {"Medium", AdvancedConfig::createDemoConfig(3)},
            {"Large", AdvancedConfig::createDemoConfig(1)},
        };
        
        std::cout << "Configuration Comparison:" << std::endl;
        for (const auto& c : configs) {
            Phase6Config config = c.config;
            config.neuronCount *= 0.5; // Adjust sizes for fair comparison
            
            Phase6Result result = experiment.run(config);
            
            std::cout << "  " << c.name << ": " << result.totalReward << " reward, " 
                      << result.avgFiringRate << " avg firing rate" << std::endl;
        }
        
        return 0;
    }
    
    // Default: run Phase 6 integration demo
    std::cout << "Running Phase 6 Integration Demo..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
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
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
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
    
    return 0;
}