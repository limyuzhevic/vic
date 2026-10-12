// NLM (熙然) - Neural Learning Machine
// Phase 6 Demo CLI - Command-line interface for Phase 6 integration tests

#include "core/CLI/NLMCLI.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>

using namespace nlm;

class Phase6DemoCLI {
public:
    Phase6DemoCLI(const CLIConfigManager& config) : configManager(config) {}
    
    int run(CLIProgressReporter& reporter) {
        reporter.log("=== NLM Phase 6 Integration Demo CLI ===");
        reporter.log("Running Phase 6 integration tests via command line...");
        
        auto startTime = std::chrono::high_resolution_clock::time_point();
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        
        // Get command options
        std::string mode = configManager.getConfig("mode", "full");
        bool quickMode = configManager.getBool("quick", false);
        bool verifyOnly = configManager.getBool("verify", false);
        std::string checkpointPath = configManager.getConfig("checkpoint", "./checkpoint_test.bin");
        int neuronCount = configManager.getInt("neurons", quickMode ? 500 : 1000);
        int maxSteps = configManager.getInt("steps", quickMode ? 2000 : 10000);
        
        if (quickMode) {
            reporter.log("Quick mode enabled - using reduced settings");
            reporter.log("  Neurons: " + std::to_string(neuronCount));
            reporter.log("  Steps: " + std::to_string(maxSteps));
        }
        
        Phase6IntegrationResult finalResult;
        
        if (verifyOnly) {
            reporter.log("=== Integration Verification ===");
            reporter.log("Running quick integration verification...");
            
            bool integrationOK = experiment.verifyIntegration();
            
            reporter.log("");
            if (integrationOK) {
                reporter.log("✓ Integration verification PASSED");
                reporter.log("  All brain systems are properly connected");
            } else {
                reporter.log("✗ Integration verification FAILED");
                reporter.log("  Some systems may not be properly integrated");
                return 1;
            }
            
            finalResult.memoryWorkingMemoryIntegrated = integrationOK;
            finalResult.memoryEpisodicMemoryIntegrated = integrationOK;
            finalResult.neuromodulationIntegrated = integrationOK;
            finalResult.predictionIntegrated = integrationOK;
            finalResult.developmentIntegrated = integrationOK;
            
        } else if (mode == "test") {
            reporter.log("=== Individual System Tests ===");
            
            // Run individual system tests
            reporter.log("--- Memory Integration Test ---");
            bool memoryOK = experiment.testMemoryIntegration();
            finalResult.memoryWorkingMemoryIntegrated = memoryOK;
            finalResult.memoryEpisodicMemoryIntegrated = memoryOK;
            
            reporter.log("--- Neuromodulation Integration Test ---");
            bool neuromodulationOK = experiment.testNeuromodulationIntegration();
            finalResult.neuromodulationIntegrated = neuromodulationOK;
            
            reporter.log("--- Checkpoint Test ---");
            bool checkpointOK = experiment.testCheckpointing();
            finalResult.checkpointingWorks = checkpointOK;
            
            reporter.log("--- Replay Test ---");
            bool replayOK = experiment.testReplay();
            finalResult.replayWorks = replayOK;
            
        } else {
            // Run full integration experiment
            reporter.log("=== Full Integration Experiment ===");
            reporter.log("Running complete Phase 6 integration experiment...");
            
            Phase6Config phase6Config;
            phase6Config.neuronCount = neuronCount;
            phase6Config.maxSteps = maxSteps;
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            phase6Config.checkpointPath = checkpointPath;
            
            startTime = std::chrono::high_resolution_clock::now();
            
            finalResult = experiment.run(phase6Config);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            finalResult.totalWallClockTime = 
                std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        }
        
        // Display comprehensive results
        displayResults(finalResult, reporter);
        
        // Save checkpoint if requested
        if (configManager.hasOption("--save-checkpoint") || configManager.hasOption("--checkpoint")) {
            std::string checkpointFile = configManager.getConfig("--checkpoint", checkpointPath);
            saveCheckpoint(experiment, checkpointFile, reporter);
        }
        
        // Generate report if requested
        if (configManager.hasOption("--generate-report") || configManager.hasOption("--report")) {
            std::string reportFile = configManager.getConfig("--report", "phase6_report.txt");
            generateReport(finalResult, reportFile, reporter);
        }
        
        reporter.log("");
        reporter.log("=== Phase 6 Demo Complete ===");
        
        return 0;
    }
    
private:
    void displayResults(const Phase6IntegrationResult& result, CLIProgressReporter& reporter) {
        reporter.log("");
        reporter.log("=== FINAL RESULTS ===");
        
        // System integration status
        reporter.log("=== Integration Status ===");
        reporter.log("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        reporter.log("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
        reporter.log("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
        reporter.log("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
        reporter.log("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
        reporter.log("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
        reporter.log("Replay: " + std::string(result.replayWorks ? "WORKING" : "NOT WORKING"));
        
        // Metrics
        reporter.log("");
        reporter.log("=== Metrics ===");
        reporter.log("Total reward: " + std::to_string(result.totalReward));
        reporter.log("Average firing rate: " + std::to_string(result.avgFiringRate));
        reporter.log("Average synaptic weight: " + std::to_string(result.avgSynapticWeight));
        reporter.log("Memory episodes stored: " + std::to_string(result.memoryEpisodesStored));
        reporter.log("Novelty level: " + std::to_string(result.noveltyLevel));
        reporter.log("Curiosity level: " + std::to_string(result.curiosityLevel));
        reporter.log("Dopamine level: " + std::to_string(result.dopamineLevel));
        
        // Timing
        reporter.log("");
        reporter.log("=== Timing ===");
        if (result.totalWallClockTime > 0) {
            reporter.log("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
        }
        
        // Overall assessment
        reporter.log("");
        reporter.log("=== Integration Assessment ===");
        
        int integratedSystems = 0;
        if (result.memoryWorkingMemoryIntegrated) integratedSystems++;
        if (result.memoryEpisodicMemoryIntegrated) integratedSystems++;
        if (result.neuromodulationIntegrated) integratedSystems++;
        if (result.predictionIntegrated) integratedSystems++;
        if (result.developmentIntegrated) integratedSystems++;
        if (result.checkpointingWorks) integratedSystems++;
        if (result.replayWorks) integratedSystems++;
        
        reporter.log("Integrated Systems: " + std::to_string(integratedSystems) + "/7");
        
        if (integratedSystems >= 6) {
            reporter.log("✓ EXCELLENT: Almost all systems are properly integrated");
        } else if (integratedSystems >= 4) {
            reporter.log("✓ GOOD: Most systems are integrated");
        } else {
            reporter.log("⚠ WARNING: Limited integration detected");
        }
        
        if (result.totalReward > 0) {
            reporter.log("✓ Learning occurred: Positive reward achieved");
        }
    }
    
    void saveCheckpoint(Phase6IntegratedExperiment& experiment, const std::string& filename, CLIProgressReporter& reporter) {
        reporter.log("Saving checkpoint to: " + filename);
        // TODO: Implement actual checkpoint saving
        reporter.log("Checkpoint saved successfully (placeholder)");
    }
    
    void generateReport(const Phase6IntegrationResult& result, const std::string& filename, CLIProgressReporter& reporter) {
        reporter.log("Generating report to: " + filename);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            reporter.error("Failed to create report file: " + filename);
            return;
        }
        
        file << "NLM Phase 6 Integration Report" << std::endl;
        file << "Generated: " << std::chrono::system_clock::now() << std::endl;
        file << std::endl;
        
        // Write all results
        file << "=== System Integration Status ===" << std::endl;
        file << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        file << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        file << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        file << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        file << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        file << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        file << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        file << std::endl << "=== Metrics ===" << std::endl;
        file << "Total reward: " << result.totalReward << std::endl;
        file << "Average firing rate: " << result.avgFiringRate << std::endl;
        file << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
        file << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        file << "Novelty level: " << result.noveltyLevel << std::endl;
        file << "Curiosity level: " << result.curiosityLevel << std::endl;
        file << "Dopamine level: " << result.dopamineLevel << std::endl;
        
        file.close();
        
        reporter.log("Report generated successfully");
    }
    
private:
    CLIConfigManager& configManager;
};

int main(int argc, char** argv) {
    // Create Phase 6 Demo CLI
    NLMCLI cli(argc, argv, "nlm_phase6");
    
    if (cli.getConfigManager().hasOption("--help") || 
        cli.getConfigManager().getConfig("subcommand") == "help") {
        cli.printUsage();
        return 0;
    }
    
    if (cli.getConfigManager().hasOption("--version")) {
        std::cout << "NLM Phase 6 Demo CLI" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Phase 6 Integration Tests" << std::endl;
        return 0;
    }
    
    // Run Phase 6 demo
    CLIProgressReporter reporter(std::cout, cli.getConfigManager().getBool("--quiet", false));
    Phase6DemoCLI phase6Demo(cli.getConfigManager());
    
    return phase6Demo.run(reporter);
}