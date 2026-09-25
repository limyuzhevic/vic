#pragma once

namespace nlm {

class Config;
class Brain;

// Factory pattern for creating brain systems
class BrainFactory {
public:
    // Create a complete brain with all systems integrated
    static std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config);
    
    // Create brain with specific configuration parameters
    static std::shared_ptr<Brain> createBrain(
        const std::shared_ptr<Config>& config,
        size_t neuronCount,
        size_t regionCount,
        float connectionProbability,
        float simulationTimestep
    );
};

// Configuration factory for validation
class ConfigFactory {
public:
    // Create configuration with validation
    static std::shared_ptr<Config> createConfig();
    
    // Create configuration from file
    static std::shared_ptr<Config> createConfigFromFile(const std::string& filepath);
    
    // Validate configuration
    static bool validateConfig(const std::shared_ptr<Config>& config);
};

// Service locator pattern for brain systems
class BrainServiceLocator {
public:
    // Set the current brain instance
    static void setBrain(const std::shared_ptr<Brain>& brain);
    
    // Get the current brain instance
    static std::shared_ptr<Brain> getBrain();
    
    // Set the current configuration
    static void setConfig(const std::shared_ptr<Config>& config);
    
    // Get the current configuration
    static std::shared_ptr<Config> getConfig();
    
    // Clear all services
    static void clear();
};

// Helper functions for common operations
class BrainUtils {
public:
    // Run a basic connectivity test
    static void runBasicConnectivityTest(const std::shared_ptr<Brain>& brain);
    
    // Run plasticity experiment
    static void runPlasticityExperiment(const std::shared_ptr<Brain>& brain);
    
    // Run STDP verification
    static void runStdpVerification(const std::shared_ptr<Brain>& brain);
};

} // namespace nlm
