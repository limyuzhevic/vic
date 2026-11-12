// Application runner for NLM examples
// Refactored from monolithic main.cpp for better organization

#pragma once

#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {
namespace examples {

// Main application runner that coordinates configuration, execution, and reporting
class MainRunner {
public:
    // Constructor with configuration file path
    explicit MainRunner(const std::string& configFile = "configs/default.cfg");
    
    // Destructor
    ~MainRunner();
    
    // Disable copying, enable moving
    MainRunner(const MainRunner&) = delete;
    MainRunner& operator=(const MainRunner&) = delete;
    MainRunner(MainRunner&&) noexcept;
    MainRunner& operator=(MainRunner&&) noexcept;
    
    // Set configuration values programmatically
    void setConfigValue(const std::string& key, const std::string& value);
    void setConfigValue(const std::string& key, int64_t value);
    void setConfigValue(const std::string& key, double value);
    void setConfigValue(const std::string& key, bool value);
    
    // Load configuration from file
    bool loadConfigFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    bool loadConfigFromArgs(int argc, char** argv);
    
    // Initialize the brain with current configuration
    bool initializeBrain();
    
    // Run Phase 2 demonstration
    bool runPhase2Demo();
    
    // Run Phase 3 demonstration
    bool runPhase3Demo();
    
    // Run Phase 4 demonstration
    bool runPhase4Demo();
    
    // Run Phase 5 demonstration
    bool runPhase5Demo();
    
    // Run Phase 6 demonstration
    bool runPhase6Demo();
    
    // Run benchmark
    bool runBenchmark();
    
    // Print application banner
    void printBanner() const;
    
    // Print configuration summary
    void printConfigSummary() const;
    
    // Print experiment results
    void printExperimentResults(const std::string& experimentName, 
                               const std::string& results) const;
    
    // Save brain state to checkpoint
    bool saveCheckpoint(const std::string& filepath) const;
    
    // Load brain state from checkpoint
    bool loadCheckpoint(const std::string& filepath);
    
    // Get current brain status
    std::string getBrainStatus() const;
    
    // Get configuration object
    std::shared_ptr<const Config> getConfig() const;
    
    // Get brain object
    std::shared_ptr<Brain> getBrain() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}} // namespace nlm::examples