/**
 * Phase 6 Demo with Interactive Visualization and Performance Profiling
 * 
 * Enhanced demo that includes real-time visualization of brain systems,
 * neural activity, and performance profiling for all 6 brain systems.
 * 
 * Features:
 * - Real-time visualization with automatic performance profiling
 * - Command-line profiling control options
 * - Performance metrics collection and analysis
 * - Multi-format profiling output (text, JSON, CSV)
 * - Brain system-specific performance monitoring
 * - Automatic performance analysis and reporting
 */

#include "experiments/Phase6DemoPerformance.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <csignal>
#include <filesystem>

using namespace nlm;

// Global flag for graceful shutdown
bool running = true;

void signalHandler(int signal) {
    running = false;
    std::cout << "\nReceived interrupt signal. Shutting down visualization..." << std::endl;
}

int main(int argc, char* argv[]) {
    // Set up signal handler
    std::signal(SIGINT, signalHandler);
    
    std::cout << "=== NLM Phase 6 Visualization Demo with Performance Profiling ===" << std::endl;
    std::cout << "Real-time visualization of the integrated artificial brain with performance analysis..." << std::endl << std::endl;
    
    // Parse command line arguments with profiling support
    VisualizationInterface::VisualizationConfig visConfig;
    Phase6DemoConfig demoConfig;
    
    // First parse base visualization config
    if (!VisualizationInterface::parseCommandLine(argc, argv, visConfig)) {
        return 1;
    }
    
    // Display help if requested
    if (visConfig.showHelp) {
        std::cout << VisualizationInterface::getCommandLineHelp() << std::endl;
        std::cout << "\n" << Phase6DemoProfiling::getPhase6DemoProfilingHelp() << std::endl;
        // Continue anyway
    }
    
    // Parse Phase 6 demo profiling configuration
    if (!Phase6DemoProfiling::parseCommandLine(argc, argv, demoConfig)) {
        std::cerr << "ERROR: Failed to parse Phase 6 demo configuration!" << std::endl;
        return 1;
    }
    
    // Override visualization config with profiling config
    if (demoConfig.enableProfiling) {
        visConfig.autoSave = demoConfig.autoSave;
        visConfig.exportPath = demoConfig.exportPath;
        visConfig.exportFormat = demoConfig.exportFormat;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Determine visualization mode from config
    VisualizationMode mode = VisualizationMode::Text;
    if (visConfig.enableTextVisualization) {
        if (visConfig.width > 80 || visConfig.height > 40) {
            mode = VisualizationMode::TextAdvanced;
        } else {
            mode = VisualizationMode::Compact;
        }
    }
    
    // Create visualization interface
    VisualizationInterface visualizer(mode);
    
    // Initialize visualization
    if (!visualizer.initialize(visConfig.windowTitle, visConfig.width, visConfig.height)) {
        std::cerr << "ERROR: Failed to initialize visualization!" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Visualization Configuration ===" << std::endl;
    std::cout << "Mode: " << (mode == VisualizationMode::Text ? "Text" : 
                          mode == VisualizationMode::TextAdvanced ? "Text Advanced" : 
                          mode == VisualizationMode::Compact ? "Compact" :
                          mode == VisualizationMode::Graphical ? "Graphical" :
                          mode == VisualizationMode::WebSocket ? "WebSocket" : 
                          "Python API") << std::endl;
    std::cout << "Size: " << visConfig.width << "x" << visConfig.height << std::endl;
    std::cout << "Update Rate: " << visualizer.getUpdateRate() << " Hz" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== Performance Profiling Configuration ===" << std::endl;
    std::cout << "Profiling: " << (demoConfig.enableProfiling ? "ENABLED" : "DISABLED") << std::endl;
    if (demoConfig.enableProfiling) {
        std::cout << "Output Directory: " << demoConfig.profileOutputDir << std::endl;
        std::cout << "Output Format: " << demoConfig.profileFormat << std::endl;
        std::cout << "Console Output: " << (demoConfig.profileConsoleOutput ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Sampling Interval: " << demoConfig.profileIntervalMs << " ms" << std::endl;
    }
    std::cout << std::endl;
    
    // Create Phase 6 experiment with profiling
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        visualizer.close();
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with visualization updates
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
    
    // Prepare visualization data
    Phase6Config config;
    config.neuronCount = demoConfig.neuronCount;
    config.maxSteps = demoConfig.maxSteps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // Create performance profiler
    PerformanceProfiler profiler(demoConfig.enableProfiling, 
                                 demoConfig.profileOutputDir,
                                 demoConfig.profileFormat,
                                 demoConfig.profileConsoleOutput);
    profiler.setSamplingInterval(demoConfig.profileIntervalMs * 1000.0); // Convert to microseconds
    
    // Apply profiling configuration to profiler
    Phase6DemoProfiling::applyProfileConfig(demoConfig, profiler);
    
    // Create minimal brain for visualization
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", config.neuronCount);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize visualization brain!" << std::endl;
        visualizer.close();
        return 1;
    }
    
    // Create simple world
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Start profiling for the experiment
    Phase6DemoProfiling::setupProfilingForExperiment(profiler, demoConfig);
    
    // Visualization loop with performance monitoring
    std::cout << "--- Visualization Loop ---" << std::endl;
    std::cout << "Real-time visualization of brain systems. Press Ctrl+C to exit." << std::endl;
    std::cout << "Performance profiling: " << (demoConfig.enableProfiling ? "ENABLED" : "DISABLED") << std::endl << std::endl;
    
    auto lastExportTime = std::chrono::high_resolution_clock::now();
    Phase6DemoMetrics demoMetrics;
    
    // Start timing for step profiling
    auto cycleStartTime = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; running && step < config.maxSteps; ++step) {
        // Start brain step timing
        auto brainStartTime = std::chrono::high_resolution_clock::now();
        
        // Step the brain
        brain->step(step, std::chrono::duration<double>::count(
            std::chrono::high_resolution_clock::now().time_since_epoch()));
        
        // Calculate brain step time
        auto brainEndTime = std::chrono::high_resolution_clock::now();
        auto brainStepTime = std::chrono::duration_cast<std::chrono::microseconds>(brainEndTime - brainStartTime).count();
        
        // Update visualization
        auto vizStartTime = std::chrono::high_resolution_clock::now();
        visualizer.update(*brain, step);
        visualizer.render();
        
        // Calculate visualization time
        auto vizEndTime = std::chrono::high_resolution_clock::now();
        auto visualizationTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(vizEndTime - vizStartTime).count();
        
        // Check for data export
        if (visConfig.autoSave && std::chrono::duration<double>::count(
            std::chrono::high_resolution_clock::now() - lastExportTime) > 10.0) {
            
            std::stringstream filename;
            filename << visConfig.exportPath << "/visualization_step_" 
                     << step << "_" << std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::high_resolution_clock::now().time_since_epoch()).count()
                     << "." << visConfig.exportFormat;
            
            visualizer.exportData(filename.str());
            visualizer.exportMetrics(filename.str() + ".csv");
            
            lastExportTime = std::chrono::high_resolution_clock::now();
        }
        
        // Record step metrics for performance analysis
        demoMetrics.recordStep(step, visualizationTimeUs, 0, profiler.getCurrentMemoryUsage());
        Phase6DemoProfiling::collectDemoMetrics(*brain, step, visualizationTimeUs, 0);
        
        Phase6DemoProfiling::recordStepMetrics(profiler, step, *brain, brainStepTime);
        
        // Small delay to control update rate
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / visualizer.getUpdateRate())));
    }
    
    // End profiling cycle
    auto cycleEndTime = std::chrono::high_resolution_clock::now();
    auto totalCycleTime = std::chrono::duration_cast<std::chrono::microseconds>(cycleEndTime - cycleStartTime).count();
    
    Phase6DemoProfiling::endProfilingCycle(profiler);
    
    // Run full experiment with visualization
    std::cout << std::endl;
    std::cout << "--- Full Integration Experiment with Visualization ---" << std::endl;
    
    // Create profiling-enhanced experiment
    auto result = experiment.run(config);
    
    // Final visualization update
    visualizer.update(*brain, config.maxSteps);
    visualizer.render();
    
    // Export final data if requested
    if (visConfig.enableExport) {
        std::stringstream finalFilename;
        finalFilename << visConfig.exportPath << "/final_visualization." << visConfig.exportFormat;
        visualizer.exportData(finalFilename.str());
        visualizer.exportMetrics(finalFilename.str() + ".csv");
        std::cout << std::endl;
        std::cout << "=== Final visualization data exported to " << finalFilename.str() << " ===" << std::endl;
    }
    
    // Export profiling results
    if (demoConfig.enableProfiling) {
        std::stringstream profileFilename;
        profileFilename << demoConfig.profileOutputDir << "/phase6_demo_performance_" 
                       << std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        Phase6DemoProfiling::exportPhase6DemoMetrics(demoMetrics, profileFilename.str());
        Phase6DemoProfiling::generatePerformanceAnalysis(demoMetrics, profileFilename.str() + "_analysis.txt");
        
        Phase6DemoProfiling::exportProfilingResults(profiler, profileFilename.str());
    }
    
    // Close visualization
    visualizer.close();
    
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
    std::cout << std::endl;
    
    std::cout << "=== Performance Summary ===" << std::endl;
    std::cout << "Total simulation steps: " << demoMetrics.getTotalSteps() << std::endl;
    std::cout << "Average steps per second: " << (demoMetrics.getTotalSteps() / (result.totalWallClockTime / 1000.0)) << std::endl;
    std::cout << "Peak memory usage: " << PerformanceUtils::formatBytes(demoMetrics.getPeakMemoryUsage()) << std::endl;
    std::cout << "Average step time: " << PerformanceUtils::formatTime(demoMetrics.getAverageStepTime() * 1000000.0) << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== Performance Profiling Complete ===" << std::endl;
    std::cout << "Performance data has been generated and exported to " << demoConfig.profileOutputDir << "/ directory." << std::endl;
    std::cout << "You can view the results in " << demoConfig.profileOutputDir << "/ directory." << std::endl;
    
    return 0;
}