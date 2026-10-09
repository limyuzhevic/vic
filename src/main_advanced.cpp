// Advanced NLM User Interface Demo
// Demonstrates the advanced interface features for expert users
// Integrates CLI, UI, debugging, monitoring, and configuration management

#include "nlm/interfaces/AdvancedInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "performance/PerformanceMonitor.hpp"
#include "debugging/DebugTools.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

void printBanner() {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM ADVANCED USER INTERFACE DEMO" << std::endl;
    std::cout << "Phase 6 Advanced Features for Expert Users" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "\nAvailable Features:" << std::endl;
    std::cout << "  Advanced Command Line Interface (CLI)" << std::endl;
    std::cout << "  Interactive Expert User Interface (UI)" << std::endl;
    std::cout << "  Configuration Validation and Schema Management" << std::endl;
    std::cout << "  Real-time Performance Monitoring" << std::endl;
    std::cout << "  Debugging and Analysis Tools" << std::endl;
    std::cout << "  Batch Processing" << std::endl;
    std::cout << "  Advanced Configuration Management" << std::endl;
    std::cout << "  Real-time Visualization" << std::endl;
    std::cout << "\nUsage Examples:" << std::endl;
    std::cout << "  nlm-expert --help                    - Show help" << std::endl;
    std::cout << "  nlm-expert config-validate           - Validate config" << std::endl;
    std::cout << "  nlm-expert monitor-start             - Start monitoring" << std::endl;
    std::cout << "  nlm-expert debug-neurons            - Debug neurons" << std::endl;
    std::cout << "  nlm-expert experiment-run            - Run experiment" << std::endl;
    std::cout << std::endl;
}

void demonstrateAdvancedCLI() {
    std::cout << "\n=== Advanced CLI Demonstration ===" << std::endl;
    
    // Create advanced CLI instance
    auto cli = nlm::InterfaceFactory::createAdvancedCLI();
    
    // Parse command line arguments (simulating)
    nlm::AdvancedCLI::CommandArgs args;
    args.command = nlm::AdvancedCLI::Command::HELP;
    args.verbose = true;
    args.jsonOutput = true;
    args.outputFile = "cli_help.json";
    
    // Execute command
    std::cout << "Executing command: help" << std::endl;
    cli->execute(args, std::cout);
}

void demonstrateConfigurationValidation() {
    std::cout << "\n=== Configuration Validation Demo ===" << std::endl;
    
    // Create config validator instance
    auto validator = nlm::InterfaceFactory::createConfigValidator();
    
    // Create and load a configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set some configuration values
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    
    // Validate configuration
    auto errors = validator->validate(*config);
    
    std::cout << "Configuration validation:" << std::endl;
    std::cout << "  Valid: " << (config->validate() ? "Yes" : "No") << std::endl;
    std::cout << "  Error count: " << errors.size() << std::endl;
    
    if (!errors.empty()) {
        std::cout << "  Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "    * " << error << std::endl;
        }
    }
    
    // Get validation summary
    std::string summary = validator->getValidationSummary(*config);
    std::cout << "\nValidation summary:" << std::endl;
    std::cout << summary;
}

void demonstratePerformanceMonitoring() {
    std::cout << "\n=== Performance Monitoring Demo ===" << std::endl;
    
    // Create performance monitor instance
    auto monitor = nlm::InterfaceFactory::createPerformanceMonitor();
    
    // Set up monitoring
    monitor->setMonitoringInterval(0.5);
    monitor->startMonitoring();
    
    // Create and initialize a simple brain (simulated)
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
    
    if (brain->initialize()) {
        monitor->setBrain(brain);
        
        // Update metrics for a few steps
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
            monitor->updateMetrics(*brain);
            
            auto metrics = monitor->getMetrics();
            std::cout << "\rStep " << step << ": " 
                      << "Spikes: " << metrics.totalSpikeCount 
                      << " | Firing: " << metrics.firingNeuronCount
                      << " | CPU: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%"
                      << " | Mem: " << metrics.memoryUsage << "%";
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << std::endl;
        
        // Stop monitoring
        monitor->stopMonitoring();
        
        // Export metrics to JSON
        bool jsonSuccess = monitor->exportMetrics("performance_metrics.json");
        std::cout << "\nJSON export: " << (jsonSuccess ? "Success" : "Failed") << std::endl;
        
        // Export metrics to CSV
        bool csvSuccess = monitor->exportMetricsCSV("performance_metrics.csv");
        std::cout << "CSV export: " << (csvSuccess ? "Success" : "Failed") << std::endl;
        
        // Get statistics
        auto stats = monitor->getStatistics();
        std::cout << "\nPerformance statistics:" << std::endl;
        for (const auto& pair : stats) {
            std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(2) << pair.second << std::endl;
        }
    }
}

