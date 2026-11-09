#pragma once

#include "../core/CLI/CLIFramework.hpp"

namespace nlm {

class AdvancedDemoBase {
public:
    virtual ~AdvancedDemoBase() = default;
    
    // Virtual method to run the demo
    virtual int run(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) = 0;
    
    // Get demo name
    virtual std::string getDemoName() const = 0;
    
    // Get demo description
    virtual std::string getDemoDescription() const = 0;
    
    // Register custom arguments and options
    virtual void registerArguments(CLIFramework& framework) = 0;
    
    // Register custom options
    virtual void registerOptions(CLIFramework& framework) = 0;
    
    // Validate configuration
    virtual bool validateConfig(const CLIConfig& config) const {
        return true;
    }
    
    // Initialize demo
    virtual bool initialize(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        return true;
    }
    
    // Cleanup demo
    virtual void cleanup() {}
    
    // Get brain reference (for checkpointing)
    virtual std::shared_ptr<Brain> getBrain() const { return nullptr; }
    
    // Set brain reference (for checkpointing)
    virtual void setBrain(const std::shared_ptr<Brain>& brain) {}
    
    // Execute with error handling and metrics
    int executeWithErrorHandling(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig) {
        try {
            // Validate configuration
            if (!validateConfig(config)) {
                std::cerr << "Invalid configuration" << std::endl;
                return 1;
            }
            
            // Initialize demo
            if (!initialize(config, nlmConfig)) {
                std::cerr << "Failed to initialize demo" << std::endl;
                return 1;
            }
            
            // Execute main demo logic
            int result = run(config, nlmConfig);
            
            // Cleanup
            cleanup();
            
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error in demo " << getDemoName() << ": " << e.what() << std::endl;
            return 1;
        }
    }
};

} // namespace nlm