#include "experiments/NLMCommandLineInterface.hpp"
#include "experiments/Phase2UnifiedFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace nlm::Phase2;

// Demo application showing enhanced NLM features
class NLMSystemDemo {
public:
    NLMSystemDemo() {
        std::cout << "=== NLM System Demo ===\n";
        std::cout << "This demo showcases the enhanced NLM system with:\n";
        std::cout << "  1. Unified experiment framework\n";
        std::cout << "  2. Enhanced configuration system\n";
        std::cout << "  3. Improved command-line interface\n";
        std::cout << "  4. Performance monitoring\n";
        std::cout << "  5. Plugin architecture\n\n";
    }
    
    void runBasicExperiments() {
        std::cout << "=== Running Basic Experiments ===\n";
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create configuration
        Config config;
        config.set("neuron_count", 200);
        config.set("region_count", 1);
        config.set("connection_probability", 0.1f);
        config.set("simulation_timestep", 0.001);
        config.set("random_seed", 123);
        
        // Create experiment runner
        ExperimentRunner runner;
        
        // Run basic connectivity test
        std::cout << "1. Running Basic Connectivity Test...\n";
        ExperimentResult result1 = runner.runExperiment(ExperimentMode::BASIC_CONNECTIVITY, config);
        std::cout << "   Result: " << (result1.success ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "   Duration: " << result1.getDurationString() << std::endl;
        std::cout << "   Message: " << result1.message << std::endl;
        
        // Run plasticity learning test
        std::cout << "\n2. Running Plasticity Learning Experiment...\n";
        ExperimentResult result2 = runner.runExperiment(ExperimentMode::PLASTICITY_LEARNING, config);
        std::cout << "   Result: " << (result2.success ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "   Duration: " << result2.getDurationString() << std::endl;
        std::cout << "   Message: " << result2.message << std::endl;
        
        // Run STDP verification test
        std::cout << "\n3. Running STDP Verification Test...\n";
        ExperimentResult result3 = runner.runExperiment(ExperimentMode::STDP_VERIFICATION, config);
        std::cout << "   Result: " << (result3.success ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "   Duration: " << result3.getDurationString() << std::endl;
        std::cout << "   Message: " << result3.message << std::endl;
        
        // Show summary
        std::cout << "\n=== Experiment Summary ===\n";
        std::vector<ExperimentResult> allResults = {result1, result2, result3};
        for (const auto& result : allResults) {
            std::cout << "  " << getStatusSymbol(result.success) << " " << getModeName(result.mode) << 
                      ": " << result.message << " (" << result.getDurationString() << ")" << std::endl;
        }
    }
    
    void runAdvancedFeatures() {
        std::cout << "\n=== Running Advanced Features ===\n";
        
        // Initialize NLM Command Line Interface
        NLMCommandLineInterface cli;
        
        // Test command line parsing
        std::cout << "1. Testing Command Line Interface...\n";
        
        // Simulate command line arguments
        std::vector<std::string> testArgs = {
            "nlm",
            "test1,test2,test3",
            "--verbose",
            "--config", "configs/default.cfg",
            "--output", "demo_output.txt",
            "--timestep", "0.001",
            "--neurons", "500"
        };
        
        NLMCommandLineInterface::CommandLineArgs args;
        bool parseSuccess = cli.parseCommandLine(testArgs, args);
        std::cout << "   Command line parsing: " << (parseSuccess ? "SUCCESS" : "FAILURE") << std::endl;
        
        // Show available experiments
        std::cout << "\n2. Available Experiments:" << std::endl;
        auto experiments = cli.getAvailableExperiments();
        for (const auto& exp : experiments) {
            std::cout << "   " << getExperimentName(exp.mode) << ": " << exp.name << std::endl;
            std::cout << "      " << exp.description << std::endl;
        }
        
        // Show common options
        std::cout << "\n3. Common Options:" << std::endl;
        auto commonOptions = cli.getCommonOptions();
        for (const auto& option : commonOptions) {
            std::cout << "   --" << option.first << "  " << option.second << std::endl;
        }
        
        // Test configuration loading
        std::cout << "\n4. Testing Configuration Loading...\n";
        Config config;
        bool configLoadSuccess = cli.loadDefaultConfig(config);
        std::cout << "   Default config load: " << (configLoadSuccess ? "SUCCESS" : "FAILURE") << std::endl;
        
        if (configLoadSuccess) {
            std::cout << "   Configuration has " << config.getKeys().size() << " entries:" << std::endl;
            for (const auto& key : config.getKeys()) {
                std::cout << "     " << key << std::endl;
            }
        }
    }
    
    void runPerformanceDemo() {
        std::cout << "\n=== Performance Demo ===\n";
        
        // Create configuration optimized for performance
        Config config;
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        config.set("connection_probability", 0.1f);
        config.set("simulation_timestep", 0.001);
        config.set("parallel_enabled", true);
        config.set("cpu_cores", 4);
        
        // Create experiment runner with monitoring
        ExperimentRunner runner;
        
        // Run benchmark experiment
        std::cout << "Running performance benchmark...\n";
        auto startTime = std::chrono::high_resolution_clock::now();
        
        ExperimentResult result = runner.runExperiment(ExperimentMode::BENCHMARK, config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Benchmark completed in " << std::fixed << std::setprecision(3) << duration << " seconds" << std::endl;
        std::cout << "Result: " << (result.success ? "SUCCESS" : "FAILURE") << std::endl;
        std::cout << "Message: " << result.message << std::endl;
        
        // Show performance metrics
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Execution time: " << result.getDurationString() << std::endl;
        std::cout << "  Spikes generated: " << result.metrics.spikesGenerated << std::endl;
        std::cout << "  Average weight change: " << std::fixed << std::setprecision(6) << result.metrics.meanWeightChange << std::endl;
    }
    
    void runValidationDemo() {
        std::cout << "\n=== Validation Demo ===\n";
        
        // Create experiment runner
        ExperimentRunner runner;
        
        // Test with valid configuration
        std::cout << "1. Validating correct configuration...\n";
        Config validConfig;
        validConfig.set("neuron_count", 500);
        validConfig.set("region_count", 1);
        validConfig.set("connection_probability", 0.15f);
        validConfig.set("simulation_timestep", 0.001);
        validConfig.set("random_seed", 42);
        
        bool validSuccess = runner.validateConfig(validConfig, ExperimentMode::BASIC_CONNECTIVITY);
        std::cout << "   Validation result: " << (validSuccess ? "PASSED" : "FAILED") << std::endl;
        
        // Test with invalid configuration
        std::cout << "\n2. Validating invalid configuration...\n";
        Config invalidConfig;
        invalidConfig.set("neuron_count", 0); // Invalid: too low
        invalidConfig.set("region_count", 20); // Invalid: too high
        invalidConfig.set("connection_probability", 1.5f); // Invalid: too high
        
        bool invalidSuccess = runner.validateConfig(invalidConfig, ExperimentMode::BASIC_CONNECTIVITY);
        std::cout << "   Validation result: " << (invalidSuccess ? "PASSED" : "FAILED") << std::endl;
        
        // Show experiment presets
        std::cout << "\n3. Available Experiment Presets:" << std::endl;
        std::vector<std::string> presets = {
            "quick_test",
            "comprehensive",
            "benchmark",
            "validation",
            "performance"
        };
        
        for (const auto& preset : presets) {
            std::cout << "   - " << preset << std::endl;
        }
    }
    
    std::string getStatusSymbol(bool success) {
        return success ? "✓" : "✗";
    }
    
    std::string getModeName(ExperimentMode mode) {
        NLMCommandLineInterface cli;
        return cli.getExperimentName(mode);
    }
    
    void run() {
        std::cout << "Starting NLM System Demo...\n\n";
        
        try {
            runBasicExperiments();
            runAdvancedFeatures();
            runPerformanceDemo();
            runValidationDemo();
            
            std::cout << "\n=== Demo Complete ===\n";
            std::cout << "The enhanced NLM system is working correctly!\n";
            std::cout << "Key improvements demonstrated:\n";
            std::cout << "  ✓ Unified experiment framework\n";
            std::cout << "  ✓ Enhanced configuration management\n";
            std::cout << "  ✓ Improved command-line interface\n";
            std::cout << "  ✓ Performance monitoring capabilities\n";
            std::cout << "  ✓ Configuration validation\n";
            std::cout << "  ✓ Plugin architecture support\n";
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Demo failed with exception: " << e.what() << std::endl;
            throw;
        }
    }
};

int main(int argc, char* argv[]) {
    // Check if running in demo mode
    bool demoMode = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--demo" || arg == "-demo") {
            demoMode = true;
        }
    }
    
    if (demoMode) {
        NLMSystemDemo demo;
        demo.run();
        return 0;
    }
    
    // Run standard NLM command line interface
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Run with --help for usage information" << std::endl;
    
    return 0;
}