void demonstrateDebugTools() {
    std::cout << "\n=== Debug Tools Demo ===" << std::endl;
    
    // Create debug tools instance
    auto debugTools = nlm::InterfaceFactory::createDebugTools();
    
    debugTools->setDebugEnabled(true);
    
    // Create a simple brain for debugging
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
    
    if (brain->initialize()) {
        debugTools->setBrain(brain);
        
        // Demonstrate debug information
        std::cout << "Brain debug info:" << std::endl;
        std::cout << debugTools->getBrainInfo(*brain);
        
        std::cout << "\nNeuron debug info (first 3 neurons):" << std::endl;
        auto neuronInfos = debugTools->getNeuronInfo(*brain, -1);
        for (size_t i = 0; i < std::min(size_t(3), neuronInfos.size()); ++i) {
            std::cout << neuronInfos[i];
        }
        
        // Get memory info
        std::cout << "\nMemory debug info:" << std::endl;
        std::cout << debugTools->getMemoryInfo(*brain);
        
        // Get prediction info
        std::cout << "\nPrediction debug info:" << std::endl;
        std::cout << debugTools->getPredictionInfo(*brain);
        
        // Get neuromodulation info
        std::cout << "\nNeuromodulation debug info:" << std::endl;
        std::cout << debugTools->getNeuromodulationInfo(*brain);
        
        // Get plasticity info
        std::cout << "\nPlasticity debug info:" << std::endl;
        std::cout << debugTools->getPlasticityInfo(*brain);
        
        // Export debug info
        bool exportSuccess = debugTools->exportDebugInfo(*brain, "debug_info.txt");
        std::cout << "\nDebug export: " << (exportSuccess ? "Success" : "Failed") << std::endl;
    }
}

