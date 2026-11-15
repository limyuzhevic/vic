// Advanced Phase 6 Demo - Comprehensive Integration Test with improved configurability
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CommandLine.hpp"
#include "core/DemoConfig.hpp"
#include <iostream>
#include <memory>

namespace nlm {

class AdvancedPhase6Demo {
public:
    AdvancedPhase6Demo() = default;
    
    // Run the complete integration test with advanced configuration
    bool runAdvancedIntegration(const DemoConfig& config) {
        std::cout << "=== Advanced NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain with advanced features..." << std::endl << std::endl;
        
        // Initialize logging with advanced settings
        Logger::getGlobal().setLevel(LogLevel::Info);
        
        // Create experiment with advanced configuration
        Phase6IntegratedExperiment experiment;
        
        // Run quick integration verification
        std::cout << "--- Advanced Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return false;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems with detailed reporting
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
        
        // Run full experiment with configuration from demo config
        std::cout << "--- Full Advanced Integration Experiment ---" << std::endl;
        
        // Create Phase 6 config from demo config
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.get<int>("neurons", 500);
        phase6Config.maxSteps = config.get<int>("steps", 2000);
        phase6Config.enableCheckpointing = config.get<bool>("enable_checkpointing", true);
        phase6Config.enableReplay = config.get<bool>("enable_replay", true);
        phase6Config.enableDevelopment = config.get<bool>("enable_development", true);
        phase6Config.learningRate = config.get<float>("learning_rate", 0.01f);
        phase6Config.plasticityEnabled = config.get<bool>("plasticity_enabled", true);
        phase6Config.stdpEnabled = config.get<bool>("stdp_enabled", true);
        phase6Config.hebbianEnabled = config.get<bool>("hebbian_enabled", true);
        
        auto result = experiment.run(phase6Config);
        
        std::cout << std::endl;
        std::cout << "=== ADVANCED FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Prediction confidence: " << result.predictionConfidence << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== ADVANCED INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Computational efficiency: " << result.efficiencyScore << " spikes/step" << std::endl;
        
        return true;
    }
    
    // Run multiple experiments with different configurations
    void runBenchmarkSuite() {
        std::cout << "=== NLM Phase 6 Benchmark Suite ===" << std::endl;
        
        // Small brain configuration
        DemoConfig smallConfig;
        smallConfig.set("neurons", 100);
        smallConfig.set("steps", 500);
        smallConfig.set("plasticity_enabled", true);
        smallConfig.set("verbose", false);
        
        Phase6IntegratedExperiment experiment;
        Phase6Config config1;
        config1.neuronCount = 100;
        config1.maxSteps = 500;
        config1.enableCheckpointing = false;
        config1.enableReplay = false;
        config1.enableDevelopment = false;
        
        auto result1 = experiment.run(config1);
        std::cout << "Small brain (100 neurons): Reward = " << result1.totalReward 
                  << ", Efficiency = " << result1.efficiencyScore << " spikes/step" << std::endl;
        
        // Medium brain configuration
        DemoConfig mediumConfig;
        mediumConfig.set("neurons", 500);
        mediumConfig.set("steps", 2000);
        mediumConfig.set("plasticity_enabled", true);
        mediumConfig.set("verbose", false);
        
        Phase6Config config2;
        config2.neuronCount = 500;
        config2.maxSteps = 2000;
        config2.enableCheckpointing = true;
        config2.enableReplay = true;
        config2.enableDevelopment = true;
        
        auto result2 = experiment.run(config2);
        std::cout << "Medium brain (500 neurons): Reward = " << result2.totalReward 
                  << ", Efficiency = " << result2.efficiencyScore << " spikes/step" << std::endl;
        
        // Large brain configuration (if resources available)
        DemoConfig largeConfig;
        largeConfig.set("neurons", 1000);
        largeConfig.set("steps", 5000);
        largeConfig.set("plasticity_enabled", true);
        largeConfig.set("verbose", false);
        
        Phase6Config config3;
        config3.neuronCount = 1000;
        config3.maxSteps = 5000;
        config3.enableCheckpointing = true;
        config3.enableReplay = true;
        config3.enableDevelopment = true;
        
        auto result3 = experiment.run(config3);
        std::cout << "Large brain (1000 neurons): Reward = " << result3.totalReward 
                  << ", Efficiency = " << result3.efficiencyScore << " spikes/step" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== BENCHMARK SUMMARY ===" << std::endl;
        std::cout << "Small brain scaling factor: " << (result2.efficiencyScore / result1.efficiencyScore) << std::endl;
        std::cout << "Large brain scaling factor: " << (result3.efficiencyScore / result1.efficiencyScore) << std::endl;
        std::cout << "Learning rate sensitivity: " << (result2.totalReward - result1.totalReward) / (config2.neuronCount - config1.neuronCount) << " reward per 100 neurons" << std::endl;
    }
};

} // namespace nlm

int main(int argc, char* argv[]) {
    // Parse advanced command line options
    auto options = parseDemoOptions(argc, argv);
    
    // Parse demo configuration
    DemoConfig config;
    if (options.experimentMode == "benchmark") {
        config = DemoPresets::getLearningDemo(); // Use learning demo as baseline
        config.set("verbose", options.verbose);
    } else {
        config = getDemoConfig(demoModeFromString(options.experimentMode));
    }
    
    // Override with command line options
    if (options.hasArgument("neurons")) {
        config.set("neurons", options.getInt("neurons", 500));
    }
    if (options.hasArgument("steps")) {
        config.set("steps", options.getInt("steps", 1000));
    }
    if (options.hasArgument("learning-rate")) {
        config.set("learning_rate", options.getFloat("learning-rate", 0.01f));
    }
    if (options.hasArgument("plasticity")) {
        config.set("plasticity_enabled", options.getBool("plasticity", true));
    }
    
    // Handle help requests
    if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h" || 
                     options.experimentMode == "help")) {
        std::cout << "Usage: advanced_phase6_demo [options]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --mode <mode>           Experiment mode: connectivity, plasticity, stdp, learning, benchmark" << std::endl;
        std::cout << "  --neurons <n>           Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <n>             Number of simulation steps (default: 2000)" << std::endl;
        std::cout << "  --learning-rate <r>      Learning rate (default: 0.01)" << std::endl;
        std::cout << "  --plasticity [bool]     Enable plasticity (default: true)" << std::endl;
        std::cout << "  --verbose                Enable verbose output" << std::endl;
        std::cout << "  --config <file>          Configuration file" << std::endl;
        std::cout << std::endl;
        std::cout << "Example runs:" << std::endl;
        std::cout << "  advanced_phase6_demo --mode benchmark" << std::endl;
        std::cout << "  advanced_phase6_demo --mode learning --neurons 1000 --steps 5000" << std::endl;
        std::cout << "  advanced_phase6_demo --mode connectivity --plasticity false" << std::endl;
        return 0;
    }
    
    // Run advanced demo
    AdvancedPhase6Demo demo;
    
    bool success = true;
    if (options.experimentMode == "benchmark") {
        demo.runBenchmarkSuite();
    } else {
        success = demo.runAdvancedIntegration(config);
    }
    
    if (!success) {
        std::cerr << "Demo failed!" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Advanced Phase 6 Demo Complete ===" << std::endl;
    
    return 0;
}
