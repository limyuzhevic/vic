// Debugging Tools Demo
// This demo showcases the Debugging Tools capabilities

#include "DebuggingTools.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace nlm;

class DebuggingToolsDemo {
public:
    DebuggingToolsDemo() {}
    ~DebuggingToolsDemo() {}
    
    void run() {
        std::cout << "========================================\n";
        std::cout << "DEBUGGING TOOLS DEMO\n";
        std::cout << "========================================\n\n";
        
        // Initialize debugging tools
        DebuggingTools debugger;
        
        // Create test scenario
        std::cout << "--- Creating Test Scenario ---\n";
        
        auto config = createConfig(500, 4, 0.15f);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        
        std::cout << "✓ Test scenario created\n\n";
        
        // Enable debugging
        debugger.setLogLevel(DebugLevel::Info);
        debugger.enableStackTrace(true);
        debugger.enableMemoryLeakDetection(true);
        debugger.enablePerformanceProfiling(true);
        debugger.enableStateValidation(true);
        
        std::cout << "--- Debugging Tools Configuration ---\n";
        std::cout << "Log level: " << debugLevelToString(debugger.getLogLevel()) << "\n";
        std::cout << "Stack trace enabled: " << (debugger.isStackTraceEnabled() ? "YES" : "NO") << "\n";
        std::cout << "Memory leak detection enabled: " << (debugger.isMemoryLeakDetectionEnabled() ? "YES" : "NO") << "\n";
        std::cout << "Performance profiling enabled: " << (debugger.isPerformanceProfilingEnabled() ? "YES" : "NO") << "\n";
        std::cout << "State validation enabled: " << (debugger.isStateValidationEnabled() ? "YES" : "NO") << "\n\n";
        
        // Perform debugging operations
        std::cout << "--- Performing Debugging Operations ---\n";
        
        // Log debugging information
        std::cout << "Logging debugging information...\n";
        debugger.logDebug("Starting debugging demonstration", DebugLevel::Info);
        debugger.logDebug("Brain initialized with " + std::to_string(brain->getTotalNeuronCount()) + " neurons", DebugLevel::Info);
        debugger.logDebug("Agent ready for simulation", DebugLevel::Info);
        
        // Create stack trace
        std::cout << "Creating stack trace...\n";
        auto stackTrace = debugger.captureStackTrace();
        std::cout << "Stack trace captured with " << stackTrace.size() << " frames\n";
        
        // Monitor memory allocation
        std::cout << "Monitoring memory allocation...\n";
        auto memoryMonitor = debugger.startMemoryMonitoring();
        
        for (int i = 0; i < 100; ++i) {
            // Create and destroy objects
            auto tempBrain = std::make_shared<Brain>(config);
            tempBrain->initialize();
            
            // Simulate memory usage
            std::vector<float> tempData(1000);
            
            // Perform some operations
            tempBrain->step(i, i * 0.001);
            
            // Cleanup (this would trigger memory leak detection)
        }
        
        debugger.stopMemoryMonitoring();
        
        // Profile performance
        std::cout << "Profiling performance...\n";
        auto perfProfiler = debugger.startPerformanceProfiling();
        
        // Run intensive operations
        for (int i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto perfResult = debugger.stopPerformanceProfiling();
        
        std::cout << "Performance profile results:\n";
        std::cout << "  Total time: " << perfResult.totalTime << "s\n";
        std::cout << "  Average CPU usage: " << perfResult.averageCpuUsage << "%\n";
        std::cout << "  Peak memory usage: " << perfResult.peakMemoryUsage << "KB\n";
        std::cout << "  Calls per second: " << perfResult.callsPerSecond << "\n\n";
        
        // Validate brain state
        std::cout << "Validating brain state...\n";
        bool stateValid = debugger.validateBrainState(brain);
        std::cout << "Brain state valid: " << (stateValid ? "YES" : "NO") << "\n";
        
        if (!stateValid) {
            auto validationErrors = debugger.getValidationErrors();
            std::cout << "Validation errors:";
            for (const auto& error : validationErrors) {
                std::cout << "\n  - " << error;
            }
        }
        
        // Check for memory leaks
        std::cout << "\nChecking for memory leaks...\n";
        auto memoryLeaks = debugger.checkMemoryLeaks();
        std::cout << "Memory leak check results:\n";
        std::cout << "  Memory leaks detected: " << (memoryLeaks.leakDetected ? "YES" : "NO") << "\n";
        std::cout << "  Leak size: " << memoryLeaks.leakSize << " bytes\n";
        std::cout << "  Leak address: " << (memoryLeaks.leakAddress ? "YES" : "NO") << "\n";
        
        if (memoryLeaks.leakDetected) {
            std::cout << "  Leak stack trace:";
            for (const auto& frame : memoryLeaks.leakStackTrace) {
                std::cout << "\n    - " << frame;
            }
        }
        
        // Debug neural network
        std::cout << "\nDebugging neural network...\n";
        
        // Check synaptic connections
        size_t totalConnections = brain->getTotalSynapseCount();
        std::cout << "Total synaptic connections: " << totalConnections << "\n";
        
        // Check neuron states
        size_t activeNeurons = brain->getFiringNeuronCount();
        std::cout << "Active neurons: " << activeNeurons << "\n";
        
        // Check region states
        size_t activeRegions = 0;
        for (const auto& region : brain->getRegions()) {
            if (region->getActivityLevel() > 0.0f) {
                activeRegions++;
            }
        }
        std::cout << "Active regions: " << activeRegions << "\n";
        
        // Debug memory systems
        std::cout << "\nDebugging memory systems...\n";
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        if (workingMemory) {
            std::cout << "Working memory active traces: " << workingMemory->getActiveTraces() << "\n";
            std::cout << "Working memory capacity: " << workingMemory->getCapacity() << "\n";
            std::cout << "Working memory utilization: " << workingMemory->getUtilization() << "\n";
        }
        
        if (episodicMemory) {
            std::cout << "Episodic memory episode count: " << episodicMemory->getEpisodeCount() << "\n";
            std::cout << "Episodic memory storage capacity: " << episodicMemory->getStorageCapacity() << "\n";
        }
        
        // Debug agent behavior
        std::cout << "\nDebugging agent behavior...\n";
        
        // Check agent metrics
        float noveltyLevel = agent.getNoveltyLevel();
        float curiosityLevel = agent.getCuriosityLevel();
        float dopamineLevel = agent.getNeuromodulationLevel();
        
        std::cout << "Agent metrics:\n";
        std::cout << "  Novelty level: " << noveltyLevel << "\n";
        std::cout << "  Curiosity level: " << curiosityLevel << "\n";
        std::cout << "  Dopamine level: " << dopamineLevel << "\n";
        
        // Debug simulation state
        std::cout << "\nDebugging simulation state...\n";
        
        // Log current state
        debugger.logState("Current simulation state", {
            {"neurons", brain->getTotalNeuronCount()},
            {"active_neurons", brain->getFiringNeuronCount()},
            {"working_memory_traces", workingMemory ? workingMemory->getActiveTraces() : 0},
            {"episodic_memory_episodes", episodicMemory ? episodicMemory->getEpisodeCount() : 0},
            {"novelty_level", noveltyLevel},
            {"curiosity_level", curiosityLevel},
            {"dopamine_level", dopamineLevel}
        });
        
        // Compare state with expected
        std::cout << "Comparing state with expected...\n";
        
        std::map<std::string, double> expectedState = {
            {"neurons", 500},
            {"working_memory_traces", 0},
            {"episodic_memory_episodes", 0},
            {"novelty_level", 0.0f},
            {"curiosity_level", 0.0f},
            {"dopamine_level", 0.0f}
        };
        
        auto stateDifferences = debugger.compareState(expectedState);
        std::cout << "State differences:\n";
        for (const auto& diff : stateDifferences) {
            std::cout << "  " << diff.first << ": expected=" << diff.second.first << ", actual=" << diff.second.second << "\n";
        }
        
        // Generate debug report
        std::cout << "\n--- Generating Debug Report ---\n";
        
        std::string debugReport = debugger.generateDebugReport();
        std::cout << "Debug report generated (" << debugReport.length() << " characters)\n";
        
        // Export debug logs
        std::cout << "\n--- Exporting Debug Logs ---\n";
        
        std::string logsPath = "/tmp/debug_logs.json";
        debugger.exportDebugLogs(logsPath);
        std::cout << "✓ Debug logs exported to: " << logsPath << "\n";
        
        // Export performance profile
        std::cout << "\n--- Exporting Performance Profile ---\n";
        
        std::string perfPath = "/tmp/performance_profile.json";
        debugger.exportPerformanceProfile(perfPath);
        std::cout << "✓ Performance profile exported to: " << perfPath << "\n";
        
        // Export memory analysis
        std::cout << "\n--- Exporting Memory Analysis ---\n";
        
        std::string memoryPath = "/tmp/memory_analysis.json";
        debugger.exportMemoryAnalysis(memoryPath);
        std::cout << "✓ Memory analysis exported to: " << memoryPath << "\n";
        
        // Export state validation
        std::cout << "\n--- Exporting State Validation ---\n";
        
        std::string validationPath = "/tmp/state_validation.json";
        debugger.exportStateValidation(validationPath);
        std::cout << "✓ State validation exported to: " << validationPath << "\n";
        
        // Generate debugging script
        std::cout << "\n--- Generating Debugging Script ---\n";
        
        std::string scriptPath = "/tmp/debugging_script.py";
        debugger.generateDebuggingScript(scriptPath, {
            {"language", "python"},
            {"framework", "matplotlib"},
            {"include_gui", "true"},
            {"export_format", "json"}
        });
        std::cout << "✓ Debugging script generated: " << scriptPath << "\n";
        
        // Generate debug visualization
        std::cout << "\n--- Generating Debug Visualization ---\n";
        
        std::string vizPath = "/tmp/debug_visualization.png";
        debugger.generateDebugVisualization(vizPath, {
            {"include_memory_usage", "true"},
            {"include_performance", "true"},
            {"include_state_validation", "true"},
            {"color_scheme", "debug"}
        });
        std::cout << "✓ Debug visualization generated: " << vizPath << "\n";
        
        // Create debugging snapshot
        std::cout << "\n--- Creating Debugging Snapshot ---\n";
        
        std::string snapshotPath = "/tmp/debugging_snapshot.json";
        debugger.createDebugSnapshot(snapshotPath);
        std::cout << "✓ Debug snapshot created: " << snapshotPath << "\n";
        
        // Export debugging artifacts
        std::cout << "\n--- Exporting Debugging Artifacts ---\n";
        
        std::string artifactsPath = "/tmp/debugging_artifacts.zip";
        debugger.exportDebuggingArtifacts(artifactsPath);
        std::cout << "✓ Debugging artifacts exported to: " << artifactsPath << "\n";
        
        // Restore from debugging snapshot
        std::cout << "\n--- Restoring from Debugging Snapshot ---\n";
        
        std::string restoredPath = "/tmp/restored_debug_state.json";
        debugger.restoreDebugState(restoredPath);
        std::cout << "✓ Debug state restored from: " << restoredPath << "\n";
        
        // Generate debugging checklist
        std::cout << "\n--- Generating Debugging Checklist ---\n";
        
        std::string checklistPath = "/tmp/debugging_checklist.json";
        debugger.generateDebuggingChecklist(checklistPath, {
            {"target", "simulation"},
            {"platform", "linux"},
            {"environment", "production"}
        });
        std::cout << "✓ Debugging checklist generated: " << checklistPath << "\n";
        
        // Generate debugging guidelines
        std::cout << "\n--- Generating Debugging Guidelines ---\n";
        
        std::string guidelinesPath = "/tmp/debugging_guidelines.txt";
        debugger.generateDebuggingGuidelines(guidelinesPath, {
            {"debug_level", "advanced"},
            {"include_best_practices", "true"},
            {"format", "markdown"}
        });
        std::cout << "✓ Debugging guidelines generated: " << guidelinesPath << "\n";
        
        // Create debugging template
        std::cout << "\n--- Creating Debugging Template ---\n";
        
        std::string templatePath = "/tmp/debugging_template.json";
        debugger.createDebuggingTemplate(templatePath, {
            {"name", "nlm_debug_template"},
            {"version", "1.0"},
            {"author", "NLM Team"},
            {"description", "Advanced debugging template for NLM simulations"}
        });
        std::cout << "✓ Debugging template created: " << templatePath << "\n";
        
        // Export debugging configuration
        std::cout << "\n--- Exporting Debugging Configuration ---\n";
        
        std::string configPath = "/tmp/debugging_config.json";
        debugger.exportDebuggingConfig(configPath);
        std::cout << "✓ Debugging configuration exported to: " << configPath << "\n";
        
        // Generate debugging help
        std::cout << "\n--- Generating Debugging Help ---\n";
        
        std::string helpPath = "/tmp/debugging_help.txt";
        debugger.generateDebuggingHelp(helpPath);
        std::cout << "✓ Debugging help generated: " << helpPath << "\n";
        
        std::cout << "\n========================================\n";
        std::cout << "DEBUGGING TOOLS DEMO COMPLETE\n";
        std::cout << "========================================\n";
    }
    
private:
    std::shared_ptr<Config> createConfig(int neuronCount, int regionCount, float connectionProb) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProb);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("simulation_timestep", 0.001);
        return config;
    }
    
    std::string debugLevelToString(DebugLevel level) {
        switch (level) {
            case DebugLevel::Trace: return "Trace";
            case DebugLevel::Debug: return "Debug";
            case DebugLevel::Info: return "Info";
            case DebugLevel::Warning: return "Warning";
            case DebugLevel::Error: return "Error";
            case DebugLevel::Critical: return "Critical";
            default: return "Unknown";
        }
    }
};


int main(int argc, char* argv[]) {
    nlm::DebuggingToolsDemo demo;
    demo.run();
    return 0;
}
