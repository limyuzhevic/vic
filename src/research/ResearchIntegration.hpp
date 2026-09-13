// Research module integration header
// This file integrates all research modules with the main NLM system

#pragma once

#include "ResearchSystem.hpp"
#include "ResearchConfig.hpp"
#include "ResearchWorkflow.hpp"
#include "ExperimentManager.hpp"
#include "ParallelBatchProcessor.hpp"

// Factory function to create a complete research system
inline std::shared_ptr<nlm::ResearchSystem> createResearchSystem() {
    auto system = std::make_shared<nlm::ResearchSystem>();
    return system;
}

// Convenience function to run a quick research experiment
inline std::string runQuickResearchExperiment(const std::string& configName, 
                                              const std::string& description,
                                              uint64_t seed = 42) {
    auto system = createResearchSystem();
    
    // Load preset configurations
    system->loadPresetConfigs();
    
    // Create and run experiment
    std::string experimentId = system->createExperiment(description, configName);
    bool success = system->runExperiment(experimentId);
    
    if (success) {
        auto experiment = system->getExperiment(experimentId);
        if (experiment) {
            const auto& metrics = experiment->getMetrics();
            double totalReward = 0.0;
            if (metrics.find("total_reward") != metrics.end()) {
                totalReward = metrics.at("total_reward");
            }
            return "Experiment " + experimentId + " completed successfully. Total reward: " + std::to_string(totalReward);
        }
    }
    
    return "Experiment " + experimentId + " failed to complete";
}

// Function to run a batch of research experiments
inline std::string runResearchBatch(const std::vector<std::string>& configNames,
                                   const std::string& experimentDescription) {
    auto system = createResearchSystem();
    system->loadPresetConfigs();
    
    // Create experiments
    std::vector<std::string> experimentIds;
    for (const auto& configName : configNames) {
        std::string expId = system->createExperiment(experimentDescription, configName);
        experimentIds.push_back(expId);
    }
    
    // Run all experiments
    std::string result = "Batch experiment results:\n";
    for (const auto& expId : experimentIds) {
        bool success = system->runExperiment(expId);
        result += "  " + expId + ": " + (success ? "Success" : "Failed") + "\n";
    }
    
    return result;
}

// Function to generate a research report
inline std::string generateResearchReport(const nlm::ResearchSystem& system) {
    std::ostringstream oss;
    oss << "NLM Research System Report\n";
    oss << "===========================" << "\n\n";
    
    // System status
    oss << "System Status:\n";
    oss << "  Running: " << (system.isRunning() ? "Yes" : "No") << "\n";
    oss << "  Paused: " << (system.isPaused() ? "Yes" : "No") << "\n";
    oss << "  Progress: " << system.getSystemProgress() * 100.0 << "%\n\n";
    
    // Available configurations
    oss << "Available Configurations:\n";
    auto configs = system.getAvailableConfigs();
    for (const auto& configName : configs) {
        const nlm::ResearchConfig* config = system.getResearchConfig(configName);
        if (config) {
            oss << "  " << configName << ": " << config->description << "\n";
            oss << "    Neurons: " << config->neuron_count << "\n";
            oss << "    Random Seed: " << config->random_seed << "\n";
        }
    }
    oss << "\n";
    
    // Experiments
    oss << "Experiments:\n";
    auto experiments = system.getAllExperimentResults();
    for (const auto& experiment : experiments) {
        oss << "  " << experiment->getName() << " (Seed: " << experiment->getSeed() << ")\n";
    }
    oss << "\n";
    
    // Batch processing stats
    auto batchStats = system.getBatchStats();
    oss << "Batch Processing:\n";
    oss << "  Total Jobs: " << batchStats.totalJobs << "\n";
    oss << "  Completed: " << batchStats.completedJobs << "\n";
    oss << "  Failed: " << batchStats.failedJobs << "\n";
    oss << "  Progress: " << batchStats.averageProgress << "%\n";
    
    return oss.str();
}

// Function to save research results
inline bool saveResearchResults(const nlm::ResearchSystem& system, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Generate comprehensive report
    std::string report = generateResearchReport(system);
    
    file << report;
    return true;
}

// Research system initialization helper
inline bool initializeResearchSystem(nlm::ResearchSystem& system, const std::string& configFile = "") {
    // Load configuration if specified
    if (!configFile.empty()) {
        bool loaded = system.loadResearchConfig("default", configFile);
        if (!loaded) {
            std::cerr << "Failed to load configuration file: " << configFile << std::endl;
            return false;
        }
    }
    
    // Initialize experiment runner
    auto experimentRunner = std::make_shared<nlm::ExperimentRunner>();
    system.integrateWithExperimentRunner(experimentRunner);
    
    // Initialize batch processor
    auto batchProcessor = std::make_shared<nlm::ParallelBatchProcessor>();
    system.integrateWithBatchProcessor(batchProcessor);
    
    // Initialize workflow manager
    auto workflowManager = std::make_shared<nlm::ResearchWorkflow>();
    system.integrateWithWorkflowManager(workflowManager);
    
    // Initialize experiment manager
    auto experimentManager = std::make_shared<nlm::ExperimentManager>();
    system.integrateWithExperimentManager(experimentManager);
    
    return true;
}

// Research system cleanup helper
inline void cleanupResearchSystem(nlm::ResearchSystem& system) {
    system.stopSystem();
    system.clearExperiments();
    system.clearWorkflows();
    system.clearBatchJobs();
}

// Example usage function
inline void printExampleUsage() {
    std::cout << "=== NLM Research System Example Usage ===\n\n";
    
    std::cout << "1. Create a research system:\n";
    std::cout << "   auto system = createResearchSystem();\n\n";
    
    std::cout << "2. Load preset configurations:\n";
    std::cout << "   system->loadPresetConfigs();\n\n";
    
    std::cout << "3. Run a single experiment:\n";
    std::cout << "   std::string result = runQuickResearchExperiment(\"standard\", \"Test experiment\", 42);\n\n";
    
    std::cout << "4. Run a batch of experiments:\n";
    std::cout << "   std::vector<std::string> configs = {\"minimal\", \"standard\", \"large_scale\"};\n";
    std::cout << "   std::string batchResult = runResearchBatch(configs, \"Batch experiment\");\n\n";
    
    std::cout << "5. Generate and save a report:\n";
    std::cout << "   std::string report = generateResearchReport(system);\n";
    std::cout << "   saveResearchResults(system, \"research_report.txt\");\n\n";
    
    std::cout << "6. Create and execute a research workflow:\n";
    std::cout << "   std::string workflowId = system->createSequentialWorkflow({\"minimal\", \"standard\"});\n";
    std::cout << "   system->executeWorkflow(workflowId);\n\n";
}

} // namespace research