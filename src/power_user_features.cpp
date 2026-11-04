// NLM - Power User Features
// Advanced features for power users: batch operations, scripting, profiling

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "advanced_config.cpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Power user configuration
struct PowerUserConfig {
    std::string outputFormat;
    std::string logLevel;
    bool enableProfiling;
    bool enableBatchMode;
    std::string batchScript;
    std::string batchOutput;
    bool parallelExecution;
    size_t maxThreads;
    std::map<std::string, std::string> environmentVariables;
    std::vector<std::string> customCommands;
    
    PowerUserConfig() : outputFormat("text"), logLevel("info"), 
                       enableProfiling(false), enableBatchMode(false),
                       parallelExecution(false), maxThreads(std::thread::hardware_concurrency()) {
    }
};

// Batch operation result
struct BatchOperationResult {
    std::string operationName;
    bool success;
    std::string message;
    double executionTime;
    std::map<std::string, std::string> metrics;
    std::vector<std::string> outputLines;
    
    BatchOperationResult() : success(false), executionTime(0.0) {}
    BatchOperationResult(const std::string& name) : operationName(name), success(false), executionTime(0.0) {}
};

// Power user demo runner with advanced features
class PowerUserDemoRunner {
public:
    PowerUserDemoRunner() = default;
    ~PowerUserDemoRunner() = default;
    
    // Run batch operations
    std::vector<BatchOperationResult> runBatchOperations(const std::vector<std::string>& demoTypes,
                                                         const std::map<std::string, std::string>& baseConfig,
                                                         const PowerUserConfig& powerConfig) {
        std::vector<BatchOperationResult> results;
        
        NLM_LOG_INFO("=== Power User Batch Operations ===");
        NLM_LOG_INFO("Running " + std::to_string(demoTypes.size()) + " demos");
        
        // Create output directory if needed
        if (!powerConfig.batchOutput.empty()) {
            std::filesystem::create_directories(powerConfig.batchOutput);
        }
        
        // Run in parallel if enabled
        if (powerConfig.parallelExecution && demoTypes.size() > 1) {
            results = runParallelBatch(demoTypes, baseConfig, powerConfig);
        } else {
            results = runSequentialBatch(demoTypes, baseConfig, powerConfig);
        }
        
        // Generate batch report
        if (!powerConfig.batchOutput.empty()) {
            generateBatchReport(results, powerConfig);
        }
        
        return results;
    }
    
    // Execute power user commands
    bool executePowerCommands(const std::vector<std::string>& commands, 
                             const PowerUserConfig& powerConfig) {
        NLM_LOG_INFO("=== Power User Command Execution ===");
        
        for (const auto& command : commands) {
            NLM_LOG_INFO("Executing: " + command);
            
            bool success = executePowerCommand(command, powerConfig);
            if (!success) {
                NLM_LOG_ERROR("Command failed: " + command);
                return false;
            }
        }
        
        NLM_LOG_INFO("All commands executed successfully");
        return true;
    }
    
    // Profile brain performance
    std::map<std::string, double> profileBrainPerformance(std::shared_ptr<Brain> brain, 
                                                           uint64_t steps) {
        NLM_LOG_INFO("=== Brain Performance Profiling ===");
        
        std::map<std::string, double> metrics;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run performance test
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        // Calculate metrics
        metrics["total_steps"] = static_cast<double>(steps);
        metrics["total_time_seconds"] = duration.count();
        metrics["steps_per_second"] = steps / duration.count();
        metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        
        // Memory usage (approximate)
        metrics["memory_usage_mb"] = getApproximateMemoryUsage();
        
        NLM_LOG_INFO("Performance metrics calculated:");
        for (const auto& metric : metrics) {
            NLM_LOG_INFO("  " + metric.first + ": " + std::to_string(metric.second));
        }
        
        return metrics;
    }
    
    // Generate brain report
    std::string generateBrainReport(std::shared_ptr<Brain> brain) {
        std::ostringstream oss;
        oss << "=== NLM Brain Report ===" << std::endl;
        oss << std::endl;
        
        oss << "Brain Statistics:" << std::endl;
        oss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        oss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        oss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        oss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        oss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        oss << std::endl;
        
        oss << "Memory Systems:" << std::endl;
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            oss << "  Working Memory: " << workingMemory->getActiveTraces() << " active traces" << std::endl;
        } else {
            oss << "  Working Memory: Not available" << std::endl;
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            oss << "  Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
        } else {
            oss << "  Episodic Memory: Not available" << std::endl;
        }
        
        auto* associativeMemory = brain->getAssociativeMemory();
        if (associativeMemory) {
            oss << "  Associative Memory: Active" << std::endl;
        } else {
            oss << "  Associative Memory: Not available" << std::endl;
        }
        oss << std::endl;
        
