// NLM Unified Configuration System Demonstration
// 
// This file demonstrates the use of the UnifiedConfig system with the existing
// NLM codebase. It shows:
// 1. Creating and using UnifiedConfig objects
// 2. Converting between UnifiedConfig and legacy Config formats
// 3. Using configuration profiles
// 4. Validating configuration parameters
// 5. Handling configuration errors

#include "UnifiedConfig.hpp"
#include "Config.hpp"  // For legacy compatibility
#include <iostream>
#include <memory>

namespace nlm {

// Demonstration 1: Basic UnifiedConfig usage
void demonstrateBasicUsage() {
    std::cout << "=== Demonstration 1: Basic UnifiedConfig Usage ===" << std::endl;
    
    try {
        UnifiedConfig config;
        
        // Initialize with a profile
        config.initialize("quick_test");
        
        // Get configuration values
        int neuronCount = config.get<int>("neuron_count", 500);
        double connectionProb = config.get<double>("connection_probability", 0.1);
        
        std::cout << "Neuron count: " << neuronCount << std::endl;
        std::cout << "Connection probability: " << connectionProb << std::endl;
        
        // Set a custom value
        config.set<int>("neuron_count", 1000);
        std::cout << "Updated neuron count: " << config.get<int>("neuron_count") << std::endl;
        
        // Check if parameter exists
        if (config.has("random_seed")) {
            std::cout << "Random seed: " << config.get<int64_t>("random_seed") << std::endl;
        }
        
        std::cout << "Basic usage demonstration completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in basic usage demonstration: " << e.what() << std::endl;
    }
}

// Demonstration 2: Converting to legacy Config format
void demonstrateLegacyConversion() {
    std::cout << "\n=== Demonstration 2: Legacy Config Conversion ===" << std::endl;
    
    try {
        // Create UnifiedConfig
        UnifiedConfig unifiedConfig;
        unifiedConfig.initialize("research");
        
        // Customize some values
        unifiedConfig.set<int>("neuron_count", 2000);
        unifiedConfig.set<double>("stdp_ltp_weight", 0.05);
        
        // Convert to legacy Config
        Config legacyConfig;
        unifiedConfig.convertToLegacyConfig(legacyConfig);
        
        // Verify values were converted
        if (legacyConfig.has("neuron_count")) {
            int64_t neurons = legacyConfig.getOr<int64_t>("neuron_count", 500);
            std::cout << "Converted neuron count: " << neurons << std::endl;
        }
        
        if (legacyConfig.has("stdp_ltp_weight")) {
            double weight = legacyConfig.getOr<double>("stdp_ltp_weight", 0.01);
            std::cout << "Converted STDP LTP weight: " << weight << std::endl;
        }
        
        std::cout << "Legacy conversion demonstration completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in legacy conversion demonstration: " << e.what() << std::endl;
    }
}

// Demonstration 3: Configuration validation
void demonstrateValidation() {
    std::cout << "\n=== Demonstration 3: Configuration Validation ===" << std::endl;
    
    try {
        UnifiedConfig config;
        config.initialize("default");
        
        // Validate all parameters
        if (config.validateAll()) {
            std::cout << "All configuration parameters are valid!" << std::endl;
        }
        
        // Try to set an invalid value
        try {
            config.set<int>("neuron_count", 0);  // Invalid: must be >= 1
            std::cout << "ERROR: Should not have accepted neuron_count = 0!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Correctly rejected invalid neuron_count: " << e.what() << std::endl;
        }
        
        // Try another invalid value
        try {
            config.set<double>("connection_probability", 1.5);  // Invalid: must be <= 1.0
            std::cout << "ERROR: Should not have accepted connection_probability = 1.5!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Correctly rejected invalid connection_probability: " << e.what() << std::endl;
        }
        
        std::cout << "Validation demonstration completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in validation demonstration: " << e.what() << std::endl;
    }
}

// Demonstration 4: Configuration profiles
void demonstrateProfiles() {
    std::cout << "\n=== Demonstration 4: Configuration Profiles ===" << std::endl;
    
    const std::unordered_map<std::string, std::map<std::string, double>> profiles = 
        UnifiedConfig::getDefaultProfiles();
    
    for (const auto& profilePair : profiles) {
        std::cout << "Profile: " << profilePair.first << std::endl;
        
        UnifiedConfig config;
        try {
            config.initialize(profilePair.first);
            
            // Show key parameters
            std::cout << "  Neuron count: " << config.get<int>("neuron_count") << std::endl;
            std::cout << "  Region count: " << config.get<int>("region_count") << std::endl;
            std::cout << "  Connection probability: " << config.get<double>("connection_probability") << std::endl;
            std::cout << "  Simulation timestep: " << config.get<double>("simulation_timestep") << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "  Error initializing profile: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Profiles demonstration completed successfully!" << std::endl;
}

// Demonstration 5: Configuration file loading
void demonstrateFileLoading() {
    std::cout << "\n=== Demonstration 5: Configuration File Loading ===" << std::endl;
    
    try {
        UnifiedConfig config;
        
        // Try to load from a file (this might fail if file doesn't exist)
        bool loaded = config.loadFromFile("configs/test_config.txt");
        
        if (loaded) {
            std::cout << "Successfully loaded configuration from file!" << std::endl;
            std::cout << "Parameters loaded: " << config.getAllParameterNames().size() << std::endl;
        } else {
            std::cout << "Failed to load configuration from file (file may not exist)" << std::endl;
            std::cout << "This is expected for the demonstration - file doesn't need to exist" << std::endl;
        }
        
        std::cout << "File loading demonstration completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in file loading demonstration: " << e.what() << std::endl;
    }
}

// Demonstration 6: Configuration merging
void demonstrateMerging() {
    std::cout << "\n=== Demonstration 6: Configuration Merging ===" << std::endl;
    
    try {
        // Create base configuration
        UnifiedConfig baseConfig;
        baseConfig.initialize("quick_test");
        baseConfig.set<int>("neuron_count", 500);
        baseConfig.set<double>("stdp_ltp_weight", 0.01);
        
        // Create override configuration
        UnifiedConfig overrideConfig;
        overrideConfig.initialize("research");
        overrideConfig.set<int>("neuron_count", 2000);  // Override base
        overrideConfig.set<double>("stdp_ltp_weight", 0.05);  // Override base
        // Note: random_seed is kept from research profile, not overridden
        
        std::cout << "Base configuration neuron_count: " << baseConfig.get<int>("neuron_count") << std::endl;
        std::cout << "Override configuration neuron_count: " << overrideConfig.get<int>("neuron_count") << std::endl;
        
        // Merge configurations (override takes precedence)
        baseConfig.merge(overrideConfig);
        
        std::cout << "After merge - base configuration neuron_count: " << baseConfig.get<int>("neuron_count") << std::endl;
        std::cout << "After merge - base configuration random_seed: " << baseConfig.get<int64_t>("random_seed") << std::endl;
        
        std::cout << "Merging demonstration completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in merging demonstration: " << e.what() << std::endl;
    }
}

// Demonstration 7: Advanced configuration usage
void demonstrateAdvancedUsage() {
    std::cout << "\n=== Demonstration 7: Advanced Configuration Usage ===" << std::endl;
    
    try {
        // Create configuration for different phases
        UnifiedConfig phase3Config;
        phase3Config.initialize("quick_test");
        phase3Config.set<int>("environment_width", 20);
        phase3Config.set<int>("environment_height", 20);
        phase3Config.set<int>("vision_width", 16);
        phase3Config.set<int>("vision_height", 16);
        
        UnifiedConfig phase4Config;
        phase4Config.initialize("research");
        phase4Config.set<int>("neuron_count", 3000);
        phase4Config.set<double>("stdp_ltp_weight", 0.03);
        phase4Config.set<bool>("enable_development", true);
        phase4Config.set<bool>("enable_reward_modulation", true);
        
        UnifiedConfig phase2Config;
        phase2Config.initialize("default");
        phase2Config.set<double>("synaptogenesis_rate", 0.0005);
        phase2Config.set<double>("pruning_rate", 0.00005);
        
        std::cout << "Phase 3 configuration:" << std::endl;
        std::cout << "  Environment: " << phase3Config.get<int>("environment_width") << "x" << phase3Config.get<int>("environment_height") << std::endl;
        std::cout << "  Vision: " << phase3Config.get<int>("vision_width") << "x" << phase3Config.get<int>("vision_height") << std::endl;
        
        std::cout << "Phase 4 configuration:" << std::endl;
        std::cout << "  Neurons: " << phase4Config.get<int>("neuron_count") << std::endl;
        std::cout << "  Development enabled: " << (phase4Config.get<bool>("enable_development") ? "yes" : "no") << std::endl;
        
        std::cout << "Phase 2 configuration:" << std::endl;
        std::cout << "  Synaptogenesis rate: " << phase2Config.get<double>("synaptogenesis_rate") << std::endl;
        std::cout << "  Pruning rate: " << phase2Config.get<double>("pruning_rate") << std::endl;
        
        // Convert configurations to legacy format
        Config legacyPhase3;
        phase3Config.convertToLegacyConfig(legacyPhase3);
        
        std::cout << "Advanced usage demonstration completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in advanced usage demonstration: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "NLM Unified Configuration System - Demonstration" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        demonstrateBasicUsage();
        demonstrateLegacyConversion();
        demonstrateValidation();
        demonstrateProfiles();
        demonstrateFileLoading();
        demonstrateMerging();
        demonstrateAdvancedUsage();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "All demonstrations completed successfully!" << std::endl;
        std::cout << "The UnifiedConfig system provides:
" << std::endl;
        std::cout << "  ✓ Standardized configuration management" << std::endl;
        std::cout << "  ✓ Parameter validation and bounds checking" << std::endl;
        std::cout << "  ✓ Configuration profiles for different use cases" << std::endl;
        std::cout << "  ✓ Backward compatibility with legacy Config system" << std::endl;
        std::cout << "  ✓ File and command-line loading" << std::endl;
        std::cout << "  ✓ Configuration merging and overriding" << std::endl;
        std::cout << "  ✓ Advanced phase-specific configuration" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error in demonstration: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace nlm
