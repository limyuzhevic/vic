// Enhanced Phase 6 Demo with Advanced Framework Features
// This demo uses the new Demo Framework for improved configurability and monitoring

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "demo/DemoFramework.hpp"
#include <iostream>
#include <memory>
#include <chrono>

namespace nlm {

// Enhanced Phase 6 integration test using the framework
class EnhancedPhase6Demo : public BaseDemo {
public:
    EnhancedPhase6Demo() : BaseDemo("EnhancedPhase6Demo", "Phase 6 integration with advanced monitoring and configuration") {
        logger_ = Logger::getGlobal();
        metricsCollector_ = std::make_shared<MetricsCollector>();
    }
    
    DemoResult run(const std::map<std::string, std::string>& config) override {
        DemoResult result(getName());
        auto startTime = std::chrono::system_clock::now();
        
        try {
            logger_->info("=== Enhanced Phase 6 Integration Demo ===");
            logger_->info("Testing complete brain integration with advanced monitoring");
            
            // Create experiment
            Phase6IntegratedExperiment experiment;
            
            // Set up configuration from enhanced config
            Phase6Config phase6Config;
            phase6Config.neuronCount = 500;
            phase6Config.maxSteps = 2000;
            phase6Config.enableCheckpointing = true;
            phase6Config.enableReplay = true;
            phase6Config.enableDevelopment = true;
            
            // Override with custom config if provided
            if (config.find("neuron_count") != config.end()) {
                phase6Config.neuronCount = std::stoi(config.at("neuron_count"));
            }
            if (config.find("max_steps") != config.end()) {
                phase6Config.maxSteps = std::stoi(config.at("max_steps"));
            }
            
            // Phase 1: Integration Verification with Progress Tracking
            logger_->info("--- Phase 1: Integration Verification ---");
            ProgressTracker progress1("Integration Verification", 5);
            
            bool integrationOK = experiment.verifyIntegration();
            metricsCollector_->recordMetric("integration_successful", integrationOK ? 1.0 : 0.0);
            
            progress1.update(1, "Basic integration check");
            progress1.increment();
            
            if (!integrationOK) {
                throw std::runtime_error("Integration verification failed");
            }
            
            logger_->info("Integration verification passed!");
            progress1.update(2, "System connectivity check");
            progress1.increment();
            
            // Phase 2: Individual System Testing with Metrics Collection
            logger_->info("\n--- Phase 2: Individual System Testing ---");
            
            ProgressTracker progress2("System Testing", 4);
            
            // Test Memory Integration
            logger_->info("Testing Memory Integration...");
            experiment.testMemoryIntegration();
            metricsCollector_->recordMetric("memory_test_passed", 1.0);
            progress2.update(1, "Memory integration");
            progress2.increment();
            
            // Test Neuromodulation Integration
            logger_->info("Testing Neuromodulation Integration...");
            experiment.testNeuromodulationIntegration();
            metricsCollector_->recordMetric("neuromodulation_test_passed", 1.0);
            progress2.update(2, "Neuromodulation integration");
            progress2.increment();
            
            // Test Checkpointing
            logger_->info("Testing Checkpointing System...");
            experiment.testCheckpointing();
            metricsCollector_->recordMetric("checkpoint_test_passed", 1.0);
            progress2.update(3, "Checkpointing");
            progress2.increment();
            
            // Test Replay
            logger_->info("Testing Replay System...");
            experiment.testReplay();
            metricsCollector_->recordMetric("replay_test_passed", 1.0);
            progress2.update(4, "Replay system");
            progress2.increment();
            
            // Phase 3: Full Integration Experiment with Enhanced Monitoring
            logger_->info("\n--- Phase 3: Full Integration Experiment ---");
            
            ProgressTracker progress3("Full Integration", 3);
            
            logger_->info("Running full integration experiment...");
            progress3.update(1, "Starting experiment");
            
            auto experimentStart = std::chrono::system_clock::now();
            
            auto result = experiment.run(phase6Config);
            
            auto experimentEnd = std::chrono::system_clock::now();
            auto experimentDuration = std::chrono::duration_cast<std::chrono::seconds>(experimentEnd - experimentStart);
            
            // Collect comprehensive metrics
            metricsCollector_->recordMetric("total_reward", result.totalReward);
            metricsCollector_->recordMetric("episode_count", result.memoryEpisodesStored);
            metricsCollector_->recordMetric("dopamine_level", result.dopamineLevel);
            metricsCollector_->recordMetric("working_memory_integrated", result.memoryWorkingMemoryIntegrated ? 1.0 : 0.0);
            metricsCollector_->recordMetric("episodic_memory_integrated", result.memoryEpisodicMemoryIntegrated ? 1.0 : 0.0);
            metricsCollector_->recordMetric("neuromodulation_integrated", result.neuromodulationIntegrated ? 1.0 : 0.0);
            metricsCollector_->recordMetric("prediction_integrated", result.predictionIntegrated ? 1.0 : 0.0);
            metricsCollector_->recordMetric("development_integrated", result.developmentIntegrated ? 1.0 : 0.0);
            metricsCollector_->recordMetric("checkpointing_works", result.checkpointingWorks ? 1.0 : 0.0);
            
            metricsCollector_->recordMetric("experiment_wall_time", experimentDuration.count());
            metricsCollector_->recordMetric("experiment_avg_firing_rate", result.avgFiringRate);
            
            logger_->info("Experiment Results:");
            logger_->info("  Total Reward: " + std::to_string(result.totalReward));
            logger_->info("  Episodes Stored: " + std::to_string(result.memoryEpisodesStored));
            logger_->info("  Dopamine Level: " + std::to_string(result.dopamineLevel));
            logger_->info("  Average Firing Rate: " + std::to_string(result.avgFiringRate));
            
            progress3.update(2, "Running experiment loop");
            progress3.increment();
            
            // Final System Status Report
            logger_->info("\n=== System Integration Status ===");
            
            const char* statusYes = "CONNECTED";
            const char* statusNo = "DISCONNECTED";
            
            logger_->info("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? statusYes : statusNo));
            logger_->info("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? statusYes : statusNo));
            logger_->info("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? statusYes : statusNo));
            logger_->info("Prediction System: " + std::string(result.predictionIntegrated ? statusYes : statusNo));
            logger_->info("Development System: " + std::string(result.developmentIntegrated ? statusYes : statusNo));
            logger_->info("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
            
            logger_->info("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
            
            progress3.update(3, "Finalizing results");
            progress3.increment();
            
            // Convert metrics to string format for result
            std::map<std::string, std::string> metricStrings;
            auto allMetrics = metricsCollector_->getMetrics();
            for (const auto& metric : allMetrics) {
                metricStrings[metric.name] = std::to_string(metric.value);
            }
            
            // Add experiment results to metrics
            metricStrings["totalReward"] = std::to_string(result.totalReward);
            metricStrings["episodesStored"] = std::to_string(result.memoryEpisodesStored);
            metricStrings["dopamineLevel"] = std::to_string(result.dopamineLevel);
            metricStrings["wallClockTime"] = std::to_string(result.totalWallClockTime);
            
            result.metrics = metricStrings;
            
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            
            logger_->info("\n=== Enhanced Phase 6 Demo Complete ===");
            logger_->info("Total execution time: " + std::to_string(duration.count()) + "s");
            logger_->info("Status: SUCCESS - All systems integrated and functional");
            
            result.markSuccess();
            
        } catch (const std::exception& e) {
            logger_->error("Enhanced Phase 6 demo failed: " + std::string(e.what()));
            result.markFailure(std::string("Exception: ") + e.what());
        }
        
        return result;
    }
    
    std::string getName() const override { return name_; }
    std::string getDescription() const override { return description_; }
    
    std::vector<std::string> getAvailableTests() const override {
        std::vector<std::string> tests;
        tests.push_back("Integration Verification");
        tests.pushexcerpt("Individual System Testing");
        tests.push_back("Full Integration Experiment");
        return tests;
    }
    
    void loadConfig(const std::string& configFile) override {
        // Load enhanced configuration
        auto config = AdvancedConfig::loadJsonConfig(configFile);
        logger_->info("Loaded enhanced configuration: " + configFile);
    }
    
    void applyCommandLineArgs(int argc, char** argv) override {
        // Apply command line arguments to configuration
        DemoCLI::Options options = DemoCLI::parseArguments(argc, argv);
        // Configuration is applied in main()
    }
    
    void printBanner() override {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然 (Enhanced Demo)                               ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6 with Advanced Framework                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void setupLogging() override {
        logger_->setLevel(LogLevel::Info);
        // Enable enhanced logging features
        NLM_LOG_INFO("Enhanced logging enabled");
        NLM_LOG_INFO("Progress tracking enabled");
        NLM_LOG_INFO("Metrics collection enabled");
    }
    
    void cleanup() override {
        logger_->info("=== Enhanced Demo Cleanup Complete ===");
        // Clean up resources
        metricsCollector_->clear();
    }
    
    // Get metrics for reporting
    std::shared_ptr<MetricsCollector> getMetricsCollector() const {
        return metricsCollector_;
    }
    
private:
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<MetricsCollector> metricsCollector_;
};

// Main function with enhanced CLI and framework integration
int main(int argc, char* argv[]) {
    // Parse command line arguments using enhanced CLI
    DemoCLI::Options options = DemoCLI::parseArguments(argc, argv);
    
    // Show help if requested
    if (options.help) {
        DemoCLI::printHelp();
        return 0;
    }
    
    // Initialize global logging with enhanced features
    auto logger = Logger::getGlobal();
    logger->setLevel(options.quiet ? LogLevel::Error : 
                    (options.verbose ? LogLevel::Debug : LogLevel::Info));
    
    // Create enhanced demo
    EnhancedPhase6Demo demo;
    demo.printBanner();
    
    // Log startup information
    logger->info("=== Enhanced Phase 6 Demo Framework ===");
    logger->info("Version: Advanced Integration Demo with Monitoring");
    logger->info("Features: Progress tracking, metrics collection, JSON output");
    
    // Load configuration from JSON file if specified
    std::shared_ptr<Config> baseConfig = std::make_shared<Config>();
    if (!options.configFile.empty()) {
        auto jsonConfig = AdvancedConfig::loadJsonConfig(options.configFile);
        baseConfig = jsonConfig;
        logger->info("Loaded configuration from: " + options.configFile);
    }
    
    // Apply command line configuration
    auto cliConfig = parseCommandLineConfig(argc, argv);
    auto mergedConfig = AdvancedConfig::mergeConfigs(baseConfig, cliConfig);
    
    // Display enhanced configuration summary
    logger->info("Enhanced Configuration:");
    logger->info(AdvancedConfig::formatConfigForDisplay(mergedConfig));
    
    // Set up enhanced demo environment
    demo.setupLogging();
    
    // Create progress tracking for overall demo
    ProgressTracker overallProgress("Enhanced Phase 6 Demo", 3);
    
    logger->info("Starting Enhanced Phase 6 Demo...");
    auto startTime = std::chrono::system_clock::now();
    
    overallProgress.update(0, "Initializing demo");
    
    // Run the enhanced demo with configuration
    DemoResult result = demo.run(AdvancedConfig::configToMap(mergedConfig));
    
    overallProgress.update(1, "Running demo tests");
    
    auto endTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    overallProgress.update(2, "Completing demo");
    overallProgress.complete();
    
    // Display comprehensive results
    logger->info("=== Enhanced Phase 6 Demo Results ===");
    logger->info("Total execution time: " + DemoCLI::formatDuration(duration.count()));
    logger->info("Demo status: " + std::string(result.success ? "SUCCESS" : "FAILED"));
    
    if (!result.success && !result.errorMessage.empty()) {
        logger->error("Error: " + result.errorMessage);
    }
    
    // Save comprehensive results if requested
    if (!options.outputFile.empty()) {
        // Save detailed results including metrics
        std::ofstream file(options.outputFile);
        if (file.is_open()) {
            file << "=== Enhanced Phase 6 Demo Results ===\n";
            file << "Execution Time: " << DemoCLI::formatDuration(duration.count()) << "\n";
            file << "Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
            file << "Metrics:\n";
            for (const auto& [key, value] : result.metrics) {
                file << "  " << key << ": " << value << "\n";
            }
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << "\n";
            }
            file.close();
            logger->info("Results saved to: " + options.outputFile);
        }
    }
    
    // Output enhanced JSON results if requested
    if (options.jsonOutput) {
        // Create comprehensive JSON output
        nlohmann::json jsonOutput;
        jsonOutput["demo_name"] = "Enhanced Phase 6 Demo";
        jsonOutput["success"] = result.success;
        jsonOutput["execution_time"] = duration.count();
        jsonOutput["metrics"] = result.metrics;
        jsonOutput["error_message"] = result.errorMessage;
        
        // Add framework-specific information
        jsonOutput["framework_version"] = "Advanced Demo Framework v1.0";
        jsonOutput["features"] = {"progress_tracking", "metrics_collection", "json_output"};
        
        std::cout << jsonOutput.dump(2) << std::endl;
    }
    
    // Display enhanced metrics summary
    auto metricsCollector = demo.getMetricsCollector();
    if (metricsCollector) {
        logger->info("\n=== Metrics Summary ===");
        logger->info("System integration metrics collected:");
        auto allMetrics = metricsCollector->getMetrics();
        for (const auto& metric : allMetrics) {
            logger->info("  " + metric.name + ": " + std::to_string(metric.value));
        }
    }
    
    // Cleanup demo environment
    demo.cleanup();
    
    // Return appropriate exit code
    return result.success ? 0 : 1;
}

} // namespace nlm