void demonstrateBatchProcessing() {
    std::cout << "\n=== Batch Processing Demo ===" << std::endl;
    
    // Create batch processor instance
    auto batchProcessor = nlm::InterfaceFactory::createBatchProcessor();
    
    // Configure batch
    nlm::BatchProcessor::BatchConfig config;
    config.name = "Demo Batch";
    config.experimentFiles = {
        "experiments/Phase3Demo.cpp",
        "experiments/Phase4Demo.cpp",
        "experiments/Phase6Demo.cpp"
    };
    config.runInParallel = false;
    config.maxParallelExperiments = 1;
    config.retryCount = 2;
    config.timeoutSeconds = 300;
    config.outputDirectory = ".";
    config.saveResults = true;
    config.cleanupAfterCompletion = true;
    
    // Start batch processing
    std::string batchId = batchProcessor->startBatch(config);
    std::cout << "Started batch: " << batchId << std::endl;
    
    // Monitor batch status
    for (int i = 0; i < 5 && batchProcessor->isBatchRunning(batchId); ++i) {
        auto result = batchProcessor->getBatchStatus(batchId);
        std::cout << "\rBatch Status: " << result.status 
                  << " | Completed: " << result.completedExperiments 
                  << "/" << result.totalExperiments
                  << " | Failed: " << result.failedExperiments;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    std::cout << std::endl;
    
    // Get final batch status
    auto finalResult = batchProcessor->getBatchStatus(batchId);
    std::cout << "\nFinal batch result:" << std::endl;
    std::cout << "  Status: " << finalResult.status << std::endl;
    std::cout << "  Completed: " << finalResult.completedExperiments << std::endl;
    std::cout << "  Failed: " << finalResult.failedExperiments << std::endl;
    
    // Export batch results
    bool exportSuccess = batchProcessor->exportBatchResults(batchId, "batch_results.json");
    std::cout << "Batch export: " << (exportSuccess ? "Success" : "Failed") << std::endl;
}

void demonstrateRealTimeMonitor() {
    std::cout << "\n=== Real-time Monitor Demo ===" << std::endl;
    
    // Create real-time monitor instance
    auto realTimeMonitor = nlm::InterfaceFactory::createRealTimeMonitor();
    
    // Start monitoring
    bool startSuccess = realTimeMonitor->startMonitoring(1.0);
    std::cout << "Started real-time monitoring: " << (startSuccess ? "Yes" : "No") << std::endl;
    
    if (startSuccess) {
        // Simulate some operations
        auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>());
        auto config = std::make_shared<nlm::Config>();
        auto experiment = std::make_shared<nlm::Phase6IntegratedExperiment>();
        auto performanceMonitor = nlm::InterfaceFactory::createPerformanceMonitor();
        auto debugTools = nlm::InterfaceFactory::createDebugTools();
        auto visualizationInterface = std::make_shared<nlm::VisualizationInterface>();
        
        realTimeMonitor->setBrain(brain);
        realTimeMonitor->setConfig(config);
        realTimeMonitor->setExperiment(experiment);
        realTimeMonitor->setPerformanceMonitor(performanceMonitor);
        realTimeMonitor->setVisualizationInterface(visualizationInterface);
        
        std::cout << "Real-time monitor configuration complete." << std::endl;
        std::cout << "Rendering dashboard..." << std::endl;
        
        // Simulate a few updates
        for (int i = 0; i < 3; ++i) {
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        std::cout << std::endl;
        
        // Stop monitoring
        realTimeMonitor->stopMonitoring();
        
        std::cout << "Real-time monitoring stopped." << std::endl;
    }
}

void demonstrateExperimentManagement() {
    std::cout << "\n=== Experiment Management Demo ===" << std::endl;
    
    // Create and run a Phase 6 integrated experiment
    auto experiment = std::make_shared<nlm::Phase6IntegratedExperiment>();
    
    std::cout << "Running Phase 6 Integration Verification..." << std::endl;
    bool integrationOK = experiment->verifyIntegration();
    std::cout << "Integration verification: " << (integrationOK ? "PASS" : "FAIL") << std::endl;
    
    if (integrationOK) {
        std::cout << "\nRunning Phase 6 Integrated Experiment..." << std::endl;
        
        nlm::Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 1000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment->run(config);
        
        std::cout << "\nExperiment Results:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << "  Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "\nIntegration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    }
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Initialize all interface components
    bool initSuccess = nlm::InterfaceFactory::initializeAll();
    std::cout << "Interface components initialized: " << (initSuccess ? "Yes" : "No") << std::endl;
    
    if (!initSuccess) {
        std::cerr << "Failed to initialize interface components!" << std::endl;
        return 1;
    }
    
    // Parse command line arguments
    if (argc > 1) {
        // Create advanced CLI and parse arguments
        auto cli = nlm::InterfaceFactory::createAdvancedCLI();
        auto args = cli->parse(argc, argv);
        
        if (args.command == nlm::AdvancedCLI::Command::HELP || args.command == nlm::AdvancedCLI::Command::UNKNOWN) {
            printBanner();
            std::cout << "\nUse 'nlm-expert --help' for complete command list." << std::endl;
        } else {
            std::cout << "\nExecuting command: " << cli->getCommandString(args.command) << std::endl;
            cli->execute(args, std::cout);
        }
    } else {
        // Demonstrate all features if no arguments provided
        std::cout << "\nNo arguments provided. Demonstrating all advanced features..." << std::endl;
        
        demonstrateAdvancedCLI();
        demonstrateConfigurationValidation();
        demonstratePerformanceMonitoring();
        demonstrateDebugTools();
        demonstrateBatchProcessing();
        demonstrateRealTimeMonitor();
        demonstrateExperimentManagement();
    }
    
    // Cleanup all interface components
    nlm::InterfaceFactory::cleanupAll();
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM Advanced Interface Demo Complete" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    return 0;
}