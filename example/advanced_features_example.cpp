// Advanced Features Usage Example
// 
// This example demonstrates the new advanced features added to NLM:
// 1. Plugin System
// 2. Scripted Experiment Runner  
// 3. Advanced Debugging Tools
// 4. State Analysis and Visualization
// 5. Plugin Manager
// 6. Script Interpreter

#include "AdvancedFeatures.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

void demonstratePluginSystem() {
    std::cout << "=== Plugin System Demo ===" << std::endl;
    
    // Initialize plugin manager
    PluginManager pluginManager;
    
    // Load example plugin
    bool pluginLoaded = pluginManager.loadPlugin("plugins/example_plugin.so");
    if (pluginLoaded) {
        std::cout << "Plugin loaded successfully!" << std::endl;
        
        // Get plugin info
        auto pluginInfo = pluginManager.getPluginInfo();
        for (const auto& info : pluginInfo) {
            std::cout << "Plugin: " << info.name << " (v" << info.version << ")" << std::endl;
            std::cout << "  Description: " << info.description << std::endl;
            std::cout << "  Enabled: " << (info.enabled ? "Yes" : "No") << std::endl;
        }
        
        // Enable plugin
        pluginManager.setPluginEnabled("example_plugin", true);
        
        // Execute plugin commands
        std::vector<std::string> statsArgs = {};
        pluginManager.executePluginCommand("stats", statsArgs);
        
        std::vector<std::string> resetArgs = {};
        pluginManager.executePluginCommand("reset", resetArgs);
        
        // Save plugin configuration
        pluginManager.savePluginConfig("plugin_config.cfg");
        
        // Cleanup
        pluginManager.unloadPlugin("example_plugin");
    }
}

void demonstrateExperimentRunner() {
    std::cout << "\n=== Experiment Runner Demo ===" << std::endl;
    
    ExperimentRunner experimentRunner;
    
    // Get available experiments
    auto experiments = experimentRunner.getAvailableExperiments();
    std::cout << "Available experiments:" << std::endl;
    for (const auto& exp : experiments) {
        std::cout << "  - " << exp << std::endl;
    }
    
    // Get experiment parameters
    std::vector<std::string> params = experimentRunner.getExperimentParams("connectivity_test");
    std::cout << "Parameters for connectivity_test:" << std::endl;
    for (const auto& param : params) {
        std::cout << "  - " << param << std::endl;
    }
    
    // Run an experiment
    std::map<std::string, std::string> expParams;
    expParams["duration"] = "1000";
    expParams["neuron_count"] = "500";
    expParams["connection_probability"] = "0.1f";
    
    // Note: This would require an actual brain instance
    // auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    // experimentRunner.runExperiment(brain, "connectivity_test", expParams);
}

void demonstrateDebugTools() {
    std::cout << "\n=== Debug Tools Demo ===" << std::endl;
    
    DebugTools debugTools;
    
    // Enable debug mode
    debugTools.enableDebugMode(true);
    
    // Set debug level
    debugTools.setDebugLevel(LogLevel::Info);
    
    // Start profiling
    debugTools.startProfiling("example_session");
    
    // Simulate some work (placeholder)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Stop profiling
    std::string outputFile;
    debugTools.stopProfiling(outputFile);
    
    // Analyze memory usage
    std::string memoryReport;
    debugTools.analyzeMemoryUsage(memoryReport);
    
    // Analyze spike patterns
    // auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    // std::string spikeReport;
    // debugTools.analyzeSpikePatterns(brain, spikeReport);
    
    // Export debug info
    debugTools.exportDebugInfo("debug_info.txt");
}

void demonstrateStateAnalysis() {
    std::cout << "\n=== State Analysis Demo ===" << std::endl;
    
    StateAnalyzer stateAnalyzer;
    
    // Note: This would require an actual brain instance
    // auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    // std::string analysis;
    // stateAnalyzer.analyzeState(brain, analysis);
    // std::cout << "Brain state analysis:" << std::endl;
    // std::cout << analysis << std::endl;
    
    // Generate visualization data
    // std::string vizData;
    // stateAnalyzer.generateVisualizationData(brain, vizData);
    
    // Export connectivity graph
    // stateAnalyzer.exportConnectivityGraph(brain, "connectivity_graph.graphml");
    
    // Export activity patterns
    // stateAnalyzer.exportActivityPatterns(brain, "activity_patterns.dat");
    
    // Export memory statistics
    // stateAnalyzer.exportMemoryStats(brain, "memory_stats.json");
}

void demonstrateScripting() {
    std::cout << "\n=== Scripting Demo ===" << std::endl;
    
    ScriptInterpreter scriptInterpreter;
    
    // Simple script
    std::string script = R"(
# NLM Experiment Script
run experiment: connectivity_test
    duration: 1000
    neuron_count: 500
    connection_probability: 0.1

set variable: learning_rate = 0.01

if neuron_count > 100 {
    print "High neuron count - running detailed analysis"
}

loop 5 times {
    print "Iteration completed"
}

print "Script completed successfully"
)";
    
    // Execute script
    std::map<std::string, std::string> variables;
    variables["brain"] = "brain_instance";
    variables["time"] = "0.0";
    variables["step"] = "0";
    
    // Note: This would require an actual brain instance
    // auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    // bool scriptExecuted = scriptInterpreter.executeScript(script, brain, variables);
    // if (scriptExecuted) {
    //     std::cout << "Script executed successfully!" << std::endl;
    // } else {
    //     std::cout << "Script execution failed!" << std::endl;
    // }
    
    // Compile script
    // std::vector<uint8_t> bytecode;
    // scriptInterpreter.compileScript(script, bytecode);
    // std::cout << "Script compiled to " << bytecode.size() << " bytes" << std::endl;
}

void demonstrateAdvancedFeatures() {
    std::cout << "NLM Advanced Features Demonstration" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    demonstratePluginSystem();
    demonstrateExperimentRunner();
    demonstrateDebugTools();
    demonstrateStateAnalysis();
    demonstrateScripting();
    
    std::cout << "\n=== Advanced Features Demo Complete ===" << std::endl;
    std::cout << "\nKey features demonstrated:" << std::endl;
    std::cout << "1. Plugin system for extending NLM functionality" << std::endl;
    std::cout << "2. Scripted experiment runner for automated testing" << std::endl;
    std::cout << "3. Advanced debugging tools for performance analysis" << std::endl;
    std::cout << "4. State analysis and visualization utilities" << std::endl;
    std::cout << "5. Plugin manager for plugin lifecycle management" << std::endl;
    std::cout << "6. Script interpreter for NLM scripting language" << std::endl;
}

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    demonstrateAdvancedFeatures();
    
    return 0;
}
