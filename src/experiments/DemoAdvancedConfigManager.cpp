// Advanced Configuration Manager Demo
// This demo showcases the Advanced Configuration Manager capabilities

#include "AdvancedConfigManager.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace nlm;

class AdvancedConfigManagerDemo {
public:
    AdvancedConfigManagerDemo() {}
    ~AdvancedConfigManagerDemo() {}
    
    void run() {
        std::cout << "========================================\n";
        std::cout << "ADVANCED CONFIGURATION MANAGER DEMO\n";
        std::cout << "========================================\n\n";
        
        // Initialize configuration manager
        ConfigManager manager;
        
        // Load default configuration
        Config defaultConfig;
        defaultConfig.set("neuron_count", 1000);
        defaultConfig.set("region_count", 4);
        defaultConfig.set("connection_probability", 0.15f);
        defaultConfig.set("stdp_ltp_weight", 0.01f);
        defaultConfig.set("stdp_ltd_weight", 0.012f);
        defaultConfig.set("synaptogenesis_rate", 0.0002f);
        defaultConfig.set("pruning_rate", 0.00002f);
        defaultConfig.set("simulation_timestep", 0.001);
        defaultConfig.set("random_seed", 12345u);
        
        manager.loadConfig(defaultConfig);
        std::cout << "✓ Default configuration loaded\n";
        
        // Create complex configuration profile
        std::cout << "\n--- Creating Configuration Profiles ---\n";
        
        // Performance profile
        Config performanceProfile;
        performanceProfile.set("neuron_count", 2000);
        performanceProfile.set("region_count", 8);
        performanceProfile.set("connection_probability", 0.2f);
        performanceProfile.set("stdp_ltp_weight", 0.02f);
        performanceProfile.set("stdp_ltd_weight", 0.02f);
        performanceProfile.set("synaptogenesis_rate", 0.0001f);
        performanceProfile.set("pruning_rate", 0.00005f);
        performanceProfile.set("simulation_timestep", 0.0005);
        
        manager.createProfile("performance_high", performanceProfile);
        std::cout << "✓ Performance profile created\n";
        
        // Memory efficiency profile
        Config memoryProfile;
        memoryProfile.set("neuron_count", 500);
        memoryProfile.set("region_count", 2);
        memoryProfile.set("connection_probability", 0.05f);
        memoryProfile.set("stdp_ltp_weight", 0.005f);
        memoryProfile.set("stdp_ltd_weight", 0.006f);
        memoryProfile.set("synaptogenesis_rate", 0.00005f);
        memoryProfile.set("pruning_rate", 0.00002f);
        memoryProfile.set("simulation_timestep", 0.002);
        
        manager.createProfile("memory_efficient", memoryProfile);
        std::cout << "✓ Memory efficiency profile created\n";
        
        // Validate configurations
        std::cout << "\n--- Configuration Validation ---\n";
        bool valid1 = manager.validateConfig(performanceProfile);
        bool valid2 = manager.validateConfig(memoryProfile);
        
        std::cout << "Performance profile valid: " << (valid1 ? "YES" : "NO") << "\n";
        std::cout << "Memory profile valid: " << (valid2 ? "YES" : "NO") << "\n";
        
        // Compare configurations
        std::cout << "\n--- Configuration Comparison ---\n";
        auto diff = manager.compareConfigs(performanceProfile, memoryProfile);
        std::cout << "Neuron count difference: " << diff["neuron_count"] << "\n";
        std::cout << "Memory usage ratio: " << diff["memory_usage_ratio"] << "x\n";
        
        // Save configurations
        std::cout << "\n--- Configuration Export ---\n";
        std::string perfPath = "/tmp/performance_config.json";
        std::string memPath = "/tmp/memory_config.json";
        
        manager.exportConfig(performanceProfile, perfPath);
        manager.exportConfig(memoryProfile, memPath);
        
        std::cout << "✓ Performance config exported to: " << perfPath << "\n";
        std::cout << "✓ Memory config exported to: " << memPath << "\n";
        
        // Import and validate imported config
        std::cout << "\n--- Configuration Import ---\n";
        Config importedConfig;
        bool importSuccess = manager.importConfig(perfPath, importedConfig);
        
        if (importSuccess) {
            std::cout << "✓ Configuration imported successfully\n";
            bool importValid = manager.validateConfig(importedConfig);
            std::cout << "Imported config valid: " << (importValid ? "YES" : "NO") << "\n";
        } else {
            std::cout << "✗ Failed to import configuration\n";
        }
        
        // Configuration optimization
        std::cout << "\n--- Configuration Optimization ---\n";
        auto optimized = manager.optimizeConfig(performanceProfile);
        std::cout << "Optimized neuron count: " << optimized.getInt("neuron_count") << "\n";
        std::cout << "Optimized memory usage: " << optimized.getInt("memory_usage_kb") << "KB\n";
        
        // Create scenario-based configurations
        std::cout << "\n--- Scenario-Based Configurations ---\n";
        
        // Scenario 1: Quick test
        Config quickTestConfig;
        manager.applyScenario(quickTestConfig, "quick_test");
        std::cout << "Quick test config neurons: " << quickTestConfig.getInt("neuron_count") << "\n";
        
        // Scenario 2: Long-term learning
        Config learningConfig;
        manager.applyScenario(learningConfig, "long_term_learning");
        std::cout << "Long-term config neurons: " << learningConfig.getInt("neuron_count") << "\n";
        
        // Scenario 3: Real-time control
        Config realtimeConfig;
        manager.applyScenario(realtimeConfig, "real_time_control");
        std::cout << "Real-time config neurons: " << realtimeConfig.getInt("neuron_count") << "\n";
        
        // Configuration statistics
        std::cout << "\n--- Configuration Statistics ---\n";
        auto stats = manager.getStats(performanceProfile);
        std::cout << "Configuration complexity score: " << stats["complexity_score"] << "\n";
        std::cout << "Estimated runtime: " << stats["estimated_runtime_seconds"] << "s\n";
        std::cout << "Memory footprint: " << stats["memory_footprint_kb"] << "KB\n";
        
        std::cout << "\n========================================\n";
        std::cout << "ADVANCED CONFIGURATION MANAGER DEMO COMPLETE\n";
        std::cout << "========================================\n";
    }
};

int main(int argc, char* argv[]) {
    nlm::AdvancedConfigManagerDemo demo;
    demo.run();
    return 0;
}