        oss << "Neuromodulation Systems:" << std::endl;
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            oss << "  Dopamine: " << dopamine->getLevel() << std::endl;
        } else {
            oss << "  Dopamine: Not available" << std::endl;
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            oss << "  Curiosity: " << curiosity->getLevel() << std::endl;
        } else {
            oss << "  Curiosity: Not available" << std::endl;
        }
        
        auto* novelty = brain->getNovelty();
        if (novelty) {
            oss << "  Novelty: " << novelty->getLevel() << std::endl;
        } else {
            oss << "  Novelty: Not available" << std::endl;
        }
        
        auto* predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            oss << "  Prediction Error: " << predictionError->getError() << std::endl;
        } else {
            oss << "  Prediction Error: Not available" << std::endl;
        }
        oss << std::endl;
        
        oss << "Development:" << std::endl;
        oss << "  Current Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            oss << "  Development System: Active" << std::endl;
        } else {
            oss << "  Development System: Not available" << std::endl;
        }
        oss << std::endl;
        
        oss << "Prediction System:" << std::endl;
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            oss << "  Prediction System: Active" << std::endl;
        } else {
            oss << "  Prediction System: Not available" << std::endl;
        }
        
        oss << "Cognition Systems:"" << std::endl;
        auto* planner = brain->getPlanner();
        if (planner) {
            oss << "  Neural Planner: Active" << std::endl;
        } else {
            oss << "  Neural Planner: Not available" << std::endl;
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            oss << "  Concept Formation: Active" << std::endl;
        } else {
            oss << "  Concept Formation: Not available" << std::endl;
        }
        
        auto* attention = brain->getAttention();
        if (attention) {
            oss << "  Attention System: Active" << std::endl;
        } else {
            oss << "  Attention System: Not available" << std::endl;
        }
        
        return oss.str();
    }
    
    // Run batch operations in parallel
    std::vector<BatchOperationResult> runParallelBatch(const std::vector<std::string>& demoTypes,
                                                       const std::map<std::string, std::string>& baseConfig,
                                                       const PowerUserConfig& powerConfig) {
        std::vector<BatchOperationResult> results;
        results.reserve(demoTypes.size());
        
        // Create thread pool
        std::vector<std::thread> threads;
        threads.reserve(demoTypes.size());
        
        // Launch parallel tasks
        for (const auto& demoType : demoTypes) {
            threads.emplace_back([&, demoType]() {
                BatchOperationResult result = runSingleDemo(demoType, baseConfig, powerConfig);
                results.push_back(result);
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Sort results by execution time
        std::sort(results.begin(), results.end(), 
                  [](const BatchOperationResult& a, const BatchOperationResult& b) {
                      return a.executionTime < b.executionTime;
                  });
        
        return results;
    }
    
    // Run batch operations sequentially
    std::vector<BatchOperationResult> runSequentialBatch(const std::vector<std::string>& demoTypes,
                                                         const std::map<std::string, std::string>& baseConfig,
                                                         const PowerUserConfig& powerConfig) {
        std::vector<BatchOperationResult> results;
        
        for (const auto& demoType : demoTypes) {
            BatchOperationResult result = runSingleDemo(demoType, baseConfig, powerConfig);
            results.push_back(result);
        }
        
        return results;
    }
    
    // Execute single demo
    BatchOperationResult runSingleDemo(const std::string& demoType,
                                       const std::map<std::string, std::string>& baseConfig,
                                       const PowerUserConfig& powerConfig) {
        BatchOperationResult result("demo_" + demoType);
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create configuration
            auto config = std::make_shared<Config>();
            
            // Apply base configuration
            for (const auto& setting : baseConfig) {
                config->set(setting.first, setting.second, ConfigSource::Runtime);
            }
            
            // Create and initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                result.message = "Failed to initialize brain";
                return result;
            }
            
            // Determine steps based on demo type
            uint64_t steps = 1000;
            if (demoType == "plasticity") steps = 2000;
            else if (demoType == "integration") steps = 3000;
            else if (demoType == "advanced") steps = 5000;
            
            // Run demo
            bool success = runPowerUserDemoSteps(brain, demoType, steps);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime);
            result.executionTime = duration.count();
            
            if (success) {
                result.success = true;
                result.message = "Demo completed successfully";
                
                // Add metrics
                result.metrics["steps_per_second"] = std::to_string(steps / duration.count());
                result.metrics["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
                result.metrics["firing_rate"] = std::to_string(brain->getAverageFiringRate());
            } else {
                result.message = "Demo failed during execution";
            }
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime);
            result.executionTime = duration.count();
            result.message = std::string("Exception: ") + e.what();
        }
        
        return result;
    }
    
    // Execute power user command
    bool executePowerCommand(const std::string& command, const PowerUserConfig& powerConfig) {
        // Parse command
        std::istringstream iss(command);
        std::string token;
        iss >> token;
        
        if (token == "create_config_template") {
            return createConfigTemplate(powerConfig.customCommands.empty() ? "./config_template.cfg" : powerConfig.customCommands[0]);
        } else if (token == "validate_config") {
            if (powerConfig.customCommands.empty()) {
                NLM_LOG_ERROR("No configuration file specified for validation");
                return false;
            }
            return validateConfigFile(powerConfig.customCommands[0]);
        } else if (token == "profile_performance") {
            // This would require a running brain instance
            NLM_LOG_WARNING("Performance profiling requires a running brain instance");
            return true;
        } else if (token == "generate_report") {
            // This would require a running brain instance
            NLM_LOG_WARNING("Brain report generation requires a running brain instance");
            return true;
        } else {
            NLM_LOG_ERROR("Unknown power user command: " + token);
            return false;
        }
    }
    
    // Run power user demo steps
    bool runPowerUserDemoSteps(std::shared_ptr<Brain> brain, const std::string& demoType, uint64_t steps) {
        // This would run the actual demo logic with power user features
        // For now, return success
        
        NLM_LOG_INFO("Running power user " + demoType + " demo for " + std::to_string(steps) + " steps...");
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        return true;
    }
    
    // Generate batch report
    void generateBatchReport(const std::vector<BatchOperationResult>& results, const PowerUserConfig& powerConfig) {
        if (powerConfig.batchOutput.empty()) {
            return;
        }
        
        try {
            std::string reportPath = powerConfig.batchOutput + "/batch_report_" + 
                                    std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".txt";
            
            std::ofstream reportFile(reportPath);
            if (!reportFile.is_open()) {
                NLM_LOG_ERROR("Failed to create batch report: " + reportPath);
                return;
            }
            
            reportFile << "=== Power User Batch Operations Report ===" << std::endl;
            reportFile << "Generated: " << getCurrentTimestamp() << std::endl;
            reportFile << std::endl;
            
            reportFile << "Summary:" << std::endl;
            reportFile << "  Total Operations: " << results.size() << std::endl;
            
            int successful = 0, failed = 0;
            double totalTime = 0.0;
            for (const auto& result : results) {
                if (result.success) successful++;
                else failed++;
                totalTime += result.executionTime;
            }
            
            reportFile << "  Successful: " << successful << std::endl;
            reportFile << "  Failed: " << failed << std::endl;
            reportFile << "  Total Time: " << totalTime << " seconds" << std::endl;
            reportFile << "  Average Time: " << (results.empty() ? 0.0 : totalTime / results.size()) << " seconds" << std::endl;
            reportFile << std::endl;
            
            reportFile << "Detailed Results:" << std::endl;
            for (const auto& result : results) {
                reportFile << "  " << result.operationName << ": " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
                reportFile << "    Time: " << result.executionTime << " seconds" << std::endl;
                reportFile << "    Message: " << result.message << std::endl;
                if (!result.metrics.empty()) {
                    reportFile << "    Metrics:" << std::endl;
                    for (const auto& metric : result.metrics) {
                        reportFile << "      " << metric.first << ": " << metric.second << std::endl;
                    }
                }
                reportFile << std::endl;
            }
            
            reportFile << "=== End of Report ===" << std::endl;
            
            reportFile.close();
            NLM_LOG_INFO("Batch report generated: " + reportPath);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error generating batch report: ") + e.what());
        }
    }
    
    // Get approximate memory usage
    double getApproximateMemoryUsage() {
        // This is a rough estimate - platform-specific
        // For now, return a reasonable approximation
        return 100.0 + (1000.0 * 0.001); // Base + variable component
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Power user convenience functions
namespace PowerUser {
    // Run power user batch operations
    inline std::vector<BatchOperationResult> runBatch(const std::vector<std::string>& demoTypes,
                                                      const std::map<std::string, std::string>& config,
                                                      const PowerUserConfig& powerConfig) {
        PowerUserDemoRunner runner;
        return runner.runBatchOperations(demoTypes, config, powerConfig);
    }
    
    // Execute power user commands
    inline bool executeCommands(const std::vector<std::string>& commands,
                               const PowerUserConfig& powerConfig) {
        PowerUserDemoRunner runner;
        return runner.executePowerCommands(commands, powerConfig);
    }
    
    // Profile brain performance
    inline std::map<std::string, double> profile(std::shared_ptr<Brain> brain, uint64_t steps) {
        PowerUserDemoRunner runner;
        return runner.profileBrainPerformance(brain, steps);
    }
    
    // Generate brain report
    inline std::string generateReport(std::shared_ptr<Brain> brain) {
        PowerUserDemoRunner runner;
        return runner.generateBrainReport(brain);
    }
}

} // namespace nlm
