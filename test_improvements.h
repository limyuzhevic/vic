#ifndef MAIN_CPP_IMPROVED_H
#define MAIN_CPP_IMPROVED_H

#include "main.cpp"  // Include the original to understand the structure

// For testing purposes only
namespace nlm::test {
    
    // Improved configuration loading with better error handling
    bool loadConfigurationImproved(nlm::Config& config, const std::string& configFile, const std::vector<std::string>& args);
    
    // Test execution with proper error handling and logging
    int runAllTestsImproved(std::shared_ptr<nlm::Brain> brain);
    
    // Individual test improvements
    void runBasicConnectivityTestImproved(std::shared_ptr<nlm::Brain> brain);
    void runPlasticityExperimentImproved(std::shared_ptr<nlm::Brain> brain);
    void runStdpVerificationImproved(std::shared_ptr<nlm::Brain> brain);
    
    // Utility functions for better code organization
    void logConfigurationSummary(const nlm::Config& config);
    bool validateBrainInitialization(std::shared_ptr<nlm::Brain> brain);
    void cleanupBrain(std::shared_ptr<nlm::Brain> brain);
    
} // namespace nlm::test

#endif // MAIN_CPP_IMPROVED_H